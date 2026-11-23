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
#include "UICashShop.h"

#include "UICashGachapon.h"
#include "UICharInfo.h"
#include "UIItemInventory.h"
#include "UIMiscShells.h"
#include "UIRemoteGachapon.h"
#include "UISoftKey.h"
#include "UINotice.h"
#include "UICashBuyConfirm.h"

#include "../UI.h"
#include "../Window.h"
#include "../KeyAction.h"

#include "../Components/MapleButton.h"

#include "../../Gameplay/Stage.h"
#include "../../Character/Player.h"
#include "../../Character/CharStats.h"
#include "../../Constants.h"
#include "../../Data/EquipData.h"
#include "../../Graphics/DrawArgument.h"
#include "../../Util/UiAssetResolve.h"

#include "../../Net/Packets/GameplayPackets.h"
#include "../../Net/Packets/LoginPackets.h"
#include "../../Net/Packets/CashShopPackets.h"
#include "../../Net/Packets/MiscOutPackets.h"
#include "../../Configuration.h"
#include "../../Util/Misc.h"
#include "../../Util/Locale.h"

#include "../UIStateCashShop.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UICashShop::UICashShop() : preview_index(0), menu_index(0), category_sub(0), promotion_index(0), mvp_grade(1), mvp_exp(0.07f), list_offset(0)
	{
		nl::node CashShop = nl::nx::UI["CashShop.img"];
		nl::node Base = CashShop["Base"];
		nl::node backgrnd = Base["backgrnd"];
		nl::node BestNew = Base["BestNew"];
		nl::node Preview = Base["Preview"];
		nl::node CSTab = CashShop["CSTab"];
		// 本包无 CSGLChargeNX；充值/查询走 CSStatus（Angel CCSWnd_Status）。
		nl::node CSGLChargeNX = CashShop["CSGLChargeNX"];
		if (!CSGLChargeNX)
			CSGLChargeNX = CSTab["CSGLChargeNX"];
		nl::node CSStatus = CashShop["CSStatus"];
		nl::node CSPromotionBanner = CashShop["CSPromotionBanner"];
		nl::node CSMVPBanner = CashShop["CSMVPBanner"];
		nl::node CSItemSearch = CashShop["CSItemSearch"];
		nl::node CSChar = CashShop["CSChar"];
		nl::node CSList = CashShop["CSList"];
		nl::node CSEffect = CashShop["CSEffect"];

		sprites.emplace_back(backgrnd);
		// [R6] BestNew 可选；热销榜槽位由 backgrnd 烘焙 + GetBestRect 命中，不依赖 BestNew 贴图。
		if (BestNew)
			BestNew_dim = Texture(BestNew).get_dimensions();

		for (size_t i = 0; i < 3; i++)
			preview_sprites[i] = Preview[i];

		// CCSWnd_Char：PreviewOnOff Tab @ (141,11) 起；预览层 CreateLayer(24,40,212,165)。
		for (size_t i = 0; i < 3; i++)
		{
			nl::node dis = Base["Tab"]["Disable"][i];
			nl::node en = Base["Tab"]["Enable"][i];
			if (dis && en)
				buttons[Buttons::BtPreview1 + i] = std::make_unique<TwoSpriteButton>(
					dis, en, Point<int16_t>(
						static_cast<int16_t>(WND_CHAR_X + 141 + static_cast<int16_t>(i * 17)),
						static_cast<int16_t>(WND_CHAR_Y + 11)));
		}
		if (auto it = buttons.find(Buttons::BtPreview1); it != buttons.end() && it->second)
			it->second->set_state(Button::State::PRESSED);

		// CCSWnd_Status @ (254,530)：BtCharge/Check/Coupon/Exit 相对 Status 的 Angel 立即数。
		try_maple_btn(buttons, Buttons::BtChargeNX, CSStatus["BtCharge"],
			Point<int16_t>(WND_STATUS_X + 248, WND_STATUS_Y + 13), "UICashShop");
		try_maple_btn(buttons, Buttons::BtChargeRefresh, CSStatus["BtCheck"],
			Point<int16_t>(WND_STATUS_X + 289, WND_STATUS_Y + 13), "UICashShop");
		try_maple_btn(buttons, Buttons::BtCoupon, CSStatus["BtCoupon"],
			Point<int16_t>(WND_STATUS_X + 330, WND_STATUS_Y + 13), "UICashShop");
		try_maple_btn(buttons, Buttons::BtExit, CSStatus["BtExit"],
			Point<int16_t>(WND_STATUS_X + 378, WND_STATUS_Y + 15), "UICashShop");
		// 可选扩展钮（本包常缺）：缺失则 SoftKey 兜底。
		try_maple_btn(buttons, Buttons::BtWish, CSStatus["BtWish"],
			Point<int16_t>(WND_STATUS_X + 10, WND_STATUS_Y + 6), "UICashShop");
		try_maple_btn(buttons, Buttons::BtMileage, CSStatus["BtMileage"],
			Point<int16_t>(WND_STATUS_X + 60, WND_STATUS_Y + 6), "UICashShop");
		try_maple_btn(buttons, Buttons::BtHelp, CSStatus["BtHelp"],
			Point<int16_t>(WND_STATUS_X + 110, WND_STATUS_Y + 6), "UICashShop");

		// CSTab/Tab 子键为 "1".."9"；Tab 子窗 @ (272,17)。
		for (size_t i = 0; i < 9; i++)
			menu_tabs[i] = CSTab["Tab"][i + 1];

		// 背包类型切换页签：Base/Tab2/Enable|Disable/0..4（2026-11-18 IMG 实测存在）。
		// 旧实现完全没读取这组节点 → 左侧背包只有烘焙格子、没有可切换的类型 tab。
		for (size_t i = 0; i < 5; i++)
		{
			bag_tab_enable[i] = Base["Tab2"]["Enable"][i];
			bag_tab_disable[i] = Base["Tab2"]["Disable"][i];
		}

		// 右侧热销榜图标初值；榜单数据异步到达后由 refresh_best_icons 重建。
		refresh_best_icons();

		job_label = Text(Text::Font::A11B, Text::Alignment::LEFT, Color::Name::SUPERNOVA, "");
		name_label = Text(Text::Font::A11B, Text::Alignment::LEFT, Color::Name::WHITE, "");
		cs_meta_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW, "");
		item_empty_notice = Text(Text::Font::A11B, Text::Alignment::CENTER, Color::Name::MINESHAFT, Locale::text("cashshop.empty_notice"));
		refresh_identity();
		refresh_cs_meta();

		// [R6] 推广/MVP 条本包常缺；有则贴在 List 上方空白，不越 800 高。
		if (CSPromotionBanner)
		{
			has_promotion = true;
			promotion_pos = Point<int16_t>(WND_LIST_X, 40);
			if (CSPromotionBanner["shadow"])
				sprites.emplace_back(CSPromotionBanner["shadow"], promotion_pos);
			if (CSPromotionBanner["basic"])
				promotion_sprites.emplace_back(CSPromotionBanner["basic"]);
			if (CSPromotionBanner["BtNext"])
				buttons[Buttons::BtNext] = std::make_unique<MapleButton>(CSPromotionBanner["BtNext"], promotion_pos);
			if (CSPromotionBanner["BtPrev"])
				buttons[Buttons::BtPrev] = std::make_unique<MapleButton>(CSPromotionBanner["BtPrev"], promotion_pos);
		}

		if (CSMVPBanner)
		{
			has_mvp = true;
			for (size_t i = 0; i < 7; i++)
				mvp_sprites[i] = CSMVPBanner["grade"][i];
			mvp_pos = Point<int16_t>(10, 560);
			if (CSMVPBanner["BtDetailPackage"])
				buttons[Buttons::BtDetailPackage] = std::make_unique<MapleButton>(CSMVPBanner["BtDetailPackage"], mvp_pos);
			if (CSMVPBanner["BtNonGrade"])
				buttons[Buttons::BtNonGrade] = std::make_unique<MapleButton>(CSMVPBanner["BtNonGrade"], mvp_pos);
			if (buttons.count(Buttons::BtDetailPackage) && buttons[Buttons::BtDetailPackage])
				buttons[Buttons::BtDetailPackage]->set_active(mvp_grade);
			if (buttons.count(Buttons::BtNonGrade) && buttons[Buttons::BtNonGrade])
				buttons[Buttons::BtNonGrade]->set_active(!mvp_grade);
			if (CSMVPBanner["gage"][0] && CSMVPBanner["gage"][1] && CSMVPBanner["gage"][2])
				mvp_gauge = Gauge(
					Gauge::Type::CASHSHOP,
					CSMVPBanner["gage"][0],
					CSMVPBanner["gage"][2],
					CSMVPBanner["gage"][1],
					84,
					0.0f
				);
		}

		Point<int16_t> search_pos = Point<int16_t>(WND_SEARCH_X, WND_SEARCH_Y);
		if (CSItemSearch["backgrnd"])
			sprites.emplace_back(CSItemSearch["backgrnd"], search_pos);
		if (CSItemSearch["search"])
			sprites.emplace_back(CSItemSearch["search"], search_pos + Point<int16_t>(0, 0));
		// F 搜索钮：CSItemSearch/BtSearch（live WZ 实测存在），点击弹 UISoftKey 本地输入。
		try_maple_btn(buttons, Buttons::BtSearch, CSItemSearch["BtSearch"],
			search_pos + Point<int16_t>(0, 0), "UICashShop");

		// CCSWnd_Char OnCreate：BtBuyAvatar(16,237) / Default(101,237) / Takeoff(187,237)。
		try_maple_btn(buttons, Buttons::BtBuyAvatar, CSChar["BtBuyAvatar"],
			Point<int16_t>(WND_CHAR_X + 16, WND_CHAR_Y + 237), "UICashShop");
		try_maple_btn(buttons, Buttons::BtDefaultAvatar, CSChar["BtDefaultAvatar"],
			Point<int16_t>(WND_CHAR_X + 101, WND_CHAR_Y + 237), "UICashShop");
		try_maple_btn(buttons, Buttons::BtTakeoffAvatar, CSChar["BtTakeoffAvatar"],
			Point<int16_t>(WND_CHAR_X + 187, WND_CHAR_Y + 237), "UICashShop");
		try_maple_btn(buttons, Buttons::BtSaveAvatar, CSChar["BtSaveAvatar"],
			Point<int16_t>(WND_CHAR_X + 16, WND_CHAR_Y + 260), "UICashShop");
		try_maple_btn(buttons, Buttons::BtInventory, CSChar["BtInventory"],
			Point<int16_t>(WND_CHAR_X + 101, WND_CHAR_Y + 260), "UICashShop");

		// E 扩栏五钮：CSInventory/BtEx*（live WZ 实测存在），竖排在背包格右侧。
		// 无服务端 opcode，点击只给 locale 提示 cashshop.ext_hint，不发明包。
		{
			nl::node CSInventory = CashShop["CSInventory"];
			const nl::node ext_nodes[5] = {
				CSInventory["BtExEquip"], CSInventory["BtExConsume"], CSInventory["BtExInstall"],
				CSInventory["BtExEtc"], CSInventory["BtExTrunk"]
			};
			for (uint8_t i = 0; i < 5; i++)
			{
				try_maple_btn(buttons, Buttons::BtExtEquip + i, ext_nodes[i],
					Point<int16_t>(EXT_BTN_X, static_cast<int16_t>(EXT_BTN_Y0 + EXT_BTN_DY * i)),
					"UICashShop");
			}
		}

		if (CSStatus["Number"])
			charge_charset = Charset(CSStatus["Number"], Charset::Alignment::RIGHT);
		else if (CSGLChargeNX && CSGLChargeNX["Number"])
			charge_charset = Charset(CSGLChargeNX["Number"], Charset::Alignment::RIGHT);

		item_base = CSList["Base"];
		item_line = Base["line"];
		// PicturePlate/NoItem =「正在准备商品」大弹层；仅商品目录为空时居中画在 List 区。
		if (CashShop["PicturePlate"]["NoItem"])
			item_none = CashShop["PicturePlate"]["NoItem"];
		else if (Base["noItem"])
			item_none = Base["noItem"];

		for (nl::node item_label : CSEffect)
			item_labels.emplace_back(item_label);

		rebuild_items();

		// GetPlateRect + SetPlateNo：Buy @ plate+(80,53)，Gift @ +(119,53)。
		for (size_t i = 0; i < MAX_ITEMS; i++)
		{
			const int col = static_cast<int>(i % 2);
			const int row = static_cast<int>(i / 2);
			const int16_t plate_x = static_cast<int16_t>(WND_LIST_X + PLATE_DX * col);
			const int16_t plate_y = static_cast<int16_t>(WND_LIST_Y + 2 + PLATE_DY * row);
			try_maple_btn(buttons, Buttons::BtBuy + static_cast<uint16_t>(i), CSList["BtBuy"],
				Point<int16_t>(static_cast<int16_t>(plate_x + 80), static_cast<int16_t>(plate_y + 53)), "UICashShop");
			try_maple_btn(buttons, Buttons::BtGift + static_cast<uint16_t>(i), CSList["BtGift"],
				Point<int16_t>(static_cast<int16_t>(plate_x + 119), static_cast<int16_t>(plate_y + 53)), "UICashShop");
			// B 购物车钮：CSList/BtReserve（live WZ 实测存在），放在 BtGift 右侧 +148。
			// 无服务端 opcode，点击只写 UIStateCashShop::local_cart_sns + locale 提示。
			try_maple_btn(buttons, Buttons::BtCart + static_cast<uint16_t>(i), CSList["BtReserve"],
				Point<int16_t>(static_cast<int16_t>(plate_x + 148), static_cast<int16_t>(plate_y + 53)), "UICashShop");

			// Align with MinMS: A13B/BLACK, LEFT-aligned = dark ink on the light card bg.
			// 2026-11-18：与 MinMS 对齐，商品名/价/折扣统一 A13B（旧折扣行降级 A11M
			// 造成"商品信息字体不对"）。四行都是黑字落在 CSList/Base 浅色文字面板。
			item_name[i] = Text(Text::Font::A13B, Text::Alignment::LEFT, Color::Name::BLACK);
			item_price[i] = Text(Text::Font::A13B, Text::Alignment::LEFT, Color::Name::BLACK);
			item_discount[i] = Text(Text::Font::A13B, Text::Alignment::LEFT, Color::Name::BLACK);
			item_percent[i] = Text(Text::Font::A13B, Text::Alignment::LEFT, Color::Name::TORCHRED);
		}

		// 2026-11-18 分页重做：统一走箭头 + 页码（移除与之并存的 THIN_MINESHAFT 滑条）。
		// 箭头复用 MonsterBook/arrowLeft/arrowRight（UIMonsterBook.cpp:170-173 同款）。
		{
			nl::node arrows = UiAssetResolve::window("MonsterBook");
			try_maple_btn(buttons, Buttons::BtPagePrev, arrows["arrowLeft"],
				Point<int16_t>(WND_LIST_X + 356, WND_LIST_Y + 408), "mb arrowL page");
			try_maple_btn(buttons, Buttons::BtPageNext, arrows["arrowRight"],
				Point<int16_t>(WND_LIST_X + 386, WND_LIST_Y + 408), "mb arrowR page");
		}
		// 页码 A13B/BLACK 落在列表底边浅色条（旧 A11B/WHITE 看不清）。
		page_label = Text(Text::Font::A13B, Text::Alignment::CENTER, Color::Name::BLACK, "");
		sync_page_chrome();

		// 4 模式 tab 文字（locale 新键，zh/en 双语）；选中态用 ColorBox 底层着色。
		for (uint8_t i = 0; i < 4; i++)
			mode_labels[i] = Text(Text::Font::A11B, Text::Alignment::CENTER, Color::Name::YELLOW,
			                      Locale::text("cashshop.mode" + std::to_string(i)));

		update_items();
		dimension = Texture(backgrnd).get_dimensions();

		// CUIAvatar：拷贝进店角色外观；update 跟帧 + 货板点击试穿。
		try
		{
			avatar_look = Stage::get().get_player().get_look();
			avatar_look.set_direction(false);
			avatar_look.set_stance(Stance::Id::STAND1);
			avatar_baseline = avatar_look;
		}
		catch (...)
		{
			// 商城可在无角色上下文打开（调试预览）；留默认 look 不绘制异常体。
		}

		init_preview_sandbox();
		CheckCashPacket().dispatch();
	}

	void UICashShop::draw_icon_in_slot(const Texture& icon, Point<int16_t> slot_pos,
	                                  int16_t slot_size)
	{
		if (!icon.is_valid())
			return;
		const Point<int16_t> dim = icon.get_dimensions();
		const int16_t maxdim = std::max(dim.x(), dim.y()) > 0 ? std::max(dim.x(), dim.y()) : 1;
		const float fit_scale = std::min(2.0f, static_cast<float>(slot_size) / maxdim);
		const int16_t scaled_w = static_cast<int16_t>(dim.x() * fit_scale + 0.5f);
		const int16_t scaled_h = static_cast<int16_t>(dim.y() * fit_scale + 0.5f);
		const Point<int16_t> inset(
			static_cast<int16_t>((slot_size - scaled_w) / 2),
			static_cast<int16_t>((slot_size - scaled_h) / 2));
		const Point<int16_t> origin = icon.get_origin();
		const Point<int16_t> anchor = slot_pos + inset + Point<int16_t>(
			static_cast<int16_t>(origin.x() * fit_scale),
			static_cast<int16_t>(origin.y() * fit_scale));
		icon.draw(DrawArgument(anchor, fit_scale, fit_scale));
	}

	void UICashShop::draw(float inter) const
	{
		UIElement::draw_sprites(inter);

		// CCSWnd_Char CreateLayer 预览区 (24,40) 212×165。
		if (preview_index < 3)
			preview_sprites[preview_index].draw(
				position + Point<int16_t>(WND_CHAR_X + PREVIEW_WELL_X, WND_CHAR_Y + PREVIEW_WELL_Y), inter);

		// 预览沙盒：CharLook 画在井内脚底（跟 preview_actor）。
		// Cash effect behind the character (z < 0).
		if (preview_effect_item_id > 0 && preview_effect_below)
			preview_effect.draw(DrawArgument(position + Point<int16_t>(WND_CHAR_X, WND_CHAR_Y) + preview_feet_art()), 0.0f);

		avatar_look.draw(
			DrawArgument(position + Point<int16_t>(WND_CHAR_X, WND_CHAR_Y) + preview_feet_art()),
			inter);

		// Preview pet lives in the same well-local coordinate space as the actor.
		if (preview_pet.get_itemid() > 0)
			preview_pet.draw(
				position.x() + WND_CHAR_X + PREVIEW_WELL_X,
				position.y() + WND_CHAR_Y + PREVIEW_WELL_Y,
				inter);

		// Cash effect in front of the character (z >= 0).
		if (preview_effect_item_id > 0 && !preview_effect_below)
			preview_effect.draw(DrawArgument(position + Point<int16_t>(WND_CHAR_X, WND_CHAR_Y) + preview_feet_art()), 0.0f);

		// D 4 模式 tab：C++ 运行时绘制于预览井底 y≈214，ColorBox 选中底色 + A11B 文字。
		draw_mode_tabs();

		// CSTab 子窗 @ (272,17)；Tab 贴图 508×78。
		if (menu_index < 9)
			menu_tabs[menu_index].draw(position + Point<int16_t>(WND_TAB_X, WND_TAB_Y), inter);

		// 二级分类栏（MinMS draw_category_sub_tabs）
		draw_category_sub_tabs();

		Point<int16_t> label_pos = position + Point<int16_t>(WND_CHAR_X + 8, WND_CHAR_Y + 4);
		job_label.draw(label_pos);
		name_label.draw(label_pos + Point<int16_t>(static_cast<int16_t>(job_label.width() + 8), 0));
		if (!cs_meta_label.empty())
			cs_meta_label.draw(label_pos + Point<int16_t>(0, 16));

		if (has_promotion && !promotion_sprites.empty())
			promotion_sprites[promotion_index].draw(position + promotion_pos, inter);

		if (has_mvp)
		{
			mvp_sprites[mvp_grade].draw(position + mvp_pos, inter);
			mvp_gauge.draw(position + mvp_pos);
		}

		// 余额数字右对齐到 Status 输入框右段；蓝字标签(点券余额/信用点数/抵用券)
		// 是 CSStatus 贴图自绘在框左侧(x~280-320)，数字必须靠右(x~470)才不重叠。
		// MinMS BALANCE_VALUE_X=462 同思路：标签在左、数值在右。
		int32_t nx = 0;
		int32_t maple = 0;
		int32_t prepaid = 0;
		if (UIStateCashShop::cash_balances_ready)
		{
			nx = UIStateCashShop::cash_nx_credit;
			maple = UIStateCashShop::cash_maple_point;
			prepaid = UIStateCashShop::cash_nx_prepaid;
		}
		else
		{
			maple = static_cast<int32_t>(Setting<MaplePointsLocal>::get().load());
		}
		// CSStatus["Number"] 在本包常为空节点（Charset 空则 draw 无输出），
		// 改用 Text 右对齐自绘三币种。2026-11-18 按 backgrnd.png 实测校正：
		// 白色输入框 x≈280..485、三行虚线 y≈548/564/580；旧值 charge_pos=(470,538)
		// 偏右且整体偏上，数字压在蓝字标签行。现对齐到框右侧 x=485、y=546 起。
		auto draw_balance = [&](int32_t value, int16_t y)
		{
			// I 余额千分位：与商品价同口径套 string_format::split_number。
			std::string balance_text = std::to_string(value);
			string_format::split_number(balance_text);
			Text label(Text::Font::A13B, Text::Alignment::RIGHT, Color::Name::BLACK,
			           balance_text);
			label.draw(position + Point<int16_t>(484, y));
		};
		draw_balance(nx, static_cast<int16_t>(WND_STATUS_Y + 16));   // 546
		draw_balance(maple, static_cast<int16_t>(WND_STATUS_Y + 32)); // 562
		draw_balance(prepaid, static_cast<int16_t>(WND_STATUS_Y + 48)); // 578

		if (items.empty())
		{
			// 「正在准备商品」居中于 List 子窗，避免漂到左下角盖住背包区。
			Point<int16_t> list_center = position + Point<int16_t>(
				static_cast<int16_t>(WND_LIST_X + 206),
				static_cast<int16_t>(WND_LIST_Y + 200));
			item_empty_notice.draw(list_center - item_empty_notice.dimensions() / 2);
		}
		else
		{
			item_line.draw(position + Point<int16_t>(WND_LIST_X, static_cast<int16_t>(WND_LIST_Y + 410)), inter);
		}

		for (size_t i = 0; i < MAX_ITEMS; i++)
		{
			int16_t index = static_cast<int16_t>(i + list_offset);
			if (index < 0 || index >= static_cast<int16_t>(items.size()))
				continue;

			const int col = static_cast<int>(i % 2);
			const int row = static_cast<int>(i / 2);
			const Point<int16_t> plate = position + Point<int16_t>(
				static_cast<int16_t>(WND_LIST_X + PLATE_DX * col),
				static_cast<int16_t>(WND_LIST_Y + 2 + PLATE_DY * row));
			Item item = items[static_cast<size_t>(index)];

			item_base.draw(plate, inter);
			// CSList/Base 白色 icon well (5,5)..(68,68)，图标居中填入 64x64 槽。
			draw_icon_in_slot(item.get_icon(), plate + Point<int16_t>(5, 5), 64);

			if (item.label != Item::Label::NONE)
			{
				size_t li = static_cast<size_t>(item.label) + 1;
				if (li < item_labels.size())
					item_labels[li].draw(plate + Point<int16_t>(4, 2), inter);
			}

			// 文字落在 CSList/Base 右侧浅色面板：图标井 x=5..68、分隔线 y≈24。
			// 2026-11-18 按贴图像素实测：名称行 y1..19、价格行 y25..71；
			// Text 位置是基线（GraphicsGL::drawtext line.position.y()），
			// A13B 13px 字高，基线放 y≈17 / y≈38 才能使字墨居中于两条浅色带。
			// 旧 y=12/32 使名称贴顶、售价偏上；这里下移到带中心。
			constexpr int16_t NAME_X = 74;
			item_name[i].draw(plate + Point<int16_t>(NAME_X, 17));
			if (item_discount[i].get_text().empty())
				item_price[i].draw(plate + Point<int16_t>(NAME_X, 38));
			else
			{
				item_discount[i].draw(plate + Point<int16_t>(NAME_X, 34));
				item_percent[i].draw(plate + Point<int16_t>(static_cast<int16_t>(NAME_X + 58), 34));
				item_price[i].draw(plate + Point<int16_t>(NAME_X, 48));
			}
		}

		// 2026-11-18：移除 THIN_MINESHAFT 滑条——它和分页箭头/页码是两套并存的分页
		// 控件，观感就是"分页和分页切换 UI 错误"。本端统一走箭头 + 页码分页。
		// page_label 落在列表底边下方空白条 (x≈603, y≈503)。
		page_label.draw(position + Point<int16_t>(WND_LIST_X + 328, WND_LIST_Y + 408));

		// ---- 右侧热销榜（2026-11-18 补齐）：数字/底纹烘焙在 backgrnd，
		// 这里只把服务端榜单道具图标画进每行右侧的浅色槽 (x=742..773)。 ----
		for (uint8_t i = 0; i < BEST_ROWS; i++)
		{
			if (!best_icons[i].is_valid())
				continue;
			draw_icon_in_slot(best_icons[i],
				position + Point<int16_t>(BEST_ITEM_X,
					static_cast<int16_t>(BEST_ITEM_Y0 + BEST_ITEM_DY * i)),
				BEST_ITEM_SLOT);
		}

		// ---- 左侧「已购道具」2x6 格（2026-11-18 补齐）：数据取自服务端权威
		// cash_inventory（0x4B/0x57/0x89），格子是 backgrnd 烘焙的白槽。 ----
		{
			// E 「商品选购」Text 按钮：标题条右侧，点击切回热卖主页签（menu_index=1）。
			Text buy_select(Text::Font::A11B, Text::Alignment::LEFT, Color::Name::YELLOW,
			                Locale::text("cashshop.buy_select"));
			buy_select.draw(position + Point<int16_t>(BUY_SELECT_X, BUY_SELECT_Y));

			const auto& inv = UIStateCashShop::cash_inventory;
			uint8_t drawn = 0;
			for (uint8_t i = 0; i < PURCH_ROWS * PURCH_COLS && drawn < inv.size(); i++)
			{
				const CashInvEntry& e = inv[drawn];
				++drawn;
				if (e.itemid <= 0)
					continue;
				const int16_t px = static_cast<int16_t>(PURCH_X0 + (i % PURCH_COLS) * PURCH_DX);
				const int16_t py = static_cast<int16_t>(PURCH_Y0 + (i / PURCH_COLS) * PURCH_DY);
				draw_icon_in_slot(ItemData::get(e.itemid).get_icon(false),
					position + Point<int16_t>(px, py), 31);
			}
		}

		// ---- 左侧「背包」4x3 格 + 类型切换页签（2026-11-18 补齐）：
		// 背包当前装的是 cash_inventory 的余项（与已购同源于服务端权威数据），
		// 类型 tab 用 IMG 实测存在的 Base/Tab2/Enable|Disable/0..4。 ----
		{
			for (size_t t = 0; t < 5; t++)
			{
				const Sprite& tab = (backpack_tab == t) ? bag_tab_enable[t] : bag_tab_disable[t];
				if (tab.width() > 0)
					tab.draw(position + Point<int16_t>(BAG_TAB_X,
						static_cast<int16_t>(BAG_TAB_Y0 + BAG_TAB_DY * t)), inter);
			}
			// E 扩栏五钮文字：装备窗/消耗窗/设置窗/其他窗/仓库（locale）。
			{
				static const char* const EXT_KEYS[5] = {
					"cashshop.ext_equip", "cashshop.ext_consume", "cashshop.ext_setup",
					"cashshop.ext_etc", "cashshop.ext_trunk"
				};
				for (uint8_t i = 0; i < 5; i++)
				{
					Text label(Text::Font::A11B, Text::Alignment::LEFT, Color::Name::YELLOW,
					           Locale::text(EXT_KEYS[i]));
					label.draw(position + Point<int16_t>(
						static_cast<int16_t>(EXT_BTN_X - 2),
						static_cast<int16_t>(EXT_BTN_Y0 + EXT_BTN_DY * i + 3)));
				}
			}
			const auto& inv = UIStateCashShop::cash_inventory;
			const size_t skip = PURCH_ROWS * PURCH_COLS; // 前 12 件画在已购区
			uint8_t shown = 0;
			for (size_t i = skip; i < inv.size() && shown < BACKPACK_ROWS * BACKPACK_COLS; i++, shown++)
			{
				const CashInvEntry& e = inv[i];
				if (e.itemid <= 0)
					continue;
				const int16_t px = static_cast<int16_t>(BACKPACK_X0 + (shown % BACKPACK_COLS) * 35);
				const int16_t py = static_cast<int16_t>(BACKPACK_Y0 + (shown / BACKPACK_COLS) * 35);
				draw_icon_in_slot(ItemData::get(e.itemid).get_icon(false),
					position + Point<int16_t>(px, py), 31);
			}
		}

		UIElement::draw_buttons(inter);
	}

	void UICashShop::update()
	{
		UIElement::update();
		sync_page_chrome();

		mvp_gauge.update(mvp_exp);
		apply_preview_controls();
		avatar_look.set_expression(Expression::Id::DEFAULT);
		avatar_look.update(Constants::TIMESTEP);

		if (preview_effect_item_id > 0)
			preview_effect.update(Constants::TIMESTEP);
		if (preview_pet.get_itemid() > 0)
		{
			if (preview_actor.is_climbing())
				preview_pet.set_stance(PetLook::HANG);
			else if (preview_pet.get_stance() == PetLook::HANG)
				preview_pet.set_stance(PetLook::STAND);
			preview_pet.update(preview_physics, preview_actor.get_position());
		}
	}

	Button::State UICashShop::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
		case Buttons::BtPreview1:
		case Buttons::BtPreview2:
		case Buttons::BtPreview3:
			buttons[preview_index]->set_state(Button::State::NORMAL);

			preview_index = buttonid;
			return Button::State::PRESSED;
		case Buttons::BtExit:
		{
			uint16_t width = Setting<Width>::get().load();
			uint16_t height = Setting<Height>::get().load();

			Constants::Constants::get().set_viewwidth(width);
			Constants::Constants::get().set_viewheight(height);

			// 与 SetField/CashShopHandlers 同批提速：0.025→0.04（200ms/侧）。
			float fadestep = 0.04f;

			Window::get().fadeout(
				fadestep,
				[]()
				{
					GraphicsGL::get().clear();
					ChangeMapPacket().dispatch();

					// [商城黑屏根治] 旧实现只切 GAME state + unlock，画面恢复全押在
					// 服务端 SET_FIELD 回包上：回包迟到/丢失时 Stage 仍是商城预览
					// (Stage::clear 后 INACTIVE)，draw 全跳过 => 永久黑屏但进程还活着。
					// 对齐 ESC 路径 exit_cashshop()：本地即时 load 真实地图 + transfer，
					// 服务端回包随后权威刷新一次（同 mapid 幂等），两路状态一致。
					// load 抛错也被捕获，不阻断后面的 enable/unlock（见下）。
					UI::get().change_state(UI::State::GAME);

					try
					{
						Stage& stage = Stage::get();
						Player& player = stage.get_player();
						int32_t mapid = player.get_stats().get_mapid();
						uint8_t portalid = player.get_stats().get_portal();
						stage.load(mapid, portalid);
						stage.transfer_player();
					}
					catch (const std::exception& ex)
					{
						LOG(LOG_ERROR, "UICashShop BtExit local restore failed: " << ex.what());
					}
					catch (...)
					{
						LOG(LOG_ERROR, "UICashShop BtExit local restore failed: unknown");
					}

					UI::get().enable();
					Timer::get().start();
					GraphicsGL::get().unlock();
				}
			);

			GraphicsGL::get().lock();
			// 退出商城回游戏：保留天气，避免闪白天/路灯重暖（服务端会补 0x373D）。
			Stage::get().clear(true);
			Timer::get().start();

			return Button::State::NORMAL;
		}
		case Buttons::BtPagePrev:
			change_list_page(-1);
			return Button::State::NORMAL;
		case Buttons::BtPageNext:
			change_list_page(1);
			return Button::State::NORMAL;
		case Buttons::BtNext:
		{
			if (!has_promotion || promotion_sprites.empty())
				return Button::State::NORMAL;

			size_t size = promotion_sprites.size() - 1;

			promotion_index++;

			if (promotion_index > size)
				promotion_index = 0;

			return Button::State::NORMAL;
		}
		case Buttons::BtPrev:
		{
			if (!has_promotion || promotion_sprites.empty())
				return Button::State::NORMAL;

			size_t size = promotion_sprites.size() - 1;

			promotion_index--;

			if (promotion_index < 0)
				promotion_index = size;

			return Button::State::NORMAL;
		}
		case Buttons::BtChargeNX:
		{
			std::string url = Configuration::get().get_chargenx();

			SDL_OpenURL(url.c_str());

			return Button::State::NORMAL;
		}
		case Buttons::BtCoupon:
		{
			// 2026-11-23 全量补齐：空 → UICoupon；bday,sn[,msg] 或 bday,sn,name,msg → GiftPacket(0x04)。
			// 包体 CashShopPackets.h 现成、服务端 CashOperationHandler.java:195 已支持，无新 opcode。
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			UI::get().emplace<UISoftKey>([pre](const std::string& tip) {
				if (tip.empty()) {
					auto coupon = UI::get().get_element<UICoupon>();
					if (!coupon) coupon = UI::get().emplace<UICoupon>();
					else if (!coupon->is_active()) coupon->makeactive();
					return;
				}
				size_t c1 = tip.find(',');
				if (c1 == std::string::npos) return;
				size_t c2 = tip.find(',', c1 + 1);
				size_t c3 = (c2 == std::string::npos) ? std::string::npos : tip.find(',', c2 + 1);
				try {
					int32_t birthday = std::stoi(tip.substr(0, c1));
					int32_t sn = 0; std::string name = pre, msg;
					if (c2 == std::string::npos) {
						sn = std::stoi(tip.substr(c1 + 1));
					} else if (c3 == std::string::npos) {
						sn = std::stoi(tip.substr(c1 + 1, c2 - c1 - 1));
						msg = tip.substr(c2 + 1);
					} else {
						sn = std::stoi(tip.substr(c1 + 1, c2 - c1 - 1));
						name = tip.substr(c2 + 1, c3 - c2 - 1);
						msg = tip.substr(c3 + 1);
					}
					if (sn > 0 && !name.empty())
						CashShopGiftPacket(birthday, sn, name, msg).dispatch();
				} catch (...) {}
			}, []() {}, pre.empty() ? "bday,sn,name,msg|empty=Coupon" : "bday,sn[,msg]");
			return Button::State::NORMAL;
		}
		case Buttons::BtHelp:
		{
			// 原生 BtHelp 是「帮助/说明」，此前误接 ENTER_MTS（把帮助钮跳到拍卖），
			// 与 Angel CCashShop 语义不符。本包无独立帮助 UI，按「不发明能力」
			// 改为在底部信息条显示本地帮助提示，不发任何包。
			cs_meta_label.change_text(Locale::text("cashshop.help_hint"));
			LOG(LOG_INFO, "UICashShop: BtHelp -> local help hint");
			return Button::State::NORMAL;
		}
		case Buttons::BtInventory:
		{
			// 2026-11-23 全量补齐：空 → 背包；t,cashId → Take(0x0D)；p,cashId[,inv] → Put(0x0E)。
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				if (tip.empty()) {
					auto inv = UI::get().get_element<UIItemInventory>();
					if (!inv)
						inv = UI::get().emplace<UIItemInventory>(Stage::get().get_player().get_inventory());
					else if (!inv->is_active()) inv->makeactive();
					return;
				}
				std::string t = tip;
				for (auto& ch : t) if (ch >= 'A' && ch <= 'Z') ch = static_cast<char>(ch - 'A' + 'a');
				try {
					if (t.size() >= 2 && t[0] == 't' && (t[1] == ',' || t[1] == ':')) {
						int32_t cashId = std::stoi(tip.substr(2));
						if (cashId > 0) CashShopTakePacket(cashId).dispatch();
						return;
					}
					if (t.size() >= 2 && t[0] == 'p' && (t[1] == ',' || t[1] == ':')) {
						std::string rest = tip.substr(2);
						size_t c = rest.find(',');
						int32_t cashId = 0; int8_t invt = 1;
						if (c == std::string::npos) cashId = std::stoi(rest);
						else { cashId = std::stoi(rest.substr(0, c)); invt = static_cast<int8_t>(std::stoi(rest.substr(c + 1))); }
						if (cashId > 0) CashShopPutPacket(cashId, invt).dispatch();
					}
				} catch (...) {}
			}, []() {}, "t,cashId|p,cashId[,inv]");
			return Button::State::NORMAL;
		}
		case Buttons::BtChargeRefresh:
		{
			list_offset = 0;
			update_items();
			// s9 TouchingCashShopHandler：CHECK_CASH → showCash(QUERY_CASH_RESULT)
			CheckCashPacket().dispatch();
			LOG(LOG_DEBUG, "UICashShop: BtChargeRefresh CHECK_CASH items=" << items.size());
			return Button::State::NORMAL;
		}
		case Buttons::BtSearch:
		{
			// 2026-11-23 全量补齐：弹 UISoftKey 输入 → 本地商品名子串过滤（不发明服务端搜索包）。
			// 输入为空清过滤；提交后 rebuild_items 用 search_query 过滤并回列表第一页。
			UI::get().emplace<UISoftKey>(
				[this](const std::string& entered) {
					search_query = entered;
					list_offset = 0;
					rebuild_items();
					LOG(LOG_INFO, "UICashShop: local search filter len=" << entered.size());
				},
				[]() {},
				Locale::text("cashshop.search_hint"),
				UISoftKey::Mode::Text);
			return Button::State::NORMAL;
		}
		case Buttons::BtWish:
		{
			// 2026-11-23 全量补齐：逗号分隔 SN（最多10个）→ WishPacket(0x05)。
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				std::array<int32_t, 10> sns{}; size_t n = 0; size_t start = 0;
				while (n < sns.size()) {
					size_t c = tip.find(',', start);
					std::string part = (c == std::string::npos) ? tip.substr(start) : tip.substr(start, c - start);
					try { int32_t sn = std::stoi(part); if (sn > 0) sns[n++] = sn; } catch (...) {}
					if (c == std::string::npos) break;
					start = c + 1;
				}
				if (n) CashShopWishPacket(sns).dispatch();
			}, []() {}, "SN[,SN...] (max 10)");
			return Button::State::NORMAL;
		}
		case Buttons::BtMileage:
		{
			// 2026-11-23 全量补齐：Mileage = useNX=2（MAPLE_POINT）买 SN。
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				try { int32_t sn = std::stoi(tip); if (sn > 0) CashShopBuyPacket(2, sn).dispatch(); } catch (...) {}
			}, []() {}, "Mileage SN");
			return Button::State::NORMAL;
		}
		case Buttons::BtDetailPackage:
		{
			// 2026-11-23 全量补齐：礼包购买 PackagePacket(0x1E)，输入 useNX,sn 或 sn。
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					size_t comma = s.find(','); int32_t useNX = 1, sn = 0;
					if (comma == std::string::npos) sn = std::stoi(s);
					else { useNX = std::stoi(s.substr(0, comma)); sn = std::stoi(s.substr(comma + 1)); }
					if (sn > 0) CashShopPackagePacket(useNX, sn).dispatch();
				} catch (...) {}
			}, []() {}, "useNX,sn");
			return Button::State::NORMAL;
		}
		case Buttons::BtNonGrade:
		{
			mvp_grade = !mvp_grade;
			if (auto it = buttons.find(Buttons::BtDetailPackage); it != buttons.end() && it->second)
				it->second->set_active(mvp_grade);
			if (auto it = buttons.find(Buttons::BtNonGrade); it != buttons.end() && it->second)
				it->second->set_active(!mvp_grade);
			return Button::State::NORMAL;
		}
		case Buttons::BtBuyAvatar:
		{
			// 2026-11-23 全量补齐：对齐 MinMS 同名键走购买（0x03），输入 SN。
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try { int32_t sn = std::stoi(s); if (sn > 0) CashShopBuyPacket(1, sn).dispatch(); } catch (...) {}
			}, []() {}, "Buy SN");
			return Button::State::NORMAL;
		}
		case Buttons::BtDefaultAvatar:
		{
			// 「我的现状」：还原进店快照（MinMS reset_preview_to_baseline）。
			if (auto it = buttons.find(preview_index); it != buttons.end() && it->second)
				it->second->set_state(Button::State::NORMAL);
			preview_index = Buttons::BtPreview1;
			if (auto it = buttons.find(Buttons::BtPreview1); it != buttons.end() && it->second)
				it->second->set_state(Button::State::PRESSED);
			reset_preview_to_baseline();
			LOG(LOG_DEBUG, "UICashShop: DefaultAvatar reset try-on");
			return Button::State::NORMAL;
		}
		case Buttons::BtTakeoffAvatar:
		{
			// 「脱下」：清试穿，保留当前预览页签。
			reset_preview_to_baseline();
			LOG(LOG_DEBUG, "UICashShop: TakeoffAvatar clear try-on");
			return Button::State::NORMAL;
		}
		case Buttons::BtSaveAvatar:
		{
			// Save 无 s9 CashOperation：切本地预览背景页 0..2（保留既有降级语义，给可见反馈）。
			preview_background = (preview_background + 1) % 3;
			LOG(LOG_INFO, "UICashShop: SaveAvatar local preview bg=" << preview_background);
			UI::get().emplace<UIOk>(Locale::text("cashshop.save_preview_hint"), [](bool) {});
			return Button::State::NORMAL;
		}
		default:
			break;
		}

		// 2026-11-23 全量补齐：扩栏五钮接真实发包（包体与服务端口令现成）。
		// 输入 cash 数量（可选 ,sn）：0=Equip/Consume/Install/Etc 用 0x06 invType，4=Trunk 用 0x07。
		if (buttonid >= Buttons::BtExtEquip && buttonid < Buttons::BtExtEquip + 5)
		{
			const uint16_t ext_idx = static_cast<uint16_t>(buttonid - Buttons::BtExtEquip);
			UI::get().emplace<UISoftKey>([ext_idx](const std::string& tip) {
				try {
					int32_t cash = std::stoi(tip); int32_t sn = 0;
					size_t c = tip.find(',');
					if (c != std::string::npos) sn = std::stoi(tip.substr(c + 1));
					if (ext_idx < 4) {
						if (sn > 0) CashShopInvSlotSnPacket(cash, sn).dispatch();
						else CashShopInvSlotPacket(cash, static_cast<int8_t>(ext_idx)).dispatch();
					} else {
						if (sn > 0) CashShopStorageSlotPacket(cash, sn).dispatch();
						else CashShopStorageSlotPacket(cash).dispatch();
					}
				} catch (...) {}
			}, []() {}, "cash[,sn]");
			return Button::State::NORMAL;
		}

		// 2026-11-23 全量补齐：购物车钮改为 toggle（再加=移除），给可见计数反馈。
		// 保持用户 11-21 拍板口径：9号 tab 仍以服务端权威 cash_inventory 为准，
		// local_cart_sns 只是"待购暂存"，不发明服务端 cart opcode。
		if (buttonid >= Buttons::BtCart && buttonid < Buttons::BtCart + MAX_ITEMS)
		{
			const int16_t index = static_cast<int16_t>(buttonid - Buttons::BtCart) + list_offset;
			if (index < 0 || index >= static_cast<int16_t>(items.size()))
				return Button::State::NORMAL;
			const int32_t sn = items[static_cast<size_t>(index)].get_sn();
			if (sn <= 0)
				return Button::State::NORMAL;
			auto& cart = UIStateCashShop::local_cart_sns;
			auto it = std::find(cart.begin(), cart.end(), sn);
			const bool added = (it == cart.end());
			if (added) cart.push_back(sn); else cart.erase(it);
			LOG(LOG_INFO, "UICashShop: local cart toggle sn=" << sn << (added ? " add" : " remove") << " size=" << cart.size());
			UI::get().emplace<UIOk>(Locale::format(added ? "cashshop.cart_added" : "cashshop.cart_removed",
			                                       std::to_string(cart.size())), [](bool) {});
			return Button::State::NORMAL;
		}

		if (buttonid >= Buttons::BtGift && buttonid < Buttons::BtGift + MAX_ITEMS)
		{
			int16_t index = static_cast<int16_t>(buttonid - Buttons::BtGift) + list_offset;
			if (index < 0 || index >= static_cast<int16_t>(items.size()))
				return Button::State::NORMAL;
			const Item& gift_it = items[index];
			int32_t sn = gift_it.get_sn();
			int32_t price = gift_it.get_price();
			std::string iname = gift_it.get_name();
			// MinMS purchase_blocked：无效 SN/价目禁止礼物。
			if (sn <= 0 || price <= 0)
			{
				LOG(LOG_DEBUG, "UICashShop: BtGift blocked sn=" << sn << " price=" << price);
				return Button::State::NORMAL;
			}
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			std::string confirm = Locale::format_all("cashshop.gift_confirm", {
				iname, std::to_string(price), std::to_string(sn) });
			UI::get().emplace<UIYesNo>(confirm, [sn, pre, iname](bool yes) {
				if (!yes) return;
				// s9 CashOperationHandler action 0x04：gift(birthday, sn, name, msg)。
				// birthday 原生用 0（非生日礼物），name 取 CharInfo 目标名或用户输入。
				auto dispatch_gift = [sn](const std::string& name) {
					if (name.empty())
						return;
					CashShopGiftPacket(0, sn, name, "").dispatch();
					LOG(LOG_INFO, "UICashShop: gift sn=" << sn << " name=" << name);
				};
				if (!pre.empty()) {
					dispatch_gift(pre);
				} else {
					UI::get().emplace<UISoftKey>([dispatch_gift](const std::string& name) {
						dispatch_gift(name);
					}, []() {}, Locale::text("cashshop.gift_name_hint"));
				}
			});
			return Button::State::NORMAL;
		}

		if (buttonid >= Buttons::BtBuy && buttonid < Buttons::BtBuy + MAX_ITEMS)
		{
			int16_t index = static_cast<int16_t>(buttonid - Buttons::BtBuy) + list_offset;

			if (index < 0 || index >= static_cast<int16_t>(items.size()))
				return Button::State::NORMAL;

			const Item& it = items[index];
			int32_t sn = it.get_sn();
			int32_t price = it.get_price();
			int32_t item_id = it.get_item_id();
			if (sn <= 0 || price <= 0)
			{
				LOG(LOG_DEBUG, "UICashShop: BtBuy blocked sn=" << sn << " price=" << price);
				return Button::State::NORMAL;
			}

			// CashShop.img/CSNotice/4（MinMS UICashBuyConfirm）：支付勾选 + OK/Cancel。
			const int32_t nx = UIStateCashShop::cash_balances_ready
				? UIStateCashShop::cash_nx_credit : 0;
			const int32_t maple = UIStateCashShop::cash_balances_ready
				? UIStateCashShop::cash_maple_point : 0;
			const int32_t prepaid = UIStateCashShop::cash_balances_ready
				? UIStateCashShop::cash_nx_prepaid : 0;
			UI::get().emplace<UICashBuyConfirm>(
				item_id, price, sn, nx, maple, prepaid,
				[sn](int32_t useNX) {
					UICashShop::set_pending_buy(sn);
					CashShopBuyPacket(useNX, sn).dispatch();
					LOG(LOG_DEBUG, "UICashShop: BtBuy CSNotice useNX=" << useNX << " sn=" << sn);
				});

			return Button::State::NORMAL;
		}

		return Button::State::DISABLED;
	}

	Cursor::State UICashShop::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Point<int16_t> cursor_relative = cursorpos - position;

		// 页签条命中：CSTab @ (272,17)，贴图 508×78，亮块边界实测见 TAB_HIT_X。
		const int16_t tab_x = cursor_relative.x() - WND_TAB_X;
		const int16_t tab_y = cursor_relative.y() - WND_TAB_Y;
		if (tab_x >= 1 && tab_x < 503 && tab_y >= 0 && tab_y < 40)
		{
			static const int16_t TAB_HIT_X[10] = { 1, 51, 104, 157, 210, 264, 315, 366, 432, 503 };
			for (int8_t i = 0; i < 9; i++)
			{
				if (tab_x >= TAB_HIT_X[i] && tab_x < TAB_HIT_X[i + 1])
				{
					if (menu_index != i)
					{
						menu_index = i;
						category_sub = 0;
						list_offset = 0;
						rebuild_items();
						update_items();
					}
					return Cursor::State::IDLE;
				}
			}
		}

		// 二级分类栏命中（与主页签区不重叠）
		{
			const int8_t sub = hit_category_sub_tab(cursor_relative);
			if (sub >= 0)
			{
				if (category_sub != sub)
				{
					category_sub = static_cast<uint8_t>(sub);
					list_offset = 0;
					rebuild_items();
					update_items();
				}
				return Cursor::State::IDLE;
			}
		}

		// 背包类型切换页签命中（2026-11-18 补齐）
		{
			const int8_t bt = hit_bag_tab(cursor_relative);
			if (bt >= 0)
			{
				if (clicked && backpack_tab != static_cast<uint8_t>(bt))
					backpack_tab = static_cast<uint8_t>(bt);
				return Cursor::State::IDLE;
			}
		}

		// D 4 模式 tab 命中（点击切 mode）：0 热点推荐 / 1 购买试穿 / 2 我的现状 / 3 原型。
		// Tooltip::Parent 无 CASHSHOP 枚举，按"照抄 UIStateGame"口径用 SHOP。
		{
			const int8_t mt = hit_mode_tab(cursor_relative);
			if (mt >= 0)
			{
				if (clicked && mode_tab != static_cast<uint8_t>(mt))
				{
					mode_tab = static_cast<uint8_t>(mt);
					if (mode_tab == 1)
					{
						// 购买试穿：进入 try-on 模式，无需额外状态（货板点击即试穿）。
						LOG(LOG_INFO, "UICashShop: mode=try-on");
					}
					else if (mode_tab == 2)
					{
						reset_preview_to_baseline();
						LOG(LOG_INFO, "UICashShop: mode=my-look reset baseline");
					}
					else if (mode_tab == 3)
					{
						// 原型：本端口无 reset_preview_to_prototype，按拍板决策回落基线。
						reset_preview_to_baseline();
						LOG(LOG_INFO, "UICashShop: mode=prototype fallback baseline");
					}
					else
					{
						LOG(LOG_INFO, "UICashShop: mode=hot-recs");
					}
					clear_tooltip_shop();
				}
				return Cursor::State::IDLE;
			}
		}

		// list_slider 已移除（2026-11-18），分页统一由箭头按钮 + change_list_page 处理。

		// 货板左区（图标）点击试穿；购买/礼物钮仍走 MapleButton。
		if (try_on_plate_at(cursor_relative, clicked))
		{
			// G 装备 tooltip：悬停商品卡显示 item tooltip（装备号段自动走 EquipTooltip）。
			show_item_at_plate(cursor_relative);
			return clicked ? Cursor::State::CLICKING : Cursor::State::IDLE;
		}

		// G 热销榜图标悬停：按榜单 SN 回查目录 itemid 显示 tooltip。
		show_best_tooltip(cursor_relative);

		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UICashShop::send_key(int32_t keycode, bool pressed, bool escape)
	{
		// 预览沙盒：方向/跳（含 key-up）；须在 ESC/B/G 之前。
		switch (static_cast<KeyAction::Id>(keycode))
		{
		case KeyAction::Id::LEFT:
			preview_actor.send_action(KeyAction::Id::LEFT, pressed);
			return;
		case KeyAction::Id::RIGHT:
			preview_actor.send_action(KeyAction::Id::RIGHT, pressed);
			return;
		case KeyAction::Id::UP:
			preview_hold_up = pressed;
			preview_actor.send_action(KeyAction::Id::UP, pressed);
			return;
		case KeyAction::Id::DOWN:
			preview_hold_down = pressed;
			preview_actor.send_action(KeyAction::Id::DOWN, pressed);
			return;
		case KeyAction::Id::JUMP:
			preview_actor.send_action(KeyAction::Id::JUMP, pressed);
			return;
		default:
			break;
		}

		if (!pressed)
			return;
		if (escape) { exit_cashshop(); return; }
		if (keycode == 'B' || keycode == 'b')
		{
			button_pressed(Buttons::BtBuyAvatar);
		}
		else if (keycode == 'S' || keycode == 's')
		{
			button_pressed(Buttons::BtSearch);
		}
		else if (keycode == 'G' || keycode == 'g')
		{
			// 礼物快捷键：按 SN 送礼（复用 BtGift 的 s9 action 0x04 逻辑）。
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				try {
					int32_t sn = std::stoi(tip);
					if (sn <= 0) return;
					std::string name;
					if (auto ci = UI::get().get_element<UICharInfo>())
						name = ci->get_target_name();
					auto give = [sn](const std::string& n) {
						if (!n.empty()) CashShopGiftPacket(0, sn, n, "").dispatch();
					};
					if (!name.empty()) give(name);
					else UI::get().emplace<UISoftKey>([give](const std::string& n) { give(n); },
						[]() {}, Locale::text("cashshop.gift_name_hint"));
				} catch (...) {}
			}, []() {}, "SN");
		}
		else if (keycode == 'T' || keycode == 't')
		{
			button_pressed(Buttons::BtTakeoffAvatar);
		}
		else if (keycode == 'P' || keycode == 'p')
		{
			button_pressed(Buttons::BtSaveAvatar);
		}
		else if (keycode == 'W' || keycode == 'w')
		{
			button_pressed(Buttons::BtWish);
		}
		else if (keycode == 'M' || keycode == 'm')
		{
			button_pressed(Buttons::BtMileage);
		}
		else if (keycode == 'K' || keycode == 'k')
		{
			// 礼包购买：复用 BtDetailPackage（打开礼包详情），不发明新 opcode。
			button_pressed(Buttons::BtDetailPackage);
		}
		else if (keycode == 'D' || keycode == 'd')
		{
			button_pressed(Buttons::BtDetailPackage);
		}
		else if (keycode == 'I' || keycode == 'i')
		{
			// 背包扩容：s9 无对应 opcode，复用扩展钮的本地提示。
			button_pressed(Buttons::BtExtConsume);
		}
		else if (keycode == 'X' || keycode == 'x')
		{
			// 仓库扩容：同上，本地提示。
			button_pressed(Buttons::BtExtTrunk);
		}
		else if (keycode == 'C' || keycode == 'c')
		{
			// 角色栏扩容：同上，本地提示。
			button_pressed(Buttons::BtExtEquip);
		}
		else if (keycode == 'A' || keycode == 'a')
		{
			auto ui = UI::get().get_element<UICashGachapon>();
			if (!ui) ui = UI::get().emplace<UICashGachapon>();
			else if (!ui->is_active()) ui->makeactive();
		}
		else if (keycode == 'R' || keycode == 'r')
		{
			refresh_cs_meta();
			LOG(LOG_DEBUG, "UICashShop: refresh_cs_meta");
		}
		else if (keycode == 'E' || keycode == 'e')
		{
			auto ui = UI::get().get_element<UIRemoteGachapon>();
			if (!ui) ui = UI::get().emplace<UIRemoteGachapon>();
			else if (!ui->is_active()) ui->makeactive();
		}
	}

	UIElement::Type UICashShop::get_type() const
	{
		return TYPE;
	}

	void UICashShop::refresh_identity()
	{
		// Angel CCashShop 顶栏用本角色职业名 + 角色名；禁止 stub 占位。
		try
		{
			const CharStats& st = Stage::get().get_player().get_stats();
			std::string job = st.get_jobname();
			std::string name = st.get_name();
			if (job.empty())
				job = Locale::text("cashshop.job_unknown");
			if (name.empty())
				name = Locale::text("cashshop.name_unknown");
			job_label.change_text(job);
			name_label.change_text(name);
		}
		catch (...)
		{
			job_label.change_text(Locale::text("cashshop.job_unknown"));
			name_label.change_text(Locale::text("cashshop.name_unknown"));
		}
	}

	void UICashShop::refresh_cs_meta()
	{
		refresh_identity();
		int wish_n = 0;
		for (int32_t sn : UIStateCashShop::cash_wishlist)
			if (sn != 0)
				++wish_n;
		std::string tip = Locale::format_all("cashshop.meta",
			{ std::to_string(UIStateCashShop::cash_inventory.size()),
			std::to_string(UIStateCashShop::cash_gifts.size()), std::to_string(wish_n) });
		if (UIStateCashShop::cash_storage_slots > 0)
			tip += " " + Locale::format("cashshop.meta_stor", std::to_string(UIStateCashShop::cash_storage_slots));
		cs_meta_label.change_text(tip);
	}

	// ---- 购买回执状态机（MinMS B3 移植；仅客户端状态，不新增 opcode） ----
	// static 定义放在 UICashShop.cpp，供 Net/Handlers 经 UICashShop:: 静态方法访问。
	int32_t UICashShop::pending_buy_sn = 0;

	void UICashShop::set_pending_buy(int32_t sn)
	{
		// 发购买包后置位：记录本次请求的 sn，供成功/失败回执对账。
		pending_buy_sn = sn;
		LOG(LOG_DEBUG, "UICashShop: pending_buy_sn set=" << sn);
	}

	void UICashShop::confirm_purchase_success()
	{
		// s9 服务端 buy 成功后回 0x57 showBoughtCashItem（单件）/0x89（礼包），
		// handler 已把到货项 push 进 cash_inventory。本端"购物车"栏即该服务端权威
		// 容器，无 MinMS 的客户端 cart_items，故这里只清 pending 并刷新 UI：
		// 不按 sn 删除——0x57 的 sn 就是刚到货项，删它会把新买的东西删掉。
		if (pending_buy_sn == 0)
			return;
		const int32_t sn = pending_buy_sn;
		pending_buy_sn = 0;
		// 购物车完整生命周期：购买成功后从待购暂存移除该 sn（不删 cash_inventory！）。
		{
			auto& cart = UIStateCashShop::local_cart_sns;
			auto it = std::find(cart.begin(), cart.end(), sn);
			if (it != cart.end()) cart.erase(it);
		}
		refresh_cs_meta();
		// 2026-11-23 K 项补齐：停在当前 tab 立即重建 items，避免必须切 tab 才见新货。
		// 仅当本 UI 处于 active 时重建，杜绝后台/关闭态的无效重绘。
		if (is_active())
		{
			rebuild_items();
			update_items();
		}
		LOG(LOG_DEBUG, "UICashShop: purchase confirmed sn=" << sn
			<< " inv=" << UIStateCashShop::cash_inventory.size());
	}

	void UICashShop::clear_pending_buy()
	{
		// 失败回执（0x5C）：清 pending，保留购物车不动（MinMS 同口径）。
		if (pending_buy_sn != 0)
			LOG(LOG_DEBUG, "UICashShop: purchase failed, pending cleared sn=" << pending_buy_sn);
		pending_buy_sn = 0;
	}

	bool UICashShop::can_try_on(int32_t item_id) const
	{
		if (item_id <= 0)
			return false;

		const int32_t type = item_id / 10000;
		const int32_t prefix = item_id / 1000000;

		// 脸/发券 2xxxx / 3xxxx
		if (type == 2 || type == 3)
			return true;

		// Cash effect coupons: Cash img carries an effect node.
		if (has_cash_item_effect(item_id))
			return true;

		// Pets (500xxxx): preview via PetLook.
		if (type == 500)
			return true;

		// 角色装备 1xxxxxx；跳过宠物装 180–183
		if (prefix == 1 && !(type >= 180 && type <= 183))
			return true;

		return false;
	}

	void UICashShop::reset_preview_to_baseline()
	{
		avatar_look = avatar_baseline;
		avatar_look.set_direction(false);
		avatar_look.set_stance(Stance::Id::STAND1);
		avatar_look.set_expression(Expression::Id::DEFAULT);
		preview_try_on_items.clear();
		clear_preview_pet();
		clear_preview_effect();
		init_preview_sandbox();
	}

	void UICashShop::init_preview_sandbox()
	{
		// 治本：preview_actor 默认构造的 look 其 face/body/hair 裸指针全为
		// nullptr，update() 里 face->get_delay() 会解引用空指针闪退。
		// 灌入进店玩家的外观，让预览沙盒立绘有完整外观（face/body/hair 指向
		// facetypes/hairstyles/bodytypes 静态 map 元素，浅拷贝指针稳定）。
		// 玩家上下文缺失时跳过赋值，由 CharLook::update() 的空守卫兜底不崩。
		try
		{
			const CharLook& playerlook = Stage::get().get_player().get_look();
			if (playerlook.get_face() && playerlook.get_body() && playerlook.get_hair())
				preview_actor.get_look() = playerlook;
		}
		catch (...)
		{
		}

		preview_physics.install_floor(
			0,
			static_cast<int16_t>(PREVIEW_WELL_W - 1),
			PREVIEW_FLOOR_Y);
		preview_ladder = Ladder(
			PREVIEW_LADDER_X,
			PREVIEW_LADDER_Y1,
			PREVIEW_LADDER_Y2,
			true);
		preview_actor.use_sandbox_fht(&preview_physics.get_fht());
		preview_actor.get_stats().set_stat(MapleStat::Id::HP, 100);
		preview_actor.get_stats().set_stat(MapleStat::Id::MAXHP, 100);
		preview_actor.get_stats().init_totalstats();
		preview_actor.set_state(Char::State::STAND);
		preview_actor.set_direction(true);
		preview_actor.respawn(
			Point<int16_t>(PREVIEW_SPAWN_X, static_cast<int16_t>(PREVIEW_FLOOR_Y - 8)),
			false);
		preview_actor.set_direction(true);
		preview_hold_up = false;
		preview_hold_down = false;
	}

	Point<int16_t> UICashShop::preview_feet_art() const
	{
		const Point<int16_t> well = preview_actor.get_position();
		return Point<int16_t>(
			static_cast<int16_t>(PREVIEW_WELL_X + well.x()),
			static_cast<int16_t>(PREVIEW_WELL_Y + well.y()));
	}

	nl::node UICashShop::cash_item_effect_root(int32_t item_id) const
	{
		if (item_id <= 0)
			return {};
		const std::string strprefix = "0" + std::to_string(item_id / 10000);
		const std::string strid = "0" + std::to_string(item_id);
		return nl::nx::Item["Cash"][strprefix + ".img"][strid]["effect"];
	}

	nl::node UICashShop::cash_item_effect_stance(int32_t item_id, Stance::Id stance) const
	{
		nl::node effect = cash_item_effect_root(item_id);
		if (!effect)
			return {};
		// v83 Cash effect nodes are keyed by stance name (stand1) or by frame
		// index ("0","1"...) under effect/ or effect/default/, NOT by this port's
		// Stance::names (whose STAND1 slot is misaligned to "stabTF"). Prefer the
		// stance literal, then frame 0, then default so every on-sale layout resolves.
		const char* stance_literals[] = { "stand1", "stand", "default" };
		for (const char* lit : stance_literals)
		{
			nl::node hit = effect[lit];
			if (hit)
				return hit;
		}
		return effect["0"];
	}

	bool UICashShop::has_cash_item_effect(int32_t item_id) const
	{
		return static_cast<bool>(cash_item_effect_stance(item_id, Stance::Id::STAND1));
	}

	void UICashShop::clear_preview_effect()
	{
		preview_effect_item_id = 0;
		preview_effect = Animation();
		preview_effect_below = false;
	}

	void UICashShop::set_preview_effect(int32_t item_id)
	{
		nl::node src = cash_item_effect_stance(item_id, Stance::Id::STAND1);
		if (!src)
		{
			clear_preview_effect();
			return;
		}
		int32_t z = 0;
		if (src["z"].data_type() == nl::node::type::integer)
			z = src["z"];
		preview_effect_item_id = item_id;
		preview_effect_below = z < 0;
		preview_effect = Animation(src);
	}

	void UICashShop::clear_preview_pet()
	{
		preview_pet = PetLook();
	}

	void UICashShop::set_preview_pet(int32_t item_id)
	{
		if (item_id <= 0)
		{
			clear_preview_pet();
			return;
		}
		const Point<int16_t> feet = preview_actor.get_position();
		const std::string pet_name = ItemData::get(item_id).get_name();
		preview_pet = PetLook(item_id, pet_name, 0,
			Point<int16_t>(static_cast<int16_t>(feet.x() - 40), feet.y()), 2, 0);
	}

	void UICashShop::apply_preview_controls()
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
		case Char::State::WALK:
			st = Stance::Id::WALK1;
			break;
		case Char::State::FALL:
			st = Stance::Id::JUMP;
			break;
		case Char::State::LADDER:
			st = Stance::Id::LADDER;
			break;
		case Char::State::ROPE:
			st = Stance::Id::ROPE;
			break;
		case Char::State::PRONE:
			st = Stance::Id::PRONE;
			break;
		case Char::State::STAND:
		default:
			st = Stance::Id::STAND1;
			break;
		}

		avatar_look.set_stance(st);
		avatar_look.set_direction(preview_actor.getflip());
	}

	void UICashShop::clear_conflicting_try_ons(int32_t item_id)
	{
		const int32_t type = item_id / 10000;

		if (type == 2 || type == 3)
		{
			for (auto it = preview_try_on_items.begin(); it != preview_try_on_items.end();)
			{
				if ((*it) / 10000 == type)
					it = preview_try_on_items.erase(it);
				else
					++it;
			}
			return;
		}

		// Pets: only one preview pet at a time.
		if (type == 500)
		{
			for (auto it = preview_try_on_items.begin(); it != preview_try_on_items.end();)
			{
				if ((*it) / 10000 == 500)
					it = preview_try_on_items.erase(it);
				else
					++it;
			}
			return;
		}

		// Cash effects: only one preview effect at a time.
		if (has_cash_item_effect(item_id))
		{
			for (auto it = preview_try_on_items.begin(); it != preview_try_on_items.end();)
			{
				if (has_cash_item_effect(*it))
					it = preview_try_on_items.erase(it);
				else
					++it;
			}
			return;
		}

		const EquipData& ed = EquipData::get(item_id);
		if (!ed)
			return;

		const EquipSlot::Id slot = ed.get_eqslot();
		for (auto it = preview_try_on_items.begin(); it != preview_try_on_items.end();)
		{
			const EquipData& other = EquipData::get(*it);
			if (other && other.get_eqslot() == slot)
				it = preview_try_on_items.erase(it);
			else
				++it;
		}
	}

	void UICashShop::apply_try_on_item(int32_t item_id)
	{
		if (!can_try_on(item_id))
			return;

		clear_conflicting_try_ons(item_id);
		preview_try_on_items.insert(item_id);

		// 回基线后按集合重放，保证同槽只留最后一件。
		avatar_look = avatar_baseline;
		avatar_look.set_direction(false);
		clear_preview_pet();
		clear_preview_effect();
		for (int32_t id : preview_try_on_items)
		{
			const int32_t type = id / 10000;
			if (type == 2)
				avatar_look.set_face(id);
			else if (type == 3)
				avatar_look.set_hair(id);
			else if (type == 500)
				set_preview_pet(id);
			else if (has_cash_item_effect(id))
				set_preview_effect(id);
			else
				avatar_look.add_equip(id);
		}

		avatar_look.set_stance(Stance::Id::STAND1);
		avatar_look.set_expression(Expression::Id::DEFAULT);
		LOG(LOG_DEBUG, "UICashShop: try-on item=" << item_id
			<< " stack=" << preview_try_on_items.size());
	}

	void UICashShop::toggle_try_on_item(int32_t item_id)
	{
		if (!can_try_on(item_id))
			return;

		if (preview_try_on_items.count(item_id) > 0)
		{
			preview_try_on_items.erase(item_id);
			avatar_look = avatar_baseline;
			avatar_look.set_direction(false);
			clear_preview_pet();
			clear_preview_effect();
			for (int32_t id : preview_try_on_items)
			{
				const int32_t type = id / 10000;
				if (type == 2)
					avatar_look.set_face(id);
				else if (type == 3)
					avatar_look.set_hair(id);
				else if (type == 500)
					set_preview_pet(id);
				else if (has_cash_item_effect(id))
					set_preview_effect(id);
				else
					avatar_look.add_equip(id);
			}
			avatar_look.set_stance(Stance::Id::STAND1);
			avatar_look.set_expression(Expression::Id::DEFAULT);
			LOG(LOG_DEBUG, "UICashShop: untry item=" << item_id);
			return;
		}

		apply_try_on_item(item_id);
	}

	bool UICashShop::try_on_plate_at(Point<int16_t> cursor_relative, bool clicked)
	{
		for (size_t i = 0; i < MAX_ITEMS; i++)
		{
			const int16_t index = static_cast<int16_t>(i + list_offset);
			if (index < 0 || index >= static_cast<int16_t>(items.size()))
				continue;

			const int col = static_cast<int>(i % 2);
			const int row = static_cast<int>(i / 2);
			const int16_t plate_x = static_cast<int16_t>(WND_LIST_X + PLATE_DX * col);
			const int16_t plate_y = static_cast<int16_t>(WND_LIST_Y + 2 + PLATE_DY * row);
			// 左图标区，避开 BtBuy/BtGift（约 x+80）。
			Rectangle<int16_t> icon_hit(
				Point<int16_t>(plate_x, plate_y),
				Point<int16_t>(static_cast<int16_t>(plate_x + 78), static_cast<int16_t>(plate_y + PLATE_H)));

			if (!icon_hit.contains(cursor_relative))
				continue;

			if (clicked)
				toggle_try_on_item(items[static_cast<size_t>(index)].get_item_id());

			return true;
		}

		return false;
	}

	void UICashShop::exit_cashshop()
	{
		// 2026-11-18 退出商城黑屏根治在 Stage::load 侧（见 Stage.cpp:62：
		// mapid 变化即强制 load_map）。进商城时 load_map(-1) 把 Stage::mapid 置 -1，
		// 这里的 load(真实mapid) 因此能真正换图、respawn 按真实地图重播 BGM。
		// 随后服务端 SET_FIELD 抵达还会权威刷新一次（同一 mapid），属"本地即时恢复 +
		// 服务端权威刷新"双路径，状态一致，不会黑屏。此路径保持原样不再手动 clear。
		UI& ui = UI::get();
		ui.change_state(UI::State::GAME);

		Stage& stage = Stage::get();
		Player& player = stage.get_player();

		PlayerLoginPacket(player.get_oid()).dispatch();

		int32_t mapid = player.get_stats().get_mapid();
		uint8_t portalid = player.get_stats().get_portal();

		stage.load(mapid, portalid);
		stage.transfer_player();

		ui.enable();
		Timer::get().start();
		GraphicsGL::get().unlock();
	}

	void UICashShop::refresh_best_icons()
	{
		// 右侧热销榜：按服务端 cash_best_sellers 顺序回查目录取图标。
		// 名次数字/底色烘焙在 backgrnd，这里只填 x=742..773 的浅色槽。
		for (uint8_t i = 0; i < BEST_ROWS; i++)
			best_icons[i] = Texture();
		const std::vector<CashCommodity>& catalog = UIStateCashShop::cashshop_commodities;
		const std::vector<int32_t>& best = UIStateCashShop::cash_best_sellers;
		for (uint8_t i = 0; i < BEST_ROWS && i < best.size(); i++)
		{
			for (const CashCommodity& c : catalog)
			{
				if (c.sn != best[i] || c.itemid <= 0)
					continue;
				best_icons[i] = ItemData::get(c.itemid).get_icon(false);
				break;
			}
		}
	}

	// A 徽章 label 推导：唯一有据可依的是服务端/Commodity 的 priority>=8（热卖标记，
	// 与 matches_sub 里装备"热销按 priority>=8"同口径）。period/price 无法可靠
	// 判定 NEW/SALE/EVENT 等标签，故其余一律 NONE——不臆测，避免误导玩家。
	// 判定内联在 rebuild_items 成员上下文：Item 是 UICashShop 私有嵌套类，
	// 若抽成命名空间级自由函数，其签名里的 Item::Label 会在类外作用域解析而触发 C2248。
	void UICashShop::rebuild_items()
	{
		refresh_best_icons();
		list_offset = 0;
		items.clear();
		const int8_t want_cat = TAB_CATEGORY[menu_index < 9 ? menu_index : 0];

		if (want_cat == -1)
		{
			// 购物车：已购未取（0x4B showCashInventory 落地的 cash_inventory）。
			for (const CashInvEntry& e : UIStateCashShop::cash_inventory)
			{
				if (e.itemid <= 0)
					continue;
				items.emplace_back(e.itemid, e.sn, 0, Item::Label::NONE, 0, static_cast<uint16_t>(e.qty > 0 ? e.qty : 1));
			}
			return;
		}

		const std::vector<CashCommodity>& catalog = UIStateCashShop::cashshop_commodities;

		if (menu_index == 1)
		{
			// 热卖：服务端 openCashShop 网格给出的榜位序列（cat × gender × 前 5）。
			// 按 SN 回查目录，保持服务端顺序；目录里没有的 SN 跳过。
			for (int32_t sn : UIStateCashShop::cash_best_sellers)
			{
				for (const CashCommodity& c : catalog)
				{
					if (c.sn != sn)
						continue;
					const Item::Label label = c.priority >= 8 ? Item::HOT : Item::NONE;
					items.emplace_back(c.itemid, c.sn, c.price, label, 0, static_cast<uint16_t>(c.count));
					break;
				}
			}
			if (!items.empty())
				return;
			// 榜单为空（新服无购买记录）时回落全目录，避免整页签空白。
		}

		for (const CashCommodity& c : catalog)
		{
			// 首页（MAIN=8）在本服目录仅 4 条，作为落地页会重新造成「商城没商品」
			// 的观感；故首页显示全量（目录已按 Priority 降序 → ItemId 升序）。
			if (want_cat != 8 && c.sn / 10000000 != want_cat)
				continue;
			if (active_sub_count() > 0 && !matches_sub(c))
				continue;
			// 2026-11-23 全量补齐：本地搜索过滤（BtSearch 写 search_query）。大小写不敏感子串匹配。
			if (!search_query.empty())
			{
				std::string lname = c.name, lq = search_query;
				std::transform(lname.begin(), lname.end(), lname.begin(), [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
				std::transform(lq.begin(), lq.end(), lq.begin(), [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
				if (lname.find(lq) == std::string::npos)
					continue;
			}
			const Item::Label label = c.priority >= 8 ? Item::HOT : Item::NONE;
			items.emplace_back(c.itemid, c.sn, c.price, label, 0, static_cast<uint16_t>(c.count));
		}
		sync_page_chrome();
	}

	// MinMS list_page_count:720-730 / sync_page_chrome:741-759 / change_list_page:760-780.
	int16_t UICashShop::list_page_count() const
	{
		return static_cast<int16_t>((items.size() + MAX_ITEMS - 1) / MAX_ITEMS);
	}

	void UICashShop::sync_page_chrome()
	{
		const int16_t pages = list_page_count();
		const int16_t cur = static_cast<int16_t>(list_offset / MAX_ITEMS);
		// H 页码列表串：当前页 <N> / 其它页 [M]，最多当前页附近 5 个 + 末页。
		// 前后翻页仍由现有箭头按钮承担，change_list_page 不变。
		{
			std::string label;
			int16_t page_total = pages < 1 ? 1 : pages;
			// 窗口：优先以当前页居中取 5 页，夹到 [0, page_total)。
			int16_t from = static_cast<int16_t>(std::max<int16_t>(0, cur - 2));
			int16_t to = static_cast<int16_t>(std::min<int16_t>(page_total - 1, from + 4));
			from = static_cast<int16_t>(std::max<int16_t>(0, std::min<int16_t>(from, to - 4)));
			for (int16_t p = from; p <= to; p++)
			{
				if (!label.empty())
					label += " ";
				if (p == cur)
					label += "<" + std::to_string(p + 1) + ">";
				else
					label += "[" + std::to_string(p + 1) + "]";
			}
			// 末页不在窗口内时补上（例：16 页第 1 页 → "<1> [2] [3] [4] [5] [16]"）。
			if (to < page_total - 1)
				label += " [" + std::to_string(page_total) + "]";
			page_label.change_text(label);
		}
		if (auto it = buttons.find(Buttons::BtPagePrev); it != buttons.end() && it->second)
			it->second->set_state(cur <= 0 ? Button::State::DISABLED : Button::State::NORMAL);
		if (auto it = buttons.find(Buttons::BtPageNext); it != buttons.end() && it->second)
			it->second->set_state(pages <= 1 || cur >= pages - 1 ? Button::State::DISABLED : Button::State::NORMAL);
	}

	void UICashShop::change_list_page(int16_t delta)
	{
		const int16_t pages = list_page_count();
		int16_t cur = static_cast<int16_t>(list_offset / MAX_ITEMS);
		cur = static_cast<int16_t>(std::max<int16_t>(0, std::min<int16_t>(cur + delta, pages - 1)));
		const int16_t new_offset = static_cast<int16_t>(cur * MAX_ITEMS);
		if (new_offset == list_offset)
			return;
		list_offset = new_offset;
		update_items();
		sync_page_chrome();
		LOG(LOG_DEBUG, "UICashShop: page=" << (cur + 1) << "/" << pages);
	}

	void UICashShop::update_items()
	{
		for (size_t i = 0; i < MAX_ITEMS; i++)
		{
			int16_t index = i + list_offset;
			bool found_item = index < items.size();

			// 正确截图：购买/礼物钮仅对有效价目商品可点（MinMS purchase_blocked）。
			bool buyable = found_item && items[index].get_sn() > 0 && items[index].get_price() > 0;
			if (auto it = buttons.find(Buttons::BtBuy + static_cast<uint16_t>(i)); it != buttons.end() && it->second)
				it->second->set_active(buyable);
			if (auto it = buttons.find(Buttons::BtGift + static_cast<uint16_t>(i)); it != buttons.end() && it->second)
				it->second->set_active(buyable);
			// B 购物车钮与购买/礼物同启用条件，避免空卡也能点。
			if (auto it = buttons.find(Buttons::BtCart + static_cast<uint16_t>(i)); it != buttons.end() && it->second)
				it->second->set_active(buyable);

			std::string name = "";
			std::string price_text = "";
			std::string discount_text = "";
			std::string percent_text = "";

			if (found_item)
			{
				Item item = items[index];

				name = item.get_name();

				// 已购未取（CSInventory/CSLocker）栏：这些数据来自服务端权威
				// cash_inventory（0x4B/0x57/0x89/0x8D），没有商城售价（rebuild_items
				// 里 price=0），显示持有数量而非误导性的"0 点券"。本端 NX 的
				// CSLocker 只有 BtRebate、CSInventory 只有 BtEx* 扩栏按钮，没有
				// 专用格子贴图（UI.nx 实测），故复用 CSList 商品格渲染——这是有
				// 资源证据的最小实现，不臆造贴图、不碰 s9 包体。
				const bool locker_view = (menu_index == 8);
				if (locker_view)
				{
					// L locker 持有数走 locale 格式 cashshop.hold_count="x{}"，不再硬编码。
					price_text = Locale::format("cashshop.hold_count",
					                            std::to_string(item.count > 0 ? item.count : 1));
				}
				else
				{
					int32_t price = item.get_price();
					price_text = std::to_string(price);

					if (item.discount_price > 0 && price > 0)
					{
						discount_text = price_text;

						uint32_t discount = item.discount_price;
						price_text = std::to_string(discount);

						// C 折扣百分比：0.5 要显示 50% 而不是 substr(2,1) 截出的 5%。
						// 用 std::lround(percent*100) 四舍五入到整数百分比。
						float_t percent = static_cast<float_t>(discount) / static_cast<float_t>(price);
						percent_text = "(" + std::to_string(static_cast<int>(std::lround(percent * 100.0))) + "%)";
					}

					string_format::split_number(price_text);
					string_format::split_number(discount_text);

					// 官方基准（商城.png）：价格 = '2,700 点券'，无 NX 后缀、无 count 括号尾巴。
					price_text += " " + Locale::text("cashshop.currency");

					if (discount_text != "")
						discount_text += " " + Locale::text("cashshop.currency");
				}
			}

			item_name[i].change_text(name);
			item_price[i].change_text(price_text);
			item_discount[i].change_text(discount_text);
			item_percent[i].change_text(percent_text);

			string_format::format_with_ellipsis(item_name[i], 116);
		}
	}
	// ---- 二级分类（移植自 MinMS CommodityData + UICashShop）----
	static const int8_t SUB_COUNTS[9] = { 0, 2, 12, 3, 0, 7, 3, 0, 0 };
	static const char* const SUB_LABELS[9][12] =
	{
		{  },
		{ "\xe6\x96\xb0\xe5\x93\x81", "\xe6\xb4\xbb\xe5\x8a\xa8" },
		{ "\xe5\xb8\xbd\xe5\xad\x90", "\xe8\x84\xb8\xe9\xa5\xb0", "\xe7\x9c\xbc\xe9\xa5\xb0", "\xe9\x95\xbf\xe8\xa2\x8d", "\xe7\x83\xad\xe9\x94\x80", "\xe8\xa3\xa4\xe8\xa3\x99", "\xe9\x9e\x8b\xe5\xad\x90", "\xe6\x89\x8b\xe5\xa5\x97", "\xe6\xad\xa6\xe5\x99\xa8", "\xe6\x88\x92\xe6\x8c\x87", "\xe5\x8b\x8b\xe7\xab\xa0", "\xe6\x8a\xab\xe9\xa3\x8e" },
		{ "\xe5\x8d\xb7\xe8\xbd\xb4", "\xe9\x80\x9a\xe8\xae\xaf\xe7\x89\xa9\xe5\x93\x81", "\xe5\xa4\xa9\xe6\xb0\x94" },
		{  },
		{ "\xe7\xbe\x8e\xe5\xae\xb9\xe7\xbe\x8e\xe5\x8f\x91", "\xe4\xb8\xaa\xe4\xba\xba\xe5\x95\x86\xe5\xba\x97", "\xe6\xb8\xb8\xe6\x88\x8f", "\xe8\xa1\xa8\xe6\x83\x85", "\xe5\xa9\x9a\xe7\xa4\xbc", "\xe6\x95\x88\xe6\x9e\x9c", "\xe4\xba\xba\xe7\x89\xa9" },
		{ "\xe5\xae\xa0\xe7\x89\xa9", "\xe5\xae\xa0\xe7\x89\xa9\xe8\xa3\x85\xe5\xa4\x87", "\xe5\xae\xa0\xe7\x89\xa9\xe6\xb6\x88\xe8\x80\x97" },
		{  },
		{  },
	};

	// D 4 模式 tab（运行时绘制，零资源改动）：C++ 画 ColorBox + Text，
	// 选中态用不同底色。位置在预览井底 y≈214，避开 BtBuyAvatar(237)/Default(260)。
	void UICashShop::draw_mode_tabs() const
	{
		for (uint8_t i = 0; i < 4; i++)
		{
			const int16_t tx = static_cast<int16_t>(WND_CHAR_X + MODE_TAB_X + MODE_TAB_W * i);
			// 选中 = 不透明黄底 + 黑字；未选 = 半透明白底 + 黄字。不是资源贴图而是运行时几何。
			ColorBox bg(MODE_TAB_W - 2, MODE_TAB_H,
				i == mode_tab ? Color::Name::YELLOW : Color::Name::WHITE,
				i == mode_tab ? 0.55f : 0.18f);
			bg.draw(DrawArgument(position + Point<int16_t>(tx, MODE_TAB_Y)));
			mode_labels[i].draw(position + Point<int16_t>(
				static_cast<int16_t>(tx + MODE_TAB_W / 2),
				static_cast<int16_t>(MODE_TAB_Y + MODE_TAB_H / 2 - mode_labels[i].height() / 2)));
		}
	}

	int8_t UICashShop::hit_mode_tab(Point<int16_t> cursor_relative) const
	{
		const int16_t x = cursor_relative.x();
		const int16_t y = cursor_relative.y();
		if (y < WND_CHAR_Y + MODE_TAB_Y || y >= WND_CHAR_Y + MODE_TAB_Y + MODE_TAB_H)
			return -1;
		const int16_t left = static_cast<int16_t>(WND_CHAR_X + MODE_TAB_X);
		if (x < left || x >= left + MODE_TAB_W * 4)
			return -1;
		return static_cast<int8_t>((x - left) / MODE_TAB_W);
	}

	// G tooltip：商城态用 SHOP 父类型（Tooltip::Parent 无 CASHSHOP 枚举）。
	// UI::show_item 落到 UIStateCashShop::show_item，装备号段自动走 EquipTooltip。
	void UICashShop::clear_tooltip_shop() const
	{
		UI::get().clear_tooltip(Tooltip::Parent::SHOP);
	}

	void UICashShop::show_item_at_plate(Point<int16_t> cursor_relative) const
	{
		for (size_t i = 0; i < MAX_ITEMS; i++)
		{
			const int16_t index = static_cast<int16_t>(i + list_offset);
			if (index < 0 || index >= static_cast<int16_t>(items.size()))
				continue;
			const int col = static_cast<int>(i % 2);
			const int row = static_cast<int>(i / 2);
			const int16_t plate_x = static_cast<int16_t>(WND_LIST_X + PLATE_DX * col);
			const int16_t plate_y = static_cast<int16_t>(WND_LIST_Y + 2 + PLATE_DY * row);
			Rectangle<int16_t> icon_hit(
				Point<int16_t>(plate_x, plate_y),
				Point<int16_t>(static_cast<int16_t>(plate_x + 78), static_cast<int16_t>(plate_y + PLATE_H)));
			if (!icon_hit.contains(cursor_relative))
				continue;
			UI::get().show_item(Tooltip::Parent::SHOP, items[static_cast<size_t>(index)].get_item_id());
			return;
		}
	}

	void UICashShop::show_best_tooltip(Point<int16_t> cursor_relative) const
	{
		for (uint8_t i = 0; i < BEST_ROWS; i++)
		{
			const int16_t bx = BEST_ITEM_X;
			const int16_t by = static_cast<int16_t>(BEST_ITEM_Y0 + BEST_ITEM_DY * i);
			Rectangle<int16_t> slot_hit(Point<int16_t>(bx, by),
			                            Point<int16_t>(static_cast<int16_t>(bx + BEST_ITEM_SLOT),
			                                           static_cast<int16_t>(by + BEST_ITEM_SLOT)));
			if (!slot_hit.contains(cursor_relative))
				continue;
			// 由榜单 SN 回查目录取 itemid（与 refresh_best_icons 同源口径）。
			const std::vector<int32_t>& best = UIStateCashShop::cash_best_sellers;
			if (i >= best.size())
				return;
			for (const CashCommodity& c : UIStateCashShop::cashshop_commodities)
			{
				if (c.sn == best[i] && c.itemid > 0)
				{
					UI::get().show_item(Tooltip::Parent::SHOP, c.itemid);
					return;
				}
			}
			return;
		}
	}

	int8_t UICashShop::active_sub_count() const
	{
		return menu_index < 9 ? SUB_COUNTS[menu_index] : 0;
	}

	void UICashShop::draw_category_sub_tabs() const
	{
		const int8_t count = active_sub_count();
		if (count <= 0)
			return;
		// 2026-11-18：二级分类必须落在 CSTab/Tab/N 贴图下半部烘焙的彩色带上。
		// 贴图 @ (WND_TAB_X, WND_TAB_Y)=(272,17)，色带局部 y=42..65、宽 508。
		// 旧实现用 strip_x=WND_LIST_X=275/y=62/w=412 导致标签偏下、窄、脱背景。
		// 现与 MinMS 同口径：局部 y=42、h=24、w=508（贴图像素实测 + MinMS .h:213-222）。
		const int16_t strip_x = WND_TAB_X;
		const int16_t strip_w = SUB_STRIP_W;
		const int16_t strip_abs_y = WND_TAB_Y + SUB_ROW_Y; // 59 局部
		ColorBox select_bg(1, 1, Color::Name::WHITE, 0.18f);
		for (int8_t i = 0; i < count; ++i)
		{
			const int16_t sx = static_cast<int16_t>(strip_x + i * strip_w / count);
			const int16_t sw = static_cast<int16_t>(strip_x + (i + 1) * strip_w / count - sx);
			if (i == category_sub)
				select_bg.draw(DrawArgument(position + Point<int16_t>(sx, strip_abs_y),
				                            Point<int16_t>(sw, SUB_ROW_H)));
			Text label(Text::Font::A13B, Text::Alignment::CENTER, Color::Name::BLACK,
			           SUB_LABELS[menu_index][i]);
			const int16_t label_y = static_cast<int16_t>(
				strip_abs_y + SUB_ROW_H / 2 - (label.height() + 3) / 2);
			label.draw(position + Point<int16_t>(static_cast<int16_t>(sx + sw / 2), label_y));
		}
	}

	int8_t UICashShop::hit_category_sub_tab(Point<int16_t> cursor_relative) const
	{
		const int8_t count = active_sub_count();
		if (count <= 0)
			return -1;
		const int16_t strip_x = WND_TAB_X;
		const int16_t strip_w = SUB_STRIP_W;
		const int16_t strip_abs_y = WND_TAB_Y + SUB_ROW_Y;
		if (cursor_relative.y() < strip_abs_y - SUB_HIT_PAD_Y ||
		    cursor_relative.y() > strip_abs_y + SUB_ROW_H + SUB_HIT_PAD_Y)
			return -1;
		if (cursor_relative.x() < strip_x || cursor_relative.x() >= strip_x + strip_w)
			return -1;
		int8_t hit = static_cast<int8_t>((cursor_relative.x() - strip_x) * count / strip_w);
		if (hit < 0 || hit >= count)
			hit = static_cast<int8_t>(count - 1);
		return hit;
	}

	int8_t UICashShop::hit_bag_tab(Point<int16_t> cursor_relative) const
	{
		const int16_t x = cursor_relative.x();
		const int16_t y = cursor_relative.y();
		if (x < BAG_TAB_X || x >= BAG_TAB_X + BAG_TAB_W)
			return -1;
		for (uint8_t t = 0; t < 5; t++)
		{
			const int16_t ty = static_cast<int16_t>(BAG_TAB_Y0 + BAG_TAB_DY * t);
			if (y >= ty && y < ty + BAG_TAB_H)
				return static_cast<int8_t>(t);
		}
		return -1;
	}

	bool UICashShop::matches_sub(const CashCommodity& c) const
	{
		const int8_t sn_sub = static_cast<int8_t>((c.sn / 100000) % 100);
		if (menu_index != 2)
			return sn_sub == category_sub;
		// 装备（menu_index==2）：热销按 priority，其余先看 SN 子类
		// （Legacy 行 SN 无子类）再按 itemid/10000 设备类型回落。
		if (category_sub == 4)
			return c.priority >= 8;
		if (sn_sub == category_sub)
			return true;
		const int32_t type = c.itemid / 10000;
		if (type >= 180 && type <= 183)
			return false; // 宠物装备归宠物，不进装备子类
		switch (category_sub)
		{
		case 0:  return type == 100;                       // 帽子
		case 1:  return type == 101 || type == 103;        // 脸饰
		case 2:  return type == 102;                       // 眼饰
		case 3:  return type == 104 || type == 105;        // 长袍
		case 5:  return type == 106;                       // 裤裙
		case 6:  return type == 107;                       // 鞋子
		case 7:  return type == 108;                       // 手套
		case 8:  return type == 109 || (type >= 130 && type <= 170); // 武器
		case 9:  return type == 111;                       // 戒指
		case 10: return type == 114;                       // 勋章
		case 11: return type == 110;                       // 披风
		default: return true;
		}
	}

}