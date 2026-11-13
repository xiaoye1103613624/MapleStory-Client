# 核心窗口面 · Angel 083/v83 带符号 IDB 全维校准清单（2026-11-08）

> 本文件是 goal `goal-11388c42-2502-43f0-b722-70abeb8d4307` 的**第三功能面**交付物（登录面 → 状态栏 HUD 面 → **核心窗口面**）。用户原始目标（m00690 逐字）：「这个ida反编译信息，是083的话，相较于之前的083反编译信息应该是详细了，我希望从登录功能开始，完整的校验和使用当前反编译内容进行校准一下所有的功能、业务、逻辑、ui、排版、样式、交互等所有内容是否正确、缺失、一致性」。
> 本面范围 = `UIWindow.img` 112 顶层节点里的「玩家常驻功能窗」族：Stat/Item/Equip/Skill/Quest/WorldMap/KeyConfig/UserInfo/UserList/CashShop/CashGachapon/Maker/ToolTip/Macro/SkillEx/Enchant/QuestAlarm/SkillMacroEx 等 62 个 Angel 具名类。
> **全程只读取证**：未改任何产品源码、未构建、未实机、未提交；按 `xiaoye-maple-story-dev` 门禁先证据→分析→决策点→一次性 Plan 后再进入实现。

---

## 0. 一句话结论

1. **Angel 与 BeiDou 是同一份 v83 二进制的不同符号状态**（本面 EA 交叉验证 **439/441 同 EA**，仅 `0x6cec47` / `0x8aa4e4` 两个 IDA 分析覆盖差异），与登录面 327/328、HUD 面 547/548 三面一致 → 上一轮无符号 IDB 校准的全部坐标/件表可直接复用，Angel 只补「类名 / ctrl id→业务语义 / 网格命中常量 / 脏词过滤 / 字体分派 / 跨窗联动」等符号语义与行为层。
2. **不缺窗口文件，也不缺排版件表**：当前端已按「12 个独立窗 + `UIMiscShells` 聚合壳（核心相关 20+ / 全文件约 70 壳声明）+ `IO/Components` 5 个 Tooltip 组件」落地；`ui-v83-window-restore/README.md:68-79` 的 W5–W9 改前→改后定案 + `asset-node-inventory.md` 147 行件表 + Angel `SP_*` 401 个 StringPool 索引（91 个 `UI_UIWINDOWIMG_*`）是**同一批节点的两种表示**，逐点吻合（`dword_BE27E4`=KeyConfig 命中表、`208×347`=Stat 底板、`175×304`=Equip 底板、`548×396`=Quest 整窗、8 片 `WORLDMAP_BORDER`=WorldMap）。
3. **结构性差异（本面最重要）**：Angel 083 是「单体 `CUIToolTip` 26 方法统一引擎 + 单体 `CUIStat` 8 + 单体 `CUIItem` 12 + `CCashShop` 66 + `CUIMacroSys` 32 格」，当前端是「按功能合并 + 自命名」的独立重写，**不是 62 类一一对应**；宏 32→5 槽、tooltip 26 方法→5 组件 1 纯虚 draw、`CWorldMapDlg 666×524`→无整机常量、管理器类（CMacroSysMan/CQuestMan/CItemInfo/CSkillInfo）→Data/Cache 层替代，均为形态/数量收缩而非纯缺失。
4. **真缺口收敛三层**：① 原生独有业务规则整族缺失（脏词/宏名过滤、技能加点 500ms 门闩、改键 5 分支、任务 4 智能分类、扩展背包 `0x7D5`）；② 显式预留/调试壳（任务窗四钮、宠物装备三页、`stub_job/stub_name`、`change_target()` 空 stub 死链、`UIMTS::update(){}`、UIMiscShells 底图+tip_label 壳）；③ 文档一致性（Q2 清单 C14/C34/C35 旧账、095 线地址污染 4 处、TooltipBackdrop 配色两版口径、`CLASSIC_SKILL_OFFSET` 文档 vs 源码冲突）。
5. **98 条判定分布 ✅49 / ⚠️36 / ❌10 / ⊘3**（§3.8 逐维 python 行级复核）。**s9 适配红线**：083 原生与 s9 服务端冲突处一律标注「改回原生会断联机，须交用户裁决，不得盲改」；**IMG 层阻塞**：orange-wz MCP 未接入本会话 Host，资源层以仓库内 `.nx` 节点引用 + `nx_inspect.py` dump + 正确端截图为准（三面一致）。

> ⚠️ **口径提示（2026-11-08 现场）**：`r16-rea-coverage-gap.md` / `r00` / `r14` / `01-parity-matrix-and-plan.md` / Q2 清单 §5·§16 的 ❌ 大表是 **2026-10-21 旧账**，本面按「源码逐字 > 2026-11 现场重算 > 2026-10 基线」重新取值；`ui-v83-window-restore/README.md:68-79` 已把绘制层逐窗定案，Angel 把构造 EA/类名推到具名层 → **「类名/行为层已达 C 前段，控件表逐点 + 包序 + 像素复核 + 实机验收仍未达 C」**。

---

## 1. 取证基线与门禁

### 1.1 端点与 IDB（每次先跑 `server_health` 读 `idb_path`，端口会变）

