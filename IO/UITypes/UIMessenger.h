//////////////////////////////////////////////////////////////////////////////////
//	Wave A · Messenger 聊天室（协议闭环：出站 122 / 入站 313）
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../Components/Textfield.h"

#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Animation.h"
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
		/// s2c mode=6：区分真聊天（含 " : "）与打字指示（尾字符 0/1，对照 v95 OnChat）。
		void handle_messenger_chat(const std::string& text);
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
			bool typing = false; // 对方正在输入（chatBalloon GA_REPEAT）
			std::string name;
			int8_t channel = 0;
			LookEntry look;
			std::unique_ptr<CharLook> charlook;
			Animation balloon; // 每座独立动画时钟
		};

		// 底板画了 4 枚名牌（NameBar/0..3，灰/蓝/红/绿，三上一中），模型却只留 3 格
		// ⇒ 服务端发到 pos=3 时 set_member 直接 return，第 4 枚名牌永远是空的。
		// 按绘制面收敛成 4（UIWhisperTargetDlg 同族教训：绘制与命中/取数必须同源）。
		static constexpr size_t NAMEBAR_COUNT = 4;
		static constexpr size_t SLOT_COUNT = 4;
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
		// BtEnter 39x17。真机逐格（evidence/w4-chatbar/messenger-layers.md §7）：
		// 原生帧里橙色 ENTER 徽标在窗口坐标 x252..279，本贴图徽标在钮内 x5..32
		// ⇒ 钮左上 x = 252-5 = 247。
		// （R14-10 曾按「参考图 local x250」把它从 247 改成 245，那是漏算了参考帧
		//   左边被裁掉的 2px：同一帧里输入槽读成 x7..243，而切片 backgrnd 自己是
		//   x9..245，四枚名牌、backgrnd3 页签条也都差这 2px。补回 2px 后三处烘焙层
		//   与徽标同时归零，245 才是把钮挪偏的那一步。）
		static constexpr int16_t BT_ENTER_X = 247;
		static constexpr int16_t BT_ENTER_Y = 322;

		// 立绘脚点（相对 NameBar 左上）：NameBar 89×19 → 水平中线 44；
		// 垂直贴名牌上沿再上抬 10（TAN 面板 y26..117，脚点≈98 时 0.70 身高约 50px 不撞标题）。
		// 证据：messenger-layers.md §2–3；r495/r474 缩放经验值（无原生 oracle，待真机微调）。
		static constexpr float LOOK_SCALE = 0.70f;
		static constexpr int16_t LOOK_FOOT_DX = 44;
		static constexpr int16_t LOOK_FOOT_DY = -10;
		static constexpr int16_t LOOK_NAME_DX = 44;
		static constexpr int16_t LOOK_NAME_DY = 2;

		// chatBalloon：v95 SetAvatar RelMove(-45,-100) 相对 Avatar 原点；本端脚点 + 0.70 缩放
		// → 近似 (-45*0.70, -100*0.70) ≈ (-31,-70)。证据 ida-v95-blink-chatballoon.txt。
		static constexpr int16_t BALLOON_DX = -31;
		static constexpr int16_t BALLOON_DY = -70;

		// blink：v95 StartBlink 置 m_nBlink=30；Update 每 tick -1，==15 隐、==0 重置并显。
		// 原生仅 m_nState==2（最小化标题条）才闪；本端无 Min 态，收消息时在标题条右上闪烁。
		static constexpr int16_t BLINK_TICKS = 30;
		static constexpr int16_t BLINK_HALF = 15;
		static constexpr int16_t BLINK_DRAW_X = 260; // 295-21(close)-14 ≈ 标题条右
		static constexpr int16_t BLINK_DRAW_Y = 4;

		// 打字状态节流：v95 m_tRestUpdateState=3000（ms 量级）。
		static constexpr int32_t TYPING_THROTTLE_MS = 3000;

		void leave_room();
		void prompt_invite();
		void send_chat(const std::string& text);
		/// s9 建房/进房对本机只发 joinMessenger(pos) 不带 look；用本机 Player::CharLook 填座位。
		void fill_local_look(int8_t pos);
		void start_blink();
		void set_slot_typing(int8_t pos, bool typing);
		void send_typing(bool typing);
		int8_t find_slot_by_name(const std::string& name) const;

		bool has_assets;
		bool in_room_;
		bool create_sent;
		bool input_typing_sent; // 已上报「正在输入」
		int8_t local_pos;
		int16_t blink_ticks; // >0 闪烁中；偶半显、奇半隐（对齐 ToggleBlink）
		int32_t typing_throttle_ms;
		std::string selected_name;

		Texture backgrnd;
		Texture backgrnd2;
		Texture backgrnd3;
		Texture blink; // UIWindow.img/Messenger/blink 12×10 橙标
		Animation balloon_proto; // chatBalloon/0..4 样板，座位拷贝用
		std::array<Texture, NAMEBAR_COUNT> namebars;
		std::array<Point<int16_t>, NAMEBAR_COUNT> namebar_pos;
		std::array<Slot, SLOT_COUNT> slots;
		std::array<Text, SLOT_COUNT> slot_labels;

		std::deque<std::string> chat_log;
		std::deque<Text> chat_labels;
		Textfield chat_input;
	};
}
