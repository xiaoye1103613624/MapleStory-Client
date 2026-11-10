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

#include "Components/Button.h"
#include "Components/Icon.h"
#include "LocaleBindings.h"

#include "../Graphics/Sprite.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	class Text;
	struct OutlinedText;

	// Base class for all types of user interfaces on screen.
	class UIElement
	{
	public:
		using UPtr = std::unique_ptr<UIElement>;

		enum Type
		{
			NONE,
			START,
			LOGIN,
			TOS,
			GENDER,
			WORLDSELECT,
			REGION,
			/// 原生「查看推荐世界」弹窗（Angel 083 CUIRecommendWorld，BtViewChoice→OnButtonClicked
			/// 1000/1001/1002 = MoveWorld(±1)/SelectWorld）。按 083 OnCreate @0x62b451 用
			/// Login.img/WorldSelect/alert/backgrd 真背景与三颗真位图按钮（.nx 实测 alert 子树完整）。
			RECOMMENDWORLD,
			CHARSELECT,
			VIEWALLCHAR,
			LOGINWAIT,
			RACESELECT,
			CLASSCREATION,
			SOFTKEYBOARD,
			LOGINNOTICE,
			LOGINNOTICE_CONFIRM,
			STATUSMESSENGER,
			MESSENGER,
			PARTYSEARCH,
			DELIVERY,
			STOREBANK,
			VICIOUSHAMMER,
			VEGASPELL,
			CASHGACHAPON,
			REMOTEGACHAPON,
			FAMILY,
			FAMILYTREE,
			MEMO,
			ENERGYBAR,
			COMBOCOUNTER,
			BOSSHP,
			CLOCK,
			MTS,
			PARTYRACE,
			WEDDING,
			MONSTERCARNIVAL,
			RPSGAME,
			ARIANTMATCH,
			NEWYEARSCARD,
			MINIGAME,
			TREASUREBOX,
			ADMIN,
			PLANT,
			RAISE,
			MAPLETV,
			ADMINCLAIM,
			GM_COMMANDLIST,
			CLASSMATCH,
			ARTMEGAPHONE,
			AVATARMEGAPHONE,
			REPORT,
			COUPON,
			CLAIM,
			INCUBATOR,
			RADIO,
			KARMASCISSORS,
			ENCHANTSKILL,
			ITEMPROTECTOR,
			TELEPORTUI,
			SPEEDQUIZ,
			INITIALQUIZ,
			/// NPC AskPet / AskPetAll（msgtype 9/10，IDA 7474A2 / 74775C）
			ASKPET,
			/// 维度镜 SlideMenu（msgtype 14，IDA 76B3F0）
			DIMENSIONALMIRROR,
			RANKINGUI,
			ITEMMEGAPHONE,
			COUNSEL,
			MURUENGRAID,
			CREATECYGNUS,
			MEGAPHONEBASIC,
			PERSONALSHOP,
			MEMBERSHOP,
			TRADINGROOM,
			ENTRUSTEDSHOP,
			CASHTRADINGROOM,
			ITEMSEARCH,
			ARANSKILLGUIDE,
			TRANSFORMUI,
			MONSTERKILLING,
			MAPLEEVENT,
			SKILLEX,
			BOOKUI,
			QUESTALARM,
			MATEMESSAGE,
			BETAEDITION,
			GMBOARD,
			SHORTCUTUI,
			SKILLMACROEX,
			CHARACTERCLONE,
			CANCELREQUESTS,
			RESETUI,
			ITEMMOVE,
			FADEYESNO,
			CONTEXTMENU,
			EVENTWINDOW,
			SLIDEMENU,
			UTILDLGEX,
			UTILDLGEX_AVATAR,
			UTILDLGEX_PET,
			UTILDLGEX_MULTIPET,
			MULTILINE,
			GAMEMENU,
			MACROUI,
			EXCEPTIONITEMSEARCH,
			MOBGAGE,
			SKILLUP,
			MACROSKILL,
			QUESTICON,
			MINIGAMETABLE,
			FLOATNOTICE,
			// 悄悄话目标弹层（原生「请输入悄悄话的对象。」那一层）。
			WHISPERTARGET,
			STATUSBAR,
			CHATBAR,
			BUFFLIST,
			TEMPORARYSTATVIEW,
			NOTICE,
			NPCTALK,
			SHOP,
			STATSINFO,
			ITEMINVENTORY,
			EQUIPINVENTORY,
			SKILLBOOK,
			QUESTLOG,
			WORLDMAP,
			USERLIST,
			MINIMAP,
			CHANNEL,
			CHAT,
			CHATRANK,
			JOYPAD,
			EVENT,
			DAILYGIFT,
			KEYCONFIG,
			QUICKSLOTCONFIG,
			OPTIONMENU,
			QUIT,
			CHARINFO,
			CASHSHOP,
			STORAGE,
			PARTY,
			BUDDYLIST,
			TRADE,
			GUILD,
			/// 公会公告板独立窗（UI/GuildBBS.img）；与 GUILD 成员窗分型，避免互相抢焦点。
			GUILDBBS,
			/// 公会 Boss PQ HUD（healer Y / pulley state，s2c 287/288）
			GUILDBOSS,
			MERCHANT,
			MONSTERBOOK,
			MAKER,
			ALLIANCE,
			// v83 目录气球的两颗独立设置窗（游戏设置 / 系统设置）。本包无
			// StatusBar3.img/OptionMenu，UIOptionMenu 只剩空壳，故另立两型。
			// 注意：这两项让 UIElement::Type::NUM_TYPES 变大，而
			// EnumMap<Type, UPtr, NUM_TYPES> 是 UIStateGame 的成员 —— 实例尺寸变了，
			// 必须整工程 Rebuild（增量编译会留下按旧尺寸编译的目标文件）。
			GAMEOPT,
			SYSOPT,
			SPECIALUI,
			NUM_TYPES
		};

		virtual ~UIElement() {}

		virtual void draw(float inter) const;
		virtual void update();
		// 视口变化时由 UIState 广播；登录模态默认可 override 为 center_on_screen()。
		virtual void update_screen(int16_t new_width, int16_t new_height) {}

		void makeactive();
		void deactivate();
		bool is_active() const;

		// 把本窗体贴到宿主窗体右侧（原生「技能组合栏」相对技能窗的位置关系）：
		// 右侧放不下翻到左侧，最后夹回视口。调用前 dimension 必须已是位图真身尺寸。
		void anchor_right_of(const UIElement& host, int16_t gap = 8);

		/// 静态文案绑定：窗体 ctor 里登记文案对象与 Locale 键，切语言后由
		/// UIState 的 update 派发点调 refresh_localized() 统一重取 —— 于是
		/// 「50 个窗各自记得刷新」退化成一处机制。带 value 的那一档是给
		/// 「前缀 + 会话内恒定的值」用的（`Ver. ` + 版本号），按 Locale::format 取。
		///
		/// 只给**值在会话内恒定**的标签用，判据是全文搜这个对象还有没有别的写入点：
		/// 有就不恒定。随数据变的行（金币数额、`Default Group (0/0)` 这类）要继续
		/// `Locale::text / Locale::format` 就地现算 —— 那条路本来就是活文案，登记进表
		/// 反而会把登记那一刻的旧值钉死；而改写若是**事件驱动**的（选中变化、数据到达），
		/// 切语言时刷回初值之后不会有人再把它写回去，界面就一直停在初值上。
		void bind_text(Text& target, std::string key);
		void bind_text(Text& target, std::string key, std::string value);
		void bind_text(OutlinedText& target, std::string key);
		void bind_text(OutlinedText& target, std::string key, std::string value);

		/// Locale 代次变了才逐条重取；无绑定的窗体是一次空 vector 判断。
		void refresh_localized();

		virtual void toggle_active();
		virtual Button::State button_pressed(uint16_t buttonid) { return Button::State::DISABLED; }
		virtual bool send_icon(const Icon& icon, Point<int16_t> cursor_position) { return true; }

		virtual void doubleclick(Point<int16_t> cursorpos) {}
		virtual void rightclick(Point<int16_t> cursorpos) {}
		virtual bool is_in_range(Point<int16_t> cursor_position) const;
		virtual void remove_cursor();
		virtual Cursor::State send_cursor(bool clicked, Point<int16_t> cursor_position);
		virtual void send_scroll(double yoffset) {}
		virtual void send_key(int32_t keycode, bool pressed, bool escape) {}

		virtual UIElement::Type get_type() const = 0;

	protected:
		UIElement(Point<int16_t> position, Point<int16_t> dimension, bool active);
		UIElement(Point<int16_t> position, Point<int16_t> dimension);
		UIElement();

		void draw_sprites(float alpha) const;
		void draw_buttons(float alpha) const;

		// 用已设置的 dimension，按当前 Constants 视口把 position 置为屏幕居中。
		// 登录弹窗：子控件用面板局部坐标，禁止再写死屏幕绝对坐标。
		void center_on_screen();

		std::map<uint16_t, std::unique_ptr<Button>> buttons;
		std::vector<Sprite> sprites;
		Point<int16_t> position;
		Point<int16_t> dimension;
		bool active;

	private:
		// 绑定表本体在 LocaleBindings（世界地图那三块非 UIElement 的吸附面板共用同一份
		// 实现）。文案对象由本窗体按值持有，指针在窗体生命周期内恒有效；不注册到全局表，
		// 因而不用管 Text 的隐式拷贝赋值与静态析构顺序。
		LocaleBindings locale_binds;
	};
}