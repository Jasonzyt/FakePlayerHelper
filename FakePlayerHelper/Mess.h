#ifndef MASS_H
#define MASS_H
#include "pch.h"
#include <nlohmann/json.hpp>
#if defined(BDS_V1_16)
#include <mc/Player.h>
#include <mc/Level.h>
#include <mc/Certificate.h>
#include <api/types/types.h>
#include <loader/loader.h>
#elif defined(BDS_LATEST)
#include <MC/Player.hpp>
#include <MC/Level.hpp>
#include <MC/Certificate.hpp>
#include <HookAPI.h>
#endif
#include "Logger.h"
#include "LangPack.h"

struct Packet;

struct RelativeFloat 
{
	float offset;
	bool relative;
};

namespace FPHelper
{
	inline void reloadWhiteList() {
		SymCall("?reload@WhitelistFile@@QEAA?AW4FileReadResult@@XZ", int, void*)(wlfile);
	}
	inline void addPlayerToWhiteList(const std::string& name) {
		nlohmann::json j;
		std::ostringstream oss;
		std::fstream fstm("whitelist.json", std::ios::app | std::ios::in);
		oss << fstm.rdbuf();
		fstm.close();
		try {
			j = nlohmann::json::parse(oss.str());
		}
		catch (std::exception& e) {
			PRINT<ERROR, RED>("Can't parse whitelist file: ", e.what());
			return;
		}
		for (auto& it : j) {
			if (it["name"] == name) {
				return;
			}
		}
		j.push_back(nlohmann::json{
			{"name", name},
			{"ignoresPlayerLimit", false}
		});
		fstm = std::fstream("whitelist.json", std::ios::ate | std::ios::out);
		fstm << j.dump();
		fstm.close();
		reloadWhiteList();
	}
	inline void forEachPlayer(std::function<bool(Player&)> cb) {
#if defined(BDS_V1_16)
		SymCall("?forEachPlayer@Level@@QEBAXV?$function@$$A6A_NAEBVPlayer@@@Z@std@@@Z",
			void, Level*, std::function<bool(Player&)>)(level, cb);
#elif defined(BDS_LATEST)
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
	inline Player* getPlayerByNameTag(const std::string& name) {
		Player* player = nullptr;
		forEachPlayer([&](Player& pl) -> bool {
			if (do_hash((&pl)->getNameTag()) == do_hash(name))
			{
				player = &pl;
				return false;
			}
		});
		return player;
	}
	inline void sendText(Player* pl, const std::string& text, TextType tp)
	{
#if defined(BDS_V1_16)
		Packet* pkt;
		SymCall(
			"?createPacket@MinecraftPackets@@SA?AV?$shared_ptr@VPacket@@@std@@W4MinecraftPacketIds@@@Z",
			void*, Packet**, int)(&pkt, 9);  
		std::string* src = new std::string("Server");
		std::string* msg = new std::string(text);
		dAccess<char, 32>(pkt) = (char)tp;
		dAccess<std::string*, 40>(pkt) = src;
		dAccess<std::string*, 80>(pkt) = msg;
		pl->sendNetworkPacket(*pkt);
#elif defined(BDS_LATEST)
		pl->sendTextPacket(text, tp);
#endif
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
    // Use sendText instead plz
	inline void sendMessage(Actor* pl, const std::string& msg) {
		PRINT<DEBUG, BLUE>("called! msg=", msg, " pl=", pl);
		std::vector<std::string> vec;
		vec.push_back("23333"); // At least one element
		SymCall("?displayLocalizableMessage@CommandUtils@@YAX_NAEAVPlayer@@AEBV?$basic_string"
			"@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV?$vector@V?$basic_string@DU?$"
			"char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_"
			"traits@D@std@@V?$allocator@D@2@@std@@@2@@4@@Z", void, bool, Actor*, const std::string&,
			std::vector<std::string>&)(true, pl, msg, vec);
	}
	inline void sendMessageAll(const std::string& msg) {
		forEachPlayer([&](Player& pl) -> bool {
			Player* player = &pl;
			if (!player) return false;
			sendMessage(player, msg);
			return true;
		});
	}
	inline std::string getVersion() {
		std::string a;
		SymCall("?getGameVersionString@Common@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			std::string&, std::string&)(a);		// IDA Common::getGameVersionString
		return a;
	}
	inline Certificate* getCert(Player* pl) {
#if defined(BDS_V1_16)
		return FETCH(Certificate*, pl + 2736);
#elif defined(BDS_LATEST)
		return pl->getCertificate();
#endif
	}
	inline xuid_t getXuid(Player* pl) {
#if defined(BDS_V1_16)
		std::string xuid_str = SymCall(
			"?getXuid@ExtendedCertificate@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@"
			"@std@@AEBVCertificate@@@Z", std::string, void*)(getCert(pl));
#elif defined(BDS_LATEST)
			std::string xuid_str = ExtendedCertificate::getXuid(pl->getCertificate());
#endif
		return atoll(xuid_str.c_str());
	}
	inline void teleport(Actor* actor, Vec3 dst, int dim) {
#if defined(BDS_V1_16)
		try {
			SymCall("?teleport@TeleportCommand@@SAXAEAVActor@@VVec3@@PEAV3@V?$AutomaticID@VDimension@@H@@VRelativeFloat"
				"@@4HAEBUActorUniqueID@@@Z", void, Actor&, Vec3, Vec3*, AutomaticID<Dimension, int>,
				RelativeFloat, RelativeFloat, int, ActorUniqueID const&)(*actor, dst, nullptr, dim,
				{ 0,true }, { 0,true }, 12, actor->getUniqueID());
		}
		catch(const seh_excpetion& e) 
		{
			PRINT<ERROR, RED>("SEH Exception: [", e.code(), ']', e.what());
		}
#elif defined(BDS_LATEST)
		actor->teleport(dst, dim);
#endif
	}

	inline std::vector<std::string> split(std::string str, char ch) {
		std::vector<std::string> result;
		if (!str.empty())
		{
			std::istringstream iss(str);
			string line;
			while (getline(iss, line, ch))
				result.push_back(line);
		}
		return result;
	}
	template<typename ... Args>
	inline std::string format(const std::string& format, Args... args) {
		int size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
		if (size <= 0)
			return "";
		std::unique_ptr<char[]> buf(new char[size]);
		snprintf(buf.get(), size, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size - 1);
	}

	inline std::string getDimensionName(int dim) {
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
	inline bool equals(const Vec3& _Left, const Vec3& _Right) {
		if (_Left.x == _Right.x && 
			_Left.y == _Right.y && 
			_Left.z == _Right.z) 
			return true;
		return false;
	}
	inline std::string Vec3ToString(const Vec3& pos) {
		return format("(%.2lf, %.2lf, %.2lf)", pos.x, pos.y, pos.z);
	}
	
}

#endif // !MASS_H
