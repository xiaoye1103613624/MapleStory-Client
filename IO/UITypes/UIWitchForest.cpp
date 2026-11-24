//////////////////////////////////////////////////////////////////////////////////
// UIWitchForest.cpp — 192 魔女森林抽奖窗 · 全链路接入实现
//////////////////////////////////////////////////////////////////////////////////
#include "UIWitchForest.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../Components/AreaButton.h"
#include "../../Net/Packets/WitchForestPackets.h"
#include "../../Data/ItemData.h"
#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// WZ canvas origin 相对窗口中心；本端锚点 = 窗口中心 + origin。
	static Point<int16_t> wf_center_anchor(const Point<int16_t>& wpos, int16_t w, int16_t h, int16_t ox, int16_t oy)
	{
		return wpos + Point<int16_t>(static_cast<int16_t>(w / 2 + ox), static_cast<int16_t>(h / 2 + oy));
	}

	UIWitchForest::UIWitchForest()
	{
		dimension = Point<int16_t>(WIN_W, WIN_H);
		dragarea = Point<int16_t>(WIN_W, 24);
		layout_policy = LayoutPolicy::SavedPosClamped;
		info = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "");
		result_text = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW, "");

#ifdef USE_NX
		nl::node root = nl::nx::UI["UIWindowCN.img"]["CN_50100"];
		if (UiAssetResolve::usable(root))
		{
			if (UiAssetResolve::usable(root["backgrnd"]))
			{
				backgrnd = Texture(root["backgrnd"]);
				has_assets = true;
				dimension = Point<int16_t>(backgrnd.width(), backgrnd.height());
				dragarea = Point<int16_t>(dimension.x(), 24);
			}
			// 奖励底槽
			const char* bg50[] = { "layer:reward50Bg0", "layer:reward50Bg1", "layer:reward50Bg2", "layer:reward50Bg3", "layer:reward50Bg4" };
			const char* bg100[] = { "layer:reward100Bg0", "layer:reward100Bg1", "layer:reward100Bg2", "layer:reward100Bg3", "layer:reward100Bg4" };
			for (int i = 0; i < 5; ++i)
			{
				if (UiAssetResolve::usable(root[bg50[i]])) reward_bg_normal[i] = Texture(root[bg50[i]]);
				if (UiAssetResolve::usable(root[bg100[i]])) reward_bg_rare[i] = Texture(root[bg100[i]]);
			}
			// Grade / Type / Number
			for (int i = 0; i < 5; ++i)
			{
				if (UiAssetResolve::usable(root["Grade"][i])) grade_icon[i] = Texture(root["Grade"][i]);
				if (UiAssetResolve::usable(root["Type"][i])) type_icon[i] = Texture(root["Type"][i]);
			}
			for (int i = 0; i < 10; ++i)
				if (UiAssetResolve::usable(root["Number"][i])) number_glyph[i] = Texture(root["Number"][i]);
			// Timer 0..9 + ':'
			for (int i = 0; i < 10; ++i)
				if (UiAssetResolve::usable(root["Timer"][i])) timer_glyph[i] = Texture(root["Timer"][i]);
			if (UiAssetResolve::usable(root["Timer"][":"])) timer_glyph[10] = Texture(root["Timer"][":"]);
			// miracleTime / specialRewardCover
			if (UiAssetResolve::usable(root["layer:miracleTime"])) miracle_time = Texture(root["layer:miracleTime"]);
			if (UiAssetResolve::usable(root["specialRewardCover0"])) special_cover[0] = Texture(root["specialRewardCover0"]);
			if (UiAssetResolve::usable(root["specialRewardCover1"])) special_cover[1] = Texture(root["specialRewardCover1"]);

			// popUp（必须逐级访问，nlnx operator[] 不支持 'popUp/button:confirm' 斜杠路径）
			nl::node popup = root["popUp"];
			if (UiAssetResolve::usable(popup["backgrnd"])) popup_bg = Texture(popup["backgrnd"]);
			// Notice
			nl::node notice = root["Notice"];
			if (UiAssetResolve::usable(notice["backgrnd"])) notice_bg = Texture(notice["backgrnd"]);

			// Result 演出窗（逐级）
			nl::node result = root["Result"];
			if (UiAssetResolve::usable(result["backgrnd"])) result_bg = Texture(result["backgrnd"]);
			if (UiAssetResolve::usable(result["layer:frame"])) result_frame = Texture(result["layer:frame"]);
			if (UiAssetResolve::usable(result["layer:title50"])) result_title[0] = Texture(result["layer:title50"]);
			if (UiAssetResolve::usable(result["layer:title100"])) result_title[1] = Texture(result["layer:title100"]);
			for (int i = 0; i < 5; ++i)
				if (UiAssetResolve::usable(result["rewardSlot"][i])) result_slot[i] = Texture(result["rewardSlot"][i]);
			const char* rbg[] = { "layer:rewardBg0", "layer:rewardBg1", "layer:rewardBg2" };
			for (int i = 0; i < 3; ++i)
				if (UiAssetResolve::usable(result[rbg[i]])) result_rewardbg[i] = Texture(result[rbg[i]]);
			nl::node fx = result["Effect"];
			nl::node fxss = result["EffectSS"];
			for (int i = 0; i < FX_FRAMES; ++i)
			{
				if (UiAssetResolve::usable(fx[i])) result_fx[i] = Texture(fx[i]);
				if (UiAssetResolve::usable(fxss[i])) result_fxss[i] = Texture(fxss[i]);
			}

			auto mkbtn = [&](uint16_t id, nl::node n, int16_t ox, int16_t oy, int16_t w, int16_t h) {
				Point<int16_t> anchor = wf_center_anchor(position, WIN_W, WIN_H, ox, oy);
				if (UiAssetResolve::usable(n)) buttons[id] = std::make_unique<MapleButton>(n, anchor);
				else buttons[id] = std::make_unique<AreaButton>(anchor, Point<int16_t>(w, h));
			};
			mkbtn(Bt50,     root["button:50"],    -108, -636, 126, 31);
			mkbtn(Bt50x10,  root["button:50X10"], -239, -635, 128, 33);
			mkbtn(Bt100,    root["button:100"],   -431, -636, 126, 31);
			mkbtn(Bt100x10, root["button:100X10"],-566, -635, 128, 33);
			mkbtn(BtHelp,   root["button:Help"],  -698,  -49,  12, 15);
			mkbtn(BtNotice, root["button:Notice"],-683,  -51,  14, 15);
			mkbtn(BtClose,  root["button:Close"], -712,  -50,  15, 15);
			mkbtn(BtPopConfirm,  popup["button:confirm"],  -92, -90, 87, 30);
			mkbtn(BtPopCancel,   popup["button:cancel"],  -211, -90, 87, 30);
			mkbtn(BtPopConfirm2, popup["button:confirm2"],-154, -90, 87, 30);
			mkbtn(BtResultOK,    result["button:OK"],    -333,-547, 135, 57);
		}
