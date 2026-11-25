////////////////////////////////////////////////////////////////////////////////
//  UIForce.cpp — 197 TMS280 神秘之力 / 奥术之力面板
//
//  视觉：本项目通用壳风格，两块面板文字/按钮坐标为通用壳占位（非像素级对齐），
//        视觉待精修；未做真机联调，仅保证编译与资源缺件降级可操作。
//  资源：神秘之力取 UIArcaneForce.img/ArcEquip，奥术之力取
//        UIAuthenticForce.img/AutEquip 的 backgrnd 作背景；
//        按钮从对应根下 BtClose / BtUpgradeArcane / BtUpgradeAuthentic 加载，
//        缺节点则不创建原生按钮 + diag_once，改用通用壳 MapleButton
//        （Basic.img::BtOK / close_btn）定位，注明通用壳。
//  数据：UIForceState 单例由 0x3781 handler 落地；draw 前比对 seq 重建文本行。
//  交互：BtClose 关窗、BtUpgradeArcane 发 mode1、BtUpgradeAuthentic 发 mode2；
//        Esc 关窗。查询(mode0)仅在打开时按约定由入口发包。
////////////////////////////////////////////////////////////////////////////////
#include "UIForce.h"
#include "UIForceState.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Net/Packets/ForcePackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#include <string>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
    namespace
    {
        // 通用壳默认尺寸：资源缺失时仍能显示文字 + 按钮，可操作。
        constexpr int16_t DEFAULT_W = 320;
        constexpr int16_t DEFAULT_H = 220;
        // 奥术面板在通用壳下叠在神秘面板下方的偏移。
        constexpr int16_t DEFAULT_AUT_OFFSET_Y = 110;
    }

    UIForce::UIForce()
        : title(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, u8"神秘 / 奥术之力"),
          arc_text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, u8"神秘之力 0 (Lv.0)"),
          aut_text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, u8"奥术之力 0 (Lv.0)"),
          status(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR,
                 u8"升级按钮发包 · Esc 关闭")
    {
        dimension = Point<int16_t>(DEFAULT_W, DEFAULT_H);

#ifdef USE_NX
        // 197 TMS280 资源已由 lead 导入 live Data/UI/UIArcaneForce.img（ArcEquip）
        // 与 UIAuthenticForce.img（AutEquip），均经转 key + resource-probe 验证。
        nl::node arc_root = nl::nx::UI["UIArcaneForce.img"];
        nl::node aut_root = nl::nx::UI["UIAuthenticForce.img"];
        nl::node arc = arc_root ? arc_root["ArcEquip"] : nl::node();
        nl::node aut = aut_root ? aut_root["AutEquip"] : nl::node();

        if (arc)
        {
            if (arc["backgrnd"])
            {
                arc_bg = arc["backgrnd"];
                has_arc_bg = true;
                dimension = arc_bg.get_dimensions();
            }
        }
        else
        {
            UiAssetResolve::diag_once("force.no_arc_img",
                "UIArcaneForce.img/ArcEquip missing; using generic shell");
        }

        if (aut)
        {
            if (aut["backgrnd"])
            {
                aut_bg = aut["backgrnd"];
                has_aut_bg = true;
                // 奥术面板叠在神秘面板下方；神秘背景缺失时用通用壳偏移兜底。
                aut_offset_y = has_arc_bg
                    ? arc_bg.get_dimensions().y()
                    : DEFAULT_AUT_OFFSET_Y;
            }
        }
        else
        {
            UiAssetResolve::diag_once("force.no_aut_img",
                "UIAuthenticForce.img/AutEquip missing; using generic shell");
        }

        // 通用壳关闭钮：优先面板自带 BtClose，缺则复用 close_btn。
        bool has_close = false;
        if (arc)
            has_close = try_maple_btn(buttons, BtClose, arc["BtClose"],
                                      Point<int16_t>(dimension.x() - 21, 6), "force BtClose");
        if (!has_close)
        {
            nl::node close = UiAssetResolve::close_btn();
            if (close)
            {
                buttons[BtClose] = std::make_unique<MapleButton>(
                    close, Point<int16_t>(dimension.x() - 21, 6));
                UiAssetResolve::diag_once("force.BtClose.generic",
                    "using generic close_btn shell for force BtClose");
            }
        }

        // 升级钮：优先面板自带按钮；缺节点则通用壳 Basic.img::BtOK 定位（注明通用壳）。
        nl::node basic = nl::nx::UI["Basic.img"];
        bool has_arc_up = false;
        bool has_aut_up = false;
        if (arc)
            has_arc_up = try_maple_btn(buttons, BtUpgradeArcane, arc["BtUpgradeArcane"],
                                       Point<int16_t>(16, 48), "force BtUpgradeArcane");
        if (aut)
            has_aut_up = try_maple_btn(buttons, BtUpgradeAuthentic, aut["BtUpgradeAuthentic"],
                                       Point<int16_t>(16, 48 + aut_offset_y), "force BtUpgradeAuthentic");
        if (!has_arc_up && basic && basic["BtOK"])
        {
            buttons[BtUpgradeArcane] = std::make_unique<MapleButton>(
                basic["BtOK"], Point<int16_t>(16, 48));
            UiAssetResolve::diag_once("force.BtUpgradeArcane.generic",
                "using generic Basic.img::BtOK shell for BtUpgradeArcane");
        }
        if (!has_aut_up && basic && basic["BtOK"])
        {
            buttons[BtUpgradeAuthentic] = std::make_unique<MapleButton>(
                basic["BtOK"], Point<int16_t>(16, 48 + aut_offset_y));
            UiAssetResolve::diag_once("force.BtUpgradeAuthentic.generic",
                "using generic Basic.img::BtOK shell for BtUpgradeAuthentic");
        }
#else
        (void)DEFAULT_AUT_OFFSET_Y;
#endif

        layout_policy = LayoutPolicy::Center;
        center_on_screen();

        // 打开即查询一次（mode0）：服务端回 0x3781 填充两力数值与等级。
        ForceActionPacket::query().dispatch();
        last_seq = 0;
    }

    void UIForce::rebuild_text()
    {
        const auto& s = UIForceState::get().data;
        arc_text.change_text(u8"神秘之力 " + std::to_string(s.arcaneForce)
                             + u8" (Lv." + std::to_string(s.arcaneLevel) + u8")");
        aut_text.change_text(u8"奥术之力 " + std::to_string(s.authenticForce)
                             + u8" (Lv." + std::to_string(s.authenticLevel) + u8")");
        last_seq = UIForceState::get().seq;
    }

    void UIForce::draw(float inter) const
    {
        if (has_arc_bg)
            arc_bg.draw(position, inter);
        if (has_aut_bg)
            aut_bg.draw(position + Point<int16_t>(0, aut_offset_y), inter);

        title.draw(position + Point<int16_t>(dimension.x() / 2, 8));
        status.draw(position + Point<int16_t>(12, dimension.y() - 20));

        // const 路径：与 UIAuction/UIVMatrix::draw 同口径，本线程绘制局部可控。
        auto& self = const_cast<UIForce&>(*this);
        if (UIForceState::get().seq != last_seq)
            self.rebuild_text();

        arc_text.draw(position + Point<int16_t>(16, 48));
        aut_text.draw(position + Point<int16_t>(16, 48 + aut_offset_y));

        UIElement::draw_buttons(inter);
    }

    Button::State UIForce::button_pressed(uint16_t buttonid)
    {
        switch (buttonid)
        {
            case BtClose:
                deactivate();
                return Button::State::NORMAL;
            case BtUpgradeArcane:
                ForceActionPacket::upgradeArcane().dispatch();
                set_status(u8"已发送升级神秘之力请求（mode=1）");
                return Button::State::NORMAL;
            case BtUpgradeAuthentic:
                ForceActionPacket::upgradeAuthentic().dispatch();
                set_status(u8"已发送升级奥术之力请求（mode=2）");
                return Button::State::NORMAL;
        }
        return Button::State::DISABLED;
    }

    void UIForce::send_key(int32_t keycode, bool pressed, bool escape)
    {
        (void)keycode;
        // 本窗只处理 Esc 关闭；不消费其它按键。
        if (pressed && escape)
            deactivate();
    }

    void UIForce::set_status(const std::string& text)
    {
        status.change_text(text);
    }

    UIElement::Type UIForce::get_type() const
    {
        return TYPE;
    }
}
