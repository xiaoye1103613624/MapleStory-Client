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
#pragma once

#include "Template/Singleton.h"
#include "Template/TypeMap.h"
#include "Util/Misc.h"

// 前向声明：避免本头全局引入 Keyring.h（防止其 <unordered_map> 等包含顺序干扰 nlnx 头）。
namespace ms { namespace res { enum class KeyMode; } }

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cstdint>
#include <ctime>

namespace ms
{
	// 客户端配置中心：统一管理两个**同格式**的配置文件。
	//
	//   1. "Settings"（用户级）
	//      - 键名采用小写点分的层级命名，按下文 SAVE_ORDER 折叠成缩进结构写出
	//      - 构造时读取；**退出时并不会自动写回**（析构在优雅退出路径上跑不到，
	//        见 Configuration.cpp 的 ~Configuration 注释），需要即时生效的设置项
	//        必须显式调用 Configuration::get().save()
	//   2. "xy-config.yaml"（部署级，位于可执行文件同目录）
	//      - 与 Settings 同格式，**优先级更高**，用于固定服务器地址/数据后端等
	//      - 内容手工维护；运行时按 mtime 变化自动热重载（见 reload_if_changed）
	//
	// 取值优先级：xy-config.yaml > Settings > Entry 构造参数中的内置默认值。
	// 两个文件均以 UTF-8（无 BOM）保存，换行使用 LF。
	//
	// Manages the 'Settings' file which contains configurations set by user behavior
	class Configuration : public Singleton<Configuration>
	{
	public:
		// Add the settings which will be used and load them
		Configuration();
		// Save
		~Configuration();

		// Load all settings
		// If something is missing, set the default value.
		// Can be used for reloading
		void load();
		// Reload the deployment config (xy-config.yaml) if it changed on disk.
		void reload_if_changed();
		// Save the current settings 
		void save() const;
		// Get private member SHOW_FPS
		bool get_show_fps() const;
		// Get private member SHOW_PACKETS
		bool get_show_packets() const;
		// Get private member AUTO_LOGIN
		bool get_auto_login() const;
		// Get the world to login with
		uint8_t get_auto_world();
		// Get the channel to login with
		uint8_t get_auto_channel();
		// Get the account to login with
		std::string get_auto_acc();
		// Get the password to login with
		std::string get_auto_pass();
		// Get the pic to login with
		std::string get_auto_pic();
		// Get the character id to login with
		int32_t get_auto_cid();
		// Get private member TITLE
		std::string get_title() const;
		// Get private member VERSION
		std::string get_version() const;
		// 数据源后端，对应层级键 data.format，取值 "nx" / "wz" / "img"。
		// 部署环境可在 xy-config.yaml 中用同名键覆盖（优先级更高）。
		// Get the data backend format ("nx", "wz" or "img")
		std::string get_data_format() const;
		// 经典 .img 数据根目录，对应层级键 data.dir，
		// 仅在 data.format = "img" 时被 ImgFiles 使用。
		// Get the IMG data directory (used only when the format is "img")
		std::string get_data_dir() const;
		// 密钥来源模式（data.keymode）：auto / builtin / external。
		ms::res::KeyMode get_keymode() const;
		// 外部密钥文件路径（data.keyfile），空字符串表示关闭。
		std::string get_keyfile() const;
		// Buff 图标剩余时间文本是否开启（ui.buff_timer）。
		bool get_buff_timer() const;
		// 启动原生启动动画是否开启（boot.logo）。
		bool get_boot_logo() const;
		// 启动动画单帧时长，毫秒（boot.logo.frame_ms，取值已限幅到 10~1000）。
		uint16_t get_boot_logo_frame_ms() const;
		// 伤害公式 v2（damage.formula_v2）：启用原生取证补齐的魔法/无视防御/暴击区间分支。
		bool get_damage_formula_v2() const;
		// 伤害行 12 字节模式（damage.long_lines）：必须与 s9 服务端成对开启。
		bool get_damage_long_lines() const;
		// 单段伤害上限（damage.cap）：默认 2^53，与 double 精确整数上限一致。
		int64_t get_damage_cap() const;
		// 装备悬停时是否显示同部位已装备物品的对比（ui.equip_compare）。
		bool get_equip_compare() const;
		// UI 语言码（ui.language），默认 zh-CN；可被 xy-config 覆盖，支持热重载。
		std::string get_ui_language() const;
		// HTTP 补丁：是否启用（patch.enabled）。
		bool get_patch_enabled() const;
		// HTTP 补丁：manifest URL（patch.url）。
		std::string get_patch_url() const;
		// Get private member LoginMusic
		std::string get_login_music() const;
		// Get private member LoginMusicSEA
		std::string get_login_music_sea() const;
		// Get private member LoginMusicNewtro
		std::string get_login_music_newtro() const;
		// Get private member JOINLINK
		std::string get_joinlink() const;
		// Get private member WEBSITE
		std::string get_website() const;
		// Get private member FINDID
		std::string get_findid() const;
		// Get private member FINDPASS
		std::string get_findpass() const;
		// Get private member RESETPIC
		std::string get_resetpic() const;
		// Get private member CHARGENX
		std::string get_chargenx() const;
		// Set private member MACS
		void set_macs(char* macs);
		// Set private member HWID
		void set_hwid(char* hwid, char* volumeSerialNumber);
		// Set private member MAXWIDTH
		void set_max_width(int16_t max_width);
		// Set private member MAXHEIGHT
		void set_max_height(int16_t max_height);
		// Get private member MACS
		std::string get_macs();
		// Get private member HWID
		std::string get_hwid();
		// Get the Hard Drive Volume Serial Number
		std::string get_vol_serial_num();
		// Get the max width allowed
		int16_t get_max_width();
		// Get the max height allowed
		int16_t get_max_height();
		// Get the shop's "Right-click to sell item" boolean
		bool get_rightclicksell();
		// Set the shop's "Right-click to sell item" boolean
		void set_rightclicksell(bool value);
		// Whether to show the weekly maple star in Maple Chat
		bool get_show_weekly();
		// Set whether to show the weekly maple star in Maple Chat
		void set_show_weekly(bool value);
		// Whether to show the start screen
		bool get_start_shown();
		// Set whether to show the start screen
		void set_start_shown(bool value);
		// Get the character's selected world
		uint8_t get_worldid();
		// Set the character's selected world
		void set_worldid(uint8_t id);
		// Get the character's selected channel
		uint8_t get_channelid();
		// Set the character's selected channel
		void set_channelid(uint8_t id);
		// SERVERLIST 解出的「该世界频道数」。登录期 UIWorldSelect 才有这个值，进图后
		// 那个窗体已被移除，UIChannel 要按它排版，所以由 ServerlistHandler 就地登记。
		static void set_channel_total(int8_t wid, uint8_t count);
		// 该世界的频道数；本轮登录没收到过 SERVERLIST 时返回 0（调用方兜底）。
		static uint8_t get_channel_total(int8_t wid);
		// Check if the current account is an admin account
		bool get_admin();
		// Set whether the current account is an admin account
		void set_admin(bool value);
		// Check if the Caps Lock key is enabled
		bool get_caps_lock_enabled();
		// Set if the Caps Lock key is enabled
		void set_caps_lock_enabled(int value);

