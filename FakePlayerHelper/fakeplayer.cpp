#include "fakeplayer.h"
#include "langpack.h"
#include "mass.h"
#define H do_hash

using namespace std;

namespace FPHelper
{
	void FakePlayer::teleport(const Vec3& pos, int dim)
	{
		FPHelper::teleport(fp_ptr, pos, dim);
		auto pos_now = fp_ptr->getPos();
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
			reconnect_num = 0;
			PRINT(LANG("fpws.connected"));
			sendMessageAll(LANG("gamemsg.ws.connected"));
			process();
		}
		else
		{
			reconnect_num++;
			PRINT<ERROR, RED>(LANG("fpws.failed.connect"), url);
		}
	}
	void FPWS::tick()
	{
		lock_guard<mutex> a(mtx);
		if (sync_timer >= 100)
			getStates();
		else sync_timer++;
		for (auto& it : timer) 
		{
			if (it.second == -1)
			{
				auto resp = resps[it.first];
				if (resp.set)
				{
					switch (resp.pt)
					{
					case FPHelper::FPWS::PacketType::List:
						break;
					case FPHelper::FPWS::PacketType::Add:
						if (resp.success)
						{

						}
						else 
						{

						}
						break;
					case FPHelper::FPWS::PacketType::Remove:
						if (resp.success)
						{

						}
						else
						{

						}
						break;
					case FPHelper::FPWS::PacketType::Connect:
						if (resp.success)
						{

						}
						else
						{

						}
						break;
					case FPHelper::FPWS::PacketType::Disconnect:
						if (resp.success)
						{

						}
						else
						{

						}
						break;
					case FPHelper::FPWS::PacketType::GetState:
						break;
					case FPHelper::FPWS::PacketType::Remove_All:
						break;
					case FPHelper::FPWS::PacketType::Connect_All:
						break;
					case FPHelper::FPWS::PacketType::Disconnect_All:
						break;
					case FPHelper::FPWS::PacketType::GetVersion:
						PRINT(localization("fpws.version.msg.format", resp.version));
						sendMessageAll(localization("gamemsg.ws.fp.version", resp.version));
						break;
					case FPHelper::FPWS::PacketType::GetState_All:
						for (auto& it : resp.players) 
						{
							if (it.state == FPState::CONNECTED)
							{
								if (IsFakePlayer(it.name)) {}
							}
						}
						break;
					case FPHelper::FPWS::PacketType::SetChatControl:
						break;
					default:
						PRINT<ERROR, RED>(LANG("fpws.unknown.packet.type"));
						break;
					}
				}
			}
			else if (it.second >= 50)
			{
				PRINT<ERROR, RED>("fpws.wait.timeout");
			}
			else it.second++;
		}
	}

	void FPWS::process()
	{
		pool->enqueue([&]() {
			while (ws->getReadyState() != WebSocket::CLOSED)
			{
				ws->poll(20); // 为0会出现CPU占用30%居高不下的问题
				ws->dispatch([&](const string& msg) {
					PRINT<DEBUG, BLUE>("WebSocket Message: \n", msg);
					lock_guard<mutex> a(mtx);
					auto resp = parseMessage(msg);
					resps.emplace(make_pair(resp.id, resp));
					timer[resp.id] = -1;
					});
			}
			connected = false;
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

#pragma region Event
	void FPWS::onAdd(FPWS::Message msg) {}

	void FPWS::onRemove(FPWS::Message msg) {}

	void FPWS::onConnect(FPWS::Message msg) {}

	void FPWS::onDisconnect(FPWS::Message msg) {}
#pragma endregion

#pragma region SendWebsocketMessage
	bool FPWS::add(FakePlayer* fp)
	{
		string id = getID();
		WSPacket pkt{ id,PacketType::Add,fp,fp->name,fp->allowChatControl };
		return send(pkt);
	}

	bool FPWS::remove(FakePlayer* fp)
	{
		string id = getID();
		WSPacket pkt{ id,PacketType::Remove,fp };
		return send(pkt);
	}

	bool FPWS::remove_all(){}

	bool FPWS::list()
	{
		string id = getID();
		WSPacket pkt{ id,PacketType::List,nullptr };
		return send(pkt);
	}

	bool FPWS::getVersion()
	{
		string id = getID();
		WSPacket pkt{ id,PacketType::GetVersion };
		return send(pkt);
	}

	bool FPWS::getStates()
	{
		string id = getID();
		WSPacket pkt{ id,PacketType::GetState_All };
		return send(pkt);
	}

	bool FPWS::send(WSPacket pkt)
	{
		if (!connected) return false;
		if (!ws || ws->getReadyState() == WebSocket::CLOSED) return false;
		auto fp = pkt.target;
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
				ws->send(format(R"({"id":"%s","type":"add","data":{"name":"%s","skin":"%s","allowChatControl":%s}})",
					pkt.id.c_str(), fp->name.c_str(), skin.c_str(), pkt.allowChatControl ? "true" : "false"));
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
		case FPWS::PacketType::GetState_All:
			ws->send(format("{\"id\":\"%s\",\"type\":\"getState_all\"}", pkt.id.c_str()));
			return true;
		case FPWS::PacketType::SetChatControl:
			if (fp)
			{
				ws->send(format(R"({"id":"%s","type":"setChatControl","data":{"allowChatControl":%s}})",
					pkt.id.c_str(), pkt.allowChatControl ? "true" : "false"));
			}
			return true;
		}
		return false;
	}
#pragma endregion
#pragma region Other
	bool FPWS::IsFakePlayer(Player* pl)
	{
		if (fpws)
		{
			for (auto& it : fpws->fp_list)
				if (pl == it->fp_ptr) return true;
		}
		return false;
	}
	bool FPWS::IsFakePlayer(const string& pl)
	{
		if (fpws)
		{
			for (auto& it : fpws->fp_list)
				if (do_hash(pl) == do_hash(it->fp_ptr->getNameTag()))
					return true;
		}
		return false;
	}
#pragma endregion
#pragma region Parse
	FPWS::PacketType FPWS::parsePacketType(const string& tp)
	{
		switch (H(tp))
		{
		case H("add"):            return PacketType::Add;
		case H("remove"):         return PacketType::Remove;
		case H("connect"):        return PacketType::Connect;
		case H("disconnect"):     return PacketType::Disconnect;
		case H("list"):           return PacketType::List;
		case H("getState"):       return PacketType::GetState;
		case H("getState_all"):   return PacketType::GetState_All;
		case H("getVersion"):     return PacketType::GetVersion;
		case H("remove_all"):     return PacketType::Remove_All;
		case H("connect_all"):    return PacketType::Connect_All;
		case H("disconnect_all"): return PacketType::Disconnect_All;
		case H("setChatControl"): return PacketType::SetChatControl;
		}
		return PacketType::Unknown;
	}
	FPWS::EventType FPWS::parseEventType(const string& ev)
	{
		switch (H(ev))
		{
		case H("add"):        return EventType::Add;
		case H("remove"):     return EventType::Remove;
		case H("connect"):    return EventType::Connect;
		case H("disconnect"): return EventType::Disconnect;
		}
		return EventType::Unknown;
	}
	const FPWS::Message FPWS::parseMessage(const string& msgstr)
	{
		Message msg;
		Document doc;
		doc.Parse(msgstr.c_str());
		if (doc.HasParseError())
		{
			PRINT<ERROR, RED>(format(LANG("fpws.parse.json.err.format", (int)doc.GetParseError())));
		}
		if (doc.HasMember("type"))
		{
			auto pt = parsePacketType(doc["type"].GetString());
			switch (pt)
			{
			case PacketType::Add:
				msg.success = doc["data"]["success"].GetBool();
				msg.name = doc["data"]["name"].GetString();
				msg.reason = doc["data"]["reason"].GetString();
				break;
			case PacketType::Remove:
				msg.success = doc["data"]["success"].GetBool();
				msg.name = doc["data"]["name"].GetString();
				msg.reason = doc["data"]["reason"].GetString();
				break;
			case PacketType::List:
				msg.success = true;
				for (int i = 0; i < doc["data"]["list"].Size(); i++)
					msg.list.push_back(doc["data"]["list"][i].GetString());
				break;
			case PacketType::GetVersion:
				msg.success = true;
				msg.version = doc["data"]["version"].GetString();
				break;
			case PacketType::Remove_All:
				msg.success = true;
				for (int i = 0; i < doc["data"]["list"].Size(); i++)
					msg.list.push_back(doc["data"]["list"][i].GetString());
				break;
			case PacketType::GetState_All:
				msg.success = true;
				for (auto it = doc["data"]["players"].MemberBegin(); it != doc["data"]["players"].MemberEnd(); it++) 
				{
					msg.players.push_back(
						PlayerData{ 
							it->name.GetString(),
							(FPState)it->value["state"].GetInt(),
							it->value["allowChatControl"].GetBool()
						}
					);
				}
				break;
			case PacketType::SetChatControl:
				msg.success = doc["data"]["success"].GetBool();
				msg.name = doc["data"]["name"].GetString();
				msg.reason = doc["data"]["reason"].GetString();
				break;
			default:
				return Message();
			}
			msg.set = true;
			msg.pt = pt;
			msg.id = doc["id"].GetString();
		}
		else if (doc.HasMember("event"))
		{
			auto ev = parseEventType(doc["event"].GetString());
			msg.name = doc["data"]["name"].GetString();
			if (ev != EventType::Remove) msg.stat = (FPState)doc["data"]["state"].GetInt();
			switch (ev)
			{
			case FPHelper::FPWS::EventType::Add:
				onAdd(msg);
				break;
			case FPHelper::FPWS::EventType::Remove:
				onRemove(msg);
				break;
			case FPHelper::FPWS::EventType::Connect:
				onConnect(msg);
				break;
			case FPHelper::FPWS::EventType::Disconnect:
				onDisconnect(msg);
				break;
			}
		}
		return Message();
	}
#pragma endregion
}