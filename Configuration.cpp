//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#include "Configuration.h"

#include "Util/Resource/Keyring.h"

#include "Constants.h"
#include "Util/Misc.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <sys/stat.h>
#ifdef __ANDROID__
#include <unistd.h>
#endif

namespace ms
{
	// =========================================================================
	// Settings 写盘模板
	//
	// 它解决三个问题：
	//   1. 稳定顺序：settings 存放在 TypeMap 中，遍历顺序不确定，
	//      直接遍历写出会导致每次退出都打乱行序，配置 diff 无从比对；
	//   2. 分组与注释：按业务含义分组，并给每个键写明含义/取值，
	//      保证"配置文件本身可读"，而不只是一堆键值；
	//   3. 可发现性：新增配置项必须登记在这里，否则不会被持久化。
	//
	// key 必须与 Configuration.h 中对应 Entry 的层级键名严格一致，
	// 写盘时按 '.' 展开为缩进层级（"server.ip" -> "server:\n  ip: ..."）。
	// =========================================================================
	struct SettingDoc
	{
		const char* section;	// 分组标题，连续相同的项归在同一个注释块下
		const char* key;		// 点分层级键名
		const char* comment;	// 写在键上方的说明
	};

	// 每一层缩进的空格数，与 xy-config.yaml 保持相同。
	constexpr size_t INDENT_WIDTH = 2;

	const SettingDoc SETTINGS_ORDER[] =
	{
		// 登录服务器
		{ "登录服务器", "server.ip",					"登录服 IP 地址" },
		{ "登录服务器", "server.port",					"登录服端口，需与服务端配置一致" },

		// 数据源
		{ "数据源",     "data.format",					"数据源格式：auto / img（生产推荐）/ wz / nx" },
		{ "数据源",     "data.dir",						"经典 .img 数据根目录，data.format=img 或定位时使用" },
		{ "数据源",     "data.keymode",					"密钥模式：auto / builtin / external" },
		{ "数据源",     "data.keyfile",					"外部密钥文件路径，可为空" },

		// HTTP 补丁（独立于游戏服）
		{ "补丁",       "patch.enabled",				"是否启用启动差异资源同步" },
		{ "补丁",       "patch.url",					"manifest 完整 URL，空则跳过" },
		{ "补丁",       "patch.channel",				"补丁频道，与 manifest.channel 比对" },
		{ "补丁",       "patch.timeout_ms",				"HTTP 超时毫秒" },
		{ "补丁",       "patch.retries",				"失败重试次数" },
		{ "补丁",       "patch.allow_skip",				"失败时是否允许继续进游戏" },

		// 窗口
		{ "窗口",       "window.width",					"窗口宽度，读取后同步到运行时常量" },
		{ "窗口",       "window.height",				"窗口高度，读取后同步到运行时常量" },
		{ "窗口",       "window.fullscreen",			"是否全屏启动" },
		{ "窗口",       "window.vsync",					"是否开启垂直同步" },

		// 字体
		{ "字体",       "font.path.normal",				"常规字体文件路径" },
		{ "字体",       "font.path.bold",				"粗体字体文件路径" },

		// 音频
		{ "音频",       "audio.bgm.volume",				"背景音乐音量 0-100" },
		{ "音频",       "audio.sfx.volume",				"音效音量 0-100" },

		// 登录
		{ "登录",       "login.save",					"是否记住上次使用的账号名" },
		{ "登录",       "login.account",				"上次使用的账号名，可为空" },
		{ "登录",       "login.world",					"上次选择的区服序号" },
		{ "登录",       "login.channel",				"上次选择的频道序号" },
		{ "登录",       "login.region",					"上次选择的地区序号" },
		{ "登录",       "login.character",				"上次选择的角色槽位" },

		// 聊天栏
		{ "聊天栏",     "chat.max",						"聊天栏是否展开" },
		{ "聊天栏",     "chat.x",						"聊天栏宽度" },
		{ "聊天栏",     "chat.y",						"聊天栏高度" },

		// 小地图
		{ "小地图",     "minimap.type",					"小地图类型（2 = 常规样式）" },
		{ "小地图",     "minimap.simple",				"是否使用简约模式" },
		{ "小地图",     "minimap.default_helpers",		"是否绘制默认标志物（NPC/传送点等）" },

		// 界面开关
		{ "界面",       "ui.buff_timer",				"是否在 buff 图标上显示剩余时间" },

		// 界面位置（关闭客户端时的最后窗口位置）
		{ "界面位置",   "ui.position.stats",			"角色属性面板" },
		{ "界面位置",   "ui.position.eqinv",			"装备栏" },
		{ "界面位置",   "ui.position.inv",				"消耗/其他栏" },
		{ "界面位置",   "ui.position.skill",			"技能面板" },
		{ "界面位置",   "ui.position.quest",			"任务面板" },
		{ "界面位置",   "ui.position.map",				"世界地图" },
		{ "界面位置",   "ui.position.userlist",			"用户列表" },
		{ "界面位置",   "ui.position.chat",				"聊天栏" },
		{ "界面位置",   "ui.position.minimap",			"小地图" },
		{ "界面位置",   "ui.position.shop",				"NPC 商店" },
		{ "界面位置",   "ui.position.notice",			"公告面板" },
		{ "界面位置",   "ui.position.maplechat",		"枫叶聊天/排行榜" },
		{ "界面位置",   "ui.position.channel",			"频道选择" },
		{ "界面位置",   "ui.position.joypad",			"手柄面板" },
		{ "界面位置",   "ui.position.event",			"活动面板" },
		{ "界面位置",   "ui.position.keyconfig",		"按键设置" },
		{ "界面位置",   "ui.position.optionmenu",		"系统选项" },
		{ "界面位置",   "ui.position.charinfo",			"角色信息" }
	};

