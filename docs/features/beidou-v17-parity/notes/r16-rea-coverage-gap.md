# r16 · BeiDou.exe REA 覆盖缺口（完整复刻主链 · 仅原生）

> 日期：2026-10-22  
> 目标原生：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe`（**非** ijl15 / `自定义功能实现`）  
> 当前端：`E:\project\MapleStory-Client`  
> 依据：[`r14-native-feature-catalog.md`](r14-native-feature-catalog.md)、[`r15-baseline-ui-gap-vs-screenshots.md`](r15-baseline-ui-gap-vs-screenshots.md)、`UIWindow.img` 112 节点、`evidence/` + `evidence/r15/` + `evidence/r16/`  
> 本文件：**调研/文档**，不改业务代码。

---

## 0. 一句话结论（给决策者）

**「没反编译完」≠「55654 个函数都没反」。**  
完整复刻所需的是：**主链各批功能达到 C 层证据**（伪代码 + 控件表/包序），而不是全量反编译每一个 `sub_*`。

当前真正卡完整复刻的，是下面这些批次仍 **未达 C**：

| 优先级 | 批次 | 现状摘要 |
| --- | --- | --- |
| P0 | Login / World·Channel / CharSelect / EnterGame / StatusBar | 端侧多已有窗，但 REA 多为 A/B；StatusBar 仅 CreateCtrl 坐标近 C |
| P0–P1 | CoreWindows（Stat/Equip/Skill/Item/Quest/…） | 功能面 ✅/🟡，几乎无 C 层深度对照 |
| P1 | SocialEco 精修 + Wave A 缺口 | Storage/Party/… 已接线但缺深度；Messenger/Duey/Hammer/… 仍缺窗 |
| P3 | Activities | catalog 有节点，多数仅 A，可后置 |

**已达 C（可当对照样板）：** Maker `sub_822350`（完整伪代码 + 控件 id）；StatusBar `sub_8D01B2` 的 CreateCtrl 坐标表（行为/点击分发仍未闭环）。

---

## 1. 三层证据（用户最易混淆）

| 层 | 含义 | 能做什么 | 不能做什么 | 典型产物 |
| --- | --- | --- | --- | --- |
| **A · 功能面 catalog** | 知道「有这个窗/系统」 | 排优先级、对 s9 Handler、列 Wave | **不能**当实现蓝图 | `UIWindow.img` 112 节点；r14 总表 |
| **B · 字符串 / xref 锚点** | 知道串 EA 与引用 `sub_XXXX` | 定点 `rea decompile` 的入口 | **不等于**已还原控件表/包序/点击链 | `ida_xrefs_key_ui.json`；`rea search` |
| **C · REA decompile + 控件表/包序** | 伪代码里能抽出 CreateCtrl / 资源路径 / 发包字段序 | **完美对照实现与验收** | — | `rea_MakerUI_sub_822350.c`；`statusbar_createctrl_coords.txt` |

```
A ──知道有──▶ B ──找到构造函数──▶ C ──可完美复刻──▶ 实机截图验收
```

补充：

- r14 catalog = **A 为主**，夹少量 **B**（Maker/Hammer/…），**几乎不是 C**。  
- r15 补了 StatusBar CreateCtrl → **半 C**；登录链多数仍停在 A + 端侧硬编码对照。  
- 全路径 `UI/UIWindow.img/Foo` 常被运行时拼接，`rea search` 对完整路径常 0 命中；应搜短关键字 / 控件叶名（如 `ViciousHammer`、`PartySearch/BtStart`、`Delivery/empty`）。

---

## 2. 已达 / 未达 C 的总览

### 2.1 已达或接近 C（可复核）

| 系统 | EA | 证据层 | 落盘 |
| --- | --- | --- | --- |
| Maker | `sub_822350` | **C** | `evidence/rea_MakerUI_sub_822350.c`、`rea_decompile_MakerUI.json` |
| StatusBar CreateCtrl | `sub_8D01B2` | **C−**（坐标表齐；点击/QS/聊天行为未抽全） | `evidence/r15/statusbar_createctrl_coords.txt`、`ida_decompile_8D01B2.json` |
| DrawWorldItems | `sub_624E13` | **B+**（函数确认；REA schema `unreadable_output`，伪代码未干净落盘） | `evidence/r15/rea_decompile_DrawWorldItems_624e13.txt` |

### 2.2 仅 B（有构造/脚本锚点，未完整反编译行为）

| 系统 | 串 / go* | 函数 | size |
| --- | --- | --- | --- |
| ViciousHammer | `0xb394e8` gauge | `sub_82A9BC` | `0x3bc` |
| VegaSpell | `0xb395d4` | `sub_82BB83` | `0x708` |
| CashGachapon | `0xb3de68` | `sub_999E4D` | `0x43c` |
| PartySearch / Alliance 脚本 | `goPartySearch` `0xaf2748` 等 | `sub_49D0B6` / `sub_49D90D` | `0x833` / `0x66c` |
| Quest | `UI/…/Quest/…` | `sub_83EA05`（及 `sub_87CE25`） | `0xaa0` |
| UserList | `UI/…/UserList/…` | `sub_917546` | `0x60a` |
| ToolTip | `UI/…/ToolTip/…` | `sub_8E49B5` | `0x215f` |
| PartyRace | `0xaf4dd8` | `sub_56BD3C` | `0x860` |
| DualMobGauge | `0xaf4fbc` | `sub_56C9B7` | `0x866` |
| Delivery（片段） | `indow.img/Delivery/*` @ `0xafcc7c`… | **xref→构造 未落盘** | — |
| PartySearch（控件叶） | `indow.img/PartySearch/Bt*` @ `0xb3a714`… | 可回指 `sub_49D0B6` 一带 | — |

### 2.3 仍停在 A（catalog / 资源有，无可用构造 EA 或伪代码）

登录 Title / Notice·Loading / CharSelect·charInfo、Messenger UI、Family/FamilyTree、StoreBank、Memo 窗、RemoteGachapon、Wedding/Minigame/MonsterCarnival 多数、以及 Core 各窗的**深度行为**（非「有没有 UIType」）。

r16 关键词检索摘要：[`evidence/r16/r16_rea_search_summary.json`](evidence/r16/r16_rea_search_summary.json)。

---

## 3. 按「完整复刻主链」分批：尚未达到 C 的功能

图例：

- **证据**：A / B / C− / C  
- **当前端**：✅ 已有可用实现 · 🟡 部分/需精修 · ❌ 缺失  
- **要补的 REA**：search / decompile（**禁止** `rea analyze` 整包）

---

### Batch Login · Logo / 标题 / 登录板 / Loading / Notice

| 子项 | 证据 | 当前端 | 要补的 REA | 验收截图 |
| --- | --- | --- | --- | --- |
| 书本框 + MapLogin 背景 | A（资源同哈希）；端侧有 `LoginBackground` 硬编码 | 🟡 | search：`Title/signboard`、`Common/frame`、短串 `signboard`；decompile：命中后的 Title OnCreate | `登录界面.png` |
| 木牌登录板 / 连接钮 | A；端侧 `UILogin` bedou 偏移 | 🟡 | 同上；抽出 button CreateCtrl 表 | 同左 |
| Ver / Logo | A | 🟡 | 对照 `Login.img/Title` 叶串 | 同左 |
| Loading 卷轴 | A；`Notice/Loading` 完整路径 search 常 0 | 🟡 `UILoginWait` | search：`Loading/backgrnd`、`Notice`；decompile Wait/Notice 构造 | Loading 实机 vs 正确端 |
| LoginNotice / TOS | A | 🟡/✅ | 有告警文案路径则定点 decompile | 弹窗叠图 |

**本批结论：整批未达 C。** 问题主因是对照未闭环（见 r15），但完美复刻仍需 C 层控件表。

---

### Batch World / Channel · 世界列表、频道板、选世界按钮

| 子项 | 证据 | 当前端 | 要补的 REA | 验收截图 |
| --- | --- | --- | --- | --- |
| 20 世界竖牌绘制 | B+：`sub_624E13` DrawWorldItems | 🟡（且 `channel_count < 2` 过滤缺陷） | 用 MCP/curl 取回 `0x624E13` 伪代码落盘；抽牌间距/原点 | `选择频道页.png` |
| 展开 CH 网格 + 负载条 | A/B | 🟡 | search：`channel` / WorldSelect Obj 叶；decompile 展开卷构造 | `选中频道页.png` |
| 「到选择的世界去」 | A | 🟡 | 按钮 CreateCtrl + 发包序 | 同左 |

**本批结论：未达 C**（`624E13` 仅确认存在）。端侧可先修过滤，但布局完美对齐依赖 C。

---

### Batch CharSelect · 角色槽、charInfo、排行、创建/删除

| 子项 | 证据 | 当前端 | 要补的 REA | 验收截图 |
| --- | --- | --- | --- | --- |
| 桥梁槽位 / 开始·创建·删除 | A；`CharSelect` 完整名 search 常 0 | 🟡 | search：`charInfo`、槽位叶名、`BtSelect` 类；xref→构造 | `选择角色页.png` |
| charInfo 羊皮属性板 | A（`charInfo1` 无串命中；MakeCharInfo 误报） | 🟡 硬编码 `CS_DETAIL_*` | 定点 CharSelect 构造，抽 `charInfo1` 原点 | 同左 |
| 排行区文案 | A（协议已有 rank 字段） | ❌ 未绘 | 伪代码中 rank/jobrank 绘制分支 | 「排行不可用」 |

**本批结论：未达 C。**

---

### Batch EnterGame · 进图、黑屏、操作提示

| 子项 | 证据 | 当前端 | 要补的 REA | 验收截图 |
| --- | --- | --- | --- | --- |
| 进图过场 / Fade | A（`FadeYesNo` 等为控件节点） | 🟡 | 仅当叠图失败时定点 | `进入游戏页.png` |
| 操作说明对话 | A | 🟡 | 与 Notice/UtilDlg 共用时再 decompile | 同左 |
| 进图后 HUD 挂载顺序 | 部分靠 StatusBar C− | 🟡 | 跟 StatusBar / UIStateGame 分发 | 同左 |

**本批结论：未达 C**（可与 StatusBar 合并验收）。

---

### Batch StatusBar · 底栏按钮、快捷格、聊天、HP/MP/EXP

| 子项 | 证据 | 当前端 | 要补的 REA | 验收截图 |
| --- | --- | --- | --- | --- |
| BtShop / BtNPT / BtMenu / BtShort | C−：CreateCtrl 坐标已齐（1000–1007…） | 🟡（回调/DBG 未闭环） | decompile `sub_8D01B2` 内 **button handler / 开窗** 分支；search 已有 `BtNPT` @ `0xb3bab4` | `进入游戏页.png` |
| 右侧快捷 8 格点击 | C− 坐标有；**行为未抽** | ❌ 仅绘制 | 同函数内 QS hit→keymap 分发；必要时 decompile 关联 `sub_*` | 点格开技能/道具/菜单 |
| 聊天条 / chatTarget | B：`chatTarget` @ `0xaf72e0` | 🟡 | decompile 聊天输入绑定 | 发言/频道切换 |
| HP/MP/EXP 条 | 坐标侧有注释痕迹 | 🟡 | 从 `8D01B2` 抽 gauge 控件 id | 数值与条长 |

**本批结论：坐标近 C，交互行为未达 C。** 这是「没反编译完」在主链上的最大可操作缺口之一。

---

### Batch CoreWindows · Stat / Equip / Skill / Item / Quest / WorldMap / KeyConfig / …

| 子项 | 证据 | 当前端 | 要补的 REA | 验收截图 |
| --- | --- | --- | --- | --- |
| Stat / Equip / Item | A（有 UIType）；布局靠 r05 格线 | ✅/🟡 | 坐标偏差时再 search 节点叶 + decompile | `打开了S-E-I-K页面.png` |
| Skill / SkillEx / Macro / Enchant | A；SkillEx 等无 EA | 🟡 | search：`SkillMacro`、`EnchantSkill`、`SkillEx`；xref→构造 | 宏页/附魔页 |
| Quest / QuestAlarm | B：`sub_83EA05` | 🟡 Alarm 未接 | **decompile `0x83EA05`** → 控件表；Alarm 另搜 | 任务窗 + 闹钟 |
| WorldMap | A | ✅ | 偏差再定点 | 大地图 |
| KeyConfig | A | ✅/🟡 | 对照正确端键盘图；一般不必全反 | `键盘设置.png` |
| ToolTip 族 | B：`sub_8E49B5` | 🟡 | 需要像素级 tip 时 decompile | 悬停 tip |
| UserInfo / Channel / MiniMap / Shop / NPC | A | ✅/🟡 | 按验收失败点定点 | 实机清单 |
| CashShop 深度（礼物/礼包） | A | 🟡 | 商城独立 img；按缺口 decompile | 商城用例 |

**本批结论：整批功能面多已有，深度均未达 C。** 优先把 Quest/SkillMacro/ToolTip 从 B→C。

---

### Batch SocialEco · 已实现窗的深度精修

| 子项 | 证据 | 当前端 | 要补的 REA | 验收截图 |
| --- | --- | --- | --- | --- |
| Storage / Trunk | A | ✅ | 布局/包序偏差时再定点 | 仓库存取 |
| Party / Buddy / Guild | A；UserList B：`sub_917546` | 🟡 | **decompile `0x917546`** 抽页签控件 | 好友/组队/公会页 |
| Trade / Merchant 变体 | A | 🟡 | CashTradingRoom / EntrustedShop 叶串 → 构造 | 交易/雇佣店主视图 |
| Maker | **C** | ✅ | 可选：对照精修布局 | 制造 |
| Alliance | B：`goAlliance*` → `sub_49D0B6` | ✅ | decompile 脚本分发精修 | 联盟邀请/对话 |
| MonsterBook | A | ✅ 简化 | 需要原生页深度时再反 | 图鉴 |

**本批结论：「有窗」≠「已反编译完」。** 缺的是 UserList/Trade 变体等 **B→C 精修**，不是从零 catalog。

---

### Batch Wave A 缺口 · Messenger / PartySearch / Duey / Hammer / …

| 子项 | 证据 | 当前端 | 要补的 REA | 验收截图 |
| --- | --- | --- | --- | --- |
| Messenger | A（`CNMMessenger*` 为无关命中） | ❌ | search 叶：`Messenger/` 控件名；ascii/UTF16 扫路径；xref→构造后 decompile | 信使开窗/发言 |
| PartySearch | B：`goPartySearch` + `PartySearch/Bt*` 叶串 | ❌ | **decompile `0x49D0B6`** + 控件叶 xref | 开始/暂停搜索 |
| Delivery / Duey | B−：`Delivery/*` 叶串 18 命中，**无构造 EA** | ❌ | xref `0xafcc7c` 等 → 构造 → decompile | 收发快递 |
| StoreBank / Fredrick | A（字符串 0 命中） | ❌ | 换叶名 / go* / s9 包名交叉；或从 Fredrick NPC 脚本入口 | 临时仓 |
| ViciousHammer | B：`sub_82A9BC` | ❌ | **decompile `0x82A9BC`** → 控件表 + UseHammer 包序 | 砸卷 UI |
| VegaSpell | B：`sub_82BB83` | ❌ | **decompile `0x82BB83`** | 维加卷轴 |
| CashGachapon | B：`sub_999E4D` | ❌ | **decompile `0x999E4D`** | 现金扭蛋 |
| RemoteGachapon | A（0 命中） | ❌ | 搜叶名 / Handler 交叉 | 远程扭蛋 |
| Family / FamilyTree | A（0 命中） | ❌ | 资源节点 + s9 OpenFamily* 包入口反推 | 家族树 |
| Memo / Fame 入口 | A（Memo 串多为误报） | ❌ | 便签叶串 / CharInfo 赠送入口 | 便签/名声 |

**本批结论：全部未达 C；多项仍停在 A 或仅有 B 锚点。** 这是 r14 Wave A 的实现前置证据债。

---

### Batch Activities · 嘉年华 / 小游戏 / 婚礼等（可后置）

| 子项 | 证据 | 当前端 | 要补的 REA | 验收截图 |
| --- | --- | --- | --- | --- |
| MonsterCarnival | A | ❌ | 节点叶 + Handler | MC UI |
| Minigame / RPS | A（Minigame 串 0） | ❌ | 同上 | 小游戏桌 |
| Wedding | A（仅 `wedding%d`） | ❌ | 同上 | 婚礼 |
| PartyRace | B：`sub_56BD3C` | ❌ | decompile 活动 HUD | 组队竞速 |
| DualMobGauge | B：`sub_56C9B7` | ❌ | decompile | 双怪血条 |
| TreasureBox / raise / plant / NewYearsCard | A | ❌ | 按开放顺序定点 | 各玩法截图 |
| MapleTV / Admin* | A | ❌ | P4 后置 | — |

**本批结论：整批未达 C；主链验收不阻塞于此。**

---

## 4. 「没反编译完」操作清单（按收益排序）

完整复刻应推进的是 **主链达 C**，推荐顺序：

1. **StatusBar 行为 C**：在已有 `sub_8D01B2` 上补 handler / QS / 聊天（P0）  
2. **WorldSelect C**：落盘 `sub_624E13` 伪代码 + 展开卷构造（P0）  
3. **CharSelect C**：找到构造 EA → charInfo/排行控件表（P1）  
4. **Login C**：Title/Loading CreateCtrl（P1）  
5. **Core 深度**：`sub_83EA05` Quest、`sub_917546` UserList、SkillMacro 叶（P1）  
6. **Wave A**：先 decompile 已有 B 锚点（Hammer/Vega/Gachapon/`49D0B6`），再补 Delivery xref；Messenger/Family/StoreBank 用叶串+包入口挖 EA（P1）  
7. **Activities**：后置；已有 B 的 PartyRace/DualMobGauge 可随时升 C（P3）

**不要做：** `rea analyze` 整包 BeiDou.exe；不要以「还有 5 万函数未命名」衡量进度。

---

## 5. 复现命令（轻量）

```powershell
$env:REA_ANALYSIS_PROVIDER = 'ida'
$env:REA_IDA_MCP_CONFIG   = 'C:\Users\11036\.rea\ida-mcp.json'
$exe = 'E:\MXD\BeiDou-ClientV17.7\BeiDou.exe'

# 搜短关键字（完整路径常 0 命中）
& 'D:\software\nodejs-24\rea.cmd' search $exe 'ViciousHammer' --kind strings --provider ida --json
& 'D:\software\nodejs-24\rea.cmd' search $exe 'PartySearch' --kind strings --provider ida --json
& 'D:\software\nodejs-24\rea.cmd' search $exe 'Delivery' --kind strings --provider ida --json

# 升 C：对已知 EA decompile（REA 可能对 ok 字段报 schema，以 observations/code 为准）
& 'D:\software\nodejs-24\rea.cmd' decompile $exe 0x82A9BC --provider ida --json
& 'D:\software\nodejs-24\rea.cmd' decompile $exe 0x49D0B6 --provider ida --json
& 'D:\software\nodejs-24\rea.cmd' decompile $exe 0x624E13 --provider ida --json

# 禁止
# rea analyze $exe
```

证据落盘建议：`notes/evidence/r16/batch-<Name>/`（search json、decompile、控件表 txt、截图对）。

---

## 6. 相关文档

| 文档 | 角色 |
| --- | --- |
| [`r14-native-feature-catalog.md`](r14-native-feature-catalog.md) | **A 层**功能面 + 少量 B 锚点 |
| [`r15-baseline-ui-gap-vs-screenshots.md`](r15-baseline-ui-gap-vs-screenshots.md) | 截图差距；说明「不全是 REA 问题」 |
| 本文件 r16 | **明确哪些批次未达 C**（真正「没反编译完」） |
| [`../../v83-hud-quest/evidence/UIWindow.img.top-level.txt`](../../v83-hud-quest/evidence/UIWindow.img.top-level.txt) | 112 节点全集 |
| [`evidence/r16/`](evidence/r16/) | 本轮关键词 search 信封与摘要 |

---

## 7. 边界（再次强调）

- 只服务 **原生完整复刻**；`ijl15` / `自定义功能实现` 不在本缺口表。  
- 端侧 ✅ 只表示「当前能玩到某种程度」，**不**表示 REA 已达 C。  
- 验收以正确客户端截图 + s9 包序为准；C 层证据是对照手段，不是目标本身。
