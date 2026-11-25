////////////////////////////////////////////////////////////////////////////////
//  UIForce — 197 TMS280 神秘之力 / 奥术之力客户端 UI
//
//  资源：nl::nx::UI["UIArcaneForce.img"]["ArcEquip"]（神秘之力面板）
//        nl::nx::UI["UIAuthenticForce.img"]["AutEquip"]（奥术之力面板）
//        两块背景 + 四态按钮由 MapleButton 读取；判空降级 + diag_once，
//        缺件则用通用壳按钮（Basic.img::BtOK / close_btn）保证可操作。
//  协议：C→S FORCE_ACTION 0x3780（Net/Packets/ForcePackets.h，int mode）；
//        S→C FORCE_INFO 0x3781 由 UIForceState 落地，本类只渲染 + 交互。
//  视觉：本项目通用壳风格（注明待精修），坐标非像素级对齐，未真机联调。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include <cstdint>
#include <string>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
    class UIForce : public UIElement
    {
    public:
        static constexpr Type TYPE = UIElement::Type::FORCE;
        static constexpr bool FOCUSED = false;
        static constexpr bool TOGGLED = true;

        UIForce();

        void draw(float inter) const override;
        void send_key(int32_t keycode, bool pressed, bool escape) override;
        Button::State button_pressed(uint16_t buttonid) override;
        UIElement::Type get_type() const override;

        // 状态栏提示（发包反馈），由交互回填。
        void set_status(const std::string& text);

    private:
        enum Buttons : uint16_t
        {
            BtClose = 1400,
            BtUpgradeArcane = 1401,   // 升级神秘之力（mode=1）
            BtUpgradeAuthentic = 1402 // 升级奥术之力（mode=2）
        };

        Sprite arc_bg;
        Sprite aut_bg;
        bool has_arc_bg = false;
        bool has_aut_bg = false;
        int16_t aut_offset_y = 0;     // 奥术面板相对 position 的 y 偏移

        Text title;
        Text arc_text;                // "神秘之力 N (Lv.M)"
        Text aut_text;                // "奥术之力 N (Lv.K)"
        Text status;
        uint32_t last_seq = 0;        // 上次重建文本对应的 UIForceState.seq

        void rebuild_text();
    };
}
