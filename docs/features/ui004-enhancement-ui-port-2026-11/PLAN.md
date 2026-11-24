# PLAN — UI_004 全强化功能族移植 192→xy（全功能一次性交付）

- 日期：2026-11-21
- 用户目标（原文要点）：**全部都做**；对应道具用户认为已移植；要求**功能、素材、使用逻辑一致**，**装备 tooltip 显示的实际效果保持一致**；可参考多方面/公开资料还原。
- 依据总览：[README.md](README.md)（功能清单与触发方式）、本文件下方「阶段 0 证据摘要」、`docs/_uiwindow2_tree_d2.json`（192 UIWindow2.img 全树）。
- 模式：**已确认，进入实现**（2026-11-21 用户拍板）。
- **用户决策（2026-11-21 23:29）**：`D1=B`（道具从 192 源移植到 live+服务端）、`D2=B`（独立 img + 代码显式引用，不合并进 UIWindow2.img）、`D3=A`（公开 Nexon 概率做可配置默认表，注明非 192 实测）、`D4=B`（tooltip 全套 192 图形化 Star/AddOption/Summary/LookAhead）、`D5=A`（纯双击道具，不加侧栏）、`D6=A`（构建+probe+协议门禁后列真机点测步骤交用户验收）。
- 实现基线：D2=B 意味着新建 `Data/UI/UIWindow2Enh.img`（独立根），代码侧在 `Util/UiAssetResolve.cpp` 增加 `window_enh()` 解析器，UI 壳统一走它；tooltip 独立 `Data/UI/UIToolTip.img`。

---

## 阶段 0 — 全面取证摘要（先证据、后分析）

### 0.1 资源现状（关键：用户认为"道具已移植"与实测不符，必须当面澄清）

| 资产 | 实测（orange-wz MCP + 磁盘 + live 日志） | 结论 |
|---|---|---|
| 192 源 `UI_004.wz/UIWindow2.img` | 315 顶层节点，魔方/锤/潜能/石/炼金节点齐全（`docs/_uiwindow2_tree_d2.json`） | 权威素材源 |
| live `Data/UI/UIWindow2.img` | **56KB**，`_resource_probe.log`: `children=4`（HyperMiracleCube/Item/ItemProtector/MiracleCube 占位 1×1），`UIWindow2.img_runtime.json` 顶键仅这 4 个 | 客户端**实际加载**的是这个瘦文件 |
| live `Data/UIWindow2（095、192主要功能ui：…）.img` | **64MB**，GMS magic `73 f8 6c 77`；orange-wz `autoParse:true` 载入后 search Cube=25/Hammer=2/Potential=2/jewel=2/stone=7（我本轮实测；研究 agent 用 `autoParse:false` 得到空子树的结论无效） | 材质完整，但**文件名不是 `UIWindow2.img`、不在 `Data/UI` 下 → 运行时不可达**（`Util/ImgFiles.cpp:248` rootdir=base/UI；`:396-419` loose img 按 stem 变独立根） |
| live `Data/UIToolTip（192装备tooltip信息框）.img` | 44,611 B；magic `73 f8 fa d9...`；**GMS/CMS 均解析失败，零 IV `AAAAAA==` 可解析**（说明未转 key） | 孤儿资源：代码全仓无 `nl::nx::UI["UIToolTip.img"]` 引用（tooltip-researcher 全仓搜索确认），且 key 规格错 |
| 192 `MapleServer192/wz/UI.wz/UIToolTip.img.xml` | 44,388 B，节点 `Item/Equip/{Star,AddOption,Summary,LookAhead,...}`，纯图形无文本 | tooltip 原生图形来源（XML 可读） |
| live `Data/Item/Cash/0506.img` | 7 节点（05060000/1/2、05061000/1/2/3），**无 5062000/5062006/5062500 等** | 关键道具**未移植** |
| s9 服务端 `wz/Item.wz/Cash/0506.img.xml` | 2,727 B，7 节点，同样无上述魔方 | 服务端也不认识这些道具 |
| live `Data/Item/Cash/0557.img` | 仅 `05570000`（金锤子，gms） | 金锤本体有，但 `5570001`/`5570111` 白金锤/变异锤无 |