		// Base class for an entry in the settings file.
		//
		// name 为「小写点分」的层级键（如 "server.ip"），写盘时会被展开为缩进结构；
		// 同一个名字同时用于 Settings 与 xy-config.yaml，保证两个文件语义一致。
		class Entry
		{
		protected:
			Entry(const char* n, const char* v) : name(n), value(v) {}

			std::string name;
			std::string value;

		private:
			friend class Configuration;
		};

		// Setting which converts to a boolean
		class BoolEntry : public Entry
		{
		public:
			void save(bool b);
			bool load() const;

		protected:
			using Entry::Entry;
		};

		// Setting which uses the raw string
		class StringEntry : public Entry
		{
		public:
			void save(std::string str);
			std::string load() const;

		protected:
			using Entry::Entry;
		};

		// Setting which converts to a Point<int16_t>
		class PointEntry : public Entry
		{
		public:
			void save(Point<int16_t> p);
			Point<int16_t> load() const;

		protected:
			using Entry::Entry;
		};

		// Setting which converts to an integer type
		template <class T>
		class IntegerEntry : public Entry
		{
		public:
			void save(T num)
			{
				value = std::to_string(num);
			}

			T load() const
			{
				return string_conversion::or_zero<T>(value);
			}

		protected:
			using Entry::Entry;
		};

		// Setting which converts to a byte
		class ByteEntry : public IntegerEntry<uint8_t>
		{
		protected:
			using IntegerEntry::IntegerEntry;
		};

		// Setting which converts to a short
		class ShortEntry : public IntegerEntry<uint16_t>
		{
		protected:
			using IntegerEntry::IntegerEntry;
		};

		// Setting which converts to an int
		class IntEntry : public IntegerEntry<uint32_t>
		{
		protected:
			using IntegerEntry::IntegerEntry;
		};

		// Setting which converts to a long
		class LongEntry : public IntegerEntry<uint64_t>
		{
		protected:
			using IntegerEntry::IntegerEntry;
		};

	private:
		template <typename T>
		friend struct Setting;

		const char* FILENAME = "Settings";
		// Optional deployment configuration, read from the executable's
		// directory. Flat "Key: value" YAML. Values found here take priority
		// over FILENAME, so a deployment can pin the server address and the
		// data backend without touching the user's saved settings.
		const char* XYCONFIG_FILENAME = "xy-config.yaml";
		// 窗口标题的基础部分，运行时由 IO/Window 追加角色/职业/破功值/时间等段落
		// （见 Window::build_title()）。
		const char* TITLE = "XiaoYE MS v0.0.1";
		const char* VERSION = "0.83";
		const char* LoginMusic = "BgmUI.img/Title";
		const char* LoginMusicSEA = "BgmGL.img/OldMaple";
		const char* LoginMusicNewtro = "BgmEvent2.img/Newtro_Login";
		const char* JOINLINK = "https://www.nexon.com/account/en/create";
		const char* WEBSITE = "http://maplestory.nexon.net/";
		const char* FINDID = "https://www.nexon.com/account/en/login";
		const char* FINDPASS = "https://www.nexon.com/account/en/reset-password";
		const char* RESETPIC = "https://www.nexon.com/account/en/login";
		const char* CHARGENX = "https://billing.nexon.net/PurchaseNX";
		const bool SHOW_FPS = false;
		const bool SHOW_PACKETS = false;
		const bool AUTO_LOGIN = false;
		const uint8_t auto_world = 0;
		const uint8_t auto_channel = 0;
		const std::string auto_acc = "";
		const std::string auto_pass = "";
		const std::string auto_pic = "";
		const int32_t auto_cid = 0;
		bool rightclicksell = false;
		bool show_weekly = true;
		bool start_shown = true;
		std::string MACS;
		std::string HWID;
		int16_t MAXWIDTH;
		int16_t MAXHEIGHT;
		std::string VolumeSerialNumber;
		uint8_t worldid;
		uint8_t channelid;
		// SERVERLIST 的实测频道数，按世界 id 记；只在内存，不落 Settings。
		static std::unordered_map<int8_t, uint8_t>& channel_totals();
		bool admin;
		bool caps_lock_enabled;
		TypeMap<Entry> settings;

