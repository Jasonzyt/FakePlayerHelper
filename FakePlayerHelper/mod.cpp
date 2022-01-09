#include "pch.h"
#include "Mess.h"
#include "Logger.h"
#include "Config.h"
#include "LangPack.h"
#include "fakeplayer.h"
#include <loader/hook.h>
#include <mc/Command.h>
#if defined(BDS_V1_16)
#include <api/commands.h>
#include <api/Basic_Event.h>
#elif defined(BDS_LATEST)
#include <api/regCommandHelper.h>
#include <api/basicEvent.h>
#endif

using namespace std;

namespace FPHelper
{
	LangPack* lpk = new LangPack;
	Config* cfg = new Config;
	WebSocket* fpws = new WebSocket;
#if defined(BDS_V1_16)
	stdio_commit sc("[FPH] ");
	OLogger<stdio_commit> coutp(sc, true);
#endif
	Level* level = nullptr;
	Minecraft* mc = nullptr;
	void* wlfile = nullptr;

	namespace CMD
	{
		enum class FPCMD_List { List = 1 };
		enum class FPCMD_Add { Add = 1 };
		enum class FPCMD_Remove { Remove = 1 };
		enum class FPCMD_Connect { Connect = 1 };
		enum class FPCMD_TP1 { TP = 1 };
		enum class FPCMD_TP2 { TP = 1 };
		enum class FPCMD_Remove_All { Remove_All = 1 };

		enum class FPCMD_Dimension : int
		{
			Overworld = 1,
			Nether = 2,
			The_End = 3
		};

		bool checkCommandPermission(CommandOrigin const& ori, CommandOutput& outp) 
		{
			switch (cfg->perm.type) 
			{
			case Config::Permission::ConsoleOnly:
				if (ori.getOriginType() != OriginType::DedicatedServer)
				{
					outp.error(LANG("fpcmd.no.permission"));
					return false;
				}
				break;
			case Config::Permission::Specified:
				if (ori.getOriginType() == OriginType::Player)
				{
					auto pl = (Player*)ori.getEntity();
					if (pl) 
					{
						auto xuid = getXuid(pl);
						for (auto& it : cfg->perm.allow_list)
							if (it == xuid) return true;
					}
				}
				else if (ori.getOriginType() == OriginType::DedicatedServer) return true;
				outp.error(LANG("fpcmd.no.permission"));
				return false;
			case Config::Permission::ALL:
				break;
			}
			return true;
		}

