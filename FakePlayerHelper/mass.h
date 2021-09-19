#ifndef MASS_H
#define MASS_H
#include "pch.h"
#include "myPacket.h"
#include "logger.h"
#include "langpack.h"
#include <mc/Player.h>
#include <mc/Level.h>
#include <mc/Certificate.h>
#include <api/types/types.h>
#include <loader/loader.h>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "TextPacket.h"

struct RelativeFloat 
{
	float offset;
	bool relative;
};

namespace FPHelper
{
	inline void reloadWhiteList() 
	{
		SymCall("?reload@WhitelistFile@@QEAA?AW4FileReadResult@@XZ", int, void*)(wlfile);
	}
	inline void addPlayerToWhiteList(const std::string& name)
	{
		rapidjson::Document doc;
		rapidjson::Value obj(rapidjson::kObjectType);
		std::ostringstream oss;
		auto alloc = doc.GetAllocator();
		std::fstream fstm("whitelist.json", std::ios::app | std::ios::in);
		oss << fstm.rdbuf();
		fstm.close();
		doc.Parse(oss.str().c_str());
		for (int i = 0; i < doc.Size(); i++)
			if (doc[i]["name"] == rapidjson::Value().SetString(name.c_str(), alloc)) return;
		obj.AddMember("ignoresPlayerLimit", false, alloc);
		obj.AddMember("name", rapidjson::Value().SetString(name.c_str(), alloc), alloc);
		doc.PushBack(obj, alloc);
		rapidjson::StringBuffer buf;
		rapidjson::Writer writer(buf);
		doc.Accept(writer);
		fstm = std::fstream("whitelist.json", std::ios::ate | std::ios::out);
		fstm << buf.GetString();
		fstm.close();
		reloadWhiteList();
	}
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
#if 0
	inline void sendText(Player* pl, const std::string& text, TextType tp)
	{
		PRINT<DEBUG, BLUE>("called! text=", text);
		/*Packet* pkt;
		SymCall(
			"?createPacket@MinecraftPackets@@SA?AV?$shared_ptr@VPacket@@@std@@W4MinecraftPacketIds@@@Z",
			void*, Packet**, int)(&pkt, 9);  //创建包
#if defined(BDS_V1_16)
		std::string src = "Server";
		std::string msg = text;
		dAccess<char, 40>(pkt) = (char)tp;
		dAccess<std::string*, 48>(pkt) = &src;
		dAccess<std::string*, 80>(pkt) = &msg;
#elif defined(BDS_LATEST)
		dAccess<char, 48>(pkt) = (char)tp;
		dAccess<string, 56>(pkt) = "Server";
		// dAccess<string, 48>(pkt) = this->getName();
		dAccess<string, 88>(pkt) = text;
#else
#error "BDS version is wrong"
#endif
		pl->sendNetworkPacket(*pkt);*/
		TextPacket* pkt;
		SymCall(
			"?createPacket@MinecraftPackets@@SA?AV?$shared_ptr@VPacket@@@std@@W4MinecraftPacketIds@@@Z",
			void*, Packet**, int)((Packet**)&pkt, 9);  //创建包
		std::string src = "Server";
		std::string msg = text;
		pkt->needsTrans = false;
		pkt->type = (TextType)0;
		pkt->sourceName = &src;
		pkt->msg = &msg;
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
#endif
	inline void sendMessage(Actor* pl, const std::string& msg)
	{
		PRINT<DEBUG, BLUE>("called! msg=", msg, " pl=", pl);
		std::vector<std::string> vec;
		vec.push_back("23333"); // 必须要有至少一个元素
		SymCall("?displayLocalizableMessage@CommandUtils@@YAX_NAEAVPlayer@@AEBV?$basic_string"
			"@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV?$vector@V?$basic_string@DU?$"
			"char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_"
			"traits@D@std@@V?$allocator@D@2@@std@@@2@@4@@Z", void, bool, Actor*, const std::string&,
			std::vector<std::string>&)(true, pl, msg, vec);
	}
	inline void sendMessageAll(const std::string& msg)
	{
		forEachPlayer([&](Player& pl) -> bool {
			Player* player = &pl;
			if (!player) return false;
			sendMessage(player, msg);
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
#elif defined(BDS_LATEST)
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

	inline std::vector<std::string> split(std::string str, char ch) 
	{
		std::vector<std::string> rv;
		if (!str.empty())
		{
			std::istringstream iss(str);
			string line;
			while (getline(iss, line, ch))
				rv.push_back(line);
		}
		return rv;
	}
	template<typename ... Args>
	inline std::string format(const std::string& format, Args... args)
	{
		int size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
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
	template<typename ... Args>
	inline std::string localization(const std::string& key, Args... args) 
	{
		return format(LANG(key), args...);
	}
}

#endif // !MASS_H
