////////////////////////////////////////////////////////////////////////////////
//  UIAuction.cpp — 192 UIAuction.img 拍卖行面板
//
//  视觉：通用壳风格（本项目 192 UI 惯例），列表面板不追求像素级对齐，视觉待精修。
//  资源：优先 UIAuction.img/main/backgrnd；按钮从 UIAuction.img/main 下同名节点加载，
//        缺节点则不创建对应按钮 + diag_once 记录，不静默落 (0,0)。
//  数据：UIAuctionState 单例由 5 个 S→C handler 落地；本类 draw 前比对 seq 重建文本行。
//  交互：列表点行=购买（首页/行情）或领取（完成视图），按钮 Sell/MarketPrice/
//        Complete 切视图并按需发包，Search 发搜索包，SlotUI/Help 提示预留。
////////////////////////////////////////////////////////////////////////////////
#include "UIAuction.h"
#include "UIAuctionState.h"

#include "../UI.h"
#include "../KeyAction.h"
#include "../Components/MapleButton.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/AuctionPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"
#include "../../Util/Locale.h"

#include <algorithm>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
    namespace
    {
        // 通用壳默认尺寸：资源缺失时仍能显示列表 + 按钮，可操作。
        constexpr int16_t DEFAULT_W = 400;
        constexpr int16_t DEFAULT_H = 460;

        std::string meso_string(int64_t meso)
        {
            if (meso >= 100000000LL)
                return std::to_string(meso / 100000000LL) + "亿";
            if (meso >= 10000LL)
                return std::to_string(meso / 10000LL) + "万";
            return std::to_string(meso);
        }
    }

    UIAuction::UIAuction()
        : title(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, u8"拍卖行"),
          status(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR, u8"点按钮操作 / Esc 关闭"),
          empty_hint(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE, u8"暂无数据")
    {
        dimension = Point<int16_t>(DEFAULT_W, DEFAULT_H);

#ifdef USE_NX
        // 192 原生在 UI_002.wz/UIAuction.img（不像 Maker 在 UIWindow.img），
        // 直接按 AGENTS 允许的方式索引 nl::nx::UI，并判空优雅降级。
        nl::node aucroot = nl::nx::UI["UIAuction.img"];
        if (aucroot)
        {
            nl::node main = aucroot["main"];
            nl::node panel = main ? main["backgrnd"] : nl::node();
            if (panel)
            {
                background = panel;
                has_background = true;
                dimension = background.get_dimensions();
            }

            if (main)
            {
                // 资源里的按钮名沿用原生 button:XXX；try_maple_btn 内部会判空 + diag。
                try_maple_btn(buttons, BtExit, main["button:Exit"],
                              Point<int16_t>(dimension.x() - 21, 6), "auction BtExit");
                try_maple_btn(buttons, BtSearch, main["button:Search"],
                              Point<int16_t>(dimension.x() - 88, 28), "auction BtSearch");
                try_maple_btn(buttons, BtSell, main["button:Sell"],
                              Point<int16_t>(16, 28), "auction BtSell");
                try_maple_btn(buttons, BtMarketPrice, main["button:MarketPrice"],
                              Point<int16_t>(88, 28), "auction BtMarketPrice");
                try_maple_btn(buttons, BtComplete, main["button:Complete"],
                              Point<int16_t>(160, 28), "auction BtComplete");
                try_maple_btn(buttons, BtSlotUI, main["button:SlotUI"],
                              Point<int16_t>(dimension.x() - 160, 28), "auction BtSlotUI");
                try_maple_btn(buttons, BtHelp, main["button:Help"],
                              Point<int16_t>(dimension.x() - 232, 28), "auction BtHelp");
            }
        }
        else
        {
            UiAssetResolve::diag_once("auction.no_img", "UIAuction.img missing; using generic shell");
        }
#endif

        // SideMenu/StatusBar 入口打开时不落 (0,0)；分辨率变化保持居中。
        layout_policy = LayoutPolicy::Center;
        center_on_screen();

        // 进入拍卖：服务端回 0x3750 + 0x3751(home) + 0x3751(quotes)。
        AuctionActionPacket::enter().dispatch();
        last_seq = 0;
    }

    void UIAuction::rebuild_rows()
    {
        rows.clear();
        auto& state = UIAuctionState::get();
        scroll = 0;

        if (view == 2)
        {
            for (const auto& e : state.complete_list)
            {
                const std::string name = ItemData::get(e.itemId).get_name();
                Row r;
                r.inv_id = e.invId;
                r.item_id = e.itemId;
                r.meso_or_bid = e.bid;
                r.icon = ItemData::get(e.itemId).get_icon(false);
                r.name = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
                              name + " x" + std::to_string(e.qty));
                r.detail = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR,
                                u8"成交/标价 " + meso_string(e.bid) + u8"  [点此领取]");
                rows.push_back(std::move(r));
            }
            last_seq = state.complete_seq;
            return;
        }

        const std::vector<AuctionEntry>& src = (view == 1) ? state.quotes_list : state.home_list;
        for (const auto& e : src)
        {
            const std::string name = ItemData::get(e.itemId).get_name();
            Row r;
            r.inv_id = e.invId;
            r.item_id = e.itemId;
            r.meso_or_bid = e.meso;
            r.icon = ItemData::get(e.itemId).get_icon(false);
            r.name = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
                          name + " x" + std::to_string(e.qty));
            r.detail = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR,
                            meso_string(e.meso) + u8" | " + e.owner);
            rows.push_back(std::move(r));
        }
        last_seq = (view == 1) ? state.quotes_seq : state.home_seq;
    }

    void UIAuction::draw(float inter) const
    {
        if (has_background)
            background.draw(position, inter);

        title.draw(position + Point<int16_t>(dimension.x() / 2, 8));
        status.draw(position + Point<int16_t>(12, dimension.y() - 38));

        // const 路径：用 mutable 缓存文本行（与 UIElement::draw_sprites 的 fade_in 同口径）。
        // 这里通过 const_cast 重建是局部可控的：UIAuction 只有本线程绘制。
        auto& self = const_cast<UIAuction&>(*this);
        auto& state = UIAuctionState::get();
        uint32_t seq = (view == 2) ? state.complete_seq : ((view == 1) ? state.quotes_seq : state.home_seq);
        if (seq != last_seq)
            self.rebuild_rows();

        if (rows.empty())
        {
            empty_hint.draw(position + Point<int16_t>(LIST_X + LIST_W / 2, LIST_Y + 40));
            return;
        }

        int16_t first = static_cast<int16_t>(scroll / ROW_H);
        int16_t visible = static_cast<int16_t>((dimension.y() - LIST_Y - 44) / ROW_H);
        for (int16_t i = first; i < first + visible && i < static_cast<int16_t>(rows.size()); ++i)
        {
            const Row& r = rows[static_cast<size_t>(i)];
            Point<int16_t> base = position + Point<int16_t>(LIST_X, LIST_Y + i * ROW_H - scroll);
            r.icon.draw(DrawArgument(base + Point<int16_t>(0, 4)));
            r.name.draw(base + Point<int16_t>(34, 2));
            r.detail.draw(base + Point<int16_t>(34, 16));
        }
    }

    int16_t UIAuction::row_at(Point<int16_t> cursorpos) const
    {
        Point<int16_t> local = cursorpos - position - Point<int16_t>(LIST_X, LIST_Y) + Point<int16_t>(0, scroll);
        if (local.x() < 0 || local.x() > LIST_W || local.y() < 0)
            return -1;
        int16_t row = static_cast<int16_t>(local.y() / ROW_H);
        if (row >= static_cast<int16_t>(rows.size()))
            return -1;
        return row;
    }

    void UIAuction::do_home_search()
    {
        // 简化：搜索框文字输入未接 Textfield（原生 combo:search 需 IMG 皮肤 + 焦点管理），
        // 这里发一条空关键词搜索包，服务端返回全量在售列表。
        AuctionActionPacket::search(40, 0, "", "").dispatch();
        set_status(u8"已请求首页列表（搜索词简化为空）");
        LOG(LOG_DEBUG, "[DBG-AUC] do_home_search");
    }

    void UIAuction::do_market_search()
    {
        AuctionActionPacket::search(41, 0, "", "").dispatch();
        set_status(u8"已请求行情列表（搜索词简化为空）");
        LOG(LOG_DEBUG, "[DBG-AUC] do_market_search");
    }

    Button::State UIAuction::button_pressed(uint16_t buttonid)
    {
        switch (buttonid)
        {
            case BtExit:
                deactivate();
                return Button::State::NORMAL;
            case BtSearch:
                if (view == 1) do_market_search();
                else do_home_search();
                return Button::State::NORMAL;
            case BtSell:
                view = 0;
                scroll = 0;
                rebuild_rows();
                // 在售页也重新进入一次，确保拿到最新可购买列表。
                AuctionActionPacket::enter().dispatch();
                set_status(u8"在售列表");
                return Button::State::NORMAL;
            case BtMarketPrice:
                view = 1;
                scroll = 0;
                rebuild_rows();
                do_market_search();
                return Button::State::NORMAL;
            case BtComplete:
                view = 2;
                scroll = 0;
                rebuild_rows();
                set_status(u8"完成列表：点行领取");
                return Button::State::NORMAL;
            case BtSlotUI:
                // 上架面板需拖拽道具 + 选价/时长，依赖完整 slotUI 节点与 SoftKey 注入，
                // 本版显式预留（不伪造价格），提示用户。
                set_status(u8"上架交互待接入：请在背包选中道具后走后续版本");
                LOG(LOG_DEBUG, "[DBG-AUC] BtSlotUI deferred");
                return Button::State::NORMAL;
            case BtHelp:
                set_status(u8"帮助：点列表购买/领取；Sell/行情/完成切页；Esc 关闭。");
                return Button::State::NORMAL;
        }
        return Button::State::DISABLED;
    }

    Cursor::State UIAuction::send_cursor(bool clicked, Point<int16_t> cursorpos)
    {
        if (clicked)
        {
            int16_t row = row_at(cursorpos);
            if (row >= 0)
            {
                const Row& r = rows[static_cast<size_t>(row)];
                if (view == 2)
                {
                    // 完成列表：按 status 区分领取报酬(30)/领取物品(31)
                    // 本版简化：统一走 31 领取物品；若状态表示已售则走 30。
                    auto& state = UIAuctionState::get();
                    int32_t st = 0;
                    for (const auto& e : state.complete_list)
                    {
                        if (e.invId == r.inv_id) { st = e.status; break; }
                    }
                    AuctionActionPacket::claim(static_cast<int32_t>(r.inv_id), st == 0).dispatch();
                    set_status(u8"已发送领取请求");
                }
                else
                {
                    AuctionActionPacket::buy(static_cast<int32_t>(r.inv_id), r.meso_or_bid).dispatch();
                    set_status(u8"已发送购买请求");
                }
                LOG(LOG_DEBUG, "[DBG-AUC] click row=" << row << " invId=" << r.inv_id);
            }
        }
        return UIElement::send_cursor(clicked, cursorpos);
    }

    void UIAuction::send_scroll(double yoffset)
    {
        scroll += static_cast<int16_t>(yoffset * ROW_H);
        int16_t maxscroll = std::max<int16_t>(
            0,
            static_cast<int16_t>(static_cast<int32_t>(rows.size()) * ROW_H) - (dimension.y() - LIST_Y - 44));
        if (scroll < 0) scroll = 0;
        if (scroll > maxscroll) scroll = maxscroll;
    }

    void UIAuction::send_key(int32_t keycode, bool pressed, bool escape)
    {
        if (pressed && escape)
        {
            deactivate();
            return;
        }
        if (!pressed)
            return;

        // 搜索框文本输入简化：不接 Textfield，键盘仅保留列表滚动方向键。
        // 原生 combo:search 的文本态需 IMG 皮肤 + 全局焦点管理，本版不伪造。
        // UI.cpp 对未绑定方向键把 GLFW 键码直接当 action 下传，这里只比较
        // KeyAction::Id::UP/DOWN（与 Keyboard.cpp 映射一致），不引入 GLFW 头。
        if (keycode == KeyAction::Id::UP || keycode == KeyAction::Id::DOWN)
            send_scroll(keycode == KeyAction::Id::UP ? -1.0 : 1.0);
    }

    void UIAuction::set_status(const std::string& text)
    {
        status.change_text(text);
    }

    UIElement::Type UIAuction::get_type() const
    {
        return TYPE;
    }
}
