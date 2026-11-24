//////////////////////////////////////////////////////////////////////////////////
// UIContentsShortcut — 192 ShortCutUI 横向弹层（实现）
//
// 布局/交互严格按 docs/features/sideicon-collapse-192-panel/PLAN.md 的 IDA 一手公式：
// 屏幕居中、6 列、56x92 pitch、三段底图、contentsIcon/{path}/normal/0 单帧。
// 悬浮展开不属原生（IDA 证实原生只有点击 toggle），由用户 2026-11-21 额外拍板加入，
// 去重在 UISideIconBar 的 btMain 命中标志里完成（见 UISideIconBar.cpp）。
//////////////////////////////////////////////////////////////////////////////////
#include "UIContentsShortcut.h"

#include "UIMessenger.h"
#include "UINotice.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Constants.h"

#include "../../Util/Locale.h"
#include "../../Util/UiAssetResolve.h"

#include <algorithm>
#include <cctype>
#include <memory>
#include <utility>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		std::string lower_name(std::string s)
		{
			std::transform(s.begin(), s.end(), s.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			return s;
		}
	}

	std::vector<UIContentsShortcut::ContentsInfo> UIContentsShortcut::build_contents()
	{
		// 唯一 Allowed 数据源：192 Etc/ContentsShortcut.img（已核对 XML 原文）。
		// toolTip 保留 XML 中文原文；显示时经 Locale::text_or(键, 原文) 做 i18n，
		// 缺词条时回退原文而不是显示键名（Locale::text 的行为）。
		std::vector<ContentsInfo> table;

		// BossArena：任务型（questIDs 25959/25964），无 openUI_idx。
		{
			ContentsInfo info;
			info.name = "BossArena";
			info.resourcePath = "bossArena";
			info.toolTip = "首领怪竞技场";
			info.openUIIndex = -1;
			info.questIDs = { 25959, 25964 };
			table.push_back(std::move(info));
		}
		// CrossHunter：openUI_idx=73，本端无对应 Type → UIOk 提示。
		{
			ContentsInfo info;
			info.name = "CrossHunter";
			info.resourcePath = "crossHunter";
			info.toolTip = "十字猎人";
			info.openUIIndex = 73;
			table.push_back(std::move(info));
		}
		// ItemPot / Bits / maplechat：192 XML 无 resourcePath，原生走 default。
		// 但 live contentsIcon/default/normal/0 是 1x1 inlink（运行时 dump 实证），画出来不可见
		// ——之前只显示 2 个图标的根因。改用 live 里真实存在的位图节点（hekaton/bossArena/crossHunter
		// 均 34~43px 有效位图），语义不精确但视觉可见；tooltip 仍用各自真实名称。
		{
			ContentsInfo info;
			info.name = "ItemPot";
			info.resourcePath = "hekaton";     // 有效位图 39x38
			info.toolTip = "道具宝宝";
			info.openUIIndex = 51;
			table.push_back(std::move(info));
		}
		{
			ContentsInfo info;
			info.name = "Bits";
			info.resourcePath = "bossArena";   // 有效位图 43x38
			info.toolTip = "拼图";
			info.openUIIndex = 125;
			table.push_back(std::move(info));
		}
		{
			ContentsInfo info;
			info.name = "maplechat";
			info.resourcePath = "crossHunter"; // 有效位图 34x37
			info.toolTip = "冒险岛聊天工具";
			info.openUIIndex = 34;   // 原生特例 34=Messenger，本端有 UIMessenger。
			table.push_back(std::move(info));
		}
		return table;
	}

	UIContentsShortcut::UIContentsShortcut()
	{
		contents = build_contents();

		// 面板尺寸（IDA）：宽=top 贴图宽，高=33 + 92*nLine + 33。
		const size_t nLine = (contents.size() + COLS - 1) / COLS;
		dimension = Point<int16_t>(
			PANEL_W,
			static_cast<int16_t>(TOP_H + MIDDLE_H * nLine + BOTTOM_H));

		// 屏幕居中：Center 策略让 update_screen 在分辨率变化时重锚 + clamp。
		layout_policy = LayoutPolicy::Center;
		center_on_screen();

#ifdef USE_NX
		// 三段背景资源（live UIWindow4.img/ShortCutUI 已 probe 完整）。
		nl::node shortroot = UiAssetResolve::window4("ShortCutUI");
		if (UiAssetResolve::usable(shortroot["top"]))
			tex_top = Texture(shortroot["top"]);
		if (UiAssetResolve::usable(shortroot["middle"]))
			tex_middle = Texture(shortroot["middle"]);
		if (UiAssetResolve::usable(shortroot["bottom"]))
			tex_bottom = Texture(shortroot["bottom"]);
		if (!tex_top.is_valid() || !tex_middle.is_valid() || !tex_bottom.is_valid())
			LOG(LOG_WARN, "UIContentsShortcut: ShortCutUI top/middle/bottom missing; panel bg incomplete");

		// 图标按钮：任务型与 openUI 型一视同仁，坐标与 IDA 完全一致。
		for (size_t i = 0; i < contents.size(); ++i)
		{
			const ContentsInfo& info = contents[i];
			nl::node icon = shortroot["contentsIcon"][info.resourcePath];

			// 图标资源缺失回退 default，并打一条 WARN（上线级：资源异常可观测）。
			if (!UiAssetResolve::usable(icon))
			{
				LOG(LOG_WARN, "UIContentsShortcut: contentsIcon/" << info.resourcePath
					<< " missing for " << info.name << "; fallback default");
				icon = shortroot["contentsIcon"]["default"];
			}
			if (!UiAssetResolve::usable(icon))
			{
				LOG(LOG_WARN, "UIContentsShortcut: contentsIcon/default missing; icon " << i << " hidden");
				continue;
			}

			// contentsIcon/{path}/normal/0 单帧；try_maple_btn 会把 normal/0 的
			// origin 加回 position，使传入点等价于贴图左上角。
			const int16_t icon_w = UiAssetResolve::usable(icon["normal"]["0"])
				? Texture(icon["normal"]["0"]).get_dimensions().x() : 37;
			const int16_t x = static_cast<int16_t>(
				((ICON_PITCH_X - icon_w) >> 1) + ICON_PITCH_X * (i % COLS) + ICON_X_PAD);
			const int16_t y = static_cast<int16_t>(ICON_PITCH_Y * (i / COLS) + TOP_H);

			try_maple_btn(buttons, static_cast<uint16_t>(BT_ICON0 + i), icon,
				Point<int16_t>(x, y), "contentsshortcut icon " + info.name);
		}

		// btClose：优先 ShortCutUI/btClose 四态，缺失回退 Basic.img/BtClose。
		const Point<int16_t> close_pos(PANEL_W - 24, 8);
		if (!try_maple_btn(buttons, BT_CLOSE, shortroot["BtClose"], close_pos, "contentsshortcut BtClose"))
		{
			nl::node fallback = UiAssetResolve::close_btn();
			if (UiAssetResolve::usable(fallback))
				buttons[BT_CLOSE] = std::make_unique<MapleButton>(fallback, close_pos);
			else
				LOG(LOG_WARN, "UIContentsShortcut: BtClose missing; ESC/btMain still close");
		}
#endif
		LOG(LOG_INFO, "UIContentsShortcut: open entries=" << contents.size()
			<< " lines=" << nLine << " dim=" << dimension.x() << "x" << dimension.y());
	}

	void UIContentsShortcut::draw(float alpha) const
	{
		// 三段底图：top -> middle*nLine -> bottom，拼成 353 x (33+92*nLine+33)。
		const float fade = fade_alpha();
		// DrawArgument 无 (Point,float) 构造（第二参被当 angle/stretch 导致贴图异常）——
		// 全项目标准写法是 DrawArgument(pos) + fade（operator+ 施加 alpha）。
		tex_top.draw(DrawArgument(position) + fade);

		const size_t nLine = (contents.size() + COLS - 1) / COLS;
		for (size_t r = 0; r < nLine; ++r)
		{
			const Point<int16_t> row_pos(
				position.x(), static_cast<int16_t>(position.y() + TOP_H + MIDDLE_H * r));
			tex_middle.draw(DrawArgument(row_pos) + fade);
		}
		tex_bottom.draw(DrawArgument(
			Point<int16_t>(position.x(), static_cast<int16_t>(position.y() + TOP_H + MIDDLE_H * nLine))) + fade);

		// 按钮（图标 + btClose）走基类状态机绘制，保证 hover/pressed 态正确。
		UIElement::draw_buttons(alpha);
	}

	void UIContentsShortcut::update()
	{
		UIElement::update();
	}

	void UIContentsShortcut::update_screen(int16_t new_width, int16_t new_height)
	{
		UIElement::update_screen(new_width, new_height);
	}

	Button::State UIContentsShortcut::button_pressed(uint16_t buttonid)
	{
		if (buttonid == BT_CLOSE)
		{
			close_panel();
			return Button::State::NORMAL;
		}
		if (buttonid >= BT_ICON0 && buttonid < BT_ICON0 + contents.size())
		{
			dispatch_icon(static_cast<size_t>(buttonid - BT_ICON0));
			return Button::State::NORMAL;
		}
		return Button::State::DISABLED;
	}

	void UIContentsShortcut::close_panel()
	{
		UI::get().clear_tooltip(Tooltip::Parent::TEXT);
		hover_icon = BTN_IDLE;
		deactivate();
	}

	int16_t UIContentsShortcut::icon_at(Point<int16_t> cursorpos) const
	{
		for (size_t i = 0; i < contents.size(); ++i)
		{
			auto it = buttons.find(static_cast<uint16_t>(BT_ICON0 + i));
			if (it != buttons.end() && it->second && it->second->is_active()
				&& it->second->bounds(position).contains(cursorpos))
				return static_cast<int16_t>(i);
		}
		return BTN_IDLE;
	}

	Cursor::State UIContentsShortcut::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		const int16_t idx = icon_at(cursorpos);
		hover_icon = idx;
		if (idx >= 0)
		{
			// UIStateGame::send_cursor 每帧派发前会 clear_tooltip(tooltipparent)，
			// 所以 hover 期间必须每帧重挂 show_text，否则 tooltip 只会闪一帧（同 UIRegion/UIDailyCheckin）。
			const ContentsInfo& info = contents[static_cast<size_t>(idx)];
			// toolTip 存 XML 原文；i18n 键按 name 小写，缺词条回退原文。
			const std::string key = "contentsshortcut.tip." + lower_name(info.name);
			const std::string text = Locale::text_or(key, info.toolTip);
			UI::get().show_text(Tooltip::Parent::TEXT, text);
		}
		// 未命中图标不清：交回 UIStateGame 每帧入口统一清 tooltip（避免与其它窗抢 clear）。
		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UIContentsShortcut::remove_cursor()
	{
		hover_icon = BTN_IDLE;
		UI::get().clear_tooltip(Tooltip::Parent::TEXT);
		UIElement::remove_cursor();
	}

	void UIContentsShortcut::send_key(int32_t keycode, bool pressed, bool escape)
	{
		(void)keycode;
		if (pressed && escape)
			close_panel();
	}

	void UIContentsShortcut::dispatch_icon(size_t index)
	{
		if (index >= contents.size())
			return;

		const ContentsInfo& info = contents[index];
		LOG(LOG_INFO, "UIContentsShortcut: click " << info.name
			<< " openUIIndex=" << info.openUIIndex);

		std::string message;
		if (info.openUIIndex > -1)
		{
			switch (info.openUIIndex)
			{
			case 34:   // 原生特例：Messenger（本端唯一有对应 Type 的入口）。
				UI::get().emplace<UIMessenger>();
				break;
			default:
				LOG(LOG_WARN, "UIContentsShortcut: openUI_idx=" << info.openUIIndex
					<< " (" << info.name << ") has no local Type; degrade to UIOk");
				message = Locale::text("contentsshortcut.entry_unavailable");
				break;
			}
		}
		else
		{
			// 任务型：本端无 CUtilDlgEx 任务选择，不臆造 StartQuest —— UIOk 提示。
			LOG(LOG_WARN, "UIContentsShortcut: quest-type " << info.name
				<< " questIDs empty=" << info.questIDs.empty() << "; degrade to UIOk");
			message = Locale::text("contentsshortcut.quest_unavailable");
		}

		if (!message.empty())
			UI::get().emplace<UIOk>(message, [](bool) {});

		// 点后关闭面板（IDA SelectIcon 后 UI_Close(195) 的本端等价）。
		close_panel();
	}

	UIElement::Type UIContentsShortcut::get_type() const
	{
		return TYPE;
	}
}
