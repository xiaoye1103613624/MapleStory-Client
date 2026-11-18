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
#include "UITermsOfService.h"

#include "UILoginWait.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../KeyAction.h"

#include "../Components/MapleButton.h"

#include "../../MapleStory.h"
#include "../../Net/Packets/LoginPackets.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		// Login.img/TOS/0 = 431x426，origin (215,213)（图心，nx_inspect 实测）。
		// 下列偏移相对面板左上（局部坐标）。
		//
		// 按钮对齐（实机取证：背景右下烘焙两个橙色按钮槽，PIL 逐像素实测）：
		//   槽1 x[266..330]  槽2 x[340..404]  y[372..401]，各约 65x30、间隙 9、中心距 74。
		// BtOk/BtCancel 为 75x32、origin=(33,14)（非 0,0！），绘制与 bounds 均按
		// 坐标-origin 落点。旧值 (268/342,388) 未补偿 origin → 视觉 (235..310)/(309..384)，
		// 相对槽整体偏左 31px，槽2 右侧露出 20px 橙色（"取消后面还有 UI"）。
		// 运行时 UI.nx 无更窄的确定/取消按钮（66 个按钮全量核对，最接近的
		// Notice/Loading/BtCancel 65x23 高度不符），故让按钮居中盖住槽：
		//   传入坐标 = 目标视觉左上 + origin：
		//   OK 视觉 [258..333] 盖槽1（组中心 334.5 ≈ 槽组中心 335，间隙 3px 不重叠）
		//   CANCEL 视觉 [336..411] 盖槽2；y 视觉 [370..402] 对槽 [372..401] 居中。
		constexpr int16_t TOS_ORIGIN_X = 215;
		constexpr int16_t TOS_ORIGIN_Y = 213;
		constexpr int16_t TOS_TEXT_X = 36;
		constexpr int16_t TOS_TEXT_Y = 50;
		constexpr int16_t TOS_TEXT_W = 340;
		constexpr int16_t TOS_CLIP_MIN = 50;
		constexpr int16_t TOS_CLIP_LEN = 316;
		constexpr int16_t TOS_SLIDER_X = 384;
		constexpr int16_t TOS_SLIDER_Y0 = 55;
		constexpr int16_t TOS_SLIDER_Y1 = 360;
		constexpr int16_t TOS_BT_OK_X = 291;		// 291-33=258..333，盖槽1 266..330
		constexpr int16_t TOS_BT_CANCEL_X = 369;	// 369-33=336..411，盖槽2 340..404
		constexpr int16_t TOS_BT_Y = 384;			// 384-14=370..402，槽 y 372..401 居中
	}

	UITermsOfService::UITermsOfService(std::function<void()> oh) : okhandler(oh), offset(0), unit_rows(1)
	{
		nl::node Login = nl::nx::UI["Login.img"];
		nl::node TOS = Login["TOS"];

		// Texture::draw 的矩形 left = args.pos.x - origin.x（get_rectangle 在 xscale=1
		// 时 center 被抵消）。直接 emplace_back(TOS) 时 args.pos=position，背景左上角
		// 变成 position-origin=(209,-66)，而正文/滚动条/按钮用 position+局部 锚定——
		// 这就是"文字居中、羊皮纸偏左上"的根因。
		// 传 stateargs.pos=origin：sprite.draw(position) 后 args.pos=position+origin，
		// left=(position+origin)-origin=position，背景与正文共用左上锚点。
		// 单 Point 构造同时令 center=origin，旋转中心落在图心（angle=0 时无副作用）。
		sprites.emplace_back(TOS, Point<int16_t>(TOS_ORIGIN_X, TOS_ORIGIN_Y));

		buttons[Buttons::OK] = std::make_unique<MapleButton>(
			Login["BtOk"],
			Point<int16_t>(TOS_BT_OK_X, TOS_BT_Y)
		);
		buttons[Buttons::CANCEL] = std::make_unique<MapleButton>(
			Login["BtCancel"],
			Point<int16_t>(TOS_BT_CANCEL_X, TOS_BT_Y)
		);

		EULA = nl::nx::String["TrialEULA.img"]["EULA"];
		max_rows = EULA.size();

		text = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "", TOS_TEXT_W, true, 2);

		slider = Slider(
			Slider::Type::LINE_PUNGA,
			Range<int16_t>(TOS_SLIDER_Y0, TOS_SLIDER_Y1),
			TOS_SLIDER_X,
			unit_rows,
			max_rows,
			[&](bool upwards)
			{
				int16_t shift = upwards ? -1 : 1;
				bool above = offset + shift >= 0;
				bool below = offset + shift <= max_rows - unit_rows;

				if (above && below)
				{
					offset += shift;
					update_accept(offset);
				}
			}
		);

		update_accept(offset);

		// TOS 是容器节点：Texture(容器) 得 0×0，center_on_screen 会把面板顶到视口正中 (640,360)。
		// 用首帧位图 TOS["0"]（运行时实测 431×426）作为面板尺寸，保证面板居中且局部坐标锚定正确。
		dimension = Texture(TOS["0"]).get_dimensions();
		center_on_screen();

		LOG(LOG_INFO, "UITermsOfService open dim=" << dimension.x() << "x" << dimension.y()
			<< " pos=(" << position.x() << "," << position.y() << ")");
	}

	void UITermsOfService::draw(float inter) const
	{
		UIElement::draw(inter);

		// drawtext 内 char_y 为绝对屏幕坐标，vertical Range 必须同为绝对坐标；
		// 原传入相对面板的 (50,366) 与 y≈410 的正文比较导致整段被裁掉（正文空白）。
		// 顶部沿用面板顶不裁，底部按局部裁剪长度换算为绝对坐标，防止正文溢出按钮区。
		const int16_t clip_top = position.y();
		const int16_t clip_bottom = static_cast<int16_t>(clip_top + TOS_CLIP_MIN + TOS_CLIP_LEN);
		text.draw(
			position + Point<int16_t>(TOS_TEXT_X, TOS_TEXT_Y),
			Range<int16_t>(clip_top, clip_bottom)
		);
		slider.draw(position);
	}

	void UITermsOfService::update_screen(int16_t, int16_t)
	{
		center_on_screen();
	}

	Cursor::State UITermsOfService::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Point<int16_t> cursoroffset = cursorpos - position;

		if (slider.isenabled())
		{
			Cursor::State state = slider.send_cursor(cursoroffset, clicked);

			if (state != Cursor::State::IDLE)
				return state;
		}

		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UITermsOfService::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (escape) { button_pressed(Buttons::CANCEL); return; }
		if (keycode == KeyAction::Id::RETURN) { button_pressed(Buttons::OK); return; }
		if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't'
			|| keycode == 'Y' || keycode == 'y' || keycode == 'N' || keycode == 'n')
		{
			
		}
	}

	UIElement::Type UITermsOfService::get_type() const
	{
		return TYPE;
	}

	Button::State UITermsOfService::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::OK:
				UI::get().emplace<UILoginWait>();

				TOSPacket().dispatch();
				break;
			case Buttons::CANCEL:
				UI::get().quit();
				break;
			default:
				break;
		}

		return Button::State::NORMAL;
	}

	void UITermsOfService::update_accept(uint16_t offset)
	{
		if (offset == max_rows - unit_rows)
			buttons[Buttons::OK]->set_state(Button::State::NORMAL);
		else
			buttons[Buttons::OK]->set_state(Button::State::DISABLED);

		std::string shownText = "";

		for (nl::node text : EULA)
		{
			std::string name = text.name();
			name = name.substr(4, 6);

			int32_t i = std::stoi(name);

			if (i >= offset && i <= offset + 5)
				shownText += text.get_string();
		}

		text.change_text(shownText);
	}
}