		// 部署配置（xy-config.yaml）的嵌套键值（如 "login.urls.joinlink"）。
		// 运行时按文件修改时间自动重载，供登录面板跳转网址等热更新。
		std::unordered_map<std::string, std::string> xysettings;
		std::time_t xy_last_write = 0;

		// 读取 xy-config.yaml 的修改时间（mtime）；文件不存在返回 false。
		bool get_xy_mtime(std::time_t& out) const;
		//
		// 配置文件解析器（Settings 与 xy-config.yaml 共用，两者格式完全一致）：
		//   - 缩进式键用 '.' 拼接成层级名字（如 server -> server.ip）
		//   - 顶层扁平键（如 "width: 1280"）同样接受，但**不建议**，新写法一律分组
		//   - 注释以 '#' 开头；值为空时，只有当该键属于 leaf_keys 才当作"空值设置项"，
		//     否则视为分组节点（这样 "server:" 与 "login.account:" 才不会混淆）
		//   - key/value 分隔符取"后跟空白或位于行尾的冒号"，避免把 URL 的 "://"
		//     误判为分隔符
		//
		// leaf_keys：所有已知配置项的名字集合，由调用方从 settings 收集后传入。
		std::unordered_map<std::string, std::string> parse_config_file(
			const char* filename, const std::unordered_set<std::string>& leaf_keys) const;
		// 重新解析 xy-config.yaml 并更新记录的文件修改时间。
		void reload_xy_config();
		// 按层级键名取当前取值；不存在时返回 nullptr。
		const std::string* find_value(const std::string& key) const;
		// 读取嵌套/扁平 URL 配置，缺失时回退到硬编码默认值。
		std::string get_url(const std::string& nested, const std::string& flat, const std::string& fallback) const;
	};

	// 客户端配置项总览
	// -------------------------------------------------------------------------
	// 键名采用「小写点分」层级命名（如 server.ip），写盘时按点分层展开为缩进结构：
	//
	//   server:
	//     ip: 127.0.0.1
	//     port: 8484
	//
	// 同一个键名既用于 Settings（用户级，客户端退出时自动写回），
	// 也用于同目录下的 xy-config.yaml（部署级，优先级更高，手写维护）。
	// 改名或新增配置项时，请同步维护 Configuration.cpp 中的 SAVE_ORDER，
	// 否则该项不会被写出到 Settings。

	// IP Address which the client will connect to
	// 登录服 IP 地址。部署环境通常通过 xy-config.yaml 覆盖此值。
	struct ServerIP : public Configuration::StringEntry
	{
		ServerIP() : StringEntry("server.ip", "127.0.0.1") {}
	};

	// Port which the client will connect to
	// 登录服端口，需与服务端 gms.service.login-port 一致。
	struct ServerPort : public Configuration::StringEntry
	{
		ServerPort() : StringEntry("server.port", "8484") {}
	};

	// Data backend selection.
	// 数据源后端选择，对应层级键 data.format：
	//   "img"  = 独立 .img 目录（生产默认）
	//   "auto" = 自动识别：按候选路径链自省，挑选命中分数最高者
	//   "wz"   = 官方 .wz 归档（运行时直读，不落盘）
	//   "nx"   = NoLifeNx 归档（文件映射，惰性读取）
	// 三者共用同一套 nl::node 读取接口，切换不需要重新编译。
	// 显式指定某一格式时，定位器仍会在候选路径链中寻找它的位置。
	// 生产默认 img（萧曳 083 平台化决策）；开发可用 auto / wz / nx。
	struct DataFormat : public Configuration::StringEntry
	{
		DataFormat() : StringEntry("data.format", "img") {}
	};

	// Directory containing the classic standalone .img data (one subdirectory
	// per standard root). Used only when data.format is "img".
	// 经典 .img 数据根目录（其下按标准根分子目录），仅 data.format=img 时生效。
	struct DataDir : public Configuration::StringEntry
	{
		DataDir() : StringEntry("data.dir", "Data") {}
	};

	// Key source mode (data.keymode): "auto" / "builtin" / "external", default "auto".
	// 密钥来源模式：auto = 内置三表 + 归档自带 key + 外部文件；builtin = 仅内置 + 自带；
	// external = 仅外部密钥文件。详见 Util/Resource/Keyring.h。
	struct KeyMode : public Configuration::StringEntry
	{
		KeyMode() : StringEntry("data.keymode", "auto") {}
	};

	// External key file path (data.keyfile). Empty string means disabled (default).
	// 外部密钥文件路径；空字符串表示关闭（默认）。明文不入库，仅驻留内存。
	struct KeyFile : public Configuration::StringEntry
	{
		KeyFile() : StringEntry("data.keyfile", "") {}
	};

	// HTTP 补丁总开关（patch.enabled）。url 为空时仍会跳过实际同步。
	struct PatchEnabled : public Configuration::BoolEntry
	{
		PatchEnabled() : BoolEntry("patch.enabled", "true") {}
	};

	// manifest 完整 URL（patch.url）；空字符串表示不拉补丁。
	struct PatchUrl : public Configuration::StringEntry
	{
		PatchUrl() : StringEntry("patch.url", "") {}
	};

