#include "fakeplayer.h"
#include "langpack.h"
#include "mass.h"

using namespace std;

namespace FPHelper
{
	void FakePlayer::teleport(const Vec3& pos,int dim)
	{
		FPHelper::teleport(fp_ptr, pos, dim);
		auto pos_now = fp_ptr->getPos();
		this->pos = pos_now;
		if (equal(pos_now, pos))
		{
			PRINT<DEBUG>("Successful");
		}
	}
	void FPWS::connect_ws()
	{
		string url = "ws://localhost:" + to_string(cfg->ws_port);
		ws = WebSocket::from_url(url);
		if (this->ws)
		{
			this->status = Status::READY;
			PRINT(LANG("fpws.connected"));
			reconnect_num = 0;
			process();
		}
		else
		{
			PRINT<ERROR, RED>(LANG("fpws.failed.connect"), url);
		}
	}
	FPWS::Error FPWS::add(FakePlayer* fp)
	{
		for (auto& it : fp_list)
			if (it->name == fp->name)
				return FPWS::Error::Duplicate_Name;
		send(PackType::Add, fp);
		if (wait_response() == Error::Success)
		{
			Document doc;
			doc.Parse(msg.c_str());
			if (doc.HasParseError()) return Error::Json_ParseErr;
			if (doc["type"].GetString() != "add") return Error::Resp_TypeErr;
			if (!doc["data"]["success"].GetBool()) return Error::Fail;
		}
	}
	FPWS::Error FPWS::wait_response()
	{
		if (status == Status::WAITING) 
		{
			auto res = pool->enqueue([&]() {
				while (1) // 1000ms
				{
					if (status == Status::PENDING) return 1;
					this_thread::sleep_for(chrono::milliseconds(10)); // 10ms
				}
			}).wait_for(chrono::milliseconds(1000));
			if (res == future_status::ready) return Error::Success;
			PRINT<WARN, YELLOW>(LANG("fpws.wait.timeout"));
			return Error::TimeOut;
		}
		return Error::Not_Requested;
	}
	bool FPWS::send(FPWS::PackType tp, FakePlayer* fp)
	{
		if (status != Status::READY || !ws || ws->getReadyState() == WebSocket::CLOSED)	return 0;
		switch (tp)
		{
		case FPWS::PackType::List:
			ws->send("{\"type\":\"list\"}");
			status = Status::WAITING;
			return 1;
			break;
		case FPWS::PackType::Add:
			if (fp)
			{
				string skin = cfg->skin;
				if (skin == "random")
				{
					srand(time(NULL));
					bool i = rand() % 2;
					skin = (i ? "steve" : "alex");
				}
				if (skin != "steve" && skin != "alex")
				{
					PRINT<WARN, RED>("fpws.invalid.skin");
					skin = "steve";
				}
				ws->send(format(R"({"type":"add","data":{"name":"%s","skin":""}})", fp->name));
				wait_list.push_back(fp);
				status = Status::WAITING;
				return 1;
			}
			break;
		case FPWS::PackType::Remove:
			if (fp)
			{
				ws->send(format(R"({"type":"remove","data":{"name":"%s"}})", fp->name));
				status = Status::WAITING;
				return 1;
			}
			break;
		case FPWS::PackType::Connect:
			if (fp)
			{
				ws->send(format(R"({"type":"connect","data":{"name":"%s"}})", fp->name));
				status = Status::WAITING;
				return 1;
			}
			break;
		case FPWS::PackType::Disconnect:
			if (fp)
			{
				ws->send(format(R"({"type":"disconnect","data":{"name":"%s"}})", fp->name));
				status = Status::WAITING;
				return 1;
			}
			break;
		case FPWS::PackType::GetState:
			if (fp)
			{
				ws->send(format(R"({"type":"getState","data":{"name":"%s"}})", fp->name));
				status = Status::WAITING;
				return 1;
			}
		}
		return 0;
	}
	void FPWS::process()
	{
		pool->enqueue([&]() {
			while (ws->getReadyState() != WebSocket::CLOSED)
			{
				ws->poll();
				ws->dispatch([&](const string& _msg) {
					msg = _msg;
					status = Status::PENDING;
				});
			}
			PRINT<WARN, YELLOW>(LANG("fpws.disconnected"));
			if (reconnect_num <= 3)
			{
				PRINT(LANG("fpws.try_reconnect"));
				connect_ws();
			}
			else
			{
				PRINT<ERROR, RED>("fpws.failed.try_reconnect");
			}
		});
	}
}