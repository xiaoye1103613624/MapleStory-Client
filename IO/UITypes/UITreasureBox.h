//////////////////////////////////////////////////////////////////////////////////
//	TreasureBox shell
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"

namespace ms
{
	class UITreasureBox : public UIDragElement<PosNOTICE>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::TREASUREBOX;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UITreasureBox();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		UIElement::Type get_type() const override;

		void set_opened(bool opened);

	private:
		bool has_assets = false;
		bool opened = false;
		Texture closed_tex;
		Texture opened_tex;
	};
}
