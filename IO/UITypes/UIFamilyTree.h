//////////////////////////////////////////////////////////////////////////////////
//	Family pedigree tree — FAMILY_CHART_RESULT (showPedigree)
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

#include <string>
#include <vector>

namespace ms
{
	struct FamilyTreeEntry
	{
		int32_t chr_id = 0;
		int32_t senior_id = 0;
		int16_t job = 0;
		uint8_t level = 0;
		bool online = false;
		int32_t rep = 0;
		int32_t tot_rep = 0;
		std::string name;
	};

	class UIFamilyTree : public UIDragElement<PosFAMILYTREE>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::FAMILYTREE;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIFamilyTree();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		UIElement::Type get_type() const override;

		void set_chart(int32_t viewed_id, std::vector<FamilyTreeEntry> entries, int16_t add_enabled);
		int32_t get_selected_cid() const { return selected_cid; }
		const FamilyTreeEntry* selected_entry() const;

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t { BtClose, BtBye, BtLeft, BtRight, BtJunior };

		static constexpr int16_t ROW_H = 18;
		static constexpr int16_t ROW_TOP = 52;

		bool has_assets;
		int32_t viewed_id;
		int16_t add_enabled;
		int32_t selected_cid = 0;
		Texture backgrnd;
		std::vector<FamilyTreeEntry> entries;
		std::vector<Text> name_labels;
		Text header;
	};
}
