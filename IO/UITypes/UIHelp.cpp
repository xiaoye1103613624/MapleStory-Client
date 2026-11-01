//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//////////////////////////////////////////////////////////////////////////////////
#include "UIHelp.h"

#include "UIMiscShells.h"
#include "UISoftKey.h"

#include "../UI.h"

#include "../Components/MapleButton.h"
#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

#include <algorithm>
#include <functional>

namespace ms
{
	UIHelp::UIHelp()
	{
		nl::node dlg = nl::nx::UI["UIWindow.img"]["UtilDlgEx"];

		if (!UiAssetResolve::usable(dlg))
			UiAssetResolve::diag_once("uihelp frame", "UIWindow.img/UtilDlgEx missing");

		top_frame = Texture(dlg["t"]);
		bottom_frame = Texture(dlg["s"]);
		// 正文底走真素材 c（529x20，竖向平铺/拉伸），不再用纯白 ColorBox：
		// 原生这条 alpha 17..255、色值约 232,232,229 的半透明纸面，纯白明显偏亮。
		body_fill = Texture(dlg["c"]);

		dimension = Point<int16_t>(FRAME_W, TOP_H + BODY_H + BOTTOM_H);

		try_maple_btn(
			buttons,
			Buttons::BT_CLOSE,
			dlg["BtClose"],
			Point<int16_t>(CLOSE_X, CLOSE_Y),
			"uihelp close");

		rebuild();

		slider = Slider(
			Slider::Type::DEFAULT_SILVER,
			Range<int16_t>(TEXT_TOP, TEXT_TOP + BODY_H - 24),
			FRAME_W - 30,
			static_cast<int16_t>((BODY_H - 24) / LINE_H),
			static_cast<int16_t>((content_h + LINE_H - 1) / LINE_H),
			std::bind(&UIHelp::on_scroll, this, std::placeholders::_1));

		center_on_screen();
	}

	std::string UIHelp::guide_line(std::string raw)
	{
		// NptGuide 的串里换行是 IMG 脚本约定的字面量「\r\n」（反斜杠+字母），
		// LayoutBuilder 只认字面量「\n」；真 CR/LF 字节也会被当字形画出来。
		// 这里统一折叠成一个「\n」转义，其余未知转义丢掉反斜杠避免刷 unknown-format 日志。
		std::string out;
		out.reserve(raw.size() + 8);

		for (size_t i = 0; i < raw.size(); ++i)
		{
			const char c = raw[i];

			if (c != '\\')
			{
				if (c == '\r' || c == '\n')
					out += "\\n";
				else
					out += c;

				continue;
			}

			if (i + 1 >= raw.size())
				break;

			const char n = raw[i + 1];

			if (n != 'r' && n != 'R' && n != 'n' && n != 'N')
			{
				out += n;
				++i;
				continue;
			}

			if ((n == 'r' || n == 'R') && i + 3 < raw.size()
				&& raw[i + 2] == '\\' && (raw[i + 3] == 'n' || raw[i + 3] == 'N'))
				i += 3;

			out += "\\n";
			++i;
		}

		return out;
	}

	void UIHelp::rebuild()
	{
		paragraphs.clear();
		offsets.clear();
		content_h = 0;

#ifdef USE_NX
		nl::node guide = nl::nx::UI["Npt_GuideLine.img"]["NptGuide"];

		if (!UiAssetResolve::usable(guide))
			UiAssetResolve::diag_once("uihelp text", "Npt_GuideLine.img/NptGuide missing");

		const int16_t lineheight = LINE_H;

		for (int i = 0; i < 64; ++i)
		{
			const std::string key = "Text" + std::string(i < 10 ? "0" : "") + std::to_string(i);
			nl::node line = guide[key];

			if (!line)
				break;

			// IMG 串表读出来就是 UTF-8，直接交给 Font/Charset（T12b 度量）渲染。
			Text t(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::BLACK, guide_line(line.get_string()), TEXT_W);

			offsets.emplace_back(static_cast<int16_t>(content_h));
			content_h = static_cast<int16_t>(content_h + std::max<int16_t>(lineheight, t.height()));
			paragraphs.emplace_back(std::move(t));
		}
#endif

		scroll = 0;
	}

	void UIHelp::draw(float inter) const
	{
		if (top_frame)
			top_frame.draw(position);

		if (body_fill)
			body_fill.draw(DrawArgument(position + Point<int16_t>(0, TOP_H), Point<int16_t>(0, BODY_H)));

		if (bottom_frame)
			bottom_frame.draw(position + Point<int16_t>(0, TOP_H + BODY_H));

		const Range<int16_t> clip(
			static_cast<int16_t>(position.y() + TEXT_TOP),
			static_cast<int16_t>(position.y() + TEXT_TOP + BODY_H - 12));

		for (size_t i = 0; i < paragraphs.size(); ++i)
		{
			const int16_t y = static_cast<int16_t>(offsets[i] - scroll * LINE_H + TEXT_TOP);

			paragraphs[i].draw(DrawArgument(position + Point<int16_t>(TEXT_X, y)), clip);
		}

		UIElement::draw(inter);
		slider.draw(position);
	}

	void UIHelp::on_scroll(bool upwards)
	{
		const int16_t visiblerows = static_cast<int16_t>((BODY_H - 24) / LINE_H);
		const int16_t maxscroll = std::max<int16_t>(0, static_cast<int16_t>((content_h + LINE_H - 1) / LINE_H) - visiblerows);

		scroll = std::max<int16_t>(0, std::min<int16_t>(maxscroll, static_cast<int16_t>(scroll + (upwards ? -1 : 1))));
	}

	Cursor::State UIHelp::send_cursor(bool clicked, Point<int16_t> cursor_position)
	{
		if (slider.isenabled())
		{
			const Cursor::State sstate = slider.send_cursor(cursor_position - position, clicked);

			if (sstate != Cursor::State::IDLE)
				return sstate;
		}

		return UIElement::send_cursor(clicked, cursor_position);
	}

	void UIHelp::send_scroll(double yoffset)
	{
		slider.send_scroll(yoffset);
	}

	void UIHelp::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && (escape || keycode == KeyAction::Id::RETURN))
		{
			deactivate();
			return;
		}
		if (!pressed) return;
		if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't')
		{
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				if (tip.empty()) return;
				bool all_digit = true;
				for (char c : tip) if (c < '0' || c > '9') { all_digit = false; break; }
				if (all_digit) {
					try {
						NpcTalkMorePacket(std::stoi(tip)).dispatch();
					} catch (...) {}
				} else {
					NpcTalkMorePacket(tip).dispatch();
				}
			}, []() {}, "Help / NPC reply");
		}
		else if (keycode == 'G' || keycode == 'g')
		{
			auto ui = UI::get().get_element<UIGMBoard>();
			if (!ui) ui = UI::get().emplace<UIGMBoard>();
			else if (!ui->is_active()) ui->makeactive();
		}
		else if (keycode == 'C' || keycode == 'c')
		{
			auto ui = UI::get().get_element<UIClaim>();
			if (!ui) ui = UI::get().emplace<UIClaim>();
			else if (!ui->is_active()) ui->makeactive();
		}
	}

	void UIHelp::update_screen(int16_t, int16_t)
	{
		center_on_screen();
	}

	UIElement::Type UIHelp::get_type() const
	{
		return TYPE;
	}

	Button::State UIHelp::button_pressed(uint16_t)
	{
		deactivate();
		return Button::State::NORMAL;
	}
}
