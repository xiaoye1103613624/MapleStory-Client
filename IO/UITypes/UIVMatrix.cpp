////////////////////////////////////////////////////////////////////////////////
//  UIVMatrix.cpp — 197 TMS280 V 矩阵 / 核心面板（VMatrixUI.img/VMatrix）
//
//  视觉：通用壳风格（本项目 UI 惯例），列表不追求像素级对齐，视觉待精修；
//        未做真机联调，仅保证编译与资源缺件降级可操作。
//  资源：优先 VMatrixUI.img/VMatrix/backgrnd；按钮从 VMatrix 节点下
//        button:All / button:SKCore / button:ENCore / button:SPCore 加载，
//        缺节点则不创建 + diag_once 记录，不静默落 (0,0)。
//  数据：VCoreState 单例由 2 个 S→C handler 落地；本类 draw 前比对
//        list_seq 重建文本行，比对 result_seq 提示最近一次操作结果。
//  交互：单击行 = 装备(action0，usedOid=0/index=行 index，见 cpp 内注释)；
//        右键或 Shift+点击 = 分解(action5)；四个页签切换本地筛选视图；
//        Esc 关窗。卸下/强化包由工厂提供，本简化 UI 未绑定输入控件。
////////////////////////////////////////////////////////////////////////////////
#include "UIVMatrix.h"
#include "VCoreState.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Net/Packets/VCorePackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"
#include "../../Util/Locale.h"

