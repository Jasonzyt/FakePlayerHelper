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
		enum class Status
		{
			NO_CONNECTION = 0,
			READY = 1,
			WAITING = 2,
			PENDING = 3
		};
		enum class Error
		{
			Success = 0,
			Fail = 1,
			TimeOut = 2,
			Not_Connected = 3,
			Duplicate_Name = 4,
			Not_Requested = 5,
			Json_ParseErr = 6,
			Resp_TypeErr = 7,
			Unknown = 8
		};
		enum class PackType
		{
			List = 0,
			Add = 1,
			Remove = 2,
			Connect = 3,
			Disconnect = 4,
			GetState = 5
		};
		WebSocket::pointer ws = nullptr;
		ThreadPool* pool = nullptr;
		Status status = Status::NO_CONNECTION;
		std::string msg;
		std::vector<FakePlayer*> fp_list;
		std::vector<FakePlayer*> wait_list;
		int reconnect_num = 0;
		FPWS()
		{
			pool = new ThreadPool(2);
		}
		void connect_ws();
		Error add(FakePlayer* fp);
		Error remove(FakePlayer* fp);
		Error list();
		void update();
	private:
		void process();
		bool send(PackType tp, FakePlayer* fp);
		Error wait_response();
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