		bool ConnectCmd(CommandOrigin const& ori, CommandOutput& outp, MyEnum<FPCMD_Connect>)
		{
			if (checkCommandPermission(ori, outp))
			{
				auto type = ori.getOriginType();
				if (type == OriginType::DedicatedServer || type == OriginType::Player)
				{
					if (fpws)
					{
						if (!fpws->connected) fpws->connect_ws();
						else outp.error(localization("fpws.already.connected"));
					}
					else outp.error("NullPointerException");
				}
			}
			return true;
		}
		bool ListCmd(CommandOrigin const& ori, CommandOutput& outp, MyEnum<FPCMD_List>)
		{
			auto type = ori.getOriginType();
			if (type == OriginType::DedicatedServer || type == OriginType::Player)
			{
				ostringstream oss;
				if (cfg->max_global_fp == 0 && cfg->max_player_fp == 0)
					oss << format("%s: %d", LANG("listcmd.opt.total"), fpws->fp_list.size());
				else 
				{
					if (type == OriginType::DedicatedServer) 
					{
						oss << format("%s: %d/%d", LANG("listcmd.opt.total"),
							fpws->fp_list.size(), cfg->max_global_fp);
					}
					else
					{
						int sum = 0;
						auto xuid = getXuid((Player*)ori.getEntity());
						for (auto& it : fpws->fp_list)
							if (it->summoner_xuid == xuid)
								sum++;
						if (cfg->max_global_fp == 0)
							oss << format("%s: %d (Yours: %d/%d)", LANG("listcmd.opt.total"),
								fpws->fp_list.size(), sum, cfg->max_player_fp);
						else if (cfg->max_player_fp == 0)
							oss << format("%s: %d/%d", LANG("listcmd.opt.total"),
								fpws->fp_list.size(), cfg->max_global_fp);
						else
							oss << format("%s: %d/%d (Yours: %d/%d)", LANG("listcmd.opt.total"),
								fpws->fp_list.size(), cfg->max_global_fp, sum, cfg->max_player_fp);
					}
				}
				if (fpws->fp_list.size())
				{
					oss << format("\n=========================== %s ===========================\n", LANG("listcmd.opt.header"));
					int i = 1;
					for (auto& it : fpws->fp_list)
					{
						// =========================== FakePlayer List ===========================
						// - [1] Name: FakePlayer_1(Summoner: Jasonzyt) Pos: Overworld(100, 64, 50)
						// - [2] 假人名: FakePlayer_2(召唤者: Jasonzyt) 位置: 主世界(114, 51, 4)
						// =======================================================================
						PRINT<DEBUG, BLUE>(it->fp_ptr->getDimensionId(), 
							Vec3ToString(it->fp_ptr->getPos()));
						oss << format("- [%d] %s: %s(%s: %s) %s: %s%s)\n", i, LANG("listcmd.opt.fpname"),
							it->name.c_str(), LANG("listcmd.opt.summoner"), it->summoner_name.c_str(), 
							LANG("listcmd.opt.fppos"), LANG(getDimensionName(it->fp_ptr->getDimensionId())), 
							Vec3ToString(it->fp_ptr->getPos()).c_str());
						i++;
					}
					oss << "=======================================================================" << endl;
				}
				outp.success(oss.str());
			}
			return true;
		}
		bool AddCmd(CommandOrigin const& ori, CommandOutput& outp, MyEnum<FPCMD_Add>, std::string name)
		{
			if (checkCommandPermission(ori, outp))
			{
				auto type = ori.getOriginType();
				if (type == OriginType::DedicatedServer || type == OriginType::Player)
				{
					if (!fpws->IsFakePlayer(name))
					{
						if (cfg->max_global_fp > 0)
						{
							if (fpws->fp_list.size() >= cfg->max_global_fp)
							{
								outp.error(LANG("fpcmd.global_limit.exceed"));
								return true;
							}
						}
						string fp_summoner;
						xuid_t fp_summoner_xuid = 0;
						if (type == OriginType::DedicatedServer) fp_summoner = "[Console]";
						else
						{
							Player* exer = (Player*)ori.getEntity();
							fp_summoner = exer->getNameTag();
							fp_summoner_xuid = getXuid(exer);
							if (cfg->max_global_fp > 0)
							{
								int sum = 0;
								for (auto& it : fpws->fp_list)
									if (it->summoner_xuid == fp_summoner_xuid)
										sum++;
								if (sum >= cfg->max_player_fp)
								{
									outp.error(LANG("fpcmd.player_limit.exceed"));
									return true;
								}
							}
						}
						auto res = fpws->add(new FakePlayer(name, fp_summoner, fp_summoner_xuid, true));
						return true;
					}
					outp.error(LANG("fpcmd.fp.already.exists"));
				}
			}
			return true;
		}
		bool RemoveCmd(CommandOrigin const& ori, CommandOutput& outp, MyEnum<FPCMD_Remove>, std::string name)
		{
			if (checkCommandPermission(ori, outp))
			{
				auto type = ori.getOriginType();
				if (type == OriginType::DedicatedServer || type == OriginType::Player)
				{
					for (auto& it : fpws->fp_list)
					{
						if (do_hash(it->name) == do_hash(name))
						{
							auto res = fpws->remove(it);
							return true;
						}
					}
					outp.error(format(LANG("fpcmd.fail.format"), LANG("fpcmd.cant_find_fp")));
				}
			}
			return true;
		}
		bool RemoveAllCmd(CommandOrigin const& ori, CommandOutput& outp, MyEnum<FPCMD_Remove_All>)
		{
			if (checkCommandPermission(ori, outp))
			{
				auto type = ori.getOriginType();
				if (type == OriginType::DedicatedServer || type == OriginType::Player)
				{
					outp.error(LANG("fp.api.unfinished"));
				}
			}
			return true;
		}
		bool TeleportCmd(CommandOrigin const& ori, CommandOutput& outp, MyEnum<FPCMD_TP1>, 
			string name, CommandSelector<Actor>& tg)
		{
			if (checkCommandPermission(ori, outp))
			{
				auto type = ori.getOriginType();
				if (type == OriginType::DedicatedServer || type == OriginType::Player)
				{
					auto res = tg.results(ori);
					if (res.empty())
					{
						outp.error(LANG("fpcmd.selector.not_matched"));
						return true;
					}
					for (auto& it : fpws->fp_list)
					{
						if (it->name == name)
						{
							auto target = *res.begin();
							auto dst = target->getPos();
							auto dst_dim = (int)(target->getDimensionId());
							it->teleport(dst, dst_dim);
							return true;
						}
					}
					outp.error(format(LANG("fpcmd.fail.format"), LANG("fpcmd.cant_find_fp")));
				}
			}
			return true;
		}

#if defined(BDS_V1_16)
		bool TeleportCmd_Pos(CommandOrigin const& ori, CommandOutput& outp, MyEnum<FPCMD_TP2>,
			std::string name, CommandPositionFloat& pos, optional<MyEnum<FPCMD_Dimension>> dim)
		{
			if (checkCommandPermission(ori, outp))
			{
				auto type = ori.getOriginType();
				if (type == OriginType::DedicatedServer || type == OriginType::Player)
				{
					for (auto& it : fpws->fp_list)
					{
						if (it->name == it->name)
						{
							auto dst = *pos.getPosition(ori);
							int exer_dim = 0;
							if (type == OriginType::Player) exer_dim = ori.getEntity()->getDimensionId();
							auto dst_dim = (dim.Set() ? (int)((FPCMD_Dimension)dim.val()) - 1 : exer_dim);
							it->teleport(dst, dst_dim);
							return true;
						}
					}
					outp.error(format(LANG("fpcmd.fail.format"), LANG("fpcmd.cant_find_fp")));
				}
			}
			return true;
		}
#elif defined(BDS_LATEST)
		bool TeleportCmd_Pos(CommandOrigin const& ori, CommandOutput& outp, MyEnum<FPCMD_TP2>,
			std::string name, float x, float y, float z, optional<MyEnum<FPCMD_Dimension>> dim)
		{
			if (checkCommandPermission(ori, outp))
			{
				auto type = ori.getOriginType();
				if (type == OriginType::DedicatedServer || type == OriginType::Player)
				{
					for (auto& it : fpws->fp_list)
					{
						if (it->name == name)
						{
							int exer_dim = 0;
							if (type == OriginType::Player) exer_dim = ori.getEntity()->getDimensionId();
							auto dst = Vec3{ x,y,z };
							auto dst_dim = (dim.Set() ? (int)((FPCMD_Dimension)dim.val()) - 1 : exer_dim);
							it->teleport(dst, dst_dim);
							return true;
						}
					}
					outp.error(format(LANG("fpcmd.fail.format"), LANG("fpcmd.cant_find_fp")));
				}
			}
			return true;
		}
#else
#error "BDS version is wrong"
#endif
	}

