//////////////////////////////////////////////////////////////////////////////////
//	Admin Block panel shell
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

namespace ms
{
	class UIAdmin : public UIDragElement<PosNOTICE>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::ADMIN;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UIAdmin();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void set_message(const std::string& msg);

	private:
		bool has_assets = false;
		Texture backgrnd;
		Text msg_label;
	};
}