	constexpr size_t SETTINGS_ORDER_SIZE = sizeof(SETTINGS_ORDER) / sizeof(SETTINGS_ORDER[0]);

	// 把 "ui.position.stats" 拆成 ["ui", "position", "stats"]。
	std::vector<std::string> split_key(const std::string& key)
	{
		std::vector<std::string> parts;
		std::string part;
		std::istringstream stream(key);

		while (getline(stream, part, '.'))
			if (!part.empty())
				parts.push_back(part);

		return parts;
	}

	Configuration::Configuration()
	{
		settings.emplace<ServerIP>();
		settings.emplace<ServerPort>();
		settings.emplace<DataFormat>();
		settings.emplace<DataDir>();
		settings.emplace<KeyMode>();
		settings.emplace<KeyFile>();
		settings.emplace<PatchEnabled>();
		settings.emplace<PatchUrl>();
		settings.emplace<PatchChannel>();
		settings.emplace<PatchTimeoutMs>();
		settings.emplace<PatchRetries>();
		settings.emplace<PatchAllowSkip>();
		settings.emplace<Fullscreen>();
		settings.emplace<Width>();
		settings.emplace<Height>();
		settings.emplace<VSync>();
		settings.emplace<BuffTimer>();
		settings.emplace<FontPathNormal>();
		settings.emplace<FontPathBold>();
		settings.emplace<BGMVolume>();
		settings.emplace<SFXVolume>();
		settings.emplace<SaveLogin>();
		settings.emplace<DefaultAccount>();
		settings.emplace<DefaultWorld>();
		settings.emplace<DefaultChannel>();
		settings.emplace<DefaultRegion>();
		settings.emplace<DefaultCharacter>();
		settings.emplace<AutoLogin>();
		settings.emplace<AutoPass>();
		settings.emplace<ChatViewMax>();
		settings.emplace<ChatViewX>();
		settings.emplace<ChatViewY>();
		settings.emplace<PosSTATS>();
		settings.emplace<PosEQINV>();
		settings.emplace<PosINV>();
		settings.emplace<PosSKILL>();
		settings.emplace<PosQUEST>();
		settings.emplace<PosMAP>();
		settings.emplace<PosUSERLIST>();
		settings.emplace<PosCHAT>();
		settings.emplace<PosMINIMAP>();
		settings.emplace<PosSHOP>();
		settings.emplace<PosNOTICE>();
		settings.emplace<PosMAPLECHAT>();
		settings.emplace<PosCHANNEL>();
		settings.emplace<PosJOYPAD>();
		settings.emplace<PosEVENT>();
		settings.emplace<PosDAILYGIFT>();
		settings.emplace<PosKEYCONFIG>();
		settings.emplace<PosOPTIONMENU>();
		settings.emplace<PosCHARINFO>();
		settings.emplace<MiniMapType>();
		settings.emplace<MiniMapSimpleMode>();
		settings.emplace<MiniMapDefaultHelpers>();

		load();
	}

	Configuration::~Configuration()
	{
#ifndef _DEBUG
		save();
#endif
	}

