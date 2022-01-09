#ifndef LANGPACK_H
#define LANGPACK_H
#include "pch.h"
#include "Mess.h"
#include "nlohmann/json.hpp"

class LangPack {

	std::string langType;
	std::unordered_map<std::string, std::string> lang;
	nlohmann::json default_lang{
		{"zh-cn", {
            {"Overworld", "主世界"},
            {"Nether", "下界"},
            {"The End", "末路之地"},
            {"dllinit.success.read_cfg", "读取配置成功!"},
            {"dllinit.register.cmd", "注册命令中..."},
            {"dllinit.exception", "插件加载时某函数抛出了一个异常"},
            {"listcmd.opt.total", "当前假人数量"},
            {"listcmd.opt.header", "假  人   列  表"},
            {"listcmd.opt.fpname", "假人名"},
            {"listcmd.opt.summoner", "召唤者"},
            {"listcmd.opt.fppos", "位置"},
            {"fpws.connected", "已连接假人WebSocket服务器"},
            {"fpws.disconnected", "假人WebSocket服务器已断开!"},
            {"fpws.try.reconnect", "尝试重新连接..."},
            {"fpws.failed.reconnect", "尝试重新连接失败!请检查假人WebSocket端口和进程状态"},
            {"fpws.failed.connect", "连接假人WebSocket服务器失败!"},
            {"fpws.stop.reconnecting", "自动重连次数超过限制,已停止自动重连。发送命令'fp connect'再次连接"},
            {"fpws.invalid.skin", "无效的皮肤配置!它只能为steve/alex/random!已默认为steve"},
            {"fpws.wait.timeout", "等待WebSocket服务器响应超时"},
            {"fpws.unknown.packet.type", "未知的响应类型!"},
            {"fpws.parse.json.err.format", "JSON解析失败: "},
            {"fpws.version.msg.format", "FakePlayer软件版本: %s"},
            {"fpws.failed.send", "发送消息失败!"},
            {"fpcmd.root.desc", "管理假人"},
            {"fpcmd.fpname", "假人名称"},
            {"fpcmd.tp.selector", "目标实体"},
            {"fpcmd.tp.dst", "目的地"},
            {"fpcmd.tp.dim", "维度(overworld|nether|end)"},
            {"fpcmd.tp.successfully.format", "传送假人 %s 到 %s%s 成功!"},
            {"fpcmd.tp.unsuccessfully.format", "传送假人 %s 到 %s%s 失败!"},
            {"fpcmd.cant_find_fp", "找不到目标假人,可能是未连接!"},
            {"fpcmd.selector.not_matched", "目标选择器无匹配!"},
            {"fpcmd.global_limit.exceed", "全局假人数量已达到上限!"},
            {"fpcmd.player_limit.exceed", "你召唤的假人数量已达到上限!"},
            {"fpcmd.api.unfinished", "你现在不能使用此命令,接口还未完成"},
            {"fpcmd.success", "执行成功!"},
            {"fpcmd.fail.format", "执行失败: %s"},
            {"fpcmd.already.connected", "WebSocket已经连接!"},
            {"fpcmd.fp.already.exists", "假人已存在!"},
            {"fpcmd.permission.denied", "您无权使用此命令"},
            {"console.successfully.add.fakeplayer", "添加成功!"},
            {"console.successfully.remove.fakeplayer", "移除成功!"},
            {"console.failed.add.fakeplayer.format", "添加失败: %s"},
            {"console.failed.remove.fakeplayer.format", "移除失败: %s"},
            {"console.found.new.fakeplayer.format", "发现了一个新的假人: %s"},
            {"console.set.chat.control.enable.format", "已启用假人 %s 的聊天信息控制"},
            {"console.set.chat.control.disable.format", "已禁用假人 %s 的聊天信息控制"},
            {"console.join.info.format", "假人 %s 进入了游戏,位置: %s%s"},
            {"console.left.info.format", "假人 %s 退出了游戏,位置: %s%s"},
            {"console.summoner.left.kick", "由于设置了假人召唤者离开时移除假人,即将移除假人 %s"},
            {"gamemsg.successfully.add.fakeplayer", "§b[FPH]§r 添加成功!"},
            {"gamemsg.successfully.remove.fakeplayer", "§b[FPH]§r 移除成功!"},
            {"gamemsg.failed.add.fakeplayer.format", "§b[FPH]§r §c§l添加失败: %s"},
            {"gamemsg.failed.remove.fakeplayer.format", "§b[FPH]§r §c§l移除失败: %s"},
            {"gamemsg.found.new.fakeplayer.format", "§b[FPH]§r 发现了一个新的假人: %s"},
            {"gamemsg.set.chat.control.format", "§b[FPH]§r 已开启假人 %s 聊天信息控制"},
            {"gamemsg.join.info.format", "§b[FPH]§r 假人 %s 进入了游戏,位置: %s%s"},
            {"gamemsg.left.info.format", "§b[FPH]§r 假人 %s 退出了游戏,位置: %s%s"},
            {"gamemsg.summoner.left.kick", "§b[FPH]§r 由于服主设置了假人召唤者离开时移除假人,即将移除假人 %s"},
            {"gamemsg.ws.connected", "§b[FPH][WS]§r 假人WebSocket服务器已连接"},
            {"gamemsg.ws.fp.version", "§b[FPH][WS]§r 假人软件版本: %s"}
        },
        {"en", {
            {"Overworld", "Overworld"},
            {"Nether", "Nether"},
            {"The End", "The End"},
            {"dllinit.success.read_cfg", "Successfully read configuration"},
            {"dllinit.register_cmd", "Registering commands..."},
            {"dllinit.exception", "A function throws an exception when plugin initialization"},
            {"listcmd.opt.total", "Corrent FakePlayers"},
            {"listcmd.opt.header", "FakePlayer List"},
            {"listcmd.opt.fpname", "Name"},
            {"listcmd.opt.summoner", "Summoner"},
            {"listcmd.opt.fppos", "Pos"},
            {"fpws.connected", "Successfully connected FakePlayer WebSocket server"},
            {"fpws.disconnected", "WebSocket connection disconnected!"},
            {"fpws.try.reconnect", "Trying to reconnect..."},
            {"fpws.failed.reconnect", "Failed to reconnect.Please check WebSocket port and FakePlayer process."},
            {"fpws.failed.connect", "Failed to connect to FakePlayer WebSocket server"},
            {"fpws.stop.reconnecting", "The number of automatic reconnecting exceeds the limit, and the automatic reconnecting has been stopped. Send the command 'fp connect' to connect again"},
            {"fpws.invalid.skin", "Invalid skin config!It must be steve/alex/random!Has defaulted to steve."},
            {"fpws.wait.timeout", "Waiting for response timed out"},
            {"fpws.unknown.packet.type", "Unknown response type!"},
            {"fpws.parse.json.err.format", "Can't parse JSON: "},
            {"fpws.version.msg.format", "FakePlayer software version: %s"},
            {"fpws.failed.send", "Failed to send WebSocket message!"},
            {"fpws.duplicate.name", "Deplicate FakePlayer name!"},
            {"fpcmd.root.desc", "Manage FakePlayer"},
            {"fpcmd.fpname", "FakePlayer name"},
            {"fpcmd.tp.selector", "Target entity"},
            {"fpcmd.tp.dst", "Destination"},
            {"fpcmd.tp.dim", "Dimension(overworld|nether|end)"},
            {"fpcmd.tp.successfully.format", "Teleport FakePlayer %s to %s%s successfully!"},
            {"fpcmd.tp.unsuccessfully.format", "Teleport FakePlayer %s to %s%s unsuccessfully!"},
            {"fpcmd.cant_find_fp", "Cannot find target FakePlayer.Maybe it is not connected."},
            {"fpcmd.selector.not_matched", "No match for Target Selector"},
            {"fpcmd.global_limit.exceed", "The number of global FakePlayers has reached the upper limit!"},
            {"fpcmd.player_limit.exceed", "The number of FakePlayers you summoned has reached the upper limit!"},
            {"fpcmd.api.unfinished", "You cannot use this command because API unfinished yet"},
            {"fpcmd.success", "Execution succeed!"},
            {"fpcmd.fail.format", "Execution failed: %s"},
            {"fpcmd.already.connected", "Already connected!"},
            {"fpcmd.fp.already.exists", "The FakePlayer already exists"},
            {"fpcmd.permission.denied", "Command permission denied"},
            {"console.successfully.add.fakeplayer", "§b[FPH]§r Added successfully!"},
            {"console.successfully.remove.fakeplayer", "§b[FPH]§r Removed successfully!"},
            {"console.failed.add.fakeplayer.format", "§b[FPH]§r §c§lAdd unsuccessfully: %s"},
            {"console.failed.remove.fakeplayer.format", "§b[FPH]§r §c§lRemove unsuccessfully: %s"},
            {"console.found.new.fakeplayer.format", "Found a new FakePlayer: %s"},
            {"console.set.chat.control.enable.format", "FakePlayer %s's chat message control is enabled"},
            {"console.set.chat.control.disable.format", "FakePlayer %s's chat message control is disabled"},
            {"console.join.info.format", "FakePlayer %s joined,position: %s%s"},
            {"console.left.info.format", "FakePlayer %s left,position: %s%s"},
            {"console.summoner.left.kick", "Because the server owner has set removing FakePlayer when its summoner leaves, removing FakePlayer %s"},
            {"gamemsg.successfully.add.fakeplayer", "§b[FPH]§r Added successfully!"},
            {"gamemsg.successfully.remove.fakeplayer", "§b[FPH]§r Removed successfully!"},
            {"gamemsg.failed.add.fakeplayer.format", "§b[FPH]§r §c§lAdd unsuccessfully: %s"},
            {"gamemsg.failed.remove.fakeplayer.format", "§b[FPH]§r §c§lRemove unsuccessfully: %s"},
            {"gamemsg.set.chat.control.disable.format", "FakePlayer %s's chat message control is disabled"},
            {"gamemsg.found.new.fakeplayer.format", "§b[FPH]§r Found a new FakePlayer: %s"},
            {"gamemsg.set.chat.control.format", "§b[FPH]§r FakePlayer %s's chat message control is enable"},
            {"gamemsg.join.info.format", "§b[FPH]§r FakePlayer %s joined,position: %s%s"},
            {"gamemsg.left.info.format", "§b[FPH]§r FakePlayer %s left,position: %s%s"},
            {"gamemsg.summoner.left.kick", "§b[FPH]§r Because the server owner has set removing FakePlayer when its summoner leaves, removing FakePlayer %s"},
            {"gamemsg.ws.connected", "§b[FPH][WS]§r Successfully connected FakePlayer WebSocket server"}
        }
    };

	inline void writeDefault(const std::string& fn) {
		std::fstream file(fn, std::ios::out | std::ios::ate);
		file << std::setw(4) << default_lang;
		file.close();
	}

	inline void init(const std::string& fn, const std::string& language) {
		nlohmann::json json;
		auto lastSlash = fn.find_last_of('/');
		auto lastBackslash = fn.find_last_of('\\');
		fs::create_directories(fn.substr(0, max((lastSlash != std::string::npos ? lastSlash : 0),
			(lastBackslash != std::string::npos ? lastBackslash : 0))));
		std::fstream file(fn, std::ios::in | std::ios::app);
		std::ostringstream oss;
		bool flag = false;
		oss << file.rdbuf();
		file.close();
		if (oss.str().empty()) {
			writeDefault(fn);
		}

		try {
			json = nlohmann::json::parse(oss.str());
		}
		catch (nlohmann::detail::exception e) {
			if (e.id != 101) {
				PRINT<ERROR, RED>("Can't parse LangPack json: ", e.what());
			}
			PRINT("Try rewriting langpack file...");
			writeDefault(fn);
			if (!default_lang.count(language)) {
				PRINT<WARN, YELLOW>("Unknown language name, please check config file! Default English");
				lang = default_lang.at("en").get<std::unordered_map<std::string, std::string>>();
			}
			else lang = default_lang.at(language).get<std::unordered_map<std::string, std::string>>();
			return;
		}

		if (!json.count(language)) {
			PRINT<WARN, YELLOW>("Unknown language name, please check config file! Default English");
			lang = default_lang.at("en").get<std::unordered_map<std::string, std::string>>();
			return;
		}
		/*
		bool modified = false;
		for (auto it = default_lang["en"].begin();
			it != default_lang.end(); it++) {
			if (!json.count(it.key())) {
				modified = true;
				if (language == "zh-cn") {
					json.at(language)[it.key()] = default_lang["zh-cn"].at(it.key());
				}
				else {
					json.at(language)[it.key()] = it.value();
				}
			}
		}
		if (modified) {
			std::fstream file(fn, std::ios::out | std::ios::ate);
			file << std::setw(4) << json;
			file.close();
		}*/
		lang = json.at(language).get<std::unordered_map<std::string, std::string>>();
	}

public:

	LangPack(const std::string& file, const std::string& language) {
		langType = language;
		init(file, language);
	}

	inline std::string get(const std::string& key) {
		if (lang.count(key)) {
			return lang.at(key);
		}
		else if (default_lang.count(langType) && default_lang[langType].count(key)) {
			return default_lang[langType][key].get<std::string>();
		}
		PRINT<WARN, YELLOW>("Could not find the translation for ", key);
		return key;
	}

	template<typename ... Args>
	inline std::string localize(const std::string& key, Args... args) {
		return format(this->get(key), args...);
	}
	
};

#endif // !LANGPACK_H
