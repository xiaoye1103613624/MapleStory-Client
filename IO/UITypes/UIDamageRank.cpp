//////////////////////////////////////////////////////////////////////////////////
// UIDamageRank — 个人伤害统计窗完整实现（对齐 Tengutake 参考包）
//
// 渲染分层（与参考 BuildCanvas 一致）：
//   背景 4 段 → 头部 5 钮 → title1/2 → 内容页（玩家 gauge 条 / 技能行）→ 滚动条。
// 所有文本走 Locale::text，缺资源时降级为纯色矩形 + 中文描边文字，保证可读不崩。
// 控制包 0x3713 在 send_control 内直发：open=1（UI 打开时）、reset=2、close=3。
//////////////////////////////////////////////////////////////////////////////////
#include "UIDamageRank.h"

#include "../UI.h"
#include "../Components/AreaButton.h"
#include "../Components/Button.h"
#include "../../Gameplay/Stage.h"
#include "../../Data/SkillData.h"
#include "../../Net/OutPacket.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Locale.h"
#include "../../MapleStory.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <sstream>
#include <string>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		// 控制包：s9/Tengutake kDamageRankControlOpcode = 0x3713（body = 1 字节 action）。
		constexpr int16_t kDamageRankControlOpcode = 0x3713;

		// OutPacket::write_byte 是 protected：按项目惯例（Net/Packets/*Packets.h）
		// 用派生结构在构造函数里写 body，避免直接调受保护成员。
		struct DamageRankControlPacket : public OutPacket
		{
			DamageRankControlPacket(uint8_t action) : OutPacket(kDamageRankControlOpcode)
			{
				write_byte(static_cast<int8_t>(action));
			}
		};
		constexpr uint8_t kControlOpen = 1;
		constexpr uint8_t kControlReset = 2;
		constexpr uint8_t kControlClose = 3;

		// 千分位；服务端给的是 uint64 累计伤害。
		std::string fmt_num(uint64_t v)
		{
			const std::string s = std::to_string(v);
			std::string out;
			out.reserve(s.size() + s.size() / 3);
			for (size_t i = 0; i < s.size(); ++i)
			{
				if (i && (s.size() - i) % 3 == 0)
					out.push_back(',');
				out.push_back(s[i]);
			}
			return out;
		}

		// 中文文案兜底：Locale 缺键时返回给定中文（Locale::text 缺键会回显 key）。
		std::string cn(const char* key, const char* fallback)
		{
			std::string s = Locale::text(key);
			return s == key ? std::string(fallback) : s;
		}

		// 颜色分量（0..255）→ DrawArgument 用 Color。
		Color rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
		{
			return Color(r, g, b, a);
		}
	}

	UIDamageRank::UIDamageRank()
	{
		// 自适应高度（参考 :186-202 GetPlayerLayerHeight/GetSkillLayerHeight）：
		// 初始无数据 → 仅头部+底边的高度；reload 后按可见行数增高。
		// 画布宽固定 kLayerWidth，高由 current_height() 决定。
		dimension = Point<int16_t>(kLayerWidth, current_height());
		// 头部 24px 为拖拽区（参考 HitTestHeaderDragArea 用 Y<24）。
		dragarea = Point<int16_t>(kLayerWidth, 24);
		layout_policy = LayoutPolicy::SavedPosClamped;

		empty_hint = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::DARKGREY, "");

		load_assets();

		// 5 颗按钮用 AreaButton 命中（与参考 HitTestHeaderButton 的矩形一致），
		// 视觉由 draw_buttons_manual 按四态纹理自绘；这样状态机完全可控，
		// 也不依赖 MapleButton 的 origin（参考资源缺态时会错位）。
		const BtnId ids[] = { kBtnReset, kBtnSwitch, kBtnAuto, kBtnMinMax, kBtnClose };
		for (BtnId id : ids)
		{
			const Rectangle<int16_t> rc = button_rect(static_cast<BtnId>(id));
			buttons[static_cast<uint16_t>(id)] = std::make_unique<AreaButton>(
				Point<int16_t>(rc.left(), rc.top()), Point<int16_t>(rc.width(), rc.height()));
		}

		// 首次打开按当前（自适应）高度居中；用户拖动过则沿用保存坐标。
		center_if_position_unset(Point<int16_t>(kLayerWidth, current_height()));
		rebuild_rows();

		// 参考 ToggleByHotkey：可见时先发 open 通知服务端上报快照。
		send_control(kControlOpen);
		LOG(LOG_INFO, "UIDamageRank: open, control=0x3713/1, gauge=" << gauge_ready
			<< " scroll=" << scroll_ready);
	}

	void UIDamageRank::load_assets()
	{
		if (assets_load_tried)
			return;
		assets_load_tried = true;

#ifdef USE_NX
		// 素材根：优先 UIWindow2 的 DamageRank，再落回 UIWindow（与 UiAssetResolve::window 一致）。
		nl::node dr = nl::nx::UI["UIWindow.img"]["DamageRank"];
		if (!dr)
			dr = UiAssetResolve::window("DamageRank");   // ThemePack 兜底

		// 背景 4 段：max/min/center/bottom 为必须，bottomover 为可选（hover 底边）。
		load_tex(bg_max, dr["backgrndmax"]);
		load_tex(bg_min, dr["backgrndmin"]);
		load_tex(bg_center, dr["backgrndcenter"]);
		load_tex(bg_bottom, dr["backgrndbottom"]);
		load_tex(bg_bottom_over, dr["backgrndbottomover"]);

		// title1/title2（:530-531）：玩家页 / 技能页标题条。
		load_tex(title1, dr["title1"]);
		load_tex(title2, dr["title2"]);

		// gauge 六种（:533-539）：任一缺失即整组降级为纯色条。
		load_tex(gauge_common_l, dr["gauge"]["commonl"]);
		load_tex(gauge_common_c, dr["gauge"]["commonc"]);
		load_tex(gauge_common_r, dr["gauge"]["commonr"]);
		load_tex(gauge_self_l, dr["gauge"]["selfl"]);
		load_tex(gauge_self_c, dr["gauge"]["selfc"]);
		load_tex(gauge_self_r, dr["gauge"]["selfr"]);
		gauge_ready = gauge_common_l && gauge_common_c && gauge_common_r &&
			gauge_self_l && gauge_self_c && gauge_self_r;

		load_tex(icon_common_atk, dr["iconCommonAtk"]);
		load_tex(icon_unknown_skill, dr["iconUnknownSkill"]);

		// 5 颗按钮四态：Reset/Switch/Auto 在 DamageRank 下，Min/Max/Close 在 Basic.img。
		// 缺态由 draw_buttons_manual 按“hover→pressed→disabled→normal→纯色”链降级。
		load_tex(btn_tex[kBtnReset][Button::State::NORMAL], dr["BtReset"]["normal"]["0"]);
		load_tex(btn_tex[kBtnReset][Button::State::MOUSEOVER], dr["BtReset"]["mouseOver"]["0"]);
		load_tex(btn_tex[kBtnReset][Button::State::PRESSED], dr["BtReset"]["pressed"]["0"]);
		load_tex(btn_tex[kBtnReset][Button::State::DISABLED], dr["BtReset"]["disabled"]["0"]);

		load_tex(btn_tex[kBtnSwitch][Button::State::NORMAL], dr["BtSwitch"]["normal"]["0"]);
		load_tex(btn_tex[kBtnSwitch][Button::State::MOUSEOVER], dr["BtSwitch"]["mouseOver"]["0"]);
		load_tex(btn_tex[kBtnSwitch][Button::State::PRESSED], dr["BtSwitch"]["pressed"]["0"]);
		load_tex(btn_tex[kBtnSwitch][Button::State::DISABLED], dr["BtSwitch"]["disabled"]["0"]);

		load_tex(btn_tex[kBtnAuto][Button::State::NORMAL], dr["BtAuto"]["normal"]["0"]);
		load_tex(btn_tex[kBtnAuto][Button::State::MOUSEOVER], dr["BtAuto"]["mouseOver"]["0"]);
		load_tex(btn_tex[kBtnAuto][Button::State::PRESSED], dr["BtAuto"]["pressed"]["0"]);
		load_tex(btn_tex[kBtnAuto][Button::State::DISABLED], dr["BtAuto"]["disabled"]["0"]);

		// MinMax 展开态 = BtMin，最小化态 = BtMax；两者都用 Basic.img（参考 :571-588）。
		nl::node basic = nl::nx::UI["Basic.img"];
		load_tex(btn_tex[kBtnMinMax][Button::State::NORMAL], basic["BtMin"]["normal"]["0"]);
		load_tex(btn_tex[kBtnMinMax][Button::State::MOUSEOVER], basic["BtMin"]["mouseOver"]["0"]);
		load_tex(btn_tex[kBtnMinMax][Button::State::PRESSED], basic["BtMin"]["pressed"]["0"]);
		load_tex(btn_max_tex[Button::State::NORMAL], basic["BtMax"]["normal"]["0"]);
		load_tex(btn_max_tex[Button::State::MOUSEOVER], basic["BtMax"]["mouseOver"]["0"]);
		load_tex(btn_max_tex[Button::State::PRESSED], basic["BtMax"]["pressed"]["0"]);

		load_tex(btn_tex[kBtnClose][Button::State::NORMAL], basic["BtClose"]["normal"]["0"]);
		load_tex(btn_tex[kBtnClose][Button::State::MOUSEOVER], basic["BtClose"]["mouseOver"]["0"]);
		load_tex(btn_tex[kBtnClose][Button::State::PRESSED], basic["BtClose"]["pressed"]["0"]);

		// 滚动条 Basic.img/VScr4/enabled（:517-528）：三态 prev/next/thumb + base。
		nl::node scr = basic["VScr4"]["enabled"];
		load_tex(scr_prev[0], scr["prev0"]);
		load_tex(scr_prev[1], scr["prev1"]);
		load_tex(scr_prev[2], scr["prev2"]);
		load_tex(scr_next[0], scr["next0"]);
		load_tex(scr_next[1], scr["next1"]);
		load_tex(scr_next[2], scr["next2"]);
		load_tex(scr_thumb[0], scr["thumb0"]);
		load_tex(scr_thumb[1], scr["thumb1"]);
		load_tex(scr_thumb[2], scr["thumb2"]);
		load_tex(scr_base, scr["base"]);
		scroll_ready = scr_base || scr_thumb[0];

		LOG(LOG_INFO, "UIDamageRank assets: bg=" << (bg_max ? 1 : 0)
			<< (bg_min ? 1 : 0) << (bg_center ? 1 : 0) << (bg_bottom ? 1 : 0)
			<< " title=" << (title1 ? 1 : 0) << (title2 ? 1 : 0)
			<< " gauge=" << (gauge_ready ? 1 : 0)
			<< " scroll=" << (scroll_ready ? 1 : 0));
#else
		LOG(LOG_WARNING, "UIDamageRank: USE_NX off, using pure-color fallback");
#endif
	}

	bool UIDamageRank::load_tex(Texture& out, nl::node n)
	{
		if (!n || n.data_type() != nl::node::type::bitmap)
			return false;
		out = Texture(n);
		return out ? true : false;
	}

	void UIDamageRank::draw(float inter) const
	{
		const float fade = fade_alpha();
		const Point<int16_t> org = position;

		draw_background(org, fade);
		// 头部按钮 + title 条（参考 DrawHeader / DrawTitleBand 顺序）。
		draw_buttons_manual(org, fade);
		if (!minimized)
		{
			draw_title_band(org, fade);
			if (mode == Mode::Player)
				draw_player_page(org, fade);
			else
				draw_skill_page(org, fade);
			draw_scrollbar(org, fade);
		}

		// AreaButton 不产生可见像素，先用基类推进 fade_in。
		UIElement::draw_buttons(inter);
	}

	void UIDamageRank::update()
	{
		if (pressed_flash > 0)
			--pressed_flash;

		// 自适应高度：数据行数可能在两次 reload 之间继续累积（伤害持续增长），
		// 每帧按当前行数同步画布高度，保证拖拽/裁剪边界与绘制出的背景一致。
		if (!minimized)
			refresh_dimension();

		UIElement::update();
	}

	Cursor::State UIDamageRank::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		// 拖拽优先（UIDragElement 处理），但滚动 thumb 按下时不拖窗。
		if (thumb_dragging)
		{
			if (!clicked)
			{
				thumb_dragging = false;
				return Cursor::State::IDLE;
			}
			const int16_t x = static_cast<int16_t>(cursorpos.x() - position.x());
			const int16_t y = static_cast<int16_t>(cursorpos.y() - position.y());
			if (x >= 0 && x < kLayerWidth && has_scroll())
				set_scroll_from_thumb_y(y);
			return Cursor::State::CLICKING;
		}

		const int16_t x = static_cast<int16_t>(cursorpos.x() - position.x());
		const int16_t y = static_cast<int16_t>(cursorpos.y() - position.y());

		// 滚动条命中：优先于按钮与拖拽。
		if (has_scroll() && x >= kScrollX && x < kScrollX + kScrollWidth &&
			y >= kRowsStartY && y < current_height() - kLayerBottomHeight)
		{
			const ScrollPart part = hit_test_scroll(x, y);
			hovered_scroll = part;
			if (clicked)
			{
				if (part == ScrollPart::Prev)
					scroll -= 1;
				else if (part == ScrollPart::Next)
					scroll += 1;
				else if (part == ScrollPart::Thumb)
					thumb_dragging = true;
				else if (part == ScrollPart::Track)
				{
					// 点击轨道：thumb 中心跳到点击处。
					set_scroll_from_thumb_y(
						y - (scroll_thumb_rect().bottom() - scroll_thumb_rect().top()) / 2);
				}
				clamp_scroll();
				rebuild_rows();
			}
			return part == ScrollPart::None ? Cursor::State::IDLE : Cursor::State::CANCLICK;
		}
		hovered_scroll = ScrollPart::None;

		// 头部按钮命中：只走 UIElement 按钮状态机，不触发窗口拖拽
		// （参考 HitTestHeaderDragArea 把按钮矩形从拖拽区中排除）。
		if (y >= 0 && y < 24)
		{
			BtnId id = kBtnNone;
			if (hit_test_button(x, y, id))
			{
				hovered_btn = id;
				return UIElement::send_cursor(clicked, cursorpos);
			}
		}
		hovered_btn = kBtnNone;
		pressed_btn = kBtnNone;

		return UIDragElement::send_cursor(clicked, cursorpos);
	}

	void UIDamageRank::remove_cursor()
	{
		hovered_btn = kBtnNone;
		pressed_btn = kBtnNone;
		hovered_scroll = ScrollPart::None;
		thumb_dragging = false;
		UIDragElement::remove_cursor();
	}

	void UIDamageRank::send_scroll(double yoffset)
	{
		scroll += (yoffset > 0 ? 1 : -1);
		clamp_scroll();
		rebuild_rows();
	}

	Button::State UIDamageRank::button_pressed(uint16_t buttonid)
	{
		if (buttonid >= 5)
			return Button::State::NORMAL;

		const BtnId id = static_cast<BtnId>(buttonid);
		pressed_btn = id;
		pressed_flash = 6;   // 约 0.1s 的按下视觉，随后自动回 normal
		on_button(id);
		return Button::State::NORMAL;   // 瞬时按下，下一帧回 normal。
	}

	UIElement::Type UIDamageRank::get_type() const
	{
		return TYPE;
	}

	void UIDamageRank::send_key(int32_t keycode, bool pressed, bool escape)
	{
		// ESC 关闭：照 UIDailyGift.cpp:497-505 标准写法。
		// UI.cpp 的 ESC 白名单已含 DAMAGERANK，这里只处理按下态；
		// deactivate 后由 UI 层收起窗口（不复发 0x3713/3，避免与按钮 close 双发）。
		(void)keycode;
		if (pressed && escape)
		{
			deactivate();
			return;
		}
	}

	void UIDamageRank::reload()
	{
		clamp_scroll();
		rebuild_rows();
		// 行数变化后同步高度（自适应窗口随内容增高）。
		refresh_dimension();
	}

	// ── 布局/几何 ──────────────────────────────────────────────────────────────

	int16_t UIDamageRank::current_height() const
	{
		// 最小化仍用 backgrndmin 的 20 高（参考 kLayerHeightMinimized）。
		if (minimized)
			return kLayerHeightMinimized;

		// 自适应高度（参考 GetPlayerLayerHeight/GetSkillLayerHeight :190-202）：
		//   height = kRowsStartY + row_pitch * visible_rows + kLayerBottomHeight
		// 玩家 pitch=23、技能行高=42、kRowsStartY=52、bottom=2。
		// 至少留一行高度，保证无数据时空态提示/标题不被底边裁掉。
		const int16_t rows = visible_rows();
		const int16_t content = static_cast<int16_t>(
			kRowsStartY + row_pitch() * rows + kLayerBottomHeight);
		const int16_t minimum = static_cast<int16_t>(
			kRowsStartY + row_pitch() + kLayerBottomHeight);
		return std::max<int16_t>(content, minimum);
	}

	void UIDamageRank::refresh_dimension()
	{
		// 行数/页签/最小化变化后同步画布高度，并保持拖拽边界合法。
		dimension = Point<int16_t>(kLayerWidth, current_height());
		clamp_drag_position();
	}

	int16_t UIDamageRank::item_count() const
	{
		return static_cast<int16_t>(mode == Mode::Player
			? DamageRank::get().player_count()
			: DamageRank::get().skill_count());
	}

	int16_t UIDamageRank::row_pitch() const
	{
		// 玩家行 21 高 + 2 间隔 = 23；技能行本身 42 高，无额外间隔。
		return mode == Mode::Player ? kPlayerRowPitch : kSkillRowHeight;
	}

	int16_t UIDamageRank::row_height() const
	{
		// 文字垂直居中用的「行内容高度」：玩家 21、技能 42。
		return mode == Mode::Player ? kPlayerRowHeight : kSkillRowHeight;
	}

	int16_t UIDamageRank::visible_rows() const
	{
		// 自适应：显示所有行，上限取参考 kMaxPlayerRows=30 / kMaxSkillRows=15。
		// 窗口随行数增高；参考包明确 "Scrollbar not implemented"，故不裁剪到固定 fit。
		const int16_t cap = mode == Mode::Player ? kMaxPlayerRows : kMaxSkillRows;
		return std::min<int16_t>(item_count(), cap);
	}

	int16_t UIDamageRank::max_scroll() const
	{
		// 参考包不实现滚动条：所有可见行数内都能显示，超出上限的部分直接不画（不滚动）。
		return 0;
	}

	bool UIDamageRank::has_scroll() const
	{
		// 参考 :818/946/1009 "Scrollbar not implemented"：常驻隐藏。
		// 保留滚动条资源/命中代码但永不显示，避免回退既有四态/gauge 链路。
		return false;
	}

	void UIDamageRank::clamp_scroll()
	{
		// 无滚动条：首行偏移恒为 0。
		scroll = 0;
	}

	int16_t UIDamageRank::centered_ink_top(int16_t row_y, int16_t row_h) const
	{
		// 目标墨迹顶按行高中线：让字形墨迹块在行内上下留白相等。
		// ink_h 是 A11 单行墨迹高（含 CJK ≈ 11px），不是 linespace/ink_offset；
		// 行高不足墨迹高时退化为贴行顶（不会出现负留白导致上溢）。
		const int16_t ink_h = std::min<int16_t>(kA11InkHeight, row_h);
		return static_cast<int16_t>(row_y + (row_h - ink_h) / 2);
	}

	const Texture* UIDamageRank::skill_icon(int32_t skill_id) const
	{
		// 参考 GetSkillIconCanvas :596-633：
		//   skill_id < 0（DoT 等虚拟类别）→ 不画，返回 nullptr；
		//   skill_id == 0（普通攻击）→ icon_common_atk，缺则 unknown；
		//   skill_id > 0 → SkillData::get(id).get_icon(NORMAL)，miss 回退 unknown。
		if (skill_id < 0)
			return nullptr;

		if (skill_id == 0)
			return icon_common_atk ? &icon_common_atk
				: (icon_unknown_skill ? &icon_unknown_skill : nullptr);

		auto it = skill_icon_cache.find(skill_id);
		if (it != skill_icon_cache.end())
			return it->second ? &it->second
				: (icon_unknown_skill ? &icon_unknown_skill : nullptr);

		// 项目接口：SkillData::get 返回 const&；根未装载时返回静态空壳（图标 invalid）。
		// 只缓存「有效图标」：miss 不写缓存，待 Skill 根装载后下一帧重试即可正常出图
		// （SkillData::get 自身是 O(1) map 查找，miss 重试开销可忽略）。
		const Texture& resolved = SkillData::get(skill_id).get_icon(SkillData::Icon::NORMAL);
		if (!resolved)
			return icon_unknown_skill ? &icon_unknown_skill : nullptr;

		// Texture 持有 nl::bitmap 值副本，可安全跨帧持有；unordered_map 元素地址在
		// rehash 后仍稳定，返回的指针在本次窗口生命周期内有效。
		auto inserted = skill_icon_cache.emplace(skill_id, resolved);
		const Texture& cached = inserted.first->second;
		return &cached;
	}

	Rectangle<int16_t> UIDamageRank::button_rect(BtnId id) const
	{
		// 参考 GetButtonRect :1333-1374：x 来自 :48-52，w 为素材宽度，h 为头部高度。
		switch (id)
		{
		case kBtnReset:  return Rectangle<int16_t>(kResetX,  kResetX + 27, 4, 21);
		case kBtnSwitch: return Rectangle<int16_t>(kSwitchX, kSwitchX + 30, 4, 20);
		case kBtnAuto:   return Rectangle<int16_t>(kAutoX,   kAutoX + 21,  4, 20);
		case kBtnMinMax: return Rectangle<int16_t>(kMinX,    kMinX + 12,   4, 20);
		case kBtnClose:  return Rectangle<int16_t>(kCloseX,  kCloseX + 12,  4, 20);
		default:         return Rectangle<int16_t>(0, 0, 0, 0);
		}
	}

	bool UIDamageRank::hit_test_button(int16_t x, int16_t y, BtnId& out) const
	{
		const BtnId ids[] = { kBtnReset, kBtnSwitch, kBtnAuto, kBtnMinMax, kBtnClose };
		for (BtnId id : ids)
		{
			const Rectangle<int16_t> rc = button_rect(id);
			if (x >= rc.left() && x < rc.right() && y >= rc.top() && y < rc.bottom())
			{
				out = id;
				return true;
			}
		}
		return false;
	}

	Rectangle<int16_t> UIDamageRank::scroll_bar_rect() const
	{
		if (!has_scroll())
			return Rectangle<int16_t>(0, 0, 0, 0);
		const int16_t bottom = static_cast<int16_t>(current_height() - kLayerBottomHeight);
		return Rectangle<int16_t>(kScrollX, kScrollX + kScrollWidth, kRowsStartY,
			std::max<int16_t>(kRowsStartY, bottom));
	}

	Rectangle<int16_t> UIDamageRank::scroll_thumb_rect() const
	{
		const Rectangle<int16_t> bar = scroll_bar_rect();
		const int16_t top = bar.top() + kScrollButtonHeight;
		const int16_t bottom = bar.bottom() - kScrollButtonHeight;
		const int16_t track = static_cast<int16_t>(bottom - top);
		int16_t thumb = static_cast<int16_t>(scr_thumb[0] ? scr_thumb[0].height() : 25);
		if (thumb < kScrollThumbMinHeight)
			thumb = kScrollThumbMinHeight;
		if (thumb > track)
			thumb = track;

		const int16_t mx = max_scroll();
		const int16_t movable = static_cast<int16_t>(track - thumb);
		int16_t ty = top;
		if (mx > 0 && movable > 0)
			ty += static_cast<int16_t>(movable * scroll / mx);
		return Rectangle<int16_t>(bar.left(), bar.right(), ty, static_cast<int16_t>(ty + thumb));
	}

	UIDamageRank::ScrollPart UIDamageRank::hit_test_scroll(int16_t x, int16_t y) const
	{
		const Rectangle<int16_t> bar = scroll_bar_rect();
		if (x < bar.left() || x >= bar.right() || y < bar.top() || y >= bar.bottom())
			return ScrollPart::None;
		if (y < bar.top() + kScrollButtonHeight)
			return ScrollPart::Prev;
		if (y >= bar.bottom() - kScrollButtonHeight)
			return ScrollPart::Next;

		const Rectangle<int16_t> thumb = scroll_thumb_rect();
		if (x >= thumb.left() && x < thumb.right() && y >= thumb.top() && y < thumb.bottom())
			return ScrollPart::Thumb;
		return ScrollPart::Track;
	}

	void UIDamageRank::set_scroll_from_thumb_y(int16_t y)
	{
		const Rectangle<int16_t> bar = scroll_bar_rect();
		const int16_t track_top = static_cast<int16_t>(bar.top() + kScrollButtonHeight);
		const int16_t track_bottom = static_cast<int16_t>(bar.bottom() - kScrollButtonHeight);
		const Rectangle<int16_t> thumb = scroll_thumb_rect();
		const int16_t thumb_h = static_cast<int16_t>(thumb.bottom() - thumb.top());
		const int16_t movable = static_cast<int16_t>(track_bottom - track_top - thumb_h);
		const int16_t mx = max_scroll();
		if (movable <= 0 || mx <= 0)
			return;

		int16_t ty = static_cast<int16_t>(y - track_top);
		ty = std::max<int16_t>(0, std::min<int16_t>(ty, movable));
		scroll = static_cast<int32_t>((ty * mx + movable / 2) / movable);
		clamp_scroll();
		rebuild_rows();
	}

	// ── 绘制 ──────────────────────────────────────────────────────────────────

	void UIDamageRank::draw_background(Point<int16_t> org, float fade) const
	{
		const DrawArgument da = DrawArgument(org) + fade;
		const int16_t h = current_height();

		// 纯色兜底底：任何资源缺失时仍有可读面板。
		ColorBox fallback(kLayerWidth, h, Color::Name::TUNA, 0.86f);
		fallback.draw(DrawArgument(org) + fade);

		if (minimized)
		{
			if (bg_min)
				bg_min.draw(da);
			return;
		}

		// 参考 DrawTiledBackground :1223-1246：max 顶段 + center 竖铺 + bottom 底段。
		if (bg_max)
			bg_max.draw(da);
		if (bg_center)
		{
			int16_t y = kRowsStartY;
			const int16_t bottom_y = static_cast<int16_t>(h - kLayerBottomHeight);
			while (y < bottom_y)
			{
				bg_center.draw(DrawArgument(org + Point<int16_t>(0, y)) + fade);
				y += bg_center.height() > 0 ? bg_center.height() : kPlayerRowHeight;
			}
		}
		const bool over = (hovered_scroll != ScrollPart::None || thumb_dragging) && bg_bottom_over;
		const Texture& bottom = over ? bg_bottom_over : bg_bottom;
		if (bottom)
			bottom.draw(DrawArgument(org + Point<int16_t>(0, h - kLayerBottomHeight)) + fade);
	}

	Texture UIDamageRank::button_texture(BtnId id, Button::State state) const
	{
		if (id == kBtnMinMax && minimized)
		{
			// 最小化显示 BtMax（参考 GetButtonCanvasForDraw :1490-1503）。
			if (btn_max_tex[state])
				return btn_max_tex[state];
			if (btn_max_tex[Button::State::NORMAL])
				return btn_max_tex[Button::State::NORMAL];
		}
		if (btn_tex[id][state])
			return btn_tex[id][state];
		if (btn_tex[id][Button::State::NORMAL])
			return btn_tex[id][Button::State::NORMAL];
		return Texture();
	}

	void UIDamageRank::draw_buttons_manual(Point<int16_t> org, float fade) const
	{
		// 最小化时 Reset/Switch/Auto 视觉禁用（参考 IsButtonVisualDisabled :1419-1428）。
		const BtnId ids[] = { kBtnReset, kBtnSwitch, kBtnAuto, kBtnMinMax, kBtnClose };
		for (BtnId id : ids)
		{
			const Rectangle<int16_t> rc = button_rect(id);
			const bool disabled = minimized && (id == kBtnReset || id == kBtnSwitch || id == kBtnAuto);

			Button::State st = Button::State::NORMAL;
			if (disabled)
				st = Button::State::DISABLED;
			else if (pressed_flash > 0 && pressed_btn == id && hovered_btn == id)
				st = Button::State::PRESSED;
			else if (hovered_btn == id)
				st = Button::State::MOUSEOVER;

			// Auto 关闭态用 disabled 图表达（参考 :1478-1488）。
			if (id == kBtnAuto && !auto_mode && !disabled && st != Button::State::PRESSED)
				st = Button::State::DISABLED;

			const Texture& tex = button_texture(id, st);
			if (tex)
			{
				tex.draw(DrawArgument(org + Point<int16_t>(rc.left(), rc.top())) + fade);
				continue;
			}

			// 缺资源：纯色块 + 中文首字，确保可见可点（优雅降级）。
			const char* label = "?";
			switch (id)
			{
			case kBtnReset:  label = "重"; break;
			case kBtnSwitch: label = "切"; break;
			case kBtnAuto:   label = "自"; break;
			case kBtnMinMax: label = minimized ? "展" : "收"; break;
			case kBtnClose:  label = "×"; break;
			default: break;
			}
			ColorBox box(static_cast<int16_t>(rc.right() - rc.left()), static_cast<int16_t>(rc.bottom() - rc.top()),
				disabled ? Color::Name::DARKGREY : Color::Name::MEDIUMBLUE, 0.9f);
			box.draw(DrawArgument(org + Point<int16_t>(rc.left(), rc.top())) + fade);
			// 兜底字按按钮矩形行高中线定墨迹顶，再走 y_for_ink_top 反解。
			Text t(Text::Font::A11B, Text::Alignment::CENTER, Color::Name::WHITE, label);
			const int16_t btn_ink_top = centered_ink_top(
				rc.top(), static_cast<int16_t>(rc.bottom() - rc.top()));
			t.draw(DrawArgument(org + Point<int16_t>(
				rc.left(), t.y_for_ink_top(btn_ink_top))) + fade);
		}
	}

	void UIDamageRank::draw_title_band(Point<int16_t> org, float fade) const
	{
		// 参考 DrawTitleBand :1582-1591：画在 (8, kTitleY=26)。
		const Texture& title = (mode == Mode::Player) ? title1 : title2;
		if (title)
			title.draw(DrawArgument(org + Point<int16_t>(8, kTitleY)) + fade);
		else
		{
			// 无 title 资源时用已本地化的中文标题兜底；墨迹顶按头部 26px 行高中线。
			Text t(Text::Font::A13B, Text::Alignment::LEFT, Color::Name::YELLOW,
				cn("damagerank.title", "伤害统计"));
			t.draw(DrawArgument(org + Point<int16_t>(8,
				t.y_for_ink_top(centered_ink_top(kTitleY, kHeaderHeight)))) + fade);
		}
	}

	void UIDamageRank::draw_gauge(
		Point<int16_t> org,
		int16_t x,
		int16_t y,
		int16_t width,
		bool self_style,
		float ratio,
		float fade) const
	{
		if (width <= 2)
			return;
		ratio = std::max(0.0f, std::min(1.0f, ratio));

		const Texture& L = self_style ? gauge_self_l : gauge_common_l;
		const Texture& C = self_style ? gauge_self_c : gauge_common_c;
		const Texture& R = self_style ? gauge_self_r : gauge_common_r;

		if (!gauge_ready || !L || !C || !R)
		{
			// 纯色降级（参考 DrawGaugeBar 无资源分支 :2112-2126）：
			// 底槽 + 一条按 ratio 裁剪的高亮条；self=金黄，common=橙棕。
			const Color edge = self_style ? rgb(231, 183, 58) : rgb(159, 106, 50);
			const Color fg = self_style ? rgb(255, 204, 51) : rgb(184, 92, 46);
			ColorBox slot(width, kPlayerRowHeight - 2, Color::Name::MINESHAFT, 0.55f);
			slot.draw(DrawArgument(org + Point<int16_t>(x, y)) + fade);
			ColorBox rail(width, 3, edge, 0.85f);
			rail.draw(DrawArgument(org + Point<int16_t>(x, y + 8)) + fade);
			const int16_t fill = static_cast<int16_t>((width - 2) * ratio);
			if (fill > 0)
			{
				ColorBox bar(fill, 3, fg, 0.95f);
				bar.draw(DrawArgument(org + Point<int16_t>(x + 1, y + 8)) + fade);
			}
			return;
		}

		// 参考 :2129-2144：底条 左+中*(width-2)+右，再按 fillWidth 叠一层填充。
		L.draw(DrawArgument(org + Point<int16_t>(x, y)) + fade);
		for (int16_t i = 1; i < width - 1; ++i)
			C.draw(DrawArgument(org + Point<int16_t>(x + i, y)) + fade);
		R.draw(DrawArgument(org + Point<int16_t>(x + width - 1, y)) + fade);

		const int16_t fill = static_cast<int16_t>(width * ratio);
		for (int16_t i = 0; i < fill; ++i)
		{
			const Texture& piece = (i == 0) ? L : (i == width - 1 ? R : C);
			piece.draw(DrawArgument(org + Point<int16_t>(x + i, y)) + fade);
		}
	}

	void UIDamageRank::draw_player_page(Point<int16_t> org, float fade) const
	{
		const auto all = DamageRank::get().snapshot_players();
		const_cast<UIDamageRank*>(this)->clamp_scroll();
		const int16_t rows = visible_rows();
		const size_t start = static_cast<size_t>(scroll);
		if (all.empty())
		{
			// 空态提示：墨迹顶按首行高度中线定；x 用 CENTER 对齐故绘制点取窗宽中线。
			empty_hint.draw(DrawArgument(org + Point<int16_t>(kLayerWidth / 2,
				empty_hint.y_for_ink_top(centered_ink_top(kRowsStartY, row_height())))) + fade);
			return;
		}

		uint64_t top = 1;
		if (!all.empty() && all.front().total_damage > 0)
			top = all.front().total_damage;

		// 本机角色名：用于 self gauge 高亮（参考 GetLocalCharacterNameRaw）。
		std::string local_name;
#ifdef USE_NX
		local_name = Stage::get().get_player().get_stats().get_name();
#endif

		const bool scrolled = has_scroll();
		const int16_t gauge_x = 6;
		const int16_t gauge_w = scrolled ? 194 : 209;
		const int16_t damage_right = scrolled ? 194 : 209;

		for (int16_t i = 0; i < rows && start + i < all.size(); ++i)
		{
			const auto& e = all[static_cast<size_t>(start + i)];
			const int16_t y = static_cast<int16_t>(kRowsStartY + i * kPlayerRowPitch);
			const bool self_style = !local_name.empty() && e.name == local_name;
			const float ratio = static_cast<float>(
				static_cast<double>(e.total_damage) / static_cast<double>(top));

			// 玩家行高 21，gauge 每行画一条（参考 :1841-1910）。
			draw_gauge(org, gauge_x, y, gauge_w, self_style, ratio, fade);

			// 名次 + 名字画在 gauge 内；伤害右对齐。
			// 文字垂直落位走项目唯一真源：目标墨迹顶 = 行高中线，
			// 绘制点由 y_for_ink_top 按本串字形度量反解（不再用裸 y+1）。
			const int16_t ink_top = centered_ink_top(y, kPlayerRowHeight);
			std::ostringstream name;
			name << (start + i + 1) << ". " << e.name;
			Text name_t(Text::Font::A11M, Text::Alignment::LEFT,
				self_style ? Color::Name::YELLOW : Color::Name::WHITE, name.str());
			name_t.draw(DrawArgument(org + Point<int16_t>(
				gauge_x + 4, name_t.y_for_ink_top(ink_top))) + fade);

			Text dmg_t(Text::Font::A11M, Text::Alignment::RIGHT,
				self_style ? Color::Name::YELLOW : Color::Name::WHITE, fmt_num(e.total_damage));
			dmg_t.draw(DrawArgument(org + Point<int16_t>(
				damage_right, dmg_t.y_for_ink_top(ink_top))) + fade);
		}
	}

	void UIDamageRank::draw_skill_page(Point<int16_t> org, float fade) const
	{
		const auto all = DamageRank::get().snapshot_skills();
		const_cast<UIDamageRank*>(this)->clamp_scroll();
		const int16_t rows = visible_rows();
		if (all.empty())
		{
			// 空态提示：墨迹顶按首行高度中线定；x 用 CENTER 对齐故绘制点取窗宽中线。
			empty_hint.draw(DrawArgument(org + Point<int16_t>(kLayerWidth / 2,
				empty_hint.y_for_ink_top(centered_ink_top(kRowsStartY, row_height())))) + fade);
			return;
		}

		uint64_t sum = 0;
		for (const auto& e : all)
			sum += e.total_damage;
		if (sum == 0)
			sum = 1;

		const bool scrolled = has_scroll();
		const int16_t row_x = 6;
		const int16_t row_w = scrolled ? 194 : 210;
		const int16_t damage_right = scrolled ? 192 : 212;
		const size_t start = static_cast<size_t>(scroll);

		for (int16_t i = 0; i < rows && start + i < all.size(); ++i)
		{
			const auto& e = all[static_cast<size_t>(start + i)];
			const int16_t y = static_cast<int16_t>(kRowsStartY + i * kSkillRowHeight);
			// 参考 :2202-2204：技能行 42px 的三层灰底。
			ColorBox outer(row_w, 40, Color::Name::BOULDER, 0.55f);
			outer.draw(DrawArgument(org + Point<int16_t>(row_x, y)) + fade);
			ColorBox inner(static_cast<int16_t>(row_w - 2), 38, Color::Name::WHITE, 0.06f);
			inner.draw(DrawArgument(org + Point<int16_t>(row_x + 1, y + 1)) + fade);

			// 技能图标（参考 GetSkillIconCanvas :596-633）：
			//   -1(DoT) 不画；0 → icon_common_atk；>0 → SkillData 图标，miss 回退 unknown。
			// 图标纹理 origin 已在 SkillData 构造时 shift 到 (0,0)，故绘制点即左上角，
			// 与参考 BlitCanvas(10, y+4) 一致。
			if (const Texture* icon = skill_icon(e.skill_id))
				icon->draw(DrawArgument(org + Point<int16_t>(10, y + 4)) + fade);

			// 42px 技能行分两个 21px 文本子行，各按行高中线定目标墨迹顶，
			// 再交给 y_for_ink_top 反解（不再用裸 y+5/y+22）。
			const int16_t text_half = static_cast<int16_t>(kSkillRowHeight / 2);
			const int16_t ink_top_1 = centered_ink_top(y, text_half);
			const int16_t ink_top_2 = centered_ink_top(
				static_cast<int16_t>(y + text_half), text_half);

			Text name_t(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::TOOLTIPBODY,
				e.skill_name.empty() ? ("#" + std::to_string(e.skill_id)) : e.skill_name);
			name_t.draw(DrawArgument(org + Point<int16_t>(
				48, name_t.y_for_ink_top(ink_top_1))) + fade);

			Text cnt_t(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::LIGHTGREY,
				"x" + std::to_string(e.count));
			cnt_t.draw(DrawArgument(org + Point<int16_t>(
				48, cnt_t.y_for_ink_top(ink_top_2))) + fade);

			Text dmg_t(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::WHITE,
				fmt_num(e.total_damage));
			dmg_t.draw(DrawArgument(org + Point<int16_t>(
				damage_right, dmg_t.y_for_ink_top(ink_top_1))) + fade);

			const int pct = static_cast<int>(e.total_damage * 100 / sum);
			Text pct_t(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::YELLOW,
				std::to_string(pct) + "%");
			pct_t.draw(DrawArgument(org + Point<int16_t>(
				damage_right, pct_t.y_for_ink_top(ink_top_2))) + fade);
		}
	}

	void UIDamageRank::draw_scrollbar(Point<int16_t> org, float fade) const
	{
		if (!has_scroll())
			return;
		const Rectangle<int16_t> bar = scroll_bar_rect();
		const int16_t track_top = static_cast<int16_t>(bar.top() + kScrollButtonHeight);
		const int16_t track_bottom = static_cast<int16_t>(bar.bottom() - kScrollButtonHeight);

		// 轨道底：base 竖铺（参考 :1295-1307）。
		if (scr_base)
		{
			for (int16_t y = track_top; y < track_bottom; y += scr_base.height())
				scr_base.draw(DrawArgument(org + Point<int16_t>(bar.left(), y)) + fade);
		}
		else
		{
			ColorBox track(kScrollWidth, static_cast<int16_t>(track_bottom - track_top),
				Color::Name::BLACK, 0.25f);
			track.draw(DrawArgument(org + Point<int16_t>(bar.left(), track_top)) + fade);
		}

		// thumb（参考 :1309-1320）。
		const Rectangle<int16_t> thumb = scroll_thumb_rect();
		const int tst = (hovered_scroll == ScrollPart::Thumb || thumb_dragging) ? 1 : 0;
		if (scr_thumb[tst])
			scr_thumb[tst].draw(DrawArgument(org + Point<int16_t>(thumb.left(), thumb.top())) + fade);
		else
		{
			ColorBox knob(static_cast<int16_t>(thumb.right() - thumb.left()),
				static_cast<int16_t>(thumb.bottom() - thumb.top()), Color::Name::WHITE, 0.7f);
			knob.draw(DrawArgument(org + Point<int16_t>(thumb.left(), thumb.top())) + fade);
		}

		// 上/下箭头最后画，避免被 thumb 压住（参考 :1323-1330）。
		const int pi = (hovered_scroll == ScrollPart::Prev) ? 1 : 0;
		const int ni = (hovered_scroll == ScrollPart::Next) ? 1 : 0;
		if (scr_prev[pi])
			scr_prev[pi].draw(DrawArgument(org + Point<int16_t>(bar.left(), bar.top())) + fade);
		if (scr_next[ni])
			scr_next[ni].draw(DrawArgument(org + Point<int16_t>(bar.left(), bar.bottom() - kScrollButtonHeight)) + fade);
	}

	// ── 交互 ──────────────────────────────────────────────────────────────────

	void UIDamageRank::on_button(BtnId id)
	{
		switch (id)
		{
		case kBtnReset:
			// 参考 :2402-2419：清本地数据 + 发 reset + 再发 open 拉快照。
			if (!minimized)
			{
				DamageRank::get().reset();
				send_control(kControlReset);
				send_control(kControlOpen);
				scroll = 0;
				rebuild_rows();
				LOG(LOG_INFO, "UIDamageRank: reset");
			}
			break;

		case kBtnSwitch:
			// 参考 :2421-2425：玩家/技能切页。
			if (!minimized)
			{
				mode = (mode == Mode::Player) ? Mode::Skill : Mode::Player;
				scroll = 0;
				clamp_scroll();
				rebuild_rows();
				LOG(LOG_INFO, "UIDamageRank: switch mode=" << static_cast<int>(mode));
			}
			break;

		case kBtnAuto:
			// 参考 :2428-2443：Auto 切换；关闭时保留当前行数并夹回滚动。
			if (!minimized)
			{
				auto_mode = !auto_mode;
				if (auto_mode)
					scroll = 0;
				clamp_scroll();
				rebuild_rows();
				LOG(LOG_INFO, "UIDamageRank: auto=" << (auto_mode ? 1 : 0));
			}
			break;

		case kBtnMinMax:
			minimized = !minimized;
			scroll = 0;
			thumb_dragging = false;
			hovered_scroll = ScrollPart::None;
			// 最小化 20 高；展开按当前行数自适应（refresh_dimension 内部按 minimized 分支）。
			refresh_dimension();
			rebuild_rows();
			LOG(LOG_INFO, "UIDamageRank: minimized=" << (minimized ? 1 : 0));
			break;

		case kBtnClose:
			// 参考 :2461-2476：发 close 后隐藏，状态复位。
			send_control(kControlClose);
			deactivate();
			minimized = false;
			mode = Mode::Player;
			scroll = 0;
			auto_mode = true;
			skill_icon_cache.clear();   // 关闭时释放图标缓存，下次打开按新数据重取
			refresh_dimension();
			LOG(LOG_INFO, "UIDamageRank: close, control=0x3713/3");
			break;

		default:
			break;
		}
	}

	void UIDamageRank::rebuild_rows()
	{
		// 行文本由 draw_player_page/draw_skill_page 依当前快照即时生成；
		// 这里只刷新空态提示（避免重复持有数据快照）。
		empty_hint.change_text(cn("damagerank.empty", "暂无伤害数据"));
	}

	void UIDamageRank::send_control(uint8_t action)
	{
		// 不走 Net/Packets 枚举（约束：不改协议文件），派生结构写 body（见文件头 DamageRankControlPacket）。
		DamageRankControlPacket packet(action);
		packet.dispatch();
		LOG(LOG_DEBUG, "UIDamageRank control=0x3713 action=" << static_cast<int>(action));
	}
}
