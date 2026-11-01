# r14 · BeiDou.exe 原生功能全景目录（非 ijl15 / 非自定义扩展）

> 日期：2026-10-22  
> 目标：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe`（v83 原版客户端，**不含** `ijl15.dll` 扩展）  
> 当前端：`E:\project\MapleStory-Client`（部署 `E:\MXD\BeiDou-Client-xy`）  
> 服务端可达性对照：`E:\project\BeiDou-Server_s9`（172 个 channel Handler）  
> 用途：在当前源码中**完整复刻参考端原生能力**的权威清单；`E:\MXD\自定义功能实现` 明确排除，后续另做。

---

## 0. 方法与证据链（REA + IDA attached）

| 步骤 | 工具 | 结果 |
| --- | --- | --- |
| doctor | `rea doctor --provider ida` | IDA registration OK；provider available |
| analyze | `rea analyze BeiDou.exe` | **失败**：adapter 误走 idalib headless（`idb_list` 300s 超时）。**勿重复** |
| search | `rea search … goPartySearch --kind strings` | ✅ 命中 `0xaf2748`（attached MCP `13338`） |
| decompile | `rea decompile … 0x822350` | 上游已返回完整伪代码；REA 因 `ok` 字段 schema 报 `unreadable_output`，证据仍有效（见 `evidence/rea_decompile_MakerUI.json`） |
| survey | IDA MCP `survey_binary` | PE32 base `0x400000`，**55654** 函数（命名仅 510），**3889** 字符串 |
| 字符串/xref | IDA `find` / `find_regex` / `xrefs_to` | 关键 UI 路径 → 构造函数已定位（下表） |
| 资源全集 | `UIWindow.img` 顶层 | **112** 节点（[`../../v83-hud-quest/evidence/UIWindow.img.top-level.txt`](../../v83-hud-quest/evidence/UIWindow.img.top-level.txt)） |

**二进制身份（survey）**

- SHA256：`1198fa57ca5a7c489bae43ec13c69681d9cabe0f96762f3dc0357facf2e7d4df`
- MD5：`44c1030ecd8484e849c83892eb8fa025`
- 模块：`BeiDou.exe` / IDB：`BeiDou.exe.i64`

**重要结论**

1. 原生功能**不在插件里**：扩展清单（套装/签到/伤害皮肤等）属于 `ijl15.dll`，本文件不收录。  
2. exe **几乎无 CUI\* 符号**（stripped）；功能边界靠 `UIWindow.img/<节点>` 字符串、`go*` 脚本钩子、与 s9 Handler 三角验证。  
3. 多数窗口路径是运行时拼接，IDA 字符串库只保留部分完整路径；**资源顶层 112 节点才是完整 UI 能力面**。

---

## 1. REA/IDA 已定位的原生 UI 锚点（可复核）

| 系统 | 字符串 EA | 引用函数 | 大小 | 证据 |
| --- | --- | --- | --- | --- |
| Maker 制造 | `UI/UIWindow.img/Maker/GaugeBar/gauge` @ `0xb39260` | `sub_822350` | `0x668` | REA decompile 加载 `notUse`/`GaugeBar`/`gauge`，建控件 id 30000/40000/20000… |
| ViciousHammer | `0xb394e8` | `sub_82A9BC` | `0x3bc` | xref |
| VegaSpell | `0xb395d4` | `sub_82BB83` | `0x708` | xref |
| CashGachapon | `0xb3de68` | `sub_999E4D` | `0x43c` | xref |
| PartySearch / Alliance 脚本 | `goPartySearch` `0xaf2748`；`goAllianceINVITE` `0xaf2734`；`goAllianceTALK` `0xaf2724` | `sub_49D0B6`（及 `sub_49D90D`） | `0x833` | REA search + xref |
| Quest | `UI/UIWindow.img/Quest/…` | `sub_83EA05` | `0xaa0` | xref |
| UserList/GuildUnion | `UI/UIWindow.img/UserList/…` | `sub_917546` | `0x60a` | xref |
| ToolTip | `UI/UIWindow.img/ToolTip/…` | `sub_8E49B5` | `0x215f` | xref |
| PartyRace | `0xaf4dd8` | `sub_56BD3C` | `0x860` | 活动 |
| DualMobGauge | `0xaf4fbc` | `sub_56C9B7` | `0x866` | 活动 |

产物目录：[`evidence/`](evidence/)（`ida_survey_binary_parsed.json`、`ida_xrefs_key_ui.json`、`rea_search_goPartySearch.json`、`rea_decompile_MakerUI.json` 等）。

---

## 2. 当前端已实现能力快照（对照基线）

### 2.1 `UIElement::Type`（50 项，含 NUM 前）

登录族：LOGIN / TOS / GENDER / WORLDSELECT / REGION / CHARSELECT / VIEWALLCHAR / LOGINWAIT / RACESELECT / CLASSCREATION / SOFTKEYBOARD / LOGINNOTICE*  

游戏 HUD：STATUSBAR / CHATBAR / BUFFLIST / NOTICE / MINIMAP / CHANNEL / CHAT / CHATRANK / KEYCONFIG / OPTIONMENU / JOYPAD / EVENT / DAILYGIFT / QUIT / STATUSMESSENGER  

核心玩法窗：NPCTALK / SHOP / STATSINFO / ITEMINVENTORY / EQUIPINVENTORY / SKILLBOOK / QUESTLOG / WORLDMAP / USERLIST / CHARINFO / CASHSHOP  

P0.5+r12 已补：STORAGE / PARTY / BUDDYLIST / TRADE / GUILD / MERCHANT / MONSTERBOOK / MAKER / ALLIANCE  

### 2.2 入站 Handler（`PacketSwitch` 约 76 个 emplace）

覆盖：登录选角、进图、移动/攻击、NPC 对话/商店、背包整理、Buff/技能、任务、仓库、组队/好友/交易/公会/联盟、雇佣商人盒、怪物图鉴卡/封面、现金商城壳、以及自定义 `LIMIT_BREAK_SYNC`。  

**未注册但 OpcodeName 表可能列出的 v83 包** → 仍会 `MSG_UNHANDLED`（见下文缺口）。

---

## 3. UIWindow.img 112 节点 → 复刻状态总表

图例：

- **状态**：`✅已有` / `🟡部分` / `❌缺失` / `➖非独立功能`（纯控件/装饰）  
- **优先级**：`P0` 主循环 / `P1` 原生内容 / `P2` 社交增强 / `P3` 活动小游戏 / `P4` 管理/废弃  
- **s9**：服务端是否已有可对接 Handler（决定能否只改客户端闭环）

### 3.1 核心主循环（多数已齐）

| 节点 | 当前端 | s9 | 优先级 | 备注 |
| --- | --- | --- | --- | --- |
| Item | ✅ ITEMINVENTORY | ✅ Inventory* | P0 | |
| Equip | ✅ EQUIPINVENTORY | ✅ | P0 | |
| Stat | ✅ STATSINFO | ✅ | P0 | |
| Skill / SkillEx / SkillMacro / SkillMacroEx / MacroSkill / SkillUp / EnchantSkill | 🟡 SKILLBOOK | ✅ Skill* | P0/P1 | SkillEx/宏/附魔 UI 深度未对齐 |
| ToolTip | 🟡 ItemTooltip 等 | — | P0 | 原生 tip 族齐全度待逐项 |
| Shop | ✅ SHOP | ✅ NPCShop | P0 | |
| UtilDlgEx / UtilDlgEx_* | 🟡 NPCTALK | ✅ NPCTalk/MoreTalk | P0 | 头像/宠物/多宠装备分支不全 |
| GameOpt / SysOpt | ✅ OPTIONMENU | — | P0 | |
| KeyConfig | ✅ KEYCONFIG | ✅ KeymapChange | P0 | |
| SoftKeyboard | ✅ SOFTKEYBOARD | — | P0 | |
| Notice / FloatNotice | ✅ NOTICE / STATUSMESSENGER | ✅ ServerMessage 等 | P0 | |
| UserInfo | ✅ CHARINFO | ✅ CharInfoRequest | P0 | |
| WorldMap | ✅ WORLDMAP | 🟡 | P0 | 原生地图；Tip 扩展属 ijl15，排除 |
| Channel | ✅ CHANNEL | ✅ ChangeChannel | P0 | |
| Quest / QuestAlarm / QuestIcon | 🟡 QUESTLOG | ✅ QuestAction | P0 | QuestAlarm 未接 |
| MiniMap | ✅ MINIMAP | — | P0 | |
| Trunk | ✅ STORAGE | ✅ StorageHandler | P0 | r11 已补 |
| TradingRoom / CashTradingRoom | 🟡 TRADE | ✅ PlayerInteraction | P0 | 现金交易房未分 |
| PersonalShop / EntrustedShop / MemberShop | 🟡 MERCHANT | ✅ PlayerInteraction / HiredMerchant | P0 | 访客购买为主；店主/雇佣变体不全 |
| UserList | 🟡 USERLIST + PARTY/BUDDY/GUILD | ✅ Party/Buddy/Guild | P0 | 页签深度 vs 参考端待对齐 |
| MonsterBook | ✅ MONSTERBOOK | ✅ MonsterBookCover | P0 | r12 已补（简化展示） |
| Maker | ✅ MAKER | ✅ MakerSkillHandler | P0 | r12 已补；对标 `sub_822350` 布局可再精修 |
| CashShop（独立 img） | 🟡 CASHSHOP | ✅ CashOperation / EnterCashShop | P0 | 购买已接；礼物/礼包等简化 |
| EventWindow | ✅ EVENT | — | P1 | |

### 3.2 原生内容 / 社交增强（当前缺口主战场）

| 节点 | 当前端 | s9 | 优先级 | 说明 |
| --- | --- | --- | --- | --- |
| Messenger | ❌ | ✅ MessengerHandler | P1 | 聊天室/信使窗 |
| Family / FamilyTree | ❌ | ✅ Family*Handler / OpenFamily* | P1 | 家族 |
| Memo | ❌ | ✅ NoteActionHandler | P1 | 便签 |
| Delivery（快递） | ❌ | ✅ DueyHandler | P1 | IDA 有 `Delivery/*` 路径串 |
| PartySearch | ❌ | ✅ PartySearch*Handler | P1 | `goPartySearch` → `sub_49D0B6` |
| Ranking | ❌ | ❌（s9 无独立 Rank Handler） | P2 | 需服务端或预留 |
| Incubator | ❌ | 相关 Use* 可能间接 | P2 | 孵化器 UI |
| ViciousHammer | ❌ | ✅ UseHammerHandler | P1 | `sub_82A9BC` |
| VegaSpell | ❌ | 卷轴类（待核对） | P1 | `sub_82BB83` |
| CashGachapon | ❌ | 🟡 CashOperation / Surprise | P1 | `sub_999E4D` |
| RemoteGachapon | ❌ | ✅ RemoteGachaponHandler | P1 | 地图扭蛋 |
| ItemProtector / KarmaScissors | ❌ | UseCashItem 等 | P2 | 现金道具专用 UI |
| Book | ❌ | — | P2 | 与 MonsterBook 不同节点 |
| StoreBank | ❌ | ✅ FredrickHandler / RemoteStore | P1 | 弗雷德里克临时仓 |
| NewYearsCard | ❌ | ✅ NewYearCardHandler | P2 | |
| Transform | ❌ | 效果/技能侧 | P2 | |
| Radio | ❌ | — | P3 | |
| AranSkillGuide | ❌ | — | P2 | 战神引导 |
| EnergyBar / TemporaryStatView | 🟡 BUFFLIST 等 | — | P1 | 能量条/临时状态视图 |
| SlideMenu / GameMenu / ShortCut / ContextMenu | 🟡 部分在 STATUSBAR | — | P1 | 菜单/快捷 |
| Megaphone / AvatarMegaphone / ItemMegaphone / ArtMegaphone | ❌ | UseCashItem / 聊天 | P2 | 喇叭类 UI |
| itemSearch / ExceptionItemSearch | ❌ | — | P2 | |
| Coupon | ❌ | ✅ CouponCodeHandler | P2 | |
| Claim / AdminClaim / Report / Admin / GMBoard / counsel | ❌ | ✅ Report / Admin* | P4 | 管理/客服 |
| Teleport / FadeYesNo / MultiLine / IconBase / BtUIClose* / Reset / BetaEdition / Title | ➖/🟡 | — | — | 通用控件或过场 |

### 3.3 活动 / 小游戏 / 赛事（P3，可后置）

| 节点 | 当前端 | s9 | 备注 |
| --- | --- | --- | --- |
| Minigame / MinigameTable / RpsGame | ❌ | ✅ RPSActionHandler | |
| SpeedQuiz / InitialQuiz | ❌ | — | |
| MonsterCarnival | ❌ | ✅ MonsterCarnivalHandler | |
| AriantMatch / classMatch | ❌ | — | |
| PartyRace | ❌ | — | `sub_56BD3C` |
| DualMobGauge / MobGage | ❌ | — | `sub_56C9B7` |
| MapleTV | ❌ | — | 多为启动器路径串 |
| Wedding | ❌ | ✅ Wedding*Handler | |
| TreasureBox | ❌ | ✅ UseTreasureChestHandler | |
| plant / raise | ❌ | ✅ Raise*Handler | 种植/养成 |
| MapleEvent / MonsterKilling / muruengRaid / CharacterClone / createCygnus | ❌ | 部分脚本/活动 | |
| MateMessage / CancelRequests | ❌ | — | |

### 3.4 Alliance（联盟）

| 能力 | 当前端 | s9 | 备注 |
| --- | --- | --- | --- |
| Guild Alliance UI / 操作 | ✅ ALLIANCE（r12） | ✅ AllianceOperation / DenyAlliance | `goAlliance*` 在 `sub_49D0B6`；可对照精修 |

---

## 4. 协议面：当前端 vs 参考端原生（不含 0x37xx 扩展）

当前端已接的**影响主循环**系统：仓库 309、组队 62/201、好友 63、交易/商店交互 314、公会 65、联盟 66、商城操作 325、图鉴 83/84、雇佣商人 265–267。

仍常见缺失或仅部分的原生向能力（需逐项开包对照 s9）：

| 能力 | 典型 s9 Handler | 客户端缺口形态 |
| --- | --- | --- |
| 信使 | MessengerHandler | 无 UI / 无包 |
| 家族 | Family* / OpenFamily* | 无 UI / 无包 |
| 便签 | NoteActionHandler | 无 UI |
| 快递 Duey | DueyHandler | 无 Delivery UI |
| 名声 | GiveFameHandler | 仅展示、无赠送入口 |
| 组队搜索 | PartySearch* | 无 PartySearch UI |
| 神秘锤 | UseHammerHandler | 无 ViciousHammer UI |
| 维加卷轴 | （卷轴使用链） | 无 VegaSpell UI |
| 现金扭蛋 | Cash* / Gachapon | 无 CashGachapon UI |
| 远程扭蛋 | RemoteGachaponHandler | 无专用 UI |
| Fredrick | FredrickHandler | 无 StoreBank UI |
| 婚礼 | Wedding* | 无 Wedding UI |
| MTS | MTSHandler / EnterMTS | 无 ITC/拍卖 UI（资源在 ITC.img） |
| RPS/小游戏 | RPSActionHandler | 无 Minigame UI |
| 怪物嘉年华 | MonsterCarnivalHandler | 无 MC UI |
| BBS | BBSOperationHandler | 无 GuildBBS UI |
| 新年卡 | NewYearCardHandler | 无 NewYearsCard UI |

> Opcode 数值以 s9 `RecvOpcode`/`SendOpcode` 与当前 `OutPacket.h` 为准，实现期逐字段对齐；本目录不臆造偏移。

---

## 5. 与「扩展功能」的边界（必须遵守）

| 属于本目录（exe 原生） | 不属于本目录（后续） |
| --- | --- |
| UIWindow / CashShop / Login 等 WZ 窗口与 v83 协议 | `ijl15.dll` ModRegistry 25+ 模块 |
| Maker / MonsterBook / Alliance / Hammer / Vega / Gachapon… | `E:\MXD\自定义功能实现\*`（签到、套装、伤害皮肤、窗口商城…） |
| 社交经济基线（仓库/组队/好友/交易/公会/商人） | 侧边栏、天气、收纳背包、扩展装备栏等 |

---

## 6. 复刻实施顺序（仅原生，建议）

### Wave A · 原生内容闭环（s9 已有 Handler，收益高）

1. **Messenger**（信使）  
2. **PartySearch**（对照 `sub_49D0B6` + s9 PartySearch*）  
3. **Delivery / Duey**  
4. **StoreBank / Fredrick**  
5. **ViciousHammer**（`sub_82A9BC`）  
6. **VegaSpell**（`sub_82BB83`）  
7. **CashGachapon / RemoteGachapon**  
8. **Family / FamilyTree**  
9. **Memo + Fame 入口**  
10. **Skill 宏 / SkillEx / EnchantSkill / EnergyBar** 深度对齐  

### Wave B · 经济/拍卖变体

- CashTradingRoom、EntrustedShop/MemberShop 店主视图、ITC/MTS（若要坚持完整原生；s9 有 MTSHandler）

### Wave C · 活动与小游戏

- Minigame/RPS、MonsterCarnival、Wedding、PartyRace、DualMobGauge、NewYearsCard、TreasureBox、raise/plant  

### Wave D · 管理/边角

- Admin/GMBoard/Report/Claim、MapleTV、Ranking（缺服务端则预留）  

每项验收仍按七层：资源节点 → 入站 → 出站 → UI → s9 字段序 → 联调 → 主链回归；单功能单提交。

---

## 7. 当前端相对「完整原生」的缺口摘要

**已具备：** 登录选角进图战斗、背包装备技能任务小地图世界地图、NPC 店/对话、设置、P0.5 社交经济七项、MonsterBook / Maker / Alliance。  

**主缺口（建议 Wave A）：** Messenger、PartySearch、Duey/Delivery、Fredrick/StoreBank、ViciousHammer、VegaSpell、Cash/Remote Gachapon、Family、Memo/Fame、技能宏与能量条等深度。  

**可后置：** 嘉年华/婚礼/小游戏/PartyRace/MapleTV/管理窗。  

**明确不做（本线）：** ijl15 与 `自定义功能实现` 全部扩展。

---

## 8. 复现命令

```powershell
$env:REA_ANALYSIS_PROVIDER = 'ida'
$env:REA_IDA_MCP_CONFIG   = 'C:\Users\11036\.rea\ida-mcp.json'

# 轻量：字符串（推荐）
& 'D:\software\nodejs-24\rea.cmd' search 'E:\MXD\BeiDou-ClientV17.7\BeiDou.exe' 'goPartySearch' --kind strings --provider ida --json

# 反编译（IDA GUI 需保持 BeiDou.exe 在 13338；REA 可能对 ok 字段报 schema，以 observations[].result.code 为准）
& 'D:\software\nodejs-24\rea.cmd' decompile 'E:\MXD\BeiDou-ClientV17.7\BeiDou.exe' 0x822350 --provider ida --json

# 勿对完整 8MB exe 跑 analyze（会误启 idalib 并超时）
```

---

## 9. 相关文档

- 基线社交经济：[`r11-baseline-replication.md`](r11-baseline-replication.md)  
- 图鉴/制造/联盟实现记录：[`r12-remaining-features-audit.md`](r12-remaining-features-audit.md)  
- UI 四窗布局：[`r05-ui-layout.md`](r05-ui-layout.md)  
- 路径记忆：[`client-path.md`](client-path.md)  
- **不要**用 [`01-parity-matrix-and-plan.md`](01-parity-matrix-and-plan.md) 的扩展矩阵当本线范围（那是 ijl15 线）
