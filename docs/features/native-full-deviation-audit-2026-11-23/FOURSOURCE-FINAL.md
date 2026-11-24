# 四源对照收口 · 最终偏差矩阵（2026-11-24）

> 用户指令：非扩展功能的所有原生行为，对照 Angel v83 / 095 / 192 / MinMS 四源校验共同逻辑一致；高版本内容对照高版客户端闭环；资源缺失三端复核真缺。
> HEAD `a364931d`。8 域四源子代理 + lead 源码复核。全部只读审计，**未改源码/未编译/未真机**。截图未采信（来源端未确认）。
> 端口实测：**13337=Angel v83 / 13338=095 / 13339=192**（旧文档映射已失效）；orange-wz=10012（128B key+IV TSPHKw==）。

## 0. 覆盖结论一句话
四源共同逻辑**主干一致**，未发现 P0/崩溃级偏差。8 域共确认：**共同逻辑偏差 4 项（A 类，可修）**、**高版本内容缺口 8 项（B 类，多为版本分叉、需裁决）**、**资源真缺 2 项（C 类）**；其余前轮旧账多项被 working tree 实现推翻（无偏差）。

## A. 共同逻辑偏差（四源都有、当前端缺/不同）— 建议修
| # | 项 | 当前端 file:line | 四源证据 | 严重度 | 修复方向 |
|---|---|---|---|---|---|
| A1 | **元素克制整链缺失** | `Data/SkillData.cpp:99` 解析 elemAttr、`SkillData.h:129` 存 element，但 `Gameplay/Combat/Attack.h` 无 element 字段、`Gameplay/MapleMap/Mob.cpp` 无 elemattr 判定（lead 已源码核实） | MinMS `Mob.cpp:7438-7475 adjust_attack_damage`（'2'→×0.5/'3'→×1.5/'1'→免疫）、`Attack.h:78 element`、`Combat.cpp:611`；083 CalcDamage 元素分支；095 XML `5201002 elemAttr=f` | **高** | 补 Attack.element + Mob.calculate_damage 元素判定（标注 MinMS 现代化增强；s9 权威结算，仅客户端表现层） |
| A2 | **finish 特效三端有源未挂接** | `Gameplay/Combat/Skill.cpp:189-191` extra_nodes 只列 prepare/keydown/keydownend/screen/special/tile，无 finish | 三端源：live 3/095 5/192 26 img；083 `ShowSkillEffect`/`ShowSkillSpecialEffect` 特效族 | 中 | extra_nodes 加 "finish"，复用 ExtraNodeUseEffect |
| A3 | **仓库底板加载根错** | `IO/UITypes/UIStorage.cpp:47` 读 `UI["Storage.img"]`（不存在），底衬+3 钮加载失败；代码注释自承"换根需 Trunk 钮坐标、无证据故预留" | Angel CTrunkDlg Draw 463×318 + 5 操作；orange-wz 实测 live `UIWindow.img/Trunk` 存在、`Storage.img` 不存在（lead 已核实） | 中 | 换根 `UIWindow.img/Trunk`，按 Angel 坐标接 backgrnd+3 钮 |
| A4 | **建角 step4 相机偏移缺** | `IO/LoginBackground.cpp:36-39 step_height` 仅 `-8-600*step`，无 step4 职业子步 | Angel `CLogin::GetStepHeight@0x5fc0e4`（step==4 时 `+*(this+133)`）；095 `0x5d1c80`/192 `0xb53790` 同构（race→UIRace 子步）；MinMS 无（旁证未移植） | 中 | 补 step4 子步偏移（需 s9 职业子集→UIRace 换算真机定） |
| A5 | **怪物手册右键菜单 Register/Release 未接** | `IO/UITypes/UIMonsterBook.cpp` 无 ContextMenu/BtRegister/BtRelease 分支（lead 已 grep 核实），卡片用 icon 轮播占位 | 192 IDA 字符串 `MonsterBook/ContextMenu/{BtRegister,BtRelease,t,c,s}`；live UI.nx + 095/192 XML 全在位（资源真有） | 中 | 右键选中卡 → 弹 ContextMenu → 发 Register/Release 包（协议需核对 s9） |
| A6 | **MobSkill 施法特效延迟起点缺** | `Gameplay/MapleMap/Mob.cpp:558-573 play_mob_skill_effect` 切 stance 瞬间全播，无 tEffectAfter 延迟 | 095 `TryDoingSkill@0x64ae60`（`tStart=tEffectAfter+update_time`）；MinMS `cast_skill_effect_after_ms`；Angel 同源 | 中 | 按 MobSkill.img level 的 tEffectAfter 延迟触发（需逐 WZ 证据定时序） |
| A7 | **BossHP 每 Boss 定制血条未消费** | `IO/UITypes/UIBossHP.cpp:33-59` 只读通用 DualMobGauge/MobGage | 192 `CField_PartyRaidBoss::Init@0x96e650` 装配 `DualMobGauge/Mob/9700036`；live UI.nx 该节点在位 | 低 | 多 oid 时按 mobid 选定制头像血条（渲染层，资源在位） |
| A8 | **物理常量与 MinMS 调校值不同** | `Gameplay/Physics/Physics.cpp:22-28` FRICTION=0.5/SWIMGRAV=0.03/SLOPE=0.1 | MinMS `Physics.cpp:23-32` 0.3/0.055/0.15（注释明写调校理由）；Angel/095/192 走 CWvsPhysicalSpace2D 无同构常量 | 低 | **需真机手感定夺**，不宜盲改（MinMS 是 Journey 调校，非 Angel 一手） |

