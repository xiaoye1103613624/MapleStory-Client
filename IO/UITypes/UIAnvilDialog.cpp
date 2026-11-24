//////////////////////////////////////////////////////////////////////////////////
// UIAnvilDialog.cpp — 见 .h。
//////////////////////////////////////////////////////////////////////////////////
#include "UIAnvilDialog.h"

#include "../UI.h"
#include "../Components/AreaButton.h"
#include "../../Net/Packets/FusionAnvilPackets.h"
#include "../../Util/UiAssetResolve.h"

#include <algorithm>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIAnvilDialog::UIAnvilDialog()
		: info(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "")
	{
		dimension = Point<int16_t>(WIN_W, WIN_H);
		dragarea = Point<int16_t>(WIN_W, 24);
		layout_policy = LayoutPolicy::SavedPosClamped;
#ifdef USE_NX
		nl::node root = nl::nx::UI["UIWindowCN.img"]["CN_50100"];
		if (!UiAssetResolve::usable(root)) root = UiAssetResolve::window("Admin")["Block"];
		if (UiAssetResolve::usable(root["backgrnd"]))
		{
			backgrnd = Texture(root["backgrnd"]);
			has_assets = true;
		}
#endif
		refresh();

		buttons[BtClose] = std::make_unique<AreaButton>(Point<int16_t>(WIN_W - 24, 4), Point<int16_t>(20, 18));
		buttons[BtSkinDec] = std::make_unique<AreaButton>(Point<int16_t>(100, 50), Point<int16_t>(24, 20));
		buttons[BtSkinInc] = std::make_unique<AreaButton>(Point<int16_t>(128, 50), Point<int16_t>(24, 20));
		buttons[BtBaseDec] = std::make_unique<AreaButton>(Point<int16_t>(100, 78), Point<int16_t>(24, 20));
		buttons[BtBaseInc] = std::make_unique<AreaButton>(Point<int16_t>(128, 78), Point<int16_t>(24, 20));
		buttons[BtApply] = std::make_unique<AreaButton>(Point<int16_t>(180, 64), Point<int16_t>(70, 24));
	}

	void UIAnvilDialog::refresh()
	{
		info.change_text(u8"融合外观：skin(被牺牲)槽=" + std::to_string(skin_slot)
			+ u8"  base(保留)槽=" + std::to_string(base_slot)
			+ u8"\n用 ◀ ▶ 调整槽位（装备 wire 负值），需先放入 anvil 道具 5900000");
	}

	void UIAnvilDialog::draw(float inter) const
	{
		UIDragElement<PosANVILDIALOG>::draw(inter);
		if (has_assets)
		{
			const float sx = static_cast<float>(WIN_W) / std::max<int16_t>(1, backgrnd.width());
			const float sy = static_cast<float>(WIN_H) / std::max<int16_t>(1, backgrnd.height());
			backgrnd.draw(DrawArgument(position, sx, sy));
		}
		info.draw(position + Point<int16_t>(10, 26));
	}

	void UIAnvilDialog::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (escape) deactivate();
	}

	Button::State UIAnvilDialog::button_pressed(uint16_t id)
	{
		switch (id)
		{
			case BtClose: deactivate(); break;
			case BtSkinDec: skin_slot -= 1; refresh(); break;
			case BtSkinInc: skin_slot += 1; refresh(); break;
			case BtBaseDec: base_slot -= 1; refresh(); break;
			case BtBaseInc: base_slot += 1; refresh(); break;
			case BtApply:
				FusionAnvilPacket(0, skin_slot, base_slot).dispatch();
				break;
			default: return Button::State::DISABLED;
		}
		return Button::State::NORMAL;
	}

	UIElement::Type UIAnvilDialog::get_type() const { return TYPE; }
}
