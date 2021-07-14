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
		std::string skin = "random"; // steve/alex/random
		bool allow_tp = true;
		bool kick_fp = false; // Kick fakeplayer when summoner left
		int max_global_fp = 0; // 0 = Unlimited
		int max_player_fp = 0; // 0 = Unlimited
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
		inline void init()
		{
			doc.SetObject();
			FILE* fp = fopen(FPH_CONFIG.c_str(), "a+");
			size_t sz = fs::file_size(FPH_CONFIG);
			char* buf = new char[sz];
			fseek(fp, 0, 0);
			if (fread(buf, sz, 1, fp));
				doc.Parse(buf);
				if (doc.HasParseError()) throw std::exception("Failed to parse JSON");
			alloc = doc.GetAllocator();
			fclose(fp);
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
			allow_tp = doc["Allow_TP"].GetBool();
			kick_fp = doc["Kick_FP_when_summoner_left"].GetBool();
			max_global_fp = doc["Max_Global_FP"].GetInt();
			max_player_fp = doc["Max_Player_FP"].GetInt();
			ws_port = doc["WS_Port"].GetInt();
			std::string perm_str = doc["Permission"]["Allow"].GetString();
			if (perm_str == "ALL") perm.type = Permission::ALL;
			else if (perm_str == "Specified") perm.type = Permission::Specified;
			else if (perm_str == "ConsoleOnly") perm.type = Permission::ConsoleOnly;
			else throw std::exception("Invalid permission");
			if (perm.type == Permission::Specified && !doc["Permission"]["Specified"].Empty())
			{
				for (int i = 0; i < doc["Permission"]["Specified"].Size(); i++)
					perm.allow_list.push_back(doc["Permission"]["Specified"][i].GetUint64());
			}
		}
	};
}

#endif // !CONFIG_H
