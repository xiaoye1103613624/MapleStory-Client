//////////////////////////////////////////////////////////////////////////////////
#include "SetTooltip.h"

#include "../../Data/ItemData.h"
#include "../../Data/SetItemData.h"
#include "../../Net/Packets/SetItemPackets.h"
#include "../../Util/Locale.h"

#include <algorithm>
#include <cctype>

namespace ms
{
	namespace
	{
		constexpr int16_t PAD_X = 10;
		constexpr int16_t PAD_TOP = 8;
		constexpr int16_t LINE_H = 14;
		constexpr int16_t TITLE_H = 18;
		constexpr int16_t SEP_H = 6;
		constexpr int16_t PAD_BOTTOM = 8;
		// 2026-11-20 高版本套装面板：成员名前的装备图标。原图图标约 20px、
		// 名称与图标垂直居中；这里按 2x 源图缩到 ICON_SZ，留出 ICON_GAP 再画文字。
		constexpr int16_t ICON_SZ = 16;
		constexpr int16_t ICON_GAP = 4;
		constexpr int16_t ICON_TEXT_X = ICON_SZ + ICON_GAP;
		// 2026-11-21：档位属性行相对「N套装效果」标题后退一个中文字（A11M 字宽≈11px）。
		constexpr int16_t ATTR_INDENT = 11;
		// 属性「标签」占固定列宽，值从该列起排 → 所有行冒号/数值对齐成一列。
		constexpr int16_t ATTR_LABEL_W = 60;

		std::string stat_label(const std::string& key)
		{
			if (key == "STR") return "力量";
			if (key == "DEX") return "敏捷";
			if (key == "INT") return "智力";
			if (key == "LUK") return "运气";
			if (key == "HP") return "MaxHP";
			if (key == "MP") return "MaxMP";
			if (key == "WATK") return "攻击力";
			if (key == "MAGIC") return "魔法力";
			if (key == "WDEF") return "防御力";
			if (key == "MDEF") return "魔法防御";
			if (key == "ACC") return "命中";
			if (key == "AVOID") return "回避";
			if (key == "HANDS") return "手技";
			if (key == "SPEED") return "移动速度";
			if (key == "JUMP") return "跳跃力";
			if (key == "ALLSTAT") return "全属性";
			if (key == "DAMAGE") return "伤害";
			if (key == "BOSS") return "首领怪伤害";
			if (key == "IGNOREDEF") return "无视防御";
			if (key == "HP_RATE") return "MaxHP%";
			if (key == "MP_RATE") return "MaxMP%";
			if (key == "STR_RATE") return "力量%";
			if (key == "DEX_RATE") return "敏捷%";
			if (key == "INT_RATE") return "智力%";
			if (key == "LUK_RATE") return "运气%";
			return key;
		}

		bool is_percent(const std::string& key)
		{
			return key == "DAMAGE" || key == "BOSS" || key == "IGNOREDEF"
				|| key.find("_RATE") != std::string::npos;
		}

		// 2026-11-21：武器号段不止 130-149，还有 152-159（双手剑/长枪/拳套等，
		// live Weapon/0152*.img 实证）与 170。旧范围漏掉 152/153 → 套装里
		// 这两类各自成行（用户红框「多件武器」）。统一折叠为一把武器行。
		bool is_weapon_cat(int cat)
		{
			return (cat >= 130 && cat <= 149) || (cat >= 152 && cat <= 159) || cat == 170;
		}

		const char* part_label(int32_t item_id)
		{
			const int cat = item_id / 10000;

			switch (cat)
			{
			case 100: return "帽子";
			case 101: return "脸饰";
			case 102: return "眼饰";
			case 103: return "耳饰";
			case 104: return "上衣";
			case 105: return "套服";
			case 106: return "裤/裙";
			case 107: return "鞋子";
			case 108: return "手套";
			case 109: return "盾牌";
			case 110: return "披风";
			case 111: return "戒指";
			case 112: return "项链";
			case 113: return "腰带";
			case 114: return "勋章";
			case 115: return "肩饰";
			default:
				if (is_weapon_cat(cat))
					return "武器";
				return "";
			}
		}

