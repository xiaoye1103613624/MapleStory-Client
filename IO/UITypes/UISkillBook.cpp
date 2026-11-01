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
#include "UISkillBook.h"
#include "UIMiscShells.h"
#include "UISoftKey.h"

#include "../UI.h"

#include "../Components/MapleButton.h"
#include "../Components/TwoSpriteButton.h"

#include "../../Character/SkillId.h"
#include "../../Character/SkillMacros.h"
#include "../../Data/JobData.h"
#include "../../Data/SkillData.h"
#include "../../Gameplay/Stage.h"

#include "../../Net/Packets/AttackAndSkillPackets.h"
#include "../../Net/Packets/PlayerPackets.h"
#include "../../Net/Packets/SkillMacroPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		nl::node basic_close_btn()
		{
			// 见 UiAssetResolve::close_btn() —— 本包 BtClose 是实心方块，
			// 原生标题栏用的是描边 X（BtClose2）。
			return UiAssetResolve::close_btn();
		}

		Button* find_btn(std::map<uint16_t, std::unique_ptr<Button>>& buttons, uint16_t id)
		{
			auto it = buttons.find(id);
			return (it != buttons.end()) ? it->second.get() : nullptr;
		}
	}

	UISkillBook::SkillIcon::SkillIcon(int32_t id) : skill_id(id) {}

	void UISkillBook::SkillIcon::drop_on_bindings(Point<int16_t> cursorposition, bool remove) const
	{
		auto keyconfig = UI::get().get_element<UIKeyConfig>();
		Keyboard::Mapping mapping = Keyboard::Mapping(KeyType::SKILL, skill_id);

		if (remove)
			keyconfig->unstage_mapping(mapping);
		else
			keyconfig->stage_mapping(cursorposition, mapping);
	}

	Icon::IconType UISkillBook::SkillIcon::get_type()
	{
		return Icon::IconType::SKILL;
	}

	UISkillBook::SkillDisplayMeta::SkillDisplayMeta(int32_t i, int32_t l, bool classic) : id(i), level(l)
	{
		const SkillData& data = SkillData::get(id);

		Texture ntx = data.get_icon(SkillData::Icon::NORMAL);
		Texture dtx = data.get_icon(SkillData::Icon::DISABLED);
		Texture motx = data.get_icon(SkillData::Icon::MOUSEOVER);
		icon = std::make_unique<StatefulIcon>(std::make_unique<SkillIcon>(id), ntx, dtx, motx);

		std::string namestr = data.get_name();
		std::string levelstr = std::to_string(level);

		// 原生 v83 经典窗技能名/等级是纯黑（参考截图行内文字最暗像素 = (0,0,0)），
		// 上游的 EMPEROR (87,87,87) 只属于现代双列窗。
		const Color::Name textcolor = classic ? Color::Name::BLACK : Color::Name::EMPEROR;

		name_text = Text(Text::Font::A11M, Text::Alignment::LEFT, textcolor, namestr);
		level_text = Text(Text::Font::A11M, Text::Alignment::LEFT, textcolor, levelstr);

		constexpr uint16_t MAX_NAME_WIDTH = 97;
		size_t overhang = 3;

		while (name_text.width() > MAX_NAME_WIDTH)
		{
			namestr.replace(namestr.end() - overhang, namestr.end(), "..");
			overhang += 1;

			name_text.change_text(namestr);
		}
	}

	void UISkillBook::SkillDisplayMeta::draw(const DrawArgument& args) const
	{
		icon->draw(args.getpos());
		name_text.draw(args + Point<int16_t>(38, -5));
		level_text.draw(args + Point<int16_t>(38, 13));
	}

	int32_t UISkillBook::SkillDisplayMeta::get_id() const
	{
		return id;
	}

	int32_t UISkillBook::SkillDisplayMeta::get_level() const
	{
		return level;
	}

	StatefulIcon* UISkillBook::SkillDisplayMeta::get_icon() const
	{
		return icon.get();
	}

	UISkillBook::UISkillBook(const CharStats& in_stats, const SkillBook& in_skillbook) : UIDragElement<PosSKILL>(), stats(in_stats), skillbook(in_skillbook), grabbing(false), tab(0), macro_enabled(false), sp_enabled(false)
	{
		nl::node Skill = UiAssetResolve::window("Skill");

		if (!UiAssetResolve::usable(Skill))
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		// v83 经典窗无 main 子节点：单列布局（r05-ui-layout.md §3）。
		classic = !UiAssetResolve::usable(Skill["main"]);

		nl::node main = Skill["main"];

		if (!UiAssetResolve::usable(main))
			main = Skill;

		nl::node ui_backgrnd = main["backgrnd"] ? main["backgrnd"] : Skill["backgrnd"];

		bg_dimensions = Texture(ui_backgrnd).get_dimensions();

		skilld = main["skill0"] ? main["skill0"] : Skill["skill0"];
		skille = main["skill1"] ? main["skill1"] : Skill["skill1"];
		skillb = main["skillBlank"] ? main["skillBlank"] : Skill["skillBlank"];
		line = main["line"] ? main["line"] : Skill["line"];

		// v83 经典窗顶部 tab 条：选中页用本包自带的 Skill/recommend/0（105x35 桃色横幅），
		// 页码点图标 Tab/enabled|disabled{0-4} 作为可点按钮叠在其上（见下方 buttons）。
		// 早期实现借用了 Item/New/Tab1|Tab0（背包分类页签，34x19 粉底白点），
		// 与原生完全不同 —— 就是用户报「新手阶段的书籍和标题位置偏了」的一半根因。
		if (classic)
			recommend = Texture(Skill["recommend"][0]);

		try_maple_btn(buttons, Buttons::BT_HYPER, main["BtHyper"]);
		try_maple_btn(buttons, Buttons::BT_GUILDSKILL, main["BtGuildSkill"]);
		try_maple_btn(buttons, Buttons::BT_RIDE, main["BtRide"]);
		// 宏指令钮不在 Skill 根下：按钮贴图取 Basic.img/BtMacro（48x18），
		// 点击后打开的宏编辑窗才是独立根 UIWindow.img/Macro（见 UIMacro）。
		try_maple_btn(buttons, Buttons::BT_MACRO, nl::nx::UI["Basic.img"]["BtMacro"], MACRO_BTN_TOPLEFT, "Skill/BtMacro");

		if (auto* b = find_btn(buttons, Buttons::BT_HYPER))
			b->set_state(Button::State::DISABLED);
		if (auto* b = find_btn(buttons, Buttons::BT_GUILDSKILL))
			b->set_state(Button::State::DISABLED);
		if (auto* b = find_btn(buttons, Buttons::BT_RIDE))
			b->set_state(Button::State::DISABLED);

		nl::node skillPoint = UiAssetResolve::window4("Skill")["skillPoint"];

		if (skillPoint)
		{
			sp_backgrnd = skillPoint["backgrnd"];
			sp_backgrnd2 = skillPoint["backgrnd2"];
			sp_backgrnd3 = skillPoint["backgrnd3"];

			try_maple_btn(buttons, Buttons::BT_CANCLE, skillPoint["BtCancle"], Point<int16_t>(bg_dimensions.x(), 0));
			try_maple_btn(buttons, Buttons::BT_OKAY, skillPoint["BtOkay"], Point<int16_t>(bg_dimensions.x(), 0));
			try_maple_btn(buttons, Buttons::BT_SPDOWN, skillPoint["BtSpDown"], Point<int16_t>(bg_dimensions.x(), 0));
			try_maple_btn(buttons, Buttons::BT_SPMAX, skillPoint["BtSpMax"], Point<int16_t>(bg_dimensions.x(), 0));
			try_maple_btn(buttons, Buttons::BT_SPUP, skillPoint["BtSpUp"], Point<int16_t>(bg_dimensions.x(), 0));

			if (auto* b = find_btn(buttons, Buttons::BT_SPDOWN))
				b->set_state(Button::State::DISABLED);

			sp_before = Charset(skillPoint["num"], Charset::Alignment::RIGHT);
			sp_after = Charset(skillPoint["num"], Charset::Alignment::RIGHT);
		}

		sp_used = Text(Text::Font::A12B, Text::Alignment::RIGHT, Color::Name::WHITE);
		sp_remaining = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::SUPERNOVA);
		sp_name = Text(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::WHITE);

		if (ui_backgrnd)
			sprites.emplace_back(ui_backgrnd, Point<int16_t>(1, 0));
		if (main["backgrnd2"])
			sprites.emplace_back(main["backgrnd2"]);
		if (main["backgrnd3"])
			sprites.emplace_back(main["backgrnd3"]);

		nl::node macro = Skill["macro"];

		if (macro)
		{
			macro_backgrnd = macro["backgrnd"];
			macro_backgrnd2 = macro["backgrnd2"];
			macro_backgrnd3 = macro["backgrnd3"];

			try_maple_btn(buttons, Buttons::BT_MACRO_OK, macro["BtOK"], Point<int16_t>(bg_dimensions.x(), 0));

			// BtOK saves macros via SKILL_MACRO(110)
		}

		for (size_t i = 0; i < macro_labels.size(); i++)
			macro_labels[i] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK);

		nl::node close = basic_close_btn();

		if (close)
			buttons[Buttons::BT_CLOSE] = std::make_unique<MapleButton>(close, Point<int16_t>(bg_dimensions.x() - 23, 6));

		nl::node Tab = main["Tab"] ? main["Tab"] : Skill["Tab"];
		nl::node enabled = Tab["enabled"];
		nl::node disabled = Tab["disabled"];

		for (uint16_t i = Buttons::BT_TAB0; i <= Buttons::BT_TAB4; ++i)
		{
			uint16_t tabid = i - Buttons::BT_TAB0;

			if (disabled[tabid] && enabled[tabid])
			{
				// tab 图标是各职业段的小符号（圆/竖条…），居中放进 33px 槽位。
				// TwoSpriteButton 的 position 是 anchor，但 Tab 件 origin=(0,0)，
				// 故 anchor == 贴图左上角，直接给左上角即可。
				Point<int16_t> dim = Texture(enabled[tabid]).get_dimensions();
				int16_t slot_center = TAB_SLOT0_CENTER + TAB_SLOT_PITCH * static_cast<int16_t>(tabid);
				Point<int16_t> pos(slot_center - dim.x() / 2, TAB_CENTER_Y - dim.y() / 2);

				buttons[i] = std::make_unique<TwoSpriteButton>(disabled[tabid], enabled[tabid], pos);
			}
		}

		nl::node bt_spup = main["BtSpUp"] ? main["BtSpUp"] : Skill["BtSpUp"];
		uint16_t y_adj = 0;

		for (uint16_t i = Buttons::BT_SPUP0; i <= Buttons::BT_SPUP11; ++i)
		{
			if (!bt_spup)
				break;

			uint16_t x_adj = 0;
			uint16_t spupid = i - Buttons::BT_SPUP0;

			// v83 经典窗列表区只有 4 行，多余升级钮会画到窗口外（无裁剪）。
			if (classic && spupid >= CLASSIC_ROWS)
				break;

			if (spupid % 2)
				x_adj = ROW_WIDTH;

			// v83 经典单列：升级钮固定在行右侧（现代双列时才有第二列）。
			Point<int16_t> spup_position = classic
				? CLASSIC_SKILL_OFFSET + CLASSIC_SPUP_OFFSET + Point<int16_t>(0, static_cast<int16_t>(ROW_HEIGHT * spupid))
				: SKILL_OFFSET + Point<int16_t>(124 + x_adj, 20 + y_adj);
			buttons[i] = std::make_unique<MapleButton>(bt_spup, spup_position);

			if (spupid % 2)
				y_adj += ROW_HEIGHT;
		}

		// 经典窗书名是左对齐贴在书封暗条左缘（原生实测墨迹 x6..59），
		// 现代窗才是 150px 盒内水平居中；两者共用一个 Text，故按分支定对齐。
		booktext = Text(Text::Font::A11M,
			classic ? Text::Alignment::LEFT : Text::Alignment::CENTER,
			Color::Name::WHITE, "", classic ? 0 : 150);
		splabel = Text(Text::Font::A12M, Text::Alignment::RIGHT, Color::Name::BLACK);

		slider = classic
			? Slider(
				Slider::Type::DEFAULT_SILVER, Range<int16_t>(CLASSIC_SLIDER_TOP, CLASSIC_SLIDER_BOTTOM), CLASSIC_SLIDER_X, CLASSIC_ROWS, 1,
				[&](bool upwards)
				{
					int16_t shift = upwards ? -1 : 1;
					bool above = offset + shift >= 0;
					bool below = offset + CLASSIC_ROWS + shift <= skillcount;

					if (above && below)
						change_offset(offset + shift);
				}
				)
			: Slider(
				Slider::Type::DEFAULT_SILVER, Range<int16_t>(93, 317), 295, ROWS, 1,
				[&](bool upwards)
				{
					int16_t shift = upwards ? -1 : 1;
					bool above = offset + shift >= 0;
					bool below = offset + 4 + shift <= skillcount;

					if (above && below)
						change_offset(offset + shift);
				}
				);

		change_job(stats.get_stat(MapleStat::Id::JOB));

		set_macro(false);
		set_skillpoint(false);

		dimension = bg_dimensions;
		dragarea = Point<int16_t>(dimension.x(), 20);
	}

	void UISkillBook::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		if (classic)
		{
			// v83 经典窗（175x289）分区实测（w9-skill/coords.md + r15 复测）：
			// 标题 0-20、tab 条 20-56、书封条 55-86、列表区 99-253、技能点行 253-289。
			// tab 条选中页 = Skill/recommend/0 桃色横幅（105x35 @ (4,22)），
			// 书页图标叠在横幅左段（26x30 @ (13,25)），书名左对齐落在暗条左缘 (5,70)。
			// 原生叠放次序是 横幅 → 页码点 → 书页 → 书名，所以书页/书名要等
			// UIElement::draw_buttons 之后再画（见本函数末尾），否则 tab0 的圆点会盖住书。
			if (recommend.is_valid())
				recommend.draw(position + RECOMMEND_TOPLEFT);

			// 技能点数值：SP 白框（UIWindow.img/Skill 烘焙）边框实测窗口相对
			// x 61..111、y 265..281（内区 62..110 / 266..280）。原生数字「0」墨迹
			// = x 86..91、y 274..282（截图绝对 394..399 / 276..284，窗口原点 (308,2)）。
			// splabel 是 A12M + RIGHT：SimHei 12px 的 linespace=18、数字 bitmap_top=9、
			// 推进 6 ⇒ 墨迹顶 = draw_y + 9、墨迹左 = draw_x - 6。解出 draw = (92, 265)。
			// 旧值 (111, 258) 把数字甩到框右上角外（偏右上 19/7px）。
			splabel.draw(position + Point<int16_t>(92, 265));
		}
		else
		{
			bookicon.draw(position + Point<int16_t>(11, 85));
			booktext.draw(position + Point<int16_t>(173, 59));
			splabel.draw(position + Point<int16_t>(304, 23));
		}

		const int16_t rows = visible_rows();

		if (classic)
		{
			// 单列技能行：起点 (10,87)（列表区顶 y=87..257），行距 40，可见 4 行；
			// 现代双列布局（第二列 x=153 已越出 175 宽窗口）仅现代窗使用。
			for (int16_t i = 0; i < rows; i++)
			{
				Point<int16_t> pos = position + CLASSIC_SKILL_OFFSET + Point<int16_t>(0, static_cast<int16_t>(ROW_HEIGHT * i));
				size_t index = skill_index(i);

				if (index < skills.size())
				{
					if (check_required(skills[index].get_id()))
					{
						skille.draw(pos);
					}
					else
					{
						skilld.draw(pos);
						skills[index].get_icon()->set_state(StatefulIcon::State::DISABLED);
					}

					skills[index].draw(pos + SKILL_META_OFFSET);

					if (i < rows - 1)
						line.draw(pos + LINE_OFFSET);
				}
			}
		}
		else
		{
			Point<int16_t> skill_position_l = position + SKILL_OFFSET + Point<int16_t>(-1, 0);
			Point<int16_t> skill_position_r = position + SKILL_OFFSET + Point<int16_t>(-1 + ROW_WIDTH, 0);

			for (int16_t i = 0; i < rows; i++)
			{
				Point<int16_t> pos = skill_position_l;
				size_t index = skill_index(i);

				if (i % 2)
					pos = skill_position_r;

				if (index < skills.size())
				{
					if (check_required(skills[index].get_id()))
					{
						skille.draw(pos);
					}
					else
					{
						skilld.draw(pos);
						skills[index].get_icon()->set_state(StatefulIcon::State::DISABLED);
					}

					skills[index].draw(pos + SKILL_META_OFFSET);
				}
				else
				{
					skillb.draw(pos);
				}

				if (i < rows - 2)
					line.draw(pos + LINE_OFFSET);

				if (i % 2)
				{
					skill_position_l.shift_y(ROW_HEIGHT);
					skill_position_r.shift_y(ROW_HEIGHT);
				}
			}
		}

		slider.draw(position);

		if (macro_enabled)
		{
			Point<int16_t> macro_pos = position + Point<int16_t>(bg_dimensions.x(), 0);

			macro_backgrnd.draw(macro_pos + Point<int16_t>(1, 0));
			macro_backgrnd2.draw(macro_pos);
			macro_backgrnd3.draw(macro_pos);

			for (size_t i = 0; i < macro_labels.size(); i++)
				macro_labels[i].draw(macro_pos + Point<int16_t>(12, static_cast<int16_t>(28 + i * 42)));
		}

		if (sp_enabled)
		{
			Point<int16_t> sp_pos = position + Point<int16_t>(bg_dimensions.x(), 0);

			sp_backgrnd.draw(sp_pos);
			sp_backgrnd2.draw(sp_pos);
			sp_backgrnd3.draw(sp_pos);

			Point<int16_t> sp_level_pos = sp_pos + Point<int16_t>(78, 149);

			sp_before.draw(sp_before_text, 12, sp_level_pos);
			sp_after.draw(sp_after_text, 11, sp_level_pos + Point<int16_t>(78, 0));
			sp_used.draw(sp_pos + Point<int16_t>(82, 87));
			sp_remaining.draw(sp_pos + Point<int16_t>(76, 65));
			sp_name.draw(sp_pos + Point<int16_t>(97, 35));
			sp_skill.draw(sp_pos + Point<int16_t>(13, 31));
		}

		UIElement::draw_buttons(alpha);

		if (classic)
		{
			// 必须在按钮之后：tab 页码点是可点按钮，原生让书页图标压在它上面。
			// Texture 以 origin 为锚点（top-left = 绘制点 - origin），故加回 get_origin()
			// 才能按「左上角」落位。
			bookicon.draw(position + BOOK_TOPLEFT + bookicon.get_origin());
			booktext.draw(position + BOOK_TEXT_TOPLEFT);
		}
	}

	Button::State UISkillBook::button_pressed(uint16_t id)
	{
		int16_t cur_sp = std::stoi(splabel.get_text());

		switch (id)
		{
		case Buttons::BT_CLOSE:
			close();
			break;
		case Buttons::BT_MACRO:
			// 原生：技能窗的「宏指令」只开**技能组合栏**（UIWindow.img/SkillMacro 207x289），
			// 且紧贴技能窗右侧。旧实现一次 emplace UIMacro / UISkillMacroEx / UISkillEx
			// (+ 法师的 UIAranSkillGuide) 四个窗，且它们构造函数里还会再弹 UISoftKey
			// （安全键盘 = 二级密码那张 148x278 底板），于是一点击糊满屏幕。
			set_macro(!macro_enabled);
			if (macro_enabled)
			{
				auto mex = UI::get().get_element<UISkillMacroEx>();
				if (!mex)
					mex = UI::get().emplace<UISkillMacroEx>();
				else if (!mex->is_active())
					mex->makeactive();

				// gap = -3：原生帧里技能窗 175 宽起于 x=0，组合栏起于 x=172（重叠 3px），
				// 两窗同高 289 ⇒ 顶边对齐。见 evidence/r15-skillmacro/README.md。
				if (mex)
					mex->anchor_right_of(*this, -3);
			}
			break;
		case Buttons::BT_CANCLE:
			set_skillpoint(false);
			break;
		case Buttons::BT_OKAY:
		{
			int32_t used = std::stoi(sp_used.get_text());

			while (used > 0)
			{
				spend_sp(sp_id);
				used--;
			}

			change_sp();
			set_skillpoint(false);
		}
		break;
		case Buttons::BT_SPDOWN:
		{
			int32_t used = std::stoi(sp_used.get_text());
			int32_t sp_after = std::stoi(sp_after_text);
			int32_t sp_before = std::stoi(sp_before_text);
			used--;
			sp_after--;

			sp_after_text = std::to_string(sp_after);
			sp_used.change_text(std::to_string(used));
			sp_remaining.change_text(std::to_string(cur_sp - used));

			if (auto* b = find_btn(buttons, Buttons::BT_SPUP))
				b->set_state(Button::State::NORMAL);
			if (auto* b = find_btn(buttons, Buttons::BT_SPMAX))
				b->set_state(Button::State::NORMAL);

			if (sp_after - 1 == sp_before)
				return Button::State::DISABLED;

			return Button::State::NORMAL;
		}
		break;
		case Buttons::BT_SPMAX:
		{
			int32_t used = std::stoi(sp_used.get_text());
			int32_t sp_before = std::stoi(sp_before_text);
			int32_t sp_touse = sp_masterlevel - sp_before - used;

			used += sp_touse;

			sp_after_text = std::to_string(sp_masterlevel);
			sp_used.change_text(std::to_string(used));
			sp_remaining.change_text(std::to_string(cur_sp - used));

			if (auto* b = find_btn(buttons, Buttons::BT_SPUP))
				b->set_state(Button::State::DISABLED);
			if (auto* b = find_btn(buttons, Buttons::BT_SPDOWN))
				b->set_state(Button::State::NORMAL);

			return Button::State::DISABLED;
		}
		break;
		case Buttons::BT_SPUP:
		{
			int32_t used = std::stoi(sp_used.get_text());
			int32_t sp_after = std::stoi(sp_after_text);
			used++;
			sp_after++;

			sp_after_text = std::to_string(sp_after);
			sp_used.change_text(std::to_string(used));
			sp_remaining.change_text(std::to_string(cur_sp - used));

			if (auto* b = find_btn(buttons, Buttons::BT_SPDOWN))
				b->set_state(Button::State::NORMAL);

			if (sp_after == sp_masterlevel)
			{
				if (auto* b = find_btn(buttons, Buttons::BT_SPMAX))
					b->set_state(Button::State::DISABLED);

				return Button::State::DISABLED;
			}

			return Button::State::NORMAL;
		}
		break;
		case Buttons::BT_TAB0:
		case Buttons::BT_TAB1:
		case Buttons::BT_TAB2:
		case Buttons::BT_TAB3:
		case Buttons::BT_TAB4:
			change_tab(id - Buttons::BT_TAB0);

			return Button::State::PRESSED;
		case Buttons::BT_SPUP0:
		case Buttons::BT_SPUP1:
		case Buttons::BT_SPUP2:
		case Buttons::BT_SPUP3:
		case Buttons::BT_SPUP4:
		case Buttons::BT_SPUP5:
		case Buttons::BT_SPUP6:
		case Buttons::BT_SPUP7:
		case Buttons::BT_SPUP8:
		case Buttons::BT_SPUP9:
		case Buttons::BT_SPUP10:
		case Buttons::BT_SPUP11:
			send_spup(id - Buttons::BT_SPUP0 + offset);
			break;
		case Buttons::BT_MACRO_OK:
			ChangeSkillMacroPacket().dispatch();
			LOG(LOG_DEBUG, "UISkillBook: SKILL_MACRO saved slots=" << SkillMacros::get().size());
			set_macro(false);
			break;
		case Buttons::BT_HYPER:
		case Buttons::BT_GUILDSKILL:
		case Buttons::BT_RIDE:
		default:
			break;
		}

		return Button::State::NORMAL;
	}

	void UISkillBook::toggle_active()
	{
		if (!is_skillpoint_enabled())
		{
			UIElement::toggle_active();

			clear_tooltip();
		}
	}

	void UISkillBook::doubleclick(Point<int16_t> cursorpos)
	{
		const SkillDisplayMeta* skill = skill_by_position(cursorpos - position);

		if (skill)
		{
			int32_t skill_id = skill->get_id();
			int32_t skill_level = skillbook.get_level(skill_id);

			if (skill_level > 0)
				Stage::get().get_combat().use_move(skill_id);
		}
	}

	void UISkillBook::remove_cursor()
	{
		UIDragElement::remove_cursor();

		slider.remove_cursor();
	}

	Cursor::State UISkillBook::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State dstate = UIDragElement::send_cursor(clicked, cursorpos);

		if (dragged)
			return dstate;

		Point<int16_t> cursor_relative = cursorpos - position;

		if (slider.isenabled())
		{
			if (Cursor::State new_state = slider.send_cursor(cursor_relative, clicked))
			{
				clear_tooltip();

				return new_state;
			}
		}

		// v83 经典单列：左右列同点、每行推进；现代双列：奇偶分列、两行推进。
		Point<int16_t> skill_position_l = classic
			? position + CLASSIC_SKILL_OFFSET
			: position + SKILL_OFFSET + Point<int16_t>(-1, 0);
		Point<int16_t> skill_position_r = classic
			? position + CLASSIC_SKILL_OFFSET
			: position + SKILL_OFFSET + Point<int16_t>(-1 + ROW_WIDTH, 0);

		if (!grabbing)
		{
			// v83 经典单列：一行一个技能、每行推进；现代双列：奇偶分列、每两行推进。
			const int16_t rows = visible_rows();

			for (int16_t r = 0; r < rows; r++)
			{
				size_t i = skill_index(r);

				if (i >= skills.size())
					break;

				Point<int16_t> skill_position = skill_position_l;

				if (!classic && r % 2)
					skill_position = skill_position_r;

				constexpr Rectangle<int16_t> bounds = Rectangle<int16_t>(0, 32, 0, 32);
				bool inrange = bounds.contains(cursorpos - skill_position);

				if (inrange)
				{
					selected_skill_id = skills[i].get_id();
					if (clicked)
					{
						clear_tooltip();
						grabbing = true;

						int32_t skill_id = skills[i].get_id();
						int32_t skill_level = skillbook.get_level(skill_id);

						if (skill_level > 0 && !SkillData::get(skill_id).is_passive())
						{
							skills[i].get_icon()->start_drag(cursorpos - skill_position);
							UI::get().drag_icon(skills[i].get_icon());

							return Cursor::State::GRABBING;
						}
						else
						{
							return Cursor::State::IDLE;
						}
					}
					else
					{
						skills[i].get_icon()->set_state(StatefulIcon::State::MOUSEOVER);
						show_skill(skills[i].get_id());

						return Cursor::State::IDLE;
					}
				}

				if (classic || r % 2)
				{
					skill_position_l.shift_y(ROW_HEIGHT);
					skill_position_r.shift_y(ROW_HEIGHT);
				}
			}

			for (size_t i = 0; i < skills.size(); i++)
				skills[i].get_icon()->set_state(StatefulIcon::State::NORMAL);

			clear_tooltip();
		}
		else
		{
			grabbing = false;
		}

		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UISkillBook::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
			{
				if (sp_enabled)
					set_skillpoint(false);
				else
					close();
			}
			else if (keycode == KeyAction::Id::TAB)
			{
				clear_tooltip();

				Job::Level level = job.get_level();
				uint16_t id = tab + 1;
				uint16_t new_tab = tab + Buttons::BT_TAB0;

				if (new_tab < Buttons::BT_TAB4 && id <= level)
					new_tab++;
				else
					new_tab = Buttons::BT_TAB0;

				change_tab(new_tab - Buttons::BT_TAB0);
			}
			else if (keycode == 'S' || keycode == 's')
			{
				if (selected_skill_id > 0) {
					SpendSpPacket(selected_skill_id).dispatch();
					LOG(LOG_DEBUG, "UISkillBook: SpendSp selected id=" << selected_skill_id);
				} else {
					UI::get().emplace<UISoftKey>([this](const std::string& s) {
						try {
							int32_t sid = std::stoi(s);
							if (sid > 0) {
								selected_skill_id = sid;
								SpendSpPacket(sid).dispatch();
								LOG(LOG_DEBUG, "UISkillBook: SoftKey SpendSp id=" << sid);
							}
						} catch (...) {}
					}, []() {}, "Skill id SP");
				}
			}
			else if (keycode == 'U' || keycode == 'u' || keycode == 'F' || keycode == 'f')
			{
				if (selected_skill_id > 0) {
					int32_t id = selected_skill_id;
					int32_t lv = 1;
					try {
						lv = Stage::get().get_player().get_skills().get_level(id);
						if (lv <= 0) lv = 1;
					} catch (...) {}
					UseSkillPacket(id, lv).dispatch();
					LOG(LOG_DEBUG, "UISkillBook: UseSkill selected id=" << id << " lv=" << lv);
				} else {
					UI::get().emplace<UISoftKey>([this](const std::string& s) {
						try {
							int32_t id = std::stoi(s);
							if (id <= 0) return;
							selected_skill_id = id;
							int32_t lv = 1;
							try {
								lv = Stage::get().get_player().get_skills().get_level(id);
								if (lv <= 0) lv = 1;
							} catch (...) {}
							UseSkillPacket(id, lv).dispatch();
							LOG(LOG_DEBUG, "UISkillBook: SoftKey UseSkill id=" << id << " lv=" << lv);
						} catch (...) {}
					}, []() {}, "Skill id use");
				}
			}
			else if (keycode == 'T' || keycode == 't')
			{
				auto ui = UI::get().get_element<UITransform>();
				if (!ui) ui = UI::get().emplace<UITransform>();
				else if (!ui->is_active()) ui->makeactive();
			}
			else if (keycode == 'P' || keycode == 'p')
			{
				auto ui = UI::get().get_element<UISkillUp>();
				if (!ui) ui = UI::get().emplace<UISkillUp>();
				else if (!ui->is_active()) ui->makeactive();
			}
		}
	}

	UIElement::Type UISkillBook::get_type() const
	{
		return TYPE;
	}

	void UISkillBook::update_stat(MapleStat::Id stat, int16_t value)
	{
		switch (stat)
		{
		case MapleStat::Id::JOB:
			change_job(value);
			break;
		case MapleStat::Id::SP:
			change_sp();
			break;
		}
	}

	void UISkillBook::update_skills(int32_t skill_id)
	{
		change_tab(tab);
	}

	void UISkillBook::change_job(uint16_t id)
	{
		job.change_job(id);

		Job::Level level = job.get_level();

		for (uint16_t i = 0; i <= Job::Level::FOURTH; i++)
			if (auto* b = find_btn(buttons, Buttons::BT_TAB0 + i))
				b->set_active(i <= level);

		change_tab(level - Job::Level::BEGINNER);
	}

	void UISkillBook::change_sp()
	{
		Job::Level joblevel = joblevel_by_tab(tab);
		uint16_t level = stats.get_stat(MapleStat::Id::LEVEL);

		if (joblevel == Job::Level::BEGINNER)
		{
			int16_t remaining_beginner_sp = 0;

			if (level >= 7)
				remaining_beginner_sp = 6;
			else
				remaining_beginner_sp = level - 1;

			for (size_t i = 0; i < skills.size(); i++)
			{
				int32_t skillid = skills[i].get_id();

				if (skillid == SkillId::Id::THREE_SNAILS || skillid == SkillId::Id::HEAL || skillid == SkillId::Id::FEATHER)
					remaining_beginner_sp -= skills[i].get_level();
			}

			beginner_sp = remaining_beginner_sp;
			splabel.change_text(std::to_string(beginner_sp));
		}
		else
		{
			sp = stats.get_stat(MapleStat::Id::SP);
			splabel.change_text(std::to_string(sp));
		}

		change_offset(offset);
		set_skillpoint(false);
	}

	void UISkillBook::change_tab(uint16_t new_tab)
	{
		if (auto* b = find_btn(buttons, Buttons::BT_TAB0 + tab))
			b->set_state(Button::NORMAL);
		if (auto* b = find_btn(buttons, Buttons::BT_TAB0 + new_tab))
			b->set_state(Button::PRESSED);
		tab = new_tab;

		skills.clear();
		skillcount = 0;

		Job::Level joblevel = joblevel_by_tab(tab);
		uint16_t subid = job.get_subjob(joblevel);

		const JobData& data = JobData::get(subid);

		bookicon = data.get_icon();
		booktext.change_text(data.get_name());

		for (int32_t skill_id : data.get_skills())
		{
			int32_t level = skillbook.get_level(skill_id);
			int32_t masterlevel = skillbook.get_masterlevel(skill_id);

			bool invisible = SkillData::get(skill_id).is_invisible();

			if (invisible && masterlevel == 0)
				continue;

			skills.emplace_back(skill_id, level, classic);
			skillcount++;
		}

		slider.setrows(visible_rows(), skillcount);
		change_offset(0);
		change_sp();
	}

	int16_t UISkillBook::visible_rows() const
	{
		return classic ? CLASSIC_ROWS : ROWS;
	}

	size_t UISkillBook::skill_index(int16_t row) const
	{
		// 现代双列：offset 以「行对」为单位，可见第 row 个槽位对应 2*offset + row。
		// 经典单列：一行一个技能，直接 offset + row。
		return classic
			? static_cast<size_t>(offset + row)
			: static_cast<size_t>(2 * offset + row);
	}

	void UISkillBook::change_offset(uint16_t new_offset)
	{
		offset = new_offset;

		for (int16_t i = 0; i < visible_rows(); i++)
		{
			uint16_t index = Buttons::BT_SPUP0 + i;
			size_t row = skill_index(i);
			auto* btn = find_btn(buttons, index);

			if (!btn)
				continue;

			btn->set_active(row < skillcount);

			if (row < skills.size())
			{
				int32_t skill_id = skills[row].get_id();
				bool canraise = can_raise(skill_id);
				btn->set_state(canraise ? Button::State::NORMAL : Button::State::DISABLED);
			}
		}
	}

	void UISkillBook::show_skill(int32_t id)
	{
		int32_t skill_id = id;
		int32_t level = skillbook.get_level(id);
		int32_t masterlevel = skillbook.get_masterlevel(id);
		int64_t expiration = skillbook.get_expiration(id);

		UI::get().show_skill(Tooltip::Parent::SKILLBOOK, skill_id, level, masterlevel, expiration);
	}

	void UISkillBook::clear_tooltip()
	{
		UI::get().clear_tooltip(Tooltip::Parent::SKILLBOOK);
	}

	bool UISkillBook::can_raise(int32_t skill_id) const
	{
		Job::Level joblevel = joblevel_by_tab(tab);

		if (joblevel == Job::Level::BEGINNER && beginner_sp <= 0)
			return false;

		if (tab + Buttons::BT_TAB0 != Buttons::BT_TAB0 && sp <= 0)
			return false;

		int32_t level = skillbook.get_level(skill_id);
		int32_t masterlevel = skillbook.get_masterlevel(skill_id);

		if (masterlevel == 0)
			masterlevel = SkillData::get(skill_id).get_masterlevel();

		if (level >= masterlevel)
			return false;

		switch (skill_id)
		{
		case SkillId::Id::ANGEL_BLESSING:
			return false;
		default:
			return check_required(skill_id);
		}
	}

	void UISkillBook::send_spup(uint16_t row)
	{
		if (row >= skills.size())
			return;

		int32_t id = skills[row].get_id();

		if (sp_enabled && id == sp_id)
		{
			set_skillpoint(false);
			return;
		}

		int32_t level = skills[row].get_level();
		int32_t used = 1;

		const SkillData& skillData = SkillData::get(id);
		std::string name = skillData.get_name();
		int16_t cur_sp = std::stoi(splabel.get_text());

		sp_before_text = std::to_string(level);
		sp_after_text = std::to_string(level + used);
		sp_used.change_text(std::to_string(used));
		sp_remaining.change_text(std::to_string(cur_sp - used));
		sp_name.change_text(name);
		sp_skill = skills[row].get_icon()->get_texture();
		sp_id = id;
		sp_masterlevel = skillData.get_masterlevel();

		if (sp_masterlevel == 1)
		{
			if (auto* b = find_btn(buttons, Buttons::BT_SPDOWN)) b->set_state(Button::State::DISABLED);
			if (auto* b = find_btn(buttons, Buttons::BT_SPMAX)) b->set_state(Button::State::DISABLED);
			if (auto* b = find_btn(buttons, Buttons::BT_SPUP)) b->set_state(Button::State::DISABLED);
		}
		else
		{
			if (auto* b = find_btn(buttons, Buttons::BT_SPDOWN)) b->set_state(Button::State::DISABLED);
			if (auto* b = find_btn(buttons, Buttons::BT_SPMAX)) b->set_state(Button::State::NORMAL);
			if (auto* b = find_btn(buttons, Buttons::BT_SPUP)) b->set_state(Button::State::NORMAL);
		}

		if (!sp_enabled)
			set_skillpoint(true);
	}

	void UISkillBook::spend_sp(int32_t skill_id)
	{
		SpendSpPacket(skill_id).dispatch();

		UI::get().disable();
	}

	Job::Level UISkillBook::joblevel_by_tab(uint16_t t) const
	{
		switch (t)
		{
		case 1:
			return Job::Level::FIRST;
		case 2:
			return Job::Level::SECOND;
		case 3:
			return Job::Level::THIRD;
		case 4:
			return Job::Level::FOURTH;
		default:
			return Job::Level::BEGINNER;
		}
	}

	const UISkillBook::SkillDisplayMeta* UISkillBook::skill_by_position(Point<int16_t> cursorpos) const
	{
		int16_t x = cursorpos.x();

		// v83 经典单列：行起点 y=87；现代双列见下。
		const int16_t base_x = classic ? CLASSIC_SKILL_OFFSET.x() : SKILL_OFFSET.x();
		const int16_t base_y = classic ? CLASSIC_SKILL_OFFSET.y() : SKILL_OFFSET.y();

		if (x < base_x || x > base_x + (classic ? ROW_WIDTH : 2 * ROW_WIDTH))
			return nullptr;

		int16_t y = cursorpos.y();

		if (y < base_y)
			return nullptr;

		uint16_t row = (y - base_y) / ROW_HEIGHT;

		if (row >= visible_rows())
			return nullptr;

		uint16_t col = classic ? 0 : (x - base_x) / ROW_WIDTH;

		size_t skill_idx = skill_index(static_cast<int16_t>(row)) + col;

		if (skill_idx >= skills.size())
			return nullptr;

		auto iter = skills.data() + skill_idx;

		return iter;
	}

	void UISkillBook::close()
	{
		clear_tooltip();
		deactivate();
	}

	bool UISkillBook::check_required(int32_t id) const
	{
		std::unordered_map<int32_t, int32_t> required = skillbook.collect_required(id);

		if (required.size() <= 0)
			required = SkillData::get(id).get_reqskills();

		for (auto reqskill : required)
		{
			int32_t reqskill_level = skillbook.get_level(reqskill.first);
			int32_t req_level = reqskill.second;

			if (reqskill_level < req_level)
				return false;
		}

		return true;
	}

	void UISkillBook::refresh_macros()
	{
		const auto& macros = SkillMacros::get();
		for (size_t i = 0; i < macro_labels.size(); i++)
		{
			const auto& slot = macros.get_slot(i);
			std::string label = slot.name.empty()
				? ("#" + std::to_string(i + 1))
				: slot.name;
			if (slot.skill1 || slot.skill2 || slot.skill3) {
				label += " [";
				if (slot.skill1) label += std::to_string(slot.skill1);
				if (slot.skill2) label += "," + std::to_string(slot.skill2);
				if (slot.skill3) label += "," + std::to_string(slot.skill3);
				label += "]";
			}
			macro_labels[i].change_text(label);
		}
	}

	void UISkillBook::set_macro(bool enabled)
	{
		macro_enabled = enabled;

		if (macro_enabled)
		{
			dimension = bg_dimensions + Point<int16_t>(macro_backgrnd.get_dimensions().x(), 0);
			refresh_macros();
		}
		else if (!sp_enabled)
		{
			dimension = bg_dimensions;
		}

		if (auto* b = find_btn(buttons, Buttons::BT_MACRO_OK))
			b->set_active(macro_enabled);

		if (macro_enabled && sp_enabled)
			set_skillpoint(false);
	}

	bool UISkillBook::send_icon(const Icon& icon, Point<int16_t>)
	{
		if (!macro_enabled)
			return true;

		int32_t sid = icon.get_skill_id();
		if (sid <= 0)
			return true;

		if (SkillMacros::get().assign_skill(sid)) {
			LOG(LOG_DEBUG, "UISkillBook: macro assign skill=" << sid);
			set_macro(true); // refresh labels
		}
		return true;
	}

	void UISkillBook::set_skillpoint(bool enabled)
	{
		sp_enabled = enabled && find_btn(buttons, Buttons::BT_OKAY) != nullptr;

		if (sp_enabled)
			dimension = bg_dimensions + Point<int16_t>(sp_backgrnd.get_dimensions().x(), 0);
		else if (!macro_enabled)
			dimension = bg_dimensions;

		if (auto* b = find_btn(buttons, Buttons::BT_CANCLE)) b->set_active(sp_enabled);
		if (auto* b = find_btn(buttons, Buttons::BT_OKAY)) b->set_active(sp_enabled);
		if (auto* b = find_btn(buttons, Buttons::BT_SPDOWN)) b->set_active(sp_enabled);
		if (auto* b = find_btn(buttons, Buttons::BT_SPMAX)) b->set_active(sp_enabled);
		if (auto* b = find_btn(buttons, Buttons::BT_SPUP)) b->set_active(sp_enabled);

		if (sp_enabled && macro_enabled)
			set_macro(false);
	}

	bool UISkillBook::is_skillpoint_enabled()
	{
		return sp_enabled;
	}
}