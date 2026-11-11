//////////////////////////////////////////////////////////////////////////////////
//	D1 / 分号键「勋章」窗实现（v83 MEDAL，Angel UI_Open:260 sub_840170(-1)）
//
//	资源现状：orange-wz 门禁实测 UIWindow.img / UIWindow2.img 全搜 Medal
//	matches:[]，24 个 .img 无 Medal.img → 按门禁 18 走无资源复用件路径：
//	ColorBox 纯色框/页签/列表/滚动条 + Text 文案 + UiAssetResolve::close_btn()。
//	版式真源 = 正确客户端截图/勋章框.png（255x372）。
//	客户端无勋章数据源（原生截帧本身 0 个），列表忠实为空，不造假数据。
//////////////////////////////////////////////////////////////////////////////////
#include "UIMedal.h"

#include "../Components/MapleButton.h"
#include "../../Graphics/DrawArgument.h"
#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#include "../../Util/Locale.h"
#endif

namespace ms
{
	UIMedal::UIMedal() :
		title(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::WHITE, Locale::text("uimedal.23")),
		count_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("uimedal.24")),
		active_tab_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::DARKRED, Locale::text("uimedal.25")),
		empty_hint(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::DARKGREY, "")
	{
		dimension = Point<int16_t>(FRAME_W, FRAME_H);
		center_on_screen();

		frame_edge = ColorBox(FRAME_W, FRAME_H, Color::Name::EMPEROR, 1.0f);
		frame_back = ColorBox(static_cast<int16_t>(FRAME_W - 4), static_cast<int16_t>(FRAME_H - 4), Color::Name::GALLERY, 1.0f);
		list_back = ColorBox(static_cast<int16_t>(FRAME_W - 24), static_cast<int16_t>(FRAME_H - 74), Color::Name::DUSTYGRAY, 1.0f);
		list_inner = ColorBox(static_cast<int16_t>(FRAME_W - 26), static_cast<int16_t>(FRAME_H - 76), Color::Name::PORCELAIN, 1.0f);
		scroll_track = ColorBox(8, static_cast<int16_t>(FRAME_H - 76), Color::Name::SILVERCHALICE, 1.0f);
		scroll_thumb = ColorBox(8, 40, Color::Name::EMPEROR, 1.0f);

		std::string names[TAB_COUNT] = { Locale::text("uimedal.25"), Locale::text("uimedal.tab.1"), Locale::text("uimedal.tab.2"), Locale::text("uimedal.tab.3"), Locale::text("uimedal.tab.4") };
		tab_plate.reserve(TAB_COUNT);
		tab_label.reserve(TAB_COUNT);

		for (int16_t i = 0; i < TAB_COUNT; ++i)
		{
			tab_plate.emplace_back(TAB_W, TAB_H, Color::Name::DUSTYGRAY, 1.0f);
			tab_label.emplace_back(Text::Font::A11M, Text::Alignment::CENTER,
				Color::Name::BLACK, names[i]);
		}

		select_tab(0);

#ifdef USE_NX
		nl::node close = UiAssetResolve::close_btn();

		if (close)
			buttons[Buttons::BtClose] = std::make_unique<MapleButton>(
				close, Point<int16_t>(static_cast<int16_t>(FRAME_W - 21), 5));
#endif
	}

	void UIMedal::select_tab(int16_t index)
	{
		if (index < 0 || index >= TAB_COUNT)
			return;

		active_tab = index;

		std::string names[TAB_COUNT] = { Locale::text("uimedal.25"), Locale::text("uimedal.tab.1"), Locale::text("uimedal.tab.2"), Locale::text("uimedal.tab.3"), Locale::text("uimedal.tab.4") };
		active_tab_label.change_text(names[index]);

		for (int16_t i = 0; i < static_cast<int16_t>(tab_plate.size()); ++i)
			tab_plate[i].set_color(i == active_tab ? Color::Name::WHITE : Color::Name::DUSTYGRAY);
	}

	void UIMedal::draw(float inter) const
	{
		// D3：本窗自绘全走 ColorBox/Text（不走基类 sprites），淡入必须手动乘进
		// DrawArgument 的 opacity 分量（operator+(float) 乘 color.a()）；
		// 推进与 buttons 淡入同源（UIElement::draw_buttons 每帧 tick）。
		const float fade = fade_alpha();

		frame_edge.draw(DrawArgument(position) + fade);
		frame_back.draw(DrawArgument(position + Point<int16_t>(2, 2)) + fade);
		title.draw(DrawArgument(position + Point<int16_t>(0, 6)) + fade);
		count_label.draw(DrawArgument(position + Point<int16_t>(12, 52)) + fade);

		for (int16_t i = 0; i < static_cast<int16_t>(tab_plate.size()); ++i)
		{
			const int16_t tx = static_cast<int16_t>(TAB_X0 + i * (TAB_W + TAB_GAP));
			tab_plate[i].draw(DrawArgument(position + Point<int16_t>(tx, TAB_Y)) + fade);
			tab_label[i].draw(DrawArgument(position + Point<int16_t>(tx, static_cast<int16_t>(TAB_Y + 2))) + fade);
		}

		list_back.draw(DrawArgument(position + Point<int16_t>(10, 62)) + fade);
		list_inner.draw(DrawArgument(position + Point<int16_t>(11, 63)) + fade);
		scroll_track.draw(DrawArgument(position + Point<int16_t>(static_cast<int16_t>(FRAME_W - 15), 63)) + fade);
		scroll_thumb.draw(DrawArgument(position + Point<int16_t>(static_cast<int16_t>(FRAME_W - 15), 63)) + fade);
		active_tab_label.draw(DrawArgument(position + Point<int16_t>(14, 66)) + fade);
		empty_hint.draw(DrawArgument(position + Point<int16_t>(0, 200)) + fade);

		UIElement::draw_buttons(inter);
	}

	void UIMedal::update()
	{
		UIElement::update();
	}

	Button::State UIMedal::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::BtClose:
				deactivate();
				break;
			default:
				break;
		}

		return Button::State::NORMAL;
	}

	Cursor::State UIMedal::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (clicked)
		{
			const Point<int16_t> rel(
				static_cast<int16_t>(cursorpos.x() - position.x()),
				static_cast<int16_t>(cursorpos.y() - position.y()));

			if (rel.y() >= TAB_Y && rel.y() < static_cast<int16_t>(TAB_Y + TAB_H))
			{
				for (int16_t i = 0; i < TAB_COUNT; ++i)
				{
					const int16_t tx = static_cast<int16_t>(TAB_X0 + i * (TAB_W + TAB_GAP));

					if (rel.x() >= tx && rel.x() < static_cast<int16_t>(tx + TAB_W))
					{
						select_tab(i);
						return Cursor::State::IDLE;
					}
				}
			}
		}

		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UIMedal::send_key(int32_t keycode, bool pressed, bool escape)
	{
		(void)keycode;

		if (pressed && escape && active)
			deactivate();
	}

	UIElement::Type UIMedal::get_type() const
	{
		return TYPE;
	}
}
