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

#include "../../Gameplay/Stage.h"

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

#ifdef USE_NX
#include <nlnx/nx.hpp>
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
		sprites.emplace_back(backgrnd);
		if (character["backgrnd2"])
			sprites.emplace_back(character["backgrnd2"]);
		if (character["name"])
			sprites.emplace_back(character["name"]);

		Point<int16_t> backgrnd_dim = Texture(backgrnd).get_dimensions();
		Point<int16_t> close_dimensions = Point<int16_t>(backgrnd_dim.x() - 21, 6);

		auto try_btn = [&](uint16_t id, nl::node src, Point<int16_t> pos = {})
		{
			if (src)
				buttons[id] = std::make_unique<MapleButton>(src, pos);
		};

		try_btn(Buttons::BtClose, close, close_dimensions);
		try_btn(Buttons::BtCollect, character["BtCollect"]);
		try_btn(Buttons::BtDamage, character["BtDamage"]);
		try_btn(Buttons::BtFamily, character["BtFamily"]);
		try_btn(Buttons::BtItem, character["BtItem"]);
		try_btn(Buttons::BtParty, character["BtParty"]);
		try_btn(Buttons::BtPersonality, character["BtPersonality"]);
		try_btn(Buttons::BtPet, character["BtPet"]);
		try_btn(Buttons::BtPopDown, character["BtPopDown"]);
		try_btn(Buttons::BtPopUp, character["BtPopUp"]);
		try_btn(Buttons::BtRide, character["BtRide"]);
		try_btn(Buttons::BtTrad, character["BtTrad"]);
		try_btn(Buttons::BtBook, character["BtBook"], Point<int16_t>(backgrnd_dim.x() - 40, 40));

		name = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE);
		job = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR);
		level = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR);
		fame = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR);
		guild = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR);
		alliance = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR);

		// 坐骑外观未暴露：Ride 保持禁用；Pet 在 load_target 后按 has_pet 切换
		if (auto it = buttons.find(Buttons::BtPet); it != buttons.end() && it->second)
			it->second->set_state(Button::State::DISABLED);
		if (auto it = buttons.find(Buttons::BtRide); it != buttons.end() && it->second)
			it->second->set_state(Button::State::DISABLED);

		/// Farm
		nl::node farm = UserInfo["farm"];
		nl::node farm_backgrnd = farm["backgrnd"];

		loading = farm["loading"];

		farm_dim = Texture(farm_backgrnd).get_dimensions();
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

		medal_text = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR, Locale::text("charinfo.medal_stub"));
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


		dimension = backgrnd_dim;
		dragarea = Point<int16_t>(dimension.x(), 20);

		target_character = Stage::get().get_character(cid).get();
		if (auto it = buttons.find(Buttons::BtPet); it != buttons.end() && it->second)
			it->second->set_state(
				(target_character && target_character->has_pet())
					? Button::State::NORMAL : Button::State::DISABLED);

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

		/// Main Window
		int16_t row_height = 18;
		Point<int16_t> text_pos = position + Point<int16_t>(153, 65);

		target_character->draw_preview(position + Point<int16_t>(63, 129), inter);

		name.draw(position + Point<int16_t>(59, 131));
		level.draw(text_pos + Point<int16_t>(0, row_height * 0));
		job.draw(text_pos + Point<int16_t>(0, row_height * 1));
		fame.draw(text_pos + Point<int16_t>(0, row_height * 2));
		guild.draw(text_pos + Point<int16_t>(0, row_height * 3) + Point<int16_t>(0, 1));
		alliance.draw(text_pos + Point<int16_t>(0, row_height * 4));

		/// Farm
		Point<int16_t> farm_pos = position + farm_adj;

		if (is_loading)
			loading.draw(farm_pos, inter);

		farm_name.draw(farm_pos + Point<int16_t>(136, 51));
		farm_level.draw(farm_level_text, farm_pos + Point<int16_t>(126, 34));

		/// Personality
		if (personality_enabled)
		{
			for (Sprite sprite : personality_sprites)
				sprite.draw(position, inter);

			bool show_personality = (target_character->get_level() >= 30);

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

		/// Monster Book button (always visible)
		if (auto it = buttons.find(Buttons::BtBook); it != buttons.end() && it->second)
			it->second->draw(position);
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
				show_right_window(buttonid);

				return Button::State::NORMAL;
			}
			case Buttons::BtBook:
			{
				UI::get().emplace<UIMonsterBook>();

				return Button::State::NORMAL;
			}
			case Buttons::BtCollect:
			case Buttons::BtPersonality:
			case Buttons::BtRide:
			case Buttons::BtPet:
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
				std::string n = target_character ? target_character->get_name() : name.get_text();
				if (!n.empty()) {
					UI::get().emplace<UISoftKey>([n](const std::string& msg) {
						if (msg.empty()) return;
						WhisperPacket(n, msg).dispatch();
						LOG(LOG_DEBUG, "UICharInfo: Whisper to=" << n << " len=" << msg.size());
					}, []() {}, "Whisper");
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
				UI::get().emplace<UISoftKey>([](const std::string& tip) {
					try {
						int32_t itemid = std::stoi(tip);
						if (itemid <= 0) return;
						const Inventory& inv = Stage::get().get_player().get_inventory();
						int16_t slot = inv.find_item(InventoryType::Id::CASH, itemid);
						if (slot > 0) {
							UseCashItemBasicPacket(slot, itemid).dispatch();
							LOG(LOG_DEBUG, "UICharInfo: BtRegist cash id=" << itemid);
						}
					} catch (...) {}
				}, []() {}, "Damage skin id");
				// 伤害皮肤：运行时 UI.nx/UIWindow.img 无 DamageSkin 节点、Effect.nx
				// BasicEff.img 无 damageSkin 节点（notes/r001 §4），s9 也无对应协议，
				// 故只保留「按现金道具 id 发送使用包」的调试入口；
				// 皮肤切换与资源装载按证据不足显式预留，不凭空造节点。
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
					UI::get().emplace<UISoftKey>([](const std::string& tip) {
						if (tip.empty()) return;
						FindPlayerPacket(tip).dispatch();
						LOG(LOG_DEBUG, "UICharInfo: Array SoftKey FindPlayer=" << tip);
					}, []() {}, "Find name");
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

		int16_t cur_x = cursorpos.x();
		int16_t cur_y = cursorpos.y();

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

		if (item_enabled)
		{
			right_bounds = Rectangle<int16_t>(position, position + item_dimensions);
			right_bounds.shift(right_window_adj);
		}

		return bounds.contains(cursorpos) || farm_bounds.contains(cursorpos) || bottom_bounds.contains(cursorpos) || right_bounds.contains(cursorpos);
	}

	void UICharInfo::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == '+' || keycode == '=')
			button_pressed(Buttons::BtPopUp);
		else if (keycode == '-' || keycode == '_')
			button_pressed(Buttons::BtPopDown);
		else if (keycode == 'W' || keycode == 'w')
			button_pressed(Buttons::BtVisit);
		else if (keycode == 'T' || keycode == 't')
			button_pressed(Buttons::BtTrad);
		else if (keycode == 'B' || keycode == 'b')
			button_pressed(Buttons::BtFriend);
		else if (keycode == 'P' || keycode == 'p')
			button_pressed(Buttons::BtParty);
		else if (keycode == 'F' || keycode == 'f')
		{
			if (target_cid > 0) {
				UI::get().emplace<UISoftKey>([this](const std::string& tip) {
					bool up = true;
					if (!tip.empty() && (tip[0] == 'd' || tip[0] == 'D' || tip[0] == '0' || tip[0] == '-'))
						up = false;
					GiveFamePacket(target_cid, up).dispatch();
					LOG(LOG_DEBUG, "UICharInfo: SoftKey fame cid=" << target_cid << " up=" << up);
				}, []() {}, "Fame up|d (target)");
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					try {
						size_t comma = s.find(',');
						int32_t cid = std::stoi(comma == std::string::npos ? s : s.substr(0, comma));
						bool up = true;
						if (comma != std::string::npos) {
							std::string m = s.substr(comma + 1);
							if (!m.empty() && (m[0] == 'd' || m[0] == 'D' || m[0] == '0' || m[0] == '-'))
								up = false;
						}
						if (cid > 0) {
							GiveFamePacket(cid, up).dispatch();
							LOG(LOG_DEBUG, "UICharInfo: SoftKey fame cid=" << cid << " up=" << up);
						}
					} catch (...) {}
				}, []() {}, "Fame cid[,d]");
			}
		}
		else if (keycode == 'S' || keycode == 's')
		{
			std::string n = target_character ? target_character->get_name() : name.get_text();
			if (!n.empty()) {
				UI::get().emplace<UISoftKey>([n](const std::string& msg) {
					if (msg.empty()) return;
					WhisperPacket(n, msg).dispatch();
					LOG(LOG_DEBUG, "UICharInfo: SoftKey whisper to=" << n);
				}, []() {}, "Whisper msg");
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& name) {
					if (name.empty()) return;
					UI::get().emplace<UISoftKey>([name](const std::string& msg) {
						if (msg.empty()) return;
						WhisperPacket(name, msg).dispatch();
					}, []() {}, "Whisper msg");
				}, []() {}, "Whisper to");
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
				UI::get().emplace<UISoftKey>([n](const std::string& msg) {
					if (msg.empty()) return;
					NoteSendPacket(n, msg).dispatch();
					LOG(LOG_DEBUG, "UICharInfo: NoteSend to=" << n << " len=" << msg.size());
				}, []() {}, Locale::text("charinfo.prompt_note"));
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& to) {
					if (to.empty()) return;
					UI::get().emplace<UISoftKey>([to](const std::string& msg) {
						if (msg.empty()) return;
						NoteSendPacket(to, msg).dispatch();
						LOG(LOG_DEBUG, "UICharInfo: NoteSend to=" << to << " len=" << msg.size());
					}, []() {}, Locale::text("charinfo.prompt_note"));
				}, []() {}, Locale::text("charinfo.prompt_note_to"));
			}
		}
		else if (keycode == 'Y' || keycode == 'y')
		{
			button_pressed(Buttons::BtFamily);
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
			disable(Buttons::BtParty);
			disable(Buttons::BtPopDown);
			disable(Buttons::BtPopUp);
			disable(Buttons::BtFriend);
		}

		Job character_job = Job(job_id);

		name.change_text(target_character->get_name());
		job.change_text(character_job.get_name());
		level.change_text(std::to_string(lv));
		fame.change_text(std::to_string(f));

		if (g == "")
			guild.change_text("-");
		else
			guild.change_text(g);

		alliance.change_text(a);

		if (auto it = buttons.find(Buttons::BtPet); it != buttons.end() && it->second)
			it->second->set_state(
				target_character->has_pet() ? Button::State::NORMAL : Button::State::DISABLED);

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
}