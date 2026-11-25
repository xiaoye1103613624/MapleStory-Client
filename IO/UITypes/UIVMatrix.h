////////////////////////////////////////////////////////////////////////////////
//  UIVMatrix — 197 TMS280 V 矩阵 / 核心（VMatrixUI.img）客户端 UI
//
//  资源：nl::nx::UI["VMatrixUI.img"]["VMatrix"]，取 backgrnd 作背景；
//        button:All / button:SKCore / button:ENCore / button:SPCore 作页签按钮
//        （四态 normal/pressed/disabled/mouseOver 由 MapleButton 自动读取，
//        缺节点则不创建 + diag_once 降级，不静默落 (0,0)）。
//  协议：C→S VCORE_ACTION 0x3770（Net/Packets/VCorePackets.h，字段按服务端
//        VCoreHandler.java 真实布局）；S→C 由 VCoreState 落地，本类只渲染 + 交互。
//  视觉：本项目通用壳风格（注明待精修），列表不追求像素级对齐，未真机联调。
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
    class UIVMatrix : public UIElement
    {
    public:
        static constexpr Type TYPE = UIElement::Type::VMATRIX;
        static constexpr bool FOCUSED = false;
        static constexpr bool TOGGLED = true;

        UIVMatrix();

        void draw(float inter) const override;
        void send_key(int32_t keycode, bool pressed, bool escape) override;
        Button::State button_pressed(uint16_t buttonid) override;
        Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
        void rightclick(Point<int16_t> cursorpos) override;
        void send_scroll(double yoffset) override;
        UIElement::Type get_type() const override;

        // 状态栏提示（发包/结果反馈），由交互回填。
        void set_status(const std::string& text);

    private:
        enum Buttons : uint16_t
        {
            BtAll = 1200,       // 全部
            BtSKCore = 1201,    // 技能核心（本地筛选视图 1）
            BtENCore = 1202,    // 强化核心（本地筛选视图 2）
            BtSPCore = 1203,    // 特殊核心（本地筛选视图 3）
        };

        // 当前列表视图：0=全部，1=SK，2=EN，3=SP。
        // 协议无筛选字段，筛选仅为客户端分组展示（见 cpp 说明，服务端筛选预留）。
        uint8_t view = 0;
        int16_t scroll = 0;
        static constexpr int16_t ROW_H = 34;
        static constexpr int16_t LIST_X = 28;
        static constexpr int16_t LIST_Y = 72;
        static constexpr int16_t LIST_W = 380;

        struct Row
        {
            Text info;          // vcoreid / skill1-3 / level / slot
            int32_t vcoreid = 0;
            int32_t index = 0;
            uint8_t slot = 0;
        };

        Sprite background;
        bool has_background = false;
        Text title;
        Text status;
        Text empty_hint;
        std::vector<Row> rows;      // 当前视图的渲染行（draw 前按 state.list_seq 重建）
        uint32_t last_seq = 0;      // 上次重建对应的 VCoreState.list_seq
        uint32_t last_result_seq = 0;

        void rebuild_rows();
        void refresh_status_from_result();
        int16_t row_at(Point<int16_t> cursorpos) const;
        void activate(int32_t vcoreid, int32_t index);   // 单击 = 装备(action0)
        void decompose(int32_t vcoreid);                 // 右键 / Shift+点击 = 分解(action5)
    };
}
