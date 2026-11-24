//////////////////////////////////////////////////////////////////////////////////
// 窗口内商城（方案 C）主窗口。
//
// 设计依据：
//  - 布局常量逐项取自参考包 cashshopwnd.cpp:585-906（760×495 底板上的实测坐标，
//    6×4 网格单元 86×88、预览井 182×165、购物车 5×4 槽、Buy/BtCart 76×18）。
//  - 预览沙盒逐行复用本端 UICashShop.cpp:1208-1371 的 init_preview_sandbox /
//    send_key / apply_preview_controls（Player + Physics + Ladder + sandbox FHT，
//    不发包、不影响真实角色）。
//  - 数据全部来自服务端 S→C 0x3731 同步包（见 Net/Handlers/CashShopWindowSync.h），
//    本窗口不读本地 Commodity.img。
//
// 线程：本对象只在主线程构造/更新/绘制；收包线程只写同步队列。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

// Cursor.h 必须在 UIDragElement.h 之前：UIDragElement::send_cursor 的返回类型 Cursor::State
// 在基类定义时就需要完整类型，否则子类 override 会因所见类型不同而报 C2555 非协变。
#include "../Cursor.h"
#include "../UIDragElement.h"

#include "../Components/MapleButton.h"
#include "../Components/Slider.h"
#include "../Components/Textfield.h"
#include "../Components/TwoSpriteButton.h"

