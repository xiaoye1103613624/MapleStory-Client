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
#include "UIStatusBar.h"

#include "../../Util/ResolutionTiers.h"

#include "UIAdmin.h"
#include "UIChannel.h"
#include "UICharInfo.h"
#include "UIChat.h"
#include "UIMessenger.h"
#include "UIDelivery.h"
#include "UIEquipInventory.h"
#include "UIDailyGift.h"
#include "UIEvent.h"
#include "UIFamily.h"
#include "UIGuild.h"
#include "UIItemInventory.h"
#include "UIJoypad.h"
#include "UIKeyConfig.h"
#include "UIMaker.h"
#include "UIMonsterBook.h"
#include "UIMonsterCarnival.h"
#include "UIOptionMenu.h"
#include "UIMiscShells.h"
#include "UIPlant.h"
#include "UIQuestLog.h"
#include "UIQuit.h"
#include "UISkillBook.h"
#include "UISoftKey.h"
#include "UIStatsInfo.h"
#include "UIUserList.h"
#include "UIWorldMap.h"

#include "../UI.h"

// B4-5 HP/MP 警报阈值取自 ui.sysopt.hp_warn/mp_warn（Setting<SysOptHpWarn>）。
#include "../../Configuration.h"

#include "../Components/MapleButton.h"

#include "../../Character/ExpTable.h"
#include "../../Gameplay/Stage.h"

#include "../../Data/ItemData.h"
#include "../../Data/SkillData.h"
#include "../../Net/Packets/GameplayPackets.h"
#include "../../Net/Packets/MessagingPackets.h"
#include "../../Net/Packets/MiscOutPackets.h"
#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Net/Packets/PlayerPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"
#include "../KeyConfig.h"
#include "../KeyType.h"
#include "UIChatBar.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

#include <algorithm>

// 经典底条的分段裁框使用 Range<int16_t>（Texture::draw 的水平裁剪参数）。
#include "../../Template/Range.h"

namespace ms
{
	// classic 三联量规图集（gauge/bar、gauge/graduation 均 340x31）。
	// 图集内三段是「同行 1:1 排列」，参考端也按图集原间距绘制（不重新拉开间距）：
	//   HP 边框 x0..107 / MP x109..215 / EXP x222..339（实测见 evidence/w1w3-hud/coords.md，
	//   参考截图 graduation 暗框整体跨 x218..557 = 340px，与 GAUGE_X=218 + 1:1 完全吻合）。
	// 因此 SEG_*（图集内左裁起点）同时就是绘制时的目标偏移，无需第二套 DST_*。
	constexpr int16_t SEG_HP = 0;
	constexpr int16_t SEG_MP = 109;
	constexpr int16_t SEG_EXP = 222;
	constexpr int16_t SEG_W_HP = 108;
	constexpr int16_t SEG_W_MP = 107;
	constexpr int16_t SEG_W_EXP = 118;

	// classic 快捷格几何（由 base/quickSlot08|26|32 图集自身反推，非截图量）：
	//   151 = 9 + 4*28 + 3*7 + 9   → quickSlot08 4 列
	//   466 = 9 + 13*28 + 12*7 + 9 → quickSlot26 13 列
	//   571 = 9 + 16*28 + 15*7 + 9 → quickSlot32 16 列
	//   80  = 9 + 2*28 + 1*6 + 9   → 2 行，行距 34
	constexpr int16_t QS_SLOT = 28;
	constexpr int16_t QS_PITCH_X = 35;
	constexpr int16_t QS_PITCH_Y = 34;
	constexpr int16_t QS_FIRST_X = 9;
	constexpr int16_t QS_FIRST_Y = 9;
	// WZ 图标锚点在贴图下方（origin.y = +32），裸 Texture 须同样下移才落到格内。
	constexpr int16_t QS_ICON_SHIFT_Y = 32;
	// 键帽带：key/0「Shift」宽 28 = 格宽，故贴纸以格左上角为锚点。
	// 数字/字母键帽字身（参考截图量得 #AABBCC 像素包围盒）左上角 = 格左上角 + (4, 4)。
	constexpr int16_t QS_CAP_X = 4;
	constexpr int16_t QS_CAP_Y = 4;
	// 中文绑定名落在格下半带（A11M 行高约 11，14 + 11 ≤ 28）。
	constexpr int16_t QS_LABEL_Y = 14;

	// classic 快捷格键位表（13 列 x2 行，参考端 KeyConfig 槽序）。
	constexpr uint8_t CLASSIC_QS_KEYS[26] = {
		KeyConfig::Key::LEFT_SHIFT, KeyConfig::Key::INSERT, KeyConfig::Key::HOME, KeyConfig::Key::PAGE_UP,
		KeyConfig::Key::NUM1, KeyConfig::Key::NUM2, KeyConfig::Key::NUM3, KeyConfig::Key::NUM4, KeyConfig::Key::NUM5,
		KeyConfig::Key::A, KeyConfig::Key::S, KeyConfig::Key::D, KeyConfig::Key::F,
		KeyConfig::Key::LEFT_CONTROL, KeyConfig::Key::DELETE, KeyConfig::Key::END, KeyConfig::Key::PAGE_DOWN,
		KeyConfig::Key::Q, KeyConfig::Key::W, KeyConfig::Key::E, KeyConfig::Key::R, KeyConfig::Key::T,
		KeyConfig::Key::Z, KeyConfig::Key::X, KeyConfig::Key::C, KeyConfig::Key::V
	};
	// key/0..6 贴纸索引（Shift/Ins/Hm/Pup/Ctrl/Del/End）；-1 = 无贴纸，走 KEYNAME。
	constexpr int8_t CLASSIC_QS_KEYCAP[26] = {
		0, 1, 2, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		4, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
	};
	// NUM1-5 用 FontMemo 位图数字；字母与 Pdn（本包无贴纸，key/7 是 28x20 红箭头）用描边文字回退。
	constexpr const char* CLASSIC_QS_KEYNAME[26] = {
		"", "", "", "", "1", "2", "3", "4", "5", "A", "S", "D", "F",
		"", "", "", "Pdn", "Q", "W", "E", "R", "T", "Z", "X", "C", "V"
	};

	// StatusBar.img/number 字库：0-9 与 slash/percent 高 7，Lbracket/Rbracket 高 9，
	// 全部 origin = (0,0)（nx 实测），且无 '.' 字形。
	// 推进模型由参考截图逐列还原（HP「[50/50]」/ MP「[50/50]」/ EXP「[0.00%]」全列吻合）：
	//   '[' ']' → getw + 2      其余字形 → getw + 1      '.'（字库缺，用 1x1 白点补）→ 4
	// 垂直：以 9px 括号行为基准，7px 字形下移 1px 居中；小数点下移 7px 贴基线。
	int16_t hud_advance(const Charset& set, char c)
	{
		if (c == '.')
			return 4;

		int16_t width = set.getw(c);

		return (c == '[' || c == ']') ? width + 2 : width + 1;
	}

	void draw_hud_number(const Charset& set, const ColorBox& dot, const std::string& text,
		const DrawArgument& args)
	{
		int16_t shift = 0;

		for (char c : text)
		{
			if (c == '.')
			{
				dot.draw(args + Point<int16_t>(static_cast<int16_t>(shift + 1), 7));
			}
			else
			{
				const bool bracket = (c == '[' || c == ']');

				set.draw(c, args + Point<int16_t>(shift, bracket ? 0 : 1));
			}

			shift += hud_advance(set, c);
		}
	}

	UIStatusBar::UIStatusBar(const CharStats& st) : stats(st)
	{
		quickslot_active = false;
		quickslot_adj = Point<int16_t>(QUICKSLOT_MAX, 0);
		VWIDTH = Constants::Constants::get().get_viewwidth();
		VHEIGHT = Constants::Constants::get().get_viewheight();

		menu_active = false;
		setting_active = false;
		community_active = false;
		character_active = false;
		event_active = false;
		classic_mode = UiAssetResolve::is_classic_hud();

		if (classic_mode)
		{
			LOG(LOG_DEBUG, "UIStatusBar: using StatusBar.img classic HUD");
			init_classic();
			return;
		}

		init_modern();
	}

