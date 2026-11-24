//////////////////////////////////////////////////////////////////////////////////
// UIDailyCheckin — 28 天每日签到窗（原生移植 2026-11-18）
// 入口：侧栏图标 / @daily 命令。协议 DAILY_CHECKIN C 0x3717 / S 0x3718。
// 快照：currentDay（可领日或最后领取日）、claimedMask（28 位）、justClaimed。
// 渲染：v4 整窗栅格（中文第N天已烧进图）+ cell/v1 状态图（mask/claimed/label）叠加；
//       格子几何按 manifest（67×71，原点(7,59)，X间距5，行距72）；点击可领格发 CLAIM 包。
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
	class UIDailyCheckin : public UIDragElement<PosDAILYCHECKIN>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::DAILYCHECKIN;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIDailyCheckin();

		void draw(float inter) const override;
		void update() override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Button::State button_pressed(uint16_t buttonid) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void remove_cursor() override;
		UIElement::Type get_type() const override;

		// 由 0x3718 handler 调用，落地快照（含 28 天 icon/tooltip）并刷新。
		void apply_snapshot(int8_t cd, int32_t mask, int8_t jc,
			const std::vector<int32_t>& icon_ids, const std::vector<std::string>& tooltips);

	private:
		enum Buttons : uint16_t { BtClose = 1000, BtCellBase = 2000 };

		// v4 整窗 513x346；格子几何来自 manifest：单格 67x71，原点(7,59)，X间距5，行距72。
		static constexpr int16_t WIN_W = 513;
		static constexpr int16_t WIN_H = 346;
		static constexpr int16_t HEADER_H = 20;
		static constexpr int16_t CELL_W = 67;
		static constexpr int16_t CELL_H = 71;   // v4 manifest: 单格 67x71（行距另用 pitch=72）
		static constexpr int16_t CELL_PITCH = 72;
		static constexpr int16_t CELL_HEADER_H = 19;  // 粉头"第N天"高 19，icon 画在其下内容区
		static constexpr int16_t CELL_X0 = 7;
		static constexpr int16_t CELL_Y0 = 59;
		static constexpr int16_t CELL_GAP = 5;
		static constexpr int16_t COLS = 7;
		static constexpr int16_t ROWS = 4;
		static constexpr int16_t DAYS = 28;

		void rebuild_cells();
		int16_t cell_at(Point<int16_t> cursorpos) const;

		int8_t current_day = 0;
		int32_t claimed_mask = 0;
		int8_t just_claimed = 0;
		bool has_snapshot = false;
		bool has_assets = false;
		Texture backgrnd;
		Texture cell_normal;    // 67x71 空态
		Texture cell_mask;      // 67x71 灰蒙版（未解锁）
		Texture cell_claimed;   // 67x71 已领取
		Texture cell_label;     // 40x18 "已领取" 角标
		Text hint;

		// 服务端 28 天奖励展示（需求包 §6：每格一主 icon + hover 多件明细 tooltip）。
		std::vector<int32_t> reward_icons;     // 28× iconItemId
		std::vector<std::string> reward_tips;  // 28× tooltip 文本
		std::vector<Texture> reward_icon_textures;
		int16_t hover_day = -1;                // 当前 hover 的格子(0-based)，-1=无

		void close();
		void refresh_icon_textures();
	};
}
