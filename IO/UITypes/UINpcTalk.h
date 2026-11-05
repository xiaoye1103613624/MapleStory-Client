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
#include "../Components/Textfield.h"

#include "../../Graphics/Geometry.h"
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
		// 多分辨率：视口变化后按新屏幕重锚对话窗。
		void update_screen(int16_t new_width, int16_t new_height) override;

		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

		// GetText/GetNumber 线参（BeiDou PacketCreator.getNPCTalkText/Num + IDA sub_9AC732/9AC7CB）
		struct TalkInputParams
		{
			std::string def_text;
			int32_t def_number = 0;
			int32_t min_number = 0;
			int32_t max_number = 0;
			// GetNumber 包内带了 def/min/max 时置位；未置位则只做本地空/非 int 校验
			bool has_number_bounds = false;
			// GetText 包尾 int（常为 0）；仅当 >0 时作最小长度（对齐 9AB50A type3 vs this+0xDC）
			int32_t text_min_len = 0;
			// Edit 字符上限：AskNumber 助手常传 10→this+0xCC；GetText 未进包则用默认
			int32_t edit_maxlen = 0;
		};

		void change_text(int32_t npcid, int8_t msgtype, int16_t style, int8_t speaker, const std::string& text);
		void change_text(int32_t npcid, int8_t msgtype, int16_t style, int8_t speaker, const std::string& text,
			const TalkInputParams& input);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		// 1280×720 保留原生 (400,240) 定位；其它档位按视口居中。
		void reanchor_talk();

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
		// 正文 Y：IDA CUtilDlgEx::LayoutControls 路径（BeiDou sub_9A64FC）在正文装得下时
		//   y = (content_h - text_h) / 2 + t.height + 7；装不下（滑条）则贴 content 顶 +7。
		// 运行时写入 text_draw_y，禁止再写死 48。
		static constexpr int16_t TEXT_X = 166;
		static constexpr int16_t TEXT_WIDTH = 310;
		// 正文可视区上下各留的余量（沿用原实现的 18px），翻页步长 = 壳高 - 该余量。
		static constexpr int16_t TEXT_SLACK = 18;
		// IDA sub_9A64FC 非 Avatar/Pet 时正文绘制点相对几何居中再 +7。
		static constexpr int16_t TEXT_Y_PAD = 7;

		// 底带按钮槽位（窗口左上为原点）。UtilDlgEx 里每个按钮的 origin 都是 (0,0)，
		// 所以位置只能由这里给；不给就会全部叠在窗口左上角。
		// 尺寸取自 .nxcache/UI.nx：BtClose 85x19 / BtOK,BtYes,BtNo,BtNext,BtPrev 46x18 / BtQ* 57x17。
		// 纵向底带 BTN_DY=49：两张独立原生帧互证（R25）。
		// Next/Prev 纵向：IDA sub_9A7C8B / sub_9A8FEF 用 wnd_h-77，Close/OK 用 wnd_h-26，
		// 差值 51 ⇒ BTN_DY_NEXT = BTN_DY - 51（相对本仓 content height 坐标系）。
		static constexpr int16_t BTN_DY = 49;
		static constexpr int16_t BTN_DY_NEXT = static_cast<int16_t>(BTN_DY - 51);
		// 横向（IDA，相对窗宽 W=top.width()）：
		//   BtOK          = W - 54          （R25 参考帧 474 = 528-54，曾误当成 Next）
		//   BtNext/BtPrev = W - inset - 68 / W - inset - 122（inset=10；Avatar 皮肤 inset=140）
		//   BtClose       = 9（TEXT）/ 10（LIST）
		// QYes/QNo 仍用 R25 像素：宽钮右位 466、成对间距 65。
		static constexpr int16_t BTN_OK_RIGHT_INSET = 54;
		static constexpr int16_t BTN_NEXT_RIGHT_INSET = 68;
		static constexpr int16_t BTN_PREV_RIGHT_INSET = 122;
		static constexpr int16_t BTN_EDGE_INSET = 10;
		static constexpr int16_t BTN_EDGE_INSET_AVATAR = 140;
		static constexpr int16_t BTN_X_CLOSE = 9;
		static constexpr int16_t BTN_X_CLOSE_LIST = 10;
		static constexpr int16_t BTN_X_RIGHT_WIDE = 466;
		static constexpr int16_t BTN_PAIR_PITCH = 65;

		// GetText/GetNumber 输入槽（IDA sub_9A83F6 / sub_9ACC20）：
		//   content_left = (speaker&6)!=0 ? 25 : 157（Pet speaker=2 置位；非 Avatar 布局）；
		//   黑框 h=18、白底 h=16、Edit h=14；
		//   黑框 y = content_top + text_h + 2，白底 +3，Edit +5；宽 = W - 2*left ± 边。
		static constexpr int16_t INPUT_LEFT = 157;
		static constexpr int16_t INPUT_LEFT_PET = 25; // IDA sub_9ACC20 + this[441]
		static constexpr int16_t INPUT_BLACK_H = 18;
		static constexpr int16_t INPUT_WHITE_H = 16;
		static constexpr int16_t INPUT_EDIT_H = 14;
		static constexpr int16_t INPUT_STACK = static_cast<int16_t>(2 + INPUT_BLACK_H + 8);
		// Avatar 变体（IDA sub_9A8716，a1[41]/speaker 头像态）：无黑白框；
		//   Edit(1000)=(19, H-73) 226×15；BtOK=(158,H-30)；BtClose=(208,H-30)。
		// 本仓 H-26 ≈ height+BTN_DY ⇒ H-30 = height+BTN_DY-4；Edit 相对 OK 高 43。
		static constexpr int16_t INPUT_AVATAR_EDIT_X = 19;
		static constexpr int16_t INPUT_AVATAR_EDIT_W = 226;
		static constexpr int16_t INPUT_AVATAR_EDIT_H = 15;
		static constexpr int16_t INPUT_AVATAR_EDIT_ABOVE_OK = 43;
		static constexpr int16_t BTN_AVATAR_OK_X = 158;
		static constexpr int16_t BTN_AVATAR_CLOSE_X = 208;
		static constexpr int16_t BTN_AVATAR_Y_DELTA = 4; // H-30 vs 非 Avatar H-26
		static constexpr int16_t INPUT_AVATAR_STACK = 56; // 预留正文下输入带（约 H-73 带）
		// SENDSIMPLE 选中态（IDA sub_9A3E9F）：黑 1px 底线 + #919191 虚线（3 on / 3 off），宽 = 标签宽+8。
		static constexpr int16_t SIMPLE_UL_EXT = 8;
		static constexpr uint8_t SIMPLE_DASH_R = 0x91;
		static constexpr uint8_t SIMPLE_DASH_G = 0x91;
		static constexpr uint8_t SIMPLE_DASH_B = 0x91;

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
		// 正文绘制点 y（窗内坐标）。change_text 按 IDA 居中公式写入；SENDSIMPLE 命中同值。
		int16_t text_draw_y;
		int16_t text_block_height;
		int8_t dlg_speaker;

		bool show_slider;
		bool draw_text;
		Slider slider;
		TalkType type;
		int8_t msgtype_wire;
		std::string formatted_text;
		size_t formatted_text_pos;
		uint16_t timestep;

		// SENDSIMPLE：#Lindex#label#l 解析后的可点选项。
		// 原生链接是**行内**排版（脚本用 \t 把三项排成一行、\r\n 分行），所以选项
		// 不是独立控件：正文照原样画，这里只记它在布局结果里的落点。
		struct SimpleChoice
		{
			int32_t selection = 0;
			// 标签在剥掉令牌后的正文里的字节区间
			size_t first = 0;
			size_t last = 0;
			// 由 layout_simple_choices() 从正文布局反取（相对正文绘制原点）
			Point<int16_t> pos;
			Point<int16_t> size;
		};
		std::vector<SimpleChoice> simple_choices;
		// SENDSIMPLE 当前高亮项（键盘/悬停）。IDA CUtilDlgEx::OnButtonClicked
		// (BeiDou sub_9AB92A) dlgType==4：BtNext(id=8193) 读 list[cur].sel 后收口；
		// 点 #L 仍可直接发 selection，下项钮发当前高亮项。
		int32_t simple_selected = 0;

		// GetText/GetNumber：内嵌输入（对齐原生 CCtrlEdit id=1000）；非本类型时 DISABLED。
		Textfield input_field;
		bool input_active = false;
		int16_t input_box_y = 0;
		TalkInputParams input_params;
		bool input_avatar_layout = false;

		std::function<void(bool)> onmoved;

		// 把 #L<n>#label#l 就地拆成正文里的 label + 可点区间，返回剥掉令牌后的文本
		std::string strip_simple_choices(const std::string& tx);
		// 正文 Text 建好布局后，把字节区间换算成命中框
		void layout_simple_choices(const Text& body);
		// SENDSIMPLE：发当前高亮项的 selection（下项钮 / SoftKey 共用）
		bool dispatch_simple_selection();
		// GetText/GetNumber：布置/刷新内嵌输入框几何与焦点
		void setup_input_field();
		void clear_input_field();
		bool dispatch_input_field();
		// 校验失败 tip（IDA sub_9AB50A → sub_9929DD，StringPool 817–821）
		void show_input_tip(const std::string& message);
		int16_t input_content_left() const;
	};
}
