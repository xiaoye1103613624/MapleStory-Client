////////////////////////////////////////////////////////////////////////////////
//  UIGuildSkill.cpp — 197 TMS280 公会技能面板
//
//  视觉：通用壳风格（本项目 UI 惯例），列表不追求像素级对齐，视觉待精修；
//        未做真机联调，仅保证编译与资源缺件降级可操作。
//  资源：优先 GuildUI2.img/guild/main/pages/guildSkill/backgrnd；
//        升级钮优先该节点下 button:GuildBattleSkillLevelUp；
//        购买钮优先 GuildUI.img/guildSkill/button:GuildBattleSkill。
//        缺节点则不创建原生按钮，改用 Basic.img::BtOK / close_btn 通用壳
//        + diag_once 记录，不静默落 (0,0)（节点结构复杂时同样降级通用壳）。
//  数据：GuildSkillState 单例由 2 个 S→C handler 落地；本类 draw 前比对
//        list_seq 重建文本行，比对 result_seq 提示最近一次操作结果。
//  交互：点行选中技能 id；BtBuy 发 mode1、BtLevelUp 发 mode2（带选中 id）；
//        BtClose / Esc 关窗；打开时发 mode0 查询。
////////////////////////////////////////////////////////////////////////////////
#include "UIGuildSkill.h"
#include "GuildSkillState.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Net/Packets/GuildSkillPackets.h"
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

        const char* result_code_text(int32_t code)
        {
            switch (code)
            {
                case 0: return u8"成功";
                case 1: return u8"权限不足";
                case 2: return u8"积分不足";
                case 3: return u8"已满级";
                default: return u8"未知结果";
            }
        }
    }

    UIGuildSkill::UIGuildSkill()
        : title(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, u8"公会技能"),
          status(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR,
                 u8"点行选中技能 · 购买/升级 · Esc 关闭"),
          empty_hint(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE, u8"暂无公会技能")
    {
        dimension = Point<int16_t>(DEFAULT_W, DEFAULT_H);

#ifdef USE_NX
        // 197 TMS280 资源已由 lead 导入 live Data/UI：
        // GuildUI2.img/guild/main/pages/guildSkill（含 button:GuildBattleSkillLevelUp）
        // GuildUI.img/guildSkill/button:GuildBattleSkill。
        // 节点结构复杂或缺失时按通用壳降级 + diag_once。
        nl::node g2 = nl::nx::UI["GuildUI2.img"];
        nl::node guild = g2 ? g2["guild"] : nl::node();
        nl::node main = guild ? guild["main"] : nl::node();
        nl::node pages = main ? main["pages"] : nl::node();
        nl::node skillroot = pages ? pages["guildSkill"] : nl::node();

        if (skillroot)
        {
            nl::node panel = skillroot["backgrnd"];
            if (panel)
            {
                background = panel;
                has_background = true;
                dimension = background.get_dimensions();
            }
        }
        else
        {
            UiAssetResolve::diag_once("guildskill.no_guildui2_node",
                "GuildUI2.img/guild/main/pages/guildSkill missing; using generic shell");
        }

        // 通用壳按钮节点：Basic.img::BtOK（升级/购买），close_btn（关闭）。
        nl::node basic = nl::nx::UI["Basic.img"];

        // BtClose：优先面板 BtClose，缺则复用 close_btn 通用壳。
        bool has_close = false;
        if (skillroot)
            has_close = try_maple_btn(buttons, BtClose, skillroot["BtClose"],
                                      Point<int16_t>(dimension.x() - 21, 6), "guildskill BtClose");
        if (!has_close)
        {
            nl::node close = UiAssetResolve::close_btn();
            if (close)
            {
                buttons[BtClose] = std::make_unique<MapleButton>(
                    close, Point<int16_t>(dimension.x() - 21, 6));
                UiAssetResolve::diag_once("guildskill.BtClose.generic",
                    "using generic close_btn shell for guildskill BtClose");
            }
        }

        // BtLevelUp：优先原生 button:GuildBattleSkillLevelUp，缺则通用壳。
        bool has_levelup = false;
        if (skillroot)
            has_levelup = try_maple_btn(buttons, BtLevelUp,
                                        skillroot["button:GuildBattleSkillLevelUp"],
                                        Point<int16_t>(dimension.x() - 132, 36),
                                        "guildskill BtLevelUp");
        if (!has_levelup && basic && basic["BtOK"])
        {
            buttons[BtLevelUp] = std::make_unique<MapleButton>(
                basic["BtOK"], Point<int16_t>(dimension.x() - 132, 36));
            UiAssetResolve::diag_once("guildskill.BtLevelUp.generic",
                "button:GuildBattleSkillLevelUp missing; using generic Basic.img::BtOK shell");
        }

        // BtBuy：优先 GuildUI.img/guildSkill/button:GuildBattleSkill，缺则通用壳。
        bool has_buy = false;
        nl::node g1 = nl::nx::UI["GuildUI.img"];
        nl::node g1skill = g1 ? g1["guildSkill"] : nl::node();
        if (g1skill)
            has_buy = try_maple_btn(buttons, BtBuy, g1skill["button:GuildBattleSkill"],
                                    Point<int16_t>(16, 36), "guildskill BtBuy");
        if (!has_buy && basic && basic["BtOK"])
        {
            buttons[BtBuy] = std::make_unique<MapleButton>(
                basic["BtOK"], Point<int16_t>(16, 36));
            UiAssetResolve::diag_once("guildskill.BtBuy.generic",
                "GuildUI.img/guildSkill/button:GuildBattleSkill missing; using generic Basic.img::BtOK shell");
        }
#endif

        // 入口打开时居中；分辨率变化保持居中。
        layout_policy = LayoutPolicy::Center;
        center_on_screen();

        // 打开即查询一次（mode0）：服务端回 0x3791 填充技能列表。
        GuildSkillActionPacket::query().dispatch();
        last_seq = 0;
        last_result_seq = GuildSkillState::get().result_seq;
    }

    void UIGuildSkill::rebuild_rows()
    {
        rows.clear();
        scroll = 0;

        const auto& state = GuildSkillState::get();
        for (const auto& e : state.list)
        {
            Row r;
            r.skill_id = e.skillId;
            r.info = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
                          u8"技能 " + std::to_string(e.skillId)
                          + u8"  Lv." + std::to_string(e.level)
                          + u8"  时间 " + std::to_string(e.timestamp)
                          + u8"  购买人 " + e.purchaser);
            rows.push_back(std::move(r));
        }
        last_seq = state.list_seq;
    }

    void UIGuildSkill::refresh_status_from_result()
    {
        const auto& state = GuildSkillState::get();
        if (state.result_seq == last_result_seq)
            return;
        last_result_seq = state.result_seq;

        const char* act = u8"操作";
        switch (state.last_mode)
        {
            case 1: act = u8"购买"; break;
            case 2: act = u8"升级"; break;
            default: break;
        }
        set_status(std::string(act) + u8" " + result_code_text(state.last_code)
                   + u8" (skillId=" + std::to_string(state.last_skill_id) + u8")");
    }

    void UIGuildSkill::draw(float inter) const
    {
        if (has_background)
            background.draw(position, inter);

        title.draw(position + Point<int16_t>(dimension.x() / 2, 8));
        status.draw(position + Point<int16_t>(12, dimension.y() - 34));

        // const 路径：与 UIAuction/UIFamiliar/UIVMatrix::draw 同口径，本线程绘制局部可控。
        auto& self = const_cast<UIGuildSkill&>(*this);
        const auto& state = GuildSkillState::get();
        if (state.list_seq != last_seq)
            self.rebuild_rows();
        self.refresh_status_from_result();

        if (rows.empty())
        {
            empty_hint.draw(position + Point<int16_t>(LIST_X + LIST_W / 2, LIST_Y + 40));
        }
        else
        {
            const int16_t first = static_cast<int16_t>(scroll / ROW_H);
            const int16_t visible = static_cast<int16_t>((dimension.y() - LIST_Y - 44) / ROW_H);
            for (int16_t i = first; i < first + visible && i < static_cast<int16_t>(rows.size()); ++i)
            {
                const Row& r = rows[static_cast<size_t>(i)];
                const Point<int16_t> base = position
                    + Point<int16_t>(LIST_X, LIST_Y + i * ROW_H - scroll);
                r.info.draw(base + Point<int16_t>(0, 2));
            }
        }

        UIElement::draw_buttons(inter);
    }

    int16_t UIGuildSkill::row_at(Point<int16_t> cursorpos) const
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

    Button::State UIGuildSkill::button_pressed(uint16_t buttonid)
    {
        switch (buttonid)
        {
            case BtClose:
                deactivate();
                return Button::State::NORMAL;
            case BtBuy:
                if (selected_skill_id == 0)
                    set_status(u8"请先点行选中技能");
                else
                {
                    GuildSkillActionPacket::buy(selected_skill_id).dispatch();
                    set_status(u8"已发送购买请求");
                }
                return Button::State::NORMAL;
            case BtLevelUp:
                if (selected_skill_id == 0)
                    set_status(u8"请先点行选中技能");
                else
                {
                    GuildSkillActionPacket::level_up(selected_skill_id).dispatch();
                    set_status(u8"已发送升级请求");
                }
                return Button::State::NORMAL;
        }
        return Button::State::DISABLED;
    }

    Cursor::State UIGuildSkill::send_cursor(bool clicked, Point<int16_t> cursorpos)
    {
        if (clicked)
        {
            const int16_t row = row_at(cursorpos);
            if (row >= 0)
            {
                selected_skill_id = rows[static_cast<size_t>(row)].skill_id;
                set_status(u8"已选中 skillId=" + std::to_string(selected_skill_id));
                LOG(LOG_DEBUG, "[DBG-GSKILL] click row=" << row
                    << " skillId=" << selected_skill_id);
            }
        }
        return UIElement::send_cursor(clicked, cursorpos);
    }

    void UIGuildSkill::send_scroll(double yoffset)
    {
        scroll += static_cast<int16_t>(yoffset * ROW_H);
        const int16_t maxscroll = std::max<int16_t>(
            0,
            static_cast<int16_t>(static_cast<int32_t>(rows.size()) * ROW_H)
                - (dimension.y() - LIST_Y - 44));
        if (scroll < 0) scroll = 0;
        if (scroll > maxscroll) scroll = maxscroll;
    }

    void UIGuildSkill::send_key(int32_t keycode, bool pressed, bool escape)
    {
        // 本窗只处理 Esc 关闭；其余按键不消费（方向键滚动走 send_scroll）。
        (void)keycode;
        if (pressed && escape)
        {
            deactivate();
            return;
        }
    }

    void UIGuildSkill::set_status(const std::string& text)
    {
        status.change_text(text);
    }

    UIElement::Type UIGuildSkill::get_type() const
    {
        return TYPE;
    }
}
