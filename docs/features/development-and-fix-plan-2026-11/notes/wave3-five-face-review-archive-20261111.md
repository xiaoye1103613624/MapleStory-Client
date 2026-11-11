# Wave3 · 五面复核归档（round87 / 2026-11-11）

## 0. 性质与授权

- 用户 m13177「五个面逐个用 Angel IDB 跑 REA→IDA→IMG 复核后，然后把 D-G（UIGuild 浮窗）等待抉择的都安排修复或处理了」；用户 m09211「全部继续，如果能自测的可以自己启动测试」；用户 m09210「记录一次有可见 PUA 输出的交付，不记录评分」。
- round73-77 / round84 会话内已完成五面 `UIChannel` / `UICharSelect` / `UICommonCreation` / `UIMiscShells` / `UIItemInventory` 的 REA→IDA→IMG 只读取证，结论「已对齐 / 合法混合面，无需改源码」。
- **该结论此前只在会话压缩证据里，dev-plan §4 与 evidence/notes 目录均无归档**（现场 `grep 五面|归档` = 0 命中）。本轮把它结构化落版本库，使结论可检索、可逐条复核。
- 本轮 **0 源码 / 0 协议 / 0 WZ 改动**：不重做取证、不改任何已对齐坐标 / 常量 / 行为字面量。

## 1. 五面结论与可核证锚点

| 面 | 源文件 | 结论 | 校准 README 主锚 | 083 / 资源依据 |
| --- | --- | --- | --- | --- |
| UIChannel 选频道 | `IO/UITypes/UIChannel.cpp` (377 行) | 已对齐 / 合法混合面 | `login-flow-angel-calibration-2026-11/README.md:168/234/242/285` | `CUIChannelSelect::OnKey @0x628a1c`；plate 三态真 NX 节点；channel_count 由 Configuration 驱动；BtChange/BtCancel 原生帧 y=99 SAD=0.20/0.22 |
| UICharSelect 选角色 | `IO/UITypes/UICharSelect.cpp` (1478 行) | 已对齐 / 合法混合面 | `login-flow-angel-calibration-2026-11/README.md:169/171/228/233` | `CUICharDetail::Draw (sub_602ADC @0x60292F)` 坐标表；占用 1384 / 空 1348；PAGESIZE=3；按钮仅 1000/1001/1002 |
| UICommonCreation 建角 | `IO/UITypes/UICommonCreation.cpp` (2385 行) | 已对齐 / 合法混合面 | `login-flow-angel-calibration-2026-11/README.md:170` | init_modern 全 `Login.img/CustomizeChar[classType]` NX 驱动；init_classic 仅少量 1280x720 截图锚；BtViewAll 真位图 99x35 |
| UIMiscShells 杂项壳 | `IO/UITypes/UIMiscShells.cpp` (7859 行) | 已对齐 / 合法混合面 | `activity-minigame-angel-calibration-2026-11/README.md:11/103/104/167` | `load_window_bg` 真 NX 底板；has_assets=false 全库 0 命中 / true 6 处；DIY 壳属 D 矩阵 P3 长尾不重写 |
| UIItemInventory 物品栏 | `IO/UITypes/UIItemInventory.cpp` (2045 行) | 已对齐 / 合法混合面 | `corewindow-angel-calibration-2026-11/README.md:94/135/308` + `drop-...:116` | `CUIItem::OnCreate @0x81c6c9` / `SetArrangeButton @0x81e3a6`；slot_col=4 / slot_row=6 / slot_pos(8,50) 展开态 SAD=0.000；除零守卫已销案 |

逐面完整 `calibration_refs` / `source_anchors` / `constants` / `evidence_dirs` / `prior_notes` 见 `evidence/five-face-review-round87.json`。

## 2. 为什么是「合法混合面」而非「虚构根同族病」

对照 round76 UIGuild（`GuildUI.img` 五证为 0、全文字自绘、083 无独立 CUIGuild 类 = 第 8 例同族病）与 round78 UIEquipInventory（`UIWindow.img/Equip` 10 真节点经 128B 门禁全在 = 合法混合面反例）：本五面全部走真 NX 资源根 / 真 083 EA 立即数 / 真位图，截图 SAD 只用于补 083 无 `OnCreate` 立即数处（门禁 18 截图备选合法），不存在虚构根、自造版式、凭空逻辑。结构性差异（UIMiscShells DIY 壳、UICommonCreation classic 截图锚）按「校准≠改回原生 / s9 优先」登记为 P3 长尾，不做架构级重写。

## 3. 归档资产与自测

- `evidence/five-face-review-round87.json`：5 面 × 结论 + 逐面锚点 + 6 条硬约束 + 5 项真机归口 + zero_change 声明。
- dev-plan §4 `| **2-35** |` 行；`pending-live-tests` §46（五面真机归口 + 4 条限制）。
- `tools/test_round87_five_face_archive.py`：静态守卫（文件存在 / 5 面结论 / 锚点路径可达 / 常量命中 / EOL 字节级 / 0 改动声明 / 幂等）+ 场景（5×结论真值表 / 锚点行号 / 常量 / 真机归口）。
- 自测：`gen_opcode_matrix.py --check`（GATE OK `s9 a78ba681a4`）+ `build_smoke.bat`（EXITCODE=0，本轮零源码改动，增量绿仅门禁回归，不声称真重编覆盖）。

## 4. 限制与真机认领

- 五屏真机观感截图（60 节 / 142 处标记）由用户认领；本归档是只读取证层结论，不能替代实机验收。
- 展开宠物面板原生帧 / `BtPet1/2/3 origin (15,13)`（§45）、D-F 五按钮 y=234、D2 聊天规则、`InPacket::available()` 393 处 §27-A 真机逐面回归 仍 open。
- 外部程序在途 11 个源码 WIP 原样留工作树，不夹带进本轮提交（hunk 级精确 add 隔离）。
- goal-11388c42 保持 active（paused/disarmed 84/90），全目标未达成不得标 complete。
