#ifndef CONFIG_H
#define CONFIG_H
#include "pch.h"
#include <nlohmann/json.hpp>

namespace FPHelper {

    class Config {
        
        nlohmann::json json;
        std::string file;
    public:
        std::string skin = ""; // steve/alex/random
        std::string lang = "";
        bool allowTeleport = true;
        bool kickFakePlayer = false; // Kick fakeplayer when summoner left
        //bool remove_fp = false; // Remove fakeplayer when disconnect
        unsigned int maxGlobalFakePlayers = 0; // 0 = Unlimited
        unsigned int maxPlayerFakePlayers = 0; // 0 = Unlimited
        unsigned short wsPort = 0;
        struct Permission
        {
            enum : char { ALL, Specified, ConsoleOnly } type;
            std::vector<xuid_t> allow_list;
        } perm;

        inline void init()
        {
            if (fs::exists(file))
            {
                std::fstream fstm(file, std::ios::out | std::ios::in | std::ios::app);
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
            parse();
        }
        inline void write()
        {
            std::fstream fstm(file, std::ios::out | std::ios::ate);
            if (!fstm.is_open()) {
                PRINT<ERROR, RED>("Can't open config file: ", file);
                return;
            }
            fstm << std::setw(4) << json;
            fstm.close();
        }
        inline void parse()
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
            allowTeleport = json["AllowTeleport"].get<bool>();

            if (!json.count("KickFakePlayer") || !json["KickFakePlayer"].is_boolean()) {
                json["KickFakePlayer"] = false;
            }
            kickFakePlayer = json["KickFakePlayer"].get<bool>();

            if (!json.count("MaxGlobalFakePlayer") || !json["MaxGlobalFakePlayer"].is_number_unsigned()) {
                json["MaxGlobalFakePlayer"] = 10;
            }
            maxGlobalFakePlayers = json["MaxGlobalFakePlayer"].get<unsigned int>();

            if (!json.count("MaxPlayerFakePlayer") || !json["MaxPlayerFakePlayer"].is_number_unsigned()) {
                json["MaxPlayerFakePlayer"] = 5;
            }
            maxPlayerFakePlayers = json["MaxPlayerFakePlayer"].get<unsigned int>();

            if (!json.count("WebSocketPort") || !json["WebSocketPort"].is_number_integer()) {
                json["WebSocketPort"] = 54321;
            }
            wsPort = json["WebSocketPort"].get<unsigned short>();

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
            write();
        }

        Config(const std::string& file) : file(file) {
            init();
        }

    };
    
}

#endif // !CONFIG_H
