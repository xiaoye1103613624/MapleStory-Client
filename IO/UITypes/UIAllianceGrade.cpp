////////////////////////////////////////////////////////////////////////////////
//	CWndAllianceGrade 实现（Angel 083 证据驱动）
////////////////////////////////////////////////////////////////////////////////
#include "UIAllianceGrade.h"
#include "UINotice.h"
#include "UIUserList.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Character/AllianceModel.h"
#include "../../Net/Packets/AlliancePackets.h"
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
		constexpr int16_t kRowTop = 75;
		constexpr int16_t kRowH = 20;
		constexpr int16_t kRowLeft = 10;
		constexpr int16_t kRowRight = 254;
		constexpr int16_t kNameX = 48;
		constexpr size_t kNameLimit = 10;
	}

	UIAllianceGrade::UIAllianceGrade()
	{
		dimension = Point<int16_t>(233, 304);

#ifdef USE_NX
		// Angel OnCreate sub_917546：UI/UIWindow.img/UserList/GuildUnion/backgrnd
		nl::node bg = nl::nx::UI["UIWindow.img"]["UserList"]["GuildUnion"]["backgrnd"];
		if (bg)
		{
			background = bg;
			has_background = true;
			dimension = background.get_dimensions();
		}

		// 职衔行条：UserList/Guild/GuildGrade/guildgrade0..4
		nl::node gg = nl::nx::UI["UIWindow.img"]["UserList"]["Guild"]["GuildGrade"];
		for (int i = 0; i < 5; i++)
		{
			nl::node row = gg["guildgrade" + std::to_string(i)];
			if (row)
				grade_row[i] = row;
			grade_labels[i] = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::BLACK, "");
		}

		nl::node close = UiAssetResolve::close_btn();
		if (close)
			buttons[Buttons::BtClose] = std::make_unique<MapleButton>(
				close, Point<int16_t>(static_cast<int16_t>(dimension.x() - 21), 6));

		// Angel：BtModf @ (25,268) id=2140；BtSave @ (125,268) id=2141
		if (nl::node mod = gg["BtModf"])
			buttons[Buttons::BtMod] = std::make_unique<MapleButton>(mod, Point<int16_t>(25, 268));
		if (nl::node save = gg["BtSave"])
			buttons[Buttons::BtSave] = std::make_unique<MapleButton>(save, Point<int16_t>(125, 268));
