//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#include "UIQuestLog.h"

#include "../Components/MapleButton.h"
#include "../Components/TwoSpriteButton.h"
#include "../UI.h"
#include "../KeyAction.h"

#include "../../Character/MapleStat.h"
#include "../../Data/QuestData.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/QuestPackets.h"
#include "../../Util/UiAssetResolve.h"

#include <ctime>
#include <sstream>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		std::string format_completion_time(int64_t raw)
		{
			if (raw <= 0)
				return {};

			// Server may send FileTime or Unix seconds; try Unix first.
			std::time_t t = static_cast<std::time_t>(raw);

			if (raw > 1000000000000LL)
				t = static_cast<std::time_t>(raw / 1000);

			std::tm tm_buf{};
#ifdef _WIN32
			if (localtime_s(&tm_buf, &t) != 0)
				return {};
#else
			if (!localtime_r(&t, &tm_buf))
				return {};
#endif
			char buf[32];
			if (std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &tm_buf) == 0)
				return {};

			return buf;
		}
	}

	UIQuestLog::UIQuestLog(const QuestLog& ql) :
		UIDragElement<PosQUEST>(),
		questlog(ql),
		flat_layout(false),
		has_search(false),
		list_offset(0),
		selected_index(-1)
	{
		tab = Buttons::TAB0;

		nl::node close = nl::nx::UI["Basic.img"]["BtClose3"];
		if (!close)
			close = nl::nx::UI["Basic.img"]["BtClose2"];

		nl::node quest = UiAssetResolve::window("Quest");

		if (!UiAssetResolve::usable(quest))
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		nl::node list = quest["list"];

		flat_layout = !static_cast<bool>(list);
		nl::node root = flat_layout ? quest : list;

		nl::node backgrnd = root["backgrnd"];

		if (backgrnd)
			sprites.emplace_back(backgrnd);

		if (root["backgrnd2"])
			sprites.emplace_back(root["backgrnd2"]);

		if (root["notice0"])
			notice_sprites.emplace_back(root["notice0"]);

		if (root["notice1"])
			notice_sprites.emplace_back(root["notice1"]);

		if (root["notice2"])
			notice_sprites.emplace_back(root["notice2"]);

		nl::node taben = root["Tab"]["enabled"];
		nl::node tabdis = root["Tab"]["disabled"];

		if (!taben)
			taben = root["Tab"]["tabEnabled"];

		if (!tabdis)
			tabdis = root["Tab"]["tabDisabled"];

		if (tabdis["0"] && taben["0"])
			buttons[Buttons::TAB0] = std::make_unique<TwoSpriteButton>(tabdis["0"], taben["0"]);

		if (tabdis["1"] && taben["1"])
			buttons[Buttons::TAB1] = std::make_unique<TwoSpriteButton>(tabdis["1"], taben["1"]);

		if (tabdis["2"] && taben["2"])
			buttons[Buttons::TAB2] = std::make_unique<TwoSpriteButton>(tabdis["2"], taben["2"]);

		if (close)
			buttons[Buttons::CLOSE] = std::make_unique<MapleButton>(close, Point<int16_t>(275, 6));

		if (root["BtSearch"])
		{
			buttons[Buttons::SEARCH] = std::make_unique<MapleButton>(root["BtSearch"]);
			has_search = true;
		}

		if (root["BtAllLevel"])
			buttons[Buttons::ALL_LEVEL] = std::make_unique<MapleButton>(root["BtAllLevel"]);

		if (root["BtMyLocation"])
			buttons[Buttons::MY_LOCATION] = std::make_unique<MapleButton>(root["BtMyLocation"]);

		if (root["BtGiveup"])
			buttons[Buttons::GIVEUP] = std::make_unique<MapleButton>(root["BtGiveup"]);
		else if (quest["BtGiveup"])
			buttons[Buttons::GIVEUP] = std::make_unique<MapleButton>(quest["BtGiveup"]);

		if (root["searchArea"])
		{
			search_area = root["searchArea"];
			has_search = true;
		}

		int16_t search_limit = 19;

		if (has_search && search_area.is_valid())
		{
			search = Textfield(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BOULDER, Rectangle<int16_t>(get_search_pos(), get_search_pos() + get_search_dim()), search_limit);
			placeholder = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BOULDER, "Enter the quest name.");
		}

		slider = Slider(Slider::Type::DEFAULT_SILVER, Range<int16_t>(0, 279), 150, 20, 5, [this](bool upwards)
		{
			int16_t shift = upwards ? -1 : 1;
			int16_t new_offset = list_offset + shift;
			int16_t max_off = static_cast<int16_t>(listed_ids.size()) - 5;

			if (max_off < 0)
				max_off = 0;

			if (new_offset < 0)
				new_offset = 0;
			else if (new_offset > max_off)
				new_offset = max_off;

			list_offset = new_offset;
		});

		detail_title = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE, "");
		detail_progress = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "", 200);

		change_tab(tab);
		rebuild_list();

		if (backgrnd)
			dimension = Texture(backgrnd).get_dimensions();
		else
			dimension = Point<int16_t>(300, 350);

		dragarea = Point<int16_t>(dimension.x(), 20);
	}

	void UIQuestLog::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		Point<int16_t> notice_position = Point<int16_t>(0, 26);

		if (!notice_sprites.empty() && tab < notice_sprites.size())
		{
			if (tab == Buttons::TAB0)
				notice_sprites[tab].draw(position + notice_position + Point<int16_t>(9, 0), alpha);
			else if (tab == Buttons::TAB1)
				notice_sprites[tab].draw(position + notice_position + Point<int16_t>(0, 0), alpha);
			else
				notice_sprites[tab].draw(position + notice_position + Point<int16_t>(-10, 0), alpha);
		}

		if (has_search && tab != Buttons::TAB2 && search_area.is_valid())
		{
			search_area.draw(position);
			search.draw(Point<int16_t>(4, -4), Point<int16_t>(2, -2));

			if (search.get_state() == Textfield::State::NORMAL && search.empty())
				placeholder.draw(position + Point<int16_t>(39, 51));
		}

		Rectangle<int16_t> area = list_area();
		Point<int16_t> rowpos = position + Point<int16_t>(area.left(), area.top());
		constexpr int16_t ROW_HEIGHT = 18;
		int16_t visible = 8;

		for (int16_t i = 0; i < visible; i++)
		{
			size_t idx = static_cast<size_t>(list_offset + i);

			if (idx >= listed_names.size())
				break;

			listed_names[idx].draw(rowpos + Point<int16_t>(0, i * ROW_HEIGHT));
		}

		if (selected_index >= 0)
		{
			Point<int16_t> detail_pos = position + Point<int16_t>(area.left(), area.bottom() + 8);
			detail_title.draw(detail_pos);
			detail_progress.draw(detail_pos + Point<int16_t>(0, 18));
		}

		if (listed_ids.size() > 5)
			slider.draw(position + Point<int16_t>(area.right() + 2, area.top()));

		UIElement::draw_buttons(alpha);
	}

	void UIQuestLog::update()
	{
		if (has_search && search_area.is_valid())
			search.update(get_search_pos(), get_search_dim());
	}

	void UIQuestLog::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
			{
				deactivate();
			}
			else if (keycode == KeyAction::Id::TAB)
			{
				uint16_t new_tab = tab;

				if (new_tab < Buttons::TAB2)
					new_tab++;
				else
					new_tab = Buttons::TAB0;

				change_tab(new_tab);
			}
		}
	}

	Cursor::State UIQuestLog::send_cursor(bool clicking, Point<int16_t> cursorpos)
	{
		if (has_search && search_area.is_valid())
		{
			if (Cursor::State new_state = search.send_cursor(cursorpos, clicking))
				return new_state;
		}

		Rectangle<int16_t> area = list_area();
		Point<int16_t> abs_lt = position + Point<int16_t>(area.left(), area.top());
		Point<int16_t> abs_rb = position + Point<int16_t>(area.right(), area.bottom());
		Rectangle<int16_t> abs_area(abs_lt, abs_rb);

		if (clicking && abs_area.contains(cursorpos))
		{
			int16_t rel_y = cursorpos.y() - abs_lt.y();
			int16_t row = rel_y / 18;

			if (row >= 0)
				select_row(static_cast<size_t>(list_offset + row));
		}

		Rectangle<int16_t> area_slider = list_area();

		if (listed_ids.size() > 5)
		{
			Point<int16_t> slider_pos = position + Point<int16_t>(area_slider.right() + 2, area_slider.top());

			if (Cursor::State sstate = slider.send_cursor(cursorpos - slider_pos, clicking))
				return sstate;
		}

		return UIDragElement::send_cursor(clicking, cursorpos);
	}

	UIElement::Type UIQuestLog::get_type() const
	{
		return TYPE;
	}

	void UIQuestLog::refresh()
	{
		rebuild_list();
	}

	Button::State UIQuestLog::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::TAB0:
			case Buttons::TAB1:
			case Buttons::TAB2:
				change_tab(buttonid);
				return Button::State::IDENTITY;
			case Buttons::CLOSE:
				deactivate();
				return Button::State::NORMAL;
			case Buttons::GIVEUP:
			{
				if (selected_index >= 0
					&& static_cast<size_t>(selected_index) < listed_ids.size()
					&& tab == Buttons::TAB1)
				{
					int16_t qid = listed_ids[static_cast<size_t>(selected_index)];
					QuestActionPacket(3, qid).dispatch();
					refresh();
				}

				return Button::State::NORMAL;
			}
			default:
				return Button::State::DISABLED;
		}
	}

	void UIQuestLog::change_tab(uint16_t tabid)
	{
		uint16_t oldtab = tab;
		tab = tabid;

		if (oldtab != tab)
		{
			if (buttons.count(Buttons::TAB0 + oldtab))
				buttons[Buttons::TAB0 + oldtab]->set_state(Button::State::NORMAL);

			if (buttons.count(Buttons::MY_LOCATION))
				buttons[Buttons::MY_LOCATION]->set_active(tab == Buttons::TAB0);

			if (buttons.count(Buttons::ALL_LEVEL))
				buttons[Buttons::ALL_LEVEL]->set_active(tab == Buttons::TAB0);

			if (buttons.count(Buttons::SEARCH))
				buttons[Buttons::SEARCH]->set_active(tab != Buttons::TAB2);

			if (buttons.count(Buttons::GIVEUP))
			{
				buttons[Buttons::GIVEUP]->set_active(tab == Buttons::TAB1);
				buttons[Buttons::GIVEUP]->set_state(
					(tab == Buttons::TAB1 && selected_index >= 0)
						? Button::State::NORMAL
						: Button::State::DISABLED
				);
			}

			if (has_search && search_area.is_valid())
			{
				if (tab == Buttons::TAB2)
					search.set_state(Textfield::State::DISABLED);
				else
					search.set_state(Textfield::State::NORMAL);
			}
		}

		if (buttons.count(Buttons::TAB0 + tab))
			buttons[Buttons::TAB0 + tab]->set_state(Button::State::PRESSED);

		list_offset = 0;
		selected_index = -1;
		rebuild_list();
	}

	void UIQuestLog::rebuild_list()
	{
		listed_ids.clear();
		listed_names.clear();

		std::string filter;

		if (has_search && search_area.is_valid() && !search.empty())
			filter = search.get_text();

		auto add_row = [&](int16_t qid, const std::string& progress)
		{
			const QuestData& qdata = QuestData::get(qid);
			std::string label = qdata.get_name();

			if (!filter.empty() && label.find(filter) == std::string::npos)
				return;

			if (!progress.empty() && tab == Buttons::TAB1)
				label += "  [" + progress + "]";

			listed_ids.push_back(qid);
			listed_names.emplace_back(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, label);
		};

		if (tab == Buttons::TAB0)
		{
			const CharStats& stats = Stage::get().get_player().get_stats();
			int16_t level = static_cast<int16_t>(stats.get_stat(MapleStat::Id::LEVEL));
			uint16_t job = stats.get_stat(MapleStat::Id::JOB);

			for (int16_t qid : QuestData::list_available(level, job, questlog))
				add_row(qid, "");
		}
		else if (tab == Buttons::TAB1)
		{
			for (const auto& entry : questlog.get_started())
				add_row(entry.first, entry.second);
		}
		else
		{
			for (const auto& entry : questlog.get_completed())
				add_row(entry.first, "");
		}

		if (selected_index >= static_cast<int16_t>(listed_ids.size()))
			selected_index = listed_ids.empty() ? -1 : 0;

		if (selected_index >= 0)
			select_row(static_cast<size_t>(selected_index));
		else
		{
			detail_title.change_text("");
			detail_progress.change_text("");
		}

		if (buttons.count(Buttons::GIVEUP))
		{
			bool can_giveup = tab == Buttons::TAB1 && selected_index >= 0;
			buttons[Buttons::GIVEUP]->set_active(tab == Buttons::TAB1);
			buttons[Buttons::GIVEUP]->set_state(can_giveup ? Button::State::NORMAL : Button::State::DISABLED);
		}

		int16_t rowmax = static_cast<int16_t>(listed_ids.size());
		int16_t visible = 8;
		slider.setrows(visible, rowmax > visible ? rowmax : visible);
	}

	void UIQuestLog::select_row(size_t index)
	{
		if (index >= listed_ids.size())
			return;

		selected_index = static_cast<int16_t>(index);
		int16_t qid = listed_ids[index];
		const QuestData& qdata = QuestData::get(qid);

		detail_title.change_text(qdata.get_name());

		if (tab == Buttons::TAB2)
		{
			std::string when = format_completion_time(questlog.get_completion_time(qid));
			detail_progress.change_text(when.empty() ? "Completed" : ("Completed  " + when));
		}
		else if (tab == Buttons::TAB0)
		{
			std::string summary = qdata.get_summary();
			detail_progress.change_text(summary.empty() ? "Available" : summary);
		}
		else
		{
			std::string progress = questlog.get_progress(qid);

			if (progress.empty())
				progress = qdata.get_summary();

			detail_progress.change_text(progress.empty() ? "In progress" : progress);
		}

		if (buttons.count(Buttons::GIVEUP))
		{
			bool can_giveup = tab == Buttons::TAB1 && selected_index >= 0;
			buttons[Buttons::GIVEUP]->set_state(can_giveup ? Button::State::NORMAL : Button::State::DISABLED);
		}
	}

	Rectangle<int16_t> UIQuestLog::list_area() const
	{
		if (flat_layout)
			return Rectangle<int16_t>(Point<int16_t>(12, 70), Point<int16_t>(260, 220));

		return Rectangle<int16_t>(Point<int16_t>(12, 75), Point<int16_t>(250, 230));
	}

	Point<int16_t> UIQuestLog::get_search_pos()
	{
		if (!search_area.is_valid())
			return position;

		Point<int16_t> search_area_origin = search_area.get_origin().abs();
		Point<int16_t> search_pos_adj = Point<int16_t>(25, 4);

		return position + search_area_origin + search_pos_adj;
	}

	Point<int16_t> UIQuestLog::get_search_dim()
	{
		if (!search_area.is_valid())
			return Point<int16_t>();

		Point<int16_t> adjust = Point<int16_t>(-75, -8);

		return search_area.get_dimensions() + adjust;
	}
}
