//////////////////////////////////////////////////////////////////////////////////
// UIBreakthroughStone — 192 突破石窗（UIWindowCN.img/CN_Limitbreakthrough）
// 底板 196x285；Effect/Normal 12 帧 176x86（内联实体帧）；BtOk/BtOnemore 四态。
// 协议 BREAKTHROUGH_STONE_ACTION C 0x371B / S 0x371C。
// 破功值从 damage::server_limit_break() 读取（0x3730 已由 CommonHandlers 同步）。
// 资源已在本端构建阶段把占位 outlink 内联为实体位图，运行时无需 link 解析。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	class UIBreakthroughStone : public UIDragElement<PosBREAKTHROUGHSTONE>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::BREAKTHROUGHSTONE;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIBreakthroughStone();

		void draw(float inter) const override;
		void update() override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Button::State button_pressed(uint16_t buttonid) override;
		UIElement::Type get_type() const override;

		// 由 0x371C handler 调用：respType=0 状态(limitBreak,stoneCount)；=1 结果。
		void apply_state(int64_t limitbreak, int32_t stonecount);
		void apply_result(bool success, int64_t before, int64_t after, int32_t remaining);

	private:
		enum Buttons : uint16_t { BtOk = 0, BtOnemore = 1, BtClose = 2 };

		static constexpr int16_t WIN_W = 196;
		static constexpr int16_t WIN_H = 285;
		static constexpr int16_t FX_FRAMES = 12;

		bool has_assets = false;
		Texture backgrnd[4];     // backgrnd..backgrnd3 (backgrnd4=标题下方实体，作第4张)
		Texture fx_normal[FX_FRAMES];
		std::vector<Texture> fx_gradeup;   // 12 帧（实体）
		std::vector<Texture> fx_use;       // 12 帧（实体）
		Text info;

		int64_t limitbreak = 0;
		int32_t stonecount = 0;
		bool has_snapshot = false;

		// 动画状态：0 空闲，1 使用过程(Normal)，2 成功(GradeUp)，3 失败(Use)
		uint8_t fx_mode = 0;
		size_t fx_frame = 0;
		uint32_t fx_delay = 0;

		void close();
	};
}
