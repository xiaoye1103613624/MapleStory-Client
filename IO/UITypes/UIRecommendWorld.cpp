//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client						//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																						//
//	This program is free software: you can redistribute it and/or modify			//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or				//
//	(at your option) any later version.												//
//																						//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of					//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the					//
//	GNU Affero General Public License for more details.							//
//																						//
//	You should have received a copy of the GNU Affero General Public License		//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.			//
//////////////////////////////////////////////////////////////////////////////////
#include "UIRecommendWorld.h"

#include "UIWorldSelect.h"
#include "../UI.h"

#include "../Components/MapleButton.h"

#include "../../Graphics/DrawArgument.h"
#include "../../Graphics/Sprite.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include <nlnx/nx.hpp>

#include <cstdint>
#include <string>

namespace ms
{
	// Angel 083 CUIRecommendWorld::OnCreate @0x62b451 的布局立即数。
	// 背景 Login.img/WorldSelect/alert/backgrd（.nx 实测 bitmap 540x167，
	// origin (0,0)），三颗真位图按钮 id/坐标逐字取自反编译：
	//   SP_5363 ..._ALERT_BTARROWL -> CCtrlButton id=1000 (45,70)
	//   SP_5364 ..._ALERT_BTARROWR -> CCtrlButton id=1001 (215,70)
	//   SP_5365 ..._ALERT_BTCHOICE -> CCtrlButton id=1002 (113,114)
	// 资产并非缺失：.nx 下 Login.img/WorldSelect/alert 的真实子节点是
	// ['BtArrowL','BtArrowR','BtChoice','BtClose','backgrd']（小写 alert/backgrd）。
	// 旧实现把该节点的大小写与拼写写错后误判「R6 裁剪」，改拿 chBackgrn 当背景、
	// 用 Text 手绘「推荐世界」与 < >，属于按 BeiDou/自行揣测的布局，全部作废。
	constexpr int16_t RW_BACKGRD_WIDTH = 540;
	constexpr int16_t RW_BACKGRD_HEIGHT = 167;

