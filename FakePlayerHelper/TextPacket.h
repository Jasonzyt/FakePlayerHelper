#pragma once
#include <api/types/types.h>
#include <vector>
#include "myPacket.h"

#pragma pack(1)
struct TextPacket : Packet
{
	// Packet 40
	TextType type;                         // 41
	char align_0[7];                       // 48
	std::string* sourceName;               // 56
	char unk_1[24];                        // 80
	std::string* msg;                      // 88
	char unk_2[24];                        // 112
	std::vector<std::string>* params;      // 120
	char unk_3[16];                        // 136
	bool needsTrans;                       // 137
	char align_1[7];                       // 144
	std::string* xuid;                     // 152
	char unk_4[24];                        // 176
	void* platformChatId;                  // 184
};
#pragma pack()