	void UIStatusBar::init_modern()
	{
		std::string stat = "status";

		if (VWIDTH == 800)
			stat += "800";

		nl::node mainBar = UiAssetResolve::status_bar_root()["mainBar"];
		nl::node status = mainBar[stat];
		nl::node EXPBar = mainBar["EXPBar"];
		nl::node EXPBarRes = EXPBar[VWIDTH];

		// 多分辨率兜底：modern HUD 的 EXPBar 只有 800/1024/1280/1366/1920 五套分件，
		// 非枚举宽度（1600/1440/1680/2560）取不到时回退 1280 那套，避免空节点。
		if (!UiAssetResolve::usable(EXPBarRes))
			EXPBarRes = EXPBar[1280];
		nl::node menu = mainBar["menu"];
		nl::node quickSlot = mainBar["quickSlot"];
		nl::node submenu = mainBar["submenu"];

		exp_pos = Point<int16_t>(0, 87);

		sprites.emplace_back(EXPBar["backgrnd"], DrawArgument(Point<int16_t>(0, 87), Point<int16_t>(VWIDTH, 0)));
		sprites.emplace_back(EXPBarRes["layer:back"], exp_pos);

		int16_t exp_max = VWIDTH - 16;

		expbar = Gauge(
			Gauge::Type::DEFAULT,
			EXPBarRes.resolve("layer:gauge"),
			EXPBarRes.resolve("layer:cover"),
			EXPBar.resolve("layer:effect"),
			exp_max, 0.0f
		);

		int16_t pos_adj = 0;

		if (VWIDTH == 1280)
			pos_adj = 87;
		else if (VWIDTH == 1366)
			pos_adj = 171;
		else if (VWIDTH == 1920)
			pos_adj = 448;

		if (VWIDTH == 1024)
			quickslot_min = 1;
		else
			quickslot_min = 0;

		if (VWIDTH == 800)
		{
			hpmp_pos = Point<int16_t>(412, 40);
			hpset_pos = Point<int16_t>(530, 70);
			mpset_pos = Point<int16_t>(528, 86);
			statset_pos = Point<int16_t>(427, 111);
			levelset_pos = Point<int16_t>(461, 48);
			namelabel_pos = Point<int16_t>(487, 40);
			quickslot_pos = Point<int16_t>(579, 0);

			// Menu
			menu_pos = Point<int16_t>(682, -280);
			setting_pos = menu_pos + Point<int16_t>(0, 168);
			community_pos = menu_pos + Point<int16_t>(-26, 196);
			character_pos = menu_pos + Point<int16_t>(-61, 168);
			event_pos = menu_pos + Point<int16_t>(-94, 252);
		}
		else
		{
			hpmp_pos = Point<int16_t>(416 + pos_adj, 40);
			hpset_pos = Point<int16_t>(550 + pos_adj, 70);
			mpset_pos = Point<int16_t>(546 + pos_adj, 86);
			statset_pos = Point<int16_t>(539 + pos_adj, 111);
			levelset_pos = Point<int16_t>(465 + pos_adj, 48);
			namelabel_pos = Point<int16_t>(493 + pos_adj, 40);
			quickslot_pos = Point<int16_t>(628 + pos_adj, 37);

			// Menu
			menu_pos = Point<int16_t>(720 + pos_adj, -280);
			setting_pos = menu_pos + Point<int16_t>(0, 168);
			community_pos = menu_pos + Point<int16_t>(-26, 196);
			character_pos = menu_pos + Point<int16_t>(-61, 168);
			event_pos = menu_pos + Point<int16_t>(-94, 252);
		}

		if (VWIDTH == 1280)
		{
			statset_pos = Point<int16_t>(580 + pos_adj, 111);
			quickslot_pos = Point<int16_t>(622 + pos_adj, 37);

			// Menu
			menu_pos += Point<int16_t>(-7, 0);
			setting_pos += Point<int16_t>(-7, 0);
			community_pos += Point<int16_t>(-7, 0);
			character_pos += Point<int16_t>(-7, 0);
			event_pos += Point<int16_t>(-7, 0);
		}
		else if (VWIDTH == 1366)
		{
			quickslot_pos = Point<int16_t>(623 + pos_adj, 37);

			// Menu
			menu_pos += Point<int16_t>(-5, 0);
			setting_pos += Point<int16_t>(-5, 0);
			community_pos += Point<int16_t>(-5, 0);
			character_pos += Point<int16_t>(-5, 0);
			event_pos += Point<int16_t>(-5, 0);
		}
		else if (VWIDTH == 1920)
		{
			quickslot_pos = Point<int16_t>(900 + pos_adj, 37);

			// Menu
			menu_pos += Point<int16_t>(272, 0);
			setting_pos += Point<int16_t>(272, 0);
			community_pos += Point<int16_t>(272, 0);
			character_pos += Point<int16_t>(272, 0);
			event_pos += Point<int16_t>(272, 0);
		}

		hpmp_sprites.emplace_back(status["backgrnd"], hpmp_pos - Point<int16_t>(1, 0));
		hpmp_sprites.emplace_back(status["layer:cover"], hpmp_pos - Point<int16_t>(1, 0));

		if (VWIDTH == 800)
			hpmp_sprites.emplace_back(status["layer:Lv"], hpmp_pos);
		else
			hpmp_sprites.emplace_back(status["layer:Lv"], hpmp_pos - Point<int16_t>(1, 0));

		int16_t hpmp_max = 139;

		if (VWIDTH > 800)
			hpmp_max += 30;

		hpbar = Gauge(Gauge::Type::DEFAULT, status.resolve("gauge/hp/layer:0"), hpmp_max, 0.0f);
		mpbar = Gauge(Gauge::Type::DEFAULT, status.resolve("gauge/mp/layer:0"), hpmp_max, 0.0f);

		statset = Charset(EXPBar["number"], Charset::Alignment::RIGHT);
		hpmpset = Charset(status["gauge"]["number"], Charset::Alignment::RIGHT);
		levelset = Charset(status["lvNumber"], Charset::Alignment::LEFT);

		namelabel = OutlinedText(Text::Font::A13M, Text::Alignment::LEFT, Color::Name::GALLERY, Color::Name::TUNA);

		quickslot[0] = quickSlot["backgrnd"];
		quickslot[1] = quickSlot["layer:cover"];

		Point<int16_t> buttonPos = Point<int16_t>(591 + pos_adj, 73);

		if (VWIDTH == 1024)
			buttonPos += Point<int16_t>(38, 0);
		else if (VWIDTH == 1280)
			buttonPos += Point<int16_t>(31, 0);
		else if (VWIDTH == 1366)
			buttonPos += Point<int16_t>(33, 0);
		else if (VWIDTH == 1920)
			buttonPos += Point<int16_t>(310, 0);

		buttons[Buttons::BT_CASHSHOP] = std::make_unique<MapleButton>(menu["button:CashShop"], buttonPos);
		buttons[Buttons::BT_MENU] = std::make_unique<MapleButton>(menu["button:Menu"], buttonPos);
		buttons[Buttons::BT_OPTIONS] = std::make_unique<MapleButton>(menu["button:Setting"], buttonPos);
		buttons[Buttons::BT_CHARACTER] = std::make_unique<MapleButton>(menu["button:Character"], buttonPos);
		buttons[Buttons::BT_COMMUNITY] = std::make_unique<MapleButton>(menu["button:Community"], buttonPos);
		buttons[Buttons::BT_EVENT] = std::make_unique<MapleButton>(menu["button:Event"], buttonPos);

		if (quickslot_active && VWIDTH > 800)
		{
			buttons[Buttons::BT_CASHSHOP]->set_active(false);
			buttons[Buttons::BT_MENU]->set_active(false);
			buttons[Buttons::BT_OPTIONS]->set_active(false);
			buttons[Buttons::BT_CHARACTER]->set_active(false);
			buttons[Buttons::BT_COMMUNITY]->set_active(false);
			buttons[Buttons::BT_EVENT]->set_active(false);
		}

		std::string fold = "button:Fold";
		std::string extend = "button:Extend";

		if (VWIDTH == 800)
		{
			fold += "800";
			extend += "800";
		}

		if (VWIDTH == 1366)
			quickslot_qs_adj = Point<int16_t>(213, 0);
		else
			quickslot_qs_adj = Point<int16_t>(211, 0);

		if (VWIDTH == 800)
		{
			Point<int16_t> quickslot_qs = Point<int16_t>(579, 0);

			buttons[Buttons::BT_FOLD_QS] = std::make_unique<MapleButton>(quickSlot[fold], quickslot_qs);
			buttons[Buttons::BT_EXTEND_QS] = std::make_unique<MapleButton>(quickSlot[extend], quickslot_qs);
		}
		else if (VWIDTH == 1024)
		{
			Point<int16_t> quickslot_qs = Point<int16_t>(627 + pos_adj, 37);

			buttons[Buttons::BT_FOLD_QS] = std::make_unique<MapleButton>(quickSlot[fold], quickslot_qs);
			buttons[Buttons::BT_EXTEND_QS] = std::make_unique<MapleButton>(quickSlot[extend], quickslot_qs);
		}
		else if (VWIDTH == 1280)
		{
			Point<int16_t> quickslot_qs = Point<int16_t>(621 + pos_adj, 37);

			buttons[Buttons::BT_FOLD_QS] = std::make_unique<MapleButton>(quickSlot[fold], quickslot_qs);
			buttons[Buttons::BT_EXTEND_QS] = std::make_unique<MapleButton>(quickSlot[extend], quickslot_qs);
		}
		else if (VWIDTH == 1366)
		{
			Point<int16_t> quickslot_qs = Point<int16_t>(623 + pos_adj, 37);

			buttons[Buttons::BT_FOLD_QS] = std::make_unique<MapleButton>(quickSlot[fold], quickslot_qs);
			buttons[Buttons::BT_EXTEND_QS] = std::make_unique<MapleButton>(quickSlot[extend], quickslot_qs);
		}
		else if (VWIDTH == 1920)
		{
			Point<int16_t> quickslot_qs = Point<int16_t>(900 + pos_adj, 37);

			buttons[Buttons::BT_FOLD_QS] = std::make_unique<MapleButton>(quickSlot[fold], quickslot_qs);
			buttons[Buttons::BT_EXTEND_QS] = std::make_unique<MapleButton>(quickSlot[extend], quickslot_qs);
		}
		else
		{
			// 多分辨率兜底：1600/1440/1680/2560 等非枚举宽度按 1280 基准的快捷栏
			// 相对位置建钮，否则下面 set_active 与 draw 里的 at() 会空指针/抛异常。
			Point<int16_t> quickslot_qs = Point<int16_t>(
				static_cast<int16_t>(621 + (VWIDTH - 1280) / 2), 37);

			buttons[Buttons::BT_FOLD_QS] = std::make_unique<MapleButton>(quickSlot[fold], quickslot_qs);
			buttons[Buttons::BT_EXTEND_QS] = std::make_unique<MapleButton>(quickSlot[extend], quickslot_qs);
		}

		if (buttons[Buttons::BT_FOLD_QS] && buttons[Buttons::BT_EXTEND_QS])
		{
			if (quickslot_active)
				buttons[Buttons::BT_EXTEND_QS]->set_active(false);
			else
				buttons[Buttons::BT_FOLD_QS]->set_active(false);
		}

#pragma region Menu
		menubackground[0] = submenu["backgrnd"]["0"];
		menubackground[1] = submenu["backgrnd"]["1"];
		menubackground[2] = submenu["backgrnd"]["2"];

		buttons[Buttons::BT_MENU_ACHIEVEMENT] = std::make_unique<MapleButton>(submenu["menu"]["button:achievement"], menu_pos);
		buttons[Buttons::BT_MENU_AUCTION] = std::make_unique<MapleButton>(submenu["menu"]["button:auction"], menu_pos);
		buttons[Buttons::BT_MENU_BATTLE] = std::make_unique<MapleButton>(submenu["menu"]["button:battleStats"], menu_pos);
		buttons[Buttons::BT_MENU_CLAIM] = std::make_unique<MapleButton>(submenu["menu"]["button:Claim"], menu_pos);
		buttons[Buttons::BT_MENU_FISHING] = std::make_unique<MapleButton>(submenu["menu"]["button:Fishing"], menu_pos + Point<int16_t>(3, 1));
		buttons[Buttons::BT_MENU_HELP] = std::make_unique<MapleButton>(submenu["menu"]["button:Help"], menu_pos);
		buttons[Buttons::BT_MENU_MEDAL] = std::make_unique<MapleButton>(submenu["menu"]["button:medal"], menu_pos);
		buttons[Buttons::BT_MENU_MONSTER_COLLECTION] = std::make_unique<MapleButton>(submenu["menu"]["button:monsterCollection"], menu_pos);
		buttons[Buttons::BT_MENU_MONSTER_LIFE] = std::make_unique<MapleButton>(submenu["menu"]["button:monsterLife"], menu_pos);
		buttons[Buttons::BT_MENU_QUEST] = std::make_unique<MapleButton>(submenu["menu"]["button:quest"], menu_pos);
		buttons[Buttons::BT_MENU_UNION] = std::make_unique<MapleButton>(submenu["menu"]["button:union"], menu_pos);

		buttons[Buttons::BT_SETTING_CHANNEL] = std::make_unique<MapleButton>(submenu["setting"]["button:channel"], setting_pos);
		buttons[Buttons::BT_SETTING_QUIT] = std::make_unique<MapleButton>(submenu["setting"]["button:GameQuit"], setting_pos);
		buttons[Buttons::BT_SETTING_JOYPAD] = std::make_unique<MapleButton>(submenu["setting"]["button:JoyPad"], setting_pos);
		buttons[Buttons::BT_SETTING_KEYS] = std::make_unique<MapleButton>(submenu["setting"]["button:keySetting"], setting_pos);
		buttons[Buttons::BT_SETTING_OPTION] = std::make_unique<MapleButton>(submenu["setting"]["button:option"], setting_pos);

		buttons[Buttons::BT_COMMUNITY_PARTY] = std::make_unique<MapleButton>(submenu["community"]["button:bossParty"], community_pos);
		buttons[Buttons::BT_COMMUNITY_FRIENDS] = std::make_unique<MapleButton>(submenu["community"]["button:friends"], community_pos);
		buttons[Buttons::BT_COMMUNITY_GUILD] = std::make_unique<MapleButton>(submenu["community"]["button:guild"], community_pos);
		buttons[Buttons::BT_COMMUNITY_MAPLECHAT] = std::make_unique<MapleButton>(submenu["community"]["button:mapleChat"], community_pos);

		buttons[Buttons::BT_CHARACTER_INFO] = std::make_unique<MapleButton>(submenu["character"]["button:character"], character_pos);
		buttons[Buttons::BT_CHARACTER_EQUIP] = std::make_unique<MapleButton>(submenu["character"]["button:Equip"], character_pos);
		buttons[Buttons::BT_CHARACTER_ITEM] = std::make_unique<MapleButton>(submenu["character"]["button:Item"], character_pos);
		buttons[Buttons::BT_CHARACTER_SKILL] = std::make_unique<MapleButton>(submenu["character"]["button:Skill"], character_pos);
		buttons[Buttons::BT_CHARACTER_STAT] = std::make_unique<MapleButton>(submenu["character"]["button:Stat"], character_pos);

		buttons[Buttons::BT_EVENT_DAILY] = std::make_unique<MapleButton>(submenu["event"]["button:dailyGift"], event_pos);
		buttons[Buttons::BT_EVENT_SCHEDULE] = std::make_unique<MapleButton>(submenu["event"]["button:schedule"], event_pos);

		for (size_t i = Buttons::BT_MENU_QUEST; i <= Buttons::BT_EVENT_DAILY; i++)
			buttons[i]->set_active(false);

		menutitle[0] = submenu["title"]["character"];
		menutitle[1] = submenu["title"]["community"];
		menutitle[2] = submenu["title"]["event"];
		menutitle[3] = submenu["title"]["menu"];
		menutitle[4] = submenu["title"]["setting"];
#pragma endregion

		if (VWIDTH == 800)
		{
			position = Point<int16_t>(0, 480);
			position_x = 410;
			position_y = position.y();
			dimension = Point<int16_t>(VWIDTH - position_x, 140);
		}
		else if (VWIDTH == 1024)
		{
			position = Point<int16_t>(0, 648);
			position_x = 410;
			position_y = position.y() + 42;
			dimension = Point<int16_t>(VWIDTH - position_x, 75);
		}
		else if (VWIDTH == 1280)
		{
			position = Point<int16_t>(0, 600);
			position_x = 500;
			position_y = position.y() + 42;
			dimension = Point<int16_t>(VWIDTH - position_x, 75);
		}
		else if (VWIDTH == 1366)
		{
			position = Point<int16_t>(0, 648);
			position_x = 585;
			position_y = position.y() + 42;
			dimension = Point<int16_t>(VWIDTH - position_x, 75);
		}
		else if (VWIDTH == 1920)
		{
			position = Point<int16_t>(0, 960 + (VHEIGHT - 1080));
			position_x = 860;
			position_y = position.y() + 40;
			dimension = Point<int16_t>(VWIDTH - position_x, 80);
		}
		else
		{
			// 多分辨率兜底：1600/1440/1680/2560 等非枚举宽度不能落到默认的
			// position=(0,0)/dimension=(0,0)（菜单命中与量表会整体崩坏）。
			// 按 1280 基准的「右侧 610 宽菜单区」推导，保持底栏贴底、菜单右区不越界。
			LOG(LOG_WARN, "[MR] StatusBar modern fallback for VWIDTH=" << VWIDTH);
			position = Point<int16_t>(0, static_cast<int16_t>(VHEIGHT - 75));
			position_x = static_cast<int16_t>(VWIDTH - 610);
			position_y = static_cast<int16_t>(position.y() + 42);
			dimension = Point<int16_t>(610, 75);
		}
	}

