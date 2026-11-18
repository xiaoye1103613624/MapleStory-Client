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
#include "UIStatusMessenger.h"

#include "UISoftKey.h"

#include "../UI.h"
#include "../../Constants.h"
#include "../../Data/ItemData.h"
#include "../../Util/HudLayout.h"

namespace ms
{
	StatusInfo::StatusInfo(const std::string& str, Color::Name color, int32_t id)
		: itemid(id)
	{
		text = Text(Text::Font::A12M, Text::Alignment::RIGHT, color, str);
		shadow = Text(Text::Font::A12M, Text::Alignment::RIGHT, Color::Name::BLACK, str);

		opacity.set(1.0f);
	}

	void StatusInfo::draw(Point<int16_t> position, float alpha) const
	{
		float interopc = opacity.get(alpha);

		// 道具飘字左侧小图标：仅道具获得/丢失消息（itemid>0）绘制。纹理来自
		// ItemData 进程级缓存，缩放到约 21px（用户要求的 1.5 倍），与文字同一透明度淡出。
		if (itemid > 0)
		{
			const ItemData& idata = ItemData::get(itemid);
			if (idata.is_valid())
			{
				const Texture& icon = idata.get_icon(false);
				if (icon)
				{
					constexpr int16_t ICON_PX = 21;      // 14 * 1.5：按用户实机反馈放大
					constexpr int16_t ICON_GAP = 3;      // 图标与文字左缘间距
					constexpr int16_t INK_CENTER = 9;    // A12M 文字墨迹中心相对 position.y() 的下移量
					const float scale = static_cast<float>(ICON_PX) / icon.height();

					// 文字右对齐：右缘在 position.x()，左缘 = 右缘 - text.width()。
					// 图标视觉矩形贴文字左缘；垂直按墨迹中心对齐（原贴 position.y() 偏上约 2px）。
					const int16_t iconw = static_cast<int16_t>(icon.width() * scale);
					const int16_t icon_left = position.x() - text.width() - ICON_GAP - iconw;
					const int16_t icon_top = static_cast<int16_t>(position.y() + INK_CENTER - ICON_PX / 2);
					// Texture::draw 视觉左上角 = pos - scale * origin，故传入 pos 需补回 origin 偏移。
					const Point<int16_t> iconpos(
						static_cast<int16_t>(icon_left + scale * icon.get_origin().x()),
						static_cast<int16_t>(icon_top + scale * icon.get_origin().y()));
					icon.draw(DrawArgument(iconpos, scale, scale, interopc));
				}
			}
		}

		shadow.draw(DrawArgument(position + Point<int16_t>(1, 1), interopc));
		text.draw(DrawArgument(position, interopc));
	}

	bool StatusInfo::update()
	{
		constexpr float FADE_STEP = Constants::TIMESTEP * 1.0f / FADE_DURATION;
		opacity -= FADE_STEP;

		return opacity.last() < FADE_STEP;
	}

	UIStatusMessenger::UIStatusMessenger()
	{
		// Edge 右下：右对齐状态飘字锚点（文字 Alignment::RIGHT）。
		set_layout_edge(LayoutEdge::BottomRight, Point<int16_t>(6, 145));

		int16_t height = Constants::Constants::get().get_viewheight();
		int16_t width = Constants::Constants::get().get_viewwidth();

		update_screen(width, height);
	}

	void UIStatusMessenger::draw(float inter) const
	{
		Point<int16_t> infopos = Point<int16_t>(position.x(), position.y());

		for (const StatusInfo& info : statusinfos)
		{
			info.draw(infopos, inter);
			infopos.shift_y(-14);
		}
	}

	void UIStatusMessenger::update()
	{
		for (StatusInfo& info : statusinfos)
			info.update();
	}

	void UIStatusMessenger::update_screen(int16_t new_width, int16_t new_height)
	{
		// 宽屏底栏更高时上抬 37px，避免压住 classic HUD；公式不跟档位 id。
		const int16_t y_adj = (new_width > HudLayout::HUD_DESIGN_W) ? 37 : 0;
		position = Point<int16_t>(
			HudLayout::edge_right(new_width, 0, layout_margin.x()),
			static_cast<int16_t>(
				HudLayout::edge_bottom(new_height, 0, layout_margin.y()) + y_adj));
	}

	UIElement::Type UIStatusMessenger::get_type() const
	{
		return TYPE;
	}

	void UIStatusMessenger::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (escape) { statusinfos.clear(); return; }
		if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't')
		{
			
		}
		else if (keycode == 'C' || keycode == 'c' || keycode == 'X' || keycode == 'x')
		{
			statusinfos.clear();
			LOG(LOG_DEBUG, "UIStatusMessenger: clear");
		}
		else if (keycode == 'R' || keycode == 'r')
		{
			
		}
		else if (keycode == 'W' || keycode == 'w')
		{
			
		}
	}

	void UIStatusMessenger::show_status(Color::Name color, const std::string& message, int32_t itemid)
	{
		statusinfos.push_front(StatusInfo(message, color, itemid));

		if (statusinfos.size() > MAX_MESSAGES)
			statusinfos.pop_back();
	}
}