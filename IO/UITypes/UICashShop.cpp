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
#include "UICashShop.h"

#include "UICashGachapon.h"
#include "UICharInfo.h"
#include "UIItemInventory.h"
#include "UIMiscShells.h"
#include "UIRemoteGachapon.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../Window.h"

#include "../Components/MapleButton.h"

#include "../../Gameplay/Stage.h"

#include "../../Net/Packets/GameplayPackets.h"
#include "../../Net/Packets/LoginPackets.h"
#include "../../Net/Packets/CashShopPackets.h"
#include "../../Net/Packets/MiscOutPackets.h"
#include "../../Configuration.h"
#include "../../Util/Misc.h"

#include "../UIStateCashShop.h"

#include <SDL3/SDL.h>

#include <array>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UICashShop::UICashShop() : preview_index(0), menu_index(1), promotion_index(0), mvp_grade(1), mvp_exp(0.07f), list_offset(0)
	{
		nl::node CashShop = nl::nx::UI["CashShop.img"];
		nl::node Base = CashShop["Base"];
		nl::node backgrnd = Base["backgrnd"];
		nl::node BestNew = Base["BestNew"];
		nl::node Preview = Base["Preview"];
		nl::node CSTab = CashShop["CSTab"];
		nl::node CSGLChargeNX = CSTab["CSGLChargeNX"];
		nl::node CSStatus = CashShop["CSStatus"];
		nl::node CSPromotionBanner = CashShop["CSPromotionBanner"];
		nl::node CSMVPBanner = CashShop["CSMVPBanner"];
		nl::node CSItemSearch = CashShop["CSItemSearch"];
		nl::node CSChar = CashShop["CSChar"];
		nl::node CSList = CashShop["CSList"];
		nl::node CSEffect = CashShop["CSEffect"];

		sprites.emplace_back(backgrnd);
		sprites.emplace_back(BestNew, Point<int16_t>(139, 346));

		BestNew_dim = Texture(BestNew).get_dimensions();

		for (size_t i = 0; i < 3; i++)
			preview_sprites[i] = Preview[i];

		for (size_t i = 0; i < 3; i++)
			buttons[Buttons::BtPreview1 + i] = std::make_unique<TwoSpriteButton>(Base["Tab"]["Disable"][i], Base["Tab"]["Enable"][i], Point<int16_t>(957 + (i * 17), 46));

		buttons[Buttons::BtPreview1]->set_state(Button::State::PRESSED);

		buttons[Buttons::BtExit] = std::make_unique<MapleButton>(CSTab["BtExit"], Point<int16_t>(5, 728));
		buttons[Buttons::BtChargeNX] = std::make_unique<MapleButton>(CSGLChargeNX["BtChargeNX"], Point<int16_t>(5, 554));
		buttons[Buttons::BtChargeRefresh] = std::make_unique<MapleButton>(CSGLChargeNX["BtChargeRefresh"], Point<int16_t>(92, 554));

		for (size_t i = 0; i < 9; i++)
			menu_tabs[i] = CSTab["Tab"][i];

		buttons[Buttons::BtChargeRefresh] = std::make_unique<MapleButton>(CSGLChargeNX["BtChargeRefresh"], Point<int16_t>(92, 554));
		buttons[Buttons::BtWish] = std::make_unique<MapleButton>(CSStatus["BtWish"], Point<int16_t>(226, 6));
		buttons[Buttons::BtMileage] = std::make_unique<MapleButton>(CSStatus["BtMileage"], Point<int16_t>(869, 4));
		buttons[Buttons::BtHelp] = std::make_unique<MapleButton>(CSStatus["BtHelp"], Point<int16_t>(997, 4));
		buttons[Buttons::BtCoupon] = std::make_unique<MapleButton>(CSStatus["BtCoupon"], Point<int16_t>(950, 4));

		Charset tab;

		job_label = Text(Text::Font::A11B, Text::Alignment::LEFT, Color::Name::SUPERNOVA, "Illium");
		name_label = Text(Text::Font::A11B, Text::Alignment::LEFT, Color::Name::WHITE, "ShomeiZekkou");
		cs_meta_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW, "");
		refresh_cs_meta();

		promotion_pos = Point<int16_t>(138, 40);
		sprites.emplace_back(CSPromotionBanner["shadow"], promotion_pos);

		promotion_sprites.emplace_back(CSPromotionBanner["basic"]);

		buttons[Buttons::BtNext] = std::make_unique<MapleButton>(CSPromotionBanner["BtNext"], promotion_pos);
		buttons[Buttons::BtPrev] = std::make_unique<MapleButton>(CSPromotionBanner["BtPrev"], promotion_pos);

		for (size_t i = 0; i < 7; i++)
			mvp_sprites[i] = CSMVPBanner["grade"][i];

		mvp_pos = Point<int16_t>(63, 681);
		buttons[Buttons::BtDetailPackage] = std::make_unique<MapleButton>(CSMVPBanner["BtDetailPackage"], mvp_pos);
		buttons[Buttons::BtNonGrade] = std::make_unique<MapleButton>(CSMVPBanner["BtNonGrade"], mvp_pos);

		buttons[Buttons::BtDetailPackage]->set_active(mvp_grade);
		buttons[Buttons::BtNonGrade]->set_active(!mvp_grade);

		mvp_gauge = Gauge(
			Gauge::Type::CASHSHOP,
			CSMVPBanner["gage"][0],
			CSMVPBanner["gage"][2],
			CSMVPBanner["gage"][1],
			84,
			0.0f
		);

		Point<int16_t> search_pos = Point<int16_t>(0, 36);
		sprites.emplace_back(CSItemSearch["backgrnd"], search_pos);
		sprites.emplace_back(CSItemSearch["search"], search_pos + Point<int16_t>(35, 8));

		buttons[Buttons::BtBuyAvatar] = std::make_unique<MapleButton>(CSChar["BtBuyAvatar"], Point<int16_t>(642, 305));
		buttons[Buttons::BtDefaultAvatar] = std::make_unique<MapleButton>(CSChar["BtDefaultAvatar"], Point<int16_t>(716, 305));
		buttons[Buttons::BtInventory] = std::make_unique<MapleButton>(CSChar["BtInventory"], Point<int16_t>(938, 305));
		buttons[Buttons::BtSaveAvatar] = std::make_unique<MapleButton>(CSChar["BtSaveAvatar"], Point<int16_t>(864, 305));
		buttons[Buttons::BtTakeoffAvatar] = std::make_unique<MapleButton>(CSChar["BtTakeoffAvatar"], Point<int16_t>(790, 305));

		charge_charset = Charset(CSGLChargeNX["Number"], Charset::Alignment::RIGHT);

		item_base = CSList["Base"];
		item_line = Base["line"];
		item_none = Base["noItem"];

		for (nl::node item_label : CSEffect)
			item_labels.emplace_back(item_label);

		// 商品列表来自 SetCashShopHandler 解析的主商品目录（UIStateCashShop::cashshop_commodities）。
		// 若服务端未下发或为空，则保持空列表（绘制 "noItem" 占位）。
		const std::vector<CashCommodity>& commodities = UIStateCashShop::cashshop_commodities;
		for (const CashCommodity& c : commodities)
		{
			items.emplace_back(c.itemid, c.sn, Item::Label::NONE, 0, static_cast<uint16_t>(c.count));
		}

		for (size_t i = 0; i < MAX_ITEMS; i++)
		{
			div_t div = std::div(i, 7);

			buttons[Buttons::BtBuy + i] = std::make_unique<MapleButton>(CSList["BtBuy"], Point<int16_t>(146, 523) + Point<int16_t>(124 * div.rem, 205 * div.quot));

			item_name[i] = Text(Text::Font::A11B, Text::Alignment::CENTER, Color::Name::MINESHAFT);
			item_price[i] = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::GRAY);
			item_discount[i] = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::SILVERCHALICE);
			item_percent[i] = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::TORCHRED);
		}

		Point<int16_t> slider_pos = Point<int16_t>(1007, 372);

		list_slider = Slider(
			Slider::Type::THIN_MINESHAFT,
			Range<int16_t>(slider_pos.y(), slider_pos.y() + 381),
			slider_pos.x(),
			2,
			7,
			[&](bool upwards)
			{
				int16_t shift = upwards ? -7 : 7;
				bool above = list_offset >= 0;
				bool below = list_offset + shift < items.size();

				if (above && below)
				{
					list_offset += shift;

					update_items();
				}
			}
		);

		update_items();

		dimension = Texture(backgrnd).get_dimensions();

		// 进商城后主动拉一次余额（进店时服务端也会推 showCash；刷新兜底）
		CheckCashPacket().dispatch();
	}

	void UICashShop::draw(float inter) const
	{
		preview_sprites[preview_index].draw(position + Point<int16_t>(644, 65), inter);

		UIElement::draw_sprites(inter);

		menu_tabs[menu_index].draw(position + Point<int16_t>(0, 63), inter);

		Point<int16_t> label_pos = position + Point<int16_t>(4, 3);
		job_label.draw(label_pos);

		size_t length = job_label.width();
		name_label.draw(label_pos + Point<int16_t>(length + 10, 0));
		if (!cs_meta_label.empty())
			cs_meta_label.draw(label_pos + Point<int16_t>(0, 16));

		promotion_sprites[promotion_index].draw(position + promotion_pos, inter);

		mvp_sprites[mvp_grade].draw(position + mvp_pos, inter);
		mvp_gauge.draw(position + mvp_pos);

		Point<int16_t> charge_pos = position + Point<int16_t>(107, 388);

		// s9 showCash：NX_CREDIT / MAPLE_POINT / NX_PREPAID；未收到包时回退 MaplePointsLocal
		int32_t nx = 0;
		int32_t maple = 0;
		int32_t prepaid = 0;
		if (UIStateCashShop::cash_balances_ready)
		{
			nx = UIStateCashShop::cash_nx_credit;
			maple = UIStateCashShop::cash_maple_point;
			prepaid = UIStateCashShop::cash_nx_prepaid;
		}
		else
		{
			maple = static_cast<int32_t>(Setting<MaplePointsLocal>::get().load());
		}
		charge_charset.draw(std::to_string(nx), charge_pos + Point<int16_t>(0, 30 * 1));
		charge_charset.draw(std::to_string(maple), charge_pos + Point<int16_t>(0, 30 * 2));
		charge_charset.draw(std::to_string(prepaid), charge_pos + Point<int16_t>(0, 30 * 3));
		charge_charset.draw(std::to_string(maple), charge_pos + Point<int16_t>(0, 30 * 4));
		charge_charset.draw(std::to_string(nx + prepaid), charge_pos + Point<int16_t>(0, 30 * 5));

		if (items.size() > 0)
			item_line.draw(position + Point<int16_t>(139, 566), inter);
		else
			item_none.draw(position + Point<int16_t>(137, 372) + Point<int16_t>(BestNew_dim.x() / 2, list_slider.getvertical().length() / 2) - item_none.get_dimensions() / 2, inter);

		for (size_t i = 0; i < MAX_ITEMS; i++)
		{
			int16_t index = i + list_offset;

			if (index < items.size())
			{
				div_t div = std::div(i, 7);
				Item item = items[index];

				item_base.draw(position + Point<int16_t>(137, 372) + Point<int16_t>(124 * div.rem, 205 * div.quot), inter);
				item.draw(DrawArgument(position + Point<int16_t>(164, 473) + Point<int16_t>(124 * div.rem, 205 * div.quot), 2.0f, 2.0f));

				if (item.label != Item::Label::NONE)
					item_labels[item.label + 1].draw(position + Point<int16_t>(152, 372) + Point<int16_t>(124 * div.rem, 205 * div.quot), inter);

				item_name[i].draw(position + Point<int16_t>(192, 480) + Point<int16_t>(124 * div.rem, 205 * div.quot));

				if (item_discount[i].get_text() == "")
				{
					item_price[i].draw(position + Point<int16_t>(195, 499) + Point<int16_t>(124 * div.rem, 205 * div.quot));
				}
				else
				{
					item_price[i].draw(position + Point<int16_t>(196, 506) + Point<int16_t>(124 * div.rem, 205 * div.quot));

					item_discount[i].draw(position + Point<int16_t>(185, 495) + Point<int16_t>(124 * div.rem, 205 * div.quot));
					item_percent[i].draw(position + Point<int16_t>(198 + (item_discount[i].width() / 2), 495) + Point<int16_t>(124 * div.rem, 205 * div.quot));
				}
			}
		}

		list_slider.draw(position);

		UIElement::draw_buttons(inter);
	}

	void UICashShop::update()
	{
		UIElement::update();

		mvp_gauge.update(mvp_exp);
	}

	Button::State UICashShop::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
		case Buttons::BtPreview1:
		case Buttons::BtPreview2:
		case Buttons::BtPreview3:
			buttons[preview_index]->set_state(Button::State::NORMAL);

			preview_index = buttonid;
			return Button::State::PRESSED;
		case Buttons::BtExit:
		{
			uint16_t width = Setting<Width>::get().load();
			uint16_t height = Setting<Height>::get().load();

			Constants::Constants::get().set_viewwidth(width);
			Constants::Constants::get().set_viewheight(height);

			float fadestep = 0.025f;

			Window::get().fadeout(
				fadestep,
				[]()
				{
					GraphicsGL::get().clear();
					ChangeMapPacket().dispatch();
				}
			);

			GraphicsGL::get().lock();
			Stage::get().clear();
			Timer::get().start();

			return Button::State::NORMAL;
		}
		case Buttons::BtNext:
		{
			size_t size = promotion_sprites.size() - 1;

			promotion_index++;

			if (promotion_index > size)
				promotion_index = 0;

			return Button::State::NORMAL;
		}
		case Buttons::BtPrev:
		{
			size_t size = promotion_sprites.size() - 1;

			promotion_index--;

			if (promotion_index < 0)
				promotion_index = size;

			return Button::State::NORMAL;
		}
		case Buttons::BtChargeNX:
		{
			std::string url = Configuration::get().get_chargenx();

			SDL_OpenURL(url.c_str());

			return Button::State::NORMAL;
		}
		case Buttons::BtCoupon:
		{
			// SoftKey：空 → UICoupon；有 CharInfo 名则 bday,sn[,msg]；否则全字段
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				std::string name = pre;
				UI::get().emplace<UISoftKey>([name](const std::string& tip) {
					if (tip.empty()) {
						auto coupon = UI::get().get_element<UICoupon>();
						if (!coupon) coupon = UI::get().emplace<UICoupon>();
						else if (!coupon->is_active()) coupon->makeactive();
						return;
					}
					size_t c1 = tip.find(',');
					size_t c2 = (c1 == std::string::npos) ? std::string::npos : tip.find(',', c1 + 1);
					if (c1 == std::string::npos) return;
					try {
						int32_t birthday = std::stoi(tip.substr(0, c1));
						int32_t sn = 0;
						std::string msg;
						if (c2 == std::string::npos) {
							sn = std::stoi(tip.substr(c1 + 1));
						} else {
							sn = std::stoi(tip.substr(c1 + 1, c2 - c1 - 1));
							msg = tip.substr(c2 + 1);
						}
						if (sn > 0) {
							CashShopGiftPacket(birthday, sn, name, msg).dispatch();
							LOG(LOG_DEBUG, "UICashShop: BtCoupon Gift sn=" << sn << " to=" << name);
						}
					} catch (...) {}
				}, []() {}, "bday,sn[,msg]|empty=Coupon");
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& tip) {
					if (tip.empty()) {
						auto coupon = UI::get().get_element<UICoupon>();
						if (!coupon) coupon = UI::get().emplace<UICoupon>();
						else if (!coupon->is_active()) coupon->makeactive();
						return;
					}
					size_t c1 = tip.find(',');
					size_t c2 = (c1 == std::string::npos) ? std::string::npos : tip.find(',', c1 + 1);
					size_t c3 = (c2 == std::string::npos) ? std::string::npos : tip.find(',', c2 + 1);
					if (c1 == std::string::npos || c2 == std::string::npos || c3 == std::string::npos) return;
					try {
						int32_t birthday = std::stoi(tip.substr(0, c1));
						int32_t sn = std::stoi(tip.substr(c1 + 1, c2 - c1 - 1));
						std::string name = tip.substr(c2 + 1, c3 - c2 - 1);
						std::string msg = tip.substr(c3 + 1);
						if (sn > 0 && !name.empty()) {
							CashShopGiftPacket(birthday, sn, name, msg).dispatch();
							LOG(LOG_DEBUG, "UICashShop: BtCoupon Gift sn=" << sn << " to=" << name);
						}
					} catch (...) {}
				}, []() {}, "Gift bday,sn,name,msg|empty=Coupon");
			}
			return Button::State::NORMAL;
		}
		case Buttons::BtHelp:
		{
			// 全客户端「帮助/问号」统一走 ENTER_MTS，落点由服务端 use_mts 决定。
			EnterMtsPacket().dispatch();
			LOG(LOG_INFO, "UICashShop: Help -> ENTER_MTS");
			return Button::State::NORMAL;
		}
		case Buttons::BtInventory:
		{
			// SoftKey：空 → 背包；t,cashId → Take；p,cashId[,inv] → Put
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				if (tip.empty()) {
					auto inv = UI::get().get_element<UIItemInventory>();
					if (!inv)
						inv = UI::get().emplace<UIItemInventory>(Stage::get().get_player().get_inventory());
					else if (!inv->is_active())
						inv->makeactive();
					return;
				}
				std::string t = tip;
				for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
				try {
					if (t.size() >= 2 && t[0] == 't' && (t[1] == ',' || t[1] == ':')) {
						int32_t cashId = std::stoi(tip.substr(2));
						if (cashId > 0) {
							CashShopTakePacket(cashId).dispatch();
							LOG(LOG_DEBUG, "UICashShop: BtInventory Take cashId=" << cashId);
						}
						return;
					}
					if (t.size() >= 2 && t[0] == 'p' && (t[1] == ',' || t[1] == ':')) {
						std::string rest = tip.substr(2);
						size_t c = rest.find(',');
						int32_t cashId = 0;
						int8_t invt = 1;
						if (c == std::string::npos)
							cashId = std::stoi(rest);
						else {
							cashId = std::stoi(rest.substr(0, c));
							invt = static_cast<int8_t>(std::stoi(rest.substr(c + 1)));
						}
						if (cashId > 0) {
							CashShopPutPacket(cashId, invt).dispatch();
							LOG(LOG_DEBUG, "UICashShop: BtInventory Put cashId=" << cashId);
						}
						return;
					}
				} catch (...) {}
			}, []() {}, "t,cash|p,cash[,inv]|empty=Inv");
			return Button::State::NORMAL;
		}
		case Buttons::BtChargeRefresh:
		{
			list_offset = 0;
			update_items();
			// s9 TouchingCashShopHandler：CHECK_CASH → showCash(QUERY_CASH_RESULT)
			CheckCashPacket().dispatch();
			LOG(LOG_DEBUG, "UICashShop: BtChargeRefresh CHECK_CASH items=" << items.size());
			return Button::State::NORMAL;
		}
		case Buttons::BtWish:
		{
			// SoftKey：单个 SN 或逗号分隔最多 10 个 → CashOperation 0x05
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				std::array<int32_t, 10> sns{};
				size_t n = 0;
				size_t start = 0;
				while (n < 10 && start <= tip.size()) {
					size_t c = tip.find(',', start);
					std::string part = (c == std::string::npos)
						? tip.substr(start) : tip.substr(start, c - start);
					try {
						int32_t sn = std::stoi(part);
						if (sn > 0)
							sns[n++] = sn;
					} catch (...) {}
					if (c == std::string::npos) break;
					start = c + 1;
				}
				if (n == 0) return;
				CashShopWishPacket(sns).dispatch();
				LOG(LOG_DEBUG, "UICashShop: Wish sns=" << n);
			}, []() {}, "Wish SN[,SN…]");
			return Button::State::NORMAL;
		}
		case Buttons::BtMileage:
		{
			// Mileage = buy 时 useNX=2（MAPLE_POINT），无独立 action
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				try {
					int32_t sn = std::stoi(tip);
					if (sn > 0) {
						CashShopBuyPacket(2, sn).dispatch();
						LOG(LOG_DEBUG, "UICashShop: Mileage buy sn=" << sn);
					}
				} catch (...) {}
			}, []() {}, "Mileage SN");
			return Button::State::NORMAL;
		}
		case Buttons::BtDetailPackage:
		{
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					size_t comma = s.find(',');
					int32_t useNX = 1;
					int32_t sn = 0;
					if (comma == std::string::npos)
						sn = std::stoi(s);
					else {
						useNX = std::stoi(s.substr(0, comma));
						sn = std::stoi(s.substr(comma + 1));
					}
					if (sn > 0) {
						CashShopPackagePacket(useNX, sn).dispatch();
						LOG(LOG_DEBUG, "UICashShop: BtDetailPackage sn=" << sn);
					}
				} catch (...) {}
			}, []() {}, "Pkg useNX,sn");
			return Button::State::NORMAL;
		}
		case Buttons::BtNonGrade:
		{
			mvp_grade = !mvp_grade;
			if (auto it = buttons.find(Buttons::BtDetailPackage); it != buttons.end() && it->second)
				it->second->set_active(mvp_grade);
			if (auto it = buttons.find(Buttons::BtNonGrade); it != buttons.end() && it->second)
				it->second->set_active(!mvp_grade);
			return Button::State::NORMAL;
		}
		case Buttons::BtBuyAvatar:
		{
			// try-on 无独立 opcode；购买走现有 0x03 SoftKey
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					size_t comma = s.find(',');
					int32_t useNX = 1;
					int32_t sn = 0;
					if (comma == std::string::npos)
						sn = std::stoi(s);
					else {
						useNX = std::stoi(s.substr(0, comma));
						sn = std::stoi(s.substr(comma + 1));
					}
					if (sn > 0) {
						CashShopBuyPacket(useNX, sn).dispatch();
						LOG(LOG_DEBUG, "UICashShop: BtBuyAvatar useNX=" << useNX << " sn=" << sn);
					}
				} catch (...) {}
			}, []() {}, "Buy useNX,sn");
			return Button::State::NORMAL;
		}
		case Buttons::BtDefaultAvatar:
		{
			if (auto it = buttons.find(preview_index); it != buttons.end() && it->second)
				it->second->set_state(Button::State::NORMAL);
			preview_index = Buttons::BtPreview1;
			if (auto it = buttons.find(Buttons::BtPreview1); it != buttons.end() && it->second)
				it->second->set_state(Button::State::PRESSED);
			LOG(LOG_DEBUG, "UICashShop: DefaultAvatar preview=0");
			return Button::State::NORMAL;
		}
		case Buttons::BtSaveAvatar:
		case Buttons::BtTakeoffAvatar:
		{
			// Save/Takeoff 无 s9 CashOperation action — SoftKey 切本地预览 0..2
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					int idx = std::stoi(s);
					if (idx < 0 || idx > 2) return;
					if (auto it = buttons.find(preview_index); it != buttons.end() && it->second)
						it->second->set_state(Button::State::NORMAL);
					preview_index = static_cast<uint8_t>(idx);
					if (auto it = buttons.find(preview_index); it != buttons.end() && it->second)
						it->second->set_state(Button::State::PRESSED);
					LOG(LOG_DEBUG, "UICashShop: Avatar preview SoftKey idx=" << idx);
				} catch (...) {}
			}, []() {}, "Preview 0-2");
			return Button::State::NORMAL;
		}
		default:
			break;
		}

		if (buttonid >= Buttons::BtBuy)
		{
			int16_t index = buttonid - Buttons::BtBuy + list_offset;

			if (index < 0 || index >= static_cast<int16_t>(items.size()))
				return Button::State::NORMAL;

			Item item = items[index];
			int32_t sn = item.get_sn();

			// SoftKey 确认：空/仅数字 useNX → 默认 useNX=1；useNX,sn 可改 SN
			UI::get().emplace<UISoftKey>([sn](const std::string& s) {
				try {
					int32_t useNX = 1;
					int32_t buy_sn = sn;
					if (!s.empty()) {
						size_t comma = s.find(',');
						if (comma == std::string::npos)
							useNX = std::stoi(s);
						else {
							useNX = std::stoi(s.substr(0, comma));
							buy_sn = std::stoi(s.substr(comma + 1));
						}
					}
					if (buy_sn > 0) {
						CashShopBuyPacket(useNX, buy_sn).dispatch();
						LOG(LOG_DEBUG, "UICashShop: BtBuy SoftKey useNX=" << useNX << " sn=" << buy_sn);
					}
				} catch (...) {}
			}, []() {}, "Buy useNX[,sn]");

			return Button::State::NORMAL;
		}

		return Button::State::DISABLED;
	}

	Cursor::State UICashShop::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Point<int16_t> cursor_relative = cursorpos - position;

		if (list_slider.isenabled())
		{
			Cursor::State state = list_slider.send_cursor(cursor_relative, clicked);

			if (state != Cursor::State::IDLE)
				return state;
		}

		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UICashShop::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (escape) { exit_cashshop(); return; }
		if (keycode == 'B' || keycode == 'b' || keycode == 'S' || keycode == 's')
		{
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					size_t comma = s.find(',');
					int32_t useNX = 1;
					int32_t sn = 0;
					if (comma == std::string::npos)
						sn = std::stoi(s);
					else {
						useNX = std::stoi(s.substr(0, comma));
						sn = std::stoi(s.substr(comma + 1));
					}
					if (sn > 0) {
						CashShopBuyPacket(useNX, sn).dispatch();
						LOG(LOG_DEBUG, "UICashShop: SoftKey buy useNX=" << useNX << " sn=" << sn);
					}
				} catch (...) {}
			}, []() {}, "Buy useNX,sn");
		}
		else if (keycode == 'G' || keycode == 'g')
		{
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				std::string name = pre;
				UI::get().emplace<UISoftKey>([name](const std::string& tip) {
					if (tip.empty()) return;
					size_t c1 = tip.find(',');
					size_t c2 = (c1 == std::string::npos) ? std::string::npos : tip.find(',', c1 + 1);
					if (c1 == std::string::npos) return;
					try {
						int32_t birthday = std::stoi(tip.substr(0, c1));
						int32_t sn = 0;
						std::string msg;
						if (c2 == std::string::npos) {
							sn = std::stoi(tip.substr(c1 + 1));
						} else {
							sn = std::stoi(tip.substr(c1 + 1, c2 - c1 - 1));
							msg = tip.substr(c2 + 1);
						}
						if (sn > 0) {
							CashShopGiftPacket(birthday, sn, name, msg).dispatch();
							LOG(LOG_DEBUG, "UICashShop: Gift sn=" << sn << " to=" << name);
						}
					} catch (...) {}
				}, []() {}, "bday,sn[,msg]");
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& tip) {
					if (tip.empty()) return;
					size_t c1 = tip.find(',');
					size_t c2 = (c1 == std::string::npos) ? std::string::npos : tip.find(',', c1 + 1);
					size_t c3 = (c2 == std::string::npos) ? std::string::npos : tip.find(',', c2 + 1);
					if (c1 == std::string::npos || c2 == std::string::npos || c3 == std::string::npos) return;
					try {
						int32_t birthday = std::stoi(tip.substr(0, c1));
						int32_t sn = std::stoi(tip.substr(c1 + 1, c2 - c1 - 1));
						std::string name = tip.substr(c2 + 1, c3 - c2 - 1);
						std::string msg = tip.substr(c3 + 1);
						if (sn > 0 && !name.empty()) {
							CashShopGiftPacket(birthday, sn, name, msg).dispatch();
							LOG(LOG_DEBUG, "UICashShop: Gift sn=" << sn << " to=" << name);
						}
					} catch (...) {}
				}, []() {}, "bday,sn,name,msg");
			}
		}
		else if (keycode == 'T' || keycode == 't')
		{
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				try {
					int32_t cashId = std::stoi(tip);
					if (cashId > 0) {
						CashShopTakePacket(cashId).dispatch();
						LOG(LOG_DEBUG, "UICashShop: Take cashId=" << cashId);
					}
				} catch (...) {}
			}, []() {}, "Take cashId");
		}
		else if (keycode == 'P' || keycode == 'p')
		{
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				try {
					size_t c = tip.find(',');
					int32_t cashId = 0;
					int8_t inv = 1;
					if (c == std::string::npos)
						cashId = std::stoi(tip);
					else {
						cashId = std::stoi(tip.substr(0, c));
						inv = static_cast<int8_t>(std::stoi(tip.substr(c + 1)));
					}
					if (cashId > 0) {
						CashShopPutPacket(cashId, inv).dispatch();
						LOG(LOG_DEBUG, "UICashShop: Put cashId=" << cashId << " inv=" << (int)inv);
					}
				} catch (...) {}
			}, []() {}, "Put cashId[,inv]");
		}
		else if (keycode == 'W' || keycode == 'w')
		{
			button_pressed(Buttons::BtWish);
		}
		else if (keycode == 'M' || keycode == 'm')
		{
			button_pressed(Buttons::BtMileage);
		}
		else if (keycode == 'K' || keycode == 'k')
		{
			// Package buy 0x1E：useNX,sn 或单 sn
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					size_t comma = s.find(',');
					int32_t useNX = 1;
					int32_t sn = 0;
					if (comma == std::string::npos)
						sn = std::stoi(s);
					else {
						useNX = std::stoi(s.substr(0, comma));
						sn = std::stoi(s.substr(comma + 1));
					}
					if (sn > 0) {
						CashShopPackagePacket(useNX, sn).dispatch();
						LOG(LOG_DEBUG, "UICashShop: Package useNX=" << useNX << " sn=" << sn);
					}
				} catch (...) {}
			}, []() {}, "Pkg useNX,sn");
		}
		else if (keycode == 'D' || keycode == 'd')
		{
			button_pressed(Buttons::BtDetailPackage);
		}
		else if (keycode == 'I' || keycode == 'i')
		{
			// Inv slot：cash,type 或 cash,sn（第三段 sn 用 s 前缀）
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				try {
					size_t c = tip.find(',');
					if (c == std::string::npos) return;
					int32_t cash = std::stoi(tip.substr(0, c));
					std::string rest = tip.substr(c + 1);
					if (!rest.empty() && (rest[0] == 's' || rest[0] == 'S')) {
						int32_t sn = std::stoi(rest.substr(1));
						if (sn > 0) CashShopInvSlotSnPacket(cash, sn).dispatch();
					} else {
						int8_t type = static_cast<int8_t>(std::stoi(rest));
						CashShopInvSlotPacket(cash, type).dispatch();
					}
					LOG(LOG_DEBUG, "UICashShop: InvSlot " << tip);
				} catch (...) {}
			}, []() {}, "cash,type|cash,sSN");
		}
		else if (keycode == 'X' || keycode == 'x')
		{
			// Storage：cash 或 cash,sn
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				try {
					size_t c = tip.find(',');
					if (c == std::string::npos) {
						CashShopStorageSlotPacket(std::stoi(tip)).dispatch();
					} else {
						int32_t cash = std::stoi(tip.substr(0, c));
						int32_t sn = std::stoi(tip.substr(c + 1));
						CashShopStorageSlotPacket(cash, sn).dispatch();
					}
					LOG(LOG_DEBUG, "UICashShop: StorageSlot " << tip);
				} catch (...) {}
			}, []() {}, "cash[,sn]");
		}
		else if (keycode == 'C' || keycode == 'c')
		{
			// Char slot：cash,sn
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				try {
					size_t c = tip.find(',');
					if (c == std::string::npos) return;
					int32_t cash = std::stoi(tip.substr(0, c));
					int32_t sn = std::stoi(tip.substr(c + 1));
					if (sn > 0) {
						CashShopCharSlotPacket(cash, sn).dispatch();
						LOG(LOG_DEBUG, "UICashShop: CharSlot cash=" << cash << " sn=" << sn);
					}
				} catch (...) {}
			}, []() {}, "cash,sn");
		}
		else if (keycode == 'A' || keycode == 'a')
		{
			auto ui = UI::get().get_element<UICashGachapon>();
			if (!ui) ui = UI::get().emplace<UICashGachapon>();
			else if (!ui->is_active()) ui->makeactive();
		}
		else if (keycode == 'R' || keycode == 'r')
		{
			refresh_cs_meta();
			LOG(LOG_DEBUG, "UICashShop: refresh_cs_meta");
		}
		else if (keycode == 'E' || keycode == 'e')
		{
			auto ui = UI::get().get_element<UIRemoteGachapon>();
			if (!ui) ui = UI::get().emplace<UIRemoteGachapon>();
			else if (!ui->is_active()) ui->makeactive();
		}
	}

	UIElement::Type UICashShop::get_type() const
	{
		return TYPE;
	}

	void UICashShop::refresh_cs_meta()
	{
		int wish_n = 0;
		for (int32_t sn : UIStateCashShop::cash_wishlist)
			if (sn != 0)
				++wish_n;
		std::string tip = "CS inv "
			+ std::to_string(UIStateCashShop::cash_inventory.size())
			+ " gift " + std::to_string(UIStateCashShop::cash_gifts.size())
			+ " wish " + std::to_string(wish_n);
		if (UIStateCashShop::cash_storage_slots > 0)
			tip += " stor" + std::to_string(UIStateCashShop::cash_storage_slots);
		cs_meta_label.change_text(tip);
	}

	void UICashShop::exit_cashshop()
	{
		UI& ui = UI::get();
		ui.change_state(UI::State::GAME);

		Stage& stage = Stage::get();
		Player& player = stage.get_player();

		PlayerLoginPacket(player.get_oid()).dispatch();

		int32_t mapid = player.get_stats().get_mapid();
		uint8_t portalid = player.get_stats().get_portal();

		stage.load(mapid, portalid);
		stage.transfer_player();

		ui.enable();
		Timer::get().start();
		GraphicsGL::get().unlock();
	}

	void UICashShop::update_items()
	{
		for (size_t i = 0; i < MAX_ITEMS; i++)
		{
			int16_t index = i + list_offset;
			bool found_item = index < items.size();

			buttons[Buttons::BtBuy + i]->set_active(found_item);

			std::string name = "";
			std::string price_text = "";
			std::string discount_text = "";
			std::string percent_text = "";

			if (found_item)
			{
				Item item = items[index];

				name = item.get_name();

				int32_t price = item.get_price();
				price_text = std::to_string(price);

				if (item.discount_price > 0 && price > 0)
				{
					discount_text = price_text;

					uint32_t discount = item.discount_price;
					price_text = std::to_string(discount);

					float_t percent = (float)discount / price;
					std::string percent_str = std::to_string(percent);
					percent_text = "(" + percent_str.substr(2, 1) + "%)";
				}

				string_format::split_number(price_text);
				string_format::split_number(discount_text);

				price_text += " NX";

				if (discount_text != "")
					discount_text += " NX";

				if (item.count > 0)
					price_text += "(" + std::to_string(item.count) + ")";
			}

			item_name[i].change_text(name);
			item_price[i].change_text(price_text);
			item_discount[i].change_text(discount_text);
			item_percent[i].change_text(percent_text);

			string_format::format_with_ellipsis(item_name[i], 92);
		}
	}
}