	// 补丁频道（patch.channel），与 manifest.channel 比对。
	struct PatchChannel : public Configuration::StringEntry
	{
		PatchChannel() : StringEntry("patch.channel", "stable") {}
	};

	// 单次 HTTP 超时毫秒（patch.timeout_ms）。
	struct PatchTimeoutMs : public Configuration::IntEntry
	{
		PatchTimeoutMs() : IntEntry("patch.timeout_ms", "15000") {}
	};

	// 失败重试次数（patch.retries）。
	struct PatchRetries : public Configuration::IntEntry
	{
		PatchRetries() : IntEntry("patch.retries", "2") {}
	};

	// 补丁失败是否允许继续进游戏（patch.allow_skip）。
	struct PatchAllowSkip : public Configuration::BoolEntry
	{
		PatchAllowSkip() : BoolEntry("patch.allow_skip", "true") {}
	};

	// Whether to start in full screen mode
	// 是否以全屏启动。false 时使用下文的 window.width / window.height。
	struct Fullscreen : public Configuration::BoolEntry
	{
		Fullscreen() : BoolEntry("window.fullscreen", "false") {}
	};

	// The width of the screen
	// 窗口/视口宽度。读取后会同步到运行时常量 Constants，影响 UI 布局基准。
	struct Width : public Configuration::ShortEntry
	{
		Width() : ShortEntry("window.width", "1280") {}
	};

	// The height of the screen
	// 窗口/视口高度。与 window.width 一同同步到 Constants。
	struct Height : public Configuration::ShortEntry
	{
		Height() : ShortEntry("window.height", "720") {}
	};

	// 多分辨率：登录态使用上面的 window.width/height（恒 1280×720），
	// 进入游戏后按下面三项切换。game_tier 合法时以 Util/ResolutionTiers.h 的档位表为准，
	// 否则用 game_width/game_height；两者缺键时回落登录尺寸，保证旧配置零回归。
	// 255 = TIER_NONE（未显式选档，只按宽高走）。
	struct GameWidth : public Configuration::ShortEntry
	{
		GameWidth() : ShortEntry("window.game_width", "1280") {}
	};

	struct GameHeight : public Configuration::ShortEntry
	{
		GameHeight() : ShortEntry("window.game_height", "720") {}
	};

	struct GameTier : public Configuration::ByteEntry
	{
		GameTier() : ByteEntry("window.game_tier", "255") {}
	};

	// Whether to use VSync
	// 是否开启垂直同步，关闭后可能出现画面撕裂。
	struct VSync : public Configuration::BoolEntry
	{
		VSync() : BoolEntry("window.vsync", "true") {}
	};

	// 启动原生启动动画（Logo.img/Nexon 片头）开关，对应 boot.logo，默认开。
	// 行为对齐经典客户端开头动画：黑底 + 居中 Logo 逐帧播放，任意按键/点击可跳过。
	// 关闭后退回加载动画（spinner），不影响启动流程。
	struct BootLogo : public Configuration::BoolEntry
	{
		BootLogo() : BoolEntry("boot.logo", "true") {}
	};

	// 启动动画单帧时长（毫秒），对应 boot.logo.frame_ms。
	// Logo.img/Nexon 的帧不带 delay 元数据（137 帧），经典客户端的节奏由代码写死；
	// 这里用可配置的固定步长近似：默认 40ms（整段约 5.5s）。
	struct BootLogoFrameMs : public Configuration::IntEntry
	{
		BootLogoFrameMs() : IntEntry("boot.logo.frame_ms", "40") {}
	};

	// Whether to overlay the remaining time on buff icons
	// buff 图标上的剩余时间文本（1 秒粒度，对齐 BeiDou V17.7 的 BuffTimer）。
	// 关闭后仅保留图标与冷却遮罩，不再绘制数字。
	struct BuffTimer : public Configuration::BoolEntry
	{
		BuffTimer() : BoolEntry("ui.buff_timer", "true") {}
	};

	// Whether to show the equipped item of the same slot next to the hovered one
	// 装备悬停对比（ui.equip_compare）：在物品栏/商店悬停装备时，右侧显示同部位已装备物品。
	// 关闭后只显示悬停物品自身的 tooltip。
	struct EquipCompare : public Configuration::BoolEntry
	{
		EquipCompare() : BoolEntry("ui.equip_compare", "true") {}
	};

	// UI language code (ui.language): zh-CN / en-US …
	// 界面语言；有中英双套 WZ 时按此切换，否则用中文 UI + locale 文案。默认中文。
	struct UiLanguage : public Configuration::StringEntry
	{
		UiLanguage() : StringEntry("ui.language", "zh-CN") {}
	};

	// The normal font which will be used
	// 常规字体文件路径（相对客户端工作目录，UTF-8 路径可直接填写）。
	struct FontPathNormal : public Configuration::StringEntry
	{
		// 中文客户端：默认字体须含 CJK 字形，否则运行时中文（职业名/对话/聊天）
		// 经 FreeType 渲染为空（纯拉丁 Arial 无中文字形）。
		// R28-B5：由 SimHei（微软系统字体，公开仓库有授权风险）换成思源宋体
		// Source Han Serif SC Regular（SIL OFL 1.11，随包 LICENSE-OFL.txt）。
		// 装机上残留的旧路径由 Configuration::load 的字体兜底处理（见 evidence/w8-font-swap）。
		FontPathNormal() : StringEntry("font.path.normal", "fonts/SourceHanSerifSC/SourceHanSerifSC-Regular.otf") {}
	};

