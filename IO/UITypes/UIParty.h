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
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

#include "../../Graphics/SpecialText.h"

#include <array>
#include <map>
#include <string>

namespace ms
{
	struct PartyMember
	{
		int32_t id;
		std::string name;
		int32_t job;
		int32_t level;
		int32_t channel;
		int32_t mapid;
		bool online;
	};

	struct Party
	{
		int32_t id;
		int32_t leader;
		std::array<PartyMember, 6> members;
	};

	class UIParty : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::PARTY;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UIParty();

		void draw(float alpha) const override;
		void update() override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void update_party(const Party& p);
		void set_leader(int32_t cid);
		void update_member_hp(int32_t cid, int32_t curhp, int32_t maxhp);

		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t
		{
			BtInvite,
			BtLeave,
			BtClose,
			BtLeader,
			BtExpel
		};

		void prompt_invite();
		void prompt_leader();
		void prompt_expel();
		int32_t member_at_row(int16_t row) const;

		Party party;
		std::map<int32_t, std::pair<int32_t, int32_t>> memberhp;
		int32_t selected_cid = 0;
		mutable OutlinedText title;
	};
}
