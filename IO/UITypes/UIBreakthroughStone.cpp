//////////////////////////////////////////////////////////////////////////////////
// UIBreakthroughStone.cpp — 192 突破石窗实现
//////////////////////////////////////////////////////////////////////////////////
#include "UIBreakthroughStone.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../Components/AreaButton.h"
#include "../../Net/Packets/WitchForestPackets.h"
#include "../../Gameplay/Combat/Damage.h"
#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// WZ canvas origin 相对窗口中心；本端锚点 = 窗口中心 + origin。
	static Point<int16_t> center_anchor(const Point<int16_t>& wpos, int16_t w, int16_t h, int16_t ox, int16_t oy)
	{
		return wpos + Point<int16_t>(static_cast<int16_t>(w / 2 + ox), static_cast<int16_t>(h / 2 + oy));
	}

	UIBreakthroughStone::UIBreakthroughStone()
	{
		dimension = Point<int16_t>(WIN_W, WIN_H);
		dragarea = Point<int16_t>(WIN_W, 24);
		layout_policy = LayoutPolicy::SavedPosClamped;
		info = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "");

#ifdef USE_NX
		nl::node root = nl::nx::UI["UIWindowCN.img"]["CN_Limitbreakthrough"];
		if (UiAssetResolve::usable(root))
		{
			const char* bgs[] = { "backgrnd", "backgrnd2", "backgrnd3", "backgrnd4" };
			for (int i = 0; i < 4; ++i)
				if (UiAssetResolve::usable(root[bgs[i]])) backgrnd[i] = Texture(root[bgs[i]]);
			has_assets = backgrnd[0] ? true : false;

			nl::node fx = root["Effect"]["Normal"];
			for (int i = 0; i < FX_FRAMES; ++i)
				if (UiAssetResolve::usable(fx[std::to_string(i)])) fx_normal[i] = Texture(fx[std::to_string(i)]);

			nl::node gu = root["Effect"]["GradeUp"];
			nl::node us = root["Effect"]["Use"];
			for (int i = 0; i < FX_FRAMES; ++i)
			{
				if (UiAssetResolve::usable(gu[std::to_string(i)])) fx_gradeup.push_back(Texture(gu[std::to_string(i)]));
				if (UiAssetResolve::usable(us[std::to_string(i)])) fx_use.push_back(Texture(us[std::to_string(i)]));
			}

			// 按钮：origin 相对窗口中心 (-146,-256)/(-10,-256)，底板 196x285。
			auto mkbtn = [&](uint16_t id, nl::node n, int16_t ox, int16_t oy) {
				Point<int16_t> anchor = center_anchor(position, WIN_W, WIN_H, ox, oy);
				if (UiAssetResolve::usable(n))
					buttons[id] = std::make_unique<MapleButton>(n, anchor);
				else
					buttons[id] = std::make_unique<AreaButton>(anchor, Point<int16_t>(40, 16));
			};
			mkbtn(BtOk, root["BtOk"], -146, -256);
			mkbtn(BtOnemore, root["BtOnemore"], -10, -256);
		}
#endif
		if (!buttons[BtOk]) buttons[BtOk] = std::make_unique<AreaButton>(Point<int16_t>(WIN_W / 2 - 146 + 98, WIN_H / 2 - 256 + 142), Point<int16_t>(40, 16));
		if (!buttons[BtOnemore]) buttons[BtOnemore] = std::make_unique<AreaButton>(Point<int16_t>(WIN_W / 2 - 10 + 98, WIN_H / 2 - 256 + 142), Point<int16_t>(40, 16));
		if (nl::node close = UiAssetResolve::close_btn())
			buttons[BtClose] = std::make_unique<MapleButton>(close, Point<int16_t>(WIN_W - 20, 2));

		center_if_position_unset(dimension);
		// 当前破功值优先用已同步的服务端值填充，避免开窗瞬间显示 0。
		limitbreak = static_cast<int64_t>(damage::server_limit_break());
		BreakthroughStoneOpenPacket().dispatch();
	}

	void UIBreakthroughStone::draw(float inter) const
	{
		const float fade = fade_alpha();
		const Point<int16_t> org = position;
		for (int i = 0; i < 4; ++i)
			if (backgrnd[i]) backgrnd[i].draw(DrawArgument(org) + fade);

		// 特效锚点：Normal (-10,-62)，GradeUp/Use 依帧 (-10,-26)/(-10,-62)。
		const Point<int16_t> fxanchor = center_anchor(org, WIN_W, WIN_H, -10, -62);
		if (fx_mode == 1 && fx_frame < FX_FRAMES && fx_normal[fx_frame])
			fx_normal[fx_frame].draw(DrawArgument(fxanchor) + fade);
		else if (fx_mode == 2 && fx_frame < fx_gradeup.size())
			fx_gradeup[fx_frame].draw(DrawArgument(fxanchor) + fade);
		else if (fx_mode == 3 && fx_frame < fx_use.size())
			fx_use[fx_frame].draw(DrawArgument(fxanchor) + fade);

		info.draw(DrawArgument(org + Point<int16_t>(10, WIN_H - 40)) + fade);
		UIElement::draw_buttons(inter);
	}

	void UIBreakthroughStone::update()
	{
		UIElement::update();
		if (fx_mode != 0)
		{
			if (++fx_delay >= 5)
			{
				fx_delay = 0;
				++fx_frame;
				const size_t total = (fx_mode == 1) ? FX_FRAMES : (fx_mode == 2 ? fx_gradeup.size() : fx_use.size());
				if (fx_frame >= total)
				{
					fx_frame = 0;
					fx_mode = 0;   // 播放完毕回到空闲
				}
			}
		}
	}

	void UIBreakthroughStone::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) close();
	}

	Button::State UIBreakthroughStone::button_pressed(uint16_t buttonid)
	{
		if (buttonid == BtClose) { close(); return Button::State::PRESSED; }
		if (buttonid == BtOk || buttonid == BtOnemore)
		{
			// 使用突破石：优先用背包中数量>0 的已配置突破石（服务端按 itemId 校验）。
			// client 不硬编码 itemId：发 0 让服务端从配置表挑第一个可用突破石。
			BreakthroughStoneUsePacket(0).dispatch();
			fx_mode = 1;
			fx_frame = 0;
			return Button::State::PRESSED;
		}
		return Button::State::NORMAL;
	}

	void UIBreakthroughStone::apply_state(int64_t lb, int32_t count)
	{
		limitbreak = lb;
		stonecount = count;
		has_snapshot = true;
		info.change_text("当前破功值：" + std::to_string(lb) + "  突破石：" + std::to_string(count));
	}

	void UIBreakthroughStone::apply_result(bool success, int64_t before, int64_t after, int32_t remaining)
	{
		limitbreak = after;
		stonecount = remaining;
		fx_mode = success ? 2 : 3;
		fx_frame = 0;
		fx_delay = 0;
		if (success)
			info.change_text("突破成功！" + std::to_string(before) + " → " + std::to_string(after));
		else
			info.change_text("突破失败，破功值保持不变。");
	}

	void UIBreakthroughStone::close()
	{
		deactivate();
	}

	UIElement::Type UIBreakthroughStone::get_type() const
	{
		return TYPE;
	}
}