	void dll_init()
	{
		PRINT("FakePlayerHelper loaded! Author: Jasonzyt");
		_set_se_translator(seh_excpetion::TranslateSEHtoCE);
		if (!fs::exists(FPH_FOLDER)) fs::create_directories(FPH_FOLDER);
		try {
			cfg->init();
			cfg->parseJson();
			lpk->init(FPH_LANGPACK, cfg->lang);
			PRINT(LANG("dllinit.success.read_cfg"));
		}
		catch (exception e) {
			PRINT<ERROR, RED>(LANG("dllinit.exception"), ": ", e.what());
		}
		Event::addEventListener([](RegCmdEV ev) {
			PRINT(LANG("dllinit.register_cmd"));
			CMDREG::SetCommandRegistry(ev.CMDRg);
			MakeCommand("fp", string(LANG("fpcmd.root.desc")).c_str(), CommandPermissionLevel::Normal);
			CEnum<CMD::FPCMD_List> _cenum1("list", { "list" });
			CEnum<CMD::FPCMD_Add> _cenum2("add", { "add" });
			CEnum<CMD::FPCMD_Remove> _cenum3("remove", { "remove" });
			CEnum<CMD::FPCMD_Remove_All> _cenum4("remove_all", { "remove_all" });
			CEnum<CMD::FPCMD_TP1> _cenum5("tp", { "tp" });
			CEnum<CMD::FPCMD_TP2> _cenum6("tp", { "tp" });
			CEnum<CMD::FPCMD_Dimension> _cenum7("dimension", { "overworld","nether","end" });
			CEnum<CMD::FPCMD_Connect> _cenum8("connect", { "connect" });
			CmdOverload(fp, CMD::ConnectCmd, "connect");
			CmdOverload(fp, CMD::ListCmd, "list");
			CmdOverload(fp, CMD::AddCmd, "add", LANG("fpcmd.fpname"));
			CmdOverload(fp, CMD::RemoveCmd, "remove", LANG("fpcmd.fpname"));
			CmdOverload(fp, CMD::RemoveAllCmd, "remove_all");
			if (cfg->allow_tp)
			{
				CmdOverload(fp, CMD::TeleportCmd, "tp", LANG("fpcmd.fpname"), LANG("fpcmd.tp.selector"));
#if defined(BDS_V1_16)
				CmdOverload(fp, CMD::TeleportCmd_Pos, "tp", LANG("fpcmd.fpname"), LANG("fpcmd.tp.dst"), LANG("fpcmd.tp.dim"));
#elif defined(BDS_LATEST)
				CmdOverload(fp, CMD::TeleportCmd_Pos, "tp", LANG("fpcmd.fpname"), "x", "y", "z", LANG("fpcmd.tp.dim"));
#else
#error "BDS version is wrong"
#endif
			}
		});
#if defined(TEST_VERSION)
		PRINT("Plugin started! Ver", FPH_VERSION, ' ', FPH_VERTYPE, "(ONLY FOR TEST)");
#elif defined(ACTIONS_BUILD)
		PRINT("Plugin started! Ver", FPH_VERSION, ' ', FPH_VERTYPE, "(ACTIONS)");
#else 
		PRINT("Plugin started! Ver", FPH_VERSION, ' ', FPH_VERTYPE, "(RELEASE)");
#endif
	}
}

