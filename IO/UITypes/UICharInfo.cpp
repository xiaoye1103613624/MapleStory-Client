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
#include "UICharInfo.h"

#include "../Components/MapleButton.h"

#include "../../Data/ItemData.h"
#include "../../Gameplay/Stage.h"
#include "../../Graphics/Geometry.h"

#include "../../Net/Packets/BuddyPackets.h"
#include "../../Net/Packets/CashItemPackets.h"
#include "../../Net/Packets/FamilyPackets.h"
#include "../../Net/Packets/MessagingPackets.h"
#include "../../Net/Packets/MessengerPackets.h"
#include "../../Net/Packets/MiscOutPackets.h"
#include "../../Net/Packets/NotePackets.h"
#include "../../Net/Packets/PartyPackets.h"
#include "../../Net/Packets/PlayerInteractionPackets.h"
#include "../../Net/Packets/PlayerPackets.h"
#include "../../Net/Packets/TradePackets.h"
#include "../../Character/Inventory/InventoryType.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"
#include "../UI.h"
#include "UIFamily.h"
#include "UIItemInventory.h"
#include "UIMiscShells.h"
#include "../../Util/Locale.h"
#include "UIMonsterBook.h"
#include "UISoftKey.h"
#include "UIChatBar.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#include "../../Configuration.h"
#endif

namespace ms
{
	UICharInfo::UICharInfo(int32_t cid) : UIDragElement<PosCHARINFO>(), is_loading(true), timestep(Constants::TIMESTEP), personality_enabled(false), collect_enabled(false), damage_enabled(false), item_enabled(false), target_cid(cid)
	{
		nl::node close = UiAssetResolve::close_btn();

		nl::node UserInfo = UiAssetResolve::window("UserInfo");
		nl::node character = UserInfo["character"] ? UserInfo["character"] : UserInfo;
		nl::node backgrnd = character["backgrnd"] ? character["backgrnd"] : UserInfo["backgrnd"];

		if (!UiAssetResolve::usable(UserInfo) || !backgrnd)
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		/// Main Window
		// 正确端个人情报框默认态 = 仅 backgrnd(275x199)。backgrnd2/5/7 是"子窗全展开"
		// 大背景板（含种类/亲密度空槽），旧代码无条件叠加导致默认态出现空框/空白面板；
		// 这些板由 show_*_window 的子窗 sprite 各自负责，主窗只画 backgrnd。
		sprites.emplace_back(backgrnd);

		Point<int16_t> backgrnd_dim = Texture(backgrnd).get_dimensions();
		Point<int16_t> close_dimensions = Point<int16_t>(backgrnd_dim.x() - 21, 6);

		auto try_btn = [&](uint16_t id, nl::node src, Point<int16_t> pos = {})
		{
			if (src)
				buttons[id] = std::make_unique<MapleButton>(src, pos);
		};

		// 坐标以正确端截图（个人请报框.png）逐像素量取，相对主窗左上角。
		// 关闭钮
		try_btn(Buttons::BtClose, close, close_dimensions);
		// 顶部功能按钮区：学院(40x37) / 组队邀请(59x17) / 道具(60x19) / 交易邀请(59x17) / 想买的(60x19)
		try_btn(Buttons::BtFamily, character["BtFamily"] ? character["BtFamily"] : UserInfo["BtFamily"], Point<int16_t>(100, 20));
		try_btn(Buttons::BtParty, character["BtParty"] ? character["BtParty"] : UserInfo["BtParty"], Point<int16_t>(148, 22));
		try_btn(Buttons::BtItem, character["BtItem"] ? character["BtItem"] : UserInfo["BtItem"], Point<int16_t>(209, 22));
		try_btn(Buttons::BtTrad, character["BtTrade"] ? character["BtTrade"] : UserInfo["BtTrade"], Point<int16_t>(148, 44));
		try_btn(Buttons::BtWish, character["BtWish"] ? character["BtWish"] : UserInfo["BtWish"], Point<int16_t>(209, 44));
		// 底部按钮区：骑宠(47x17) / 宠物(49x17) / 收藏(49x17) / 排除道具(71x17)
		try_btn(Buttons::BtRide, character["BtTamingShow"] ? character["BtTamingShow"] : UserInfo["BtTamingShow"], Point<int16_t>(5, 173));
		try_btn(Buttons::BtPet, character["BtPetShow"] ? character["BtPetShow"] : UserInfo["BtPetShow"], Point<int16_t>(93, 173));
		try_btn(Buttons::BtCollect, character["BtCollectionShow"] ? character["BtCollectionShow"] : UserInfo["BtCollectionShow"], Point<int16_t>(147, 173));
		try_btn(Buttons::BtException, character["BtExceptionShow"] ? character["BtExceptionShow"] : UserInfo["BtExceptionShow"], Point<int16_t>(198, 173));
		// 兼容保留：旧子窗/调试按钮（运行时扁平 UserInfo 多不存在，try_btn guard 跳过），不给定坐标。
		try_btn(Buttons::BtDamage, character["BtDamage"]);
		try_btn(Buttons::BtPersonality, character["BtPersonality"]);
		try_btn(Buttons::BtPopDown, character["BtPopDown"]);
		try_btn(Buttons::BtPopUp, character["BtPopUp"]);
		// 不加载 BtBook：正确端主窗无此按钮（「道具/想买的」已占右上区域）。

		// 正确端个人情报框字段值：深色黑字、A11M 字重，与粉底标签同一行基线。
		// 旧代码用 EMPEROR 灰字 + 值起点 y=65 → 观感「字体不对/位置偏上」（实测值字 y≈60，标签 y≈75）。
		name = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE);
		job = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK);
		level = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK);
		fame = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK);
		guild = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK);
		alliance = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK);

		// 正确端底部按钮（骑宠/宠物/收藏/排除道具）normal 态贴图本身即带文字，
		// 不再强制 DISABLED——旧代码把 BtPet/BtRide 设为禁用导致渲染成灰空框。

		/// Farm
		nl::node farm = UserInfo["farm"];
		nl::node farm_backgrnd = farm["backgrnd"];

		loading = farm["loading"];

		farm_dim = farm_backgrnd ? Texture(farm_backgrnd).get_dimensions() : Point<int16_t>(0, 0);
		farm_adj = Point<int16_t>(-farm_dim.x(), 0);

		if (farm_backgrnd)
			sprites.emplace_back(farm_backgrnd, farm_adj);
		if (farm["backgrnd2"])
			sprites.emplace_back(farm["backgrnd2"], farm_adj);
		if (farm["default"])
			sprites.emplace_back(farm["default"], farm_adj);
		if (farm["cover"])
			sprites.emplace_back(farm["cover"], farm_adj);

		try_btn(Buttons::BtFriend, farm["btFriend"], farm_adj);
		try_btn(Buttons::BtVisit, farm["btVisit"], farm_adj);

		farm_name = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::SUPERNOVA);
		farm_level = Charset(farm["number"], Charset::Alignment::LEFT);

