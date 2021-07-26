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

	const std::string FPH_LANGPACK_CONTENT = 
u8R"({
  "zh-cn": {
    "Overworld": "主世界",
    "Nether": "下界",
    "The End": "末路之地",
    "dllinit.success.read_cfg": "读取配置成功!",
    "dllinit.register_cmd": "注册命令中...",
    "dllinit.exception": "插件加载时某函数抛出了一个异常",
    "listcmd.opt.total": "当前假人数量",
    "listcmd.opt.header": "假  人   列  表",
    "listcmd.opt.fpname": "假人名",
    "listcmd.opt.summoner": "召唤者",
    "listcmd.opt.fppos": "假人位置",
    "fpws.connected": "已连接假人WebSocket服务器",
    "fpws.disconnected": "假人WebSocket服务器已断开!",
    "fpws.try_reconnect": "尝试重新连接...",
    "fpws.failed.try_reconnect": "尝试重新连接失败!请检查假人WebSocket端口和进程状态",
    "fpws.failed.connect": "连接假人WebSocket服务器失败!",
    "fpws.invalid.skin": "无效的皮肤配置!它只能为steve/alex/random!已默认为steve",
    "fpws.wait.timeout": "等待服务响应超时",
    "fpws.response.type_err": "WebSocket响应类型错误,请重试",
    "fpcmd.root.desc": "管理假人",
    "fpcmd.fpname": "假人名称",
    "fpcmd.tp.selector": "目标实体",
    "fpcmd.tp.dst": "目的地",
    "fpcmd.tp.dim": "维度",
    "fpcmd.cant_find_fp": "找不到目标假人,可能是未连接!",
    "fpcmd.selector.not_match": "目标选择器无匹配!",
    "fpcmd.global_limit.exceed": "全局假人数量已达到上限!",
    "fpcmd.player_limit.exceed": "你召唤的假人数量已达到上限!",
    "fpcmd.api.unfinished": "你现在不能使用此命令,接口还未完成",
    "fp.join.info.format": "假人 %s 进入了游戏,位置: %s%s",
    "fp.left.info.format": "假人 %s 退出了游戏,位置: %s%s",
    "fp.summoner.left.kick": "由于服主设置了假人召唤者离开时移除假人,即将移除假人 %s"
  },
  "en": {
    "Overworld": "Overworld",
    "Nether": "Nether",
    "The End": "The End",
    "dllinit.success.read_cfg": "Successfully read configuration",
    "dllinit.register_cmd": "Registering commands...",
    "dllinit.exception": "A function throws an exception when plugin initialization",
    "listcmd.opt.total": "Corrent FakePlayers",
    "listcmd.opt.header": "FakePlayer List",
    "listcmd.opt.fpname": "Name",
    "listcmd.opt.summoner": "Summoner",
    "listcmd.opt.fppos": "Pos",
    "fpws.connected": "Connected to WebSocket server",
    "fpws.disconnected": "WebSocket connection disconnected!",
    "fpws.try_reconnect": "Trying to reconnect...",
    "fpws.failed.try_reconnect": "Failed to reconnect.Please check WebSocket port and FakePlayer process.",
    "fpws.failed.connect": "Failed to connect to WebSocket server!",
    "fpws.invalid.skin": "Invalid skin config!It must be steve/alex/random!Has defaulted to steve.",
    "fpws.wait.timeout": "Waiting for response timed out",
    "fpws.response.type_err": "WebSocket response type error,please try again.",
    "fpcmd.root.desc": "Manage FakePlayer",
    "fpcmd.fpname": "FakePlayer name",
    "fpcmd.tp.selector": "Target entity",
    "fpcmd.tp.dst": "Destination",
    "fpcmd.tp.dim": "Dimension",
    "fpcmd.cant_find_fp": "Cannot find target FakePlayer.Maybe it is not connect.",
    "fpcmd.selector.not_matched": "No match for Target Selector",
    "fpcmd.global_limit.exceed": "The number of global FakePlayers has reached the upper limit!",
    "fpcmd.player_limit.exceed": "The number of FakePlayers you summoned has reached the upper limit!",
    "fpcmd.api.unfinished": "You cannot use this command because API unfinished yet",
    "fp.join.info.format": "FakePlayer %s joined,position: %s%s",
    "fp.left.info.format": "FakePlayer %s left,position: %s%s",
    "fp.summoner.left.kick": "Because the server owner has set removing FakePlayer when its summoner leaves,about to remove FakePlayer %s"
  }
})";

}

#endif //PCH_H
