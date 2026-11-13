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
#pragma once

#include "UIElement.h"

#include "../Configuration.h"
#include "../Constants.h"
#include "../Util/HudLayout.h"

namespace ms
{
	template <typename T>
	// Base class for UI Windows which can be moved with the mouse cursor.
	class UIDragElement : public UIElement
	{
	public:
		void remove_cursor() override
		{
			UIElement::remove_cursor();

			if (dragged)
			{
				dragged = false;
				clamp_drag_position();
				Setting<T>::get().save(position);
			}
		}

		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override
		{
			if (clicked)
			{
				if (dragged)
				{
					position = cursorpos - cursoroffset;
					clamp_drag_position();

					return Cursor::State::CLICKING;
				}
				else if (indragrange(cursorpos))
				{
					cursoroffset = cursorpos - position;
					dragged = true;

					return UIElement::send_cursor(clicked, cursorpos);
				}
			}
			else
			{
				if (dragged)
				{
					dragged = false;
					clamp_drag_position();
					Setting<T>::get().save(position);
				}
			}

			return UIElement::send_cursor(clicked, cursorpos);
		}

	protected:
		UIDragElement() : UIDragElement(Point<int16_t>(0, 0)) {}

		UIDragElement(Point<int16_t> d) : dragarea(d)
		{
			// 可拖窗：分辨率变化时夹回视口，拖曳中亦不允许出客户区。
			set_layout_policy(LayoutPolicy::SavedPosClamped);
			position = Setting<T>::get().load();
			clamp_drag_position();
		}

		void clamp_drag_position()
		{
			const int16_t vw = Constants::Constants::get().get_viewwidth();
			const int16_t vh = Constants::Constants::get().get_viewheight();
			Point<int16_t> dim = dimension;

			// ctor 早期 dimension 可能仍为 0；用 dragarea 兜底避免误夹到 (0,0)。
			if (dim.x() <= 0)
				dim = Point<int16_t>(dragarea.x() > 0 ? dragarea.x() : static_cast<int16_t>(1), dim.y());
			if (dim.y() <= 0)
				dim = Point<int16_t>(dim.x(), dragarea.y() > 0 ? dragarea.y() : static_cast<int16_t>(1));

			position = HudLayout::clamp_to_view(position, dim, vw, vh);
		}

		bool dragged = false;
		Point<int16_t> dragarea;
		Point<int16_t> cursoroffset;

	private:
		virtual bool indragrange(Point<int16_t> cursorpos) const
		{
			auto bounds = Rectangle<int16_t>(position, position + dragarea);

			return bounds.contains(cursorpos);
		}
	};
}