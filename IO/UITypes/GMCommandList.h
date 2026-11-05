////////////////////////////////////////////////////////////////////////////////
//	GM 命令静态表（客户端补全用）
//
//	命令名与所需 GM 等级取自 s9 服务端
//	org.gms.client.command.CommandsExecutor::registerLv0~Lv6（权威清单）。
//	客户端仅有布尔 admin（= s9 gmLevel>1），无精确 gmLevel 数值，故补全对
//	admin 账号展示全量命令，越权由服务端最终校验。
//
//	用法：聊天框输入以 '!' 开头时按 Tab 触发前缀补全（见 UIChatBar）。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>
#include <string>

namespace ms
{
	struct GMCommandInfo
	{
		const char* name;
		int8_t min_gm_level; // 所需 GM 等级（s9 rank），仅供未来按等级过滤参考
	};

	// header-only：每个 TU 内 static local 线程安全且共享。
	inline const std::vector<GMCommandInfo>& gm_commands()
	{
		static const std::vector<GMCommandInfo> list =
		{
			// Lv0
			{ "help", 0 }, { "commands", 0 }, { "droplimit", 0 }, { "time", 0 },
			{ "credits", 0 }, { "staff", 0 }, { "uptime", 0 }, { "gacha", 0 },
			{ "dispose", 0 }, { "changel", 0 }, { "equiplv", 0 }, { "showrates", 0 },
			{ "rates", 0 }, { "online", 0 }, { "gm", 0 }, { "reportbug", 0 },
			{ "points", 0 }, { "joinevent", 0 }, { "leaveevent", 0 }, { "ranks", 0 },
			{ "str", 0 }, { "dex", 0 }, { "int", 0 }, { "luk", 0 },
			{ "enableauth", 0 }, { "toggleexp", 0 }, { "mylawn", 0 },
			// Lv1
			{ "bosshp", 1 }, { "mobhp", 1 }, { "whatdropsfrom", 1 }, { "whodrops", 1 },
			{ "buffme", 1 }, { "goto", 1 },
			// Lv2
			{ "recharge", 2 }, { "whereami", 2 }, { "hide", 2 }, { "unhide", 2 },
			{ "sp", 2 }, { "ap", 2 }, { "empowerme", 2 }, { "buffmap", 2 },
			{ "buff", 2 }, { "bomb", 2 }, { "dc", 2 }, { "cleardrops", 2 },
			{ "clearslot", 2 }, { "clearsavelocs", 2 }, { "warp", 2 }, { "warphere", 2 },
			{ "summon", 2 }, { "warpto", 2 }, { "reach", 2 }, { "follow", 2 },
			{ "gmshop", 2 }, { "heal", 2 }, { "item", 2 }, { "drop", 2 },
			{ "level", 2 }, { "levelpro", 2 }, { "setslot", 2 }, { "setstat", 2 },
			{ "maxstat", 2 }, { "maxskill", 2 }, { "resetskill", 2 }, { "search", 2 },
			{ "jail", 2 }, { "unjail", 2 }, { "job", 2 }, { "unbug", 2 },
			{ "id", 2 }, { "gachalist", 2 }, { "loot", 2 }, { "mobskill", 2 },
			{ "warpmap", 2 }, { "warparea", 2 },
			// Lv3
			{ "debuff", 3 }, { "fly", 3 }, { "spawn", 3 }, { "mutemap", 3 },
			{ "checkdmg", 3 }, { "inmap", 3 }, { "reloadevents", 3 }, { "reloaddrops", 3 },
			{ "reloadportals", 3 }, { "reloadmap", 3 }, { "reloadshops", 3 },
			{ "hpmp", 3 }, { "maxhpmp", 3 }, { "music", 3 }, { "monitor", 3 },
			{ "monitors", 3 }, { "ignore", 3 }, { "ignored", 3 }, { "pos", 3 },
			{ "togglecoupon", 3 }, { "togglewhitechat", 3 }, { "fame", 3 }, { "givenx", 3 },
			{ "givevp", 3 }, { "givems", 3 }, { "giverp", 3 }, { "expeds", 3 },
			{ "kill", 3 }, { "seed", 3 }, { "maxenergy", 3 }, { "killall", 3 },
			{ "notice", 3 }, { "rip", 3 }, { "openportal", 3 }, { "closeportal", 3 },
			{ "pe", 3 }, { "startevent", 3 }, { "endevent", 3 }, { "startmapevent", 3 },
			{ "stopmapevent", 3 }, { "online2", 3 }, { "ban", 3 }, { "unban", 3 },
			{ "healmap", 3 }, { "healperson", 3 }, { "hurt", 3 }, { "killmap", 3 },
			{ "night", 3 }, { "npc", 3 }, { "face", 3 }, { "hair", 3 },
			{ "startquest", 3 }, { "completequest", 3 }, { "resetquest", 3 },
			{ "timer", 3 }, { "timermap", 3 }, { "timerall", 3 },
			// Lv4
			{ "servermessage", 4 }, { "proitem", 4 }, { "seteqstat", 4 }, { "exprate", 4 },
			{ "mesorate", 4 }, { "droprate", 4 }, { "bossdroprate", 4 }, { "questrate", 4 },
			{ "travelrate", 4 }, { "fishrate", 4 }, { "itemvac", 4 }, { "forcevac", 4 },
			{ "zakum", 4 }, { "horntail", 4 }, { "pinkbean", 4 }, { "pap", 4 },
			{ "pianus", 4 }, { "cake", 4 }, { "playernpc", 4 }, { "playernpcremove", 4 },
			{ "pnpc", 4 }, { "pnpcremove", 4 }, { "pmob", 4 }, { "pmobremove", 4 },
			{ "warptolife", 4 },
			// Lv5
			{ "debug", 5 }, { "set", 5 }, { "showpackets", 5 }, { "showmovelife", 5 },
			{ "showsessions", 5 }, { "iplist", 5 },
			// Lv6
			{ "setgmlevel", 6 }, { "warpworld", 6 }, { "saveall", 6 }, { "dcall", 6 },
			{ "mapplayers", 6 }, { "getacc", 6 }, { "shutdown", 6 }, { "clearquestcache", 6 },
			{ "clearquest", 6 }, { "supplyratecoupon", 6 }, { "spawnallpnpcs", 6 },
			{ "eraseallpnpcs", 6 }, { "addchannel", 6 }, { "addworld", 6 },
			{ "removechannel", 6 }, { "removeworld", 6 }, { "devtest", 6 },
		};
		return list;
	}
}
