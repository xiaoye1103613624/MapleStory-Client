# 四源对照偏差修复 · 实施记录（2026-11-24）

> 用户授权「全修，如果发现存在其他问题一定也要校准后一并修复」。四源证据见 FOURSOURCE-FINAL.md + FOURSOURCE-COVERAGE.md + 各 fx-* 取证回传。
> 构建：`tools/build_smoke.bat` → EXITCODE=0 / GATE OK（Release/x64，含全部改动）。真机：**未验收**。
> 基线 HEAD `a364931d`，工作树既有 446 项未提交改动，本提交仅隔离本轮文件。

## 已修（逐条 file:line）
| 项 | 证据 file:line | 改动 |
|---|---|---|
| **A1 元素克制整链** | `Attack.h:80 element`；`SkillData.h:91 get_element` / `SkillData.cpp:267`；`Skill.cpp:264` 透传；`Mob.h:267 elemattr` / `Mob.cpp:61` 解析；`Mob.cpp:1046-1075` 判定 | 补 Attack.element + Mob.elemattr（WZ `info/elemAttr` 字母+强度码对）；移植 MinMS `adjust_attack_damage` 语义（1=免疫/2=×0.5/3=×1.5），标注现代化增强非 083 原生逐证；s9 权威结算，仅飘字表现层 |
| **A2 finish 挂接** | `Skill.cpp:190` extra_nodes 加 "finish" | finish 三端有源（live3/0955/19226），复用 ExtraNodeUseEffect；receiver 三端真缺不挂 |
| **A3 仓库换根** | `UIStorage.cpp:43-57` | `Storage.img`（不存在）→ `UI["UIWindow.img"]["Trunk"]`（backgrnd 463×318）；按钮按 Angel CTrunkDlg::OnCreate@0x7c618e 坐标：BtSort(154,55)/BtInCoin(236,294)/BtOutCoin(5,294)；关闭钮维持描边 X |
| **A4 step4 相机职业子步** | `LoginBackground.h:56-57/91-94/145`、`LoginBackground.cpp:89-104`、`UICommonCreation.cpp:~104` 接线 | 新增 set_creation_job(job 0..4)→095 ConvertSelectedRaceToUIRace 映射{4,1,0,3,2}；仅 step4 叠加 `-600*UIRace`；建角构造时按 classType 设 job。当前建角走 step3+场景 overlay，默认不启用，零行为变化 |
| **A5 手册封面 bug + 右键** | `UIMonsterBook.cpp:677-696 rightclick`、`:713-716 C 键修 cardId`；`.h rightclick override`；locale cover_set/cover_cleared | 四源证实无独立 Register/Release 包，原生右键=设/取消封面（0x39+cardId）；修真实 bug：发 suffix 被 s9 拒 → 还原 `2380000+suffix`；右键选中卡按当前 cover 决定设/消 |
| **A6 MobSkill effectAfter 延迟** | `Mob.h show_effect +delay_ms`；`Mob.cpp:570-597`、`:907/934` | live 实证 `Mob.img/<id>/info/skill/<i>/effectAfter`（ms，如 8800000 的 176）；按 skillid+level 匹配取延迟，effect/hit/tile 带同一 delay 走 EffectLayer::add 的 wait_ms；0→1 防漏帧（095 tStart==0→1 等价） |

## 未修（如实声明，非遗漏）
- **A7 BossHP 每 Boss 定制血条**：set_hp 收到的是 oid 非 mobid，UIBossHP 无 oid→mobid 映射；当前通用血条已工作，属低优先渲染增强，强改需扩协议/状态，本轮不造。
- **A8 物理常量**：四源中只有 MinMS 有调校值（0.3/0.055/0.15），Angel/095/192 走 CWvsPhysicalSpace2D 无同构常量，**无四源共识**；审计原文亦"需真机手感定夺，不宜盲改"，维持 Journey 原值。
- **B 类高版本缺口**（JobSelect/SPW/暴击倍率/KeyDown/二段跳数值/starPlanet/ChatFontColor/PartyHP/宠物transform/RPS/Omok 等）：版本分叉或既有决策域，s9 优先，不动。
- **资源真缺**：Mob/SpecialEff.img、receiver 三端皆无（唯一硬缺源），维持预留；其余（Trunk/ContextMenu/LeftTabInfo/BossHP/CashShop/DamageSkin/天气）三端在位，属代码接线（本轮已修 Trunk/ContextMenu）。
- **DBG 日志**：EquipTooltip/UIEnhance 等多处 `[DBG-xx]` 为既有 _DEBUG 守卫日志（Release LOG 空宏），非本轮引入，不属本功能闭环范围，未批量删除。

## 验证
- 构建 EXITCODE=0 / GATE OK（最终含 UICommonCreation A4 接线那次）。
- 静态：A1–A6 各 dispatch/字段/节点路径 grep 行号见上；A3 根路径经 orange-wz 实证；A6 字段名/值经 orange-wz 实证。
- 真机（待用户 s9+MySQL）：元素克制飘字、finish 特效、仓库底板+三钮、step4 相机（需切职业）、手册右键封面与 C 键、MobSkill 延迟特效。截图须先确认来源端。

## 声明
全部静态四源对照 + IDA 反编译 + orange-wz 只读解析；未真机发包。A4 采用 095 映射是"五职业语义正确"选择，v83 原生该字段仅 0/1 标志，属版本口径差异已记录。