> **必须先让用户拍板**：用户说"对应道具应该也移植过来了"，实测是「UI 大图移植了但躺在 Data 根、tooltip 图未转 key 且无人引用、道具（魔方/附加/白金锤/印章）实际不在 live 与服务端的 item 数据里」。若用户另有渠道（比如服务端 DB 直接发道具、或另有 item 包）需要指出路径；否则按 Phase A 把道具一并移植。

### 0.2 触发方式（已验证，本端原生模型 = 双击道具，无专门入口）

- `UIItemInventory.cpp:828-833` → `204xxxx` 卷轴双击开 `UIEnchantSkill`（break 截断普通 UseItem）。
- `UIItemInventory.cpp:887-890` → CASH `557xxxx` 双击开 `UIViciousHammer`；`:891-894` `561`→`UIVegaSpell`；`:895-898` `559`→`UIItemProtector`；`:981-987` `243`→魔方（当前死代码，未发包）。
- Shell 通用交互：SoftKey `E` 选装备槽、`S` 选道具槽、`Enter/R/G` 确认发包（`UIViciousHammer.cpp:112-173`、`UIMiscShells.cpp:810-917`、notes r129/r194/r778）。
- 侧栏 `UISideIconBar.cpp:387-457` 只有活动类入口，强化类**无槽位**。
- 192 二进制加壳（ida-192.md：3 函数/10 字符串），原生回调不可静态取证；用道具描述（`tools/_192txt/Cash.u8.txt:230-249,2774`："双击…拖动到装备(快捷键E)"）交叉印证。

### 0.3 客户端 UI 壳与协议链路（已验证，可直接复制的模式）

| 能力 | 壳/枚举/配置 | 发包 | 回包 |
|---|---|---|---|
| 金锤 | `UIViciousHammer.h:15-47`、`UIElement.h:91`、`Configuration.h:1030-1033`/`.cpp:299`、ctor `UIViciousHammer.cpp:30-60`、send_icon `:81-98`、SoftKey `:112-137` | `UseViciousHammerPacket` `UIViciousHammer.cpp:147-173`→`CashItemPackets.h:13-25`(USE_CASH_ITEM=79)；空锤 `HammerPackets.h:10-14`(USE_HAMMER=260) | `PacketSwitch.cpp:168,486` VICIOUS_HAMMER=354→`WaveAShellHandlers.cpp:196-234`→`UIViciousHammer.cpp:70-79` |
| 卷轴强化 | `UIMiscShells.h:107-177`、ctor `UIMiscShells.cpp:695-734`、send_icon `:777-803`、SoftKey `:810-851` | `apply_scroll()` `:892-917`→`ScrollEquipPacket::dispatch_bag_equip`(Net/Packets/InventoryPackets.h:186-221, opcode OutPacket.h:140) | `PacketSwitch.cpp:116,778` SCROLL_RESULT=167→`MessagingHandlers.cpp:611-659`→`UIMiscShells.cpp:919-948` |
| 维加 | `UIVegaSpell.h:15-48`、ctor `.cpp:29-63` | `UseVegaSpellPacket` `:185`→`CashItemPackets.h:28-41` | `PacketSwitch.cpp:169,487` VEGA_SCROLL=358→`WaveAShellHandlers.cpp:236-251` |
| 保护 | `UIMiscShells.h:178-193`、ctor `UIMiscShells.cpp:992-1013` | `UseItemProtectorPacket` `:1071` | 回包 handler **未取证** |
| 突破石（侧栏模板） | `UIBreakthroughStone.h:20-64`、`.cpp:25-74` `nl::nx::UI["UIWindowCN.img"]["CN_Limitbreakthrough"]` | `BreakthroughStoneOpenPacket` `:73`、Use `:127` | `PacketSwitch.cpp:441,831`→`WaveAShellHandlers.cpp:5024-5054` |

资源解析统一走 `Util/UiAssetResolve.cpp:45-58` `nl::nx::UI["UIWindow2.img"][name]`，回退 `UIWindow.img`。**只有合并进 `Data/UI/UIWindow2.img` 的节点才能被 UI 找到**（两条后端路径 `Util/ImgFiles.cpp:248,292-295` 与 `Util/Resource/ImgSource.cpp:126-137,423-435` 结论一致）。

