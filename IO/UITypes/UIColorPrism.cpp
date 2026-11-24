//////////////////////////////////////////////////////////////////////////////////
// UIColorPrism.cpp — 七彩棱镜染色窗（D1 中文包协议 0x11D / 0x184）。
//
// 链路：NPC 开 0x184 OPEN → 本窗；拖入已穿戴现金装 → 3 条滑条（±100）
//       实时预览（TintContext）→ 确定发 0x11D CONFIRM(hue=v*1.8,s=v*0.007,
//       l=v*0.0075 的 float 位) → 提示「染色成功，请更换地图刷新」并关窗；
//       取消/关闭发 CLEAR；BtOff 还原。DYE_LIST 同步本人表。
//
// 分级日志：INFO = 开窗/确定/清除/收表等业务里程碑；WARN = 拒绝原因/资源缺失；
// DEBUG = 滑条/拖拽/图标细节（联调期，功能闭环后保留为默认关闭的调试级）。
//////////////////////////////////////////////////////////////////////////////////
#include "UIColorPrism.h"

#include "../UI.h"
#include "../Components/AreaButton.h"
#include "../Components/MapleButton.h"
#include "../Components/Icon.h"
#include "../UITypes/UIEquipInventory.h"
#include "../UITypes/UINotice.h"   // UIOk 提示窗（染色成功 / 拒绝原因）
#include "../../Character/Player.h"
#include "../../Character/Inventory/Inventory.h"
#include "../../Data/ItemData.h"
#include "../../Net/Packets/ColorPrismPackets.h"
#include "../../Gameplay/Stage.h"
#include "../../Graphics/TintContext.h"
#include "../../Util/Locale.h"
#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

#include <cmath>
#include <string>

namespace ms
{
	namespace
	{
		// 滑条行 → Locale 键（中英在 ini 里给到：色相/彩度/明度）
		const char* const ROW_KEYS[3] = {
			"colorprism.hue", "colorprism.sat", "colorprism.light"
		};

		int16_t clamp_slider(int32_t v)
		{
			return static_cast<int16_t>(v < -100 ? -100 : (v > 100 ? 100 : v));
		}

		/// float 位 → int（与服务端 Float.intBitsToFloat 对向，仅日志用）。
		float slider_to_hue(int16_t v) { return static_cast<float>(v) * 1.8f; }
		float slider_to_sat(int16_t v) { return static_cast<float>(v) * 0.007f; }
		float slider_to_light(int16_t v) { return static_cast<float>(v) * 0.0075f; }
	}

	void UIColorPrism::ensure_open()
	{
		auto ui = UI::get().get_element<UIColorPrism>();
		if (!ui)
			ui = UI::get().emplace<UIColorPrism>();
		else if (!ui->is_active())
			ui->makeactive();
		LOG(LOG_INFO, "[ColorPrism] ensure_open ok=" << static_cast<bool>(ui));
	}

	UIColorPrism::UIColorPrism()
	{
		dimension = Point<int16_t>(WIN_W, WIN_H);
		dragarea = Point<int16_t>(WIN_W, TITLE_H);  // 仅标题条可拖；按钮/滑条区不在拖拽区
		layout_policy = LayoutPolicy::SavedPosClamped;

		// 文案初值走 Locale（缺键时 Locale 回显 key，不发中文 UTF-8 字面量到 UI）。
		label_title = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, Locale::text("colorprism.title"));
		for (int16_t r = 0; r < ROW_COUNT; ++r)
		{
			row_labels[r] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text(ROW_KEYS[r]));
			row_values[r] = Text(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::BLACK, "0");
		}
		drop_hint = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::DARKGREY, Locale::text("colorprism.drop_hint"));
		success_hint = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK, Locale::text("colorprism.confirm_hint"));

