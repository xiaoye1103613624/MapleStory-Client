////////////////////////////////////////////////////////////////////////////////
//	Guild BBS — 原生 GuildBBS.img 三页签（列表 / 读帖 / 发帖）
////////////////////////////////////////////////////////////////////////////////
#include "UIGuildBBS.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Net/Packets/BbsPackets.h"
#include "../../Util/Locale.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		// 列表底栏按钮横向间距：贴图宽 57，间隔 8（runtime 无 layout 节点，按底板量测落位）
		constexpr int16_t kBtnGap = 65;
		constexpr int16_t kBtnX0 = 24;
	}

	UIGuildBBS::UIGuildBBS()
		: title_label(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::BLACK),
		notice_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW, Color::Name::BLACK),
		tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::LIGHTGREY, Color::Name::BLACK),
		body_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE)
	{
		position = Point<int16_t>(40, 40);
		dimension = Point<int16_t>(734, 526);
		dragarea = Point<int16_t>(734, 24);

#ifdef USE_NX
		// 部署 Data/UI/GuildBBS.img → 根目录名 GuildBBS（runtime.json 证实）
		nl::node root = nl::nx::UI["GuildBBS.img"]["GuildBBS"];
		if (!root)
		{
			LOG(LOG_WARN, "[GuildBBS] missing UI/GuildBBS.img/GuildBBS");
			UiAssetResolve::diag_once("guildbbs", "GuildBBS.img missing");
			return;
		}
		has_assets = true;

		if (root["backgrnd"])
		{
			backgrnd = Texture(root["backgrnd"]);
			Point<int16_t> o = backgrnd.get_origin();
			// 本包 runtime origin 居中 (367,263)；绘制时用左上角贴齐，需 shift 回原点。
			if (o.x() || o.y())
				backgrnd.shift(o);
			dimension = backgrnd.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 24);
		}
		if (root["backgrnd2"])
		{
			backgrnd2 = Texture(root["backgrnd2"]);
			Point<int16_t> o = backgrnd2.get_origin();
			if (o.x() || o.y())
				backgrnd2.shift(o);
		}
		if (root["backgrnd3"])
		{
			backgrnd3 = Texture(root["backgrnd3"]);
			Point<int16_t> o = backgrnd3.get_origin();
			if (o.x() || o.y())
				backgrnd3.shift(o);
		}

		// try_maple_btn：传入左上角；内部会加 normal origin，避免居中 origin 把钮拧飞
		try_maple_btn(buttons, BtQuit, root["BtQuit"],
			Point<int16_t>(static_cast<int16_t>(dimension.x() - 48), 6), "GuildBBS.BtQuit");
		try_maple_btn(buttons, BtWrite, root["BtWrite"],
			Point<int16_t>(kBtnX0, BTN_Y_LIST), "GuildBBS.BtWrite");
		try_maple_btn(buttons, BtRetouch, root["BtRetouch"],
			Point<int16_t>(static_cast<int16_t>(kBtnX0 + kBtnGap), BTN_Y_LIST), "GuildBBS.BtRetouch");
		try_maple_btn(buttons, BtDelete, root["BtDelete"],
			Point<int16_t>(static_cast<int16_t>(kBtnX0 + kBtnGap * 2), BTN_Y_LIST), "GuildBBS.BtDelete");
		try_maple_btn(buttons, BtReply, root["BtReply"],
			Point<int16_t>(static_cast<int16_t>(kBtnX0 + kBtnGap * 3), BTN_Y_LIST), "GuildBBS.BtReply");
		try_maple_btn(buttons, BtReplyDelete, root["BtReplyDelete"],
			Point<int16_t>(static_cast<int16_t>(kBtnX0 + kBtnGap * 3 + 28), BTN_Y_LIST), "GuildBBS.BtReplyDelete");

		const int16_t ox = static_cast<int16_t>((dimension.x() - 327) / 2);
		const int16_t oy = static_cast<int16_t>((dimension.y() - 288) / 2);
		try_maple_btn(buttons, BtRegister, root["BtRegister"],
			Point<int16_t>(static_cast<int16_t>(ox + 40), static_cast<int16_t>(oy + 248)), "GuildBBS.BtRegister");
		try_maple_btn(buttons, BtCancel, root["BtCancel"],
			Point<int16_t>(static_cast<int16_t>(ox + 120), static_cast<int16_t>(oy + 248)), "GuildBBS.BtCancel");
		try_maple_btn(buttons, BtNotice, root["BtNotice"],
			Point<int16_t>(static_cast<int16_t>(ox + 200), static_cast<int16_t>(oy + 248)), "GuildBBS.BtNotice");

		try_maple_btn(buttons, BtPageLeft, root["MoveEmoticon"]["BtLeft"],
			Point<int16_t>(static_cast<int16_t>(dimension.x() - 70), BTN_Y_LIST), "GuildBBS.BtPageLeft");
		try_maple_btn(buttons, BtPageRight, root["MoveEmoticon"]["BtRight"],
			Point<int16_t>(static_cast<int16_t>(dimension.x() - 48), BTN_Y_LIST), "GuildBBS.BtPageRight");

		// Emoticon/Basic 是 canvas 而非 MapleButton 四态；发帖页自绘，键 1/2/3 选 icon。
		nl::node emo = root["Emoticon"]["Basic"];
		for (int i = 0; i < 3; ++i)
		{
			if (emo[i])
				emoticon_basic[i] = Texture(emo[i]);
		}
		if (root["Emoticon"]["Select"])
			emoticon_select = Texture(root["Emoticon"]["Select"]);

		set_page(Page::List);
		LOG(LOG_INFO, "[GuildBBS] open dim=" << dimension.x() << "x" << dimension.y());
