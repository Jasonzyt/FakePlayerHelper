#ifndef FAKEPLAYER_H
#define FAKEPLAYER_H
#include "pch.h"
#include "config.h"
#include "logger.h"
#include "threadpool.h"
#include "easywsclient/easywsclient.hpp"
#include <mc/Actor.h>
#include <mc/Player.h>

namespace FPHelper
{
	class FakePlayer;
	class WebSocket
	{
	public:
		enum class PacketType {
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
		enum class EventType {
			Unknown    = -1,
			Add        = 0,
			Remove     = 1,
			Connect    = 2,
			Disconnect = 3
		};
		enum class FPState {
			CONNECTING,
			CONNECTED,
			DISCONNECTING,
			DISCONNECTED,
			RECONNECTING,
			STOPPING,
			STOPPED
		};
		struct WSPacket {
			std::string id;
			PacketType pt = PacketType::Unknown;
			FakePlayer* target = nullptr;
			std::string name;
			bool allowChatControl;
		};
		struct PlayerData {
			std::string name;
			FPState state;
			bool allowChatControl;
		};
		struct Message {
			bool set = false;
			std::string id, name, reason, version;
			FPState stat;
			bool success;
			PacketType pt;
			std::vector<std::string> list;
			std::vector<PlayerData> players;
		};
		std::mutex mtx;
		easywsclient::WebSocket::pointer ws = nullptr;
		ThreadPool* pool = nullptr;
		bool connected = false;
		std::unordered_map<std::string, int> timer;
		std::unordered_map<std::string, Message> resps;
		std::vector<std::string> fpListAll;
		std::vector<std::unique_ptr<FakePlayer>> fpList;
		std::vector<std::unique_ptr<FakePlayer>> waitList;
		std::vector<std::function<void(Player*)>> onConnect_cb;
		std::vector<std::function<void(Player*)>> onDisconnect_cb;
		int reconnect_num = 0;
		WebSocket() {
			pool = new ThreadPool(std::thread::hardware_concurrency());
			srand(time(0));
		}
		void connect_ws();
		void tick();
		bool add(FakePlayer* fp);
		bool remove(FakePlayer* fp);
		bool removeAll();
		bool getVersion();
		bool list();
		bool getStates();
		bool IsInWaitList(const std::string& pl);
		bool IsFakePlayer(Player* pl);
		bool IsFakePlayer(const std::string& pl);
		bool deleteFakePlayer(const std::string& name);
		const Message parseMessage(const std::string& msg);
	private:
		void onAdd(Message msg);
		void onRemove(Message msg);
		void onConnect(Message msg);
		void onDisconnect(Message msg);
		static PacketType parsePacketType(const std::string& tp);
		static EventType parseEventType(const std::string& ev);
		void process();
		bool send(WSPacket pkt);
		static std::string getID();

	};

	class FakePlayer {
	public:
		Player* pl = nullptr;
		std::string name;
		std::string summonerName;
		xuid_t summonerXuid = 0;
		bool online = false;
		bool allowChatControl = false;
		FakePlayer(Player* pl) : pl(pl), summonerName("[Unknown]"), online(true) {
			name = pl->getNameTag();
		}
		FakePlayer(const std::string& fp_name, const std::string& summonerName, 
			xuid_t summonerXuid, bool allowChatControl = true)
			: name(fp_name), summonerName(summonerName), summonerXuid(summonerXuid),
			allowChatControl(allowChatControl), online(true) {}
		void teleport(const Vec3& pos, int dim);

	};
}

#endif // !FAKEPLAYER_H