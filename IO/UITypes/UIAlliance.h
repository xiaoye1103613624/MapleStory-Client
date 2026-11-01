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

#include "../../Graphics/Text.h"

#include <string>
#include <vector>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// 联盟窗口：从 UIGuild 的 BtAlliance 打开，展示联盟名/公告/成员公会列表，
	// 数据来自服务端 ALLIANCE_OPERATION(66) 下发的 getAllianceInfo(0x0C)/allianceNotice(0x1C)。
	class UIAlliance : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::ALLIANCE;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIAlliance();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Button::State button_pressed(uint16_t buttonid) override;
		UIElement::Type get_type() const override;

		void set_alliance(int32_t id, const std::string& name, const std::vector<std::string>& ranks,
			const std::vector<int32_t>& guilds, const std::string& notice);
		void set_alliance(int32_t id, const std::string& name, const std::vector<std::string>& ranks,
			const std::vector<int32_t>& guilds, const std::vector<std::string>& guild_names,
			const std::string& notice);
		void set_notice(const std::string& notice);
		const std::string& get_notice() const { return alliance_notice; }

	private:
		void refresh_guild_label();

		enum Buttons : uint16_t
		{
			BtClose,
			BtRefresh,
			BtLeave,
			BtInvite,
			BtNotice
		};

		Sprite background;
		bool has_background = false;
		Text title;
		Text name_label;
		Text notice_label;
		Text guild_label;
		int32_t alliance_id = 0;
		std::string alliance_name;
		std::string alliance_notice;
		std::vector<int32_t> guilds;
		std::vector<std::string> guild_names;
	};
}
