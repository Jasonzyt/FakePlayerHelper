#include "FakePlayer.h"
#include "Logger.h"
#include "LangPack.h"
#include "Config.h"
#include "Mess.h"

std::unordered_map<std::string, std::unique_ptr<FakePlayer>> fakePlayers;

// class FakePlayer
void FakePlayer::setOnline() {
    this->online = true;
}
void FakePlayer::setOffline() {
    this->online = false;
}
void FakePlayer::setPlayerPtr(Player* pl) {
    this->pl = pl;
}
void FakePlayer::connect() {
    if (!online) {
        fpws->connect(this);
    }
}
void FakePlayer::disconnect() {
    if (online) {
        fpws->remove(this);
    }
}
void FakePlayer::setChatControl(bool v) {
    fpws->setChatControl(this, v);
}

// class WebSocket
WebSocket::WebSocket(): pool(std::thread::hardware_concurrency()) {
    srand((uint32_t)time(NULL));
}

void WebSocket::start() {
    std::string url = "ws://127.0.0.1:" + std::to_string(cfg->wsPort);
    ws = easywsclient::WebSocket::from_url(url);
    if (ws) {
        connected = true;
        reconnectCount = 0;
        PRINT(lpk->localize("fpws.connected"));
        sendTextAll(lpk->localize("gamemsg.ws.connected"));
        poll();
        getVersion();
        sync();
    }
    else {
        if (reconnectCount) {
            PRINT<ERROR, RED>(lpk->localize("fpws.failed.reconnect"));
        }
        else {
            PRINT<ERROR, RED>(lpk->localize("fpws.failed.connect"), url);
        }
        if (reconnectCount++ < 3) {
            pool.enqueue([&]() {
                std::this_thread::sleep_for(std::chrono::seconds(5));
                PRINT(lpk->localize("fpws.try.reconnect"));
                start();
            });
        }
        else {
            PRINT<ERROR, RED>(lpk->localize("fpws.stop.reconnecting"));
            reconnectCount = 0;
        }
        return;
    }
}

void WebSocket::stop() { // Not used
    connected = false;
    if (ws) {
        ws->close();
        ws = nullptr;
    }
}

void WebSocket::tick() {
    if (connected) {
        if (++syncTimer >= 200) {
            syncTimer = 0;
            sync();
        }
        std::lock_guard<std::mutex> lck(mtx);
        std::vector<std::string> del;
        for (auto& [id, t] : timer) {
            if (t == -1) { // Received
                if (resp.count(id) && resp[id]) {
                    process(*resp[id]);
                }
            }
            else if (++t >= 100) { // Timeout
                PRINT<ERROR, RED>(lpk->localize("fpws.wait.timeout"));
            }
            del.push_back(id);
        }
        for (auto& id : del) {
            timer.erase(id);
            resp.erase(id);
        }
    }
}

void WebSocket::add(std::unique_ptr<FakePlayer> fp) {
    std::lock_guard<std::mutex> lck(mtx);
    if (isFakePlayer(fp->name)) {
        connect(fp.get());
        return;
    }
    if (connected) {
        nlohmann::json j, dat;
        j["id"] = generateID();
        j["type"] = "add";
        dat["name"] = fp->name;
        dat["skin"] = getSkinName();
        dat["allowChatControl"] = fp->allowChatControl;
        fakePlayers.emplace(fp->name, std::move(fp));
        j["data"] = dat;
        send(j);
    }
}

void WebSocket::connect(FakePlayer* fp) {
    std::lock_guard<std::mutex> lck(mtx);
    if (connected) {
        nlohmann::json j, dat;
        j["id"] = generateID();
        j["type"] = "connect";
        dat["name"] = fp->name;
        j["data"] = dat;
        send(j);
    }
}

