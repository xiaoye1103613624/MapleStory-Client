////////////////////////////////////////////////////////////////////////////////
//  UIAuction — 192 原生 UIAuction.img（UI_002.wz）拍卖行面板
//
//  资源：nl::nx::UI["UIAuction.img"]，取 main/backgrnd 作背景；
//        main/button:Exit / button:Search / button:Sell / button:MarketPrice /
//        button:Complete / button:SlotUI / button:Help 作按钮（缺件优雅降级）。
//  协议：C→S AUCTION_ACTION 0x3751（Net/Packets/AuctionPackets.h）；
//        S→C 由 UIAuctionState（UIAuctionState.h）落地，本类只负责渲染 + 交互。
//  视觉：本项目 192 UI 用通用壳风格，列表面板不需像素级对齐 —— 视觉待精修。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

#include "../../Data/ItemData.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include <cstdint>
#include <string>
#include <vector>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
    class UIAuction : public UIElement
    {
    public:
        static constexpr Type TYPE = UIElement::Type::AUCTION;
        static constexpr bool FOCUSED = false;
        static constexpr bool TOGGLED = true;

        UIAuction();

        void draw(float inter) const override;
        void send_key(int32_t keycode, bool pressed, bool escape) override;
        Button::State button_pressed(uint16_t buttonid) override;
        Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
        void send_scroll(double yoffset) override;
        UIElement::Type get_type() const override;

        // 状态栏提示（成功/失败/行情摘要），由 handler 的 message 码回填。
        void set_status(const std::string& text);

    private:
        enum Buttons : uint16_t
        {
            BtExit = 1000,
            BtSearch = 1001,
            BtSell = 1002,
            BtMarketPrice = 1003,
            BtComplete = 1004,
            BtSlotUI = 1005,
            BtHelp = 1006,
        };

        struct Row
        {
            Text name;
            Text detail;
            Texture icon;
            int64_t inv_id = 0;
            int32_t item_id = 0;
            int64_t meso_or_bid = 0;
        };

        // 当前视图：0=首页在售（home），1=行情（quotes），2=完成（complete）
        uint8_t view = 0;
        int16_t scroll = 0;
        static constexpr int16_t ROW_H = 32;
        static constexpr int16_t LIST_X = 24;
        static constexpr int16_t LIST_Y = 70;
        static constexpr int16_t LIST_W = 360;
        static constexpr int16_t LIST_ROWS = 10;

        Sprite background;
        bool has_background = false;
        Text title;
        Text status;
        Text empty_hint;
        std::vector<Row> rows;      // 当前视图的渲染行（每次 draw 前按 state 重建）
        uint32_t last_seq = 0;      // 上次重建对应的 UIAuctionState 序号

        void rebuild_rows();
        int16_t row_at(Point<int16_t> cursorpos) const;
        void do_home_search();
        void do_market_search();
    };
}