#endif
		auto fallback = [&](uint16_t id, int16_t ox, int16_t oy, int16_t w, int16_t h) {
			if (!buttons[id]) buttons[id] = std::make_unique<AreaButton>(
				wf_center_anchor(position, WIN_W, WIN_H, ox, oy), Point<int16_t>(w, h));
		};
		fallback(Bt50, -108, -636, 126, 31);
		fallback(Bt50x10, -239, -635, 128, 33);
		fallback(Bt100, -431, -636, 126, 31);
		fallback(Bt100x10, -566, -635, 128, 33);
		fallback(BtClose, -712, -50, 15, 15);
		fallback(BtPopConfirm, -92, -90, 87, 30);
		fallback(BtPopCancel, -211, -90, 87, 30);
		fallback(BtResultOK, -333, -547, 135, 57);

		center_if_position_unset(dimension);
		WitchForestOpenPacket().dispatch();
	}

	// 用 Number/Timer 图集绘制数字（等宽推进）
	void UIWitchForest::draw_number(const Point<int16_t>& org, int32_t value, float fade) const
	{
		if (value < 0) value = 0;
		std::string s = std::to_string(value);
		int16_t x = 0;
		for (char ch : s)
		{
			int idx = ch - '0';
			if (idx >= 0 && idx < 10 && number_glyph[idx])
			{
				number_glyph[idx].draw(DrawArgument(org + Point<int16_t>(x, 0)) + fade);
				x += static_cast<int16_t>(number_glyph[idx].width());
			}
		}
	}

	// Timer 倒计时 "M:SS" 用 Timer 图集绘制
	void UIWitchForest::draw_timer(const Point<int16_t>& org, float fade) const
	{
		if (secondsleft <= 0) return;
		int32_t m = secondsleft / 60;
		int32_t s = secondsleft % 60;
		int16_t x = 0;
		auto put = [&](int idx) {
			if (idx >= 0 && idx < 11 && timer_glyph[idx])
			{
				timer_glyph[idx].draw(DrawArgument(org + Point<int16_t>(x, 0)) + fade);
				x += static_cast<int16_t>(timer_glyph[idx].width());
			}
		};
		put(m);
		put(10); // ':'
		put(s / 10);
		put(s % 10);
	}

	void UIWitchForest::draw(float inter) const
	{
		const float fade = fade_alpha();
		const Point<int16_t> org = position;

		// ---- 主窗 ----
		if (has_assets && backgrnd && !show_notice && !show_result) backgrnd.draw(DrawArgument(org) + fade);

		if (!show_notice && !show_result)
		{
			const int16_t specs[5][2] = { {-81,-401},{-237,-401},{-81,-520},{-184,-520},{-287,-520} };
			const int16_t rare_specs[5][2] = { {-406,-401},{-562,-401},{-406,-520},{-509,-520},{-612,-520} };
			for (int i = 0; i < 5; ++i)
			{
				Point<int16_t> p = wf_center_anchor(org, WIN_W, WIN_H, specs[i][0], specs[i][1] + 60);
				if (reward_bg_normal[i]) reward_bg_normal[i].draw(DrawArgument(p) + fade);
				Point<int16_t> rp = wf_center_anchor(org, WIN_W, WIN_H, rare_specs[i][0], rare_specs[i][1] + 60);
				if (reward_bg_rare[i]) reward_bg_rare[i].draw(DrawArgument(rp) + fade);
			}
			// 奖励 icon + 评级/类型（最多 5）
			const size_t n = result_itemids.size() < 5 ? result_itemids.size() : 5;
			for (size_t i = 0; i < n; ++i)
			{
				Texture icon = ItemData::get(result_itemids[i]).get_icon(false);
				Point<int16_t> slotp = wf_center_anchor(org, WIN_W, WIN_H, specs[i][0], specs[i][1] + 60);
				if (icon) icon.draw(DrawArgument(slotp + Point<int16_t>(20, 30)) + fade);
				if (i < result_grades.size() && result_grades[i] >= 0 && result_grades[i] < 5 && grade_icon[result_grades[i]])
					grade_icon[result_grades[i]].draw(DrawArgument(slotp + Point<int16_t>(110, 20)) + fade);
				if (i < result_grades.size() && result_grades[i] >= 0 && result_grades[i] < 5 && type_icon[result_grades[i]])
					type_icon[result_grades[i]].draw(DrawArgument(slotp + Point<int16_t>(20, 50)) + fade);
			}
			// 特殊奖励封面（有评级 4 时）
			bool has_special = false;
			for (int8_t g : result_grades) if (g >= 4) { has_special = true; break; }
			if (has_special && special_cover[0])
				special_cover[0].draw(DrawArgument(wf_center_anchor(org, WIN_W, WIN_H, 0, -231)) + fade);
			// 奇迹时间特效
			if (miracle > 0 && miracle_time)
				miracle_time.draw(DrawArgument(wf_center_anchor(org, WIN_W, WIN_H, -58, -231)) + fade);
			// 次数（Number）+ 倒计时（Timer）
			draw_number(org + Point<int16_t>(40, WIN_H - 90), totaldraws, fade);
			draw_timer(wf_center_anchor(org, WIN_W, WIN_H, 250, -60), fade);
			info.draw(DrawArgument(org + Point<int16_t>(20, WIN_H - 60)) + fade);
			result_text.draw(DrawArgument(org + Point<int16_t>(20, WIN_H - 42)) + fade);
		}

		// ---- Notice 公告层 ----
		if (show_notice && notice_bg)
		{
			notice_bg.draw(DrawArgument(org) + fade);
			Text notice_text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::BLACK,
				"魔女森林：消耗点券探险，普通池 50 / 高级池 100，X10 为十连。"
				"抽取可获得珍贵道具，集齐向导妖精可进入封印的魔女森林。");
			notice_text.draw(DrawArgument(org + Point<int16_t>(40, 80)) + fade);
		}

		// ---- popUp 二次确认 ----
		if (show_popup && popup_bg)
		{
			Point<int16_t> pp = wf_center_anchor(org, WIN_W, WIN_H, 0, 0);
			popup_bg.draw(DrawArgument(pp) + fade);
			Text q(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::BLACK, "确认消耗点券进行探险？");
			q.draw(DrawArgument(pp + Point<int16_t>(60, 40)) + fade);
		}

		// ---- Result 演出窗 ----
		if (show_result)
		{
			if (result_bg) result_bg.draw(DrawArgument(org) + fade);
			if (result_frame) result_frame.draw(DrawArgument(org) + fade);
			int titleIdx = (result_drawtype >= 2) ? 1 : 0;
			if (result_title[titleIdx])
				result_title[titleIdx].draw(DrawArgument(wf_center_anchor(org, WIN_W, WIN_H,
					titleIdx ? -280 : -304, titleIdx ? -129 : -128)) + fade);
			// 7 奖励槽（layer:rewardBg0/1/2 + 复用 3..6）
			const int16_t rbg[7][2] = { {-226,-229},{-349,-229},{-472,-229},{-164,-330},{-287,-330},{-410,-330},{-532,-330} };
			for (int i = 0; i < 7; ++i)
			{
				const Texture& bg = (i <= 2) ? result_rewardbg[i < 2 ? 0 : 2] : result_rewardbg[2];
				Point<int16_t> rp = wf_center_anchor(org, WIN_W, WIN_H, rbg[i][0], rbg[i][1]);
				if (bg) bg.draw(DrawArgument(rp) + fade);
				if (i < 5 && result_slot[i])
					result_slot[i].draw(DrawArgument(rp) + fade);
				if (i < static_cast<int>(result_itemids.size()))
				{
					Texture icon = ItemData::get(result_itemids[i]).get_icon(false);
					if (icon) icon.draw(DrawArgument(rp + Point<int16_t>(20, 20)) + fade);
				}
			}
			// 特效动画：Effect 65 帧（普通）/ EffectSS 65 帧（特殊）
			Point<int16_t> fxanchor = wf_center_anchor(org, WIN_W, WIN_H, -70, -60);
			const Texture* fxset = result_special ? result_fxss : result_fx;
			if (fx_frame < FX_FRAMES && fxset[fx_frame]) fxset[fx_frame].draw(DrawArgument(fxanchor) + fade);
			if (result_special && special_cover[1]) special_cover[1].draw(DrawArgument(fxanchor) + fade);
			result_text.draw(DrawArgument(org + Point<int16_t>(20, WIN_H - 42)) + fade);
		}

		UIElement::draw_buttons(inter);
	}

	void UIWitchForest::update()
	{
		UIElement::update();
		// 倒计时：每秒递减本地显示，服务端 snapshot 会校正
		if (secondsleft > 0 && (++draw_tick % 100) == 0) --secondsleft;
		if (show_result && fx_frame < FX_FRAMES)
		{
			if (++fx_delay >= 3) { fx_delay = 0; ++fx_frame; }
		}
	}

	void UIWitchForest::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (escape)
		{
			if (show_result) { show_result = false; return; }
			if (show_notice) { show_notice = false; return; }
			if (show_popup) { show_popup = false; pending_drawtype = -1; return; }
			close();
		}
	}

	Button::State UIWitchForest::button_pressed(uint16_t buttonid)
	{
		if (buttonid == BtClose) { close(); return Button::State::PRESSED; }
		if (buttonid == BtResultOK) { show_result = false; return Button::State::PRESSED; }
		if (buttonid == BtHelp || buttonid == BtNotice)
		{
			if (buttonid == BtNotice) show_notice = true;
			else info.change_text("魔女森林：消耗点券探险，可获得特别道具。50/100 为单抽，X10 为十连抽。");
			return Button::State::PRESSED;
		}
		if (buttonid >= Bt50 && buttonid <= Bt100x10)
		{
			pending_drawtype = static_cast<int8_t>(buttonid - Bt50);
			show_popup = true;
			return Button::State::PRESSED;
		}
		if (buttonid == BtPopConfirm || buttonid == BtPopConfirm2)
		{
			if (pending_drawtype >= 0)
			{
				WitchForestDrawPacket(pending_drawtype).dispatch();
				pending_drawtype = -1;
			}
			show_popup = false;
			return Button::State::PRESSED;
		}
		if (buttonid == BtPopCancel)
		{
			pending_drawtype = -1;
			show_popup = false;
			return Button::State::PRESSED;
		}
		return Button::State::NORMAL;
	}

	Cursor::State UIWitchForest::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		for (auto& kv : buttons)
			if (kv.second && kv.second->is_active() && kv.second->bounds(position).contains(cursorpos))
				return UIElement::send_cursor(clicked, cursorpos);
		return UIDragElement::send_cursor(clicked, cursorpos);
	}

	void UIWitchForest::remove_cursor()
	{
		UIDragElement::remove_cursor();
	}

	void UIWitchForest::refresh_result_text()
	{
		if (result_itemids.empty()) { result_text.change_text(""); return; }
		std::string s = "获得：";
		for (size_t i = 0; i < result_itemids.size(); ++i)
		{
			if (i) s += "、";
			s += std::to_string(result_itemids[i]);
			if (i < result_qtys.size() && result_qtys[i] > 1) s += "x" + std::to_string(result_qtys[i]);
		}
		result_text.change_text(s);
	}

	void UIWitchForest::apply_state(int64_t nx, int32_t draws, int32_t secs, int8_t mrc)
	{
		nxcredit = nx;
		totaldraws = draws;
		secondsleft = secs;
		miracle = mrc;
		has_snapshot = true;
		info.change_text("点券：" + std::to_string(nx) + "  累计探险：" + std::to_string(draws));
	}

	void UIWitchForest::apply_result(int8_t drawtype, int32_t count,
		const std::vector<int32_t>& itemids, const std::vector<int32_t>& qtys,
		const std::vector<int8_t>& grades, int64_t nxafter, bool special)
	{
		nxcredit = nxafter;
		result_itemids = itemids;
		result_qtys = qtys;
		result_grades = grades;
		result_drawtype = drawtype;
		result_special = special;
		totaldraws += count;
		show_result = true;
		fx_frame = 0;
		fx_delay = 0;
		refresh_result_text();
		info.change_text("探险完成！剩余点券：" + std::to_string(nxafter));
	}

	void UIWitchForest::close()
	{
		remove_cursor();
		deactivate();
	}

	UIElement::Type UIWitchForest::get_type() const
	{
		return TYPE;
	}
}
