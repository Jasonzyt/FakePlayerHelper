#include "FakePlayer.h"
#include "Logger.h"
#include "LangPack.h"
#include "Config.h"

namespace FPHelper {

    WebSocket::WebSocket(): pool(std::thread::hardware_concurrency()) {
        srand((uint32_t)time(NULL));
    }

    void WebSocket::start() {
        std::string url = "ws://127.0.0.1:" + std::to_string(cfg->port);
        ws = easywsclient::WebSocket::from_url(url);
        if (ws) {
            connected = true;
            reconnectCount = 0;
			PRINT(lpk->localize("fpws.connected"));
			sendTextAll(lpk->localize("gamemsg.ws.connected"));
            process();
            getVersion();
        }
        else {
            if (reconnect) {
                PRINT<ERROR, RED>(lpk->localize("fpws.failed.reconnect"));
            }
            else {
                PRINT<ERROR, RED>(lpk->localize("fpws.failed.connect"), url);
            }
            if (reconnectCount++ < 3) {
                PRINT<ERROR, RED>(lpk->localize("fpws.try.reconnect"));
                start();
            }
            else {
                PRINT<ERROR, RED>(lpk->localize("fpws.stop.reconnecting"));
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
            std::lock_guard<std::mutex> lck(mtx);
            if (++syncTimer >= 2400) {
                syncTimer = 0;
                sync();
            }
            std::vector<std::string> del;
            for (auto& [id, t] : timer) {
                if (t == -1) { // Received
                    if (resp.count(id) && resp[id]) {
                        process(*resp[id]);
                    }
                    del.push_back(id);
                }
                else if (++t >= 100) { // Timeout
				    PRINT<ERROR, RED>(lpk->localize("fpws.wait.timeout"));
                    del.push_back(id);
                }
            }
            for (auto& id : del) {
                timer.erase(id);
                resp.erase(id);
            }
        }
    }

}