### 0.4 服务端现状与缺口

- 已有：`RecvOpcode.java:94 USE_CASH_ITEM(0x4F)`、`:100 USE_UPGRADE_SCROLL(0x56)`、`:199 CASHSHOP_OPERATION(0xE5)`、`:206 USE_HAMMER(0x104)`；`SendOpcode.java:356 VICIOUS_HAMMER(0x162)`、`:357 VEGA_SCROLL(0x166)`。
- `CashOperationHandler.java:83-129`：action `0x40` 魔方重洗——读 EQUIP short slot、随机 1..50 写 5 条潜能、DB 回写、modifyInventory、serverNotice（**无分级概率、无 before/after 选择、无 Onemore**）。
- `UseCashItemHandler.java:635-649`（557 金锤：setVicious+1/setUpgradeSlots+1，sendHammerData）、`:650-709`（561 维加）；`UseHammerHandler.java:32-35`（0x104）。
- `PacketCreator.java:503-607`：equip 包 5×short 潜能（:596-600）+ spark/soul(short×2)/starForce(byte)（:605-607）；客户端 `ItemParser.cpp:127-155` 严格对齐——**字段顺序不可错位**。
- 缺失：魔方全族分级概率、附加潜能、白金锤、星之力强化、石净化、宝石炼金/协同、卷轴协同、itemPot、MasterPiece、SoulCollector 的 opcode/handler/快照；突破石只有 SendOpcode 常量，无 Recv/handler/快照序列化。

### 0.5 Equip Tooltip 显示链（用户明确要求"显示实际效果保持一致"）

- 构建：`EquipTooltip.cpp:469-1407`（对比窗 `:438-467` 入口、`:623-902` 镜像）。资源仅用 `nl::nx::UI["UIWindow.img"]["ToolTip"]["Equip"]`（`:302`）与 `IconBase/0`（`:321`）——**当前完全没用 192 UIToolTip.img**。
- 已显示：slots+金锤 vicious（`:1197-1211`，label `equip.label_hammers`）、卷轴 stat（`:1262-1307`）、潜能 3 主+2 附加（`:1309-1352`，`Locale::text_or("equip.potline."+id, "潜能 "+id)` :1331-1332，附加 "(B)" :1334-1335）、spark/soul/star（`:1354-1406`，`"星之力 N ***"` :1392-1399）、图形星 `draw_stars` `:1496-1522`+调用 `:1635`。
- 数据模型：`Equip.h:29-38,55-89`、`Equip.cpp:22-48,105-144`（potrank 由第一条潜能 id 分段推导，`1..50`→RARE..LEGENDARY）；`ItemParser.cpp:127-155` 解析 5 潜能+spark/soul/starForce；服务端 `PacketCreator.java:596-607` 写入——**字段链已通，无需扩协议**。
- 缺口：potline/spark/soul 是简化文案/占位回退（live `locale/zh-CN.ini:1158-1237` 的 `equip.potline.1..50` 是简化表），192 原生用 `UIToolTip.img/Item/Equip/{Star,AddOption,Summary,LookAhead}` 图形+String；`Summary/incline/decline`（增量箭头+%数字位图）本端只有文本 `( +N )/( -N )`。
- 关键取舍：是否把 tooltip 改成 192 图形化（含星形 canvas/AddOption 图形） —— 属大工程，需拍板（见阶段 1）。

### 0.6 公开资料（用户要求"多方面参考公开资料"，仅作概率/规则参考，不是 192 原生值）