void WebSocket::remove(FakePlayer* fp) {
    std::lock_guard<std::mutex> lck(mtx);
    if (connected && isOnlineFakePlayer(fp->name)) {
        nlohmann::json j, dat;
        j["id"] = generateID();
        j["type"] = "disconnect";
        dat["name"] = fp->name;
        j["data"] = dat;
        send(j);
    }
}

void WebSocket::removeAll() {
    std::lock_guard<std::mutex> lck(mtx);
    if (connected) {
        nlohmann::json j;
        j["id"] = generateID();
        j["type"] = "disconnect_all";
        send(j);
    }
}

void WebSocket::del(FakePlayer* fp) {
    std::lock_guard<std::mutex> lck(mtx);
    if (connected) {
        nlohmann::json j, dat;
        j["id"] = generateID();
        j["type"] = "remove";
        dat["name"] = fp->name;
        j["data"] = dat;
        send(j);
    }
}

void WebSocket::getVersion() {
    std::lock_guard<std::mutex> lck(mtx);
    if (connected) {
        nlohmann::json j;
        j["id"] = generateID();
        j["type"] = "getVersion";
        send(j);
    }
}

void WebSocket::sync() {
    std::lock_guard<std::mutex> lck(mtx);
    if (connected) {
        nlohmann::json j;
        j["id"] = generateID();
        j["type"] = "getState_all";
        send(j);
    }
}

void WebSocket::setChatControl(FakePlayer* fp, bool allow) {
    std::lock_guard<std::mutex> lck(mtx);
    if (connected && isFakePlayer(fp->name)) {
        nlohmann::json j, dat;
        j["id"] = generateID();
        j["type"] = "setChatControl";
        dat["name"] = fp->name;
        dat["allowChatControl"] = allow;
        j["data"] = dat;
        lastSetChatControl[fp->name] = true;
        send(j);
    }
}

void WebSocket::send(nlohmann::json& data) {
    if (connected) {
        timer[data["id"]] = 0;
        resp[data["id"]] = nullptr;
        auto str = data.dump();
        PRINT<DEBUG, BLUE>("-> ", str);
        ws->send(str);
    }
}

void WebSocket::onAdd(nlohmann::json& data) {
    std::lock_guard<std::mutex> lck(mtx);
    if (connected) {
        if (!isFakePlayer(data["data"]["name"])) {
            fakePlayers.emplace(data["data"]["name"], 
                std::make_unique<FakePlayer>(nullptr, data["data"]["name"], true, "[Unknown]"));
        }
    }
}

void WebSocket::onRemove(nlohmann::json& data) {
    std::lock_guard<std::mutex> lck(mtx);
    if (connected) {
        if (isFakePlayer(data["data"]["name"])) {
            fakePlayers.erase(data["data"]["name"]);
        }
    }
}

void WebSocket::onConnect(nlohmann::json& data) {
    std::lock_guard<std::mutex> lck(mtx);
    if (connected) {
        if (!isFakePlayer(data["data"]["name"])) {
            fakePlayers.emplace(data["data"]["name"],
                                std::make_unique<FakePlayer>(nullptr, data["data"]["name"], true, "[Unknown]"));
        }
        else {
            //fakePlayers[data["data"]["name"]]->setOnline(); 
        }
    }
}

void WebSocket::onDisconnect(nlohmann::json& data) {
    std::lock_guard<std::mutex> lck(mtx);
    if (connected) {
        if (isFakePlayer(data["data"]["name"])) {
            fakePlayers[data["data"]["name"]]->setOffline();
        }
    }
}

