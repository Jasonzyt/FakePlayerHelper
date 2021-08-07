#include "pch.h"
#include "mass.h"
#include "logger.h"
#include "config.h"
#include "langpack.h"
#include "fakeplayer.h"
#include <loader/hook.h>
#include <mc/Command.h>
#include <api/commands.h>
#include <api/Basic_Event.h>

using namespace std;

namespace FPHelper
{
	LangPack* lpk = new LangPack;
	Config* cfg = new Config;
	FPWS* fpws = new FPWS;
	stdio_commit sc("[FPH] ");
	Logger<stdio_commit*> coutp(&sc, true);
	VA p_spscqueue;
	Level* level = nullptr;
	Minecraft* mc = nullptr;

	namespace CMD
	{
		enum class FPCMD_List { List = 1 };
		enum class FPCMD_Add { Add = 1 };
		enum class FPCMD_Remove { Remove = 1 };
		enum class FPCMD_TP1 { TP = 1 };
		enum class FPCMD_TP2 { TP = 1 };
		enum class FPCMD_Remove_All { Remove = 1 };

		enum class FPCMD_Dimension : int
		{
			Overworld = 1,
			Nether = 2,
			The_End = 3
		};

		bool ListCmd(CommandOrigin const& ori, CommandOutput& outp, MyEnum<FPCMD_List>)
		{
			auto type = ori.getOriginType();
			if (type == OriginType::DedicatedServer || type == OriginType::Player)
			{
				ostringstream oss;
				oss << format("%s: %d/%d", LANG("listcmd.opt.total"), fpws->fp_list.size(), cfg->max_global_fp);
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
							LANG("listcmd.opt.fppos"), LANG(getDimensionName(it->dim)), Vec3ToString(it->pos).c_str());
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
			auto type = ori.getOriginType();
			if (type == OriginType::DedicatedServer || type == OriginType::Player)
			{
				if (fpws->fp_list.size() >= cfg->max_global_fp)
				{
					outp.error(LANG("fpcmd.global_limit.exceed"));
					return true;
				}
				string fp_summoner;
				xuid_t fp_summoner_xuid = 0;
				if (type == OriginType::DedicatedServer) fp_summoner = "[Console]";
				else
				{
					int sum = 0;
					Player* exer = (Player*)ori.getEntity();
					fp_summoner = exer->getNameTag();
					fp_summoner_xuid = getXuid(exer);
					for (auto& it : fpws->fp_list)
						if (it->summoner_xuid == fp_summoner_xuid)
							sum++;
					if (sum >= cfg->max_player_fp)
					{
						outp.error(LANG("fpcmd.player_limit.exceed"));
						return true;
					}
				}
				auto res = fpws->add(new FakePlayer(name, fp_summoner, fp_summoner_xuid));
				if (res.tp == FPWS::ErrorType::Success) outp.success(LANG("fpcmd.success"));
				else outp.error(format(LANG("fpcmd.fail.format"), res.reason));
			}
			return true;
		}
		bool RemoveCmd(CommandOrigin const& ori, CommandOutput& outp, MyEnum<FPCMD_Remove>, std::string name)
		{
			auto type = ori.getOriginType();
			if (type == OriginType::DedicatedServer || type == OriginType::Player)
			{
				for (auto& it : fpws->fp_list)
				{
					if (it->name == name)
					{
						auto res = fpws->remove(it);
						if (res.tp == FPWS::ErrorType::Success) outp.success(LANG("fpcmd.success"));
						else outp.error(format(LANG("fpcmd.fail.format"), res.reason));
						return true;
					}
				}
				outp.error(format(LANG("fpcmd.fail.format"), LANG("fpcmd.cant_find_fp")));
			}
			return true;
		}
		bool RemoveAllCmd(CommandOrigin const& ori, CommandOutput& outp, MyEnum<FPCMD_Remove_All>)
		{
			auto type = ori.getOriginType();
			if (type == OriginType::DedicatedServer || type == OriginType::Player)
			{
				outp.error(LANG("fp.api.unfinished"));
			}
			return true;
		}
		bool TeleportCmd(CommandOrigin const& ori, CommandOutput& outp, MyEnum<FPCMD_TP1>, 
			string name, CommandSelector<Actor>& tg)
		{
			auto type = ori.getOriginType();
			if (type == OriginType::DedicatedServer || type == OriginType::Player)
			{
				if (tg.results(ori).count() == 0)
				{
					outp.error(LANG("fpcmd.selector.not_matched"));
					return true;
				}
				for (auto& it : fpws->fp_list)
				{
					if (it->name.c_str() == name.c_str())
					{
						auto target = *(tg.results(ori).begin());
						it->teleport(target->getPos(), (int)(target->getDimensionId()));
						return true;
					}
				}
				outp.error(format(LANG("fpcmd.fail.format"), LANG("fpcmd.cant_find_fp")));
			}
			return true;
		}
#if defined(BDS_V1_16)
		bool TeleportCmd_Pos(CommandOrigin const& ori, CommandOutput& outp, MyEnum<FPCMD_TP2>,
			std::string name, CommandPositionFloat& pos, optional<MyEnum<FPCMD_Dimension>> dim)
		{
			auto type = ori.getOriginType();
			if (type == OriginType::DedicatedServer || type == OriginType::Player)
			{
				for (auto& it : fpws->fp_list)
				{
					if (it->name.c_str() == it->name.c_str())
					{
						int exer_dim = 0;
						if (type == OriginType::Player) exer_dim = ori.getEntity()->getDimensionId();
						it->teleport(*(pos.getPosition(ori)), (dim.Set() ? (int)((FPCMD_Dimension)dim.val()) - 1 : exer_dim));
						return true;
					}
				}
				outp.error(format(LANG("fpcmd.fail.format"), LANG("fpcmd.cant_find_fp")));
			}
			return true;
		}
#elif defined(BDS_V1_17)
		bool TeleportCmd_Pos(CommandOrigin const& ori, CommandOutput& outp, MyEnum<FPCMD_TP2>,
			std::string name, float x, float y, float z, optional<MyEnum<FPCMD_Dimension>> dim)
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
						it->teleport(Vec3{ x,y,z } , (dim.Set() ? (int)((FPCMD_Dimension)dim.val()) - 1 : exer_dim));
						return true;
					}
				}
				outp.error(format(LANG("fpcmd.fail.format"), LANG("fpcmd.cant_find_fp")));
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
			CEnum<CMD::FPCMD_Dimension> _cenum7("dimen", { "overworld","nether","end" });
			CmdOverload(fp, CMD::ListCmd, "list");
			CmdOverload(fp, CMD::AddCmd, "add", LANG("fpcmd.fpname"));
			CmdOverload(fp, CMD::RemoveCmd, "remove", LANG("fpcmd.fpname"));
			CmdOverload(fp, CMD::RemoveAllCmd, "remove_all");
			CmdOverload(fp, CMD::TeleportCmd, "tp", LANG("fpcmd.fpname"), LANG("fpcmd.tp.selector"));
