#include "fakeplayer.h"
#include "langpack.h"
#include "mass.h"
#define H do_hash
#define FIND(x,v) find(x.begin(),x.end(),v)
#define EXISTS(x,val) (FIND(x,val) != x.end())

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

	void WebSocket::connect_ws()
	{
		string url = "ws://127.0.0.1:" + to_string(cfg->ws_port);
		ws = easywsclient::WebSocket::from_url(url);
		if (this->ws)
		{
			connected = true;
			reconnect_num = 0;
			PRINT(LANG("fpws.connected"));
			sendMessageAll(LANG("gamemsg.ws.connected"));
			process();
			getVersion();
			fpws->sync_timer = 100;
		}
		else
		{
			reconnect_num++;
			PRINT<ERROR, RED>(LANG("fpws.failed.connect"), url);
		}
	}
	void WebSocket::tick()
	{
		lock_guard<mutex> a(mtx);
		if (sync_timer >= 100)
		{
			getStates();
			sync_timer = 0;
		}
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
					case FPHelper::WebSocket::PacketType::List:
						break;
					case FPHelper::WebSocket::PacketType::Add:
					case FPHelper::WebSocket::PacketType::Connect:
						if (resp.success)
						{
							PRINT(localization("console.successfully.add.fakeplayer"));
							sendMessageAll(localization("gamemsg.successfully.add.fakeplayer"));
						}
						else
						{
							PRINT<ERROR, RED>(localization("console.failed.add.fakeplayer.format", resp.reason.c_str()));
							sendMessageAll(localization("gamemsg.failed.add.fakeplayer.format", resp.reason.c_str()));
						}
						break;
					case FPHelper::WebSocket::PacketType::Remove:
					case FPHelper::WebSocket::PacketType::Disconnect:
						if (resp.success)
						{
							PRINT(localization("console.successfully.remove.fakeplayer"));
							sendMessageAll(localization("gamemsg.successfully.remove.fakeplayer"));
						}
						else
						{
							PRINT<ERROR, RED>(localization("console.failed.remove.fakeplayer.format", resp.reason.c_str()));
							sendMessageAll(localization("gamemsg.failed.remove.fakeplayer.format", resp.reason.c_str()));
						}
						break;
					case FPHelper::WebSocket::PacketType::GetState:
						break;
					case FPHelper::WebSocket::PacketType::Remove_All:
						break;
					case FPHelper::WebSocket::PacketType::Connect_All:
						break;
					case FPHelper::WebSocket::PacketType::Disconnect_All:
						break;
					case FPHelper::WebSocket::PacketType::GetVersion:
						PRINT(localization("fpws.version.msg.format", resp.version.c_str()));
						sendMessageAll(localization("gamemsg.ws.fp.version", resp.version.c_str()));
						break;
					case FPHelper::WebSocket::PacketType::GetState_All:
						for (auto& it : resp.players)
						{
							if (it.state == FPState::CONNECTED)
							{
								if (!IsFakePlayer(it.name))
								{
									auto pl = getPlayerByNameTag(it.name);
									if (pl)
									{
										fp_list.push_back(new FakePlayer(pl));
										PRINT(localization("console.found.new.fakeplayer.format", it.name));
										sendMessageAll(localization("gamemsg.found.new.fakeplayer.format", it.name));
									}
								}
							}
							else if (it.state == FPState::CONNECTING || it.state == FPState::RECONNECTING) {}
							else
							{
								if (IsFakePlayer(it.name) && !getPlayerByNameTag(it.name)) deleteFakePlayer(it.name);
							}
						}
						break;
					case FPHelper::WebSocket::PacketType::SetChatControl:
						break;
					default:
						PRINT<ERROR, RED>(LANG("fpws.unknown.packet.type"));
						break;
					}
				}
				it.second = -2;
			}
			else if (it.second == -2) continue;
			else if (it.second >= 50)
			{
				PRINT<ERROR, RED>("fpws.wait.timeout");
			}
			else it.second++;
		}
	}

	void WebSocket::process()
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
	string WebSocket::getID()
	{
		string res;
		for (int i = 0; i < 20; i++)
		{
			string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefgnijklmnopqrstuvwxyz";
			// m<=r<=n
			// rand()%(n-m+1)+m
			int random_num = rand() % 63;
			res += str[random_num];
		}
		return res;
	}

#pragma region Event
	void WebSocket::onAdd(Message msg) {}

	void WebSocket::onRemove(Message msg)
	{
		auto it = all_fp.begin();
		for (; it != all_fp.end(); it++)
		{
			if (*it == msg.name)
			{
				all_fp.erase(it);
				break;
			}
		}
	}

	void WebSocket::onConnect(Message msg) 
	{
		if (!IsFakePlayer(msg.name) && !IsInWaitList(msg.name))
		{
			wait_list.push_back(new FakePlayer(msg.name, "[Unknown]", 0));
			PRINT(localization("console.found.new.fakeplayer.format", msg.name));
			sendMessageAll(localization("gamemsg.found.new.fakeplayer.format", msg.name));
		}
	}

	void WebSocket::onDisconnect(Message msg) {}
#pragma endregion

