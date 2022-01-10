#include "pch.h"
#include "Config.h"
#include "LangPack.h"
#include "FakePlayer.h"
#if defined(BDS_V1_16)
#include <loader/Loader.h>
#include <mc/Level.h>
#elif defined(BDS_LATEST)
#include <Global.h>
#include <MC/Minecraft.hpp>
#include <MC/Level.hpp>
#include <MC/ServerLevel.hpp>
#endif

void subscribeCommandRegistry(); // Command.cpp
namespace FPHelper {

	Config* cfg = nullptr;
    LangPack* lpk = nullptr;
	WebSocket* fpws = nullptr;
#if defined(BDS_V1_16)
	stdio_commit sc("[FPH] ");
	OLogger<stdio_commit*> coutp(&sc, true);
#endif
	Level* level = nullptr;
	Minecraft* mc = nullptr;
	void* wlfile = nullptr;

    void entry() {
        PRINT("FakePlayerHelper loaded! v", VERSION.toString(), " Author: JasonZYT BuildTime: " __DATE__ " " __TIME__);
        cfg = new Config(FPH_CONFIG);
        lpk = new LangPack(FPH_LANGPACK, cfg->lang);
        fpws = new WebSocket();
        PRINT("FakePlayerHelper started!");
        subscribeCommandRegistry();
    }
}

using namespace FPHelper;

bool onPlayerJoin(Player* pl) {

}

// onTick
THook(void, "?tick@ServerLevel@@UEAAXXZ", ServerLevel* thiz) {
	if (fpws) { 
		fpws->tick(); 
	}
	original(thiz);
}
// onServerInit(getMinecraftPtr)
THook(void, "?initAsDedicatedServer@Minecraft@@QEAAXXZ", Minecraft* thiz) {
	mc = thiz;
	original(mc);
}
// onServerStart(getLevelPtr)
THook(void, "?startServerThread@ServerInstance@@QEAAXXZ", void* thiz) {
	original(thiz);
	level = mc->getLevel();
	fpws->start();
}
// onWhitelistFileReload(getWhitelistFilePtr)
THook(int, "?reload@WhitelistFile@@QEAA?AW4FileReadResult@@XZ", void* a) {
	wlfile = a;
	return original(a);
}
