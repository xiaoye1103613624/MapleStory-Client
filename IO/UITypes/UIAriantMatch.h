//////////////////////////////////////////////////////////////////////////////////
//	AriantMatch result / score HUD
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

namespace ms
{
	class UIAriantMatch : public UIDragElement<PosNOTICE>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::ARIANTMATCH;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIAriantMatch();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void set_score(const std::string& line);
		void show_result_frame(int index);

	private:
		bool has_assets = false;
		Texture result_tex;
		Text score_label;
	};
}
