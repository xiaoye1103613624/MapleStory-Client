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
#include "UIGender.h"

#include "UISoftKey.h"

#include "../UI.h"
#include "../KeyAction.h"

#include "../Components/MapleButton.h"
#include "../Components/MapleComboBox.h"
#include "../UITypes/UILoginWait.h"

#include "../../Net/Packets/LoginPackets.h"
#include "../../Util/Misc.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#include "../../Util/Locale.h"
#endif

namespace ms
{
	UIGender::UIGender(std::function<void()> oh) : okhandler(oh)
	{
		CUR_TIMESTEP = 0;

		nl::node Gender = nl::nx::UI["Login.img"]["Gender"];

		for (size_t i = 0; i < 3; i++)
			gender_sprites[i] = Gender["scroll"]["0"][i];

		// 卷轴三帧是分时展开动画（354x46 → 354x200 → 354x196，origin 均 (0,0)），
		// 非垂直拼接。以最终稳定帧尺寸 354x196 作为面板 dimension。
		// 原实现 position=(0,15) + 一堆 800x600 时代的绝对坐标（卷轴锚 (355,185)、
		// 按钮 (578/650,349)、下拉 (510,283)），在 1280x720 下整体偏左上。
		// 现改为：position 由 center_on_screen 居中，元素全部用相对卷轴左上的局部坐标
		// （局部 = 原绝对 - 原卷轴锚 (355,185)），保持内部相对关系不变。
		dimension = Texture(gender_sprites[2]).get_dimensions();
		center_on_screen();

		constexpr int16_t BT_YES_X = 223;
		constexpr int16_t BT_NO_X = 295;
		constexpr int16_t BT_Y = 164;
		constexpr int16_t COMBO_X = 155;
		constexpr int16_t COMBO_Y = 98;

		// 提示语恢复 WZ 原生位图 Gender/text/0（298x100，origin=(224,116)）：
		// 一张图烘焙完整三行文案 + ✳性别标签 + 下拉框底座（参考端原样）。
		// 上一轮 Text+Locale 单行方案把它退化成「欢迎光临！请选择您的性别。」，
		// 丢掉了后半句与性别标签——R1/R6：数据即真相，恢复位图。
		// 视觉落点：目标 = position+(22,25)（原版绝对 (601,326) 减卷轴锚 (355,185)
		// = (246,141)，其中 origin 已含在内）；Sprite::draw 的 left=pos-origin，
		// 故 stateargs 传 Point(246,141) → left=(246-224,141-116)=(22,25)。
		sprites.emplace_back(Gender["text"]["0"], Point<int16_t>(246, 141));

		// 下拉框选项走 locale：uigender.131=男 / uigender.129=女（已有 key）。
		std::vector<std::string> options;
		options.push_back(Locale::text("uigender.131"));
		options.push_back(Locale::text("uigender.129"));

		uint16_t default_option = 0;

		buttons[Buttons::NO] = std::make_unique<MapleButton>(Gender["BtNo"], Point<int16_t>(BT_NO_X, BT_Y));
		buttons[Buttons::YES] = std::make_unique<MapleButton>(Gender["BtYes"], Point<int16_t>(BT_YES_X, BT_Y));
		buttons[Buttons::SELECT] = std::make_unique<MapleComboBox>(MapleComboBox::Type::DEFAULT, options, default_option, position, Point<int16_t>(COMBO_X, COMBO_Y), 65);
	}

	void UIGender::draw(float inter) const
	{
		// 卷轴 origin=(0,0)，锚点即面板 position；局部偏移改为 0（原 355,185 是绝对坐标）。
		Point<int16_t> gender_pos = Point<int16_t>(0, 0);

		if (CUR_TIMESTEP == 0)
		{
			gender_sprites[0].draw(position + gender_pos);
		}
		else if (CUR_TIMESTEP == Constants::TIMESTEP * 3)
		{
			gender_sprites[1].draw(position + gender_pos);
		}
		else if (CUR_TIMESTEP >= Constants::TIMESTEP * 6)
		{
			gender_sprites[2].draw(position + gender_pos);

			// 展开完成后才画 text/0 位图（在 sprites 中）与按钮/下拉框（原生时序）。
			// UIElement::draw 内部先 sprites 后 buttons，保证下拉框底座在控件下层。
			UIElement::draw(inter);
		}
	}

	void UIGender::update_screen(int16_t, int16_t)
	{
		center_on_screen();
	}

	void UIGender::update()
	{
		UIElement::update();

		if (CUR_TIMESTEP <= Constants::TIMESTEP * 6)
			CUR_TIMESTEP += Constants::TIMESTEP;
	}

	Cursor::State UIGender::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		auto& combobox = buttons[Buttons::SELECT];

		if (combobox->is_pressed() && combobox->in_combobox(cursorpos))
			if (Cursor::State new_state = combobox->send_cursor(clicked, cursorpos))
				return new_state;

		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UIGender::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (escape) { button_pressed(Buttons::NO); return; }
		if (keycode == KeyAction::Id::RETURN) { button_pressed(Buttons::YES); return; }
		if (keycode == 'M' || keycode == 'm' || keycode == '0')
		{
			UI::get().emplace<UILoginWait>();
			GenderPacket(false).dispatch();
			// 同 button_pressed(YES)：确认即收框，不等封包时序（见该处注释）。
			deactivate();
			LOG(LOG_DEBUG, "UIGender: male");
			return;
		}
		if (keycode == 'F' || keycode == 'f' || keycode == '1')
		{
			UI::get().emplace<UILoginWait>();
			GenderPacket(true).dispatch();
			deactivate();
			LOG(LOG_DEBUG, "UIGender: female");
			return;
		}
		if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't')
		{
			
		}
	}

	UIElement::Type UIGender::get_type() const
	{
		return TYPE;
	}

	Button::State UIGender::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::NO:
			{
				deactivate();
				okhandler();

				return Button::State::NORMAL;
			}
			case Buttons::YES:
			{
				UI::get().emplace<UILoginWait>();

				uint16_t selected_value = buttons[Buttons::SELECT]->get_selected();
				GenderPacket(selected_value).dispatch();

				// 修复（2026-11-03 实机：创建流程多 UI 态同屏叠加）：欢迎框此前
				// 只等 LOGIN_RESULT 的 remove(GENDER) 回收，而旧 remove 在
				// `loginwait->is_active()` 门内——Wait 满条自 deactivate 后该门
				// 不成立，GENDER 永不拆除，跨页残留与频道/创建窗口叠加。
				// 原生行为是「确认即收框」：就地 deactivate（实例由
				// LOGIN_RESULT/GENDER_DONE 的统一清理回收），不再赌封包时序。
				deactivate();

				return Button::State::NORMAL;
			}
			case Buttons::SELECT:
			{
				buttons[Buttons::SELECT]->toggle_pressed();

				return Button::State::NORMAL;
			}
			default:
			{
				return Button::State::DISABLED;
			}
		}
	}
}