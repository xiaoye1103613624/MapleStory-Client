//////////////////////////////////////////////////////////////////////////////////
//	CashShop.img/CSNotice/4 — 购买确认（支付勾选），对照 MinMS UICashBuyConfirm
//	桌面端 scale=1.0（不做手机 CHROME_SCALE）。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include <functional>

namespace ms
{
	class UICashBuyConfirm : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::NOTICE;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		// ok(useNX)：CashShopBuyPacket 的 useNX（1=NX / 2=MaplePoint / 4=Prepaid）
		UICashBuyConfirm(
			int32_t item_id,
			int32_t price,
			int32_t sn,
			int32_t nx,
			int32_t maple,
			int32_t prepaid,
			std::function<void(int32_t useNX)> ok_handler,
			std::function<void()> cancel_handler = {});

		void draw(float alpha) const override;
		void update_screen(int16_t new_width, int16_t new_height) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Cursor::State send_cursor(bool pressed, Point<int16_t> cursorpos) override;
		UIElement::Type get_type() const override;

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t
		{
			BT_OK = 0,
			BT_CANCEL,
			BT_OPT_MAPLE,
			BT_OPT_CREDIT,
			BT_OPT_NX
		};

		enum PayOpt : int8_t
		{
			OPT_MAPLE = 0,
			OPT_CREDIT,
			OPT_NX,
			OPT_COUNT
		};

		enum CashType : int32_t
		{
			NX_CREDIT = 1,
			MAPLE_POINT = 2,
			NX_PREPAID = 4
		};

		static constexpr int16_t NATIVE_W = 266;
		static constexpr int16_t NATIVE_H = 189;
		static constexpr int16_t ICON_X = 19;
		static constexpr int16_t ICON_Y = 25;
		static constexpr int16_t ICON_SLOT = 34;
		static constexpr int16_t NAME_X = 60;
		static constexpr int16_t NAME_Y = 28;
		static constexpr int16_t PRICE_Y = 44;
		static constexpr int16_t WARN_X = 14;
		static constexpr int16_t WARN_Y = 66;
		static constexpr int16_t CHECK_X = 28;
		static constexpr int16_t CHECK_Y[3] = { 105, 119, 133 };
		static constexpr int16_t CHECK_HIT_W = 200;
		static constexpr int16_t CHECK_HIT_H = 22;
		static constexpr int16_t BTN_GAP = 10;
		static constexpr int16_t BTN_RIGHT = 21;
		static constexpr int16_t BTN_BOTTOM = 6;

		void layout_chrome();
		void select_opt(PayOpt opt);
		int32_t selected_use_nx() const;
		bool opt_available(PayOpt opt) const;

		Texture strip[5];
		Texture checkbox[2];
		Texture icon;
		Text name_label;
		Text price_label;
		Text warn_label;

		int32_t sn = 0;
		int32_t nx_cash = 0;
		int32_t maple_points = 0;
		int32_t prepaid_cash = 0;
		PayOpt selected = OPT_NX;

		std::function<void(int32_t)> ok_handler;
		std::function<void()> cancel_handler;
	};
}
