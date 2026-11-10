//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																					//
//	This program is free software: you can redistribute it and/or modify			//
//	under the terms of the GNU Affero General Public License as published by		//
//	the Free Software Foundation, either version 3 of the License, or				//
//	(at your option) any later version.											//
//																					//
//	This program is distributed in the hope that it will be useful				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the					//
//	GNU Affero General Public License for more details.							//
//																					//
//	You should have received a copy of the GNU Affero General Public License		//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

#include "../../Graphics/Color.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include <cstddef>
#include <cstdint>

namespace ms
{
	class UIWorldSelect;

	// 原生「查看推荐世界」弹窗（Angel 083 CUIRecommendWorld）。
	// 布局依据：CUIRecommendWorld::OnCreate @0x62b451 —— 背景
	// Login.img/WorldSelect/alert/backgrd（540x167），三颗真位图按钮
	// BtArrowL id=1000 (45,70) / BtArrowR id=1001 (215,70) / BtChoice id=1002 (113,114)，
	// 消息区矩形 {78,61,212,110} 点击直接 SelectWorld（OnMouseButton @0x62bef3）。
	// OnButtonClicked @0x62b708：1000 → MoveWorld(-1)，1001 → MoveWorld(+1)，
	// 1002 → SelectWorld；ResetWorldAndMessage @0x62b9da 世界数 >1 才启用两箭头。
	//
	// 校准≠改回原生：数据源沿用当前端 SERVERLIST 世界列表 + change_world(World)，
	// 不动 s9 协议、不动 RecommendedWorldMessageHandler；仅按 083 重修 UI 布局。
	class UIRecommendWorld : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::RECOMMENDWORLD;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UIRecommendWorld(UIWorldSelect* worldselect);

		void draw(float alpha) const override;

		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		UIElement::Type get_type() const override;

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Ids : uint16_t
		{
			BT_MOVE_LEFT = 1000,
			BT_MOVE_RIGHT = 1001,
			BT_SELECT = 1002
		};

		void refresh();
		void move_world(int delta);
		void select_world();

		UIWorldSelect* worldselect;
		size_t current_index;
		bool has_worlds;

		Texture world_name;
		Text name_text;
		Text message_text;

		Point<int16_t> name_pos;
		Point<int16_t> message_pos;
	};
}