	// The bold font which will be used
	// 粗体字体文件路径，缺失时回退到常规字体渲染。
	struct FontPathBold : public Configuration::StringEntry
	{
		FontPathBold() : StringEntry("font.path.bold", "fonts/SourceHanSerifSC/SourceHanSerifSC-Bold.otf") {}
	};

	// Music Volume
	// Number from 0 to 100
	// 背景音乐音量 0-100。
	struct BGMVolume : public Configuration::ByteEntry
	{
		BGMVolume() : ByteEntry("audio.bgm.volume", "50") {}
	};

	// Sound Volume
	// Number from 0 to 100
	// 音效音量 0-100。
	struct SFXVolume : public Configuration::ByteEntry
	{
		SFXVolume() : ByteEntry("audio.sfx.volume", "50") {}
	};

	// Whether to save the last used account name
	// 是否记住上次登录的账号名（写入 login.account）。
	struct SaveLogin : public Configuration::BoolEntry
	{
		SaveLogin() : BoolEntry("login.save", "false") {}
	};

	// The last used account name
	// 上次使用的账号名，允许为空（空值同样会被正确读回）。
	struct DefaultAccount : public Configuration::StringEntry
	{
		DefaultAccount() : StringEntry("login.account", "") {}
	};

	// The last used world
	// 上次选择的区服序号（世界）。
	struct DefaultWorld : public Configuration::ByteEntry
	{
		DefaultWorld() : ByteEntry("login.world", "0") {}
	};

	// The last used channel
	// 上次选择的频道序号。
	struct DefaultChannel : public Configuration::ByteEntry
	{
		DefaultChannel() : ByteEntry("login.channel", "0") {}
	};

	// The last used region
	// 上次选择的地区序号。
	struct DefaultRegion : public Configuration::ByteEntry
	{
		DefaultRegion() : ByteEntry("login.region", "5") {}
	};

	// The last used character
	// 上次选择的角色槽位序号。
	struct DefaultCharacter : public Configuration::ByteEntry
	{
		DefaultCharacter() : ByteEntry("login.character", "0") {}
	};

	// 是否启用启动自动登录（opt-in，读取 login.auto）。
	// 供自动化/调试直连：开启后客户端启动即用保存的账号+login.pass 自动发包登录，
	// 跳过手动输入。默认 false；Release 构建同样生效（取代原先 NDEBUG 强制关闭的行为）。
	struct AutoLogin : public Configuration::BoolEntry
	{
		AutoLogin() : BoolEntry("login.auto", "false") {}
	};

	// 自动登录使用的密码（login.pass，opt-in 才生效）。
	// 明文仅驻留配置文件与内存；不使用自动登录时留空即可。
	struct AutoPass : public Configuration::StringEntry
	{
		AutoPass() : StringEntry("login.pass", "") {}
	};

	// Whether the UIChatBar is expanded or not
	// 聊天栏是否处于展开状态。
	struct ChatViewMax : public Configuration::BoolEntry
	{
		ChatViewMax() : BoolEntry("chat.max", "false") {}
	};

	// The user defined width for UIChatBar
	// 聊天栏用户自定义宽度。
	struct ChatViewX : public Configuration::ShortEntry
	{
		ChatViewX() : ShortEntry("chat.x", "0") {}
	};

	// The user defined height for UIChatBar
	// 聊天栏用户自定义高度。
	struct ChatViewY : public Configuration::ShortEntry
	{
		ChatViewY() : ShortEntry("chat.y", "0") {}
	};

	// The default position of UIStatsInfo
	// 角色属性面板默认位置。
	struct PosSTATS : public Configuration::PointEntry
	{
		PosSTATS() : PointEntry("ui.position.stats", "(72,72)") {}
	};

	// The default position of UIEquipInventory
	// 装备栏默认位置。
	struct PosEQINV : public Configuration::PointEntry
	{
		PosEQINV() : PointEntry("ui.position.eqinv", "(250,160)") {}
	};

	// The default position of UIItemInventory
	// 消耗/其他栏默认位置。
	struct PosINV : public Configuration::PointEntry
	{
		PosINV() : PointEntry("ui.position.inv", "(24,24)") {}
	};

	// The default position of UISkillBook
	// 技能面板默认位置。
	struct PosSKILL : public Configuration::PointEntry
	{
		PosSKILL() : PointEntry("ui.position.skill", "(96,96)") {}
	};

	// The default position of UIQuestLog
	// 任务面板默认位置。
	struct PosQUEST : public Configuration::PointEntry
	{
		PosQUEST() : PointEntry("ui.position.quest", "(300,160)") {}
	};

	// The default position of UIWorldMap
	// 世界地图默认位置。
	struct PosMAP : public Configuration::PointEntry
	{
		// 大地图窗拼框固定 654x521（见 evidence/w11-map）；1280x720 下水平居中、垂直偏上。
		PosMAP() : PointEntry("ui.position.map", "(313,64)") {}
	};

	// The default position of UIUserList
	// 用户列表默认位置。
	struct PosUSERLIST : public Configuration::PointEntry
	{
		PosUSERLIST() : PointEntry("ui.position.userlist", "(104, 104)") {}
	};

	// The default position of UIChatBar
	// 聊天栏默认位置。
	struct PosCHAT : public Configuration::PointEntry
	{
		PosCHAT() : PointEntry("ui.position.chat", "(0, 572)") {}
	};

