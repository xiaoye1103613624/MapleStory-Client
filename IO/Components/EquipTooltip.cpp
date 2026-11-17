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

	// 2026-11-17 [悬停][套装][对比]：套装面板插在主窗与对比窗之间时，
	// 对比窗整体右移「套装窗起点偏移 + 套装宽 + 缝隙」；set 无效时保持
	// 原 PREVIEW_OFFSET，与 m00832「两窗留 6px 缝隙」修复完全一致。
	int16_t EquipTooltip::set_aware_preview_offset() const
	{
		if (!settip.valid())
			return PREVIEW_OFFSET;

		// 套装窗内容起点 = 主窗起点 + FRAME_WIDTH + 18（= PREVIEW_OFFSET=282），
		// 背板右缘 = 起点 + SetTooltip::FRAME_WIDTH + 6，再留 6px 缝隙给对比窗。
		return PREVIEW_OFFSET + SetTooltip::FRAME_WIDTH + 12 + 6;
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

		// 079 参照帧：正文各行（含 inc 属性与元信息）均为纯白 + 冒号压中轴，
		// 名/冒号/值同色；非居中行（历史路径）同样取白，仅值用 valuecolor。
		// 079 原生 1:1 (m00303)：正文标签奶油(255,238,204)、数值纯白；
		// 旧实现让居中行标签跟随 valuecolor，一刀切 WHITE / 奶油都不对。
		const Color::Name namecolor = Color::Name::TOOLTIPBODY;

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
	// 2026-11-15 按 079 参照帧 + Angel 上游重排：头部区块整体上移，
	// 图标底板紧贴名称分隔线、需求块与框顶齐、成长/职业/正文间距一致。
	// 依据：079 逐像素（缩放 ≈1.288）、Angel base.draw(pos+(12,10))。
	namespace
	{
		constexpr int16_t REQ_LABEL_X = 97;      // 需求块标签位图 x（相对需求块起点）
		constexpr int16_t REQ_VALUE_DX = 50;     // 数字紧跟标签，47px 位图 + 3px 间隙
		// 5x7 字模相对 47x11 标签下沉 2px，视觉与标签中线齐（2026-11-11 用户实拍）。
		constexpr int16_t REQ_VALUE_DY = 2;
		constexpr int16_t GROWTH_LEV_Y = 72;     // 第 7 行「道具等级」（紧跟 6 行需求）
		constexpr int16_t GROWTH_EXP_Y = 84;     // 第 8 行「道具经验」
		// 需求块推进：6 行 / 8 行分别 77 / 101；基准 41 与需求块起点前移 47 同步。
		constexpr int16_t REQ_SHIFT_LEGACY = 41;
		constexpr int16_t REQ_SHIFT_BASE = 77;
		constexpr int16_t REQ_SHIFT_GROWTH = 101;
		// 框体基准高度：内容整体较原布局上移 73px，同步收紧 73px，背板底边距不变。
		constexpr int16_t HEIGHT_BASE = 540 + 15 - 73;
		// 079 原生 1:1 (m00303)：名称墨团 y95..110、图标框顶 y138 ⇒ 名称底到图标 28px。
		// A14B 名称墨高约 15px（绘制原点 y7 ⇒ 墨底 y22），图标/需求块起点取 52 ⇒ 30px 间距。
		// 旧 30 把 req 压到名称脚边（m00304 ③）；52 同时给名称区两行隐藏词条
		//（潜能 y24 / 交易 y38）留出不重叠空间。
		constexpr int16_t HEADER_SHIFT = 52;  // m00304 ③：名称→req/图标 30px 间距
	}

	// 道具图标底板（2026-11-15 按 079 参照帧还原）：UIWindow.img/IconBase/0 是
	// 32x32 的白底 + 黑描边 + 蓝角方框，与道具图标同为 2x 绘制、两者同心同尺寸。
	// orange-wz 实读该位图：白(255,255,255) 780px / 黑 112px / 蓝角(187,204,221)。
	// 079 帧里它是半透明浅蓝（非实心白），故以 ICON_BOX_ALPHA 控制不透明度。
	namespace
	{
		constexpr int16_t ICON_BOX_SIZE = 32;	// 原生 32x32
		constexpr float ICON_BOX_SCALE = 2.0f;	// 与 itemicon 同 2x
		constexpr float ICON_BOX_ALPHA = 0.65f;	// 079 半透明浅蓝白
		constexpr int16_t ICON_BOX_X = 16;		// 相对分隔线（与原 iconbase 同位）
		constexpr int16_t ICON_BOX_Y = 0;				// 与需求首行同起点（079）
	}

	EquipTooltip::EquipTooltip()
	{
		nl::node Equip = nl::nx::UI["UIWindow.img"]["ToolTip"]["Equip"];
		nl::node EquipCan = Equip["Can"];
		nl::node EquipCannot = Equip["Cannot"];

		// 原生职业行下方分隔线为 1px 纯白（参考帧 y237 全宽 255,255,255），旧 α0.4 偏灰。
		separator = ColorLine(FRAME_WIDTH, Color::Name::WHITE, 1.0f, false);

		// 2026-11-15 按 079 参照帧还原：图标背后就是 UIWindow.img/IconBase/0
		//（32x32 白底 + 黑描边 + 蓝角），与道具图标同为 2x 绘制。
		// 旧实现用 32x32 ColorBox 直接 draw（未乘 2x），底板只有图标一半大，
		// 图标溢出底板 —— 这正是 079 参照与当前截图「背景大小不一致」的根因。
		nl::node iconbase_node = nl::nx::UI["UIWindow.img"]["IconBase"]["0"];

		if (iconbase_node)
		iconbase = Texture(iconbase_node);


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
		reqstatpositions[MapleStat::Id::LEVEL] = Point<int16_t>(REQ_LABEL_X, 0);
		reqstatpositions[MapleStat::Id::STR] = Point<int16_t>(REQ_LABEL_X, 12);
		reqstatpositions[MapleStat::Id::DEX] = Point<int16_t>(REQ_LABEL_X, 24);
		reqstatpositions[MapleStat::Id::INT] = Point<int16_t>(REQ_LABEL_X, 36);
		reqstatpositions[MapleStat::Id::LUK] = Point<int16_t>(REQ_LABEL_X, 48);
		reqstatpositions[MapleStat::Id::FAME] = Point<int16_t>(REQ_LABEL_X, 60);

		reqset[false] = Charset(EquipCannot, Charset::Alignment::LEFT);
		reqset[true] = Charset(EquipCan, Charset::Alignment::LEFT);

		// 包内无 YellowNumber，可装备态改用 Can 数字字模（绿）
		lvset[false] = Charset(EquipCannot, Charset::Alignment::LEFT);
		lvset[true] = Charset(EquipCan, Charset::Alignment::LEFT);

		// 原生职业名双态位图：Can/* 纯白 (255,255,255)、Cannot/* 红 (204,17,34)
		//（orange-wz 实读 live UIWindow.img 逐像素坐实）。6 张恒画，逐职业取态。
		static const char* const jobnames[6] = {"beginner", "warrior", "magician", "bowman", "thief", "pirate"};

		for (uint8_t i = 0; i < 6; ++i)
		{
			jobs[false][i] = EquipCannot[jobnames[i]];
			jobs[true][i] = EquipCan[jobnames[i]];
		}

		invpos = 0;
		invpos_preview = 0;
		show_compare = Configuration::get().get_equip_compare();
		has_trade = false;
		has_trade_preview = false;
		has_owner = false;
		has_owner_preview = false;
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
		has_trade = false;
		has_trade_preview = false;
		has_owner = false;
		has_owner_preview = false;
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
					case 3: // Magician, Warrior
						okjobs_preview.push_back(1);
						okjobs_preview.push_back(2);
						canequip_preview[MapleStat::Id::JOB] = (stats.get_stat(MapleStat::Id::JOB) / 100 == 1) || (stats.get_stat(MapleStat::Id::JOB) / 100 >= 20) || stats.get_stat(MapleStat::Id::JOB) / 100 == 2;
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

					// 079 原生 1:1 名称墨高约 16px、粗白字；A12B 实测仅 11px（m00304 ①）。
					name_preview = Text(Text::Font::A14B, Text::Alignment::CENTER, namecolor, namestr, 400);
					// 2026-11-11 用户实拍：右上「攻击力增加」整行移除（历史误接的 equip.label_atkinc）。

					std::string desctext = itemdata.get_desc();
					hasdesc_preview = desctext.size() > 0;

					if (hasdesc_preview)
					{
						// 分隔线以下描述：水平居中（非图标旁前置块）。
						desc_preview = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::TOOLTIPBODY, desctext, 250);
						height_preview += desc_preview.height() + 10;
					}

					// 分隔线以下：冒号中轴居中（center_colon=true）；全部纯白。
					itemid_preview.build(Locale::format("equip.label_itemid", std::to_string(item_id)), Color::Name::WHITE, true);
					category_preview.build(Locale::format("equip.label_type", Locale::enum_text("equip.type.", equipdata.get_type())), Color::Name::WHITE, true);


					{
					const std::string tlab = equip_trade_label(itemdata);
					has_trade_preview = !tlab.empty();
					// 079 原生（m00304 ④）：交易词条在名称区下方独立叠高行，
					// 不再计入分隔线以下属性块高度。
					if (has_trade_preview)
						trade_line_preview.build(tlab, Color::Name::ORANGE, true);
					}

					// 所有者行（对比窗侧）：数据源为已装备物品 equip（:352），
					// owner 非空才显示，与主窗口径对称。
					{
						const std::string& owner = equip.get_owner();
						has_owner_preview = !owner.empty();
						if (has_owner_preview)
						{
							owner_line_preview.build(Locale::format("equip.owner", owner), Color::Name::ORANGE, true);
							height_preview += 14;
						}
					}

					is_weapon_preview = equipdata.is_weapon();

					if (is_weapon_preview)
					{
						const WeaponData& weapon = WeaponData::get(item_id);
						wepspeed_preview.build(Locale::format("equip.label_wepspeed", Locale::enum_text("equip.speed.", weapon.getspeedstring())), Color::Name::WHITE, true);
					}
					else
					{
						height_preview -= 18;
					}

					hasslots_preview = (equip.get_slots() > 0) || (equip.get_level() > 0);

					if (hasslots_preview)
					{
						slots_preview.build(Locale::format("equip.label_slots", std::to_string(equip.get_slots())), Color::Name::WHITE, true);

						std::string vicious = std::to_string(equip.get_vicious());

						if (equip.get_vicious() > 1)
							vicious.append(" " + Locale::text("equip.label_max") + " ");

						hammers_preview.build(Locale::format("equip.label_hammers", vicious), Color::Name::WHITE, true);
					}
					else
					{
						height_preview -= 36;
					}

					statlabels_preview.clear();
					statdeltas_preview.clear();

					// 095 潜能行（对比窗侧）：镜像主窗 set_equip 的 :852-888 口径，
					// 数据源为已装备物品 equip（:352），文案走 locale equip.potline.*，
					// 缺键回退带编号占位（Locale::text_or，Util/Locale.h:55-57）。
					{
						int16_t pot_height_preview = 0;

						for (size_t i = 0; i < 5; ++i)
						{
							haspotlines_preview[i] = false;

							int16_t pot_id = (i < 3) ? equip.get_potential(i) : equip.get_bonus_potential(i - 3);

							if (pot_id <= 0)
								continue;

							const std::string key = "equip.potline." + std::to_string(pot_id);
							std::string content = Locale::text_or(key, "潜能 " + std::to_string(pot_id));

							if (i >= 3)
								content = "(B) " + content;

							potlines_preview[i] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
								content, FRAME_WIDTH - 20);
							haspotlines_preview[i] = true;
							pot_height_preview += 15;
						}

						if (pot_height_preview > 0)
							height_preview += pot_height_preview + 6;
					}

					// 095 强化三件套（对比窗侧）：镜像主窗 :890-941，火花/宝珠 WHITE、
					// 星之力 YELLOW；星之力是星数不是词条 id，按「星之力 N ★×」显示。
					{
						int16_t extra_height_preview = 0;

						has_spark_line_preview = false;
						has_soul_line_preview = false;
						has_star_line_preview = false;

						if (int16_t sid = equip.get_spark(); sid > 0)
						{
							const std::string content = Locale::text_or(
								"equip.spark." + std::to_string(sid),
								"火花 " + std::to_string(sid));
							spark_line_preview = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
								content, FRAME_WIDTH - 20);
							has_spark_line_preview = true;
							extra_height_preview += 15;
						}

						if (int16_t slid = equip.get_soul(); slid > 0)
						{
							const std::string content = Locale::text_or(
								"equip.soul." + std::to_string(slid),
								"宝珠 " + std::to_string(slid));
							soul_line_preview = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
								content, FRAME_WIDTH - 20);
							has_soul_line_preview = true;
							extra_height_preview += 15;
						}

						if (uint8_t sf = equip.get_starforce(); sf > 0)
						{
							std::string stars;
							for (uint8_t i = 0; i < sf && i < 25; ++i)
								stars += "*";
							star_line_preview = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW,
								"星之力 " + std::to_string(sf) + " " + stars, FRAME_WIDTH - 20);
							has_star_line_preview = true;
							extra_height_preview += 15;
						}

						if (extra_height_preview > 0)
							height_preview += extra_height_preview + 6;
					}

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

		// 079 原生 1:1 名称墨高约 16px、粗白字；A12B 实测仅 11px（m00304 ①）。
		// A14B 墨高约 15px，是「加大加粗」后的 079 口径。
		name = Text(Text::Font::A14B, Text::Alignment::CENTER, namecolor, namestr, 400);
		// 2026-11-11 用户实拍：右上「攻击力增加」整行移除（历史误接的 equip.label_atkinc）。

		std::string desctext = itemdata.get_desc();
		hasdesc = desctext.size() > 0;

		if (hasdesc)
		{
			desc = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::TOOLTIPBODY, desctext, 250);
			height += desc.height() + 10;
		}

		// 用户裁决：分隔线以下全部冒号中轴居中；inc 行全白，元信息纯白。
		itemid.build(Locale::format("equip.label_itemid", std::to_string(item_id)), Color::Name::WHITE, true);
		category.build(Locale::format("equip.label_type", Locale::enum_text("equip.type.", equipdata.get_type())), Color::Name::WHITE, true);


		{
		const std::string tlab = equip_trade_label(itemdata);
		has_trade = !tlab.empty();
		// 079 原生（m00304 ④）：交易词条在名称区下方独立叠高行，
		// 不再计入分隔线以下属性块高度。
		if (has_trade)
			trade_line.build(tlab, Color::Name::ORANGE, true);
		}

		// 所有者行（095/服务端 owner 字段）：owner 非空才显示，空 owner 不留空行。
		// 与交易行同色（ORANGE），画在名称区下方词条区；多一行补偿高度防止背板截断。
		{
		const std::string& owner = equip.get_owner();
		has_owner = !owner.empty();
		if (has_owner)
		{
			owner_line.build(Locale::format("equip.owner", owner), Color::Name::ORANGE, true);
			height += 14;
		}
		}

		is_weapon = equipdata.is_weapon();

		if (is_weapon)
		{
			const WeaponData& weapon = WeaponData::get(item_id);
			wepspeed.build(Locale::format("equip.label_wepspeed", Locale::enum_text("equip.speed.", weapon.getspeedstring())), Color::Name::WHITE, true);
		}
		else
		{
			height -= 18;
		}

		hasslots = (equip.get_slots() > 0) || (equip.get_level() > 0);

		if (hasslots)
		{
			slots.build(Locale::format("equip.label_slots", std::to_string(equip.get_slots())), Color::Name::WHITE, true);

			std::string vicious = std::to_string(equip.get_vicious());

			if (equip.get_vicious() > 1)
				vicious.append(" " + Locale::text("equip.label_max") + " ");

			hammers.build(Locale::format("equip.label_hammers", vicious), Color::Name::WHITE, true);
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

				potlines[i] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
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

				spark_line = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
					content, FRAME_WIDTH - 20);
				has_spark_line = true;
				extra_height += 15;
			}

			if (int16_t slid = equip.get_soul(); slid > 0)
			{
				const std::string content = Locale::text_or(
					"equip.soul." + std::to_string(slid),
					"宝珠 " + std::to_string(slid));

				soul_line = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
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
		// 底板锚点：名称之后 (16,0)，32x32 再 2x，与需求首行同起点（079/Angel）。
		const Point<int16_t> boxpos = pos + Point<int16_t>(ICON_BOX_X, ICON_BOX_Y);

		// 079 参照帧：底板是半透明浅蓝白（IconBase/0 @ α0.65 叠在 (79,79,119)
		// 背板上 ≈ (193,193,207)，对照取样 (196,196,211)），不是纯白实心块。
		if (iconbase.is_valid())
			iconbase.draw(DrawArgument(boxpos, ICON_BOX_SCALE, ICON_BOX_SCALE, ICON_BOX_ALPHA));

		if (icon.is_valid())
		{
			// Texture::draw 的 pos 对应图标 origin 点：渲染矩形中心 =
			// pos + scale*(w/2 - origin)。让图标中心落在底板中心 (32,32)，须反解
			// pos = boxpos + 32 + origin*scale - (w,h)*scale/2。旧实现把左上角
			// 钉死在 (2,2)，小图标整体偏左上（2026-11-15 用户实拍打回）。
			const Point<int16_t> ivorg = icon.get_origin();
			const int16_t half = static_cast<int16_t>(ICON_BOX_SIZE * ICON_BOX_SCALE / 2);
			const int16_t hw = static_cast<int16_t>(icon.width() * ICON_BOX_SCALE / 2);
			const int16_t hh = static_cast<int16_t>(icon.height() * ICON_BOX_SCALE / 2);
			const Point<int16_t> iconpos(
				boxpos.x() + half + static_cast<int16_t>(ivorg.x() * ICON_BOX_SCALE) - hw,
				boxpos.y() + half + static_cast<int16_t>(ivorg.y() * ICON_BOX_SCALE) - hh
			);

			icon.draw(DrawArgument(iconpos, ICON_BOX_SCALE, ICON_BOX_SCALE));
		}
	}

	void EquipTooltip::draw_job_line(Point<int16_t> pos, const std::vector<uint8_t>& allowedjobs) const
	{
		// 079 原生 / Angel：职业行恒为 6 张职业名位图并排，不因 reqJOB 裁剪。
		// 该职业在允许集合里 -> Can（白）；否则 -> Cannot（红，实测 204,17,34）。
		// m00754 ⑦ 连打三回的真因：用户要的是「成员之间空隙一致」，不是
		// 「墨迹左沿等距」。orange-wz 实读 live Can/Cannot 六位图墨盒完全一致：
		// 透明左留白 {0,9,1,2,0,3}、墨宽 {23,23,35,34,23,23}。R4 让墨沿按
		// JOB_STEP=42 等距 ⇒ 空隙 {19,19,7,8,19,19} 忽大忽小，实拍 m00749
		// {20,20,7,14,19} 与推算吻合。现按「上一张墨迹右沿 + 固定 JOB_GAP
		// = 下一张墨迹左沿」累计，空隙恒 11px（079 原生逐像素反算 gap≈11），
		// drawx 再减去该位图 leftpad 把墨沿钉到累计点。
		static constexpr int16_t JOB_INK_PAD[6] = { 0, 9, 1, 2, 0, 3 };
		static constexpr int16_t JOB_INK_W[6] = { 23, 23, 35, 34, 23, 23 };
		static constexpr int16_t JOB_GAP = 11;
		// m00877 ①：空隙恒 11 之外，左右边距也必须相等 —— 墨迹总宽
		// 216px 在 FRAME_WIDTH 内两侧均分起始点，而非贴 JOB_SIDE=4 起画
		// （否则左窄右宽，用户一眼看出不对称）。
		constexpr int16_t total_ink = 23 + 23 + 35 + 34 + 23 + 23 + 5 * JOB_GAP;
		int16_t ink_x = static_cast<int16_t>((FRAME_WIDTH - total_ink) / 2);

		for (uint8_t i = 0; i < 6; ++i)
		{
			bool allowed = false;

			for (uint8_t j : allowedjobs)
			{
				if (j == i)
				{
					allowed = true;
					break;
				}
			}

			const int16_t drawx = static_cast<int16_t>(ink_x - JOB_INK_PAD[i]);
			const Texture& tex = jobs[allowed].at(i);
			tex.draw(pos + Point<int16_t>(drawx, 0));

			ink_x = static_cast<int16_t>(ink_x + JOB_INK_W[i] + JOB_GAP);
		}
	}

	void EquipTooltip::draw(Point<int16_t> pos) const
	{
		if (invpos == 0)
			return;

		draw_preview(pos);

		int16_t max_width = Constants::Constants::get().get_viewwidth();
		int16_t max_height = Constants::Constants::get().get_viewheight();
		// 主窗背板右缘 = X + FRAME_WIDTH + 6（夹取再多留 6px 余量）。
		int16_t cur_width = pos.x() + FRAME_WIDTH + 12;
		int16_t cur_height = pos.y() + 36;

		// 2026-11-17 布局 [悬停][套装][对比]：套装插在主窗与对比窗之间。
		// 右边界取「当前存在的最右一窗」：套装 > 主窗；对比窗 > 套装。
		// 每个 if 用覆盖式赋值（非累加），保证三窗共用同一右界口径。
		if (settip.valid())
			cur_width = pos.x() + PREVIEW_OFFSET + SetTooltip::FRAME_WIDTH + 12;

		if (invpos_preview == 1)
			cur_width = pos.x() + set_aware_preview_offset() + FRAME_WIDTH + 12;

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

		// 图2/装备tip参照帧定案：装备 tooltip 带原生十字织纹背板
		backdrop.draw_equip(pos + Point<int16_t>(-6, -6), FRAME_WIDTH + 12, height + 32);

		name.draw(pos + Point<int16_t>(BODY_CENTER_X, 7));

		// 名称区隐藏词条：潜能等级、交易限制（唯一道具 / 不可交易）逐行叠高，
		// 都画在名称下方、图标上方，不侵占分隔线以下属性区（m00304 ②④）。
		int16_t sub_y = 24;

		if (prank != Equip::Potential::POT_NONE)
		{
			potflag.draw(pos + Point<int16_t>(BODY_CENTER_X, sub_y));
			sub_y += 14;
		}

		if (has_trade)
		{
			trade_line.draw(pos + Point<int16_t>(0, sub_y));
			sub_y += 14;
		}

		// 所有者行：与交易行同区叠高；owner 为空时 has_owner=false 不占行。
		if (has_owner)
		{
			owner_line.draw(pos + Point<int16_t>(0, sub_y));
			sub_y += 14;
		}

		// 词条 ≤2 行时 sub_y ≤52，仍用 HEADER_SHIFT 保持原有间距不变；
		// 仅当 3 条词条全满才按 sub_y 下压，避免 owner 行压住图标。
		pos.shift_y((sub_y > HEADER_SHIFT) ? sub_y : HEADER_SHIFT);

		draw_item_icon(pos, itemicon);

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

		Point<int16_t> job_position(pos + Point<int16_t>(0, 14));

		// 职业行恒画 6 张，逐职业 Can(白)/Cannot(红)；draw_job_line 内按
		// 「墨迹右沿 + JOB_GAP」累计排布，成员间空隙恒 11px（m00754 ⑦）。
		draw_job_line(job_position, okjobs);

		// 079 原生 1:1：职业墨团 y219-229 到分隔线 y237 仅 8px；
		// 旧 47 留白 20px（m00304 ⑤「上下间距过大」），收到 35。
		separator.draw(pos + Point<int16_t>(0, 35));

		pos.shift_y(37);

		// 分隔线以下（用户 2026-11-12）：整区水平居中；inc 行冒号中轴+全白；
		// 元信息冒号中轴+纯白；交易限制橙字居中。前置需求/职业块在上方不改。
		int16_t stat_y = 15;

		itemid.draw(pos);
		pos.shift_y(stat_y);

		category.draw(pos);
		pos.shift_y(stat_y);

		// 079 原生（m00304 ④）：交易词条已移到名称区下方，此处不再占属性行。

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

		// ⑤ 套装 companion 面板：2026-11-17 布局 [悬停][套装][对比]，
		// 套装固定插在主窗右侧（对比窗左侧），与是否显示对比窗无关。
		if (settip.valid())
		{
			// 顶端对齐：pos 在绘制末尾已被需求块下推，故用「tip 起点 y」对齐，
			// 起点 = 当前 pos.y - 已下推量（height 是需求块之后的总高）。
			int16_t tip_top = pos.y() - height;
			// 主窗背板右缘 = X + FRAME_WIDTH + 6，再留 12px 缝隙 → 套装起点。
			int16_t set_x = pos.x() + PREVIEW_OFFSET;
			Point<int16_t> setpos(set_x, tip_top);

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

		// 总右界与 draw() 完全一致（基于未平移的 X），保证两窗同量夹取；
		// 旧实现先 shift_x(FRAME_WIDTH) 再算 cur_width，让两片背板重叠 12px。
		int16_t max_width = Constants::Constants::get().get_viewwidth();
		int16_t max_height = Constants::Constants::get().get_viewheight();
		int16_t preview_off = set_aware_preview_offset();
		int16_t cur_width = pos.x() + preview_off + FRAME_WIDTH + 12;

		int16_t cur_height = pos.y() + height_preview + 36;

		int16_t adj_x = cur_width - max_width;
		int16_t adj_y = cur_height - max_height;

		if (adj_x > 0)
			pos.shift_x(adj_x * -1);

		if (adj_y > 0)
			pos.shift_y(adj_y * -1);

		// 2026-11-17 [悬停][套装][对比]：对比窗从套装窗右侧起排。
		pos.shift_x(preview_off);

		// 装备 tooltip 预览层同带织纹背板
		backdrop.draw_equip(pos + Point<int16_t>(-6, -6), FRAME_WIDTH + 12, height_preview + 32);

		name_preview.draw(pos + Point<int16_t>(BODY_CENTER_X, 7));

		// 名称区隐藏词条：潜能等级、交易限制逐行叠高（m00304 ②④）。
		int16_t sub_y = 24;

		if (prank_preview != Equip::Potential::POT_NONE)
		{
			potflag_preview.draw(pos + Point<int16_t>(BODY_CENTER_X, sub_y));
			sub_y += 14;
		}

		if (has_trade_preview)
		{
			trade_line_preview.draw(pos + Point<int16_t>(0, sub_y));
			sub_y += 14;
		}

		// 所有者行（对比窗侧）：与交易行同区叠高，owner 为空不占行。
		if (has_owner_preview)
		{
			owner_line_preview.draw(pos + Point<int16_t>(0, sub_y));
			sub_y += 14;
		}

		// 词条 ≤2 行保持 HEADER_SHIFT，3 条全满才下压防压图标。
		pos.shift_y((sub_y > HEADER_SHIFT) ? sub_y : HEADER_SHIFT);

		// 2026-11-11：攻击力增加行（equip.label_atkinc）属历史误接，原生 tip 无此行，
		// 已整段移除。图标与主窗体共用 draw_item_icon（IconBase 位图底板 2x + 中心对齐）。
		draw_item_icon(pos, itemicon_preview);

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

		Point<int16_t> job_position(pos + Point<int16_t>(0, 14));

		// 与主窗体同一处：恒画 6 张、逐职业白/红双态、成员间空隙恒 11px。
		draw_job_line(job_position, okjobs_preview);

		// 079 原生 1:1：职业墨团 y219-229 到分隔线 y237 仅 8px；
		// 旧 47 留白 20px（m00304 ⑤「上下间距过大」），收到 35。
		separator.draw(pos + Point<int16_t>(0, 35));

		pos.shift_y(37);

		int16_t stat_y = 15;

		itemid_preview.draw(pos);
		pos.shift_y(stat_y);

		category_preview.draw(pos);
		pos.shift_y(stat_y);

		// 079 原生（m00304 ④）：交易词条已移到名称区下方，此处不再占属性行。

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

		// 095 潜能行（对比窗侧）：镜像主窗 :1163-1184 的顺序与配色，
		// 画在 slots/hammers 之后、描述之前，数据源为已装备物品。
		{
			bool any_preview = false;
			for (bool h : haspotlines_preview)
				any_preview = any_preview || h;

			if (any_preview)
			{
				pos.shift_y(6);
				for (size_t i = 0; i < 5; ++i)
				{
					if (!haspotlines_preview[i])
						continue;
					potlines_preview[i].draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
					pos.shift_y(15);
				}
			}
		}

		// 095 强化三件套（对比窗侧）：镜像主窗 :1186-1208，缺项跳过不留空行。
		if (has_spark_line_preview || has_soul_line_preview || has_star_line_preview)
		{
			pos.shift_y(6);

			if (has_spark_line_preview)
			{
				spark_line_preview.draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
				pos.shift_y(15);
			}

			if (has_soul_line_preview)
			{
				soul_line_preview.draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
				pos.shift_y(15);
			}

			if (has_star_line_preview)
			{
				star_line_preview.draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
				pos.shift_y(15);
			}
		}

		if (hasdesc_preview)
		{
			pos.shift_y(-4);
			separator.draw(pos);
			desc_preview.draw(pos + Point<int16_t>(BODY_CENTER_X, 8));
		}
	}
}
