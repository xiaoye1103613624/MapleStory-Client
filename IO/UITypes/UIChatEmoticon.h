//////////////////////////////////////////////////////////////////////////////////
// UIChatEmoticon — Kaentake 动态聊天表情选择窗（原生移植 2026-11-18）
//
// 入口：聊天栏表情按钮（UIChatBar::BtChatEmoticon）。
// 数据：Effect/ChatEmoticon.img/Dynamic{0..3}/{id}/effect/{frame}（资源已入 live）。
// 发包：ChatEmoticonPacket(id) → s9 ChatEmoticonHandler → 同图广播 → 他人头顶动画。
// 协议：CHAT_EMOTICON C 0x3715 / S 0x3716；白名单 ID 四组，与服务端 handler 一致。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	class UIChatEmoticon : public UIDragElement<PosCHATEMOTICON>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::CHATEMOTICON;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIChatEmoticon();

		void draw(float inter) const override;
		void update() override;
		Button::State button_pressed(uint16_t buttonid) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		UIElement::Type get_type() const override;

		// 头顶动画播放（由收包 handler 调用，在目标角色 charId 对应位置显示）。
		static void show_overhead(int32_t char_id, int32_t emoticon_id);
		static void update_overhead();
		// 由 Stage 绘制链每帧调用，绘制所有活动头顶表情动画。
		// viewpos = camera.position(alpha)，与角色屏幕坐标口径一致（MapChars.cpp:42）。
		static void draw_overhead(Point<int16_t> viewpos, float alpha);

	private:
		enum Buttons : uint16_t
		{
			BtClose = 1000,
			BtTabBase = 2000,   // 2000..2003 = Dynamic0..3
		};

		struct EmoticonEntry
		{
			int32_t id;
			Texture icon;   // 首帧缩略图（可能为空）
			bool usable = false;
		};

		static constexpr int16_t WIN_W = 360;
		static constexpr int16_t WIN_H = 300;
		static constexpr int16_t HEADER_H = 36;
		static constexpr int16_t CELL = 40;
		static constexpr int16_t COLS = 7;
		static constexpr int16_t ROWS = 5;
		static constexpr int16_t PER_PAGE = COLS * ROWS;
		static constexpr int16_t TAB_COUNT = 4;

		void build_ids();
		void load_page_icons();
		int32_t page_count() const;
		int16_t cell_at(Point<int16_t> cursorpos) const;

		int16_t tab = 0;
		int16_t page = 0;
		std::vector<std::vector<int32_t>> ids_by_tab;   // 每类的全部 ID
		std::vector<EmoticonEntry> page_entries;         // 当前页条目（含缩略图）
	};
}