	// The default position of UIMiniMap
	// 小地图默认位置。
	struct PosMINIMAP : public Configuration::PointEntry
	{
		// 参考端（正确客户端截图/进入游戏页.png）小地图九切片顶边实测落在 y=24：
		// y≤23 是天空，y=24 起为整幅白边。顶部 0..22 是 ScrollingNotice 的全宽
		// 黑底公告条（ColorBox(viewwidth, 23, BLACK, 0.535f)，UI::draw 里画在
		// state->draw 之后，压在任何 UI 上面）。旧值 (5, 5) 会让收缩条整条落进
		// 公告条，实测其 221 灰被压成 103（×0.465）。x 仍留 5px 安全边距。
		PosMINIMAP() : PointEntry("ui.position.minimap", "(5, 24)") {}
	};

	// The default position of UIShop
	// NPC 商店默认位置。
	struct PosSHOP : public Configuration::PointEntry
	{
		PosSHOP() : PointEntry("ui.position.shop", "(146, 48)") {}
	};

	// The default position of UINotice
	// 公告面板默认位置。
	struct PosNOTICE : public Configuration::PointEntry
	{
		PosNOTICE() : PointEntry("ui.position.notice", "(400, 285)") {}
	};

	// The default position of UIChat and UIRank
	// 枫叶聊天/排行榜默认位置。
	struct PosMAPLECHAT : public Configuration::PointEntry
	{
		PosMAPLECHAT() : PointEntry("ui.position.maplechat", "(50, 46)") {}
	};

	// The default position of UIChannel
	// 频道选择面板默认位置。
	struct PosCHANNEL : public Configuration::PointEntry
	{
		PosCHANNEL() : PointEntry("ui.position.channel", "(215, 100)") {}
	};

	// The default position of UIJoypad
	// 手柄面板默认位置。
	struct PosJOYPAD : public Configuration::PointEntry
	{
		PosJOYPAD() : PointEntry("ui.position.joypad", "(312, 134)") {}
	};

	// The default position of UIEvent
	// 活动面板默认位置。
	struct PosEVENT : public Configuration::PointEntry
	{
		PosEVENT() : PointEntry("ui.position.event", "(99, 100)") {}
	};

	// The default position of UIDailyGift
	// 每日礼物面板默认位置。
	struct PosDAILYGIFT : public Configuration::PointEntry
	{
		PosDAILYGIFT() : PointEntry("ui.position.dailygift", "(99, 80)") {}
	};

	// The default position of UIKeyConfig
	// 按键设置面板默认位置。
	struct PosKEYCONFIG : public Configuration::PointEntry
	{
		PosKEYCONFIG() : PointEntry("ui.position.keyconfig", "(144, 144)") {}
	};

	// The default position of UIOptionMenu
	// 系统选项面板默认位置。
	// 历史 bug 说明：该项此前误用 "PosUSERLIST" 作为键名，与用户列表位置撞名，
	// 导致生成的 Settings 中 ui.position.userlist 出现两次并互相覆盖。
	struct PosOPTIONMENU : public Configuration::PointEntry
	{
		PosOPTIONMENU() : PointEntry("ui.position.optionmenu", "(170, -1)") {}
	};

	// The default position of UIGameOpt
	// v83「游戏设置」窗默认位置。原生取证件是**窗口裁图**（无屏幕上下文），
	// 所以这个默认值不是 oracle 读数；窗体可拖拽并按 ui.position.* 落盘。
	struct PosGAMEOPT : public Configuration::PointEntry
	{
		PosGAMEOPT() : PointEntry("ui.position.gameopt", "(170, 24)") {}
	};

	// The default position of UISysOpt
	// v83「系统设置」窗默认位置，同上：非 oracle 读数，可拖拽落盘。
	struct PosSYSOPT : public Configuration::PointEntry
	{
		PosSYSOPT() : PointEntry("ui.position.sysopt", "(330, 24)") {}
	};

	// Game-option accept mask (ui.gameopt.mask)
	// 「游戏设置」12 行的同意/拒绝位图：bit i = 第 i 行（画布自上而下），
	// 1 = 同意。默认 0xFFF（全同意）—— 原生截帧里第 7 行未勾是**那位玩家当时的
	// 状态**，不是出厂默认，故不照抄。消费方见 UIGameOpt.h 顶部说明。
	struct GameOptMask : public Configuration::ShortEntry
	{
		GameOptMask() : ShortEntry("ui.gameopt.mask", "4095") {}
	};

	// Screen quality (ui.sysopt.quality)
	// 「画质调整」滑条 0-100。当前无消费方（本包无画质分级管线），仅落盘。
	// 默认值取原生截帧读数（滑钮 x=221 → 98%），这样首帧能直接与 oracle 逐格比对。
	struct SysOptQuality : public Configuration::ByteEntry
	{
		SysOptQuality() : ByteEntry("ui.sysopt.quality", "98") {}
	};

	// Mouse cursor speed (ui.sysopt.mouse_speed)
	// 「鼠标速度调整」滑条 0-100。当前无消费方，仅落盘。默认同上（滑钮 x=164 → 52%）。
	struct SysOptMouseSpeed : public Configuration::ByteEntry
	{
		SysOptMouseSpeed() : ByteEntry("ui.sysopt.mouse_speed", "52") {}
	};

	// HP warning threshold percent (ui.sysopt.hp_warn)
	// 「HP 警报」阈值百分比；左端「没有」= 0（不报警）。当前无消费方，仅落盘。
	struct SysOptHpWarn : public Configuration::ByteEntry
	{
		SysOptHpWarn() : ByteEntry("ui.sysopt.hp_warn", "52") {}
	};