		int equip_sort_key(int32_t item_id)
		{
			const int cat = item_id / 10000;

			if (is_weapon_cat(cat))
				return 1000; // 武器 OR 组统一排序键，折叠为一行

			switch (cat)
			{
			case 100: return 10;
			case 101: return 20;
			case 102: return 30;
			case 103: return 40;
			case 104: return 50;
			case 105: return 55;
			case 106: return 60;
			case 107: return 70;
			case 108: return 80;
			case 109: return 85;
			case 110: return 90;
			case 113: return 95;
			case 115: return 96;
			case 112: return 97;
			case 111: return 98;
			case 114: return 99;
			default: return 200 + cat;
			}
		}

		/// 折叠分组键：同类部位（含全武器）合为一行，对齐 completeCount。
		int slot_group_key(int32_t item_id)
		{
			return equip_sort_key(item_id);
		}

		const char* desc_key_for_group(int group_key)
		{
			if (group_key == 1000)
				return "weapon";

			return nullptr;
		}

		// 2026-11-20：String/Eqp.img 里没有 1052357/1072521 等成员名（服务端
		// SetItemDO 也只有 setName，无成员名字段）。按用户新参考图的命名法，
		// 从「套装名」派生成员名：去掉尾部「套装」→「冒险岛宝石」+ 部位 = 「冒险岛宝石帽子」。
		// 这样既不露裸 ID，也不露「未鉴定」，与参考图逐件命名一致。
		std::string derived_member_name(const std::string& set_name, const char* part)
		{
			std::string base = set_name;

			// 2026-11-21：服务端文案首行可能带「(n/m)」件数后缀（如
			//「冒险岛宝石套装 (8/8)」），派生成员名时必须去掉，否则武器行
			// 会显示成「冒险岛宝石套装 (8/8)」而非「冒险岛宝石武器」。
			const size_t paren = base.rfind('(');
			if (paren != std::string::npos && base.find(')', paren) != std::string::npos)
				base.erase(paren);
			// 去尾部空白
			while (!base.empty() && base.back() == ' ') base.pop_back();

			const std::string suffix = "套装";
			if (base.size() >= suffix.size()
				&& base.compare(base.size() - suffix.size(), suffix.size(), suffix) == 0)
				base.erase(base.size() - suffix.size());

			if (!part || !part[0])
				return base.empty() ? set_name : base;

			return base + part;
		}

		// 未激活行用 SILVER 而非 DARKGREY：深色 tip 背板上 DARKGREY 几乎看不见，
		// 会误看成「只有悬停那一件 / 没有档位内容」。
		Color::Name line_color(bool equipped, bool hover, bool active_tier)
		{
			if (hover)
				return Color::Name::YELLOW;

			if (active_tier)
				return Color::Name::YELLOW;

			if (equipped)
				return Color::Name::WHITE;

			return Color::Name::SILVER;
		}

		// 2026-11-21 用户实拍「档位标题颜色偏灰、要纯色、加粗」：
		// 旧 GAUGEGREEN(153,204,51) 在深色背板上发灰；改用 LIGHTGREEN(0,1,0)
		// 纯饱和绿，字体在构造处改 A11B 加粗。阶段标题恒绿，与是否达到无关。
		Color::Name tier_header_color(int16_t count, bool reached)
		{
			(void)reached;
			(void)count;
			return Color::Name::LIGHTGREEN;
		}

		// 服务端文案标题若大量 '?' / 替换符，视为编码损坏，保留本地 WZ 名。
		bool looks_garbled(const std::string& s)
		{
			if (s.empty())
				return true;

			size_t q = 0;
			size_t han = 0;

			for (size_t i = 0; i < s.size();)
			{
				const unsigned char c = static_cast<unsigned char>(s[i]);

				if (c == '?')
				{
					++q;
					++i;
					continue;
				}

				if (c >= 0xE0 && c <= 0xEF && i + 2 < s.size()
					&& (static_cast<unsigned char>(s[i + 1]) & 0xC0) == 0x80
					&& (static_cast<unsigned char>(s[i + 2]) & 0xC0) == 0x80)
				{
					++han;
					i += 3;
					continue;
				}

				++i;
			}

			if (q >= 3 && han == 0)
				return true;

			return false;
		}

