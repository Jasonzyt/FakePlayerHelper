#pragma once
#include "pch.h"
#include <easywsclient/easywsclient.hpp>
#include <ThreadPool.h>
#if defined(BDS_V1_16)
#include <mc/Player.h>
#elif defined(BDS_LATEST)
#include <MC/Player.hpp>
#endif

namespace FPHelper {

    class FakePlayer {
    
    public:

        struct Summoner {
            std::string name;
            xuid_t xuid;
        } summoner;
        Player* pl = nullptr;
        std::string name;
        std::string skin;
        bool online = false;
        bool allowChatControl = false;

        FakePlayer(Player* pl, 
                   const std::string& name, 
                   bool allowChatControl, 
                   const std::string& sname, 
                   xuid_t sxuid) {
            this->pl = pl;
            this->name = name;
            this->allowChatControl = allowChatControl;
            this->online = pl;
            this->summoner.name = sname;
            this->summoner.xuid = sxuid;
        }

    }

    extern std::vector<std::unique_ptr<FakePlayer>> fpList;
    class WebSocket {

    public:

        enum class PacketType {
			Unknown        = -1,
			List           = 0,
			Add            = 1,
			Remove         = 2,
			Connect        = 3,
			Disconnect     = 4,
            List           = 5,
			GetState       = 6,
			Remove_All     = 7,
			Connect_All    = 8,
			Disconnect_All = 9,
			GetVersion     = 10,
			GetState_All   = 11,
			SetChatControl = 12
		};
        static const std::unordered_map<std::string, PacketType> packetTypeMap = {
            { "list", PacketType::List },
            { "add", PacketType::Add },
            { "remove", PacketType::Remove },
            { "connect", PacketType::Connect },
            { "disconnect", PacketType::Disconnect },
            { "list", PacketType::List },
            { "getState", PacketType::GetState },
            { "remove_all", PacketType::Remove_All },
            { "connect_all", PacketType::Connect_All },
            { "disconnect_all", PacketType::Disconnect_All },
            { "getVersion", PacketType::GetVersion },
            { "getState_all", PacketType::GetState_All },
            { "setChatControl", PacketType::SetChatControl }
        };
        enum class EventType {
            Unknown    = -1,
            Add        = 0,
            Remove     = 1,
            Connect    = 2,
            Disconnect = 3
        };
        static const std::unordered_map<std::string, EventType> eventTypeMap = {
            { "add", EventType::Add },
            { "remove", EventType::Remove },
            { "connect", EventType::Connect },
            { "disconnect", EventType::Disconnect }
        };

        std::mutex mtx;
        std::unordered_map<std::string, int> timer;                // Judge if the packet is timeout
        std::unordered_map<std::string, 
                           std::unique_ptr<nlohmann::json>> resp;  // Response of the packet
        std::unordered_map<std::string, FakePlayer*> waiting;      // Waiting joining players
        easywsclient::WebSocket::pointer ws = nullptr;             // WebSocket Client
        ThreadPool pool;                                           // Thread pool
        bool connected = false;                                    // If the WebSocket is connected
        int syncTimer = 0;                                         // Sync timer(each 2400 ticks)
        int reconnectCount = 0;                                    // Reconnect count
        std::vector<std::function<void(Player*)>> onConnect, onDisconnect; // Callback functions

        WebSocket();
        void start();
        void stop();
        void tick();
        void add(std::unique_ptr<FakePlayer> fp);
        void remove(std::unique_ptr<FakePlayer> fp);
        void removeAll();
        void del(std::unique_ptr<FakePlayer> fp);
        void getVersion();
        void sync();
        void send(const std::string& data);

    private:

        void onAdd(nlohmann::json& j);
        void onRemove(nlohmann::json& j);
        void onConnect(nlohmann::json& j);
        void onDisconnect(nlohmann::json& j);
        void process();
        void process(nlohmann::json& j);
        std::string generateID();

    };

}