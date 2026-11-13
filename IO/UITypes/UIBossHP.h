//////////////////////////////////////////////////////////////////////////////////
//	Boss HP bar — FIELD_EFFECT mode 5 (showBossHP)
//	P1：多 oid 同时显示（双段扎昆等），按 oid 分条叠画，不再只留最后一条。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

#include <cstdint>
#include <map>
#include <vector>

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
		struct Entry
		{
			int32_t cur_hp = 0;
			int32_t max_hp = 1;
			int32_t hide_ms = 0;
			uint8_t tag_color = 0;
			uint8_t tag_bg = 0;
			Text label;
		};

		static constexpr int16_t BAR_INNER_W = 640;
		static constexpr int16_t STACK_GAP = 4;
		static constexpr size_t MAX_BARS = 4;

		bool has_assets = false;
		std::map<int32_t, Entry> bars;	// oid → 血条
		std::vector<int32_t> order;		// 绘制序（新到在上或下）
		Texture backgrnd;
		Texture fill;
		Texture textlay;

		void touch_order(int32_t oid);
		int16_t bar_height() const;
	};
}