#pragma region SendWebsocketMessage
	bool WebSocket::add(FakePlayer* fp)
	{
		string id = getID();
		WSPacket pkt;
		if (EXISTS(all_fp, fp->name))
		{
			WSPacket pkt{ id,PacketType::Connect,fp,fp->name };
			return send(pkt);
		}
		pkt = WSPacket{ id,PacketType::Add,fp,fp->name,fp->allowChatControl };
		return send(pkt);
	}

	bool WebSocket::remove(FakePlayer* fp)
	{
		string id = getID();
		WSPacket pkt{ id,PacketType::Disconnect,fp };
		return send(pkt);
	}

	bool WebSocket::remove_all(){}

	bool WebSocket::list()
	{
		string id = getID();
		WSPacket pkt{ id,PacketType::List,nullptr };
		return send(pkt);
	}

	bool WebSocket::getVersion()
	{
		string id = getID();
		WSPacket pkt{ id,PacketType::GetVersion };
		return send(pkt);
	}

	bool WebSocket::getStates()
	{
		string id = getID();
		WSPacket pkt{ id,PacketType::GetState_All };
		return send(pkt);
	}

	bool WebSocket::send(WSPacket pkt)
	{
		if (!connected) return false;
		if (!ws || ws->getReadyState() == WebSocket::CLOSED) return false;
		auto fp = pkt.target;
		switch (pkt.pt)
		{
		case WebSocket::PacketType::List:
			ws->send(format("{\"id\":\"%s\",\"type\":\"list\"}", pkt.id));
			return true;
			break;
		case WebSocket::PacketType::Add:
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
				addPlayerToWhiteList(fp->name);
				ws->send(format(R"({"id":"%s","type":"add","data":{"name":"%s","skin":"%s","allowChatControl":%s}})",
					pkt.id.c_str(), fp->name.c_str(), skin.c_str(), pkt.allowChatControl ? "true" : "false"));
				wait_list.push_back(fp);
				return true;
			}
			break;
		case WebSocket::PacketType::Remove:
			if (fp)
			{
				ws->send(format(R"({"id":"%s","type":"remove","data":{"name":"%s"}})", pkt.id.c_str(), fp->name.c_str()));
				return true;
			}
			break;
		case WebSocket::PacketType::Connect:
			if (fp)
			{
				addPlayerToWhiteList(fp->name);
				ws->send(format(R"({"id":"%s","type":"connect","data":{"name":"%s"}})", pkt.id.c_str(), fp->name.c_str()));
				wait_list.push_back(fp);
				return true;
			}
			break;
		case WebSocket::PacketType::Disconnect:
			if (fp)
			{
				ws->send(format(R"({"id":"%s","type":"disconnect","data":{"name":"%s"}})", pkt.id.c_str(), fp->name.c_str()));
				return true;
			}
			break;
		case WebSocket::PacketType::GetState:
			if (fp)
			{
				ws->send(format(R"({"id":"%s","type":"getState","data":{"name":"%s"}})", pkt.id.c_str(), fp->name.c_str()));
				return true;
			}
			break;
		case WebSocket::PacketType::Remove_All:
			ws->send(format("{\"id\":\"%s\",\"type\":\"remove_all\"}", pkt.id.c_str()));
			return true;
		case WebSocket::PacketType::Connect_All:
			ws->send(format("{\"id\":\"%s\",\"type\":\"connect_all\"}", pkt.id.c_str()));
			return true;
		case WebSocket::PacketType::Disconnect_All:
			ws->send(format("{\"id\":\"%s\",\"type\":\"disconnect_all\"}", pkt.id.c_str()));
			return true;
		case WebSocket::PacketType::GetVersion:
			ws->send(format("{\"id\":\"%s\",\"type\":\"getVersion\"}", pkt.id.c_str()));
			return true;
		case WebSocket::PacketType::GetState_All:
			ws->send(format("{\"id\":\"%s\",\"type\":\"getState_all\"}", pkt.id.c_str()));
			return true;
		case WebSocket::PacketType::SetChatControl:
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
	bool WebSocket::IsInWaitList(const string& pl)
	{
		for (auto& it : wait_list)
			if (it->name == pl) return true;
		return false;
	}
	bool WebSocket::IsFakePlayer(Player* pl)
	{
		for (auto& it : fp_list)
			if (pl == it->fp_ptr) return true;
		return false;
	}
	bool WebSocket::IsFakePlayer(const string& pl)
	{
		for (auto& it : fp_list)
			if (it->fp_ptr && pl == it->fp_ptr->getNameTag())
				return true;
		return false;
	}
	bool WebSocket::deleteFakePlayer(const string& name)
	{
		auto it = fp_list.begin();
		for (; it != fp_list.end(); it++)
			if (do_hash(name) == do_hash((*it)->fp_ptr->getNameTag()))
			{
				fp_list.erase(it);
				return true;
			}
		return false;
	}

#pragma endregion

#pragma region Parse
	WebSocket::PacketType WebSocket::parsePacketType(const string& tp)
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
	WebSocket::EventType WebSocket::parseEventType(const string& ev)
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
	const WebSocket::Message WebSocket::parseMessage(const string& msgstr)
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
				all_fp.clear();
				for (auto it = doc["data"]["playersData"].MemberBegin(); it != doc["data"]["playersData"].MemberEnd(); it++) 
				{
					auto name = it->name.GetString();
					msg.players.push_back(
						PlayerData{ 
							name,
							(FPState)it->value["state"].GetInt(),
							it->value["allowChatControl"].GetBool()
						}
					);
					all_fp.push_back(name);
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
			return msg;
		}
		else if (doc.HasMember("event"))
		{
			auto ev = parseEventType(doc["event"].GetString());
			msg.name = doc["data"]["name"].GetString();
			if (ev != EventType::Remove) msg.stat = (FPState)doc["data"]["state"].GetInt();
			switch (ev)
			{
			case FPHelper::WebSocket::EventType::Add:
				onAdd(msg);
				break;
			case FPHelper::WebSocket::EventType::Remove:
				onRemove(msg);
				break;
			case FPHelper::WebSocket::EventType::Connect:
				onConnect(msg);
				break;
			case FPHelper::WebSocket::EventType::Disconnect:
				onDisconnect(msg);
				break;
			}
		}
		return Message();
	}
#pragma endregion
}