#ifdef USE_NX
		// —— 资源：live UI/UIWindow.img/ColoringPrism（父 agent 转 key 入库）——
		nl::node prism = nl::nx::UI["UIWindow.img"]["ColoringPrism"];
		nl::node bg = prism["backgrnd"];
		if (UiAssetResolve::usable(bg))
		{
			backgrnd = Texture(bg);
			has_assets = true;
			LOG(LOG_INFO, "[ColorPrism] backgrnd loaded " << backgrnd.width() << "x" << backgrnd.height());
		}
		else
		{
			LOG(LOG_WARN, "[ColorPrism] UIWindow.img/ColoringPrism/backgrnd missing — 以纯文字/控件绘制降级窗口");
		}

		const char* const TRACK_NODES[3] = { "trackTone", "trackChroma", "trackBright" };
		for (int16_t r = 0; r < ROW_COUNT; ++r)
		{
			nl::node t = prism[TRACK_NODES[r]];
			if (UiAssetResolve::usable(t))
				track[r] = Texture(t);
			else
				LOG(LOG_WARN, "[ColorPrism] track node missing: " << TRACK_NODES[r]);
		}

		// thumb 取自 Basic.img/Slider（22x14），与轨道分开，缺则退为 AreaButton 命中。
		nl::node slider = nl::nx::UI["Basic.img"]["Slider"];
		const char* const THUMB_NODES[3] = { "thumbNormal", "thumbPressed", "thumbMouseOver" };
		for (int16_t i = 0; i < 3; ++i)
		{
			nl::node n = slider[THUMB_NODES[i]];
			if (UiAssetResolve::usable(n)) thumb[i] = Texture(n);
		}
		if (!thumb[0])
		{
			nl::node alt = slider["thumb"];
			if (UiAssetResolve::usable(alt)) thumb[0] = Texture(alt);
			if (!thumb[2] && thumb[0]) thumb[2] = thumb[0];
			if (!thumb[1] && thumb[0]) thumb[1] = thumb[0];
		}

		// —— 真按钮：BtOK / BtCancel / BtOff / BtClose（MapleButton 四态）——
		// try_maple_btn 传素材左上角，内部补 normal origin。命中区来自按钮自身
		// bounds，且这些按钮不在 dragarea（TITLE_H=22，底部按钮 y>=360）→ 不会误拖窗。
		const Point<int16_t> BTN_POS[4] = {
			Point<int16_t>(12, BTN_T),                       // BtOff
			Point<int16_t>(WIN_W - 12 - BTN_W * 2 - 8, BTN_T),// BtCancel
			Point<int16_t>(WIN_W - 12 - BTN_W, BTN_T),       // BtOK
			Point<int16_t>(CLOSE_X, CLOSE_Y)                 // BtClose（标题条）
		};
		const char* const BTN_NODES[4] = { "BtOff", "BtCancel", "BtOK", "BtClose" };
		const uint16_t BTN_IDS[4] = { BtOff, BtCancel, BtOK, BtClose };
		for (int i = 0; i < 4; ++i)
		{
			nl::node btn = prism[BTN_NODES[i]];
			if (!try_maple_btn(buttons, BTN_IDS[i], btn, BTN_POS[i], std::string("cp ") + BTN_NODES[i]))
			{
				// 资源缺失时以透明命中区兜底，保证按钮仍可点（不崩、不静默失效）。
				Point<int16_t> size = (i == 3) ? Point<int16_t>(CLOSE_SIZE, CLOSE_SIZE)
					: Point<int16_t>(BTN_W, BTN_H);
				buttons[BTN_IDS[i]] = std::make_unique<AreaButton>(BTN_POS[i], size);
				LOG(LOG_WARN, "[ColorPrism] button node missing: " << BTN_NODES[i] << " (AreaButton fallback)");
			}
		}
#else
		buttons[BtOff] = std::make_unique<AreaButton>(Point<int16_t>(12, BTN_T), Point<int16_t>(BTN_W, BTN_H));
		buttons[BtCancel] = std::make_unique<AreaButton>(Point<int16_t>(WIN_W - 12 - BTN_W * 2 - 8, BTN_T), Point<int16_t>(BTN_W, BTN_H));
		buttons[BtOK] = std::make_unique<AreaButton>(Point<int16_t>(WIN_W - 12 - BTN_W, BTN_T), Point<int16_t>(BTN_W, BTN_H));
		buttons[BtClose] = std::make_unique<AreaButton>(Point<int16_t>(CLOSE_X, CLOSE_Y), Point<int16_t>(CLOSE_SIZE, CLOSE_SIZE));