#endif
	}

	void UIGuildBBS::set_page(Page p)
	{
		page = p;
		refresh_button_visibility();
	}

	void UIGuildBBS::refresh_button_visibility()
	{
		auto show = [&](uint16_t id, bool on)
		{
			if (buttons[id])
				buttons[id]->set_state(on ? Button::State::NORMAL : Button::State::DISABLED);
			if (buttons[id])
				buttons[id]->set_active(on);
		};

		const bool list = page == Page::List;
		const bool view = page == Page::View;
		const bool write = page == Page::Write;

		show(BtQuit, true);
		show(BtWrite, list || view);
		show(BtRetouch, list || view);
		show(BtDelete, list || view);
		show(BtReply, view);
		show(BtReplyDelete, view);
		show(BtPageLeft, list);
		show(BtPageRight, list);
		show(BtRegister, write);
		show(BtCancel, write);
		show(BtNotice, write);
	}

	void UIGuildBBS::request_list(int32_t page_i)
	{
		if (page_i < 0)
			page_i = 0;
		page_index = page_i;
		BbsListPacket(page_index).dispatch();
		LOG(LOG_DEBUG, "[GuildBBS] list request page=" << page_index);
	}

	void UIGuildBBS::apply_list(bool notice_flag, BbsThreadSummary notice_row,
		int32_t total_count, std::vector<BbsThreadSummary> page_rows, int32_t page_index_hint)
	{
		has_notice = notice_flag;
		notice = std::move(notice_row);
		total_threads = total_count;
		threads = std::move(page_rows);
		if (page_index_hint >= 0)
			page_index = page_index_hint;
		selected_row = threads.empty() ? -1 : 0;
		selected_reply = -1;
		set_page(Page::List);
		tip_label.change_text(Locale::format("guildbbs.list_tip",
			std::to_string(threads.size()) + "/" + std::to_string(total_threads)));
		LOG(LOG_INFO, "[GuildBBS] list notice=" << (has_notice ? 1 : 0)
			<< " page=" << page_index << " rows=" << threads.size()
			<< " total=" << total_threads);
	}

	void UIGuildBBS::apply_thread(BbsThreadDetail detail)
	{
		current = std::move(detail);
		selected_reply = current.replies.empty() ? -1 : 0;
		set_page(Page::View);
		tip_label.change_text(Locale::format("guildbbs.view_tip",
			std::to_string(current.local_id) + "/" + std::to_string(current.replies.size())));
		LOG(LOG_INFO, "[GuildBBS] view id=" << current.local_id
			<< " replies=" << current.replies.size());
	}

	const BbsThreadSummary* UIGuildBBS::selected_summary() const
	{
		if (selected_row >= 0 && selected_row < static_cast<int>(threads.size()))
			return &threads[static_cast<size_t>(selected_row)];
		return nullptr;
	}

	int16_t UIGuildBBS::list_row_at(Point<int16_t> rel) const
	{
		if (rel.x() < 20 || rel.x() > dimension.x() - 20)
			return -1;
		int16_t y0 = LIST_ROW_Y0;
		if (has_notice)
			y0 = static_cast<int16_t>(y0 + LIST_ROW_H);
		int16_t row = static_cast<int16_t>((rel.y() - y0) / LIST_ROW_H);
		if (row < 0 || row >= LIST_MAX_ROWS)
			return -1;
		return row;
	}

	void UIGuildBBS::draw(float inter) const
	{
		if (!has_assets)
			return;

		if (backgrnd)
			backgrnd.draw(position);

		title_label.change_text(Locale::text("guildbbs.title"));
		title_label.draw(position + Point<int16_t>(16, 8));

		if (page == Page::List)
		{
			int16_t y = 48;
			if (has_notice)
			{
				notice_label.change_text(Locale::format("guildbbs.notice_row", notice.subject));
				notice_label.draw(position + Point<int16_t>(24, y));
				y = static_cast<int16_t>(y + LIST_ROW_H);
			}
			y = LIST_ROW_Y0;
			if (has_notice)
				y = static_cast<int16_t>(y + LIST_ROW_H);

			for (size_t i = 0; i < threads.size() && i < static_cast<size_t>(LIST_MAX_ROWS); ++i)
			{
				const bool sel = static_cast<int>(i) == selected_row;
				OutlinedText row(Text::Font::A11M, Text::Alignment::LEFT,
					sel ? Color::Name::YELLOW : Color::Name::WHITE, Color::Name::BLACK);
				const auto& t = threads[i];
				std::string line = "#" + std::to_string(t.local_id) + "  " + t.subject
					+ "  [" + std::to_string(t.reply_count) + "]";
				if (line.size() > 70)
					line = line.substr(0, 67) + "...";
				row.change_text(line);
				row.draw(position + Point<int16_t>(24, y));
				y = static_cast<int16_t>(y + LIST_ROW_H);
			}
		}
		else if (page == Page::View)
		{
			if (backgrnd3)
			{
				const int16_t ox = static_cast<int16_t>((dimension.x() - 327) / 2);
				const int16_t oy = 48;
				backgrnd3.draw(position + Point<int16_t>(ox, oy));
			}
			OutlinedText sub(Text::Font::A12M, Text::Alignment::LEFT,
				Color::Name::YELLOW, Color::Name::BLACK);
			sub.change_text(current.subject);
			sub.draw(position + Point<int16_t>(40, 60));

			std::string body = current.body;
			if (body.size() > 220)
				body = body.substr(0, 217) + "...";
			body_label.change_text(body);
			body_label.draw(position + Point<int16_t>(40, 88));

			int16_t y = 160;
			for (size_t i = 0; i < current.replies.size() && i < 8; ++i)
			{
				const bool sel = static_cast<int>(i) == selected_reply;
				OutlinedText row(Text::Font::A11M, Text::Alignment::LEFT,
					sel ? Color::Name::YELLOW : Color::Name::LIGHTGREY, Color::Name::BLACK);
				std::string line = "#" + std::to_string(current.replies[i].reply_id)
					+ " " + current.replies[i].content;
				if (line.size() > 64)
					line = line.substr(0, 61) + "...";
				row.change_text(line);
				row.draw(position + Point<int16_t>(48, y));
				y = static_cast<int16_t>(y + 18);
			}
		}
		else if (page == Page::Write)
		{
			if (backgrnd2)
			{
				const int16_t ox = static_cast<int16_t>((dimension.x() - 327) / 2);
				const int16_t oy = static_cast<int16_t>((dimension.y() - 288) / 2);
				backgrnd2.draw(position + Point<int16_t>(ox, oy));
			}
			tip_label.change_text(Locale::text(write_as_notice
				? "guildbbs.write_notice_hint"
				: (write_edit ? "guildbbs.write_edit_hint" : "guildbbs.write_hint")));
			const int16_t ox = static_cast<int16_t>((dimension.x() - 327) / 2);
			const int16_t oy = static_cast<int16_t>((dimension.y() - 288) / 2);
			for (int i = 0; i < 3; ++i)
			{
				if (emoticon_basic[i])
					emoticon_basic[i].draw(position + Point<int16_t>(
						static_cast<int16_t>(ox + 36 + i * 28),
						static_cast<int16_t>(oy + 210)));
			}
			if (emoticon_select && write_icon >= 0 && write_icon < 3)
			{
				emoticon_select.draw(position + Point<int16_t>(
					static_cast<int16_t>(ox + 36 + write_icon * 28 - 2),
					static_cast<int16_t>(oy + 208)));
			}
		}

		if (!tip_label.inner.get_text().empty())
			tip_label.draw(position + Point<int16_t>(16, static_cast<int16_t>(dimension.y() - 28)));

		UIElement::draw_buttons(inter);
	}

	void UIGuildBBS::update()
	{
	}

	Cursor::State UIGuildBBS::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State st = UIDragElement::send_cursor(clicked, cursorpos);
		if (!clicked || !active || !has_assets)
			return st;

		Point<int16_t> rel = cursorpos - position;
		if (page == Page::List)
		{
			int16_t row = list_row_at(rel);
			if (row >= 0 && row < static_cast<int16_t>(threads.size()))
			{
				selected_row = row;
				LOG(LOG_DEBUG, "[GuildBBS] select row=" << selected_row
					<< " id=" << threads[static_cast<size_t>(row)].local_id);
			}
			else if (has_notice && rel.y() >= 48 && rel.y() < LIST_ROW_Y0 + LIST_ROW_H
				&& rel.x() >= 20 && rel.x() <= dimension.x() - 20)
			{
				// 点公告条 → 直接打开 local_id=0
				BbsOpenPacket(0).dispatch();
				LOG(LOG_DEBUG, "[GuildBBS] open notice");
			}
		}
		else if (page == Page::View)
		{
			int16_t y = 160;
			int16_t row = static_cast<int16_t>((rel.y() - y) / 18);
			if (row >= 0 && row < static_cast<int16_t>(current.replies.size()) && row < 8
				&& rel.x() >= 40 && rel.x() <= 400)
			{
				selected_reply = row;
				LOG(LOG_DEBUG, "[GuildBBS] select reply="
					<< current.replies[static_cast<size_t>(row)].reply_id);
			}
		}
		return st;
	}

	void UIGuildBBS::doubleclick(Point<int16_t> cursorpos)
	{
		if (!has_assets || page != Page::List)
			return;
		Point<int16_t> rel = cursorpos - position;
		int16_t row = list_row_at(rel);
		if (row >= 0 && row < static_cast<int16_t>(threads.size()))
		{
			selected_row = row;
			BbsOpenPacket(threads[static_cast<size_t>(row)].local_id).dispatch();
			LOG(LOG_DEBUG, "[GuildBBS] dblopen id="
				<< threads[static_cast<size_t>(row)].local_id);
		}
	}

	void UIGuildBBS::prompt_write(bool as_notice, bool edit)
	{
		write_as_notice = as_notice;
		write_edit = edit;
		edit_local_id = 0;
		if (edit)
		{
			if (page == Page::View)
				edit_local_id = current.local_id;
			else if (const BbsThreadSummary* s = selected_summary())
				edit_local_id = s->local_id;
			else
			{
				tip_label.change_text(Locale::text("guildbbs.none_selected"));
				return;
			}
		}
		set_page(Page::Write);
		UI::get().emplace<UISoftKey>([this](const std::string& title)
		{
			if (title.empty())
			{
				set_page(Page::List);
				return;
			}
			UI::get().emplace<UISoftKey>([this, title](const std::string& body)
			{
				if (body.empty())
				{
					set_page(Page::List);
					return;
				}
				if (write_edit)
				{
					BbsEditPacket(edit_local_id, write_as_notice, title, body, write_icon).dispatch();
					LOG(LOG_INFO, "[GuildBBS] edit id=" << edit_local_id);
				}
				else
				{
					BbsPostPacket(write_as_notice, title, body, write_icon).dispatch();
					LOG(LOG_INFO, "[GuildBBS] post notice=" << (write_as_notice ? 1 : 0)
						<< " icon=" << write_icon);
				}
				request_list(page_index);
				set_page(Page::List);
			}, [this]() { set_page(Page::List); }, "BBS body");
		}, [this]() { set_page(Page::List); }, "BBS title");
	}

	void UIGuildBBS::prompt_reply()
	{
		int32_t tid = current.local_id;
		if (tid < 0)
		{
			tip_label.change_text(Locale::text("guildbbs.none_selected"));
			return;
		}
		UI::get().emplace<UISoftKey>([this, tid](const std::string& text)
		{
			if (text.empty())
				return;
			BbsReplyPacket(tid, text).dispatch();
			BbsOpenPacket(tid).dispatch();
			LOG(LOG_INFO, "[GuildBBS] reply id=" << tid);
		}, []() {}, "BBS reply");
	}

	void UIGuildBBS::dispatch_delete_selected()
	{
		int32_t id = -1;
		if (page == Page::View)
			id = current.local_id;
		else if (const BbsThreadSummary* s = selected_summary())
			id = s->local_id;
		if (id < 0)
		{
			tip_label.change_text(Locale::text("guildbbs.none_selected"));
			return;
		}
		BbsDeletePacket(id).dispatch();
		LOG(LOG_INFO, "[GuildBBS] delete id=" << id);
		request_list(page_index);
		set_page(Page::List);
	}

	void UIGuildBBS::dispatch_delete_reply_selected()
	{
		if (selected_reply < 0 || selected_reply >= static_cast<int>(current.replies.size()))
		{
			tip_label.change_text(Locale::text("guildbbs.none_reply"));
			return;
		}
		int32_t rid = current.replies[static_cast<size_t>(selected_reply)].reply_id;
		BbsDeleteReplyPacket(current.local_id, rid).dispatch();
		BbsOpenPacket(current.local_id).dispatch();
		LOG(LOG_INFO, "[GuildBBS] delete reply=" << rid);
	}

	Button::State UIGuildBBS::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case BtQuit:
				if (page == Page::Write || page == Page::View)
				{
					set_page(Page::List);
					request_list(page_index);
				}
				else
					deactivate();
				break;
			case BtWrite:
				prompt_write(false, false);
				break;
			case BtRetouch:
				prompt_write(false, true);
				break;
			case BtNotice:
				write_as_notice = !write_as_notice;
				tip_label.change_text(Locale::text(write_as_notice
					? "guildbbs.write_notice_hint" : "guildbbs.write_hint"));
				LOG(LOG_DEBUG, "[GuildBBS] notice flag=" << (write_as_notice ? 1 : 0));
				break;
			case BtRegister:
				// SoftKey 流程已在 prompt_write 内完成；此处再点则重新拉起
				prompt_write(write_as_notice, write_edit);
				break;
			case BtCancel:
				set_page(Page::List);
				break;
			case BtDelete:
				dispatch_delete_selected();
				break;
			case BtReply:
				prompt_reply();
				break;
			case BtReplyDelete:
				dispatch_delete_reply_selected();
				break;
			case BtPageLeft:
				if (page_index > 0)
					request_list(page_index - 1);
				break;
			case BtPageRight:
				// 与 BtPageLeft 的 page_index > 0 下界对称：当前页未满即末页，
				// 不发必然返回空列表的越界页请求（否则 selected_row 被清成 -1、
				// 窗口空白、page_index 停在越界值，只能再点「上一页」回退）。
				if (static_cast<int>(threads.size()) >= LIST_MAX_ROWS)
					request_list(page_index + 1);
				break;
			default:
				break;
		}
		return Button::State::NORMAL;
	}

	void UIGuildBBS::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape)
		{
			if (page != Page::List)
			{
				set_page(Page::List);
				return;
			}
			deactivate();
			return;
		}
		if (!pressed)
			return;

		if (keycode == 'R' || keycode == 'r')
			request_list(page_index);
		else if (keycode == 'W' || keycode == 'w')
			button_pressed(BtWrite);
		else if (keycode == 'E' || keycode == 'e')
			button_pressed(BtRetouch);
		else if (keycode == 'D' || keycode == 'd')
			button_pressed(BtDelete);
		else if (keycode == 'O' || keycode == 'o')
		{
			if (const BbsThreadSummary* s = selected_summary())
				BbsOpenPacket(s->local_id).dispatch();
		}
		else if (keycode == 'N' || keycode == 'n')
			prompt_write(true, false);
		else if (keycode == 'P' || keycode == 'p')
			button_pressed(BtReply);
		else if (keycode == ',' || keycode == '[')
			button_pressed(BtPageLeft);
		else if (keycode == '.' || keycode == ']')
			button_pressed(BtPageRight);
		else if (keycode >= '1' && keycode <= '3')
		{
			write_icon = keycode - '1';
			LOG(LOG_DEBUG, "[GuildBBS] icon=" << write_icon);
		}
	}

	UIElement::Type UIGuildBBS::get_type() const
	{
		return TYPE;
	}
}