		std::vector<std::string> split_rows(const std::string& text)
		{
			std::vector<std::string> rows;
			size_t start = 0;

			while (start <= text.size())
			{
				size_t nl = text.find_first_of("\r\n", start);
				std::string row = text.substr(start,
					nl == std::string::npos ? std::string::npos : nl - start);

				// 去掉服务端色码残留（#c #k 等），避免当正文显示。
				std::string cleaned;

				for (size_t i = 0; i < row.size(); ++i)
				{
					if (row[i] == '#' && i + 1 < row.size())
					{
						const char n = row[i + 1];

						if (std::isalpha(static_cast<unsigned char>(n))
							|| std::isdigit(static_cast<unsigned char>(n)))
						{
							++i;
							continue;
						}
					}

					cleaned.push_back(row[i]);
				}

				while (!cleaned.empty()
					&& (cleaned.back() == ' ' || cleaned.back() == '\t'))
					cleaned.pop_back();

				if (!cleaned.empty())
					rows.push_back(cleaned);

				if (nl == std::string::npos)
					break;

				start = nl + 1;

				while (start < text.size()
					&& (text[start] == '\r' || text[start] == '\n'))
					++start;
			}

			return rows;
		}
	}

	void SetTooltip::set_item(int32_t item_id, const std::vector<int32_t>& equipped_item_ids, uint8_t hover_level)
	{
		is_valid = false;
		lines.clear();
		panel_height = 0;

		int32_t set_id = SetItemData::set_of(item_id);

		if (set_id <= 0)
			return;

		const SetItemData::Definition* def = SetItemData::get(set_id);

		if (!def)
			return;

		if (SetItemData::begin_bonus_request(set_id))
			SetItemBonusRequestPacket(set_id).dispatch();

		int16_t equipped = SetItemData::count_equipped(set_id, equipped_item_ids);

		std::string local_title = def->name;

		if (local_title.empty())
			local_title = "套装" + std::to_string(set_id);

		// 服务端文案标题（去色码后）作为套装名权威，成员名从它派生更贴近参考图。
		std::string set_name_base = local_title;

		if (SetItemData::has_server_text(set_id))
		{
			const std::vector<std::string> srows = split_rows(SetItemData::get_server_text(set_id));
			if (!srows.empty() && !looks_garbled(srows[0]))
				set_name_base = srows[0];
		}

		local_title += " (" + std::to_string(equipped) + "/"
			+ std::to_string(def->complete_count) + ")";

		// 2026-11-20 图2/高版本：套装名居中、暖金色（参考图「神秘之影套装」）。
		title = Text(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::YELLOW,
			local_title, FRAME_WIDTH - PAD_X * 2);

		// ---- 件列表：按部位/OR 组折叠（高版本 representName/Desc 口径） ----
		// 全量 item_ids 仍用于计数；展示时同部位多件（各职业武器）合成一行。
		std::vector<int32_t> sorted_ids = def->item_ids;
		std::stable_sort(sorted_ids.begin(), sorted_ids.end(),
			[](int32_t a, int32_t b) {
				const int ga = slot_group_key(a);
				const int gb = slot_group_key(b);

				if (ga != gb)
					return ga < gb;

				return a < b;
			});

		for (size_t i = 0; i < sorted_ids.size();)
		{
			const int group = slot_group_key(sorted_ids[i]);
			std::vector<int32_t> members;

			while (i < sorted_ids.size() && slot_group_key(sorted_ids[i]) == group)
			{
				members.push_back(sorted_ids[i]);
				++i;
			}

			int32_t worn_id = 0;
			int32_t hover_id = 0;

			for (int32_t pid : members)
			{
				if (!worn_id
					&& std::find(equipped_item_ids.begin(), equipped_item_ids.end(), pid)
						!= equipped_item_ids.end())
					worn_id = pid;

				if (pid == item_id)
					hover_id = pid;
			}

			const bool worn = worn_id != 0;
			const bool hover = hover_id != 0;
			int32_t display_id = worn_id ? worn_id : (hover_id ? hover_id : members.front());
			std::string name;

			if (worn || hover || members.size() == 1)
			{
				name = ItemData::get(display_id).get_name();

				// 2026-11-20 用户新参考图：成员名 = 套装名去「套装」+ 部位
				//（「冒险岛宝石帽子/上衣/鞋…」）。String/Eqp.img 无这些 id 的 name
				// 节点，服务端 SetItemDO 也无成员名字段，故按服务端权威套装名派生。
				if (name.empty())
				{
					const char* part = part_label(display_id);
					name = derived_member_name(set_name_base, part);
				}

				// 2026-11-21 用户要求：套装成员名前加「Lv:等级」前缀，
				// 取纯装备强化等级（悬停件 Equip::get_level）。只有当前悬停/穿著的
				// 那件有该值；其余成员无实例数据，等级 0 时不显示前缀避免误导。
				if (hover && hover_level > 0)
					name = Locale::format("setitem.level", std::to_string(hover_level)) + name;
			}
			else
			{
				// OR 组未穿未悬停：优先 Desc/<slot>，其次 representName(group)，再部位标签。
				const char* dkey = desc_key_for_group(group);

				if (dkey)
				{
					auto it = def->slot_desc.find(dkey);

					if (it != def->slot_desc.end())
						name = it->second;
				}

				if (name.empty())
				{
					auto git = def->slot_desc.find("group");

					if (git != def->slot_desc.end())
						name = git->second;
				}

				if (name.empty())
				{
					const char* part = part_label(display_id);
					name = (part && part[0]) ? std::string(part) : std::to_string(display_id);
				}
			}

			Line line;
			line.active = worn || hover;
			// 2026-11-20 高版本：名称前有装备图标，故文字整体右移 ICON_TEXT_X。
			line.left = Text(Text::Font::A11M, Text::Alignment::LEFT,
				line_color(worn, hover, false), name,
				FRAME_WIDTH - PAD_X * 2 - 48 - ICON_TEXT_X);

			// 2026-11-20 用户新参考图：每个成员名前显示装备图标。
			// 多选武器（武器 OR 组，members.size()>1）参考图用「X」标记代替单件图标，
			// 表示"这组里任选一件"；其余成员显示各自真图标。
			line.multi_weapon = (group == 1000 && members.size() > 1);
			line.icon = ItemData::get(display_id).get_icon(false);
			line.has_icon = line.icon.is_valid() && !line.multi_weapon;

			const char* part = part_label(display_id);

			if (part && part[0])
			{
				line.has_right = true;
				line.right = Text(Text::Font::A11M, Text::Alignment::RIGHT,
					line_color(worn, hover, false),
					std::string("(") + part + ")", 48);
			}

			lines.push_back(std::move(line));
		}

		{
			Line blank;
			blank.left = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::TOOLTIPBODY, " ", 4);
			lines.push_back(std::move(blank));
		}

