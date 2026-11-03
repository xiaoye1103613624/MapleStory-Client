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

#include "../UIDragElement.h"

namespace ms
{
	class UIChannel : public UIDragElement<PosCHANNEL>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::CHANNEL;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIChannel();

		void draw(float inter) const override;
		void update() override;

		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;

		UIElement::Type get_type() const override;

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t
		{
			CANCEL,
			CHANGE,
			CH
		};

		void cancel();
		void change_channel();
		void step_channel(uint8_t& target, int delta);

		// 槽位落位全部由 index 现算，常数来自 evidence/w7-option-channel/channel-layout.md §5
		// （原生 频道变更.png 逐格量出，channel0 在三个空槽上 SAD=0.00）。
		static constexpr uint8_t SLOTS_PER_ROW = 6;
		static constexpr int16_t GRID_TOP = 67;
		static constexpr int16_t SLOT_PITCH_X = 70;
		static constexpr int16_t SLOT_PITCH_Y = 20;
		static constexpr int16_t SLOT_LABEL_DX = 8;
		static constexpr int16_t SLOT_LABEL_DY = 5;

		static int16_t slot_x(uint8_t i);
		static int16_t slot_y(uint8_t i);
		size_t slot_total() const;
		int16_t bottom_strip_y() const;

		uint8_t current_channel;
		uint8_t selected_channel;
		uint8_t channel_count;
		// 三块底板：空槽 / 待变更 / 当前所在（蓝色带 ▶）。
		// 旧实现用 BoolPair 只装得下两块，把当前频道画成了浅灰，`channel2` 一次也没被用上。
		Texture plate_empty;
		Texture plate_selected;
		Texture plate_current;
		std::vector<Sprite> ch;
	};
}