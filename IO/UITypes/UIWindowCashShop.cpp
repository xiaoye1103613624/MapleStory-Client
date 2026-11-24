//////////////////////////////////////////////////////////////////////////////////
// 窗口内商城（方案 C）实现。见 UIWindowCashShop.h 顶部的证据来源说明。
//////////////////////////////////////////////////////////////////////////////////
#include "UIWindowCashShop.h"

#include "../UI.h"
#include "../Window.h"
#include "../KeyAction.h"
#include "../Components/AreaButton.h"

#include "../../Gameplay/Stage.h"
#include "../../Character/Player.h"
#include "../../Constants.h"
#include "../../Graphics/DrawArgument.h"
#include "../../Graphics/Geometry.h"
#include "../../Util/Locale.h"
#include "../../Util/Misc.h"

#include "../../Net/Packets/CashShopWindowPackets.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cctype>
#include <sstream>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		// ---------- 上线级节点日志前缀（功能闭环后保留，无临时 DBG 标记） ----------
		constexpr const char* LOG_TAG = "[WindowCashShop]";

		std::string buy_code_text(int8_t code)
		{
			switch (static_cast<UIWindowCashShop::BuyCode>(code))
			{
			case UIWindowCashShop::BuyCode::OK:              return Locale::text("windowcashshop.buy_ok");
			case UIWindowCashShop::BuyCode::NO_CASH:         return Locale::text("windowcashshop.buy_no_cash");
			case UIWindowCashShop::BuyCode::UNKNOWN_ITEM:    return Locale::text("windowcashshop.buy_unknown");
			case UIWindowCashShop::BuyCode::INVENTORY_FULL:  return Locale::text("windowcashshop.buy_inv_full");
			case UIWindowCashShop::BuyCode::NOT_ON_SALE:     return Locale::text("windowcashshop.buy_not_on_sale");
			case UIWindowCashShop::BuyCode::BUSY:            return Locale::text("windowcashshop.buy_busy");
			case UIWindowCashShop::BuyCode::BAD_CART:        return Locale::text("windowcashshop.buy_bad_cart");
			default:                                         return Locale::text("windowcashshop.buy_failed");
			}
		}

		std::string format_cash(int32_t value)
		{
			std::ostringstream oss;
			oss << value;
			return oss.str();
		}

		// ASCII 大小写不敏感子串（搜索仅本地过滤，不发协议）。
		bool ascii_contains_ci(const std::string& haystack, const std::string& needle)
		{
			if (needle.empty())
				return true;
			if (haystack.size() < needle.size())
				return false;
			const auto lower = [](unsigned char c) -> unsigned char
			{
				return static_cast<unsigned char>(std::tolower(c));
			};
			return std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end(),
			                   [&lower](char a, char b) { return lower(a) == lower(b); }) != haystack.end();
		}
	}

	// ============================================================
	// 构造 / 析构
	// ============================================================
	UIWindowCashShop::UIWindowCashShop()
	{
#ifdef USE_NX
		nl::node CashShop = nl::nx::UI["CashShop.img"];
		nl::node Base = CashShop["Base"];
		// WndBg 由用户 probe 确认 760×495，已导入 live CashShop.img。
		nl::node wndbg = Base["WndBg"];
		if (wndbg)
			wnd_bg = Texture(wndbg);
		else if (Base["backgrnd"])
			wnd_bg = Texture(Base["backgrnd"]);   // 退化：老包用 backgrnd

		// 活体预览三背景：Base/Preview/0..2（probe 212×165）。
		nl::node Preview = Base["Preview"];
		for (size_t i = 0; i < 3; ++i)
		{
			if (Preview[static_cast<int>(i)])
				preview_bg[i] = Texture(Preview[static_cast<int>(i)]);
		}

		// BtBuy/BtCart 四态 76×18（probe 已确认）。
		auto make_btn = [&](uint16_t id, nl::node src, int16_t x, int16_t y, const char* site)
		{
			if (!src)
			{
				LOG(LOG_WARN, LOG_TAG << " missing button node: " << site);
				return;
			}
			buttons[id] = std::make_unique<MapleButton>(src, x, y);
		};
		make_btn(BT_BUY, Base["BtBuy"], PREVIEW_X + 4, 440, "BtBuy");
		make_btn(BT_CART, Base["BtCart"], PREVIEW_X + 90, 440, "BtCart");

		// 关闭按钮：复用 Basic.img/close_btn（本端通用四态关闭钮）。
		nl::node close = nl::nx::UI["Basic.img"]["close_btn"];
		if (close)
			buttons[BT_CLOSE] = std::make_unique<MapleButton>(close, WND_W - 24, 4);
		else
			LOG(LOG_WARN, LOG_TAG << " Basic.img/close_btn unavailable");

		// 焦点灯两态：优先 CashShop.img/Base/FocusLight（如存在），否则纯色块兜底。
		nl::node focus = Base["FocusLight"];
		if (focus)
		{
			if (focus["Fill"])  focus_fill = Texture(focus["Fill"]);
			if (focus["Border"]) focus_border = Texture(focus["Border"]);
		}
#endif

		// 网格滚动条（参考包 VScr4 的客户端等价物）：本端 Slider 抽象掉贴图，
		// 用 DEFAULT_SILVER 类型即可；位置落在网格右缘。
		grid_slider = Slider(Slider::Type::DEFAULT_SILVER,
		                     Range<int16_t>(CELL_Y0, CELL_Y0 + GRID_ROWS * CELL_H),
		                     CELL_X0 + GRID_COLS * CELL_W + 2, GRID_ROWS, 1,
		                     [&](bool upwards)
		                     {
			                        const int16_t shift = upwards ? -1 : 1;
			                        const int16_t max_offset = static_cast<int16_t>(
				                        std::max<int>(0, static_cast<int>(visible_rows.size()) - GRID_SLOTS));
			                        scroll_offset = std::clamp<int16_t>(
				                        static_cast<int16_t>(scroll_offset + shift), 0, max_offset);
		                     });

		// 搜索框（本地 ASCII 子串过滤）。
		search_field = Textfield(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
		                         Rectangle<int16_t>(CELL_X0 + 268, 56, CELL_X0 + 520, 72), 24, 12);
		search_field.set_text_callback([&]() { search_query = search_field.get_text(); rebuild_visible_rows(); });

		// 文本。
		cash_label = Text(Text::Font::A11B, Text::Alignment::LEFT, Color::Name::MINESHAFT);
		status_label = Text(Text::Font::A11B, Text::Alignment::LEFT, Color::Name::MINESHAFT);
		cart_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::MINESHAFT);
		empty_label = Text(Text::Font::A11B, Text::Alignment::CENTER, Color::Name::MINESHAFT,
		                   Locale::text("windowcashshop.no_items"));
		loading_label = Text(Text::Font::A11B, Text::Alignment::CENTER, Color::Name::MINESHAFT,
		                     Locale::text("windowcashshop.loading"));

		// 底板尺寸 / 拖拽区（标题条 20px）。
		dimension = Point<int16_t>(WND_W, WND_H);
		dragarea = Point<int16_t>(WND_W, 20);
		center_if_position_unset(dimension);   // 首次打开居中；用户拖过则尊重保存位置
		clamp_drag_position();

		// 购物车槽位命中区（AreaButton 便于复用按钮状态机；逻辑双击 pin）。
		for (int16_t i = 0; i < CART_MAX; ++i)
		{
			const int16_t col = i % CART_COLS;
			const int16_t row = i / CART_COLS;
			buttons[BT_CART_SLOT0 + i] = std::make_unique<AreaButton>(
				Point<int16_t>(CART_X0 + col * (CART_BOX + CART_GAP),
				               CART_Y0 + row * (CART_BOX + CART_GAP)),
				Point<int16_t>(CART_BOX, CART_BOX));
		}

		// 预览背景切换三钮（小 AreaButton，落在预览井右上）。
		for (uint8_t i = 0; i < 3; ++i)
		{
			buttons[BT_PREVIEW_BG0 + i] = std::make_unique<AreaButton>(
				Point<int16_t>(PREVIEW_X + PREVIEW_W - 48 + i * 15, PREVIEW_Y - 18),
				Point<int16_t>(14, 14));
		}

		// 焦点灯切换钮。
		buttons[BT_FOCUS] = std::make_unique<AreaButton>(
			Point<int16_t>(PREVIEW_X - 22, PREVIEW_Y + 4), Point<int16_t>(16, 16));

		// 预览沙盒（照搬 UICashShop 的 init_preview_sandbox）。
		init_preview_sandbox();

		status_message = Locale::text("windowcashshop.loading");

		// 开场请求：分类索引 + 余额（服务端回 RESP_INDEX + RESP_CASH）。
		CashShopWindowRequestCatalogPacket().dispatch();
		LOG(LOG_INFO, LOG_TAG << " window opened; REQUEST_CATALOG sent");
	}

	UIWindowCashShop::~UIWindowCashShop()
	{
		clear_tooltip();
		CashShopWindowSync::clear();
		LOG(LOG_DEBUG, LOG_TAG << " window destroyed");
	}

	// ============================================================
	// 数据落地（主线程，由 CashShopWindowSync::pump 调用）
	// ============================================================
	void UIWindowCashShop::apply_open(const int32_t in_cash[3])
	{
		for (int i = 0; i < 3; ++i)
			cash[i] = in_cash[i];
		LOG(LOG_INFO, LOG_TAG << " OPEN NX=" << cash[0] << " Maple=" << cash[1] << " Prepaid=" << cash[2]);
	}

	void UIWindowCashShop::apply_cash(const int32_t in_cash[3])
	{
		for (int i = 0; i < 3; ++i)
			cash[i] = in_cash[i];
		LOG(LOG_DEBUG, LOG_TAG << " CASH refreshed");
	}

	void UIWindowCashShop::apply_index(const std::vector<CashShopWindowIndexRow>& rows)
	{
		// INDEX 到达即清 buckets 重建（参考包 :3388-3407 同语义）。
		buckets.clear();
		loaded_buckets.clear();
		indices.clear();
		indices.reserve(rows.size());
		for (const auto& r : rows)
		{
			IndexEntry e;
			e.tab = r.tab;
			e.cat = r.cat;
			e.count = r.count;
			indices.push_back(std::move(e));
		}
		index_received = true;

		// 选第一个可用的分类作为默认活动分类。
		if (!indices.empty())
		{
			active_tab = indices.front().tab;
			active_cat = indices.front().cat;
			ensure_category_requested();
		}
		LOG(LOG_INFO, LOG_TAG << " INDEX rows=" << indices.size()
			<< " active=(" << static_cast<int>(active_tab) << "," << static_cast<int>(active_cat) << ")");
	}

	void UIWindowCashShop::apply_catalog(int8_t clear, int8_t tab, int8_t cat,
	                                      const std::vector<CashShopWindowCatalogRow>& rows)
	{
		const int32_t key = bucket_key(tab, cat);
		auto& bucket = buckets[key];
		if (clear != 0)
			bucket.clear();
		for (const auto& r : rows)
		{
			CatalogRow e;
			e.itemId = r.itemId;
			e.price = r.price;
			e.count = r.count;
			e.tab = r.tab;
			e.cat = r.cat;
			e.name = r.name;
			bucket.push_back(std::move(e));
		}
		loaded_buckets.insert(key);   // 空分类也标 loaded → 结束"加载中"
		if (tab == active_tab && cat == active_cat)
			rebuild_visible_rows();
		LOG(LOG_DEBUG, LOG_TAG << " CATALOG (" << static_cast<int>(tab) << ","
			<< static_cast<int>(cat) << ") +" << rows.size() << " rows");
	}

	void UIWindowCashShop::apply_buy(int8_t code, int32_t itemId)
	{
		status_message = buy_code_text(code) + " (" + std::to_string(itemId) + ")";
		LOG(LOG_INFO, LOG_TAG << " BUY result code=" << static_cast<int>(code) << " item=" << itemId);
	}

	void UIWindowCashShop::apply_buy_cart(int8_t code, int32_t itemId, int8_t delivered, int32_t spent)
	{
		if (code == static_cast<int8_t>(BuyCode::OK))
		{
			std::ostringstream oss;
			oss << Locale::text("windowcashshop.cart_bought") << " " << static_cast<int>(delivered)
			    << " / " << spent;
			status_message = oss.str();
			cart_bought = true;   // update() 里 retire 已发条目
		}
		else
		{
			status_message = buy_code_text(code);
			if (itemId != 0)
				status_message += " (" + std::to_string(itemId) + ")";
			status_message += " " + Locale::text("windowcashshop.cart_nothing");
		}
		LOG(LOG_INFO, LOG_TAG << " BUY_CART code=" << static_cast<int>(code)
			<< " delivered=" << static_cast<int>(delivered) << " spent=" << spent);
	}

	void UIWindowCashShop::apply_taxonomy(const std::vector<CashShopWindowTaxonomyRow>& rows)
	{
		taxonomy.clear();
		taxonomy.reserve(rows.size());
		for (const auto& r : rows)
		{
			TaxonomyEntry e;
			e.id = r.id; e.parentId = r.parentId; e.clickType = r.clickType;
			e.clickParam = r.clickParam; e.legacyTab = r.legacyTab; e.legacyCat = r.legacyCat;
			e.itemCount = r.itemCount; e.name = r.name;
			taxonomy.push_back(std::move(e));
		}
		if (!taxonomy.empty())
		{
			active_tab = taxonomy.front().legacyTab;
			active_cat = taxonomy.front().legacyCat;
			ensure_category_requested();
		}
		LOG(LOG_INFO, LOG_TAG << " TAXONOMY rows=" << taxonomy.size());
	}

	// ============================================================
	// 分类请求 / 过滤
	// ============================================================
	void UIWindowCashShop::ensure_category_requested()
	{
		const int32_t key = bucket_key(active_tab, active_cat);
		if (loaded_buckets.count(key))
			return;
		CashShopWindowRequestCategoryPacket(active_tab, active_cat).dispatch();
		LOG(LOG_DEBUG, LOG_TAG << " REQUEST_CATEGORY (" << static_cast<int>(active_tab)
			<< "," << static_cast<int>(active_cat) << ")");
	}

	void UIWindowCashShop::rebuild_visible_rows()
	{
		visible_rows.clear();
		const int32_t key = bucket_key(active_tab, active_cat);
		auto it = buckets.find(key);
		if (it == buckets.end())
			return;
		visible_rows.reserve(it->second.size());
		for (const auto& r : it->second)
		{
			if (ascii_contains_ci(r.name, search_query) ||
			    ascii_contains_ci(std::to_string(r.itemId), search_query))
				visible_rows.push_back(r);
		}
		scroll_offset = 0;
	}

	// ============================================================
	// 绘制
	// ============================================================
	void UIWindowCashShop::draw(float inter) const
	{
		if (wnd_bg.is_valid())
			wnd_bg.draw(DrawArgument(position, 1.0f, 1.0f, fade_alpha()));

		draw_index_tabs(inter);
		draw_grid(inter);
		draw_cash_bar(inter);
		draw_preview(inter);
		draw_cart(inter);
		draw_focus_light(inter);

		// 搜索框提示词（空时显示）。
		if (search_field.get_text().empty())
		{
			Text hint(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::MINESHAFT,
			          Locale::text("windowcashshop.search_hint"));
			hint.draw(position + Point<int16_t>(CELL_X0 + 270, 57));
		}
		search_field.draw(position);

		draw_buttons(inter);
		grid_slider.draw(position);

		if (!status_message.empty())
		{
			Text status_text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::MINESHAFT, status_message);
			status_text.draw(position + Point<int16_t>(CELL_X0, 466));
		}
	}

	void UIWindowCashShop::draw_index_tabs(float inter) const
	{
		// 顶层分类 chips：INDEX 的 tab 去重后横排在 y=26。
		int16_t x = CELL_X0;
		int8_t last_tab = -1;
		for (size_t i = 0; i < indices.size(); ++i)
		{
			if (indices[i].tab == last_tab)
				continue;
			last_tab = indices[i].tab;
			const bool selected = (indices[i].tab == active_tab);
			Color::Name color = selected ? Color::Name::WHITE : Color::Name::MINESHAFT;
			Text tab(Text::Font::A11B, Text::Alignment::LEFT, color,
			         indices[i].name.empty() ? std::string("T") + std::to_string(static_cast<int>(indices[i].tab))
			                                 : indices[i].name);
			tab.draw(position + Point<int16_t>(x, 26));
			x += static_cast<int16_t>(tab.width() + 12);
			if (x > PREVIEW_X - 80)
				break;
		}

		// 二级分类 chips：当前 tab 下所有 cat，横排在 y=60。
		int16_t cx = CELL_X0;
		for (const auto& e : indices)
		{
			if (e.tab != active_tab)
				continue;
			const bool selected = (e.cat == active_cat);
			Color::Name color = selected ? Color::Name::WHITE : Color::Name::MINESHAFT;
			Text chip(Text::Font::A11M, Text::Alignment::LEFT, color,
			          "[" + std::to_string(static_cast<int>(e.cat)) + "] " + std::to_string(e.count));
			chip.draw(position + Point<int16_t>(cx, 62));
			cx += static_cast<int16_t>(chip.width() + 8);
			if (cx > PREVIEW_X - 80)
				break;
		}
	}

	void UIWindowCashShop::draw_grid(float inter) const
	{
		const int16_t count = static_cast<int16_t>(visible_rows.size());
		if (count == 0)
		{
			(loaded_buckets.count(bucket_key(active_tab, active_cat))
			 ? empty_label : loading_label).draw(position + Point<int16_t>(250, 240));
			return;
		}

		for (int16_t slot = 0; slot < GRID_SLOTS; ++slot)
		{
			const int16_t idx = scroll_offset + slot;
			if (idx >= count)
				break;
			const auto& row = visible_rows[static_cast<size_t>(idx)];
			const int16_t col = slot % GRID_COLS;
			const int16_t r = slot / GRID_COLS;
			const Point<int16_t> cell(position.x() + CELL_X0 + col * CELL_W,
			                          position.y() + CELL_Y0 + r * CELL_H);
			const Texture& icon = ItemData::get(row.itemId).get_icon(false);
			if (icon.is_valid())
			{
				const Point<int16_t> dim = icon.get_dimensions();
				const float scale = std::min(1.0f, 32.0f / static_cast<float>(std::max(dim.x(), dim.y())));
				const Point<int16_t> origin = icon.get_origin();
				icon.draw(DrawArgument(
					cell + Point<int16_t>(static_cast<int16_t>(CELL_W / 2 + origin.x() * scale),
					                      static_cast<int16_t>(5 + dim.y() * scale + origin.y() * scale)),
					scale, scale));
			}

			Text name(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::MINESHAFT, row.name);
			name.draw(cell + Point<int16_t>(CELL_W / 2, 44));
			Text price(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::MINESHAFT,
			           format_cash(row.price) + " " + Locale::text("windowcashshop.currency"));
			price.draw(cell + Point<int16_t>(CELL_W / 2, 60));

			// 在购物车中的条目画一个角标（focus 绿点亮起）。
			if (std::find_if(cart.begin(), cart.end(), [&](const CartEntry& c) { return c.itemId == row.itemId; }) != cart.end())
			{
				Text in_cart(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::GREEN,
				             Locale::text("windowcashshop.in_cart"));
				in_cart.draw(cell + Point<int16_t>(CELL_W / 2, 74));
			}
		}
	}

	void UIWindowCashShop::draw_cash_bar(float inter) const
	{
		std::ostringstream oss;
		oss << Locale::text("windowcashshop.balance") << ": "
		    << Locale::text("windowcashshop.currency") << " " << format_cash(cash[0])
		    << "  |  " << Locale::text("windowcashshop.maple_point") << " " << format_cash(cash[1])
		    << "  |  " << Locale::text("windowcashshop.prepaid") << " " << format_cash(cash[2]);
		Text balance_text(Text::Font::A11B, Text::Alignment::LEFT, Color::Name::MINESHAFT, oss.str());
		balance_text.draw(position + Point<int16_t>(CELL_X0, 8));
	}

	void UIWindowCashShop::draw_preview(float inter) const
	{
		// 三背景（212×165 裁到 182 宽，与参考包 kPlayW/kPrevArtSrcX 同口径）。
		if (preview_bg[preview_bg_index].is_valid())
			preview_bg[preview_bg_index].draw(
				DrawArgument(position + Point<int16_t>(PREVIEW_X, PREVIEW_Y), 1.0f, 1.0f));

		// 现金特效：角色脚底坐标（复用 preview_feet_art）。
		avatar_look.draw(DrawArgument(position + Point<int16_t>(PREVIEW_X, PREVIEW_Y) + preview_feet_art()), inter);
		if (preview_effect_item_id > 0)
		{
			preview_effect.draw(
				DrawArgument(position + Point<int16_t>(PREVIEW_X, PREVIEW_Y) + preview_feet_art()), 0.0f);
		}
	}

	void UIWindowCashShop::draw_cart(float inter) const
	{
		std::ostringstream oss;
		oss << Locale::text("windowcashshop.cart") << " " << cart.size() << "/" << CART_MAX;
		Text cart_text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::MINESHAFT, oss.str());
		cart_text.draw(position + Point<int16_t>(CART_X0, CART_Y0 - 18));

		for (size_t i = 0; i < cart.size(); ++i)
		{
			const int16_t col = static_cast<int16_t>(i % CART_COLS);
			const int16_t row = static_cast<int16_t>(i / CART_COLS);
			const Point<int16_t> cell(position.x() + CART_X0 + col * (CART_BOX + CART_GAP),
			                          position.y() + CART_Y0 + row * (CART_BOX + CART_GAP));
			const Texture& icon = ItemData::get(cart[i].itemId).get_icon(false);
			if (icon.is_valid())
				icon.draw(cell);
		}
	}

	void UIWindowCashShop::draw_focus_light(float inter) const
	{
		// 绿/红点：两态都画 Fill + Border（Border 在外，Fill 在内）。
		const Point<int16_t> p = position + Point<int16_t>(PREVIEW_X + PREVIEW_W - 10, PREVIEW_Y + 4);
		if (focus_border.is_valid())
			focus_border.draw(DrawArgument(p, fade_alpha()));
		else
			ColorBox(10, 10, Color::Name::BLACK, 1.0f).draw(DrawArgument(p - Point<int16_t>(1, 1)));
		if (focus_fill.is_valid())
			focus_fill.draw(DrawArgument(p, fade_alpha()));
		else if (preview_focused)
			ColorBox(8, 8, Color::Name::GREEN, 1.0f).draw(DrawArgument(p));
		else
			ColorBox(8, 8, Color::Name::RED, 1.0f).draw(DrawArgument(p));
	}

	// ============================================================
	// 输入
	// ============================================================
	Cursor::State UIWindowCashShop::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		const Point<int16_t> rel = cursorpos - position;

		// 搜索框优先（焦点态由 UI::focus_textfield 管理，这里只传光标）。
		const Cursor::State search_state = search_field.send_cursor(rel, clicked);
		if (search_state != Cursor::State::IDLE)
			return search_state;

		// 滚动条。
		grid_slider.send_cursor(rel, clicked);

		// 网格 hover tooltip + 单击试穿 / 双击 pin 购物车。
		const int16_t slot = slot_at(rel);
		hover_slot = slot;
		if (slot >= 0)
		{
			const int16_t idx = scroll_offset + slot;
			if (idx < static_cast<int16_t>(visible_rows.size()))
			{
				const auto& row = visible_rows[static_cast<size_t>(idx)];
				show_item_tooltip(row.itemId);
				if (clicked)
				{
					// 单击试穿（装备类）；双击由 UI::doubleclick 派发到 send_cursor 的另一路，
					// 这里以连续点击同一槽判定 pin（与参考包双击语义等价）。
					apply_try_on_item(row.itemId);
					if (last_click_slot == slot && SDL_GetTicks() - last_click_ticks < 350)
					{
						if (cart.size() < CART_MAX &&
						    std::find_if(cart.begin(), cart.end(), [&](const CartEntry& c) { return c.itemId == row.itemId; }) == cart.end())
						{
							cart.push_back({ row.itemId, row.price, row.name });
							status_message = Locale::text("windowcashshop.cart_added") + " " + row.name;
							LOG(LOG_DEBUG, LOG_TAG << " cart pin item=" << row.itemId);
						}
						last_click_slot = -1;
					}
					else
					{
						last_click_slot = slot;
						last_click_ticks = SDL_GetTicks();
					}
				}
			}
		}
		else
		{
			const int16_t cslot = cart_slot_at(rel);
			if (cslot >= 0 && clicked && cslot < static_cast<int16_t>(cart.size()))
			{
				// 点购物车条目：从购物车移除并撤销试穿。
				cart.erase(cart.begin() + cslot);
				reset_preview_to_baseline();
				for (const auto& e : cart)
					apply_try_on_item(e.itemId);
				status_message = Locale::text("windowcashshop.cart_removed");
			}
			else
			{
				clear_tooltip();
			}
		}

		// 按钮状态机（与 UIShop 同款四态）。
		for (auto& kv : buttons)
		{
			Button* btn = kv.second.get();
			if (!btn)
				continue;
			const Rectangle<int16_t> b = btn->bounds(position);
			const bool hover = b.contains(cursorpos);
			if (hover)
			{
				if (clicked)
				{
					btn->set_state(Button::State::PRESSED);
					btn->set_state(button_pressed(kv.first));
				}
				else if (btn->get_state() != Button::State::PRESSED)
				{
					btn->set_state(Button::State::MOUSEOVER);
				}
			}
			else if (btn->get_state() != Button::State::NORMAL)
			{
				btn->set_state(Button::State::NORMAL);
			}
		}

		return UIDragElement::send_cursor(clicked, cursorpos);
	}

	void UIWindowCashShop::send_scroll(double yoffset)
	{
		grid_slider.send_scroll(yoffset);
	}

	void UIWindowCashShop::send_key(int32_t keycode, bool pressed, bool escape)
	{
		// 预览沙盒按键：须在 ESC 之前（照搬 UICashShop::send_key）。
		switch (static_cast<KeyAction::Id>(keycode))
		{
		case KeyAction::Id::LEFT:  preview_actor.send_action(KeyAction::Id::LEFT, pressed); return;
		case KeyAction::Id::RIGHT: preview_actor.send_action(KeyAction::Id::RIGHT, pressed); return;
		case KeyAction::Id::UP:    preview_hold_up = pressed; preview_actor.send_action(KeyAction::Id::UP, pressed); return;
		case KeyAction::Id::DOWN:  preview_hold_down = pressed; preview_actor.send_action(KeyAction::Id::DOWN, pressed); return;
		case KeyAction::Id::JUMP:  preview_actor.send_action(KeyAction::Id::JUMP, pressed); return;
		default: break;
		}

		if (pressed && escape)
			close_window();
	}

	void UIWindowCashShop::remove_cursor()
	{
		clear_tooltip();
		preview_hold_up = false;
		preview_hold_down = false;
		UIDragElement::remove_cursor();
	}

	UIElement::Type UIWindowCashShop::get_type() const
	{
		return TYPE;
	}

	void UIWindowCashShop::close_window()
	{
		clear_tooltip();
		CashShopWindowSync::clear();
		deactivate();
		LOG(LOG_INFO, LOG_TAG << " window closed");
	}

	// ============================================================
	// 按钮
	// ============================================================
	Button::State UIWindowCashShop::button_pressed(uint16_t id)
	{
		switch (id)
		{
		case BT_CLOSE:
			close_window();
			return Button::State::NORMAL;

		case BT_BUY:
			if (!visible_rows.empty())
			{
				const int16_t idx = scroll_offset + hover_slot;
				if (idx < static_cast<int16_t>(visible_rows.size()))
				{
					const int32_t itemId = visible_rows[static_cast<size_t>(idx)].itemId;
					CashShopWindowBuyPacket(itemId).dispatch();
					status_message = Locale::text("windowcashshop.buying") + " " + std::to_string(itemId);
					LOG(LOG_INFO, LOG_TAG << " BUY sent item=" << itemId);
				}
				else
				{
					status_message = Locale::text("windowcashshop.select_first");
				}
			}
			return Button::State::NORMAL;

		case BT_CART:
			if (cart.empty())
			{
				status_message = Locale::text("windowcashshop.cart_empty");
			}
			else
			{
				std::vector<int32_t> ids;
				ids.reserve(cart.size());
				for (const auto& e : cart)
					ids.push_back(e.itemId);
				CashShopWindowBuyCartPacket(ids).dispatch();
				status_message = Locale::text("windowcashshop.cart_buying");
				LOG(LOG_INFO, LOG_TAG << " BUY_CART sent n=" << ids.size());
			}
			return Button::State::NORMAL;

		case BT_PREVIEW_BG0: case BT_PREVIEW_BG1: case BT_PREVIEW_BG2:
			preview_bg_index = static_cast<uint8_t>(id - BT_PREVIEW_BG0);
			status_message = Locale::text("windowcashshop.preview_bg") + " " + std::to_string(preview_bg_index + 1);
			return Button::State::MOUSEOVER;

		case BT_FOCUS:
			preview_focused = !preview_focused;
			return Button::State::MOUSEOVER;

		default:
			if (id >= BT_INDEX0 && id < BT_INDEX_MAX)
			{
				const size_t i = id - BT_INDEX0;
				if (i < indices.size())
				{
					active_tab = indices[i].tab;
					active_cat = indices[i].cat;
					rebuild_visible_rows();
					ensure_category_requested();
					status_message = Locale::text("windowcashshop.loading");
				}
				return Button::State::MOUSEOVER;
			}
			break;
		}
		return Button::State::NORMAL;
	}

	// ============================================================
	// 命中 / tooltip
	// ============================================================
	int16_t UIWindowCashShop::slot_at(Point<int16_t> rel) const
	{
		const int16_t x = rel.x() - CELL_X0;
		const int16_t y = rel.y() - CELL_Y0;
		if (x < 0 || y < 0 || x >= GRID_COLS * CELL_W || y >= GRID_ROWS * CELL_H)
			return -1;
		return static_cast<int16_t>((y / CELL_H) * GRID_COLS + (x / CELL_W));
	}

	int16_t UIWindowCashShop::cart_slot_at(Point<int16_t> rel) const
	{
		const int16_t x = rel.x() - CART_X0;
		const int16_t y = rel.y() - CART_Y0;
		if (x < 0 || y < 0)
			return -1;
		const int16_t col = x / (CART_BOX + CART_GAP);
		const int16_t row = y / (CART_BOX + CART_GAP);
		if (col >= CART_COLS || row >= CART_ROWS)
			return -1;
		return static_cast<int16_t>(row * CART_COLS + col);
	}

	void UIWindowCashShop::clear_tooltip() const
	{
		UI::get().clear_tooltip(Tooltip::Parent::SHOP);
	}

	void UIWindowCashShop::show_item_tooltip(int32_t itemId) const
	{
		UI::get().show_item(Tooltip::Parent::SHOP, itemId);
	}

	// ============================================================
	// Update：消费购物车成功旗 + 驱动预览沙盒
	// ============================================================
	void UIWindowCashShop::update()
	{
		search_field.update();
		for (auto& kv : buttons)
			if (kv.second)
				kv.second->update();

		if (cart_bought)
		{
			cart_bought = false;
			cart.clear();                 // 服务端校验全有或全无，成功后 retired
			reset_preview_to_baseline();
		}

		// 预览控件（照搬 UICashShop::apply_preview_controls）。
		apply_preview_controls();
	}

	// ============================================================
	// 预览沙盒（照搬 UICashShop.cpp:1208-1371）
	// ============================================================
	void UIWindowCashShop::init_preview_sandbox()
	{
		try
		{
			const CharLook& playerlook = Stage::get().get_player().get_look();
			if (playerlook.get_face() && playerlook.get_body() && playerlook.get_hair())
			{
				preview_actor.get_look() = playerlook;
				avatar_look = playerlook;
				avatar_look.set_direction(false);
				avatar_look.set_stance(Stance::Id::STAND1);
				avatar_look.set_expression(Expression::Id::DEFAULT);
				avatar_baseline = avatar_look;
			}
		}
		catch (...)
		{
			// 无角色上下文（调试预览）：由 CharLook 空守卫兜底不崩。
		}

		preview_physics.install_floor(0, PREVIEW_W - 1, FLOOR_Y);
		preview_ladder = Ladder(LADDER_X, LADDER_Y1, LADDER_Y2, true);
		preview_actor.use_sandbox_fht(&preview_physics.get_fht());
		preview_actor.get_stats().set_stat(MapleStat::Id::HP, 100);
		preview_actor.get_stats().set_stat(MapleStat::Id::MAXHP, 100);
		preview_actor.get_stats().init_totalstats();
		preview_actor.set_state(Char::State::STAND);
		preview_actor.set_direction(true);
		preview_actor.respawn(Point<int16_t>(PREVIEW_W / 2, static_cast<int16_t>(FLOOR_Y - 8)), false);
		preview_actor.set_direction(true);
		preview_hold_up = false;
		preview_hold_down = false;
	}

	void UIWindowCashShop::apply_preview_controls()
	{
		if (!preview_actor.is_climbing())
		{
			const Point<int16_t> pos = preview_actor.get_position();
			if (preview_hold_up && preview_ladder.inrange(pos, true))
				preview_actor.set_ladder(preview_ladder);
			else if (preview_hold_down && preview_ladder.inrange(pos, false))
				preview_actor.set_ladder(preview_ladder);
		}
		preview_actor.update(preview_physics);

		Stance::Id st = Stance::Id::STAND1;
		switch (preview_actor.get_char_state())
		{
		case Char::State::WALK:  st = Stance::Id::WALK1; break;
		case Char::State::FALL:  st = Stance::Id::JUMP; break;
		case Char::State::LADDER: st = Stance::Id::LADDER; break;
		case Char::State::ROPE:   st = Stance::Id::ROPE; break;
		case Char::State::PRONE:  st = Stance::Id::PRONE; break;
		default: break;
		}
		avatar_look.set_stance(st);
		avatar_look.set_direction(preview_actor.getflip());
		avatar_look.update(Constants::TIMESTEP);
	}

	Point<int16_t> UIWindowCashShop::preview_feet_art() const
	{
		const Point<int16_t> feet = preview_actor.get_position();
		return Point<int16_t>(static_cast<int16_t>(feet.x() + (212 - PREVIEW_W) / 2),
		                      feet.y());
	}

	void UIWindowCashShop::reset_preview_to_baseline()
	{
		avatar_look = avatar_baseline;
		avatar_look.set_direction(false);
		avatar_look.set_stance(Stance::Id::STAND1);
		avatar_look.set_expression(Expression::Id::DEFAULT);
		try_on_items.clear();
		clear_preview_effect();
	}

	void UIWindowCashShop::clear_conflicting_try_ons(int32_t itemId)
	{
		const int32_t type = itemId / 10000;
		if (type != 2 && type != 3)
			return;
		for (auto it = try_on_items.begin(); it != try_on_items.end();)
		{
			if (*it / 10000 == type)
				it = try_on_items.erase(it);
			else
				++it;
		}
	}

	void UIWindowCashShop::apply_try_on_item(int32_t itemId)
	{
		const int32_t type = itemId / 10000;
		// 现金装备（1x/2x/3x 段）走 CharLook::add_equip；消耗/特效走 cash_item_effect。
		if (type >= 1 && type <= 5)
		{
			clear_conflicting_try_ons(itemId);
			avatar_look.add_equip(itemId);
			try_on_items.insert(itemId);
		}
		if (has_cash_item_effect(itemId))
			set_preview_effect(itemId);
	}

	nl::node UIWindowCashShop::cash_item_effect_root(int32_t itemId) const
	{
		if (itemId <= 0)
			return {};
		const std::string prefix = "0" + std::to_string(itemId / 10000);
		const std::string strid = "0" + std::to_string(itemId);
		return nl::nx::Item["Cash"][prefix + ".img"][strid]["effect"];
	}

	nl::node UIWindowCashShop::cash_item_effect_stance(int32_t itemId) const
	{
		nl::node effect = cash_item_effect_root(itemId);
		if (!effect)
			return {};
		const char* literals[] = { "stand1", "stand", "default" };
		for (const char* lit : literals)
			if (nl::node hit = effect[lit])
				return hit;
		return effect["0"];
	}

	bool UIWindowCashShop::has_cash_item_effect(int32_t itemId) const
	{
		return static_cast<bool>(cash_item_effect_stance(itemId));
	}

	void UIWindowCashShop::clear_preview_effect()
	{
		preview_effect_item_id = 0;
		preview_effect = Animation();
		preview_effect_below = false;
	}

	void UIWindowCashShop::set_preview_effect(int32_t itemId)
	{
		nl::node src = cash_item_effect_stance(itemId);
		if (!src)
		{
			clear_preview_effect();
			return;
		}
		int32_t z = 0;
		if (src["z"].data_type() == nl::node::type::integer)
			z = src["z"];
		preview_effect_item_id = itemId;
		preview_effect_below = z < 0;
		preview_effect = Animation(src);
	}

	// ============================================================
	// 收包队列消费（主线程）
	// ============================================================
	std::mutex CashShopWindowSync::mtx;
	std::queue<CashShopWindowEvent> CashShopWindowSync::queue;

	void CashShopWindowSync::enqueue(CashShopWindowEvent event)
	{
		std::lock_guard<std::mutex> lk(mtx);
		queue.push(std::move(event));
	}

	void CashShopWindowSync::clear()
	{
		std::lock_guard<std::mutex> lk(mtx);
		while (!queue.empty())
			queue.pop();
	}

	void CashShopWindowSync::pump()
	{
		// 在锁内一次性 swap 出来，锁外处理，避免持锁调用 UI::emplace。
		std::queue<CashShopWindowEvent> local;
		{
			std::lock_guard<std::mutex> lk(mtx);
			local.swap(queue);
		}

		while (!local.empty())
		{
			CashShopWindowEvent ev = std::move(local.front());
			local.pop();

			auto ui = UI::get().get_element<UIWindowCashShop>();
			if (ev.kind == 0)   // OPEN：窗口由收包事件在主线程创建
			{
				if (!ui)
				{
					UI::get().emplace<UIWindowCashShop>();
					ui = UI::get().get_element<UIWindowCashShop>();
				}
				if (ui)
					ui->apply_open(ev.cash);
				continue;
			}
			if (!ui)
			{
				LOG(LOG_DEBUG, LOG_TAG << " pump: drop resp=" << static_cast<int>(ev.kind)
					<< " (window not open)");
				continue;
			}
			switch (ev.kind)
			{
			case 1:  ui->apply_catalog(ev.clear, ev.tab, ev.cat, ev.rows); break;
			case 2:  ui->apply_buy(ev.code, ev.itemId); break;
			case 3:  ui->apply_cash(ev.cash); break;
			case 4:  ui->apply_buy_cart(ev.code, ev.itemId, ev.delivered, ev.spent); break;
			case 5:  ui->apply_index(ev.index); break;
			case 6:  ui->apply_taxonomy(ev.taxonomy); break;
			default: break;
			}
		}
	}
}
