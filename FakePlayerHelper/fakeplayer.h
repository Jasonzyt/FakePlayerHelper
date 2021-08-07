#ifndef FAKEPLAYER_H
#define FAKEPLAYER_H
#include "pch.h"
#include "config.h"
#include "logger.h"
#include "threadpool.h"
#include "easywsclient/easywsclient.hpp"
#include <mc/Actor.h>
#include <mc/Player.h>

using easywsclient::WebSocket;

namespace FPHelper
{
	class FakePlayer;
	class FPWS
	{
	public:
		enum class ErrorType
		{
			Success = 0,
			Fail = 1,
			TimeOut = 2,
			Not_Connected = 3,
			Duplicate_Name = 4,
			Not_Requested = 5,
			Json_ParseErr = 6,
			Resp_InvalidID = 7,
			Send_Failed = 8,
			Unknown = 9
		};
		struct Error
		{
			ErrorType tp;
			std::string reason = "";
		};
		enum class PacketType
		{
			Unknown = -1,
			List = 0,
			Add = 1,
			Remove = 2,
			Connect = 3,
			Disconnect = 4,
			GetState = 5,
			Remove_All = 6,
			Connect_All = 7,
			Disconnect_All = 8,
			GetVersion = 9
		};
		enum class FPState
		{
			CONNECTING,
			CONNECTED,
			DISCONNECTING,
			DISCONNECTED,
			RECONNECTING,
			STOPPING,
			STOPPED
		};
		struct WSPacket
		{
			std::string id;
			PacketType pt = PacketType::Unknown;
			FakePlayer* target = nullptr;
			std::string name;
		};
		struct Response
		{
			bool set = false;
			std::string id, name, reason, version;
			FPState stat;
			bool success;
			PacketType pt;
			std::vector<std::string> list;
		};
		WebSocket::pointer ws = nullptr;
		ThreadPool* pool = nullptr;
		bool connected = false;
		std::unordered_map<std::string, std::pair<WSPacket, Response>> pkts;
		std::vector<FakePlayer*> fp_list;
		std::vector<FakePlayer*> wait_list;
		int reconnect_num = 0;
		int thread_total = 0;
		FPWS()
		{
			pool = new ThreadPool(std::thread::hardware_concurrency());
			srand(time(0));
		}
		void connect_ws();
		Error add(FakePlayer* fp);
		Error remove(FakePlayer* fp);
		Error remove_all();
		std::string getVersion();
		Error list(std::vector<std::string>* list);
		Error refresh();
	private:
		PacketType parsePacketType(std::string tp);
		void process();
		bool send(WSPacket pkt);
		bool checkID(std::string id);
		std::string getID();
		Error wait_response(std::string id);
	};

	class FakePlayer
	{
	public:
		Player* fp_ptr = nullptr;
		std::string name = 0;
		short dim = 0;
		Vec3 pos;
		std::string summoner_name = 0;
		xuid_t summoner_xuid = 0;
		bool online = false;
		FakePlayer(const std::string& fp_name, const std::string& fp_summoner_name, xuid_t fp_summoner_xuid)
			: name(fp_name), summoner_name(fp_summoner_name), summoner_xuid(fp_summoner_xuid) {}
		void teleport(const Vec3& pos, int dim);
	};
}

#endif // !FAKEPLAYER_H
