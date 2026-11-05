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
#include "EquipTooltip.h"

#include "../../Configuration.h"
#include "../../Data/WeaponData.h"
#include "../../Gameplay/Stage.h"
#include "../../Util/Locale.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	EquipTooltip::EquipTooltip()
	{
		nl::node Equip = nl::nx::UI["UIWindow.img"]["ToolTip"]["Equip"];
		nl::node EquipCan = Equip["Can"];
		nl::node EquipCannot = Equip["Cannot"];

		separator = ColorLine(FRAME_WIDTH, Color::Name::WHITE, 0.4f, false);

		requirements.push_back(MapleStat::Id::LEVEL);
		requirements.push_back(MapleStat::Id::STR);
		requirements.push_back(MapleStat::Id::DEX);
		requirements.push_back(MapleStat::Id::INT);
		requirements.push_back(MapleStat::Id::LUK);

		reqstattextures[MapleStat::Id::LEVEL][false] = EquipCannot["reqLEV"];
		reqstattextures[MapleStat::Id::LEVEL][true] = EquipCan["reqLEV"];
		reqstattextures[MapleStat::Id::FAME][false] = EquipCannot["reqPOP"];
		reqstattextures[MapleStat::Id::FAME][true] = EquipCan["reqPOP"];
		reqstattextures[MapleStat::Id::STR][false] = EquipCannot["reqSTR"];
		reqstattextures[MapleStat::Id::STR][true] = EquipCan["reqSTR"];
		reqstattextures[MapleStat::Id::DEX][false] = EquipCannot["reqDEX"];
		reqstattextures[MapleStat::Id::DEX][true] = EquipCan["reqDEX"];
		reqstattextures[MapleStat::Id::INT][false] = EquipCannot["reqINT"];
		reqstattextures[MapleStat::Id::INT][true] = EquipCan["reqINT"];
		reqstattextures[MapleStat::Id::LUK][false] = EquipCannot["reqLUK"];
		reqstattextures[MapleStat::Id::LUK][true] = EquipCan["reqLUK"];

		// 三行需求位的行距必须一致（15px）。DEX/INT 原先是 y=71，与上一行 STR/LUK(y=62)
		// 只差 9px，而 A11M 行高约 15px ⇒ 线上实测第 2、3 行文字直接叠成糊块
		// （见 docs/features/ui-v83-window-restore/notes/open-issues.md §B8.2）。
		reqstatpositions[MapleStat::Id::LEVEL] = Point<int16_t>(97, 47);
		reqstatpositions[MapleStat::Id::STR] = Point<int16_t>(97, 62);
		reqstatpositions[MapleStat::Id::LUK] = Point<int16_t>(177, 62);
		reqstatpositions[MapleStat::Id::DEX] = Point<int16_t>(97, 77);
		reqstatpositions[MapleStat::Id::INT] = Point<int16_t>(177, 77);

		reqset[false] = Charset(EquipCannot, Charset::Alignment::LEFT);
		reqset[true] = Charset(EquipCan, Charset::Alignment::LEFT);

		// 包内无 YellowNumber，可装备态改用 Can 数字字模（绿）
		lvset[false] = Charset(EquipCannot, Charset::Alignment::LEFT);
		lvset[true] = Charset(EquipCan, Charset::Alignment::LEFT);

		// 包内无 Job enable/disable 四态节点；Can/Cannot 下的职业名位图即原生职业名图标
		static const char* const jobnames[6] = {"beginner", "warrior", "magician", "bowman", "thief", "pirate"};

		for (uint8_t i = 0; i < 6; ++i)
		{
			jobs[false][i] = EquipCannot[jobnames[i]];
			jobs[true][i] = EquipCan[jobnames[i]];
		}

		invpos = 0;
		invpos_preview = 0;
		show_compare = Configuration::get().get_equip_compare();
	}

	void EquipTooltip::set_equip(Tooltip::Parent parent, int16_t ivp)
	{
		if (invpos == ivp)
			return;

		invpos = ivp;
		invpos_preview = 0;

		const Player& player = Stage::get().get_player();

		InventoryType::Id invtype;

		switch (parent)
		{
		case Tooltip::Parent::ITEMINVENTORY:
		case Tooltip::Parent::SHOP:
			invtype = InventoryType::Id::EQUIP;
			break;
		case Tooltip::Parent::EQUIPINVENTORY:
			invtype = InventoryType::Id::EQUIPPED;
			break;
		default:
			invtype = InventoryType::Id::NONE;
		}

		const Inventory& inventory = player.get_inventory();
		auto oequip = inventory.get_equip(invtype, invpos);
		const CharStats& stats = player.get_stats();

		if (show_compare && invtype == InventoryType::Id::EQUIP && oequip)
		{
			const int32_t item_id = oequip.get()->get_item_id();
			const EquipData& equipdata = EquipData::get(item_id);
			EquipSlot::Id eqslot = equipdata.get_eqslot();

			if (inventory.has_equipped(eqslot))
			{
				auto eequip = inventory.get_equip(InventoryType::Id::EQUIPPED, eqslot);

				if (eequip)
				{
					const Equip& equip = *eequip;

					int32_t item_id = equip.get_item_id();

					const EquipData& equipdata = EquipData::get(item_id);
					const ItemData& itemdata = equipdata.get_itemdata();

					height_preview = 540;

					itemicon_preview = itemdata.get_icon(false);

					for (auto& ms : requirements)
					{
						canequip_preview[ms] = stats.get_stat(ms) >= equipdata.get_reqstat(ms);
						std::string reqstr = std::to_string(equipdata.get_reqstat(ms));

						if (ms != MapleStat::Id::LEVEL)
							reqstr.insert(0, 3 - reqstr.size(), '0');

						reqstatstrings_preview[ms] = reqstr;
					}

					okjobs_preview.clear();

					// TODO: Remove from UIItemInventory and move into Job?
					switch (equipdata.get_reqstat(MapleStat::Id::JOB))
					{
					case 0:
						okjobs_preview.push_back(0);
						okjobs_preview.push_back(1);
						okjobs_preview.push_back(2);
						okjobs_preview.push_back(3);
						okjobs_preview.push_back(4);
						okjobs_preview.push_back(5);
						canequip_preview[MapleStat::Id::JOB] = true;
						break;
					case 1:
						okjobs_preview.push_back(1);
						canequip_preview[MapleStat::Id::JOB] = (stats.get_stat(MapleStat::Id::JOB) / 100 == 1) || (stats.get_stat(MapleStat::Id::JOB) / 100 >= 20);
						break;
					case 2:
						okjobs_preview.push_back(2);
						canequip_preview[MapleStat::Id::JOB] = stats.get_stat(MapleStat::Id::JOB) / 100 == 2;
						break;
					case 4:
						okjobs_preview.push_back(3);
						canequip_preview[MapleStat::Id::JOB] = stats.get_stat(MapleStat::Id::JOB) / 100 == 3;
						break;
					case 8:
						okjobs_preview.push_back(4);
						canequip_preview[MapleStat::Id::JOB] = stats.get_stat(MapleStat::Id::JOB) / 100 == 4;
						break;
					case 16:
						okjobs_preview.push_back(5);
						canequip_preview[MapleStat::Id::JOB] = stats.get_stat(MapleStat::Id::JOB) / 100 == 5;
						break;
					default:
						canequip_preview[MapleStat::Id::JOB] = false;
						break;
					}
					// End of TODO

					prank_preview = equip.get_potrank();

					switch (prank_preview)
					{
					case Equip::Potential::POT_HIDDEN:
						potflag_preview = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::RED);
						potflag_preview.change_text(Locale::text("equip.pot.hidden"));
						break;
					case Equip::Potential::POT_RARE:
						potflag_preview = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE);
						potflag_preview.change_text(Locale::text("equip.pot.rare"));
						break;
					case Equip::Potential::POT_EPIC:
						potflag_preview = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE);
						potflag_preview.change_text(Locale::text("equip.pot.epic"));
						break;
					case Equip::Potential::POT_UNIQUE:
						potflag_preview = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE);
						potflag_preview.change_text(Locale::text("equip.pot.unique"));
						break;
					case Equip::Potential::POT_LEGENDARY:
						potflag_preview = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE);
						potflag_preview.change_text(Locale::text("equip.pot.legendary"));
						break;
					default:
						height_preview -= 16;
						break;
					}

					Color::Name namecolor;

					switch (equip.get_quality())
					{
					case EquipQuality::Id::GREY:
						namecolor = Color::Name::LIGHTGREY;
						break;
					case EquipQuality::Id::ORANGE:
						namecolor = Color::Name::ORANGE;
						break;
					case EquipQuality::Id::BLUE:
						namecolor = Color::Name::MEDIUMBLUE;
						break;
					case EquipQuality::Id::VIOLET:
						namecolor = Color::Name::VIOLET;
						break;
					case EquipQuality::Id::GOLD:
						namecolor = Color::Name::YELLOW;
						break;
					default:
						namecolor = Color::Name::WHITE;
						break;
					}

					std::string namestr = itemdata.get_name();
					const int8_t reqGender = itemdata.get_gender();
					const bool female = stats.get_female();

					switch (reqGender)
					{
					case 0: // Male
						namestr += " " + Locale::text("equip.gender.male");
						break;
					case 1: // Female
						namestr += " " + Locale::text("equip.gender.female");
						break;
					case 2: // Unisex
					default:
						break;
					}

					if (equip.get_level() > 0)
					{
						namestr.append(" (+");
						namestr.append(std::to_string(equip.get_level()));
						namestr.append(")");
					}

					name_preview = Text(Text::Font::A12B, Text::Alignment::LEFT, namecolor, namestr, 400);
					atkinc_preview = Text(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::DUSTYGRAY, Locale::text("equip.label_atkinc"));

					std::string desctext = itemdata.get_desc();
					hasdesc_preview = desctext.size() > 0;

					if (hasdesc_preview)
					{
						desc_preview = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, desctext, 250);
						height_preview += desc_preview.height() + 10;
					}

					category_preview = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::format("equip.label_type", Locale::enum_text("equip.type.", equipdata.get_type())));

					is_weapon_preview = equipdata.is_weapon();

					if (is_weapon_preview)
					{
						const WeaponData& weapon = WeaponData::get(item_id);
						wepspeed_preview = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::format("equip.label_wepspeed", Locale::enum_text("equip.speed.", weapon.getspeedstring())));
					}
					else
					{
						height_preview -= 18;
					}

					hasslots_preview = (equip.get_slots() > 0) || (equip.get_level() > 0);

					if (hasslots_preview)
					{
						slots_preview = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::format("equip.label_slots", std::to_string(equip.get_slots())));

						std::string vicious = std::to_string(equip.get_vicious());

						if (equip.get_vicious() > 1)
							vicious.append(" " + Locale::text("equip.label_max") + " ");

						hammers_preview = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::format("equip.label_hammers", vicious));
					}
					else
					{
						height_preview -= 36;
					}

					statlabels_preview.clear();
					statdeltas_preview.clear();

					for (EquipStat::Id es = EquipStat::Id::STR; es <= EquipStat::Id::JUMP; es = static_cast<EquipStat::Id>(es + 1))
					{
						const uint16_t equipped_value = equip.get_stat(es);
						const uint16_t hovered_value = oequip.get()->get_stat(es);

						// 两侧都没有数值就不占行；任一侧存在才输出，才能展示「新装备多出的属性」
						if (equipped_value == 0 && hovered_value == 0)
						{
							height_preview -= 18;
							continue;
						}

						// 已装备物品自身的卷轴加成，保持与普通 tooltip 一致
						int16_t scroll = equipped_value - equipdata.get_defstat(es);
						std::string statstr = std::to_string(equipped_value);

						if (scroll != 0)
						{
							statstr.append(" (");
							statstr.append((scroll < 0) ? "-" : "+");
							statstr.append(std::to_string(abs(scroll)) + ")");
						}

						statlabels_preview[es] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::format(Locale::enum_key("equip.stat.", EquipStat::names[es]), statstr));

						// 悬停物品相对已装备物品的差值：正数绿色 (+N)，负数红色 (N)
						int32_t diff = static_cast<int32_t>(hovered_value) - static_cast<int32_t>(equipped_value);

						if (diff != 0)
						{
							std::string diffstr = " (";

							if (diff > 0)
								diffstr += "+";

							diffstr += std::to_string(diff) + ")";

							statdeltas_preview[es] = Text(
								Text::Font::A11M,
								Text::Alignment::LEFT,
								diff > 0 ? Color::Name::GREEN : Color::Name::RED,
								diffstr
							);
						}
					}

					invpos_preview = 1;
				}
			}
		}

		if (!oequip)
			return;

		const Equip& equip = *oequip;

		int32_t item_id = equip.get_item_id();

		const EquipData& equipdata = EquipData::get(item_id);
		const ItemData& itemdata = equipdata.get_itemdata();

		height = 540;

		itemicon = itemdata.get_icon(false);

		for (auto& ms : requirements)
		{
			canequip[ms] = stats.get_stat(ms) >= equipdata.get_reqstat(ms);
			std::string reqstr = std::to_string(equipdata.get_reqstat(ms));

			if (ms != MapleStat::Id::LEVEL && reqstr.size() < 3)
				reqstr.insert(0, 3 - reqstr.size(), '0');

			reqstatstrings[ms] = reqstr;
		}

		okjobs.clear();

		switch (equipdata.get_reqstat(MapleStat::Id::JOB))
		{
		case 0: // Common
			okjobs.push_back(0);
			okjobs.push_back(1);
			okjobs.push_back(2);
			okjobs.push_back(3);
			okjobs.push_back(4);
			okjobs.push_back(5);
			canequip[MapleStat::Id::JOB] = true;
			break;
		case 1: // Warrior
			okjobs.push_back(1);
			canequip[MapleStat::Id::JOB] = (stats.get_stat(MapleStat::Id::JOB) / 100 == 1) || (stats.get_stat(MapleStat::Id::JOB) / 100 >= 20);
			break;
		case 2: // Magician
			okjobs.push_back(2);
			canequip[MapleStat::Id::JOB] = stats.get_stat(MapleStat::Id::JOB) / 100 == 2;
			break;
		case 3: // Magician, Warrior
			okjobs.push_back(1);
			okjobs.push_back(2);
			canequip[MapleStat::Id::JOB] = (stats.get_stat(MapleStat::Id::JOB) / 100 == 1) || (stats.get_stat(MapleStat::Id::JOB) / 100 >= 20) || stats.get_stat(MapleStat::Id::JOB) / 100 == 2;
			break;
		case 4: // Bowman
			okjobs.push_back(3);
			canequip[MapleStat::Id::JOB] = stats.get_stat(MapleStat::Id::JOB) / 100 == 3;
			break;
		case 8: // Thief
			okjobs.push_back(4);
			canequip[MapleStat::Id::JOB] = stats.get_stat(MapleStat::Id::JOB) / 100 == 4;
			break;
		case 16: // Pirate
			okjobs.push_back(5);
			canequip[MapleStat::Id::JOB] = stats.get_stat(MapleStat::Id::JOB) / 100 == 5;
			break;
		default:
			canequip[MapleStat::Id::JOB] = false;
			break;
		}

		prank = equip.get_potrank();

		switch (prank)
		{
		case Equip::Potential::POT_HIDDEN:
			potflag = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::RED);
			potflag.change_text(Locale::text("equip.pot.hidden"));
			break;
		case Equip::Potential::POT_RARE:
			potflag = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE);
			potflag.change_text(Locale::text("equip.pot.rare"));
			break;
		case Equip::Potential::POT_EPIC:
			potflag = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE);
			potflag.change_text(Locale::text("equip.pot.epic"));
			break;
		case Equip::Potential::POT_UNIQUE:
			potflag = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE);
			potflag.change_text(Locale::text("equip.pot.unique"));
			break;
		case Equip::Potential::POT_LEGENDARY:
			potflag = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE);
			potflag.change_text(Locale::text("equip.pot.legendary"));
			break;
		default:
			height -= 16;
			break;
		}

		Color::Name namecolor;

		switch (equip.get_quality())
		{
		case EquipQuality::Id::GREY:
			namecolor = Color::Name::LIGHTGREY;
			break;
		case EquipQuality::Id::ORANGE:
			namecolor = Color::Name::ORANGE;
			break;
		case EquipQuality::Id::BLUE:
			namecolor = Color::Name::MEDIUMBLUE;
			break;
		case EquipQuality::Id::VIOLET:
			namecolor = Color::Name::VIOLET;
			break;
		case EquipQuality::Id::GOLD:
			namecolor = Color::Name::YELLOW;
			break;
		default:
			namecolor = Color::Name::WHITE;
			break;
		}

		std::string namestr = itemdata.get_name();
		const int8_t reqGender = itemdata.get_gender();
		const bool female = stats.get_female();

		switch (reqGender)
		{
		case 0: // Male
			namestr += " " + Locale::text("equip.gender.male");
			break;
		case 1: // Female
			namestr += " " + Locale::text("equip.gender.female");
			break;
		case 2: // Unisex
		default:
			break;
		}

		if (equip.get_level() > 0)
		{
			namestr.append(" (+");
			namestr.append(std::to_string(equip.get_level()));
			namestr.append(")");
		}

		name = Text(Text::Font::A12B, Text::Alignment::LEFT, namecolor, namestr, 400);
		atkinc = Text(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::DUSTYGRAY, Locale::text("equip.label_atkinc"));

		std::string desctext = itemdata.get_desc();
		hasdesc = desctext.size() > 0;

		if (hasdesc)
		{
			desc = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, desctext, 250);
			height += desc.height() + 10;
		}

		category = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::format("equip.label_type", Locale::enum_text("equip.type.", equipdata.get_type())));

		is_weapon = equipdata.is_weapon();

		if (is_weapon)
		{
			const WeaponData& weapon = WeaponData::get(item_id);
			wepspeed = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::format("equip.label_wepspeed", Locale::enum_text("equip.speed.", weapon.getspeedstring())));
		}
		else
		{
			height -= 18;
		}

		hasslots = (equip.get_slots() > 0) || (equip.get_level() > 0);

		if (hasslots)
		{
			slots = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::format("equip.label_slots", std::to_string(equip.get_slots())));

			std::string vicious = std::to_string(equip.get_vicious());

			if (equip.get_vicious() > 1)
				vicious.append(" " + Locale::text("equip.label_max") + " ");

			hammers = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::format("equip.label_hammers", vicious));
		}
		else
		{
			height -= 36;
		}

		statlabels.clear();

		for (EquipStat::Id es = EquipStat::Id::STR; es <= EquipStat::Id::JUMP; es = static_cast<EquipStat::Id>(es + 1))
		{
			if (equip.get_stat(es) > 0)
			{
				int16_t delta = equip.get_stat(es) - equipdata.get_defstat(es);
				std::string statstr = std::to_string(equip.get_stat(es));

				if (delta != 0)
				{
					statstr.append(" (");
					statstr.append((delta < 0) ? "-" : "+");
					statstr.append(std::to_string(abs(delta)) + ")");
				}

				statlabels[es] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::format(Locale::enum_key("equip.stat.", EquipStat::names[es]), statstr));
			}
			else
			{
				height -= 18;
			}
		}
	}

	void EquipTooltip::draw(Point<int16_t> pos) const
	{
		if (invpos == 0)
			return;

		draw_preview(pos);

		int16_t max_width = Constants::Constants::get().get_viewwidth();
		int16_t max_height = Constants::Constants::get().get_viewheight();
		int16_t cur_width = pos.x() + FRAME_WIDTH;
		int16_t cur_height = pos.y() + 36;

		if (invpos_preview == 1)
			cur_width += FRAME_WIDTH;

		if (invpos_preview == 1)
			cur_height += (height > height_preview) ? height : height_preview;
		else
			cur_height += height;

		int16_t adj_x = cur_width - max_width;
		int16_t adj_y = cur_height - max_height;

		if (adj_x > 0)
			pos.shift_x(adj_x * -1);

		if (adj_y > 0)
			pos.shift_y(adj_y * -1);

		// Q1 定案：框体程序描边（半透明黑底 + 1px 灰边），替代缺失的 UIToolTip Frame 三段
		backdrop.draw(pos + Point<int16_t>(-6, -6), FRAME_WIDTH + 12, height + 32);

		name.draw(pos + Point<int16_t>(17, 7));

		if (prank != Equip::Potential::POT_NONE)
		{
			potflag.draw(pos + Point<int16_t>(130, 20));
			pos.shift_y(16);
		}

		pos.shift_y(44);

		separator.draw(pos);

		atkinc.draw(pos + Point<int16_t>(248, 4));
		itemicon.draw(DrawArgument(pos + Point<int16_t>(18, 82), 2.0f, 2.0f));

		pos.shift_y(12);

		for (MapleStat::Id ms : requirements)
		{
			Point<int16_t> reqpos = reqstatpositions[ms];
			bool reqok = canequip[ms];
			reqstattextures[ms][reqok].draw(pos + reqpos);

			if (ms != MapleStat::Id::LEVEL)
				reqset[reqok].draw(reqstatstrings[ms], 6, pos + reqpos + Point<int16_t>(54, 0));
			else
				lvset[reqok].draw(reqstatstrings[ms], 6, pos + reqpos + Point<int16_t>(54, 0));
		}

		pos.shift_y(88);

		Point<int16_t> job_position(pos + Point<int16_t>(10, 14));

		// 职业列表 = 横排一行（2026-11-04 正确端截图《背包中鼠标悬浮物品上显示的物品
		// 信息内容.png》复核：全职业装备的 6 个职业名沿一行依次排开、间距 2px，并非
		// 此前 SAD 对拍判定的同原点叠绘 —— 叠绘只在单职业装备上碰巧成立，全职业时
		// 糊成白块正是用户所报缺陷。旧证据 equip-native-compare.md §4 作废）。
		{
			int16_t job_x = 0;
			const auto& jobmap = jobs[canequip[MapleStat::Id::JOB]];
			for (auto& jbit : okjobs)
			{
				const Texture& jobtex = jobmap.at(jbit);
				jobtex.draw(job_position + Point<int16_t>(job_x, 0));
				job_x += static_cast<int16_t>(jobtex.get_dimensions().x() + 2);
			}
		}

		separator.draw(pos + Point<int16_t>(0, 47));

		pos.shift_y(49);

		int16_t stat_x = 13;
		int16_t stat_y = 15;

		category.draw(pos + Point<int16_t>(stat_x, 0));

		pos.shift_y(stat_y);

		if (is_weapon)
		{
			wepspeed.draw(pos + Point<int16_t>(stat_x, 0));
			pos.shift_y(stat_y);
		}

		for (const Text& label : statlabels.values())
		{
			if (label.empty())
				continue;

			label.draw(pos + Point<int16_t>(stat_x, 0));
			pos.shift_y(stat_y);
		}

		if (hasslots)
		{
			slots.draw(pos + Point<int16_t>(stat_x, 0));
			pos.shift_y(stat_y);
			hammers.draw(pos + Point<int16_t>(stat_x, 0));
			pos.shift_y(stat_y);
		}

		if (hasdesc)
		{
			pos.shift_y(13);
			separator.draw(pos);
			desc.draw(pos + Point<int16_t>(9, 3));
		}
	}

	void EquipTooltip::draw_preview(Point<int16_t> pos) const
	{
		if (invpos_preview == 0)
			return;

		pos.shift_x(FRAME_WIDTH);

		int16_t max_width = Constants::Constants::get().get_viewwidth();
		int16_t max_height = Constants::Constants::get().get_viewheight();
		int16_t cur_width = pos.x() + FRAME_WIDTH;
		int16_t cur_height = pos.y() + height_preview + 36;

		int16_t adj_x = cur_width - max_width;
		int16_t adj_y = cur_height - max_height;

		if (adj_x > 0)
			pos.shift_x(adj_x * -1);

		if (adj_y > 0)
			pos.shift_y(adj_y * -1);

		// Q1 定案：对比窗框体同为程序描边，间距 = FRAME_WIDTH
		backdrop.draw(pos + Point<int16_t>(-6, -6), FRAME_WIDTH + 12, height_preview + 32);

		name_preview.draw(pos + Point<int16_t>(17, 7));

		if (prank_preview != Equip::Potential::POT_NONE)
		{
			potflag_preview.draw(pos + Point<int16_t>(130, 20));
			pos.shift_y(16);
		}

		pos.shift_y(44);

		separator.draw(pos);

		atkinc_preview.draw(pos + Point<int16_t>(248, 4));
		itemicon_preview.draw(DrawArgument(pos + Point<int16_t>(18, 82), 2.0f, 2.0f));

		pos.shift_y(12);

		for (MapleStat::Id ms : requirements)
		{
			Point<int16_t> reqpos = reqstatpositions[ms];
			bool reqok = canequip_preview[ms];
			reqstattextures[ms][reqok].draw(pos + reqpos);

			if (ms != MapleStat::Id::LEVEL)
				reqset[reqok].draw(reqstatstrings_preview[ms], 6, pos + reqpos + Point<int16_t>(54, 0));
			else
				lvset[reqok].draw(reqstatstrings_preview[ms], 6, pos + reqpos + Point<int16_t>(54, 0));
		}

		pos.shift_y(88);

		Point<int16_t> job_position(pos + Point<int16_t>(10, 14));

		// 与主窗体同一处：原生横排一行（证据见上方注释）。
		{
			int16_t job_x = 0;
			const auto& jobmap = jobs[canequip_preview[MapleStat::Id::JOB]];
			for (auto& jbit : okjobs_preview)
			{
				const Texture& jobtex = jobmap.at(jbit);
				jobtex.draw(job_position + Point<int16_t>(job_x, 0));
				job_x += static_cast<int16_t>(jobtex.get_dimensions().x() + 2);
			}
		}

		separator.draw(pos + Point<int16_t>(0, 47));

		pos.shift_y(49);

		int16_t stat_x = 13;
		int16_t stat_y = 15;

		category_preview.draw(pos + Point<int16_t>(stat_x, 0));

		pos.shift_y(stat_y);

		if (is_weapon_preview)
		{
			wepspeed_preview.draw(pos + Point<int16_t>(stat_x, 0));
			pos.shift_y(stat_y);
		}

		for (EquipStat::Id es = EquipStat::Id::STR; es <= EquipStat::Id::JUMP; es = static_cast<EquipStat::Id>(es + 1))
		{
			const Text& label = statlabels_preview[es];

			if (label.empty())
				continue;

			label.draw(pos + Point<int16_t>(stat_x, 0));

			// 差值紧跟属性文本，颜色在构造时已确定（正绿负红）
			const Text& diff = statdeltas_preview[es];

			if (!diff.empty())
				diff.draw(pos + Point<int16_t>(stat_x + static_cast<int16_t>(label.width()), 0));

			pos.shift_y(stat_y);
		}

		if (hasslots_preview)
		{
			slots_preview.draw(pos + Point<int16_t>(stat_x, 0));
			pos.shift_y(stat_y);
			hammers_preview.draw(pos + Point<int16_t>(stat_x, 0));
			pos.shift_y(stat_y);
		}

		if (hasdesc_preview)
		{
			pos.shift_y(-4);
			separator.draw(pos);
			desc_preview.draw(pos + Point<int16_t>(9, 8));
		}
	}
}