#pragma region BottomWindow
		bottom_window_adj = Point<int16_t>(0, backgrnd_dim.y() + 1);

		/// Personality
		nl::node personality = UserInfo["personality"];
		nl::node personality_backgrnd = personality["backgrnd"];

		if (personality_backgrnd)
			personality_sprites.emplace_back(personality_backgrnd, bottom_window_adj);
		if (personality["backgrnd2"])
			personality_sprites.emplace_back(personality["backgrnd2"], bottom_window_adj);

		if (personality["backgrnd3"])
			personality_sprites_enabled[true].emplace_back(personality["backgrnd3"], bottom_window_adj);
		if (personality["backgrnd4"])
			personality_sprites_enabled[true].emplace_back(personality["backgrnd4"], bottom_window_adj);
		if (personality["center"])
			personality_sprites_enabled[true].emplace_back(personality["center"], bottom_window_adj);
		if (personality["before30level"])
			personality_sprites_enabled[false].emplace_back(personality["before30level"], bottom_window_adj);

		personality_dimensions = Texture(personality_backgrnd).get_dimensions();

		/// Collect
		nl::node collect = UserInfo["collect"];
		nl::node collect_backgrnd = collect["backgrnd"];

		if (collect_backgrnd)
			collect_sprites.emplace_back(collect_backgrnd, bottom_window_adj);
		if (collect["backgrnd2"])
			collect_sprites.emplace_back(collect["backgrnd2"], bottom_window_adj);

		default_medal = collect["icon1"];

		try_btn(Buttons::BtArrayGet, collect["BtArrayGet"], bottom_window_adj);
		try_btn(Buttons::BtArrayName, collect["BtArrayName"], bottom_window_adj);

		medal_text = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR, "-");
		medal_total = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR, "2");

		collect_dimensions = Texture(collect_backgrnd).get_dimensions();

		/// Damage
		nl::node damage = UserInfo["damage"];
		nl::node damage_backgrnd = damage["backgrnd"];

		if (damage_backgrnd)
			damage_sprites.emplace_back(damage_backgrnd, bottom_window_adj);
		if (damage["backgrnd2"])
			damage_sprites.emplace_back(damage["backgrnd2"], bottom_window_adj);
		if (damage["backgrnd3"])
			damage_sprites.emplace_back(damage["backgrnd3"], bottom_window_adj);

		try_btn(Buttons::BtFAQ, damage["BtFAQ"], bottom_window_adj);
		try_btn(Buttons::BtRegist, damage["BtRegist"], bottom_window_adj);

		damage_dimensions = Texture(damage_backgrnd).get_dimensions();
#pragma endregion

#pragma region RightWindow
		right_window_adj = Point<int16_t>(backgrnd_dim.x(), 0);

		/// Item
		nl::node item = UserInfo["item"];
		nl::node item_backgrnd = item["backgrnd"];

		if (item_backgrnd)
			item_sprites.emplace_back(item_backgrnd, right_window_adj);
		if (item["backgrnd2"])
			item_sprites.emplace_back(item["backgrnd2"], right_window_adj);

		item_dimensions = Texture(item_backgrnd).get_dimensions();