using namespace FPHelper;
FPHAPI bool IsFakePlayer(Player* pl);
THook(void, "?sendLoginMessageLocal@ServerNetworkHandler@@QEAAXAEBVNetworkIdentifier@@"
	"AEBVConnectionRequest@@AEAVServerPlayer@@@Z",
	void* thiz, NetworkIdentifier* Ni, void* ConnectionRequest, ServerPlayer* sp)
{
	auto pl = (Player*)sp;
	auto pname = pl->getNameTag();
	for (int i = 0; i < fpws->wait_list.size(); i++)
	{
		auto fp = fpws->wait_list.back();
		if (fp->name == pname)
		{
			fp->fp_ptr = pl;
			fp->online = true;
			fpws->wait_list.pop_back();
			fpws->fp_list.push_back(fp);
			for (auto& it : fpws->onConnect_cb) it(pl);
			auto cinfo = localization("console.join.info.format", fp->name.c_str(), 
				LANG(getDimensionName(fp->fp_ptr->getDimensionId())), Vec3ToString(fp->fp_ptr->getPos()).c_str());
			auto ginfo = localization("gamemsg.join.info.format", fp->name.c_str(), 
				LANG(getDimensionName(fp->fp_ptr->getDimensionId())), Vec3ToString(fp->fp_ptr->getPos()).c_str());
			PRINT(cinfo);
			sendMessageAll(ginfo);
		}
	}
	return original(thiz, Ni, ConnectionRequest, sp);
}
THook(void, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z",
	ServerNetworkHandler* thiz, ServerPlayer* sp, bool a3)
{
	auto pl = (Player*)sp;
	for (auto it = fpws->fp_list.begin();
		it != fpws->fp_list.end(); it++)
	{
		FakePlayer* fp = *it;
		if ((*it)->name == pl->getNameTag())
		{
			fpws->fp_list.erase(it);
			for (auto& it2 : fpws->onConnect_cb) it2(pl);
			auto cinfo = localization("console.left.info.format", fp->name.c_str(), 
				LANG(getDimensionName(fp->fp_ptr->getDimensionId())), Vec3ToString(fp->fp_ptr->getPos()).c_str());
			auto ginfo = localization("gamemsg.left.info.format", fp->name.c_str(),  
				LANG(getDimensionName(fp->fp_ptr->getDimensionId())), Vec3ToString(fp->fp_ptr->getPos()).c_str());
			delete fp;
			PRINT(cinfo);
			sendMessageAll(ginfo);
			return original(thiz, sp, a3);
		}
	}
	if (cfg->kick_fp)
	{
		for (auto it = fpws->fp_list.begin();
			it != fpws->fp_list.end(); it++)
		{
			if (getXuid(pl) == (*it)->summoner_xuid)
			{
				fpws->remove((*it));
				auto info = format(LANG("gamemsg.summoner.left.kick"), (*it)->name.c_str());
				PRINT(info);
				sendMessageAll(info);
			}
		}
	}
	return original(thiz, sp, a3);
}
THook(void, "?tick@ServerLevel@@UEAAXXZ", ServerLevel* thiz) 
{
	if (fpws) fpws->tick();
	original(thiz);
}
THook(void, "?initAsDedicatedServer@Minecraft@@QEAAXXZ", Minecraft* thiz) 
{
	mc = thiz;
	original(mc);
}
THook(void, "?startServerThread@ServerInstance@@QEAAXXZ", void* thiz) 
{
	original(thiz);
	level = mc->getLevel();
	fpws->connect_ws();
}
THook(int, "?reload@WhitelistFile@@QEAA?AW4FileReadResult@@XZ", void* a)
{
	wlfile = a;
	return original(a);
}

// FakePlayerHelper API
extern "C" {
	FPHAPI vector<Player*> getFakePlayers()
	{
		vector<Player*> rv;
		for (auto& it : fpws->fp_list)
			rv.push_back(it->fp_ptr);
		return rv;
	}
	FPHAPI bool IsFakePlayer(Player* pl)
	{
		for (auto& it : fpws->fp_list)
			if (pl->getNameTag() == it->name)
				return true;
		return false;
	}
	FPHAPI void addFakePlayerConnectCallback(function<void(Player*)> cb)
	{
		fpws->onConnect_cb.push_back(cb);
	}
	FPHAPI void addFakePlayerDisconnectCallback(function<void(Player*)> cb)
	{
		fpws->onDisconnect_cb.push_back(cb);
	}
}
