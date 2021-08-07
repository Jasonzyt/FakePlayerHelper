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
		this->dim = dim;
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
			connected = true;
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
				return Error{ ErrorType::Duplicate_Name,LANG("fpws.duplicate.name") };
		string id = getID();
		WSPacket pkt{ id,PacketType::Add,fp,fp->name };
		if (send(pkt))
		{
			auto res = wait_response(id);
			if (res.tp == ErrorType::Success)
			{
				auto data = pkts.at(id);
				if (!data.second.success)
					return Error{ ErrorType::Fail,data.second.reason };
			}
			pkts.erase(pkts.find(id));
			return res;
		}
		return Error{ ErrorType::Send_Failed,LANG("fpws.failed.send") };
	}
	FPWS::Error FPWS::remove(FakePlayer* fp)
	{
		string id = getID();
		WSPacket pkt{ id,PacketType::Remove,fp };
		if (send(pkt))
		{
			auto res = wait_response(id);
			if (res.tp == ErrorType::Success)
			{
				auto data = pkts.at(id);
				if (!data.second.success)
					return Error{ ErrorType::Fail,data.second.reason };
			}
			pkts.erase(pkts.find(id));
			return res;
		}
		return Error{ ErrorType::Send_Failed,LANG("fpws.failed.send") };
	}
	FPWS::Error FPWS::remove_all(){}
	FPWS::Error FPWS::list(vector<string>* list)
	{
		string id = getID();
		WSPacket pkt{ id,PacketType::List,nullptr };
		if (send(pkt))
		{
			auto res = wait_response(id);
			if (res.tp == ErrorType::Success)
			{
				auto data = pkts.at(id);
				list = &data.second.list;
			}
			pkts.erase(pkts.find(id));
			return res;
		}
		return Error{ ErrorType::Send_Failed,LANG("fpws.failed.send") };
	}
	FPWS::Error FPWS::refresh()
	{
		vector<string> fp_list;
		if (list(&fp_list).tp == ErrorType::Success)
		{
			for (auto& it : fp_list)
			{
				string id = getID();
				WSPacket pkt{ id,PacketType::GetState,nullptr,it };
				if (send(pkt))
				{
					auto res = wait_response(id);
					if (res.tp == ErrorType::Success)
					{
						auto data = pkts.at(id);
						if (!data.second.success)
							return Error{ ErrorType::Fail,data.second.reason };
						if (data.second.stat == FPState::CONNECTED)
						{
							int flag = 0;
							FakePlayer* fp = nullptr;
							for (auto& it2 : this->fp_list)
							{
								if (it2->name == it)
								{
									fp = it2;
									flag = 1;
								}
							}
							if (!flag) fp = new FakePlayer(it, "[Unknown]", 0);
							Player* fp_ptr = nullptr;
							forEachPlayer([&](Player& pl) -> bool {
								Player* pl_ptr = &pl;
								if (pl_ptr->getNameTag() == it)
								{
									fp->fp_ptr = pl_ptr;
									fp->online = true;
									fp->dim = pl_ptr->getDimensionId();
									fp->pos = pl_ptr->getPos();
									if (!flag) fpws->fp_list.push_back(fp);
								}
							});
						}
						else if (data.second.stat != FPState::STOPPED && data.second.stat != FPState::STOPPING)
							fpws->wait_list.push_back(new FakePlayer(it, "[Unknown]", 0));
					}
					pkts.erase(pkts.find(id));
					return res;
				}
			}
		}
	}
	string FPWS::getVersion()
	{
		string id = getID();
		WSPacket pkt{ id,PacketType::GetVersion };
		if (send(pkt))
		{
			if (wait_response(id).tp == ErrorType::Success)
			{
				auto data = pkts.at(id);
				string version = data.second.version;
				pkts.erase(pkts.find(id));
				return version;
			}
			pkts.erase(pkts.find(id));
		}
		return "";
	}
	FPWS::Error FPWS::wait_response(string id)
	{
		if (pkts.find(id) != pkts.end());
		{
			auto res = pool->enqueue([&]() {
				while (20)
				{
					if (pkts.at(id).second.set) return;
					this_thread::sleep_for(chrono::milliseconds(5));
				}
				return;
			}).wait_for(chrono::milliseconds(110));
			if (res == future_status::ready) return Error{ ErrorType::Success };
			else
			{
				PRINT<WARN, YELLOW>(LANG("fpws.wait.timeout"));
			}
			return Error{ ErrorType::TimeOut };
		}
		return Error{ ErrorType::Not_Requested };
	}
	bool FPWS::send(WSPacket pkt)
	{
		if (!connected) return false;
		if (!ws || ws->getReadyState() == WebSocket::CLOSED) return false;
		auto fp = pkt.target;
		pkts.insert(make_pair(pkt.id, make_pair(pkt, Response())));
		switch (pkt.pt)
		{
		case FPWS::PacketType::List:
			ws->send(format("{\"id\":\"%s\",\"type\":\"list\"}", pkt.id));
			return true;
			break;
		case FPWS::PacketType::Add:
			if (fp)
			{
				string skin = cfg->skin;
				if (skin == "random")
				{
					bool i = rand() % 2;
					skin = (i ? "steve" : "alex");
				}
				if (skin != "steve" && skin != "alex")
				{
					PRINT<WARN, RED>("fpws.invalid.skin");
					skin = "steve";
				}
				ws->send(format(R"({"id":"%s","type":"add","data":{"name":"%s","skin":"%s"}})", pkt.id.c_str(), fp->name.c_str(), skin.c_str()));
				wait_list.push_back(fp);
				return true;
			}
			break;
		case FPWS::PacketType::Remove:
			if (fp)
			{
				ws->send(format(R"({"id":"%s","type":"remove","data":{"name":"%s"}})", pkt.id.c_str(), fp->name.c_str()));
				return true;
			}
			break;
		case FPWS::PacketType::Connect:
			if (fp)
			{
				ws->send(format(R"({"id":"%s","type":"connect","data":{"name":"%s"}})", pkt.id.c_str(), fp->name.c_str()));
				return true;
			}
			break;
		case FPWS::PacketType::Disconnect:
			if (fp)
			{
				ws->send(format(R"({"id":"%s","type":"disconnect","data":{"name":"%s"}})", pkt.id.c_str(), fp->name.c_str()));
				return true;
			}
			break;
		case FPWS::PacketType::GetState:
			if (fp)
			{
				ws->send(format(R"({"id":"%s","type":"getState","data":{"name":"%s"}})", pkt.id.c_str(), fp->name.c_str()));
				return true;
			}
			break;
		case FPWS::PacketType::Remove_All:
			ws->send(format("{\"id\":\"%s\",\"type\":\"remove_all\"}", pkt.id.c_str()));
			return true;
		case FPWS::PacketType::Connect_All:
			ws->send(format("{\"id\":\"%s\",\"type\":\"connect_all\"}", pkt.id.c_str()));
			return true;
		case FPWS::PacketType::Disconnect_All:
			ws->send(format("{\"id\":\"%s\",\"type\":\"disconnect_all\"}", pkt.id.c_str()));
			return true;
		case FPWS::PacketType::GetVersion:
			ws->send(format("{\"id\":\"%s\",\"type\":\"getVersion\"}", pkt.id.c_str()));
			return true;
		}
		return false;
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
			string str = "0123456789AB@CDEFGHIJKLMNO%PQRST%UVWX@YZabcde%fgnijklmnopqrs#tuvwxyz";
			// m<=r<=n
			// rand()%(n-m+1)+m
			int random_num = rand() % 69;
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
					resp.set = true;
					resp.pt = pt;
					resp.id = doc["id"].GetString();
					auto it = pkts.find(resp.id);
					if (it != pkts.end()) it->second.second = resp;
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