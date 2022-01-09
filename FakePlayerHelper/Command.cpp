#include "pch.h"
#include "Config.h"
#include "Mess.h"
#if defined(BDS_V1_16)
#include <api/commands.h>
#include <mc/Command.h>
#include <mc/CommandReg.h>
#elif defined(BDS_LATEST)
#include <EventAPI.h>
#include <RegCommandAPI.h>
#endif

#define EXISTS(x, y) (std::find(x.begin(), x.end(), y) != x.end())

namespace FPHelper {

	bool checkPermission(CommandOrigin const& ori, CommandOutput& outp) {
        if ((OriginType)ori.getOriginType() == OriginType::DedicatedServer) {
			return true;
		}
		if (cfg->perm.type == Config::Permission::ALL) {
			return true;
		}
		else if (cfg->perm.type == Config::Permission::Specified) {
            if ((OriginType)ori.getOriginType() == OriginType::Player) {
				auto player = ori.getPlayer();
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
	}

	void onAddCommand(CommandOrigin const& ori, CommandOutput& outp, std::string name) {
        if (checkPermission(ori, outp)) {
            
        }
	}

}

#if defined(BDS_V1_16)

#elif defined(BDS_LATEST)

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
		Overworld,
		Nether,
		TheEnd
	} dim;
	std::string name;
	std::string settings;
	bool val = false;
	bool acc = true; // Allow chat control
	CommandSelector<Actor> cActor;
	CommandSelector<Actor> cPlayer;
	CommandPosition dstPos;

	bool acc_set = false;
	bool val_set = false;
	bool dim_set = false;

public:

	void execute(CommandOrigin const& ori, CommandOutput& outp) {

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
			{"nether", DimID::Nether}, {"the_end", DimID::TheEnd} });
		registry->addSoftEnum("Settings", { "ChatControl" });
		registry->registerOverload<FpCommand>("fp", ARG_ENUM(op, "Connect", "Connect"));
		registry->registerOverload<FpCommand>(
			"fp",
			ARG_ENUM(op, "Add", "Add"),
			ARG(name, "FakePlayer name"),
			ARG_OPTION(acc, acc_set, "Allow chat control(default true)")
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
		registry->registerOverload<FpCommand>(
			"fp",
			ARG_ENUM(op, "Set", "Set"),
			ARG(name, "FakePlayer name"),
			makeMandatory<TYPE_SOFT_ENUM>(
				&FpCommand::settings, "Settings name", "Settings"),
			ARG(val, "Value")
		);
		registry->registerOverload<FpCommand>(
			"fp",
			ARG_ENUM(op, "List", "List"),
			ARG_OPTION(val, val_set, "Display offline FakePlayers(default false)")
		);
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
			ARG(dstPos, "Dst"),
			makeOptional<TYPE_ENUM>(
				&FpCommand::dim, "Dimension", "Dimension", &FpCommand::dim_set)
		);

	}

};

void subscribeCommandRegistry() {
	Event::RegCmdEvent::subscribe([&](Event::RegCmdEvent ev) {
		FpCommand::setup(ev.mCommandRegistry);
		return true;
	});
}

#endif
