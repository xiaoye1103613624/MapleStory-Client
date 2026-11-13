//////////////////////////////////////////////////////////////////////////////////
//	char-stats-ui-ps v4 数值坐标表（由 layout_cn.json 的 *_val_* 派生）
//	验收：数值墨迹顶落在白框内；ink_top = value_box_y + 3（16px 框）。
//	详情相对坐标 = 画布坐标 − DETAIL_OX(306)。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>

namespace ms::UIStatsLayoutV4
{
	constexpr int16_t MAIN_W = 300;
	constexpr int16_t MAIN_H = 392;
	constexpr int16_t DETAIL_W = 352;
	constexpr int16_t DETAIL_H = 392;
	constexpr int16_t GAP = 6;
	constexpr int16_t DETAIL_OX = MAIN_W + GAP;	// 306
	constexpr int16_t DETAIL_OY = 0;

	// 值框内文字左内边距；墨迹顶相对值框顶
	constexpr int16_t VAL_PAD_X = 4;
	constexpr int16_t INK_PAD_Y = 3;

	constexpr int16_t MAIN_VAL_X = 88 + VAL_PAD_X;
	constexpr int16_t DETAIL_VAL_LX = 88 + VAL_PAD_X;	// 394-306
	constexpr int16_t DETAIL_VAL_RX = 254 + VAL_PAD_X;	// 560-306
	constexpr int16_t DETAIL_ATK_X = DETAIL_VAL_LX;

	// 主栏身份/资源行值框 y（layout main_val_0..10）
	constexpr int16_t MAIN_Y_NAME = 24;
	constexpr int16_t MAIN_Y_JOB = 43;
	constexpr int16_t MAIN_Y_LEVEL = 62;
	constexpr int16_t MAIN_Y_GUILD = 81;
	constexpr int16_t MAIN_Y_FAME = 100;
	constexpr int16_t MAIN_Y_DAMAGE = 119;
	constexpr int16_t MAIN_Y_HP = 138;
	constexpr int16_t MAIN_Y_MP = 157;
	constexpr int16_t MAIN_Y_AP = 176;
	constexpr int16_t MAIN_Y_EXP = 195;
	constexpr int16_t MAIN_Y_EP = 214;
	constexpr int16_t MAIN_Y_STR = 236;
	constexpr int16_t MAIN_Y_DEX = 255;
	constexpr int16_t MAIN_Y_INT = 274;
	constexpr int16_t MAIN_Y_LUK = 293;

	// 左栏抗性值框（3×2）
	constexpr int16_t RES_X0 = 6 + VAL_PAD_X;
	constexpr int16_t RES_X1 = 103 + VAL_PAD_X;
	constexpr int16_t RES_X2 = 200 + VAL_PAD_X;
	constexpr int16_t RES_Y0 = 334;
	constexpr int16_t RES_Y1 = 369;

	// 详情：通栏物攻 + 双列（dL/dR y 从 42 起 pitch 18）
	constexpr int16_t DET_Y_ATK = 24;
	constexpr int16_t DET_Y0 = 42;
	constexpr int16_t DET_PITCH = 18;

	inline constexpr int16_t det_y(int row) { return static_cast<int16_t>(DET_Y0 + row * DET_PITCH); }

	// 按钮（相对主窗；详情收起相对整窗）
	constexpr int16_t BTN_CLOSE_X = 283;	// 300-14-3
	constexpr int16_t BTN_CLOSE_Y = 5;
	constexpr int16_t BTN_APUP_X = 280;	// 值框右缘内侧
	constexpr int16_t BTN_DETAIL_X = 245;	// 300-46-9
	constexpr int16_t BTN_DETAIL_Y = 369;	// 392-19-4
	constexpr int16_t BTN_HIDE_X = 637;	// DETAIL_OX+352-13-8
	constexpr int16_t BTN_HIDE_Y = 371;	// 392-12-9
	constexpr int16_t BTN_AUTO_X = 198;	// 300-100-2
	constexpr int16_t BTN_AUTO_Y = 176;	// 对齐 AP 行
}