	void Configuration::load()
	{
		// 收集所有已知配置项的名字。解析器用它区分两种"冒号后没有值"的情况：
		// 分组标题（server:）与取值被清空的设置项（login.account:）。
		std::unordered_set<std::string> leaf_keys;

		for (const auto& setting : settings)
		{
			if (!setting.second)
				continue;

			leaf_keys.insert(setting.second->name);
		}

		// 用户级配置：客户端退出时按 SAVE_ORDER 写回。
		std::unordered_map<std::string, std::string> rawsettings = parse_config_file(FILENAME, leaf_keys);

		// 部署级配置：与 Settings 完全同格式，但优先级更高，
		// 便于在不动用户设置的前提下固定服务器地址、数据后端等部署相关取值。
		xysettings = parse_config_file(XYCONFIG_FILENAME, leaf_keys);

		for (const auto& kv : xysettings)
			rawsettings[kv.first] = kv.second;

		// 记录文件修改时间，供运行时按需热重载（如修改跳转网址）。
		{
			std::time_t wt = 0;
			if (get_xy_mtime(wt))
				xy_last_write = wt;
		}

#ifdef __ANDROID__
		// [DBG-CFG] 临时诊断：记录 load 发生时的 cwd、xy-config.yaml 是否可打开、
		// 以及解析出的键数量，用于判断单例是否在 chdir 之前就被构造。
		{
			char cwdbuf[512] = { 0 };
			std::ofstream dbg("_config_debug.log", std::ios::app);
			dbg << "load: cwd=" << (::getcwd(cwdbuf, sizeof(cwdbuf)) ? cwdbuf : "(getcwd failed)")
				<< " raw_keys=" << rawsettings.size()
				<< " xy_keys=" << xysettings.size()
				<< " xy_file_open=" << (std::ifstream(XYCONFIG_FILENAME).good() ? 1 : 0)
				<< '\n';
			const auto it = rawsettings.find("server.ip");
			dbg << "  server.ip=" << (it == rawsettings.end() ? std::string("(absent)") : it->second) << '\n';
			for (const auto& kv : rawsettings)
				dbg << "  key=[" << kv.first << "] = [" << kv.second << "]\n";
		}
#endif

		// Replace default values with loaded values.
		// 取值为空也要生效：旧实现用 "split + 2 < line.size()" 过滤，
		// 会把 "Account =" 这类空值整条丢弃，导致"清空账号名"无法保存。
		for (auto& setting : settings)
		{
			if (!setting.second)
				continue;

			auto rsiter = rawsettings.find(setting.second->name);

			if (rsiter != rawsettings.end())
				setting.second->value = rsiter->second;
		}

		// 将配置中的宽高同步到运行时常量，供 Window/渲染/布局使用。
		Constants::Constants::get().set_viewwidth(Setting<Width>::get().load());
		Constants::Constants::get().set_viewheight(Setting<Height>::get().load());
		}

