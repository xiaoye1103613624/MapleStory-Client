////////////////////////////////////////////////////////////////////////////////
//  UIFamiliar — 197 TMS280 精灵收藏（MonsterFamiliar）客户端 UI
//
//  资源：nl::nx::UI["UIFamiliar.img"]，取 book/backgrnd 作背景；按钮从 book 下
//        BtOut / BTBack / BtHome / BtFilter1..6 加载，缺件优雅降级 + diag_once。
//  协议：C→S FAMILIAR_ACTION 0x3760（Net/Packets/FamiliarPackets.h）；
//        S→C 由 UIFamiliarState（UIFamiliarState.h）落地，本类只负责渲染 + 交互。
//  视觉：本项目通用壳风格（注明待精修），列表面板不追求像素级对齐。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

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
    class UIFamiliar : public UIElement
    {
    public:
        static constexpr Type TYPE = UIElement::Type::FAMILIAR;
        static constexpr bool FOCUSED = false;
        static constexpr bool TOGGLED = true;

        UIFamiliar();

        void draw(float inter) const override;
        void send_key(int32_t keycode, bool pressed, bool escape) override;
        Button::State button_pressed(uint16_t buttonid) override;
        Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
        void send_scroll(double yoffset) override;
        UIElement::Type get_type() const override;

        // 状态栏提示（进入/召唤/升级反馈），由结果包或交互回填。
        void set_status(const std::string& text);

    private:
        enum Buttons : uint16_t
        {
            BtOut = 1100,       // 关闭窗口
            BtBack = 1101,      // 返回上一级（简化：切回默认视图）
            BtHome = 1102,      // 进入收藏（发 mode0 包）
            BtFilter1 = 1110,   // 筛选页签 1..6（切换列表视图）
            BtFilter2 = 1111,
            BtFilter3 = 1112,
            BtFilter4 = 1113,
            BtFilter5 = 1114,
            BtFilter6 = 1115,
        };

        // 当前列表视图：0=全部，1..6=对应筛选；筛选视图仅做客户端分组展示（见 cpp 说明）。
        uint8_t view = 0;
        int16_t scroll = 0;
        static constexpr int16_t ROW_H = 34;
        static constexpr int16_t LIST_X = 28;
        static constexpr int16_t LIST_Y = 96;
        static constexpr int16_t LIST_W = 380;
        static constexpr int16_t LIST_ROWS = 9;

        struct Row
        {
            Text name;          // 名字
            Text detail;        // 等级 / 品级 / 技能
            int32_t familiar_id = 0;
        };

        Sprite background;
        bool has_background = false;
        Text title;
        Text status;
        Text empty_hint;
        std::vector<Row> rows;      // 当前视图的渲染行（draw 前按 state.seq 重建）
        uint32_t last_seq = 0;      // 上次重建对应的 UIFamiliarState.list_seq

        void rebuild_rows();
        int16_t row_at(Point<int16_t> cursorpos) const;
        void send_enter();
    };
}