void WebSocket::poll() {
    pool.enqueue([&]() {
        while (ws->getReadyState() != easywsclient::WebSocket::CLOSED) {
            ws->poll(20); // If this number is too small, the CPU usage will be high.
            ws->dispatch([&](const std::string& msg) {
                PRINT<DEBUG, GREEN>("<- ", msg);
                if (msg.empty()) {
                    return;
                }
                nlohmann::json j;
                try {
                    j = nlohmann::json::parse(msg);
                }
                catch (nlohmann::json::parse_error& e) {
                    mtx.lock();
                    PRINT<ERROR, RED>(lpk->localize("fpws.parse.json.err"), e.what());
                    mtx.unlock();
                    return;
                }
                if (j.contains("event")) {
                    switch (getEventType(j["event"])) {
                    case EventType::Connect:
                        onConnect(j);
                        break;
                    case EventType::Disconnect:
                        onDisconnect(j);
                        break;
                    case EventType::Add:
                        onAdd(j);
                        break;
                    case EventType::Remove:
                        onRemove(j);
                        break;
                    }
                    return;
                }
                std::lock_guard<std::mutex> lck(mtx);
                resp[j["id"]] = std::make_unique<nlohmann::json>(j);
                timer[j["id"]] = -1;
            });
        }
        connected = false;
        PRINT<WARN, YELLOW>(lpk->localize("fpws.disconnected"));
        sendTextAll(lpk->localize("gamemsg.ws.disconnected"));
        ws = nullptr;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        PRINT(lpk->localize("fpws.try.reconnect"));
        start();
    });
}

void WebSocket::process(nlohmann::json& j) {
    switch (getPacketType(j["type"])) {
    case WebSocket::PacketType::List:
        break;
    case WebSocket::PacketType::Add:
    case WebSocket::PacketType::Connect:
        if (j["data"]["success"]) {
            PRINT(lpk->localize("console.successfully.add.fakeplayer"));
            sendTextAll(lpk->localize("gamemsg.successfully.add.fakeplayer"));
        }
        else {
            PRINT<ERROR, RED>(lpk->localize("console.failed.add.fakeplayer.format", j["data"]["reason"].get<std::string>().c_str()));
            sendTextAll(lpk->localize("gamemsg.failed.add.fakeplayer.format", j["data"]["reason"].get<std::string>().c_str()));
        }
        break;
    case WebSocket::PacketType::Remove:
        if (j["data"]["success"]) {
            fakePlayers.erase(j["data"]["name"]);
        }
        break;
    case WebSocket::PacketType::Disconnect:
        if (j["data"]["success"]) {
            PRINT(lpk->localize("console.successfully.remove.fakeplayer"));
            sendTextAll(lpk->localize("gamemsg.successfully.remove.fakeplayer"));
        }
        else {
            PRINT<ERROR, RED>(lpk->localize("console.failed.remove.fakeplayer.format", j["data"]["reason"].get<std::string>().c_str()));
            sendTextAll(lpk->localize("gamemsg.failed.remove.fakeplayer.format", j["data"]["reason"].get<std::string>().c_str()));
        }
        break;
    case WebSocket::PacketType::GetState:
        break;
    case WebSocket::PacketType::Remove_All:
        for (auto& it : j["data"]["list"]) {
            fakePlayers.erase(it);
        }
        break;
    case WebSocket::PacketType::Connect_All:
        break;
    case WebSocket::PacketType::Disconnect_All:
        break;
    case WebSocket::PacketType::GetVersion:
        PRINT(lpk->localize("fpws.version.msg.format", j["data"]["version"].get<std::string>().c_str()));
        sendTextAll(lpk->localize("gamemsg.ws.fp.version", j["data"]["version"].get<std::string>().c_str()));
        break;
    case WebSocket::PacketType::GetState_All: {
        auto& data = j["data"]["playersData"];
        for (auto iter = data.begin(); iter != data.end(); ++iter) {
            auto& it = iter.value();
            auto status = (FPStatus)it["state"].get<int>();
            auto& name = iter.key();
            if (status == FPStatus::CONNECTED) {
                if (!isOnlineFakePlayer(name)) {
                    auto pl = getPlayerByRealName(name);
                    if (pl) {
                        fakePlayers.emplace(name,
                                            std::make_unique<FakePlayer>(pl, name, it["allowChatControl"], "[Unknown]"));
                        PRINT(lpk->localize("console.found.new.fakeplayer.format", name.c_str()));
                        sendTextAll(lpk->localize("gamemsg.found.new.fakeplayer.format", name.c_str()));
                    }
                }
            }
            else {
                if (!isFakePlayer(name)) {
                    fakePlayers.emplace(name,
                                        std::make_unique<FakePlayer>(nullptr, name, it["allowChatControl"], "[Unknown]"));
                }
                else if (isOnlineFakePlayer(name)) {
                    fakePlayers[name]->setOffline();
                }
            }
        }
        break;
    }
    case WebSocket::PacketType::SetChatControl: {
        auto name = j["data"]["name"].get<std::string>();
        auto exists = lastSetChatControl.count(name);
        if (j["data"]["success"].get<bool>() && exists) {
            if (lastSetChatControl.at(name)) {
                PRINT(lpk->localize("console.enabled.chat.control.format", name.c_str()));
                sendTextAll(lpk->localize("gamemsg.enabled.chat.control.format", name.c_str()));
            }
            else {
                PRINT(lpk->localize("console.disabled.chat.control.format", name.c_str()));
                sendTextAll(lpk->localize("gamemsg.disabled.chat.control.format", name.c_str()));
            }
        }
        if (exists) {
            lastSetChatControl.erase(j["data"]["name"]);
        }
        break;
    }
    default:
        PRINT<ERROR, RED>(lpk->localize("fpws.unknown.packet.type"));
        break;
    }
}