		// ---- 档位（本地 WZ / 服务端 tier 注入后的 def->tiers） ----
		std::vector<int16_t> counts;

		for (const SetItemData::Tier& t : def->tiers)
			if (std::find(counts.begin(), counts.end(), t.count) == counts.end())
				counts.push_back(t.count);

		std::sort(counts.begin(), counts.end());

		for (int16_t c : counts)
		{
			const bool reached = equipped >= c;

			Line header;
			header.active = reached;
			// 2026-11-21：档位标题加粗(A11B) + 纯色（tier_header_color）。
			header.left = Text(Text::Font::A11B, Text::Alignment::LEFT,
				tier_header_color(c, reached),
				std::to_string(c) + "套装效果", FRAME_WIDTH - PAD_X * 2);
			lines.push_back(std::move(header));

			for (const SetItemData::Tier& t : def->tiers)
			{
				if (t.count != c)
					continue;

				// 2026-11-21：属性比阶段标题再后退一个中文字，且「值」对齐到统一列，
				// 消除 STR:+8 / 攻击力:+6 / MaxHP:+100 数值忽左忽右（用户图证）。
				// 标签/值拆成两段 Text，值从固定列 ATTR_LABEL_W 起排。
				const Color::Name vcolor = reached
					? ((t.stat_key == "WATK" || t.stat_key == "MAGIC")
						? Color::Name::ORANGE : Color::Name::WHITE)
					: Color::Name::SILVER;
				std::string valuestr = "+ " + std::to_string(t.value);
				if (is_percent(t.stat_key))
					valuestr += "%";

				Line line;
				line.active = reached;
				line.indent = ATTR_INDENT;
				line.split_value = true;
				line.left = Text(Text::Font::A11M, Text::Alignment::LEFT,
					vcolor, stat_label(t.stat_key), ATTR_LABEL_W);
				line.value = Text(Text::Font::A11M, Text::Alignment::LEFT,
					vcolor, valuestr, FRAME_WIDTH - PAD_X * 2 - ATTR_LABEL_W);
				lines.push_back(std::move(line));
			}
		}