| 端口 | idb_path | module | 状态 | 本面用途 |
|---|---|---|---|---|
| 13339 | `E:\MXD\反编译IDA\v83\`Angel.i64` | `` `Angel.exe `` | `auto_analysis_ready=true` / `hexrays_ready=true` / strings 1281 | **权威校准源**：441 具名函数 / 62 类、296 函数逐字 Hex-Rays |
| 13337 | `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64` | `BeiDou.exe` | strings 3889 | EA 交叉验证 / 字符串锚点（Angel 字符串缓存少，明文锚点仍以此为准） |
| 13338 | headless `Maplestory95.i64` | — | 需 `database=<session_id>` | **本任务不用**（095 线，MEMORY.md 禁止作 083 依据） |

两端各 66 工具齐全（`decompile` / `lookup_funcs` / `list_funcs` / `xrefs_to` / `analyze_batch` / `py_eval` / `get_bytes` / `disasm` / `search_text`）。`decompile` schema `{addr, include_addresses}`；`py_eval` 须把逻辑包进 `def _scan(): ...; _scan()` + `new_locals=True`（规避 exec/eval 双作用域 `NameError`）；>50KB 大函数返回截断 + `/output/<uuid>.json` 端点，已批量补全。

### 1.2 三层证据门禁（REA→IDA→IMG）

| 层 | 本面落地 | 状态 |
|---|---|---|
| REA | 本会话 REA MCP 未注册；沿用 `D:\software\nodejs-24\rea.cmd` CLI + `REA_ANALYSIS_PROVIDER=ida` 的既有成果（`r16` B 锚点表、Maker `sub_822350` 达 C） | ✅ 历史证据 |
| IDA | Angel 13339 441 具名函数/62 类 EA 索引 + 296 函数逐字反编译 + 401 个 `SP_*` + BeiDou 13337 EA 交叉 439/441 | ✅ 本轮新增 |
| IMG | **orange-wz MCP 未接入**（`mcp_connector_status` 仅 `json-photoshop`；`~/.cursor/mcp.json:3-6` 有声明但 Host 未注册；`img-evidence-audit.md:7` 已知 `Index 16 out of bounds` 解析缺陷） | ⚠️ **记阻塞** |
| 替代 IMG 链 | `tools/nx_inspect.py` 对 `UI.nx` 的实测：`asset-node-inventory.md`(147 行) / `UIWindow.img.top-level.txt`(112) / `raw-nx-dumps/` 9 份 / `_w5_tooltip_root.txt`(149) / `r24/*_wz_nodes.txt` 3 份 / `w{0..12}/coords.md` 12 份 / `StatusBar.img.xml` / `correct_shots/` 7 张（含 `打开了S-E-I-K页面.png` 1280×720、`键盘设置.png` 629×369） | ✅ 三面最充分 |

### 1.3 产物清单

`evidence/` **6 文件**：`angel-corewindow-ea.json`(441/62)、`angel-corewindow-decompile.json`(296)、`angel-corewindow-decompile-full.json`(296 归一化，2,075,708 B)、`angel-corewindow-truncated.json`(7 大函数 receipt)、`angel-corewindow-facts.md`(4,937 行 / 253,252 B)、`angel-beidou-corewindow-ea-crosscheck.json`(439/441)。
`notes/` **5 文件**：`r01-angel-native-corewindow-facts.md`、`r02-main-agent-crosscheck-cancel.md`、`r03-corewindow-img-style-evidence.md`、`r04-current-corewindow-source-inventory.md`、`r05-prior-corewindow-doc-audit.md`。
采集脚本 **5 个**：`tools/ida_angel_corewindow_ea.py` / `_decompile.py` / `_normalize.py` / `_crosscheck.py` / `_facts.py`。

---

## 2. 口径与判定定义

### 2.1 六维 × 三判定 + 两辅助

- **六维**：A 功能面 / B 业务面 / C 逻辑面 / D UI 面 / E 排版面 / F 样式面 / G 交互面（7 组，用户「所有内容」的展开）。
- **判定**：`✅` 正确·一致（源码/IMG/IDA 三证至少两证吻合，或已销案）｜`⚠️` 需核·形态差异（s9 适配 / 实现路径差异 / 覆盖度未逐条证 / 文档未同步）｜`❌` 缺失·不一致（原生有、当前端无或显式 stub）｜`⊘` 明确不做（既有裁决 / 证据不足显式预留 / 服务端无后端）。
- **性质**：`[观察]` 逐字引文件行号｜`[推断]` 跨文档归并/冲突裁决/覆盖度归类｜`[待实机]` 明写需真机/截图。

### 2.2 证据优先级（沿用 Q2 §2.3，本面补 095 剔除）

`协议层 2026-11-08 权威矩阵` > **源码逐字 `emplace`/`.dispatch()`/常量行** > `2026-11 现场重算（ui-v83-window-restore/README.md:68-79、parity-gap-2026-11）` > `真机记录` > `Wave 销案笔记` > `2026-10-21 基线（r00/r14/r15/r16/01）` > `gap-inventory.md`（旧 head `44d0d5d9ca`）。
**MEMORY.md critical 硬约束**：参照基准固定 083/v83，`Maplestory95.i64` / port-095 / `0x867b90` / `0x7CCC90` / `0x7CE520` / `0x7CC4F0` / `ExpeditionIntermediary` 等 **095 线地址仅登记存在、不作 083 判定依据**。

### 2.3 本面 7 处冲突取值表

| # | 冲突 | 取值 | 依据 |
|---|---|---|---|
| 1 | `feature-gap:141` C14「5 tooltip 组件指向不存在的 `UIToolTip.img/Item`」 | **已销案**：`PLAN.md:20,34` 是修复前诊断，`README.md:74`/`CHECKLIST.md:70` 已清零，源码 `EquipTooltip.cpp:53` = `UIWindow.img/ToolTip/Equip` | r03 §9.1 + r05 §9.4（三路一致） |
| 2 | `feature-gap:140` C13「SkillEx/宏/附魔/QuestAlarm 深度 A/B 层」 | **EA 缺口销案、深度 🟡 未闭环** | Angel 已给全部具名 EA；`UIMiscShells` 仍是底图+tip_label 壳 |
| 3 | `feature-gap:161` C34 / `:162` C35「CashShop 礼物/礼包未接、购买未完整」 | **入口/素材/协议销案，深度 🟡** | r02 §8.4b 逐字证 `UICashShop.cpp` Take/Put/Buy/Package 0x1E/4 槽位/wishlist/3 余额/UICoupon 全接线；stub 仅 2 Text |
| 4 | TooltipBackdrop 配色：`README.md:74` 黑 0.9α+灰边 vs `TextTooltip.h:28-47` (79,79,119) 紫底亮边 | **以源码 `TextTooltip.h:28-47` 为真值**，文档记中间态 | 门禁「checked-in source > docs」 |
| 5 | `CLASSIC_SKILL_OFFSET`：`README.md:78` (7,99) vs `UISkillBook.h:173` (10,87) | **以源码 (10,87) 为真值**，文档 (7,99) 列 D6 待实机裁决 | 主 agent 20:39 直读复核（r05 §9.2） |
| 6 | Q2 §16 #9「`channel_count<2` P0」/ #8「TemporaryStatView 全缺」 | 核心窗口面**无同型硬缺陷**；`TemporaryStatView` 属真全缺且 Angel 本面未补 EA | r05 §5 |
| 7 | 095 线地址污染（`UIStatsInfo.cpp:146` 等 4 处） | 换引 Angel `CUIStat::OnCreate @ 0x8c4c7f` / `CUIItem::OnChildNotify @ 0x81d01f`；坐标结论保留 | r02 §5 + r04 §0 + r05 §0 |

---

## 3. 六维对照总表（98 条）

> 每行 `判定` 列为唯一状态标记；证据列全部带相对路径:行号，Angel 侧 EA/伪代码见 `notes/r01`，当前端源码见 `notes/r04`，IMG 见 `notes/r03`，文档四态见 `notes/r05`。

### 3.1 · A 功能面（14）

| 编号 | 判定 | 校验项 | 证据 |
|---|---|---|---|
| A1 | ✅ | E/I/S/K 四键完整装配装备/物品/能力/技能窗（TYPE+emplace 闭环） | `IO/UIStateGame.cpp:319-355` |
| A2 | ✅ | F 键学院 `OpenFamilyPacket()+emplace<UIFamily>()` / G 键家族落 UIUserList 第 3 页签 | `IO/UIStateGame.cpp:296-309,361-393` |
| A3 | ✅ | 现金商城进出（EnterCashShop / BtExit fadeout 回城） | `IO/UIStateGame.cpp:513-517`、`UICashShop.cpp:305-330` |
| A4 | ✅ | 现金扭蛋扫 CASH 522 段 → `UseCashItemBasicPacket` | `UICashGachapon.cpp:16-24,73-88` |
| A5 | ✅ | 制造窗 Maker.img 配方 → `MakerCreatePacket` | `UIMaker.cpp:91-107,175-192` |
| A6 | ✅ | 任务窗三页签/折叠/详细/放弃 `QuestActionPacket(3)` | `UIQuestLog.cpp:926-950` |
| A7 | ✅ | 世界地图三子面板 + 付费传送四入口 | `UIWorldMap.cpp:191-230`、`UIWorldMap.h:122` |
| A8 | ✅ | 改键窗 CLOSE/DEFAULT/DELETE + 两套默认键表 | `UIKeyConfig.cpp:718-757`、`UIKeyConfig.h:134-229` |
| A9 | ✅ | 角色信息窗 Family/Party/Item/Book 多分支发包 | `UICharInfo.cpp:322-371` |
| A10 | ❌ | 扩展背包 `0x7D5=SP_4501_EXPAND_INVENTORY` 当前端无分支 | Angel `CUIItem::OnChildNotify 0x81d01f` vs `UIItemInventory.cpp:1872` 仅注释 |
| A11 | ❌ | 任务窗 MARKNPC/OK/NO/ALERT 四钮全 `return NORMAL` | `UIQuestLog.cpp:951-960`（r04 §5.A 逐字） |
| A12 | ❌ | 宠物装备三页 BtPet1/2/3 穿戴/卸下未接 | `UIEquipInventory.cpp:622` |
| A13 | ❌ | `change_target()` 空 stub → 6 个频道快捷键死链 | `UIChatBar.h:122-123`、`UIStateGame.cpp:471-482`、`UIUserList.cpp:916` |
| A14 | ⊘ | NOTIFIER（v83 L）服务端无后端，按门禁不做假窗 | `UIStateGame.cpp:310-318` |

### 3.2 · B 业务面（14）

| 编号 | 判定 | 校验项 | 证据 |
|---|---|---|---|
| B1 | ❌ | `CCurseProcess::ProcessString` 脏词过滤整族缺失（HUD `CChatHelper::TryChat` + 本面 `CUIMacroSys::OnButtonClicked` 两处原生都有） | `r01 §2`、`r02 §2.3`；全仓 `*.cpp` grep 零命中 |
| B2 | ⚠️ | 技能加点 `CanSendExclRequest(500,0)` 独占门闩 + `SP_3509/3510/3511` 三档前置当前端未逐条核 | Angel `CUISkill::OnSkillLevelUpButton 0x8acf1a` vs `UISkillBook.cpp:547-560` |
| B3 | ⚠️ | `CUIStat` 6 掩码 `0x800/0x2000/0x40/0x80/0x100/0x200` ↔ `AutoAssignApPacket` StatCode 覆盖度 | `r01 §2`、`UIStatsInfo.cpp:726-752` |
| B4 | ⚠️ | `CUIStat::OnButtonClicked` 10 case ↔ 当前端 8 有效 case | `r01 §2` vs `UIStatsInfo.cpp:638-697` |
| B5 | ⚠️ | `CCashShop` 66 方法礼物/礼包/愿望/点券深度分支未逐条重判（入口已销案） | `r02 §8.4b`、`feature-gap:161` C34 |
| B6 | ⚠️ | `CUIQuestInfo` 4 个 `TryShow*Quest` 智能分类 ↔ 当前端 3 静态页签 | `r01 §2`、`UIQuestLog.cpp:41,50-53,694-702` |
| B7 | ⚠️ | `CUIMacroSys` 32 格 ↔ `SkillMacros::MAX_SLOTS=5` / `MACRO_COUNT=5` / `VISIBLE_ROWS=3` | `r01 §2`、`Character/SkillMacros.h:23`、`UIMiscShells.h:648-650` |
| B8 | ⚠️ | `CUIToolTip` 26 方法单体引擎 ↔ 5 组件（`Tooltip.h:46` 仅 1 纯虚 draw） | `r01 §2`、`IO/Components/Tooltip.h:26-46` |
| B9 | ⚠️ | `CUIKeyConfig::OnButtonClicked` 5 分支（保存/恢复默认+ReDrawQuickslot/清空 445B/跨窗重绘） | `r01 §2` vs `UIQuickslotConfig.cpp:92-144` |
| B10 | ⚠️ | `CUIItemMaker` 22 方法 ↔ UIMaker 仅 1 钮 + s9 兜底静态表 | `r05 §2`、`UIMaker.cpp:107`、`MakerPackets.h:35-40` |
| B11 | ⚠️ | `CWorldMapDlg` 666×524 整机布局 ↔ 当前端仅 240×490 子面板常量 | `r01 §2`、`UIMapInfoPanel.cpp:58-94` |
| B12 | ✅ | 任务放弃复用 `QuestActionPacket(3)` 不发明新包（门禁合规） | `UIQuestLog.cpp:947-949` |
| B13 | ✅ | CashShop 购买/礼包 0x1E/愿望清单/三余额/优惠券/4 槽位包逐字接线 | `UICashShop.cpp:230-234,371-400,430-469,618-630,770,795-844,872-880` |
| B14 | ✅ | `WORLD_MAP_PLAYERS(376)` 真机闭环 2026-11-05（SEND 277→RECV 376） | `feature-gap-inventory-2026-11/README.md:73`、`world-map-players/README.md` |

### 3.3 · C 逻辑面（14）

| 编号 | 判定 | 校验项 | 证据 |
|---|---|---|---|
| C1 | ⚠️ | Angel 62 类 ↔ 当前端**非一一对应**（12 独立窗 + 20+ 壳 + 5 组件 + 单一 EnumMap 调度） | `r04 §8①`；全库原生类名仅 8 命中且全是注释/095 引用 |
| C2 | ⚠️ | `CUIStat` ctrl id 2000–2009 与当前端 `UIStatsInfo.h:113-129` Buttons 枚举不是同一套编号 | `r02 §2.1`、`r01 §2` |
| C3 | ⚠️ | `CUIItem::OnChildNotify` 2004/2005 ↔ 当前端 0x7D3/0x7D4（且注释引 095 `0x7CC4F0`） | `r01 §2`、`r02 §2.2`、`r04 §0` |
| C4 | ✅ | `CUIKeyConfig::GetShortCutIndexByPos` 命中表 `dword_BE27E4..BE2C6C` 与 `ida-layout-tables.md dword_BE27E0` / `Icon.h:105-106` 32×32 三证同表 | `r01 §2`、`r03 §7①` |
| C5 | ✅ | `CUIMacroSys::GetIndexByPos` 网格 65/39/260 · 44/37/155 · 拖动 145–177→第3列 已取 | `r01 §2`、`angel-corewindow-facts.md:813-823` |
| C6 | ✅ | `CUIMacroSysEx::GetIndexByPos` **不同网格** 223/163/146–178 已标注勿混 | `angel-corewindow-facts.md:954-965`、`r05 §4④` |
| C7 | ✅ | `CUISkill::OnSkillLevelUpButton` 三档前置 + `get_novice_skill_point` 已取逐字 | `r01 §2` |
| C8 | ✅ | `CWorldMapDlg::MakeBorder` 八片 `SP_2281..2288` + `SP_2289` title 已取 | `r01 §2`、`r03 §7⑦` |
| C9 | ✅ | `CCashShop` 子窗偏移 1360/5384/5560/7644 已取 | `r01 §2` |
| C10 | ❌ | `sub_8E49B5`(0x215f ToolTip) / `sub_83EA05`(0xaa0 Quest) 不在 Angel 具名方法 EA 表内，待映射 ctor/helper/数据层 | `r05 §4⑪`、`r02 §2.4` |
| C11 | ❌ | ViciousHammer `sub_82A9BC` / VegaSpell `sub_82BB83` 的 Angel 具名映射本轮未命中（CashGachapon 已吻合） | `r05 §2`、`r16:63-65,196-198` |
| C12 | ❌ | `CTemporaryStatView` 类/方法 EA 本面未补采（仅 `SP_3550` 一次出现） | `r05 §5`、`angel-corewindow-facts.md:188` |
| C13 | ⚠️ | 095 线地址污染 4 处（`UIStatsInfo.cpp:146` / `UIItemInventory.h:49-50` / `.cpp:124,530-532` / `InventoryHandlers.cpp:56,71`） | `r02 §5`、`r04 §0`、`r05 §0` |
| C14 | ✅ | EA 交叉验证 439/441 同 EA（三面一致），`CUICashItemGachapon::OnCreate 0x999e4d→sub_999E4D 0x43c` 与 r16 B 锚点逐字吻合 | `evidence/angel-beidou-corewindow-ea-crosscheck.json`、`r02 §4` |

### 3.4 · D UI 面（14）

| 编号 | 判定 | 校验项 | 证据 |
|---|---|---|---|
| D1 | ✅ | 12 个核心窗 TYPE 全部绑定 `UIElement::Type` | `r04 §1.1`、`IO/UIElement.h:162-203` |
| D2 | ✅ | `UIMiscShells` 聚合壳（核心相关 20+ / 全文件约 70 声明）TYPE 已注册 | `UIMiscShells.h:12-970`、`r04 §1.2` |
| D3 | ✅ | Tooltip 以 `IO/Components/` 5 组件 + `Tooltip.h:26` 统一接口承接 | `r04 §1.3/§4` |
| D4 | ✅ | `UI["UIToolTip.img"]["Item"]` 已清零定案（PLAN.md:20,34 为修复前诊断） | `ui-v83-window-restore/README.md:74`、`CHECKLIST.md:70`、`EquipTooltip.cpp:53` |
| D5 | ✅ | `UICashShop` 非 stub 窗（stub 仅 `:100/:102` 两个 Text） | `r02 §8.4b`、`r04 §0` |
| D6 | ✅ | 快捷键 8 键 + UISkillMacroEx 一屏三行 + UIShortCut 8 颗真按钮 | `UIMiscShells.h:634-640,643-669`、`UISkillBook.cpp:524-541` |
| D7 | ⚠️ | 任务窗四钮显式预留（决策性缺口，非漏实现） | `UIQuestLog.cpp:955-959`、`open-issues.md:818-824` |
| D8 | ⚠️ | 宠物装备三页 / POCKET 槽「无判据 → 不改色」显式预留 | `UIEquipInventory.h:104-105,119-120`、`.cpp:567,622` |
| D9 | ⚠️ | `cashshop.stub_job` / `stub_name` 本地化占位 | `UICashShop.cpp:100,102`、`locale/zh-CN.ini:344-347` |
| D10 | ⚠️ | `charinfo.medal_stub` + 皮肤切换/资源装载显式预留 | `UICharInfo.cpp:171,460` |
| D11 | ⚠️ | `CUIToolTip` 单一窗口类无（`glob IO/UITypes/UIToolTip.*` No files found） | `r04 §1.3/§8①` |
| D12 | ⚠️ | 管理器类 CMacroSysMan / CQuestMan / CItemInfo / CSkillInfo 无同名窗口，由 Data/Cache 层替代 | `r04 §3.3/§8①` |
| D13 | ⚠️ | `UIMTS::update(){}` 空体 / `UIOptionMenu` 退化空壳 / 下拉展开只 log | `feature-gap:128-130`、`open-issues.md:686-689` |
| D14 | ⚠️ | `CUIQuestAlarm::Draw` 等 7 个 >50KB 超大函数已补全但**未逐行人工核完** | `r01 §5`、`r05 §1.2` |

### 3.5 · E 排版面（16）

| 编号 | 判定 | 校验项 | 证据 |
|---|---|---|---|
| E1 | ✅ | Angel `SP_*` 401 个 StringPool 索引 ↔ 仓库 `UIWindow.img/*` 明文节点名是同批节点两种表示 | `r03 §7`、`r01 §1` |
| E2 | ✅ | KeyConfig `dword_BE27E4..BE2C6C` ↔ `ida-layout-tables.md:3-30 dword_BE27E0` ↔ `Icon.h:105-106` 三证同表 | `r03 §7①` |
| E3 | ✅ | Equip `175/208×304` + `BtPet1/2/3` ↔ Angel `ToggleDragonEquip/TogglePetEquip` 解释两套宽 | `r03 §7②`、`asset-node-inventory.md:43-53` |
| E4 | ✅ | Stat `208x347`/`BtAuto 100x35`/`BtDetail 46x19` ↔ `UIStatsInfo.cpp:155,159,358` 逐点 | `r03 §7③` |
| E5 | ✅ | Item `BtFull/BtGather/BtSmall/BtSort/BtCashshop` ↔ `SP_1851/1852/5580/5581/4500` 全对上 | `r03 §7④` |
| E6 | ✅ | Skill `BtSpUp/skill0,1/CoolTime/tip0-2` ↔ `SP_1923..1926/2660` | `r03 §7⑤` |
| E7 | ✅ | Quest `548x396` 整窗 + 6 颗 Bt ↔ `SP_1897..1901/2664/2665/3232` | `r03 §7⑥`、`w10-quest/coords.md:4-9` |
| E8 | ✅ | WorldMap `Border{0..7}+title` ↔ `SP_2281..2289`（正好 8 片 + title） | `r03 §7⑦` |
| E9 | ✅ | `ui-v83-window-restore/README.md:68-79` W5–W9 改前→改后逐窗定案（能力 13 行数值/收缩钮/背包 tab 0–4/技能 tab 间距 33） | `r05 §9.1` |
| E10 | ✅ | 快捷格 151/466/571 + 格 28/列距 35/行距 34/首格 (9,9) ↔ HUD 面 `UIStatusBar.cpp:106-118` 逐值 | `README.md:68`、`r05 §9.1` |
| E11 | ✅ | 背包 `BtGather/BtSort` 叠同坐标 `(W−31,4)` 是原生设计非缺陷 | `README.md:77`、`open-issues.md:723-726` |
| E12 | ✅ | 收缩钮真身 `Basic.img/BtHide` (405,326) 定案（旧取不存在的 `main/BtDetailClose`） | `README.md:75` |
| E13 | ⚠️ | `CLASSIC_SKILL_OFFSET` 文档 (7,99) ↔ 源码 `UISkillBook.h:173` (10,87) 冲突 | `r05 §9.2`（主 agent 20:39 直读复核） |
| E14 | ⚠️ | `CWorldMapDlg 666×524` / z=10 / bScreenCoord=1 与当前端 `UIWorldMap` 八片边框拼接未对点 | `r01 §2`、`r05 §4⑫` |
| E15 | ⚠️ | `BtUIClose/2` 32×15 按 `dim.x()-21` 摆放越窗 11px | `asset-node-inventory.md:146`、`open-issues.md:565` |
| E16 | ⚠️ | 三窗「窗内按钮坐标仍缺（无 IDA、原生参考帧刚补齐）⇒ 坐标落定前不改根」 | `asset-node-inventory.md:100`、`r05 §1.2` |

### 3.6 · F 样式面（12）

| 编号 | 判定 | 校验项 | 证据 |
|---|---|---|---|
| F1 | ⚠️ | TooltipBackdrop 配色两版口径（黑 0.9α+1px 灰边 vs `Color(79,79,119)` 紫底+亮边「无灰色外框」），以源码为真值 | `README.md:74` / `open-issues.md:479` vs `TextTooltip.h:28-47` |
| F2 | ✅ | 5 个 tooltip 组件共用 `TooltipBackdrop` 程序描边（非贴图九切片） | `TextTooltip.h:28-59`、`r03 §9.1` |
| F3 | ✅ | `MapleButton` 4 态 + `src["normal"]["0"]` origin + `animations[state].update(6)` | `MapleButton.h:28-62`、`.cpp:45-89` |
| F4 | ✅ | `MapleFrame` 九切片 c/e/ne/n/nw/w/sw/s/se + xtile/ytile 取位图尺寸 | `MapleFrame.h:24-44`、`.cpp:26-72` |
| F5 | ✅ | `Icon` cell 32×32 / `StatefulIcon` 3 态 / `IconCover` 30×30 | `Icon.h:32-114`、`StatefulIcon.h:27-33` |
| F6 | ✅ | `Gauge` 4 态 `DEFAULT/CASHSHOP/WORLDSELECT/CLIP`（CLIP 用于 v83 StatusBar HP/MP） | `Gauge.h:27-53`、`.cpp:70-93` |
| F7 | ✅ | `Graphics/Text.h` 12 档字号 A11M..A18B + Alignment/Background | `Graphics/Text.h:33-61` |
| F8 | ✅ | `Charset::charset_char()` Lbracket→'[' / Rbracket→']' / slash→'/' / percent→'%' | `Charset.cpp:30-43` |
| F9 | ✅ | `ScrollingNotice` width 800 / 23px 黑底 0.535α / YELLOW A12M / 滚出左界连底消失 | `ScrollingNotice.cpp:22-30,64-74` |
| F10 | ⚠️ | `CUIToolTip::GetFontByType` case 1–13+ 取 `this+265..278` IWzFont ↔ 当前端 12 档字号 | `r01 §2`、`Graphics/Text.h:33-48` |
| F11 | ⚠️ | locale `equip.*` 已译中文；cashshop/charinfo/maker/enchantskill/skillex/questalarm/macro/macroskill/skillup/questicon 全英文原值占位（用户 2026-11-02 决策 B） | `locale/zh-CN.ini:327-332,393-397,514-520,570-577,604-621` |
| F12 | ⚠️ | `UIKeyConfig.cpp:731` 硬编码英文 `"Would you like to revert to default settings?"`（零风险可改） | `r04 §1.1/§5.A` |

### 3.7 · G 交互面（14）

| 编号 | 判定 | 校验项 | 证据 |
|---|---|---|---|
| G1 | ✅ | E/I/S/K/F/G/O/WORLDMAP/QUESTLOG/KEYBINDINGS/CHARINFO/PROFESSION 快捷键完整装配 | `IO/UIStateGame.cpp:296-355,397-402,436-544` |
| G2 | ✅ | G 键「家族」= UIUserList 第 3 页签（083 原版口径，非独立 UIGuild 浮窗） | `IO/UIStateGame.cpp:361-364`、`UIUserList.h:176-179` |
| G3 | ⚠️ | `CUIStat::OnButtonClicked` 10 case ↔ `UIStatsInfo.cpp:638-697` 8 有效 case | `r01 §2`、`r04 §8②` |
| G4 | ⚠️ | `CUIItem::OnChildNotify` 2004/2005 ↔ 当前端 0x7D3/0x7D4（消息 id 口径不同，095 线注释） | `r02 §2.2`、`r04 §0` |
| G5 | ⚠️ | `CUIKeyConfig::OnButtonClicked` 5 分支（含跨窗 `InvalidateRect(StatusBar)` + `ReDrawQuickslot`）当前端覆盖度未证 | `r01 §2`、`r05 §4⑥` |
| G6 | ⚠️ | 宏键位拖拽 `X∈[145,177]→第3列` / `CUIMacroSysEx 146–178` 两套几何当前端未逐点核 | `r05 §4③④`、`angel-corewindow-facts.md:813-823,954-965` |
| G7 | ⚠️ | `CanSendExclRequest(500,0)` 独占门闩 + 三档前置技能点拦截当前端未核 | `r01 §2`、`UISkillBook.cpp:547-560` |
| G8 | ✅ | `UISkillBook BT_MACRO` 只开 `UISkillMacroEx` + `anchor_right_of(*this,-3)`（修「一点击糊满屏」） | `UISkillBook.cpp:524-541` |
| G9 | ⚠️ | `UIQuestLog` GIVEUP 已接、DETAIL/COLLAPSE/TAB 已接，四钮预留 | `UIQuestLog.cpp:926-960` |
| G10 | ❌ | `change_target()` 空 stub → SAY/PARTYCHAT/FRIENDSCHAT/GUILDCHAT/ALLIANCECHAT/TOSPOUSE 6 键死链 | `UIChatBar.h:122-123`、`UIStateGame.cpp:471-482` |
| G11 | ❌ | `CUIMacroSys::OnButtonClicked` 宏名保存走 `ProcessString` 脏词过滤 + `SP_3296` 保存提示整族缺失 | `r01 §2`、`r02 §2.3` |
| G12 | ⊘ | 任务窗 MARKNPC 需 npc→map 反查表 + 世界地图标记入口，「无任何封包/IDA 证据 ⇒ 不发明协议」 | `UIQuestLog.cpp:955-959`、`open-issues.md:818-824` |
| G13 | ⊘ | NOTIFIER（v83 L）服务端无后端 intentionally inert；场景变更/商城暂未支持同族 | `UIStateGame.cpp:310-318`、`open-issues.md:61-62,861` |
| G14 | ✅ | classic `is_in_range` 命中区上移 100px 已定位代码根因（聊天栏「点了没反应」） | `ui-v83-window-restore/README.md:71` |

### 3.8 · 判定汇总（逐维，python 行级复核）

| 维 | 条数 | ✅ | ⚠️ | ❌ | ⊘ |
|---|---|---|---|---|---|
| A 功能 | 14 | 9 | 0 | 4 | 1 |
| B 业务 | 14 | 3 | 10 | 1 | 0 |
| C 逻辑 | 14 | 7 | 4 | 3 | 0 |
| D UI | 14 | 6 | 8 | 0 | 0 |
| E 排版 | 16 | 12 | 4 | 0 | 0 |
| F 样式 | 12 | 8 | 4 | 0 | 0 |
| G 交互 | 14 | 4 | 6 | 2 | 2 |
| **合计** | **98** | **49** | **36** | **10** | **3** |

⚠️36 里：12 条 s9 适配须裁决、9 条形态/实现路径差异、6 条覆盖度未逐条证、5 条文档未同步/冲突、3 条超大函数未逐行核完、1 条样式口径冲突（F1）。❌10 收敛 5 类：业务规则整族（B1/G11）、空 stub 死链（A13/G10）、预留控件（A10/A11/A12）、原生 EA 待补采（C10/C11/C12）、管理器收缩（并入 ⚠️ 计）。

---

## 4. Angel 补上的 10 条关键新发现（BeiDou 无符号 IDB 拿不到）

1. **`CUIStat::OnButtonClicked @ 0x8c522f` 10 case + 6 掩码**：`2000→SendAbilityUpRequest(2048)` / `2001→0x2000` / `2002→64` / `2003→128` / `2004→256` / `2005→512` / `2006→ToggleDetail` / `2007+2008→AutoApUp(1)` / `2009→AutoApUp(0)` → 当前端 `AutoAssignApPacket` StatCode 是不同体系。
2. **`CUIItem::OnChildNotify @ 0x81d01f`**：`0x7D4=SP_698_ITEM_SORT` / **`0x7D5=SP_4501_EXPAND_INVENTORY`**（当前端无分支）；`2000+a3==500→OnTabChanged`、`a3==101` 分发。
3. **`CUIMacroSys::GetIndexByPos @ 0x8b8de3`**：32 格网格，行 Y 65 步进 39 到 260 / 列 X 44 步进 37 到 155 / 拖动 `X∈[145,177]→第3列`；`CUIMacroSysEx` 另有一套 223/163/146–178。
4. **`CUIMacroSys::OnButtonClicked @ 0x8aa134`**：宏名保存走 `CCurseProcess::ProcessString(Buffer,0,1)` + `SP_3296_IT_IS_SAVED` / `SP_272_CURSING_IS_NOT_ALLOWED`（与 HUD 面 `CChatHelper::TryChat` 构成脏词整族）。
5. **`CUIKeyConfig::OnButtonClicked @ 0x8335d0` 5 分支**：`1=Save` / `2=InvalidateRect(CUIStatusBar)` / `2000=DoModal→Default+ReDrawQuickslot` / `2001=memset(dword_BED5A0+4,0,0x1BD=445)` / `2002=sub_836C9B` + `UI_Close(5)`；`GetShortCutIndexByPos @ 0x833a65` 命中表 `dword_BE27E4..BE2C6C` + `'6'→42/'Y'→29/'Z'→56`。
6. **`CUISkill::OnSkillLevelUpButton @ 0x8acf1a`**：`CanSendExclRequest(500,0)` 500ms 独占门闩 + `SP_3509/3510/3511` 三档前置技能点拦截 + `_ZtlSecureFuse<short>` / `get_novice_skill_point`。
7. **`CUIToolTip` 26 方法单体引擎**：`GetFontByType` 13+ 档 IWzFont 分派 + `SetToolTip_{Bundle,Package,Pet,Ring,Skill,SlotInc,WorldMap,MacroSysSkill,String_MultiLine}` 各支；`SetToolTip_Equip @ 0x8e8252` size `0x1e47`。
8. **`CWorldMapDlg::CreateWorldMapDlg @ 0x9eb366`**：`CreateDlg(67,0,666,524,10,1,0)` ⇒ 666×524 / z=10 / bScreenCoord=1；`MakeBorder` 八片 `SP_2281..2288` + `SP_2289` title；**无在线玩家列表 UI**。
9. **`CCashShop` 66 方法**：`OnBuyPackage`/`IsUsableItemGift`/`OnGiftMateInfoResult`/`SendGiftsPacket`/`OnQueryCashResult`/`OnSetWish`/`OnRemoveWish`/`ProcessBuy` + 4 子窗偏移 1360/5384/5560/7644。
10. **`SP_` StringPool 401 个（91 个 `UI_UIWINDOWIMG_*`）** 与仓库 IMG 件表逐一对上；`CMacroSysMan` 29B/条 + `FlushToSvr COutPacket(110)`；`CUISkillEx::OnButtonClicked COutPacket(165)` + `ShiftMacroUIState` 用 `SP_4345/4346`。

---

## 5. 与上一轮 BeiDou 校准 / Q2 清单的关系（四态）

### 5.1 销案（旧账 ❌/🟡 → 本面 ✅）

- **C14 `UIToolTip.img/Item`**：`PLAN.md:20,34` 修复前诊断，`README.md:74`/`CHECKLIST.md:70` 已清零 → 源码 `EquipTooltip.cpp:53` 走 `UIWindow.img/ToolTip/Equip` + `TooltipBackdrop`。三路子代理一致同意。
- **CashShop「购买/结算未接 / 点了没反应」**：`open-issues.md:899-901` 作废 40 vs 229 误判；r02 §8.4b 逐字证 `UICashShop.cpp` 71 命中全接线；`asset-node-inventory.md:94` 商城 23 根素材齐。
- **Skill/SkillEx/Macro/Enchant「无 EA」**：Angel 已给 `CUISkill`5/`CUISkillEx`3/`CUIMacroSys`8/`CUIMacroSysEx`8/`CMacroSysMan`8/`CUIEnchantDlg`2 全具名 EA。
- **C8 连锁任务页码图标**：`open-issues.md:785-789` / `quest-page-icons/README.md:3` 已闭环（真机点测仍待）。
- **C4 背包 Gather/Sort 叠槽**：`open-issues.md:723-728` / `README.md:77` 定案「叠槽是原生设计、按钮表示下一步动作」。
- **`WORLD_MAP_PLAYERS(376)`**：2026-11-05 真机闭环（`feature-gap:73`）。
- **B8 tooltip 五条实测**：底 α=0.90 / DEX·INT y 71→77 / `reqstr.size()<3` 守卫 / 职业名同原点叠绘回退 / 英文串证伪「原生就是英文」。

### 5.2 推翻（本面新证据改变结论）

- **「5 tooltip 组件指向不存在的根」** → 实为已修，属文档层未同步（`feature-gap:141`、r02 §2.4 须回写）。
- **`CLASSIC_SKILL_OFFSET (10,87)→(7,99)`** → 源码仍是 (10,87)，文档与源码冲突，列 D6。
- **TooltipBackdrop 配色** → 以 `TextTooltip.h:28-47` 紫底亮边为真值，`README.md:74` 黑底灰边是中间态（F1）。
- **「`ShortCut` 11 钮一个都没实例化」「键盘设定窗没按钮」** → `README.md:69` 已实例化 11 颗 + 补 5 钮坐标。
- **「背包中间钮有一颗在本包不存在」** → `open-issues.md:706-707` 证伪，六颗 Bt 全在且各 4 态。

### 5.3 新增（Angel 才看得见的真缺口）

- 脏词/宏名过滤整族（B1/G11）；技能加点 500ms 门闩（B2/G7）；改键 5 分支 + 跨窗重绘（B9/G5）；任务 4 智能分类（B6）；扩展背包 `0x7D5`（A10）；`CUIToolTip` 26 方法收缩（B8/D11）；宏 32→5 格（B7）；`CWorldMapDlg 666×524` 未对点（B11/E14）；`sub_8E49B5`/`sub_83EA05` 待映射 + `ViciousHammer`/`VegaSpell`/`CTemporaryStatView` 待补采 EA（C10–C12）。
- 095 线地址污染 4 处（C13）；`CLASSIC_SKILL_OFFSET` 文档/源码冲突（E13）；TooltipBackdrop 两版配色（F1）。

### 5.4 遗留（挂账未重判，不假装闭环）

- `CUIQuestAlarm`「未接」：`r00:75`/`r14:102`/`feature-gap:140` 仍挂账，r40 已铺壳 + Angel 6 方法 EA 已取，但 `Draw` >50KB 未逐行核完。
- `feature-gap:161` C34 / `:162` C35 礼物/礼包深度 + stub 文案 🟡。
- `TemporaryStatView` 全缺（Q2 §16 #8）、`MobDropData` stub loader、`UIMTS::update(){}`、`UIOptionMenu` 退化空壳。
- 7 个 >50KB 超大函数未逐行人工核完；296 反编译是精选（非全部 441）。
- ⏳ 真机挂起：equip-compare 回归 4 步、连锁任务 Gauge spot、Gather↔Sort 切钮、技能窗三窗 Δ=1、五页签 `layout.py mark/bias`（本环境抓屏全黑）、公会 BBS 完整度、`pending-live-tests/README.md` §15 全表。

---

## 6. 决策点（D1–D14，每条带推荐默认值）

| 决策点 | 问题 | 推荐默认 | 影响面 |
|---|---|---|---|
| **D1** | 是否把 12 独立窗 + 20+ 壳 + 5 组件合并回 Angel 62 类一一对应？ | **保持现有重构**（合并会重写全部核心窗、风险极高且破坏已实机对齐的排版） | 全局架构 |
| **D2** | 083 原生语义 vs s9 联机，冲突处以谁为准？ | **保持 s9 联机优先**，Angel 仅作语义校准源，差异逐条标注「改回原生会断联机」 | 全部 ⚠️ s9 适配项 |
| **D3** | 脏词过滤整族（HUD 聊天 + 本面宏名）是否补？ | **先确认 s9 服务端是否已实现**，避免双罚；确认后再定客户端是否补 | B1/G11 |
| **D4** | 扩展背包 `0x7D5` / 国服 `0x7D3=BtGather` 如何处理？ | **保留 s9/国服适配**，仅记录差异，不补原生分支 | A10/G4 |
| **D5** | 095 线地址污染 4 处是否换依据？ | **零风险换注释**：改引 Angel `CUIStat::OnCreate 0x8c4c7f` / `CUIItem::OnChildNotify 0x81d01f`，坐标结论保留 | C13 |
| **D6** | `CLASSIC_SKILL_OFFSET` 文档 (7,99) vs 源码 (10,87)？ | **不改源码**，先实机比对技能窗文字/贴图 12px 错位再定 | E13 |
| **D7** | TooltipBackdrop 配色两版以谁为准？ | **以源码 `TextTooltip.h:28-47` 为真值**，只回写文档中间态说明 | F1 |
| **D8** | `CUIStat` 6 掩码 / `CUISkill` 500ms 门闩 / `CUIKeyConfig` 5 分支覆盖度？ | **先出逐条对照清单**，不改代码；确认 s9 是否需要 | B2/B3/B9/G5/G7 |
| **D9** | 宏系统 32 格 → 当前端 5 槽是否补？ | **保持 5 槽**（`SkillMacros::MAX_SLOTS=5` 与 s9 `SKILL_MACRO(110)` 对齐） | B7/G6 |
| **D10** | `CWorldMapDlg 666×524` 是否改当前端世界地图？ | **保持 s9 布局**，原生「无在线玩家列表」结论已坐实，右上列表为 s9 挂接，不判缺失 | B11/E14 |
| **D11** | Q2 清单 C14/C34/C35/C13 及 §16 #8/#9 等旧账是否回写？ | **交用户裁决后统一回写**（本面只出销案证据，不擅改 Q2 文档） | 文档一致性 |
| **D12** | 是否补采 `ViciousHammer`/`VegaSpell`/`CTemporaryStatView` EA + 映射 `sub_8E49B5`/`sub_83EA05`？ | **下一轮补采**（本面 441 精选未覆盖，用 `r24/*_wz_nodes.txt` 把 r16「仅 B 锚点」推到 C 前段） | C10–C12 |
| **D13** | orange-wz IMG MCP 阻塞如何处理？ | **不接**（已知解析缺陷），保持仓库内 `nx_inspect.py` dump + 正确端截图链 | IMG 层 |
| **D14** | stub / 英文占位文案是否本地化？ | **P0 零风险先做**：`cashshop.stub_job/stub_name`、`UIKeyConfig.cpp:731` 英文、locale 英文原值批次（用户 2026-11-02 决策 B 保持英文原值者不动） | D9/F11/F12 |

**最关键组合**：D1 + D2 + D9 + D10 共同决定「**校准 ≠ 改回原生**」——与登录面 D1、HUD 面 D1/D6 三面一致。

---

## 7. 一次性 Plan

### P0 · 零风险纯客户端 / 文档（无需裁决，可先做）

1. **D5**：把 `UIStatsInfo.cpp:146`、`UIItemInventory.h:49-50`、`.cpp:124,530-532`、`InventoryHandlers.cpp:56,71` 的 095 线地址注释换引 Angel 083 EA（只改注释，坐标不动）。
2. **D7**：在本面 README §2.3 与 `ui-v83-window-restore/README.md:74` 补注「TooltipBackdrop 已迭代为 `TextTooltip.h:28-47` 紫底亮边，旧黑底灰边为中间态」。
3. **D14 前半**：`UIKeyConfig.cpp:731` 英文提示改走 `Locale::text(...)` + zh/en 双键；`cashshop.stub_job/stub_name` 保持用户决策 B 的英文原值但补「非窗口 stub」注释，避免误判。
4. **D11 前半**：把 §5.1 销案表整理成 Q2 清单 `feature-gap-inventory-2026-11/README.md` 的「回写候选」批注（不擅改，交用户）。
5. **门禁**：`python tools/gen_opcode_matrix.py --check` → `GATE OK`；`wc -l -c` / `grep '^## '` / 表格行与判定计数一致性复核。

### P1 · 需用户裁决后实施

6. **D2/D4/D9/D10**：确认 s9 保持项清单，逐条在源码补「083 原生 ↔ s9 适配」对照注释（不改行为）。
7. **D3**：确认 s9 服务端脏词策略 → 决定客户端是否补 `ProcessString` 等价过滤。
8. **D8**：产出 `CUIStat` 6 掩码 / `CUISkill` 500ms 门闩 / `CUIKeyConfig` 5 分支的「Angel 逐字 ↔ 当前端逐条」覆盖度表（`notes/r06-...`）。
9. **D6**：技能窗文字/贴图 12px 错位实机比对后再定 `CLASSIC_SKILL_OFFSET`。
10. **D11 后半**：按用户裁决统一回写 Q2 清单旧账。

### P2 · 需 IMG / 实机

11. **D12**：补采 3 类 EA + 映射 2 个 `sub_*` 锚点，用 `r24/*_wz_nodes.txt` 把 ViciousHammer/VegaSpell/CashGachapon 推到 C 前段。
12. **D13**：如用户要求接 orange-wz，先按 `img-position-attributes.md:5` R1/R2 导出运行时 `wz_dump/*_runtime.json` 再复核；本轮默认不接。
13. **验证门禁**：单元测试（无）+ `tools/build_smoke.bat` **全量 Rebuild**（`UIElement.h:195-199` 警示 `NUM_TYPES` 变更须整工程重建，增量编译会留旧尺寸 obj）+ 1280×720 五屏实机比对（`correct_shots/` 同名帧）+ 原子提交须用户明确授权。

### P3 · goal 续作（后续功能面，同一流水线）

按 README §7 P3 顺序推进 **社交经济 → 宠物召唤龙 → 活动小游戏**，复用 5 个采集脚本（改 `exact`/`*_TARGETS`/FOCUS 关键词即可）+ 三路只读 subagent（源码盘点 / 文档四态 / IMG 样式）+ 六维对照 + 决策点 + 一次性 Plan + `docs/features/<面名>-angel-calibration-2026-11/{README,notes,evidence}` 归档。goal 保持 active/armed。

---

## 8. 限制 / 未知 / 后续

- **未做像素级视觉复核**：`read_image` 对 PNG 返回 `model "hy4free" does not declare image input`；截图只作尺寸/文件名证据，Δ>2px 判据须用户屏前复拍。
- **未做实机构建 / 五屏比对 / 任何产品源码修改 / 原子提交**（按门禁等用户裁决 D1–D14）。
- **7 个 >50KB 超大函数**（`CUIToolTip::SetToolTip_Equip/Package`、`CUIUserInfo::Draw`、`CUIQuestAlarm::Draw`、`CUIStat::Draw`、`CUIStatDetail::Draw`、`CUIFadeYesNo::Draw`、`CQuest::OnCompleteQuestFailed`）已 `/output/` 补全但**未逐行人工核完**；296 函数是精选（非全部 441）。
- **IMG 层阻塞**（三面一致）：orange-wz MCP 未接入本会话 Host；仓库内 `raw-nx-dumps/` 9 份 + `asset-node-inventory.md` 147 行 + `correct_shots/` 7 张是替代真值链；核心窗口专用 `layout*.json` / `manifest.md` / 整窗单窗实机截图未找到。
- **095 线剔除**：`c4-btsort/README.md:26`、`open-issues.md:728`、`userlist-five-tabs/...:35` 等 v95 借证仅登记存在，不作 083 判定依据。
- **后续 goal**：`goal-11388c42-2502-43f0-b722-70abeb8d4307` 仍 active/armed（roundsStarted 2 / maxGoalRounds 30）；登录面 + HUD 面 + 核心窗口面已闭环，按 §7 P3 续作。

---

## 9. 证据索引与复现命令

### 9.1 主证据源（按优先级）

1. **Angel 13339 IDA 一手**：`evidence/angel-corewindow-ea.json`(441/62)、`angel-corewindow-decompile-full.json`(296 归一化伪代码)、`angel-corewindow-facts.md`(401 个 `SP_*`)、`angel-beidou-corewindow-ea-crosscheck.json`(439/441)。
2. **当前端源码逐字**：`notes/r04-current-corewindow-source-inventory.md`（12 独立窗 + 20+ 壳 + 5 组件 + 协议/数据/装配逐行）、`notes/r02-main-agent-crosscheck-cancel.md`（跨端销案 + §8.4b CashShop 深度接线 + §8.4c 任务 3 页签）。
3. **IMG / 样式**：`notes/r03-corewindow-img-style-evidence.md`（§7 `SP_*`↔件表交叉印证、§9 `UIToolTip.img` 终判）、`docs/features/ui-v83-window-restore/notes/asset-node-inventory.md`(147 行)、`README.md:68-79`(W5–W9 定案)、`evidence/_w5_tooltip_root.txt`(149 行)。
4. **文档四态**：`notes/r05-prior-corewindow-doc-audit.md`（四态 + 4 冲突 + 常量表 A–D + 15 复核点 + §9 增量）、`notes/r01-angel-native-corewindow-facts.md`（Angel 逐字语义）。
5. **Q2 旧账**：`docs/features/feature-gap-inventory-2026-11/README.md` §5/§14/§16/§17、`docs/features/beidou-v17-parity/notes/r16-rea-coverage-gap.md:29-38,156-169`、`docs/features/parity-gap-2026-11/README.md` §2。

### 9.2 复现命令（Windows bash，中文路径须 `PYTHONIOENCODING=utf-8`）

```bash
cd /e/project/MapleStory-Client
PY="C:/Users/11036/.dsh/dsh-runtimes/dsh-primary-runtime/dependencies/python/python.exe"
D=docs/features/corewindow-angel-calibration-2026-11/evidence

# ① EA 索引（IDA 侧 py_eval 写文件，短 receipt）
PYTHONIOENCODING=utf-8 "$PY" tools/ida_angel_corewindow_ea.py
# ② 批量反编译（296 函数，每函数独立捕获）
PYTHONIOENCODING=utf-8 "$PY" tools/ida_angel_corewindow_decompile.py "$D/angel-corewindow-decompile.json"
# ③ 大函数 /output/<uuid>.json 补全 + 嵌套 JSON 归一化
PYTHONIOENCODING=utf-8 "$PY" tools/ida_angel_corewindow_normalize.py
# ④ BeiDou 13337 EA 交叉验证
PYTHONIOENCODING=utf-8 "$PY" tools/ida_angel_corewindow_crosscheck.py
# ⑤ SP_ / 坐标 / 按钮 id 候选行抽取
PYTHONIOENCODING=utf-8 "$PY" tools/ida_angel_corewindow_facts.py
# ⑥ 协议矩阵门禁
"$PY" tools/gen_opcode_matrix.py --check
# ⑦ 文档结构校验
wc -l -c docs/features/corewindow-angel-calibration-2026-11/README.md
grep '^## ' docs/features/corewindow-angel-calibration-2026-11/README.md
```

`tools/ida_angel_corewindow_decompile.py` 亦支持覆盖参数：`python tools/ida_angel_corewindow_decompile.py <out.json> [Class | Class::Method | 0xEA ...]`。端口会变，每次先跑 `server_health` 读 `idb_path`（见 `docs/features/ida-v83-angel-instance/README.md` §6）。

---

## 10. 性质标注

- **观察** = 文档明写判定或源码/IDA 逐字行（本文所有证据列、§4 十条硬发现、§9 索引）。
- **推断** = 跨文档去重、六维归并、冲突裁决（§2.3 七处取值、§5 四态、§3.8 汇总、§6 推荐默认值、§7 排序）、覆盖度归类。
- **待实机** = §5.4 遗留 + §7 P2 验证门禁 + §8 限制（明确标注，不假装已验证）。
- 三路 subagent（`18924e1b` 源码盘点 / `2956105c` IMG 样式 / `4cfbbb14` 文档四态）均声明**全程只读、未修改/未写入/未提交任何文件**；主 agent 本轮同样只读取证 + 写 `docs/features/corewindow-angel-calibration-2026-11/` 归档文档，**未改任何产品源码**。
- 本文落地即第三功能面（核心窗口）阶段 0 取证 + 0.5 全维对照 + 1 决策点 + 2 一次性 Plan 的交付物；**实施须等用户裁决 D1–D14**。
