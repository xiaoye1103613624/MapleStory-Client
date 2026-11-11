////////////////////////////////////////////////////////////////////////////////
//	W5 TemporaryStatView — 原生节点仅 32×32 图标；明细列表本地绘制（宋体/原色）
////////////////////////////////////////////////////////////////////////////////
#include "UITemporaryStatView.h"

#include "../UI.h"
#include "../../Character/Buff.h"
#include "../../Character/Player.h"
#include "../../Constants.h"
#include "../../Data/SkillData.h"
#include "../../Data/ItemData.h"
#include "../../Gameplay/Stage.h"
#include "../../Graphics/Geometry.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

#include <cstdio>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#include "../../Util/Locale.h"
#endif

namespace ms
{
	UITemporaryStatView::UITemporaryStatView()
		: title_label(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE, Locale::text("uitemporarystatview.26"))
	{
		position = Point<int16_t>(16, 80);
		dimension = Point<int16_t>(220, 200);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("TemporaryStatView");
		if (root && root["1"])
		{
			back_icon = Texture(root["1"]);
			has_assets = true;
		}
#endif
		rebuild_rows();
	}

	void UITemporaryStatView::rebuild_rows()
	{
		rows.clear();
		line_labels.clear();

		const Player& pl = Stage::get().get_player();
		// 按 skillid 聚合：同技能多掩码合一行
		std::map<int32_t, Row> by_skill;

		for (size_t i = 1; i < static_cast<size_t>(Buffstat::Id::LENGTH); ++i)
		{
			auto st = static_cast<Buffstat::Id>(i);
			if (!pl.has_buff(st))
				continue;
			int32_t sid = pl.get_buff_skillid(st);
			if (sid == 0)
				sid = -static_cast<int32_t>(i); // 无技能 id 时用负 stat 作键
			int16_t val = pl.get_buff_value(st);
			int32_t dur = pl.get_buff_duration(st);

			auto& row = by_skill[sid];
			row.skillid = sid;
			if (val > row.value)
				row.value = val;
			if (dur > row.remain_ms)
				row.remain_ms = dur;
		}

		for (auto& kv : by_skill)
		{
			Row& row = kv.second;
			if (remain_by_skill.count(row.skillid))
			{
				// 已有倒计时：取较小（新包可能刷新）
				if (row.remain_ms > 0 && row.remain_ms < remain_by_skill[row.skillid])
					remain_by_skill[row.skillid] = row.remain_ms;
				else if (row.remain_ms > remain_by_skill[row.skillid] + 500)
					remain_by_skill[row.skillid] = row.remain_ms; // 刷新延长
				row.remain_ms = remain_by_skill[row.skillid];
			}
			else if (row.remain_ms > 0)
			{
				remain_by_skill[row.skillid] = row.remain_ms;
			}

			if (row.skillid > 0)
			{
				if (row.skillid >= 1000000)
					row.title = ItemData::get(row.skillid).get_name();
				else
					row.title = SkillData::get(row.skillid).get_name();
				if (row.title.empty())
					row.title = "#" + std::to_string(row.skillid);
			}
			else
			{
				row.title = Locale::text("uitemporarystatview.98") + std::to_string(-row.skillid);
			}

			char buf[96];
			int sec = row.remain_ms > 0 ? (row.remain_ms + 999) / 1000 : 0;
			if (sec > 0)
				std::snprintf(buf, sizeof(buf), "%s  x%d  %ds", row.title.c_str(), (int)row.value, sec);
			else
				std::snprintf(buf, sizeof(buf), "%s  x%d", row.title.c_str(), (int)row.value);

			rows.push_back(row);
			// A11M = 宋体 11；WHITE 对齐原生明细字色
			line_labels.emplace_back(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, buf);
		}

		int16_t h = static_cast<int16_t>(28 + static_cast<int>(rows.size()) * 16);
		if (h < 64) h = 64;
		if (h > 280) h = 280;
		dimension = Point<int16_t>(220, h);
	}

	void UITemporaryStatView::draw(float) const
	{
		// 无 buff 时不渲染（避免一块空的半透明黑板叠在场景/小地图上）
		if (rows.empty())
			return;

		// 原生仅 32×32 图标；明细区用半透明黑底（α≈0.55）托住宋体字
		ColorBox panel(dimension.x(), dimension.y(), Color::Name::BLACK, 0.55f);
		panel.draw(position);

		if (has_assets && back_icon)
			back_icon.draw(position + Point<int16_t>(4, 4));

		title_label.draw(position + Point<int16_t>(has_assets ? 40 : 8, 4));
		int16_t y = 22;
		for (const auto& lab : line_labels)
		{
			lab.draw(position + Point<int16_t>(8, y));
			y = static_cast<int16_t>(y + 16);
			if (y > dimension.y() - 8)
				break;
		}
	}

	void UITemporaryStatView::update()
	{
		for (auto& kv : remain_by_skill)
		{
			if (kv.second > 0)
			{
				kv.second -= Constants::TIMESTEP;
				if (kv.second < 0)
					kv.second = 0;
			}
		}
		// 清掉已无 buff 的计时
		const Player& pl = Stage::get().get_player();
		for (auto it = remain_by_skill.begin(); it != remain_by_skill.end();)
		{
			bool still = false;
			for (size_t i = 1; i < static_cast<size_t>(Buffstat::Id::LENGTH); ++i)
			{
				auto st = static_cast<Buffstat::Id>(i);
				if (pl.has_buff(st) && pl.get_buff_skillid(st) == it->first)
				{
					still = true;
					break;
				}
			}
			if (!still)
				it = remain_by_skill.erase(it);
			else
				++it;
		}
		rebuild_rows();
	}

	void UITemporaryStatView::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape)
		{
			deactivate();
			return;
		}
		if (!pressed) return;
		if (keycode == 'X' || keycode == 'x')
			deactivate();
	}

	UIElement::Type UITemporaryStatView::get_type() const
	{
		return TYPE;
	}
}
