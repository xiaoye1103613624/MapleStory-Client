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
#include "UINpcTalk.h"

#include "UINotice.h"

#include "../UI.h"

#include "../Components/MapleButton.h"
#include "../KeyAction.h"

#include "../../Gameplay/Stage.h"

#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"
#include "../../Util/ResolutionTiers.h"

#include "../../Constants.h"
#include "../../MapleStory.h"
#include "../../Graphics/Geometry.h"

#include <cctype>
#include <climits>
#include <cstdio>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		void try_add_button(std::map<uint16_t, std::unique_ptr<Button>>& buttons, uint16_t id, nl::node src)
		{
			if (src)
				buttons[id] = std::make_unique<MapleButton>(src);
		}

		// BeiDou StringPool（tools/beidou_stringpool.py / IDA sub_406455）：校验 tip 817–821。
		constexpr const char* TIP_817 = "Only numbers are allowed.";
		constexpr const char* TIP_818 = "You may only enter a number \r\nequal to or higher than %d.";
		constexpr const char* TIP_819 = "You may only enter a number \r\nequal to or lower than %d.";
		constexpr const char* TIP_821 = "Please enter at least %d characters.";

		std::string format_tip(const char* fmt, int32_t value)
		{
			char buf[192];
			std::snprintf(buf, sizeof(buf), fmt, value);
			return std::string(buf);
		}
	}

	UINpcTalk::UINpcTalk() :
		offset(0),
		unitrows(0),
		rowmax(0),
		text_draw_y(0),
		text_block_height(0),
		dlg_speaker(0),
		show_slider(false),
		draw_text(false),
		formatted_text(""),
		formatted_text_pos(0),
		timestep(0),
		type(TalkType::NONE),
		msgtype_wire(0),
		input_active(false),
		input_box_y(0)
	{
		nl::node UtilDlgEx = UiAssetResolve::window("UtilDlgEx");

		if (!UiAssetResolve::usable(UtilDlgEx))
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		top = UtilDlgEx["t"];
		fill = UtilDlgEx["c"];
		bottom = UtilDlgEx["s"];
		nametag = UtilDlgEx["bar"];

		min_height = 8 * fill.height() + 14;

		try_add_button(buttons, Buttons::ALLLEVEL, UtilDlgEx["BtAllLevel"]);
		try_add_button(buttons, Buttons::CLOSE, UtilDlgEx["BtClose"]);
		try_add_button(buttons, Buttons::MYLEVEL, UtilDlgEx["BtMyLevel"]);
		try_add_button(buttons, Buttons::NEXT, UtilDlgEx["BtNext"]);
		try_add_button(buttons, Buttons::NO, UtilDlgEx["BtNo"]);
		try_add_button(buttons, Buttons::OK, UtilDlgEx["BtOK"]);
		try_add_button(buttons, Buttons::PREV, UtilDlgEx["BtPrev"]);
		try_add_button(buttons, Buttons::QAFTER, UtilDlgEx["BtQAfter"]);
		try_add_button(buttons, Buttons::QCNO, UtilDlgEx["BtQCNo"]);
		try_add_button(buttons, Buttons::QCYES, UtilDlgEx["BtQCYes"]);
		try_add_button(buttons, Buttons::QGIVEUP, UtilDlgEx["BtQGiveup"]);
		try_add_button(buttons, Buttons::QNO, UtilDlgEx["BtQNo"]);
		try_add_button(buttons, Buttons::QSTART, UtilDlgEx["BtQStart"]);
		try_add_button(buttons, Buttons::QYES, UtilDlgEx["BtQYes"]);
		try_add_button(buttons, Buttons::YES, UtilDlgEx["BtYes"]);

		name = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE);

		onmoved = [&](bool upwards)
		{
			int16_t shift = upwards ? -unitrows : unitrows;
			bool above = offset + shift >= 0;
			bool below = offset + shift <= rowmax - unitrows;

			if (above && below)
				offset += shift;
		};

		UI::get().remove_textfield();
	}

	void UINpcTalk::draw(float inter) const
	{
		Point<int16_t> drawpos = position;
		top.draw(drawpos);
		drawpos.shift_y(top.height());
		fill.draw(DrawArgument(drawpos, Point<int16_t>(0, height)));
		drawpos.shift_y(height);
		bottom.draw(drawpos);
		drawpos.shift_y(bottom.height());

		UIElement::draw(inter);

		int16_t speaker_y = (top.height() + height + bottom.height()) / 2;
		Point<int16_t> speaker_pos = position + Point<int16_t>(22, 11 + speaker_y);
		Point<int16_t> center_pos = speaker_pos + Point<int16_t>(nametag.width() / 2, 0);

		// 立绘不镜像：原生参考帧里 希娜(Npc/0002101/stand/0) 原图就是朝右、面向正文的，
		// 之前恒传 flip=true 会把她翻成背对正文。DrawArgument 的 bool 位就是 flip。
		speaker.draw(DrawArgument(center_pos));
		nametag.draw(speaker_pos);
		name.draw(center_pos + Point<int16_t>(0, -4));

		if (show_slider)
		{
			int16_t text_min_height = position.y() + top.height() - 1;
			// 每翻一页恰好滚动一屏（可见高 = height - TEXT_SLACK）。原来是固定 400px，
			// 而可见区最多 248-18=230px，翻页会把中间约 170px 的正文直接跳过去。
			text.draw(position + Point<int16_t>(TEXT_X, text_draw_y - offset * (height - TEXT_SLACK)), Range<int16_t>(text_min_height, text_min_height + height - TEXT_SLACK));
			slider.draw(position);
		}
		else
		{
			// 正文 Y：IDA sub_9A64FC 按 content 高度居中（见 change_text 写入的 text_draw_y）。
			text.draw(position + Point<int16_t>(TEXT_X, text_draw_y));
		}

		// SENDSIMPLE 选中态：IDA sub_9A3E9F 在匹配 list[cur] 的正文元素下画
		// 黑 1px 实线 + #919191 虚线（3px 实 / 3px 空），宽 = 标签宽+8。
		if (type == TalkType::SENDSIMPLE
			&& !simple_choices.empty()
			&& simple_selected >= 0
			&& static_cast<size_t>(simple_selected) < simple_choices.size())
		{
			const SimpleChoice& c = simple_choices[static_cast<size_t>(simple_selected)];
			const int16_t scroll = show_slider ? offset * (height - TEXT_SLACK) : 0;
			const int16_t y = static_cast<int16_t>(text_draw_y + c.pos.y() - scroll);
			const int16_t band_top = top.height() - 1;
			const int16_t band_bottom = static_cast<int16_t>(band_top + height - TEXT_SLACK);
			const bool visible = !show_slider
				|| (y >= band_top && y + c.size.y() <= band_bottom);

			if (visible && c.size.x() > 0)
			{
				const int16_t ux = static_cast<int16_t>(position.x() + TEXT_X + c.pos.x());
				const int16_t uy = static_cast<int16_t>(position.y() + y + c.size.y());
				const int16_t uw = static_cast<int16_t>(c.size.x() + SIMPLE_UL_EXT);
				ColorBox(uw, 1, Color::Name::BLACK, 1.0f)
					.draw(DrawArgument(Point<int16_t>(ux, uy)));
				const Color dash(SIMPLE_DASH_R, SIMPLE_DASH_G, SIMPLE_DASH_B, 255);
				for (int16_t dx = 0; dx + 3 <= uw; dx = static_cast<int16_t>(dx + 6))
				{
					ColorBox(3, 1, dash, 1.0f)
						.draw(DrawArgument(Point<int16_t>(
							static_cast<int16_t>(ux + dx), uy)));
				}
			}
		}

		// GetText/GetNumber 输入槽：
		//   非 Avatar：IDA sub_9A83F6 黑框(-16777216) + 白底(-1) + Edit；
		//   Avatar：IDA sub_9A8716 仅 Edit（无 DrawRect 黑白框）。
		if (input_active)
		{
			if (!input_avatar_layout)
			{
				const int16_t dlg_w = top.width() > 0 ? top.width() : static_cast<int16_t>(529);
				const int16_t ileft = input_content_left();
				const int16_t bx = static_cast<int16_t>(position.x() + ileft - 1);
				const int16_t by = static_cast<int16_t>(position.y() + input_box_y);
				const int16_t bw = static_cast<int16_t>(dlg_w - 2 * ileft + 3);
				const int16_t wx = static_cast<int16_t>(position.x() + ileft);
				const int16_t wy = static_cast<int16_t>(by + 1);
				const int16_t ww = static_cast<int16_t>(dlg_w - 2 * ileft + 1);
				ColorBox(bw, INPUT_BLACK_H, Color::Name::BLACK, 1.0f)
					.draw(DrawArgument(Point<int16_t>(bx, by)));
				ColorBox(ww, INPUT_WHITE_H, Color::Name::WHITE, 1.0f)
					.draw(DrawArgument(Point<int16_t>(wx, wy)));
			}
			input_field.draw(Point<int16_t>(0, 0));
		}
	}

	void UINpcTalk::update()
	{
		UIElement::update();

		if (input_active)
			input_field.update();

		if (draw_text)
		{
			if (timestep > 4)
			{
				if (formatted_text_pos < formatted_text.size())
				{
					std::string t = text.get_text();
					char c = formatted_text[formatted_text_pos];

					text.change_text(t + c);

					formatted_text_pos++;
					timestep = 0;
				}
				else
				{
					draw_text = false;
				}
			}
			else
			{
				timestep++;
			}
		}
	}

	int8_t UINpcTalk::wire_msgtype() const
	{
		if (type == TalkType::SENDNEXT || type == TalkType::SENDNEXTPREV || type == TalkType::SENDPREV)
			return static_cast<int8_t>(TalkType::SENDOK);

		return msgtype_wire;
	}

	Button::State UINpcTalk::button_pressed(uint16_t buttonid)
	{
		// GetText/GetNumber 点 OK：先校验；失败则保持开窗（对齐 IDA sub_9AB50A 弹 tip 不收口）。
		const bool submit_input = input_active
			&& buttonid == Buttons::OK
			&& (type == TalkType::SENDGETTEXT || type == TalkType::SENDGETNUMBER);
		if (submit_input)
		{
			if (!dispatch_input_field())
			{
				LOG(LOG_DEBUG, "UINpcTalk: input OK reject; keep dialog");
				return Button::State::NORMAL;
			}
			clear_input_field();
			deactivate();
			return Button::State::NORMAL;
		}

		clear_input_field();
		deactivate();

		int8_t lastmsg = wire_msgtype();

		switch (type)
		{
			case TalkType::SENDNEXT:
			case TalkType::SENDOK:
			{
				switch (buttonid)
				{
					case Buttons::CLOSE:
						NpcTalkMorePacket(lastmsg, -1).dispatch();
						break;
					case Buttons::NEXT:
					case Buttons::OK:
						NpcTalkMorePacket(lastmsg, 1).dispatch();
						break;
				}

				break;
			}
			case TalkType::SENDPREV:
			{
				switch (buttonid)
				{
					case Buttons::CLOSE:
						NpcTalkMorePacket(lastmsg, -1).dispatch();
						break;
					case Buttons::PREV:
						NpcTalkMorePacket(lastmsg, 0).dispatch();
						break;
				}

				break;
			}
			case TalkType::SENDNEXTPREV:
			{
				switch (buttonid)
				{
					case Buttons::CLOSE:
						NpcTalkMorePacket(lastmsg, -1).dispatch();
						break;
					case Buttons::NEXT:
						NpcTalkMorePacket(lastmsg, 1).dispatch();
						break;
					case Buttons::PREV:
						NpcTalkMorePacket(lastmsg, 0).dispatch();
						break;
				}

				break;
			}
			case TalkType::SENDYESNO:
			{
				switch (buttonid)
				{
					case Buttons::CLOSE:
						NpcTalkMorePacket(lastmsg, -1).dispatch();
						break;
					case Buttons::NO:
					case Buttons::QNO:
					case Buttons::QCNO:
					case Buttons::QGIVEUP:
						NpcTalkMorePacket(lastmsg, 0).dispatch();
						break;
					case Buttons::YES:
					case Buttons::QYES:
					case Buttons::QCYES:
					case Buttons::QSTART:
						NpcTalkMorePacket(lastmsg, 1).dispatch();
						break;
				}

				break;
			}
			case TalkType::SENDACCEPTDECLINE:
			{
				switch (buttonid)
				{
					case Buttons::CLOSE:
						NpcTalkMorePacket(lastmsg, -1).dispatch();
						break;
					case Buttons::QNO:
					case Buttons::QCNO:
					case Buttons::QGIVEUP:
					case Buttons::NO:
						NpcTalkMorePacket(lastmsg, 0).dispatch();
						break;
					case Buttons::QYES:
					case Buttons::QCYES:
					case Buttons::QSTART:
					case Buttons::QAFTER:
					case Buttons::YES:
						NpcTalkMorePacket(lastmsg, 1).dispatch();
						break;
				}

				break;
			}
			case TalkType::SENDGETTEXT:
			case TalkType::SENDGETNUMBER:
			{
				// OK 已在入口经 dispatch_input_field 收口；此处仅处理关闭。
				if (buttonid == Buttons::CLOSE)
					NpcTalkMorePacket(lastmsg, 0).dispatch();
				break;
			}
			case TalkType::SENDSIMPLE:
			{
				switch (buttonid)
				{
					case Buttons::CLOSE:
						// 原生 BtClose id=2 → 收口取消；本端仍发 response=0
						NpcTalkMorePacket(lastmsg, 0).dispatch();
						break;
					case Buttons::NEXT:
						// IDA sub_9AB92A case dlgType==4：BtNext(8193) 取当前高亮项
						// selection 后收口。点 #L 直发不变；下项钮走同一封包。
						if (!dispatch_simple_selection())
						{
							// 无选项时下项等同关闭，避免「看得见点了没反应」
							NpcTalkMorePacket(lastmsg, 0).dispatch();
						}
						break;
					default:
						break;
				}

				break;
			}
			default:
			{
				break;
			}
		}

		return Button::State::NORMAL;
	}

	Cursor::State UINpcTalk::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Point<int16_t> cursor_relative = cursorpos - position;

		if (input_active)
		{
			Cursor::State istate = input_field.send_cursor(cursorpos, clicked);
			if (istate != Cursor::State::IDLE)
				return istate;
		}

		if (type == TalkType::SENDSIMPLE && !simple_choices.empty())
		{
			// 选项随正文一起滚动，命中框必须跟着同一套偏移；否则翻页后点的是
			// 屏幕上的空位，真正被选中的是另一行。
			const int16_t scroll = show_slider ? offset * (height - TEXT_SLACK) : 0;
			const int16_t band_top = top.height() - 1;
			const int16_t band_bottom = band_top + height - TEXT_SLACK;

			for (size_t i = 0; i < simple_choices.size(); ++i)
			{
				const SimpleChoice& c = simple_choices[i];
				const int16_t y = text_draw_y + c.pos.y() - scroll;

				// 滚出可视带的选项不可点（绘制也被裁掉了）。
				if (show_slider && (y < band_top || y + c.size.y() > band_bottom))
					continue;

				Rectangle<int16_t> box(
					TEXT_X + c.pos.x(),
					TEXT_X + c.pos.x() + c.size.x(),
					y,
					y + c.size.y());
				if (box.contains(cursor_relative))
				{
					// 悬停即高亮索引，供「下项」钮提交（对齐原生 list[cur]）
					simple_selected = static_cast<int32_t>(i);
					if (clicked)
					{
						deactivate();
						NpcTalkMorePacket(c.selection).dispatch();
						LOG(LOG_DEBUG, "UINpcTalk: SENDSIMPLE click sel=" << c.selection);
					}
					return Cursor::State::CANCLICK;
				}
			}
		}

		if (show_slider && slider.isenabled())
			if (Cursor::State sstate = slider.send_cursor(cursor_relative, clicked))
				return sstate;

		Cursor::State estate = UIElement::send_cursor(clicked, cursorpos);

		if (estate == Cursor::State::CLICKING && clicked && draw_text)
		{
			draw_text = false;
			text.change_text(formatted_text);
		}

		return estate;
	}

	void UINpcTalk::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape)
		{
			clear_input_field();
			deactivate();

			NpcTalkMorePacket(wire_msgtype(), 0).dispatch();
			return;
		}
		if (!pressed) return;

		// UI 路由传入的是 KeyAction::Id（见 UI.cpp），不是 GLFW 原始键码。
		if (type == TalkType::SENDGETTEXT || type == TalkType::SENDGETNUMBER)
		{
			if (keycode == KeyAction::Id::RETURN)
			{
				if (dispatch_input_field())
				{
					clear_input_field();
					deactivate();
				}
				return;
			}
		}

		if (type == TalkType::SENDSIMPLE)
		{
			if (keycode == KeyAction::Id::UP)
			{
				if (simple_selected > 0)
					--simple_selected;
				return;
			}
			if (keycode == KeyAction::Id::DOWN)
			{
				if (!simple_choices.empty()
					&& simple_selected + 1 < static_cast<int32_t>(simple_choices.size()))
					++simple_selected;
				return;
			}
			// 回车：等同下项钮（IDA sub_9AC0B6 Enter → 提交 list[cur]）
			if (keycode == KeyAction::Id::RETURN)
			{
				if (dispatch_simple_selection())
				{
					clear_input_field();
					deactivate();
				}
				return;
			}
		}
	}

	UIElement::Type UINpcTalk::get_type() const
	{
		return TYPE;
	}

	UINpcTalk::TalkType UINpcTalk::get_by_value(int8_t value)
	{
		switch (value)
		{
			case 0: return TalkType::SENDOK;
			case 1: return TalkType::SENDYESNO;
			case 2: return TalkType::SENDGETTEXT;
			case 3: return TalkType::SENDGETNUMBER;
			case 4: return TalkType::SENDSIMPLE;
			case 12: return TalkType::SENDACCEPTDECLINE;
			default: return TalkType::NONE;
		}
	}

	UINpcTalk::TalkType UINpcTalk::resolve_say_style(int16_t style) const
	{
		// endBytes as little-endian short: low = prev, high = next
		bool prev = (style & 0xFF) != 0;
		bool next = ((style >> 8) & 0xFF) != 0;

		if (prev && next)
			return TalkType::SENDNEXTPREV;

		if (next)
			return TalkType::SENDNEXT;

		if (prev)
			return TalkType::SENDPREV;

		return TalkType::SENDOK;
	}

	std::string UINpcTalk::format_text(const std::string& tx, const int32_t& npcid)
	{
		std::string formatted_text = tx;
		size_t begin = formatted_text.find("#p");

		if (begin != std::string::npos)
		{
			size_t end = formatted_text.find("#", begin + 1);

			if (end != std::string::npos)
			{
				std::string namestr = nl::nx::String["Npc.img"][std::to_string(npcid)]["name"];
				formatted_text.replace(begin, end - begin, namestr);
			}
		}

		begin = formatted_text.find("#h");

		if (begin != std::string::npos)
		{
			size_t end = formatted_text.find("#", begin + 1);

			if (end != std::string::npos)
			{
				std::string charstr = Stage::get().get_player().get_name();
				formatted_text.replace(begin, end - begin, charstr);
			}
		}

		begin = formatted_text.find("#t");

		if (begin != std::string::npos)
		{
			size_t end = formatted_text.find("#", begin + 1);

			if (end != std::string::npos)
			{
				size_t b = begin + 2;
				try
				{
					int32_t itemid = std::stoi(formatted_text.substr(b, end - b));
					std::string itemname = nl::nx::String["Consume.img"][itemid]["name"];
					formatted_text.replace(begin, end - begin, itemname);
				}
				catch (const std::exception&)
				{
					// 非法 #t 令牌：保留原文，避免 NPC 对话路径抛异常崩溃。
				}
			}
		}

		// 令牌剥除放在这里，而不是交给布局器：打字机逐字符喂的就是这份文本，
		// 布局器只负责排版，中途出现半截 #L 会画成乱码。
		return strip_simple_choices(formatted_text);
	}

	bool UINpcTalk::dispatch_simple_selection()
	{
		if (simple_choices.empty())
			return false;

		const size_t idx = (simple_selected >= 0
			&& static_cast<size_t>(simple_selected) < simple_choices.size())
			? static_cast<size_t>(simple_selected)
			: 0;
		const int32_t sel = simple_choices[idx].selection;
		NpcTalkMorePacket(sel).dispatch();
		LOG(LOG_DEBUG, "UINpcTalk: SENDSIMPLE next/confirm sel=" << sel
			<< " idx=" << idx);
		return true;
	}

	std::string UINpcTalk::strip_simple_choices(const std::string& tx)
	{
		simple_choices.clear();
		simple_selected = 0;

		std::string out;
		out.reserve(tx.size());

		size_t pos = 0;

		while (true)
		{
			size_t l = tx.find("#L", pos);

			if (l == std::string::npos)
			{
				out.append(tx, pos, std::string::npos);
				break;
			}

			size_t num_end = tx.find('#', l + 2);

			if (num_end == std::string::npos)
			{
				out.append(tx, pos, std::string::npos);
				break;
			}

			int32_t sel;

			try
			{
				sel = std::stoi(tx.substr(l + 2, num_end - l - 2));
			}
			catch (const std::exception&)
			{
				// 非法 #L 令牌：原样保留，不吞掉正文，也不留半个令牌。
				out.append(tx, pos, num_end + 1 - pos);
				pos = num_end + 1;
				continue;
			}

			size_t label_begin = num_end + 1;
			size_t label_end = tx.find("#l", label_begin);
			const size_t next_l = tx.find("#L", label_begin);

			// 规范写法是 #L<n>#label#l，但移植过来的原生脚本大量写成
			// "#L0#选项\r\n#L1#选项"——**没有闭合 #l**（全服 779 段含 #L 的字面量里
			// 残留令牌几乎都来自这一类）。这类标签的右界取「下一个 #L / 断行 / 文本末尾」
			// 里最早的那个；否则要么整段正文被当成一个标签吞掉（命中框横跨全文），
			// 要么 #L 令牌原样漏进布局器画成乱码。
			const bool terminated = label_end != std::string::npos
				&& (next_l == std::string::npos || label_end < next_l);

			if (!terminated)
			{
				label_end = tx.size();

				if (next_l != std::string::npos && next_l < label_end)
					label_end = next_l;

				const size_t brk = tx.find_first_of("\r\n", label_begin);

				if (brk != std::string::npos && brk < label_end)
					label_end = brk;
			}

			out.append(tx, pos, l - pos);

			if (label_end > label_begin)
			{
				SimpleChoice c;
				c.selection = sel;
				c.first = out.size();
				out.append(tx, label_begin, label_end - label_begin);
				c.last = out.size();
				simple_choices.push_back(c);
			}

			pos = terminated ? label_end + 2 : label_end;
		}

		return out;
	}

	void UINpcTalk::layout_simple_choices(const Text& body)
	{
		for (SimpleChoice& c : simple_choices)
		{
			c.pos = body.position_of(c.first);

			// 宽高都另用量：正文是整段折行的结果，拿不到「只有这一段」的边界。
			// maxwidth=0 ⇒ 不折行，量出的就是标签的自然宽；色标由同一段 Text
			// 自己解释，所以 #e 加粗之类的宽度也对得上。
			Text label(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::DARKGREY,
				formatted_text.substr(c.first, c.last - c.first));

			// 高度取布局自身而不是单行行距：带闭合 #l 的标签可以合法跨行
			// （npc/2040030.js 的「我的宠物变成了玩偶\r\n请帮我让它再次活动起来！」），
			// 写死一行高会让下半截点不到。
			c.size = Point<int16_t>(label.width(), label.height());
		}
	}

	void UINpcTalk::apply_dlg_skin(int8_t speaker)
	{
		const char* node = "UtilDlgEx";
		if (speaker == 1)
			node = "UtilDlgEx_Avatar";
		else if (speaker == 2)
			node = "UtilDlgEx_Pet";

		nl::node root = UiAssetResolve::window(node);
		if (!UiAssetResolve::usable(root))
			root = UiAssetResolve::window("UtilDlgEx");
		if (!UiAssetResolve::usable(root))
			return;

		if (root["t"]) top = root["t"];
		if (root["c"]) fill = root["c"];
		if (root["s"]) bottom = root["s"];
		if (root["bar"]) nametag = root["bar"];
		min_height = 8 * fill.height() + 14;
		LOG(LOG_DEBUG, "UINpcTalk: skin=" << node << " speaker=" << static_cast<int>(speaker));
	}

	void UINpcTalk::change_text(int32_t npcid, int8_t msgtype, int16_t style, int8_t speakerbyte, const std::string& tx)
	{
		change_text(npcid, msgtype, style, speakerbyte, tx, TalkInputParams{});
	}

	void UINpcTalk::change_text(int32_t npcid, int8_t msgtype, int16_t style, int8_t speakerbyte, const std::string& tx,
		const TalkInputParams& input)
	{
		apply_dlg_skin(speakerbyte);
		dlg_speaker = speakerbyte;
		input_params = input;
		// speaker==1 → UtilDlgEx_Avatar 皮肤；布局走 sub_9A8716（IDA a1[41] 非零）。
		input_avatar_layout = (speakerbyte == 1)
			&& (get_by_value(msgtype) == TalkType::SENDGETTEXT
				|| get_by_value(msgtype) == TalkType::SENDGETNUMBER);

		msgtype_wire = msgtype;
		type = get_by_value(msgtype);

		if (type == TalkType::SENDOK)
			type = resolve_say_style(style);

		timestep = 0;
		draw_text = true;
		formatted_text_pos = 0;
		formatted_text = format_text(tx, npcid);

		// 折行宽度 = 原生三帧量出来的正文块右缘(474..476) - TEXT_X(166)。
		// 原来 maxwidth=320 起笔在 x162，正文右缘铺到 x482，和右下角「确定」(474..520) 叠住。
		text = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::DARKGREY, formatted_text, TEXT_WIDTH);

		text_block_height = text.height();
		const int16_t text_height = text_block_height;

		// 命中框必须在正文被清空前反取：下面 change_text("") 会把布局丢掉，
		// 而选项的落点只存在于整段文本的折行结果里。
		layout_simple_choices(text);

		text.change_text("");

		if (speakerbyte == 0)
		{
			// 与 Npc.cpp 对齐用 extend_id：原来的 7 - strid.size() 遇到 8 位 npcid 会无符号下溢，
			// insert(0, 巨值, '0') 抛 length_error，整条对话直接打不开。
			speaker = nl::nx::Npc[string_format::extend_id(npcid, 7) + ".img"]["stand"]["0"];

			std::string namestr = nl::nx::String["Npc.img"][std::to_string(npcid)]["name"];
			name.change_text(namestr);
		}
		else
		{
			speaker = Texture();
			name.change_text("");
		}

		height = min_height;
		show_slider = false;

		if (text_height > height)
		{
			if (text_height > MAX_HEIGHT)
			{
				height = MAX_HEIGHT;
				show_slider = true;
				rowmax = text_height / (MAX_HEIGHT - TEXT_SLACK) + 1;
				unitrows = 1;

				int16_t slider_y = top.height() - 7;
				slider = Slider(Slider::Type::DEFAULT_SILVER, Range<int16_t>(slider_y, slider_y + height - 20), top.width() - 26, unitrows, rowmax, onmoved);
			}
			else
			{
				height = text_height;
			}
		}

		// IDA sub_9A64FC：装得下 → (content_h - text_h)/2 + t.height + 7；滑条 → 贴顶 +7。
		if (show_slider)
			text_draw_y = static_cast<int16_t>(top.height() + TEXT_Y_PAD);
		else
			text_draw_y = static_cast<int16_t>(
				top.height() + (height - text_block_height) / 2 + TEXT_Y_PAD);

		// GetText/GetNumber：在正文下方留输入栈，正文在剩余区内再居中。
		//   非 Avatar：IDA sub_9A83F6（INPUT_STACK）；Avatar：sub_9A8716 固定带。
		input_box_y = 0;
		if (type == TalkType::SENDGETTEXT || type == TalkType::SENDGETNUMBER)
		{
			const int16_t stack = input_avatar_layout ? INPUT_AVATAR_STACK : INPUT_STACK;
			const int16_t need = static_cast<int16_t>(text_block_height + stack);
			if (need > height)
				height = need;
			const int16_t text_area = static_cast<int16_t>(height - stack);
			if (text_area > text_block_height)
				text_draw_y = static_cast<int16_t>(
					top.height() + (text_area - text_block_height) / 2 + TEXT_Y_PAD);
			else
				text_draw_y = static_cast<int16_t>(top.height() + TEXT_Y_PAD);
			input_box_y = static_cast<int16_t>(text_draw_y + text_block_height + 2);
		}

		clear_input_field();

		for (auto& button : buttons)
		{
			button.second->set_active(false);
			button.second->set_state(Button::State::NORMAL);
		}

		// 底带（OK/Close/Yes/No）：y = height + BTN_DY（R25 = wnd≈height+86 的 -26 校准）。
		// Next/Prev：IDA 相对 Close 高 51px ⇒ height + BTN_DY_NEXT。
		const int16_t y_bottom = static_cast<int16_t>(height + BTN_DY);
		const int16_t y_next = static_cast<int16_t>(height + BTN_DY_NEXT);
		const int16_t dlg_w = top.width() > 0 ? top.width() : static_cast<int16_t>(529);
		const int16_t edge = (dlg_speaker == 1) ? BTN_EDGE_INSET_AVATAR : BTN_EDGE_INSET;
		const int16_t x_ok = static_cast<int16_t>(dlg_w - BTN_OK_RIGHT_INSET);
		const int16_t x_next = static_cast<int16_t>(dlg_w - edge - BTN_NEXT_RIGHT_INSET);
		const int16_t x_prev = static_cast<int16_t>(dlg_w - edge - BTN_PREV_RIGHT_INSET);
		const int16_t x_close = (type == TalkType::SENDSIMPLE) ? BTN_X_CLOSE_LIST : BTN_X_CLOSE;
		const int16_t x_pair_wide = static_cast<int16_t>(BTN_X_RIGHT_WIDE - BTN_PAIR_PITCH);
		// Avatar GetText/Number：OK/Close 用 sub_9A8716 固定槽（H-30 ⇒ y_bottom-4）。
		const int16_t y_input_btn = input_avatar_layout
			? static_cast<int16_t>(y_bottom - BTN_AVATAR_Y_DELTA) : y_bottom;
		if (input_avatar_layout)
			input_box_y = static_cast<int16_t>(y_input_btn - INPUT_AVATAR_EDIT_ABOVE_OK);

		if (buttons.count(Buttons::CLOSE))
		{
			const int16_t cx = input_avatar_layout ? BTN_AVATAR_CLOSE_X : x_close;
			const int16_t cy = input_avatar_layout ? y_input_btn : y_bottom;
			buttons[Buttons::CLOSE]->set_position(Point<int16_t>(cx, cy));
			buttons[Buttons::CLOSE]->set_active(true);
		}

		auto activate = [&](uint16_t id, Point<int16_t> pos)
		{
			if (!buttons.count(id))
				return;

			buttons[id]->set_position(pos);
			buttons[id]->set_active(true);
		};

		switch (type)
		{
			case TalkType::SENDOK:
			{
				// IDA sub_9A7C8B：无 Next 时 BtOK @ (W-54, H-26)。
				activate(Buttons::OK, Point<int16_t>(x_ok, y_bottom));
				break;
			}
			case TalkType::SENDNEXT:
			{
				activate(Buttons::NEXT, Point<int16_t>(x_next, y_next));
				break;
			}
			case TalkType::SENDPREV:
			{
				activate(Buttons::PREV, Point<int16_t>(x_prev, y_next));
				break;
			}
			case TalkType::SENDNEXTPREV:
			{
				activate(Buttons::PREV, Point<int16_t>(x_prev, y_next));
				activate(Buttons::NEXT, Point<int16_t>(x_next, y_next));
				break;
			}
			case TalkType::SENDYESNO:
			{
				// IDA sub_9A8062：Yes @ W-(2*44+20)、No @ W-44-10；主操作在左。
				// 46 宽钮与 44 槽接近；Q* 回退仍走 R25 宽钮常量。
				const int16_t x_no = x_ok;
				const int16_t x_yes = static_cast<int16_t>(dlg_w - (2 * 44 + 20));
				activate(Buttons::YES, Point<int16_t>(x_yes, y_bottom));
				activate(Buttons::NO, Point<int16_t>(x_no, y_bottom));

				if (!buttons.count(Buttons::YES))
					activate(Buttons::QYES, Point<int16_t>(x_pair_wide, y_bottom));
				if (!buttons.count(Buttons::NO))
					activate(Buttons::QNO, Point<int16_t>(BTN_X_RIGHT_WIDE, y_bottom));
				break;
			}
			case TalkType::SENDACCEPTDECLINE:
			{
				Point<int16_t> yes_position = Point<int16_t>(x_pair_wide, y_bottom);
				activate(Buttons::QYES, yes_position);
				activate(Buttons::QNO, yes_position + Point<int16_t>(BTN_PAIR_PITCH, 0));

				if (!buttons.count(Buttons::QYES))
					activate(Buttons::QSTART, yes_position);
				if (!buttons.count(Buttons::QYES) && !buttons.count(Buttons::QSTART))
					activate(Buttons::QCYES, yes_position);
				if (!buttons.count(Buttons::QNO))
					activate(Buttons::QCNO, yes_position + Point<int16_t>(BTN_PAIR_PITCH, 0));
				break;
			}
			case TalkType::SENDGETNUMBER:
			case TalkType::SENDGETTEXT:
			{
				// 非 Avatar：sub_9A83F6 BtOK@(W-54,H-26)；Avatar：sub_9A8716 BtOK@(158,H-30)。
				const int16_t ok_x = input_avatar_layout ? BTN_AVATAR_OK_X : x_ok;
				activate(Buttons::OK, Point<int16_t>(ok_x, y_input_btn));
				setup_input_field();
				LOG(LOG_DEBUG, "UINpcTalk: GetText/Number OK x=" << ok_x
					<< " input_y=" << input_box_y
					<< " avatar=" << (input_avatar_layout ? 1 : 0)
					<< " bounds=" << (input_params.has_number_bounds ? 1 : 0));
				break;
			}
			case TalkType::SENDSIMPLE:
			{
				// IDA sub_9A8FEF：BtNext(8193) @ (W-edge-68, H-77) + BtClose(2) @ (10, H-26)。
				activate(Buttons::NEXT, Point<int16_t>(x_next, y_next));
				simple_selected = 0;
				LOG(LOG_DEBUG, "UINpcTalk: SENDSIMPLE activate NEXT x=" << x_next
					<< " y=" << y_next << " choices=" << simple_choices.size());
				break;
			}
			default:
			{
				break;
			}
		}

		position = Point<int16_t>(400 - top.width() / 2, 240 - height / 2);
		// 命中区 = 壳体重绘高度（t + 正文 + s）。原来写死 height + 120，比实际绘制高出 30 余px，
		// 那段空白会白白吞掉点击（点不到下层的窗口/世界）。
		dimension = Point<int16_t>(top.width(), top.height() + height + bottom.height());

		// 多分辨率：上面的 (400,240) 是 800×600 时代按中心点记的常量，高分辨率下
		// 会把对话窗钉在左上。非 1280×720 档位按视口居中；1280×720 零回归。
		reanchor_talk();
		if (input_active)
			setup_input_field();
	}

	void UINpcTalk::update_screen(int16_t, int16_t)
	{
		reanchor_talk();
		if (input_active)
			setup_input_field();
	}

	void UINpcTalk::reanchor_talk()
	{
		// UtilDlgEx 缺失时构造早退，dimension 为 0，居中会把窗口挪出视口。
		if (top.width() <= 0 || dimension.x() <= 0)
			return;

		const int16_t vieww = Constants::Constants::get().get_viewwidth();
		const int16_t viewh = Constants::Constants::get().get_viewheight();

		if (vieww == ResolutionTiers::LOGIN_W && viewh == ResolutionTiers::LOGIN_H)
		{
			// 切回 1280×720 时也要恢复原生绝对定位（不能沿用上一次的居中坐标）。
			position = Point<int16_t>(
				static_cast<int16_t>(400 - top.width() / 2),
				static_cast<int16_t>(240 - height / 2));
			return;
		}

		center_on_screen();
	}

	void UINpcTalk::setup_input_field()
	{
		if (type != TalkType::SENDGETTEXT && type != TalkType::SENDGETNUMBER)
			return;

		const int16_t dlg_w = top.width() > 0 ? top.width() : static_cast<int16_t>(529);
		int16_t edit_x;
		int16_t edit_y;
		int16_t edit_w;
		int16_t edit_h;
		if (input_avatar_layout)
		{
			// IDA sub_9A8716：Edit @ (19, H-73)，226×15
			edit_x = static_cast<int16_t>(position.x() + INPUT_AVATAR_EDIT_X);
			edit_y = static_cast<int16_t>(position.y() + input_box_y);
			edit_w = INPUT_AVATAR_EDIT_W;
			edit_h = INPUT_AVATAR_EDIT_H;
		}
		else
		{
			// IDA sub_9A83F6 / sub_9ACC20：Edit @ (left+2, …)；left = (speaker&6)?25:157
			const int16_t ileft = input_content_left();
			edit_x = static_cast<int16_t>(position.x() + ileft + 2);
			edit_y = static_cast<int16_t>(position.y() + input_box_y + 3);
			edit_w = static_cast<int16_t>(dlg_w - (2 * ileft + 2));
			edit_h = INPUT_EDIT_H;
		}
		const Point<int16_t> tl(edit_x, edit_y);
		const Point<int16_t> br(static_cast<int16_t>(edit_x + edit_w),
			static_cast<int16_t>(edit_y + edit_h));
		size_t limit = (type == TalkType::SENDGETNUMBER) ? 10u : 64u;
		if (input_params.edit_maxlen > 0)
			limit = static_cast<size_t>(input_params.edit_maxlen);
		const std::string keep = input_active ? input_field.get_text() : std::string();
		std::string initial = keep;
		if (initial.empty())
		{
			if (type == TalkType::SENDGETTEXT)
				initial = input_params.def_text;
			else if (input_params.has_number_bounds || input_params.def_number != 0)
				initial = std::to_string(input_params.def_number);
		}

		input_field = Textfield(
			Text::Font::A12M,
			Text::Alignment::LEFT,
			Color::Name::BLACK,
			Rectangle<int16_t>(tl, br),
			limit,
			12);
		input_field.set_accept_unicode(type == TalkType::SENDGETTEXT);
		input_field.set_text_shift(Point<int16_t>(1, 11));
		if (!initial.empty())
			input_field.change_text(initial);
		input_field.set_enter_callback(
			[this](std::string)
			{
				if (dispatch_input_field())
				{
					clear_input_field();
					deactivate();
				}
			});
		input_field.set_state(Textfield::State::FOCUSED);
		UI::get().focus_textfield(&input_field);
		input_active = true;
	}

	void UINpcTalk::clear_input_field()
	{
		if (!input_active && input_field.get_state() == Textfield::State::DISABLED)
			return;

		input_field.set_state(Textfield::State::DISABLED);
		UI::get().remove_textfield();
		input_active = false;
	}

	int16_t UINpcTalk::input_content_left() const
	{
		// IDA sub_9ACC20：this[441]=(speaker&6)!=0 → content_left 25，否则 157。
		// Pet speaker=2 置位；Avatar speaker=1 的 (1&6)==0 仍 157（且走 9A8716 固定槽）。
		if ((static_cast<uint8_t>(dlg_speaker) & 6u) != 0)
			return INPUT_LEFT_PET;
		return INPUT_LEFT;
	}

	void UINpcTalk::show_input_tip(const std::string& message)
	{
		// IDA sub_9AB50A 失败 → sub_9929DD（Notice tip，不关对话窗）。
		input_field.set_state(Textfield::State::DISABLED);
		UI::get().emplace<UIOk>(message, [this](bool)
		{
			if (input_active)
			{
				input_field.set_state(Textfield::State::FOCUSED);
				UI::get().focus_textfield(&input_field);
			}
			if (buttons.count(Buttons::OK))
				buttons[Buttons::OK]->set_state(Button::State::NORMAL);
		});
		LOG(LOG_DEBUG, "UINpcTalk: input tip len=" << message.size());
	}

	bool UINpcTalk::dispatch_input_field()
	{
		if (!input_active)
			return false;

		const std::string s = input_field.get_text();
		if (type == TalkType::SENDGETTEXT)
		{
			// IDA sub_9AB50A：内部 dlgType==3 时 len < this+0xDC → tip 821；线参常为 0。
			if (static_cast<int32_t>(s.size()) < input_params.text_min_len)
			{
				LOG(LOG_DEBUG, "UINpcTalk: GetText reject len=" << s.size()
					<< " min=" << input_params.text_min_len);
				show_input_tip(format_tip(TIP_821, input_params.text_min_len));
				return false;
			}
			NpcTalkMorePacket(s).dispatch();
			LOG(LOG_DEBUG, "UINpcTalk: GetText enter len=" << s.size());
			return true;
		}
		if (type == TalkType::SENDGETNUMBER)
		{
			// IDA sub_9AB50A：内部 dlgType==2 — 全数字、atoi64、min/max（this+0xD0/0xD4）。
			// tip 817 非数字；818 <min；819 >max / 越界负；空串 → atoi64=0 再走 min。
			for (unsigned char ch : s)
			{
				if (ch < '0' || ch > '9')
				{
					LOG(LOG_DEBUG, "UINpcTalk: GetNumber reject non-digit");
					show_input_tip(TIP_817);
					return false;
				}
			}
			try
			{
				const long long n64 = s.empty() ? 0LL : std::stoll(s);
				if (n64 < 0 || n64 > static_cast<long long>(INT32_MAX))
				{
					const int32_t tip_max = input_params.has_number_bounds
						? input_params.max_number : INT32_MAX;
					show_input_tip(format_tip(TIP_819, tip_max));
					return false;
				}
				const int32_t n = static_cast<int32_t>(n64);
				if (input_params.has_number_bounds)
				{
					if (n < input_params.min_number)
					{
						LOG(LOG_DEBUG, "UINpcTalk: GetNumber reject <min " << n
							<< "<" << input_params.min_number);
						show_input_tip(format_tip(TIP_818, input_params.min_number));
						return false;
					}
					if (n > input_params.max_number)
					{
						LOG(LOG_DEBUG, "UINpcTalk: GetNumber reject >max " << n
							<< ">" << input_params.max_number);
						show_input_tip(format_tip(TIP_819, input_params.max_number));
						return false;
					}
				}
				NpcTalkMorePacket(NpcTalkMorePacket::GetNumber{}, n).dispatch();
				LOG(LOG_DEBUG, "UINpcTalk: GetNumber enter=" << n);
				return true;
			}
			catch (...)
			{
				show_input_tip(TIP_817);
				return false;
			}
		}
		return false;
	}
}
