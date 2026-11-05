////////////////////////////////////////////////////////////////////////////////
//	公会 Boss PQ HUD — s2c GUILD_BOSS_HEALER_MOVE(287) / PULLEY(288)
//	原生：地图物件属性 healer/pulley（IDA sub_5587B3）；本端无完整物件动画态机时
//	先以顶栏 HUD 承接同步，避免再塞进 UIQuestAlarm tip。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"
#include "../../Graphics/SpecialText.h"

#include <cstdint>
#include <string>

namespace ms
{
	class UIGuildBoss : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::GUILDBOSS;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UIGuildBoss();

		void draw(float inter) const override;
		void update() override;
		void update_screen(int16_t new_width, int16_t new_height) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void set_healer_y(int16_t y);
		void set_pulley_state(int8_t state);

	private:
		void bump_visible();

		bool visible = false;
		int32_t hide_ms = 0;
		int16_t healer_y = 0;
		int8_t pulley_state = 0;
		bool has_healer = false;
		bool has_pulley = false;
		mutable OutlinedText title;
		mutable OutlinedText line_healer;
		mutable OutlinedText line_pulley;
	};
}
