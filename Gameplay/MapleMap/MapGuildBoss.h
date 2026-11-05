////////////////////////////////////////////////////////////////////////////////
//	公会 PQ（CField_GuildBoss）地图物件 —— healer / pulley 真驱动
//	证据：IDA sub_5587B3 读地图属性；sub_558B0F 分发 287/288；
//	      sub_558C0C 把 short Y 落到 healer 层；sub_558C3B 存 pulley state 字节。
//	样板图：Map9/990000900.img（莎莲娜王室）。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../Graphics/Animation.h"
#include "../../Template/Point.h"

#include <cstdint>
#include <string>

namespace ms
{
	class MapGuildBoss
	{
	public:
		MapGuildBoss();
		explicit MapGuildBoss(nl::node map_src);

		void clear();
		void draw(Point<int16_t> viewpos, float alpha) const;
		void update();

		/// s2c 287：绝对 Y（原生 sub_558C0C 读 short，相对旧 Y RelMove 后覆写）。
		void set_healer_y(int16_t y);
		/// s2c 288：滑轮状态字节（原生仅写入 field+507，交互门控用）。
		void set_pulley_state(uint8_t state);

		bool has_healer() const { return healer_loaded; }
		bool has_pulley() const { return pulley_loaded; }
		int16_t healer_y() const { return healer_y_now; }
		uint8_t pulley_state() const { return pulley_state_byte; }

	private:
		static nl::node resolve_obj(const std::string& wz_path);

		bool healer_loaded = false;
		bool pulley_loaded = false;

		Animation healer_anim;
		Animation pulley_anim;

		int16_t healer_x = 0;
		int16_t healer_y_now = 0;
		int16_t healer_y_from = 0;
		int16_t healer_y_to = 0;
		int16_t healer_y_min = 0;
		int16_t healer_move_ms = 0;
		int16_t healer_move_dur = 0;
		int16_t healer_rise = 1;
		int16_t healer_fall = 10;

		int16_t pulley_x = 0;
		int16_t pulley_y = 0;
		uint8_t pulley_state_byte = 0;
	};
}
