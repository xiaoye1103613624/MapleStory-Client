//////////////////////////////////////////////////////////////////////////////////
// UIChatEmoticon — 动态聊天表情选择窗 + 头顶动画管理器
// 资源：Effect/ChatEmoticon.img/Dynamic{0..3}/{id}/effect/{frame}（live 已入）
// 发包：ChatEmoticonPacket(id) → s9 0x3715；收包 0x3716 → show_overhead()
//////////////////////////////////////////////////////////////////////////////////
#include "UIChatEmoticon.h"

#include "../UI.h"
#include "../Components/AreaButton.h"
#include "../../Net/Packets/ChatEmoticonPackets.h"
#include "../../Gameplay/Stage.h"
#include "../../Graphics/Animation.h"
#include "../../MapleStory.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		// 与服务端 ChatEmoticonHandler.isSupported 完全一致的 21 段白名单。
		struct IdRange { int32_t first, last; };
		const IdRange RANGES[] = {
			{100001,100010},{101001,101010},{102001,102024},{103001,103007},
			{104001,104005},{105001,105004},{106001,106006},{107001,107006},{108000,108027},
			{200001,200005},{201001,201004},{202001,202004},{203001,203006},
			{204001,204006},{205001,205006},{206001,206006},{207001,207006},{208000,208003},
			{300001,300120},{301000,301002},
		};

		// 每个 Dynamic 分类覆盖的号段前缀（tab -> 首位数字组）。
		bool id_in_tab(int32_t id, int16_t tab)
		{
			const int32_t group = id / 1000;
			switch (tab)
			{
			case 0: return group >= 100 && group <= 108;
			case 1: return group >= 200 && group <= 208;
			case 2: return group == 300;
			case 3: return group == 301;
			default: return false;
			}
		}

		// 头顶动画条目：charId + 动画 + 剩余时长（毫秒）。
		struct OverheadEntry
		{
			int32_t char_id;
			int32_t emoticon_id;
			Animation anim;
			int32_t remaining;
			Point<int16_t> offset;
		};
		std::vector<OverheadEntry> overhead_entries;
	}

	UIChatEmoticon::UIChatEmoticon()
	{
		dimension = Point<int16_t>(WIN_W, WIN_H);
		dragarea = Point<int16_t>(WIN_W, HEADER_H);
		layout_policy = LayoutPolicy::SavedPosClamped;

		buttons[BtClose] = std::make_unique<AreaButton>(
			Point<int16_t>(WIN_W - 22, 4), Point<int16_t>(18, 18));
		for (int16_t i = 0; i < TAB_COUNT; ++i)
			buttons[static_cast<uint16_t>(BtTabBase + i)] = std::make_unique<AreaButton>(
				Point<int16_t>(10 + i * 56, HEADER_H - 18), Point<int16_t>(52, 16));

		build_ids();
		load_page_icons();
		center_if_position_unset(dimension);
	}

	void UIChatEmoticon::build_ids()
	{
		ids_by_tab.assign(TAB_COUNT, {});
		for (const auto& r : RANGES)
		{
			for (int32_t id = r.first; id <= r.last; ++id)
			{
				for (int16_t t = 0; t < TAB_COUNT; ++t)
				{
					if (id_in_tab(id, t))
					{
						ids_by_tab[static_cast<size_t>(t)].push_back(id);
						break;
					}
				}
			}
		}
	}

	void UIChatEmoticon::load_page_icons()
	{
		page_entries.clear();
		const auto& all = ids_by_tab[static_cast<size_t>(tab)];
		const int32_t first = page * PER_PAGE;

#ifdef USE_NX
		nl::node root = nl::nx::Effect["ChatEmoticon.img"];
		const std::string dir = "Dynamic" + std::to_string(tab);

		for (int32_t i = 0; i < PER_PAGE && first + i < static_cast<int32_t>(all.size()); ++i)
		{
			const int32_t id = all[static_cast<size_t>(first + i)];
			EmoticonEntry e;
			e.id = id;
			// Texture 需 canvas 节点（frame "0"），effect 是 imgdir 容器。
			nl::node frame0 = root[dir][std::to_string(id)]["effect"]["0"];
			if (frame0)
			{
				e.icon = Texture(frame0);
				e.usable = true;
			}
			page_entries.push_back(std::move(e));
		}
#else
		for (int32_t i = 0; i < PER_PAGE && first + i < static_cast<int32_t>(all.size()); ++i)
		{
			EmoticonEntry e;
			e.id = all[static_cast<size_t>(first + i)];
			page_entries.push_back(std::move(e));
		}
#endif
	}

	int32_t UIChatEmoticon::page_count() const
	{
		const size_t n = ids_by_tab[static_cast<size_t>(tab)].size();
		return static_cast<int32_t>((n + PER_PAGE - 1) / PER_PAGE);
	}

	int16_t UIChatEmoticon::cell_at(Point<int16_t> cursorpos) const
	{
		const Point<int16_t> local = cursorpos - position;
		const int16_t gx = local.x() - 10;
		const int16_t gy = local.y() - HEADER_H;
		if (gx < 0 || gy < 0)
			return -1;
		const int16_t col = static_cast<int16_t>(gx / CELL);
		const int16_t row = static_cast<int16_t>(gy / CELL);
		if (col >= COLS || row >= ROWS)
			return -1;
		const int16_t idx = static_cast<int16_t>(row * COLS + col);
		return idx < static_cast<int16_t>(page_entries.size()) ? idx : -1;
	}

	void UIChatEmoticon::draw(float inter) const
	{
		UIElement::draw_buttons(inter);
	}

	void UIChatEmoticon::update()
	{
		UIElement::update();
	}

	Button::State UIChatEmoticon::button_pressed(uint16_t buttonid)
	{
		if (buttonid == BtClose)
		{
			deactivate();
			return Button::State::PRESSED;
		}
		if (buttonid >= BtTabBase && buttonid < BtTabBase + TAB_COUNT)
		{
			tab = static_cast<int16_t>(buttonid - BtTabBase);
			page = 0;
			load_page_icons();
			return Button::State::PRESSED;
		}
		return Button::State::NORMAL;
	}

	Cursor::State UIChatEmoticon::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (clicked)
		{
			const int16_t idx = cell_at(cursorpos);
			if (idx >= 0 && page_entries[static_cast<size_t>(idx)].usable)
			{
				ChatEmoticonPacket(page_entries[static_cast<size_t>(idx)].id).dispatch();
				return Cursor::State::CLICKING;
			}
		}
		return UIDragElement::send_cursor(clicked, cursorpos);
	}

	UIElement::Type UIChatEmoticon::get_type() const
	{
		return TYPE;
	}

	void UIChatEmoticon::show_overhead(int32_t char_id, int32_t emoticon_id)
	{
#ifdef USE_NX
		nl::node root = nl::nx::Effect["ChatEmoticon.img"];
		// 按 ID 前缀定位 Dynamic 目录（与 build_ids 同源）。
		std::string dir;
		const int32_t group = emoticon_id / 1000;
		if (group >= 100 && group <= 108) dir = "Dynamic0";
		else if (group >= 200 && group <= 208) dir = "Dynamic1";
		else if (group == 300) dir = "Dynamic2";
		else if (group == 301) dir = "Dynamic3";
		if (dir.empty())
			return;

		nl::node eff = root[dir][std::to_string(emoticon_id)]["effect"];
		if (!eff)
			return;

		OverheadEntry entry;
		entry.char_id = char_id;
		entry.emoticon_id = emoticon_id;
		entry.anim = Animation(eff);
		entry.remaining = 3000;   // 参考包约 3 秒
		entry.offset = Point<int16_t>(0, -80);
		overhead_entries.push_back(std::move(entry));
#else
		(void)char_id;
		(void)emoticon_id;
#endif
	}

	void UIChatEmoticon::update_overhead()
	{
		auto it = overhead_entries.begin();
		while (it != overhead_entries.end())
		{
			it->anim.update();
			it->remaining -= 16;   // ~60fps 粗略计时
			if (it->remaining <= 0)
				it = overhead_entries.erase(it);
			else
				++it;
		}
	}

	void UIChatEmoticon::draw_overhead(Point<int16_t> viewpos, float alpha)
	{
#ifdef USE_NX
		for (auto& entry : overhead_entries)
		{
			// get_position() 为相对 view 坐标（MapChars.cpp:42 用 +viewpos 得屏幕坐标）。
			if (auto chr = Stage::get().get_character(entry.char_id))
			{
				const Point<int16_t> pos = chr->get_position() + viewpos + entry.offset;
				entry.anim.draw(DrawArgument(pos), alpha);
			}
		}
#else
		(void)viewpos;
		(void)alpha;
#endif
	}
}
