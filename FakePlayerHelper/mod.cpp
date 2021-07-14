#include "pch.h"
#include "mass.h"
#include "logger.h"
#include "config.h"
#include "fakeplayer.h"
#include <loader/hook.h>
#include <mc/Command.h>
#include <api/commands.h>
#include <api/Basic_Event.h>

using namespace std;

namespace FPHelper
{
	Config* cfg;
	FPWS* fpws;
	stdio_commit sc("[FPH] ");
	Logger<stdio_commit*> coutp(&sc, true);
	VA p_spscqueue;

	namespace CMD
	{
		enum class FPCMD_List { List = 1 };
		enum class FPCMD_Add_Remove
		{
			Add = 1,
			Remove = 2
		};
		enum class FPCMD_TP1 { TP = 1 };
		enum class FPCMD_TP2 { TP = 1 };

		bool ListCmd(CommandOrigin const& ori, CommandOutput& outp, MyEnum<FPCMD_List>)
		{
			auto type = ori.getOriginType();
			if (type == OriginType::DedicatedServer || type == OriginType::Player)
			{
				ostringstream oss;
				oss << "Corrent FakePlayer: " << fpws->fp_list.size() << "/" << cfg->max_global_fp << endl;
				if (fpws->fp_list.size())
				{
					oss << "=========== FakePlayer List ===========" << endl;
					int i = 1;
					for (auto& it : fpws->fp_list)
					{
						// - [1] Name: FakePlayer_1(Summoner: Jasonzyt) Pos: Overworld(100,64,50)
						oss << "- [" << i << "] Name: " << it->name << "(Summoner: " << it->summoner_name << ") "
							<< "Pos: " << getDimensionName(it->dim) << "(" << it->x << "," << it->y << "," << it->z << ")" << endl;
					}
					oss << "=======================================" << endl;
				}
				outp.success(oss.str());
			}
			return true;
		}
		bool AddRemoveCmd(CommandOrigin const& ori, CommandOutput& outp, MyEnum<FPCMD_Add_Remove> op, std::string name)
		{
			auto type = ori.getOriginType();
			if (type == OriginType::DedicatedServer || type == OriginType::Player)
			{
				switch (op)
				{
				case FPCMD_Add_Remove::Add:
				{
					string fp_summoner;
					xuid_t fp_summoner_xuid = 0;
					if (type == OriginType::DedicatedServer) fp_summoner = "[Console]";
					else
					{
						Player* exer = (Player*)ori.getEntity();
						
					}
					auto res = fpws->add(new FakePlayer(name, fp_summoner, fp_summoner_xuid));
					break;
				}
				case FPCMD_Add_Remove::Remove:
				{
					break;
				}
				}
			}
			return true;
		}
	}

	void dll_init()
	{
		PRINT("FakePlayerHelper loaded! Author: Jasonzyt");
		PRINT("Reading config...");
		cfg->init();
		cfg->parseJson();
		Event::addEventListener([](RegCmdEV ev) {
			PRINT("Registering command...");
			CMDREG::SetCommandRegistry(ev.CMDRg);
			MakeCommand("fp", "FakePlayer command", CommandPermissionLevel::Normal);
			CEnum<CMD::FPCMD_List> _cenum1("list", { "list" });
			CmdOverload(fp, CMD::ListCmd, "list");
		});
		PRINT("Plugin started! Ver", FPH_VERSION, ' ', FPH_VERTYPE);
	}
}
using namespace FPHelper;
THook(VA, "??0?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@QEAA@_K@Z", VA _this)
{
	p_spscqueue = original(_this);
	return p_spscqueue;
}