//////////////////////////////////////////////////////////////////////////////////
//	plant / raise farming shells
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

namespace ms
{
	class UIPlant : public UIDragElement<PosNOTICE>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::PLANT;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIPlant();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void set_tip(const std::string& tip);

	private:
		bool has_assets = false;
		Texture backgrnd;
		Text tip_label;
	};

	class UIRaise : public UIDragElement<PosNOTICE>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::RAISE;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIRaise();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void set_tip(const std::string& tip);

	private:
		bool has_assets = false;
		Texture backgrnd;
		Text tip_label;
	};
}
