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
		string url = "ws://127.0.0.1:" + to_string(cfg->ws_port);
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
		string id = getID();
		WSPacket pkt{ id,PacketType::Add,fp };
		if (send(pkt))
			return wait_response(id);
	}
	FPWS::Error FPWS::remove(FakePlayer* fp)
	{
		string id = getID();
		WSPacket pkt{ id,PacketType::Remove,fp };
		if (send(pkt))
		{
			auto res = wait_response(id);
			delete 
			return res;
		}
		return Error::Send_Failed;
	}
	FPWS::Error FPWS::list()
	{
		string id = getID();
		WSPacket pkt{ id,PacketType::List };
		if (send(pkt))
		{
			if (wait_response(id) == Error::Success)
			{
				pkts.erase(pkts.find(id));
			}
		}
	}
	FPWS::Error FPWS::wait_response(string id)
	{
		if (status == Status::WAITING)
		{
			auto res = pool->enqueue([&]() {
				while (1) // 20ms
				{
					if (status == Status::PENDING && pkts.at(id).second) return;
					this_thread::sleep_for(chrono::milliseconds(5));
				}
			}).wait_for(chrono::milliseconds(20)); // 本机实际上5ms就够了
			if (res == future_status::ready) return Error::Success;
			else
			{
				PRINT<WARN, YELLOW>(LANG("fpws.wait.timeout"));
			}
			return Error::TimeOut;
		}
		return Error::Not_Requested;
	}
	bool FPWS::send(WSPacket pkt)
	{
		if (status != Status::READY) return false;
		if (!ws || ws->getReadyState() == WebSocket::CLOSED) return false;
		auto fp = pkt.target;
		pkts.emplace(pair<std::string, pair<WSPacket*, Response*>>(pkt.id, { &pkt,nullptr }));
		switch (pkt.pt)
		{
		case FPWS::PacketType::List:
			ws->send(format("{\"id\":\"%s\",\"type\":\"list\"}", pkt.id));
			status = Status::WAITING;
			return true;
			break;
		case FPWS::PacketType::Add:
			if (fp)
			{
				string skin = cfg->skin;
				if (skin == "random")
				{
					//srand(time(NULL));
					bool i = rand() % 2;
					skin = (i ? "steve" : "alex");
				}
				if (skin != "steve" && skin != "alex")
				{
					PRINT<WARN, RED>("fpws.invalid.skin");
					skin = "steve";
				}
				ws->send(format(R"({"id":"%s","type":"add","data":{"name":"%s","skin":"%s"}})", pkt.id, fp->name, skin));
				wait_list.push_back(fp);
				status = Status::WAITING;
				return true;
			}
			break;
		case FPWS::PacketType::Remove:
			if (fp)
			{
				ws->send(format(R"({"id":"%s","type":"remove","data":{"name":"%s"}})", pkt.id, fp->name));
				status = Status::WAITING;
				return true;
			}
			break;
		case FPWS::PacketType::Connect:
			if (fp)
			{
				ws->send(format(R"({"id":"%s","type":"connect","data":{"name":"%s"}})", pkt.id, fp->name));
				status = Status::WAITING;
				return true;
			}
			break;
		case FPWS::PacketType::Disconnect:
			if (fp)
			{
				ws->send(format(R"({"id":"%s","type":"disconnect","data":{"name":"%s"}})", pkt.id, fp->name));
				status = Status::WAITING;
				return true;
			}
			break;
		case FPWS::PacketType::GetState:
			if (fp)
			{
				ws->send(format(R"({"id":"%s","type":"getState","data":{"name":"%s"}})", pkt.id, fp->name));
				status = Status::WAITING;
				return true;
			}
			break;
		case FPWS::PacketType::Remove_All:
			ws->send(format("{\"id\":\"%s\",\"type\":\"remove_all\"}", pkt.id));
			status = Status::WAITING;
			return true;
		case FPWS::PacketType::Connect_All:
			ws->send(format("{\"id\":\"%s\",\"type\":\"connect_all\"}", pkt.id));
			status = Status::WAITING;
			return true;
		case FPWS::PacketType::Disconnect_All:
			ws->send(format("{\"id\":\"%s\",\"type\":\"disconnect_all\"}", pkt.id));
			status = Status::WAITING;
			return true;
		case FPWS::PacketType::GetVersion:
			ws->send(format("{\"id\":\"%s\",\"type\":\"getVersion\"}", pkt.id));
			status = Status::WAITING;
			return true;
		}
		return 0;
	}
	FPWS::PacketType FPWS::parsePacketType(string tp)
	{
		if (tp == "add") return PacketType::Add;
		if (tp == "remove") return PacketType::Remove;
		if (tp == "connect") return PacketType::Connect;
		if (tp == "disconnect") return PacketType::Disconnect;
		if (tp == "list") return PacketType::List;
		if (tp == "getState") return PacketType::GetState;
		if (tp == "getVersion") return PacketType::GetVersion;
		if (tp == "remove_all") return PacketType::Remove_All;
		if (tp == "connect_all") return PacketType::Connect_All;
		if (tp == "disconnect_all") return PacketType::Disconnect_All;
		return PacketType::Unknown;
	}
	string FPWS::getID()
	{
		string res;
		for (int i = 0; i < 20; i++)
		{
			string str = "01234567=89AB@CDEF=GHI&JKLMNO%PQRST%UVWX@YZabcde%fgnij&klmnopqrs#tuvwxyz";
			// m<=r<=n
			// rand()%(n-m+1)+m
			int random_num = rand() % 73;
			res += str[random_num];
		}
		return res;
	}
	void FPWS::process()
	{
		pool->enqueue([&]() {
			while (ws->getReadyState() != WebSocket::CLOSED)
			{
				ws->poll(20); // 为0会出现CPU占用30%居高不下的问题
				ws->dispatch([&](const string& msg) {
					PRINT<DEBUG, BLUE>("WebSocket Response: \n", msg);
					Response resp;
					Document doc;
					doc.Parse(msg.c_str());
					if (doc.HasParseError())
					{
						PRINT<ERROR, RED>(format(LANG("fpws.parse.json.err.format", (int)doc.GetParseError())));
					}
					auto pt = parsePacketType(doc["type"].GetString());
					switch (pt)
					{
					case PacketType::Add:
						resp.success = doc["data"]["success"].GetBool();
						resp.name = doc["data"]["name"].GetString();
						resp.reason = doc["data"]["reason"].GetString();
						break;
					case PacketType::Remove:
						resp.success = doc["data"]["success"].GetBool();
						resp.name = doc["data"]["name"].GetString();
						resp.reason = doc["data"]["reason"].GetString();
						break;
					case PacketType::List:
						resp.success = true;
						for (int i = 0; i < doc["data"]["list"].Size(); i++)
							resp.list.push_back(doc["data"]["list"][i].GetString());
						break;
					case PacketType::GetVersion:
						resp.success = true;
						resp.version = doc["data"]["version"].GetString();
						break;
					case PacketType::Remove_All:
						resp.success = true;
						for (int i = 0; i < doc["data"]["list"].Size(); i++)
							resp.list.push_back(doc["data"]["list"][i].GetString());
						break;
					default:
						return;
					}
					resp.pt = pt;
					resp.id = doc["id"].GetString();
					auto it = pkts.find(resp.id); 
					if (it != pkts.end()) it->second.second = &resp;
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