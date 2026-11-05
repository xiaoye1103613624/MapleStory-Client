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

#include <string>
#include <vector>

namespace ms
{
	struct GuildMember
	{
		int32_t id;
		std::string name;
		int32_t job;
		int32_t level;
		int32_t rank;
		bool online;
		int32_t allianceRank;
	};

	struct GuildData
	{
		int32_t id;
		std::string name;
		std::string rankTitles[5];
		std::vector<GuildMember> members;
		int32_t capacity;
		int32_t gp;
		int32_t allianceId;
		std::string notice;
	};

	class UIGuild : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::GUILD;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UIGuild();

		void draw(float alpha) const override;
		void update() override;
		void doubleclick(Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void set_guild(const GuildData& g);
		void clear_guild();
		void add_member(const GuildMember& m);
		void remove_member(int32_t id);
		void set_member_rank(int32_t id, int32_t rank);
		void set_member_online(int32_t id, bool online);
		void set_notice(const std::string& notice);
		// SoftKey BBS 列表/帖摘要（与公会公告 notice 分栏）
		void set_bbs_summary(const std::vector<std::string>& lines);

		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t
		{
			BtCreate,
			BtInvite,
			BtNotice,
			BtClose,
			BtAlliance
		};

		static constexpr int16_t ROW_H = 18;

		int16_t row_by_position(Point<int16_t> pos) const;
		void prompt_create();
		void prompt_invite();
		void prompt_notice();
		void prompt_deny();
		const GuildMember* selected_member() const;

		GuildData guild;
		int32_t selected_cid = 0;
		mutable OutlinedText title;
		mutable OutlinedText notice_label;
		std::vector<std::string> bbs_lines;
		mutable OutlinedText bbs_label;
	};
}
