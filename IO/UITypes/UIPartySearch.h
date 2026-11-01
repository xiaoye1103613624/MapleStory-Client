//////////////////////////////////////////////////////////////////////////////////
//	Wave A #2 · PartySearch（队长自动匹配；s9 START/UPDATE，无列表板回包）
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"

#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

namespace ms
{
	class UIPartySearch : public UIDragElement<PosPARTYSEARCH>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::PARTYSEARCH;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIPartySearch();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void toggle_active() override;
		void set_searching(bool on);
		bool is_searching() const { return searching; }

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t
		{
			BtClose,
			BtStart,
			BtStop,
			BtPause,
			BtCancel,
			BtReg
		};

		void start_search();
		void stop_search();

		bool has_assets;
		bool searching;
		int32_t min_level;
		int32_t max_level;
		int32_t jobs_mask;

		Texture backgrnd;
		Text status_label;
	};
}