#include "../../Character/Look/CharLook.h"
#include "../../Character/Player.h"
#include "../../Data/ItemData.h"
#include "../../Gameplay/Physics/Physics.h"
#include "../../Gameplay/MapleMap/MapInfo.h"   // Ladder（预览沙盒爬梯）
#include "../../Graphics/Text.h"
#include "../../Graphics/Animation.h"
#include "../../Net/Handlers/CashShopWindowSync.h"

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	class UIWindowCashShop : public UIDragElement<PosWINDOWCASHSHOP>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::WINDOWCASHSHOP;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		// 与服务端 CashShopWindowPackets 的 buy code 一一对应（0..6）。
		enum class BuyCode : uint8_t
		{
			OK = 0, NO_CASH, UNKNOWN_ITEM, INVENTORY_FULL,
			NOT_ON_SALE, BUSY, BAD_CART
		};

		UIWindowCashShop();
		~UIWindowCashShop() override;

		void draw(float inter) const override;
		void update() override;

		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_scroll(double yoffset) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		void remove_cursor() override;

		UIElement::Type get_type() const override;

		// ---- 由 CashShopWindowSync::pump（主线程）调用的数据落地接口 ----
		void apply_open(const int32_t cash[3]);
		void apply_cash(const int32_t cash[3]);
		void apply_index(const std::vector<CashShopWindowIndexRow>& rows);
		void apply_catalog(int8_t clear, int8_t tab, int8_t cat,
		                   const std::vector<CashShopWindowCatalogRow>& rows);
		void apply_buy(int8_t code, int32_t itemId);
		void apply_buy_cart(int8_t code, int32_t itemId, int8_t delivered, int32_t spent);
		void apply_taxonomy(const std::vector<CashShopWindowTaxonomyRow>& rows);

		// 关闭窗口时由自身/外部调用：清 tooltip、清队列残留。
		void close_window();

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		// ---------- 常量（全部来自参考包实测，勿凭感觉改） ----------
		static constexpr int16_t WND_W = 760;
		static constexpr int16_t WND_H = 495;
		static constexpr int16_t GRID_COLS = 6;
		static constexpr int16_t GRID_ROWS = 4;
		static constexpr int16_t GRID_SLOTS = GRID_COLS * GRID_ROWS;   // 24
		static constexpr int16_t CELL_W = 86;
		static constexpr int16_t CELL_H = 88;
		static constexpr int16_t CELL_X0 = 12;
		static constexpr int16_t CELL_Y0 = 90;
		static constexpr int16_t CART_MAX = 20;
		static constexpr int16_t CART_COLS = 5;
		static constexpr int16_t CART_ROWS = 4;
		static constexpr int16_t CART_BOX = 34;
		static constexpr int16_t CART_GAP = 2;
		static constexpr int16_t CART_X0 = 566;
		static constexpr int16_t CART_Y0 = 322;
		static constexpr int16_t PREVIEW_X = 566;
		static constexpr int16_t PREVIEW_Y = 90;
		static constexpr int16_t PREVIEW_W = 182;
		static constexpr int16_t PREVIEW_H = 165;
		static constexpr int16_t FLOOR_Y = 134;    // 相对预览井顶（Backdrop 0..2 共用地面行）
		static constexpr int16_t LADDER_X = 146;   // 实测三背景梯中点 146..149 取 146
		static constexpr int16_t LADDER_Y1 = 24;
		static constexpr int16_t LADDER_Y2 = 134;

		// 单个分类桶的键：tab<<8 | cat，便于 map 查找。
		static int32_t bucket_key(int8_t tab, int8_t cat)
		{
			return (static_cast<int32_t>(static_cast<uint8_t>(tab)) << 8)
			     | static_cast<uint8_t>(cat);
		}

		// ---------- 数据 ----------
		struct CatalogRow
		{
			int32_t itemId = 0;
			int32_t price = 0;
			int16_t count = 0;
			int8_t tab = 0;
			int8_t cat = 0;
			std::string name;
		};

		struct IndexEntry
		{
			int8_t tab = 0;
			int8_t cat = 0;
			int32_t count = 0;
			std::string name;
		};

		struct TaxonomyEntry
		{
			int32_t id = 0;
			int32_t parentId = 0;
			int8_t clickType = 0;
			std::string clickParam;
			int8_t legacyTab = 0;
			int8_t legacyCat = 0;
			int32_t itemCount = 0;
			std::string name;
		};

		// tab（顶层分类）与分类 chips。INDEX 到达时清 buckets 重建。
		std::vector<IndexEntry> indices;
		std::map<int32_t, std::vector<CatalogRow>> buckets;
		std::unordered_set<int32_t> loaded_buckets;
		std::vector<TaxonomyEntry> taxonomy;

		// 当前选中的 tab/cat（INDEX 的 tab/cat 对，或 TAXONOMY 的 legacy 对）。
		int8_t active_tab = 0;
		int8_t active_cat = 0;
		bool index_received = false;

		// 商品网格当前可见行（过滤 + 搜索后的结果）。
		std::vector<CatalogRow> visible_rows;
		int16_t scroll_offset = 0;

		// 购物车：双击商品 pin 进来（最多 20）。
		struct CartEntry
		{
			int32_t itemId = 0;
			int32_t price = 0;
			std::string name;
		};
		std::vector<CartEntry> cart;
		bool cart_bought = false;   // 购物车购买成功后，清掉已发出的条目

		int32_t cash[3] = { 0, 0, 0 };

		// 搜索过滤（本地 ASCII 子串，大小写不敏感）。
		std::string search_query;

		// 单击/双击判定：连续点同一槽 350ms 内视为双击 pin 到购物车。
		int16_t last_click_slot = -1;
		uint64_t last_click_ticks = 0;
		// 当前鼠标悬停的网格槽（Buy 按钮购买目标），-1 表示未悬停。
		int16_t hover_slot = -1;

		// 焦点灯：PREVIEW 旁绿/红点（Fill + Border 两态都画）。
		bool preview_focused = true;

		// 状态栏提示（购买结果 / 加载中 / 无商品）。
		std::string status_message;

		// ---------- UI 资源 ----------
		Texture wnd_bg;
		Texture preview_bg[3];
		uint8_t preview_bg_index = 0;
		// 按钮直接用 UIElement 基类保护的 buttons map（勿在此重复声明遮蔽）。
		Textfield search_field;
		Slider grid_slider;
		Text cash_label;
		Text status_label;
		Text cart_label;
		Text empty_label;
		Text loading_label;

		// 焦点灯贴图（Fill/Border，两态）；Basic.img 缺失时退化成纯色块。
		Texture focus_fill;
		Texture focus_border;

		// ---------- 预览沙盒（复用 UICashShop 逻辑） ----------
		CharLook avatar_look;
		CharLook avatar_baseline;
		std::unordered_set<int32_t> try_on_items;
		Physics preview_physics;
		Player preview_actor;
		Ladder preview_ladder;
		bool preview_hold_up = false;
		bool preview_hold_down = false;
		Animation preview_effect;
		int32_t preview_effect_item_id = 0;
		bool preview_effect_below = false;

		// ---------- 帮助方法 ----------
		enum Buttons : uint16_t
		{
			BT_CLOSE = 0,
			BT_BUY,
			BT_CART,
			BT_PREVIEW_BG0,
			BT_PREVIEW_BG1,
			BT_PREVIEW_BG2,
			BT_FOCUS,
			BT_INDEX0,           // 顶层 tab 起始（最多 32 个，动态）
			BT_INDEX_MAX = BT_INDEX0 + 32,
			BT_CAT0 = BT_INDEX_MAX,
			BT_CAT_MAX = BT_CAT0 + 64,
			BT_CART_SLOT0 = BT_CAT_MAX,
			BT_CART_SLOT_MAX = BT_CART_SLOT0 + CART_MAX
		};

		void rebuild_visible_rows();
		void ensure_category_requested();
		void draw_grid(float inter) const;
		void draw_cart(float inter) const;
		void draw_index_tabs(float inter) const;
		void draw_preview(float inter) const;
		void draw_focus_light(float inter) const;
		void draw_cash_bar(float inter) const;
		void clear_tooltip() const;
		void show_item_tooltip(int32_t itemId) const;
		int16_t slot_at(Point<int16_t> rel) const;
		int16_t cart_slot_at(Point<int16_t> rel) const;

		// 预览沙盒（照搬 UICashShop 对应函数）
		void init_preview_sandbox();
		void apply_preview_controls();
		void reset_preview_to_baseline();
		void apply_try_on_item(int32_t itemId);
		void clear_conflicting_try_ons(int32_t itemId);
		nl::node cash_item_effect_root(int32_t itemId) const;
		nl::node cash_item_effect_stance(int32_t itemId) const;
		bool has_cash_item_effect(int32_t itemId) const;
		void clear_preview_effect();
		void set_preview_effect(int32_t itemId);
		Point<int16_t> preview_feet_art() const;
	};
}
