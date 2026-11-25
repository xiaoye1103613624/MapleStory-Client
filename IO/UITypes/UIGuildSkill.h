////////////////////////////////////////////////////////////////////////////////
//  UIGuildSkill — 197 TMS280 公会技能客户端 UI
//
//  资源：nl::nx::UI["GuildUI2.img"]["guild"]["main"]["pages"]["guildSkill"]
//        取 backgrnd 作背景；button:GuildBattleSkillLevelUp 作升级钮；
//        购买钮取 nl::nx::UI["GuildUI.img"]["guildSkill"]["button:GuildBattleSkill"]。
//        缺节点则不创建 + diag_once 降级为通用壳（不静默落 (0,0)）。
//  协议：C→S GUILD_SKILL_ACTION 0x3790（Net/Packets/GuildSkillPackets.h）：
//        int mode(0查询/1购买/2升级) +（mode=1/2 时）int skillId；
//        S→C 由 GuildSkillState（GuildSkillState.h）落地，本类只负责渲染 + 交互。
//  视觉：本项目通用壳风格（注明待精修），列表不追求像素级对齐；未真机联调。
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
    class UIGuildSkill : public UIElement
    {
    public:
        static constexpr Type TYPE = UIElement::Type::GUILDSKILL;
        static constexpr bool FOCUSED = false;
        static constexpr bool TOGGLED = true;

        UIGuildSkill();

        void draw(float inter) const override;
        void send_key(int32_t keycode, bool pressed, bool escape) override;
        Button::State button_pressed(uint16_t buttonid) override;
        Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
        void send_scroll(double yoffset) override;
        UIElement::Type get_type() const override;

        // 状态栏提示（发包/结果反馈），由交互回填。
        void set_status(const std::string& text);

    private:
        enum Buttons : uint16_t
        {
            BtClose = 1300,     // 关闭窗口
            BtBuy = 1301,       // 购买（mode=1，对当前选中技能）
            BtLevelUp = 1302,   // 升级（mode=2，对当前选中技能）
        };

        int16_t scroll = 0;
        static constexpr int16_t ROW_H = 34;
        static constexpr int16_t LIST_X = 28;
        static constexpr int16_t LIST_Y = 72;
        static constexpr int16_t LIST_W = 380;

        struct Row
        {
            Text info;          // skillId / level / timestamp / purchaser
            int32_t skill_id = 0;
        };

        Sprite background;
        bool has_background = false;
        Text title;
        Text status;
        Text empty_hint;
        int32_t selected_skill_id = 0;   // 当前选中技能（点行回填；买/升级用）
        uint32_t last_seq = 0;           // 上次重建对应的 GuildSkillState.list_seq
        uint32_t last_result_seq = 0;    // 上次读取的操作反馈序号

        std::vector<Row> rows;           // draw 前按 state.list_seq 重建

        void rebuild_rows();
        void refresh_status_from_result();
        int16_t row_at(Point<int16_t> cursorpos) const;
    };
}
