#include "UIViewAllChar.h"

#include <nlnx/nx.hpp>
#include <string>

#include "../UIStateLogin.h"
#include "../UI.h"
#include "../../Util/Resource/ImgSource.h"
#include "../Components/MapleButton.h"
#include "../../Net/Login.h"

namespace ms
{
	UIViewAllChar::UIViewAllChar() : UIElement(Point<int16_t>(0, 0), Point<int16_t>(800, 600)), selected(-1)
	{
		nl::node vac = nl::nx::UI["Login.img"]["ViewAllChar"];

		// 背景注册为 sprite，由 draw_sprites 绘制（与 UICharSelect 同范式）。
		sprites.emplace_back(Sprite(vac["backgrnd"], Point<int16_t>(0, 0)));

		buttons[Buttons::BtVAC] = std::make_unique<MapleButton>(vac["BtVAC"]);
		buttons[Buttons::BtEnter] = std::make_unique<MapleButton>(vac["BtEnter"]);

		// 遍历 UIStateLogin::viewall_worlds 构建世界/角色列表视图。
		// 北斗为单世界，VIEW_ALL_CHAR 退化为单世界角色列表（与 CharSelect 等价）。
		const auto& worlds = UIStateLogin::viewall_worlds;

		OutlinedText labelproto(Text::Font::A12M, Text::Alignment::LEFT,
			Color::Name::WHITE, Color::Name::TOBACCOBROWN);

		Point<int16_t> base(30, 90);
		int16_t y = base.y();

		for (const auto& world : worlds)
		{
			EntryView we;
			we.pos = Point<int16_t>(base.x(), y);
			we.label = labelproto;
			we.label.change_text("World " + std::to_string(world.worldid));
			entry_views.push_back(std::move(we));
			y += 24;

			for (const auto& chr : world.chars)
			{
				EntryView ce;
				ce.pos = Point<int16_t>(base.x() + 24, y);
				ce.label = labelproto;
				ce.label.change_text("Lv." + std::to_string(chr.stats.stats[MapleStat::Id::LEVEL])
					+ "  " + chr.stats.name);
				entry_views.push_back(std::move(ce));
				y += 20;
			}

			y += 12;
		}
	}

	void UIViewAllChar::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		for (const auto& e : entry_views)
			e.label.draw(position + e.pos);

		UIElement::draw_buttons(alpha);
	}

	void UIViewAllChar::update()
	{
		UIElement::update();
	}

	Cursor::State UIViewAllChar::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		return UIElement::send_cursor(clicked, cursorpos);
	}

	UIElement::Type UIViewAllChar::get_type() const
	{
		return TYPE;
	}

	Button::State UIViewAllChar::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case BtVAC:
			case BtEnter:
				// 查看全部角色窗口关闭后，下层 CharSelect 仍显示，玩家回到选角流程。
				// （完整「进入所选角色」由既有的 UICharSelect 选角/进游戏负责。）
				deactivate();
				break;
			default:
				break;
		}

		return Button::State::NORMAL;
	}
}