	void UIStatusBar::draw(float alpha) const
	{
		if (classic_mode)
		{
			draw_classic(alpha);
			return;
		}

		UIElement::draw_sprites(alpha);

		for (size_t i = 0; i <= Buttons::BT_EVENT; i++)
			buttons.at(i)->draw(position);

		hpmp_sprites[0].draw(position, alpha);

		expbar.draw(position + exp_pos);
		hpbar.draw(position + hpmp_pos);
		mpbar.draw(position + hpmp_pos);

		hpmp_sprites[1].draw(position, alpha);
		hpmp_sprites[2].draw(position, alpha);

		int16_t level = stats.get_stat(MapleStat::Id::LEVEL);
		int16_t hp = stats.get_stat(MapleStat::Id::HP);
		int16_t mp = stats.get_stat(MapleStat::Id::MP);
		int32_t maxhp = stats.get_total(EquipStat::Id::HP);
		int32_t maxmp = stats.get_total(EquipStat::Id::MP);
		int64_t exp = stats.get_exp();

		std::string expstring = std::to_string(100 * getexppercent());

		statset.draw(
			std::to_string(exp) + "[" + expstring.substr(0, expstring.find('.') + 3) + "%]",
			position + statset_pos
		);

		hpmpset.draw(
			"[" + std::to_string(hp) + "/" + std::to_string(maxhp) + "]",
			position + hpset_pos
		);

		hpmpset.draw(
			"[" + std::to_string(mp) + "/" + std::to_string(maxmp) + "]",
			position + mpset_pos
		);

		levelset.draw(
			std::to_string(level),
			position + levelset_pos
		);

		namelabel.draw(position + namelabel_pos);

		// 多分辨率兜底：非枚举宽度下若这两个钮创建失败，不能用 at() 抛异常。
		if (const auto fold_it = buttons.find(Buttons::BT_FOLD_QS);
			fold_it != buttons.end() && fold_it->second)
			fold_it->second->draw(position + quickslot_adj);

		if (const auto extend_it = buttons.find(Buttons::BT_EXTEND_QS);
			extend_it != buttons.end() && extend_it->second)
			extend_it->second->draw(position + quickslot_adj - quickslot_qs_adj);

		if (VWIDTH > 800 && VWIDTH < 1366)
		{
			quickslot[0].draw(position + quickslot_pos + Point<int16_t>(-1, 0) + quickslot_adj);
			quickslot[1].draw(position + quickslot_pos + Point<int16_t>(-1, 0) + quickslot_adj);
		}
		else
		{
			quickslot[0].draw(position + quickslot_pos + quickslot_adj);
			quickslot[1].draw(position + quickslot_pos + quickslot_adj);
		}

#pragma region Menu
		Point<int16_t> pos_adj = Point<int16_t>(0, 0);

		if (quickslot_active)
		{
			if (VWIDTH == 800)
				pos_adj += Point<int16_t>(0, -73);
			else
				pos_adj += Point<int16_t>(0, -31);
		}

		Point<int16_t> pos;
		uint8_t button_count, menutitle_index;

		if (character_active)
		{
			pos = character_pos;
			button_count = 5;
			menutitle_index = 0;
		}
		else if (community_active)
		{
			pos = community_pos;
			button_count = 4;
			menutitle_index = 1;
		}
		else if (event_active)
		{
			pos = event_pos;
			button_count = 2;
			menutitle_index = 2;
		}
		else if (menu_active)
		{
			pos = menu_pos;
			button_count = 11;
			menutitle_index = 3;
		}
		else if (setting_active)
		{
			pos = setting_pos;
			button_count = 5;
			menutitle_index = 4;
		}
		else
		{
			return;
		}

		Point<int16_t> mid_pos = Point<int16_t>(0, 29);

		uint16_t end_y = std::floor(28.2 * button_count);

		if (menu_active)
			end_y -= 1;

		uint16_t mid_y = end_y - mid_pos.y();

		menubackground[0].draw(position + pos + pos_adj);
		menubackground[1].draw(DrawArgument(position + pos + pos_adj) + DrawArgument(mid_pos, Point<int16_t>(0, mid_y)));
		menubackground[2].draw(position + pos + pos_adj + Point<int16_t>(0, end_y));

		menutitle[menutitle_index].draw(position + pos + pos_adj);

		for (size_t i = Buttons::BT_MENU_QUEST; i <= Buttons::BT_EVENT_DAILY; i++)
			buttons.at(i)->draw(position);
#pragma endregion
	}

	void UIStatusBar::update()
	{
		if (classic_mode)
		{
			update_classic();
			return;
		}

		UIElement::update();

		for (Sprite sprite : hpmp_sprites)
			sprite.update();

		expbar.update(getexppercent());
		hpbar.update(gethppercent());
		mpbar.update(getmppercent());

		namelabel.change_text(stats.get_name());

		Point<int16_t> pos_adj = get_quickslot_pos();

		if (quickslot_active)
		{
			if (quickslot_adj.x() > quickslot_min)
			{
				int16_t new_x = quickslot_adj.x() - Constants::TIMESTEP;

				if (new_x < quickslot_min)
					quickslot_adj.set_x(quickslot_min);
				else
					quickslot_adj.shift_x(-Constants::TIMESTEP);
			}
		}
		else
		{
			if (quickslot_adj.x() < QUICKSLOT_MAX)
			{
				int16_t new_x = quickslot_adj.x() + Constants::TIMESTEP;

				if (new_x > QUICKSLOT_MAX)
					quickslot_adj.set_x(QUICKSLOT_MAX);
				else
					quickslot_adj.shift_x(Constants::TIMESTEP);
			}
		}

		for (size_t i = Buttons::BT_MENU_QUEST; i <= Buttons::BT_MENU_CLAIM; i++)
		{
			Point<int16_t> menu_adj = Point<int16_t>(0, 0);

			if (i == Buttons::BT_MENU_FISHING)
				menu_adj = Point<int16_t>(3, 1);

			buttons[i]->set_position(menu_pos + menu_adj + pos_adj);
		}

		for (size_t i = Buttons::BT_SETTING_CHANNEL; i <= Buttons::BT_SETTING_QUIT; i++)
			buttons[i]->set_position(setting_pos + pos_adj);

		for (size_t i = Buttons::BT_COMMUNITY_FRIENDS; i <= Buttons::BT_COMMUNITY_MAPLECHAT; i++)
			buttons[i]->set_position(community_pos + pos_adj);

		for (size_t i = Buttons::BT_CHARACTER_INFO; i <= Buttons::BT_CHARACTER_ITEM; i++)
			buttons[i]->set_position(character_pos + pos_adj);

		for (size_t i = Buttons::BT_EVENT_SCHEDULE; i <= Buttons::BT_EVENT_DAILY; i++)
			buttons[i]->set_position(event_pos + pos_adj);
	}