#pragma endregion

		/// Full-fill 子窗背景板（运行时 UserInfo.img 独立 canvas）
		// 右侧浮层板（240x162/159x147）挂在 right_window_adj；整窗展开板（275宽）挂 (0,0)。
		panel_offset = right_window_adj;
		if (UserInfo["backgrnd2"]) panel_bg_pet = Sprite(UserInfo["backgrnd2"], Point<int16_t>(0, 0));
		if (UserInfo["backgrnd5"]) panel_bg_mount = Sprite(UserInfo["backgrnd5"], Point<int16_t>(0, 0));
		if (UserInfo["backgrnd7"]) panel_bg_medal = Sprite(UserInfo["backgrnd7"], Point<int16_t>(0, 0));
		if (UserInfo["backgrnd3"]) panel_bg_wish = Sprite(UserInfo["backgrnd3"], panel_offset);
		if (UserInfo["backgrnd4"]) panel_bg_item = Sprite(UserInfo["backgrnd4"], panel_offset);
		if (UserInfo["backgrnd6"]) panel_bg_exclude = Sprite(UserInfo["backgrnd6"], panel_offset);

		// 子窗数据文本（A11M/LEFT/EMPEROR 与字段区风格一致）
		for (int i = 0; i < 3; i++)
		{
			pet_name_texts.emplace_back(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE);
			pet_level_texts.emplace_back(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR);
		}
		mount_info_text = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR);
		mount_exinfo_text = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR);
		for (int i = 0; i < 6; i++)
		{
			wish_item_texts.emplace_back(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR);
			item_slot_texts.emplace_back(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR);
		}
		book_info_text = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR);
		medal_info_text = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR);
		for (int i = 0; i < 12; i++)
			medal_quest_texts.emplace_back(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE);

		dimension = backgrnd_dim;
		dragarea = Point<int16_t>(dimension.x(), 20);

		target_character = Stage::get().get_character(cid).get();

		CharInfoRequestPacket(cid).dispatch();
	}

	void UICharInfo::draw(float inter) const
	{
		UIElement::draw_sprites(inter);

		for (size_t i = 0; i < Buttons::BtArrayGet; i++)
		{
			auto it = buttons.find(static_cast<uint16_t>(i));
			if (it != buttons.end() && it->second)
				it->second->draw(position);
		}
		// 正确端顶部「想买的」/ 底部「排除道具」按钮（追加枚举值，不落在旧循环范围）
		for (uint16_t id : { static_cast<uint16_t>(Buttons::BtWish), static_cast<uint16_t>(Buttons::BtException) })
		{
			auto it = buttons.find(id);
			if (it != buttons.end() && it->second)
				it->second->draw(position);
		}

		/// Main Window
		// 正确端（个人请报框.png，r04 量取）：粉底标签 x=102 起、y=75/93/111/129/147（行距18）；
		// 值起点 x≈155，与标签同一基线。旧值 text_pos=(153,65) 使值字 y≈60，比标签偏上约19px。
		// 修正为 y=75 首行，与标签齐平；字体 A11M 深黑（构造函数已改）。
		int16_t row_height = 18;
		Point<int16_t> text_pos = position + Point<int16_t>(155, 75);

		// 目标可能已离图：禁止裸解引用（双击开窗后角色瞬移会空指针崩绘）。
		if (target_character)
			target_character->draw_preview(position + Point<int16_t>(63, 129), inter);

		// 名字条：正确端 y=147..162，居中文字基线取 y=150（旧值 131 偏上约16px）。
		name.draw(position + Point<int16_t>(59, 150));
		level.draw(text_pos + Point<int16_t>(0, row_height * 0));
		job.draw(text_pos + Point<int16_t>(0, row_height * 1));
		fame.draw(text_pos + Point<int16_t>(0, row_height * 2));
		guild.draw(text_pos + Point<int16_t>(0, row_height * 3));
		alliance.draw(text_pos + Point<int16_t>(0, row_height * 4));

		/// Farm（v83 扁平面常无 farm 节点；invalid Texture 跳过）
		Point<int16_t> farm_pos = position + farm_adj;

		if (is_loading && farm_dim.x() > 0)
			loading.draw(farm_pos, inter);

		farm_name.draw(farm_pos + Point<int16_t>(136, 51));
		farm_level.draw(farm_level_text, farm_pos + Point<int16_t>(126, 34));

		/// Personality
		if (personality_enabled)
		{
			for (Sprite sprite : personality_sprites)
				sprite.draw(position, inter);

			bool show_personality = target_character && (target_character->get_level() >= 30);

			for (Sprite sprite : personality_sprites_enabled[show_personality])
				sprite.draw(position, inter);
		}

		/// Collect
		if (collect_enabled)
		{
			for (Sprite sprite : collect_sprites)
				sprite.draw(position, inter);

			for (size_t i = 0; i < 15; i++)
			{
				div_t div = std::div(i, 5);
				default_medal.draw(position + bottom_window_adj + Point<int16_t>(61, 66) + Point<int16_t>(38 * div.rem, 38 * div.quot), inter);
			}

			// buttons 是稀疏 map（try_btn 只在资源节点存在时插入），裸 at(i) 对缺失 id
			// 会抛 std::out_of_range；主窗口 :225-230 的 find+判空才是库层要求范式
			// （IO/UIElement.cpp:168-176 注释明言「必须判空后再解引用」）。
			for (size_t i = Buttons::BtArrayGet; i < Buttons::BtFAQ; i++)
			{
				auto it = buttons.find(static_cast<uint16_t>(i));
				if (it != buttons.end() && it->second)
					it->second->draw(position);
			}

			Point<int16_t> text_pos = Point<int16_t>(121, 8);

			medal_text.draw(position + bottom_window_adj + text_pos);
			medal_total.draw(position + bottom_window_adj + text_pos + Point<int16_t>(0, 19));
		}

		/// Damage
		if (damage_enabled)
		{
			for (Sprite sprite : damage_sprites)
				sprite.draw(position, inter);

			// 同上：稀疏 map 用 find 判空。damage 子窗键域是 [BtFAQ, BtBook)（BtFAQ=16 /
			// BtRegist=17），与主窗 [0,BtArrayGet)、collect [BtArrayGet,BtFAQ) 同为枚举分段；
			// 原上界 buttons.size() 是元素个数而非键域，缺资源时漏绘子窗按钮、键齐时又重复绘 BtBook(18)（:317 单独绘）。
			for (size_t i = Buttons::BtFAQ; i < Buttons::BtBook; i++)
			{
				auto it = buttons.find(static_cast<uint16_t>(i));
				if (it != buttons.end() && it->second)
					it->second->draw(position);
			}
		}

		/// Item
		if (item_enabled)
			for (Sprite sprite : item_sprites)
				sprite.draw(position, inter);

		// 正确端主窗无独立 BOOK INFO 按钮：顶部「道具/想买的」已占该区域；
		// 旧的 BtBook @(backgrnd_dim.x()-40,40) 会压在「想买的」上，故不加载也不绘制。

		// 全功能子窗（宠物/坐骑/心愿/道具/勋章/排除道具）叠加绘制
		draw_subpanel(inter);
		// 人气 ± 箭头（默认态显示；子窗展开时随整窗板覆盖）
		if (active_panel == SubPanel::NONE)
			draw_fame_arrows();
	}

	void UICharInfo::update()
	{
		if (timestep >= Constants::TIMESTEP * UCHAR_MAX)
		{
			is_loading = false;
		}
		else
		{
			loading.update();
			timestep += Constants::TIMESTEP;
		}
	}

	Button::State UICharInfo::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::BtClose:
			{
				deactivate();

				return Button::State::NORMAL;
			}
			case Buttons::BtFamily:
			{
				int32_t self = Stage::get().get_player().get_oid();
				std::string n = target_character ? target_character->get_name() : name.get_text();
				if (target_cid > 0 && target_cid != self && !n.empty()) {
					FamilyAddPacket(n).dispatch();
					LOG(LOG_DEBUG, "UICharInfo: FamilyAdd name=" << n);
				} else {
					OpenFamilyPacket().dispatch();
					UI::get().emplace<UIFamily>();
					LOG(LOG_DEBUG, "UICharInfo: Family open");
				}
				return Button::State::NORMAL;
			}
			case Buttons::BtParty:
			{
				std::string n = target_character ? target_character->get_name() : name.get_text();
				if (!n.empty()) {
					PartyInvitePacket(n).dispatch();
					LOG(LOG_DEBUG, "UICharInfo: PartyInvite name=" << n);
				}
				return Button::State::NORMAL;
			}
			case Buttons::BtItem:
			{
				show_subpanel(SubPanel::ITEM);

				return Button::State::NORMAL;
			}
			case Buttons::BtBook:
			{
				UI::get().emplace<UIMonsterBook>();

				return Button::State::NORMAL;
			}
			case Buttons::BtWish:
			{
				// 「想买的」= 心愿单子窗（UserInfo/backgrnd3），数据来自 CHAR_INFO wishlist。
				show_subpanel(SubPanel::WISH);
				return Button::State::NORMAL;
			}
			case Buttons::BtException:
			{
				// 「排除道具」子窗（UserInfo/backgrnd6）。列表数据由 PET_EXCEPTION_LIST(173)
				// 提供，当前 handler 仅记录日志；子窗面板先展示原生背景 + 已有数据占位。
				show_subpanel(SubPanel::EXCLUDE);
				return Button::State::NORMAL;
			}
			case Buttons::BtRide:
			{
				show_subpanel(SubPanel::MOUNT);
				return Button::State::NORMAL;
			}
			case Buttons::BtPet:
			{
				show_subpanel(SubPanel::PET);
				return Button::State::NORMAL;
			}
			case Buttons::BtCollect:
			{
				// 收藏 → 勋章/图鉴子窗（UserInfo/backgrnd7）
				show_subpanel(SubPanel::MEDAL);
				return Button::State::NORMAL;
			}
			case Buttons::BtPersonality:
			case Buttons::BtDamage:
			{
				show_bottom_window(buttonid);

				return Button::State::NORMAL;
			}
			case Buttons::BtFriend:
			{
				std::string n = target_character ? target_character->get_name() : name.get_text();
				if (!n.empty()) {
					BuddyAddPacket(n, "Default Group").dispatch();
					LOG(LOG_DEBUG, "UICharInfo: BuddyAdd name=" << n);
				}
				return Button::State::NORMAL;
			}
			case Buttons::BtTrad:
			{
				if (target_cid > 0) {
					dispatch_trade_invite(target_cid);
					LOG(LOG_DEBUG, "UICharInfo: TradeInvite cid=" << target_cid);
				}
				return Button::State::NORMAL;
			}
			case Buttons::BtVisit:
			{
				// Angel CUIUserInfo 私聊入口 = CUIStatusBar::SetWhisperTarget（切密语频道 +
				// 预置 "/w <名字> "，焦点留给玩家输入），与右键菜单 BtWhisper 同语义。
				std::string n = target_character ? target_character->get_name() : name.get_text();
				if (n.empty())
					n = get_target_name();
				if (!n.empty()) {
					if (auto chat = UI::get().get_element<UIChatBar>())
						chat->set_whisper_target(n);
					else
						UI::get().emplace<UIChatBar>()->set_whisper_target(n);
					deactivate();
				}
				return Button::State::NORMAL;
			}
			case Buttons::BtPopUp:
			{
				if (target_cid > 0) {
					GiveFamePacket(target_cid, true).dispatch();
					LOG(LOG_DEBUG, "UICharInfo: Fame up cid=" << target_cid);
				}
				return Button::State::NORMAL;
			}
			case Buttons::BtPopDown:
			{
				if (target_cid > 0) {
					GiveFamePacket(target_cid, false).dispatch();
					LOG(LOG_DEBUG, "UICharInfo: Fame down cid=" << target_cid);
				}
				return Button::State::NORMAL;
			}
			case Buttons::BtFAQ:
			{
				// 全客户端「帮助/问号」统一走 ENTER_MTS，落点由服务端 use_mts 决定。
				EnterMtsPacket().dispatch();
				LOG(LOG_INFO, "UICharInfo: FAQ -> ENTER_MTS");
				return Button::State::NORMAL;
			}
			case Buttons::BtRegist:
			{
				if (auto uii = UI::get().get_element<UIItemInventory>()) {
					if (uii->get_tab() == InventoryType::Id::CASH) {
						int16_t slot = uii->get_selected_slot();
						if (slot > 0) {
							const Inventory& inv = Stage::get().get_player().get_inventory();
							int32_t itemid = inv.get_item_id(InventoryType::Id::CASH, slot);
							if (itemid > 0) {
								UseCashItemBasicPacket(slot, itemid).dispatch();
								LOG(LOG_DEBUG, "UICharInfo: BtRegist cash slot=" << slot << " id=" << itemid);
								return Button::State::NORMAL;
							}
						}
					}
				}
				
				// 伤害皮肤：s9 现已支持——characters.damage_skin 列 + 登录 DAMAGE_SKIN(247)
				// 包下发 + DAMAGE_MONSTER 包附带 skin id；客户端 Data/Effect/DamageSkin.img
				// 已移植 60+ 套皮肤，DamageNumber 按 id 切换字形、缺失回退 BasicEff。
				// 本按钮仅作「按现金道具 id 发送使用包」的调试入口；
				// 正式的皮肤选择 UI 按需求再接入。
				return Button::State::NORMAL;
			}
			case Buttons::BtArrayGet:
			case Buttons::BtArrayName:
			{
				std::string n = target_character ? target_character->get_name() : name.get_text();
				if (n.empty())
					n = get_target_name();
				if (!n.empty()) {
					FindPlayerPacket(n).dispatch();
					LOG(LOG_DEBUG, "UICharInfo: Array FindPlayer=" << n);
				} else {
					
				}
				return Button::State::NORMAL;
			}
			default:
			{
				break;
			}
		}

		return Button::State::DISABLED;
	}

	bool UICharInfo::is_in_range(Point<int16_t> cursorpos) const
	{
		Rectangle<int16_t> bounds = Rectangle<int16_t>(position, position + dimension);

		Rectangle<int16_t> farm_bounds = Rectangle<int16_t>(position, position + farm_dim);
		farm_bounds.shift(farm_adj);

		Rectangle<int16_t> bottom_bounds = Rectangle<int16_t>(Point<int16_t>(0, 0), Point<int16_t>(0, 0));
		Rectangle<int16_t> right_bounds = Rectangle<int16_t>(Point<int16_t>(0, 0), Point<int16_t>(0, 0));

		if (personality_enabled)
		{
			bottom_bounds = Rectangle<int16_t>(position, position + personality_dimensions);
			bottom_bounds.shift(bottom_window_adj);
		}

		if (collect_enabled)
		{
			bottom_bounds = Rectangle<int16_t>(position, position + collect_dimensions);
			bottom_bounds.shift(bottom_window_adj);
		}

		if (damage_enabled)
		{
			bottom_bounds = Rectangle<int16_t>(position, position + damage_dimensions);
			bottom_bounds.shift(bottom_window_adj);
		}

		// 旧 item_enabled 只覆盖 UserInfo/item 小浮层；六子窗改走 active_panel + 独立底板。
		if (item_enabled)
		{
			right_bounds = Rectangle<int16_t>(position, position + item_dimensions);
			right_bounds.shift(right_window_adj);
		}

		auto panel_bounds = [&](const Sprite& bg, bool use_panel_offset) -> Rectangle<int16_t>
		{
			if (bg.width() <= 0 || bg.height() <= 0)
				return Rectangle<int16_t>(Point<int16_t>(0, 0), Point<int16_t>(0, 0));

			Point<int16_t> origin = use_panel_offset ? (position + panel_offset) : position;
			return Rectangle<int16_t>(origin, origin + Point<int16_t>(bg.width(), bg.height()));
		};

		Rectangle<int16_t> sub_bounds(Point<int16_t>(0, 0), Point<int16_t>(0, 0));

		switch (active_panel)
		{
		case SubPanel::ITEM:
			sub_bounds = panel_bounds(panel_bg_item, true);
			break;
		case SubPanel::WISH:
			sub_bounds = panel_bounds(panel_bg_wish, true);
			break;
		case SubPanel::EXCLUDE:
			sub_bounds = panel_bounds(panel_bg_exclude, true);
			break;
		case SubPanel::PET:
			sub_bounds = panel_bounds(panel_bg_pet, false);
			break;
		case SubPanel::MOUNT:
			sub_bounds = panel_bounds(panel_bg_mount, false);
			break;
		case SubPanel::MEDAL:
			sub_bounds = panel_bounds(panel_bg_medal, false);
			break;
		default:
			break;
		}

		return bounds.contains(cursorpos)
			|| farm_bounds.contains(cursorpos)
			|| bottom_bounds.contains(cursorpos)
			|| right_bounds.contains(cursorpos)
			|| sub_bounds.contains(cursorpos);
	}

	Cursor::State UICharInfo::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State state = UIDragElement<PosCHARINFO>::send_cursor(clicked, cursorpos);

		// 道具/想买的：每帧重挂 tip（UIStateGame 派发入口先清 tooltip，仅 on-change 会一闪即没）。
		int16_t new_hover = -1;
		if (active_panel == SubPanel::ITEM || active_panel == SubPanel::WISH)
		{
			const std::vector<ItemSlotView>& views = (active_panel == SubPanel::ITEM) ? item_views : wish_views;
			Point<int16_t> org = position + panel_offset;
			for (size_t i = 0; i < views.size(); i++)
			{
				if (!views[i].itemid)
					continue;
				Rectangle<int16_t> hit = views[i].hit;
				hit.shift(org);
				if (hit.contains(cursorpos))
				{
					new_hover = (active_panel == SubPanel::ITEM)
						? static_cast<int16_t>(i)
						: static_cast<int16_t>(100 + i);
					break;
				}
			}
		}

		hovered_slot = new_hover;
		if (hovered_slot >= 0)
		{
			int32_t iid = (hovered_slot < 100)
				? (hovered_slot < static_cast<int16_t>(item_views.size()) ? item_views[hovered_slot].itemid : 0)
				: ((hovered_slot - 100) < static_cast<int16_t>(wish_views.size()) ? wish_views[hovered_slot - 100].itemid : 0);
			if (iid)
				UI::get().show_item(Tooltip::Parent::ITEMINVENTORY, iid);
			else
				UI::get().clear_tooltip(Tooltip::Parent::ITEMINVENTORY);
		}
		else
			UI::get().clear_tooltip(Tooltip::Parent::ITEMINVENTORY);

		return state;
	}

	void UICharInfo::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		// 快捷键须尊重按钮禁用态（如「看自己」时禁用组队/交易/好友），避免绕过门禁。
		auto enabled = [&](uint16_t id) {
			auto it = buttons.find(id);
			return it == buttons.end() || !it->second || it->second->get_state() != Button::State::DISABLED;
		};
		if (keycode == '+' || keycode == '=')
		{
			if (enabled(Buttons::BtPopUp)) button_pressed(Buttons::BtPopUp);
		}
		else if (keycode == '-' || keycode == '_')
		{
			if (enabled(Buttons::BtPopDown)) button_pressed(Buttons::BtPopDown);
		}
		else if (keycode == 'W' || keycode == 'w')
			button_pressed(Buttons::BtVisit);
		else if (keycode == 'T' || keycode == 't')
		{
			if (enabled(Buttons::BtTrad)) button_pressed(Buttons::BtTrad);
		}
		else if (keycode == 'B' || keycode == 'b')
		{
			if (enabled(Buttons::BtFriend)) button_pressed(Buttons::BtFriend);
		}
		else if (keycode == 'P' || keycode == 'p')
		{
			if (enabled(Buttons::BtParty)) button_pressed(Buttons::BtParty);
		}
		else if (keycode == 'F' || keycode == 'f')
		{
			if (target_cid > 0) {
				
			} else {
				
			}
		}
		else if (keycode == 'S' || keycode == 's')
		{
			std::string n = target_character ? target_character->get_name() : name.get_text();
			auto do_whisper = [](const std::string& to, const std::string& msg) {
				if (msg.empty()) return;
				// 统一走 send_whisper_external 以暂存正文，WhisperResult 成功时回显。
				if (auto chat = UI::get().get_element<UIChatBar>())
					chat->send_whisper_external(to, msg);
				else
					WhisperPacket(to, msg).dispatch();
			};
			if (!n.empty()) {
				
			} else {
				
			}
		}
		else if (keycode == 'M' || keycode == 'm')
		{
			std::string n = target_character ? target_character->get_name() : name.get_text();
			if (!n.empty()) {
				MessengerInvitePacket(n).dispatch();
				LOG(LOG_DEBUG, "UICharInfo: MessengerInvite name=" << n);
			}
		}
		else if (keycode == 'N' || keycode == 'n')
		{
			// NOTE_ACTION(131) action0：商城便签额度 >0 时 s9 才落库（NoteActionHandler）。
			// SoftKey 补齐 CharInfo「写便签」发包路径（parity-gap §1.2-E 复核项）。
			std::string n = target_character ? target_character->get_name() : name.get_text();
			if (n.empty())
				n = get_target_name();
			if (!n.empty()) {
				
			} else {
				
			}
		}
		else if (keycode == 'Y' || keycode == 'y')
		{
			button_pressed(Buttons::BtFamily);
		}
		else if (keycode == 'O' || keycode == 'o')
		{
			// 怪物图鉴入口（正确端主窗无独立 BtBook 按钮，改用快捷键 O）
			button_pressed(Buttons::BtBook);
		}
		else if (keycode == 'C' || keycode == 'c')
		{
			auto ui = UI::get().get_element<UICharacterClone>();
			if (!ui) ui = UI::get().emplace<UICharacterClone>();
			else if (!ui->is_active()) ui->makeactive();
		}
		else if (keycode == 'R' || keycode == 'r')
		{
			auto ui = UI::get().get_element<UICancelRequests>();
			if (!ui) ui = UI::get().emplace<UICancelRequests>();
			else if (!ui->is_active()) ui->makeactive();
		}
		else if (keycode == 'E' || keycode == 'e')
		{
			auto ui = UI::get().get_element<UIMapleEvent>();
			if (!ui) ui = UI::get().emplace<UIMapleEvent>();
			else if (!ui->is_active()) ui->makeactive();
		}
		else if (keycode == 'K' || keycode == 'k')
		{
			auto ui = UI::get().get_element<UIMonsterKilling>();
			if (!ui) ui = UI::get().emplace<UIMonsterKilling>();
			else if (!ui->is_active()) ui->makeactive();
		}
		else if (keycode == 'A' || keycode == 'a')
		{
			auto ui = UI::get().get_element<UIUtilDlgExAvatar>();
			if (!ui) ui = UI::get().emplace<UIUtilDlgExAvatar>();
			else if (!ui->is_active()) ui->makeactive();
		}
		else if (keycode == 'Z' || keycode == 'z')
		{
			button_pressed(Buttons::BtPet);
			auto pet = UI::get().get_element<UIUtilDlgExPet>();
			if (!pet) pet = UI::get().emplace<UIUtilDlgExPet>();
			else if (!pet->is_active()) pet->makeactive();
		}
	}

	UIElement::Type UICharInfo::get_type() const
	{
		return TYPE;
	}

	std::string UICharInfo::get_target_name() const
	{
		if (target_character)
			return target_character->get_name();
		return name.get_text();
	}

	void UICharInfo::update_stats(int32_t character_id, int16_t job_id, uint16_t lv, int16_t f, std::string g, std::string a)
	{
		int32_t player_id = Stage::get().get_player().get_oid();

		if (character_id == player_id)
		{
			auto disable = [&](uint16_t id)
			{
				auto it = buttons.find(id);
				if (it != buttons.end() && it->second)
					it->second->set_state(Button::State::DISABLED);
			};
			// 正确端「看自己」态：组队邀请 / 交易邀请为灰禁用（对自己无意义）；
			// 道具/想买的保持粉红正常态。
			disable(Buttons::BtParty);
			disable(Buttons::BtTrad);
			disable(Buttons::BtPopDown);
			disable(Buttons::BtPopUp);
			disable(Buttons::BtFriend);
		}

		Job character_job = Job(job_id);

		// 目标可能已离图 / 跨频道（CHAR_INFO 回包到达时 Stage 里查不到角色）：
		// target_character 为 nullptr，不得裸解引用。名字优先用现场对象，回退 name 当前值/占位。
		std::string target_name = target_character ? target_character->get_name() : name.get_text();
		if (target_name.empty())
			target_name = "-";
		name.change_text(target_name);
		job.change_text(character_job.get_name());
		level.change_text(std::to_string(lv));
		fame.change_text(std::to_string(f));

		if (g == "")
			guild.change_text("-");
		else
			guild.change_text(g);

		alliance.change_text(a);

		farm_name.change_text("");
		farm_level_text = "1";
	}

	void UICharInfo::update_extras(int pet_count, int wish_count, int32_t book_cards, int32_t medal_id, bool married)
	{
		medal_text.change_text(
			Locale::format_all("charinfo.pet_wish", { std::to_string(pet_count), std::to_string(wish_count) })
			+ (medal_id ? (" " + Locale::format("charinfo.medal_id", std::to_string(medal_id))) : "")
			+ (married ? (" " + Locale::text("charinfo.married")) : "")
		);
		medal_total.change_text(Locale::format("charinfo.book_count", std::to_string(book_cards)));
	}

	void UICharInfo::show_bottom_window(uint16_t buttonid)
	{
		personality_enabled = false;
		collect_enabled = false;
		damage_enabled = false;

		switch (buttonid)
		{
			case Buttons::BtPersonality:
				personality_enabled = true;
				break;
			case Buttons::BtCollect:
				collect_enabled = true;
				break;
			case Buttons::BtDamage:
				damage_enabled = true;
				break;
		}
	}

	void UICharInfo::show_right_window(uint16_t buttonid)
	{
		item_enabled = false;

		switch (buttonid)
		{
			case Buttons::BtItem:
				item_enabled = true;
				break;
		}
	}

	void UICharInfo::update_full(const CharInfoData& data)
	{
		full_data = data;
		has_full_data = true;

		// 宠物：名字 + Lv/亲密度/饱清感（空态 "-"，避免整片空白被误判未实现）
		for (size_t i = 0; i < pet_name_texts.size(); i++)
		{
			if (i < data.pets.size() && data.pets[i].itemid != 0)
			{
				const auto& p = data.pets[i];
				pet_name_texts[i].change_text(p.name.empty() ? "-" : p.name);
				pet_level_texts[i].change_text(
					"Lv." + std::to_string(p.level)
					+ "  " + std::to_string(p.tameness)
					+ "  " + std::to_string(p.fullness));
			}
			else
			{
				pet_name_texts[i].change_text("-");
				pet_level_texts[i].change_text("-");
			}
		}

		// 坐骑（backgrnd5 下半：种类/等级/经验值/疲劳度 标签由资源烘焙，值画右侧）
		// 旧代码 "Mount:.../EXP:/Tired:" 打印在背景上（用户图6 "Mount:" 裸字即此），改为纯值/空态。
		if (data.mount.id != 0)
		{
			const ItemData& mount_item = ItemData::get(data.mount.id);
			mount_info_text.change_text(std::to_string(data.mount.id)
				+ (mount_item ? (" " + mount_item.get_name()) : "")
				+ "  Lv." + std::to_string(data.mount.level));
			mount_exinfo_text.change_text(std::to_string(data.mount.exp)
				+ " / " + std::to_string(data.mount.tiredness));
		}
		else
		{
			mount_info_text.change_text("-");
			mount_exinfo_text.change_text("-");
		}

		// 心愿单（backgrnd3 三空槽）：itemid → 道具名 + 图标；空槽显示占位。
		auto build_view = [](int32_t itemid, int16_t left, int16_t top) -> ItemSlotView
		{
			ItemSlotView v;
			if (itemid != 0)
			{
				const ItemData& idata = ItemData::get(itemid);
				v.itemid = itemid;
				if (idata)
				{
					v.icon = idata.get_icon(idata.get_id() / 10000 == 238);
					v.name = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, idata.get_name());
				}
				else
				{
					v.name = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, std::to_string(itemid));
				}
			}
			// 命中矩形：图标框 x≈5..45、整行 y=top..top+40（相对面板左上角）
			v.hit = Rectangle<int16_t>(left, static_cast<int16_t>(left + 230),
				top, static_cast<int16_t>(top + 40));
			return v;
		};

		wish_views.clear();
		for (size_t i = 0; i < 3; i++)
		{
			int32_t iid = i < data.wishlist.size() ? data.wishlist[i] : 0;
			wish_views.push_back(build_view(iid, 5, static_cast<int16_t>(24 + i * 44)));
			// 兼容旧文本成员（draw 仍可能用到）
			wish_item_texts[i].change_text(iid ? (std::to_string(iid)) : "");
		}

		// 道具子窗（backgrnd4 三空槽）：他人装备 itemid → 图标 + 名称 + tooltip
		item_views.clear();
		for (size_t i = 0; i < 3; i++)
		{
			int32_t iid = i < data.equips.size() ? data.equips[i].itemid : 0;
			item_views.push_back(build_view(iid, 5, static_cast<int16_t>(24 + i * 44)));
			item_slot_texts[i].change_text(iid ? std::to_string(iid) : "");
		}

		// 图鉴 / 勋章（backgrnd7：标签由资源烘焙，这里只填数值，避免英文串压在中文标签上）
		book_info_text.change_text(std::to_string(data.book.level) + " / "
			+ std::to_string(data.book.normal) + " / " + std::to_string(data.book.special)
			+ " / " + std::to_string(data.book.total));
		medal_info_text.change_text((data.medalId ? std::to_string(data.medalId) : std::string("-")));
		for (size_t i = 0; i < medal_quest_texts.size(); i++)
		{
			if (i < data.medalQuests.size())
				medal_quest_texts[i].change_text(std::to_string(data.medalQuests[i]));
			else
				medal_quest_texts[i].change_text("");
		}
	}

	void UICharInfo::show_subpanel(SubPanel panel)
	{
		// toggle：点当前已开的子窗则收起
		active_panel = (active_panel == panel) ? SubPanel::NONE : panel;
		hovered_slot = -1;
		UI::get().clear_tooltip(Tooltip::Parent::ITEMINVENTORY);
	}

	void UICharInfo::draw_pet_panel(float inter) const
	{
		if (panel_bg_pet.width() > 0) panel_bg_pet.draw(position, inter);
		// backgrnd2(275x399) 下半：左侧头像框 x≈5..100，右侧三行信息 x≈110、y≈205/240/275；
		// 底部「种类/等级/亲密度/饱清感」标签由资源烘焙，值画在右侧对应空栏。
		for (size_t i = 0; i < 3 && i < pet_name_texts.size(); i++)
		{
			Point<int16_t> base = position + Point<int16_t>(110, static_cast<int16_t>(205 + i * 35));
			pet_name_texts[i].draw(base);
			pet_level_texts[i].draw(base + Point<int16_t>(0, 15));
		}
		// 空态提示（Admin 无宠物）：避免整片空白被误判为「没实现」
		if (full_data.pets.empty())
		{
			Text empty(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::DARKGREY, "-");
			empty.draw(position + Point<int16_t>(110, 205));
		}
	}

	void UICharInfo::draw_mount_panel(float inter) const
	{
		if (panel_bg_mount.width() > 0) panel_bg_mount.draw(position, inter);
		// backgrnd5(275x361) 下半：「种类/等级/经验值/疲劳度」标签烘焙在 y≈290/310，
		// 值画右侧空栏：种类值 x≈110,y≈290；经验值 x≈270,y≈290；等级值 x≈110,y≈310；疲劳度 x≈270,y≈310。
		// 有坐骑时在左侧预览区画道具图标（非完整骑乘合并）。
		if (full_data.mount.id != 0)
		{
			const ItemData& mid = ItemData::get(full_data.mount.id);
			if (mid)
			{
				Texture icon = mid.get_icon(false);
				if (icon.width() > 0)
					icon.draw(DrawArgument(position + Point<int16_t>(40, 220)));
			}
		}
		mount_info_text.draw(position + Point<int16_t>(110, 288));   // 种类 + Lv
		mount_exinfo_text.draw(position + Point<int16_t>(110, 308)); // 经验 / 疲劳（左列下移避免压种类行）
	}

	void UICharInfo::draw_wish_panel(float inter) const
	{
		Point<int16_t> org = position + panel_offset;
		if (panel_bg_wish.width() > 0) panel_bg_wish.draw(org, inter);
		// backgrnd3 三空槽：图标(x≈8) + 名称(x≈52)；悬浮由 send_cursor 弹 tooltip。
		for (size_t i = 0; i < wish_views.size(); i++)
		{
			const ItemSlotView& v = wish_views[i];
			int16_t y = static_cast<int16_t>(24 + i * 44);
			if (v.itemid)
			{
				if (v.icon.width() > 0) v.icon.draw(DrawArgument(org + Point<int16_t>(8, y)));
				v.name.draw(org + Point<int16_t>(52, y + 2));
			}
		}
	}

	void UICharInfo::draw_item_panel(float inter) const
	{
		Point<int16_t> org = position + panel_offset;
		if (panel_bg_item.width() > 0) panel_bg_item.draw(org, inter);
		// backgrnd4 三空槽：他人装备图标(x≈8) + 名称(x≈52)；悬浮弹 item tooltip。
		for (size_t i = 0; i < item_views.size(); i++)
		{
			const ItemSlotView& v = item_views[i];
			int16_t y = static_cast<int16_t>(24 + i * 44);
			if (v.itemid)
			{
				if (v.icon.width() > 0) v.icon.draw(DrawArgument(org + Point<int16_t>(8, y)));
				v.name.draw(org + Point<int16_t>(52, y + 2));
			}
		}
	}

	void UICharInfo::draw_medal_panel(float inter) const
	{
		if (panel_bg_medal.width() > 0) panel_bg_medal.draw(position, inter);
		// backgrnd7(275x404) 下半：「手册名/总卡片/一般/特殊/佩戴中/总勋章数/获得过的勋章」标签由资源烘焙。
		// 旧值 book_info_text@(150,205) 压在中文标签上（用户图5「一般 .I Nor 特殊 Special」重叠即此）→
		// 改为把数值画在标签右侧空栏：手册名/总卡片值 x≈150,y≈210/225；佩戴中/总勋章数 x≈150,y≈310/330。
		book_info_text.draw(position + Point<int16_t>(150, 210));
		medal_info_text.draw(position + Point<int16_t>(150, 330));
		for (size_t i = 0; i < medal_quest_texts.size() && i < 12; i++)
			medal_quest_texts[i].draw(position + Point<int16_t>(110, static_cast<int16_t>(350 + i * 15)));
		// 空态（Admin 无勋章任务）：避免列表整片空白
		if (full_data.medalQuests.empty())
		{
			Text empty(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::DARKGREY, "-");
			empty.draw(position + Point<int16_t>(110, 350));
		}
	}

	void UICharInfo::draw_fame_arrows() const
	{
		// 运行时 IMG 无独立人气箭头节点，程序化绘制两个灰框按钮（↑加人气/↓减人气）。
		// 正确端（个人请报框.png，r04 量取权威）：按钮 x=232..246 / x=249..263，人气行 y=113..127（14×14）。
		// 旧值 x=158 偏左约 74px；上轮 x=239 又与参照左边界差 7px。本轮对齐 r04 权威值 232/249。
		// 浅灰底 + 深灰描边 + 黑色 ▲/▼ 字形，保证浅色字段底上清晰可见且语义正确（非纯色块）。
		const int16_t ay = 113;
		Point<int16_t> b1 = position + Point<int16_t>(232, ay);
		Point<int16_t> b2 = position + Point<int16_t>(249, ay);
		// 边框（深灰）+ 内底（浅灰）
		ColorBox(14, 14, Color(0x808080FFu), 1.0f).draw(DrawArgument(b1));
		ColorBox(14, 14, Color(0x808080FFu), 1.0f).draw(DrawArgument(b2));
		ColorBox(12, 12, Color(0xE8E8E8FFu), 1.0f).draw(DrawArgument(b1 + Point<int16_t>(1, 1)));
		ColorBox(12, 12, Color(0xE8E8E8FFu), 1.0f).draw(DrawArgument(b2 + Point<int16_t>(1, 1)));
		// ▲ / ▼ 三角（程序化像素三角，居中于按钮）
		Text up(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK, "^");
		Text dn(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK, "v");
		up.draw(position + Point<int16_t>(232 + 7, ay + 1));
		dn.draw(position + Point<int16_t>(249 + 7, ay + 1));
	}

	void UICharInfo::draw_subpanel(float inter) const
	{
		if (!has_full_data) return;
		switch (active_panel)
		{
			case SubPanel::PET:    draw_pet_panel(inter); break;
			case SubPanel::MOUNT:  draw_mount_panel(inter); break;
			case SubPanel::WISH:   draw_wish_panel(inter); break;
			case SubPanel::ITEM:   draw_item_panel(inter); break;
			case SubPanel::MEDAL:  draw_medal_panel(inter); break;
			case SubPanel::EXCLUDE:
				draw_exclude_panel(inter);
				break;
			default: break;
		}
	}

	void UICharInfo::store_excluded_items(int32_t cid, const std::vector<int32_t>& itemids)
	{
		// 按 cid 覆盖暂存（同一角色的多只宠物列表累积）。
		auto& store = excluded_store();
		auto& dst = store[cid];
		dst.insert(dst.end(), itemids.begin(), itemids.end());
	}

	std::vector<int32_t> UICharInfo::excluded_items(int32_t cid)
	{
		auto& store = excluded_store();
		auto it = store.find(cid);
		return it == store.end() ? std::vector<int32_t>() : it->second;
	}

	std::unordered_map<int32_t, std::vector<int32_t>>& UICharInfo::excluded_store()
	{
		static std::unordered_map<int32_t, std::vector<int32_t>> s_store;
		return s_store;
	}

	void UICharInfo::draw_exclude_panel(float inter) const
	{
		if (panel_bg_exclude.width() > 0) panel_bg_exclude.draw(position + panel_offset, inter);
		// 排除道具列表（backgrnd6 159x147 仅标题+空框）：动态行，每行显示 itemid/名称；无数据显示空态。
		Point<int16_t> org = position + panel_offset;
		std::vector<int32_t> items = excluded_items(target_cid);
		if (items.empty())
		{
			Text empty(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::DARKGREY, "无排除道具");
			empty.draw(org + Point<int16_t>(12, 30));
			return;
		}
		for (size_t i = 0; i < items.size() && i < 6; i++)
		{
			const ItemData& idata = ItemData::get(items[i]);
			Text line(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK,
				idata ? idata.get_name() : std::to_string(items[i]));
			line.draw(org + Point<int16_t>(12, static_cast<int16_t>(24 + i * 18)));
		}
	}
}