	std::unordered_map<std::string, std::string> Configuration::parse_config_file(
		const char* filename, const std::unordered_set<std::string>& leaf_keys) const
	{
		std::unordered_map<std::string, std::string> result;

		// 二进制读取：避免不同平台的换行转换干扰，也便于统一处理 UTF-8 BOM。
		std::ifstream file(filename, std::ios::binary);

		if (!file.is_open())
			return result;

		std::ostringstream buffer;
		buffer << file.rdbuf();

		std::string contents = buffer.str();

		// UTF-8 BOM：本项目的配置文件统一保存为**不带 BOM** 的 UTF-8，
		// 但手工编辑时可能被编辑器加上，读取阶段直接剥掉，
		// 否则首行的键名会带上 "\xEF\xBB\xBF" 前缀而永远匹配不上。
		if (contents.size() >= 3
			&& static_cast<unsigned char>(contents[0]) == 0xEF
			&& static_cast<unsigned char>(contents[1]) == 0xBB
			&& static_cast<unsigned char>(contents[2]) == 0xBF)
		{
			contents.erase(0, 3);
		}

		// 去掉首尾空白（含 '\r'，兼容外部工具写出的 CRLF）。
		auto trim = [](const std::string& s)
		{
			auto first = s.find_first_not_of(" \t\r\n");

			if (first == std::string::npos)
				return std::string();

			auto last = s.find_last_not_of(" \t\r\n");

			return s.substr(first, last - first + 1);
		};

		// 记录父节点栈（缩进, 键名），用于把缩进还原成 "a.b.c" 形式的层级键。
		std::vector<std::pair<int, std::string>> stack;
		std::string line;
		std::istringstream stream(contents);

		while (getline(stream, line))
		{
			int indent = 0;
			while (indent < static_cast<int>(line.size()) && (line[indent] == ' ' || line[indent] == '\t'))
				indent++;

			std::string content = trim(line);

			if (content.empty() || content[0] == '#')
				continue;

			// key/value 分隔符：第一个后跟空白或位于行尾的冒号，
			// 避免把 "http://..." 中的 "://" 当作分隔符。
			size_t split = std::string::npos;
			for (size_t i = 0; i < content.size(); i++)
			{
				if (content[i] == ':')
				{
					if (i + 1 >= content.size() || content[i + 1] == ' ' || content[i + 1] == '\t')
					{
						split = i;
						break;
					}
				}
			}

			if (split == std::string::npos)
				continue;

			std::string key = trim(content.substr(0, split));
			std::string value = trim(content.substr(split + 1));

			if (key.empty())
				continue;

			// 弹出缩进 >= 当前层的父节点（处理同层兄弟）。
			while (!stack.empty() && indent <= stack.back().first)
				stack.pop_back();

			std::string composite = key;
			for (const auto& p : stack)
				composite = p.second + "." + composite;

			if (value.empty() && leaf_keys.find(composite) == leaf_keys.end())
			{
				// 无值且不是已知配置项：视为分组节点，压栈供子键拼接。
				stack.push_back({ indent, key });
			}
			else
			{
				// 已知配置项的空值同样要记录，否则"清空某个设置"无法生效。
				result[composite] = value;
			}
		}

		return result;
	}

	bool Configuration::get_xy_mtime(std::time_t& out) const
	{
#if defined(_WIN32)
		struct _stat st;
		if (_stat(XYCONFIG_FILENAME, &st) != 0)
			return false;
		out = st.st_mtime;
#else
		struct stat st;
		if (::stat(XYCONFIG_FILENAME, &st) != 0)
			return false;
		out = st.st_mtime;
#endif
		return true;
	}

	void Configuration::reload_xy_config()
	{
		// 与 load() 保持一致：热重载时同样要提供已知配置项集合，
		// 否则 xy-config.yaml 里的分组标题会被误当成设置项。
		std::unordered_set<std::string> leaf_keys;

		for (const auto& setting : settings)
		{
			if (!setting.second)
				continue;

			leaf_keys.insert(setting.second->name);
		}

		xysettings = parse_config_file(XYCONFIG_FILENAME, leaf_keys);

		std::time_t wt = 0;
		if (get_xy_mtime(wt))
			xy_last_write = wt;
	}

	void Configuration::reload_if_changed()
	{
		std::time_t wt = 0;
		if (!get_xy_mtime(wt))
			return;

		if (wt == xy_last_write)
			return;

		reload_xy_config();
	}

	// 外链取值：三级回退。
	//   1. nested：xy-config.yaml 中的层级键（推荐写法，如 "login.urls.joinlink"）
	//   2. flat  ：同文件中的扁平别名（如 "joinlink"），仅为兼容早期部署文件保留
	//   3. fallback：源码内置常量
	// 命中空串视为未配置，继续向下一级回退，这样"置空"与"未配置"表现一致。
	std::string Configuration::get_url(const std::string& nested, const std::string& flat, const std::string& fallback) const
	{
		auto it = xysettings.find(nested);
		if (it != xysettings.end() && !it->second.empty())
			return it->second;

		it = xysettings.find(flat);
		if (it != xysettings.end() && !it->second.empty())
			return it->second;

		return fallback;
	}