	// MP warning threshold percent (ui.sysopt.mp_warn)
	// 「MP 警报」阈值百分比，同上。
	struct SysOptMpWarn : public Configuration::ByteEntry
	{
		SysOptMpWarn() : ByteEntry("ui.sysopt.mp_warn", "52") {}
	};

	// Allow skill effects (ui.sysopt.skill_effects)
	// 「画面效果 · 技能使用时允许画面效果」。当前无消费方，仅落盘。
	struct SysOptSkillEffects : public Configuration::BoolEntry
	{
		SysOptSkillEffects() : BoolEntry("ui.sysopt.skill_effects", "true") {}
	};

	// Windowed mode (ui.sysopt.window_mode)
	// 「画面模式 · 窗口化模式」= window.fullscreen 的镜像面：勾上 = 窗口化。
	// 真源是 window.fullscreen（启动真源在 Window::init，切换在 set_fullscreen/
	// toggle_fullscreen），本键只在启动时被单向回灌、ALT+回车时被镜像，
	// 因此默认值与装机窗口化运行保持一致（true）。
	// 消费方：UIOptionMenu 的 UISysOpt（显示与「动过勾才切」门禁）。
	struct SysOptWindowMode : public Configuration::BoolEntry
	{
		SysOptWindowMode() : BoolEntry("ui.sysopt.window_mode", "true") {}
	};

	// ALT+ENTER window-mode switching permission (ui.sysopt.alt_enter)
	// 「画面模式」第二格：允许用 ALT+回车在窗口化/全屏之间随时切换。
	struct SysOptAltEnter : public Configuration::BoolEntry
	{
		SysOptAltEnter() : BoolEntry("ui.sysopt.alt_enter", "false") {}
	};

	// Mob info display mode (ui.sysopt.mob_info)
	// 「怪物情报」下拉序号 0..3：不显示 / 只显示名字 / 只显示HP / 名字和HP都显示
	// （ijl15.dll@0x293ec 四条，与原生截帧 + 注册表 soMobInfo=3 互证，B4-6）。
	// 消费方：Gameplay/MapleMap/Mob.cpp 的 MOB_INFO_TIER。默认 3 = 原生截帧当前项。
	struct SysOptMobInfo : public Configuration::ByteEntry
	{
		SysOptMobInfo() : ByteEntry("ui.sysopt.mob_info", "3") {}
	};

	// Screenshot save location (ui.sysopt.screenshot_path)
	// 「截图保存位置」下拉序号。本包只有默认目录，无第二选项，仅落盘。
	struct SysOptScreenshotPath : public Configuration::ByteEntry
	{
		SysOptScreenshotPath() : ByteEntry("ui.sysopt.screenshot_path", "0") {}
	};

	// The default position of UICharInfo
	// 角色信息面板默认位置。
	struct PosCHARINFO : public Configuration::PointEntry
	{
		PosCHARINFO() : PointEntry("ui.position.charinfo", "(264, 264)") {}
	};

	// Messenger（聊天室/信使）窗默认位置。
	struct PosMESSENGER : public Configuration::PointEntry
	{
		PosMESSENGER() : PointEntry("ui.position.messenger", "(500, 80)") {}
	};

	// PartySearch（组队搜索）窗默认位置。
	struct PosPARTYSEARCH : public Configuration::PointEntry
	{
		PosPARTYSEARCH() : PointEntry("ui.position.partysearch", "(260, 60)") {}
	};

	struct PosDELIVERY : public Configuration::PointEntry
	{
		PosDELIVERY() : PointEntry("ui.position.delivery", "(280, 80)") {}
	};

	struct PosSTOREBANK : public Configuration::PointEntry
	{
		PosSTOREBANK() : PointEntry("ui.position.storebank", "(300, 100)") {}
	};

	struct PosVICIOUSHAMMER : public Configuration::PointEntry
	{
		PosVICIOUSHAMMER() : PointEntry("ui.position.vicioushammer", "(320, 140)") {}
	};

	struct PosVEGASPELL : public Configuration::PointEntry
	{
		PosVEGASPELL() : PointEntry("ui.position.vegaspell", "(320, 120)") {}
	};

	struct PosCASHGACHAPON : public Configuration::PointEntry
	{
		PosCASHGACHAPON() : PointEntry("ui.position.cashgachapon", "(340, 160)") {}
	};

	struct PosREMOTEGACHAPON : public Configuration::PointEntry
	{
		PosREMOTEGACHAPON() : PointEntry("ui.position.remotegachapon", "(300, 120)") {}
	};

	struct PosFAMILY : public Configuration::PointEntry
	{
		PosFAMILY() : PointEntry("ui.position.family", "(280, 60)") {}
	};

	struct PosFAMILYTREE : public Configuration::PointEntry
	{
		PosFAMILYTREE() : PointEntry("ui.position.familytree", "(120, 80)") {}
	};

	struct PosMEMO : public Configuration::PointEntry
	{
		PosMEMO() : PointEntry("ui.position.memo", "(280, 100)") {}
	};

	struct PosPARTYRACE : public Configuration::PointEntry
	{
		PosPARTYRACE() : PointEntry("ui.position.partyrace", "(20, 80)") {}
	};

	// The default type of UIMiniMap
	// 小地图默认类型（2 = 常规样式）。
	struct MiniMapType : public Configuration::ByteEntry
	{
		MiniMapType() : ByteEntry("minimap.type", "2") {}
	};

