//////////////////////////////////////////////////////////////////////////////////
//	MTS / ITC list shell (MTS_OPERATION / MTS_OPERATION2)
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"
#include "../../Graphics/Text.h"

#include <string>
#include <utility>
#include <vector>

namespace ms
{
	class UIMTS : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::MTS;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIMTS();

		void draw(float inter) const override;
		void update() override;
		void update_screen(int16_t new_width, int16_t new_height) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void set_list(int32_t tab, int32_t type, int32_t page, int32_t count, int32_t slots);
		void set_cash(int32_t nx, int32_t maple);
		void set_status(const std::string& tip);
		void set_items(const std::vector<std::pair<int32_t, int32_t>>& items); // itemid, price

	private:
		bool visible = false;
		int32_t tab = 0;
		int32_t type = 0;
		int32_t page = 0;
		int32_t count = 0;
		int32_t slots = 0;
		int32_t nx = 0;
		int32_t maple = 0;
		Text title;
		Text cash_line;
		Text list_line;
		Text items_line;
		Text status_line;
	};
}
