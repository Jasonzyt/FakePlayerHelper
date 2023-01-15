#include "pch.h"
#include "Config.h"
#include "LangPack.h"
#include "Mess.h"
#include "FakePlayer.h"
#if defined(BDS_V1_16)
#include <api/commands.h>
#include <mc/Command.h>
#include <mc/CommandReg.h>
#include <api/Basic_Event.h>
#elif defined(BDS_V1_18)
#include <EventAPI.h>
#include <RegCommandAPI.h>
#include <MC/ServerPlayer.hpp>

#endif

#define EXISTS(x, y) (std::find(x.begin(), x.end(), y) != x.end())

bool checkPermission(CommandOrigin const& ori, CommandOutput& outp) {
    // Only supports Player and Console
    auto oriType = (OriginType)ori.getOriginType();
    if (oriType == OriginType::DedicatedServer) {
        return true;
    }
    if (oriType != OriginType::Player) {
        return false;
    }
    if (cfg->perm.type == Config::Permission::ALL) {
        return true;
    }
    else if (cfg->perm.type == Config::Permission::Specified) {
        if ((OriginType)ori.getOriginType() == OriginType::Player) {
            auto player = (Player*)ori.getEntity();
            auto xuid = getXuid(player);
            if (EXISTS(cfg->perm.allow_list, xuid)) {
                return true;
            }
            else {
                outp.error("You don't have permission to use this command.");
                return false;
            }
        }
        else {
            outp.error("You don't have permission to use this command.");
            return false;
        }
    }
    else if (cfg->perm.type == Config::Permission::ConsoleOnly) {
        outp.error("You don't have permission to use this command.");
        return false;
    }
    return false;
}

void onAddCommand(CommandOrigin const& ori, CommandOutput& outp, const std::string& name) {
    if (checkPermission(ori, outp)) {
        if (isOnlineFakePlayer(name)) {
            outp.error(lpk->localize("fpcmd.fp.already.exists"), {});
            return;
        }
        else {
            std::string sname = "[Console]";
            xuid_t sxuid;
            auto globalFpCount = fakePlayers.size();
            if (globalFpCount > cfg->maxGlobalFakePlayers) {
                outp.error(lpk->localize("fpcmd.global.limit.exceed"), {});
                return;
            }
            if ((OriginType)ori.getOriginType() == OriginType::Player) {
                int fpCount = 0;
                auto pl = (Player*)ori.getEntity();
                auto xuid = getXuid(pl);
                for (auto& [name, fp] : fakePlayers) {
                    if (fp->summoner.xuid == xuid) {
                        fpCount++;
                    }
                }
                if (fpCount > cfg->maxPlayerFakePlayers) {
                    outp.error(lpk->localize("fpcmd.player.limit.exceed"), {});
                    return;
                }
                sname = getRealName(pl);
                sxuid = xuid;
            }
            addPlayerToWhiteList(name);
            fpws->add(std::move(std::make_unique<FakePlayer>(nullptr, name, true, sname, sxuid)));
            outp.success();
        }
    }
}

void onRemoveCommand(CommandOrigin const& ori, CommandOutput& outp, const std::string& name) {
    if (checkPermission(ori, outp)) {
        if (isOnlineFakePlayer(name)) {
            auto& fp = fakePlayers.at(name);
            fp->disconnect();
            outp.success();
            return;
        }
        else {
            outp.error(lpk->localize("fpcmd.cannot.find.fp"), {});
            return;
        }
    }
}

void onConnectCommand(CommandOrigin const& ori, CommandOutput& outp, const std::string& name) {
    if (checkPermission(ori, outp)) {
        if (isOnlineFakePlayer(name)) {
            outp.error(lpk->localize("fpcmd.fp.already.exists"), {});
            return;
        }
        else {
            if (fakePlayers.count(name) == 0) {
                outp.error(lpk->localize("fpcmd.cannot.find.fp"), {});
                outp.error("Please use '/fp add <name>' instead");
                return;
            }
            auto& fp = fakePlayers.at(name);
            fp->connect();
            outp.success();
            return;
        }
    }
}

void onDeleteCommand(CommandOrigin const& ori, CommandOutput& outp, const std::string& name) {
    if (checkPermission(ori, outp)) {
        if (fakePlayers.count(name)) {
            auto& fp = fakePlayers.at(name);
            fpws->del(fp.get());
            outp.success();
            return;
        }
        else {
            outp.error(lpk->localize("fpcmd.cannot.find.fp"), {});
            return;
        }
    }
}

