//////////////////////////////////////////////////////////////////////////////////
//	TreasureBox — 金/银宝箱开箱结果窗
//////////////////////////////////////////////////////////////////////////////////
#include "UITreasureBox.h"

#include "../UI.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UITreasureBox::UITreasureBox()
	{
		position = Point<int16_t>(320, 180);
		dimension = Point<int16_t>(170, 172);
		dragarea = Point<int16_t>(170, 20);
		load_variant(0);
		center_on_screen();
	}

	void UITreasureBox::load_variant(int variant_index)
	{
		has_assets = false;
		closed_tex = Texture();
		opened_tex = Texture();
		variant = variant_index < 0 ? 0 : variant_index;

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("TreasureBox");
		if (!root)
		{
			LOG(LOG_WARN, "UITreasureBox: missing UIWindow.img/TreasureBox");
			return;
		}

		// 0=金宝箱皮肤，1=银宝箱皮肤（与 s9 4280000/4280001 对应）
		nl::node box = root[std::to_string(variant)];
		if (!box)
			box = root["0"] ? root["0"] : root["1"];
		if (!box)
		{
			LOG(LOG_WARN, "UITreasureBox: no variant node");
			return;
		}

		if (box["backgrndClosed"])
			closed_tex = Texture(box["backgrndClosed"]);
		else if (box["backgrnd"])
			closed_tex = Texture(box["backgrnd"]);
		if (box["backgrndOpened"])
			opened_tex = Texture(box["backgrndOpened"]);

		if (closed_tex)
		{
			Point<int16_t> o = closed_tex.get_origin();
			if (o.x() || o.y())
				closed_tex.shift(o);
			dimension = closed_tex.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}
		if (opened_tex)
		{
			Point<int16_t> o = opened_tex.get_origin();
			if (o.x() || o.y())
				opened_tex.shift(o);
		}

		has_assets = static_cast<bool>(closed_tex) || static_cast<bool>(opened_tex);
		LOG(LOG_DEBUG, "UITreasureBox: variant=" << variant
			<< " closed=" << static_cast<bool>(closed_tex)
			<< " opened=" << static_cast<bool>(opened_tex));
#endif
	}

	void UITreasureBox::apply_result(int32_t item_id)
	{
		// s9 UseTreasureChestHandler：4280000 金 / 4280001 银
		const int v = (item_id == 4280001) ? 1 : 0;
		if (!has_assets || variant != v)
			load_variant(v);
		opened = true;
		if (!is_active())
			makeactive();
		LOG(LOG_DEBUG, "UITreasureBox: apply_result item=" << item_id << " variant=" << variant);
	}

	void UITreasureBox::set_opened(bool o)
	{
		opened = o;
	}

	void UITreasureBox::draw(float) const
	{
		if (!has_assets)
			return;
		if (opened && opened_tex)
			opened_tex.draw(position);
		else if (closed_tex)
			closed_tex.draw(position);
	}

	void UITreasureBox::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape)
		{
			deactivate();
			return;
		}
		if (!pressed)
			return;
		// Esc 关窗；O/回车仅调试切开合态（正式开箱靠 apply_result）
		if (keycode == 13 || keycode == 'O' || keycode == 'o')
			opened = !opened;
		else if (keycode == 'C' || keycode == 'c')
			opened = false;
	}

	Cursor::State UITreasureBox::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State st = UIDragElement<PosNOTICE>::send_cursor(clicked, cursorpos);
		if (clicked && active)
		{
			Rectangle<int16_t> hit{ position, position + dimension };
			if (hit.contains(cursorpos))
			{
				opened = !opened;
				return Cursor::State::CANCLICK;
			}
		}
		return st;
	}

	UIElement::Type UITreasureBox::get_type() const
	{
		return TYPE;
	}
}
