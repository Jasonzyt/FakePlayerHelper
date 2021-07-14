#ifndef MASS_H
#define MASS_H
#include "pch.h"
#include "bstream.h"
#include "mypacket.h"
#include <mc/Player.h>
#include <loader/loader.h>

template <typename T>
struct VarInts {
	T v;
	operator T() {
		return v;
	}
	VarInts() {}
	VarInts(T x) { v = x; }
	template<typename _TP>
	inline void pack(WBStreamImpl<_TP>& ws) const {
		unsigned char buf[16];
		int ptr = 0;
		T enc = v;
		do {
			buf[ptr++] = enc & 0x7f;
			enc >>= 7;
		} while (enc);
		for (int i = 0; i < ptr - 1; ++i) {
			buf[i] |= 0x80;
		}
		ws.write(buf, ptr);
	}
	inline void unpack(RBStream& rs) {
		unsigned char buf[16];
		v = 0;
		int ptr = 0;
		for (; ptr < 16; ++ptr) {
			rs.apply(buf[ptr]);
			if (!(buf[ptr] & 0x80)) {
				++ptr;
				break;
			}
			buf[ptr] &= 0x7f;
		}
		for (ptr = ptr - 1; ptr >= 0; --ptr) {
			v <<= 7;
			v |= buf[ptr];
		}
	}
};
using VarUInt = VarInts<unsigned int>;
using VarULong = VarInts<unsigned long long>;

struct MCString {
	string_view view;
	MCString() {}
	MCString(string_view sv) : view(sv) {}
	template <typename T>
	inline void pack(T& ws) const {
		ws.apply(VarUInt((unsigned int)view.size()));
		ws.write(view.data(), view.size());
	}
	inline void unpack(RBStream& rs) {
		VarUInt sz;
		rs.apply(sz);
		view = string_view((const char*)rs.data, sz.v);
		rs.data += sz.v;
	}
};

namespace FPHelper
{
	enum class TextType : char {
		RAW = 0,
		POPUP = 3,
		JUKEBOX_POPUP = 4,
		TIP = 5
	};
	inline bool runcmd(const std::string& cmd)
	{
		extern VA p_spscqueue;
		if (p_spscqueue != 0)
			return SymCall("??$inner_enqueue@$0A@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@AEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
				bool, VA, std::string)(p_spscqueue, cmd);
		return false;
	}
	inline bool sendText(Player* pl, const std::string& text, TextType tp)
	{
		WBStream txtpkws;
		txtpkws.data.reserve(8 + text.size());
		txtpkws.apply((char)tp, (char)0, MCString(text));
		MyPkt<9> pk{ txtpkws.data };
		pl->sendNetworkPacket(pk);
	}
	inline std::string getVersion()
	{
		std::string a;
		SymCall("?getGameVersionString@Common@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			std::string&, std::string&)(a);		// IDA Common::getGameVersionString
		return a;
	}

	inline std::string getDimensionName(int dim)
	{
		switch (dim)
		{
		case 0:
			return "Overworld"; // 主世界
		case 1:
			return "Nether"; // 下界
		case 2:
			return "The End"; // 末路之地
		default:
			return "Unknown"; // 未知
		}
	}
}

#endif // !MASS_H
