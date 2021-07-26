#ifndef LANGPACK_H
#define LANGPACK_H
#include "pch.h"
#include "logger.h"
#include "rapidjson/document.h"
#include <unordered_map>
#define LANG(x) lpk->get(x)

using rapidjson::Document;

namespace FPHelper
{
	class LangPack
	{
	public:
		std::unordered_map<std::string, std::string> lang;
		LangPack(){}
		inline void init(std::unordered_map<std::string, std::string> m)
		{
			lang = m;
		}
		inline void init(const std::string& file, const std::string& ltype)
		{
			Document doc;
			doc.SetObject();
			std::fstream fstm(file, std::ios::out | std::ios::in | std::ios::app);
			std::ostringstream oss;
			oss << fstm.rdbuf();
			std::string buf = oss.str();
			if (buf.empty())
			{
				fstm << FPH_LANGPACK_CONTENT;
				buf = FPH_LANGPACK_CONTENT.c_str();
			}
			doc.Parse(buf.c_str());
			fstm.close();
			if (doc.HasParseError())
			{
				throw std::exception(("Failed to parse JSON: errcode(" + std::to_string(doc.GetParseError()) + ")").c_str());
				return;
			}
			if (!doc.HasMember(ltype.c_str()))
			{
				throw std::exception(("Cannot find language type: " + ltype).c_str());
				return;
			}
			for (auto iter = doc[ltype.c_str()].MemberBegin(); iter != doc[ltype.c_str()].MemberEnd(); ++iter)
			{
				auto key = iter->name.GetString();
				auto val = iter->value.GetString();
				PRINT<DEBUG, BLUE>(key, ": ", val);
				lang.emplace(std::pair<std::string, std::string>(key, val));
			}
		}
		inline const char* get(const std::string& key)
		{
			if (!key.empty())
			{
				auto res = lang.find(key);
				if (res != lang.end())
					return res->second.c_str();
				PRINT<ERROR, RED>("Cannot find langpack for ", key);
			}
			return key.c_str();
		}
	};
}

#endif // !LANGPACK_H
