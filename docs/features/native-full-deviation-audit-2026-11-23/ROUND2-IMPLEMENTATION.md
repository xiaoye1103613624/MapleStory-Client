# 第二轮可修偏差 · 实施记录（2026-11-24）

> 用户授权「修复并提交」（第二轮查漏补缺 ROUND2-README.md 的优先修复清单）。
> 构建：最终 `tools/build_smoke.bat` → EXITCODE=0 / GATE OK（中途因新增 GUILD_OPERATION 发包点触发协议门禁，已按门禁要求 `gen_opcode_matrix.py` 重生成矩阵后通过）。
> 真机：**未验收**（需用户 s9+MySQL 点验）；本记录只声明源码与构建事实。

## 基线
- HEAD 起始 `9ef96287`，工作树既有 452 项未提交改动（清单 `tools/_baseline_before_round2_fix_20261124.txt`）；提交隔离既有改动。

## 修复项（逐条 file:line）

| # | 项 | 证据 | 改动 |
|---|---|---|---|
| 1 | 右 Ctrl 键码 | `IO/Keyboard.cpp:127` | `rightctrlcode()` 从误返 LEFT_CONTROL → `GLFW_KEY_RIGHT_CONTROL`(345)，修复右 Ctrl 无法参与 Ctrl+C/V |
| 2 | EquipGrowthData 文案保留分支恒假 | `Data/EquipGrowthData.cpp:213` | 把 `keep_local = e.local_only && ...` 从 `e.local_only=false` 之后移到之前，恢复「服务端 has_data=false 时保留本地成长文案」语义 |
| 3 | 按键 repeat 连发（P1） | `IO/UI.cpp:324-333` | `UI::send_key` 的道具/技能/宏映射加 one-shot 守卫：`pressed && is_key_down[keycode] && (ITEM/SKILL/MACRO)` → return，对齐 MinMS `UI/UI.cpp:401-408`。`IO/Window.cpp:827-836` 补注释（`event.key.repeat` 语义说明，行为不变） |
| 4a | 公会徽章发包 | `Net/Packets/GuildPackets.h:106-120`、`IO/UITypes/UIGuild.cpp:322-343`（M 键 `:260`）、`UIGuild.h:78 prompt_emblem()`、`locale/{zh-CN,en-US}.ini guild.emblem_hint` | 新增 `GuildChangeEmblemPacket`（0x0F + short bg, byte bgcolor, short logo, byte logocolor，严格按 s9 GuildOperationHandler.java:233-234）；UIGuild M 键 SoftKey 复合输入 `bg,bgcolor,logo,logocolor` 发包。不发明编辑器坐标 UI |
| 4b | UIGuild 四处空提示 | `IO/UITypes/UIGuild.cpp:287-360` | `prompt_invite()`/`prompt_deny()` 无 CharInfo 目标的 else 空 → 接 UISoftKey；`prompt_notice()` 空体 → 接 UISoftKey + `GuildNoticePacket`；`prompt_create()` 空体 → 接 UISoftKey + `GuildCreatePacket`。新增 locale `guild.create_prompt`/`guild.deny_prompt`，复用既有 `guild.invite_prompt`/`guild.notice_prompt` |
| 5 | RECOIL_SHOT 激退射杀 | `Character/SkillId.h:171`、`Character/Player.h:86`、`Character/Player.cpp:711-727`、`Gameplay/Combat/Combat.cpp:391-394` | 加枚举 5201006；`Player::apply_recoil` 移植 MinMS Player.cpp:1552（hx=±(250+40*(lv/4))、vy=-(250+20*(lv/4)) px/s，背离朝向，播 JUMP 音）；apply_result_movement 加 case。纯客户端表现，服务端 SkillFactory 已识别 |
| 6 | 技能特效族 6 类（首轮误判纠正） | `Gameplay/Combat/SkillUseEffect.h:134-150`、`SkillUseEffect.cpp:29-47`、`Gameplay/Combat/Skill.h:56`、`Skill.cpp:186-208/214-216` | 首轮只探 210.img 误判"资源缺源"；本轮 D3 跨 5 img 实证 prepare/keydown/keydownend/screen/special/tile 有源（CANVAS 帧）。新增 `ExtraNodeUseEffect`（复用 Effect→show_attack_effect，受画面开关约束）+ `CompositeUseEffect`；Skill 构造时探测这 6 节点逐个挂接，`apply_useeffects` 并列播放。receiver/finish 全库无源不接；special/tile 多段落点物理本端 EffectLayer 不支持，降级为原地逐帧动画（诚实近似，真机观感待验） |

## 验证
- 构建：最终 EXITCODE=0 / GATE OK（Release/x64，exe 输出 `E:\MXD\BeiDou-Client-xy\`）。
- 协议门禁：新增 GUILD_OPERATION 发包点后按门禁重生成 `docs/features/s9-protocol-align/protocol/opcode-matrix.json`（s9 head 966d633eba），复检 GATE OK。
- 静态：6 类特效节点 existence 由 orange-wz 实证（tools/_d3_skill_effect_verify2.py / _d3_effect_shape.py，128B key，isError:false）。
- 真机（待用户）：长按道具/技能键不连发；右 Ctrl 复制粘贴；公会公告/徽章（会长+公会大厅+金币）；激退射杀后跃；14121004 等多类技能特效可见；成长文案保留。

## 有意不修（见 ROUND2-README.md §4）
Mob 出生特效（真缺 SpecialEff.img）、receiver/finish 特效（全库 0）、活动 HUD、PARTY_SEARCH_REGISTER（s9 空 stub）、交易所/MapleTV、元素克制/mastery（风险裁决）、冲锋手感/mount action（需真机/逆向）、渲染 R1/R2（潜在未触发）、EffectLayer/Animation 高级族（大改动，需单独评估）。

## 提交
隔离提交本轮改动源文件 + 文档 + 重生成的 opcode-matrix.json；不含基线 452 项既有改动。
