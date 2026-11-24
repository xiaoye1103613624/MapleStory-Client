//////////////////////////////////////////////////////////////////////////////////
//	UISideIconBar — SideMenu tree + right flyout (1A) + MainNotice frames.
//////////////////////////////////////////////////////////////////////////////////
#include "UISideIconBar.h"

#include "UIContentsShortcut.h"
#include "UINotice.h"

#include "../UI.h"
#include "../Components/AreaButton.h"
#include "../Components/LocalFrameButton.h"
#include "../Components/LocalTwoSpriteButton.h"
#include "../Components/MapleButton.h"
#include "../../Configuration.h"
#include "../../Constants.h"
#include "../../Gameplay/Stage.h"
#include "../../Gameplay/MapleMap/Npc.h"
#include "../../Net/Packets/MessagingPackets.h"
#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Util/FeatureRegistry.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Graphics/Texture.h"

#include <algorithm>
#include <cctype>
#include <string>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		std::string lower_copy(std::string s)
		{
			std::transform(s.begin(), s.end(), s.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			return s;
		}

#ifdef USE_NX
		nl::node resolve_ui_path(const std::string& path)
		{
			nl::node n = nl::nx::UI;
			if (path.empty() || !n)
				return {};
			size_t start = 0;
			while (start < path.size())
			{
				size_t slash = path.find('/', start);
				const std::string part = path.substr(
					start, slash == std::string::npos ? std::string::npos : slash - start);
				if (!part.empty())
					n = n[part];
				if (!n)
					return {};
				if (slash == std::string::npos)
					break;
				start = slash + 1;
			}
			return n;
		}
#endif
	}

	UISideIconBar::UISideIconBar()
	{
		layout_policy = LayoutPolicy::None;
		active = Setting<SideIconsEnabled>::get().load();
		reload_config();
		update_screen(
			Constants::Constants::get().get_viewwidth(),
			Constants::Constants::get().get_viewheight());
	}

	void UISideIconBar::reload_config()
	{
		doc = SideMenuConfig::load_or_default();
		root = SideMenuConfig::root_items(doc);
		close_flyout();
		rebuild_buttons();
		layout_buttons();
		LOG(LOG_INFO, "UISideIconBar: reloaded revision=" << doc.revision
			<< " root=" << root.size());
	}

	bool UISideIconBar::try_make_frame_button(uint16_t id, const std::string& icon_key, size_t atlas_slot)
	{
		std::vector<LocalPngTexture> defs, notis, apps;
		const size_t atlas = ATLAS_BASE + atlas_slot * 64;
		if (!LocalFrameButton::try_load(icon_key, atlas, defs, notis, apps))
			return false;
		buttons[id] = std::make_unique<LocalFrameButton>(
			std::move(defs), std::move(notis), std::move(apps), Point<int16_t>(0, 0));
		return true;
	}

	bool UISideIconBar::make_btmain_button()
	{
#ifdef USE_NX
		nl::node shortroot = UiAssetResolve::window4("ShortCutUI");
		if (!UiAssetResolve::usable(shortroot["btMain"]))
		{
			UiAssetResolve::diag_once("sideicon.btMain",
				"UIWindow4.img/ShortCutUI/btMain missing; contents shortcut entry disabled");
			return false;
		}
		buttons[BT_MAIN] = std::make_unique<MapleButton>(shortroot["btMain"], Point<int16_t>(0, 0));
		return true;
#else
		return false;
#endif
	}

	void UISideIconBar::clear_flyout_buttons()
	{
		for (uint16_t id : fly_button_ids)
			buttons.erase(id);
		fly_button_ids.clear();
		fly_items.clear();
	}

	void UISideIconBar::close_flyout()
	{
		expanded_folder_id.clear();
		clear_flyout_buttons();
	}

	void UISideIconBar::rebuild_buttons()
	{
		++buttons_generation; // 使任何正在进行的 send_cursor 遍历失效，避免悬空指针
		buttons.clear();
		root_button_ids.clear();
		fly_button_ids.clear();
		fly_items.clear();

#ifdef USE_NX
		make_btmain_button();

		const size_t n = std::min(root.size(), static_cast<size_t>(MAX_ROOT));
		size_t atlas_slot = 0;
		for (size_t i = 0; i < n; ++i)
		{
			const auto& item = root[i];
			const uint16_t id = static_cast<uint16_t>(BT_ROOT0 + i);
			if (!try_make_frame_button(id, item.icon_key, atlas_slot))
			{
				nl::node basic = nl::nx::UI["Basic.img"];
				nl::node icon = resolve_ui_path("UIWindow4.img/ShortCutUI/contentsIcon/default");
				if (!UiAssetResolve::usable(icon) && UiAssetResolve::usable(basic["BtClaim"]))
					icon = basic["BtClaim"];
				if (UiAssetResolve::usable(icon))
				{
					Point<int16_t> origin;
					if (UiAssetResolve::usable(icon["normal"]["0"]))
						origin = Texture(icon["normal"]["0"]).get_origin();
					buttons[id] = std::make_unique<MapleButton>(icon, origin);
				}
				else
				{
					buttons[id] = std::make_unique<AreaButton>(
						Point<int16_t>(0, 0), Point<int16_t>(32, 32));
				}
			}
			++atlas_slot;
			root_button_ids.push_back(id);
		}

		if (!expanded_folder_id.empty())
		{
			fly_items = SideMenuConfig::children_of(doc, expanded_folder_id);
			const size_t fn = std::min(fly_items.size(), static_cast<size_t>(MAX_FLY));
			for (size_t i = 0; i < fn; ++i)
			{
				const uint16_t id = static_cast<uint16_t>(BT_FLY0 + i);
				if (!try_make_frame_button(id, fly_items[i].icon_key, atlas_slot + i))
				{
					// 与 root 相同的可见占位（BtClaim），不能用透明 AreaButton——
					// 新树 iconKey（attendance/EventList/Achieve...）在本地无序列帧时会全部
					// 加载失败，透明按钮导致横排「只看到一个/看不见」。
					nl::node basic = nl::nx::UI["Basic.img"];
					nl::node icon = basic["BtClaim"];
					if (UiAssetResolve::usable(icon))
					{
						Point<int16_t> origin;
						if (UiAssetResolve::usable(icon["normal"]["0"]))
							origin = Texture(icon["normal"]["0"]).get_origin();
						buttons[id] = std::make_unique<MapleButton>(icon, origin);
					}
					else
					{
						buttons[id] = std::make_unique<AreaButton>(
							Point<int16_t>(0, 0), Point<int16_t>(32, 32));
					}
				}
				fly_button_ids.push_back(id);
			}
		}
#endif
	}

	void UISideIconBar::layout_buttons()
	{
		int16_t y = 0;
		int16_t max_w = 16;

		auto place = [&](uint16_t id) {
			auto it = buttons.find(id);
			if (it == buttons.end() || !it->second)
				return;
			const Point<int16_t> org = it->second->origin();
			it->second->set_position(Point<int16_t>(org.x(), static_cast<int16_t>(y + org.y())));
			it->second->set_active(true);
			const Rectangle<int16_t> b = it->second->bounds(Point<int16_t>(0, 0));
			const int16_t bw = std::max<int16_t>(b.width(), it->second->width());
			const int16_t bh = std::max<int16_t>(b.height(), 12);
			if (bw > max_w) max_w = bw;
			y = static_cast<int16_t>(y + bh + SLOT_GAP);
		};

		place(BT_MAIN);
		for (uint16_t id : root_button_ids)
			place(id);

		dimension = Point<int16_t>(max_w, std::max<int16_t>(y, 16));

		const int16_t vh = view_h > 0
			? view_h
			: Constants::Constants::get().get_viewheight();
		int16_t top = static_cast<int16_t>((vh - dimension.y()) / 2);
		if (top < 0)
			top = 0;
		position = Point<int16_t>(MARGIN_X, top);

		layout_flyout();
	}

	void UISideIconBar::layout_flyout()
	{
		if (fly_button_ids.empty() || expanded_folder_id.empty())
			return;

		// Find root folder button bounds (absolute).
		int folder_index = -1;
		for (size_t i = 0; i < root.size() && i < root_button_ids.size(); ++i)
		{
			if (root[i].id == expanded_folder_id)
			{
				folder_index = static_cast<int>(i);
				break;
			}
		}
		if (folder_index < 0)
			return;

		const uint16_t folder_btn = root_button_ids[static_cast<size_t>(folder_index)];
		auto fit = buttons.find(folder_btn);
		if (fit == buttons.end() || !fit->second)
			return;

		const Rectangle<int16_t> fb = fit->second->bounds(position);
		// 避免 Win32 RECT 字段名宏/解析歧义：用 Point 访问而非 right()/top()。
		int16_t start_x = static_cast<int16_t>(fb.get_right_bottom().x() + FLY_GAP);
		int16_t start_y = fb.get_left_top().y();

		const int16_t vw = view_w > 0
			? view_w
			: Constants::Constants::get().get_viewwidth();
		const int16_t vh = view_h > 0
			? view_h
			: Constants::Constants::get().get_viewheight();

		for (size_t i = 0; i < fly_button_ids.size(); ++i)
		{
			auto it = buttons.find(fly_button_ids[i]);
			if (it == buttons.end() || !it->second)
				continue;

			const int col = static_cast<int>(i % FLY_COLS);
			const int row = static_cast<int>(i / FLY_COLS);
			int16_t x = static_cast<int16_t>(start_x + col * FLY_PITCH_X);
			int16_t y = static_cast<int16_t>(start_y + row * FLY_PITCH_Y);

			// Clamp into viewport (flyout positions are absolute; button pos is relative to element).
			int16_t rel_x = static_cast<int16_t>(x - position.x());
			int16_t rel_y = static_cast<int16_t>(y - position.y());

			const Point<int16_t> org = it->second->origin();
			it->second->set_position(Point<int16_t>(
				static_cast<int16_t>(rel_x + org.x()),
				static_cast<int16_t>(rel_y + org.y())));
			it->second->set_active(true);

			// Soft clamp: if past right edge, shift column start left next rebuild via start_x.
			(void)vw;
			(void)vh;
		}

		// Expand element hit box to cover flyout so clicks are received.
		// 原实现此处 dimensional.x() 自叠加写错、且把高度直接压成 extra_h（覆盖 root 竖栏高度），
		// 修正为：宽度 = root 宽 + flyout 横跨宽；高度 = max(root 高, flyout 起始 y + 行数高)。
		const int16_t cols_used = static_cast<int16_t>(
			std::min<int16_t>(FLY_COLS, static_cast<int16_t>(fly_button_ids.size())));
		const int16_t rows = static_cast<int16_t>((fly_button_ids.size() + FLY_COLS - 1) / FLY_COLS);
		const int16_t fly_w = static_cast<int16_t>(FLY_GAP + cols_used * FLY_PITCH_X);
		const int16_t fly_h = static_cast<int16_t>((start_y - position.y()) + rows * FLY_PITCH_Y);
		dimension = Point<int16_t>(
			static_cast<int16_t>(dimension.x() + fly_w),
			static_cast<int16_t>(std::max<int16_t>(dimension.y(), fly_h)));
	}

	void UISideIconBar::defer_rebuild()
	{
		// 绝不在 send_cursor 的 buttons 遍历中 clear()：交给 update() 下一帧做。
		rebuild_pending = true;
	}

	void UISideIconBar::toggle_folder(const std::string& folder_id)
	{
		if (expanded_folder_id == folder_id)
		{
			close_flyout();
			defer_rebuild();
			return;
		}

		expanded_folder_id = folder_id;
		defer_rebuild();
		// children 计数日志在 update() 重建后统一输出（此时 fly_items 才填充）。
	}

	void UISideIconBar::dispatch_npc_script(const std::string& arg)
	{
		// arg = "npcId" or "npcId:shop" — TalkToNPC needs map oid; find by template id.
		std::string token = arg;
		const size_t colon = token.find(':');
		std::string mode;
		if (colon != std::string::npos)
		{
			mode = lower_copy(token.substr(colon + 1));
			token = token.substr(0, colon);
		}
		int32_t npcid = 0;
		try { npcid = std::stoi(token); }
		catch (...) { npcid = 0; }
		if (npcid <= 0)
		{
			UI::get().emplace<UIOk>(u8"NpcScript 参数无效: " + arg, [](bool) {});
			return;
		}

		int32_t oid = 0;
		for (auto& kv : *Stage::get().get_npcs().get_npcs())
		{
			if (auto* npc = dynamic_cast<Npc*>(kv.second.get()))
			{
				if (npc->get_npcid() == npcid)
				{
					oid = kv.first;
					break;
				}
			}
		}

		if (oid <= 0)
		{
			UI::get().emplace<UIOk>(
				u8"当前地图找不到 NPC " + std::to_string(npcid),
				[](bool) {});
			return;
		}

		TalkToNPCPacket(oid).dispatch();
		LOG(LOG_INFO, "UISideIconBar: NpcScript npcid=" << npcid
			<< " oid=" << oid << " mode=" << mode);
		(void)mode; // shop vs talk: server script decides; no separate shop opcode by template id.
	}

	void UISideIconBar::dispatch_item(const SideMenuConfig::Item& item)
	{
		LOG(LOG_INFO, "UISideIconBar: dispatch id=" << item.id
			<< " action=" << SideMenuConfig::action_name(item.action)
			<< " arg=" << item.arg);

		switch (item.action)
		{
		case SideMenuConfig::Action::Builtin:
			FeatureRegistry::open(item.arg.empty() ? item.id : item.arg);
			break;
		case SideMenuConfig::Action::ChatCmd:
			if (!item.arg.empty())
				GeneralChatPacket(item.arg, true).dispatch();
			break;
		case SideMenuConfig::Action::NpcScript:
			dispatch_npc_script(item.arg);
			break;
		}
	}

	void UISideIconBar::draw(float inter) const
	{
		if (!active)
			return;
		UIElement::draw(inter);
	}

	void UISideIconBar::update()
	{
		if (!Setting<SideIconsEnabled>::get().load())
		{
			if (active)
				deactivate();
			return;
		}
		if (!active)
			makeactive();

		// 点击回调里只置脏；此处在 send_cursor 遍历之外统一重建，避免
		// buttons.clear() 使遍历迭代器失效（WER 15:06 c0000005 @ UIElement.cpp:386）。
		if (rebuild_pending)
		{
			rebuild_pending = false;
			rebuild_buttons();
			layout_buttons();
			if (!expanded_folder_id.empty())
				LOG(LOG_INFO, "UISideIconBar: expand folder=" << expanded_folder_id
					<< " children=" << fly_items.size());
		}

		static int layout_tick = 0;
		layout_tick += Constants::TIMESTEP;
		if (layout_tick >= 2000)
		{
			layout_tick = 0;
			layout_buttons();
		}

		reload_cooldown -= Constants::TIMESTEP;
		if (reload_cooldown <= 0)
		{
			reload_cooldown = 5000;
			const auto fresh = SideMenuConfig::load_or_default();
			bool dirty = fresh.revision != doc.revision
				|| fresh.items.size() != doc.items.size();
			if (!dirty)
			{
				for (size_t i = 0; i < fresh.items.size(); ++i)
				{
					const auto& a = fresh.items[i];
					const auto& b = doc.items[i];
					if (a.id != b.id || a.parent_id != b.parent_id || a.kind != b.kind
						|| a.enabled != b.enabled || a.icon_key != b.icon_key
						|| a.order != b.order || a.action != b.action || a.arg != b.arg)
					{
						dirty = true;
						break;
					}
				}
			}
			if (dirty)
			{
				doc = fresh;
				root = SideMenuConfig::root_items(doc);
				const std::string keep = expanded_folder_id;
				rebuild_buttons();
				if (!keep.empty())
				{
					expanded_folder_id = keep;
					rebuild_buttons();
				}
				layout_buttons();
			}
		}

		UIElement::update();
	}

	void UISideIconBar::update_screen(int16_t new_width, int16_t new_height)
	{
		view_w = new_width;
		view_h = new_height;
		layout_buttons();
	}

	UIElement::Type UISideIconBar::get_type() const
	{
		return TYPE;
	}

	Button::State UISideIconBar::button_pressed(uint16_t buttonid)
	{
		if (buttonid == BT_MAIN)
		{
			auto current = UI::get().get_element<UIContentsShortcut>();
			if (current && current->is_active())
				current->deactivate();
			else
				UI::get().emplace<UIContentsShortcut>();
			return Button::State::NORMAL;
		}

		if (buttonid >= BT_ROOT0 && buttonid < BT_ROOT0 + MAX_ROOT)
		{
			const size_t idx = static_cast<size_t>(buttonid - BT_ROOT0);
			if (idx >= root.size())
				return Button::State::DISABLED;
			const auto& item = root[idx];
			if (item.kind == SideMenuConfig::Kind::Folder)
			{
				toggle_folder(item.id);
			}
			else
			{
				close_flyout();
				defer_rebuild();
				dispatch_item(item);
			}
			return Button::State::NORMAL;
		}

		if (buttonid >= BT_FLY0 && buttonid < BT_FLY0 + MAX_FLY)
		{
			const size_t idx = static_cast<size_t>(buttonid - BT_FLY0);
			if (idx >= fly_items.size())
				return Button::State::DISABLED;
			const auto item = fly_items[idx]; // 先拷值：defer_rebuild 后 fly_items 会被清空
			close_flyout();
			defer_rebuild();
			dispatch_item(item);
			return Button::State::NORMAL;
		}

		return Button::State::DISABLED;
	}

	Cursor::State UISideIconBar::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		// btMain 悬浮打开 ContentsShortcut（保留原逻辑）。
		auto main_it = buttons.find(BT_MAIN);
		bool on_main = false;
		if (main_it != buttons.end() && main_it->second && main_it->second->is_active()
			&& main_it->second->bounds(position).contains(cursorpos))
			on_main = true;
		if (on_main)
		{
			if (!hover_main_opened)
			{
				auto current = UI::get().get_element<UIContentsShortcut>();
				if (!current || !current->is_active())
					UI::get().emplace<UIContentsShortcut>();
				hover_main_opened = true;
			}
		}
		else
		{
			hover_main_opened = false;
		}

		// 安全遍历：复制 id 列表，避免遍历中 buttons 被 clear() 导致迭代器/指针悬空。
		// 点击 dispatch→UI::emplace 会在本调用栈内间接触发 update() 的 dirty 重建
		// （buttons.clear()），基类 UIElement::send_cursor 直接遍历容器会崩溃（WER 实测
		// 0xe138e=UIElement.cpp:381 / 0xb1f60=Button::is_active）。generation 变化即中止。
		std::vector<uint16_t> ids;
		ids.reserve(buttons.size());
		for (const auto& kv : buttons)
			ids.push_back(kv.first);

		const uint32_t gen = buttons_generation;
		Cursor::State ret = clicked ? Cursor::State::CLICKING : Cursor::State::IDLE;
		for (uint16_t id : ids)
		{
			if (buttons_generation != gen)
				break; // 按钮集合已重建，后续指针全部失效，立即停止
			auto it = buttons.find(id);
			if (it == buttons.end() || !it->second)
				continue;
			Button* button = it->second.get();
			if (button->is_active() && button->bounds(position).contains(cursorpos))
			{
				if (button->get_state() == Button::State::NORMAL)
				{
					Sound(Sound::Name::BUTTONOVER).play();
					button->set_state(Button::State::MOUSEOVER);
					ret = Cursor::State::CANCLICK;
					if (clicked)
					{
						Sound(Sound::Name::BUTTONCLICK).play();
						Button::State result = button_pressed(id);
						// 重建可能已发生，旧 button 指针失效，不可再 set_state。
						if (buttons_generation == gen)
						{
							auto it2 = buttons.find(id);
							if (it2 != buttons.end() && it2->second)
								it2->second->set_state(result);
						}
						ret = Cursor::State::IDLE;
					}
				}
				else if (button->get_state() == Button::State::MOUSEOVER)
				{
					if (clicked)
					{
						Sound(Sound::Name::BUTTONCLICK).play();
						Button::State result = button_pressed(id);
						if (buttons_generation == gen)
						{
							auto it2 = buttons.find(id);
							if (it2 != buttons.end() && it2->second)
								it2->second->set_state(result);
						}
						ret = Cursor::State::IDLE;
					}
					else
					{
						ret = Cursor::State::CANCLICK;
					}
				}
			}
			else if (button->get_state() == Button::State::MOUSEOVER)
			{
				button->set_state(Button::State::NORMAL);
			}
		}
		return ret;
	}

	void UISideIconBar::remove_cursor()
	{
		hover_main_opened = false;
		UIElement::remove_cursor();
	}
}