WebSocket::PacketType WebSocket::getPacketType(const std::string& type) {
    if (packetTypeMap.count(type)) {
        return packetTypeMap.at(type);
    }
    return PacketType::Unknown;
}
WebSocket::EventType WebSocket::getEventType(const std::string& type) {
    if (eventTypeMap.count(type)) {
        return eventTypeMap.at(type);
    }
    return EventType::Unknown;
}

std::string WebSocket::generateID() {
    static const std::string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefgnijklmnopqrstuvwxyz";
    std::string res;
    for (int i = 0; i < 20; i++)
    {
        // m <= r <= n
        // rand()%(n-m+1)+m
        int random_num = rand() % 62;
        res += str[random_num];
    }
    return res;
}

std::string WebSocket::getSkinName() {
    if (cfg->skin == "random") {
        return (rand() % 2 ? "steve" : "alex");
    }
    else if (cfg->skin == "alex") {
        return "alex";
    }
    else if (cfg->skin == "steve") {
        return "steve";
    }
    else {
        PRINT<WARN, RED>("fpws.invalid.skin");
        return "steve";
    }
}

bool isOnlineFakePlayer(Player* pl) {
    for (auto& [name, fp] : fakePlayers) {
        if (fp->pl == pl) {
            return true;
        }
    }
    return false;
}
bool isOnlineFakePlayer(const std::string& name) {
    return (fakePlayers.count(name) && fakePlayers[name]->online);
}
bool isFakePlayer(const std::string& name) {
    return fakePlayers.count(name);
}
FakePlayer* getFakePlayer(Player* pl) {
    for (auto& [name, fp] : fakePlayers) {
        if (fp->pl == pl) {
            return fp.get();
        }
    }
    return nullptr;
}
FakePlayer* getFakePlayer(const std::string& name) {
    if (fakePlayers.count(name)) {
        return fakePlayers[name].get();
    }
    return nullptr;
}
std::vector<FakePlayer*> getOnlineFakePlayers() {
    std::vector<FakePlayer*> res;
    for (auto& [name, fp] : fakePlayers) {
        if (fp->online) {
            res.push_back(fp.get());
        }
    }
    return res;
}
std::pair<std::vector<FakePlayer*>, std::vector<FakePlayer*>> getFakePlayersCategorized() {
    std::vector<FakePlayer*> online, offline;
    for (auto& [name, fp] : fakePlayers) {
        if (fp->online) {
            online.push_back(fp.get());
        }
        else {
            offline.push_back(fp.get());
        }
    }
    return std::make_pair(online, offline);
}