	void Configuration::save() const
	{
		// 二进制写出：保证不产生 UTF-8 BOM，也不会被 CRT 改写成 CRLF，
		// 使 Settings 在任何工具下都保持「UTF-8 无 BOM + LF」的一致形态。
		std::ofstream config(FILENAME, std::ios::binary);

		if (!config.is_open())
			return;

		config << "# MapleStory client settings\n"
			   << "#\n"
			   << "# 本文件在客户端退出时自动生成，键名采用小写点分的层级命名，\n"
			   << "# 按缩进展开分组（例如 server.ip 写作 server: 之下的 ip:）。\n"
			   << "#\n"
			   << "# 优先级：同目录下的 xy-config.yaml（部署级）> 本文件（用户级）> 内置默认值。\n"
			   << "# 想固定某项部署取值时，请在 xy-config.yaml 中用同名层级键覆盖，\n"
			   << "# 而不是改这里（本文件会被客户端覆写）。\n"
			   << "#\n"
			   << "# 编码：UTF-8（无 BOM），换行：LF。\n";

		std::string current_section;
		std::vector<std::string> open_groups;

		for (size_t i = 0; i < SETTINGS_ORDER_SIZE; i++)
		{
			const SettingDoc& doc = SETTINGS_ORDER[i];

			// 换分组：输出分组标题，并重新从顶层开始展开缩进。
			if (current_section != doc.section)
			{
				current_section = doc.section;

				config << "\n# ---- " << doc.section << " ----\n";

				open_groups.clear();
			}

			const std::vector<std::string> parts = split_key(doc.key);

			if (parts.empty())
				continue;

			// 保留与上一行相同的分组前缀，只补齐发生变化的那几层。
			size_t common = 0;
			while (common < open_groups.size()
				&& common + 1 < parts.size()
				&& open_groups[common] == parts[common])
			{
				common++;
			}

			open_groups.resize(common);

			while (open_groups.size() + 1 < parts.size())
			{
				const std::string indent(open_groups.size() * INDENT_WIDTH, ' ');

				config << indent << parts[open_groups.size()] << ":\n";

				open_groups.push_back(parts[open_groups.size()]);
			}

			const std::string indent(open_groups.size() * INDENT_WIDTH, ' ');

			config << indent << "# " << doc.comment << "\n";

			const std::string* value = find_value(doc.key);

			config << indent << parts.back() << ": " << (value ? *value : "") << "\n";
		}
	}

	const std::string* Configuration::find_value(const std::string& key) const
	{
		for (const auto& setting : settings)
		{
			if (!setting.second)
				continue;

			if (setting.second->name == key)
				return &setting.second->value;
		}

		return nullptr;
	}

	void Configuration::BoolEntry::save(bool b)
	{
		value = b ? "true" : "false";
	}

	bool Configuration::BoolEntry::load() const
	{
		return value == "true";
	}

	void Configuration::StringEntry::save(std::string str)
	{
		value = str;
	}

	std::string Configuration::StringEntry::load() const
	{
		return value;
	}

	void Configuration::PointEntry::save(Point<int16_t> vec)
	{
		value = vec.to_string();
	}

	Point<int16_t> Configuration::PointEntry::load() const
	{
		std::string xstr = value.substr(1, value.find(",") - 1);
		std::string ystr = value.substr(value.find(",") + 1, value.find(")") - value.find(",") - 1);

		auto x = string_conversion::or_zero<int16_t>(xstr);
		auto y = string_conversion::or_zero<int16_t>(ystr);

		return { x, y };
	}

	bool Configuration::get_show_fps() const
	{
		return SHOW_FPS;
	}

	bool Configuration::get_show_packets() const
	{
		return SHOW_PACKETS;
	}

	bool Configuration::get_auto_login() const
	{
		// opt-in 自动登录：由 login.auto 配置驱动（Release 亦生效），供自动化/调试直连。
		return Setting<AutoLogin>::get().load();
	}

	uint8_t Configuration::get_auto_world()
	{
		return auto_world;
	}

	uint8_t Configuration::get_auto_channel()
	{
		return auto_channel;
	}

	std::string Configuration::get_auto_acc()
	{
		if (!auto_acc.empty())
			return auto_acc;

		// 回退到已保存的登录账号（login.account）。
		return Setting<DefaultAccount>::get().load();
	}

	std::string Configuration::get_auto_pass()
	{
		if (!auto_pass.empty())
			return auto_pass;

		// 回退到配置中的自动登录密码（login.pass）。
		return Setting<AutoPass>::get().load();
	}

	std::string Configuration::get_auto_pic()
	{
		return auto_pic;
	}

	int32_t Configuration::get_auto_cid()
	{
		return auto_cid;
	}

	std::string Configuration::get_title() const
	{
		return TITLE;
	}

	std::string Configuration::get_version() const
	{
		return VERSION;
	}

	std::string Configuration::get_data_format() const
	{
		return Setting<DataFormat>::get().load();
	}

	std::string Configuration::get_data_dir() const
	{
		return Setting<DataDir>::get().load();
	}