- Nexon 官方概率公示（韩服，含红/黑/附加魔方 tier-up 与 option 位置概率）：黑魔方 Rare→Epic 15%、Epic→Unique 3.5%、Unique→Legendary 1.4%；红魔方 6%/1.8%/0.3%；附加 4.7619%/1.9608%/0.4975%（[Nexon notice](https://maplestory.nexon.com/News/Notice/Notice/133316)、[Nexon 概率月报](https://archive.maplestory.nexon.com/News/ProbabilityResult/MonthData)）。
- 星之力规则：0★→25★，失败掉星、高星破坏，成本随星数升（[CalculatorBox Starforce](https://calculatorbox.org/de/t/starforce-calculator/)、[namu.wiki 星之力](https://namu.wiki/w/%EB%A9%94%EC%9D%B4%ED%94%8C%EC%8A%A4%ED%86%A0%EB%A6%AC/%EC%8A%A4%ED%83%80%ED%8F%AC%EC%8A%A4%20%EA%B0%95%ED%99%94)）。
- 金锤规则：Golden Hammer + Vicious Hammer 各 +1 槽（共 2），失败即消耗槽，先 Golden 后 Vicious（[Whackybeanz Spell Trace/Scrolls](https://www.whackybeanz.com/guides/spell-trace)）。
- **诚实边界**：192 服务端是私服版本且二进制加壳，以上概率与 192 实际数值是否一致**未取证**；按 skill 门禁不臆造，默认做成可配置表并写明"参考公开 Nexon 公示，非 192 实测"。

---

## 阶段 0.5 — 分析归纳

1. **已证实**：素材侧 64MB 合并 img 是完整的 192 强化 UI（Cube/Hammer/stone/jewel 全在），但没进客户端加载路径；道具侧只有 204 卷轴、5570000 金锤本体可用，魔方/附加/白金锤等实际缺失；UI 壳侧 557/561/559/204 已有可复制的双击→软键→发包→回包闭环；协议侧 5 潜能+spark/soul/starForce 字段链已通，服务端魔方只有粗粒度 0x40。
2. **差异**：①UI 资源落错位置+孤儿 tooltip 未转 key；②道具资源未入 live/服务端；③大多数功能无 UI 类/发包/回包/handler；④tooltip 显示靠简化文案与内嵌像素星，未用 192 原生 canvas。
3. **推荐方案**：按"资源→道具→UI 壳→协议→tooltip→验证"六层推进；每个功能族复用现有壳模板；概率表独立 Config 可改；tooltip 优先保证**字段语义与数值一致**（已通），192 图形化 UI 作为可选项（需拍板）。
4. **仍不确定（需拍板）**：见阶段 1。

---

## 阶段 1 — 决策点（阻塞，等用户回复后进入实现）

| # | 问题 | 选项 / 推荐 |
|---|---|---|
| D1 | **道具资源现状与用户认知冲突**：实测 live/s9 均无 5062000/5062006/5062500/5530111 等节点。用户所说"已移植"是否另有路径（DB 直发 / 其它包）？ | A. 用户给出具体路径，按该路径接入（推荐先确认）；B. 本计划 Phase A 从 192 源（`MapleServer192/wz/Item.wz/Cash/0506.img.xml` 等）一并移植道具到 live+服务端 |
| D2 | **64MB UIWindow2 大图如何入库**：必须以 `UIWindow2.img` 文件名合并进 `Data/UI/UIWindow2.img`（保留现有 Item/ItemProtector/MiracleCube 等节点），遵守 §3.1 转 key + probe；还是新建单独 img 并用 `nl::nx::UI["UIWindow2Enh.img"]` 引用？ | A. 合并进 `Data/UI/UIWindow2.img`（推荐，改动面最小）；B. 独立 img + 代码显式引用（隔离性好但要改 UiAssetResolve） |
| D3 | **数值/概率**：公开 Nexon 概率 vs 192 私服实际（不可静态取证） | A. 按公开 Nexon 公示做可配置默认表（推荐，写明非 192 实测）；B. 等用户提供 192 服务端概率表/抓包后再填 |
| D4 | **tooltip 还原深度**：字段语义已一致（5 潜能/spark/soul/starForce 全链路通）；是否要把 192 `UIToolTip.img` 的图形化 Star/AddOption/Summary 全套换上？ | A. 先保字段/文案一致（现有像素星+文本），图形化列为后续（推荐，风险低）；B. 全套 192 图形化（需转 key 孤儿 tooltip、改 EquipTooltip.cpp:302 加载路径、替换 draw_stars/文本行，工作量大） |
| D5 | **范围与交付**：全部功能族一次性做（用户已明确"都做"）；是否包含侧栏专门入口？ | A. 按原生双击道具触发，不加侧栏（推荐，与 192 一致）；B. 另加侧栏入口方便测试（与原生不符，仅调试用） |
| D6 | **验证深度**：真机双击验收需要客户端可关闭（历史多次 PID 占用无法结束） | A. 构建+probe+协议门禁后，列出人工真机点测步骤，由用户关客户端后自行验收（推荐）；B. 用户先关客户端，AI 部署后短启截图验收 |

> 回答格式：直接按 D1…D6 给选择即可；若全选推荐可回复"按推荐"。**D1 是硬阻塞**——道具路径不明会直接影响资源与发包设计。

---

## 阶段 2 — 完整任务计划（用户拍板后执行，计划一次成型）

> 下列任务按依赖排序；每项标注闭环/预留与验证门禁。预计改动文件按当前证据列出，实际以拍板结果微调。

### Task 1 — 资源入库：合并 192 强化 UI 到 live `Data/UI/UIWindow2.img`（闭环）
- 源：`UI_004.wz/UIWindow2.img` 全量节点（或只合并强化族，视 D2）+ 192 `UIToolTip.img.xml`（若 D4=B）。
- 流程（AGENTS.md §3.1 硬门禁）：`load_files(源, 128B key + AAAAAA==, exclusive:false)` → 以 `autoParse:true` 展开所有 outlink canvas（本端 nlnx 不解析 UI outlink，ui006 教训）→ `copy_paste_nodes` 到 staging（保留 live 现有 4 节点）→ `save_as` 异路径 → `batch_convert_key(AAAAAA== → TSPHKw==, verify:true)` → 再 `save_as` 最终 → `unload_all` → 原子替换 `Data/UI/UIWindow2.img`（关客户端）→ `--resource-probe` 确认 UI 根含 Cube_Red/GoldHammer/jewelCraft 等且 canvas 尺寸正确、无 `0xc0000374`。
- 若 D4=B：同步把 `UIToolTip（192装备tooltip信息框）.img`（当前零 IV）`batch_convert_key(AAAAAA==→TSPHKw==)` 并改名为 `UIToolTip.img` 放入 `Data/UI/`（或合并进 `UIWindow.img/ToolTip/Equip`），并改 `EquipTooltip.cpp:302` 加载路径。
- 产物：`patches/uiwindow2-merge.md`（节点清单+SHA+probe 日志）、`resources/资源清单.md`。

### Task 2 — 道具资源迁移（取决于 D1，闭环/预留）
- 从 192 服务端 XML `MapleServer192/wz/Item.wz/Cash/{0506,0553,0557}.img.xml` 抽取魔方/印章/白金锤等节点；按 item id 补进 live `Data/Item/Cash/0506.img`、`0553.img`、`0557.img`（经 orange-wz 转 key）与 s9 服务端 `wz/Item.wz/Cash/*.img.xml`（或 DB，按服务端加载路径）。
- String 名称：192 `MapleServer192/wz/String/Cash.img.xml`（或 `String.txt`）补进 live `Data/String/` + s9 `wz/String/`。
- 抽样验证：orange-wz `get_node_detail` 核对 icon/iconRaw 非 1×1、String 名正确；服务端 `ItemInformationProvider` 能识别 id。

### Task 3 — UI 壳实现（每族一个类，闭环）
按现有模板新增（每类 = `.h/.cpp` + `UIElement.h` 枚举 + `Configuration.{h,cpp}` 位置 + 双击前缀接线）：
1. `UICube`（魔方全族共壳）：backgrnd/BtOk/BtOnemore/remainBackgrnd/Before/After；SoftKey E 选装备、Enter 确认；按 item id 区分 Red/Black/Platinum/Additional/White/NewHexa 变体（同一壳不同资源节点+概率表）。
2. `UIGoldHammer`/`UIWhiteGoldHammer`（或复用 `UIViciousHammer` 扩 variant）：BtStart/GaugeBar/EffectP/EffectE；拖装备+SoftKey E；发包区分 5570000/5570001。
3. `UIPotentialStamp`（潜能印章）、`UIJewelCraft`（宝石炼金）、`UISynergyMachine`（协同机）、`UIScrollSynergyMachine`、`UIStonePurify`（石净化，含 Probtab/success/fail）、`UIItemPot`、`UIMasterPiece`、`UISoulCollector`——按节点结构实现最小闭环（开窗、选装备/材料、发包、结果动画）。
- 双击接线 `UIItemInventory.cpp:868-999` CASH 分支加前缀（506/553/557/243 等）；卷轴强化 `204` 分支保持 `UIEnchantSkill`（`:828-833`）。
- 验证：每类 `--resource-probe` 节点存在；build_smoke EXITCODE=0。

### Task 4 — 协议与服务端口（闭环）
- 新增 opcode（客户端 `Net/OutPacket.h` + 服务端 `RecvOpcode`/`SendOpcode`）：魔方重洗（分级）、附加潜能魔方、白金锤、星之力强化、石净化、宝石炼金/协同、卷轴协同、itemPot、MasterPiece、SoulCollector；突破石补 Recv 0x371B + handler + 快照（research 缺口）。
- 客户端包类仿 `CashItemPackets.h:13-54`、`InventoryPackets.h:186-221`；回包 `PacketSwitch.cpp` + `WaveAShellHandlers.{h,cpp}`（或新 handler 文件）+ UI setter。
- 服务端 handler 仿 `CashOperationHandler.java:83-129`、`UseCashItemHandler.java:635-709`；概率表独立 Config（Nexon 公开值可配置，见 D3）；结果写回 `Equip` 5 潜能+spark/soul/starForce（`PacketCreator.java:596-607` 顺序不动）+ DB 持久化。
- 闭环要点：魔方 Before/After 与 BtOnemore 需要服务端回包带新潜能值 + 客户端对比显示；星之力需要服务端强回 `starForce` byte 并触发图形星刷新。

### Task 5 — Tooltip 一致性（按 D4）
- **A（推荐）**：确认 `EquipTooltip.cpp:1309-1406` 的 5 潜能/`(B)` 前缀/spark/soul/star 文本渲染与服务端写入值一致；用 `Equip.cpp:22-48` 的 potrank 分段核对 `zh-CN.ini:1158-1237` 文案；补缺失词条；截图真机核对。
- **B**：叠加 192 图形化——`EquipTooltip.cpp:302` 改加载 `UIToolTip.img`（或合并节点），`draw_stars :1496-1522` 换 192 Star canvas，potline 换 AddOption/Summary 图形；需新增资源节点与像素对齐校验（skill `maplestory-ui-layout-qa`）。
- 验证：主窗/对比窗（`EquipTooltip.cpp:469-1407` / `:623-902`）双路径显示一致。

### Task 6 — 验证、部署、产物（闭环）
- 门禁：客户端 `build_smoke.bat` EXITCODE=0；服务端 `mvn package`；opcode 双端一致脚本；`--resource-probe` 通过；真机点测清单（每功能族：双击道具→开窗→选装备→确认→tooltip 变化）。
- 部署铁律（MEMORY.md critical）：**只部署到 live `E:\MXD\BeiDou-Client-xy\`**，exe 直接覆盖 `MapleStory.exe`，禁带后缀旁挂；声明"已部署"前实时核对 live SHA。
- 产物：`docs/features/ui004-enhancement-ui-port-2026-11/{README,PLAN}.md` + `notes/`（节点映射表、道具 ID 表、概率来源）+ `evidence/`（MCP 节点树、probe 日志、IDA 摘要、公开资料链接）+ `patches/` + `resources/`；最终实现逻辑文档；单功能原子 commit（用户要求时）。
- 诚实边界：192 二进制加壳→原生逻辑/概率未取证；真机未点测前不宣称闭环；截图必须来自 live 本端（USER.md 硬规则）。

---

## 风险与回滚
- 合并 UIWindow2 巨量节点可能撑大 `UI.nx` 并触发 0xc0000374（ui006/StatusBar3 教训）→ probe 不通过则回滚 `UIWindow2.img.bak`，只合并子集。
- outlink canvas 未内联 → 按钮/特效空白 → 构建时强制 `set_png`+删 link（ui006 75 canvas 教训）。
- 协议字段错位 → equip 显示全乱 → 5 潜能+spark/soul/starForce 顺序与 `ItemParser.cpp:127-155` 严格对齐，新增字段一律追加在尾部并双端同步。
- 概率数值争议 → 全部集中在 Config 类，默认值可单点改。
