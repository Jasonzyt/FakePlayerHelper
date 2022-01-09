#ifndef PCH_H
#define PCH_H
// Windows 头文件
#include <windows.h>
// C++ 标准库
#include <string>
#include <string_view>
#include <map>
#include <vector>
#include <filesystem>
#include <atomic>
#include <thread>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>

#include <cstdio>
#include <ctime>
// LiteLoader
#include <liteloader.h>
#include "build.h"
#include "seh_excpetion.h"

#define FETCH(tp, ptr) (*reinterpret_cast<tp*>(ptr))
#define FPHAPI __declspec(dllexport)
#undef max
#undef min
#undef ERROR
#if defined(TEST_VERSION)
    //#define BDS_LATEST
    #define BDS_V1_16
#endif
#if defined(BDS_V1_16)
	#define PRINT coutp.p
	#pragma comment(lib,"./LLSDK_1.16/LiteLoader.lib")
#elif defined(BDS_LATEST)
	#define PRINT lllog
	#pragma comment(lib,"./LLSDK/Lib/LiteLoader.lib")
	#pragma comment(lib,"./LLSDK/Lib/Chakra.lib")
	#pragma comment(lib,"./LLSDK/Lib/bedrock_server_api.lib")
	#pragma comment(lib,"./LLSDK/Lib/bedrock_server_var.lib")
#endif
#pragma warning(disable:4996)

using VA = unsigned long long;
using RVA = unsigned int; 
typedef unsigned long long xuid_t;
typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;

template <typename COMMITER>
class OLogger;
class Level;
struct stdio_commit;
namespace FPHelper
{
	namespace fs = std::filesystem;
	// 类声明
	class Config;
	class WebSocket;
	class LangPack;
	// 外部变量
    extern void* wlfile;
#if defined(BDS_V1_16)
	extern OLogger<stdio_commit> coutp;
#endif
	extern Config* cfg;
	extern WebSocket* fpws;
	extern LangPack* lpk;
	extern Level* level;

	const std::string FPH_VERSION  = "1.0.1";
	const std::string FPH_VERTYPE  = "Beta";
	const std::string FPH_FOLDER   = "./plugins/FakePlayerHelper";
	const std::string FPH_CONFIG   = "./plugins/FakePlayerHelper/config.json";
	const std::string FPH_LANGPACK = "./plugins/FakePlayerHelper/langpack.json";
}

#endif //PCH_H
