//////////////////////////////////////////////////////////////////////////////////
// 天气前景/天空素材层（独立自绘，不改 MapBackgrounds 类与 083 地图数据）
//
// 坐标契约（本引擎实测）：Camera.cpp:108 viewx = VWIDTH/2 - 玩家世界x（偏移量），
// Char.cpp:44 世界→屏幕 = world + viewx。视差与滚动完全照抄 083 Background::draw
// （MapBackgrounds.cpp:112-165）：HMOVE 层 x = 累计位移 + viewx；静态层
// shift = rx*(WOFFSET-viewx)/100 + WOFFSET 再 + 基准x。
//
// 分层契约（bundle weatherfx.cpp:153-157）：天空类素材（渐变/月/星/云/沙暴/彩虹/雪远）
// 画在 Stage::draw 的 drawbackgrounds 之前——盖住地图原天空(back/0-1)，随后地图全部
// 背景（含山树 back/2+）画在其上，树永不被盖。前景素材（雨/雾/雪近）画在角色之后。
// 与 bundle 的差异：bundle 只保留 back/0-1 在注入层后，本版天空素材垫在所有背景后，
// 地图若自带不透明天空则渐变被其盖住（保守、绝不遮树）。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Graphics/Texture.h"
#include "../Graphics/DrawArgument.h"

#include <cstddef>
#include <vector>

namespace ms
{
	namespace weather
	{
		class BackLayers
		{
		public:
			static BackLayers& get();

			// bs/no = 地图 back/0 的基座（来自 MapBackgrounds::get_backmost_bs/no）。
			// 对齐插件 FieldHasSky：仅当 (bs,no) 命中天空表才注入天空/雨层；
			// 且**不要求 has_state()**——服务端 0x373D 在进图之后才到，
			// 若在此处 gate 状态，每个会话首图会恒为白天（weather.cpp:2079-2084）。
			void on_map_loaded(const std::string& bs, int32_t no);
			void on_map_loaded();
			void update(bool snap, uint16_t timestep);

			// 天空类：drawbackgrounds 之前（树在其上）。
			void draw_sky(double viewx, double viewy, float alpha) const;
			// 前景类：角色之后、夜色蒙版之前。
			void draw_front(double viewx, double viewy, float alpha) const;

			void clear();

		private:
			BackLayers() = default;

			struct Layer
			{
				// 用 Texture 而非 Animation：Animation::draw 会把其自身 opacity linear
				// （单帧 canvas 缺省 a0 常为 0）经 operator+ 乘进 alpha，导致素材全透明。
				// Texture::draw 只认 DrawArgument 的 color.alpha，直接受控。
				Texture texture;
				int16_t base_x = 0, base_y = 0;   // 083 back 节点 x/y 语义
				double rx = 0, ry = 0;
				int type = 0;                     // 083 Background::Type 同编码
				int16_t cx = 0, cy = 0;           // 平铺间距（0=用画布尺寸）
				int htile = 1, vtile = 1;
				double acc_x = 0, acc_y = 0;      // HMOVE/VMOVE 累计位移
				float current = 0.0f;
				int kind = 0;                     // K_* 驱动源
			};

			std::vector<Layer> sky_;    // 背景天空层
			std::vector<Layer> front_;  // 前景层
			bool active_ = false;
			// 月亮呼吸（新增效果；参考实现 weatherfx.cpp:1098-1104 明确无呼吸，本项为
			// 用户 2026-11-20 明确要求新增）。周期 7s，亮度 0.82..1.00。
			static constexpr uint32_t kMoonBreatheMs = 7000;
			uint32_t moon_breathe_ms_ = 0;
			float moon_breathe_ = 1.0f;

			void build(std::vector<Layer>& out, bool front);
			void draw_list(const std::vector<Layer>& list, double viewx, double viewy, float alpha) const;
		};
	}
}
