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
#include "UIKeyConfig.h"
#include "../../Net/Packets/SkillMacroPackets.h"

#include "UICharInfo.h"
#include "UIJoypad.h"
#include "UIItemInventory.h"
#include "UIMiscShells.h"
#include "UIQuickslotConfig.h"
#include "UISoftKey.h"

#include "../UI.h"

#include "../Components/MapleButton.h"
#include "../UITypes/UILoginNotice.h"
#include "../UITypes/UINotice.h"

#include "../../Data/ItemData.h"
#include "../../Data/SkillData.h"
#include "../../Gameplay/Stage.h"

#include "../../Net/Packets/PlayerPackets.h"
#include "../../Net/Packets/CashItemPackets.h"
#include "../../Configuration.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		nl::node basic_close_btn()
		{
			// 见 UiAssetResolve::close_btn() —— 本包 BtClose 是实心方块，
			// 原生标题栏用的是描边 X（BtClose2）。
			return UiAssetResolve::close_btn();
		}

		nl::node first_node(nl::node a, nl::node b)
		{
			return a ? a : b;
		}
	}

	UIKeyConfig::UIKeyConfig(const Inventory& in_inventory, const SkillBook& in_skillbook) : UIDragElement<PosKEYCONFIG>(), inventory(in_inventory), skillbook(in_skillbook), dirty(false)
	{
		keyboard = &UI::get().get_keyboard();
		staged_mappings = keyboard->get_maplekeys();

		nl::node KeyConfig = UiAssetResolve::status_bar_root()["KeyConfig"];

		if (!UiAssetResolve::usable(KeyConfig))
			KeyConfig = UiAssetResolve::window("KeyConfig");

		if (!UiAssetResolve::usable(KeyConfig))
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		icon = KeyConfig["icon"];
		key = KeyConfig["key"];

		nl::node backgrnd = KeyConfig["backgrnd"];
		Texture bg = backgrnd;
		Point<int16_t> bg_dimensions = bg.get_dimensions();
		const bool classic = UiAssetResolve::is_classic_hud();

		// v83 KeyConfig/backgrnd origin 为画布中心 (314,186)。Sprite 按 origin 回减，
		// 必须用 origin 作绘制偏移，底图 TL 才与 IDA keys_pos 对齐。
		if (backgrnd)
			sprites.emplace_back(backgrnd, classic ? DrawArgument(bg.get_origin()) : DrawArgument());
		if (KeyConfig["backgrnd2"])
		{
			Texture bg2 = KeyConfig["backgrnd2"];
			sprites.emplace_back(KeyConfig["backgrnd2"], classic ? DrawArgument(bg2.get_origin()) : DrawArgument());
		}
		if (KeyConfig["backgrnd3"])
		{
			Texture bg3 = KeyConfig["backgrnd3"];
			sprites.emplace_back(KeyConfig["backgrnd3"], classic ? DrawArgument(bg3.get_origin()) : DrawArgument());
		}

		nl::node BtClose = basic_close_btn();
		if (!BtClose)
			BtClose = KeyConfig["BtClose"];

		try_maple_btn(buttons, Buttons::CLOSE, BtClose, Point<int16_t>(bg_dimensions.x() - 18, 3));

		// classic / modern 两支取的是同一个 KeyConfig 根（见构造函数开头），故合并成一套坐标。
		// 本包只有 UIWindow.img/KeyConfig 一张底板（629x373），没有新版 button:XXX 命名，
		// 全部落到 Bt* 回退；坐标按 正确客户端截图/键盘设置.png 逐像素实测
		// （五个钮同处底图 y234 一行，从左到右 确定/取消/取消修改/全部消除/变更快捷键）。
		// modern 支此前五颗都没传 position，被 W0 的 maple_btn_nopos 哨兵整批跳过 →
		// 表现为「键盘设定窗没有按钮」。推导过程见 evidence/w2-hud-buttons/coords.md（T-4）。
		try_maple_btn(buttons, Buttons::OK, first_node(KeyConfig["button:OK"], KeyConfig["BtOK"]), Point<int16_t>(8, 234), "keyconfig ok");
		try_maple_btn(buttons, Buttons::CANCEL, first_node(KeyConfig["button:Cancel"], KeyConfig["BtCancel"]), Point<int16_t>(58, 234), "keyconfig cancel");
		try_maple_btn(buttons, Buttons::DEFAULT, first_node(KeyConfig["button:Default"], KeyConfig["BtDefault"]), Point<int16_t>(112, 234), "keyconfig default");
		try_maple_btn(buttons, Buttons::DELETE, first_node(KeyConfig["button:Delete"], KeyConfig["BtDelete"]), Point<int16_t>(177, 234), "keyconfig delete");
		try_maple_btn(buttons, Buttons::KEYSETTING, first_node(KeyConfig["button:keySetting"], KeyConfig["BtQuickSlot"]), Point<int16_t>(260, 234), "keyconfig keysetting");

		dimension = bg_dimensions;
		dragarea = Point<int16_t>(bg_dimensions.x(), 20);

		load_keys_pos();
		load_unbound_actions_pos();
		load_key_textures();
		load_actions();
		load_icons();

		bind_staged_action_keys();
	}

	/// Load
	void UIKeyConfig::load_keys_pos()
	{
		// BeiDou v83：IDA CUIKeyConfig 快捷键位表（dword_BE27E0，32x32 槽）。
		// HeavenClient Key 枚举与官方位号在 F11/F12/Scroll 上不完全一致，见下方注释。
		if (UiAssetResolve::is_classic_hud())
		{
			keys_pos[KeyConfig::Key::NUM1] = Point<int16_t>(48, 66);
			keys_pos[KeyConfig::Key::NUM2] = Point<int16_t>(82, 66);
			keys_pos[KeyConfig::Key::NUM3] = Point<int16_t>(116, 66);
			keys_pos[KeyConfig::Key::NUM4] = Point<int16_t>(150, 66);
			keys_pos[KeyConfig::Key::NUM5] = Point<int16_t>(184, 66);
			keys_pos[KeyConfig::Key::NUM6] = Point<int16_t>(218, 66);
			keys_pos[KeyConfig::Key::NUM7] = Point<int16_t>(252, 66);
			keys_pos[KeyConfig::Key::NUM8] = Point<int16_t>(286, 66);
			keys_pos[KeyConfig::Key::NUM9] = Point<int16_t>(320, 66);
			keys_pos[KeyConfig::Key::NUM0] = Point<int16_t>(354, 66);
			keys_pos[KeyConfig::Key::MINUS] = Point<int16_t>(388, 66);
			keys_pos[KeyConfig::Key::EQUAL] = Point<int16_t>(422, 66);

			keys_pos[KeyConfig::Key::Q] = Point<int16_t>(64, 99);
			keys_pos[KeyConfig::Key::W] = Point<int16_t>(98, 99);
			keys_pos[KeyConfig::Key::E] = Point<int16_t>(132, 99);
			keys_pos[KeyConfig::Key::R] = Point<int16_t>(166, 99);
			keys_pos[KeyConfig::Key::T] = Point<int16_t>(200, 99);
			keys_pos[KeyConfig::Key::Y] = Point<int16_t>(234, 99);
			keys_pos[KeyConfig::Key::U] = Point<int16_t>(268, 99);
			keys_pos[KeyConfig::Key::I] = Point<int16_t>(302, 99);
			keys_pos[KeyConfig::Key::O] = Point<int16_t>(336, 99);
			keys_pos[KeyConfig::Key::P] = Point<int16_t>(370, 99);
			keys_pos[KeyConfig::Key::LEFT_BRACKET] = Point<int16_t>(404, 99);
			keys_pos[KeyConfig::Key::RIGHT_BRACKET] = Point<int16_t>(438, 99);
			keys_pos[KeyConfig::Key::BACKSLASH] = Point<int16_t>(472, 99);

			keys_pos[KeyConfig::Key::LEFT_CONTROL] = Point<int16_t>(22, 198);
			keys_pos[KeyConfig::Key::RIGHT_CONTROL] = Point<int16_t>(461, 198);
			keys_pos[KeyConfig::Key::A] = Point<int16_t>(81, 132);
			keys_pos[KeyConfig::Key::S] = Point<int16_t>(115, 132);
			keys_pos[KeyConfig::Key::D] = Point<int16_t>(149, 132);
			keys_pos[KeyConfig::Key::F] = Point<int16_t>(183, 132);
			keys_pos[KeyConfig::Key::G] = Point<int16_t>(217, 132);
			keys_pos[KeyConfig::Key::H] = Point<int16_t>(251, 132);
			keys_pos[KeyConfig::Key::J] = Point<int16_t>(285, 132);
			keys_pos[KeyConfig::Key::K] = Point<int16_t>(319, 132);
			keys_pos[KeyConfig::Key::L] = Point<int16_t>(353, 132);
			keys_pos[KeyConfig::Key::SEMICOLON] = Point<int16_t>(387, 132);
			keys_pos[KeyConfig::Key::APOSTROPHE] = Point<int16_t>(421, 132);
			keys_pos[KeyConfig::Key::GRAVE_ACCENT] = Point<int16_t>(14, 66);

			keys_pos[KeyConfig::Key::LEFT_SHIFT] = Point<int16_t>(38, 165);
			keys_pos[KeyConfig::Key::RIGHT_SHIFT] = Point<int16_t>(457, 165);
			keys_pos[KeyConfig::Key::Z] = Point<int16_t>(98, 165);
			keys_pos[KeyConfig::Key::X] = Point<int16_t>(132, 165);
			keys_pos[KeyConfig::Key::C] = Point<int16_t>(166, 165);
			keys_pos[KeyConfig::Key::V] = Point<int16_t>(200, 165);
			keys_pos[KeyConfig::Key::B] = Point<int16_t>(234, 165);
			keys_pos[KeyConfig::Key::N] = Point<int16_t>(268, 165);
			keys_pos[KeyConfig::Key::M] = Point<int16_t>(302, 165);
			keys_pos[KeyConfig::Key::COMMA] = Point<int16_t>(336, 165);
			keys_pos[KeyConfig::Key::PERIOD] = Point<int16_t>(370, 165);

			keys_pos[KeyConfig::Key::LEFT_ALT] = Point<int16_t>(122, 198);
			keys_pos[KeyConfig::Key::RIGHT_ALT] = Point<int16_t>(348, 198);
			keys_pos[KeyConfig::Key::SPACE] = Point<int16_t>(233, 198);

			keys_pos[KeyConfig::Key::F1] = Point<int16_t>(82, 27);
			keys_pos[KeyConfig::Key::F2] = Point<int16_t>(116, 27);
			keys_pos[KeyConfig::Key::F3] = Point<int16_t>(150, 27);
			keys_pos[KeyConfig::Key::F4] = Point<int16_t>(184, 27);
			keys_pos[KeyConfig::Key::F5] = Point<int16_t>(226, 27);
			keys_pos[KeyConfig::Key::F6] = Point<int16_t>(260, 27);
			keys_pos[KeyConfig::Key::F7] = Point<int16_t>(294, 27);
			keys_pos[KeyConfig::Key::F8] = Point<int16_t>(328, 27);
			keys_pos[KeyConfig::Key::F9] = Point<int16_t>(370, 27);
			keys_pos[KeyConfig::Key::F10] = Point<int16_t>(404, 27);
			// 官方位号 87/88 为 F11/F12；本枚举 F11=69/F12=70，纹理仍读 key[87]/[88]。
			keys_pos[KeyConfig::Key::F11] = Point<int16_t>(438, 27);
			keys_pos[KeyConfig::Key::F12] = Point<int16_t>(472, 27);

			keys_pos[KeyConfig::Key::INSERT] = Point<int16_t>(514, 66);
			keys_pos[KeyConfig::Key::HOME] = Point<int16_t>(548, 66);
			keys_pos[KeyConfig::Key::PAGE_UP] = Point<int16_t>(582, 66);
			keys_pos[KeyConfig::Key::DELETE] = Point<int16_t>(514, 99);
			keys_pos[KeyConfig::Key::END] = Point<int16_t>(548, 99);
			keys_pos[KeyConfig::Key::PAGE_DOWN] = Point<int16_t>(582, 99);
			// ESC / SCROLL 在本包键盘画布上无独立槽，保持 (0,0) 跳过绘制。
			return;
		}

		int16_t slot_width = 33;
		int16_t slot_width_lg = 98;
		int16_t slot_height = 33;

		int16_t row_y = 126;
		int16_t row_special_y = row_y - slot_height - 5;

		int16_t row_quickslot_x = 535;

		int16_t row_one_x = 31;
		int16_t row_two_x = 80;
		int16_t row_three_x = 96;
		int16_t row_four_x = 55;
		int16_t row_five_x = 39;

		int16_t row_special_x = row_one_x;

		keys_pos[KeyConfig::Key::ESCAPE] = Point<int16_t>(row_one_x, row_special_y);

		row_special_x += slot_width * 2;

		for (size_t i = KeyConfig::Key::F1; i <= KeyConfig::Key::F12; i++)
		{
			KeyConfig::Key id = KeyConfig::actionbyid(i);

			keys_pos[id] = Point<int16_t>(row_special_x, row_special_y);

			row_special_x += slot_width;

			if (id == KeyConfig::Key::F4 || id == KeyConfig::Key::F8)
				row_special_x += 17;
		}

		keys_pos[KeyConfig::Key::SCROLL_LOCK] = Point<int16_t>(row_quickslot_x + (slot_width * 1), row_special_y);

		keys_pos[KeyConfig::Key::GRAVE_ACCENT] = Point<int16_t>(row_one_x + (slot_width * 0), row_y + (slot_height * 0));
		keys_pos[KeyConfig::Key::NUM1] = Point<int16_t>(row_one_x + (slot_width * 1), row_y + (slot_height * 0));
		keys_pos[KeyConfig::Key::NUM2] = Point<int16_t>(row_one_x + (slot_width * 2), row_y + (slot_height * 0));
		keys_pos[KeyConfig::Key::NUM3] = Point<int16_t>(row_one_x + (slot_width * 3), row_y + (slot_height * 0));
		keys_pos[KeyConfig::Key::NUM4] = Point<int16_t>(row_one_x + (slot_width * 4), row_y + (slot_height * 0));
		keys_pos[KeyConfig::Key::NUM5] = Point<int16_t>(row_one_x + (slot_width * 5), row_y + (slot_height * 0));
		keys_pos[KeyConfig::Key::NUM6] = Point<int16_t>(row_one_x + (slot_width * 6), row_y + (slot_height * 0));
		keys_pos[KeyConfig::Key::NUM7] = Point<int16_t>(row_one_x + (slot_width * 7), row_y + (slot_height * 0));
		keys_pos[KeyConfig::Key::NUM8] = Point<int16_t>(row_one_x + (slot_width * 8), row_y + (slot_height * 0));
		keys_pos[KeyConfig::Key::NUM9] = Point<int16_t>(row_one_x + (slot_width * 9), row_y + (slot_height * 0));
		keys_pos[KeyConfig::Key::NUM0] = Point<int16_t>(row_one_x + (slot_width * 10), row_y + (slot_height * 0));
		keys_pos[KeyConfig::Key::MINUS] = Point<int16_t>(row_one_x + (slot_width * 11), row_y + (slot_height * 0));
		keys_pos[KeyConfig::Key::EQUAL] = Point<int16_t>(row_one_x + (slot_width * 12), row_y + (slot_height * 0));

		for (size_t i = KeyConfig::Key::Q; i <= KeyConfig::Key::RIGHT_BRACKET; i++)
		{
			KeyConfig::Key id = KeyConfig::actionbyid(i);

			keys_pos[id] = Point<int16_t>(row_two_x + (slot_width * (i - KeyConfig::Key::Q)), row_y + (slot_height * 1));
		}

		row_two_x += 9;

		keys_pos[KeyConfig::Key::BACKSLASH] = Point<int16_t>(row_two_x + (slot_width * 12), row_y + (slot_height * 1));

		for (size_t i = KeyConfig::Key::A; i <= KeyConfig::Key::APOSTROPHE; i++)
		{
			KeyConfig::Key id = KeyConfig::actionbyid(i);

			keys_pos[id] = Point<int16_t>(row_three_x + (slot_width * (i - KeyConfig::Key::A)), row_y + (slot_height * 2));
		}

		keys_pos[KeyConfig::Key::LEFT_SHIFT] = Point<int16_t>(row_four_x + (slot_width * 0), row_y + (slot_height * 3));

		row_four_x += 24;

		for (size_t i = KeyConfig::Key::Z; i <= KeyConfig::Key::PERIOD; i++)
		{
			KeyConfig::Key id = KeyConfig::actionbyid(i);

			keys_pos[id] = Point<int16_t>(row_four_x + (slot_width * (i - KeyConfig::Key::Z + 1)), row_y + (slot_height * 3));
		}

		row_four_x += 24;

		keys_pos[KeyConfig::Key::RIGHT_SHIFT] = Point<int16_t>(row_four_x + (slot_width * 11), row_y + (slot_height * 3));

		keys_pos[KeyConfig::Key::LEFT_CONTROL] = Point<int16_t>(row_five_x + (slot_width_lg * 0), row_y + (slot_height * 4));
		keys_pos[KeyConfig::Key::LEFT_ALT] = Point<int16_t>(row_five_x + (slot_width_lg * 1), row_y + (slot_height * 4));

		row_five_x += 24;

		keys_pos[KeyConfig::Key::SPACE] = Point<int16_t>(row_five_x + (slot_width_lg * 2), row_y + (slot_height * 4));

		row_five_x += 27;

		keys_pos[KeyConfig::Key::RIGHT_ALT] = Point<int16_t>(row_five_x + (slot_width_lg * 3), row_y + (slot_height * 4));

		row_five_x += 2;

		keys_pos[KeyConfig::Key::RIGHT_CONTROL] = Point<int16_t>(row_five_x + (slot_width_lg * 4), row_y + (slot_height * 4));

		keys_pos[KeyConfig::Key::INSERT] = Point<int16_t>(row_quickslot_x + (slot_width * 0), row_y + (slot_height * 0));
		keys_pos[KeyConfig::Key::HOME] = Point<int16_t>(row_quickslot_x + (slot_width * 1), row_y + (slot_height * 0));
		keys_pos[KeyConfig::Key::PAGE_UP] = Point<int16_t>(row_quickslot_x + (slot_width * 2), row_y + (slot_height * 0));
		keys_pos[KeyConfig::Key::DELETE] = Point<int16_t>(row_quickslot_x + (slot_width * 0), row_y + (slot_height * 1));
		keys_pos[KeyConfig::Key::END] = Point<int16_t>(row_quickslot_x + (slot_width * 1), row_y + (slot_height * 1));
		keys_pos[KeyConfig::Key::PAGE_DOWN] = Point<int16_t>(row_quickslot_x + (slot_width * 2), row_y + (slot_height * 1));
	}

	void UIKeyConfig::load_unbound_actions_pos()
	{
		// v83：IDA 未绑定动作条 (9,267) 起，34px 步进、约 17 列。
		if (UiAssetResolve::is_classic_hud())
		{
			constexpr int16_t row_x = 9;
			constexpr int16_t row_y0 = 267;
			constexpr int16_t slot = 34;
			constexpr int16_t cols = 17;

			static const KeyAction::Id kClassicActions[] = {
				KeyAction::Id::EQUIPMENT, KeyAction::Id::ITEMS, KeyAction::Id::STATS, KeyAction::Id::SKILLS,
				KeyAction::Id::FRIENDS, KeyAction::Id::WORLDMAP, KeyAction::Id::MAPLECHAT, KeyAction::Id::MINIMAP,
				KeyAction::Id::QUESTLOG, KeyAction::Id::KEYBINDINGS, KeyAction::Id::SAY, KeyAction::Id::WHISPER,
				KeyAction::Id::PARTYCHAT, KeyAction::Id::FRIENDSCHAT, KeyAction::Id::MENU, KeyAction::Id::QUICKSLOTS,
				KeyAction::Id::TOGGLECHAT, KeyAction::Id::GUILD, KeyAction::Id::GUILDCHAT, KeyAction::Id::PARTY,
				KeyAction::Id::NOTIFIER, KeyAction::Id::MAPLENEWS, KeyAction::Id::CASHSHOP, KeyAction::Id::ALLIANCECHAT,
				KeyAction::Id::MANAGELEGION, KeyAction::Id::MEDALS, KeyAction::Id::BOSSPARTY,
				KeyAction::Id::PICKUP, KeyAction::Id::SIT, KeyAction::Id::ATTACK, KeyAction::Id::JUMP,
				KeyAction::Id::INTERACT_HARVEST, KeyAction::Id::CHARINFO, KeyAction::Id::CHANGECHANNEL,
				KeyAction::Id::SCREENSHOT
			};

			for (size_t i = 0; i < sizeof(kClassicActions) / sizeof(kClassicActions[0]); i++)
			{
				int16_t col = static_cast<int16_t>(i % cols);
				int16_t row = static_cast<int16_t>(i / cols);
				unbound_actions_pos[kClassicActions[i]] = Point<int16_t>(
					static_cast<int16_t>(row_x + col * slot),
					static_cast<int16_t>(row_y0 + row * slot)
				);
			}

			return;
		}

		int16_t row_x = 26;
		int16_t row_y = 307;

		int16_t slot_width = 36;
		int16_t slot_height = 36;

		/// Row 1
		unbound_actions_pos[KeyAction::Id::MAPLECHAT] = Point<int16_t>(row_x + (slot_width * 0), row_y + (slot_height * 0));
		unbound_actions_pos[KeyAction::Id::TOGGLECHAT] = Point<int16_t>(row_x + (slot_width * 1), row_y + (slot_height * 0));
		unbound_actions_pos[KeyAction::Id::WHISPER] = Point<int16_t>(row_x + (slot_width * 2), row_y + (slot_height * 0));
		unbound_actions_pos[KeyAction::Id::MEDALS] = Point<int16_t>(row_x + (slot_width * 3), row_y + (slot_height * 0));
		unbound_actions_pos[KeyAction::Id::BOSSPARTY] = Point<int16_t>(row_x + (slot_width * 4), row_y + (slot_height * 0));
		unbound_actions_pos[KeyAction::Id::PROFESSION] = Point<int16_t>(row_x + (slot_width * 5), row_y + (slot_height * 0));
		unbound_actions_pos[KeyAction::Id::EQUIPMENT] = Point<int16_t>(row_x + (slot_width * 6), row_y + (slot_height * 0));
		unbound_actions_pos[KeyAction::Id::ITEMS] = Point<int16_t>(row_x + (slot_width * 7), row_y + (slot_height * 0));
		unbound_actions_pos[KeyAction::Id::CHARINFO] = Point<int16_t>(row_x + (slot_width * 8), row_y + (slot_height * 0));
		unbound_actions_pos[KeyAction::Id::MENU] = Point<int16_t>(row_x + (slot_width * 9), row_y + (slot_height * 0));
		unbound_actions_pos[KeyAction::Id::QUICKSLOTS] = Point<int16_t>(row_x + (slot_width * 10), row_y + (slot_height * 0));
		unbound_actions_pos[KeyAction::Id::PICKUP] = Point<int16_t>(row_x + (slot_width * 11), row_y + (slot_height * 0));
		unbound_actions_pos[KeyAction::Id::SIT] = Point<int16_t>(row_x + (slot_width * 12), row_y + (slot_height * 0));
		unbound_actions_pos[KeyAction::Id::ATTACK] = Point<int16_t>(row_x + (slot_width * 13), row_y + (slot_height * 0));
		unbound_actions_pos[KeyAction::Id::JUMP] = Point<int16_t>(row_x + (slot_width * 14), row_y + (slot_height * 0));
		unbound_actions_pos[KeyAction::Id::INTERACT_HARVEST] = Point<int16_t>(row_x + (slot_width * 15), row_y + (slot_height * 0));
		unbound_actions_pos[KeyAction::Id::SOULWEAPON] = Point<int16_t>(row_x + (slot_width * 16), row_y + (slot_height * 0));

		/// Row 2
		unbound_actions_pos[KeyAction::Id::SAY] = Point<int16_t>(row_x + (slot_width * 0), row_y + (slot_height * 1));
		unbound_actions_pos[KeyAction::Id::PARTYCHAT] = Point<int16_t>(row_x + (slot_width * 1), row_y + (slot_height * 1));
		unbound_actions_pos[KeyAction::Id::FRIENDSCHAT] = Point<int16_t>(row_x + (slot_width * 2), row_y + (slot_height * 1));
		unbound_actions_pos[KeyAction::Id::ITEMPOT] = Point<int16_t>(row_x + (slot_width * 3), row_y + (slot_height * 1));
		unbound_actions_pos[KeyAction::Id::EVENT] = Point<int16_t>(row_x + (slot_width * 4), row_y + (slot_height * 1));
		unbound_actions_pos[KeyAction::Id::SILENTCRUSADE] = Point<int16_t>(row_x + (slot_width * 5), row_y + (slot_height * 1));
		unbound_actions_pos[KeyAction::Id::STATS] = Point<int16_t>(row_x + (slot_width * 6), row_y + (slot_height * 1));
		unbound_actions_pos[KeyAction::Id::SKILLS] = Point<int16_t>(row_x + (slot_width * 7), row_y + (slot_height * 1));
		unbound_actions_pos[KeyAction::Id::QUESTLOG] = Point<int16_t>(row_x + (slot_width * 8), row_y + (slot_height * 1));
		unbound_actions_pos[KeyAction::Id::CHANGECHANNEL] = Point<int16_t>(row_x + (slot_width * 9), row_y + (slot_height * 1));
		unbound_actions_pos[KeyAction::Id::GUILD] = Point<int16_t>(row_x + (slot_width * 10), row_y + (slot_height * 1));
		unbound_actions_pos[KeyAction::Id::PARTY] = Point<int16_t>(row_x + (slot_width * 11), row_y + (slot_height * 1));
		unbound_actions_pos[KeyAction::Id::NOTIFIER] = Point<int16_t>(row_x + (slot_width * 12), row_y + (slot_height * 1));
		unbound_actions_pos[KeyAction::Id::FRIENDS] = Point<int16_t>(row_x + (slot_width * 13), row_y + (slot_height * 1));
		unbound_actions_pos[KeyAction::Id::WORLDMAP] = Point<int16_t>(row_x + (slot_width * 14), row_y + (slot_height * 1));
		unbound_actions_pos[KeyAction::Id::MINIMAP] = Point<int16_t>(row_x + (slot_width * 15), row_y + (slot_height * 1));
		unbound_actions_pos[KeyAction::Id::KEYBINDINGS] = Point<int16_t>(row_x + (slot_width * 16), row_y + (slot_height * 1));

		/// Row 3
		unbound_actions_pos[KeyAction::Id::GUILDCHAT] = Point<int16_t>(row_x + (slot_width * 0), row_y + (slot_height * 2));
		unbound_actions_pos[KeyAction::Id::ALLIANCECHAT] = Point<int16_t>(row_x + (slot_width * 1), row_y + (slot_height * 2));
		unbound_actions_pos[KeyAction::Id::BATTLEANALYSIS] = Point<int16_t>(row_x + (slot_width * 2), row_y + (slot_height * 2));
		unbound_actions_pos[KeyAction::Id::GUIDE] = Point<int16_t>(row_x + (slot_width * 3), row_y + (slot_height * 2));
		unbound_actions_pos[KeyAction::Id::ENHANCEEQUIP] = Point<int16_t>(row_x + (slot_width * 4), row_y + (slot_height * 2));
		unbound_actions_pos[KeyAction::Id::MONSTERCOLLECTION] = Point<int16_t>(row_x + (slot_width * 5), row_y + (slot_height * 2));
		unbound_actions_pos[KeyAction::Id::MANAGELEGION] = Point<int16_t>(row_x + (slot_width * 6), row_y + (slot_height * 2));
		//unbound_actions_pos[KeyAction::Id::LENGTH] = Point<int16_t>(row_x + (slot_width * 7), row_y + (slot_height * 2));
		unbound_actions_pos[KeyAction::Id::MAPLENEWS] = Point<int16_t>(row_x + (slot_width * 8), row_y + (slot_height * 2));
		unbound_actions_pos[KeyAction::Id::CASHSHOP] = Point<int16_t>(row_x + (slot_width * 9), row_y + (slot_height * 2));
		unbound_actions_pos[KeyAction::Id::MAINMENU] = Point<int16_t>(row_x + (slot_width * 10), row_y + (slot_height * 2));
		unbound_actions_pos[KeyAction::Id::SCREENSHOT] = Point<int16_t>(row_x + (slot_width * 11), row_y + (slot_height * 2));
		unbound_actions_pos[KeyAction::Id::PICTUREMODE] = Point<int16_t>(row_x + (slot_width * 12), row_y + (slot_height * 2));
		//unbound_actions_pos[KeyAction::Id::LENGTH] = Point<int16_t>(row_x + (slot_width * 13), row_y + (slot_height * 2));
		unbound_actions_pos[KeyAction::Id::MUTE] = Point<int16_t>(row_x + (slot_width * 14), row_y + (slot_height * 2));
		unbound_actions_pos[KeyAction::Id::MAPLERELAY] = Point<int16_t>(row_x + (slot_width * 15), row_y + (slot_height * 2));
		//unbound_actions_pos[KeyAction::Id::LENGTH] = Point<int16_t>(row_x + (slot_width * 16), row_y + (slot_height * 2));

		/// Row 4
		unbound_actions_pos[KeyAction::Id::FACE1] = Point<int16_t>(row_x + (slot_width * 0), row_y + (slot_height * 3));
		unbound_actions_pos[KeyAction::Id::FACE2] = Point<int16_t>(row_x + (slot_width * 1), row_y + (slot_height * 3));
		unbound_actions_pos[KeyAction::Id::FACE3] = Point<int16_t>(row_x + (slot_width * 2), row_y + (slot_height * 3));
		unbound_actions_pos[KeyAction::Id::FACE4] = Point<int16_t>(row_x + (slot_width * 3), row_y + (slot_height * 3));
		unbound_actions_pos[KeyAction::Id::FACE5] = Point<int16_t>(row_x + (slot_width * 4), row_y + (slot_height * 3));
		unbound_actions_pos[KeyAction::Id::FACE6] = Point<int16_t>(row_x + (slot_width * 5), row_y + (slot_height * 3));
		unbound_actions_pos[KeyAction::Id::FACE7] = Point<int16_t>(row_x + (slot_width * 6), row_y + (slot_height * 3));
		unbound_actions_pos[KeyAction::Id::MAPLEACHIEVEMENT] = Point<int16_t>(row_x + (slot_width * 7), row_y + (slot_height * 3));
		unbound_actions_pos[KeyAction::Id::FAMILIAR] = Point<int16_t>(row_x + (slot_width * 8), row_y + (slot_height * 3));
		unbound_actions_pos[KeyAction::Id::TOSPOUSE] = Point<int16_t>(row_x + (slot_width * 9), row_y + (slot_height * 3));
		unbound_actions_pos[KeyAction::Id::EMOTICON] = Point<int16_t>(row_x + (slot_width * 10), row_y + (slot_height * 3));
		//unbound_actions_pos[KeyAction::Id::LENGTH] = Point<int16_t>(row_x + (slot_width * 11), row_y + (slot_height * 3));
		//unbound_actions_pos[KeyAction::Id::LENGTH] = Point<int16_t>(row_x + (slot_width * 12), row_y + (slot_height * 3));
		//unbound_actions_pos[KeyAction::Id::LENGTH] = Point<int16_t>(row_x + (slot_width * 13), row_y + (slot_height * 3));
		//unbound_actions_pos[KeyAction::Id::LENGTH] = Point<int16_t>(row_x + (slot_width * 14), row_y + (slot_height * 3));
		//unbound_actions_pos[KeyAction::Id::LENGTH] = Point<int16_t>(row_x + (slot_width * 15), row_y + (slot_height * 3));
		//unbound_actions_pos[KeyAction::Id::LENGTH] = Point<int16_t>(row_x + (slot_width * 16), row_y + (slot_height * 3));
	}

	void UIKeyConfig::load_key_textures()
	{
		key_textures[KeyConfig::Key::ESCAPE] = key[1];
		key_textures[KeyConfig::Key::NUM1] = key[2];
		key_textures[KeyConfig::Key::NUM2] = key[3];
		key_textures[KeyConfig::Key::NUM3] = key[4];
		key_textures[KeyConfig::Key::NUM4] = key[5];
		key_textures[KeyConfig::Key::NUM5] = key[6];
		key_textures[KeyConfig::Key::NUM6] = key[7];
		key_textures[KeyConfig::Key::NUM7] = key[8];
		key_textures[KeyConfig::Key::NUM8] = key[9];
		key_textures[KeyConfig::Key::NUM9] = key[10];
		key_textures[KeyConfig::Key::NUM0] = key[11];
		key_textures[KeyConfig::Key::MINUS] = key[12];
		key_textures[KeyConfig::Key::EQUAL] = key[13];

		key_textures[KeyConfig::Key::Q] = key[16];
		key_textures[KeyConfig::Key::W] = key[17];
		key_textures[KeyConfig::Key::E] = key[18];
		key_textures[KeyConfig::Key::R] = key[19];
		key_textures[KeyConfig::Key::T] = key[20];
		key_textures[KeyConfig::Key::Y] = key[21];
		key_textures[KeyConfig::Key::U] = key[22];
		key_textures[KeyConfig::Key::I] = key[23];
		key_textures[KeyConfig::Key::O] = key[24];
		key_textures[KeyConfig::Key::P] = key[25];
		key_textures[KeyConfig::Key::LEFT_BRACKET] = key[26];
		key_textures[KeyConfig::Key::RIGHT_BRACKET] = key[27];

		key_textures[KeyConfig::Key::LEFT_CONTROL] = key[29];
		key_textures[KeyConfig::Key::RIGHT_CONTROL] = key[29];

		key_textures[KeyConfig::Key::A] = key[30];
		key_textures[KeyConfig::Key::S] = key[31];
		key_textures[KeyConfig::Key::D] = key[32];
		key_textures[KeyConfig::Key::F] = key[33];
		key_textures[KeyConfig::Key::G] = key[34];
		key_textures[KeyConfig::Key::H] = key[35];
		key_textures[KeyConfig::Key::J] = key[36];
		key_textures[KeyConfig::Key::K] = key[37];
		key_textures[KeyConfig::Key::L] = key[38];
		key_textures[KeyConfig::Key::SEMICOLON] = key[39];
		key_textures[KeyConfig::Key::APOSTROPHE] = key[40];
		key_textures[KeyConfig::Key::GRAVE_ACCENT] = key[41];

		key_textures[KeyConfig::Key::LEFT_SHIFT] = key[42];
		key_textures[KeyConfig::Key::RIGHT_SHIFT] = key[42];

		key_textures[KeyConfig::Key::BACKSLASH] = key[43];
		key_textures[KeyConfig::Key::Z] = key[44];
		key_textures[KeyConfig::Key::X] = key[45];
		key_textures[KeyConfig::Key::C] = key[46];
		key_textures[KeyConfig::Key::V] = key[47];
		key_textures[KeyConfig::Key::B] = key[48];
		key_textures[KeyConfig::Key::N] = key[49];
		key_textures[KeyConfig::Key::M] = key[50];
		key_textures[KeyConfig::Key::COMMA] = key[51];
		key_textures[KeyConfig::Key::PERIOD] = key[52];

		key_textures[KeyConfig::Key::LEFT_ALT] = key[56];
		key_textures[KeyConfig::Key::RIGHT_ALT] = key[56];

		key_textures[KeyConfig::Key::SPACE] = key[57];

		key_textures[KeyConfig::Key::F1] = key[59];
		key_textures[KeyConfig::Key::F2] = key[60];
		key_textures[KeyConfig::Key::F3] = key[61];
		key_textures[KeyConfig::Key::F4] = key[62];
		key_textures[KeyConfig::Key::F5] = key[63];
		key_textures[KeyConfig::Key::F6] = key[64];
		key_textures[KeyConfig::Key::F7] = key[65];
		key_textures[KeyConfig::Key::F8] = key[66];
		key_textures[KeyConfig::Key::F9] = key[67];
		key_textures[KeyConfig::Key::F10] = key[68];

		key_textures[KeyConfig::Key::SCROLL_LOCK] = key[70];
		key_textures[KeyConfig::Key::HOME] = key[71];

		key_textures[KeyConfig::Key::PAGE_UP] = key[73];

		key_textures[KeyConfig::Key::END] = key[79];

		key_textures[KeyConfig::Key::PAGE_DOWN] = key[81];
		key_textures[KeyConfig::Key::INSERT] = key[82];
		key_textures[KeyConfig::Key::DELETE] = key[83];

		key_textures[KeyConfig::Key::F11] = key[87];
		key_textures[KeyConfig::Key::F12] = key[88];
	}

	void UIKeyConfig::load_actions()
	{
		for (size_t i = KeyAction::Id::EQUIPMENT; i < KeyAction::Id::LENGTH; i++)
		{
			if (i == KeyAction::Id::SAFEMODE || i == KeyAction::Id::MAPLESTORAGE || i == KeyAction::Id::VIEWERSCHAT || i == KeyAction::Id::BITS)
				continue;

			if (icon[i])
			{
				KeyAction::Id action = KeyAction::actionbyid(i);

				action_mappings.push_back(Keyboard::Mapping(get_keytype(action), action));

				// icon/<n> 是 32x32、origin (0,32) 的「底边锚点」键帽美术，而 keys_pos 与
				// unbound_actions_pos 都按「键帽左上角」书写（r27-oracle/origin-rule.md §3：
				// 四颗键帽 tmpl 探针与表项逐格相等、误差 0）。Texture::draw 会自动回减 origin，
				// 不抹平则键帽整层落在表值上方 32px，而字模 key/<n>（origin 0,0）留在原位，
				// 美术与字模分家 —— 即「键盘设定 内部的按键图片全都偏上」。
				Texture keycap = icon[i];
				keycap.shift(keycap.get_origin());

				action_icons[action] = std::make_unique<Icon>(
					std::make_unique<KeyIcon>(action),
					keycap,
					-1
					);
			}
		}
	}

	void UIKeyConfig::load_icons()
	{
		for (auto const& it : staged_mappings)
		{
			Keyboard::Mapping mapping = it.second;
			int32_t id = mapping.action;

			if (mapping.type == KeyType::Id::ITEM)
			{
				int16_t count = inventory.get_total_item_count(id);

				item_icons[id] = std::make_unique<Icon>(
					std::make_unique<KeyIcon>(mapping, count),
					get_item_texture(id),
					count
					);
			}
			else if (mapping.type == KeyType::Id::SKILL)
			{
				int16_t count = -1;

				skill_icons[id] = std::make_unique<Icon>(
					std::make_unique<KeyIcon>(mapping, count),
					get_skill_texture(id),
					count
					);
			}
		}
	}

	/// UI: General
	void UIKeyConfig::draw(float inter) const
	{
		UIElement::draw(inter);

		// Bound Keys
		for (auto const& iter : staged_mappings)
		{
			Keyboard::Mapping mapping = iter.second;

			if (mapping.type != KeyType::Id::NONE)
			{
				int32_t id = mapping.action;
				Icon* icon = nullptr;

				if (mapping.type == KeyType::Id::ITEM)
				{
					icon = item_icons.at(id).get();
				}
				else if (mapping.type == KeyType::Id::SKILL)
				{
					icon = skill_icons.at(id).get();
				}
				else if (is_action_mapping(mapping))
				{
					KeyAction::Id action = KeyAction::actionbyid(mapping.action);

					if (action < KeyAction::Id::LENGTH)
						icon = action_icons[action].get();
				}
				else
				{
					LOG(LOG_DEBUG, "Invalid mapping (" << mapping.type << ", " << mapping.action << ") for key [" << iter.first << "].");
				}

				if (icon)
				{
					KeyConfig::Key key = KeyConfig::actionbyid(iter.first);

					if (key < KeyConfig::Key::LENGTH)
					{
						// 键帽美术的 origin 已在 load_actions 抹平 ⇒ 表值即左上。
						// 原先道具/技能图标还要再减一个手写的 adjust（SPACE 减 (0,3)，
						// 其余减 (2,3)），而命中区 key_by_position() 用的是**没减**的表值
						// —— 画在上、点在下，差整整 2x3；那个残差从来没有 oracle，
						// 且用户报的「键盘设定按键图偏上」（CHECKLIST #87）正是它偏上的方向。
						// 现在绘制/命中/抓取三处都只认 keys_pos，版式若仍有系统性偏差，
						// 应该改这张表一次，而不是再叠一个只有绘制侧看得到的补偿。
						if (key == KeyConfig::Key::LEFT_CONTROL || key == KeyConfig::Key::RIGHT_CONTROL)
						{
							// 一个图标两格（左右 Ctrl/Alt/Shift）：逐格写本格再画，
							// 抓取时按「命中的那一格」显式算偏移，不依赖这里最后留下的那格。
							icon->set_cell(keys_pos[KeyConfig::Key::LEFT_CONTROL]);
							icon->draw_at(position);
							icon->set_cell(keys_pos[KeyConfig::Key::RIGHT_CONTROL]);
							icon->draw_at(position);
						}
						else if (key == KeyConfig::Key::LEFT_ALT || key == KeyConfig::Key::RIGHT_ALT)
						{
							icon->set_cell(keys_pos[KeyConfig::Key::LEFT_ALT]);
							icon->draw_at(position);
							icon->set_cell(keys_pos[KeyConfig::Key::RIGHT_ALT]);
							icon->draw_at(position);
						}
						else if (key == KeyConfig::Key::LEFT_SHIFT || key == KeyConfig::Key::RIGHT_SHIFT)
						{
							icon->set_cell(keys_pos[KeyConfig::Key::LEFT_SHIFT]);
							icon->draw_at(position);
							icon->set_cell(keys_pos[KeyConfig::Key::RIGHT_SHIFT]);
							icon->draw_at(position);
						}
						else
						{
							icon->set_cell(keys_pos[key]);
							icon->draw_at(position);
						}
					}
				}
			}
		}

		// Unbound Keys
		for (const auto& icon : action_icons)
		{
			if (!icon.second)
				continue;

			if (std::find(bound_actions.begin(), bound_actions.end(), icon.first) != bound_actions.end())
				continue;

			Point<int16_t> up = unbound_actions_pos[icon.first];

			if (up.x() == 0 && up.y() == 0)
				continue;

			icon.second->set_cell(up);
			icon.second->draw_at(position);
		}

		// Keys（(0,0) 表示 classic 下无槽，跳过避免堆到窗口原点）
		for (const auto& key_texture : key_textures)
		{
			KeyConfig::Key key = key_texture.first;
			Texture texture = key_texture.second;
			Point<int16_t> kp = keys_pos[key];

			if (kp.x() == 0 && kp.y() == 0)
				continue;

			texture.draw(position + kp);
		}
	}

	void UIKeyConfig::close()
	{
		clear_tooltip();
		deactivate();
		reset();
	}

	Button::State UIKeyConfig::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::CLOSE:
			case Buttons::CANCEL:
			{
				close();

				return Button::State::NORMAL;
			}
			case Buttons::DEFAULT:
			{
				static const std::string& message = "Would you like to revert to default settings?";

				auto onok = [&](bool ok)
				{
					if (ok)
					{
						auto keysel_onok = [&](bool alternate)
						{
							clear();

							if (alternate)
								staged_mappings = alternate_keys;
							else
								staged_mappings = basic_keys;

							bind_staged_action_keys();
						};

						UI::get().emplace<UIKeySelect>(keysel_onok, false);
					}
				};

				UI::get().emplace<UIOk>(message, onok);

				return Button::State::NORMAL;
			}
			case Buttons::DELETE:
			{
				static const std::string& message = "Would you like to clear all key bindings?";

				auto onok = [&](bool ok)
				{
					if (ok)
						clear();
				};

				UI::get().emplace<UIOk>(message, onok);

				return Button::State::NORMAL;
			}
			case Buttons::OK:
			{
				save_staged_mappings();
				close();

				return Button::State::NORMAL;
			}
			case Buttons::KEYSETTING:
			{
				// 原生「变更快捷键」→ quickslotConfig：点槽再按键上行 CHANGE_QUICKSLOT(183)
				auto qs = UI::get().get_element<UIQuickslotConfig>();
				if (!qs)
					qs = UI::get().emplace<UIQuickslotConfig>();
				else
					qs->makeactive();
				return Button::State::NORMAL;
			}
			default:
			{
				return Button::State::PRESSED;
			}
		}
	}

	Cursor::State UIKeyConfig::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State dstate = UIDragElement::send_cursor(clicked, cursorpos);

		if (dragged)
			return dstate;

		if (clicked)
		{
			KeyAction::Id action = unbound_action_by_position(cursorpos);

			if (action < KeyAction::Id::LENGTH)
			{
				if (auto icon = action_icons[action].get())
				{
					Point<int16_t> up = unbound_actions_pos[action];

					if (up.x() || up.y())
					{
						icon->start_drag_at(position, up, cursorpos);

						UI::get().drag_icon(icon);

						return Cursor::State::GRABBING;
					}
				}
			}

			KeyConfig::Key key = key_by_position(cursorpos);

			if (key < KeyConfig::Key::LENGTH)
			{
				Keyboard::Mapping mapping = get_staged_mapping(key);

				if (mapping.type != KeyType::Id::NONE)
				{
					int32_t id = mapping.action;
					Icon* icon = nullptr;

					if (mapping.type == KeyType::Id::ITEM)
					{
						icon = item_icons[id].get();
					}
					else if (mapping.type == KeyType::Id::SKILL)
					{
						icon = skill_icons[id].get();
					}
					else if (is_action_mapping(mapping))
					{
						KeyAction::Id action = KeyAction::actionbyid(mapping.action);

						if (action < KeyAction::Id::LENGTH)
							icon = action_icons[action].get();
					}
					else
					{
						LOG(LOG_DEBUG, "Invalid mapping (" << mapping.type << ", " << mapping.action << ") for key [" << key << "].");
					}

					if (icon)
					{
						clear_tooltip();

						icon->start_drag_at(position, keys_pos[key], cursorpos);

						UI::get().drag_icon(icon);

						return Cursor::State::GRABBING;
					}
				}
			}
		}
		else
		{
			KeyConfig::Key key = key_by_position(cursorpos);

			if (key < KeyConfig::Key::LENGTH)
			{
				Keyboard::Mapping mapping = get_staged_mapping(key);

				if (mapping.type == KeyType::Id::ITEM || mapping.type == KeyType::Id::SKILL)
				{
					int32_t id = mapping.action;

					if (mapping.type == KeyType::Id::ITEM)
						show_item(id);
					else
						show_skill(id);
				}
			}
		}

		return UIElement::send_cursor(clicked, cursorpos);
	}

	bool UIKeyConfig::send_icon(const Icon& icon, Point<int16_t> cursorpos)
	{
		for (auto iter : unbound_actions_pos)
		{
			Rectangle<int16_t> icon_rect = Rectangle<int16_t>(
				position + iter.second,
				position + iter.second + Point<int16_t>(32, 32)
				);

			if (icon_rect.contains(cursorpos))
				icon.drop_on_bindings(cursorpos, true);
		}

		KeyConfig::Key fkey = key_by_position(cursorpos);

		if (fkey != KeyConfig::Key::LENGTH)
			icon.drop_on_bindings(cursorpos, false);

		return true;
	}

	void UIKeyConfig::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { safe_close(); return; }
		if (!pressed) return;
		if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't')
		{
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				std::string t = tip;
				for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
				if (t == "save" || t == "ok" || t == "y" || t == "1") {
					save_staged_mappings();
					close();
					LOG(LOG_DEBUG, "UIKeyConfig: SoftKey save");
				} else if (t == "close" || t == "n" || t == "0" || t == "cancel") {
					close();
				}
			}, []() {}, "save|close");
		}
		else if (keycode == 'H' || keycode == 'h' || keycode == 'M' || keycode == 'm')
		{
			// 宠物自动药：键位 91=HP / 92=MP；背包有选中道具则直接绑定
			const bool hp = (keycode == 'H' || keycode == 'h');
			int32_t from_inv = 0;
			if (auto inv = UI::get().get_element<UIItemInventory>()) {
				int16_t slot = inv->get_selected_slot();
				if (slot > 0)
					from_inv = Stage::get().get_player().get_inventory().get_item_id(inv->get_tab(), slot);
			}
			auto apply = [hp](int32_t itemid) {
				if (itemid < 0) return;
				if (hp) Setting<AutoHpPotItem>::get().save(itemid);
				else Setting<AutoMpPotItem>::get().save(itemid);
				std::vector<std::tuple<KeyConfig::Key, KeyType::Id, int32_t>> upd;
				upd.emplace_back(
					static_cast<KeyConfig::Key>(hp ? 91 : 92),
					KeyType::Id::ITEM,
					itemid
				);
				ChangeKeyMapPacket(upd).dispatch();
				LOG(LOG_DEBUG, "UIKeyConfig: Auto" << (hp ? "Hp" : "Mp") << " item=" << itemid);
			};
			if (from_inv > 0) {
				apply(from_inv);
			} else {
				UI::get().emplace<UISoftKey>([hp, apply](const std::string& tip) {
					try {
						int32_t itemid = tip.empty()
							? (hp ? Setting<AutoHpPotItem>::get().load() : Setting<AutoMpPotItem>::get().load())
							: std::stoi(tip);
						apply(itemid);
					} catch (...) {}
				}, []() {}, hp ? "AutoHP itemId|empty=show" : "AutoMP itemId|empty=show");
			}
		}
		else if (keycode == 'L' || keycode == 'l')
		{
			// MapleLife 试名：CharInfo 有名则直接发
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				UseMapleLifePacket(pre).dispatch();
				LOG(LOG_DEBUG, "UIKeyConfig: UseMapleLife name=" << pre);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& name) {
					if (name.empty()) return;
					UseMapleLifePacket(name).dispatch();
					LOG(LOG_DEBUG, "UIKeyConfig: UseMapleLife name=" << name);
				}, []() {}, "MapleLife name");
			}
		}
		else if (keycode == 'K' || keycode == 'k')
		{
			auto ui = UI::get().get_element<UIMacroSkill>();
			if (!ui) ui = UI::get().emplace<UIMacroSkill>();
			else if (!ui->is_active()) ui->makeactive();
		}
		else if (keycode == 'J' || keycode == 'j')
		{
			auto ui = UI::get().get_element<UIJoypad>();
			if (!ui) ui = UI::get().emplace<UIJoypad>();
			else if (!ui->is_active()) ui->makeactive();
		}
	}

	UIElement::Type UIKeyConfig::get_type() const
	{
		return TYPE;
	}

	void UIKeyConfig::safe_close()
	{
		if (dirty)
		{
			static const std::string& message = "Do you want to save your changes?";

			auto onok = [&](bool ok)
			{
				if (ok)
				{
					save_staged_mappings();
					close();
				}
				else
				{
					close();
				}
			};

			UI::get().emplace<UIOk>(message, onok);
		}
		else
		{
			close();
		}
	}

	/// UI: Tooltip
	void UIKeyConfig::show_item(int32_t item_id)
	{
		UI::get().show_item(Tooltip::Parent::KEYCONFIG, item_id);
	}

	void UIKeyConfig::show_skill(int32_t skill_id)
	{
		int32_t level = skillbook.get_level(skill_id);
		int32_t masterlevel = skillbook.get_masterlevel(skill_id);
		int64_t expiration = skillbook.get_expiration(skill_id);

		UI::get().show_skill(Tooltip::Parent::KEYCONFIG, skill_id, level, masterlevel, expiration);
	}

	void UIKeyConfig::clear_tooltip()
	{
		UI::get().clear_tooltip(Tooltip::Parent::KEYCONFIG);
	}

	/// Keymap Staging
	void UIKeyConfig::stage_mapping(Point<int16_t> cursorposition, Keyboard::Mapping mapping)
	{
		KeyConfig::Key key = key_by_position(cursorposition);
		Keyboard::Mapping prior_staged = staged_mappings[key];

		if (prior_staged == mapping)
			return;

		unstage_mapping(prior_staged);

		int32_t id = mapping.action;

		if (is_action_mapping(mapping))
		{
			KeyAction::Id action = KeyAction::actionbyid(id);

			if (std::find(bound_actions.begin(), bound_actions.end(), action) == bound_actions.end())
				bound_actions.emplace_back(action);
		}

		for (auto const& it : staged_mappings)
		{
			Keyboard::Mapping staged_mapping = it.second;

			if (staged_mapping == mapping)
			{
				if (it.first == KeyConfig::Key::LEFT_CONTROL || it.first == KeyConfig::Key::RIGHT_CONTROL)
				{
					staged_mappings.erase(KeyConfig::Key::LEFT_CONTROL);
					staged_mappings.erase(KeyConfig::Key::RIGHT_CONTROL);
				}
				else if (it.first == KeyConfig::Key::LEFT_ALT || it.first == KeyConfig::Key::RIGHT_ALT)
				{
					staged_mappings.erase(KeyConfig::Key::LEFT_ALT);
					staged_mappings.erase(KeyConfig::Key::RIGHT_ALT);
				}
				else if (it.first == KeyConfig::Key::LEFT_SHIFT || it.first == KeyConfig::Key::RIGHT_SHIFT)
				{
					staged_mappings.erase(KeyConfig::Key::LEFT_SHIFT);
					staged_mappings.erase(KeyConfig::Key::RIGHT_SHIFT);
				}
				else
				{
					staged_mappings.erase(it.first);
				}

				break;
			}
		}

		if (key == KeyConfig::Key::LEFT_CONTROL || key == KeyConfig::Key::RIGHT_CONTROL)
		{
			staged_mappings[KeyConfig::Key::LEFT_CONTROL] = mapping;
			staged_mappings[KeyConfig::Key::RIGHT_CONTROL] = mapping;
		}
		else if (key == KeyConfig::Key::LEFT_ALT || key == KeyConfig::Key::RIGHT_ALT)
		{
			staged_mappings[KeyConfig::Key::LEFT_ALT] = mapping;
			staged_mappings[KeyConfig::Key::RIGHT_ALT] = mapping;
		}
		else if (key == KeyConfig::Key::LEFT_SHIFT || key == KeyConfig::Key::RIGHT_SHIFT)
		{
			staged_mappings[KeyConfig::Key::LEFT_SHIFT] = mapping;
			staged_mappings[KeyConfig::Key::RIGHT_SHIFT] = mapping;
		}
		else
		{
			staged_mappings[key] = mapping;
		}

		if (mapping.type == KeyType::Id::ITEM)
		{
			if (item_icons.find(id) == item_icons.end())
			{
				int16_t count = inventory.get_total_item_count(id);

				item_icons[id] = std::make_unique<Icon>(
					std::make_unique<KeyIcon>(mapping, count),
					get_item_texture(id),
					count
					);
			}
		}
		else if (mapping.type == KeyType::Id::SKILL)
		{
			if (skill_icons.find(id) == skill_icons.end())
			{
				int16_t count = -1;

				skill_icons[id] = std::make_unique<Icon>(
					std::make_unique<KeyIcon>(mapping, count),
					get_skill_texture(id),
					count
					);
			}
		}

		dirty = true;
	}

	void UIKeyConfig::unstage_mapping(Keyboard::Mapping mapping)
	{
		if (is_action_mapping(mapping))
		{
			KeyAction::Id action = KeyAction::actionbyid(mapping.action);
			auto iter = std::find(bound_actions.begin(), bound_actions.end(), action);

			if (iter != bound_actions.end())
				bound_actions.erase(iter);
		}

		for (auto const& it : staged_mappings)
		{
			Keyboard::Mapping staged_mapping = it.second;

			if (staged_mapping == mapping)
			{
				if (it.first == KeyConfig::Key::LEFT_CONTROL || it.first == KeyConfig::Key::RIGHT_CONTROL)
				{
					staged_mappings.erase(KeyConfig::Key::LEFT_CONTROL);
					staged_mappings.erase(KeyConfig::Key::RIGHT_CONTROL);
				}
				else if (it.first == KeyConfig::Key::LEFT_ALT || it.first == KeyConfig::Key::RIGHT_ALT)
				{
					staged_mappings.erase(KeyConfig::Key::LEFT_ALT);
					staged_mappings.erase(KeyConfig::Key::RIGHT_ALT);
				}
				else if (it.first == KeyConfig::Key::LEFT_SHIFT || it.first == KeyConfig::Key::RIGHT_SHIFT)
				{
					staged_mappings.erase(KeyConfig::Key::LEFT_SHIFT);
					staged_mappings.erase(KeyConfig::Key::RIGHT_SHIFT);
				}
				else
				{
					staged_mappings.erase(it.first);
				}

				if (staged_mapping.type == KeyType::Id::ITEM)
				{
					int32_t item_id = staged_mapping.action;
					item_icons.erase(item_id);
				}
				else if (staged_mapping.type == KeyType::Id::SKILL)
				{
					int32_t skill_id = staged_mapping.action;
					skill_icons.erase(skill_id);
				}

				dirty = true;

				break;
			}
		}
	}

	void UIKeyConfig::save_staged_mappings()
	{
		std::vector<std::tuple<KeyConfig::Key, KeyType::Id, int32_t>> updated_actions;

		for (auto& key : staged_mappings)
		{
			KeyConfig::Key k = KeyConfig::actionbyid(key.first);
			Keyboard::Mapping mapping = key.second;
			Keyboard::Mapping saved_mapping = keyboard->get_maple_mapping(key.first);

			if (mapping != saved_mapping)
				updated_actions.emplace_back(std::make_tuple(k, mapping.type, mapping.action));
		}

		auto maplekeys = keyboard->get_maplekeys();

		for (auto& key : maplekeys)
		{
			bool keyFound = false;
			KeyConfig::Key keyConfig = KeyConfig::actionbyid(key.first);

			for (auto& tkey : staged_mappings)
			{
				KeyConfig::Key tKeyConfig = KeyConfig::actionbyid(tkey.first);

				if (keyConfig == tKeyConfig)
				{
					keyFound = true;
					break;
				}
			}

			if (!keyFound)
				updated_actions.emplace_back(std::make_tuple(keyConfig, KeyType::Id::NONE, KeyAction::Id::LENGTH));
		}

		if (updated_actions.size() > 0)
			ChangeKeyMapPacket(updated_actions).dispatch();
		// W3：保存键位同时同步快捷栏触发键
		if (keyboard)
			ChangeQuickslotPacket(keyboard->quickslots()).dispatch();

		for (auto& action : updated_actions)
		{
			KeyConfig::Key key = std::get<0>(action);
			KeyType::Id type = std::get<1>(action);
			int32_t keyAction = std::get<2>(action);

			if (type == KeyType::Id::NONE)
				keyboard->remove(key);
			else
				keyboard->assign(key, type, keyAction);
		}

		dirty = false;
	}

	void UIKeyConfig::bind_staged_action_keys()
	{
		for (auto fkey : key_textures)
		{
			Keyboard::Mapping mapping = get_staged_mapping(fkey.first);

			if (mapping.type != KeyType::Id::NONE)
			{
				KeyAction::Id action = KeyAction::actionbyid(mapping.action);

				if (action < KeyAction::Id::LENGTH)
				{
					KeyType::Id type = get_keytype(action);

					if (type == KeyType::Id::NONE || mapping.type != type)
						continue;

					bound_actions.emplace_back(action);
				}
			}
		}
	}

	void UIKeyConfig::clear()
	{
		item_icons.clear();
		skill_icons.clear();
		bound_actions.clear();
		staged_mappings = {};
		dirty = true;
	}

	void UIKeyConfig::reset()
	{
		clear();

		staged_mappings = keyboard->get_maplekeys();

		load_icons();
		bind_staged_action_keys();

		dirty = false;
	}

	/// Helpers
	Texture UIKeyConfig::get_item_texture(int32_t item_id) const
	{
		const ItemData& data = ItemData::get(item_id);
		return data.get_icon(false);
	}

	Texture UIKeyConfig::get_skill_texture(int32_t skill_id) const
	{
		const SkillData& data = SkillData::get(skill_id);
		return data.get_icon(SkillData::Icon::NORMAL);
	}

	KeyConfig::Key UIKeyConfig::key_by_position(Point<int16_t> cursorpos) const
	{
		for (auto iter : keys_pos)
		{
			if (iter.second.x() == 0 && iter.second.y() == 0)
				continue;

			// 命中区 = 绘制用的同一格（keys_pos + Icon 本格大小），不再有只有绘制侧
			// 看得见的补偿量。
			Rectangle<int16_t> icon_rect = Rectangle<int16_t>(
				position + iter.second,
				position + iter.second + Point<int16_t>(Icon::cell_width, Icon::cell_height)
				);

			if (icon_rect.contains(cursorpos))
				return iter.first;
		}

		return KeyConfig::Key::LENGTH;
	}

	KeyAction::Id UIKeyConfig::unbound_action_by_position(Point<int16_t> cursorpos) const
	{
		for (auto iter : unbound_actions_pos)
		{
			if (iter.second.x() == 0 && iter.second.y() == 0)
				continue;

			if (std::find(bound_actions.begin(), bound_actions.end(), iter.first) != bound_actions.end())
				continue;

			Rectangle<int16_t> icon_rect = Rectangle<int16_t>(
				position + iter.second,
				position + iter.second + Point<int16_t>(Icon::cell_width, Icon::cell_height)
				);

			if (icon_rect.contains(cursorpos))
				return iter.first;
		}

		return KeyAction::Id::LENGTH;
	}

	Keyboard::Mapping UIKeyConfig::get_staged_mapping(int32_t keycode) const
	{
		auto iter = staged_mappings.find(keycode);

		if (iter == staged_mappings.end())
			return {};

		return iter->second;
	}

	bool UIKeyConfig::is_action_mapping(Keyboard::Mapping mapping) const
	{
		return std::find(action_mappings.begin(), action_mappings.end(), mapping) != action_mappings.end();
	}

	KeyType::Id UIKeyConfig::get_keytype(KeyAction::Id action)
	{
		if (action == KeyAction::Id::PICKUP || action == KeyAction::Id::SIT || action == KeyAction::Id::ATTACK || action == KeyAction::Id::JUMP || action == KeyAction::Id::INTERACT_HARVEST)
			return KeyType::Id::ACTION;
		else if (action < KeyAction::Id::FACE1 || action > KeyAction::Id::FACE7 && action < KeyAction::Id::LEFT)
			return KeyType::Id::MENU;
		else if (action >= KeyAction::Id::FACE1 && action <= KeyAction::Id::FACE7)
			return KeyType::Id::FACE;
		else
			return KeyType::Id::NONE;
	}

	/// Item count
	void UIKeyConfig::update_item_count(InventoryType::Id type, int16_t slot, int16_t change)
	{
		int32_t item_id = inventory.get_item_id(type, slot);

		if (item_icons.find(item_id) == item_icons.end())
			return;

		int16_t item_count = item_icons[item_id]->get_count();
		item_icons[item_id]->set_count(item_count + change);
	}

	/// MappingIcon
	UIKeyConfig::KeyIcon::KeyIcon(Keyboard::Mapping mapping, int16_t count) : mapping(mapping), count(count) {}

	UIKeyConfig::KeyIcon::KeyIcon(KeyAction::Id action)
	{
		KeyType::Id type = UIKeyConfig::get_keytype(action);
		mapping = Keyboard::Mapping(type, action);
	}

	void UIKeyConfig::KeyIcon::drop_on_stage() const
	{
		if (mapping.type == KeyType::Id::ITEM || mapping.type == KeyType::Id::SKILL)
		{
			auto keyconfig = UI::get().get_element<UIKeyConfig>();
			keyconfig->unstage_mapping(mapping);
		}
	}

	void UIKeyConfig::KeyIcon::drop_on_bindings(Point<int16_t> cursorposition, bool remove) const
	{
		auto keyconfig = UI::get().get_element<UIKeyConfig>();

		if (remove)
			keyconfig->unstage_mapping(mapping);
		else
			keyconfig->stage_mapping(cursorposition, mapping);
	}

	void UIKeyConfig::KeyIcon::set_count(int16_t c)
	{
		count = c;
	}

	Icon::IconType UIKeyConfig::KeyIcon::get_type()
	{
		return Icon::IconType::KEY;
	}
}