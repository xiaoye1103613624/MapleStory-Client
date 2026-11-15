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
#include "ChatBalloon.h"

#include "../../Constants.h"

#include <algorithm>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	ChatBalloon::ChatBalloon(int8_t type)
	{
		std::string typestr;

		if (type < 0)
		{
			switch (type)
			{
				case -1:
					typestr = "dead";
					break;
			}
		}
		else
		{
			typestr = std::to_string(type);
		}

		nl::node src = nl::nx::UI["ChatBalloon.img"][typestr];

		arrow = src["arrow"];
		frame = src;

		textlabel = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK, "", 80);

		duration = 0;
	}

	ChatBalloon::ChatBalloon() : ChatBalloon(0) {}

	void ChatBalloon::change_text(const std::string& text)
	{
		textlabel.change_text(text);

		duration = DURATION;
	}

	void ChatBalloon::draw(Point<int16_t> position) const
	{
		if (duration == 0)
			return;

		int16_t width = textlabel.width();
		int16_t height = textlabel.height();

		frame.draw(position, width, height);
		arrow.draw(position);
		textlabel.draw(position - Point<int16_t>(0, height + 4));
	}

	void ChatBalloon::update()
	{
		duration -= Constants::TIMESTEP;

		if (duration < 0)
			duration = 0;
	}

	void ChatBalloon::expire()
	{
		duration = 0;
	}

	ChatBalloonHorizontal::ChatBalloonHorizontal()
	{
		nl::node Balloon = nl::nx::UI["Login.img"]["WorldNotice"]["Balloon"];

		arrow = Balloon["arrow"];
		center = Balloon["c"];
		northeast = Balloon["ne"];
		north = Balloon["n"];
		northwest = Balloon["nw"];
		west = Balloon["w"];
		southwest = Balloon["sw"];
		south = Balloon["s"];
		southeast = Balloon["se"];

		xtile = std::max<int16_t>(north.width(), 1);
		ytile = std::max<int16_t>(west.height(), 1);

		textlabel = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::BLACK);
	}

	void ChatBalloonHorizontal::draw(Point<int16_t> position) const
	{
		// 空文案不画边框，避免与 BalloonForLoginProcess 公告叠成「两个弹窗」。
		if (textlabel.get_text().empty())
			return;

		int16_t width = textlabel.width() + 9;
		int16_t height = textlabel.height() - 1;

		int16_t left = position.x() - width / 2;
		int16_t top = position.y() - height;
		int16_t right = left + width;
		int16_t bottom = top + height;

		northwest.draw(DrawArgument(left, top));
		southwest.draw(DrawArgument(left, bottom));

		for (int16_t y = top; y < bottom; y += ytile)
		{
			west.draw(DrawArgument(left, y));
		}

		center.draw(DrawArgument(Point<int16_t>(left, top), Point<int16_t>(width + 1, height)));

		for (int16_t x = left; x < right; x += xtile)
		{
			north.draw(DrawArgument(x, top));
			south.draw(DrawArgument(x, bottom));
		}

		northeast.draw(DrawArgument(right, top));
		southeast.draw(DrawArgument(right, bottom));

		arrow.draw(DrawArgument(right + 1, top));
		textlabel.draw(DrawArgument(left + 6, top - 4));
	}

	void ChatBalloonHorizontal::change_text(const std::string& text)
	{
		textlabel.change_text(text);
	}

	LoginProcessNotice::LoginProcessNotice() : box_w(0), box_h(0), top_border(0), bottom_border(0)
	{
		// 优先整板用 BalloonForLoginProcess 九宫（决策 9.7）；缺件时回退 Balloon 小九宫 + Title。
		nl::node loginproc = nl::nx::UI["Login.img"]["WorldNotice"]["BalloonForLoginProcess"];
		nl::node balloon = nl::nx::UI["Login.img"]["WorldNotice"]["Balloon"];
		nl::node frame = loginproc;

		if (!loginproc["nw"] && !loginproc["c"])
			frame = balloon;

		title = loginproc["Title"] ? loginproc["Title"] : Texture();
		center = frame["c"] ? frame["c"] : balloon["c"];
		northeast = frame["ne"] ? frame["ne"] : balloon["ne"];
		north = frame["n"] ? frame["n"] : balloon["n"];
		northwest = frame["nw"] ? frame["nw"] : balloon["nw"];
		west = frame["w"] ? frame["w"] : balloon["w"];
		east = frame["e"] ? frame["e"] : balloon["e"];
		southwest = frame["sw"] ? frame["sw"] : balloon["sw"];
		south = frame["s"] ? frame["s"] : balloon["s"];
		southeast = frame["se"] ? frame["se"] : balloon["se"];

		xtile = std::max<int16_t>(north.width(), 1);
		ytile = std::max<int16_t>(west.height(), 1);

		// 九宫边框厚度以角件为准（运行时取证 Login.img_runtime.json：
		// BalloonForLoginProcess nw/ne 22x40、sw/se 22x24 —— 上边框 40、下边框 24，
		// 静止框高 64，与参考图「选中频道页.png」公告 (287,298)-(486,366) 高 68
		// 一致。旧实现按 west 条高(9px)铺行数，框高/框宽都算错（实机 ≈135x73 小方盒）。
		top_border = northwest.is_valid() ? northwest.height() : 0;
		bottom_border = southwest.is_valid() ? southwest.height() : 0;
		left_border = northwest.is_valid() ? northwest.width() : 0;
		right_border = northeast.is_valid() ? northeast.width() : 0;

		textlabel = Text(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::BLACK);
	}

	void LoginProcessNotice::change_text(const std::string& text)
	{
		textlabel.change_text(text);
		relayout();
	}

	void LoginProcessNotice::relayout()
	{
		if (textlabel.get_text().empty())
		{
			box_w = 0;
			box_h = 0;
			return;
		}

		// 九宫语义（运行时 Login.img 取证）：每块 tile 的 origin 是其「内角」，
		// 绘制时 tile 从内容区角点向外延展——nw/ne 上带 40px（容纳标题），
		// sw/se 下带 24px（容纳正文），左右带 22px。box_w/box_h 为**外框**尺寸。
		// 原生公告板为固定板：orange-wz 真角件（WorldNotice/BalloonForLoginProcess
		// nw/ne/se，128B GMS128 key 重载 Login.img 取得）在两端帧做 alpha-NCC
		// 独立定位（ui_r3/real_corners_settle.json）：
		//   TARGET nw@280 ne@450 se@352 / OURS(v1) nw@285 ne@447 se@344
		//   span(ne-nw) = box_w-22（OURS 184-22=162 精确自洽；ne_left+22=472 即框右缘）
		//   ⇒ TARGET span=170 ⇒ box_w=192。
		// 【196 已证伪】曾按 title(170) 居中反推 (box_w-170)/2=13 得 196，但真 ne
		// 模板给出右缘 472，196 会推到 476 过冲 4px——只用两个左锚点拟合全局框宽
		// 属过拟合。原生 title 并非严格居中：目标 title293-nw280 inset=13 ≠
		// (192-170)/2=11，故 title_x 改独立常量 left+13（见 draw()），与框宽解耦。
		// 框高：真 se 显示目标约 80 / 本端 74，但 box_h 增大会经正文居中公式把
		// 已偏低 3.4px 的正文再下推，正文像素判据不可靠 → 本轮保持 74，
		// 框底(se)/正文 ~8px / ~3px 残差如实披露，不动已验证的垂直内部位置。
		// 内容区 = 外框 - 左右边框44 = 148x10，容纳一行 A12B 正文。
		constexpr int16_t box_w_ref = 192;
		constexpr int16_t box_h_ref = 74;

		box_w = box_w_ref;
		box_h = box_h_ref;
	}

	bool LoginProcessNotice::empty() const
	{
		return textlabel.get_text().empty();
	}

	int16_t LoginProcessNotice::width() const
	{
		return box_w;
	}

	int16_t LoginProcessNotice::height() const
	{
		return box_h;
	}

	void LoginProcessNotice::draw(Point<int16_t> topleft) const
	{
		if (empty() || box_w <= 0 || box_h <= 0)
			return;

		// 九宫语义（运行时取证 + Texture::draw origin 机制）：tile 实际渲染左上
		// = 绘制参数 - origin。BalloonForLoginProcess 各件 origin 即「内角」：
		//   nw 22x40 o(22,40)  ne 22x40 o(0,40)  sw 22x24 o(22,0)  se 22x24 o(0,0)
		//   n 12x40 o(0,40)  s 12x24 o(0,0)  w 19x9 o(19,0)  e 22x9 o(0,0)
		// 因此四角件参数取内容框角点 (cl/ct/cr/cb)，渲染后自动向外延展成边框；
		// 上/下边件平铺于顶/底带（参数 y=top/bottom 渲染带顶/带底），左右边件
		// 平铺于左/右带（参数 x=left/right 渲染带左/带右）；中心件拉伸铺满
		// 内容框。旧实现把参数当渲染位直传 → 边框整体向内塌陷、板面错位。
		const int16_t left = topleft.x();
		const int16_t top = topleft.y();
		const int16_t right = static_cast<int16_t>(left + box_w);
		const int16_t bottom = static_cast<int16_t>(top + box_h);

		const int16_t cl = static_cast<int16_t>(left + left_border);
		const int16_t cr = static_cast<int16_t>(right - right_border);
		const int16_t ct = static_cast<int16_t>(top + top_border);
		const int16_t cb = static_cast<int16_t>(bottom - bottom_border);

		northwest.draw(DrawArgument(cl, ct));
		northeast.draw(DrawArgument(cr, ct));
		southwest.draw(DrawArgument(cl, cb));
		southeast.draw(DrawArgument(cr, cb));

		// 边件同理：渲染位 = 参数 - origin，故 n/s 参数 y 取 ct/cb（内容框上下缘），
		// w/e 参数 x 取 cl/cr（内容框左右缘），tile 自动外扩进对应边框带。
		for (int16_t x = cl; x < cr; x += xtile)
		{
			north.draw(DrawArgument(x, ct));
			south.draw(DrawArgument(x, cb));
		}

		for (int16_t y = ct; y < cb; y += ytile)
		{
			west.draw(DrawArgument(cl, y));
			east.draw(DrawArgument(cr, y));
		}

		if (cr > cl && cb > ct)
		{
			center.draw(DrawArgument(Point<int16_t>(cl, ct), Point<int16_t>(cr - cl, cb - ct)));
		}

		// 标题「公告」(170x18, origin 0) 画在顶带；正文居中于内容区。
		// R3 十二帧稳态实测（ui_r3/notice_slide_curve.json）：本端 f01..f11
		// title 模板顶恒为 301（ncc=1.0），目标帧「选中频道页.png」为 313
		// ⇒ 偏上 12px。title.draw 的模板顶即 top+K，K=6 时 301 ⇒ top=295，
		// 故 K=18 才落到 313。外框两端框顶仅差 1px（nw 角 T296/O295），
		// 不是整体平移 bug，是标题在框内的内部偏移；切勿改 notice_y。
		// 旧注释「标题行 y≈305-315」系过场帧误量，作废。
		// 正文「Scania」y≈339-350（内容区）。
		// 原生 title 非严格居中：目标 title_left=293、nw_left=280 ⇒ inset=13，
		// 而 (box_w_ref-title.width())/2=(192-170)/2=11 ≠ 13。沿用居中公式会
		// 让标题左偏 2px；用真角件定框宽后必须把 title inset 与框宽解耦。
		const int16_t title_x = static_cast<int16_t>(left + 13);
		title.draw(Point<int16_t>(title_x, static_cast<int16_t>(top + 18)));

		const int16_t text_x = static_cast<int16_t>(cl + (cr - cl) / 2);
		// m08208 ①：measure_r4.json balloon_body_ink 目标墨带 y337..345，本端
		// y341..350（weighted 14.23 vs 16.93）=> 正文比目标低约 4~5px。
		// 只上移正文 text_y，不动 Title(:304)/九宫/notice_y，保 noticeTitle NCC=1.0。
		const int16_t text_y = static_cast<int16_t>(ct + (cb - ct - textlabel.height()) / 2 - 4);
		textlabel.draw(Point<int16_t>(text_x, text_y));
	}
}