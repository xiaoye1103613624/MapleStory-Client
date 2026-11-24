//////////////////////////////////////////////////////////////////////////////////
// UIAnvilDialog — 融合外观窗。左槽 skin（被牺牲）/右槽 base（保留），
// 槽位用有符号字节表示（冒险岛装备 wire：负=装备槽）。选好后发 FusionAnvilPacket。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

namespace ms
{
	class UIAnvilDialog : public UIDragElement<PosANVILDIALOG>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::ANVILDIALOG;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIAnvilDialog();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Button::State button_pressed(uint16_t buttonid) override;
		UIElement::Type get_type() const override;

	private:
		enum Buttons : uint16_t
		{
			BtClose = 1,
			BtSkinDec, BtSkinInc,
			BtBaseDec, BtBaseInc,
			BtApply
		};

		static constexpr int16_t WIN_W = 300;
		static constexpr int16_t WIN_H = 130;

		int16_t skin_slot = -5;
		int16_t base_slot = -11;
		Texture backgrnd;   // 复用原生通用窗体底图，避免白字裸叠
		bool has_assets = false;
		mutable Text info;
		void refresh();
	};
}
