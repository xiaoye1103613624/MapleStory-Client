////////////////////////////////////////////////////////////////////////////////
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
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.	//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

#include "../../Data/ItemData.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

#include <string>
#include <vector>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// 制造/工匠窗口：从 Maker.img 列出配方（可滚动），点击配方行发送创建请求。
	class UIMaker : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::MAKER;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIMaker();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Button::State button_pressed(uint16_t buttonid) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_scroll(double yoffset) override;
		UIElement::Type get_type() const override;

		void set_status(const std::string& text);

	private:
		struct Recipe
		{
			int32_t itemId;  // 结果道具；亦作为发给服务端的 toCreate
			int32_t meso;    // 消耗金币
			int32_t reqLevel;// 所需角色等级
		};

		void load_recipes();

		enum Buttons : uint16_t
		{
			BtClose
		};

		Sprite background;
		bool has_background = false;
		Text title;
		Text status;
		std::vector<Recipe> recipes;
		std::vector<Texture> icons;
		std::vector<Text> names;
		std::vector<Text> costs;
		int16_t scroll = 0;
		int16_t row_h = 56;
		Point<int16_t> list_origin;
		int32_t selected_item_id = 0;
	};
}
