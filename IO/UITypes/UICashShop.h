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
#pragma once

#include "../UIElement.h"

#include "../Components/Charset.h"
#include "../Components/Gauge.h"
#include "../Components/Slider.h"

#include "../../Character/Look/CharLook.h"
#include "../../Character/Look/PetLook.h"
#include "../../Character/Player.h"
#include "../../Gameplay/MapleMap/MapInfo.h"
#include "../../Gameplay/Physics/Physics.h"

#include "../../Data/ItemData.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Geometry.h"

#include <unordered_set>

namespace ms
{
	struct CashCommodity;

	class UICashShop : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::CASHSHOP;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UICashShop();

		void draw(float inter) const;
		void update() override;

		Button::State button_pressed(uint16_t buttonid);

		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

		void exit_cashshop();
		void refresh_cs_meta();
		/// 用进店角色职业/名字刷新顶栏（替换 stub 文案）。
		void refresh_identity();

		/// 购买回执状态机（MinMS B3：pending_buy_sn + confirm/clear）。
		/// 发购买包时置位；收 0x57/0x89 成功回执时 confirm、0x5C 失败时 clear。
		/// 本端"购物车"栏是服务端权威 cash_inventory（已购未取），0x57 会把刚买
		/// 的项 push 进去；因此成功回执只清 pending 并刷新 UI，不做删除（删 sn
		/// 会误删刚到货项）。MinMS remove_cart_item_by_sn 针对其客户端 cart_items，
		/// 本端无此容器，故不移植删除动作——这是有证据的语义差异，不是疏漏。
		static int32_t pending_buy_sn;
		static void set_pending_buy(int32_t sn);
		void confirm_purchase_success();
		void clear_pending_buy();

	private:
		void update_items();
		// MinMS PAGE_BTN: page-count / label / button enable sync (feature 4).
		int16_t list_page_count() const;
		void sync_page_chrome();
		void change_list_page(int16_t delta);
		/// 按当前页签重建 items（目录 → 过滤/排序 → 展示序列）。
		void rebuild_items();
		/// 用服务端榜单 SN 回查目录，重建右侧热销榜 5 个道具图标。
		/// 数据在进店后由 openCashShop 包异步落到 cash_best_sellers，
		/// 故不可只在构造期取一次，须随 rebuild_items 一起刷新。
		void refresh_best_icons();
		// 二级分类（MinMS 移植）
		int8_t active_sub_count() const;
		void draw_category_sub_tabs() const;
		int8_t hit_category_sub_tab(Point<int16_t> cursor_relative) const;
		/// 背包类型切换页签命中（Base/Tab2 0..4，竖排 5 个）。-1 表示未命中。
		int8_t hit_bag_tab(Point<int16_t> cursor_relative) const;
		bool matches_sub(const CashCommodity& c) const;

		/// 预览试穿（MinMS apply_try_on 子集：脸/发/装备；无宠物/特效沙盒）。
		nl::node cash_item_effect_root(int32_t item_id) const;
		nl::node cash_item_effect_stance(int32_t item_id, Stance::Id stance) const;
		bool has_cash_item_effect(int32_t item_id) const;
		void clear_preview_effect();
		void set_preview_effect(int32_t item_id);
		void clear_preview_pet();
		void set_preview_pet(int32_t item_id);
		bool can_try_on(int32_t item_id) const;
		void reset_preview_to_baseline();
		void apply_try_on_item(int32_t item_id);
		void toggle_try_on_item(int32_t item_id);
		void clear_conflicting_try_ons(int32_t item_id);
		bool try_on_plate_at(Point<int16_t> cursor_relative, bool clicked);

		void init_preview_sandbox();
		void apply_preview_controls();
		Point<int16_t> preview_feet_art() const;

