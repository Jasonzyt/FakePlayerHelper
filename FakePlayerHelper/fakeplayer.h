#ifndef FAKEPLAYER_H
#define FAKEPLAYER_H
#include "pch.h"
#include "config.h"
#include "logger.h"
#include "threadpool.h"
#include "easywsclient/easywsclient.hpp"
#include <mc/Actor.h>
#include <mc/Player.h>

using easywsclient::WebSocket;

namespace FPHelper
{
	class FakePlayer;
	class FPWS
	{
	public:
		enum class PacketType
		{
			Unknown        = -1,
			List           = 0,
			Add            = 1,
			Remove         = 2,
			Connect        = 3,
			Disconnect     = 4,
			GetState       = 5,
			Remove_All     = 6,
			Connect_All    = 7,
			Disconnect_All = 8,
			GetVersion     = 9,
			GetState_All   = 10,
			SetChatControl = 11
		};
		enum class EventType 
		{
			Unknown    = -1,
			Add        = 0,
			Remove     = 1,
			Connect    = 2,
			Disconnect = 3
		};
		enum class FPState
		{
			CONNECTING,
			CONNECTED,
			DISCONNECTING,
			DISCONNECTED,
			RECONNECTING,
			STOPPING,
			STOPPED
		};
		struct WSPacket
		{
			std::string id;
			PacketType pt = PacketType::Unknown;
			FakePlayer* target = nullptr;
			std::string name;
			bool allowChatControl;
		};
		struct PlayerData 
		{
			std::string name;
			FPState state;
			bool allowChatControl;
		};
		struct Message
		{
			bool set = false;
			std::string id, name, reason, version;
			FPState stat;
			bool success;
			PacketType pt;
			std::vector<std::string> list;
			std::vector<PlayerData> players;
		};
		std::mutex mtx;
		WebSocket::pointer ws = nullptr;
		ThreadPool* pool = nullptr;
		bool connected = false;
		std::unordered_map<std::string, int> timer;
		std::unordered_map<std::string, Message> resps;
		std::vector<std::string> all_fp;
		std::vector<FakePlayer*> fp_list;
		std::vector<FakePlayer*> wait_list;
		int sync_timer = 0;
		int reconnect_num = 0;
		int thread_total = 0;
		FPWS()
		{
			pool = new ThreadPool(std::thread::hardware_concurrency());
			srand(time(0));
		}
		void connect_ws();
		void tick();
		bool add(FakePlayer* fp);
		bool remove(FakePlayer* fp);
		bool remove_all();
		bool getVersion();
		bool list();
		bool getStates();
		void onAdd(Message msg);
		void onRemove(Message msg);
		void onConnect(Message msg);
		void onDisconnect(Message msg);
		static bool IsFakePlayer(Player* pl);
		static bool IsFakePlayer(const std::string& pl);
		bool deleteFakePlayer(const std::string& name);
		const Message parseMessage(const std::string& msg);
	private:
		static PacketType parsePacketType(const std::string& tp);
		static EventType parseEventType(const std::string& ev);
		void process();
		bool send(WSPacket pkt);
		std::string getID();
	};

	class FakePlayer
	{
	public:
		Player* fp_ptr = nullptr;
		std::string name;
		std::string summoner_name;
		xuid_t summoner_xuid = 0;
		bool online = false;
		bool allowChatControl = false;
		FakePlayer(Player* pl) : fp_ptr(pl), summoner_name("Unknown"), online(true) {}
		FakePlayer(const std::string& fp_name, const std::string& fp_summoner_name, 
			xuid_t fp_summoner_xuid, bool allowChatControl = true)
			: name(fp_name), summoner_name(fp_summoner_name), summoner_xuid(fp_summoner_xuid),
			allowChatControl(allowChatControl) {}
		void teleport(const Vec3& pos, int dim);
	};
}

#endif // !FAKEPLAYER_H
