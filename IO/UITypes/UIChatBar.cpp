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
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Locale.h"
#include "../../Util/Misc.h"

#include <list>

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
				Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "对所有人"
			);

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

				UIElement::draw(inter);
				return;
			}

			// 聊天历史框（展开时，输入行上方）：与 tooltip 同一 Q1 判据 ——
			// 半透明黑底 + 1px 灰边（共享件 TooltipBackdrop），不再裸画 ColorBox。
			if (classic_log_open)
			{
				// 底边与白带顶齐平：min_view_y == position.y - user_view_y，
				// 框就是 input_pos - (0, user_view_y) 起、高 user_view_y。
				Point<int16_t> log_pos = classic_log_position();

				classic_log_backdrop.draw(log_pos, dimension.x(), user_view_y);

				// 右端竖滚动条：原生那列灰轨 x 551..564、行 4..70（历史框内），
				// 拖拽改高度后由 sync_classic_scrollbar 同步轨道长度与可见性。
				classic_scrollbar.draw(log_pos);
			}

			// 输入行（26px 白带）：聊天目标对话框（对谁聊天）+ 带下细条 + 输入文本。
			if (classic_chat_target)
				classic_chat_target.draw(input_pos + Point<int16_t>(0, TARGET_BOX_Y));

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
				if (classic_mode)
					set_classic_log(true);
				else
					toggle_view(true, true);

				return Button::State::NORMAL;
			}
			case ms::UIChatBar::BtMin:
			{
				if (classic_mode)
					set_classic_log(false);
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
			// chatTarget 框体本身（箭头右侧）仍是贴图不是按钮，点击 = 切换目标。
			// 命中矩形与 draw() 用的 TARGET_BOX_Y 保持一致。
			Point<int16_t> target_pos = position + Point<int16_t>(TARGET_BT_W, TARGET_BOX_Y);

			if (Rectangle<int16_t>(target_pos, target_pos + Point<int16_t>(CHAT_TARGET_W - TARGET_BT_W, CHAT_TARGET_H)).contains(cursor_position))
			{
				if (clicked)
				{
					cycle_chat_target();
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

	void UIChatBar::cycle_chat_target()
	{
		static const char* TARGET_NAMES[] = { "对所有人", "密语", "组队", "工会" };
		static constexpr uint8_t TARGET_COUNT = 4;

		classic_chat_target_id = static_cast<uint8_t>((classic_chat_target_id + 1) % TARGET_COUNT);
		classic_target_text.change_text(TARGET_NAMES[classic_chat_target_id]);
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
		else
			LOG(LOG_DEBUG, "[UIChatBar::show_message]: " << type << " not supported.");

		message_history.push_back(Message(MessageGroup::ALL, type, Text(Text::Font::A11M, Text::Alignment::LEFT, color, message)));

		// 行数变了要重算可滚范围；贴在最新一条（offset==0）时保持贴底，
		// 不然新消息会被顶出可视区、看起来像「来消息了但滚动条不动」。
		if (classic_mode)
			sync_classic_scrollbar();
	}

	void UIChatBar::focus_whisper()
	{
		focus_whisper("");
	}

	void UIChatBar::focus_whisper(const std::string& target)
	{
		// SoftKey：有目标则直接输入正文；否则 CharInfo / SoftKey 名再填 /w
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

		UI::get().emplace<UISoftKey>([this](const std::string& name)
		{
			if (name.empty()) return;
			if (classic_mode)
				view_input = true;
			else if (!view_input)
				toggle_input(true);

			input_text.change_text("/w " + name + " ");
			input_text.set_state(Textfield::State::FOCUSED);
		}, []() {}, "Whisper target");
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
			user_message_history.push_back(message);
			user_message_history_index = user_message_history.size();

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
						WhisperPacket(rest.substr(0, sp2), rest.substr(sp2 + 1)).dispatch();
						LOG(LOG_DEBUG, "UIChatBar: /w Whisper");
						close_input();
						return;
					}
				}
				else if ((verb == "find" || verb == "f") && !rest.empty())
				{
					FindPlayerPacket(rest).dispatch();
					LOG(LOG_DEBUG, "UIChatBar: /find");
					close_input();
					return;
				}
				else if ((verb == "p" || verb == "party") && !rest.empty())
				{
					std::list<int32_t> empty;
					MultiChatPacket(MultiChatPacket::Type::PARTY, empty, rest).dispatch();
					LOG(LOG_DEBUG, "UIChatBar: /p MultiChat");
					close_input();
					return;
				}
				else if ((verb == "g" || verb == "guild") && !rest.empty())
				{
					std::list<int32_t> empty;
					MultiChatPacket(MultiChatPacket::Type::GUILD, empty, rest).dispatch();
					LOG(LOG_DEBUG, "UIChatBar: /g MultiChat");
					close_input();
					return;
				}
				else if ((verb == "a" || verb == "alliance") && !rest.empty())
				{
					std::list<int32_t> empty;
					MultiChatPacket(MultiChatPacket::Type::ALLIANCE, empty, rest).dispatch();
					LOG(LOG_DEBUG, "UIChatBar: /a MultiChat");
					close_input();
					return;
				}
				else if ((verb == "b" || verb == "buddy") && !rest.empty())
				{
					std::list<int32_t> empty;
					MultiChatPacket(MultiChatPacket::Type::BUDDY, empty, rest).dispatch();
					LOG(LOG_DEBUG, "UIChatBar: /b MultiChat");
					close_input();
					return;
				}
				else if ((verb == "s" || verb == "spouse" || verb == "mate") && !rest.empty())
				{
					SpouseChatPacket(rest).dispatch();
					LOG(LOG_DEBUG, "UIChatBar: /s SpouseChat");
					close_input();
					return;
				}
			}

			GeneralChatPacket(message, true).dispatch();

			close_input();
		}
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