		// Angel CCSWnd_* 七面板在本端映射为 CashShop.img 子树（不拆 C++ 类）：
		// Char→CSChar / List→CSList / Inventory→CSInventory / Locker→CSLocker
		// Tab→CSTab / Best→GetBestRect / Status→CSStatus
		// CCashShop::Init CreateWnd 钉死 800×600 子窗原点（见 notes）。
		static constexpr uint8_t MAX_ITEMS = 10u;	// GetPlateRect：2 列 × 5 行
		static constexpr int16_t WND_CHAR_X = 0;
		static constexpr int16_t WND_CHAR_Y = 0;
		// 预览井（相对 CSChar / WND_CHAR；MinMS PREVIEW_*）。
		static constexpr int16_t PREVIEW_WELL_X = 24;
		static constexpr int16_t PREVIEW_WELL_Y = 40;
		static constexpr int16_t PREVIEW_WELL_W = 212;
		static constexpr int16_t PREVIEW_FLOOR_Y = 134;
		static constexpr int16_t PREVIEW_LADDER_X = 150;
		static constexpr int16_t PREVIEW_LADDER_Y1 = 10;
		static constexpr int16_t PREVIEW_LADDER_Y2 = 134;
		static constexpr int16_t PREVIEW_SPAWN_X = 86;
		static constexpr int16_t WND_TAB_X = 272;
		static constexpr int16_t WND_TAB_Y = 17;
		static constexpr int16_t WND_LIST_X = 275;
		static constexpr int16_t WND_LIST_Y = 95;
		static constexpr int16_t WND_BEST_X = 690;
		static constexpr int16_t WND_BEST_Y = 157;
		static constexpr int16_t WND_STATUS_X = 254;
		static constexpr int16_t WND_STATUS_Y = 530;
		static constexpr int16_t WND_SEARCH_X = 690;
		static constexpr int16_t WND_SEARCH_Y = 97;
		static constexpr int16_t PLATE_W = 200;
		static constexpr int16_t PLATE_H = 80;
		static constexpr int16_t PLATE_DX = 206;
		static constexpr int16_t PLATE_DY = 81;
		// 二级分类栏布局（2026-11-18 按 live CSTab/Tab/N 508x78 像素实测重定）
		// 贴图下半部 y=42..65 是烘焙的绿/粉/紫色带，二级分类标签必须落在这条带上。
		// 旧值 SUB_ROW_Y=62 / 宽 412 导致标签压在带下沿且窄 96px —— 即"偏上且不在背景中"。
		static constexpr int16_t SUB_ROW_Y = 42;   // 贴图局部 y（相对 CSTab 原点）
		static constexpr int16_t SUB_ROW_H = 24;   // 与 MinMS SUB_ROW_H 一致
		static constexpr int16_t SUB_STRIP_W = 508; // 与 CSTab/Tab 贴图同宽（非旧的 412）
		static constexpr int16_t SUB_HIT_PAD_Y = 4; // 命中区上下各扩 4px（MinMS 同口径）

		// 右侧热销榜（2026-11-18 按 backgrnd.png 实测）：
		// 标题"热销榜" y≈130；五个名次行 y=157..189/227..259/295..327/365..397/433..465。
		// 每行右侧浅色物品槽 x=742..773 (32px)，数字在左侧。数字已烘焙在 backgrnd，
		// 本端只需往浅色槽里画道具 icon。
		static constexpr int16_t BEST_ITEM_X = 742;
		static constexpr int16_t BEST_ITEM_Y0 = 157;
		static constexpr int16_t BEST_ITEM_DY = 69;   // 行距
		static constexpr int16_t BEST_ITEM_SLOT = 32;
		static constexpr uint8_t BEST_ROWS = 5;

		// 左侧面板格子（2026-11-18 按 backgrnd.png 像素实测）：
		// 已购道具：2 行 x 6 列，白格 31x31，x=21..191 (dx=35)，y=349..379 (dy=35)。
		// 背包：3 行 x 4 列，白格 31x31，x=23..158 (dx=35)，y=481..581 (dy=35)。
		static constexpr int16_t PURCH_X0 = 21;
		static constexpr int16_t PURCH_Y0 = 349;
		static constexpr int16_t PURCH_DX = 35;
		static constexpr int16_t PURCH_DY = 35;
		static constexpr uint8_t PURCH_COLS = 6;
		static constexpr uint8_t PURCH_ROWS = 2;
		static constexpr int16_t BACKPACK_X0 = 23;
		static constexpr int16_t BACKPACK_Y0 = 481;
		static constexpr uint8_t BACKPACK_COLS = 4;
		static constexpr uint8_t BACKPACK_ROWS = 3;
		// 背包类型切换页签：Base/Tab2/Enable|Disable/0..4（5 类）。
		// 实测位于背包格右侧 x≈176 起、竖排 5 个，高约 22。
		static constexpr int16_t BAG_TAB_X = 176;
		static constexpr int16_t BAG_TAB_Y0 = 470;
		static constexpr int16_t BAG_TAB_W = 56;
		static constexpr int16_t BAG_TAB_H = 22;
		static constexpr int16_t BAG_TAB_DY = 25;

		// CSTab/Tab 1..9 命中区（相对 CSTab 子窗原点，实测自图元亮块边界）：
		// 首页 热卖 装备 消耗 设置 其他 宠物 礼包 | 购物车
		// 前 8 个 = s9 CategoryType id（= SN 首位，World.java:1354 snid/10000000）
		// 在页签条上的落位；第 9 个是购物车（读 cash_inventory）。
		static constexpr int8_t TAB_CATEGORY[9] = { 8, 1, 2, 3, 4, 5, 6, 7, -1 };