	Button::State UIStatusBar::button_pressed(uint16_t id)
	{
		switch (id)
		{
			case Buttons::BT_CLASSIC_SHOP:
			{
				// 本包商城走 s9 协议线（EnterCashShop 40 vs 229），跨线必失败（PLAN RC7）。
				// 不换 opcode，改成玩家可见提示，避免「点了没反应」。
				if (auto chatbar = UI::get().get_element<UIChatBar>())
					chatbar->show_message("商城暂未支持。", UIChatBar::MessageType::YELLOW);
				break;
			}
			case Buttons::BT_CASHSHOP:
			{
				EnterCashShopPacket().dispatch();
				break;
			}
			case Buttons::BT_CLASSIC_MENU:
			case Buttons::BT_MENU:
			{
				if (classic_mode)
				{
					// BtMenu（目录）：只开主菜单（UIGameMenu）。
					// 旧行为同时弹 UIOptionMenu（游戏设置），与参考端不符（用户实测反馈）；
					// 游戏设置由系统按键/主菜单内入口打开。
					auto gm = UI::get().get_element<UIGameMenu>();
					if (!gm)
						gm = UI::get().emplace<UIGameMenu>();
					else if (!gm->is_active())
						gm->makeactive();
					else
						gm->deactivate();
					break;
				}

				toggle_menu();
				break;
			}
			case Buttons::BT_CLASSIC_EQUIP:
			{
				if (!UiAssetResolve::usable(UiAssetResolve::window("Equip")))
					break;

				UI::get().emplace<UIEquipInventory>(
					Stage::get().get_player().get_inventory()
				);
				break;
			}
			case Buttons::BT_CLASSIC_INV:
			{
				if (!UiAssetResolve::usable(UiAssetResolve::window("Item")))
					break;

				UI::get().emplace<UIItemInventory>(
					Stage::get().get_player().get_inventory()
				);
				break;
			}
			case Buttons::BT_CLASSIC_STAT:
			{
				if (!UiAssetResolve::usable(UiAssetResolve::window("Stat")))
					break;

				UI::get().emplace<UIStatsInfo>(
					Stage::get().get_player().get_stats()
				);
				break;
			}
			case Buttons::BT_CLASSIC_SKILL:
			{
				if (!UiAssetResolve::usable(UiAssetResolve::window("Skill")))
					break;

				UI::get().emplace<UISkillBook>(
					Stage::get().get_player().get_stats(),
					Stage::get().get_player().get_skills()
				);
				break;
			}
			case Buttons::BT_CLASSIC_KEYSET:
			{
				UI::get().emplace<UIKeyConfig>(
					Stage::get().get_player().get_inventory(),
					Stage::get().get_player().get_skills()
				);
				break;
			}
			case Buttons::BT_CLASSIC_QS:
			case Buttons::BT_CLASSIC_QSD:
			{
				quickslot_active = !quickslot_active;
				sync_classic_qs_buttons();
				break;
			}
			case Buttons::BT_CLASSIC_NPT:
			{
				// BtNPT 位图为「帮助」，但帮助内容不在客户端：原生发 ENTER_MTS(0x9C)，
				// 由服务端 use_mts 决定进拍卖行 UI 还是 npc/9900001.js 脚本中心对话。
				EnterMtsPacket().dispatch();
				LOG(LOG_INFO, "UIStatusBar: Help(BtNPT) -> ENTER_MTS");
				break;
			}
			case Buttons::BT_CLASSIC_SHORT:
			{
				// BtShort「界面」= 只开 UIShortCut 面板（UIWindow.img/ShortCut 的 11 钮气球）。
				// 旧行为同时 emplace UIKeyConfig，导致点一次弹两窗（键盘设定另有
				// BT_CLASSIC_KEYSET 独立入口，见上）。
				auto sc = UI::get().get_element<UIShortCut>();
				if (!sc)
					sc = UI::get().emplace<UIShortCut>();
				else if (!sc->is_active())
					sc->makeactive();
				else
					// 再点一次「界面」必须收起。旧实现在这里什么都不做，
					// 于是气球展开后只能靠 Esc/X 关，鼠标点同一颗按钮关不掉。
					sc->deactivate();
				break;
			}
			case Buttons::BT_CLASSIC_WHISPER:
			{
				if (auto chatbar = UI::get().get_element<UIChatBar>())
					chatbar->focus_whisper();
				break;
			}
			case Buttons::BT_CLASSIC_CLAIM:
			{
				// BtClaim 旁路：打开 SlideMenu（展开快捷入口）
				auto slide = UI::get().get_element<UISlideMenu>();
				if (!slide)
					slide = UI::get().emplace<UISlideMenu>();
				else if (!slide->is_active())
					slide->makeactive();
				if (slide)
					slide->toggle_expand();
				break;
			}
			case Buttons::BT_OPTIONS:
			{
				toggle_setting();
				break;
			}
			case Buttons::BT_CHARACTER:
			{
				toggle_character();
				break;
			}
			case Buttons::BT_COMMUNITY:
			{
				toggle_community();
				break;
			}
			case Buttons::BT_EVENT:
			{
				toggle_event();
				break;
			}
			case Buttons::BT_FOLD_QS:
			{
				toggle_qs(false);
				break;
			}
			case Buttons::BT_EXTEND_QS:
			{
				toggle_qs(true);
				break;
			}
			case Buttons::BT_MENU_QUEST:
			{
				UI::get().emplace<UIQuestLog>(
					Stage::get().get_player().get_quests()
					);

				remove_menus();
				break;
			}
			case Buttons::BT_MENU_MONSTER_COLLECTION:
			{
				UI::get().emplace<UIMonsterBook>();
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_CLAIM:
			{
				auto claim = UI::get().get_element<UIClaim>();
				if (!claim)
					claim = UI::get().emplace<UIClaim>();
				else if (!claim->is_active())
					claim->makeactive();
				auto duey = UI::get().get_element<UIDelivery>();
				if (!duey)
					UI::get().emplace<UIDelivery>();
				else if (!duey->is_active())
					duey->makeactive();
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_MEDAL:
			{
				UI::get().emplace<UIDailyGift>();
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_ACHIEVEMENT:
			{
				auto ui = UI::get().get_element<UIQuestAlarm>();
				if (!ui) ui = UI::get().emplace<UIQuestAlarm>();
				else if (!ui->is_active()) ui->makeactive();
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_UNION:
			{
				UI::get().emplace<UIGuild>();
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_AUCTION:
			{
				// 拍卖行入口同样只有 ENTER_MTS 一条通道（此前用 FindPlayer 冒充检索，
				// 属自创代理）；服务端 use_mts=false 时该包回脚本中心对话。
				EnterMtsPacket().dispatch();
				LOG(LOG_INFO, "UIStatusBar: Auction -> ENTER_MTS");
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_MONSTER_LIFE:
			{
				UI::get().emplace<UIMonsterBook>();
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_BATTLE:
			{
				auto mc = UI::get().get_element<UIMonsterCarnival>();
				if (!mc)
					mc = UI::get().emplace<UIMonsterCarnival>();
				else if (!mc->is_active())
					mc->makeactive();
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_FISHING:
			{
				auto plant = UI::get().get_element<UIPlant>();
				if (!plant)
					plant = UI::get().emplace<UIPlant>();
				else if (!plant->is_active())
					plant->makeactive();
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_HELP:
			{
				EnterMtsPacket().dispatch();
				LOG(LOG_INFO, "UIStatusBar: Help(menu) -> ENTER_MTS");
				remove_menus();
				break;
			}
			case Buttons::BT_SETTING_CHANNEL:
			{
				UI::get().emplace<UIChannel>();

				remove_menus();
				break;
			}
			case Buttons::BT_SETTING_OPTION:
			{
				UI::get().emplace<UIOptionMenu>();

				remove_menus();
				break;
			}
			case Buttons::BT_SETTING_KEYS:
			{
				UI::get().emplace<UIKeyConfig>(
					Stage::get().get_player().get_inventory(),
					Stage::get().get_player().get_skills()
					);

				remove_menus();
				break;
			}
			case Buttons::BT_SETTING_JOYPAD:
			{
				UI::get().emplace<UIJoypad>();

				remove_menus();
				break;
			}
			case Buttons::BT_SETTING_QUIT:
			{
				UI::get().emplace<UIQuit>(stats);

				remove_menus();
				break;
			}
			case Buttons::BT_COMMUNITY_FRIENDS:
			case Buttons::BT_COMMUNITY_PARTY:
			{
				auto userlist = UI::get().get_element<UIUserList>();
				auto tab = (id == Buttons::BT_COMMUNITY_FRIENDS) ? UIUserList::Tab::FRIEND : UIUserList::Tab::PARTY;

				if (!userlist)
				{
					UI::get().emplace<UIUserList>(tab);
				}
				else
				{
					auto cur_tab = userlist->get_tab();
					auto is_active = userlist->is_active();

					if (cur_tab == tab)
					{
						if (is_active)
							userlist->deactivate();
						else
							userlist->makeactive();
					}
					else
					{
						if (!is_active)
							userlist->makeactive();

						userlist->change_tab(tab);
					}
				}

				remove_menus();
				break;
			}
			case Buttons::BT_COMMUNITY_GUILD:
			{
				auto guild = UI::get().get_element<UIGuild>();
				if (!guild)
					guild = UI::get().emplace<UIGuild>();
				else if (!guild->is_active())
					guild->makeactive();
				remove_menus();
				break;
			}
			case Buttons::BT_COMMUNITY_MAPLECHAT:
			{
				// 社区 mapleChat 槽 → Messenger（出站建房）
				if (UiAssetResolve::usable(UiAssetResolve::window("Messenger")))
				{
					UI::get().emplace<UIMessenger>();

					if (auto m = UI::get().get_element<UIMessenger>())
					{
						if (m->is_active())
							m->try_create_room();
					}
				}
				else
				{
					UI::get().emplace<UIChat>();
				}

				remove_menus();
				break;
			}
			case Buttons::BT_CHARACTER_INFO:
			{
				UI::get().emplace<UICharInfo>(
					Stage::get().get_player().get_oid()
					);

				remove_menus();
				break;
			}
			case Buttons::BT_CHARACTER_STAT:
			{
				UI::get().emplace<UIStatsInfo>(
					Stage::get().get_player().get_stats()
					);

				remove_menus();
				break;
			}
			case Buttons::BT_CHARACTER_SKILL:
			{
				UI::get().emplace<UISkillBook>(
					Stage::get().get_player().get_stats(),
					Stage::get().get_player().get_skills()
					);

				remove_menus();
				break;
			}
			case Buttons::BT_CHARACTER_EQUIP:
			{
				UI::get().emplace<UIEquipInventory>(
					Stage::get().get_player().get_inventory()
					);

				remove_menus();
				break;
			}
			case Buttons::BT_CHARACTER_ITEM:
			{
				UI::get().emplace<UIItemInventory>(
					Stage::get().get_player().get_inventory()
					);

				remove_menus();
				break;
			}
			case Buttons::BT_EVENT_SCHEDULE:
			{
				UI::get().emplace<UIEvent>();
				auto ew = UI::get().get_element<UIEventWindow>();
				if (!ew)
					ew = UI::get().emplace<UIEventWindow>();
				else if (!ew->is_active())
					ew->makeactive();

				remove_menus();
				break;
			}
			case Buttons::BT_EVENT_DAILY:
			{
				UI::get().emplace<UIDailyGift>();

				remove_menus();
				break;
			}
		}

		return Button::State::NORMAL;
	}

	void UIStatusBar::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
			{
				if (!menu_active && !setting_active && !community_active && !character_active && !event_active)
					toggle_setting();
				else
					remove_menus();
			}
			else if (keycode == KeyAction::Id::RETURN)
			{
				// 修复：buttons 可能有缺失条目，禁止 operator[] 只读访问（会插入空 unique_ptr 再解引用）。
				auto btn_at = [this](uint16_t id) -> Button*
				{
					auto it = buttons.find(id);
					return (it != buttons.end()) ? it->second.get() : nullptr;
				};

				for (size_t i = Buttons::BT_MENU_QUEST; i <= Buttons::BT_EVENT_DAILY; i++)
					if (btn_at(static_cast<uint16_t>(i))
						&& btn_at(static_cast<uint16_t>(i))->get_state() == Button::State::MOUSEOVER)
						button_pressed(static_cast<uint16_t>(i));
			}
			else if (keycode == KeyAction::Id::UP || keycode == KeyAction::Id::DOWN)
			{
				uint16_t min_id, max_id;

				if (menu_active)
				{
					min_id = Buttons::BT_MENU_QUEST;
					max_id = Buttons::BT_MENU_CLAIM;
				}
				else if (setting_active)
				{
					min_id = Buttons::BT_SETTING_CHANNEL;
					max_id = Buttons::BT_SETTING_QUIT;
				}
				else if (community_active)
				{
					min_id = Buttons::BT_COMMUNITY_FRIENDS;
					max_id = Buttons::BT_COMMUNITY_MAPLECHAT;
				}
				else if (character_active)
				{
					min_id = Buttons::BT_CHARACTER_INFO;
					max_id = Buttons::BT_CHARACTER_ITEM;
				}
				else if (event_active)
				{
					min_id = Buttons::BT_EVENT_SCHEDULE;
					max_id = Buttons::BT_EVENT_DAILY;
				}

				uint16_t id = min_id;

				auto btn_at = [this](uint16_t bid) -> Button*
				{
					auto it = buttons.find(bid);
					return (it != buttons.end()) ? it->second.get() : nullptr;
				};

				for (size_t i = min_id; i <= max_id; i++)
				{
					Button* b = btn_at(static_cast<uint16_t>(i));
					if (b && b->get_state() != Button::State::NORMAL)
					{
						id = static_cast<uint16_t>(i);

						b->set_state(Button::State::NORMAL);
						break;
					}
				}

				if (keycode == KeyAction::Id::DOWN)
				{
					if (id < max_id)
						id++;
					else
						id = min_id;
				}
				else if (keycode == KeyAction::Id::UP)
				{
					if (id > min_id)
						id--;
					else
						id = max_id;
				}

				if (Button* b = btn_at(id))
					b->set_state(Button::State::MOUSEOVER);
			}
			else if (keycode == 'W' || keycode == 'w')
			{
				std::string pre;
				if (auto ci = UI::get().get_element<UICharInfo>())
					pre = ci->get_target_name();
				if (!pre.empty()) {
					std::string to = pre;
					UI::get().emplace<UISoftKey>([to](const std::string& msg) {
						if (msg.empty()) return;
						WhisperPacket(to, msg).dispatch();
						LOG(LOG_DEBUG, "UIStatusBar: SoftKey whisper to=" << to);
					}, []() {}, "Whisper msg");
				} else {
					UI::get().emplace<UISoftKey>([](const std::string& to) {
						if (to.empty()) return;
						UI::get().emplace<UISoftKey>([to](const std::string& msg) {
							if (msg.empty()) return;
							WhisperPacket(to, msg).dispatch();
							LOG(LOG_DEBUG, "UIStatusBar: SoftKey whisper to=" << to);
						}, []() {}, "Whisper msg");
					}, []() {}, "Whisper to");
				}
			}
			else if (keycode == 'S' || keycode == 's')
			{
				UI::get().emplace<UISoftKey>([](const std::string& msg) {
					if (msg.empty()) return;
					GeneralChatPacket(msg, true).dispatch();
				}, []() {}, "Chat");
			}
			else if (keycode == 'E' || keycode == 'e')
			{
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					try {
						int32_t emote = std::stoi(s);
						if (emote >= 0) {
							FaceExpressionPacket(emote).dispatch();
							LOG(LOG_DEBUG, "UIStatusBar: SoftKey emote=" << emote);
						}
					} catch (...) {}
				}, []() {}, "Emote id");
			}
			else if (keycode == 'C' || keycode == 'c')
			{
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					try {
						int ch = std::stoi(s);
						if (ch >= 0 && ch < 256) {
							ChangeChannelPacket(static_cast<uint8_t>(ch)).dispatch();
							LOG(LOG_DEBUG, "UIStatusBar: SoftKey channel=" << ch);
						}
					} catch (...) {}
				}, []() {}, "Channel #");
			}
			else if (keycode == 'H' || keycode == 'h')
			{
				int32_t from_inv = 0;
				if (auto inv = UI::get().get_element<UIItemInventory>()) {
					int16_t slot = inv->get_selected_slot();
					if (slot > 0)
						from_inv = Stage::get().get_player().get_inventory()
							.get_item_id(inv->get_tab(), slot);
				}
				if (from_inv > 0) {
					UseChairPacket(from_inv).dispatch();
					LOG(LOG_DEBUG, "UIStatusBar: SoftKey chair=" << from_inv);
				} else {
					UI::get().emplace<UISoftKey>([](const std::string& s) {
						try {
							int32_t id = std::stoi(s);
							if (id > 0) {
								UseChairPacket(id).dispatch();
								LOG(LOG_DEBUG, "UIStatusBar: SoftKey chair=" << id);
							}
						} catch (...) {}
					}, []() {}, "Chair itemid");
				}
			}
			else if (keycode == 'X' || keycode == 'x')
			{
				CancelChairPacket().dispatch();
				LOG(LOG_DEBUG, "UIStatusBar: CancelChair");
			}
			else if (keycode == 'D' || keycode == 'd')
			{
				UseDoorPacket(true).dispatch();
				LOG(LOG_DEBUG, "UIStatusBar: UseDoor party");
			}
			else if (keycode == 'K' || keycode == 'k')
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				auto loot = Stage::get().get_drops().find_loot_at(pos);
				if (loot.first > 0) {
					PickupItemPacket(loot.first, loot.second).dispatch();
					LOG(LOG_DEBUG, "UIStatusBar: Pickup nearest oid=" << loot.first);
				} else {
					UI::get().emplace<UISoftKey>([](const std::string& s) {
						try {
							int32_t oid = std::stoi(s);
							if (oid > 0) {
								Point<int16_t> p = Stage::get().get_player().get_position();
								PickupItemPacket(oid, p).dispatch();
								LOG(LOG_DEBUG, "UIStatusBar: SoftKey pickup oid=" << oid);
							}
						} catch (...) {}
					}, []() {}, "Pickup oid");
				}
			}
			else if (keycode == 'R' || keycode == 'r')
			{
				int32_t near = 0;
				if (MapObjects* objs = Stage::get().get_reactors().get_reactors()) {
					Point<int16_t> p = Stage::get().get_player().get_position();
					int64_t bestd = -1;
					for (auto it = objs->begin(); it != objs->end(); ++it) {
						if (!it->second) continue;
						Point<int16_t> np = it->second->get_position();
						int64_t dx = static_cast<int64_t>(np.x()) - p.x();
						int64_t dy = static_cast<int64_t>(np.y()) - p.y();
						int64_t d = dx * dx + dy * dy;
						if (bestd < 0 || d < bestd) { bestd = d; near = it->second->get_oid(); }
					}
				}
				Point<int16_t> pos = Stage::get().get_player().get_position();
				if (near > 0) {
					DamageReactorPacket(near, pos, 0, 0).dispatch();
					LOG(LOG_DEBUG, "UIStatusBar: DamageReactor nearest oid=" << near);
				} else {
					UI::get().emplace<UISoftKey>([](const std::string& s) {
						try {
							int32_t oid = std::stoi(s);
							if (oid > 0) {
								Point<int16_t> p = Stage::get().get_player().get_position();
								DamageReactorPacket(oid, p, 0, 0).dispatch();
								LOG(LOG_DEBUG, "UIStatusBar: SoftKey reactor oid=" << oid);
							}
						} catch (...) {}
					}, []() {}, "Reactor oid");
				}
			}
			else if (keycode == 'I' || keycode == 'i')
			{
				int32_t from_inv = 0;
				if (auto uii = UI::get().get_element<UIItemInventory>()) {
					int16_t slot = uii->get_selected_slot();
					if (slot > 0)
						from_inv = Stage::get().get_player().get_inventory().get_item_id(uii->get_tab(), slot);
				}
				if (from_inv > 0) {
					UseItemEffectPacket(from_inv).dispatch();
					LOG(LOG_DEBUG, "UIStatusBar: effect id=" << from_inv);
				} else {
					UI::get().emplace<UISoftKey>([](const std::string& s) {
						try {
							int32_t id = std::stoi(s);
							if (id > 0) {
								UseItemEffectPacket(id).dispatch();
								LOG(LOG_DEBUG, "UIStatusBar: SoftKey effect id=" << id);
							}
						} catch (...) {}
					}, []() {}, "Effect itemid");
				}
			}
			else if (keycode == 'B' || keycode == 'b')
			{
				CloseChalkboardPacket().dispatch();
				LOG(LOG_DEBUG, "UIStatusBar: CloseChalkboard");
			}
			else if (keycode == 'N' || keycode == 'n')
			{
				int32_t near = 0;
				if (MapObjects* npcs = Stage::get().get_npcs().get_npcs()) {
					Point<int16_t> p = Stage::get().get_player().get_position();
					int64_t bestd = -1;
					for (auto it = npcs->begin(); it != npcs->end(); ++it) {
						if (!it->second) continue;
						Point<int16_t> np = it->second->get_position();
						int64_t dx = static_cast<int64_t>(np.x()) - p.x();
						int64_t dy = static_cast<int64_t>(np.y()) - p.y();
						int64_t d = dx * dx + dy * dy;
						if (bestd < 0 || d < bestd) { bestd = d; near = it->second->get_oid(); }
					}
				}
				if (near > 0) {
					TalkToNPCPacket(near).dispatch();
					LOG(LOG_DEBUG, "UIStatusBar: TalkToNPC nearest oid=" << near);
				} else {
					UI::get().emplace<UISoftKey>([](const std::string& s) {
						try {
							int32_t oid = std::stoi(s);
							if (oid <= 0) return;
							TalkToNPCPacket(oid).dispatch();
							LOG(LOG_DEBUG, "UIStatusBar: SoftKey TalkToNPC oid=" << oid);
						} catch (...) {}
					}, []() {}, "NPC oid");
				}
			}
			else if (keycode == 'T' || keycode == 't')
			{
				auto ui = UI::get().get_element<UIItemSearch>();
				if (!ui) ui = UI::get().emplace<UIItemSearch>();
				else if (!ui->is_active()) ui->makeactive();
			}
			else if (keycode == 'P' || keycode == 'p')
			{
				auto ui = UI::get().get_element<UISkillUp>();
				if (!ui) ui = UI::get().emplace<UISkillUp>();
				else if (!ui->is_active()) ui->makeactive();
			}
			else if (keycode == 'F' || keycode == 'f')
			{
				auto ui = UI::get().get_element<UIFadeYesNo>();
				if (!ui) ui = UI::get().emplace<UIFadeYesNo>();
				else if (!ui->is_active()) ui->makeactive();
			}
			else if (keycode == 'G' || keycode == 'g')
			{
				auto ui = UI::get().get_element<UIGMBoard>();
				if (!ui) ui = UI::get().emplace<UIGMBoard>();
				else if (!ui->is_active()) ui->makeactive();
			}
			else if (keycode == 'Q' || keycode == 'q')
			{
				auto ui = UI::get().get_element<UIQuestLog>();
				if (!ui)
					ui = UI::get().emplace<UIQuestLog>(
						Stage::get().get_player().get_quests());
				else if (!ui->is_active())
					ui->makeactive();
			}
			else if (keycode == 'Z' || keycode == 'z')
			{
				auto ui = UI::get().get_element<UIWorldMap>();
				if (!ui) ui = UI::get().emplace<UIWorldMap>();
				else if (!ui->is_active()) ui->makeactive();
			}
			else if (keycode == 'L' || keycode == 'l')
			{
				auto ui = UI::get().get_element<UIUserList>();
				if (!ui) ui = UI::get().emplace<UIUserList>(0);
				else if (!ui->is_active()) ui->makeactive();
			}
			else if (keycode == 'A' || keycode == 'a')
			{
				auto ui = UI::get().get_element<UIAdmin>();
				if (!ui) ui = UI::get().emplace<UIAdmin>();
				else if (!ui->is_active()) ui->makeactive();
			}
			else if (keycode == 'Y' || keycode == 'y')
			{
				auto ui = UI::get().get_element<UIFamily>();
				if (!ui) ui = UI::get().emplace<UIFamily>();
				else if (!ui->is_active()) ui->makeactive();
			}
			else if (keycode == 'V' || keycode == 'v')
			{
				auto ui = UI::get().get_element<UICoupon>();
				if (!ui) ui = UI::get().emplace<UICoupon>();
				else if (!ui->is_active()) ui->makeactive();
			}
			else if (keycode == 'M' || keycode == 'm')
			{
				auto ui = UI::get().get_element<UIMaker>();
				if (!ui) ui = UI::get().emplace<UIMaker>();
				else if (!ui->is_active()) ui->makeactive();
			}
		}
	}

	bool UIStatusBar::is_in_range(Point<int16_t> cursor_position) const
	{
		Point<int16_t> pos;
		Rectangle<int16_t> bounds;

		if (!character_active && !community_active && !event_active && !menu_active && !setting_active)
		{
			pos = Point<int16_t>(position_x, position_y);
			bounds = Rectangle<int16_t>(pos, pos + dimension);
		}
		else
		{
			uint8_t button_count;
			int16_t pos_y_adj;

			if (character_active)
			{
				pos = character_pos;
				button_count = 5;
				pos_y_adj = 248;
			}
			else if (community_active)
			{
				pos = community_pos;
				button_count = 4;
				pos_y_adj = 301;
			}
			else if (event_active)
			{
				pos = event_pos;
				button_count = 2;
				pos_y_adj = 417;
			}
			else if (menu_active)
			{
				pos = menu_pos;
				button_count = 11;
				pos_y_adj = -90;
			}
			else if (setting_active)
			{
				pos = setting_pos;
				button_count = 5;
				pos_y_adj = 248;
			}

			pos_y_adj += VHEIGHT - 600;

			Point<int16_t> pos_adj = get_quickslot_pos();
			pos = Point<int16_t>(pos.x(), std::abs(pos.y()) + pos_y_adj) + pos_adj;

			uint16_t end_y = std::floor(28.2 * button_count);

			bounds = Rectangle<int16_t>(pos, pos + Point<int16_t>(113, end_y + 35));
		}

		return bounds.contains(cursor_position);
	}

	UIElement::Type UIStatusBar::get_type() const
	{
		return TYPE;
	}

	void UIStatusBar::toggle_qs()
	{
		if (classic_mode)
		{
			quickslot_active = !quickslot_active;
			sync_classic_qs_buttons();
			return;
		}

		if (!menu_active && !setting_active && !community_active && !character_active && !event_active)
			toggle_qs(!quickslot_active);
	}

	void UIStatusBar::toggle_qs(bool quick_slot_active)
	{
		if (classic_mode)
		{
			quickslot_active = quick_slot_active;
			sync_classic_qs_buttons();
			return;
		}

		if (quickslot_active == quick_slot_active)
			return;

		quickslot_active = quick_slot_active;

		// 修复：禁止 operator[] 只读访问（可能插入空 unique_ptr 再解引用）。
		auto btn_at = [this](uint16_t id) -> Button*
		{
			auto it = buttons.find(id);
			return (it != buttons.end()) ? it->second.get() : nullptr;
		};

		if (Button* b = btn_at(Buttons::BT_FOLD_QS))
			b->set_active(quickslot_active);
		if (Button* b = btn_at(Buttons::BT_EXTEND_QS))
			b->set_active(!quickslot_active);

		if (VWIDTH > 800)
		{
			for (uint16_t bid : { Buttons::BT_CASHSHOP, Buttons::BT_MENU, Buttons::BT_OPTIONS,
				Buttons::BT_CHARACTER, Buttons::BT_COMMUNITY, Buttons::BT_EVENT })
			{
				if (Button* b = btn_at(bid))
					b->set_active(!quickslot_active);
			}
		}
	}

	Button* UIStatusBar::button_at(uint16_t id) const
	{
		auto it = buttons.find(id);
		return (it != buttons.end()) ? it->second.get() : nullptr;
	}

	void UIStatusBar::set_buttons_active(std::initializer_list<uint16_t> ids, bool active)
	{
		for (uint16_t id : ids)
		{
			if (Button* b = button_at(id))
				b->set_active(active);
		}
	}

	void UIStatusBar::toggle_menu()
	{
		remove_active_menu(MenuType::MENU);

		menu_active = !menu_active;

		set_buttons_active({ Buttons::BT_MENU_ACHIEVEMENT, Buttons::BT_MENU_AUCTION,
			Buttons::BT_MENU_BATTLE, Buttons::BT_MENU_CLAIM, Buttons::BT_MENU_FISHING,
			Buttons::BT_MENU_HELP, Buttons::BT_MENU_MEDAL, Buttons::BT_MENU_MONSTER_COLLECTION,
			Buttons::BT_MENU_MONSTER_LIFE, Buttons::BT_MENU_QUEST, Buttons::BT_MENU_UNION }, menu_active);

		if (menu_active)
		{
			if (Button* b = button_at(Buttons::BT_MENU_QUEST))
				b->set_state(Button::State::MOUSEOVER);

			Sound(Sound::Name::DLGNOTICE).play();
		}
	}

	void UIStatusBar::toggle_setting()
	{
		remove_active_menu(MenuType::SETTING);

		setting_active = !setting_active;

		set_buttons_active({ Buttons::BT_SETTING_CHANNEL, Buttons::BT_SETTING_QUIT,
			Buttons::BT_SETTING_JOYPAD, Buttons::BT_SETTING_KEYS, Buttons::BT_SETTING_OPTION }, setting_active);

		if (setting_active)
		{
			if (Button* b = button_at(Buttons::BT_SETTING_CHANNEL))
				b->set_state(Button::State::MOUSEOVER);

			Sound(Sound::Name::DLGNOTICE).play();
		}
	}

	void UIStatusBar::toggle_community()
	{
		remove_active_menu(MenuType::COMMUNITY);

		community_active = !community_active;

		set_buttons_active({ Buttons::BT_COMMUNITY_PARTY, Buttons::BT_COMMUNITY_FRIENDS,
			Buttons::BT_COMMUNITY_GUILD, Buttons::BT_COMMUNITY_MAPLECHAT }, community_active);

		if (community_active)
		{
			if (Button* b = button_at(Buttons::BT_COMMUNITY_FRIENDS))
				b->set_state(Button::State::MOUSEOVER);

			Sound(Sound::Name::DLGNOTICE).play();
		}
	}

	void UIStatusBar::toggle_character()
	{
		remove_active_menu(MenuType::CHARACTER);

		character_active = !character_active;

		set_buttons_active({ Buttons::BT_CHARACTER_INFO, Buttons::BT_CHARACTER_EQUIP,
			Buttons::BT_CHARACTER_ITEM, Buttons::BT_CHARACTER_SKILL, Buttons::BT_CHARACTER_STAT }, character_active);

		if (character_active)
		{
			if (Button* b = button_at(Buttons::BT_CHARACTER_INFO))
				b->set_state(Button::State::MOUSEOVER);

			Sound(Sound::Name::DLGNOTICE).play();
		}
	}

	void UIStatusBar::toggle_event()
	{
		remove_active_menu(MenuType::EVENT);

		event_active = !event_active;

		set_buttons_active({ Buttons::BT_EVENT_DAILY, Buttons::BT_EVENT_SCHEDULE }, event_active);

		if (event_active)
		{
			if (Button* b = button_at(Buttons::BT_EVENT_SCHEDULE))
				b->set_state(Button::State::MOUSEOVER);

			Sound(Sound::Name::DLGNOTICE).play();
		}
	}

	void UIStatusBar::remove_menus()
	{
		if (menu_active)
			toggle_menu();
		else if (setting_active)
			toggle_setting();
		else if (community_active)
			toggle_community();
		else if (character_active)
			toggle_character();
		else if (event_active)
			toggle_event();
	}

	void UIStatusBar::remove_active_menu(MenuType type)
	{
		for (size_t i = Buttons::BT_MENU_QUEST; i <= Buttons::BT_EVENT_DAILY; i++)
		{
			if (Button* b = button_at(static_cast<uint16_t>(i)))
				b->set_state(Button::State::NORMAL);
		}

		if (menu_active && type != MenuType::MENU)
			toggle_menu();
		else if (setting_active && type != MenuType::SETTING)
			toggle_setting();
		else if (community_active && type != MenuType::COMMUNITY)
			toggle_community();
		else if (character_active && type != MenuType::CHARACTER)
			toggle_character();
		else if (event_active && type != MenuType::EVENT)
			toggle_event();
	}

	void UIStatusBar::sync_classic_qs_buttons()
	{
		if (!classic_mode)
			return;

		auto btn_at = [this](uint16_t id) -> Button*
		{
			auto it = buttons.find(id);
			return (it != buttons.end()) ? it->second.get() : nullptr;
		};

		if (Button* b = btn_at(Buttons::BT_CLASSIC_QS))
			b->set_active(quickslot_active);

		if (Button* b = btn_at(Buttons::BT_CLASSIC_QSD))
			b->set_active(!quickslot_active);
	}

	Point<int16_t> UIStatusBar::get_quickslot_pos() const
	{
		if (quickslot_active)
		{
			if (VWIDTH == 800)
				return Point<int16_t>(0, -73);
			else
				return Point<int16_t>(0, -31);
		}

		return Point<int16_t>(0, 0);
	}

	bool UIStatusBar::is_menu_active()
	{
		return menu_active || setting_active || community_active || character_active || event_active;
	}

	float UIStatusBar::getexppercent() const
	{
		int16_t level = stats.get_stat(MapleStat::Id::LEVEL);

		if (level >= ExpTable::LEVELCAP)
			return 0.0f;

		int64_t exp = stats.get_exp();

		return static_cast<float>(
			static_cast<double>(exp) / ExpTable::values[level]
			);
	}

	float UIStatusBar::gethppercent() const
	{
		int16_t hp = stats.get_stat(MapleStat::Id::HP);
		int32_t maxhp = stats.get_total(EquipStat::Id::HP);

		return static_cast<float>(hp) / maxhp;
	}

	float UIStatusBar::getmppercent() const
	{
		int16_t mp = stats.get_stat(MapleStat::Id::MP);
		int32_t maxmp = stats.get_total(EquipStat::Id::MP);

		return static_cast<float>(mp) / maxmp;
	}

	void UIStatusBar::init_classic()
	{
		nl::node sb = UiAssetResolve::status_bar_root();
		nl::node base = sb["base"];
		nl::node gauge = sb["gauge"];

		classic_backgrnd = Texture(base["backgrnd"]);
		classic_backgrnd2 = Texture(base["backgrnd2"]);
		classic_graduation = Texture(gauge["graduation"]);
		classic_gauge_bar = Texture(gauge["bar"]);
		classic_gauge_gray = Texture(gauge["gray"]);

		// 快捷栏面板在下方按格数选型后赋值，此处不提前占位。

		// nxcache 脏 origin 会把底条整块抬离屏幕底边 / 左缘，统一清零为 TL 锚点。
		auto zero_origin = [](Texture& tex)
		{
			if (!tex.is_valid())
				return;

			Point<int16_t> o = tex.get_origin();

			if (o.x() != 0 || o.y() != 0)
				tex.shift(o);
		};

		zero_origin(classic_backgrnd);
		zero_origin(classic_backgrnd2);
		zero_origin(classic_graduation);
		zero_origin(classic_gauge_bar);
		zero_origin(classic_gauge_gray);

		// ============ 经典底条布局（左贴边 + 右侧面板贴右缘）============
		// 权威来源：
		//  a) IDA BeiDou v17.7 脚本化 CreateCtrl(node, id, x, y, type)（sub_8D01B2）：
		//       BtShop=1000/(573,543)  BtNPT=1001/(629,543)  BtMenu=1002/(685,543)
		//       BtShort=1007/(741,543)  EquipKey=2000/(618,515) InvenKey=2001/(648,515)
		//       StatKey=2002/(678,515) SkillKey=2003/(708,515) KeySet=2004/(738,515)
		//       QuickSlot=2005/(768,515) QuickSlotD=2006/(573,515)
		//     创建窗体高 578、top≈22 → 底条相对 y = CreateY - 507：543→36、515→8。
		//     x 末端 741+54=795 / 768+28=796，证明设计网格宽 800（而非右移贴边）。
		//  b) StatusBar.img 节点实测（nx_inspect，origin 全部 (0,0)）：
		//       base/backgrnd  817x71  ← 固定尺寸精灵，旧代码拉伸到 VWIDTH 会失真
		//       base/backgrnd2 570x71  ← 叠加层：上带亮板 y6..29（名字）；
		//                                下带深色量表面板 y36..69 x0..562，LV 盒 x0..84
		//                                「LV.」字样已烘焙在 x10..29 / y47..59 → 不重画
		//       gauge/bar & gauge/graduation 340x31 三联图集（同行排列，非上下叠放）：
		//         填充 HP 红 x2..106 / MP 蓝 x110..214 / EXP 绿 x223..337，均 y15..28
		//         边框 HP x0..107  / MP x109..215  / EXP x222..339，均 y13..30
		//       gauge/hpFlash|mpFlash 109x18（5 帧闪光覆盖层）
		//       number 字符集 0-9(5x7)+Lbracket/Rbracket(2x9)+slash/percent(7x7)，无 '.' 字形
		//       FontMemo 0-9(6x7，'1' 为 3x7) 位图数字
		//       key/0..6 = 28x11 / 18x11 / 19x11 / 22x13 / 24x11 / 19x11 / 21x11 键名贴纸
		//       base/quickSlot08 151x80、quickSlot26 466x80、quickSlot32 571x80
		//         （格 28x28、首格 (9,9)、列距 35、行距 34 —— 见文件顶部 QS_* 推导）
		//  c) 正确端截图：底栏左贴边、右快捷格贴右边，backgrnd 横向铺满。
		//     「800 设计网格 + right_adj」是旧版整体居中的产物，已废弃：左侧贴边、
		//     右侧面板贴 VWIDTH 右缘即可，无需再算右偏移量。

		// 参考截图（进入游戏页.png，1280x720，底栏 position 顶 = 649）逐像素判定：
		//   graduation 暗边框整幅跨 x218..557（= 340，与图集同宽）→ GAUGE_X=218、
		//   三段间距就是图集打包间距 0/109/222（旧「128/255」是把商城红按钮当成了段界）。
		//   边框 y700..717（18 行）、HP 红填充 y702..715（14 行）→ 图集 y0 = 687 → GAUGE_Y=38。
		constexpr int16_t GAUGE_X = 218;
		constexpr int16_t GAUGE_Y = 38;

		hpmp_pos = Point<int16_t>(GAUGE_X, GAUGE_Y);
		exp_pos = Point<int16_t>(GAUGE_X, GAUGE_Y);
		// number 字库锚点 = 参考截图 Lbracket 实测位（底栏 rel 坐标，数字基线偏差 0..1px）：
		// HP '[' 屏 x237 y690 / MP '[' x349 / EXP '[' x472 → 各段起点 + 烘焙标签宽度。
		hpset_pos = Point<int16_t>(static_cast<int16_t>(GAUGE_X + SEG_HP + 19), 41);
		mpset_pos = Point<int16_t>(static_cast<int16_t>(GAUGE_X + SEG_MP + 22), 41);
		statset_pos = Point<int16_t>(static_cast<int16_t>(GAUGE_X + SEG_EXP + 32), 41);
		// 等级徽章：Basic.img/LevelNo 橙盒数字 11x13，参考实测屏 x50..60 / y696..708
		// → rel (50,47)。多位数按 11+1 推进并以单字中心 x=55 居中。
		// 「LV.」前缀与右侧方括号：参考底板上没有可画的 Bracket（x61..84 全暗），
		// ShowLevel 仅作为 LevelNo 缺失时的回退字库。
		lvbadge_pos = Point<int16_t>(50, 47);
		// 芯片内两行（参考实测**墨迹顶** rel y：职业名 40、角色名 56；旧值 21/39
		// 落在聊天输入行 → 白字白底「职业没有显示」）。
		// 这里 y 槽存的是量到的墨迹顶，不是绘制点：绘制点在 draw_classic 里按
		// 本行文字自己的度量反解（R27-B1b Text::y_for_ink_top，SimHei 12px 的
		// CJK bitmap_top 实测 10 而非 r15 手抄的 12 ⇒ 此前两行各低 2/3px，
		// 就是用户说的「底栏职业/名称偏下」）。
		joblabel_pos = Point<int16_t>(87, 40);
		namelabel_pos = Point<int16_t>(87, 56);

		// 三联填充：同一图集按分段裁剪，HP/MP/EXP 各绘在各自分段起点。
		hpbar = Gauge(Gauge::Type::CLIP, classic_gauge_bar, SEG_W_HP, 0.0f);
		hpbar.set_clip_left(SEG_HP);
		mpbar = Gauge(Gauge::Type::CLIP, classic_gauge_bar, SEG_W_MP, 0.0f);
		mpbar.set_clip_left(SEG_MP);
		expbar = Gauge(Gauge::Type::CLIP, classic_gauge_bar, SEG_W_EXP, 0.0f);
		expbar.set_clip_left(SEG_EXP);

		// 闪光覆盖层对齐各分段可视框：图集边框自 y13 起（参考实测 700-687），
		// 109x18 再向左外扩 1px。（此前误用 GAUGE_Y，闪光画到了亮板上。）
		// 传 hpFlash/mpFlash 根而非 ["0"]：Animation 只有在拿到多帧根时才建 5 帧动画。
		if (gauge["hpFlash"]["0"])
		{
			classic_hp_flash_sprite = Sprite(gauge["hpFlash"], hpmp_pos + Point<int16_t>(-1, 13));
			classic_hp_flash = 0;
		}

		if (gauge["mpFlash"]["0"])
		{
			classic_mp_flash_sprite = Sprite(gauge["mpFlash"], hpmp_pos + Point<int16_t>(static_cast<int16_t>(SEG_MP - 1), 13));
			classic_mp_flash = 0;
		}

		if (sb["number"])
		{
			statset = Charset(sb["number"], Charset::Alignment::LEFT);
			hpmpset = Charset(sb["number"], Charset::Alignment::LEFT);
		}
#ifdef USE_NX
		{
			nl::node basic = nl::nx::UI["Basic.img"];

			if (basic["LevelNo"])
				classic_levelno = Charset(basic["LevelNo"], Charset::Alignment::LEFT);

			nl::node showlevel = basic["ShowLevel"];

			if (showlevel)
			{
				classic_showlevel = Charset(showlevel["0"], Charset::Alignment::LEFT);
				classic_showlevel_bracket = Texture(showlevel["Bracket"]["right"]);
			}
		}
#endif

		namelabel = OutlinedText(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::DARKGREY);

		// MapleButton 锚点 = CreateCtrl TL + canvas origin（StatusBar.img 各按钮 origin 实测均为 (0,0)，
		// 故锚点即网格坐标本身；保留 origin 相加以防更换资源后 origin 非零）。
		auto add_btn = [&](Buttons id, nl::node node, int16_t x, int16_t y)
		{
			if (!node)
				return;

			Point<int16_t> origin;
			if (node["normal"]["0"])
				origin = Texture(node["normal"]["0"]).get_origin();

			buttons[id] = std::make_unique<MapleButton>(
				node,
				Point<int16_t>(static_cast<int16_t>(x + origin.x()), static_cast<int16_t>(y + origin.y()))
			);
		};

		// 快捷栏面板按格数选型（W3/RC14）：32 格=quickSlot32(571x80,16 列)、26 格=quickSlot26(466x80,13 列)、
		// 窄屏 8 格=quickSlot08(151x80,4 列)。键位表现为 26 键 → 宽屏取 quickSlot26；
		// 表扩到 32 时自动切 quickSlot32。quickSlot 与 quickSlot26 像素全同（已 diff 证实），
		// 故这里不再回退到 quickSlot。注意：quickSlot* 在 StatusBar.img/base/ 下（不是顶层）。
		const bool wide = VWIDTH >= 1024;
		classic_qs_wide = wide;
		if (wide)
		{
			classic_qs_count = CLASSIC_QS_SLOTS;
			classic_qs_cols = classic_qs_count == 32 ? 16 : 13;
			classic_quickslot = Texture(base[classic_qs_count == 32 ? "quickSlot32" : "quickSlot26"]);
		}
		else
		{
			classic_qs_count = 8;
			classic_qs_cols = 4;
			classic_quickslot = Texture(base["quickSlot08"]);
		}
		zero_origin(classic_quickslot);

		const int16_t qs_panel_w = classic_quickslot.get_dimensions().x();
		// 多分辨率（用户决策，明确偏离原生）：原生是底栏左贴边 + 快捷格贴 VWIDTH
		// 右缘（见本函数上方注释 c）。分辨率过宽时把「1280 设计网格」整组水平居中；
		// 1280 及以下保持原生布局。底栏背景仍通栏铺满，不随内容组偏移。
		const int16_t hud_center_x = VWIDTH > ResolutionTiers::LOGIN_W
			? static_cast<int16_t>((VWIDTH - ResolutionTiers::LOGIN_W) / 2)
			: 0;
		const int16_t hud_grid_w = hud_center_x > 0 ? ResolutionTiers::LOGIN_W : VWIDTH;

		// 参考端：面板贴底条右缘（1280 屏 x=814=1280-466，实测与 quickSlot26 宽一致）；
		// 面板自身 y2 是 1px 高亮线，参考截图该线在 y643 → 面板顶 y=641 = 底条 649 - 8。
		quickslot_pos = Point<int16_t>(
			static_cast<int16_t>(hud_grid_w - qs_panel_w),
			-8
		);
		const int16_t group_x = static_cast<int16_t>(quickslot_pos.x() - (wide ? 241 : 75));

		// IDA CreateCtrl：y=543 → 底条 36；y=515 → 底条 8。按钮簇整体定位于 group_x。
		add_btn(Buttons::BT_CLASSIC_SHOP, sb["BtShop"], group_x + 0, 36);
		add_btn(Buttons::BT_CLASSIC_NPT, sb["BtNPT"], group_x + 56, 36);
		add_btn(Buttons::BT_CLASSIC_MENU, sb["BtMenu"], group_x + 112, 36);
		add_btn(Buttons::BT_CLASSIC_SHORT, sb["BtShort"], group_x + 168, 36);

		add_btn(Buttons::BT_CLASSIC_EQUIP, sb["EquipKey"], group_x + 45, 8);
		add_btn(Buttons::BT_CLASSIC_INV, sb["InvenKey"], group_x + 75, 8);
		add_btn(Buttons::BT_CLASSIC_STAT, sb["StatKey"], group_x + 105, 8);
		add_btn(Buttons::BT_CLASSIC_SKILL, sb["SkillKey"], group_x + 135, 8);
		add_btn(Buttons::BT_CLASSIC_KEYSET, sb["KeySet"], group_x + 165, 8);
		// 快捷栏收起(上箭头 QuickSlot)/展开(下箭头 QuickSlotD)为同位单按钮：
		// 图标随 quickslot_active 上下切换，而非两颗按钮并存（用户实测反馈）。
		add_btn(Buttons::BT_CLASSIC_QS, sb["QuickSlot"], group_x + 195, 8);
		add_btn(Buttons::BT_CLASSIC_QSD, sb["QuickSlotD"], group_x + 195, 8);

		// 举报/密语：参考端在按钮簇首两格（y=8）。
		if (sb["BtClaim"])
			add_btn(Buttons::BT_CLASSIC_CLAIM, sb["BtClaim"], group_x - 15, 8);
		if (sb["BtWhisper"])
			add_btn(Buttons::BT_CLASSIC_WHISPER, sb["BtWhisper"], group_x + 15, 8);

		// 量规数值改画 number 位图字库（W1）；标签 HP/MP/EXP 用 gauge/bar 图集烘焙，不建 Text。
		classic_exp_dot = ColorBox(1, 1, Color::Name::WHITE, 1.0f);
		joblabel = OutlinedText(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::DARKGREY);

		// 键帽层（W3）：key/0..6 = Shift/Ins/Hm/Pup/Ctrl/Del/End 贴纸（key/7 本包为红箭头，非 Pdn）；
		// 字母键名用描边文字回退，中文名称走 Text maxwidth（T12b 度量换行）。
		for (size_t i = 0; i < 7; i++)
			classic_key_label[i] = Texture(sb["key"][static_cast<int16_t>(i)]);
		// 参考截图逐像素取证：原生键帽字（数字/字母）字身 = #AABBCC + 1px 白描边，
		// 数字字形与 StatusBar.img/FontMemo 位图完全同形（6x7，笔画 2px）。本包只有
		// FontMemo 的 0-9、无字母，且 Texture 通道不支持着色，故数字也改走同一套
		// 描边文字，至少保证颜色/描边/字号一致，不再出现白字压白格看不见。
		classic_qs_keytext = OutlinedText(Text::Font::A11B, Text::Alignment::LEFT, Color::Name::KEYCAPBLUE, Color::Name::WHITE);
		classic_qs_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "", 28);

		quickslot[0] = classic_quickslot;
		quickslot_active = true;
		quickslot_adj = Point<int16_t>(0, 0);

		// 1280 及以下：原生「左贴边 + 底贴边」；过宽：整组按 1280 设计网格水平居中
		// （用户决策，唯一明确偏离原生定位处）。按钮命中基于 position，自动跟随偏移。
		position = Point<int16_t>(hud_center_x, static_cast<int16_t>(VHEIGHT - CLASSIC_BAR_H));
		position_x = position.x();
		position_y = static_cast<int16_t>(position.y() - 9);
		dimension = Point<int16_t>(VWIDTH, 80);

		refresh_classic_qs_icons();
	}

	void UIStatusBar::refresh_classic_qs_icons()
	{
		const Keyboard& keyboard = UI::get().get_keyboard();

#ifdef USE_NX
		nl::node key_icons = nl::nx::UI["UIWindow.img"]["KeyConfig"]["icon"];
		if (!key_icons)
			key_icons = nl::nx::UI["UIWindow2.img"]["KeyConfig"]["icon"];
#endif

		for (size_t i = 0; i < CLASSIC_QS_SLOTS; i++)
		{
			classic_qs_icons[i] = Texture();
			classic_qs_names[i].clear();
			Keyboard::Mapping mapping = keyboard.get_maple_mapping(CLASSIC_QS_KEYS[i]);

			if (mapping.type == KeyType::Id::SKILL)
			{
				const SkillData& skill = SkillData::get(mapping.action);
				const Texture& icon = skill.get_icon(SkillData::Icon::NORMAL);

				if (icon.is_valid())
					classic_qs_icons[i] = icon;

				classic_qs_names[i] = skill.get_name();
			}
			else if (mapping.type == KeyType::Id::ITEM)
			{
				const ItemData& item = ItemData::get(mapping.action);

				if (item)
				{
					classic_qs_icons[i] = item.get_icon(false);
					classic_qs_names[i] = item.get_name();
				}
			}
#ifdef USE_NX
			else if (mapping.type == KeyType::Id::MENU || mapping.type == KeyType::Id::ACTION
				|| mapping.type == KeyType::Id::FACE)
			{
				// MENU/ACTION/FACE：KeyConfig/icon/<actionId>
				nl::node n = key_icons[std::to_string(mapping.action)];
				if (!n)
					n = key_icons[mapping.action];
				if (n)
					classic_qs_icons[i] = Texture(n);
			}
#endif
		}
	}

	Point<int16_t> UIStatusBar::classic_qs_cell_at(size_t slot, int16_t cols) const
	{
		const int16_t c = cols > 0 ? cols : 1;
		const int16_t col = static_cast<int16_t>(slot % static_cast<size_t>(c));
		const int16_t row = static_cast<int16_t>(slot / static_cast<size_t>(c));

		return position + quickslot_pos + Point<int16_t>(
			static_cast<int16_t>(QS_FIRST_X + col * QS_PITCH_X),
			static_cast<int16_t>(QS_FIRST_Y + row * QS_PITCH_Y)
		);
	}

	int16_t UIStatusBar::classic_qs_slot_at(Point<int16_t> cursor_position) const
	{
		if (!classic_mode || !quickslot_active || !classic_quickslot)
			return -1;

		const int16_t cols = classic_qs_cols;
		const size_t count = std::min(classic_qs_count, CLASSIC_QS_SLOTS);

		for (size_t i = 0; i < count; i++)
		{
			const Point<int16_t> tl = classic_qs_cell_at(i, cols);
			Rectangle<int16_t> cell(
				tl,
				tl + Point<int16_t>(QS_SLOT, QS_SLOT)
			);

			if (cell.contains(cursor_position))
				return static_cast<int16_t>(i);
		}

		return -1;
	}

	void UIStatusBar::activate_classic_qs_slot(size_t slot)
	{
		if (slot >= CLASSIC_QS_SLOTS)
			return;

		Keyboard::Mapping mapping = UI::get().get_keyboard().get_maple_mapping(CLASSIC_QS_KEYS[slot]);

		if (mapping.type == KeyType::Id::NONE)
			return;

		Sound(Sound::Name::BUTTONCLICK).play();
		Stage::get().send_key(mapping.type, mapping.action, true);
	}

	Cursor::State UIStatusBar::send_cursor(bool clicked, Point<int16_t> cursor_position)
	{
		if (classic_mode && quickslot_active)
		{
			int16_t slot = classic_qs_slot_at(cursor_position);

			if (slot >= 0)
			{
				if (clicked)
					activate_classic_qs_slot(static_cast<size_t>(slot));

				return clicked ? Cursor::State::CLICKING : Cursor::State::CANCLICK;
			}
		}

		return UIElement::send_cursor(clicked, cursor_position);
	}

	void UIStatusBar::draw_classic(float alpha) const
	{
		// backgrnd：通栏横向平铺铺满 VWIDTH，消除左右黑边（正确端截图为通栏）。
		// 多分辨率：内容组过宽居中后，通栏背景仍必须从 x=0 起画，不能跟着 position.x
		// 右移（否则左侧留黑边、右侧溢出）。backgrnd2 等内容层继续按 position 绘制。
		if (classic_backgrnd)
			classic_backgrnd.draw(DrawArgument(
				Point<int16_t>(0, position.y()), Point<int16_t>(VWIDTH, 0)));

		if (classic_backgrnd2)
			classic_backgrnd2.draw(position);

		// 量规三段（gauge/bar 与 gauge/graduation 同为 340x31 三联图集，HP|MP|EXP 同行 1:1）。
		// 图集自身给出的绘制模型（逐行比对参考截图，见 evidence/w1w3-hud/coords.md §1.4）：
		//   bar        = 上带 y2..10 烘焙「HP/MP/EXP」标签 + 下带 y15..28 彩色填充
		//   gray 1x16  = 未填充段的底色 (204,204,204)
		//   graduation = 白边框 + 框内半透明黑渐变（给填充上阴影）→ 必须画在最上层
		// 旧实现按百分比裁 bar 整段，于是 0% 的 EXP 连标签一起消失，且填充盖住了 graduation 的阴影。
		// 水平 Range(first, second) 会同时把屏幕矩形右移 first，故三段均传 hpmp_pos 原点。
		{
			constexpr int16_t atlas_w = 340;
			constexpr int16_t fill_top = 15;	// bar 彩色带首行（实测红块 y15..28）
			constexpr int16_t fill_h = 14;

			struct Gauge_seg
			{
				int16_t x;		// 分段在图集内的起点
				int16_t w;		// 分段宽度
				int16_t fill_x;	// 彩色填充相对分段起点的偏移
				int16_t fill_w;	// 彩色填充宽度
			};

			// 填充带实测：红 x2..106 / 蓝 x110..214 / 绿 x223..337。
			const Gauge_seg segs[3] = {
				{ SEG_HP, SEG_W_HP, 2, 105 },
				{ SEG_MP, SEG_W_MP, 1, 105 },
				{ SEG_EXP, SEG_W_EXP, 1, 115 }
			};
			const float pct[3] = { hpbar.get_percentage(), mpbar.get_percentage(), expbar.get_percentage() };

			for (int i = 0; i < 3; ++i)
			{
				const Gauge_seg& s = segs[i];
				Range<int16_t> crop(s.x, static_cast<int16_t>(atlas_w - (s.x + s.w)));
				DrawArgument at(position + hpmp_pos);

				if (classic_gauge_bar)
					classic_gauge_bar.draw(at, {}, crop);

				int16_t filled = static_cast<int16_t>(pct[i] * s.fill_w + 0.5f);

				filled = std::clamp<int16_t>(filled, 0, s.fill_w);

				if (classic_gauge_gray && filled < s.fill_w)
				{
					classic_gauge_gray.draw(
						DrawArgument(
							position + hpmp_pos + Point<int16_t>(static_cast<int16_t>(s.x + s.fill_x + filled), fill_top),
							Point<int16_t>(static_cast<int16_t>(s.fill_w - filled), fill_h)
						)
					);
				}

				if (classic_graduation)
					classic_graduation.draw(at, {}, crop);
			}
		}

		// 闪光只在数值变化的那一小段播放（5 帧 x130ms）；常驻覆盖会把整条量规洗成粉色。
		if (classic_hp_flash > 0)
			classic_hp_flash_sprite.draw(position, alpha);

		if (classic_mp_flash > 0)
			classic_mp_flash_sprite.draw(position, alpha);

		int16_t level = stats.get_stat(MapleStat::Id::LEVEL);
		int16_t hp = stats.get_stat(MapleStat::Id::HP);
		int16_t mp = stats.get_stat(MapleStat::Id::MP);
		int32_t maxhp = stats.get_total(EquipStat::Id::HP);
		int32_t maxmp = stats.get_total(EquipStat::Id::MP);

		// 等级：Basic.img/LevelNo 橙盒数字（11x13，推进 11+1），以参考实测单字中心
		// x = lvbadge_pos.x() + 5 居中对齐；LevelNo 缺失时回退 ShowLevel 白字 + 右括号。
		const std::string lvstr = std::to_string(level);
		const int16_t lvw = static_cast<int16_t>(static_cast<int16_t>(lvstr.length()) * 12 - 1);
		const Point<int16_t> lv_at = position
			+ Point<int16_t>(static_cast<int16_t>(lvbadge_pos.x() + 5 - lvw / 2), lvbadge_pos.y());

		if (classic_levelno.getw('0') > 0)
		{
			classic_levelno.draw(lvstr, DrawArgument(lv_at));
		}
		else if (classic_showlevel.getw('0') > 0)
		{
			const int16_t slw = static_cast<int16_t>(static_cast<int16_t>(lvstr.length()) * 6 - 1);
			Point<int16_t> sl_at = position + Point<int16_t>(
				static_cast<int16_t>(lvbadge_pos.x() + 5 - slw / 2), lvbadge_pos.y() + 1);

			classic_showlevel.draw(lvstr, DrawArgument(sl_at));

			if (classic_showlevel_bracket)
				classic_showlevel_bracket.draw(
					DrawArgument(sl_at + Point<int16_t>(static_cast<int16_t>(slw + 1), 0)));
		}

		// 芯片内两行：职业名（上）+ 角色名（下）。*_pos.y() 是参考实测的墨迹顶，
		// 绘制点按各自当前文字反解 —— 中文名含「新/等级」这类 bt=10 的字，
		// 纯拉丁名只有 bt=9，同一档常量必然错 1px，只有按串精算才对（R27-B1b）。
		joblabel.change_text(stats.get_jobname());
		joblabel.draw(position + Point<int16_t>(joblabel_pos.x(), joblabel.y_for_ink_top(joblabel_pos.y())));
		namelabel.draw(position + Point<int16_t>(namelabel_pos.x(), namelabel.y_for_ink_top(namelabel_pos.y())));

		// 量规数值改画 StatusBar.img/number 位图字库（W1/RC6）：
		// 5x7 白数字 + 2x9 黄绿括号 + 7x7 slash/percent，不再走 Text(A11B, YELLOW)。
		// 参考截图逐字对拍：HP/MP = "[hp/maxhp]"，EXP = "[xx.xx%]"（两位小数，带括号）。
		// 标签 HP/MP/EXP 由 gauge/graduation 图集烘焙，不叠加 Text（否则重影）。
		draw_hud_number(hpmpset, classic_exp_dot,
			"[" + std::to_string(hp) + "/" + std::to_string(maxhp) + "]",
			DrawArgument(position + hpset_pos));

		draw_hud_number(hpmpset, classic_exp_dot,
			"[" + std::to_string(mp) + "/" + std::to_string(maxmp) + "]",
			DrawArgument(position + mpset_pos));

		// number 无 '.' 字形：小数点用 classic_exp_dot（1x1 白点）补，见 draw_hud_number。
		std::string expstr = std::to_string(getexppercent() * 100.0f);
		size_t decpt = expstr.find('.');

		if (decpt != std::string::npos)
			expstr = expstr.substr(0, decpt + 3);

		draw_hud_number(hpmpset, classic_exp_dot, "[" + expstr + "%]",
			DrawArgument(position + statset_pos));

		if (quickslot_active && classic_quickslot)
		{
			classic_quickslot.draw(position + quickslot_pos);

			// 槽格几何来自图集本身（见文件顶部 QS_* 推导），三种面板通用：
			// 151 = 9+4*28+3*7+9 / 466 = 9+13*28+12*7+9 / 571 = 9+16*28+15*7+9。
			const int16_t cols = classic_qs_cols;
			const size_t count = std::min(classic_qs_count, CLASSIC_QS_SLOTS);

			for (size_t i = 0; i < count; i++)
			{
				const Point<int16_t> cell_pos = classic_qs_cell_at(i, cols);

				if (classic_qs_icons[i])
					classic_qs_icons[i].draw(cell_pos + Point<int16_t>(0, QS_ICON_SHIFT_Y));

				// 键帽层（W3/RC14）：宽窄面板都画，且画在图标之上（贴纸高 11，占格顶带）。
				const int8_t cap = CLASSIC_QS_KEYCAP[i];
				const char* keyname = CLASSIC_QS_KEYNAME[i];

				if (cap >= 0 && classic_key_label[cap])
				{
					classic_key_label[cap].draw(cell_pos);
				}
				else if (keyname[0] != '\0')
				{
					// 原生键帽字身左上角 = 格左上角 + (4,4)（参考截图 FontMemo 同形量得），
					// OutlinedText 的 inner 即字身，故直接按该偏移落笔。
					classic_qs_keytext.change_text(keyname);
					classic_qs_keytext.draw(cell_pos + Point<int16_t>(QS_CAP_X, QS_CAP_Y));
				}

				// 绑定中文名：Text(A11M, maxwidth=28) —— 复用 T12b 的 CJK 字宽度量做
				// 断行，不在本批重写换行；落在格下半带。
				if (!classic_qs_names[i].empty())
				{
					classic_qs_label.change_text(classic_qs_names[i]);
					classic_qs_label.draw(DrawArgument(cell_pos + Point<int16_t>(0, QS_LABEL_Y)));
				}
			}
		}

		UIElement::draw_buttons(alpha);
	}

	void UIStatusBar::update_classic()
	{
		UIElement::update();

		if (classic_hp_flash > 0)
			classic_hp_flash_sprite.update();

		if (classic_mp_flash > 0)
			classic_mp_flash_sprite.update();

		expbar.update(getexppercent());

		float hp = gethppercent();
		float mp = getmppercent();

		// B4-5「HP/MP 警报」消费端（R28-B4b 接线）：低于阈值百分比时周期性重放
		// gauge/hpFlash·mpFlash 覆盖层，0 = 不报警（原生档 0 就是不闪）。
		// 量纲：ui.sysopt.hp_warn/mp_warn 存 0..100 百分比；默认 52 ↔ 原生注册表
		// soHPFlash/soMPFlash = 10（0..20 档 × 5% = 50%），是证据允许的最贴近校准。
		// 不再「数值一变就闪」—— 那是旧占位行为，与「警报」设置项语义无关，
		// 且满血受伤也闪（用户裁决 R28-B4b 决策点：低血才闪）。
		// 素材缺失（-1）时完全不触发，与旧口径一致；classic_*_flash 自身倒计时
		// 驱动重放（5 帧闪 + 1 帧间隔），**不新增类数据成员** —— 公共头改布局会踩
		// 增量构建旧 obj 坑（memory:msbuild-header-layout-stale-objs）。
		// 注：覆盖层素材只有 classic 底版有，现代皮肤（is_classic_hud()==false）
		// 无 hpFlash/mpFlash 节点，本设置对其无观感 —— 已记入 evidence §4 预留。
		const uint8_t hp_warn = Setting<SysOptHpWarn>::get().load();
		const uint8_t mp_warn = Setting<SysOptMpWarn>::get().load();

		const bool hp_low = hp_warn > 0 && hp <= static_cast<float>(hp_warn) / 100.0f;
		const bool mp_low = mp_warn > 0 && mp <= static_cast<float>(mp_warn) / 100.0f;

		if (classic_hp_flash > 0)
		{
			--classic_hp_flash;
		}
		else if (classic_hp_flash == 0 && hp_low)
		{
			classic_hp_flash = 5;
		}

		if (classic_mp_flash > 0)
		{
			--classic_mp_flash;
		}
		else if (classic_mp_flash == 0 && mp_low)
		{
			classic_mp_flash = 5;
		}

		// classic_last_hp/mp 原为「数值一变就闪」的比对基准，现语义已被阈值取代；
		// 成员保留（公共头布局冻结），仅作最近一次读数的 bookkeeping。
		classic_last_hp = hp;
		classic_last_mp = mp;

		hpbar.update(hp);
		mpbar.update(mp);
		namelabel.change_text(stats.get_name());
		refresh_classic_qs_icons();
	}
}