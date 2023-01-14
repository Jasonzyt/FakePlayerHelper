#pragma once
#include "pch.h"
#include <nlohmann/json.hpp>
#include <easywsclient/easywsclient.hpp>
#include <ThreadPool.h>
#if defined(BDS_V1_16)
#include <mc/Player.h>
#include <api/types/types.h>
#elif defined(BDS_V1_18)
#include <MC/Player.hpp>
#endif

enum class FPStatus {
    CONNECTING,
    CONNECTED,
    DISCONNECTING,
    DISCONNECTED,
    RECONNECTING,
    STOPPING,
    STOPPED
};

class FakePlayer {

public:

    struct Summoner {
        std::string name;
        xuid_t xuid;
    } summoner;
    Player* pl = nullptr;
    std::string name;
    //std::string skin;
    bool online = false;
    bool allowChatControl = true;

    FakePlayer(Player* pl,
        const std::string& name,
        bool allowChatControl,
        const std::string& sname,
        xuid_t sxuid = 0) {
        this->pl = pl;
        this->name = name;
        this->allowChatControl = allowChatControl;
        this->online = pl;
        this->summoner.name = sname;
        this->summoner.xuid = sxuid;
    }

    void setOnline();
    void setOffline();
    void setPlayerPtr(Player* pl);

    // Send request, not a synchronized function
    void connect();
    void disconnect();
    void setChatControl(bool v);

};

extern std::unordered_map<std::string, std::unique_ptr<FakePlayer>> fakePlayers;
class WebSocket {

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
    std::unordered_map<std::string, PacketType> packetTypeMap{
        { "list", PacketType::List },
        { "add", PacketType::Add },
        { "remove", PacketType::Remove },
        { "connect", PacketType::Connect },
        { "disconnect", PacketType::Disconnect },
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
    std::unordered_map<std::string, EventType> eventTypeMap{
        { "add", EventType::Add },
        { "remove", EventType::Remove },
        { "connect", EventType::Connect },
        { "disconnect", EventType::Disconnect }
    };

    std::mutex mtx;
    std::unordered_map<std::string, int> timer;                // Judge if the packet is timeout
    std::unordered_map<std::string, 
                       std::unique_ptr<nlohmann::json>> resp;  // Response of the packet
    easywsclient::WebSocket::pointer ws = nullptr;             // WebSocket Client
    ThreadPool pool;                                           // Thread pool
    bool connected = false;                                    // If the WebSocket is connected
    int syncTimer = 0;                                         // Sync timer(each 2400 ticks)
    int reconnectCount = 0;                                    // Reconnect count
    std::vector<std::function<void(Player*)>> 
        onConnectCallbacks, onDisconnectCallbacks;             // Callback functions

    WebSocket();
    void start();
    void stop();
    void tick();
    void add(std::unique_ptr<FakePlayer> fp);
    void connect(FakePlayer* fp);
    void remove(FakePlayer* fp);
    void removeAll();
    void del(FakePlayer* fp);
    void getVersion();
    void sync();
    void setChatControl(FakePlayer* fp, bool allowChatControl);
    void send(nlohmann::json& data);

private:

    std::unordered_map<std::string, bool> lastSetChatControl;
    std::unordered_map<std::unique_ptr<FakePlayer>, FakePlayer::Summoner> setSummoner;

    void onAdd(nlohmann::json& j);
    void onRemove(nlohmann::json& j);
    void onConnect(nlohmann::json& j);
    void onDisconnect(nlohmann::json& j);
    void poll();
    void process(nlohmann::json& j);
    PacketType getPacketType(const std::string& type);
    EventType getEventType(const std::string& type);
    static std::string generateID();
    static std::string getSkinName();

};

bool isOnlineFakePlayer(Player* pl);
bool isOnlineFakePlayer(const std::string& name);
bool isFakePlayer(const std::string& name);
FakePlayer* getFakePlayer(Player* pl);
FakePlayer* getFakePlayer(const std::string& name);
std::vector<FakePlayer*> getOnlineFakePlayers();
std::pair<std::vector<FakePlayer*>, std::vector<FakePlayer*>> getFakePlayersCategorized();