		class Item
		{
		public:
			enum Label : uint8_t
			{
				ACTION,
				BOMB_SALE,
				BONUS,
				EVENT = 4,
				HOT,
				LIMITED,
				LIMITED_BRONZE,
				LIMITED_GOLD,
				LIMITED_SILVER,
				LUNA_CRYSTAL,
				MASTER = 12,
				MUST,
				NEW,
				SALE = 17,
				SPEICAL,
				SPECIAL_PRICE,
				TIME,
				TODAY,
				WEEKLY,
				WONDER_BERRY,
				WORLD_SALE,
				NONE
			};

			Item(int32_t itemid, int32_t sn, int32_t nx_price, Label label, int32_t discount, uint16_t count) : label(label), discount_price(discount), count(count), sn(sn), nx_price(nx_price), data(ItemData::get(itemid)) {}

			Label label;
			int32_t discount_price;
			uint16_t count;
			int32_t sn;
			// 商城 NX 售价来自 Etc/Commodity.img 的 Price；ItemData::get_price 是
			// Item.nx 的商品价值价（仅 5xxxxxx 段同号段），不能当商城标价用。
			int32_t nx_price;

			void draw(const DrawArgument& args) const
			{
				data.get_icon(false).draw(args);
			}

			const std::string get_name() const
			{
				return data.get_name();
			}

			const Texture& get_icon() const
			{
				return data.get_icon(false);
			}

			const int32_t get_price() const
			{
				return nx_price > 0 ? nx_price : data.get_price();
			}

			const int32_t get_sn() const
			{
				return sn;
			}

			const int32_t get_item_id() const
			{
				return data.get_id();
			}

		private:
			const ItemData& data;
		};

		enum Buttons : uint16_t
		{
			BtPreview1,
			BtPreview2,
			BtPreview3,
			BtExit,
			BtChargeNX,
			BtChargeRefresh,
			BtWish,
			BtMileage,
			BtHelp,
			BtCoupon,
			BtNext,
			BtPrev,
			BtDetailPackage,
			BtNonGrade,
			BtBuyAvatar,
			BtDefaultAvatar,
			BtInventory,
			BtSaveAvatar,
			BtTakeoffAvatar,
			BtBuy,
			BtPagePrev = BtBuy + MAX_ITEMS,
			BtPageNext,
			// CSList/BtGift：与 BtBuy 同槽位偏移，枚举从 BtBuy+MAX_ITEMS 起
			BtGift = BtPageNext + 1
		};

		Point<int16_t> BestNew_dim;

		Sprite preview_sprites[3];
		uint8_t preview_index;

		/// CUIAvatar 等价：进店角色立绘（构造时拷贝 player look）。
		CharLook avatar_look;
		/// 进店快照；「我的现状 / 脱下试穿」还原用。
		CharLook avatar_baseline;
		std::unordered_set<int32_t> preview_try_on_items;

		/// 预览沙盒：独立地板 + 预览 Player（仅物理，不发包）。
		Physics preview_physics;
		PetLook preview_pet;
		Animation preview_effect;
		int32_t preview_effect_item_id = 0;
		bool preview_effect_below = false;
		Player preview_actor;
		Ladder preview_ladder;
		bool preview_hold_up = false;
		bool preview_hold_down = false;

		Sprite menu_tabs[9];
		uint8_t menu_index;

		uint8_t category_sub;

		/// 背包类型切换页签（Base/Tab2/Enable|Disable/0..4）与当前选中类。
		Sprite bag_tab_enable[5];
		Sprite bag_tab_disable[5];
		uint8_t backpack_tab = 0;

		/// 右侧热销榜的道具图标：下标 i 对应名次 i+1，空 SN 为 invalid Texture。
		/// 数据来自 UIStateCashShop::cash_best_sellers（服务端 openCashShop 榜单）。
		Texture best_icons[BEST_ROWS];

		/// 把一张道具图标等比缩放居中填进给定矩形槽（复用于货板/热销榜/已购/背包）。
		/// DrawArgument 以 origin*scale 为锚，必须补 origin 抵消贴图原点漂移。
		static void draw_icon_in_slot(const Texture& icon, Point<int16_t> slot_pos,
		                              int16_t slot_size);

		Text job_label;
		Text name_label;
		Text cs_meta_label;

		std::vector<Sprite> promotion_sprites;
		Point<int16_t> promotion_pos;
		int8_t promotion_index;
		bool has_promotion = false;

		Sprite mvp_sprites[7];
		Point<int16_t> mvp_pos;
		bool has_mvp = false;
		uint8_t mvp_grade;
		Gauge mvp_gauge;
		float_t mvp_exp;

		Charset charge_charset;

		Sprite item_base;
		Sprite item_line;
		Sprite item_none;
		std::vector<Sprite> item_labels;
		std::vector<Item> items;
		Text item_name[MAX_ITEMS];
		Text item_price[MAX_ITEMS];
		Text item_discount[MAX_ITEMS];
		Text item_percent[MAX_ITEMS];
		Text item_empty_notice;

		int16_t list_offset;
		Text page_label;
	};
}