	// Whether to use a simple version of UIMiniMap
	// 是否使用小地图简约模式。
	struct MiniMapSimpleMode : public Configuration::BoolEntry
	{
		MiniMapSimpleMode() : BoolEntry("minimap.simple", "false") {}
	};

	// Whether to use default helpers for UIMiniMap
	// 小地图是否绘制默认的标志物（NPC/传送点等）。
	struct MiniMapDefaultHelpers : public Configuration::BoolEntry
	{
		MiniMapDefaultHelpers() : BoolEntry("minimap.default_helpers", "false") {}
	};

	// 伤害公式补齐开关（damage.formula_v2，默认 false = 保持原 Fyms085 公式）。
	// 开启后 Skill/Mob/CharStats 走原生取证补齐的魔法攻击、无视防御、暴击区间、
	// PAD/MAD 1999 钳制与玩家受伤公式；关闭时伤害结果与改造前一致，便于回滚。
	struct DamageFormulaV2 : public Configuration::BoolEntry
	{
		DamageFormulaV2() : BoolEntry("damage.formula_v2", "false") {}
	};

	// 12 字节/行伤害封包开关（damage.long_lines，默认 false = legacy 4 字节/行）。
	// 必须与 s9 服务端 damage.long-lines 同时开启/关闭；失步会导致整包读错位。
	struct DamageLongLines : public Configuration::BoolEntry
	{
		DamageLongLines() : BoolEntry("damage.long_lines", "false") {}
	};

	// 单段伤害上限（damage.cap，默认 9007199254740992 = 2^53）。
	// 超过该值的整数无法被 double 公式精确表示；破功值下发时会取两者较小值。
	// 不用 LongEntry：string_conversion::or_default 内部用 std::stoi，
	// 会把 2^53 当越界异常吞成 0；这里用 StringEntry + std::stoull 自己解析。
	struct DamageCap : public Configuration::StringEntry
	{
		DamageCap() : StringEntry("damage.cap", "9007199254740992") {}
	};

	// 封包调试转储（debug.packet_dump，默认 false）。
	// 开启后把收/发的每个明文封包按参考端 PacketDump 格式（前缀 "PacketDump : "
	// + 每字节 "%02x " 小写十六进制；格式重建自 BeiDou.exe 0x6EC7C0：
	// push "PacketDump : "@0xAFCC14 + 循环 "%02x "@0xAFCC0C）
	// 追加写入 exe 同目录 packet_dump.log，供联调时与服务端日志逐字节核对。
	struct PacketDumpLog : public Configuration::BoolEntry
	{
		PacketDumpLog() : BoolEntry("debug.packet_dump", "false") {}
	};

	// 分段耗时文件日志（debug.perf_log，默认 false；Wave 0.1 测量仪器）。
	// Release 下 LOG() 是 no-op（MapleStory.h _DEBUG 门控），本键是 [BOOT]/
	// [SETFIELD] 毫秒分段唯一的落盘通道（Util/PerfLog.h → boot_perf.log）。
	// 基线采集期临时开；采集完成按门禁保持默认关。
	struct PerfLogEnabled : public Configuration::BoolEntry
	{
		PerfLogEnabled() : BoolEntry("debug.perf_log", "false") {}
	};

	// 真机冒烟自驱动（debug.gm_smoke，默认 false；仅 GM 号自测用，生产必关）。
	// 开启后进图 3s 自动发一次 ADMIN_COMMAND(0x80) 的 0x17 刷怪，用来在**无输入
	// 注入手段**的环境里验证 GM 包与"怪物相关"链路（触怪仇恨 0xBD / 手雷 0x6D /
	// 一键 AP 0x58 都需要场上先有怪）。参数两条独立键，见下。
	struct GmSmokeEnabled : public Configuration::BoolEntry
	{
		GmSmokeEnabled() : BoolEntry("debug.gm_smoke", "false") {}
	};

	// 自驱动刷的怪 id（默认 100100 = 绿水灵，新手图必在 Mob.nx 内）。
	struct GmSmokeMob : public Configuration::IntegerEntry<int32_t>
	{
		GmSmokeMob() : IntegerEntry("debug.gm_smoke_mob", "100100") {}
	};

	// 自驱动刷怪数量（默认 3；运行时钳 1..50）。
	struct GmSmokeCount : public Configuration::IntegerEntry<int32_t>
	{
		GmSmokeCount() : IntegerEntry("debug.gm_smoke_count", "3") {}
	};

	// 本地点券标签缓存（服务端字段就绪前 SoftKey/Point 按钮写入）
	struct MaplePointsLocal : public Configuration::LongEntry
	{
		MaplePointsLocal() : LongEntry("ui.maplepoints.local", "0") {}
	};

	// 宠物自动药：s9 AUTO_HP_POT(336) / AUTO_MP_POT(337) 下发的 itemId
	struct AutoHpPotItem : public Configuration::IntEntry
	{
		AutoHpPotItem() : IntEntry("ui.autopot.hp_item", "0") {}
	};

	struct AutoMpPotItem : public Configuration::IntEntry
	{
		AutoMpPotItem() : IntEntry("ui.autopot.mp_item", "0") {}
	};

	template <typename T>
	// Can be used to access settings
	struct Setting
	{
		// Access a setting
		static T& get()
		{
			static_assert(std::is_base_of<Configuration::Entry, T>::value, "template parameter T for Setting must inherit from Configuration::Entry.");

			auto* entry = Configuration::get().settings.get<T>();

			if (entry)
			{
				return *entry;
			}
			else
			{
				static T defaultentry;
				return defaultentry;
			}
		}
	};
}