	ms::res::KeyMode Configuration::get_keymode() const
	{
		std::string v = Setting<KeyMode>::get().load();
		if (v == "builtin")
			return ms::res::KeyMode::Builtin;
		if (v == "external")
			return ms::res::KeyMode::External;
		return ms::res::KeyMode::Auto;
	}

	std::string Configuration::get_keyfile() const
	{
		return Setting<KeyFile>::get().load();
	}

	bool Configuration::get_buff_timer() const
	{
		return Setting<BuffTimer>::get().load();
	}

	bool Configuration::get_patch_enabled() const
	{
		return Setting<PatchEnabled>::get().load();
	}

	std::string Configuration::get_patch_url() const
	{
		return Setting<PatchUrl>::get().load();
	}

	std::string Configuration::get_login_music() const
	{
		return LoginMusic;
	}

	std::string Configuration::get_login_music_sea() const
	{
		return LoginMusicSEA;
	}

	std::string Configuration::get_login_music_newtro() const
	{
		return LoginMusicNewtro;
	}

	std::string Configuration::get_joinlink() const
	{
		return get_url("login.urls.joinlink", "joinlink", JOINLINK);
	}

	std::string Configuration::get_website() const
	{
		return get_url("login.urls.website", "website", WEBSITE);
	}

	std::string Configuration::get_findid() const
	{
		return get_url("login.urls.findid", "findid", FINDID);
	}

	std::string Configuration::get_findpass() const
	{
		return get_url("login.urls.findpass", "findpass", FINDPASS);
	}

	std::string Configuration::get_resetpic() const
	{
		return RESETPIC;
	}

	std::string Configuration::get_chargenx() const
	{
		return CHARGENX;
	}

	void Configuration::set_macs(char* macs)
	{
		MACS = macs;
	}

	void Configuration::set_hwid(char* hwid, char* volumeSerialNumber)
	{
		VolumeSerialNumber = volumeSerialNumber;

		std::string newHWID;

		newHWID.append(hwid);
		newHWID.append("_");

		std::string part1 = VolumeSerialNumber.substr(0, 2);
		std::string part2 = VolumeSerialNumber.substr(2, 2);
		std::string part3 = VolumeSerialNumber.substr(4, 2);
		std::string part4 = VolumeSerialNumber.substr(6, 2);

		// TODO: VolumeSerialNumber was not eight characters long, added fix for last part.
		// TODO: Need to look into why the VolumeSerialNumber was seven characters long
		part4 = string_format::pad_string(part4, 2);

		newHWID.append(part4);
		newHWID.append(part3);
		newHWID.append(part2);
		newHWID.append(part1);

		HWID = newHWID;
	}

	void Configuration::set_max_width(int16_t max_width)
	{
		MAXWIDTH = max_width;
	}

	void Configuration::set_max_height(int16_t max_height)
	{
		MAXHEIGHT = max_height;
	}

	std::string Configuration::get_macs()
	{
		return MACS;
	}

	std::string Configuration::get_hwid()
	{
		return HWID;
	}

	std::string Configuration::get_vol_serial_num()
	{
		return VolumeSerialNumber;
	}

	int16_t Configuration::get_max_width()
	{
		return MAXWIDTH;
	}

	int16_t Configuration::get_max_height()
	{
		return MAXHEIGHT;
	}

	bool Configuration::get_rightclicksell()
	{
		return rightclicksell;
	}

	void Configuration::set_rightclicksell(bool value)
	{
		rightclicksell = value;
	}

	bool Configuration::get_show_weekly()
	{
		return show_weekly;
	}

	void Configuration::set_show_weekly(bool value)
	{
		show_weekly = value;
	}

	bool Configuration::get_start_shown()
	{
		return start_shown;
	}

	void Configuration::set_start_shown(bool value)
	{
		start_shown = value;
	}

	uint8_t Configuration::get_worldid()
	{
		return worldid;
	}

	void Configuration::set_worldid(uint8_t id)
	{
		worldid = id;
	}

	uint8_t Configuration::get_channelid()
	{
		return channelid;
	}

	void Configuration::set_channelid(uint8_t id)
	{
		channelid = id;
	}

	bool Configuration::get_admin()
	{
		return admin;
	}

	void Configuration::set_admin(bool value)
	{
		admin = value;
	}

	bool Configuration::get_caps_lock_enabled()
	{
		return caps_lock_enabled;
	}

	void Configuration::set_caps_lock_enabled(int value)
	{
		caps_lock_enabled = value;
	}
}