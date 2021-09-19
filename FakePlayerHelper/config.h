#ifndef CONFIG_H
#define CONFIG_H
#include "pch.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

using rapidjson::Document, rapidjson::Value;

namespace FPHelper
{
	class Config
	{
	public:
		std::string skin = ""; // steve/alex/random
		std::string lang = "";
		bool allow_tp = true;
		bool kick_fp = false; // Kick fakeplayer when summoner left
		//bool remove_fp = false; // Remove fakeplayer when disconnect
		unsigned int max_global_fp = 0; // 0 = Unlimited
		unsigned int max_player_fp = 0; // 0 = Unlimited
		int ws_port = 0;
		struct Permission
		{
			enum : char { ALL, Specified, ConsoleOnly } type;
			std::vector<xuid_t> allow_list;
		} perm;
	private:
		Document doc;
		rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> alloc;
	public:
		Config(){}
		inline void init()
		{
			doc.SetObject();
			if (fs::exists(FPH_CONFIG))
			{
				std::fstream fstm(FPH_CONFIG, std::ios::out | std::ios::in | std::ios::app);
				std::ostringstream oss;
				oss << fstm.rdbuf();
				std::string buf = oss.str();
				doc.Parse(buf.c_str());
				fstm.close();
				if (doc.HasParseError())
				{
					throw std::exception(("Failed to parse JSON: errcode(" + std::to_string(doc.GetParseError()) + ")").c_str());
					return;
				}
				alloc = doc.GetAllocator();
			}
		}
		inline bool writeJson()
		{
			rapidjson::StringBuffer buf;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
			bool res = doc.Accept(writer);
			FILE* fp = fopen(FPH_CONFIG.c_str(), "w+");
			fseek(fp, 0, 0);
			fwrite(buf.GetString(), buf.GetSize(), 1, fp);
			fclose(fp);
			return res && buf.GetSize();
		}
		inline void parseJson()
		{
			if (!doc.HasMember("Skin") || !doc["Skin"].IsString()) 
				doc.AddMember("Skin", Value().SetString("random"), alloc);

			if (!doc.HasMember("Language") || !doc["Language"].IsString())
				doc.AddMember("Language", Value().SetString("zh-cn"), alloc);

			if (!doc.HasMember("Allow_TP") || !doc["Allow_TP"].IsBool())
				doc.AddMember("Allow_TP", true, alloc);

			if (!doc.HasMember("Kick_FP_when_summoner_left") || !doc["Kick_FP_when_summoner_left"].IsBool())
				doc.AddMember("Kick_FP_when_summoner_left", false, alloc);

			if (!doc.HasMember("Max_Global_FP") || !doc["Max_Global_FP"].IsInt())
				doc.AddMember("Max_Global_FP", 10, alloc);

			if (!doc.HasMember("Max_Player_FP") || !doc["Max_Player_FP"].IsInt())
				doc.AddMember("Max_Player_FP", 2, alloc);

			if (!doc.HasMember("WS_Port") || !doc["WS_Port"].IsInt())
				doc.AddMember("WS_Port", 54321, alloc);

			if (!doc.HasMember("Permission") || !doc["Permission"].IsObject())
				doc.AddMember("Permission", Value().SetObject(), alloc);

			if (!doc["Permission"].HasMember("Allow") || !doc["Permission"]["Allow"].IsString())
				doc["Permission"].AddMember("Allow", Value().SetString("ALL"), alloc);

			if (!doc["Permission"].HasMember("Specified") || !doc["Permission"]["Specified"].IsArray())
				doc["Permission"].AddMember("Specified", Value().SetArray().PushBack((xuid_t)1145141919810, alloc), alloc);

			writeJson();
			skin = doc["Skin"].GetString();
			lang = doc["Language"].GetString();
			allow_tp = doc["Allow_TP"].GetBool();
			kick_fp = doc["Kick_FP_when_summoner_left"].GetBool();
			max_global_fp = doc["Max_Global_FP"].GetInt();
			max_player_fp = doc["Max_Player_FP"].GetInt();
			ws_port = doc["WS_Port"].GetInt();
			if (ws_port <= 0 || ws_port > 65535)
				throw std::exception("Invalid websocket port!!!");
			std::string perm_str = doc["Permission"]["Allow"].GetString();
			if (perm_str == "ALL") perm.type = Permission::ALL;
			else if (perm_str == "Specified") perm.type = Permission::Specified;
			else if (perm_str == "ConsoleOnly") perm.type = Permission::ConsoleOnly;
			else throw std::exception("Invalid permission");
			if (perm.type == Permission::Specified && !doc["Permission"]["Specified"].Empty())
			{
				for (u32 i = 0; i < doc["Permission"]["Specified"].Size(); i++)
					perm.allow_list.push_back(doc["Permission"]["Specified"][i].GetUint64());
			}
		}
	};
}

#endif // !CONFIG_H
