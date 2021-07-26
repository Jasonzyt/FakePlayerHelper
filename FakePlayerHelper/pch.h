#ifndef PCH_H
#define PCH_H
#define WIN32_LEAN_AND_MEAN
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

#define FETCH(tp, ptr) (*reinterpret_cast<tp*>(ptr))
#define PRINT coutp.p
#define FPHAPI __declspec(dllexport)
#define BDS_V1_16
//#define BDS_V1_17
//#define BUILD_RELEASE
#undef max
#undef min
#undef ERROR
#if defined(BDS_V1_16)
	#pragma comment(lib,"./LLSDK_1.16/LiteLoader.lib")
#elif defined(BDS_V1_17)
	#pragma comment(lib,"./LLSDK_1.17/LiteLoader.lib")
#endif
#pragma warning(disable:4996)

using VA = unsigned long long;
using RVA = unsigned int; 
typedef unsigned long long xuid_t;
typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;

template <typename COMMITER>
class Logger;
class Level;
struct stdio_commit;
namespace FPHelper
{
	namespace fs = std::filesystem;
	// 类声明
	class Config;
	class FPWS;
	class LangPack;
	// 外部变量
	extern Logger<stdio_commit*> coutp;
	extern Config* cfg;
	extern FPWS* fpws;
	extern LangPack* lpk;
	extern Level* level;

	const std::string FPH_VERSION  = "1.0.0";
	const std::string FPH_VERTYPE  = "Beta";
	const std::string FPH_FOLDER   = "./plugins/FakePlayerHelper";
	const std::string FPH_CONFIG   = "./plugins/FakePlayerHelper/config.json";
	const std::string FPH_LANGPACK = "./plugins/FakePlayerHelper/langpack.json";

	const std::string FPH_LANGPACK_CONTENT = u8R"()";
}

#endif //PCH_H
