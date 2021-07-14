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
	class FakePlayer
	{
	public:
		enum class Status
		{
			NO_CONNECTION = 0,
			READY = 1,
			WAITING = 2,
			PENDING = 3
		};
		Player* fp_ptr = nullptr;
		WebSocket::pointer ws = nullptr;
		ThreadPool* pool = nullptr;
		Status status = Status::NO_CONNECTION;
		std::string msg = 0;

		std::string name = 0;
		short dim = 0;
		int x = 0, y = 0, z = 0;
		std::string summoner_name = 0;
		xuid_t summoner_xuid = 0;
		FakePlayer(const std::string& fp_name, const std::string& fp_summoner_name, xuid_t fp_summoner_xuid)
			: name(fp_name), summoner_name(fp_summoner_name), summoner_xuid(fp_summoner_xuid) {}
		void init();
		bool add();
		bool teleport(Vec3 pos);
		bool teleport(Player* pl);
	private:
		void process();
	};
}

#endif // !FAKEPLAYER_H
