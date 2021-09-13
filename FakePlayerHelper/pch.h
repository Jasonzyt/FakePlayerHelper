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
#define PRINT coutp.p
#define FPHAPI __declspec(dllexport)
#undef max
#undef min
#undef ERROR
#if defined(TEST_VERSION)
    //#define BDS_LATEST
    #define BDS_V1_16
#endif
#if defined(BDS_V1_16)
	#pragma comment(lib,"./LLSDK_1.16/LiteLoader.lib")
#elif defined(BDS_LATEST)
	#pragma comment(lib,"./LLSDK/LiteLoader.lib")
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
    "listcmd.opt.fppos": "位置",
    "fpws.connected": "已连接假人WebSocket服务器",
    "fpws.disconnected": "假人WebSocket服务器已断开!",
    "fpws.try_reconnect": "尝试重新连接...",
    "fpws.failed.try_reconnect": "尝试重新连接失败!请检查假人WebSocket端口和进程状态",
    "fpws.failed.connect": "连接假人WebSocket服务器失败!",
    "fpws.invalid.skin": "无效的皮肤配置!它只能为steve/alex/random!已默认为steve",
    "fpws.wait.timeout": "等待WebSocket服务器响应超时",
    "fpws.unknown.packet.type": "未知的响应类型!",
    "fpws.parse.json.err.format": "JSON解析失败! 错误码(%d) ",
    "fpws.version.msg.format": "FakePlayer软件版本: %s",
    "fpws.failed.send": "发送消息失败!",
    "fpcmd.root.desc": "管理假人",
    "fpcmd.fpname": "假人名称",
    "fpcmd.tp.selector": "目标实体",
    "fpcmd.tp.dst": "目的地",
    "fpcmd.tp.dim": "维度(overworld|nether|end) ",
        "fpcmd.tp.successfully.format": "传送假人 %s 到 %s%s 成功!",
        "fpcmd.tp.unsuccessfully.format" : "传送假人 %s 到 %s%s 失败!",
        "fpcmd.cant_find_fp" : "找不到目标假人,可能是未连接!",
        "fpcmd.selector.not_matched" : "目标选择器无匹配!",
        "fpcmd.global_limit.exceed" : "全局假人数量已达到上限!",
        "fpcmd.player_limit.exceed" : "你召唤的假人数量已达到上限!",
        "fpcmd.api.unfinished" : "你现在不能使用此命令,接口还未完成",
        "fpcmd.success" : "执行成功!",
        "fpcmd.fail.format" : "执行失败: %s",
        "fpcmd.already.connected" : "WebSocket已经连接!",
        "fpcmd.fp.already.exists" : "假人已存在!",
        "console.successfully.add.fakeplayer" : "添加成功!",
        "console.successfully.remove.fakeplayer" : "移除成功!",
        "console.failed.add.fakeplayer.format" : "添加失败: %s",
        "console.failed.remove.fakeplayer.format" : "移除失败: %s",
        "console.found.new.fakeplayer.format" : "发现了一个新的假人: %s",
        "console.set.chat.control.enable.format" : "已启用假人 %s 的聊天信息控制",
        "console.set.chat.control.disable.format" : "已禁用假人 %s 的聊天信息控制",
        "console.join.info.format" : "假人 %s 进入了游戏,位置: %s%s",
        "console.left.info.format" : "假人 %s 退出了游戏,位置: %s%s",
        "console.summoner.left.kick" : "由于设置了假人召唤者离开时移除假人,即将移除假人 %s",
        "gamemsg.successfully.add.fakeplayer" : "§b[FPH]§r 添加成功!",
        "gamemsg.successfully.remove.fakeplayer" : "§b[FPH]§r 移除成功!",
        "gamemsg.failed.add.fakeplayer.format" : "§b[FPH]§r §c§l添加失败: %s",
        "gamemsg.failed.remove.fakeplayer.format" : "§b[FPH]§r §c§l移除失败: %s",
        "gamemsg.found.new.fakeplayer.format" : "§b[FPH]§r 发现了一个新的假人: %s",
        "gamemsg.set.chat.control.format" : "§b[FPH]§r 已开启假人 %s 聊天信息控制",
        "gamemsg.join.info.format" : "§b[FPH]§r 假人 %s 进入了游戏,位置: %s%s",
        "gamemsg.left.info.format" : "§b[FPH]§r 假人 %s 退出了游戏,位置: %s%s",
        "gamemsg.summoner.left.kick" : "§b[FPH]§r 由于服主设置了假人召唤者离开时移除假人,即将移除假人 %s",
        "gamemsg.ws.connected" : "§b[FPH][WS]§r 假人WebSocket服务器已连接",
        "gamemsg.ws.fp.version" : "§b[FPH][WS]§r 假人软件版本: %s"
},
"en": {
        "Overworld": "Overworld",
            "Nether" : "Nether",
            "The End" : "The End",
            "dllinit.success.read_cfg" : "Successfully read configuration",
            "dllinit.register_cmd" : "Registering commands...",
            "dllinit.exception" : "A function throws an exception when plugin initialization",
            "listcmd.opt.total" : "Corrent FakePlayers",
            "listcmd.opt.header" : "FakePlayer List",
            "listcmd.opt.fpname" : "Name",
            "listcmd.opt.summoner" : "Summoner",
            "listcmd.opt.fppos" : "Pos",
            "fpws.connected" : "Successfully connected FakePlayer WebSocket server",
            "fpws.disconnected" : "WebSocket connection disconnected!",
            "fpws.try_reconnect" : "Trying to reconnect...",
            "fpws.failed.try_reconnect" : "Failed to reconnect.Please check WebSocket port and FakePlayer process.",
            "fpws.failed.connect" : "Failed to connect to WebSocket server!",
            "fpws.invalid.skin" : "Invalid skin config!It must be steve/alex/random!Has defaulted to steve.",
            "fpws.wait.timeout" : "Waiting for response timed out",
            "fpws.unknown.packet.type" : "Unknown response type!",
            "fpws.parse.json.err.format" : "Parse JSON error! Error Code(%d) ",
            "fpws.version.msg.format" : "FakePlayer software version: %s",
            "fpws.failed.send" : "Failed to send WebSocket message!",
            "fpws.duplicate.name" : "Deplicate FakePlayer name!",
            "fpcmd.root.desc" : "Manage FakePlayer",
            "fpcmd.fpname" : "FakePlayer name",
            "fpcmd.tp.selector" : "Target entity",
            "fpcmd.tp.dst" : "Destination",
            "fpcmd.tp.dim" : "Dimension(overworld|nether|end) ",
            "fpcmd.tp.successfully.format" : "Teleport FakePlayer %s to %s%s successfully!",
            "fpcmd.tp.unsuccessfully.format" : "Teleport FakePlayer %s to %s%s unsuccessfully!",
            "fpcmd.cant_find_fp" : "Cannot find target FakePlayer.Maybe it is not connected.",
            "fpcmd.selector.not_matched" : "No match for Target Selector",
            "fpcmd.global_limit.exceed" : "The number of global FakePlayers has reached the upper limit!",
            "fpcmd.player_limit.exceed" : "The number of FakePlayers you summoned has reached the upper limit!",
            "fpcmd.api.unfinished" : "You cannot use this command because API unfinished yet",
            "fpcmd.success" : "Execution succeed!",
            "fpcmd.fail.format" : "Execution failed: %s",
            "fpcmd.already.connected" : "Already connected!",
            "fpcmd.fp.already.exists" : "The FakePlayer already exists",
            "console.successfully.add.fakeplayer" : "§b[FPH]§r Added successfully!",
            "console.successfully.remove.fakeplayer" : "§b[FPH]§r Removed successfully!",
            "console.failed.add.fakeplayer.format" : "§b[FPH]§r §c§lAdd unsuccessfully: %s",
            "console.failed.remove.fakeplayer.format" : "§b[FPH]§r §c§lRemove unsuccessfully: %s",
            "console.found.new.fakeplayer.format" : "Found a new FakePlayer: %s",
            "console.set.chat.control.enable.format" : "FakePlayer %s's chat message control is enabled",
            "console.set.chat.control.disable.format" : "FakePlayer %s's chat message control is disabled",
            "console.join.info.format" : "FakePlayer %s joined,position: %s%s",
            "console.left.info.format" : "FakePlayer %s left,position: %s%s",
            "console.summoner.left.kick" : "Because the server owner has set removing FakePlayer when its summoner leaves, removing FakePlayer %s",
            "gamemsg.successfully.add.fakeplayer" : "§b[FPH]§r Added successfully!",
            "gamemsg.successfully.remove.fakeplayer" : "§b[FPH]§r Removed successfully!",
            "gamemsg.failed.add.fakeplayer.format" : "§b[FPH]§r §c§lAdd unsuccessfully: %s",
            "gamemsg.failed.remove.fakeplayer.format" : "§b[FPH]§r §c§lRemove unsuccessfully: %s",
            "gamemsg.set.chat.control.disable.format" : "FakePlayer %s's chat message control is disabled",
            "gamemsg.found.new.fakeplayer.format" : "§b[FPH]§r Found a new FakePlayer: %s",
            "gamemsg.set.chat.control.format" : "§b[FPH]§r FakePlayer %s's chat message control is enable",
            "gamemsg.join.info.format" : "§b[FPH]§r FakePlayer %s joined,position: %s%s",
            "gamemsg.left.info.format" : "§b[FPH]§r FakePlayer %s left,position: %s%s",
            "gamemsg.summoner.left.kick" : "§b[FPH]§r Because the server owner has set removing FakePlayer when its summoner leaves, removing FakePlayer %s",
            "gamemsg.ws.connected" : "§b[FPH][WS]§r Successfully connected FakePlayer WebSocket server"
    }
})";

}

#endif //PCH_H
