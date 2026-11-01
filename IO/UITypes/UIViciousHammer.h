//////////////////////////////////////////////////////////////////////////////////
//	ViciousHammer — drag equip + USE_CASH_ITEM(557)
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"
#include "../../Data/ItemData.h"

#include <string>

namespace ms
{
	class UIViciousHammer : public UIDragElement<PosVICIOUSHAMMER>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::VICIOUSHAMMER;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIViciousHammer();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		bool send_icon(const Icon& icon, Point<int16_t> cursor_position) override;
		UIElement::Type get_type() const override;

		void set_status(const std::string& msg);
		void set_hammer_used(int32_t used);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t { BtStart, BtCancel };

		bool has_assets;
		int32_t hammer_used;
		int16_t equip_slot;
		int32_t equip_itemid;
		Texture backgrnd;
		Texture equip_icon;
		Text status;

		Rectangle<int16_t> drop_area() const;
	};
}
