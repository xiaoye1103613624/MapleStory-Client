////////////////////////////////////////////////////////////////////////////////
//  UIFamiliar.cpp — 197 TMS280 精灵收藏（MonsterFamiliar）面板
//
//  视觉：通用壳风格（本项目 UI 惯例），列表不追求像素级对齐，视觉待精修。
//  资源：优先 UIFamiliar.img/book/backgrnd；按钮从 book 下同名节点加载，
//        缺节点则不创建对应按钮 + diag_once 记录，不静默落 (0,0)。
//  数据：UIFamiliarState 单例由 2 个 S→C handler 落地；本类 draw 前比对
//        list_seq 重建文本行。
//  交互：点列表行发召唤切换(mode1)；BtOut 关窗、BtHome 发进入包(mode0)、
//        BtFilter1..6 切视图、BtBack 回全部；Esc 关窗。
////////////////////////////////////////////////////////////////////////////////
#include "UIFamiliar.h"
#include "UIFamiliarState.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Net/Packets/FamiliarPackets.h"
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
        constexpr int16_t DEFAULT_W = 440;
        constexpr int16_t DEFAULT_H = 520;
    }

    UIFamiliar::UIFamiliar()
        : title(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, u8"精灵收藏"),
          status(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR,
                 u8"点条目召唤/取消 · BtHome 进入 · Esc 关闭"),
          empty_hint(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE, u8"暂无精灵")
    {
        dimension = Point<int16_t>(DEFAULT_W, DEFAULT_H);

#ifdef USE_NX
        // 197 TMS280 资源已由 lead 导入 live Data/UI/UIFamiliar.img（--resource-probe
        // 验证 children=7：book/jewel/familiarCard/reinforce/evolve/boost/cube）。
        // 直接按 AGENTS 允许的方式索引 nl::nx::UI，并判空优雅降级。
        nl::node famroot = nl::nx::UI["UIFamiliar.img"];
        if (famroot)
        {
            nl::node book = famroot["book"];
            nl::node panel = book ? book["backgrnd"] : nl::node();
            if (panel)
            {
                background = panel;
                has_background = true;
                dimension = background.get_dimensions();
            }

            if (book)
            {
                // 按钮名沿用原生 book 下节点；try_maple_btn 内部判空 + diag_once。
                try_maple_btn(buttons, BtOut, book["BtOut"],
                              Point<int16_t>(dimension.x() - 24, 8), "familiar BtOut");
                try_maple_btn(buttons, BtBack, book["BTBack"],
                              Point<int16_t>(24, 48), "familiar BTBack");
                try_maple_btn(buttons, BtHome, book["BtHome"],
                              Point<int16_t>(dimension.x() - 96, 48), "familiar BtHome");
                // BtFilter1..6：横向排布于列表上方，缺件不创建。
                for (uint16_t i = 0; i < 6; ++i)
                {
                    const uint16_t id = static_cast<uint16_t>(BtFilter1 + i);
                    const std::string node = "BtFilter" + std::to_string(i + 1);
                    try_maple_btn(buttons, id, book[node],
                                  Point<int16_t>(24 + static_cast<int16_t>(i * 56), 78),
                                  "familiar " + node);
                }
            }
        }
        else
        {
            UiAssetResolve::diag_once("familiar.no_img", "UIFamiliar.img missing; using generic shell");
        }
#endif

        // 入口打开时居中；分辨率变化保持居中。
        layout_policy = LayoutPolicy::Center;
        center_on_screen();

        // 进入收藏：服务端回 0x3761 列表。与 UIAuction ctor 同口径。
        send_enter();
        last_seq = 0;
    }

    void UIFamiliar::send_enter()
    {
        FamiliarActionPacket::enter().dispatch();
    }

    void UIFamiliar::rebuild_rows()
    {
        rows.clear();
        scroll = 0;

        const auto& state = UIFamiliarState::get();
        for (const auto& e : state.list)
        {
            // 视图 0 = 全部；1..6 为客户端筛选页签，按 grade 简单分组显示。
            // 注：服务端协议无筛选字段，筛选仅本地视图切换（待精修/服务端筛选预留）。
            if (view != 0 && static_cast<uint8_t>(e.grade + 1) != view)
                continue;

            Row r;
            r.familiar_id = e.familiarId;
            // 出战标记用 ASCII "*"，避免 GBK 客户端上特殊符号乱码。
            r.name = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
                          (e.summon ? std::string("* ") : std::string()) + e.name);
            r.detail = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR,
                            u8"Lv." + std::to_string(e.level)
                            + u8"  品级" + std::to_string(e.grade)
                            + u8"  技能" + std::to_string(e.skill)
                            + u8"  Exp" + std::to_string(e.exp));
            rows.push_back(std::move(r));
        }
        last_seq = state.list_seq;
    }

    void UIFamiliar::draw(float inter) const
    {
        if (has_background)
            background.draw(position, inter);

        title.draw(position + Point<int16_t>(dimension.x() / 2, 8));
        status.draw(position + Point<int16_t>(12, dimension.y() - 34));

        // const 路径：用 mutable 缓存文本行（与 UIAuction::draw 同口径）。
        auto& self = const_cast<UIFamiliar&>(*this);
        const auto& state = UIFamiliarState::get();
        if (state.list_seq != last_seq)
            self.rebuild_rows();

        if (rows.empty())
        {
            empty_hint.draw(position + Point<int16_t>(LIST_X + LIST_W / 2, LIST_Y + 40));
            return;
        }

        const int16_t first = static_cast<int16_t>(scroll / ROW_H);
        const int16_t visible = static_cast<int16_t>(
            (dimension.y() - LIST_Y - 44) / ROW_H);
        for (int16_t i = first; i < first + visible && i < static_cast<int16_t>(rows.size()); ++i)
        {
            const Row& r = rows[static_cast<size_t>(i)];
            const Point<int16_t> base = position
                + Point<int16_t>(LIST_X, LIST_Y + i * ROW_H - scroll);
            r.name.draw(base + Point<int16_t>(0, 2));
            r.detail.draw(base + Point<int16_t>(0, 18));
        }
    }

    int16_t UIFamiliar::row_at(Point<int16_t> cursorpos) const
    {
        const Point<int16_t> local = cursorpos - position
            - Point<int16_t>(LIST_X, LIST_Y) + Point<int16_t>(0, scroll);
        if (local.x() < 0 || local.x() > LIST_W || local.y() < 0)
            return -1;
        const int16_t row = static_cast<int16_t>(local.y() / ROW_H);
        if (row >= static_cast<int16_t>(rows.size()))
            return -1;
        return row;
    }

    Button::State UIFamiliar::button_pressed(uint16_t buttonid)
    {
        switch (buttonid)
        {
            case BtOut:
                deactivate();
                return Button::State::NORMAL;
            case BtHome:
                send_enter();
                set_status(u8"已请求精灵列表");
                return Button::State::NORMAL;
            case BtBack:
                view = 0;
                scroll = 0;
                rebuild_rows();
                set_status(u8"全部精灵");
                return Button::State::NORMAL;
            case BtFilter1:
            case BtFilter2:
            case BtFilter3:
            case BtFilter4:
            case BtFilter5:
            case BtFilter6:
                view = static_cast<uint8_t>(buttonid - BtFilter1 + 1);
                scroll = 0;
                rebuild_rows();
                set_status(u8"筛选视图 " + std::to_string(view) + u8"（本地视图，服务端筛预留）");
                return Button::State::NORMAL;
        }
        return Button::State::DISABLED;
    }

    Cursor::State UIFamiliar::send_cursor(bool clicked, Point<int16_t> cursorpos)
    {
        if (clicked)
        {
            const int16_t row = row_at(cursorpos);
            if (row >= 0)
            {
                const Row& r = rows[static_cast<size_t>(row)];
                // 点条目发召唤切换(mode1)：familiarId。
                FamiliarActionPacket::summon(r.familiar_id).dispatch();
                set_status(u8"已发送召唤切换请求");
                LOG(LOG_DEBUG, "[DBG-FAM] click row=" << row
                    << " familiarId=" << r.familiar_id);
            }
        }
        return UIElement::send_cursor(clicked, cursorpos);
    }

    void UIFamiliar::send_scroll(double yoffset)
    {
        scroll += static_cast<int16_t>(yoffset * ROW_H);
        const int16_t maxscroll = std::max<int16_t>(
            0,
            static_cast<int16_t>(static_cast<int32_t>(rows.size()) * ROW_H)
                - (dimension.y() - LIST_Y - 44));
        if (scroll < 0) scroll = 0;
        if (scroll > maxscroll) scroll = maxscroll;
    }

    void UIFamiliar::send_key(int32_t keycode, bool pressed, bool escape)
    {
        // 本窗只处理 Esc 关闭；其余按键不消费（方向键滚动走 send_scroll）。
        (void)keycode;
        if (pressed && escape)
        {
            deactivate();
            return;
        }
    }

    void UIFamiliar::set_status(const std::string& text)
    {
        status.change_text(text);
    }

    UIElement::Type UIFamiliar::get_type() const
    {
        return TYPE;
    }
}
