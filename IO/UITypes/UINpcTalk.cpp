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

#include "../UI.h"

#include "UISoftKey.h"

#include "../Components/MapleButton.h"

#include "../../Gameplay/Stage.h"

#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#include "../../MapleStory.h"

#include <cctype>
#include <map>
#include <memory>
#include <stdexcept>

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
	}

	UINpcTalk::UINpcTalk() :
		offset(0),
		unitrows(0),
		rowmax(0),
		show_slider(false),
		draw_text(false),
		formatted_text(""),
		formatted_text_pos(0),
		timestep(0),
		type(TalkType::NONE),
		msgtype_wire(0)
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
			text.draw(position + Point<int16_t>(TEXT_X, TEXT_Y - offset * (height - TEXT_SLACK)), Range<int16_t>(text_min_height, text_min_height + height - TEXT_SLACK));
			slider.draw(position);
		}
		else
		{
			// 正文顶对齐（原生即如此）：原来用 48 - (height - min_height) 让窗口一拉长就整体上移，
			// 结果是长文本反而往上贴到窗框、往下压住「确定」。
			text.draw(position + Point<int16_t>(TEXT_X, TEXT_Y));
		}

		for (const SimpleChoice& c : simple_choices)
			c.label.draw(position + c.pos);
	}

	void UINpcTalk::update()
	{
		UIElement::update();

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
			{
				switch (buttonid)
				{
					case Buttons::CLOSE:
						NpcTalkMorePacket(lastmsg, 0).dispatch();
						break;
					case Buttons::OK:
						UI::get().emplace<UISoftKey>([this](const std::string& s) {
							if (s.empty()) return;
							deactivate();
							NpcTalkMorePacket(s).dispatch();
							LOG(LOG_DEBUG, "UINpcTalk: SoftKey GetText len=" << s.size());
						}, []() {}, "Npc text");
						break;
				}
				break;
			}
			case TalkType::SENDGETNUMBER:
			{
				switch (buttonid)
				{
					case Buttons::CLOSE:
						NpcTalkMorePacket(lastmsg, 0).dispatch();
						break;
					case Buttons::OK:
						UI::get().emplace<UISoftKey>([this](const std::string& s) {
							try {
								int32_t n = std::stoi(s);
								deactivate();
								NpcTalkMorePacket(NpcTalkMorePacket::GetNumber{}, n).dispatch();
								LOG(LOG_DEBUG, "UINpcTalk: SoftKey GetNumber=" << n);
							} catch (...) {}
						}, []() {}, "Npc number");
						break;
				}

				break;
			}
			case TalkType::SENDSIMPLE:
			{
				switch (buttonid)
				{
					case Buttons::CLOSE:
						NpcTalkMorePacket(lastmsg, 0).dispatch();
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

		if (type == TalkType::SENDSIMPLE && !simple_choices.empty())
		{
			for (const SimpleChoice& c : simple_choices)
			{
				Rectangle<int16_t> box(
					c.pos.x(),
					c.pos.x() + c.label.width(),
					c.pos.y(),
					c.pos.y() + c.label.height());
				if (box.contains(cursor_relative))
				{
					if (clicked)
					{
						deactivate();
						NpcTalkMorePacket(c.selection).dispatch();
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
			deactivate();

			NpcTalkMorePacket(wire_msgtype(), 0).dispatch();
			return;
		}
		if (!pressed) return;
		if (type == TalkType::SENDGETTEXT && (keycode == 'T' || keycode == 't' || keycode == 'S' || keycode == 's'))
		{
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				if (s.empty()) return;
				deactivate();
				NpcTalkMorePacket(s).dispatch();
			}, []() {}, "Npc text");
		}
		else if (type == TalkType::SENDGETNUMBER && (keycode == 'N' || keycode == 'n' || keycode == 'S' || keycode == 's'))
		{
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					int32_t n = std::stoi(s);
					deactivate();
					NpcTalkMorePacket(NpcTalkMorePacket::GetNumber{}, n).dispatch();
				} catch (...) {}
			}, []() {}, "Npc number");
		}
		else if (type == TalkType::SENDSIMPLE && (keycode == 'S' || keycode == 's'))
		{
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					int32_t sel = std::stoi(s);
					deactivate();
					NpcTalkMorePacket(sel).dispatch();
				} catch (...) {}
			}, []() {}, "Npc select");
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

		return formatted_text;
	}

	void UINpcTalk::parse_simple_choices(const std::string& tx)
	{
		simple_choices.clear();
		// 形如 #L0#选项A# 或 #L0#选项A#L1#选项B#
		size_t pos = 0;
		int16_t row = 0;
		while ((pos = tx.find("#L", pos)) != std::string::npos)
		{
			size_t num_begin = pos + 2;
			size_t num_end = tx.find('#', num_begin);
			if (num_end == std::string::npos)
				break;
			size_t label_end = tx.find('#', num_end + 1);
			std::string num = tx.substr(num_begin, num_end - num_begin);
			std::string label = label_end == std::string::npos
				? tx.substr(num_end + 1)
				: tx.substr(num_end + 1, label_end - num_end - 1);
			int32_t sel = 0;
			try
			{
				sel = std::stoi(num);
			}
			catch (const std::exception&)
			{
				pos = num_end + 1;
				continue;
			}
			SimpleChoice c;
			c.selection = sel;
			c.label = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::LIGHTBLUE, label);
			c.pos = Point<int16_t>(TEXT_X, 70 + row * 18);
			simple_choices.push_back(std::move(c));
			++row;
			pos = label_end == std::string::npos ? tx.size() : label_end;
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
		apply_dlg_skin(speakerbyte);

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

		int16_t text_height = text.height();

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

		for (auto& button : buttons)
		{
			button.second->set_active(false);
			button.second->set_state(Button::State::NORMAL);
		}

		int16_t y_cord = height + BTN_DY;

		if (buttons.count(Buttons::CLOSE))
		{
			buttons[Buttons::CLOSE]->set_position(Point<int16_t>(BTN_X_CLOSE, y_cord));
			buttons[Buttons::CLOSE]->set_active(true);
		}

		auto activate = [&](uint16_t id, Point<int16_t> pos)
		{
			if (!buttons.count(id))
				return;

			buttons[id]->set_position(pos);
			buttons[id]->set_active(true);
		};

		const int16_t x_right = BTN_X_RIGHT;
		const int16_t x_pair = static_cast<int16_t>(BTN_X_RIGHT - BTN_PAIR_PITCH);
		const int16_t x_pair_wide = static_cast<int16_t>(BTN_X_RIGHT_WIDE - BTN_PAIR_PITCH);

		switch (type)
		{
			case TalkType::SENDOK:
			{
				activate(Buttons::OK, Point<int16_t>(x_right, y_cord));
				break;
			}
			case TalkType::SENDNEXT:
			{
				activate(Buttons::NEXT, Point<int16_t>(x_right, y_cord));
				break;
			}
			case TalkType::SENDPREV:
			{
				activate(Buttons::PREV, Point<int16_t>(x_pair, y_cord));
				break;
			}
			case TalkType::SENDNEXTPREV:
			{
				activate(Buttons::PREV, Point<int16_t>(x_pair, y_cord));
				activate(Buttons::NEXT, Point<int16_t>(x_right, y_cord));
				break;
			}
			case TalkType::SENDYESNO:
			{
				// 原生顺序：主操作在左、次操作在右（莎丽的 接受/拒绝 实测 401 / 466，间距 65）。
				Point<int16_t> yes_position = Point<int16_t>(x_pair, y_cord);
				activate(Buttons::YES, yes_position);
				activate(Buttons::NO, yes_position + Point<int16_t>(BTN_PAIR_PITCH, 0));

				// Fall back to quest-styled yes/no if normal buttons are missing.
				if (!buttons.count(Buttons::YES))
					activate(Buttons::QYES, Point<int16_t>(x_pair_wide, y_cord));
				if (!buttons.count(Buttons::NO))
					activate(Buttons::QNO, Point<int16_t>(BTN_X_RIGHT_WIDE, y_cord));
				break;
			}
			case TalkType::SENDACCEPTDECLINE:
			{
				Point<int16_t> yes_position = Point<int16_t>(x_pair_wide, y_cord);
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
			{
				activate(Buttons::OK, Point<int16_t>(x_right, y_cord));
				break;
			}
			case TalkType::SENDSIMPLE:
			case TalkType::SENDGETTEXT:
			default:
			{
				break;
			}
		}

		position = Point<int16_t>(400 - top.width() / 2, 240 - height / 2);
		// 命中区 = 壳体重绘高度（t + 正文 + s）。原来写死 height + 120，比实际绘制高出 30 余px，
		// 那段空白会白白吞掉点击（点不到下层的窗口/世界）。
		dimension = Point<int16_t>(top.width(), top.height() + height + bottom.height());
	}
}
