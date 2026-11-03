#include "UIViewAllChar.h"

#include <nlnx/nx.hpp>
#include <string>

#include "UISoftKey.h"

#include "../UIStateLogin.h"
#include "../UI.h"
#include "../KeyAction.h"
#include "../../Util/Resource/ImgSource.h"
#include "../Components/MapleButton.h"
#include "../../Net/Login.h"
#include "../../Net/Packets/SelectCharPackets.h"
#include "../../Util/Misc.h"
#include "../../Util/Locale.h"

namespace ms
{
	UIViewAllChar::UIViewAllChar() : UIElement(Point<int16_t>(0, 0), Point<int16_t>(800, 600)), selected(-1), sel_cid(-1), sel_use_pic(false)
	{
		nl::node vac = nl::nx::UI["Login.img"]["ViewAllChar"];

		// 背景：backgrnd 是容器节点，含 0=顶栏(579x37) + 1/2/3=主体变体(582x440/444/447)。
		// 直接 Sprite(vac["backgrnd"]) 传容器不会渲染，须取具体位图（顶栏 + 主体）。
		if (vac["backgrnd"]["0"]) sprites.emplace_back(Sprite(vac["backgrnd"]["0"], Point<int16_t>(0, 0)));
		if (vac["backgrnd"]["1"]) sprites.emplace_back(Sprite(vac["backgrnd"]["1"], Point<int16_t>(0, 37)));

		buttons[Buttons::BtVAC] = std::make_unique<MapleButton>(vac["BtVAC"]);
		buttons[Buttons::BtEnter] = std::make_unique<MapleButton>(vac["BtEnter"]);

		// 遍历 UIStateLogin::viewall_worlds 构建世界/角色列表视图。
		// 北斗为单世界，VIEW_ALL_CHAR 退化为单世界角色列表（与 CharSelect 等价）。
		const auto& worlds = UIStateLogin::viewall_worlds;

		OutlinedText labelproto(Text::Font::A12M, Text::Alignment::LEFT,
			Color::Name::WHITE, Color::Name::TOBACCOBROWN);
		OutlinedText hlproto(Text::Font::A12M, Text::Alignment::LEFT,
			Color::Name::YELLOW, Color::Name::TOBACCOBROWN);

		Point<int16_t> base(30, 90);
		int16_t y = base.y();

		for (const auto& world : worlds)
		{
			// 世界旗帜（WorldIcons[worldid]，20x20）；文字右移 24 给图标留位。
			EntryView we;
			we.pos = Point<int16_t>(base.x() + 24, y);
			we.label = labelproto;
			we.label.change_text(Locale::format("viewallchar.world", std::to_string(world.worldid)));
			we.highlight = hlproto;
			we.highlight.change_text(Locale::format("viewallchar.world", std::to_string(world.worldid)));
			we.is_world = true;
			nl::node wicon = vac["WorldIcons"][std::to_string(world.worldid)];
			if (wicon) we.icon = wicon;
			entry_views.push_back(std::move(we));
			y += 24;

			for (const auto& chr : world.chars)
			{
				std::string ctxt = "Lv." + std::to_string(chr.stats.stats[MapleStat::Id::LEVEL])
					+ "  " + chr.stats.name;

				EntryView ce;
				ce.pos = Point<int16_t>(base.x() + 24, y);
				ce.label = labelproto;
				ce.label.change_text(ctxt);
				ce.highlight = hlproto;
				ce.highlight.change_text(ctxt);
				ce.cid = chr.id;
				ce.is_char = true;
				ce.use_pic = world.use_pic;
				// 职业图标（Job[0]，19x11；该 img 仅定义 beginner）
				nl::node jobicon = vac["Job"]["0"];
				if (jobicon) ce.icon = jobicon;
				entry_views.push_back(std::move(ce));
				y += 20;
			}

			y += 12;
		}
	}