void onRemoveAllCommand(CommandOrigin const& ori, CommandOutput& outp) {
    if (checkPermission(ori, outp)) {
        fpws->removeAll();
        outp.success();
        return;
    }
}

void onTeleportCommand(CommandOrigin const& ori, CommandOutput& outp, const std::string& name, const Vec3& pos, int dimid) {
    if (checkPermission(ori, outp)) {
        if (isOnlineFakePlayer(name)) {
            auto& fp = fakePlayers.at(name);
            if (fp->pl == nullptr) {
                // Try find
                if ((fp->pl = getPlayerByRealName(name)) == nullptr) {
                    outp.error("Internal Error(FakePlayer::pl is nullptr)", {});
                }
                return;
            }
            if (dimid < 0 || dimid > 2) {
                outp.error("Invalid dimension id", {});
                return;
            }
            teleport(fp->pl, pos, dimid);
            outp.success();
            return;
        }
        else {
            outp.error(lpk->localize("fpcmd.cannot.find.fp"), {});
            return;
        }
    }
}

void onListCommand(CommandOrigin const& ori, CommandOutput& outp) {
    if (checkPermission(ori, outp)) {
        std::string ONLINE_STR = "§aOnline§r";
        std::string OFFLINE_STR = "§cOffline§r";
        std::ostringstream oss;
        auto&& [onlines, offlines] = getFakePlayersCategorized();
        oss << "§lFakePlayers§r | " << ONLINE_STR << " " << onlines.size() << " Total " << fakePlayers.size() << std::endl;
        for (auto& fp : onlines) {
            oss << "- §7" << fp->name << "§r " << ONLINE_STR << " Summonned by " << fp->summoner.name << std::endl;
        }
        for (auto& fp : offlines) {
            oss << "- §7" << fp->name << "§r " << OFFLINE_STR << " Summonned by " << fp->summoner.name << std::endl;
        }
        outp.success(oss.str());
        return;
    }
}

#if defined(BDS_V1_16)

enum class Operation_Add {
    Add = 1,
};
enum class Operation_Remove {
    Remove = 1,
};
enum class Operation_Connect {
    Connect = 1,
};
enum class Operation_Delete {
    Delete = 1,
};
enum class Operation_RemoveAll {
    RemoveAll = 1,
};
enum class Operation_Teleport {
    Teleport = 1,
};
enum class Operation_List {
    List = 1,
};

bool _onAddCommand(CommandOrigin const& ori, CommandOutput& outp, MyEnum<Operation_Add>, const std::string& name) {
    onAddCommand(ori, outp, name);
    return true;
}
bool _onRemoveCommand(CommandOrigin const& ori, CommandOutput& outp, MyEnum<Operation_Remove>, const std::string& name) {
    onRemoveCommand(ori, outp, name);
    return true;
}
bool _onConnectCommand(CommandOrigin const& ori, CommandOutput& outp, MyEnum<Operation_Connect>, const std::string& name) {
    onConnectCommand(ori, outp, name);
    return true;
}
bool _onDeleteCommand(CommandOrigin const& ori, CommandOutput& outp, MyEnum<Operation_Delete>, const std::string& name) {
    onDeleteCommand(ori, outp, name);
    return true;
}
bool _onRemoveAllCommand(CommandOrigin const& ori, CommandOutput& outp, MyEnum<Operation_RemoveAll>) {
    onRemoveAllCommand(ori, outp);
    return true;
}
bool _onTeleportCommand1(CommandOrigin const& ori, CommandOutput& outp, MyEnum<Operation_Teleport>, const std::string& name, CommandSelector<Actor>& dst) {
    auto tgts = dst.results(ori);
    if (tgts.empty()) {
        outp.error("No target found!");
        return false;
    }
    Actor* ac = *tgts.begin();
    onTeleportCommand(ori, outp, name, ac->getPos(), (int)ac->getDimensionId());
    return true;
}
bool _onTeleportCommand2(CommandOrigin const& ori, CommandOutput& outp, MyEnum<Operation_Teleport>, const std::string& name, CommandPositionFloat pos, optional<int> dimid) {
    int dim = 0;
    if (dimid.Set()) {
        dim = dimid.val();
    }
    else {
        if (ori.getOriginType() == OriginType::Player || ori.getOriginType() == OriginType::Actor) {
            dim = (int)ori.getEntity()->getDimensionId();
        }
    }
    onTeleportCommand(ori, outp, name, *pos.getPosition(ori), dim);
    return true;
}
bool _onListCommand(CommandOrigin const& ori, CommandOutput& outp, MyEnum<Operation_List>) {
    onListCommand(ori, outp);
    return true;
}

