#ifndef CONFIG_H
#define CONFIG_H
#include "pch.h"
#include <nlohmann/json.hpp>

namespace FPHelper {

    class Config {
        
        nlohmann::json json;

    public:
        std::string skin = ""; // steve/alex/random
        std::string lang = "";
        bool allow_tp = true;
        bool kick_fp = false; // Kick fakeplayer when summoner left
        //bool remove_fp = false; // Remove fakeplayer when disconnect
        unsigned int max_global_fp = 0; // 0 = Unlimited
        unsigned int max_player_fp = 0; // 0 = Unlimited
        unsigned short ws_port = 0;
        struct Permission
        {
            enum : char { ALL, Specified, ConsoleOnly } type;
            std::vector<xuid_t> allow_list;
        } perm;
    public:
        Config() {}
        inline void init()
        {
            if (fs::exists(FPH_CONFIG))
            {
                std::fstream fstm(FPH_CONFIG, std::ios::out | std::ios::in | std::ios::app);
                std::ostringstream oss;
                oss << fstm.rdbuf();
                std::string buf = oss.str();
                if (buf.empty()) {
                    buf = "{}";
                }
                fstm.close();
                try {
                    json = nlohmann::json::parse(buf);
                }
                catch (nlohmann::json::parse_error& e) {
                    PRINT<ERROR, RED>("Can't parse config: ", e.what());
                    json = nlohmann::json::object();
                }
            }
        }
        inline bool writeJson()
        {
            std::fstream fstm(FPH_CONFIG, std::ios::out | std::ios::ate);
            if (!fstm.is_open()) {
                PRINT<ERROR, RED>("Can't open config file: ", FPH_CONFIG);
                return false;
            }
            fstm << setw(4) << json;
            fstm.close();
        }
        inline void parseJson()
        {
            if (!json.count("Skin") || !json["Skin"].is_string()) {
                json["Skin"] = "random";
            }
            skin = json["Skin"].get<std::string>();

            if (!json.count("Language") || !json["Language"].is_string()) {
                json["Language"] = "en";
            }
            lang = json["Language"].get<std::string>();

            if (!json.count("AllowTeleport") || !json["AllowTeleport"].is_boolean()) {
                json["AllowTeleport"] = true;
            }
            allow_tp = json["AllowTeleport"].get<bool>();

            if (!json.count("KickFakePlayer") || !json["KickFakePlayer"].is_boolean()) {
                json["KickFakePlayer"] = false;
            }
            kick_fp = json["KickFakePlayer"].get<bool>();

            if (!json.count("MaxGlobalFakePlayer") || !json["MaxGlobalFakePlayer"].is_number_unsigned()) {
                json["MaxGlobalFakePlayer"] = 10;
            }
            max_global_fp = json["MaxGlobalFakePlayer"].get<unsigned int>();

            if (!json.count("MaxPlayerFakePlayer") || !json["MaxPlayerFakePlayer"].is_number_unsigned()) {
                json["MaxPlayerFakePlayer"] = 5;
            }
            max_player_fp = json["MaxPlayerFakePlayer"].get<unsigned int>();

            if (!json.count("WebSocketPort") || !json["WebSocketPort"].is_number_integer()) {
                json["WebSocketPort"] = 54321;
            }
            ws_port = json["WebSocketPort"].get<unsigned short>();

            if (!json.count("Permission") || !json["Permission"].is_object()) {
                json["Permission"] = nlohmann::json{{"Allow", "ALL"}, {"Specified", {114514, 1919810}}};
            }
            auto str = json["Permission"]["Allow"].get<std::string>();
            if (str == "ALL") {
                perm.type = Permission::ALL;
            }
            else if (str == "Specified") {
                perm.type = Permission::Specified;
            }
            else if (str == "ConsoleOnly") {
                perm.type = Permission::ConsoleOnly;
            }
            else {
                perm.type = Permission::ALL;
            }
            if (perm.type == Permission::Specified) {
                perm.allow_list = json["Permission"]["Specified"].get<std::vector<xuid_t>>();
            }
        }

    };
    
}

#endif // !CONFIG_H
