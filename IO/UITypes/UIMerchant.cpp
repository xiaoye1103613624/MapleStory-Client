////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
////////////////////////////////////////////////////////////////////////////////
#include "UIMerchant.h"
#include "UICharInfo.h"
#include "UIItemInventory.h"
#include "UINotice.h"
#include "UIStatusMessenger.h"

#include "../UI.h"
#include "../Components/MapleButton.h"

#include "../../Net/Packets/PlayerInteractionPackets.h"
#include "../../Util/Locale.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		void sm_tip(Color::Name color, const std::string& msg)
		{
			auto sm = UI::get().get_element<UIStatusMessenger>();
			if (!sm) sm = UI::get().emplace<UIStatusMessenger>();
			if (sm) sm->show_status(color, msg);
		}
	}

	UIMerchant::UIMerchant()
		: owner_label(Text::Font::A11B, Text::Alignment::LEFT, Color::Name::WHITE, "")
		, tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW, "")
	{
		position = Point<int16_t>(200, 40);
		dimension = Point<int16_t>(525, 553);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("PersonalShop");
		if (root && root["backgrnd"])
		{
			background = Texture(root["backgrnd"]);
			Point<int16_t> o = background.get_origin();
			if (o.x() || o.y())
				background.shift(o);
			dimension = background.get_dimensions();
			has_background = true;
		}
		else
		{
			LOG(LOG_WARN, "UIMerchant: PersonalShop/backgrnd missing");
		}

		if (root["select"])
		{
			select_tex = Texture(root["select"]);
			has_select = true;
		}
		if (root["soldout"])
		{
			soldout_tex = Texture(root["soldout"]);
			has_soldout = true;
		}

		// 钮坐标：UIWindow.img 无 LayoutMan；按 525×553 底板 + 70×19 钮几何落位。
		// MapleStoryT 钮语义：Start=0x3E9 Buy=0x3EB Close=0x3EA Exit=0x3EF Info=0x3EC Item=0x3ED。
		struct BtnDef { Buttons id; const char* name; Point<int16_t> pos; };
		const BtnDef defs[] = {
			{ BtClose, "BtClose", Point<int16_t>(440, 8) },
			{ BtInfo,  "BtInfo",  Point<int16_t>(440, 48) },
			{ BtItem,  "BtItem",  Point<int16_t>(440, 72) },
			{ BtBuy,   "BtBuy",   Point<int16_t>(440, 200) },
			{ BtStart, "BtStart", Point<int16_t>(440, 230) },
			{ BtExit,  "BtExit",  Point<int16_t>(440, 520) },
		};
		for (const auto& d : defs)
			try_maple_btn(buttons, d.id, root[d.name], d.pos, "UIMerchant");
#endif
		for (size_t i = 0; i < MAX_SLOTS; i++)
			price_labels[i] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "");
		center_on_screen();
		tip_label.change_text(Locale::text("merchant.shelf_tip"));
	}

	void UIMerchant::draw(float inter) const
	{
		if (has_background)
			background.draw(position);

		owner_label.draw(position + Point<int16_t>(16, 10));
		tip_label.draw(position + Point<int16_t>(16, 32));

		for (size_t i = 0; i < merch_items.size() && i < MAX_SLOTS; i++)
		{
			int16_t row = static_cast<int16_t>(static_cast<int16_t>(i) - scroll);
			if (row < 0 || row >= VISIBLE_ROWS)
				continue;

			Point<int16_t> base = position + Point<int16_t>(ITEM_X, ITEM_Y0 + row * ITEM_ROW);
			const MerchItem& mi = merch_items[i];

			if (selected_slot == static_cast<int8_t>(i) && has_select)
				select_tex.draw(base);
			else if (mi.quantity <= 0 && has_soldout)
				soldout_tex.draw(base);

			ItemData::get(mi.itemid).get_icon(mi.itemid / 10000 == 238).draw(DrawArgument(base + Point<int16_t>(4, 2), 1.0f, 1.0f));
			price_labels[i].draw(base + Point<int16_t>(40, 8));
		}

		UIElement::draw_buttons(inter);
	}

	void UIMerchant::update()
	{
		UIElement::update();
	}

	Button::State UIMerchant::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
		case BtClose:
		case BtExit:
			send_exit();
			return Button::State::NORMAL;
		case BtBuy:
			request_buy();
			return Button::State::NORMAL;
		case BtStart:
			request_open_store();
			return Button::State::NORMAL;
		case BtInfo:
		{
			std::string name;
			if (auto ci = UI::get().get_element<UICharInfo>())
				name = ci->get_target_name();
			if (!name.empty())
				set_owner(name);
			else
				sm_tip(Color::Name::YELLOW, Locale::text("merchant.info_need_char"));
			return Button::State::NORMAL;
		}
		case BtItem:
			request_put_item();
			return Button::State::NORMAL;
		default:
			return Button::State::DISABLED;
		}
	}

	Cursor::State UIMerchant::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State st = UIElement::send_cursor(clicked, cursorpos);
		if (!clicked || !active)
			return st;

		Point<int16_t> rel = cursorpos - position;
		int8_t hit = hit_slot(rel);
		if (hit >= 0)
		{
			selected_slot = hit;
			LOG(LOG_DEBUG, "UIMerchant: select slot=" << static_cast<int>(selected_slot));
		}
		return st;
	}

	void UIMerchant::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape)
		{
			send_exit();
			return;
		}
		if (!pressed)
			return;

		if (keycode == 'X' || keycode == 'x')
		{
			send_exit();
			return;
		}
		// Enter / KP Enter（本端 keycode 与 GLFW 对齐时常为 13 / 257 / 335）
		if (keycode == 13 || keycode == 257 || keycode == 335 || keycode == '\r')
		{
			request_buy();
			return;
		}
	}

	UIElement::Type UIMerchant::get_type() const
	{
		return TYPE;
	}

	void UIMerchant::set_items(const std::vector<MerchItem>& items)
	{
		merch_items = items;
		if (selected_slot >= static_cast<int8_t>(merch_items.size()))
			selected_slot = merch_items.empty() ? -1 : 0;

		for (size_t i = 0; i < MAX_SLOTS; i++)
		{
			if (i < merch_items.size())
			{
				const MerchItem& mi = merch_items[i];
				std::string text = ItemData::get(mi.itemid).get_name()
					+ "  " + std::to_string(mi.price)
					+ " ×" + std::to_string(mi.quantity);
				price_labels[i].change_text(text);
			}
			else
			{
				price_labels[i].change_text("");
			}
		}
		tip_label.change_text(Locale::format("merchant.item_count", std::to_string(merch_items.size())));
	}

	void UIMerchant::set_owner(const std::string& owner)
	{
		owner_label.change_text(Locale::format("merchant.owner_shop", owner));
	}

	int8_t UIMerchant::hit_slot(Point<int16_t> rel) const
	{
		if (rel.x() < ITEM_X || rel.x() > ITEM_X + ITEM_W)
			return -1;
		int16_t row = static_cast<int16_t>((rel.y() - ITEM_Y0) / ITEM_ROW);
		if (row < 0 || row >= VISIBLE_ROWS)
			return -1;
		int16_t idx = static_cast<int16_t>(row + scroll);
		if (idx < 0 || idx >= static_cast<int16_t>(merch_items.size()) || idx >= MAX_SLOTS)
			return -1;
		return static_cast<int8_t>(idx);
	}

	void UIMerchant::send_exit()
	{
		MinigameExitPacket().dispatch();
		deactivate();
	}

	void UIMerchant::request_open_store()
	{
		HiredMerchantOpenPacket().dispatch();
		tip_label.change_text(Locale::text("common.open_store"));
		sm_tip(Color::Name::WHITE, Locale::text("common.open_store"));
		LOG(LOG_DEBUG, "UIMerchant: OPEN_STORE");
	}

	void UIMerchant::request_buy()
	{
		if (selected_slot < 0 || static_cast<size_t>(selected_slot) >= merch_items.size())
		{
			sm_tip(Color::Name::RED, Locale::text("merchant.no_slot"));
			return;
		}
		int8_t slot = selected_slot;
		int16_t maxq = merch_items[static_cast<size_t>(slot)].quantity;
		if (maxq <= 0)
		{
			sm_tip(Color::Name::RED, Locale::text("merchant.sold_out"));
			return;
		}
		if (maxq == 1)
		{
			HiredMerchantBuyPacket(slot, 1).dispatch();
			LOG(LOG_DEBUG, "UIMerchant: BUY slot=" << static_cast<int>(slot) << " qty=1");
			return;
		}
		UI::get().emplace<UIEnterNumber>(
			Locale::text("merchant.buy_qty"),
			[slot](int32_t n) {
				if (n <= 0) return;
				HiredMerchantBuyPacket(slot, static_cast<int16_t>(n)).dispatch();
				LOG(LOG_DEBUG, "UIMerchant: BUY slot=" << static_cast<int>(slot) << " qty=" << n);
			},
			maxq,
			1
		);
	}

	void UIMerchant::request_put_item()
	{
		int16_t inv_slot = 0;
		int8_t inv_type = 0;
		if (auto inv = UI::get().get_element<UIItemInventory>())
		{
			inv_slot = inv->get_selected_slot();
			if (inv_slot > 0)
				inv_type = static_cast<int8_t>(inv->get_tab());
		}
		if (inv_slot <= 0 || inv_type <= 0)
		{
			sm_tip(Color::Name::RED, Locale::text("merchant.put_need_inv"));
			return;
		}

		int8_t target = 1;
		if (selected_slot >= 0)
			target = static_cast<int8_t>(selected_slot + 1);
		else
			target = static_cast<int8_t>(std::min<size_t>(merch_items.size() + 1, MAX_SLOTS));
		if (target < 1) target = 1;

		int8_t iv = inv_type;
		int16_t pos = inv_slot;
		UI::get().emplace<UIEnterNumber>(
			Locale::text("merchant.put_qty"),
			[iv, pos, target](int32_t n) {
				if (n <= 0) return;
				HiredMerchantSetItemPacket(iv, pos, static_cast<int16_t>(n), target).dispatch();
				LOG(LOG_DEBUG, "UIMerchant: SET_ITEMS iv=" << (int)iv << " pos=" << pos
					<< " qty=" << n << " slot=" << (int)target);
			},
			100,
			1
		);
	}
}
