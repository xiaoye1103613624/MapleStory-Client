//////////////////////////////////////////////////////////////////////////////////
//	Boss HP bar — FIELD_EFFECT mode 5 (showBossHP)
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

namespace ms
{
	class UIBossHP : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::BOSSHP;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UIBossHP();

		void draw(float inter) const override;
		void update() override;
		void update_screen(int16_t new_width, int16_t new_height) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void set_hp(int32_t oid, int32_t cur, int32_t max, uint8_t tag_color, uint8_t tag_bg);

	private:
		static constexpr int16_t BAR_INNER_W = 640;

		bool has_assets = false;
		bool visible = false;
		int32_t oid = 0;
		int32_t cur_hp = 0;
		int32_t max_hp = 1;
		int32_t hide_ms = 0;
		uint8_t tag_color = 0;
		uint8_t tag_bg = 0;
		Texture backgrnd;
		Texture fill;      // DualMobGauge/gauge：1×9 填充条（横向拉伸）
		Texture textlay;   // DualMobGauge/text：791×26 名牌底（可选）
		Text label;
	};
}