	void UIViewAllChar::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		for (size_t i = 0; i < entry_views.size(); ++i)
		{
			const EntryView& e = entry_views[i];
			// 世界旗帜 / 职业图标：绘制在文字左侧（基于 img 节点，避免布局与资源脱节）。
			if (e.icon)
				Sprite(e.icon).draw(position + Point<int16_t>(e.pos.x() - 22, e.pos.y() - 2), alpha);

			if (e.is_char && static_cast<int32_t>(i) == selected)
				e.highlight.draw(position + e.pos);
			else
				e.label.draw(position + e.pos);
		}

		// 选中角色高亮框（Select[0]，126x125，来自 img）。
		if (selected >= 0 && selected < static_cast<int32_t>(entry_views.size()))
		{
			const EntryView& e = entry_views[selected];
			if (e.is_char)
			{
				nl::node sel = nl::nx::UI["Login.img"]["ViewAllChar"]["Select"]["0"];
				if (sel)
					Sprite(sel).draw(position + Point<int16_t>(e.pos.x() - 26, e.pos.y() - 50), alpha);
			}
		}

		UIElement::draw_buttons(alpha);
	}

	void UIViewAllChar::update()
	{
		UIElement::update();
	}

	Cursor::State UIViewAllChar::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (clicked)
		{
			Point<int16_t> local = cursorpos - position;
			for (size_t i = 0; i < entry_views.size(); ++i)
			{
				const EntryView& e = entry_views[i];
				if (!e.is_char)
					continue;
				if (local.x() >= 20 && local.x() <= 780 &&
					local.y() >= e.pos.y() && local.y() <= e.pos.y() + 18)
				{
					selected = static_cast<int16_t>(i);
					sel_cid = e.cid;
					sel_use_pic = e.use_pic;
					break;
				}
			}
		}
		return UIElement::send_cursor(clicked, cursorpos);
	}

	UIElement::Type UIViewAllChar::get_type() const
	{
		return TYPE;
	}

	void UIViewAllChar::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (escape) { deactivate(); return; }
		if (keycode == KeyAction::Id::RETURN) { button_pressed(Buttons::BtEnter); return; }
		if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't')
		{
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				if (tip.empty()) return;
				try {
					int32_t cid = std::stoi(tip);
					if (cid > 0) {
						SelectCharPacket(cid).dispatch();
						LOG(LOG_DEBUG, "UIViewAllChar: SoftKey SelectChar cid=" << cid);
					}
				} catch (...) {}
			}, []() {}, "Char cid");
		}
		else if (keycode == 'E' || keycode == 'e')
		{
			button_pressed(Buttons::BtEnter);
		}
		else if (keycode == 'V' || keycode == 'v')
		{
			button_pressed(Buttons::BtVAC);
		}
	}

	Button::State UIViewAllChar::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case BtVAC:
				// 查看全部角色窗口关闭后，下层 CharSelect 仍显示，玩家回到选角流程。
				deactivate();
				break;
			case BtEnter:
			{
				if (sel_cid > 0)
				{
					if (sel_use_pic)
					{
						// 该世界要求二次密码：弹软键盘输入 PIC 后随选角包提交。
						UI::get().emplace<UISoftKey>(
							[this](const std::string& pic) {
								if (!pic.empty())
									SelectCharPicPacket(pic, sel_cid).dispatch();
							},
							[]() {}, "PIC");
					}
					else
					{
						SelectCharPacket(sel_cid).dispatch();
					}
					deactivate();
				}
				else
				{
					// 未点选角色：退回 cid 输入方式，避免按钮无响应。
					UI::get().emplace<UISoftKey>([](const std::string& tip) {
						if (tip.empty()) return;
						try {
							int32_t cid = std::stoi(tip);
							if (cid > 0)
								SelectCharPacket(cid).dispatch();
						} catch (...) {}
					}, []() {}, "Char cid");
				}
				break;
			}
			default:
				break;
		}

		return Button::State::NORMAL;
	}
}
