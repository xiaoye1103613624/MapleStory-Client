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
#include "UIChatBar.h"
#include "UIChat.h"
#include "UIBuddyList.h"
#include "UICharInfo.h"
#include "UIItemInventory.h"
#include "UISoftKey.h"
#include "GMCommandList.h"
// 悄悄话目标弹层 UIWhisperTargetDlg（它落在已编译的 UIMiscShells TU 里 —— 本工程
// 的 vcxproj 逐文件列源，新增 .cpp 必须改工程文件）。
#include "UIMiscShells.h"

#include "../UI.h"

#include "../Components/MapleButton.h"

#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/InventoryPackets.h"
#include "../../Net/Packets/MessagingPackets.h"
#include "../../Net/Packets/MiscOutPackets.h"
#include "../../Net/Packets/PlayerPackets.h"
#include "../../Net/Packets/PlayerInteractionPackets.h" // CharInfoRequestPacket（名字菜单「查看角色信息」）
#include "../../Net/Packets/BuddyPackets.h"             // BuddyAddPacket（名字菜单「加好友」）
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Locale.h"
#include "../../Util/Misc.h"

#include <list>
#include <algorithm>
#include <cctype>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIChatBar::UIChatBar() : temp_view_x(0), temp_view_y(0), drag_direction(DragDirection::NONE), view_input(false), view_adjusted(false), position_adjusted(false), classic_mode(false)
	{
		classic_mode = UiAssetResolve::is_classic_hud();

		if (classic_mode)
		{
			LOG(LOG_DEBUG, "UIChatBar: classic StatusBar.img chat input");

			nl::node base = UiAssetResolve::status_bar_root()["base"];
			classic_input_bg = Texture(base["chat"]);

			if (base["chatTarget"])
				classic_chat_target = Texture(base["chatTarget"]);

			int16_t vh = Constants::Constants::get().get_viewheight();
			int16_t vw = Constants::Constants::get().get_viewwidth();

			user_view_x = 560;
			// 历史框高度跟 modern 一样落在 chat.y 里（原生就是「拖完记住」）；
			// 老存档没写过这一项，默认串是 "0"，越界一律回落到默认 80。
			if (const int16_t saved_h = Setting<ChatViewY>::get().load();
				saved_h >= CLASSIC_LOG_H_MIN && saved_h <= CLASSIC_LOG_H_MAX)
			{
				user_view_y = saved_h;
			}
			else
			{
				user_view_y = CLASSIC_LOG_H_DEFAULT;
			}

			// D7：聊天类型（原生 SetChatType 三态）持久化恢复。
			if (const int8_t saved_type = static_cast<int8_t>(Setting<ChatType>::get().load());
				saved_type >= 1 && saved_type <= 3)
				chat_type = saved_type;
			if (chat_type == 1)
				classic_log_open = false;          // 收起：仅输入行
			else if (chat_type == 2)
			{
				classic_log_open = true;           // 紧凑：小历史框
				user_view_y = CLASSIC_LOG_H_COMPACT;
			}
			// chat_type == 3：保持 classic_log_open=true 与已算高度（完整）
			min_x = 560;
			max_x = 560;
			top_y = 8;
			center_y = 12;
			bottom_y = 8;
			btMin_x = 0;
			input_bg_x = classic_input_bg.is_valid() ? classic_input_bg.width() : 560;
			input_bg_y = classic_input_bg.is_valid() ? classic_input_bg.height() : 5;
			input_max_x = input_bg_x;
			input_origin_x = 0;
			input_origin_y = 0;
			input_btns_x = 0;
			// 输入行 = 底栏 backgrnd 自身烘焙的那条白输入带（带顶 = 底栏顶+4、
			// 带内高 26），不是底栏上方另起的一行。定版依据见 UIChatBar.h 常量注释。
			position = Point<int16_t>(0, static_cast<int16_t>(vh - CLASSIC_BAR_H + INPUT_BAND_TOP));
			dimension = Point<int16_t>(std::min(vw, MAX_ROW_W), INPUT_ROW_H);
			// 收起态灰带：颜色取原生截帧内区实测平涂值 (119,119,119)，不透明；再补一条
			// 原生实测 (87,87,95) 的 1px 内沿暗线（evidence/w4-chatbar/coords-fold.md）。
			// 宽度不是整行：原生收起态截帧（566 宽）里灰带只铺到 x=550，x 551..565 是
			// 那一列滚动条（实测 x551=173 灰轨 + 552..564 蓝色箭头）。所以灰带 =
			// 行宽 - 滚动条列宽，右侧那一列留给滚动条，不能一起涂灰。
			const int16_t fold_w = static_cast<int16_t>(dimension.x() - SCROLL_W);

			classic_fold_panel = ColorBox(fold_w, FOLD_PANEL_H, Color(0x777777FF), 1.0f);
			classic_fold_edge = ColorBox(fold_w, 1, Color(0x57575FFF), 1.0f);
			min_view_y = static_cast<int16_t>(position.y() - user_view_y);
			view_max = true;
			// 原生进图时聊天栏就是「收起态」：灰带 + 最近一条消息，白色输入带要
			// 按 Enter 才出现（UIStateGame 的 TOGGLECHAT 分支已负责开框）。这里
			// 若留 true，进图永远顶着一条白色输入带 —— 就是用户报的「聊天收起
			// 不是应该在白色框区域嘛」。
			view_input = false;
			dragarea = Point<int16_t>(dimension.x(), 12);

			// 展开态右端竖滚动条（B5-1）。坐标约定跟 UIItemInventory 一样：vertical 和
			// x 都是「相对历史框左上角」的局部量，draw(log_pos) / send_cursor(c - log_pos)
			// 时才加窗口偏移；上下各内缩 SCROLL_INSET 是原生实测（轨道两端到框沿 11~12px，
			// 证据 evidence/w4-chatbar/coords-expand.md），不留就会盖住白带上沿。
			classic_scrollbar = Slider(
				Slider::Type::CHATBAR,
				Range<int16_t>(SCROLL_INSET, user_view_y - SCROLL_INSET),
				classic_scroll_x(),
				classic_visible_lines(),
				static_cast<int16_t>(message_history.size()),
				[&](bool upwards)
				{
					// Slider 的 row 是「滑块在轨道上的行位」，顶部=0；聊天正文是倒着排的
					// （最新在最下），所以 offset 与 row 反相：往上翻一格 = 多看一条旧的。
					classic_scroll_offset = static_cast<int16_t>(
						std::max<int16_t>(0, classic_scroll_offset + (upwards ? 1 : -1)));
				}
			);
			classic_scrollbar.setenabled(false);

			// 输入正文颜色：白带上必须是深色，原先跟着历史消息用 WHITE，打出来的
			// 字直接隐进白带（真机抓帧实测：带内除那条 1px 暗线外零墨迹，但发送
			// 是成功的）。本包所有浅底 Textfield（UIMessenger / UISoftKey /
			// UIMapSearchPanel）一律用 BLACK，这里对齐。
			input_text = Textfield(
				Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK,
				Rectangle<int16_t>(
					Point<int16_t>(position.x() + CHAT_TARGET_W + 2, position.y() + 6),
					Point<int16_t>(position.x() + dimension.x() - 34, position.y() + INPUT_TEXT_HEIGHT)
				),
				70, 11
			);

			// 字形顶部比 Textfield 的 bounds 顶低约 10px（FreeType 基线），本行只有
			// 24px 且下沿就是那条 5px 下划线，不上移的话打出来的字整个压在状态面板上。
			input_text.set_text_shift(Point<int16_t>(0, -9));

			// 聊天输入框接收中文：启用系统 IME（SDL_EVENT_TEXT_INPUT 通路），UTF-8 直接进
			// 字图缓存（思源宋体含 CJK）。不启用时 Textfield::add_string 会丢弃全部 >=128
			// 的字节，中文一个字都进不来。注意 limit=70 是字节上限（belowlimit 用 text.size()），
			// 中文 3 字节/字故实际约 23 字；s9 GeneralChatHandler 按字符数限 127，
			// 放宽上限属行为变更，本轮按「不改 happy-path」保留 70 并记台账。
			input_text.set_accept_unicode(true);

			input_text.set_enter_callback(
				[&](std::string message)
				{
					input_text_enter_callback(message);
				}
			);

			input_text.set_key_callback(
				KeyAction::Id::ESCAPE,
				[&]()
				{
					input_text_escape_callback();
				}
			);

			show_message("[Welcome] Welcome to MapleStory!!", MessageType::YELLOW);

			// 目标对象行：base/chatTarget(81x20) 作底（draw() 里画在行内 (0,4)），
			// BtWhisper(12x19) 的 ▶ 箭头压在框左沿 —— 参考端截帧里框左沿露出的那截
			// 黑箭头就是它，两者同顶。
			// 展开/缩放：Basic.img/BtMax(+) 与 BtMin(-) 同位互斥（本包无 chat/ingame，
			// 那批钮只在 modern 分支存在，classic 必须走本包根）。
			nl::node sbroot = UiAssetResolve::status_bar_root();
			nl::node basic = nl::nx::UI["Basic.img"];

			try_maple_btn(buttons, Buttons::BtWhisper, sbroot["BtWhisper"],
				Point<int16_t>(0, TARGET_BOX_Y),
				"chatbar.classic.BtWhisper");

			const Point<int16_t> log_btn_pos(LOG_BT_X, LOG_BT_Y);

			try_maple_btn(buttons, Buttons::BtMax, basic["BtMax"], log_btn_pos, "chatbar.classic.BtMax");
			try_maple_btn(buttons, Buttons::BtMin, basic["BtMin"], log_btn_pos, "chatbar.classic.BtMin");

			classic_target_text = Text(
				Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
				Locale::text("chat.target.all")
			);

			// 目标列表：八项文案都走 Locale（非位图文案一律 i18n，用户 2026-11-01 定调）。
			for (uint8_t i = 0; i < ChatTargetIndex::TARGET_COUNT; ++i)
				classic_target_list_texts.emplace_back(Text::Font::A11M,
					Text::Alignment::LEFT, Color::Name::WHITE, chat_target_name(i));

			classic_target_list_back = ColorBox(TARGET_LIST_W, TARGET_LIST_ROW_H, Color(153.0f, 153.0f, 153.0f, 255.0f), 1.0f);
			classic_target_list_inner = ColorBox(TARGET_LIST_W - 2, TARGET_LIST_ROW_H - 2, Color(51.0f, 85.0f, 119.0f, 255.0f), 1.0f);
			// 原生选中条 = 亮蓝 (85,153,187)，整行贯通内区、高 15（行距 16 的帧
			// 「聊天选择对话的类型.png」实测，见 coords-targetlist.md）。
			classic_target_list_mark = ColorBox(TARGET_LIST_W - 2, TARGET_LIST_ROW_H - 1, Color(85.0f, 153.0f, 187.0f, 255.0f), 1.0f);

			set_classic_log(classic_log_open);

			return;
		}

		nl::node ingame = UiAssetResolve::status_bar_root()["chat"]["ingame"];
		nl::node input = ingame["input"];

		// 本包（v83 BeiDou）只有 classic StatusBar.img，没有 chat/ingame：走到这里
		// 说明 is_classic_hud() 判假了，下面这批贴图/按钮会全部空掉，必须留痕。
		if (!UiAssetResolve::usable(ingame))
			UiAssetResolve::diag_once("chatbar.modern.chat_ingame",
				"StatusBar.img/chat/ingame missing -> modern chat view/input textures and BtMax/BtMin/BtChat are empty");

		nl::node view = ingame["view"];
		nl::node max = view["max"];
		nl::node min = view["min"];

		drag = view["drag"];

		max_textures.emplace_back(max["top"]);
		max_textures.emplace_back(max["center"]);
		max_textures.emplace_back(max["bottom"]);

		min_textures.emplace_back(min["top"]);
		min_textures.emplace_back(min["center"]);
		min_textures.emplace_back(min["bottom"]);

		min_x = min_textures[0].width();
		max_x = max_textures[0].width();
		top_y = min_textures[0].height();
		center_y = min_textures[1].height();
		bottom_y = min_textures[2].height();
		btMin_x = Texture(view["btMin"]["normal"]["0"]).width();

		input_textures.emplace_back(input["layer:backgrnd"]);
		input_textures.emplace_back(input["layer:chatEnter"]);

		input_bg_x = input_textures[0].width();
		input_bg_y = input_textures[0].height();
		input_max_x = input_textures[1].width();

		Point<int16_t> input_origin = input_textures[1].get_origin().abs();
		input_origin_x = input_origin.x();
		input_origin_y = input_origin.y();

		min_view_y = Constants::Constants::get().get_viewheight() - input_bg_y;
		user_view_x = Setting<ChatViewX>::get().load();

		if (user_view_x == 0)
			user_view_x = min_x;

		user_view_y = Setting<ChatViewY>::get().load();

		if (user_view_y == 0)
		{
			// 20 pixels for the extra height by default
			user_view_y = top_y + center_y + bottom_y + 20;
		}

		int16_t btMax_x = Texture(view["btMax"]["normal"]["0"]).width();

		// Five pixels left and seven pixels up for padding
		buttons[Buttons::BtMax] = std::make_unique<MapleButton>(view["btMax"], Point<int16_t>(min_x - btMax_x - 5, -7));
		buttons[Buttons::BtMin] = std::make_unique<MapleButton>(view["btMin"]);

		Point<int16_t> input_btns_pos = Point<int16_t>(input_max_x - (input_bg_x - user_view_x) + input_origin_x - 17, 15 + input_origin_y + 1);
		int16_t input_btns_padding = 3;
		input_btns_x = Texture(input["button:chat"]["normal"]["0"]).width() + input_btns_padding;

		buttons[Buttons::BtChat] = std::make_unique<MapleButton>(input["button:chat"], input_btns_pos + Point<int16_t>(input_btns_x * 0, 0));
		buttons[Buttons::BtItemLink] = std::make_unique<MapleButton>(input["button:itemLink"], input_btns_pos + Point<int16_t>(input_btns_x * 1, 0));
		buttons[Buttons::BtChatEmoticon] = std::make_unique<MapleButton>(input["button:chatEmoticon"], input_btns_pos + Point<int16_t>(input_btns_x * 2, 0));
		buttons[Buttons::BtHelp] = std::make_unique<MapleButton>(input["button:help"], input_btns_pos + Point<int16_t>(input_btns_x * 3, 0));
		buttons[Buttons::BtOutChat] = std::make_unique<MapleButton>(input["button:outChat"], input_btns_pos + Point<int16_t>(input_btns_x * 4, 0));

		buttons[Buttons::BtChat]->set_active(false);
		buttons[Buttons::BtItemLink]->set_active(false);
		buttons[Buttons::BtChatEmoticon]->set_active(false);
		buttons[Buttons::BtHelp]->set_active(false);
		buttons[Buttons::BtOutChat]->set_active(false);

		int16_t input_text_limit = 70;
		int16_t input_text_marker_height = 11;

		input_text = Textfield(
			Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
			Rectangle<int16_t>(get_input_text_position(), get_input_text_position() + Point<int16_t>(283, INPUT_TEXT_HEIGHT)),
			input_text_limit, input_text_marker_height
		);

		// 同 classic 分支：启用系统 IME，否则中文被 >=128 过滤丢弃（见上处注释）。
		input_text.set_accept_unicode(true);

		input_text.set_enter_callback(
			[&](std::string message)
			{
				input_text_enter_callback(message);
			}
		);

		input_text.set_key_callback(
			KeyAction::Id::ESCAPE,
			[&]()
			{
				input_text_escape_callback();
			}
		);

		input_text.set_key_callback(
			KeyAction::Id::UP,
			[&]()
			{
				change_message(true);
			}
		);

		input_text.set_key_callback(
			KeyAction::Id::DOWN,
			[&]()
			{
				change_message(false);
			}
		);

		dragarea = drag.get_dimensions();

		toggle_view(Setting<ChatViewMax>::get().load(), false);

		// 进图默认就是收起态，箭头按钮必须跟着收 —— 构造函数里 try_maple_btn 建的
		// 按钮默认 active，不重算会压在灰带正文左沿上。
		if (classic_mode)
			sync_classic_buttons();

#if LOG_LEVEL >= LOG_UI
		dragarea_box = ColorBox(dragarea.x(), dragarea.y(), Color::Name::BLUE, 0.5f);
		input_box = ColorBox(input_bg_x, input_bg_y, Color::Name::RED, 0.5f);
#endif

		show_message("[Welcome] Welcome to MapleStory!!", MessageType::YELLOW);
	}

	void UIChatBar::draw(float inter) const
	{
		if (classic_mode)
		{
			Point<int16_t> input_pos = position;

			// 收起态 = 历史框关 且 输入未激活（发完消息 / 按 ESC 之后）。原生这时
			// 不显示白色输入带，而是把带内区铺成一条不透明灰带（顶 = position+3、
			// 高 23，上方再压一条 (87,87,95) 的 1px 内沿暗线），只放最近一条消息。
			const bool folded = !classic_log_open && !view_input;

			if (folded)
			{
				classic_fold_panel.draw(DrawArgument(input_pos + Point<int16_t>(0, FOLD_PANEL_TOP)));
				classic_fold_edge.draw(DrawArgument(input_pos + Point<int16_t>(0, FOLD_EDGE_TOP)));

				if (!message_history.empty())
				{
					const Text& latest = message_history.back().text;

					latest.draw(DrawArgument(input_pos
						+ Point<int16_t>(FOLD_TEXT_X, latest.y_for_ink_top(FOLD_TEXT_INK_TOP))));
				}

				if (classic_input_bg)
					classic_input_bg.draw(
						DrawArgument(input_pos + Point<int16_t>(0, UNDERLINE_Y), Point<int16_t>(dimension.x(), 0))
					);

				if (name_menu_open)
					draw_name_menu();
				UIElement::draw(inter);
				return;
				}

			// 聊天历史框（展开时，输入行上方）：半透明压暗底板（用户 2026-11-07 裁决有透明度）。
			// 实测「聊天框展开时候.png」col y0:场景 (51,51,17)→(24,24,8)、(102,102,34)→(48,40,16)
			// 三通道比一致≈0.47~0.53 ⇒ 黑色 α≈0.5 平涂，无边；宽=566 全行，高=user_view_y。
			// **先画底板**，再画消息/滚动条；目标列表在底板之上（draw() 里稍后画）。
			if (classic_log_open)
			{
				// 底边与白带顶齐平：min_view_y == position.y - user_view_y，
				// 框就是 input_pos - (0, user_view_y) 起、高 user_view_y。
				Point<int16_t> log_pos = classic_log_position();

				ColorBox(566, user_view_y, Color(0, 0, 0, 0.5f), 1.0f)
					.draw(DrawArgument(log_pos));

				// 右端竖滚动条：原生那列灰轨 x 551..564、行 4..70（历史框内），
				// 拖拽改高度后由 sync_classic_scrollbar 同步轨道长度与可见性。
				classic_scrollbar.draw(log_pos);
			}

			// 输入行（26px 白带）：聊天目标对话框（对谁聊天）+ 带下细条 + 输入文本。
			if (classic_chat_target)
			{
				classic_chat_target.draw(input_pos + Point<int16_t>(0, TARGET_BOX_Y));
			}
			else
			{
				// R6：base/chatTarget 缺失时仍要保证下面的 WHITE 目标文字可读。
				// 底栏 backgrnd 自带白色输入带，不铺深色框就是白字压白底（用户报的
				// 「聊天类型不显示 / 聊天 UI 变白底」）。深色取原生 chatTarget 内区色。
				ColorBox(CHAT_TARGET_W, CHAT_TARGET_H, Color(0x1B3159FF), 1.0f)
					.draw(input_pos + Point<int16_t>(0, TARGET_BOX_Y));
			}

			// 目标文字：行内墨迹顶 TARGET_TEXT_INK_TOP，绘制点按 classic_target_text
			// 当前内容（密语对象名，可能是中文也可能是拉丁）自己的度量反解
			// （R27-B1b 唯一真源，定版模型见 evidence/w0-common/text-metrics.md）。
			// 参考端是左对齐紧跟箭头右侧（框内 x≈6），不居中。
			int16_t target_x = TARGET_TEXT_X;
			// 密语对象名可能长过框宽：整体左移贴框左沿，不溢出到输入区。
			if (const int16_t target_w = classic_target_text.width(); target_w > CHAT_TARGET_W - TARGET_TEXT_X)
				target_x = std::max<int16_t>(0, CHAT_TARGET_W - target_w);

			classic_target_text.draw(
				DrawArgument(input_pos + Point<int16_t>(target_x, classic_target_text.y_for_ink_top(TARGET_TEXT_INK_TOP)))
			);

			// 目标列表（点开框/箭头后向上展开）：后画，压在历史框底板与历史消息之上。
			// 样式 = 原生截帧「聊天选择对话的类型.png」实测：1px 灰沿 (153,153,153) +
			// 蓝内区 (51,85,119)，行距 16；选中条整行贯通亮蓝 (85,153,187)、高 15，
			// 正文白字左沿 = 面板左 + 4，墨迹顶 = 面板顶 + 2。hover 初始化为已选中的行。
			if (classic_target_list_open)
			{
				const Point<int16_t> list_pos = classic_target_list_position();
				const int16_t list_h = static_cast<int16_t>(classic_target_list_texts.size() * TARGET_LIST_ROW_H);

				classic_target_list_back.setheight(list_h);
				classic_target_list_inner.setheight(list_h - 2);
				classic_target_list_mark.setheight(TARGET_LIST_ROW_H - 1); // 选中条高 15（row16）

				classic_target_list_back.draw(list_pos);
				classic_target_list_inner.draw(list_pos + Point<int16_t>(1, 1));

				if (classic_target_hover < classic_target_list_texts.size())
					classic_target_list_mark.draw(list_pos + Point<int16_t>(1, 1
						+ static_cast<int16_t>(classic_target_hover * TARGET_LIST_ROW_H)));

				for (size_t i = 0; i < classic_target_list_texts.size(); ++i)
					classic_target_list_texts[i].draw(list_pos
						+ Point<int16_t>(4, static_cast<int16_t>(i * TARGET_LIST_ROW_H) + 2));
			}

			if (classic_input_bg)
				classic_input_bg.draw(
					DrawArgument(input_pos + Point<int16_t>(0, UNDERLINE_Y), Point<int16_t>(dimension.x(), 0))
				);

			// 历史框展开/缩放钮（输入行右端）：Basic.img/BtMax(+) 收起态 / BtMin(-)
			// 展开态，同位互斥，由 set_classic_log 切 active，UIElement::draw 画。

			// 历史消息：自输入行上方自下而上排布（最新在最下）。首行留 23px =
			// 字形带（请求 y 之下约 10px）贴住输入行顶；上限按历史框顶算，
			// 框现在是 input_pos.y - user_view_y 起、高 user_view_y。
			// 滚动条向上翻时从 classic_scroll_offset 起算（0 = 贴最新一条）。
			int16_t message_y = LOG_FIRST_Y;

			const int first_visible = static_cast<int>(message_history.size()) - 1 - classic_scroll_offset;

			for (int i = first_visible; i >= 0; i--)
			{
				if (message_y > user_view_y + LOG_LINE_SLACK)
					break;

				message_history[i].text.draw(
					input_pos + Point<int16_t>(LOG_TEXT_X, -message_y));
				message_y += LOG_LINE_H;
			}

			// marker_adjust 与 ctor 的 set_text_shift(0,-9) 配套：文字带落在
			// input_pos.y+5..+16（蓝框可见区内、下划线上方），光标取同一条带。
			input_text.draw(Point<int16_t>(1, -2), Point<int16_t>(1, 8));
			if (name_menu_open)
				draw_name_menu();
			UIElement::draw(inter);
			return;
		}

		if (view_max)
		{
			Range<int16_t> vertical = Range<int16_t>(0, 0);

			max_textures[0].draw(position - Point<int16_t>(0, center_y + user_view_y), vertical, Range<int16_t>(0, max_x - user_view_x + 5));
			max_textures[0].draw(position - Point<int16_t>(max_x - user_view_x, center_y + user_view_y), vertical, Range<int16_t>(max_x - 5, 0));

			max_textures[1].draw(DrawArgument(position - Point<int16_t>(0, center_y + user_view_y), Point<int16_t>(0, user_view_y + 1)), vertical, Range<int16_t>(0, max_x - user_view_x + 2));
			max_textures[1].draw(DrawArgument(position - Point<int16_t>(max_x - user_view_x, center_y + user_view_y), Point<int16_t>(0, user_view_y + 1)), vertical, Range<int16_t>(max_x - 2, 0));

			max_textures[2].draw(position, vertical, Range<int16_t>(0, max_x - user_view_x + 5));
			max_textures[2].draw(position - Point<int16_t>(max_x - user_view_x, 0), vertical, Range<int16_t>(max_x - 5, 0));

			drag.draw(position - Point<int16_t>(0, top_y + center_y + user_view_y));

			int16_t message_y = 0;

			for (int i = message_history.size() - 1; i >= 0; i--)
			{
				if (message_y <= center_y + user_view_y)
				{
					message_history[i].text.draw(position - Point<int16_t>(drag.get_origin().x() - 2, 5) - Point<int16_t>(0, message_y));

					message_y += 13;
				}
			}

			if (view_input)
			{
				Point<int16_t> pos = get_input_position();

				input_textures[0].draw(pos, vertical, Range<int16_t>(0, input_bg_x - user_view_x + 5));
				input_textures[0].draw(pos - Point<int16_t>(input_bg_x - user_view_x, 0), vertical, Range<int16_t>(input_bg_x - 5, 0));

				input_textures[1].draw(pos, vertical, Range<int16_t>(0, input_max_x - (input_bg_x - user_view_x) - input_origin_x - 22 + 3));
				input_textures[1].draw(pos - Point<int16_t>(input_max_x - (input_bg_x - user_view_x) - input_origin_x - 22, 0), vertical, Range<int16_t>(input_max_x - 3, 0));

				input_text.draw(Point<int16_t>(1, -2), Point<int16_t>(1, -3));

#if LOG_LEVEL >= LOG_UI
				input_box.draw(pos);
#endif
			}

			if (dragged)
			{
				if (temp_view_y > 0 && temp_view_x == 0)
				{
					Point<int16_t> pos = position;

					if (drag_direction == DragDirection::DOWN)
						pos = temp_position;

					max_textures[0].draw(pos - Point<int16_t>(0, center_y + temp_view_y), vertical, Range<int16_t>(0, max_x - user_view_x + 5));
					max_textures[0].draw(pos - Point<int16_t>(max_x - user_view_x, center_y + temp_view_y), vertical, Range<int16_t>(max_x - 5, 0));

					max_textures[1].draw(DrawArgument(pos - Point<int16_t>(0, center_y + temp_view_y), Point<int16_t>(0, temp_view_y + 1)), vertical, Range<int16_t>(0, max_x - user_view_x + 2));
					max_textures[1].draw(DrawArgument(pos - Point<int16_t>(max_x - user_view_x, center_y + temp_view_y), Point<int16_t>(0, temp_view_y + 1)), vertical, Range<int16_t>(max_x - 2, 0));

					max_textures[2].draw(pos, vertical, Range<int16_t>(0, max_x - user_view_x + 5));
					max_textures[2].draw(pos - Point<int16_t>(max_x - user_view_x, 0), vertical, Range<int16_t>(max_x - 5, 0));
				}
				else if (temp_view_y == 0 && temp_view_x > 0)
				{
					Point<int16_t> pos = position;

					if (drag_direction == DragDirection::LEFT)
						pos = temp_position;

					max_textures[0].draw(pos - Point<int16_t>(0, center_y + user_view_y), vertical, Range<int16_t>(0, max_x - temp_view_x + 5));
					max_textures[0].draw(pos - Point<int16_t>(max_x - temp_view_x, center_y + user_view_y), vertical, Range<int16_t>(max_x - 5, 0));

					max_textures[1].draw(DrawArgument(pos - Point<int16_t>(0, center_y + user_view_y), Point<int16_t>(0, user_view_y + 1)), vertical, Range<int16_t>(0, max_x - temp_view_x + 2));
					max_textures[1].draw(DrawArgument(pos - Point<int16_t>(max_x - temp_view_x, center_y + user_view_y), Point<int16_t>(0, user_view_y + 1)), vertical, Range<int16_t>(max_x - 2, 0));

					max_textures[2].draw(pos, vertical, Range<int16_t>(0, max_x - temp_view_x + 5));
					max_textures[2].draw(pos - Point<int16_t>(max_x - temp_view_x, 0), vertical, Range<int16_t>(max_x - 5, 0));
				}
				else if (temp_view_y > 0 && temp_view_x > 0)
				{
					Point<int16_t> pos = position;

					if (drag_direction == DragDirection::DOWN || drag_direction == DragDirection::LEFT || drag_direction == DragDirection::DOWNLEFT)
						pos = temp_position;

					max_textures[0].draw(pos - Point<int16_t>(0, center_y + temp_view_y), vertical, Range<int16_t>(0, max_x - temp_view_x + 5));
					max_textures[0].draw(pos - Point<int16_t>(max_x - temp_view_x, center_y + temp_view_y), vertical, Range<int16_t>(max_x - 5, 0));

					max_textures[1].draw(DrawArgument(pos - Point<int16_t>(0, center_y + temp_view_y), Point<int16_t>(0, temp_view_y + 1)), vertical, Range<int16_t>(0, max_x - temp_view_x + 2));
					max_textures[1].draw(DrawArgument(pos - Point<int16_t>(max_x - temp_view_x, center_y + temp_view_y), Point<int16_t>(0, temp_view_y + 1)), vertical, Range<int16_t>(max_x - 2, 0));

					max_textures[2].draw(pos, vertical, Range<int16_t>(0, max_x - temp_view_x + 5));
					max_textures[2].draw(pos - Point<int16_t>(max_x - temp_view_x, 0), vertical, Range<int16_t>(max_x - 5, 0));
				}
			}
		}
		else
		{
			min_textures[0].draw(position - Point<int16_t>(0, center_y));
			min_textures[1].draw(position - Point<int16_t>(0, center_y));
			min_textures[2].draw(position);

			drag.draw(position - Point<int16_t>(0, top_y + center_y));

			const size_t size = message_history.size();

			if (size > 0)
				message_history[size - 1].text.draw(position - Point<int16_t>(0, top_y + center_y) + Point<int16_t>(drag.get_origin().abs().x(), drag.height()) + Point<int16_t>(2, 7));
		}

		if (name_menu_open)
			draw_name_menu();
		UIElement::draw(inter);

#if LOG_LEVEL >= LOG_UI
		dimension_box.draw(get_position());
		dragarea_box.draw(get_dragarea_position());

		if (view_max)
		{
			top_box.draw(get_position());
			bottom_box.draw(get_position() + Point<int16_t>(0, dimension.y() - 3));
			left_box.draw(get_position());
			right_box.draw(get_position() + Point<int16_t>(dimension.x() - 3, 0));
		}
#endif
	}

	void UIChatBar::update()
	{
		if (classic_mode)
		{
			// 与构造函数里 Textfield 的初始矩形保持同一套算式：白带内区从
			// chatTarget 右沿(+2)起，右端留 34px 给展开钮与呼吸区。
			input_text.update(
				Point<int16_t>(position.x() + CHAT_TARGET_W + 2, position.y() + 6),
				Point<int16_t>(std::max<int16_t>(40, dimension.x() - CHAT_TARGET_W - 36), INPUT_TEXT_HEIGHT)
			);
			return;
		}

		input_text.update(get_input_text_position(), Point<int16_t>(input_max_x - (input_bg_x - user_view_x) - 22, INPUT_TEXT_HEIGHT));
	}

	Button::State UIChatBar::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case ms::UIChatBar::BtMax:
			{
				// classic 没有 max/min 视图，这对钮翻的是聊天历史框。
				// D7：原生 SetChatType 三态的「展开」侧：收起(1)→完整(3)；紧凑(2)→完整(3)。
				if (classic_mode)
				{
					if (chat_type == 2)
					{
						chat_type = 3;
						if (user_view_y <= CLASSIC_LOG_H_COMPACT)
							user_view_y = CLASSIC_LOG_H_DEFAULT;
					}
					set_classic_log(true);
				}
				else
					toggle_view(true, true);

				return Button::State::NORMAL;
			}
			case ms::UIChatBar::BtMin:
			{
				// D7：原生 SetChatType 三态的「收起」侧：完整(3)→紧凑(2)→收起(1)。
				if (classic_mode)
				{
					if (chat_type == 3)
					{
						user_view_y = CLASSIC_LOG_H_COMPACT;
						chat_type = 2;
						set_classic_log(true);
					}
					else
					{
						set_classic_log(false);
					}
				}
				else
					toggle_view(false, true);

				return Button::State::NORMAL;
			}
			case ms::UIChatBar::BtChatEmoticon:
			{
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					try {
						int32_t emote = std::stoi(s);
						FaceExpressionPacket(emote).dispatch();
						LOG(LOG_DEBUG, "UIChatBar: FaceExpression id=" << emote);
					} catch (...) {}
				}, []() {}, "Emote id");
				return Button::State::NORMAL;
			}
			case ms::UIChatBar::BtChat:
			{
				UI::get().emplace<UISoftKey>([](const std::string& msg) {
					if (msg.empty()) return;
					SpouseChatPacket(msg).dispatch();
					LOG(LOG_DEBUG, "UIChatBar: SpouseChat len=" << msg.size());
				}, []() {}, "Spouse chat");
				return Button::State::NORMAL;
			}
			case ms::UIChatBar::BtHelp:
			{
				// 全客户端「帮助/问号」统一走 ENTER_MTS，落点由服务端 use_mts 决定。
				EnterMtsPacket().dispatch();
				LOG(LOG_INFO, "UIChatBar: Help -> ENTER_MTS");
				return Button::State::NORMAL;
			}
			case ms::UIChatBar::BtWhisper:
			{
				// 原生是「点开列表、从列表选择」，不是点击循环（用户 2026-11-02 定调）。
				toggle_classic_target_list(!classic_target_list_open);
				return Button::State::NORMAL;
			}
			case ms::UIChatBar::BtOutChat:
			{
				auto chat = UI::get().get_element<UIChat>();
				if (!chat)
					chat = UI::get().emplace<UIChat>();
				else if (!chat->is_active())
					chat->makeactive();
				return Button::State::NORMAL;
			}
			case ms::UIChatBar::BtItemLink:
			{
				if (auto uii = UI::get().get_element<UIItemInventory>()) {
					int16_t slot = uii->get_selected_slot();
					auto tab = uii->get_tab();
					if (slot > 0 && (tab == InventoryType::Id::USE || tab == InventoryType::Id::ETC)) {
						int32_t itemid = Stage::get().get_player().get_inventory().get_item_id(tab, slot);
						if (itemid > 0) {
							UseItemPacket(slot, itemid).dispatch();
							LOG(LOG_DEBUG, "UIChatBar: ItemLink UseItem from inv id=" << itemid);
							return Button::State::NORMAL;
						}
					}
				}
				UI::get().emplace<UISoftKey>([](const std::string& tip) {
					if (tip.empty()) return;
					try {
						int32_t itemid = std::stoi(tip);
						if (itemid > 0) {
							const Inventory& inv = Stage::get().get_player().get_inventory();
							int16_t slot = inv.find_item(InventoryType::Id::USE, itemid);
							if (slot <= 0) slot = inv.find_item(InventoryType::Id::ETC, itemid);
							if (slot > 0) {
								UseItemPacket(slot, itemid).dispatch();
								LOG(LOG_DEBUG, "UIChatBar: ItemLink UseItem id=" << itemid);
								return;
							}
						}
					} catch (...) {}
					GeneralChatPacket("[item] " + tip, false).dispatch();
					LOG(LOG_DEBUG, "UIChatBar: ItemLink tip len=" << tip.size());
				}, []() {}, "itemid|link");
				return Button::State::NORMAL;
			}
			default:
			{
				return Button::State::DISABLED;
			}
		}
	}

	bool UIChatBar::is_in_range(Point<int16_t> cursor_position) const
	{
		// classic 命中区修正：基类 get_position() 会按 top_y+center_y+user_view_y 把
		// 矩形整体上移 100px，于是输入行（position..position+dimension）根本不在命中
		// 区内 —— UIStateGame::get_front 用 is_in_range 派发，聊天栏收不到光标，
		// 展开/缩放「点了没反应」的根因在此。classic 的 position 就是输入行左上角。
		// classic_resizing：拖顶边时光标会跑到框上方（甚至屏幕顶），必须继续归聊天栏，
		// 否则框架改派给场景、send_cursor 不再进来，拖到一半就断。
		if (classic_mode && (classic_resizing || (temp_view_y == 0 && temp_view_x == 0)))
		{
			if (Rectangle<int16_t>(position, position + dimension).contains(cursor_position))
				return true;

			// 目标列表是画在历史框「上方」的，天然落在聊天栏矩形之外。UIStateGame
			// 无论 hover 还是按下都用 is_in_range 选元素（IO/UIStateGame.cpp:564），
			// 不在区内就根本不会调 UIChatBar::send_cursor —— 真机实测「点了列表行
			// 没反应、列表不关也不选」就是这个门把点击漏给了场景。
			if (classic_target_list_open)
			{
				const Point<int16_t> list_lt = classic_target_list_position();
				const Point<int16_t> list_rb = list_lt + Point<int16_t>(TARGET_LIST_W, TARGET_COUNT * TARGET_LIST_ROW_H);

				if (Rectangle<int16_t>(list_lt, list_rb).contains(cursor_position))
					return true;
			}

			if (classic_log_open
				&& Rectangle<int16_t>(position - Point<int16_t>(0, user_view_y),
					position + Point<int16_t>(dimension.x(), 0)).contains(cursor_position))
				return true;
		}

		if (temp_view_y == 0 && temp_view_x == 0)
		{
			Rectangle<int16_t> bounds = Rectangle<int16_t>(get_position(), get_position() + dimension);
			Rectangle<int16_t> input_bounds = Rectangle<int16_t>(get_input_position(), get_input_position() + Point<int16_t>(user_view_x, input_bg_y));

			if (bounds.contains(cursor_position) || input_bounds.contains(cursor_position))
				return true;

			return false;
		}
		else
		{
			Rectangle<int16_t> bounds = Rectangle<int16_t>(
				Point<int16_t>(0, 0),
				Point<int16_t>(
					Constants::Constants::get().get_viewwidth(),
					Constants::Constants::get().get_viewheight()
				)
			);

			return bounds.contains(cursor_position);
		}
	}

	Cursor::State UIChatBar::send_cursor(bool clicked, Point<int16_t> cursor_position)
	{
		if (classic_mode)
		{
			// Q2: 名字点击菜单优先（它画在聊天栏之上）。
			if (name_menu_open)
			{
				Point<int16_t> sp = position + cursor_position; // 转屏幕坐标
				for (size_t i = 0; i < NM_COUNT; ++i)
				{
					if (name_menu_rect(i).contains(sp))
					{
						name_menu_hover = static_cast<int16_t>(i);
						if (clicked)
						{
							name_menu_select(i);
							return Cursor::State::CLICKING;
						}
						return Cursor::State::CANCLICK;
					}
				}
				name_menu_hover = -1;
				if (clicked)
					close_name_menu(); // 点菜单外：关闭并交下层
			}
			else if (classic_log_open)
			{
				// 名字热区：只测当前可见消息，避免滚动后旧坐标误触。
				int16_t message_y = LOG_FIRST_Y;
				const int first_visible = static_cast<int>(message_history.size()) - 1 - classic_scroll_offset;
				for (int i = first_visible; i >= 0; i--)
				{
					if (message_y > user_view_y + LOG_LINE_SLACK)
						break;
					const Message& m = message_history[i];
					if (m.clickable)
					{
						Rectangle<int16_t> nb(LOG_TEXT_X + m.name_x_off, -message_y, m.name_w, LOG_LINE_H);
						if (nb.contains(cursor_position))
						{
							if (clicked)
							{
								open_name_menu(m.speaker, position + cursor_position);
								return Cursor::State::CLICKING;
							}
							return Cursor::State::CANCLICK;
						}
					}
					message_y += LOG_LINE_H;
				}
			}
			else if (!classic_log_open && !view_input)
			{
				// Q3-①：收起态（历史框关 + 输入未激活）点灰带展开历史框，
				// 给出「未展开也能看历史」的入口；右侧按钮簇仍走各自 button_pressed。
				if (clicked && cursor_position.x() < dimension.x() - 80)
				{
					set_classic_log(true);
					return Cursor::State::CLICKING;
				}
				if (cursor_position.x() < dimension.x() - 80)
					return Cursor::State::CANCLICK;
				return Cursor::State::IDLE;
			}

			// B5-1：拖历史框顶边改高度（原生「按住上沿上下拖」），松手把高度写进
			// chat.y。拖拽中优先处理 —— 一旦进拖拽态，光标跑出台阶也得继续跟。
			//
			// 返回值不能是 CHATBARVDRAG：Cursor::set_state (IO/Cursor.cpp:137) 会把
			// 「没绑定素材的状态」一律降级成 IDLE，而 Cursor::init 在 classic 包里
			// 刻意不绑 kModernExtra（误绑会把数字字形画成光标）。状态一旦变 IDLE，
			// UIStateGame 的 clicked 判据（CLICKING / VSCROLLIDLE / dragged+拖动态）
			// 全落空，按住后的每个 motion 帧都被当成松手。真机追踪是铁证：
			//   grip start y=576 h0=80 → sc pt=300,570 clicked=0 → resize end h=80
			// 所以按住期间返回 CLICKING（kClassicStates 里必定加载），松手帧交回
			// IDLE —— 交回 CHATBARVDRAG 会被降成 IDLE，等于白说，但更重要的是
			// 松手帧绝不能留 CLICKING：那会让下一个 hover 帧被认成按住，贴着
			// 顶边再次触发 grip start。
			if (classic_resizing)
			{
				if (clicked)
				{
					const int16_t nh = static_cast<int16_t>(
						classic_resize_h0 + (classic_resize_y0 - cursor_position.y()));

					set_classic_log_height(nh);

					return Cursor::State::CLICKING;
				}

				classic_resizing = false;

				Setting<ChatViewY>::get().save(user_view_y);

				// 原生是「拖完立刻记住」，所以这里就落盘，不等退出。
				// 常规持久化路径是 ~Configuration → save()（Configuration.cpp:242），
				// 而真机取证发现退出那一次并没有把 chat.y 写出去（拖到 140、Settings
				// 里仍是 0），所以手势结束这一处显式存一次，保证「拖完记住」成立。
				Configuration::get().save();

				return Cursor::State::IDLE;
			}

			// 目标列表打开时优先判列表：它在历史框上方，不先判会被滚动条/拖拽带抢掉。
			if (classic_target_list_open)
			{
				for (size_t i = 0; i < classic_target_list_texts.size(); ++i)
				{
					if (!in_classic_target_row(cursor_position, i))
						continue;

					classic_target_hover = i;

					if (clicked)
					{
						pick_classic_target(i);
						classic_target_list_open = false;

						return Cursor::State::CLICKING;
					}

					return Cursor::State::CANCLICK;
				}

				// 点在列表外：关列表并吃掉这一帧的点击，否则同一次点击会接着落到
				// 下面的 chatTarget 命中区，把列表又开回去。
				if (clicked)
				{
					classic_target_list_open = false;

					return Cursor::State::CLICKING;
				}

				return Cursor::State::IDLE;
			}

			// 右端竖滚动条命中：Slider 的坐标是「相对历史框左上角」，所以先把光标
			// 换算到局部。列范围自己先卡一次 —— 它在轨道外也会返回非 IDLE（函数末尾
			// 那支无条件 VSCROLL），不卡就会把整块历史区的指针都变成滚动箭头。
			if (classic_log_open && classic_scrollbar.isenabled())
			{
				const Point<int16_t> log_pos = classic_log_position();
				const int16_t relx = cursor_position.x() - (log_pos.x() + classic_scroll_x());

				if (relx >= 0 && relx <= SCROLL_W)
					if (const Cursor::State sstate = classic_scrollbar.send_cursor(cursor_position - log_pos, clicked))
						return sstate;
			}

			// 顶边拖拽起手：整条上沿 GRIP_H 命中带（不含滚动条列，那列已经先判过）。
			// 同上方说明：按住期间只能交回 CLICKING 这种已加载状态，拖拽才不断。
			if (classic_log_open && clicked && classic_in_grip(cursor_position))
			{
				classic_resizing = true;
				classic_resize_h0 = user_view_y;
				classic_resize_y0 = cursor_position.y();

				return Cursor::State::CLICKING;
			}

			// 展开/缩放已改真按钮（Basic.img/BtMax|BtMin），手搓 12x12 命中区撤掉，
			// 否则它会抢在 UIElement::send_cursor 之前吞掉点击。
			// chatTarget 框体本身（箭头右侧）仍是贴图不是按钮，点击 = 展开目标列表。
			// 命中矩形与 draw() 用的 TARGET_BOX_Y 保持一致。
			Point<int16_t> target_pos = position + Point<int16_t>(TARGET_BT_W, TARGET_BOX_Y);

			if (Rectangle<int16_t>(target_pos, target_pos + Point<int16_t>(CHAT_TARGET_W - TARGET_BT_W, CHAT_TARGET_H)).contains(cursor_position))
			{
				if (clicked)
				{
					toggle_classic_target_list(!classic_target_list_open);
					return Cursor::State::CLICKING;
				}

				return Cursor::State::CANCLICK;
			}

			// classic 分支必须在这里收尾。下面的 modern 机器（八向拖拽缩放、intoprange/
			// inbottomrange 那组命中区）建立在 get_position() 上，而 classic 构造把
			// view_max 置了 true，get_position() 会再加 top_y+center_y+user_view_y 的
			// 偏移 —— 真机实测「拖历史框顶边」落进 inbottomrange，被当成整窗移动，底栏
			// 被拖高 80px（帧 05..07 灰带从 y656 跳到 y576）。
			if (view_input)
				if (Cursor::State new_state = input_text.send_cursor(cursor_position, clicked))
					return new_state;

			return UIElement::send_cursor(clicked, cursor_position);
		}

		if (view_input && temp_view_y == 0 && temp_view_x == 0)
			if (Cursor::State new_state = input_text.send_cursor(cursor_position, clicked))
				return new_state;

		if (clicked)
		{
			if (dragged)
			{
				if (temp_view_y == 0 && temp_view_x == 0)
				{
					Point<int16_t> new_pos = cursor_position - cursoroffset;
					int16_t new_pos_x = new_pos.x();
					int16_t new_pos_y = new_pos.y();

					if (new_pos_x < 0)
						new_pos.set_x(0);

					int16_t min_y = MIN_HEIGHT - 2 + drag.get_origin().y() * -1;

					if (view_input)
						min_y += user_view_y;

					if (new_pos_y < min_y)
						new_pos.set_y(min_y);

					int16_t max_x = Constants::Constants::get().get_viewwidth() - user_view_x;

					if (new_pos_x > max_x)
						new_pos.set_x(max_x);

					int16_t max_y = min_view_y;

					if (view_input)
						max_y -= input_bg_y;

					if (new_pos_y > max_y)
						new_pos.set_y(max_y);

					position = new_pos;

					return Cursor::State::CHATBARMOVE;
				}
				else
				{
					if (temp_view_x == 0)
					{
						if (drag_direction == DragDirection::DOWN)
						{
							// TODO: The top gets shifted by a pixel
							Point<int16_t> pos_y = cursor_position - position + Point<int16_t>(0, user_view_y) - Point<int16_t>(0, 13);
							Point<int16_t> pos = cursor_position - cursoroffset;

							if (pos_y.y() <= MIN_HEIGHT)
							{
								temp_view_y = MIN_HEIGHT;
								temp_position = position - Point<int16_t>(0, user_view_y) + Point<int16_t>(0, 13);
							}
							else if (pos_y.y() >= MAX_HEIGHT)
							{
								temp_view_y = MAX_HEIGHT;
								temp_position = position - Point<int16_t>(0, user_view_y) + Point<int16_t>(0, MAX_HEIGHT);
							}
							else
							{
								temp_view_y = pos_y.y();
								temp_position = Point<int16_t>(position.x(), pos.y());
							}
						}
						else
						{
							Point<int16_t> pos = position - cursor_position - Point<int16_t>(0, 13);

							if (pos.y() <= MIN_HEIGHT)
								temp_view_y = MIN_HEIGHT;
							else if (pos.y() >= MAX_HEIGHT)
								temp_view_y = MAX_HEIGHT;
							else
								temp_view_y = pos.y();
						}

#if LOG_LEVEL >= LOG_UI
						dimension = Point<int16_t>(user_view_x, top_y + center_y + bottom_y) + Point<int16_t>(0, temp_view_y);

						dimension_box = ColorBox(dimension.x(), dimension.y(), Color::Name::RED, 0.5f);
						left_box = ColorBox(3, dimension.y(), Color::Name::YELLOW, 0.5f);
						right_box = ColorBox(3, dimension.y(), Color::Name::YELLOW, 0.5f);
#endif

						return Cursor::State::CHATBARVDRAG;
					}
					else if (temp_view_y == 0)
					{
						if (drag_direction == DragDirection::LEFT)
						{
							// TODO: The right gets shifted by a pixel
							Point<int16_t> pos_x = position - cursor_position + Point<int16_t>(user_view_x, 0);
							Point<int16_t> pos = cursor_position - cursoroffset;

							if (pos_x.x() <= min_x)
							{
								temp_view_x = min_x;
								temp_position = position + Point<int16_t>(user_view_x, 0) - Point<int16_t>(min_x, 0);
							}
							else if (pos_x.x() >= max_x)
							{
								temp_view_x = max_x;
								temp_position = position - Point<int16_t>(max_x, 0) + Point<int16_t>(user_view_x, 0);
							}
							else
							{
								temp_view_x = pos_x.x();
								temp_position = Point<int16_t>(pos.x(), position.y());
							}
						}
						else
						{
							Point<int16_t> pos = cursor_position - position;

							if (pos.x() <= min_x)
								temp_view_x = min_x;
							else if (pos.x() >= max_x)
								temp_view_x = max_x;
							else
								temp_view_x = pos.x();
						}

#if LOG_LEVEL >= LOG_UI
						dimension = Point<int16_t>(temp_view_x, top_y + center_y + bottom_y) + Point<int16_t>(0, user_view_y);

						dimension_box = ColorBox(dimension.x(), dimension.y(), Color::Name::RED, 0.5f);
						top_box = ColorBox(dimension.x(), 3, Color::Name::GREEN, 0.5f);
						bottom_box = ColorBox(dimension.x(), 3, Color::Name::GREEN, 0.5f);
#endif

						return Cursor::State::CHATBARHDRAG;
					}
					else
					{
						if (drag_direction == DragDirection::DOWNLEFT)
						{
							int16_t temp_position_x, temp_position_y;

							Point<int16_t> pos = cursor_position - cursoroffset;

							// TODO: The right gets shifted by a pixel
							Point<int16_t> pos_x = position - cursor_position + Point<int16_t>(user_view_x, 0);

							if (pos_x.x() <= min_x)
							{
								temp_view_x = min_x;
								temp_position_x = position.x() + user_view_x - min_x;
							}
							else if (pos_x.x() >= max_x)
							{
								temp_view_x = max_x;
								temp_position_x = position.x() - max_x + user_view_x;
							}
							else
							{
								temp_view_x = pos_x.x();
								temp_position_x = pos.x();
							}

							// TODO: The top gets shifted by a pixel
							Point<int16_t> pos_y = cursor_position - position + Point<int16_t>(0, user_view_y) - Point<int16_t>(0, 13);

							if (pos_y.y() <= MIN_HEIGHT)
							{
								temp_view_y = MIN_HEIGHT;
								temp_position_y = position.y() - user_view_y + 13;
							}
							else if (pos_y.y() >= MAX_HEIGHT)
							{
								temp_view_y = MAX_HEIGHT;
								temp_position_y = position.y() - user_view_y + MAX_HEIGHT;
							}
							else
							{
								temp_view_y = pos_y.y();
								temp_position_y = pos.y();
							}

							temp_position = Point<int16_t>(temp_position_x, temp_position_y);
						}
						else
						{
							if (drag_direction == DragDirection::LEFT)
							{
								// TODO: The right gets shifted by a pixel
								Point<int16_t> pos_x = position - cursor_position + Point<int16_t>(user_view_x, 0);
								Point<int16_t> pos = cursor_position - cursoroffset;

								if (pos_x.x() <= min_x)
								{
									temp_view_x = min_x;
									temp_position = position + Point<int16_t>(user_view_x, 0) - Point<int16_t>(min_x, 0);
								}
								else if (pos_x.x() >= max_x)
								{
									temp_view_x = max_x;
									temp_position = position - Point<int16_t>(max_x, 0) + Point<int16_t>(user_view_x, 0);
								}
								else
								{
									temp_view_x = pos_x.x();
									temp_position = Point<int16_t>(pos.x(), position.y());
								}
							}
							else
							{
								Point<int16_t> pos_x = cursor_position - position;

								if (pos_x.x() <= min_x)
									temp_view_x = min_x;
								else if (pos_x.x() >= max_x)
									temp_view_x = max_x;
								else
									temp_view_x = pos_x.x();
							}

							if (drag_direction == DragDirection::DOWN)
							{
								// TODO: The top gets shifted by a pixel
								Point<int16_t> pos_y = cursor_position - position + Point<int16_t>(0, user_view_y) - Point<int16_t>(0, 13);
								Point<int16_t> pos = cursor_position - cursoroffset;

								if (pos_y.y() <= MIN_HEIGHT)
								{
									temp_view_y = MIN_HEIGHT;
									temp_position = position - Point<int16_t>(0, user_view_y) + Point<int16_t>(0, 13);
								}
								else if (pos_y.y() >= MAX_HEIGHT)
								{
									temp_view_y = MAX_HEIGHT;
									temp_position = position - Point<int16_t>(0, user_view_y) + Point<int16_t>(0, MAX_HEIGHT);
								}
								else
								{
									temp_view_y = pos_y.y();
									temp_position = Point<int16_t>(position.x(), pos.y());
								}
							}
							else
							{
								Point<int16_t> pos_y = position - cursor_position - Point<int16_t>(0, 13);

								if (pos_y.y() <= MIN_HEIGHT)
									temp_view_y = MIN_HEIGHT;
								else if (pos_y.y() >= MAX_HEIGHT)
									temp_view_y = MAX_HEIGHT;
								else
									temp_view_y = pos_y.y();
							}
						}

#if LOG_LEVEL >= LOG_UI
						dimension = Point<int16_t>(temp_view_x, top_y + center_y + bottom_y) + Point<int16_t>(0, temp_view_y);

						dimension_box = ColorBox(dimension.x(), dimension.y(), Color::Name::RED, 0.5f);
						top_box = ColorBox(dimension.x(), 3, Color::Name::GREEN, 0.5f);
						bottom_box = ColorBox(dimension.x(), 3, Color::Name::GREEN, 0.5f);
						left_box = ColorBox(3, dimension.y(), Color::Name::YELLOW, 0.5f);
						right_box = ColorBox(3, dimension.y(), Color::Name::YELLOW, 0.5f);
#endif

						if (drag_direction == DragDirection::DOWN || drag_direction == DragDirection::LEFT)
							return Cursor::State::CHATBARBRTLDRAG;
						else
							return Cursor::State::CHATBARBLTRDRAG;
					}
				}
			}
			else if (indragrange(cursor_position))
			{
				cursoroffset = cursor_position - position;
				dragged = true;

				return Cursor::State::CHATBARMOVE;
			}
			else if (view_max)
			{
				if (intoprange(cursor_position) && !inleftrange(cursor_position) && !inrightrange(cursor_position))
				{
					dragged = true;

					temp_view_y = user_view_y;

					return Cursor::State::CHATBARVDRAG;
				}
				else if (inrightrange(cursor_position) && !intoprange(cursor_position) && !inbottomrange(cursor_position))
				{
					dragged = true;

					temp_view_x = user_view_x;

					return Cursor::State::CHATBARHDRAG;
				}
				else if (intoprightrange(cursor_position))
				{
					dragged = true;

					temp_view_x = user_view_x;
					temp_view_y = user_view_y;

					return Cursor::State::CHATBARBLTRDRAG;
				}
				else if (inbottomrange(cursor_position) && !inleftrange(cursor_position) && !inrightrange(cursor_position))
				{
					cursoroffset = cursor_position - position;
					dragged = true;

					temp_view_y = user_view_y;
					temp_position = position;
					drag_direction = DragDirection::DOWN;

					return Cursor::State::CHATBARVDRAG;
				}
				else if (inbottomrightrange(cursor_position))
				{
					cursoroffset = cursor_position - position;
					dragged = true;

					temp_view_x = user_view_x;
					temp_view_y = user_view_y;
					temp_position = position;
					drag_direction = DragDirection::DOWN;

					return Cursor::State::CHATBARBRTLDRAG;
				}
				else if (inleftrange(cursor_position) && !intoprange(cursor_position) && !inbottomrange(cursor_position))
				{
					cursoroffset = cursor_position - position;
					dragged = true;

					temp_view_x = user_view_x;
					temp_position = position;
					drag_direction = DragDirection::LEFT;

					return Cursor::State::CHATBARHDRAG;
				}
				else if (intopleftrange(cursor_position))
				{
					cursoroffset = cursor_position - position;
					dragged = true;

					temp_view_x = user_view_x;
					temp_view_y = user_view_y;
					temp_position = position;
					drag_direction = DragDirection::LEFT;

					return Cursor::State::CHATBARBRTLDRAG;
				}
				else if (inbottomleftrange(cursor_position))
				{
					cursoroffset = cursor_position - position;
					dragged = true;

					temp_view_x = user_view_x;
					temp_view_y = user_view_y;
					temp_position = position;
					drag_direction = DragDirection::DOWNLEFT;

					return Cursor::State::CHATBARBLTRDRAG;
				}
			}
		}
		else
		{
			if (dragged)
			{
				if (temp_view_y == 0 && temp_view_x == 0)
				{
					dragged = false;

					Setting<PosCHAT>::get().save(position);

					return Cursor::State::CHATBARMOVE;
				}
				else
				{
					if (temp_view_x == 0)
					{
						user_view_y = temp_view_y;
						temp_view_y = 0;
						dragged = false;

						update_view(false);

						Setting<ChatViewY>::get().save(user_view_y);

						if (drag_direction == DragDirection::DOWN)
						{
							drag_direction = DragDirection::NONE;

							if (temp_position.y() > min_view_y - input_bg_y)
							{
								if (view_input)
									temp_position.set_y(min_view_y - input_bg_y);
								else
									temp_position.set_y(min_view_y);
							}

							position = temp_position;

							Setting<PosCHAT>::get().save(position);
						}

						return Cursor::State::CHATBARVDRAG;
					}
					else if (temp_view_y == 0)
					{
						user_view_x = temp_view_x;
						temp_view_x = 0;
						dragged = false;

						update_view(false);

						Setting<ChatViewX>::get().save(user_view_x);

						if (drag_direction == DragDirection::LEFT)
						{
							drag_direction = DragDirection::NONE;

							position = temp_position;

							Setting<PosCHAT>::get().save(position);
						}

						return Cursor::State::CHATBARHDRAG;
					}
					else
					{
						user_view_x = temp_view_x;
						temp_view_x = 0;

						user_view_y = temp_view_y;
						temp_view_y = 0;

						dragged = false;

						update_view(false);

						Setting<ChatViewX>::get().save(user_view_x);
						Setting<ChatViewY>::get().save(user_view_y);

						if (drag_direction == DragDirection::DOWN || drag_direction == DragDirection::LEFT || drag_direction == DragDirection::DOWNLEFT)
						{
							drag_direction = DragDirection::NONE;

							if (temp_position.y() > min_view_y - input_bg_y)
							{
								if (view_input)
									temp_position.set_y(min_view_y - input_bg_y);
								else
									temp_position.set_y(min_view_y);
							}

							position = temp_position;

							Setting<PosCHAT>::get().save(position);

							if (drag_direction != DragDirection::DOWNLEFT)
								return Cursor::State::CHATBARBRTLDRAG;
						}

						return Cursor::State::CHATBARBLTRDRAG;
					}
				}
			}
			else if (indragrange(cursor_position))
			{
				return Cursor::State::CHATBARMOVE;
			}
			else if (view_max)
			{
				if (intopleftrange(cursor_position) || inbottomrightrange(cursor_position))
					return Cursor::State::CHATBARBRTLDRAG;
				else if (intoprightrange(cursor_position) || inbottomleftrange(cursor_position))
					return Cursor::State::CHATBARBLTRDRAG;
				else if (intoprange(cursor_position) || inbottomrange(cursor_position))
					return Cursor::State::CHATBARVDRAG;
				else if (inleftrange(cursor_position) || inrightrange(cursor_position))
					return Cursor::State::CHATBARHDRAG;
			}
		}

		return UIElement::send_cursor(clicked, cursor_position);
	}

	void UIChatBar::send_key(int32_t keycode, bool pressed, bool escape)
	{
		// ESC 优先关目标列表：列表开着时按 ESC 应该只是收回列表，而不是把整个
		// 输入行收掉（原生下拉就是这个次序）。
		if (pressed && escape && classic_target_list_open)
		{
			classic_target_list_open = false;
			return;
		}

		if (pressed)
		{
			if (keycode == 'W' || keycode == 'w')
			{
				focus_whisper();
				return;
			}
			if (keycode == 'S' || keycode == 's')
			{
				UI::get().emplace<UISoftKey>([](const std::string& msg) {
					if (msg.empty()) return;
					GeneralChatPacket(msg, true).dispatch();
				}, []() {}, "Chat");
				return;
			}
			if (keycode == 'P' || keycode == 'p')
			{
				UI::get().emplace<UISoftKey>([](const std::string& msg) {
					if (msg.empty()) return;
					SpouseChatPacket(msg).dispatch();
				}, []() {}, "Spouse chat");
				return;
			}
			if (keycode == 'Y' || keycode == 'y')
			{
				UI::get().emplace<UISoftKey>([](const std::string& msg) {
					if (msg.empty()) return;
					std::list<int32_t> empty;
					MultiChatPacket(MultiChatPacket::Type::PARTY, empty, msg).dispatch();
				}, []() {}, "Party chat");
				return;
			}
			if (keycode == 'G' || keycode == 'g')
			{
				UI::get().emplace<UISoftKey>([](const std::string& msg) {
					if (msg.empty()) return;
					std::list<int32_t> empty;
					MultiChatPacket(MultiChatPacket::Type::GUILD, empty, msg).dispatch();
				}, []() {}, "Guild chat");
				return;
			}
			if (keycode == 'L' || keycode == 'l')
			{
				UI::get().emplace<UISoftKey>([](const std::string& msg) {
					if (msg.empty()) return;
					std::list<int32_t> empty;
					MultiChatPacket(MultiChatPacket::Type::ALLIANCE, empty, msg).dispatch();
				}, []() {}, "Alliance chat");
				return;
			}
			if (keycode == 'B' || keycode == 'b')
			{
				int32_t cid = 0;
				if (auto bl = UI::get().get_element<UIBuddyList>())
					cid = bl->get_selected_cid();
				if (cid <= 0)
					if (auto ci = UI::get().get_element<UICharInfo>())
						cid = ci->get_target_cid();
				UI::get().emplace<UISoftKey>([cid](const std::string& msg) {
					if (msg.empty()) return;
					std::list<int32_t> recips;
					if (cid > 0) recips.push_back(cid);
					MultiChatPacket(MultiChatPacket::Type::BUDDY, recips, msg).dispatch();
				}, []() {}, "Buddy chat");
				return;
			}
			if (keycode == 'F' || keycode == 'f')
			{
				std::string pre;
				if (auto ci = UI::get().get_element<UICharInfo>())
					pre = ci->get_target_name();
				if (!pre.empty()) {
					FindPlayerPacket(pre).dispatch();
					LOG(LOG_DEBUG, "UIChatBar: SoftKey find=" << pre);
				} else {
					UI::get().emplace<UISoftKey>([](const std::string& to) {
						if (to.empty()) return;
						FindPlayerPacket(to).dispatch();
						LOG(LOG_DEBUG, "UIChatBar: SoftKey find=" << to);
					}, []() {}, "Find name");
				}
				return;
			}
			if (keycode == KeyAction::Id::TAB && view_input)
			{
				complete_gm_command();
				return;
			}
			if (keycode == KeyAction::Id::RETURN)
			{
				if (!view_input)
				{
					toggle_input(true);
				}
				else
				{
					input_text.change_text("");
					input_text.set_state(Textfield::State::FOCUSED);
				}
			}
			else
			{
				int32_t index = UI::get().get_keyboard().get_mapping_index(keycode);

				input_text.change_text("");
				input_text.set_state(Textfield::State::FOCUSED);
				input_text.send_key(KeyType::Id::TEXT, index, pressed);
			}
		}
	}

	UIElement::Type UIChatBar::get_type() const
	{
		return TYPE;
	}

	bool UIChatBar::has_input() const
	{
		return view_input;
	}

	bool UIChatBar::is_classic() const
	{
		return classic_mode;
	}

	void UIChatBar::toggle_view()
	{
		// classic 没有 max/min 视图，对外语义就是翻聊天历史框。
		if (classic_mode)
		{
			set_classic_log(!classic_log_open);
			return;
		}

		toggle_view(!view_max, true);
	}

	std::string UIChatBar::chat_target_name(uint8_t target_id) const
	{
		// Q5: 八键与 ChatTargetIndex 一一对应（前四项为原生 v83 文案，
		// 后四项 NO-NATIVE，见 locale/*.ini 注释）。缺键时 Locale::text
		// 回落键名本身，不会崩，但下拉会显示原始键 —— 上线前双侧都已补齐。
		static constexpr const char* KEYS[] =
		{
			"chat.target.all",
			"chat.target.whisper",
			"chat.target.party",
			"chat.target.guild",
			"chat.target.buddy",
			"chat.target.alliance",
			"chat.target.spouse",
			"chat.target.megaphone"
		};

		return target_id < TARGET_COUNT ? Locale::text(KEYS[target_id]) : std::string();
	}

	// 频道快捷键接线（statusbar-hud/D4 + social-economy/D1）。
	// 依据 Angel 083 `CWvsContext::UseFuncKeyMapped` @ 0xa0773d：
	//   normal 键 case 4 -> `sub_8D4D43(0)` -> `CUIStatusBar::StartChat(0)`，
	//   即「切到对所有人 + 立刻把输入行拉起来并聚焦」；case 17 -> 2、case 19 -> 1
	//   同族。功能键侧（a2<0 分支）只产 0/1/2/3/4/5/7 这些目标号，与
	//   ChatTargetIndex 的前三项一一对齐。
	// 修前是 `{}` 空 stub：SAY/PARTYCHAT/FRIENDSCHAT/GUILDCHAT/ALLIANCECHAT/
	// TOSPOUSE 六个频道键按下完全无反应。
	// 说明：目标路由（send_to_target_channel）只在 classic HUD 的 chatTarget 上生效；
	// modern HUD 没有该控件，此时至少保证输入行能开，频道仍可用斜杠别名切换。
	void UIChatBar::change_target(int32_t action)
	{
		uint8_t target = TARGET_ALL;

		switch (action)
		{
		case KeyAction::Id::SAY:
			target = TARGET_ALL;
			break;
		case KeyAction::Id::PARTYCHAT:
			target = TARGET_PARTY;
			break;
		case KeyAction::Id::FRIENDSCHAT:
			target = TARGET_BUDDY;
			break;
		case KeyAction::Id::GUILDCHAT:
			target = TARGET_GUILD;
			break;
		case KeyAction::Id::ALLIANCECHAT:
			target = TARGET_ALLIANCE;
			break;
		case KeyAction::Id::TOSPOUSE:
			target = TARGET_SPOUSE;
			break;
		default:
			return;
		}

		set_classic_target(target);

		if (!view_input)
			toggle_input(true);

		LOG(LOG_DEBUG, "UIChatBar: change_target target=" << static_cast<int>(target));
	}

	void UIChatBar::set_classic_target(uint8_t target_id)
	{
		if (target_id >= TARGET_COUNT)
			return;

		classic_chat_target_id = target_id;
		classic_target_text.change_text(chat_target_name(target_id));
	}

	void UIChatBar::toggle_classic_target_list(bool open)
	{
		// 收起态（灰带）没有 chatTarget 框，箭头钮也被 sync_classic_buttons 关了，
		// 列表无处贴附，原生此时也不给选。
		if (open && !classic_log_open && !view_input)
			return;

		classic_target_list_open = open;
		// 初始选中当前频道行（hover = 已选中的 row），不是固定 0；
		// 这样打开时标记条直接落在「对所有人/密语/组队…」对应位置，视觉不跳。
		classic_target_hover = classic_chat_target_id;
	}

	void UIChatBar::pick_classic_target(size_t index)
	{
		if (index >= TARGET_COUNT)
			return;

		classic_target_hover = index; // 选中后更新 hover，视觉标记条落位到刚点的那行
		set_classic_target(static_cast<uint8_t>(index));

		// 原生选了「悄悄话」之后要输对象名，走「请输入悄悄话的对象。」那一层
		// （UIWhisperTargetDlg）；其余三项选中即生效，不需要再问名字。
		if (index == TARGET_WHISPER)
		{
			if (auto dlg = UI::get().get_element<UIWhisperTargetDlg>())
			{
				dlg->makeactive();
				dlg->open();
			}
			else if (auto new_dlg = UI::get().emplace<UIWhisperTargetDlg>())
			{
				new_dlg->open();
			}
		}
	}

	Point<int16_t> UIChatBar::classic_target_list_position() const
	{
		// 向上展开：列表底边贴在 chatTarget 框顶（行内 TARGET_BOX_Y）。底栏贴屏幕
		// 下沿，向下没有空间。input_pos 与 draw() 同源，就是 position。
		const int16_t list_h = static_cast<int16_t>(TARGET_COUNT * TARGET_LIST_ROW_H);

		return Point<int16_t>(position.x(), position.y() + TARGET_BOX_Y - list_h);
	}

	bool UIChatBar::in_classic_target_row(Point<int16_t> cursor_position, size_t row) const
	{
		const Point<int16_t> lt = classic_target_list_position()
			+ Point<int16_t>(1, static_cast<int16_t>(row * TARGET_LIST_ROW_H) + 1);
		const Point<int16_t> rb = lt + Point<int16_t>(TARGET_LIST_W - 2, TARGET_LIST_ROW_H - 2);

		return Rectangle<int16_t>(lt, rb).contains(cursor_position);
	}

	void UIChatBar::sync_classic_buttons()
	{
		// 收起态（历史框关 + 输入未激活）原生不画左端的 ▶ 密语箭头 —— 灰带里只有
		// 正文和右端按钮（见 evidence/w4-chatbar/coords-fold.md 的截帧对齐）。箭头
		// 是 MapleButton，由 UIElement::draw 画，只能显式 deactivate。
		// 一律用 find 而不是 operator[] —— 后者会往 map 里塞空 unique_ptr 再被解引用。
		const bool folded = !classic_log_open && !view_input;

		if (const auto it = buttons.find(Buttons::BtWhisper); it != buttons.end() && it->second)
			it->second->set_active(!folded);

		// 同位互斥：收起态显示 BtMax(+)，展开态显示 BtMin(-)。
		if (const auto it = buttons.find(Buttons::BtMax); it != buttons.end() && it->second)
			it->second->set_active(!classic_log_open);

		if (const auto it = buttons.find(Buttons::BtMin); it != buttons.end() && it->second)
			it->second->set_active(classic_log_open);
	}

	void UIChatBar::set_classic_log(bool open)
	{
		classic_log_open = open;

		// 关框即回到最新一条：偏移留着的话，下次只开输入行（不展框）也会从
		// 半路的旧消息开始画，看起来像「消息记录停在奇怪的地方」。
		if (!open)
			classic_scroll_offset = 0;

		// D7：聊天类型持久化（1=收起 / 2=紧凑 / 3=完整）。展开且当前是收起态时
		// 升级为完整(3)；紧凑(2)展开态保持不变。
		if (!open)
			chat_type = 1;
		else if (chat_type == 1)
			chat_type = 3;
		Setting<ChatType>::get().save(chat_type);

		// 高度/状态变了，重算历史框顶（min_view_y 是展开态几何唯一真源）。
		if (open)
			min_view_y = static_cast<int16_t>(position.y() - user_view_y);

		sync_classic_buttons();
		sync_classic_scrollbar();
	}

	Point<int16_t> UIChatBar::classic_log_position() const
	{
		return Point<int16_t>(position.x(), static_cast<int16_t>(position.y() - user_view_y));
	}

	int16_t UIChatBar::classic_scroll_x() const
	{
		// 滚动条贴历史框右沿往内一列：原生灰轨 551..564，本框宽 566 ⇒ 566-15=551。
		return static_cast<int16_t>(dimension.x() - SCROLL_W);
	}

	int16_t UIChatBar::classic_visible_lines() const
	{
		// 与 draw() 的循环判据严格同源：第 k 行（0 起）的 message_y =
		// LOG_FIRST_Y + k * LOG_LINE_H，可画条件是它 <= user_view_y + LOG_LINE_SLACK。
		const int16_t k = static_cast<int16_t>(
			(user_view_y + LOG_LINE_SLACK - LOG_FIRST_Y) / LOG_LINE_H);

		return static_cast<int16_t>(std::max<int16_t>(1, k + 1));
	}

	void UIChatBar::sync_classic_scrollbar()
	{
		const int16_t visible = classic_visible_lines();
		const int16_t total = static_cast<int16_t>(message_history.size());
		const int16_t scrollmax = static_cast<int16_t>(std::max<int16_t>(0, total - visible));

		if (classic_scroll_offset > scrollmax)
			classic_scroll_offset = scrollmax;

		// 轨道上下各内缩 SCROLL_INSET（原生实测 11~12），高度已被夹到 >= 40，
		// 不会出现 first > second。
		classic_scrollbar.setvertical(Range<int16_t>(SCROLL_INSET, static_cast<int16_t>(user_view_y - SCROLL_INSET)));

		// setrows 的第一参会覆写 Slider 内部的 row，必须按「row 与 offset 反相」换算：
		// 滑块贴轨道底部 = 看最新一条 = offset 0（正文是倒着排的，最新在最下）。
		// 第三参是「总行数」，Slider 内部算 rowmax = 总行数 - 每页行数，所以总行数
		// 不足一页时先抬到 visible，避免 rowmax 变负。
		classic_scrollbar.setrows(static_cast<int16_t>(scrollmax - classic_scroll_offset),
			visible, static_cast<int16_t>(std::max<int16_t>(visible, total)));

		classic_scrollbar.setenabled(classic_log_open && scrollmax > 0);
	}

	void UIChatBar::send_scroll(double yoffset)
	{
		if (!classic_mode || !classic_log_open)
			return;

		const int16_t visible = classic_visible_lines();
		const int16_t total = static_cast<int16_t>(message_history.size());
		const int16_t scrollmax = static_cast<int16_t>(std::max<int16_t>(0, total - visible));
		if (scrollmax <= 0)
			return;

		// 滚轮向上（yoffset>0，原生手感）看更早的消息 = 历史偏移增大；约 3 行/格。
		const int16_t step = static_cast<int16_t>(
			std::clamp<int16_t>(static_cast<int16_t>(yoffset * 3.0), -scrollmax, scrollmax));

		classic_scroll_offset = static_cast<int16_t>(
			std::clamp<int16_t>(classic_scroll_offset + step, 0, scrollmax));

		sync_classic_scrollbar();
	}

	void UIChatBar::set_classic_log_height(int16_t height)
	{
		int16_t h = height;

		if (h < CLASSIC_LOG_H_MIN)
			h = CLASSIC_LOG_H_MIN;
		else if (h > CLASSIC_LOG_H_MAX)
			h = CLASSIC_LOG_H_MAX;

		if (h == user_view_y)
			return;

		user_view_y = h;

		// 这个不变式在 modern 分支才读得到，但构造函数就是按它写的，改高度后不更新
		// 会留下「同一名词两种值」的坑。
		min_view_y = static_cast<int16_t>(position.y() - user_view_y);

		// 高度变了，原来的偏移可能已经超出新的可滚范围，也可能整框都是正文
		// （offset 归 0 才看得到最新一条）。交给 sync 统一夹。
		sync_classic_scrollbar();
	}

	bool UIChatBar::classic_in_grip(Point<int16_t> cursor_position) const
	{
		const Point<int16_t> log_pos = classic_log_position();

		return Rectangle<int16_t>(
			log_pos,
			log_pos + Point<int16_t>(classic_scroll_x(), GRIP_H)
		).contains(cursor_position);
	}

	void UIChatBar::remove_cursor()
	{
		UIDragElement<PosCHAT>::remove_cursor();

		// 指针离开聊天栏（框架只对「非前台」元素调这里）：滑块的 scrolling /
		// PRESSED 态必须清，否则拖出命中区后下次进区接着滚、箭头还停在按下态。
		classic_scrollbar.remove_cursor();
	}

	void UIChatBar::show_message(const char* message, MessageType type)
	{
		Color::Name color = Color::Name::RED;

		if (type == MessageType::YELLOW)
			color = Color::Name::YELLOW;
		else if (type == MessageType::WHITE)
			color = Color::Name::WHITE;
		else if (type == MessageType::BLUE)
			color = Color::Name::BLUE;
		else if (type == MessageType::RED)
			color = Color::Name::RED;
		else if (type == MessageType::GM)
			color = Color::Name::VIOLET;
		else
			LOG(LOG_DEBUG, "[UIChatBar::show_message]: " << type << " not supported.");

		message_history.push_back(Message(MessageGroup::ALL, type, Text(Text::Font::A11M, Text::Alignment::LEFT, color, message)));
		trim_message_history();

		// 行数变了要重算可滚范围；贴在最新一条（offset==0）时保持贴底，
		// 不然新消息会被顶出可视区、看起来像「来消息了但滚动条不动」。
		if (classic_mode)
			sync_classic_scrollbar();
	}

	// Q4: 历史上限裁剪。原生（本仓库 v83 派生）没有任何聊天日志上限常量，
	// message_history 从落地起全程 push_back 无 cap；用户 2026-11-04 裁决按
	// GMS v83 常见值取 200。超限时从最旧一条起丢弃 —— classic_scroll_offset
	// 是「距最新的行数」，从头部擦不影响贴底视图。
	void UIChatBar::trim_message_history()
	{
		if (message_history.size() > MESSAGE_HISTORY_CAP)
			message_history.erase(message_history.begin(),
				message_history.begin() + static_cast<std::ptrdiff_t>(message_history.size() - MESSAGE_HISTORY_CAP));
	}

	Color::Name UIChatBar::channel_color(ChatChannel ch)
	{
		switch (ch)
		{
			case ChatChannel::ALL:       return Color::Name::WHITE;
			case ChatChannel::WHISPER:   return Color::Name::BLUE;
			case ChatChannel::PARTY:     return Color::Name::GREEN;
			case ChatChannel::GUILD:     return Color::Name::ORANGE;
			case ChatChannel::BUDDY:     return Color::Name::LIGHTBLUE;
			case ChatChannel::ALLIANCE:  return Color::Name::MALIBU;
			case ChatChannel::SPOUSE:    return Color::Name::PINK;
			case ChatChannel::MEGAPHONE: return Color::Name::SUPERNOVA;
			case ChatChannel::SYSTEM:    return Color::Name::YELLOW;
			case ChatChannel::GM:        return Color::Name::VIOLET;
		}
		return Color::Name::WHITE;
	}

	void UIChatBar::show_message(const char* message, ChatChannel ch)
	{
		Color::Name color = channel_color(ch);

		message_history.push_back(Message(MessageGroup::ALL, ch, Text(Text::Font::A11M, Text::Alignment::LEFT, color, message)));
		trim_message_history();

		// 行数变了要重算可滚范围；贴在最新一条（offset==0）时保持贴底，
		// 不然新消息会被顶出可视区、看起来像「来消息了但滚动条不动」。
		if (classic_mode)
			sync_classic_scrollbar();
	}

	void UIChatBar::show_message(const std::string& prefix, const std::string& speaker, const std::string& message, ChatChannel ch)
	{
		Color::Name color = channel_color(ch);
		// 显示串 = prefix + speaker + " : " + message（与既有 "Name : ..." 格式一致）。
		std::string full = prefix + speaker + " : " + message;
		Message msg(MessageGroup::ALL, ch, Text(Text::Font::A11M, Text::Alignment::LEFT, color, full));
		msg.speaker = speaker;
		msg.prefix = prefix;
		msg.clickable = !speaker.empty();
		// 量出前缀与名字像素宽，供 send_cursor 实时算名字热区（不存 stale 坐标）。
		Text prefix_t(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, prefix);
		Text speaker_t(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, speaker);
		msg.name_x_off = prefix.empty() ? 0 : prefix_t.width();
		msg.name_w = speaker.empty() ? 0 : speaker_t.width();
		message_history.push_back(std::move(msg));
		trim_message_history();

		if (classic_mode)
			sync_classic_scrollbar();
	}

	Rectangle<int16_t> UIChatBar::name_menu_rect(size_t i) const
	{
		Point<int16_t> lt = name_menu_pos + Point<int16_t>(0, static_cast<int16_t>(i) * NAME_MENU_ROW_H);
		return Rectangle<int16_t>(lt, lt + Point<int16_t>(NAME_MENU_W, NAME_MENU_ROW_H));
	}

	void UIChatBar::draw_name_menu() const
	{
		if (!name_menu_open)
			return;

		const int16_t h = NAME_MENU_ROW_H * static_cast<int16_t>(NM_COUNT);
		ColorBox back(NAME_MENU_W, h, Color::Name::BLACK, 0.85f);
		back.draw(DrawArgument(name_menu_pos));

		for (size_t i = 0; i < NM_COUNT; ++i)
		{
			Point<int16_t> rp = name_menu_pos + Point<int16_t>(0, static_cast<int16_t>(i) * NAME_MENU_ROW_H);
			if (name_menu_hover == static_cast<int16_t>(i))
			{
				ColorBox hl(NAME_MENU_W, NAME_MENU_ROW_H, Color::Name::IRISHCOFFEE, 0.6f);
				hl.draw(DrawArgument(rp));
			}
			name_menu_items[i].draw(DrawArgument(rp + Point<int16_t>(10, name_menu_items[i].y_for_ink_top(NAME_MENU_ROW_H / 2 + 2))));
		}
	}

	void UIChatBar::open_name_menu(const std::string& name, Point<int16_t> screen_pos)
	{
		name_menu_target = name;
		name_menu_pos = screen_pos;
		name_menu_hover = -1;
		name_menu_items[NM_WHISPER] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "密语");
		name_menu_items[NM_CHARINFO] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "查看角色信息");
		name_menu_items[NM_BUDDY] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "加好友");
		name_menu_open = true;
	}

	void UIChatBar::close_name_menu()
	{
		name_menu_open = false;
		name_menu_hover = -1;
	}

	void UIChatBar::name_menu_select(size_t i)
	{
		std::string name = name_menu_target;
		close_name_menu();

		switch (i)
		{
			case NM_WHISPER:
				set_whisper_target(name);
				break;
			case NM_CHARINFO:
			{
				// 仅当该角色在本地地图已加载时才能拿到 oid；否则原生也无能为力。
				int32_t cid = Stage::get().find_character_id(name);
				if (cid > 0)
				{
					UI::get().emplace<UICharInfo>(cid);
					CharInfoRequestPacket(cid).dispatch();
				}
				else
				{
					LOG(LOG_DEBUG, "UIChatNameMenu: 找不到角色 oid（可能不在当前地图）：" << name);
				}
				break;
			}
			case NM_BUDDY:
				BuddyAddPacket(name, "All").dispatch();
				break;
		}
	}

	void UIChatBar::focus_whisper()
	{
		focus_whisper("");
	}

	void UIChatBar::focus_whisper(const std::string& target)
	{
		// SoftKey：有目标则直接输入正文；否则走原生那一层「请输入悄悄话的对象。」
		// （UIWhisperTargetDlg：Notice4 框 + 输入行 + ComboBox 下拉候选 + BtOK/BtNo）。
		std::string t = target;
		if (t.empty()) {
			if (auto ci = UI::get().get_element<UICharInfo>())
				t = ci->get_target_name();
		}
		if (!t.empty())
		{
			UI::get().emplace<UISoftKey>([t](const std::string& text) {
				if (text.empty()) return;
				WhisperPacket(t, text).dispatch();
				LOG(LOG_DEBUG, "UIChatBar: Whisper to=" << t << " len=" << text.size());
			}, []() {}, "Whisper msg");
			return;
		}

		if (auto dlg = UI::get().get_element<UIWhisperTargetDlg>())
		{
			dlg->makeactive();
			dlg->open();
			return;
		}

		if (auto dlg = UI::get().emplace<UIWhisperTargetDlg>())
			dlg->open();
	}

	void UIChatBar::set_whisper_target(const std::string& target)
	{
		if (target.empty())
			return;

		// Q5: 记下对象 —— 下拉「悄悄话」频道的直发路径（send_to_target_channel）
		// 读它；此前这里只往输入行塞 "/w name " 前缀，频道本身不可直发。
		whisper_target_name = target;

		if (classic_mode)
		{
			// 目标标签切到「悄悄话」，与原生的观感一致：选完对象后左框显示的是频道名，
			// 具体对象只在输入行的 "/w 名字 " 前缀里。
			set_classic_target(TARGET_WHISPER);
			toggle_classic_target_list(false);
		}

		if (!view_input)
			toggle_input(true);

		input_text.change_text("/w " + target + " ");
		input_text.set_state(Textfield::State::FOCUSED);

		LOG(LOG_DEBUG, "UIChatBar: whisper target prefilled len=" << target.size());
	}

	bool UIChatBar::indragrange(Point<int16_t> cursor_position) const
	{
		Rectangle<int16_t> bounds = Rectangle<int16_t>(get_dragarea_position(), get_dragarea_position() + dragarea);

		return bounds.contains(cursor_position);
	}

	bool UIChatBar::intoprange(Point<int16_t> cursor_position) const
	{
		Rectangle<int16_t> bounds = Rectangle<int16_t>(get_position(), get_position() + Point<int16_t>(dimension.x(), 3));

		return bounds.contains(cursor_position);
	}

	bool UIChatBar::inbottomrange(Point<int16_t> cursor_position) const
	{
		Point<int16_t> position = get_position() + Point<int16_t>(0, dimension.y() - 3);
		Rectangle<int16_t> bounds = Rectangle<int16_t>(position, position + Point<int16_t>(dimension.x(), 3));

		return bounds.contains(cursor_position);
	}

	bool UIChatBar::inleftrange(Point<int16_t> cursor_position) const
	{
		Rectangle<int16_t> bounds = Rectangle<int16_t>(get_position(), get_position() + Point<int16_t>(3, dimension.y()));

		return bounds.contains(cursor_position);
	}

	bool UIChatBar::inrightrange(Point<int16_t> cursor_position) const
	{
		Point<int16_t> position = get_position() + Point<int16_t>(dimension.x() - 3, 0);
		Rectangle<int16_t> bounds = Rectangle<int16_t>(position, position + Point<int16_t>(3, dimension.y()));

		return bounds.contains(cursor_position);
	}

	bool UIChatBar::intopleftrange(Point<int16_t> cursor_position) const
	{
		return intoprange(cursor_position) && inleftrange(cursor_position);
	}

	bool UIChatBar::inbottomrightrange(Point<int16_t> cursor_position) const
	{
		return inbottomrange(cursor_position) && inrightrange(cursor_position);
	}

	bool UIChatBar::intoprightrange(Point<int16_t> cursor_position) const
	{
		return intoprange(cursor_position) && inrightrange(cursor_position);
	}

	bool UIChatBar::inbottomleftrange(Point<int16_t> cursor_position) const
	{
		return inbottomrange(cursor_position) && inleftrange(cursor_position);
	}

	Point<int16_t> UIChatBar::get_position() const
	{
		if (temp_view_y == 0 && temp_view_x == 0)
		{
			if (view_max)
				return position - Point<int16_t>(0, top_y + center_y + user_view_y);
			else
				return position - Point<int16_t>(0, top_y + center_y);
		}
		else
		{
			if (temp_view_y > 0 && temp_view_x == 0)
			{
				if (drag_direction == DragDirection::DOWN)
					return temp_position - Point<int16_t>(0, top_y + center_y + temp_view_y);
				else
					return position - Point<int16_t>(0, top_y + center_y + temp_view_y);
			}
			else if (temp_view_y == 0 && temp_view_x > 0)
			{
				if (drag_direction == DragDirection::LEFT)
					return temp_position - Point<int16_t>(0, top_y + center_y + user_view_y);
				else
					return position - Point<int16_t>(0, top_y + center_y + user_view_y);
			}
			else
			{
				if (drag_direction == DragDirection::DOWN || drag_direction == DragDirection::LEFT || drag_direction == DragDirection::DOWNLEFT)
					return temp_position - Point<int16_t>(0, top_y + center_y + temp_view_y);
				else
					return position - Point<int16_t>(0, top_y + center_y + temp_view_y);
			}
		}
	}

	Point<int16_t> UIChatBar::get_dragarea_position() const
	{
		Point<int16_t> drag_origin = drag.get_origin();

		if (view_max)
			return position - Point<int16_t>(drag_origin.x(), top_y + center_y + user_view_y + drag_origin.y());
		else
			return position - Point<int16_t>(drag_origin.x(), top_y + center_y + drag_origin.y());
	}

	Point<int16_t> UIChatBar::get_input_position() const
	{
		return position + Point<int16_t>(0, 15);
	}

	Point<int16_t> UIChatBar::get_input_text_position()
	{
		Point<int16_t> adjust = Point<int16_t>(57, 20);

		return position + adjust;
	}

	void UIChatBar::toggle_input(bool enabled)
	{
		view_input = enabled;

		if (view_input && !view_max)
		{
			view_adjusted = true;

			toggle_view(true, true);
		}
		else
		{
			if (view_adjusted)
			{
				view_adjusted = false;

				toggle_view(false, true);
			}
			else
			{
				update_view(true);
			}
		}

		if (classic_mode)
		{
			sync_classic_buttons();
			return;
		}

		buttons[Buttons::BtChat]->set_active(view_input);
		buttons[Buttons::BtItemLink]->set_active(view_input);
		buttons[Buttons::BtChatEmoticon]->set_active(view_input);
		buttons[Buttons::BtHelp]->set_active(view_input);
		buttons[Buttons::BtOutChat]->set_active(view_input);
	}

	void UIChatBar::toggle_view(bool max, bool pressed)
	{
		view_max = max;

		if (!view_max)
		{
			view_input = false;
			view_adjusted = false;

			if (!classic_mode)
			{
				buttons[Buttons::BtChat]->set_active(view_input);
				buttons[Buttons::BtItemLink]->set_active(view_input);
				buttons[Buttons::BtChatEmoticon]->set_active(view_input);
				buttons[Buttons::BtHelp]->set_active(view_input);
				buttons[Buttons::BtOutChat]->set_active(view_input);
			}
		}

		Setting<ChatViewMax>::get().save(view_max);

		if (!classic_mode)
		{
			buttons[Buttons::BtMax]->set_active(!view_max);
			buttons[Buttons::BtMin]->set_active(view_max);
		}

		update_view(pressed);
	}
	void UIChatBar::update_view(bool pressed)
	{
		if (view_input)
			input_text.set_state(Textfield::State::FOCUSED);
		else
			input_text.set_state(Textfield::State::DISABLED);

		// classic HUD 的输入行位置由构造函数固定，且 BtMin/BtChat 等按钮从未创建
		// （buttons 是 std::map，operator[] 会插入空指针再解引用 => 0xc0000005）。
		if (classic_mode)
			return;

		if (pressed)
		{
			if (position_adjusted)
			{
				position_adjusted = false;

				if (position.y() >= min_view_y - input_bg_y)
					position.shift_y(input_bg_y);
			}
			else
			{
				if (view_input && position.y() >= min_view_y)
				{
					position_adjusted = true;
					position.shift_y(-input_bg_y);
				}
			}
		}

		int16_t y = position.y() - center_y - user_view_y;

		if (y < 0)
		{
			position.shift_y(-y);
			position.shift_y(15);
		}

		Point<int16_t> btMin_padding = Point<int16_t>(-4, 3);

		if (view_max)
		{
			dimension = Point<int16_t>(user_view_x, top_y + center_y + bottom_y) + Point<int16_t>(0, user_view_y);

			buttons[Buttons::BtMin]->set_position(Point<int16_t>(user_view_x - btMin_x, -top_y - center_y - user_view_y) + btMin_padding);

			Point<int16_t> input_btns_pos = Point<int16_t>(input_max_x - (input_bg_x - user_view_x) + input_origin_x - 17, 15 + input_origin_y + 1);

			buttons[Buttons::BtChat]->set_position(input_btns_pos + Point<int16_t>(input_btns_x * 0, 0));
			buttons[Buttons::BtItemLink]->set_position(input_btns_pos + Point<int16_t>(input_btns_x * 1, 0));
			buttons[Buttons::BtChatEmoticon]->set_position(input_btns_pos + Point<int16_t>(input_btns_x * 2, 0));
			buttons[Buttons::BtHelp]->set_position(input_btns_pos + Point<int16_t>(input_btns_x * 3, 0));
			buttons[Buttons::BtOutChat]->set_position(input_btns_pos + Point<int16_t>(input_btns_x * 4, 0));
		}
		else
		{
			dimension = Point<int16_t>(min_x, top_y + center_y + bottom_y);

			buttons[Buttons::BtMin]->set_position(Point<int16_t>(min_x - btMin_x, -top_y - center_y - user_view_y) + btMin_padding);
		}

#if LOG_LEVEL >= LOG_UI
		dimension_box = ColorBox(dimension.x(), dimension.y(), Color::Name::RED, 0.5f);
		top_box = ColorBox(dimension.x(), 3, Color::Name::GREEN, 0.5f);
		bottom_box = ColorBox(dimension.x(), 3, Color::Name::GREEN, 0.5f);
		left_box = ColorBox(3, dimension.y(), Color::Name::YELLOW, 0.5f);
		right_box = ColorBox(3, dimension.y(), Color::Name::YELLOW, 0.5f);
		input_box = ColorBox(user_view_x, input_bg_y, Color::Name::RED, 0.5f);
#endif
	}

	void UIChatBar::input_text_enter_callback(std::string message)
	{
		if (message == "")
		{
			input_text_escape_callback();
		}
		else
		{
			// D2：原生 CChatHelper::TryChat 四规则预检（刷屏/重复句/脏词/禁言）。
			// 拦截则直接返回且不入历史（与原生「发不出去就不记录」一致）。
			if (!try_chat(message))
				return;

			user_message_history.push_back(message);
			user_message_history_index = user_message_history.size();
			// Q4: 输入历史（↑/↓ 回溯）同样夹到 200 条，从最旧裁。
			if (user_message_history.size() > MESSAGE_HISTORY_CAP)
			{
				user_message_history.erase(user_message_history.begin(),
					user_message_history.begin() + static_cast<std::ptrdiff_t>(user_message_history.size() - MESSAGE_HISTORY_CAP));
				user_message_history_index = user_message_history.size();
			}

			// Q5: 斜杠命令先解析成 (verb, body)；命中命令别名则剥掉命令头，
			// 正文按对应频道直发（保留为「快捷别名」，与频道下拉等价）。
			// /w name text → WhisperPacket；/find name → FindPlayerPacket
			if (message.size() >= 3 && (message[0] == '/' || message[0] == '@'))
			{
				std::string cmd = message.substr(1);
				auto sp = cmd.find(' ');
				std::string verb = (sp == std::string::npos) ? cmd : cmd.substr(0, sp);
				std::string rest = (sp == std::string::npos) ? "" : cmd.substr(sp + 1);
				while (!rest.empty() && rest[0] == ' ') rest.erase(0, 1);

				if ((verb == "w" || verb == "whisper") && !rest.empty())
				{
					auto sp2 = rest.find(' ');
					if (sp2 != std::string::npos && sp2 + 1 < rest.size())
					{
						// Q5: 走统一密语出口 —— 正文暂存，WhisperResult 成功才回显。
						send_whisper(rest.substr(0, sp2), rest.substr(sp2 + 1));
						LOG(LOG_DEBUG, "UIChatBar: /w Whisper");
						keep_input_focus();
						return;
					}
				}
				else if ((verb == "find" || verb == "f") && !rest.empty())
				{
					FindPlayerPacket(rest).dispatch();
					LOG(LOG_DEBUG, "UIChatBar: /find");
					keep_input_focus();
					return;
				}
				else if ((verb == "p" || verb == "party") && !rest.empty())
				{
					std::list<int32_t> empty;
					MultiChatPacket(MultiChatPacket::Type::PARTY, empty, rest).dispatch();
					// Q5 回显矩阵：服务端 partyChat 显式排除发送者（World.java
					// if(!name.equals(namefrom))），不发回来 ⇒ 必须本地回显。
					echo_own_message(rest, ChatChannel::PARTY);
					LOG(LOG_DEBUG, "UIChatBar: /p MultiChat");
					keep_input_focus();
					return;
				}
				else if ((verb == "g" || verb == "guild") && !rest.empty())
				{
					std::list<int32_t> empty;
					MultiChatPacket(MultiChatPacket::Type::GUILD, empty, rest).dispatch();
					echo_own_message(rest, ChatChannel::GUILD);
					LOG(LOG_DEBUG, "UIChatBar: /g MultiChat");
					keep_input_focus();
					return;
				}
				else if ((verb == "a" || verb == "alliance") && !rest.empty())
				{
					std::list<int32_t> empty;
					MultiChatPacket(MultiChatPacket::Type::ALLIANCE, empty, rest).dispatch();
					echo_own_message(rest, ChatChannel::ALLIANCE);
					LOG(LOG_DEBUG, "UIChatBar: /a MultiChat");
					keep_input_focus();
					return;
				}
				else if ((verb == "b" || verb == "buddy") && !rest.empty())
				{
					// Q5: /b 与下拉好友频道共用同一条收件人收集逻辑。
					std::list<int32_t> recips = collect_online_buddy_cids();
					MultiChatPacket(MultiChatPacket::Type::BUDDY, recips, rest).dispatch();
					echo_own_message(rest, ChatChannel::BUDDY);
					LOG(LOG_DEBUG, "UIChatBar: /b MultiChat n=" << recips.size());
					keep_input_focus();
					return;
				}
				else if ((verb == "s" || verb == "spouse" || verb == "mate") && !rest.empty())
				{
					SpouseChatPacket(rest).dispatch();
					// 回显矩阵：SpouseChatHandler 对发送者本人也 sendPacket
					// （c.sendPacket(OnCoupleMessage)）⇒ 禁止本地回显，否则双份。
					LOG(LOG_DEBUG, "UIChatBar: /s SpouseChat");
					keep_input_focus();
					return;
				}
				// '@'/'!' 开头的 GM/用户指令维持原路径（服务端当指令吃掉），
				// 落不到上面任何命令时继续往下走通用发送 + admin 回显。
			}

			// Q5: 非命令文本按聊天栏当前选中频道路由。下拉不再只是标签 ——
			// 选中什么频道，Enter 就发往什么频道。
			if (classic_mode && classic_chat_target_id != TARGET_ALL)
			{
				const uint8_t outcome = send_to_target_channel(message);
				if (outcome == CHANNEL_SENT)
				{
					keep_input_focus();
					return;
				}
				if (outcome == CHANNEL_HELD)
				{
					// 频道就绪度不足（如密语没对象）：入口已弹，文本留在输入行，
					// 不发送、不清空、不进通用兜底 —— 原生此时同样不发。
					return;
				}
				// CHANNEL_FALLBACK：等价于「对所有人」，走下面 GeneralChat。
			}

			GeneralChatPacket(message, true).dispatch();

			// GM 命令本地回显：服务端将 '!'（GM 专用）与 '@'（用户/GM 通用）开头的
			// 消息当指令处理、不广播，GM 账号因此看不到自己输入了什么。对 admin 账号
			// （gmLevel>1）乐观回显紫字，便于辨识；非 admin 不回显（避免普通玩家
			// 误发 @/! 时本地刷字）。
			if (Configuration::get().get_admin() && !message.empty() &&
				(message[0] == '!' || message[0] == '@'))
				show_message(message.c_str(), MessageType::GM);

			// 回显矩阵：GeneralChatHandler 走 MapleMap.broadcastMessage(packet)，
			// source=null 不排除任何人 ⇒ 自己的那条会原样发回来，禁止本地回显。

			// Q0: 发送后保持输入框打开并聚焦，支持连续输入；方向键历史回调随之始终可达。
			keep_input_focus();
			}
			}

	// Q5: 发送成功后的统一收口（原先各斜杠分支各自 close_input()，会把输入行
	// 关掉、与 Q0「连续输入」直接冲突）。语义 = Q0 定稿的三件套。
	// D2：脏词表（客户端本地，缺服务端词表下发时的兜底）。保持小写。
	// 实际词库请以服务端 CURSE 规则为准，这里仅提供脱敏算法的接入点。
	const std::vector<std::string>& UIChatBar::curse_words()
	{
		static const std::vector<std::string> words = {};
		return words;
	}

	// D2：原生 CChatHelper::TryChat 四规则预检。返回 false = 拦截（已弹提示），
	// 调用方不应继续发送；message 可能被就地脱敏（脏词打星）。
	bool UIChatBar::try_chat(std::string& message)
	{
		// ② 刷屏限流：2000ms（原生 SP_892_TOO_MUCH_CHATTING）。稳定时钟，与帧率无关。
		auto now = std::chrono::steady_clock::now();
		if (last_sent_time.time_since_epoch().count() != 0)
		{
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_sent_time).count();
			if (ms < 2000)
			{
				show_message("说话速度太快，请稍后再试。", MessageType::RED);
				return false;
			}
		}

		// ③ 连续 4 条同句（原生 SP_891_REPEATING_THE_SAME_LINE）：最近 4 条全相同则拦。
		recent_lines[recent_count % 4] = message;
		int cnt = (recent_count < 4) ? (recent_count + 1) : 4;
		bool allsame = (cnt == 4);
		for (int i = 0; allsame && i < cnt; ++i)
			if (recent_lines[i] != message)
				allsame = false;
		if (allsame)
		{
			show_message("你这句说了太多遍了。", MessageType::RED);
			return false;
		}
		recent_count++;

		// ④ 脏词脱敏：大小写不敏感，命中子串整段打星（不拦截）。
		if (!curse_words().empty())
		{
			std::string lower = message;
			std::transform(lower.begin(), lower.end(), lower.begin(),
				[](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });
			for (const std::string& w : curse_words())
			{
				if (w.empty())
					continue;
				size_t pos = 0;
				while ((pos = lower.find(w, pos)) != std::string::npos)
				{
					for (size_t k = pos; k < pos + w.size(); ++k)
						message[k] = '*';
					pos += w.size();
				}
			}
		}

		last_sent_time = now;
		return true;
	}

	void UIChatBar::keep_input_focus()
	{
		input_text.change_text("");
		input_text.set_state(Textfield::State::FOCUSED);
		user_message_history_index = user_message_history.size();
	}

	// Q5: 好友频道收件人 = UIBuddyList 里的在线好友 cid。
	// 数据边界（已核实 BuddyHandlers.cpp case 7）：好友名单只在服务端推送
	// updateBuddylist 时写进 UIBuddyList；窗没开过就是空表 ⇒ 发出 0 收件人，
	// 服务端循环不命中任何人（buddyChat 按 cid 查人），等价于静默失败 ——
	// 与原生「没开过好友列表发 /b 无人收到」一致，不假装能发。
	std::list<int32_t> UIChatBar::collect_online_buddy_cids() const
	{
		std::list<int32_t> recips;
		if (auto bl = UI::get().get_element<UIBuddyList>())
			for (const auto& b : bl->get_buddies())
				if (b.online && b.cid > 0)
					recips.push_back(b.cid);
		return recips;
	}

	// Q5: 密语统一出口。s9 WhisperHandler 只给发送者回 WhisperResult 标志、
	// 不回正文（handleWhisper: target.sendPacket(recv) + user.sendPacket(result)），
	// 所以正文暂存，收到 ok=true 才由 on_whisper_result 回显 —— 目标不存在时
	// 输入区只剩服务端的红色 "no such character"，不出正文，行为对齐原生。
	void UIChatBar::send_whisper(const std::string& target, const std::string& text)
	{
		WhisperPacket(target, text).dispatch();
		pending_whisper_target = target;
		pending_whisper_text = text;
	}

	// Q5: WhisperResultHandler 成功分支回调（MessagingHandlers.cpp 调进来）。
	// ok=true 且暂存对象匹配 ⇒ 回显 "(To X) 我名 : 正文"；只清暂存、不重复出
	// 旧的 "Whisper sent to X" 白字行（原生此时显示的就是正文行）。
	void UIChatBar::on_whisper_result(const std::string& target, bool ok)
	{
		if (ok && !pending_whisper_text.empty() && pending_whisper_target == target)
			echo_own_message(pending_whisper_text, ChatChannel::WHISPER, "(To " + target + ") ");
		pending_whisper_target.clear();
		pending_whisper_text.clear();
	}

	// Q5: 本地回显自己发出的消息。前缀与收包端 MultiChatHandler 的 "[Tag] " 完全
	// 同构，名字用自身角色名 ⇒ 别人看到和你自己看到的一致（服务端排除发送者的
	// 那四类，客户端不补这一行就永远只有别人能看见你说话）。
	// 名字可点（clickable）沿用 Q2 逻辑：自己名字点出菜单也无害（原生如此）。
	void UIChatBar::echo_own_message(const std::string& text, ChatChannel ch, const std::string& prefix_in)
	{
		const std::string self = Stage::get().get_player().get_name();
		std::string prefix = prefix_in;
		if (prefix.empty())
		{
			switch (ch)
			{
				case ChatChannel::PARTY:    prefix = "[Party] ";    break;
				case ChatChannel::GUILD:    prefix = "[Guild] ";    break;
				case ChatChannel::BUDDY:    prefix = "[Buddy] ";    break;
				case ChatChannel::ALLIANCE: prefix = "[Alliance] "; break;
				default: break; // WHISPER 等由调用方给显式前缀
			}
		}
		show_message(prefix, self, text, ch);
	}

	// Q5: 按聊天栏选中频道路由非命令文本。返回值语义见 header。
	// TARGET_ALL 不在这里处理（调用方保留 GeneralChat 兜底路径，行为不变）。
	uint8_t UIChatBar::send_to_target_channel(const std::string& text)
	{
		switch (classic_chat_target_id)
		{
			case TARGET_WHISPER:
			{
				if (whisper_target_name.empty())
				{
					// 原生此处弹「请输入悄悄话的对象。」层；文本留在输入行。
					focus_whisper();
					return CHANNEL_HELD;
				}
				send_whisper(whisper_target_name, text);
				return CHANNEL_SENT;
			}
			case TARGET_PARTY:
			{
				std::list<int32_t> empty;
				MultiChatPacket(MultiChatPacket::Type::PARTY, empty, text).dispatch();
				echo_own_message(text, ChatChannel::PARTY);
				return CHANNEL_SENT;
			}
			case TARGET_GUILD:
			{
				std::list<int32_t> empty;
				MultiChatPacket(MultiChatPacket::Type::GUILD, empty, text).dispatch();
				echo_own_message(text, ChatChannel::GUILD);
				return CHANNEL_SENT;
			}
			case TARGET_ALLIANCE:
			{
				std::list<int32_t> empty;
				MultiChatPacket(MultiChatPacket::Type::ALLIANCE, empty, text).dispatch();
				echo_own_message(text, ChatChannel::ALLIANCE);
				return CHANNEL_SENT;
			}
			case TARGET_BUDDY:
			{
				std::list<int32_t> recips = collect_online_buddy_cids();
				MultiChatPacket(MultiChatPacket::Type::BUDDY, recips, text).dispatch();
				echo_own_message(text, ChatChannel::BUDDY);
				return CHANNEL_SENT;
			}
			case TARGET_SPOUSE:
			{
				// 服务端对本人也回包（SpouseChatHandler c.sendPacket）⇒ 不回显。
				SpouseChatPacket(text).dispatch();
				return CHANNEL_SENT;
			}
			case TARGET_MEGAPHONE:
			{
				// 喇叭走道具协议（UseCashItemHandler case 2/5/6：从背包定位
				// 507xxxx 道具 + 单独正文），不消费聊天输入行的文本：打开喇叭窗，
				// 由它自己的 SoftKey 收正文。输入行文本保留（HELD）——
				// 显式预留：v83 下拉里没有「喇叭」项，原生喇叭入口是道具使用；
				// 本端把它收进下拉是 Q5 范围扩张，正文移交点即 UIMegaphoneBasic。
				if (!UI::get().get_element<UIMegaphoneBasic>())
					UI::get().emplace<UIMegaphoneBasic>();
				return CHANNEL_HELD;
			}
			default:
				return CHANNEL_FALLBACK;
		}
	}

	void UIChatBar::complete_gm_command()
	{
		if (!Configuration::get().get_admin())
			return;

		std::string text = input_text.get_text();
		if (text.empty() || (text[0] != '!' && text[0] != '@'))
			return;

		// 提取命令名部分（保留参数），支持 "!cmd args" 形式补全命令名。
		size_t sp = text.find(' ');
		std::string cmd = (sp == std::string::npos) ? text.substr(1) : text.substr(1, sp - 1);
		std::string args = (sp == std::string::npos) ? "" : text.substr(sp); // 含前导空格

		if (cmd.empty())
			return;

		const auto& cmds = gm_commands();
		std::vector<std::string> matches;
		for (const auto& c : cmds)
			// 【2026-11-03 构建修复】c.name 是 const char*，无 .rfind 成员
			// （C2228 阻塞全工程构建）；包一层 std::string 保持前缀匹配语义。
			if (std::string(c.name).rfind(cmd, 0) == 0) // 前缀匹配
				matches.push_back(c.name);

		if (matches.empty())
			return;

		// 计算最长公共前缀，多匹配时只补到 LCP。
		std::string lcp = matches[0];
		for (size_t i = 1; i < matches.size(); ++i)
		{
			size_t j = 0;
			size_t m = lcp.size() < matches[i].size() ? lcp.size() : matches[i].size();
			while (j < m && lcp[j] == matches[i][j])
				++j;
			lcp = lcp.substr(0, j);
		}

		// 唯一匹配且无条件时补空格，方便继续输入参数；多匹配只补到 LCP。
		std::string completion = (matches.size() == 1) ? matches[0] : lcp;
		if (matches.size() == 1 && args.empty())
			completion += ' ';

		input_text.change_text("!" + completion + args);
	}

	void UIChatBar::set_input(const std::string& text)
	{
		if (view_input)
			input_text.change_text(text);
	}

	void UIChatBar::input_text_escape_callback()
	{
		toggle_input(false);
	}

	// 参考端回车即"发送并退出聊天态"；只清空不收框会让输入行一直挂着，
	// 而唯一能收框的 ESC 在 classic HUD 下还踩过空按钮（见 notes/crash-chatbar.md §1-§4）。
	// 收框走 set_state(DISABLED) → UI::remove_textfield()，键盘焦点同时释放。
	void UIChatBar::close_input()
	{
		input_text.change_text("");
		input_text_escape_callback();
	}

	void UIChatBar::change_message(bool up)
	{
		size_t size = user_message_history.size();
		size_t message_history_min = 1;
		size_t message_history_max = size;
		size_t max = 7;

		if (size > max)
			message_history_min = size - max;

		if (user_message_history.size() > 0)
		{
			if (up && user_message_history_index > message_history_min)
				user_message_history_index--;

			if (!up && user_message_history_index < message_history_max)
				user_message_history_index++;

			input_text.change_text(user_message_history[user_message_history_index - 1]);
		}
	}
}