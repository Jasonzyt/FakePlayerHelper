#include "fakeplayer.h"

using namespace std;

namespace FPHelper
{
	void FPWS::init()
	{
		ws = WebSocket::from_url("ws://localhost:" + std::to_string(cfg->ws_port));
		assert(ws);
	}
	bool FPWS::add(FakePlayer* fp)
	{

	}

	void FPWS::process()
	{
		this->pool->enqueue([&]() {
			while (ws->getReadyState() != WebSocket::CLOSED)
			{
				ws->poll();
				ws->dispatch([&](const string& _msg) {
					msg = _msg;
					status = Status::PENDING;
				});
			}
			PRINT<WARN, YELLOW>("WebSocket disconncted!");
		});
	}
}