	UIRecommendWorld::UIRecommendWorld(UIWorldSelect* worldselect)
		: UIElement(),
		  worldselect(worldselect),
		  current_index(0),
		  has_worlds(false),
		  world_name(),
		  // 083 ResetWorldAndMessage @0x62b9da：IWzFont::CalcLongestText(..., 200, ...)
		  // 换行宽度 200；name_text 仅在世界名牌图缺失时作文字回退。
		  name_text(Text::Font::A13B, Text::Alignment::CENTER, Color::Name::BLACK, "", 200),
		  message_text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "", 200),
		  name_pos(45, 116),
		  message_pos(283, 68)
	{
		nl::node alert = nl::nx::UI["Login.img"]["WorldSelect"]["alert"];

		// 真背景交给基类 sprites（UIElement::draw 自动加 position）。
		if (alert["backgrd"])
			sprites.emplace_back(alert["backgrd"]);

		dimension = Point<int16_t>(RW_BACKGRD_WIDTH, RW_BACKGRD_HEIGHT);

		// 三颗真位图按钮：try_maple_btn 的 position 是 sprite 左上角，
		// 内部会加回 normal 态 origin，故 083 立即数可直接落。
		try_maple_btn(buttons, BT_MOVE_LEFT, alert["BtArrowL"], Point<int16_t>(45, 70), "UIRecommendWorld");
		try_maple_btn(buttons, BT_MOVE_RIGHT, alert["BtArrowR"], Point<int16_t>(215, 70), "UIRecommendWorld");
		try_maple_btn(buttons, BT_SELECT, alert["BtChoice"], Point<int16_t>(113, 114), "UIRecommendWorld");

		uint16_t wc = worldselect ? worldselect->get_world_count() : 0;
		has_worlds = wc > 0;
		if (has_worlds)
		{
			uint8_t preselected = worldselect->get_worldid();
			current_index = preselected < wc ? preselected : 0;
		}

		refresh();

		center_on_screen();
	}

	void UIRecommendWorld::draw(float alpha) const
	{
		// 基类画真背景 sprites + 三颗真按钮 buttons。
		UIElement::draw(alpha);

		// 083 ResetWorldAndMessage：SP_1366 WORLD_D 取世界名牌/描述图，
		// 置于左浅色面板（backgrd.png 左面板约 x23..267、y58..145）水平居中。
		if (world_name)
		{
			Point<int16_t> dim = world_name.get_dimensions();
			Point<int16_t> lt(
				static_cast<int16_t>(position.x() + 145 - dim.x() / 2),
				static_cast<int16_t>(position.y() + 72));
			// Texture::draw 会扣 origin；按左上角 blit 需先加回 origin。
			world_name.draw(DrawArgument(lt + world_name.get_origin()));
		}

		// 名牌图缺失时的文字回退；message 落在右浅色面板（约 x275..519）。
		if (!name_text.empty())
			name_text.draw(DrawArgument(position + name_pos));

		if (!message_text.empty())
			message_text.draw(DrawArgument(position + message_pos));
	}

	Cursor::State UIRecommendWorld::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		// 083 OnMouseButton @0x62bef3：v7[0..3] = {78, 61, 212, 110}，
		// 命中该消息区且 msg==513(WM_LBUTTONDOWN) 时直接 SelectWorld。
		// 该立即数是窗口相对坐标，本窗居中后需叠加 position。
		Rectangle<int16_t> message_rect(
			Point<int16_t>(static_cast<int16_t>(position.x() + 78), static_cast<int16_t>(position.y() + 61)),
			Point<int16_t>(static_cast<int16_t>(position.x() + 212), static_cast<int16_t>(position.y() + 110)));

		if (clicked && message_rect.contains(cursorpos))
		{
			select_world();
			return Cursor::State::IDLE;
		}

		return UIElement::send_cursor(clicked, cursorpos);
	}

	UIElement::Type UIRecommendWorld::get_type() const
	{
		return TYPE;
	}

	Button::State UIRecommendWorld::button_pressed(uint16_t buttonid)
	{
		// 083 OnButtonClicked @0x62b708（IsRequestValid 守卫后）：
		// 1000 -> MoveWorld(-1)，1001 -> MoveWorld(+1)，1002 -> SelectWorld。
		switch (buttonid)
		{
			case BT_MOVE_LEFT:
				move_world(-1);
				break;
			case BT_MOVE_RIGHT:
				move_world(1);
				break;
			case BT_SELECT:
				select_world();
				break;
			default:
				break;
		}

		return Button::State::NORMAL;
	}

	void UIRecommendWorld::refresh()
	{
		world_name = Texture();
		name_text.change_text("");
		message_text.change_text("");

		uint16_t wc = worldselect ? worldselect->get_world_count() : 0;
		has_worlds = wc > 0;

		// 083 ResetWorldAndMessage @0x62b9da：数组长度 > 1 时才把两箭头
		// 置为可用（a2:1），否则 a2:0。用 DISABLED 态让真素材的 disabled
		// 位图得以显示；buttons 只读访问一律走 find，避免 operator[] 插空项。
		Button::State arrow_state = wc > 1 ? Button::State::NORMAL : Button::State::DISABLED;
		const Ids arrow_ids[2] = { BT_MOVE_LEFT, BT_MOVE_RIGHT };
		for (Ids id : arrow_ids)
		{
			auto it = buttons.find(id);
			if (it != buttons.end() && it->second)
				it->second->set_state(arrow_state);
		}

		if (!has_worlds)
		{
			message_text.change_text("暂无可用世界");
			return;
		}

		if (current_index >= wc)
			current_index = 0;

		const World* w = worldselect->get_world(static_cast<uint16_t>(current_index));
		if (!w)
			return;

		// 083 WORLD_D 用 worldID 拼 UOL 取名牌图；当前端 UIWorldSelect.cpp:995
		// 同样按 std::to_string(world.id) 取 Login.img/WorldSelect/world/<id>，
		// 缺失回退 "0"。数据源沿用当前端 SERVERLIST，不照搬 083 的 worldID 语义。
		nl::node worldsrc = nl::nx::UI["Login.img"]["WorldSelect"]["world"];
		std::string key = std::to_string(static_cast<int32_t>(w->id));
		nl::node name_node = worldsrc[key] ? worldsrc[key] : worldsrc["0"];
		if (name_node)
			world_name = Texture(name_node);

		if (!world_name)
			name_text.change_text(w->name.empty() ? "(未知世界)" : w->name);

		// 083 消息 ZXString -> Assign<char> -> CalcLongestText(200) 换行。
		message_text.change_text(w->event_message);
	}

	void UIRecommendWorld::move_world(int delta)
	{
		uint16_t wc = worldselect ? worldselect->get_world_count() : 0;
		if (wc == 0)
			return; // 083 MoveWorld 的 (n+v3+idx) % v3 在 v3==0 时是原生 UB，本窗修掉。

		// 083 MoveWorld @0x62b99d：(nMovement + v3 + current) % v3 环绕。
		int64_t next = static_cast<int64_t>(current_index) + delta;
		next %= static_cast<int64_t>(wc);
		if (next < 0)
			next += wc;

		current_index = static_cast<size_t>(next);
		refresh();
	}

	void UIRecommendWorld::select_world()
	{
		uint16_t wc = worldselect ? worldselect->get_world_count() : 0;
		if (wc > 0 && current_index < wc)
		{
			if (const World* w = worldselect->get_world(static_cast<uint16_t>(current_index)))
				worldselect->change_world(*w);
		}

		// 083 SelectWorld @0x62b96b 末尾 CWnd::Destroy(1) 关窗。
		// UIStateLogin::remove 用 element.release() 放弃所有权而不析构，
		// 故返回后基类 send_cursor 继续 set_state 不会 use-after-free。
		UI::get().remove(TYPE);
	}
}
