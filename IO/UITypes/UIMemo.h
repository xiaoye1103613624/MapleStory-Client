//////////////////////////////////////////////////////////////////////////////////
//	Wave A · Memo / 便签
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

#include <string>
#include <vector>

namespace ms
{
	struct MemoEntry
	{
		int32_t id = 0;
		std::string from;
		std::string message;
		int8_t fame = 0;
	};

	class UIMemo : public UIDragElement<PosMEMO>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::MEMO;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIMemo();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		UIElement::Type get_type() const override;

		void set_notes(std::vector<MemoEntry> notes);
		void show_tip(const std::string& tip);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t { BtClose, BtOpen, BtSave, BtDelete };

		bool has_assets;
		Texture backgrnd;
		Texture backgrnd2;
		std::vector<MemoEntry> notes;
		int selected;
		Text tip_label;
	};
}
