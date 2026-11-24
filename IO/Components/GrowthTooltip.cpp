//////////////////////////////////////////////////////////////////////////////////
#include "GrowthTooltip.h"

#include "../../Data/EquipGrowthData.h"
#include "../../Net/Packets/SetItemPackets.h"

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
	}

	void GrowthTooltip::set_item(int32_t item_id, int32_t item_level, int32_t scroll_level)
	{
		is_valid = false;
		lines.clear();
		panel_height = 0;

		if (item_id <= 0)
			return;

		EquipGrowthData::TipEntry entry;

		if (!EquipGrowthData::try_get(item_id, entry)
			|| entry.item_level != item_level
			|| entry.scroll != scroll_level
			|| !entry.resolved
			|| entry.text.empty())
		{
			EquipGrowthData::TipEntry local;

			if (!EquipGrowthData::build_local(item_id, item_level, scroll_level, local)
				|| local.text.empty())
			{
				if (EquipGrowthData::begin_request(item_id))
					EquipGrowthTipRequestPacket(item_id).dispatch();

				return;
			}

			entry = local;

			// 本地已有段落时仍可按需拉一次服务端色条（最多一次）
			if (!entry.from_server && EquipGrowthData::begin_request(item_id))
				EquipGrowthTipRequestPacket(item_id).dispatch();
		}

		std::vector<std::string> rows;
		size_t start = 0;
		const std::string& body = entry.text;

		while (start <= body.size())
		{
			size_t nl = body.find_first_of("\r\n", start);
			std::string row = body.substr(start, nl == std::string::npos ? std::string::npos : nl - start);

			if (!row.empty())
				rows.push_back(row);

			if (nl == std::string::npos)
				break;

			start = nl + 1;

			while (start < body.size() && (body[start] == '\r' || body[start] == '\n'))
				++start;
		}

		if (rows.empty())
			return;

		title = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE,
			rows[0], FRAME_WIDTH - PAD_X * 2);

		for (size_t i = 1; i < rows.size(); ++i)
		{
			const bool header = rows[i].find("级效果") != std::string::npos;
			Line line;
			line.header = header;
			line.text = Text(Text::Font::A11M, Text::Alignment::LEFT,
				header ? Color::Name::YELLOW : Color::Name::TOOLTIPBODY,
				rows[i], FRAME_WIDTH - PAD_X * 2);
			lines.push_back(std::move(line));
		}

		panel_height = static_cast<int16_t>(
			PAD_TOP + TITLE_H + SEP_H
			+ static_cast<int16_t>(lines.size()) * LINE_H
			+ PAD_BOTTOM);
		is_valid = true;
	}

	void GrowthTooltip::draw(Point<int16_t> pos) const
	{
		if (!is_valid)
			return;

		backdrop.draw(pos + Point<int16_t>(-6, -6), FRAME_WIDTH + 12, panel_height + 12);

		Point<int16_t> cur = pos + Point<int16_t>(PAD_X, PAD_TOP);
		title.draw(cur);
		cur.shift_y(TITLE_H);
		separator.draw(cur - Point<int16_t>(PAD_X, 0));
		cur.shift_y(SEP_H);

		for (const Line& line : lines)
		{
			line.text.draw(cur);
			cur.shift_y(LINE_H);
		}
	}
}
