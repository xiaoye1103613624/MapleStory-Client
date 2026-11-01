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

#include "../UIElement.h"

#include "../Components/Slider.h"

#include "../../Graphics/Text.h"

#include <string>
#include <utility>
#include <vector>

namespace ms
{
	class UINpcTalk : public UIElement
	{
	public:
		// Wire msgtype values (BeiDou / v83):
		// 0=Say(OK/Next/Prev via style), 1=YesNo, 2=GetText, 3=GetNumber,
		// 4=Simple, 12=AcceptDecline
		enum TalkType : int8_t
		{
			NONE = -1,
			SENDOK = 0,
			SENDYESNO = 1,
			SENDGETTEXT = 2,
			SENDGETNUMBER = 3,
			SENDSIMPLE = 4,
			SENDACCEPTDECLINE = 12,
			// Style-derived variants of SENDOK (not wire values)
			SENDNEXT = 100,
			SENDNEXTPREV = 101,
			SENDPREV = 102
		};

		static constexpr Type TYPE = UIElement::Type::NPCTALK;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UINpcTalk();

		void draw(float inter) const override;
		void update() override;

		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

		void change_text(int32_t npcid, int8_t msgtype, int16_t style, int8_t speaker, const std::string& text);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		TalkType get_by_value(int8_t value);
		TalkType resolve_say_style(int16_t style) const;
		std::string format_text(const std::string& tx, const int32_t& npcid);
		int8_t wire_msgtype() const;
		// speaker 0=NPC→UtilDlgEx；1=玩家→UtilDlgEx_Avatar；2=宠物→UtilDlgEx_Pet
		void apply_dlg_skin(int8_t speaker);

		static constexpr int16_t MAX_HEIGHT = 248;

		// 正文块位置：三张原生对话帧（528x261 / 526x349 / 523x203）逐列量的暗字像素边界，
		// 左缘 163..170、右缘 474..476，取中位 166 + 折行宽 310（= 右缘 476）。
		// 原来写死 x=162/166 + maxwidth=320 会把正文铺到 x482，和右下角「确定」(474..520) 叠住。
		// TEXT_Y 是绘制点（不是字面墨迹顶）：三张帧量到的墨迹顶是 39 / 58 / 72，原生似乎按
		// 正文高度在正文区里居中；这里保持 cj 原来的 48（落在区间中间），留待真机对帧再定。
		static constexpr int16_t TEXT_X = 166;
		static constexpr int16_t TEXT_Y = 48;
		static constexpr int16_t TEXT_WIDTH = 310;
		// 正文可视区上下各留的余量（沿用原实现的 18px），翻页步长 = 壳高 - 该余量。
		static constexpr int16_t TEXT_SLACK = 18;

		// 底带按钮槽位（窗口左上为原点）。UtilDlgEx 里每个按钮的 origin 都是 (0,0)，
		// 所以位置只能由这里给；不给就会全部叠在窗口左上角。
		// 尺寸取自 .nxcache/UI.nx：BtClose 85x19 / BtOK,BtYes,BtNo,BtNext,BtPrev 46x18 / BtQ* 57x17。
		// 纵向 49 由两张独立原生帧互证：壳高 = t(28)+正文+s(58)，
		// 528x261 帧反解正文 175、按钮顶 224 ⇒ 差 49；526x349 帧反解正文 263、按钮顶 312 ⇒ 差 49。
		// 也即底片 s 内分隔条 +15..+20、按钮压 +21，21 + t.height(28) = 49。
		static constexpr int16_t BTN_DY = 49;
		// 横向：右对齐留边 9（46 宽 → 474..520），成对时左那颗退 65（原生 接受/拒绝 实测间距 65）。
		static constexpr int16_t BTN_X_RIGHT = 474;
		static constexpr int16_t BTN_X_RIGHT_WIDE = 466;
		static constexpr int16_t BTN_PAIR_PITCH = 65;
		static constexpr int16_t BTN_X_CLOSE = 12;

		enum Buttons
		{
			ALLLEVEL,
			CLOSE,
			MYLEVEL,
			NEXT,
			NO,
			OK,
			PREV,
			QAFTER,
			QCNO,
			QCYES,
			QGIVEUP,
			QNO,
			QSTART,
			QYES,
			YES
		};

		Texture top;
		Texture fill;
		Texture bottom;
		Texture nametag;
		Texture speaker;

		Text text;
		Text name;

		int16_t height;
		int16_t offset;
		int16_t unitrows;
		int16_t rowmax;
		int16_t min_height;

		bool show_slider;
		bool draw_text;
		Slider slider;
		TalkType type;
		int8_t msgtype_wire;
		std::string formatted_text;
		size_t formatted_text_pos;
		uint16_t timestep;

		// SENDSIMPLE：#Lindex#label# 解析后的可点选项
		struct SimpleChoice
		{
			int32_t selection = 0;
			Text label;
			Point<int16_t> pos;
		};
		std::vector<SimpleChoice> simple_choices;

		std::function<void(bool)> onmoved;

		void parse_simple_choices(const std::string& tx);
	};
}
