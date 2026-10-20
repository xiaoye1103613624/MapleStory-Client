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

#include "UIChannel.h"
#include "UICharInfo.h"
#include "UIChat.h"
#include "UIEquipInventory.h"
#include "UIDailyGift.h"
#include "UIEvent.h"
#include "UIHelp.h"
#include "UIItemInventory.h"
#include "UIJoypad.h"
#include "UIKeyConfig.h"
#include "UIOptionMenu.h"
#include "UIQuestLog.h"
#include "UIQuit.h"
#include "UISkillBook.h"
#include "UIStatsInfo.h"
#include "UIUserList.h"

#include "../UI.h"

#include "../Components/MapleButton.h"

#include "../../Character/ExpTable.h"
#include "../../Gameplay/Stage.h"

#include "../../Data/ItemData.h"
#include "../../Data/SkillData.h"
#include "../../Net/Packets/GameplayPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"
#include "../KeyConfig.h"
#include "../KeyType.h"
#include "UIChatBar.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

// 经典底条的分段裁框使用 Range<int16_t>（Texture::draw 的水平裁剪参数）。
#include "../../Template/Range.h"

namespace ms
{
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
			buttons[Buttons::BT_EXTEND_QS] = std::make_unique<MapleButton>(quickSlot[extend], quickslot_qs + quickslot_qs_adj);
		}
		else if (VWIDTH == 1024)
		{
			Point<int16_t> quickslot_qs = Point<int16_t>(627 + pos_adj, 37);

			buttons[Buttons::BT_FOLD_QS] = std::make_unique<MapleButton>(quickSlot[fold], quickslot_qs);
			buttons[Buttons::BT_EXTEND_QS] = std::make_unique<MapleButton>(quickSlot[extend], quickslot_qs + quickslot_qs_adj);
		}
		else if (VWIDTH == 1280)
		{
			Point<int16_t> quickslot_qs = Point<int16_t>(621 + pos_adj, 37);

			buttons[Buttons::BT_FOLD_QS] = std::make_unique<MapleButton>(quickSlot[fold], quickslot_qs);
			buttons[Buttons::BT_EXTEND_QS] = std::make_unique<MapleButton>(quickSlot[extend], quickslot_qs + quickslot_qs_adj);
		}
		else if (VWIDTH == 1366)
		{
			Point<int16_t> quickslot_qs = Point<int16_t>(623 + pos_adj, 37);

			buttons[Buttons::BT_FOLD_QS] = std::make_unique<MapleButton>(quickSlot[fold], quickslot_qs);
			buttons[Buttons::BT_EXTEND_QS] = std::make_unique<MapleButton>(quickSlot[extend], quickslot_qs + quickslot_qs_adj);
		}
		else if (VWIDTH == 1920)
		{
			Point<int16_t> quickslot_qs = Point<int16_t>(900 + pos_adj, 37);

			buttons[Buttons::BT_FOLD_QS] = std::make_unique<MapleButton>(quickSlot[fold], quickslot_qs);
			buttons[Buttons::BT_EXTEND_QS] = std::make_unique<MapleButton>(quickSlot[extend], quickslot_qs + quickslot_qs_adj);
		}

		if (quickslot_active)
			buttons[Buttons::BT_EXTEND_QS]->set_active(false);
		else
			buttons[Buttons::BT_FOLD_QS]->set_active(false);

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

		buttons.at(Buttons::BT_FOLD_QS)->draw(position + quickslot_adj);
		buttons.at(Buttons::BT_EXTEND_QS)->draw(position + quickslot_adj - quickslot_qs_adj);

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
					// BtMenu 位图为「目录」：打开系统选项，勿直接 Quit。
					UI::get().emplace<UIOptionMenu>();
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
				break;
			}
			case Buttons::BT_CLASSIC_NPT:
			{
				// BtNPT 位图为「帮助」；旧逻辑误开 QuestLog，缺资源时易卡死。
				UI::get().emplace<UIHelp>();
				break;
			}
			case Buttons::BT_CLASSIC_SHORT:
			{
				// BtShort 位图为「界面」→ 快捷键设置。
				UI::get().emplace<UIKeyConfig>(
					Stage::get().get_player().get_inventory(),
					Stage::get().get_player().get_skills()
				);
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
				// v83 Claim has no dedicated UI in this client — quiet no-op.
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
			case Buttons::BT_MENU_MEDAL:
			case Buttons::BT_MENU_UNION:
			case Buttons::BT_MENU_MONSTER_COLLECTION:
			case Buttons::BT_MENU_AUCTION:
			case Buttons::BT_MENU_MONSTER_LIFE:
			case Buttons::BT_MENU_BATTLE:
			case Buttons::BT_MENU_ACHIEVEMENT:
			case Buttons::BT_MENU_FISHING:
			case Buttons::BT_MENU_CLAIM:
			{
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_HELP:
			{
				UI::get().emplace<UIHelp>();
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
				remove_menus();
				break;
			}
			case Buttons::BT_COMMUNITY_MAPLECHAT:
			{
				UI::get().emplace<UIChat>();

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

	void UIStatusBar::toggle_menu()
	{
		remove_active_menu(MenuType::MENU);

		menu_active = !menu_active;

		buttons[Buttons::BT_MENU_ACHIEVEMENT]->set_active(menu_active);
		buttons[Buttons::BT_MENU_AUCTION]->set_active(menu_active);
		buttons[Buttons::BT_MENU_BATTLE]->set_active(menu_active);
		buttons[Buttons::BT_MENU_CLAIM]->set_active(menu_active);
		buttons[Buttons::BT_MENU_FISHING]->set_active(menu_active);
		buttons[Buttons::BT_MENU_HELP]->set_active(menu_active);
		buttons[Buttons::BT_MENU_MEDAL]->set_active(menu_active);
		buttons[Buttons::BT_MENU_MONSTER_COLLECTION]->set_active(menu_active);
		buttons[Buttons::BT_MENU_MONSTER_LIFE]->set_active(menu_active);
		buttons[Buttons::BT_MENU_QUEST]->set_active(menu_active);
		buttons[Buttons::BT_MENU_UNION]->set_active(menu_active);

		if (menu_active)
		{
			buttons[Buttons::BT_MENU_QUEST]->set_state(Button::State::MOUSEOVER);

			Sound(Sound::Name::DLGNOTICE).play();
		}
	}

	void UIStatusBar::toggle_setting()
	{
		remove_active_menu(MenuType::SETTING);

		setting_active = !setting_active;

		buttons[Buttons::BT_SETTING_CHANNEL]->set_active(setting_active);
		buttons[Buttons::BT_SETTING_QUIT]->set_active(setting_active);
		buttons[Buttons::BT_SETTING_JOYPAD]->set_active(setting_active);
		buttons[Buttons::BT_SETTING_KEYS]->set_active(setting_active);
		buttons[Buttons::BT_SETTING_OPTION]->set_active(setting_active);

		if (setting_active)
		{
			buttons[Buttons::BT_SETTING_CHANNEL]->set_state(Button::State::MOUSEOVER);

			Sound(Sound::Name::DLGNOTICE).play();
		}
	}

	void UIStatusBar::toggle_community()
	{
		remove_active_menu(MenuType::COMMUNITY);

		community_active = !community_active;

		buttons[Buttons::BT_COMMUNITY_PARTY]->set_active(community_active);
		buttons[Buttons::BT_COMMUNITY_FRIENDS]->set_active(community_active);
		buttons[Buttons::BT_COMMUNITY_GUILD]->set_active(community_active);
		buttons[Buttons::BT_COMMUNITY_MAPLECHAT]->set_active(community_active);

		if (community_active)
		{
			buttons[Buttons::BT_COMMUNITY_FRIENDS]->set_state(Button::State::MOUSEOVER);

			Sound(Sound::Name::DLGNOTICE).play();
		}
	}

	void UIStatusBar::toggle_character()
	{
		remove_active_menu(MenuType::CHARACTER);

		character_active = !character_active;

		buttons[Buttons::BT_CHARACTER_INFO]->set_active(character_active);
		buttons[Buttons::BT_CHARACTER_EQUIP]->set_active(character_active);
		buttons[Buttons::BT_CHARACTER_ITEM]->set_active(character_active);
		buttons[Buttons::BT_CHARACTER_SKILL]->set_active(character_active);
		buttons[Buttons::BT_CHARACTER_STAT]->set_active(character_active);

		if (character_active)
		{
			buttons[Buttons::BT_CHARACTER_INFO]->set_state(Button::State::MOUSEOVER);

			Sound(Sound::Name::DLGNOTICE).play();
		}
	}

	void UIStatusBar::toggle_event()
	{
		remove_active_menu(MenuType::EVENT);

		event_active = !event_active;

		buttons[Buttons::BT_EVENT_DAILY]->set_active(event_active);
		buttons[Buttons::BT_EVENT_SCHEDULE]->set_active(event_active);

		if (event_active)
		{
			buttons[Buttons::BT_EVENT_SCHEDULE]->set_state(Button::State::MOUSEOVER);

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
			buttons[i]->set_state(Button::State::NORMAL);

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

		// 快捷栏面板按槽数取图集变体：quickSlot08=151x80(4列x2行=8槽)、
		// quickSlot=466x80(13列=26槽)、quickSlot32=571x80(16列=32槽)。
		// 经典底条只有 8 个键位（Insert/Home/PgUp/Scroll + Del/End/PgDn/F1），
		// 且 IDA 的 CreateCtrl x=648 只有配 151 宽才恰好落在 800 网格内（648+151=799）；
		// 旧代码取 466 宽的 quickSlot 却按 151 宽摆放，右侧 314px 被截出屏外。
		classic_quickslot = Texture(base["quickSlot08"] ? base["quickSlot08"] : base["quickSlot"]);

		// ============ 经典底条布局（800 设计网格，bar 原点整体居中）============
		// 权威来源：
		//  a) IDA BeiDou v17.7 脚本化 CreateCtrl(node, id, x, y, type)（sub_8D01B2）：
		//       BtShop=1000/(573,543)  BtNPT=1001/(629,543)  BtMenu=1002/(685,543)
		//       BtShort=1007/(741,543)  EquipKey=2000/(618,515) InvenKey=2001/(648,515)
		//       StatKey=2002/(678,515) SkillKey=2003/(708,515) KeySet=2004/(738,515)
		//       QuickSlot=2005/(768,515) QuickSlotD=2006/(573,515)
		//     创建窗体高 578、top≈22 → 底条相对 y = CreateY - 507：543→36、515→8。
		//     x 末端 741+54=795 / 768+28=796，证明设计网格宽 800（而非右移贴边）。
		//  b) StatusBar.img 节点实测（resource-probe 递归 dump）：
		//       base/backgrnd  817x71  ← 固定尺寸精灵，旧代码拉伸到 VWIDTH 会失真
		//       base/backgrnd2 570x71  ← 叠加层：上带亮板 y6..29（名字）；
		//                                下带深色量表面板 y36..69 x0..562，LV 盒 x0..84
		//       gauge/bar & gauge/graduation 340x31 三联图集（同行排列，非上下叠放）：
		//         填充 HP 红 x2..106 / MP 蓝 x110..214 / EXP 绿 x223..337，均 y15..28
		//         边框 HP x0..107  / MP x109..215  / EXP x222..339，均 y15..30
		//       gauge/hpFlash|mpFlash 109x18（5 帧闪光覆盖层）
		//       number 字符集 0-9(5x7)+Lbracket/Rbracket(2x9)+slash/percent(7x7)，无 '.' 字形
		//       FontMemo 0-9(6x7) 等级数字
		//       base/quickSlot08 151x80 = 4列x2行；槽 28x28，列距 35（首槽 x=9），行距 34（首行 y=9）
		//  c) 分辨率：北斗把 800 宽设计网格整体居中 left=(VWIDTH-800)/2，不拉伸也不右移。
		constexpr int16_t BAR_GRID_W = 800;

		// 量表三联同行：图集 340 宽在 570 宽面板中水平居中；边框 y15..30 落在面板 y38..53。
		constexpr int16_t GAUGE_X = 115;
		constexpr int16_t GAUGE_Y = 23;
		// 图集内分段左裁量（CLIP 模式把分段平移到绘制点）。
		constexpr int16_t SEG_HP = 0;
		constexpr int16_t SEG_MP = 109;
		constexpr int16_t SEG_EXP = 222;
		constexpr int16_t GAUGE_LEN = 109;

		hpmp_pos = Point<int16_t>(GAUGE_X, GAUGE_Y);
		exp_pos = Point<int16_t>(GAUGE_X, GAUGE_Y);
		// 数值行：量表框 y38..53 之下方面板 y56..63。
		hpset_pos = Point<int16_t>(GAUGE_X, 56);
		mpset_pos = Point<int16_t>(static_cast<int16_t>(GAUGE_X + 110), 56);
		statset_pos = Point<int16_t>(static_cast<int16_t>(GAUGE_X + 222), 56);
		// LV 盒 x0..84 y38..60，位图内已含 "LV." 字样（x6..26 y46..58），数字接排其后。
		levelset_pos = Point<int16_t>(31, 47);
		// 名字：上带亮板 y6..29。
		namelabel_pos = Point<int16_t>(88, 9);

		// 三联填充：同一图集按分段裁剪，HP/MP/EXP 各绘在各自分段起点。
		hpbar = Gauge(Gauge::Type::CLIP, classic_gauge_bar, GAUGE_LEN, 0.0f);
		hpbar.set_clip_left(SEG_HP);
		mpbar = Gauge(Gauge::Type::CLIP, classic_gauge_bar, GAUGE_LEN, 0.0f);
		mpbar.set_clip_left(SEG_MP);
		expbar = Gauge(Gauge::Type::CLIP, classic_gauge_bar, static_cast<int16_t>(GAUGE_LEN + 9), 0.0f);
		expbar.set_clip_left(SEG_EXP);

		// 闪光覆盖层对齐各分段可视框：图集帧从 y15 起，故可视 y = GAUGE_Y + 15，
		// 再按 109x18 外扩 1px。（此前误用 GAUGE_Y，闪光画到了亮板上。）
		if (gauge["hpFlash"]["0"])
			hpmp_sprites.emplace_back(gauge["hpFlash"]["0"], hpmp_pos + Point<int16_t>(-1, 14));

		if (gauge["mpFlash"]["0"])
			hpmp_sprites.emplace_back(gauge["mpFlash"]["0"], hpmp_pos + Point<int16_t>(108, 14));

		if (sb["number"])
		{
			statset = Charset(sb["number"], Charset::Alignment::LEFT);
			hpmpset = Charset(sb["number"], Charset::Alignment::LEFT);
			levelset = Charset(sb["FontMemo"] ? sb["FontMemo"] : sb["number"], Charset::Alignment::LEFT);
		}

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

		// IDA CreateCtrl：y=543 → 底条 36；y=515 → 底条 8。800 网格原值，不再加 right 偏移。
		add_btn(Buttons::BT_CLASSIC_SHOP, sb["BtShop"], 573, 36);
		add_btn(Buttons::BT_CLASSIC_NPT, sb["BtNPT"], 629, 36);
		add_btn(Buttons::BT_CLASSIC_MENU, sb["BtMenu"], 685, 36);
		add_btn(Buttons::BT_CLASSIC_SHORT, sb["BtShort"], 741, 36);

		add_btn(Buttons::BT_CLASSIC_EQUIP, sb["EquipKey"], 618, 8);
		add_btn(Buttons::BT_CLASSIC_INV, sb["InvenKey"], 648, 8);
		add_btn(Buttons::BT_CLASSIC_STAT, sb["StatKey"], 678, 8);
		add_btn(Buttons::BT_CLASSIC_SKILL, sb["SkillKey"], 708, 8);
		add_btn(Buttons::BT_CLASSIC_KEYSET, sb["KeySet"], 738, 8);
		add_btn(Buttons::BT_CLASSIC_QS, sb["QuickSlot"], 768, 8);
		add_btn(Buttons::BT_CLASSIC_QSD, sb["QuickSlotD"], 573, 8);

		if (sb["BtWhisper"])
			add_btn(Buttons::BT_CLASSIC_WHISPER, sb["BtWhisper"], 376, 45);

		if (sb["BtClaim"])
			add_btn(Buttons::BT_CLASSIC_CLAIM, sb["BtClaim"], 396, 45);

		quickslot[0] = classic_quickslot;
		quickslot_active = true;
		quickslot_adj = Point<int16_t>(0, 0);
		// quickSlot08 151x80：面板上沿高出底条 9px（y=-9），与 71 高的底条合计 80。
		quickslot_pos = Point<int16_t>(648, -9);

		// 800 设计网格整体居中：left=(VWIDTH-800)/2。底条贴屏幕底部。
		position = Point<int16_t>(static_cast<int16_t>(std::max(0, VWIDTH - BAR_GRID_W) / 2), static_cast<int16_t>(VHEIGHT - CLASSIC_BAR_H));
		position_x = 0;
		position_y = position.y();
		dimension = Point<int16_t>(BAR_GRID_W, CLASSIC_BAR_H);

		refresh_classic_qs_icons();
	}

	void UIStatusBar::refresh_classic_qs_icons()
	{
		// KeyConfig quickslot cluster (2x4): Insert/Home/PgUp/Scroll + Del/End/PgDn/F1
		static constexpr uint8_t kQsKeys[CLASSIC_QS_SLOTS] = {
			KeyConfig::Key::INSERT,
			KeyConfig::Key::HOME,
			KeyConfig::Key::PAGE_UP,
			KeyConfig::Key::SCROLL_LOCK,
			KeyConfig::Key::DELETE,
			KeyConfig::Key::END,
			KeyConfig::Key::PAGE_DOWN,
			KeyConfig::Key::F1
		};

		const Keyboard& keyboard = UI::get().get_keyboard();

		for (size_t i = 0; i < CLASSIC_QS_SLOTS; i++)
		{
			classic_qs_icons[i] = Texture();
			Keyboard::Mapping mapping = keyboard.get_maple_mapping(kQsKeys[i]);

			if (mapping.type == KeyType::Id::SKILL)
			{
				const SkillData& skill = SkillData::get(mapping.action);
				const Texture& icon = skill.get_icon(SkillData::Icon::NORMAL);

				if (icon.is_valid())
					classic_qs_icons[i] = icon;
			}
			else if (mapping.type == KeyType::Id::ITEM)
			{
				const ItemData& item = ItemData::get(mapping.action);

				if (item)
					classic_qs_icons[i] = item.get_icon(false);
			}
		}
	}

	void UIStatusBar::draw_classic(float alpha) const
	{
		// backgrnd 是 817x71 固定尺寸精灵：按自然尺寸绘制。
		// 旧代码用 DrawArgument(position, Point<int16_t>(VWIDTH, 0)) 把它拉伸到整个屏宽，
		// 金属边框被横向拉扁，是 HUD 整体失真的主因之一。
		if (classic_backgrnd)
			classic_backgrnd.draw(position);

		if (classic_backgrnd2)
			classic_backgrnd2.draw(position);

		// gauge/graduation 与 gauge/bar 同为 340x31 三联图集（HP|MP|EXP 同行）。
		// 边框逐段用水平 Range 裁出，锚在该段绘制点；填充由 Gauge(CLIP) 裁同段。
		if (classic_graduation)
		{
			constexpr int16_t atlas_w = 340;

			// HP 段：图集 x0..107（108 宽）
			classic_graduation.draw(
				DrawArgument(position + hpmp_pos),
				{},
				Range<int16_t>(0, static_cast<int16_t>(atlas_w - 108))
			);

			// MP 段：图集 x109..215（107 宽）
			classic_graduation.draw(
				DrawArgument(position + hpmp_pos + Point<int16_t>(-109, 0)),
				{},
				Range<int16_t>(109, static_cast<int16_t>(atlas_w - 216))
			);

			// EXP 段：图集 x222..339（118 宽）
			classic_graduation.draw(
				DrawArgument(position + hpmp_pos + Point<int16_t>(-222, 0)),
				{},
				Range<int16_t>(222, 0)
			);
		}

		// Gauge(CLIP) 的 -clip_left 平移会与 GraphicsGL Range 的 +first 平移抵消，
		// 使每个分段都锚定在传入的 x 上；因此三段填充必须各传自己分段的 x，
		// 否则 MP/EXP 会叠画在 HP 之上（表现为 HP 条被蓝色覆盖）。
		hpbar.draw(position + hpmp_pos);
		mpbar.draw(position + hpmp_pos + Point<int16_t>(109, 0));
		expbar.draw(position + hpmp_pos + Point<int16_t>(222, 0));

		for (const Sprite& sprite : hpmp_sprites)
			sprite.draw(position, alpha);

		int16_t level = stats.get_stat(MapleStat::Id::LEVEL);
		int16_t hp = stats.get_stat(MapleStat::Id::HP);
		int16_t mp = stats.get_stat(MapleStat::Id::MP);
		int32_t maxhp = stats.get_total(EquipStat::Id::HP);
		int32_t maxmp = stats.get_total(EquipStat::Id::MP);

		levelset.draw(std::to_string(level), position + levelset_pos);
		namelabel.draw(position + namelabel_pos);
		hpmpset.draw(std::to_string(hp) + "/" + std::to_string(maxhp), position + hpset_pos);
		hpmpset.draw(std::to_string(mp) + "/" + std::to_string(maxmp), position + mpset_pos);

		// number 字符集没有 '.' 字形（0-9 + [ ] / %），真客户端也不显示小数：
		// 旧代码 substr(0, find('.')+3) 在无小数点时把整个串后面多截 3 字符，画出一串错字（"000"）。
		int32_t exppercent = static_cast<int32_t>(100.0f * getexppercent());
		statset.draw(std::to_string(exppercent) + "%", position + statset_pos);

		if (quickslot_active && classic_quickslot)
		{
			classic_quickslot.draw(position + quickslot_pos);

			// quickSlot08 151x80 = 4列x2行：槽 28x28，列距 35（首槽 x=9），行距 34（首行 y=9）。
			// 图标纹理自带 +32 y 偏移（WZ 图标锚点在贴图下方），裸 Texture 须同样下移。
			constexpr int16_t slot = 28;
			constexpr int16_t pitch_x = 35;
			constexpr int16_t pitch_y = 34;
			constexpr int16_t first_x = 9;
			constexpr int16_t first_y = 9;
			constexpr int16_t icon_shift_y = 32;
			constexpr int16_t cols = 4;

			for (size_t i = 0; i < CLASSIC_QS_SLOTS; i++)
			{
				if (!classic_qs_icons[i])
					continue;

				int16_t col = static_cast<int16_t>(i % cols);
				int16_t row = static_cast<int16_t>(i / cols);
				Point<int16_t> icon_pos = position + quickslot_pos
					+ Point<int16_t>(
						static_cast<int16_t>(first_x + col * pitch_x),
						static_cast<int16_t>(first_y + row * pitch_y + icon_shift_y)
					);
				classic_qs_icons[i].draw(icon_pos);
			}
		}

		UIElement::draw_buttons(alpha);
	}

	void UIStatusBar::update_classic()
	{
		UIElement::update();

		for (Sprite& sprite : hpmp_sprites)
			sprite.update();

		expbar.update(getexppercent());
		hpbar.update(gethppercent());
		mpbar.update(getmppercent());
		namelabel.change_text(stats.get_name());
		refresh_classic_qs_icons();
	}
}