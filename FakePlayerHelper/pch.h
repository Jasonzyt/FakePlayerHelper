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

#include "seh_excpetion.h"

#define FETCH(tp, ptr) (*reinterpret_cast<tp*>(ptr))
#define FPHAPI __declspec(dllexport)
#undef max
#undef min
#undef ERROR
#define BUILD_RELEASE
#define BDS_V1_18
#if defined(TEST_VERSION)
//#define BDS_V1_18
#define BDS_V1_16
#endif
#if defined(BDS_V1_16)
#define PRINT coutp.p
#pragma comment(lib, "./LLSDK_1.16/LiteLoader.lib")
#elif defined(BDS_V1_18)
#define PRINT lllog
//#pragma comment(lib, "../SDK/Lib/LiteLoader.lib")
//#pragma comment(lib, "../SDK/Lib/Chakra.lib")
//#pragma comment(lib, "../SDK/Lib/SymDBHelper.lib")
//#pragma comment(lib, "../SDK/Lib/bedrock_server_api.lib")
//#pragma comment(lib, "../SDK/Lib/bedrock_server_var.lib")
#endif
#pragma warning(disable : 4996)
#pragma warning(disable : 26812)

#include "Logger.h"

using VA = unsigned long long;
using RVA = unsigned int;

template <typename COMMITER>
class OLogger;
class Level;
struct stdio_commit;
class LangPack;
namespace fs = std::filesystem;

class Config;
class WebSocket;
class AllowListFile;

extern AllowListFile* wlfile;
#if defined(BDS_V1_16)
extern OLogger<stdio_commit*> coutp;
#endif
extern Config* cfg;
extern WebSocket* fpws;
extern LangPack* lpk;
extern Level* level;

static const struct {
    const enum : int { Dev,
                       Beta,
                       Release } tag = Beta;
    const int major = 1;
    const int minor = 1;
    const int patch = 0;
    inline std::string toString() const {
        auto result = std::to_string(major) + "." +
                      std::to_string(minor) + "." +
                      std::to_string(patch);
        switch (tag) {
            case Dev: result += "-Dev"; break;
            case Beta: result += "-Beta"; break;
        }
        return result;
    }
} VERSION;
const std::string FPH_FOLDER = "./plugins/FakePlayerHelper/";
const std::string FPH_CONFIG = FPH_FOLDER + "config.json";
const std::string FPH_LANGPACK = FPH_FOLDER + "langpack.json";

#endif // PCH_H
