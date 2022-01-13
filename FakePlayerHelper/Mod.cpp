#include "pch.h"
#include "Config.h"
#include "LangPack.h"
#include "FakePlayer.h"
#include "Mess.h"
#if defined(BDS_V1_16)
#include <loader/Loader.h>
#include <mc/Level.h>
#elif defined(BDS_LATEST)
#include <Global.h>
#include <EventAPI.h>
#include <MC/Minecraft.hpp>
#include <MC/Level.hpp>
#include <MC/ServerLevel.hpp>
#endif

void subscribeCommandRegistry(); // Command.cpp
bool onPlayerJoin(Player* pl);
bool onPlayerLeft(Player* pl);

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
#if defined(BDS_LATEST)
    Event::PlayerJoinEvent::subscribe([&](const Event::PlayerJoinEvent& ev) {
        onPlayerJoin(ev.mPlayer);
        return true;    
    });
    Event::PlayerLeftEvent::subscribe([&](const Event::PlayerLeftEvent& ev) {
        onPlayerLeft(ev.mPlayer);
        return true;
    });
#endif

}

bool onPlayerJoin(Player* pl) {
    auto rname = getRealName(pl);
    if (isFakePlayer(rname)) {
        auto& fp = fakePlayers.at(rname);
        fp->setOnline();
        fp->setPlayerPtr(pl);
        auto dim = lpk->localize(getDimensionName(fp->pl->getDimensionId()));
        auto pos = Vec3ToString(fp->pl->getPos());
        PRINT(lpk->localize("console.join.info.format", fp->name.c_str(), dim.c_str(), pos.c_str()));
        sendTextAll(lpk->localize("gamemsg.join.info.format", fp->name.c_str(), dim.c_str(), pos.c_str()));
    }
    return true;
}

bool onPlayerLeft(Player* pl) {
    if (isOnlineFakePlayer(pl)) {
        auto& fp = fakePlayers.at(getRealName(pl));
        fp->setOffline();
        auto dim = lpk->localize(getDimensionName(fp->pl->getDimensionId()));
        auto pos = Vec3ToString(fp->pl->getPos());
        PRINT(lpk->localize("console.left.info.format", fp->name.c_str(), dim.c_str(), pos.c_str()));
        sendTextAll(lpk->localize("gamemsg.left.info.format", fp->name.c_str(), dim.c_str(), pos.c_str()));
        fp->setPlayerPtr(nullptr);
    }
    if (cfg->kickFakePlayer) {
        auto xuid = getXuid(pl);
        for (auto& [name, fp] : fakePlayers) {
            if (fp->summoner.xuid == xuid) {
                PRINT(lpk->localize("console.summoner.left.kick", name.c_str()));
                sendTextAll(lpk->localize("gamemsg.summoner.left.kick", name.c_str()));
                fpws->remove(fp.get());
            }
        }
    }
    return true;
}

#if defined(BDS_V1_16)

THook(void, "?sendLoginMessageLocal@ServerNetworkHandler@@QEAAXAEBVNetworkIdentifier@@"
    "AEBVConnectionRequest@@AEAVServerPlayer@@@Z", void* thiz, NetworkIdentifier* ni, 
    void* cr, ServerPlayer* sp) {
    onPlayerJoin(sp);
    return original(thiz, ni, cr, sp);
}

THook(void, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z",
    ServerNetworkHandler* thiz, ServerPlayer* sp, bool a3) {
    onPlayerLeft(sp);
    return original(thiz, sp, a3);
}

#endif

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