#if defined(BDS_V1_16)
			CmdOverload(fp, CMD::TeleportCmd_Pos, "tp", LANG("fpcmd.fpname"), LANG("fpcmd.tp.dst"), LANG("fpcmd.tp.dim"));
#elif defined(BDS_V1_17)
			CmdOverload(fp, CMD::TeleportCmd_Pos, "tp", LANG("fpcmd.fpname"), "x", "y", "z", LANG("fpcmd.tp.dim"));
#else
#error "BDS version is wrong"
#endif
		});
		PRINT("Plugin started! Ver", FPH_VERSION, ' ', FPH_VERTYPE);
		/*thread ad([]() {
			while (true)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DK_CYAN);
				cout << "[FPH][AD] " << endl;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				this_thread::sleep_for(chrono::seconds(120));
			}
		});*/
	}
}
using namespace FPHelper;
FPHAPI bool IsFakePlayer(Player* pl);
THook(VA, "??0?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@QEAA@_K@Z", VA _this)
{
	p_spscqueue = original(_this);
	return p_spscqueue;
}
THook(void, "?sendLoginMessageLocal@ServerNetworkHandler@@QEAAXAEBVNetworkIdentifier@@"
	"AEBVConnectionRequest@@AEAVServerPlayer@@@Z",
	void* ServerNetworkHandler_this, NetworkIdentifier* Ni, void* ConnectionRequest, ServerPlayer* sp)
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
			fp->dim = pl->getDimensionId();
			fp->pos = pl->getPos();
			fpws->wait_list.pop_back();
			fpws->fp_list.push_back(fp);
			auto info = format(LANG("fp.join.info.format"), fp->name.c_str(),
				LANG(getDimensionName(fp->dim)), Vec3ToString(fp->pos).c_str());
			PRINT(info);
			sendTextAll(info, TextType::RAW);
		}
	}
	return original(ServerNetworkHandler_this, Ni, ConnectionRequest, sp);
}
THook(void, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z",
	ServerNetworkHandler* _this, ServerPlayer* sp, bool a3)
{
	auto pl = (Player*)sp;
	for (auto it = fpws->fp_list.begin();
		it != fpws->fp_list.end(); it++)
	{
		FakePlayer* fp = *it;
		if ((*it)->name == pl->getNameTag())
		{
			fpws->fp_list.erase(it);
			auto info = format(LANG("fp.left.info.format"), fp->name.c_str(), 
				LANG(getDimensionName(fp->dim)), Vec3ToString(fp->pos).c_str());
			delete fp;
			PRINT(info);
			sendTextAll(info, TextType::RAW);
			return original(_this, sp, a3);
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
				auto info = format(LANG("fp.summoner.left.kick"), (*it)->name.c_str());
				PRINT(info);
				sendTextAll(info, TextType::RAW);
			}
		}
	}
	return original(_this, sp, a3);
}
THook(void, "?initAsDedicatedServer@Minecraft@@QEAAXXZ", Minecraft* _mc) 
{
	mc = _mc;
	original(mc);
}
THook(void, "?startServerThread@ServerInstance@@QEAAXXZ", void* a) 
{
	fpws->connect_ws();
	PRINT(format(LANG("fpws.version.msg.format"),fpws->getVersion()));
	level = mc->getLevel();
	original(a);
}

// FakePlayerHelper API
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