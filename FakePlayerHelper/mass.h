#ifndef MASS_H
#define MASS_H
#include "pch.h"
#include "myPacket.h"
#include "logger.h"
#include <mc/Player.h>
#include <mc/Level.h>
#include <mc/Certificate.h>
#include <api/types/types.h>
#include <loader/loader.h>

namespace FPHelper
{
	inline bool runcmd(const std::string& cmd)
	{
		extern VA p_spscqueue;
		if (p_spscqueue != 0)
			return SymCall("??$inner_enqueue@$0A@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@AEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
				bool, VA, std::string)(p_spscqueue, cmd);
		return false;
	}
	inline void forEachPlayer(std::function<bool(Player&)> cb)
	{
#if defined(BDS_V1_16)
		SymCall("?forEachPlayer@Level@@QEBAXV?$function@$$A6A_NAEBVPlayer@@@Z@std@@@Z",
			void, Level*, std::function<bool(Player&)>)(level, cb);
#elif defined(BDS_V1_17)
		SymCall("?forEachPlayer@Level@@UEBAXV?$function@$$A6A_NAEBVPlayer@@@Z@std@@@Z",
			void, Level*, std::function<bool(Player&)>)(level, cb);
#else
#error "BDS version is wrong"
#endif
	}
	inline std::vector<Player*> getAllPlayers() {
		std::vector<Player*> player_list;
		forEachPlayer([&](Player& pl) -> bool {
			Player* player = &pl;
			if (!player) return false;
			player_list.push_back(player);
			return true;
		});
		return player_list;
	}
	inline void sendText(Player* pl, const std::string& text, TextType tp)
	{
		Packet* pkt;
		SymCall(
			"?createPacket@MinecraftPackets@@SA?AV?$shared_ptr@VPacket@@@std@@W4MinecraftPacketIds@@@Z",
			void*, Packet**, int)(&pkt, 9);  //创建包
#if defined(BDS_V1_16)
		dAccess<char, 40>(pkt) = (char)tp;
		dAccess<std::string, 48>(pkt) = "Server";
		dAccess<std::string, 80>(pkt) = text;
#elif defined(BDS_V1_17)
#else
		dAccess<char, 48>(pkt) = (char)tp;
		dAccess<string, 56>(pkt) = "Server";
		// dAccess<string, 48>(pkt) = this->getName();
		dAccess<string, 88>(pkt) = text;
#error "BDS version is wrong"
#endif
		pl->sendNetworkPacket(*pkt);
	}
	inline void sendTextAll(const std::string& text, TextType tp)
	{
		forEachPlayer([&](Player& pl) -> bool {
			Player* player = &pl;
			if (!player) return false;
			//sendText(player, text, tp);
			return true;
		});
	}
	inline std::string getVersion()
	{
		std::string a;
		SymCall("?getGameVersionString@Common@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			std::string&, std::string&)(a);		// IDA Common::getGameVersionString
		return a;
	}
	inline Certificate* getCert(Player* pl) 
	{
#if defined(BDS_V1_16)
		return FETCH(Certificate*, pl + 2736);
#elif defined(BDS_V1_17)
		return SymCall("?getCertificate@Player@@QEBAPEBVCertificate@@XZ", Certificate*, Player*)(pl);
#else
#error "BDS version is wrong"
#endif
	}
	inline xuid_t getXuid(Player* pl)
	{
		std::string xuid_str = SymCall(
			"?getXuid@ExtendedCertificate@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@"
			"@std@@AEBVCertificate@@@Z", std::string, void*)(getCert(pl));
		/* BAD CODE
		std::string xuid_str = SymCall("?getPlayerXUID@Level@@QEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBVUUID@mce@@@Z",
			std::string&, Level*, VA)(level, FETCH(VA, pl + 2720));
		*/
		return atoll(xuid_str.c_str());
	}
	inline void teleport(Actor* actor, Vec3 dst, int dim)
	{
#if defined(BDS_V1_16)
		// 先改变维度
		SymCall("?entityChangeDimension@Level@@QEAAXAEAVActor@@V?$AutomaticID@VDimension@@H@@@Z",
			__int64, Level*, Actor*, uint32_t)(level, actor, dim);
		SymCall("?teleportTo@Actor@@UEAAXAEBVVec3@@_NHHAEBUActorUniqueID@@@Z",
			void, Actor*, const Vec3*, const ActorUniqueID&)(actor, &dst, actor->getUniqueID());
#elif defined(BDS_V1_17)
		char mem[128];
		SymCall(
			"?computeTarget@TeleportCommand@@SA?AVTeleportTarget@@AEAVActor@@VVec3@@PEAV4@V?$"
			"AutomaticID@VDimension@@H@@VRelativeFloat@@4H@Z",
			void*, void*, Actor*, Vec3, int, int, float, float,
			int)(&mem, actor, dst, 0, dim, 0, 0, 15);
		SymCall("?applyTarget@TeleportCommand@@SAXAEAVActor@@VTeleportTarget@@@Z", void, Actor*,
			void*)(actor, &mem);
#else
#error "BDS version is wrong"
#endif
	}

	template<typename ... Args>
	inline std::string format(const std::string& format, Args ... args) {
		int size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;
		if (size <= 0)
			return "";
		std::unique_ptr<char[]> buf(new char[size]);
		snprintf(buf.get(), size, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size - 1);
	}

	inline std::string getDimensionName(int dim)
	{
		switch (dim)
		{
		case 0:
			return "Overworld"; // 主世界
		case 1:
			return "Nether"; // 下界
		case 2:
			return "The End"; // 末路之地
		default:
			return "Unknown"; // 未知
		}
	}
	inline bool equal(const Vec3& _Left, const Vec3& _Right)
	{
		if (_Left.x == _Right.x && 
			_Left.y == _Right.y && 
			_Left.z == _Right.z) 
			return true;
		return false;
	}
	inline std::string Vec3ToString(const Vec3& pos)
	{
		return format("(%.2lf, %.2lf, %.2lf)", pos.x, pos.y, pos.z);
	}
}

#endif // !MASS_H
