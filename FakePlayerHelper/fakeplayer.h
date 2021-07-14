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
			TimeOut = 1,
			Not_Connected = 2,
			Unknown = 3,
		};
		WebSocket::pointer ws = nullptr;
		ThreadPool* pool = nullptr;
		Status status = Status::NO_CONNECTION;
		std::string msg = 0;
		vector<FakePlayer*> fp_list{};
		void init();
		void connect();
		bool add(FakePlayer* fp);
		bool remove(FakePlayer* fp);
		bool list();
		bool update();
	private:
		void process();
	};

	class FakePlayer
	{
	public:
		Player* fp_ptr = nullptr;
		std::string name = 0;
		short dim = 0;
		int x = 0, y = 0, z = 0;
		std::string summoner_name = 0;
		xuid_t summoner_xuid = 0;
		FakePlayer(const std::string& fp_name, const std::string& fp_summoner_name, xuid_t fp_summoner_xuid)
			: name(fp_name), summoner_name(fp_summoner_name), summoner_xuid(fp_summoner_xuid) {}
		bool teleport(Vec3 pos);
		bool teleport(Player* pl);
	};
}

#endif // !FAKEPLAYER_H