void subscribeCommandRegistry() {
    Event::addEventListener([](const RegCmdEV& ev) {
        CMDREG::SetCommandRegistry(ev.CMDRg);
        CEnum<Operation_Add> _1("add", {"add"});
        CEnum<Operation_Remove> _2("remove", {"remove"});
        CEnum<Operation_Connect> _3("connect", {"connect"});
        CEnum<Operation_Delete> _4("del", {"del"});
        CEnum<Operation_RemoveAll> _5("remove_all", {"remove_all"});
        CEnum<Operation_Teleport> _6("tp", {"tp"});
        CEnum<Operation_List> _7("list", {"list"});
        MakeCommand("fp", "FakePlayer Helper Command", 0);
        CmdOverload(fp, _onAddCommand, "add", "name");
        CmdOverload(fp, _onRemoveCommand, "remove", "name");
        CmdOverload(fp, _onConnectCommand, "connect", "name");
        CmdOverload(fp, _onDeleteCommand, "del", "name");
        CmdOverload(fp, _onRemoveAllCommand, "remove_all");
        CmdOverload(fp, _onListCommand, "list");
        if (cfg->allowTeleport) {
            CmdOverload(fp, _onTeleportCommand1, "tp", "name", "dst");
            //CmdOverload(fp, _onTeleportCommand2, "tp", "name", "pos", "dim");
        }
    });
}

#elif defined(BDS_V1_18)

#define TYPE_SOFT_ENUM CommandParameterDataType::SOFT_ENUM
#define TYPE_ENUM CommandParameterDataType::ENUM
#define ARG_ENUM(e, ename, desc) \
  makeMandatory<CommandParameterDataType::ENUM>(&FpCommand::e, desc, ename)
#define ARG(v, desc) \
  makeMandatory(&FpCommand::v, desc)
#define ARG_OPTION(v, i, desc) \
  makeOptional(&FpCommand::v, desc, &FpCommand::i)

using namespace RegisterCommandHelper;

class FpCommand : public Command {
    enum Operation : int {
        Connect,
        Add,
        Remove,
        RemoveAll,
        Delete,
        Set,
        List,
        Teleport
    } op;
    enum DimID : int {
        Overworld = 0,
        Nether,
        TheEnd
    } dim;
    std::string name;
    std::string settings;
    bool val = false;
    bool acc = true; // Allow chat control
    CommandSelector<Actor> cActor;
    CommandSelector<Player> cPlayer;
    CommandPosition dstPos;

    bool acc_set = false;
    bool val_set = false;
    bool dim_set = false;
    bool pos_set = false;

public:

    void execute(CommandOrigin const& ori, CommandOutput& outp) const {
        switch (op) {
            case Operation::Add:
                onAddCommand(ori, outp, name);
                break;
            case Operation::Connect:
                onConnectCommand(ori, outp, name);
                break;
            case Operation::Remove: {
                auto res = cPlayer.results(ori);
                if (res.empty()) {
                    outp.error("No FakePlayer to remove");
                    break;
                }
                for (auto it = res.begin(); it != res.end(); ++it) {
                    onRemoveCommand(ori, outp, (*it)->getNameTag());
                }
                break;
            }
            case Operation::Delete:
                onDeleteCommand(ori, outp, name);
                break;
            case Operation::RemoveAll:
                onRemoveAllCommand(ori, outp);
                break;
            case Operation::List:
                onListCommand(ori, outp);
                break;
            case Operation::Teleport: {
                auto res = cPlayer.results(ori);
                if (res.empty()) {
                    outp.error("No FakePlayer to teleport");
                    break;
                }
                if (pos_set) {
                    int dimid = dim_set ? dim : 0;
                    if (!dim_set && (OriginType)ori.getOriginType() == OriginType::Player) {
                        dimid = ori.getPlayer()->getDimensionId();
                    }
                    onTeleportCommand(ori, outp, name, dstPos.getPosition(ori, Vec3(0, 0, 0)), dimid);
                }
                else {
                    auto res1 = cActor.results(ori);
                    if (res1.empty()) {
                        outp.error("No target to teleport the FakePlayer to");
                        break;
                    }
                    if (res1.count() != 1) {
                        outp.error("Too many targets to teleport the FakePlayer to");
                        break;
                    }
                    onTeleportCommand(ori, outp, name, (*res1.begin())->getPosition(), (*res1.begin())->getDimensionId());
                }
                break;
            }

        }
    }

