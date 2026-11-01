//////////////////////////////////////////////////////////////////////////////////
//	Wave A · Family 信息窗
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

#include <string>
#include <vector>

namespace ms
{
	struct FamilyInfoData
	{
		int32_t cur_rep = 0;
		int32_t tot_rep = 0;
		int32_t today_rep = 0;
		int16_t juniors = 0;
		int16_t juniors_max = 2;
		int32_t leader_id = 0;
		std::string name;
		std::string message;
	};

	class UIFamily : public UIDragElement<PosFAMILY>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::FAMILY;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIFamily();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void set_info(const FamilyInfoData& data);
		void set_privileges(const std::vector<std::string>& names);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t { BtClose, BtOK, BtTree, BtLeft, BtRight, BtJunior, BtPrecept, BtSpecial };

		bool has_assets;
		Texture backgrnd;
		FamilyInfoData info;
		Text name_label;
		Text msg_label;
		Text rep_label;
	};
}
