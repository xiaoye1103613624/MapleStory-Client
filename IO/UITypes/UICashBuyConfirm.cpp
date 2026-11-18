#include "UICashBuyConfirm.h"

#include "../UI.h"
#include "../Components/AreaButton.h"
#include "../Components/MapleButton.h"

#include "../../Audio/Audio.h"
#include "../../Constants.h"
#include "../../Data/ItemData.h"
#include "../../Graphics/DrawArgument.h"
#include "../../Util/Locale.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

#include <algorithm>
#include <string>

namespace ms
{
	UICashBuyConfirm::UICashBuyConfirm(
		int32_t item_id,
		int32_t price,
		int32_t buy_sn,
		int32_t nx,
		int32_t maple,
		int32_t prepaid,
		std::function<void(int32_t)> ok,
		std::function<void()> cancel)
		: UIElement({}, {}, true),
		  sn(buy_sn),
		  nx_cash(nx),
		  maple_points(maple),
		  prepaid_cash(prepaid),
		  ok_handler(std::move(ok)),
		  cancel_handler(std::move(cancel))
	{
#ifdef USE_NX
		nl::node notice = nl::nx::UI["CashShop.img"]["CSNotice"]["4"];
		for (int i = 0; i < 5; ++i)
			strip[i] = Texture(notice[std::to_string(i)]);

		nl::node checks = nl::nx::UI["Basic.img"]["CheckBox"];
		if (checks)
		{
			checkbox[0] = Texture(checks["0"]);
			checkbox[1] = Texture(checks["1"]);
		}
#endif

		const ItemData& idata = ItemData::get(item_id);
		if (idata.is_valid())
			icon = idata.get_icon(true);

		name_label = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE,
			idata.is_valid() ? idata.get_name() : ("#" + std::to_string(item_id)));
		price_label = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE,
			Locale::format("cashshop.buy_price", std::to_string(price)));
		warn_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
			Locale::text("cashshop.buy_warn"));

		if (maple_points >= price && maple_points > 0)
			selected = OPT_MAPLE;
		else if (nx_cash > 0 || prepaid_cash > 0)
			selected = OPT_NX;
		else if (maple_points > 0)
			selected = OPT_MAPLE;
		else
			selected = OPT_NX;

		layout_chrome();
		center_on_screen();
		Sound(Sound::Name::DLGNOTICE).play();
	}

	void UICashBuyConfirm::layout_chrome()
	{
		int16_t stacked_h = 0;
		for (int i = 0; i < 5; ++i)
		{
			if (strip[i])
				stacked_h = static_cast<int16_t>(stacked_h + strip[i].height());
		}
		dimension = Point<int16_t>(NATIVE_W, stacked_h > 0 ? stacked_h : NATIVE_H);

#ifdef USE_NX
		nl::node basic = nl::nx::UI["Basic.img"];
		try
		{
			buttons[BT_CANCEL] = std::make_unique<MapleButton>(basic["BtCancel2"]);
			buttons[BT_OK] = std::make_unique<MapleButton>(basic["BtOK2"]);
		}
		catch (...) {}
#endif

		if (buttons.count(BT_OK) && buttons[BT_OK])
		{
			const Rectangle<int16_t> okb = buttons[BT_OK]->bounds(Point<int16_t>(0, 0));
			const int16_t btn_h = okb.height() > 0 ? okb.height() : 19;
			const int16_t by = static_cast<int16_t>(dimension.y() - BTN_BOTTOM - btn_h);
			const int16_t cancel_w = (buttons.count(BT_CANCEL) && buttons[BT_CANCEL])
				? buttons[BT_CANCEL]->width() : 40;
			const int16_t ok_w = buttons[BT_OK]->width();
			const int16_t cancel_x = static_cast<int16_t>(dimension.x() - BTN_RIGHT - cancel_w);
			const int16_t ok_x = static_cast<int16_t>(cancel_x - BTN_GAP - ok_w);
			buttons[BT_OK]->set_position(Point<int16_t>(ok_x, by));
			if (buttons.count(BT_CANCEL) && buttons[BT_CANCEL])
				buttons[BT_CANCEL]->set_position(Point<int16_t>(cancel_x, by));
		}

		for (int8_t i = 0; i < OPT_COUNT; ++i)
		{
			const uint16_t id = static_cast<uint16_t>(BT_OPT_MAPLE + i);
			buttons[id] = std::make_unique<AreaButton>(
				Point<int16_t>(static_cast<int16_t>(CHECK_X - 4),
					static_cast<int16_t>(CHECK_Y[i] - 4)),
				Point<int16_t>(CHECK_HIT_W, CHECK_HIT_H));
		}
	}

	bool UICashBuyConfirm::opt_available(PayOpt opt) const
	{
		switch (opt)
		{
		case OPT_MAPLE:
			return maple_points > 0;
		case OPT_CREDIT:
			return false;
		case OPT_NX:
			return nx_cash > 0 || prepaid_cash > 0;
		default:
			return false;
		}
	}

	void UICashBuyConfirm::select_opt(PayOpt opt)
	{
		if (!opt_available(opt))
			return;
		selected = opt;
	}

	int32_t UICashBuyConfirm::selected_use_nx() const
	{
		if (selected == OPT_MAPLE)
			return MAPLE_POINT;
		if (nx_cash > 0)
			return NX_CREDIT;
		return NX_PREPAID;
	}

	void UICashBuyConfirm::draw(float) const
	{
		Point<int16_t> at = position;
		for (int i = 0; i < 5; ++i)
		{
			if (!strip[i])
				continue;
			strip[i].draw(at);
			at.shift_y(strip[i].height());
		}

		if (icon)
		{
			const Point<int16_t> dim = icon.get_dimensions();
			const Point<int16_t> origin = icon.get_origin();
			const int16_t src = std::max<int16_t>(1, std::max(dim.x(), dim.y()));
			const float fill = static_cast<float>(ICON_SLOT) / static_cast<float>(src);
			const int16_t scaled_w = static_cast<int16_t>(dim.x() * fill + 0.5f);
			const int16_t scaled_h = static_cast<int16_t>(dim.y() * fill + 0.5f);
			const int16_t ox = static_cast<int16_t>(
				ICON_X + (ICON_SLOT - scaled_w) / 2 + origin.x() * fill + 0.5f);
			const int16_t oy = static_cast<int16_t>(
				ICON_Y + (ICON_SLOT - scaled_h) / 2 + origin.y() * fill + 0.5f);
			icon.draw(DrawArgument(position + Point<int16_t>(ox, oy), fill, fill));
		}

		name_label.draw(position + Point<int16_t>(NAME_X, NAME_Y));
		price_label.draw(position + Point<int16_t>(NAME_X, PRICE_Y));
		warn_label.draw(position + Point<int16_t>(WARN_X, WARN_Y));

		for (int8_t i = 0; i < OPT_COUNT; ++i)
		{
			const bool on = (selected == static_cast<PayOpt>(i))
				&& opt_available(static_cast<PayOpt>(i));
			if (checkbox[on ? 1 : 0])
				checkbox[on ? 1 : 0].draw(
					position + Point<int16_t>(CHECK_X, CHECK_Y[i]));
		}

		UIElement::draw(0.0f);
	}

	void UICashBuyConfirm::update_screen(int16_t, int16_t)
	{
		center_on_screen();
	}

	void UICashBuyConfirm::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed)
			return;
		if (keycode == 13)
			button_pressed(BT_OK);
		else if (escape)
			button_pressed(BT_CANCEL);
	}

	Cursor::State UICashBuyConfirm::send_cursor(bool pressed, Point<int16_t> cursorpos)
	{
		Cursor::State st = UIElement::send_cursor(pressed, cursorpos);
		if (is_in_range(cursorpos))
			return pressed ? Cursor::State::CLICKING : Cursor::State::IDLE;
		return st;
	}

	UIElement::Type UICashBuyConfirm::get_type() const
	{
		return TYPE;
	}

	Button::State UICashBuyConfirm::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
		case BT_OK:
			if (!opt_available(selected))
				return Button::State::NORMAL;
			if (ok_handler)
				ok_handler(selected_use_nx());
			deactivate();
			break;
		case BT_CANCEL:
			if (cancel_handler)
				cancel_handler();
			deactivate();
			break;
		case BT_OPT_MAPLE:
			select_opt(OPT_MAPLE);
			break;
		case BT_OPT_CREDIT:
			select_opt(OPT_CREDIT);
			break;
		case BT_OPT_NX:
			select_opt(OPT_NX);
			break;
		default:
			break;
		}
		return Button::State::NORMAL;
	}
}
