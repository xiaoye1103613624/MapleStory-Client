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
	// 正文行水平锚点（FRAME_WIDTH = 264 为类私有常量，这里按 132 记一半）。
	namespace
	{
		// 附加属性行的冒号中轴。
		constexpr int16_t BODY_CENTER_X = 132;
		// 元信息行（道具ID/装备分类/攻击速度/…）的正文左缘。
		constexpr int16_t BODY_LEFT_X = 9;
	}

	// 正文行「标签：值」的拆分与摆放（2026-11-11 用户实拍反馈）。
	//
	// 原生 tip 的两类正文行：
	//   元信息行  道具ID: 1060002 / 装备分类: 下衣 / 攻击速度: 普通 / 可升级次数: 7
	//             → 标签贴正文左缘，冒号紧跟标签；值紧接冒号。
	//   附加属性行 力量: 3 / 敏捷: 0 ...
	//             → 冒号固定在正文中线，标签右对齐到冒号、值自冒号起左对齐，
	//               多行叠起来冒号成一条竖直的中轴线。
	//
	// 旧实现把整串（含冒号）用 Text::Alignment::CENTER 画在中线上，于是
	// 「装备分类: 下衣」的冒号随标签长度左右漂移，用户看到的就是「冒号不齐」。
	namespace
	{
		// 全角「：」的 UTF-8 三字节序列。
		constexpr const char* FULLWIDTH_COLON = "\xEF\xBC\x9A";

		// 拆出整串里最后一个冒号：中文 locale 用全角「：」，英文 locale 用半角 ':'。
		// 只认最后一个，避免本地化文本里出现别的冒号（例如「上限」拼接）。
		size_t last_colon(const std::string& text)
		{
			size_t pos = text.rfind(FULLWIDTH_COLON);

			if (pos != std::string::npos)
				return pos;

			return text.rfind(':');
		}

		size_t colon_width(const std::string& text, size_t pos)
		{
			return (pos != std::string::npos && text.compare(pos, 3, FULLWIDTH_COLON) == 0) ? 3 : 1;
		}
	}

	void EquipTooltip::BodyLine::build(const std::string& full, Color::Name valuecolor, bool center_colon)
	{
		label = Text();
		colon = Text();
		value = Text();
		centered = center_colon;

		const size_t pos = last_colon(full);

		// 冒号居中行：名/冒号/值同色（inc 行传 WHITE → 全白；元信息传 TOOLTIPBODY）。
		// 非居中行：名/冒号保持奶油色，仅值用 valuecolor（历史路径，现分隔线下已少用）。
		const Color::Name namecolor = center_colon ? valuecolor : Color::Name::TOOLTIPBODY;

		if (pos == std::string::npos)
		{
			// 无冒号：居中行整串按中线摆（draw 用 CENTER 宽估）；非居中仍左对齐。
			if (center_colon)
				value = Text(Text::Font::A11M, Text::Alignment::CENTER, valuecolor, full);
			else
				value = Text(Text::Font::A11M, Text::Alignment::LEFT, valuecolor, full);
			return;
		}

		const size_t cw = colon_width(full, pos);

		label = Text(Text::Font::A11M, Text::Alignment::LEFT, namecolor, full.substr(0, pos));
		colon = Text(Text::Font::A11M, Text::Alignment::LEFT, namecolor, full.substr(pos, cw));
		value = Text(Text::Font::A11M, Text::Alignment::LEFT, valuecolor, full.substr(pos + cw));
	}

	int16_t EquipTooltip::BodyLine::draw(Point<int16_t> pos) const
	{
		if (empty())
			return 0;

		const int16_t lw = label.width();
		const int16_t cw = colon.width();
		const int16_t vw = value.width();

		// 无冒号的居中行：整串以 tip 中线为轴（CENTER 文本原点即中心）。
		if (centered && label.empty() && colon.empty() && !value.empty())
		{
			value.draw(pos + Point<int16_t>(BODY_CENTER_X, 0));
			return static_cast<int16_t>(BODY_CENTER_X + vw / 2);
		}

		int16_t lx = 0;

		if (centered && !label.empty())
			lx = static_cast<int16_t>(BODY_CENTER_X - cw / 2 - lw);
		else
			lx = BODY_LEFT_X;

		int16_t cx = static_cast<int16_t>(lx + lw);

		if (!label.empty())
			label.draw(pos + Point<int16_t>(lx, 0));

		if (!colon.empty())
			colon.draw(pos + Point<int16_t>(cx, 0));

		const int16_t vx = static_cast<int16_t>(cx + cw);

		if (!value.empty())
			value.draw(pos + Point<int16_t>(vx, 0));

		return static_cast<int16_t>(vx + vw);
	}
	// 2026-11-08 原生装备 tooltip 布局常量。
	// 依据：evidence/r28-tooltip-native/req_6x.png（单列 8 行、12px 行距）、
	// UIWindow.img_runtime.json 的 ToolTip/Equip/Can/reqXXX 47x11 与
	// GrowthEnabled/{itemLEV,itemEXP,percent} 47x11 / 5x7 位图。
	namespace
	{
		constexpr int16_t REQ_LABEL_X = 97;      // 需求块标签位图 x（相对需求块起点）
		constexpr int16_t REQ_VALUE_DX = 50;     // 数字紧跟标签，47px 位图 + 3px 间隙
		// 5x7 字模相对 47x11 标签下沉 2px，视觉与标签中线齐（2026-11-11 用户实拍）。
		constexpr int16_t REQ_VALUE_DY = 2;
		constexpr int16_t GROWTH_LEV_Y = 119;    // 第 7 行「道具等级」
		constexpr int16_t GROWTH_EXP_Y = 131;    // 第 8 行「道具经验」
		// 需求块推进：旧两列布局 88；单列 6 行 / 8 行分别 124 / 148。
		constexpr int16_t REQ_SHIFT_LEGACY = 88;
		constexpr int16_t REQ_SHIFT_BASE = 124;
		constexpr int16_t REQ_SHIFT_GROWTH = 148;
		// 框体基准高度：旧 540 对应推进 88；新增道具 ID 行占 15px。
		constexpr int16_t HEIGHT_BASE = 540 + 15;
	}

	// 道具图标底板（2026-11-11 用户实拍）：参考帧里图标背后是一块 32x32 的灰底，
	// 图标叠在灰底之上、两者同心。部署包的 UIWindow.img/IconBase/0 是「白底黑框 +
	// 蓝角」的另一套图，2x 垫在图标下就变成截图里那块掉在下方的白框，因此改为
	// 程序自绘灰底（不依赖位图节点，也不受 itemicon 自身 origin 影响）。
	namespace
	{
		constexpr int16_t ICON_BOX_SIZE = 32;					// 原生 32x32
		constexpr float ICON_BOX_SCALE = 2.0f;					// 与 itemicon 同 2x
		constexpr int16_t ICON_BOX_X = 16;						// 相对分隔线（与原 iconbase 同位）
		constexpr int16_t ICON_BOX_Y = 80;
		constexpr int16_t ICON_OFF_X = 2;						// 图标相对底板右下偏移
		constexpr int16_t ICON_OFF_Y = 2;
		// 参考帧逐像素取样：底板 (74,74,94) 一档灰紫，压在半透背板上。
		constexpr GLfloat ICON_BOX_R = 74.0f / 255.0f;
		constexpr GLfloat ICON_BOX_G = 74.0f / 255.0f;
		constexpr GLfloat ICON_BOX_B = 94.0f / 255.0f;
	}

	EquipTooltip::EquipTooltip()
	{
		nl::node Equip = nl::nx::UI["UIWindow.img"]["ToolTip"]["Equip"];
		nl::node EquipCan = Equip["Can"];
		nl::node EquipCannot = Equip["Cannot"];

		// 原生职业行下方分隔线为 1px 纯白（参考帧 y237 全宽 255,255,255），旧 α0.4 偏灰。
		separator = ColorLine(FRAME_WIDTH, Color::Name::WHITE, 1.0f, false);

		// 道具图标下的灰底板：程序自绘（2026-11-11 用户实拍），不再取
		// UIWindow.img/IconBase/0——部署包该节点是「白底黑框 + 蓝角」，
		// 垫在图标背后就是截图里那块错位的白框。
		iconbase_box = ColorBox(ICON_BOX_SIZE, ICON_BOX_SIZE,
			Color(ICON_BOX_R, ICON_BOX_G, ICON_BOX_B, 1.0f), 1.0f);

		// 现金/金币徽标（前置块，贴图标旁）；节点缺失则跳过。
		if (Equip["cash"])
			cash_badge = Equip["cash"];
		if (Equip["mesos"])
			mesos_badge = Equip["mesos"];

		// 成长两行资源：启用态标签/字模/百分号，禁用态字模备用。
		nl::node growth_on = Equip["GrowthEnabled"];
		nl::node growth_off = Equip["GrowthDisabled"];

		growth_lev_label = Texture(growth_on["itemLEV"]);
		growth_exp_label = Texture(growth_on["itemEXP"]);
		growth_percent = Texture(growth_on["percent"]);
		fame_none = Texture(EquipCan["none"]);
		growthset[true] = Charset(growth_on, Charset::Alignment::LEFT);
		growthset[false] = Charset(growth_off, Charset::Alignment::LEFT);

		show_growth = false;
		show_growth_preview = false;
		req_shift = REQ_SHIFT_BASE;
		req_shift_preview = REQ_SHIFT_BASE;

		requirements.push_back(MapleStat::Id::LEVEL);
		requirements.push_back(MapleStat::Id::STR);
		requirements.push_back(MapleStat::Id::DEX);
		requirements.push_back(MapleStat::Id::INT);
		requirements.push_back(MapleStat::Id::LUK);
		// 原生需求块含「要求人气」行，位图 reqPOP 已在本函数映射。
		requirements.push_back(MapleStat::Id::FAME);

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

		// 2026-11-08 原生复校：需求块是单列 8 行、行距 12px（req_6x.png 逐像素取样），
		// 旧两列 15px 布局与原生不符且缺「要求人气/道具等级/道具经验」三行。
		// 47x11 标签位图 + 数字：行距 12px 才不会像 9px 那样叠成糊块。
		reqstatpositions[MapleStat::Id::LEVEL] = Point<int16_t>(REQ_LABEL_X, 47);
		reqstatpositions[MapleStat::Id::STR] = Point<int16_t>(REQ_LABEL_X, 59);
		reqstatpositions[MapleStat::Id::DEX] = Point<int16_t>(REQ_LABEL_X, 71);
		reqstatpositions[MapleStat::Id::INT] = Point<int16_t>(REQ_LABEL_X, 83);
		reqstatpositions[MapleStat::Id::LUK] = Point<int16_t>(REQ_LABEL_X, 95);
		reqstatpositions[MapleStat::Id::FAME] = Point<int16_t>(REQ_LABEL_X, 107);

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
		show_cash = false;
		show_cash_preview = false;
		has_trade = false;
		has_trade_preview = false;
	}

	namespace
	{
		// ItemTooltip 同源：unique/untradable → 交易限制文案（i18n）。
		std::string equip_trade_label(const ItemData& itemdata)
		{
			const bool u = itemdata.is_unique();
			const bool t = itemdata.is_untradable();
			if (u && t)
				return Locale::text("equip.trade.both");
			if (u)
				return Locale::text("equip.trade.unique");
			if (t)
				return Locale::text("equip.trade.untradable");
			return {};
		}
	}

	void EquipTooltip::set_equip(Tooltip::Parent parent, int16_t ivp)
	{
		// 早退只跳过「同一槽位重复悬停」的重建，但 ⑤ 套装件数依赖**已穿戴集合**，
		// 换装备后同一槽位的 itemid 会变而 invp 不变。因此把「已穿戴快照哈希」
		// 纳入判定：集合变了就必须重算，否则套装面板会停在旧件数上。
		const std::vector<int32_t> equipped_snapshot = Stage::get().get_player().get_inventory().equipped_item_ids();

		size_t snapshot_hash = 1469598103934665603ull;

		for (int32_t id : equipped_snapshot)
		{
			snapshot_hash ^= static_cast<size_t>(id);
			snapshot_hash *= 1099511628211ull;
		}

		if (invpos == ivp && snapshot_hash == equip_snapshot_hash)
			return;

		equip_snapshot_hash = snapshot_hash;

		invpos = ivp;
		invpos_preview = 0;

		// 每次重新悬停都重置成长行状态，避免沿用上一件装备的数据。
		show_growth = false;
		show_growth_preview = false;
		show_cash = false;
		show_cash_preview = false;
		has_trade = false;
		has_trade_preview = false;
		req_shift = REQ_SHIFT_BASE;
		req_shift_preview = REQ_SHIFT_BASE;

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

					const uint8_t preview_itemlevel = equip.get_itemlevel();
					const int16_t preview_itemexp = equip.get_itemexp();

					// 用户抉择：成长两行仅当 itemlevel/itemexp 任一非零时显示。
					show_growth_preview = (preview_itemlevel > 0) || (preview_itemexp > 0);
					req_shift_preview = show_growth_preview ? REQ_SHIFT_GROWTH : REQ_SHIFT_BASE;
					growth_level_str_preview = std::to_string(preview_itemlevel);
					growth_exp_str_preview = std::to_string(preview_itemexp);
					height_preview = HEIGHT_BASE + (req_shift_preview - REQ_SHIFT_LEGACY);

					itemicon_preview = itemdata.get_icon(false);

					for (auto& ms : requirements)
					{
						canequip_preview[ms] = stats.get_stat(ms) >= equipdata.get_reqstat(ms);
						// 需求值按原值输出：0 就是 "0"。旧的 3 位左补零（及
						// reqstr.size() < 3 判空）是 v095 遗风，用户 2026-11-11
						// 明确要求「属性为0就显示0 不要显示000」，且
						// 3 - reqstr.size() 在 size>3 时会无符号下溢。
						reqstatstrings_preview[ms] = std::to_string(equipdata.get_reqstat(ms));
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
					// 2026-11-11 用户实拍：右上「攻击力增加」整行移除（历史误接的 equip.label_atkinc）。

					std::string desctext = itemdata.get_desc();
					hasdesc_preview = desctext.size() > 0;

					if (hasdesc_preview)
					{
						// 分隔线以下描述：水平居中（非图标旁前置块）。
						desc_preview = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::TOOLTIPBODY, desctext, 250);
						height_preview += desc_preview.height() + 10;
					}

					// 分隔线以下：冒号中轴居中（center_colon=true）；元信息奶油色。
					itemid_preview.build(Locale::format("equip.label_itemid", std::to_string(item_id)), Color::Name::TOOLTIPBODY, true);
					category_preview.build(Locale::format("equip.label_type", Locale::enum_text("equip.type.", equipdata.get_type())), Color::Name::TOOLTIPBODY, true);

					show_cash_preview = itemdata.is_cashitem() && cash_badge.is_valid();

					{
						const std::string tlab = equip_trade_label(itemdata);
						has_trade_preview = !tlab.empty();
						if (has_trade_preview)
						{
							trade_line_preview.build(tlab, Color::Name::ORANGE, true);
							height_preview += 18;
						}
					}

					is_weapon_preview = equipdata.is_weapon();

					if (is_weapon_preview)
					{
						const WeaponData& weapon = WeaponData::get(item_id);
						wepspeed_preview.build(Locale::format("equip.label_wepspeed", Locale::enum_text("equip.speed.", weapon.getspeedstring())), Color::Name::TOOLTIPBODY, true);
					}
					else
					{
						height_preview -= 18;
					}

					hasslots_preview = (equip.get_slots() > 0) || (equip.get_level() > 0);

					if (hasslots_preview)
					{
						slots_preview.build(Locale::format("equip.label_slots", std::to_string(equip.get_slots())), Color::Name::TOOLTIPBODY, true);

						std::string vicious = std::to_string(equip.get_vicious());

						if (equip.get_vicious() > 1)
							vicious.append(" " + Locale::text("equip.label_max") + " ");

						hammers_preview.build(Locale::format("equip.label_hammers", vicious), Color::Name::TOOLTIPBODY, true);
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

						statlabels_preview[es].build(Locale::format(Locale::enum_key("equip.stat.", EquipStat::names[es]), statstr), Color::Name::WHITE, true);

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

		// ⑤ 套装 companion 面板：与 Player::recalc_stats 的注入走同一份
		// Data/SetItemData + 同一套「已穿戴 itemid」口径，保证面板显示的
		// 档位与实际生效属性永远一致（不会各说各话）。
		// 不属于任何套装时 settip.valid() 为 false，draw 自动跳过。
		settip.set_item(item_id, equipped_snapshot);

		const EquipData& equipdata = EquipData::get(item_id);
		const ItemData& itemdata = equipdata.get_itemdata();

		const uint8_t itemlevel = equip.get_itemlevel();
		const int16_t itemexp = equip.get_itemexp();

		show_growth = (itemlevel > 0) || (itemexp > 0);
		req_shift = show_growth ? REQ_SHIFT_GROWTH : REQ_SHIFT_BASE;
		growth_level_str = std::to_string(itemlevel);
		growth_exp_str = std::to_string(itemexp);
		// height 与 draw 的需求块推进量共用同一增量，避免内容越出背板。
		height = HEIGHT_BASE + (req_shift - REQ_SHIFT_LEGACY);

		itemicon = itemdata.get_icon(false);

		for (auto& ms : requirements)
		{
			canequip[ms] = stats.get_stat(ms) >= equipdata.get_reqstat(ms);
			// 同 preview 分支：原值输出，不补零（用户 2026-11-11 第 5 条）。
			reqstatstrings[ms] = std::to_string(equipdata.get_reqstat(ms));
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
		// 2026-11-11 用户实拍：右上「攻击力增加」整行移除（历史误接的 equip.label_atkinc）。

		std::string desctext = itemdata.get_desc();
		hasdesc = desctext.size() > 0;

		if (hasdesc)
		{
			desc = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::TOOLTIPBODY, desctext, 250);
			height += desc.height() + 10;
		}

		// 用户裁决：分隔线以下全部冒号中轴居中；inc 行全白，元信息奶油色。
		itemid.build(Locale::format("equip.label_itemid", std::to_string(item_id)), Color::Name::TOOLTIPBODY, true);
		category.build(Locale::format("equip.label_type", Locale::enum_text("equip.type.", equipdata.get_type())), Color::Name::TOOLTIPBODY, true);

		show_cash = itemdata.is_cashitem() && cash_badge.is_valid();

		{
			const std::string tlab = equip_trade_label(itemdata);
			has_trade = !tlab.empty();
			if (has_trade)
			{
				trade_line.build(tlab, Color::Name::ORANGE, true);
				height += 18;
			}
		}

		is_weapon = equipdata.is_weapon();

		if (is_weapon)
		{
			const WeaponData& weapon = WeaponData::get(item_id);
			wepspeed.build(Locale::format("equip.label_wepspeed", Locale::enum_text("equip.speed.", weapon.getspeedstring())), Color::Name::TOOLTIPBODY, true);
		}
		else
		{
			height -= 18;
		}

		hasslots = (equip.get_slots() > 0) || (equip.get_level() > 0);

		if (hasslots)
		{
			slots.build(Locale::format("equip.label_slots", std::to_string(equip.get_slots())), Color::Name::TOOLTIPBODY, true);

			std::string vicious = std::to_string(equip.get_vicious());

			if (equip.get_vicious() > 1)
				vicious.append(" " + Locale::text("equip.label_max") + " ");

			hammers.build(Locale::format("equip.label_hammers", vicious), Color::Name::TOOLTIPBODY, true);
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

				statlabels[es].build(Locale::format(Locale::enum_key("equip.stat.", EquipStat::names[es]), statstr), Color::Name::WHITE, true);
			}
			else
			{
				height -= 18;
			}
		}

		// ⑦ 095 潜能行（2026-11-12 Round 8）：把 EquipTooltip.h 里存在已久但
		// 从未被赋值的 potlines[5] 真正接上。
		// 数据：Equip::get_potential(0..2) = 3 条主潜能，get_bonus_potential(0..1)
		// = 2 条附加潜能；值语义 = 潜能条目 id（0 = 空）。
		// 文案：live 包内**没有** Potential.img / PotentialOption.img（实测 Etc/
		// 目录无该节点），因此走本地词条表 locale 的 equip.potline.* ，
		// 键 = 潜能 id，缺键时显示「潜能 N」而不是留空——宁可显示编号也不静默。
		{
			int16_t pot_height = 0;

			for (size_t i = 0; i < 5; ++i)
			{
				haspotlines[i] = false;

				int16_t pot_id = (i < 3) ? equip.get_potential(i) : equip.get_bonus_potential(i - 3);

				if (pot_id <= 0)
					continue;

				// 交付口径：主潜能与附加潜能用不同前缀区分（附加潜能行加「(B)」）。
				// Locale::text_or = has ? tr(key) : fallback（Util/Locale.h:55-57），
				// 正是「缺键显示编号、不留空」的语义。
				const std::string key = "equip.potline." + std::to_string(pot_id);
				std::string content = Locale::text_or(key, "潜能 " + std::to_string(pot_id));

				if (i >= 3)
					content = "(B) " + content;

				potlines[i] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::TOOLTIPBODY,
					content, FRAME_WIDTH - 20);
				haspotlines[i] = true;
				pot_height += 15;
			}

			if (pot_height > 0)
				height += pot_height + 6;
		}

		// ⑦ 095 强化三件套（Round 15）：火花 / 宝珠 / 星之力。
		// 与潜能同一套口径：值 0 = 无 → 不出行；文案走 locale 词条表
		// （live 包内无 AdditionalOption.img / SoulOption.img，实测 Etc/ 无此节点），
		// 缺键用 Locale::text_or 回退成带编号的占位，绝不静默留空。
		{
			int16_t extra_height = 0;

			has_spark_line = false;
			has_soul_line = false;
			has_star_line = false;

			if (int16_t sid = equip.get_spark(); sid > 0)
			{
				const std::string content = Locale::text_or(
					"equip.spark." + std::to_string(sid),
					"火花 " + std::to_string(sid));

				spark_line = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::TOOLTIPBODY,
					content, FRAME_WIDTH - 20);
				has_spark_line = true;
				extra_height += 15;
			}

			if (int16_t slid = equip.get_soul(); slid > 0)
			{
				const std::string content = Locale::text_or(
					"equip.soul." + std::to_string(slid),
					"宝珠 " + std::to_string(slid));

				soul_line = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::TOOLTIPBODY,
					content, FRAME_WIDTH - 20);
				has_soul_line = true;
				extra_height += 15;
			}

			if (uint8_t sf = equip.get_starforce(); sf > 0)
			{
				// 星之力是「星数」不是词条 id，直接按原生「★ × N」显示。
				std::string stars;

				for (uint8_t i = 0; i < sf && i < 25; ++i)
					stars += "*";

				star_line = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW,
					"星之力 " + std::to_string(sf) + " " + stars, FRAME_WIDTH - 20);
				has_star_line = true;
				extra_height += 15;
			}

			if (extra_height > 0)
				height += extra_height + 6;
		}
	}

	void EquipTooltip::draw_item_icon(Point<int16_t> pos, const Texture& icon) const
	{
		// 底板锚点：pos 之后 (16,80)，32x32 再 2x。
		const Point<int16_t> boxpos = pos + Point<int16_t>(ICON_BOX_X, ICON_BOX_Y);
		iconbase_box.draw(boxpos);

		if (icon.is_valid())
		{
			// 关键：Texture::draw 的 pos 是「图标 origin 对应的那个点」
			// （DrawArgument::get_rectangle 里 rlt = pos - center - origin），
			// 且 2x 缩放会把 origin 的影响同样放大两倍。所以要让图标左上角落在
			// 底板上 (2,2) 处，必须把 origin*2 加回去，否则图标会按自身 origin
			// （道具图标普遍在中心偏下）向上/左跑，看起来就是「白框掉在图标下方」。
			const Point<int16_t> ivorg = icon.get_origin();
			const Point<int16_t> iconpos(
				boxpos.x() + ICON_OFF_X + static_cast<int16_t>(ivorg.x() * ICON_BOX_SCALE),
				boxpos.y() + ICON_OFF_Y + static_cast<int16_t>(ivorg.y() * ICON_BOX_SCALE)
			);

			icon.draw(DrawArgument(iconpos, ICON_BOX_SCALE, ICON_BOX_SCALE));
		}
	}

	void EquipTooltip::draw_cash_badge(Point<int16_t> pos, bool cash) const
	{
		// 前置块：现金徽标贴在图标底板右上外侧；非 cash 且有 mesos 节点时画金币标。
		if (!cash && !mesos_badge.is_valid())
			return;
		const Texture& badge = cash ? cash_badge : mesos_badge;
		if (!badge.is_valid())
			return;
		const Point<int16_t> boxpos = pos + Point<int16_t>(ICON_BOX_X, ICON_BOX_Y);
		badge.draw(boxpos + Point<int16_t>(
			static_cast<int16_t>(ICON_BOX_SIZE * ICON_BOX_SCALE) - 2,
			-2));
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

		// ⑤ 套装面板占第三段宽度（[悬停][比较][套装]），参与视口右边界夹取，
		// 否则贴屏幕右侧悬停时套装面板会被裁掉。
		if (settip.valid())
			cur_width += SetTooltip::FRAME_WIDTH;

		if (invpos_preview == 1)
			cur_height += (height > height_preview) ? height : height_preview;
		else
			cur_height += height;

		if (settip.valid() && settip.height() > cur_height - (pos.y() + 36))
			cur_height = pos.y() + 36 + settip.height();

		int16_t adj_x = cur_width - max_width;
		int16_t adj_y = cur_height - max_height;

		if (adj_x > 0)
			pos.shift_x(adj_x * -1);

		if (adj_y > 0)
			pos.shift_y(adj_y * -1);

		backdrop.draw(pos + Point<int16_t>(-6, -6), FRAME_WIDTH + 12, height + 32);

		name.draw(pos + Point<int16_t>(17, 7));

		if (prank != Equip::Potential::POT_NONE)
		{
			potflag.draw(pos + Point<int16_t>(130, 20));
			pos.shift_y(16);
		}

		pos.shift_y(44);

		separator.draw(pos);

		draw_item_icon(pos, itemicon);
		draw_cash_badge(pos, show_cash);

		pos.shift_y(12);

		// 需求块：单列 12px，6 条固定需求 + 可选成长 2 行（图标右侧前置区，Can/Cannot 色保留）。
		for (MapleStat::Id ms : requirements)
		{
			Point<int16_t> reqpos = reqstatpositions[ms];
			bool reqok = canequip[ms];
			reqstattextures[ms][reqok].draw(pos + reqpos);

			Point<int16_t> valuepos = pos + reqpos + Point<int16_t>(REQ_VALUE_DX, REQ_VALUE_DY);

			if (ms == MapleStat::Id::FAME)
			{
				// 原生「要求人气」值恒为短横（Can/none 5x1），不画 000。
				fame_none.draw(valuepos);
			}
			else if (ms == MapleStat::Id::LEVEL)
			{
				lvset[reqok].draw(reqstatstrings[ms], 6, valuepos);
			}
			else
			{
				reqset[reqok].draw(reqstatstrings[ms], 6, valuepos);
			}
		}

		if (show_growth)
		{
			Point<int16_t> levpos = pos + Point<int16_t>(REQ_LABEL_X, GROWTH_LEV_Y);
			Point<int16_t> exppos = pos + Point<int16_t>(REQ_LABEL_X, GROWTH_EXP_Y);

			growth_lev_label.draw(levpos);
			growth_exp_label.draw(exppos);

			growthset[true].draw(growth_level_str, 6, levpos + Point<int16_t>(REQ_VALUE_DX, REQ_VALUE_DY));

			int16_t expw = growthset[true].draw(growth_exp_str, 6, exppos + Point<int16_t>(REQ_VALUE_DX, REQ_VALUE_DY));
			growth_percent.draw(exppos + Point<int16_t>(REQ_VALUE_DX + expw + 2, REQ_VALUE_DY));
		}

		pos.shift_y(req_shift);

		Point<int16_t> job_position(pos + Point<int16_t>(10, 14));

		// 职业列表 = 横排一行。步长固定 JOB_STEP（41），避免「新手」40px 与
		// 「战士」38px 交替时肉眼看出间距不一致（2026-11-11 用户实拍）。
		{
			int16_t job_x = 0;
			const auto& jobmap = jobs[canequip[MapleStat::Id::JOB]];

			for (auto& jbit : okjobs)
			{
				jobmap.at(jbit).draw(job_position + Point<int16_t>(job_x, 0));
				job_x += JOB_STEP;
			}
		}

		separator.draw(pos + Point<int16_t>(0, 47));

		pos.shift_y(49);

		// 分隔线以下（用户 2026-11-12）：整区水平居中；inc 行冒号中轴+全白；
		// 元信息冒号中轴+奶油色；交易限制橙字居中。前置需求/职业块在上方不改。
		int16_t stat_y = 15;

		itemid.draw(pos);
		pos.shift_y(stat_y);

		category.draw(pos);
		pos.shift_y(stat_y);

		if (has_trade)
		{
			trade_line.draw(pos);
			pos.shift_y(stat_y);
		}

		if (is_weapon)
		{
			wepspeed.draw(pos);
			pos.shift_y(stat_y);
		}

		for (const BodyLine& line : statlabels.values())
		{
			if (line.empty())
				continue;

			line.draw(pos);
			pos.shift_y(stat_y);
		}

		if (hasslots)
		{
			slots.draw(pos);
			pos.shift_y(stat_y);
			hammers.draw(pos);
			pos.shift_y(stat_y);
		}

		// ⑦ 095 潜能行：画在「可升级次数/金锤子」之后、「描述」之前，
		// 顺序与原生 tip 的「附加属性块」位置一致。
		{
			bool any = false;

			for (bool h : haspotlines)
				any = any || h;

			if (any)
			{
				pos.shift_y(6);

				for (size_t i = 0; i < 5; ++i)
				{
					if (!haspotlines[i])
						continue;

					potlines[i].draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
					pos.shift_y(15);
				}
			}
		}

		// ⑦ 095 强化三件套（Round 15）：紧接潜能行之后，缺项跳过。
		if (has_spark_line || has_soul_line || has_star_line)
		{
			pos.shift_y(6);

			if (has_spark_line)
			{
				spark_line.draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
				pos.shift_y(15);
			}

			if (has_soul_line)
			{
				soul_line.draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
				pos.shift_y(15);
			}

			if (has_star_line)
			{
				star_line.draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
				pos.shift_y(15);
			}
		}

		if (hasdesc)
		{
			pos.shift_y(13);
			separator.draw(pos);
			desc.draw(pos + Point<int16_t>(BODY_CENTER_X, 3));
		}

		// ⑤ 套装 companion 面板：画在已夹取过的 pos 右侧。
		// 布局 = [悬停装备 tip][套装 tip][已穿比较 tip]（参考 README §UI），
		// 这里插在装备 tip 与比较 tip 之间需要重排两段，代价高且会动既有
		// 对比逻辑；本端采用「挂在最右」——信息完整、零回归，
		// 且 compare tip 的 draw_preview 已在前面按 FRAME_WIDTH 定位，
		// 两者互不覆盖（中间隔一帧宽度）。
		if (settip.valid())
		{
			// 顶端对齐：pos 在绘制末尾已被需求块下推，故用「tip 起点 y」对齐，
			// 起点 = 当前 pos.y - 已下推量（height 是需求块之后的总高）。
			int16_t tip_top = pos.y() - height;
			Point<int16_t> setpos(pos.x() + FRAME_WIDTH, tip_top);

			// 右侧溢出时翻到左侧（与原生「放不下就翻边」同策略）。
			if (setpos.x() + SetTooltip::FRAME_WIDTH > max_width)
				setpos = Point<int16_t>(pos.x() - SetTooltip::FRAME_WIDTH - 12, tip_top);

			settip.draw(setpos);
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

		backdrop.draw(pos + Point<int16_t>(-6, -6), FRAME_WIDTH + 12, height_preview + 32);

		name_preview.draw(pos + Point<int16_t>(17, 7));

		if (prank_preview != Equip::Potential::POT_NONE)
		{
			potflag_preview.draw(pos + Point<int16_t>(130, 20));
			pos.shift_y(16);
		}

		pos.shift_y(44);

		separator.draw(pos);

		// 2026-11-11：攻击力增加行（equip.label_atkinc）属历史误接，原生 tip 无此行，
		// 已整段移除。图标与主窗体共用 draw_item_icon（灰底 + origin*2 补偿）。
		draw_item_icon(pos, itemicon_preview);
		draw_cash_badge(pos, show_cash_preview);

		pos.shift_y(12);

		for (MapleStat::Id ms : requirements)
		{
			Point<int16_t> reqpos = reqstatpositions[ms];
			bool reqok = canequip_preview[ms];
			reqstattextures[ms][reqok].draw(pos + reqpos);

			Point<int16_t> valuepos = pos + reqpos + Point<int16_t>(REQ_VALUE_DX, REQ_VALUE_DY);

			if (ms == MapleStat::Id::FAME)
			{
				fame_none.draw(valuepos);
			}
			else if (ms == MapleStat::Id::LEVEL)
			{
				lvset[reqok].draw(reqstatstrings_preview[ms], 6, valuepos);
			}
			else
			{
				reqset[reqok].draw(reqstatstrings_preview[ms], 6, valuepos);
			}
		}

		if (show_growth_preview)
		{
			Point<int16_t> levpos = pos + Point<int16_t>(REQ_LABEL_X, GROWTH_LEV_Y);
			Point<int16_t> exppos = pos + Point<int16_t>(REQ_LABEL_X, GROWTH_EXP_Y);

			growth_lev_label.draw(levpos);
			growth_exp_label.draw(exppos);

			growthset[true].draw(growth_level_str_preview, 6, levpos + Point<int16_t>(REQ_VALUE_DX, REQ_VALUE_DY));

			int16_t expw = growthset[true].draw(growth_exp_str_preview, 6, exppos + Point<int16_t>(REQ_VALUE_DX, REQ_VALUE_DY));
			growth_percent.draw(exppos + Point<int16_t>(REQ_VALUE_DX + expw + 2, REQ_VALUE_DY));
		}

		pos.shift_y(req_shift_preview);

		Point<int16_t> job_position(pos + Point<int16_t>(10, 14));

		// 与主窗体同一处：固定 JOB_STEP 横排。
		{
			int16_t job_x = 0;
			const auto& jobmap = jobs[canequip_preview[MapleStat::Id::JOB]];

			for (auto& jbit : okjobs_preview)
			{
				jobmap.at(jbit).draw(job_position + Point<int16_t>(job_x, 0));
				job_x += JOB_STEP;
			}
		}

		separator.draw(pos + Point<int16_t>(0, 47));

		pos.shift_y(49);

		int16_t stat_y = 15;

		itemid_preview.draw(pos);
		pos.shift_y(stat_y);

		category_preview.draw(pos);
		pos.shift_y(stat_y);

		if (has_trade_preview)
		{
			trade_line_preview.draw(pos);
			pos.shift_y(stat_y);
		}

		if (is_weapon_preview)
		{
			wepspeed_preview.draw(pos);
			pos.shift_y(stat_y);
		}

		for (EquipStat::Id es = EquipStat::Id::STR; es <= EquipStat::Id::JUMP; es = static_cast<EquipStat::Id>(es + 1))
		{
			const BodyLine& line = statlabels_preview[es];

			if (line.empty())
				continue;

			int16_t right = line.draw(pos);

			// 差值紧跟居中后的属性文本右缘。
			const Text& diff = statdeltas_preview[es];

			if (!diff.empty())
				diff.draw(Point<int16_t>(right, pos.y()));

			pos.shift_y(stat_y);
		}

		if (hasslots_preview)
		{
			slots_preview.draw(pos);
			pos.shift_y(stat_y);
			hammers_preview.draw(pos);
			pos.shift_y(stat_y);
		}

		if (hasdesc_preview)
		{
			pos.shift_y(-4);
			separator.draw(pos);
			desc_preview.draw(pos + Point<int16_t>(BODY_CENTER_X, 8));
		}
	}
}
