//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"

#include "../Components/Textfield.h"
// 展开态右端竖滚动条（B5-1），与背包/帮助窗同一件 Basic.img/VScr 组件。
#include "../Components/Slider.h"
// Q1 定案的共享框体件（半透明黑底 + 1px 灰边），聊天历史框与 tooltip 同判据。
#include "../Components/TextTooltip.h"
// 收起态灰带用纯色矩形（本包没有对应位图，见 classic_fold_panel 注释）。
#include "../../Graphics/Geometry.h"
#include <array>
// Q5: collect_online_buddy_cids 返回 std::list（MultiChatPacket 收件人表同型）。
#include <list>

// TODO: Change these?
#include "../Messages.h"

namespace ms
{
	class UIChatBar : public UIDragElement<PosCHAT>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::CHATBAR;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIChatBar();

		void draw(float inter) const override;
		void update() override;
		// 指针离开聊天栏时框架会调这里：必须把滚动条的按压态清掉，否则滑块拖出
		// 命中区后一直停在 PRESSED，下次进区直接接着滚。
		void remove_cursor() override;

		Button::State button_pressed(uint16_t buttonid) override;

		bool is_in_range(Point<int16_t> cursor_position) const override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursor_position) override;
		void send_scroll(double yoffset) override; // Q3: 历史框滚轮翻页
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

		enum MessageGroup : uint8_t
		{
			ALL,
			BATTLE,
			PARTY,
			FRIEND,
			GUILD,
			ALLIANCE,
			SPOUSE,
			GROUP,
			LENGTH
		};

		enum MessageType : uint8_t
		{
			UNK0,
			WHITE,
			RED,
			BLUE,
			YELLOW,
			GM // GM 账号本地回显自己输入的 !命令（服务端当指令吃掉不广播）
		};

		// 频道/对象枚举：与收包 handler（GeneralChat/Whisper/MultiChat/SpouseChat/
		// ServerMessage）一一对应，决定聊天正文配色（见 channel_color）。
		enum class ChatChannel : uint8_t
		{
			ALL,        // 普通/对世界（白字）
			WHISPER,    // 密语
			PARTY,      // 组队
			GUILD,      // 家族
			BUDDY,      // 好友
			ALLIANCE,   // 联盟
			SPOUSE,     // 夫妻
			MEGAPHONE,  // 喇叭/超级喇叭
			SYSTEM,     // 系统公告/服务器通知
			GM          // GM 指令本地回显
		};

		bool has_input() const;
		bool is_classic() const;
		void toggle_view();
		// UIStateGame 的 TOGGLECHAT 分支要在输入行收起时把它打开（classic 的 Enter 是
		// 唯一入口，展开/缩放钮只管历史框），故必须公开。
		void toggle_input(bool enabled);
		void show_message(const char* message, MessageType type);
		void show_message(const char* message, ChatChannel ch);
		// Q2: 带可点击玩家名的聊天消息。显示串 = prefix + speaker + " : " + message；
		// speaker 非空即可点击，点击弹出名字菜单（密语/查角色/加好友）。
		void show_message(const std::string& prefix, const std::string& speaker, const std::string& message, ChatChannel ch);
		static Color::Name channel_color(ChatChannel ch);
		void focus_whisper();
		void focus_whisper(const std::string& target);
		// 悄悄话目标弹层（UIWhisperTargetDlg）确认后的回填入口：目标切到「密语」、
		// 展开输入行并预置 "/w <名字> "，键盘焦点留给玩家补正文。
		void set_whisper_target(const std::string& target);
		// WhisperResultHandler（Net/Handlers/MessagingHandlers）成功分支回调：
		// 把 Q5 暂存的自说悄悄话正文回显成 "(To X) 我 : 正文"。ok=false 时只清暂存。
		void on_whisper_result(const std::string& target, bool ok);

		// TODO: Change these?
		void change_target(int32_t action) {}

	private:
		static constexpr int16_t INPUT_TEXT_HEIGHT = 18;
		static constexpr int16_t MIN_HEIGHT = 12;
		static constexpr int16_t MAX_HEIGHT = 467;

		bool indragrange(Point<int16_t> cursor_position) const override;
		bool intoprange(Point<int16_t> cursor_position) const;
		bool inbottomrange(Point<int16_t> cursor_position) const;
		bool inleftrange(Point<int16_t> cursor_position)  const;
		bool inrightrange(Point<int16_t> cursor_position) const;
		bool intopleftrange(Point<int16_t> cursor_position)  const;
		bool inbottomrightrange(Point<int16_t> cursor_position) const;
		bool intoprightrange(Point<int16_t> cursor_position)  const;
		bool inbottomleftrange(Point<int16_t> cursor_position) const;

		Point<int16_t> get_position() const;
		Point<int16_t> get_dragarea_position() const;
		Point<int16_t> get_input_position() const;
		Point<int16_t> get_input_text_position();
		void toggle_view(bool max, bool pressed);
		void update_view(bool pressed);
		void input_text_enter_callback(std::string message);
		void input_text_escape_callback();
		void close_input();
		void change_message(bool up);

		enum Buttons
		{
			BtMax,
			BtMin,
			BtChat,
			BtHelp,
			BtItemLink,
			BtChatEmoticon,
			BtOutChat,
			BtWhisper
		};

		enum DragDirection
		{
			NONE,
			DOWN,
			LEFT,
			DOWNLEFT
		};

		Texture drag;
		std::vector<Texture> max_textures;
		std::vector<Texture> min_textures;
		std::vector<Texture> input_textures;

		int16_t min_x;
		int16_t max_x;
		int16_t top_y;
		int16_t center_y;
		int16_t bottom_y;
		int16_t btMin_x;
		int16_t user_view_x;
		int16_t user_view_y;
		int16_t temp_view_x;
		int16_t temp_view_y;
		int16_t min_view_y;
		bool view_max;
		bool view_input;
		bool view_adjusted;
		bool position_adjusted;
		DragDirection drag_direction;
		Point<int16_t> temp_position;

		int16_t input_bg_x;
		int16_t input_bg_y;
		int16_t input_max_x;
		int16_t input_origin_x;
		int16_t input_origin_y;
		int16_t input_btns_x;
		Textfield input_text;

		struct Message
		{
			MessageGroup group;
			MessageType type;
			ChatChannel channel;
			Text text;
			std::string speaker;   // 可点击玩家名（空=系统/无名不可点）
			std::string prefix;    // 名字前的非点击前缀，如 "[Party] " / "(ch1) "
			bool clickable = false;
			int16_t name_x_off = 0; // 前缀像素宽：名字热区起点相对行首的 x 偏移
			int16_t name_w = 0;     // 名字像素宽

			Message() : group(MessageGroup::ALL), type(MessageType::UNK0), channel(ChatChannel::ALL), text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "") {}
			Message(std::string text) : group(MessageGroup::ALL), type(MessageType::UNK0), channel(ChatChannel::ALL), text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, text) {}
			Message(MessageGroup group, MessageType type, Text text) : group(group), type(type), channel(ChatChannel::ALL), text(text) {}
			Message(MessageGroup group, ChatChannel channel, Text text) : group(group), type(MessageType::UNK0), channel(channel), text(text) {}
		};

		// Q4: 聊天历史上限。原生（v83 派生本仓库）没有任何聊天日志上限常量
		// ——message_history 从落地起全程 push_back 无 cap，全代码搜索无上限
		// 数值；用户 2026-11-04 裁决按 GMS v83 常见值取 200。超上限从最旧一条
		// 裁起；classic_scroll_offset 是「距最新的行数」，从头擦除不影响贴底。
		static constexpr size_t MESSAGE_HISTORY_CAP = 200;
		// 三个 show_message 重载共用的裁剪入口（push_back 之后调）。
		void trim_message_history();

		std::vector<Message> message_history;
		std::vector<std::string> user_message_history;
		size_t user_message_history_index;

		bool classic_mode;
		Texture classic_input_bg;
		Texture classic_chat_target;

		// classic 聊天区布局。参考端底栏截帧（1282x142）与 StatusBar.img 逐件 SAD
		// 配准（SAD≤5，见 evidence/w4-chatbar/coords.md §1'）给出的**相对**几何：
		//   base/backgrnd(817x71) 顶 = 底栏顶，白输入带是它自身烘焙的一条，
		//     带顶 = 底栏顶 + 4、带内高 26（带下沿再往下 2px 深色分隔线）；
		//   base/chatTarget(81x20) 压在带内左上 (底栏左沿, 底栏顶 + 8)；
		//   base/chat(566x5) 在底栏顶 + 32（带下方那条半透明细条）；
		//   Basic.img/BtMin|BtMax(12x12) 在屏幕 (536, 661) = 行内 (536, 8)。
		// UIStatusBar 把 backgrnd 画在 (0, vh-71)，所以本行必须落在带内。旧公式
		// `vh-71-INPUT_ROW_H` 把整行抬到底栏上方 28px，正是用户看到的
		// 「对所有人显示在白色输入框上面、展开按钮也在上面」。
		//   输入行内含 chatTarget 目标框(81x20) + BtWhisper 切换箭头(12x19，压在框左沿)
		//   + 带下细条(base/chat 566x5) + 输入文本 + BtMax/BtMin(12x12)；
		//   历史框位于输入行上方（user_view_y 高）。
		static constexpr int16_t CLASSIC_BAR_H = 71;
		static constexpr int16_t INPUT_BAND_TOP = 4;
		static constexpr int16_t INPUT_ROW_H = 26;
		// 原生底栏输入带右沿 = 566：展开态截帧里滚动条灰轨落在 x 551..564、白带一直
		// 铺到 568（截帧切在 569），而本包 base/chat 正是 566x5。旧值 560 是从
		// chat/ingame 的 modern 宽度抄的，比原生窄 6px（滚动条就没地方放了）。
		static constexpr int16_t MAX_ROW_W = 566;
		static constexpr int16_t CHAT_TARGET_W = 81;
		static constexpr int16_t CHAT_TARGET_H = 20;
		static constexpr int16_t TARGET_BT_W = 12;
		// chatTarget 与 BtWhisper(12x19) 同顶（行内 y=4 → 屏幕 657）
		static constexpr int16_t TARGET_BOX_Y = 4;
		// 目标文字「对所有人」的行内**墨迹顶** = 8（框内区 4..24、字高 11 ⇒ 居中），
		// 绘制点由 Text::y_for_ink_top 按当前对象名反解（R27-B1b）。
		// 上一版把 bitmap_top 手抄成 11（PIL 口径，SimHei 11px 的 CJK 实测 9~10）
		// ⇒ 常量写成 draw_y=2，字实际低 2~3px。
		static constexpr int16_t TARGET_TEXT_INK_TOP = 8;
		// 展开/缩放钮(12x12)：参考端实测屏幕 (536, 661)，行内即 (536, 8)。底栏是定宽
		// 817 的贴图，所以原生用的是固定左偏移，这里跟着用固定值而不是右锚。
		static constexpr int16_t LOG_BT_X = 536;
		static constexpr int16_t LOG_BT_Y = 8;
		// base/chat 带下细条：底栏顶 +32 = 行内 28 = INPUT_ROW_H + 2
		static constexpr int16_t UNDERLINE_Y = INPUT_ROW_H + 2;
		// 参考端目标文字左对齐、紧跟箭头右侧（框内 x≈6，箭头占 0..12）
		static constexpr int16_t TARGET_TEXT_X = 14;

		// 收起态（历史框关 + 输入未激活）：原生把整条输入带的**内区**涂成不透明
		// 灰 (119,119,119)，只在里面显示最近一条消息 —— 白色输入带、chatTarget
		// 目标框、目标文字这时都不画。这就是用户报的「聊天收起不是应该在白色框
		// 区域嘛」。定版依据见 evidence/w4-chatbar/coords-fold.md：两张原生截帧
		// （聊天框展开时候.png / 聊天框收起的时候.png）按白带顶缘那条
		// (255,255,255) 对齐后，收起态灰带顶 = 对齐点 +3、高 23，文字墨迹顶 = +9。
		// 墨迹左沿原生是 6（收起态截帧行 17，与我们的行偏移同源可比）。本轮用同一把
		// 尺子（sum>447 且 >=2 像素）量两端同一串「[Welcome]」：原生首墨 x=6、我们在
		// 绘制点 4 时墨迹落在 7 ⇒ A11M 的 '[' 左 bearing 是 +3 而不是先前估的 +2，
		// 所以绘制点取 3。
		static constexpr int16_t FOLD_PANEL_TOP = 3;
		static constexpr int16_t FOLD_PANEL_H = 23;
		static constexpr int16_t FOLD_TEXT_INK_TOP = 9;
		static constexpr int16_t FOLD_TEXT_X = 3;
		// 灰带上方那条 1px 内沿暗线：原生实测 (87,87,95)（x=20/200/300 三列同值，
		// 只有 ±8 的抖动），是灰带面板自带的内阴影，不是 backgrnd 的像素（我们
		// 这行透出来的是 (182,195,201) 亮带）。所以贴着灰带顶再画一条。
		static constexpr int16_t FOLD_EDGE_TOP = FOLD_PANEL_TOP - 1;

		// 展开态（B5-1）：历史框高度可拖、右侧可滚。逐格读数见
		// evidence/w4-chatbar/coords-expand.md（原生「聊天框展开时候.png」569x103，
		// 白输入带顶缘 = 截帧 y75 = 本端 position.y()，x0 与视图 x0 对齐）。
		//   历史框：顶 y3、底 y71（白带顶 -4）⇒ 高 72（本次截帧的用户值）；
		//   滚动条：灰轨 x 551..564（宽 14，VScr5 滑块实测 15）、行 4..70，
		//            上箭头高光 y15、下箭头高光 y60 ⇒ 两端各内缩约 11~12；
		//   半透明底板覆盖 0..565（x549 已被压暗、x565 之外是亮场景）。
		static constexpr int16_t CLASSIC_LOG_H_DEFAULT = 80;
		// 下限按「3 行正文 + 上下呼吸」定，再小滚动条上下内缩会重叠；
		// 上限 = 视图 720 - 底栏 71 - 顶部 229（留出场景可视区）。
		static constexpr int16_t CLASSIC_LOG_H_MIN = 40;
		static constexpr int16_t CLASSIC_LOG_H_MAX = 420;
		// 滚动条列宽与上下内缩（内缩取自原生：轨道两端到框沿各 11~12px）。
		static constexpr int16_t SCROLL_W = 15;
		static constexpr int16_t SCROLL_INSET = 11;
		// 历史正文：首行离白带顶 23、行距 13、顶边允许溢出 10（原生就是半行裁切）。
		static constexpr int16_t LOG_FIRST_Y = 23;
		static constexpr int16_t LOG_LINE_H = 13;
		static constexpr int16_t LOG_LINE_SLACK = 10;
		// 历史正文左沿的**绘制点**：与收起态同判据 —— 原生收起态实测墨迹左沿 6，
		// A11M 左 bearing +2 ⇒ 绘制点 4。展开/收起同一列，消息折回灰带时不跳位。
		static constexpr int16_t LOG_TEXT_X = 4;
		// 顶边拖拽命中带（原生那条 1px 上沿只有 1px，手指按不到，给 6px）。
		static constexpr int16_t GRIP_H = 6;

		// 聊天目标：BtWhisper 箭头钮 + chatTarget 框体本身都可点，点击 = 展开目标列表，
		// 从列表里选（原生没有点击循环，用户 2026-11-02 定调）；历史框展开/收起用
		// Basic.img 的 BtMax(+) / BtMin(-) 真按钮同位互斥显示。
		bool classic_log_open = false;
		TooltipBackdrop classic_log_backdrop;
		// 展开态右端竖滚动条（Basic.img/VScr5，Slider::Type::CHATBAR 已兜底到它）。
		// 坐标约定与 UIItemInventory 一致：vertical/x 是「相对历史框左上角」的局部量，
		// draw(log_pos) 时才加窗口偏移，所以拖拽改高度必须同步 setvertical。
		Slider classic_scrollbar;
		// 历史框向上翻了几行（0 = 贴最新一条）。与 Slider 内部 row 反相：正文最新在
		// 最下，所以滑块贴轨道底部才是 offset 0。
		int16_t classic_scroll_offset = 0;
		// 顶边拖拽中：起点高度 + 起点光标 y，避免每帧用「光标到白带顶」直接当高度
		// 时按命中带上下沿起跳各差 6px。
		bool classic_resizing = false;
		int16_t classic_resize_h0 = 0;
		int16_t classic_resize_y0 = 0;
		// 本包没有任何位图能拼出收起态那条灰带（base/chat 是 5px 黑白条纹细条、
		// base/backgrnd2 是半透明白渐变，逐行取样都不等于实测的 (119,119,119) 平
		// 涂），所以按原生实测色自己铺一块不透明矩形。宽度取输入带宽（定值）。
		ColorBox classic_fold_panel;
		// 灰带顶的 1px 内沿暗线，见 FOLD_EDGE_TOP 的实测依据。
		ColorBox classic_fold_edge;
		Text classic_target_text;
		uint8_t classic_chat_target_id = 0;
		// Q5: 当前悄悄话对象名（下拉选「悄悄话」→ UIWhisperTargetDlg 确认后回填；
		// 名字菜单「密语」也会带出它）。send_to_target_channel 的 WHISPER 分支用它；
		// 为空时不发送、先弹对象层。切换其它频道不清（原生同样保留最后一次对象）。
		std::string whisper_target_name;
		// Q5: 密语正文的「待回显」暂存 —— s9 WhisperHandler 只回 WhisperResult
		// 标志（不回正文），客户端在 ok=true 时才把正文回显成 "(To X) 我 : 正文"
		// （对齐原生），失败时不出现正文（服务端给红色提示）。
		std::string pending_whisper_target;
		std::string pending_whisper_text;

		// 目标列表（点开 chatTarget 框 / ▶ 箭头后向上展开）。
		// 原生 v83 的展开态截图本包没有，所以框体沿用本客户端已定版的下拉观感
		// （1px 黑沿 + 白内区 + 悬浮橄榄色，与 UIWhisperTargetDlg 同一套）；
		// 位置只能向上 —— 底栏贴屏幕下沿，向下没有空间。
		// Q5: 频道下拉补全。原生 v83 的 chatTarget 列表就是全频道选择器；本端
		// 此前只有 4 项，且发送完全不读它（只认斜杠命令）。扩到 8 项后，
		// input_text_enter_callback 按 classic_chat_target_id 路由，斜杠命令保留为别名。
		// 顺序与斜杠命令别名一致：/w /p /g /a /b /s；喇叭无斜杠命令，走道具流程。
		enum ChatTargetIndex : uint8_t
		{
			TARGET_ALL,
			TARGET_WHISPER,
			TARGET_PARTY,
			TARGET_GUILD,
			TARGET_BUDDY,
			TARGET_ALLIANCE,
			TARGET_SPOUSE,
			TARGET_MEGAPHONE,
			TARGET_COUNT
		};

		static constexpr int16_t TARGET_LIST_ROW_H = 15;
		static constexpr int16_t TARGET_LIST_W = CHAT_TARGET_W;

		bool classic_target_list_open = false;
		size_t classic_target_hover = 0;
		std::vector<Text> classic_target_list_texts;
		// 列表高度随项数变，而 draw() 是 const ⇒ 只有 setheight 的那两块 mutable。
		mutable ColorBox classic_target_list_back;
		mutable ColorBox classic_target_list_inner;
		ColorBox classic_target_list_mark;

		void set_classic_target(uint8_t target_id);
		// 目标项文案（chat.target.* 八键，Q5 补全）；列表与当前标签共用同一处取值。
		std::string chat_target_name(uint8_t target_id) const;
		void toggle_classic_target_list(bool open);
		void pick_classic_target(size_t index);
		bool in_classic_target_row(Point<int16_t> cursor_position, size_t row) const;
		// 列表左上角（屏幕坐标）：贴在 chatTarget 框顶上方。
		Point<int16_t> classic_target_list_position() const;

		// Q5: 按当前选中频道 classic_chat_target_id 路由非命令文本（enter_callback）。
		// 返回三态：CHANNEL_SENT 已发送并处理回显；CHANNEL_HELD 就绪度不足
		// （密语没对象 → 已弹对象层；喇叭 → 已开窗），文本留在输入行、不发不清；
		// CHANNEL_FALLBACK 该频道等价于「对所有人」，调用方继续走 GeneralChat。
		enum ChannelSendOutcome : uint8_t
		{
			CHANNEL_SENT,
			CHANNEL_HELD,
			CHANNEL_FALLBACK
		};
		uint8_t send_to_target_channel(const std::string& text);
		// Q5: 本地回显自己发出的消息。是否回显按 s9 服务端回显矩阵定死：
		//   Party/Guild/Alliance/Buddy：服务端广播显式排除发送者 → 必须本地回显；
		//   General/Spouse/Megaphone：服务端会把包发回自己 → 禁止本地回显（否则双份）；
		//   Whisper：服务端只回 WhisperResult 标志 → 成功即回显（原生行为一致）。
		// prefix_in 为空时按 ch 自动补 "[Party] " 等同构前缀（与收包端一致）。
		void echo_own_message(const std::string& text, ChatChannel ch, const std::string& prefix_in = std::string());
		// Q5: 好友频道收件人 = UIBuddyList 中在线好友的 cid（详见 cpp 注释：
		// 名单只来自 updateBuddylist 推送，窗没开过就是空表，等价原生静默失败）。
		std::list<int32_t> collect_online_buddy_cids() const;
		// Q5: 发一条悄悄话并暂存正文 —— WhisperResult 成功时才回显
		// 「(To X) 自己 : 正文」（失败服务端会给红色提示，这里不出正文，
		// 避免原生没有的「发了但对方不在」的白字）。
		void send_whisper(const std::string& target, const std::string& text);
		// Q0: 发送后保持输入行开启并聚焦（清空文本 + 复位历史索引）。
		// 斜杠命令分支原先各自 close_input()，与 Q0 连续输入口径冲突，统一走这里。
		void keep_input_focus();
		void set_classic_log(bool open);
		// 三个贴图按钮（▶ 箭头 / + / −）的可见性只有一个判据：folded = 历史框关
		// 且输入未激活。toggle_input 与 set_classic_log 两条状态迁移都要重算，
		// 所以抽成一处，别在两个调用点各写一遍。
		void sync_classic_buttons();
		// 展开态历史框的几何唯一真源：底边贴白带顶，高 = user_view_y。
		Point<int16_t> classic_log_position() const;
		// 滚动条列的窗口 x（原生 551..564，即贴历史框右沿往内一列）。
		int16_t classic_scroll_x() const;
		// 当前高度下能整行显示几行正文（与 draw() 的循环判据同一套算式）。
		int16_t classic_visible_lines() const;
		// 高度/行数变了都要重算：Slider 的轨道范围、行数和可见性都存在构造时的
		// 局部量里，不重算就会出现「框拖高了但滚动条还按旧高度命中」。
		void sync_classic_scrollbar();
		// 拖顶边改高度（夹到 [MIN, MAX]）并同步滚动条。
		void set_classic_log_height(int16_t height);
		// 顶边拖拽命中区：整条上沿，但不含滚动条列（那一列优先给滑块）。
		bool classic_in_grip(Point<int16_t> cursor_position) const;

		// Q2: 聊天名字点击弹出的小菜单（密语 / 查看角色信息 / 加好友）。
		// 作为聊天栏自管弹层（不开新 UIElement 类型）：绘制与命中都在本类内完成。
		enum NameMenuItem : uint8_t
		{
			NM_WHISPER,
			NM_CHARINFO,
			NM_BUDDY,
			NM_COUNT
		};
		static constexpr int16_t NAME_MENU_W = 132;
		static constexpr int16_t NAME_MENU_ROW_H = 20;
		bool name_menu_open = false;
		std::string name_menu_target;
		Point<int16_t> name_menu_pos; // 屏幕坐标（锚点）
		mutable int16_t name_menu_hover = -1;
		std::array<Text, NM_COUNT> name_menu_items;
		Rectangle<int16_t> name_menu_rect(size_t i) const;
		void draw_name_menu() const;
		void open_name_menu(const std::string& name, Point<int16_t> screen_pos);
		void close_name_menu();
		void name_menu_select(size_t i);

#if LOG_LEVEL >= LOG_UI
		ColorBox dimension_box;
		ColorBox dragarea_box;
		ColorBox top_box;
		ColorBox bottom_box;
		ColorBox left_box;
		ColorBox right_box;
		ColorBox input_box;
#endif
	};
}