    static void setup(CommandRegistry* registry) {
        registry->registerCommand("fp", "Manage the FakePlayers",
            CommandPermissionLevel::Any,
            { CommandFlagValue::None },
            { (CommandFlagValue)0x80 });
        registry->addEnum<Operation>("Connect", { {"connect", Operation::Connect} });
        registry->addEnum<Operation>("Add", { {"add", Operation::Add} });
        registry->addEnum<Operation>("Remove", { {"remove", Operation::Remove} });
        registry->addEnum<Operation>("RemoveAll", { {"remove_all", Operation::RemoveAll} });
        registry->addEnum<Operation>("Delete", { {"delete", Operation::Delete} });
        registry->addEnum<Operation>("Set", { {"set", Operation::Set} });
        registry->addEnum<Operation>("List", { {"list", Operation::List} });
        registry->addEnum<Operation>("Teleport", { {"tp", Operation::Teleport} });
        registry->addEnum<DimID>("Dimension", { {"overworld", DimID::Overworld}, 
            {"nether", DimID::Nether}, {"end", DimID::TheEnd} });
        registry->registerOverload<FpCommand>(
            "fp",
            ARG_ENUM(op, "Connect", "Connect"),
            ARG(name, "FakePlayer name")
        );
        registry->registerOverload<FpCommand>(
            "fp",
            ARG_ENUM(op, "Add", "Add"),
            ARG(name, "FakePlayer name")
            //ARG_OPTION(acc, acc_set, "Allow chat control(default true)")
        );
        registry->registerOverload<FpCommand>(
            "fp",
            ARG_ENUM(op, "Remove", "Remove"),
            ARG(cPlayer, "FakePlayer")
        );
        registry->registerOverload<FpCommand>("fp", ARG_ENUM(op, "RemoveAll", "RemoveAll"));
        registry->registerOverload<FpCommand>(
            "fp",
            ARG_ENUM(op, "Delete", "Delete"),
            ARG(name, "FakePlayer name")
        );
        /*registry->registerOverload<FpCommand>(
            "fp",
            ARG_ENUM(op, "Set", "Set"),
            ARG(name, "FakePlayer name"),
            makeMandatory<TYPE_SOFT_ENUM>(
                &FpCommand::settings, "Settings name", "Settings"),
            ARG(val, "Value")
        );*/
        registry->registerOverload<FpCommand>(
            "fp",
            ARG_ENUM(op, "List", "List"),
            ARG_OPTION(val, val_set, "Display offline FakePlayers(default false)")
        );
        if (cfg->allowTeleport) {
            registry->registerOverload<FpCommand>(
                "fp",
                ARG_ENUM(op, "Teleport", "Teleport"),
                ARG(cPlayer, "FakePlayer"),
                ARG(cActor, "Dst entity")
            );
            registry->registerOverload<FpCommand>(
                "fp",
                ARG_ENUM(op, "Teleport", "Teleport"),
                ARG(cPlayer, "FakePlayer"),
                makeMandatory(&FpCommand::dstPos, "Dst", &FpCommand::pos_set),
                makeOptional<TYPE_ENUM>(
                    &FpCommand::dim, "Dimension", "Dimension", &FpCommand::dim_set)
            );
        }

    }

};

void subscribeCommandRegistry() {
    Event::RegCmdEvent::subscribe([&](Event::RegCmdEvent ev) {
        PRINT(lpk->localize("event.register.cmd"));
        try {
            FpCommand::setup(ev.mCommandRegistry);
        }
        catch (seh_exception e) {
            PRINT<ERROR, RED>(e.what());
        }
        return true;
    });
}

#endif
