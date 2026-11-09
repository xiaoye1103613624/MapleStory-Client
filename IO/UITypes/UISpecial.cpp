#include "UISpecial.h"

#include "../../Net/Packets/SpecialPackets.h"
#include "../Components/MapleButton.h"
#include "../../Configuration.h"
#include "../../Constants.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UISpecial::UISpecial() : hasbackgrnd(false)
	{
		dimension = Point<int16_t>(300, 360);
		position = Point<int16_t>(
			Constants::Constants::get().get_viewwidth() / 2 - dimension.x() / 2,
			Constants::Constants::get().get_viewheight() / 2 - dimension.y() / 2);

		title = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::YELLOW, "福利中心", dimension.x());
		const char* names[5] = { "每日签到", "兑换中心", "排行榜", "转职", "传送" };
		for (int16_t i = 0; i < 5; i++)
			labels[i] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, names[i], 160);

		nl::node bg = nl::nx::UI["UIWindow.img"]["Special"]["backgrnd"];
		if (bg)
		{
			backgrnd = Sprite(bg);
			hasbackgrnd = true;
		}

		nl::node yes = nl::nx::UI["UIWindow.img"]["Notice"]["BtYes"];
		int16_t y = 50;
		for (int16_t i = 0; i < 5; i++)
		{
			buttons[static_cast<Buttons>(i)] = std::make_unique<MapleButton>(yes, Point<int16_t>(40, y));
			y += 50;
		}
		buttons[BT_CLOSE] = std::make_unique<MapleButton>(yes, Point<int16_t>(40, y));
	}

	void UISpecial::draw(float inter) const
	{
		if (hasbackgrnd)
			backgrnd.draw(position, inter);

		UIElement::draw(inter);

		title.draw(position + Point<int16_t>(dimension.x() / 2, 10));
		int16_t y = 54;
		for (int16_t i = 0; i < 5; i++)
		{
			labels[i].draw(position + Point<int16_t>(120, y));
			y += 50;
		}
	}

	UIElement::Type UISpecial::get_type() const
	{
		return TYPE;
	}

	Button::State UISpecial::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case BT_SIGNIN:
				SpecialRequestPacket(0).dispatch();
				break;
			case BT_EXCHANGE:
				SpecialRequestPacket(1).dispatch();
				break;
			case BT_RANK:
				SpecialRequestPacket(2).dispatch();
				break;
			case BT_JOB:
				SpecialRequestPacket(3).dispatch();
				break;
			case BT_WARP:
				SpecialRequestPacket(4).dispatch();
				break;
			case BT_CLOSE:
				deactivate();
				break;
		}
		return Button::State::NORMAL;
	}

	bool UISpecial::is_in_range(Point<int16_t> cursor_position) const
	{
		return cursor_position.x() >= position.x() && cursor_position.x() <= position.x() + dimension.x()
			&& cursor_position.y() >= position.y() && cursor_position.y() <= position.y() + dimension.y();
	}
}