#endif

		// Angel ToggleAllianceGrade：贴在 UserList 右侧。
		if (auto ul = UI::get().get_element<UIUserList>())
			anchor_right_of(*ul, 0);
		else
			position = Point<int16_t>(320, 104);

		reload_from_model();
	}

	void UIAllianceGrade::reload_from_model()
	{
		const AllianceData& a = AllianceModel::get().data();
		for (int i = 0; i < 5; i++)
		{
			grades[i] = a.rank_titles[i];
			if (grades[i].empty())
			{
				// Angel OnCreate 默认：Master / Jr.Master / Member×3
				if (i == 0)
					grades[i] = Locale::text("alliance.grade_master");
				else if (i == 1)
					grades[i] = Locale::text("alliance.grade_jrmaster");
				else
					grades[i] = Locale::text("alliance.grade_member");
			}
			grade_labels[i].change_text(grades[i]);
			grade_labels[i].change_color(
				(selected == i + 1) ? Color::Name::BLUE : Color::Name::BLACK);
		}
	}

	void UIAllianceGrade::draw(float inter) const
	{
		if (has_background)
			background.draw(DrawArgument(position));

		for (int i = 0; i < 5; i++)
		{
			const int16_t y = static_cast<int16_t>(kRowTop + i * kRowH);
			if (grade_row[i].is_valid())
				grade_row[i].draw(DrawArgument(position + Point<int16_t>(kRowLeft, y)));
			grade_labels[i].draw(position + Point<int16_t>(kNameX, static_cast<int16_t>(y + 2)));
		}

		UIElement::draw(inter);
	}

	Cursor::State UIAllianceGrade::send_cursor(bool pressed, Point<int16_t> cursorpos)
	{
		if (pressed)
		{
			const int32_t idx = grade_index_at(cursorpos);
			if (idx > 0)
			{
				selected = idx;
				for (int i = 0; i < 5; i++)
					grade_labels[i].change_color(
						(selected == i + 1) ? Color::Name::BLUE : Color::Name::BLACK);
			}
		}
		return UIElement::send_cursor(pressed, cursorpos);
	}

	int32_t UIAllianceGrade::grade_index_at(Point<int16_t> cursorpos) const
	{
		const Point<int16_t> rel = cursorpos - position;
		if (rel.x() < kRowLeft || rel.x() > kRowRight)
			return 0;
		for (int i = 0; i < 5; i++)
		{
			const int16_t y0 = static_cast<int16_t>(kRowTop + i * kRowH);
			if (rel.y() >= y0 && rel.y() < y0 + kRowH)
				return i + 1;
		}
		return 0;
	}

	void UIAllianceGrade::on_change_grade_name()
	{
		if (selected < 1 || selected > 5)
		{
			UI::get().emplace<UIOk>(Locale::text("alliance.grade_need_select"), [](bool) {});
			return;
		}
		if (!AllianceModel::get().data().in_alliance())
			return;

		// Angel OnChangeGradeName：CUtilDlgEx INPUT_STR → SoftKey 口径 = 物理键 Textfield。
		UI::get().emplace<UIEnterString>(
			Locale::text("alliance.grade_enter_name"),
			[](std::string name) {
				if (auto w = UI::get().get_element<UIAllianceGrade>())
					w->set_selected_grade_name(std::move(name));
			},
			kNameLimit,
			true);
	}

	void UIAllianceGrade::set_selected_grade_name(std::string name)
	{
		while (!name.empty() && (name.back() == ' ' || name.back() == '\t'))
			name.pop_back();
		size_t start = 0;
		while (start < name.size() && (name[start] == ' ' || name[start] == '\t'))
			++start;
		if (start > 0)
			name = name.substr(start);

		const int idx = selected - 1;
		if (idx < 0 || idx >= 5)
			return;

		// Angel：职级 1..3 空名不可用；4/5 允许空（删职衔）但有服务端侧校验。
		if (name.empty() && selected <= 3)
		{
			UI::get().emplace<UIOk>(Locale::text("alliance.grade_invalid"), [](bool) {});
			return;
		}

		grades[idx] = name;
		grade_labels[idx].change_text(name);

		// 职级 5 清空且 4 仍有名时，Angel 把 4 回填为 "Member"。
		if (selected == 5 && name.empty() && !grades[3].empty())
		{
			grades[3] = Locale::text("alliance.grade_member");
			grade_labels[3].change_text(grades[3]);
		}
	}

	void UIAllianceGrade::on_save_grade_name()
	{
		if (!AllianceModel::get().data().in_alliance())
			return;

		UI::get().emplace<UIYesNo>(Locale::text("alliance.grade_save_confirm"),
			[](bool yes) {
				if (!yes)
					return;
				if (auto w = UI::get().get_element<UIAllianceGrade>())
					w->confirm_save_grade_names();
			});
	}

	void UIAllianceGrade::confirm_save_grade_names()
	{
		const AllianceData& cur = AllianceModel::get().data();
		bool changed = false;
		for (int i = 0; i < 5; i++)
		{
			if (grades[i] != cur.rank_titles[i])
			{
				changed = true;
				break;
			}
		}
		if (!changed)
		{
			UI::get().emplace<UIOk>(Locale::text("alliance.grade_saved"), [](bool) {});
			return;
		}
		AllianceGradeNamePacket(
			grades[0], grades[1], grades[2], grades[3], grades[4]).dispatch();
		LOG(LOG_DEBUG, "UIAllianceGrade: SaveGradeName 0x08");
	}

	Button::State UIAllianceGrade::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::BtClose:
				deactivate();
				return Button::State::NORMAL;
			case Buttons::BtMod:
				on_change_grade_name();
				return Button::State::NORMAL;
			case Buttons::BtSave:
				on_save_grade_name();
				return Button::State::NORMAL;
			default:
				return Button::State::NORMAL;
		}
	}

	void UIAllianceGrade::send_key(int32_t /*keycode*/, bool pressed, bool escape)
	{
		if (pressed && escape)
			deactivate();
	}

	UIElement::Type UIAllianceGrade::get_type() const
	{
		return TYPE;
	}
}