		// 服务端长文案：仅在标题未乱码时覆盖标题；有额外属性行时追加，
		// 绝不清空本地件列表 / 档位（此前会 resize 掉档位 → 只剩 (0/8)）。
		if (SetItemData::has_server_text(set_id))
		{
			const std::vector<std::string> rows = split_rows(SetItemData::get_server_text(set_id));

			if (!rows.empty() && !looks_garbled(rows[0]))
			{
				// 2026-11-20 图2/高版本：服务端文案标题同样居中 + 暖金色。
				title = Text(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::YELLOW,
					rows[0], FRAME_WIDTH - PAD_X * 2);
			}

			for (size_t r = 1; r < rows.size(); ++r)
			{
				const bool header = rows[r].find("套装") != std::string::npos
					|| rows[r].find("件") != std::string::npos;
				Line line;
				line.active = true;
				line.left = Text(Text::Font::A11M, Text::Alignment::LEFT,
					header ? Color::Name::YELLOW : Color::Name::WHITE,
					rows[r], FRAME_WIDTH - PAD_X * 2);
				lines.push_back(std::move(line));
			}
		}

		panel_height = static_cast<int16_t>(
			PAD_TOP + TITLE_H + SEP_H
			+ static_cast<int16_t>(lines.size()) * LINE_H
			+ PAD_BOTTOM);
		is_valid = true;
	}

	void SetTooltip::draw(Point<int16_t> pos) const
	{
		if (!is_valid)
			return;

		backdrop.draw(pos + Point<int16_t>(-6, -6), FRAME_WIDTH + 12, panel_height + 12);

		// 2026-11-20 用户实拍「套装名没居中还溢出」根因：
		// title 已是 CENTER 对齐（CENTER 文本的原点就是文本中心），但旧代码
		// 仍把它画在 pos.x+PAD_X=+10，于是整条标题以 x=10 为中心向左溢出面板。
		// 正确做法：CENTER 标题画在面板水平中线 pos.x + FRAME_WIDTH/2。
		Point<int16_t> cur = pos + Point<int16_t>(PAD_X, PAD_TOP);
		title.draw(Point<int16_t>(pos.x() + FRAME_WIDTH / 2, cur.y()));
		cur.shift_y(TITLE_H);
		separator.draw(cur - Point<int16_t>(PAD_X, 0));
		cur.shift_y(SEP_H);

		for (const Line& line : lines)
		{
			if (line.has_icon)
			{
				// 图标按 ICON_SZ 缩放，垂直方向与文字行居中对齐
				//（LINE_H=14，图标略高于行高即可；保持图标中心 = 文字行中心）。
				const float sc = static_cast<float>(ICON_SZ)
					/ static_cast<float>(line.icon.width() > line.icon.height()
						? line.icon.width() : line.icon.height());
				const Point<int16_t> ivorg = line.icon.get_origin();
				const int16_t iw = static_cast<int16_t>(line.icon.width() * sc);
				const int16_t ih = static_cast<int16_t>(line.icon.height() * sc);
				// 2026-11-21 用户实拍「图标偏上，往下来一半字体高度」：
				// A11M 字高约 11px，半个字高≈6px；在居中基线下再下移 ICON_DY。
				constexpr int16_t ICON_DY = 6;
				const Point<int16_t> ipos(
					cur.x() + static_cast<int16_t>(ivorg.x() * sc) - iw / 2 + ICON_SZ / 2,
					cur.y() + LINE_H / 2 + ICON_DY + static_cast<int16_t>(ivorg.y() * sc) - ih / 2);
				line.icon.draw(DrawArgument(ipos, sc, sc));
			}
			else if (line.multi_weapon)
			{
				// OR 组多选武器：参考图用「X」标记，画在图标槽中心，与名称同行。
				Text xmark(Text::Font::A11M, Text::Alignment::CENTER,
					Color::Name::WHITE, "X", ICON_SZ);
				// 与真图标同口径下移半字高，避免 X 比名称偏上。
				xmark.draw(Point<int16_t>(cur.x() + ICON_SZ / 2,
					cur.y() + LINE_H / 2 + 2));
			}

			// 2026-11-21：属性行(line.indent>0)在统一图标槽偏移之外再右移一个中文字，
			// 与「N套装效果」标题拉开层级（前导空格会被 Text 布局吞掉，故用像素偏移）。
			const int16_t line_x = ICON_TEXT_X + line.indent;
			line.left.draw(cur + Point<int16_t>(line_x, 0));

			// 属性「值」对齐到固定列：标签 + 固定宽 + 值，所有行数值成一列。
			if (line.split_value)
				line.value.draw(cur + Point<int16_t>(line_x + ATTR_LABEL_W, 0));

			if (line.has_right)
				line.right.draw(cur + Point<int16_t>(FRAME_WIDTH - PAD_X * 2, 0));

			cur.shift_y(LINE_H);
		}
	}
}