#endif

		center_if_position_unset(dimension);
		locale_gen = Locale::alive() ? Locale::get().generation() : 0;
		refresh_labels();
		LOG(LOG_INFO, "[ColorPrism] window constructed 300x398 assets=" << has_assets);
	}

	void UIColorPrism::refresh_labels()
	{
		for (int16_t r = 0; r < ROW_COUNT; ++r)
		{
			row_labels[r].change_text(Locale::text(ROW_KEYS[r]));
			row_values[r].change_text((slider[r] >= 0 ? "+" : "") + std::to_string(slider[r]));
		}
		label_title.change_text(Locale::text("colorprism.title"));
		drop_hint.change_text(has_target ? std::to_string(target_item_id)
			: Locale::text("colorprism.drop_hint"));
		success_hint.change_text(Locale::text("colorprism.confirm_hint"));
	}

	bool UIColorPrism::is_dyeable_category(int32_t item_id)
	{
		// 指南 §3.5：104 上衣 / 105 套服 / 106 裤 / 110 披风 / 170 现金武器。
		const int32_t cat = item_id / 10000;
		return cat == 104 || cat == 105 || cat == 106 || cat == 110 || cat == 170;
	}

	void UIColorPrism::apply_dye_list(const std::vector<DyeEntry>& entries)
	{
		own_dyes.clear();
		for (const auto& e : entries)
			own_dyes[e.item_id] = e;
		LOG(LOG_INFO, "[ColorPrism] DYE_LIST applied entries=" << entries.size());
		// 若当前投放目标正好在表内，按 HSL 反推滑条近似值（仅显示，不覆盖玩家正在拖的值）。
		if (has_target)
		{
			auto it = own_dyes.find(target_item_id);
			if (it != own_dyes.end())
			{
				slider[0] = clamp_slider(static_cast<int32_t>(std::lround(it->second.hue / 1.8f)));
				slider[1] = clamp_slider(static_cast<int32_t>(std::lround(it->second.sat / 0.007f)));
				slider[2] = clamp_slider(static_cast<int32_t>(std::lround(it->second.light / 0.0075f)));
				refresh_labels();
				LOG(LOG_DEBUG, "[ColorPrism] DYE_LIST synced sliders for target=" << target_item_id);
			}
		}
	}

	void UIColorPrism::apply_dye_merge(int32_t char_id, const std::vector<DyeEntry>& entries)
	{
		auto& table = foreign_dyes[char_id];
		table.clear();
		for (const auto& e : entries)
			table[e.item_id] = e;
		LOG(LOG_DEBUG, "[ColorPrism] DYE_MERGE cached char=" << char_id << " entries=" << entries.size());
		// 说明：他人实时画布染色由 TintContext（handler 内 set_char_tints）承载；
		// 本窗只缓存数据供以后扩展他人预览 UI，当前不重画他人动作（见报告降级项）。
	}

	void UIColorPrism::preview_refresh()
	{
		// SoftRefresh：仅更新 TintContext 中本人 oid 的表 → Texture::draw 时按 key 路由，
		// 不 Sweep、不 Clear+Prepare、不动共享画布（指南 §6 硬禁止）。
		if (!has_target) return;
		TintCache::HSL hsl;
		// 本端 TintCache 的 hue 为「正旋转度」：滑条±100 → ±180 度，折进 [0,360)。
		int32_t deg = static_cast<int32_t>(std::lround(slider_to_hue(slider[0]))) % 360;
		if (deg < 0) deg += 360;
		hsl.hue = static_cast<int16_t>(deg);
		auto to100 = [](float v) -> int8_t {
			int r = static_cast<int>(std::lround(v * 100.0f));
			return static_cast<int8_t>(r < -100 ? -100 : (r > 100 ? 100 : r));
		};
		hsl.chroma = to100(slider_to_sat(slider[1]));
		hsl.bright = to100(slider_to_light(slider[2]));

		const int32_t own = Stage::get().get_player().get_oid();
		// 以当前本人表为基底，覆盖正在编辑的 itemId（避免拖动时丢掉其他已染装备）。
		// 全量重建而非增量 patch：TintContext 只暴露 set_char_tints 替换接口。
		std::unordered_map<int32_t, TintCache::HSL> merged;
		for (const auto& kv : own_dyes)
		{
			TintCache::HSL base;
			int32_t bdeg = static_cast<int32_t>(std::lround(kv.second.hue)) % 360;
			if (bdeg < 0) bdeg += 360;
			base.hue = static_cast<int16_t>(bdeg);
			base.chroma = to100(kv.second.sat);
			base.bright = to100(kv.second.light);
			merged.emplace(kv.first, base);
		}
		merged[target_item_id] = hsl;
		TintContext::get().set_char_tints(own, merged);
		LOG(LOG_DEBUG, "[ColorPrism] soft preview item=" << target_item_id
			<< " h=" << hsl.hue << " c=" << static_cast<int>(hsl.chroma) << " b=" << static_cast<int>(hsl.bright));
	}

	void UIColorPrism::reset_sliders()
	{
		slider[0] = slider[1] = slider[2] = 0;
		refresh_labels();
		preview_refresh();
	}

	void UIColorPrism::confirm_and_close()
	{
		if (!has_target)
		{
			UI::get().emplace<UIOk>(Locale::text("colorprism.need_target"), [](bool) {});
			LOG(LOG_WARN, "[ColorPrism] CONFIRM rejected: no target");
			return;
		}
		// 滑条 ±100 → 协议浮点：H=v*1.8、S=v*0.007、L=v*0.0075（指南 §4.3）。
		const float hue = slider_to_hue(slider[0]);
		const float sat = slider_to_sat(slider[1]);
		const float light = slider_to_light(slider[2]);
		ColorPrismConfirmPacket(target_inv_type, target_slot, target_item_id, hue, sat, light).dispatch();
		LOG(LOG_INFO, "[ColorPrism] 0x11D CONFIRM item=" << target_item_id
			<< " slot=" << target_slot << " h=" << hue << " s=" << sat << " l=" << light);

		// 成功后弹提示并关窗（需求文案：染色成功，请更换地图刷新）。
		UI::get().emplace<UIOk>(Locale::text("colorprism.success"), [](bool) {});
		deactivate();
	}

	void UIColorPrism::clear_current(bool close_window)
	{
		if (has_target)
		{
			ColorPrismClearPacket(target_inv_type, target_slot, target_item_id).dispatch();
			// 本机即时还原：从表中移除该项并重建 TintContext 表。
			own_dyes.erase(target_item_id);
			auto own = Stage::get().get_player().get_oid();
			std::unordered_map<int32_t, TintCache::HSL> merged;
			auto to100 = [](float v) -> int8_t {
				int r = static_cast<int>(std::lround(v * 100.0f));
				return static_cast<int8_t>(r < -100 ? -100 : (r > 100 ? 100 : r));
			};
			for (const auto& kv : own_dyes)
			{
				TintCache::HSL base;
				int32_t deg = static_cast<int32_t>(std::lround(kv.second.hue)) % 360;
				if (deg < 0) deg += 360;
				base.hue = static_cast<int16_t>(deg);
				base.chroma = to100(kv.second.sat);
				base.bright = to100(kv.second.light);
				merged.emplace(kv.first, base);
			}
			TintContext::get().set_char_tints(own, merged);
			LOG(LOG_INFO, "[ColorPrism] 0x11D CLEAR item=" << target_item_id);
		}
		reset_sliders();
		if (close_window) deactivate();
	}

	int32_t UIColorPrism::value(int16_t row) const
	{
		return row >= 0 && row < ROW_COUNT ? slider[row] : 0;
	}

	void UIColorPrism::set_value(int16_t row, int32_t v)
	{
		if (row < 0 || row >= ROW_COUNT) return;
		v = clamp_slider(v);
		if (v == slider[row]) return;
		slider[row] = static_cast<int16_t>(v);
		refresh_labels();
		preview_refresh();
	}

	void UIColorPrism::set_value_from_x(int16_t row, int16_t x)
	{
		if (row < 0 || row >= ROW_COUNT) return;
		const int32_t raw = static_cast<int32_t>(x - TRACK_X) * (SLIDER_MAX - SLIDER_MIN)
			/ (TRACK_TRAVEL > 0 ? TRACK_TRAVEL : 1) + SLIDER_MIN;
		set_value(row, raw);
	}

	int16_t UIColorPrism::thumb_x(int16_t row) const
	{
		if (TRACK_TRAVEL <= 0) return TRACK_X;
		return static_cast<int16_t>(TRACK_X + (value(row) - SLIDER_MIN) * TRACK_TRAVEL
			/ (SLIDER_MAX - SLIDER_MIN));
	}

	bool UIColorPrism::thumb_contains(int16_t row, Point<int16_t> local) const
	{
		if (row < 0 || row >= ROW_COUNT) return false;
		const int16_t x = thumb_x(row);
		const int16_t y = static_cast<int16_t>(row_top(row) + (ROW_H - THUMB_H) / 2);
		return local.x() >= x && local.x() < x + THUMB_W
			&& local.y() >= y && local.y() < y + THUMB_H;
	}

	bool UIColorPrism::button_contains(uint16_t id, Point<int16_t> local) const
	{
		auto it = buttons.find(id);
		if (it == buttons.end() || !it->second) return false;
		Rectangle<int16_t> b = it->second->bounds(Point<int16_t>(0, 0));
		return b.contains(local);
	}

	bool UIColorPrism::is_in_well(Point<int16_t> local) const
	{
		return local.x() >= WELL_X && local.x() < WELL_X + WELL_SIZE
			&& local.y() >= WELL_Y && local.y() < WELL_Y + WELL_SIZE;
	}

	bool UIColorPrism::send_icon(const Icon& icon, Point<int16_t> cursor_position)
	{
		const Point<int16_t> local = cursor_position - position;
		Rectangle<int16_t> win{ position, position + dimension };
		if (!is_in_well(local) && !win.contains(cursor_position))
			return true;  // 丢在窗外，吞掉图标（不落到地面）

		// 只收已穿戴（EQUIPPED）装备；背包/消耗/其他一律拒绝。
		// 注意：装备窗的 EquipIcon 未覆写 identity getter（get_inventory_tab() 默认
		// NONE、get_item_id() 默认 0），因此以「当前选中的装备槽」为准——
		// UIEquipInventory::send_cursor 在抓起图标时已把 selected_eqslot 设为该槽。
		auto equips = UI::get().get_element<UIEquipInventory>();
		if (!equips || !equips->is_active())
		{
			UI::get().emplace<UIOk>(Locale::text("colorprism.need_equipped"), [](bool) {});
			LOG(LOG_WARN, "[ColorPrism] drop rejected: equip inventory not active");
			return true;
		}
		EquipSlot::Id eqslot = equips->get_selected_eqslot();
		if (eqslot == EquipSlot::Id::NONE)
		{
			UI::get().emplace<UIOk>(Locale::text("colorprism.need_equipped"), [](bool) {});
			LOG(LOG_WARN, "[ColorPrism] drop rejected: no selected equip slot");
			return true;
		}

		// 以装备槽反查 itemId 与现金/品类（EquipSlot::Id 值 = wire 绝对值，见 EquipSlot.h 不变量）。
		const Player& player = Stage::get().get_player();
		const int32_t item_id = player.get_inventory().get_item_id(InventoryType::Id::EQUIPPED, eqslot);
		if (item_id <= 0 || !is_dyeable_category(item_id) || !ItemData::get(item_id).is_cashitem())
		{
			UI::get().emplace<UIOk>(Locale::text("colorprism.bad_item"), [](bool) {});
			LOG(LOG_WARN, "[ColorPrism] drop rejected: not dyeable cash item=" << item_id << " slot=" << static_cast<int>(eqslot));
			return true;
		}

		has_target = true;
		target_inv_type = INVTYPE_EQUIPPED;
		// 已穿戴槽 wire 号为负（EquipSlot::Id 为正绝对值 → 取负）。
		target_slot = static_cast<int16_t>(-static_cast<int16_t>(eqslot));
		target_item_id = item_id;
		target_icon = Texture(ItemData::get(item_id).get_icon(item_id / 10000 == 238));
		refresh_labels();

		// 若服务端已下发该件染色，滑条切到其近似值，否则归零。
		auto it = own_dyes.find(item_id);
		if (it != own_dyes.end())
		{
			slider[0] = clamp_slider(static_cast<int32_t>(std::lround(it->second.hue / 1.8f)));
			slider[1] = clamp_slider(static_cast<int32_t>(std::lround(it->second.sat / 0.007f)));
			slider[2] = clamp_slider(static_cast<int32_t>(std::lround(it->second.light / 0.0075f)));
		}
		else
		{
			reset_sliders();
		}
		refresh_labels();
		LOG(LOG_INFO, "[ColorPrism] accepted item=" << item_id << " slot=" << target_slot);
		return true;
	}

	void UIColorPrism::draw(float inter) const
	{
		const float fade = fade_alpha();
		const Point<int16_t> org = position;

		if (backgrnd)
			backgrnd.draw(DrawArgument(org) + fade);

		// 标题"七彩棱镜"、投放提示"请插入要染色的现金装备"、滑条标签"颜色/饱和度/亮度"
		// 均已烧进 backgrnd.png（导出实测），不再重复绘制 Text——否则与素材字重叠错位。

		// 投放槽图标（拖入后）
		if (target_icon)
			target_icon.draw(DrawArgument(org + Point<int16_t>(WELL_X + (WELL_SIZE - ICON_SIZE) / 2,
				WELL_Y + (WELL_SIZE - ICON_SIZE) / 2)) + fade);

		// 预览区：玩家自身（TintContext 已含当前滑条染色，Texture::draw 路由）。
		const Player& player = Stage::get().get_player();
		{
			TintContext::Scope tint_scope(player.get_oid());
			player.draw_preview(org + Point<int16_t>(AVATAR_X, AVATAR_Y), inter);
		}

		// 三条滑条：轨道 + thumb（拖拽行用 pressed）+ 标签 + 数值。
		for (int16_t r = 0; r < ROW_COUNT; ++r)
		{
			const int16_t y = row_top(r);
			if (track[r])
				track[r].draw(DrawArgument(org + Point<int16_t>(TRACK_X, y + (ROW_H - TRACK_H) / 2)) + fade);
			const int16_t tx = thumb_x(r);
			const Texture& th = (drag_row == r && thumb[1]) ? thumb[1] : thumb[0];
			if (th)
				th.draw(DrawArgument(org + Point<int16_t>(tx, y + (ROW_H - THUMB_H) / 2)) + fade);
			// 滑条标签"颜色/饱和度/亮度"已烧进素材，只画实时数值（素材右侧数值区留空）。
			row_values[r].draw(DrawArgument(org + Point<int16_t>(VALUE_X + VALUE_W, y + (ROW_H - 13) / 2)) + fade);
		}

		// 底部说明文字素材未烧，但截图实测压在按钮上方且与滑条重叠，移除（确认结果走 UIOk 弹窗提示）。

		UIElement::draw_buttons(inter);
	}

	void UIColorPrism::update()
	{
		UIElement::update();
		// 切语言后本窗文案统一重取（Locale 代次变化触发）。
		if (Locale::alive() && Locale::get().generation() != locale_gen)
		{
			locale_gen = Locale::get().generation();
			refresh_labels();
		}
		// 拖拽中主键松开（失焦等）兜底收尾，防止 thumb 卡按下态。
		if (drag_row >= 0 && !UI::get().is_primary_down())
		{
			LOG(LOG_DEBUG, "[ColorPrism] drag ended by primary-up row=" << drag_row);
			drag_row = -1;
		}
	}

	Cursor::State UIColorPrism::send_cursor(bool clicked, Point<int16_t> cursor)
	{
		// 按钮命中优先：标题条 BtClose 与滑条 thumb 不能被窗口拖拽吞掉。
		for (uint16_t id : { BtOff, BtCancel, BtOK, BtClose })
		{
			if (button_contains(id, cursor - position))
				return UIElement::send_cursor(clicked, cursor);
		}
		Cursor::State ret = UIElement::send_cursor(clicked, cursor);
		if (ret == Cursor::State::CANCLICK || ret == Cursor::State::CANCLICK2)
			return ret;

		const Point<int16_t> local = cursor - position;
		if (drag_row >= 0)
		{
			if (UI::get().is_primary_down())
			{
				set_value_from_x(drag_row, local.x());
				return Cursor::State::HSCROLLIDLE;
			}
			LOG(LOG_DEBUG, "[ColorPrism] drag end row=" << drag_row << " value=" << value(drag_row));
			drag_row = -1;
			return Cursor::State::IDLE;
		}
		for (int16_t r = 0; r < ROW_COUNT; ++r)
		{
			if (thumb_contains(r, local))
			{
				if (clicked)
				{
					drag_row = r;
					LOG(LOG_DEBUG, "[ColorPrism] drag start row=" << r);
					return Cursor::State::HSCROLLIDLE;
				}
				return Cursor::State::HSCROLL;
			}
		}
		return UIDragElement::send_cursor(clicked, cursor);
	}

	void UIColorPrism::send_key(int32_t keycode, bool pressed, bool escape)
	{
		(void)keycode;
		// ESC 关窗：不加拖入校验、不发包（与 BtClose 一致）。UI.cpp ESC 白名单已含 COLORPRISM。
		if (pressed && escape)
		{
			LOG(LOG_INFO, "[ColorPrism] ESC close");
			deactivate();
		}
	}

	Button::State UIColorPrism::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
		case BtOff:
			// 还原原型：发 CLEAR，留在窗内（滑条归零）。
			clear_current(false);
			return Button::State::PRESSED;
		case BtCancel:
			// 取消：发 CLEAR 并关窗（恢复进入前的服务端染色）。
			clear_current(true);
			return Button::State::PRESSED;
		case BtOK:
			confirm_and_close();
			return Button::State::PRESSED;
		case BtClose:
			deactivate();
			return Button::State::PRESSED;
		default:
			return Button::State::NORMAL;
		}
	}

	UIElement::Type UIColorPrism::get_type() const
	{
		return TYPE;
	}
}
