//////////////////////////////////////////////////////////////////////////////////
//	VegaSpell — drag equip + scroll → USE_CASH_ITEM(561)
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"
#include "../../Data/ItemData.h"

#include <string>

namespace ms
{
	class UIVegaSpell : public UIDragElement<PosVEGASPELL>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::VEGASPELL;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIVegaSpell();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		bool send_icon(const Icon& icon, Point<int16_t> cursor_position) override;
		UIElement::Type get_type() const override;

		void set_result(uint8_t op);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t { BtStart, BtOK, BtCancel };

		bool has_assets;
		int16_t equip_slot;
		int32_t equip_itemid;
		int16_t scroll_slot;
		int32_t scroll_itemid;
		int16_t cash_slot;
		int32_t cash_itemid;
		Texture backgrnd;
		Texture equip_icon;
		Texture scroll_icon;
		Text status;
	};
}