#include <algorithm>
#include <SDL3/SDL.h>

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

        // 本 UI 的 Shift 判定用 SDL 修饰键态（与 send_cursor(bool,pos) 同帧读取）。
        // rightclick() 由 UIStateGame::rightclick 派发，无需修饰键。
        bool shift_held()
        {
            return (SDL_GetModState() & SDL_KMOD_SHIFT) != 0;
        }
    }

    UIVMatrix::UIVMatrix()
        : title(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, u8"V 矩阵"),
          status(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR,
                 u8"点行装备 · 右键/Shift+点击分解 · Esc 关闭"),
          empty_hint(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE, u8"暂无核心")
    {
        dimension = Point<int16_t>(DEFAULT_W, DEFAULT_H);

#ifdef USE_NX
        // 197 TMS280 资源已由 lead 导入 live Data/UI/VMatrixUI.img
        // （10.7MB，4 顶层节点：VMatrix / VMatrix_Craft / VMatrix_Mycore /
        //  VMatrix_Corestone；VMatrix 下 44 子节点含 backgrnd 与四个页签按钮）。
        nl::node vmroot = nl::nx::UI["VMatrixUI.img"];
        nl::node main = vmroot ? vmroot["VMatrix"] : nl::node();
        if (main)
        {
            nl::node panel = main["backgrnd"];
            if (panel)
            {
                background = panel;
                has_background = true;
                dimension = background.get_dimensions();
            }

            // 原生按钮带 normal/pressed/disabled/mouseOver 四态，MapleButton 自动读取。
            try_maple_btn(buttons, BtAll, main["button:All"],
                          Point<int16_t>(24, 36), "vmatrix button:All");
            try_maple_btn(buttons, BtSKCore, main["button:SKCore"],
                          Point<int16_t>(96, 36), "vmatrix button:SKCore");
            try_maple_btn(buttons, BtENCore, main["button:ENCore"],
                          Point<int16_t>(168, 36), "vmatrix button:ENCore");
            try_maple_btn(buttons, BtSPCore, main["button:SPCore"],
                          Point<int16_t>(240, 36), "vmatrix button:SPCore");
        }
        else
        {
            UiAssetResolve::diag_once("vmatrix.no_img",
                "VMatrixUI.img/VMatrix missing; using generic shell");
        }
#endif

        // 入口打开时居中；分辨率变化保持居中。
        layout_policy = LayoutPolicy::Center;
        center_on_screen();

        last_seq = 0;
        last_result_seq = VCoreState::get().result_seq;
    }

    void UIVMatrix::rebuild_rows()
    {
        rows.clear();
        scroll = 0;

        const auto& state = VCoreState::get();
        for (const auto& e : state.list)
        {
            // 【本地筛选视图，服务端协议无筛选字段】0x3771 没有核心类型字段，
            // 这里用 skill1/skill2/skill3 是否非 0 作为 SK/EN/SP 的占位分组，
            // 属于客户端视图简化，非服务端真实分类，待后续协议/资源校准。
            if (view == 1 && e.skill1 == 0) continue;
            if (view == 2 && e.skill2 == 0) continue;
            if (view == 3 && e.skill3 == 0) continue;

            Row r;
            r.vcoreid = e.vcoreid;
            r.index = e.index;
            r.slot = e.slot;
            r.info = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
                          u8"核心 " + std::to_string(e.vcoreid)
                          + u8"  Lv." + std::to_string(static_cast<int>(e.level))
                          + u8"  Slot " + std::to_string(static_cast<int>(e.slot))
                          + u8"  Idx " + std::to_string(e.index)
                          + u8"  [S" + std::to_string(e.skill1)
                          + u8"/" + std::to_string(e.skill2)
                          + u8"/" + std::to_string(e.skill3) + u8"]");
            rows.push_back(std::move(r));
        }
        last_seq = state.list_seq;
    }

    void UIVMatrix::refresh_status_from_result()
    {
        const auto& state = VCoreState::get();
        if (state.result_seq == last_result_seq)
            return;
        last_result_seq = state.result_seq;

        const char* act = "操作";
        switch (state.last_action)
        {
            case 0: act = "装备"; break;
            case 1: act = "卸下"; break;
            case 4: act = "强化"; break;
            case 5: act = "分解"; break;
            default: break;
        }
        set_status(std::string(act) + u8" 结果 code=" + std::to_string(state.last_code)
                   + u8" vcoreid=" + std::to_string(state.last_vcoreid));
    }

    void UIVMatrix::draw(float inter) const
    {
        if (has_background)
            background.draw(position, inter);

        title.draw(position + Point<int16_t>(dimension.x() / 2, 8));
        status.draw(position + Point<int16_t>(12, dimension.y() - 34));

        // const 路径：与 UIAuction/UIFamiliar::draw 同口径，本线程绘制局部可控。
        auto& self = const_cast<UIVMatrix&>(*this);
        const auto& state = VCoreState::get();
        if (state.list_seq != last_seq)
            self.rebuild_rows();
        self.refresh_status_from_result();

        if (rows.empty())
        {
            empty_hint.draw(position + Point<int16_t>(LIST_X + LIST_W / 2, LIST_Y + 40));
            UIElement::draw_buttons(inter);
            return;
        }

        const int16_t first = static_cast<int16_t>(scroll / ROW_H);
        const int16_t visible = static_cast<int16_t>((dimension.y() - LIST_Y - 44) / ROW_H);
        for (int16_t i = first; i < first + visible && i < static_cast<int16_t>(rows.size()); ++i)
        {
            const Row& r = rows[static_cast<size_t>(i)];
            const Point<int16_t> base = position
                + Point<int16_t>(LIST_X, LIST_Y + i * ROW_H - scroll);
            r.info.draw(base + Point<int16_t>(0, 2));
        }

        UIElement::draw_buttons(inter);
    }

    int16_t UIVMatrix::row_at(Point<int16_t> cursorpos) const
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

    void UIVMatrix::activate(int32_t vcoreid, int32_t index)
    {
        // 单击 = 装备(action0)。服务端布局为 vcoreOid + usedOid + 保留0 + index。
        // 【简化】0x3771 列表不含 usedOid（被替换核心 oid），且无拖拽选槽交互，
        // 故这里 usedOid 固定填 0（空槽装备）、index 用列表回报的 index；
        // 真机上若需“替换已装核心”须另接 usedOid 输入（待联调）。
        VCoreActionPacket::equip(vcoreid, 0, index).dispatch();
        set_status(u8"已发送装备请求（usedOid=0 简化）");
        LOG(LOG_DEBUG, "[DBG-VCORE] equip vcoreid=" << vcoreid << " index=" << index);
    }

    void UIVMatrix::decompose(int32_t vcoreid)
    {
        // 右键 / Shift+点击 = 分解(action5)，服务端仅读 int vcoreOid。
        VCoreActionPacket::decompose(vcoreid).dispatch();
        set_status(u8"已发送分解请求");
        LOG(LOG_DEBUG, "[DBG-VCORE] decompose vcoreid=" << vcoreid);
    }

    Button::State UIVMatrix::button_pressed(uint16_t buttonid)
    {
        switch (buttonid)
        {
            case BtAll:
            case BtSKCore:
            case BtENCore:
            case BtSPCore:
                view = static_cast<uint8_t>(buttonid - BtAll);
                scroll = 0;
                rebuild_rows();
                set_status(u8"筛选视图 " + std::to_string(view)
                           + u8"（本地视图，服务端筛选预留）");
                return Button::State::NORMAL;
        }
        return Button::State::DISABLED;
    }

    Cursor::State UIVMatrix::send_cursor(bool clicked, Point<int16_t> cursorpos)
    {
        if (clicked)
        {
            const int16_t row = row_at(cursorpos);
            if (row >= 0)
            {
                const Row& r = rows[static_cast<size_t>(row)];
                if (shift_held())
                    decompose(r.vcoreid);
                else
                    activate(r.vcoreid, r.index);
            }
        }
        return UIElement::send_cursor(clicked, cursorpos);
    }

    void UIVMatrix::rightclick(Point<int16_t> cursorpos)
    {
        const int16_t row = row_at(cursorpos);
        if (row >= 0)
            decompose(rows[static_cast<size_t>(row)].vcoreid);
    }

    void UIVMatrix::send_scroll(double yoffset)
    {
        scroll += static_cast<int16_t>(yoffset * ROW_H);
        const int16_t maxscroll = std::max<int16_t>(
            0,
            static_cast<int16_t>(static_cast<int32_t>(rows.size()) * ROW_H)
                - (dimension.y() - LIST_Y - 44));
        if (scroll < 0) scroll = 0;
        if (scroll > maxscroll) scroll = maxscroll;
    }

    void UIVMatrix::send_key(int32_t keycode, bool pressed, bool escape)
    {
        (void)keycode;
        // 本窗只处理 Esc 关闭；不消费其它按键。
        if (pressed && escape)
            deactivate();
    }

    void UIVMatrix::set_status(const std::string& text)
    {
        status.change_text(text);
    }

    UIElement::Type UIVMatrix::get_type() const
    {
        return TYPE;
    }
}