## B. 高版本内容缺口（版本分叉，需用户裁决是否移植）
| 项 | 高版本证据 | 当前端 | 判定 |
|---|---|---|---|
| 建角独立 JobSelect 步骤 + 职业全集 | 095 `CUINewCharJobSelect`/192 `CUINewCharNameSelect{Ser,Mercedes,Res}` | 五团（s9 服务端范围） | 版本分叉，s9 仅 5 团，不盲扩 |
| 192 SPW 开关按钮 | 192 `Login.img.xml BtEnableSPW/BtDisableSPW` | 全仓 0 | P3，s9 若不下发则不影响 |
| 元素/暴击倍率基数 | 当前 1.2~1.5 vs MinMS 1.5~1.75；083 PDamage 常量链未取全 | — | 需用户定目标倍率（s9 权威结算） |
| KeyDown/蓄力状态机 | 083 `is_keydown_skill`/MinMS 完整 | 无（旧账 P2 预留） | 能力缺口，需单项立项 |
| 二段跳/瞬移数值模型 | Angel `350/40` px/s vs 当前 MinMS 冲量 `3.2+0.12lv` | MinMS 近似 | 行为族一致，数值非原生复刻（s9 权威） |
| 天气道具系统/FOB 障碍区/192 Restore 族/动态 foothold | Angel `TransientLayer_Weather`、192 `LoadMap` 扩展族 | 自研 weather 或缺失 | s9 优先，不回改（已文档化） |
| 商城/宠物/小游戏高版本语义 | 192 宠物 transform(534 包)、RPS modal、Omok/MatchCard | 壳/169 固定 | 既有决策域（P2），未重复立项 |
| starPlanet 侧栏/192 ChatFontColor 面板/PartyHP 配置开关 | 095/192 有资源+符号 | 自研等价或预留 | 低/中，功能等价、资源未闭环 |

## C. 资源真缺三端复核（只有三端皆无才判真缺）
| 资源 | live | 095 | 192 | 结论 |
|---|---|---|---|---|
| `Mob/SpecialEff.img` | 无（.nxcache 全 id.img，无该名） | 无 | 无 | **真缺**（唯一硬缺源；播放点已预留 `Mob.cpp:224-230`） |
| `receiver` 特效节点 | 全 92img 0 | 全 117img 0 | 全 319img 0 | **真缺**（三端皆无，非版本差异） |
| `MatchCard/MemoryGame` 独立节点 | 0 | 不可解析（095 key 缺） | 0 | 当前端/192 皆无 |
| Storage.img | 无（真身 `UIWindow.img/Trunk` 有） | — | — | 不是资源真缺，是加载根错（A3） |
| MonsterBook ContextMenu/LeftTabInfo、BossHP DualMobGauge、CashShop.img、DamageSkin.img、DailyGiftStage.img、天气粒子、StatusBar 四份 | 全在位 | XML 在位 | XML 在位 | **均不缺**（代码接线问题，非资源） |

## D. 明确无偏差（推翻旧账，防重复立项）
- 登录域：60 频道翻页、ViewChoice 真弹窗、线性缓动、step0 相机 X、单频道世界、PIC 主链路——working tree 均已实现；登录背景/Loading 卷轴/WorldSelect 资源三端在位。
- HUD：快捷栏 hit→send_key、ITEM 键、拖拽复位、聊天三态、HP/MP/EXP 条、Buff、小地图三态——四源等价；**右键清除快捷槽是旧账误判**（Angel OnMouseButton 只处理左键，无原生右键清除，当前端拖出清除已闭环）。
- 核心窗：MARKNPC、宠物三页签、改键文案、UIOptionMenu、AskAvatar、装备 tooltip 星力/附加/潜力——working tree 已实现，旧 README ❌ 过期。
- 技能：Rush、5201006 后跃（与 MinMS 逐值一致）、伤害公式族、无视防御、瞬移/二段跳施放动效、receiver 真缺——闭环。
- 社交：仓库/交易/便签/快递/公会发包（9ef96287 已修）、联盟高级包、好友分组本地筛选——已修/闭环。
- 商城：赠礼、宠物命令 opcode169、雇佣商人服务端翻案、每日签到例外、伤害皮肤、ColorPrism——已修/用户认可/私有协议无原生对照。
- 协议层：161 c2s opcode 全有构造点、无空 handler；92/93/101 同号异义、104/165 c2s↔s2c 撞号按 s9 优先（既有红线）。

## E. 建议一次性修复（纯客户端、资源在位、四源证据充分）
**A1（元素克制）+ A2（finish 挂接）+ A5（手册右键菜单协议对齐后再做）+ A3（仓库换根坐标从 Angel 取）+ A4（step4 相机，需职业换算）** 中：
- 可立即低风险修：**A1、A2、A3**（A1 需按 MinMS 移植并标注；A3 需先从 Angel CTrunkDlg 取 5 钮坐标）。
- 需协议/真机证据再修：**A4**（职业换算）、**A5**（Register/Release 包 s9 是否有 handler）、**A6**（tEffectAfter 逐 WZ 时序）、**A7**（可修但低优先）、**A8**（真机手感，不盲改）。

## 声明
全部静态四源对照 + orange-wz 实测，未改源码、未编译、未真机发包。截图未采信（来源端未确认）。用户拍板修复范围后再一次性实现+编译+提交。
