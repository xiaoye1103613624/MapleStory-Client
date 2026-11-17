//////////////////////////////////////////////////////////////////////////////////
//	SetTooltip — 套装 companion 面板（2026-11-12 装备槽位扩展 ⑤ · 展示层）
//////////////////////////////////////////////////////////////////////////////////
#include "SetTooltip.h"

#include "../../Data/SetItemData.h"
#include "../../Graphics/Geometry.h"
#include "../../Util/Locale.h"

#include <algorithm>

namespace ms
{
	namespace
	{
		// 与装备 tip 同一套字体/配色（EquipTooltip.cpp:91/487 实测范式）：
		// 标题用 A12B 加粗白字，档位行用 A11M 正文色。
		constexpr int16_t PAD_X = 10;
		constexpr int16_t PAD_TOP = 8;
		constexpr int16_t LINE_H = 14;
		constexpr int16_t TITLE_H = 18;
		constexpr int16_t SEP_H = 6;
		constexpr int16_t PAD_BOTTOM = 8;

		// 档位行文案：`3件套：STR +5`（多个词条同行用 `、` 连接）。
		std::string stat_label(const std::string& key)
		{
			// 与 Data/SetItem.txt 的 statKey 词表一一对应。
			if (key == "STR") return "STR";
			if (key == "DEX") return "DEX";
			if (key == "INT") return "INT";
			if (key == "LUK") return "LUK";
			if (key == "HP") return "HP";
			if (key == "MP") return "MP";
			if (key == "WATK") return "攻击力";
			if (key == "MAGIC") return "魔法力";
			if (key == "WDEF") return "防御力";
			if (key == "MDEF") return "魔法防御";
			if (key == "ACC") return "命中";
			if (key == "AVOID") return "回避";
			if (key == "HANDS") return "灵巧";
			if (key == "SPEED") return "移动速度";
			if (key == "JUMP") return "跳跃力";
			if (key == "ALLSTAT") return "全属性";
			if (key == "DAMAGE") return "伤害";
			if (key == "BOSS") return "首领怪伤害";
			if (key == "IGNOREDEF") return "无视防御";
			return key;
		}

		// 百分比类词条带 %，其余纯数值。
		bool is_percent(const std::string& key)
		{
			return key == "DAMAGE" || key == "BOSS" || key == "IGNOREDEF";
		}
	}

	void SetTooltip::set_item(int32_t item_id, const std::vector<int32_t>& equipped_item_ids)
	{
		is_valid = false;
		tier_lines.clear();
		panel_height = 0;

		int32_t set_id = SetItemData::set_of(item_id);

		if (set_id <= 0)
			return;

		const SetItemData::Definition* def = SetItemData::get(set_id);

		if (!def)
			return;

		int16_t equipped = SetItemData::count_equipped(set_id, equipped_item_ids);

		// 标题：`套装名 (已穿/需求)`，让玩家一眼看出还差几件。
		title = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE,
			def->name + " (" + std::to_string(equipped) + "/"
			+ std::to_string(def->complete_count) + ")", FRAME_WIDTH - PAD_X * 2);

		// 档位逐行：同一 count 的多个词条合并成一行，避免档位多时面板过高。
		std::vector<int16_t> counts;

		for (const SetItemData::Tier& t : def->tiers)
		{
			if (std::find(counts.begin(), counts.end(), t.count) == counts.end())
				counts.push_back(t.count);
		}

		std::sort(counts.begin(), counts.end());

		for (int16_t c : counts)
		{
			std::string joined;

			for (const SetItemData::Tier& t : def->tiers)
			{
				if (t.count != c)
					continue;

				if (!joined.empty())
					joined += "、";

				joined += stat_label(t.stat_key) + " +" + std::to_string(t.value);

				if (is_percent(t.stat_key))
					joined += "%";
			}

			Line line;
			bool reached = equipped >= c;

			// 已达成的档位用高亮色（与原生「已生效」观感一致），未达成用正文色。
			line.active = reached;
			line.text = Text(Text::Font::A11M, Text::Alignment::LEFT,
				reached ? Color::Name::YELLOW : Color::Name::TOOLTIPBODY,
				std::to_string(c) + "件套：" + joined, FRAME_WIDTH - PAD_X * 2);

			tier_lines.push_back(std::move(line));
		}

		// 2026-11-18 服务端权威：若收到服务端文案（0x176），用它覆盖本地构建。
		// 服务端文本首行 = 标题「套装名 (n/N)」，其后每行 = 一条属性，按 \r\n 拆分。
		if (SetItemData::has_server_text(set_id))
		{
			tier_lines.clear();
			const std::string server_text = SetItemData::get_server_text(set_id);
			std::vector<std::string> rows;
			size_t start = 0;
			while (start <= server_text.size())
			{
				size_t nl = server_text.find_first_of("\r\n", start);
				std::string row = server_text.substr(start, nl == std::string::npos ? std::string::npos : nl - start);
				if (!row.empty())
					rows.push_back(row);
				if (nl == std::string::npos)
					break;
				start = nl + 1;
				while (start < server_text.size() && (server_text[start] == '\r' || server_text[start] == '\n'))
					++start;
			}

			if (!rows.empty())
			{
				// 首行作为标题（服务端已带 套装名(n/N)，不再本地拼）
				title = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE,
					rows[0], FRAME_WIDTH - PAD_X * 2);
				// 其余行作为档位/属性行
				for (size_t r = 1; r < rows.size(); ++r)
				{
					Line line;
					line.active = true;
					line.text = Text(Text::Font::A11M, Text::Alignment::LEFT,
						Color::Name::YELLOW, rows[r], FRAME_WIDTH - PAD_X * 2);
					tier_lines.push_back(std::move(line));
				}
			}
		}

		panel_height = static_cast<int16_t>(
			PAD_TOP + TITLE_H + SEP_H
			+ static_cast<int16_t>(tier_lines.size()) * LINE_H
			+ PAD_BOTTOM);

		is_valid = true;
	}

	void SetTooltip::draw(Point<int16_t> pos) const
	{
		if (!is_valid)
			return;

		backdrop.draw(pos + Point<int16_t>(-6, -6), FRAME_WIDTH + 12, panel_height + 12);

		Point<int16_t> cur = pos + Point<int16_t>(PAD_X, PAD_TOP);

		title.draw(cur);
		cur.shift_y(TITLE_H);

		separator.draw(cur - Point<int16_t>(PAD_X, 0));
		cur.shift_y(SEP_H);

		for (const Line& line : tier_lines)
		{
			line.text.draw(cur);
			cur.shift_y(LINE_H);
		}
	}
}
