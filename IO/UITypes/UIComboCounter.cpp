//////////////////////////////////////////////////////////////////////////////////
//	Aran combo counter
//////////////////////////////////////////////////////////////////////////////////
#include "UIComboCounter.h"

#include "../../Constants.h"
#include "../../Util/UiAssetResolve.h"

#include <string>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIComboCounter::UIComboCounter()
		: label(Text::Font::A18M, Text::Alignment::CENTER, Color::Name::YELLOW)
	{
		int16_t w = Constants::Constants::get().get_viewwidth();
		int16_t h = Constants::Constants::get().get_viewheight();
		update_screen(w, h);
#ifdef USE_NX
		nl::node root = UiAssetResolve::window("Combo");
		if (!root)
			root = UiAssetResolve::window("Number");
		if (root && root["backgrnd"])
		{
			backgrnd = Texture(root["backgrnd"]);
			Point<int16_t> o = backgrnd.get_origin();
			if (o.x() != 0 || o.y() != 0)
				backgrnd.shift(o);
		}
		// 本客户端的 UI.nx/UIWindow.img 里没有 Combo/Number 节点
		// （tools/nx_inspect.py list UI.nx 1 UIWindow.img 可复核），
		// 因此只降级为纯文字计数，不再打成功日志。
#endif
	}

	void UIComboCounter::set_combo(int32_t count)
	{
		combo = count;
		// 服务端 AranComboHandler.java:41 是「距上次超过 3000ms 且连击>0 就归零」，
		// 且 setCombo 只在计数递增时下发 SHOW_COMBO（Character.java:890-897），
		// 不会主动下发 0。本地用同样的 3000ms 静默期隐藏，避免显示一个
		// 服务端已经作废的连击数。
		hide_ms = count > 0 ? 3000 : 0;
		if (combo > 0)
			label.change_text(std::to_string(combo) + " Combo");
		else
			label.change_text("");
	}

	void UIComboCounter::draw(float) const
	{
		if (combo <= 0 || hide_ms <= 0)
			return;
		if (backgrnd)
			backgrnd.draw(position - Point<int16_t>(60, 12));
		label.draw(position);
	}

	void UIComboCounter::update()
	{
		if (hide_ms > 0) {
			hide_ms -= Constants::TIMESTEP;
			if (hide_ms <= 0) {
				hide_ms = 0;
				combo = 0;
				label.change_text("");
			}
		}
	}

	void UIComboCounter::update_screen(int16_t new_width, int16_t new_height)
	{
		position = Point<int16_t>(
			static_cast<int16_t>(new_width / 2),
			static_cast<int16_t>(new_height / 2 - 80)
		);
		dimension = Point<int16_t>(120, 24);
	}

	UIElement::Type UIComboCounter::get_type() const
	{
		return TYPE;
	}
}
