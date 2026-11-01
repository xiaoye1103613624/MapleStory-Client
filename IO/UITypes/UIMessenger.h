//////////////////////////////////////////////////////////////////////////////////
//	Wave A · Messenger 聊天室（协议闭环：出站 122 / 入站 313）
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../Components/Textfield.h"

#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"
#include "../../Character/Look/CharLook.h"
#include "../../Net/Login.h"

#include <array>
#include <deque>
#include <memory>
#include <string>

namespace ms
{
	class UIMessenger : public UIDragElement<PosMESSENGER>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::MESSENGER;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIMessenger();

		void draw(float inter) const override;
		void update() override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void toggle_active() override;

		/// 用户主动开窗时建房（messengerid=0）；已在房 / 仅被邀请进房则 noop。
		void try_create_room();

		void set_local_pos(int8_t pos);
		void set_member(int8_t pos, const std::string& name, int8_t channel, const LookEntry& look);
		void clear_member(int8_t pos);
		void append_chat(const std::string& line);
		bool in_room() const { return in_room_; }

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t
		{
			BtClose,
			BtEnter
		};

		struct Slot
		{
			bool occupied = false;
			std::string name;
			int8_t channel = 0;
			LookEntry look;
			std::unique_ptr<CharLook> charlook;
		};

		static constexpr size_t NAMEBAR_COUNT = 4;
		static constexpr size_t SLOT_COUNT = 3;
		static constexpr size_t CHAT_LINES = 8;

		// —— 版式常量：全部是窗口内坐标（Messenger 一族切片 origin 都是 (0,0)，
		//    所以 draw 点 == 可见左上角）。逐条测量依据见
		//    docs/features/ui-v83-window-restore/evidence/w4-chatbar/messenger-layers.md
		//
		// 聊天历史白框：backgrnd 的 WHITE 带是 y150..301，但 240 高的 backgrnd2
		// 叠在它上面盖住了 y0..239，所以可见白框只有 y240..301（62 行）。
		// A11M 未加 text_shift 时字形顶 ≈ draw_y + 10，故首行 draw_y = 231 → 字形 241。
		static constexpr int16_t CHAT_LOG_TOP = 231;
		static constexpr int16_t CHAT_LOG_LINE_H = 14;
		// 4 行 = 字形 241..296，离框底 301 还留 5，第 5 行就会顶出白框
		static constexpr size_t CHAT_LOG_VISIBLE = 4;
		// 输入槽是烤在 backgrnd 里的美术（黑边 y317 / y333，x9..245）→ 内沿 y318..332。
		// Textfield 自己不画框，bounds 只当命中区 + 文字锚点用。
		static constexpr int16_t CHAT_INPUT_X = 12;
		static constexpr int16_t CHAT_INPUT_Y = 318;
		static constexpr int16_t CHAT_INPUT_W = 228;
		static constexpr int16_t CHAT_INPUT_H = 15;
		// 与 UIChatBar 同一套（那边真机验过）：把字形从 bounds 顶 +10 抬回槽内。
		static constexpr int16_t CHAT_INPUT_TEXT_SHIFT_Y = -9;
		// BtEnter 39x17，贴图里橙色 ENTER 徽标占 x5..32 / y6..10；
		// 参考端徽标落在窗口 (250,328) → 钮左上 = (245,322)。
		static constexpr int16_t BT_ENTER_X = 245;
		static constexpr int16_t BT_ENTER_Y = 322;

		void leave_room();
		void prompt_invite();
		void send_chat(const std::string& text);

		bool has_assets;
		bool in_room_;
		bool create_sent;
		int8_t local_pos;
		std::string selected_name;

		Texture backgrnd;
		Texture backgrnd2;
		Texture backgrnd3;
		std::array<Texture, NAMEBAR_COUNT> namebars;
		std::array<Point<int16_t>, NAMEBAR_COUNT> namebar_pos;
		std::array<Slot, SLOT_COUNT> slots;
		std::array<Text, SLOT_COUNT> slot_labels;

		std::deque<std::string> chat_log;
		std::deque<Text> chat_labels;
		Textfield chat_input;
	};
}
