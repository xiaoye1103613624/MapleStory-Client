# -*- coding: utf-8 -*-
"""round87 五面复核归档生成器（幂等，EOL 精确保留）。

性质：纯文档归档。round73-77 / round84 会话内对五个面做过 REA→IDA→IMG 只读取证，
结论「已对齐 / 合法混合面，无需改源码」只在会话证据里，从未落版本库。本生成器把
可核证锚点（校准 README 行号 / 源文件常量 / 既有 notes / evidence 子目录）结构化归档，
0 源码 / 0 协议 / 0 WZ 改动。重跑字节幂等。
"""
import json
import os

ROOT = "E:/project/MapleStory-Client"
DEV = os.path.join(ROOT, "docs/features/development-and-fix-plan-2026-11/README.md")
PLT = os.path.join(ROOT, "docs/features/pending-live-tests/README.md")
EVID = os.path.join(ROOT, "docs/features/development-and-fix-plan-2026-11/evidence/five-face-review-round87.json")
NOTE = os.path.join(ROOT, "docs/features/development-and-fix-plan-2026-11/notes/wave3-five-face-review-archive-20261111.md")

ALIGNED = "已对齐 / 合法混合面，无需改源码"

FACES = [
    {
        "id": "UIChannel",
        "label": "选频道窗",
        "file": "IO/UITypes/UIChannel.cpp",
        "file_lines": 377,
        "verdict": ALIGNED,
        "calibration_refs": [
            "docs/features/login-flow-angel-calibration-2026-11/README.md:168",
            "docs/features/login-flow-angel-calibration-2026-11/README.md:234",
            "docs/features/login-flow-angel-calibration-2026-11/README.md:242",
            "docs/features/login-flow-angel-calibration-2026-11/README.md:285",
        ],
        "calibration_symbols": ["CUIChannelSelect::OnKey @ 0x628a1c", "CUIChannelSelect 0x62B2CF = CreateDlg(1"],
        "source_anchors": [
            "IO/UITypes/UIChannel.cpp:28",
            "IO/UITypes/UIChannel.cpp:42-50",
            "IO/UITypes/UIChannel.cpp:86-88",
            "IO/UITypes/UIChannel.cpp:105",
            "IO/UITypes/UIChannel.cpp:114",
            "IO/UITypes/UIChannel.cpp:130",
        ],
        "constants": {
            "SLOTS_PER_ROW": 6, "GRID_TOP": 67, "SLOT_PITCH_X": 70,
            "SLOT_PITCH_Y": 20, "STRIP_HEIGHT": 33,
            "plate_tristate": "channel0(empty)/channel1(selected)/channel2(current)",
            "channel_count_source": "Configuration::get().get_channel_total(worldid)",
            "bt_y_native_frame": 99, "bt_sad": "0.20/0.22",
        },
        "evidence_dirs": ["docs/features/ui-v83-window-restore/evidence/w8-channel-layout-fix"],
        "prior_notes": [],
        "basis": "plate 三态取真 NX 节点、频道数由 Configuration 驱动；BtChange/BtCancel 顶边 y=99 与末排灰板由原生帧 SAD 定标，非自造版式。",
    },
    {
        "id": "UICharSelect",
        "label": "选角色窗",
        "file": "IO/UITypes/UICharSelect.cpp",
        "file_lines": 1478,
        "verdict": ALIGNED,
        "calibration_refs": [
            "docs/features/login-flow-angel-calibration-2026-11/README.md:169",
            "docs/features/login-flow-angel-calibration-2026-11/README.md:171",
            "docs/features/login-flow-angel-calibration-2026-11/README.md:228",
            "docs/features/login-flow-angel-calibration-2026-11/README.md:233",
        ],
        "calibration_symbols": ["CUICharSelect::OnButtonClicked @ 0x601667", "CUICharDetailVAC @ 0x60745A", "CLogin::SendDeleteCharPacket @ 0x5f7c4a"],
        "source_anchors": [
            "IO/UITypes/UICharSelect.cpp:93",
            "IO/UITypes/UICharSelect.cpp:107-109",
            "IO/UITypes/UICharSelect.cpp:338-339",
            "IO/UITypes/UICharSelect.cpp:1373",
            "IO/UITypes/UICharSelect.cpp:1380",
            "IO/UITypes/UICharSelect.cpp:1384-1386",
        ],
        "constants": {
            "PAGESIZE": 3,
            "CS_CHARINFO_OFF": "(-94,-231)",
            "CUICharDetail_Draw": "sub_602ADC @ 0x60292F",
            "slot_occupied_width": 1384, "slot_empty_width": 1348,
            "button_ids": "1000/1001/1002 only",
            "addr_table": "0x602960 / 0x60292F / 0x5F42CD / 0x9E03A6 / 0x48BBA5",
        },
        "evidence_dirs": [],
        "prior_notes": ["docs/features/development-and-fix-plan-2026-11/notes/wave1-charselect-recheck-20261109.md"],
        "basis": "标签/值坐标表逐字来自 Angel 083 CUICharDetail::Draw (sub_602ADC)，占用 1384 / 空 1348、PAGESIZE=3、按钮仅 1000/1001/1002；截图只作旁证，非自造。",
    },
    {
        "id": "UICommonCreation",
        "label": "建角 / 取名窗",
        "file": "IO/UITypes/UICommonCreation.cpp",
        "file_lines": 2385,
        "verdict": ALIGNED,
        "calibration_refs": [
            "docs/features/login-flow-angel-calibration-2026-11/README.md:170",
        ],
        "calibration_symbols": ["CUINewCharNameSelect::OnButtonClicked"],
        "source_anchors": [
            "IO/UITypes/UICommonCreation.cpp:62-63",
            "IO/UITypes/UICommonCreation.cpp:69",
            "IO/UITypes/UICommonCreation.cpp:76",
            "IO/UITypes/UICommonCreation.cpp:89-97",
            "IO/UITypes/UICommonCreation.cpp:394",
            "IO/UITypes/UICommonCreation.cpp:495",
            "IO/UITypes/UICommonCreation.cpp:499",
            "IO/UITypes/UICommonCreation.cpp:584",
            "IO/UITypes/UICommonCreation.cpp:618",
        ],
        "constants": {
            "resource_root": "Login.img/Common + Login.img/CustomizeChar[classType]",
            "init_modern": "全 NX 资源驱动 (board/genderSelect/info/avatarOrigin/avatarSize/charNameOrigin)",
            "init_classic": "1280x720 三职业 sign/yes/no/slot/avatar 截图锚",
            "BtViewAll": "真资产 PNG 99x35",
            "ATLAS_BTVA": "0x43524254 ('CRBT')",
            "scene": "Map001/Back/login.img/back/<no>",
            "ida_helpers": "sub_5F5962 / sub_5FCF8A",
        },
        "evidence_dirs": [],
        "prior_notes": ["docs/features/development-and-fix-plan-2026-11/notes/wave2-commoncreation-recheck-20261110.md"],
        "basis": "init_modern 全 NX 节点驱动；init_classic 仅少量 1280x720 截图锚补 083 无 OnCreate 立即数处（门禁 18 截图备选合法），BtViewAll 是真位图。",
    },
    {
        "id": "UIMiscShells",
        "label": "20+ shell 杂项窗",
        "file": "IO/UITypes/UIMiscShells.cpp",
        "file_lines": 7859,
        "verdict": ALIGNED,
        "calibration_refs": [
            "docs/features/activity-minigame-angel-calibration-2026-11/README.md:11",
            "docs/features/activity-minigame-angel-calibration-2026-11/README.md:103",
            "docs/features/activity-minigame-angel-calibration-2026-11/README.md:104",
            "docs/features/activity-minigame-angel-calibration-2026-11/README.md:167",
        ],
        "calibration_symbols": ["COmokDlg::OnKey", "CMemoryGameDlg::OnKey", "MiniRoom 6 类 → 单壳 (D09)"],
        "source_anchors": [
            "IO/UITypes/UIMiscShells.cpp:156",
            "IO/UITypes/UIMiscShells.cpp:244",
            "IO/UITypes/UIMiscShells.cpp:314",
            "IO/UITypes/UIMiscShells.cpp:353-355",
            "IO/UITypes/UIMiscShells.cpp:412",
            "IO/UITypes/UIMiscShells.cpp:1758",
            "IO/UITypes/UIMiscShells.cpp:2111",
            "IO/UITypes/UIMiscShells.cpp:3870-3871",
            "IO/UITypes/UIMiscShells.cpp:3963",
            "IO/UITypes/UIMiscShells.cpp:4707-4709",
        ],
        "constants": {
            "loader": "load_window_bg(name, Texture&, Point<int16_t>&) 真 NX 资源驱动",
            "has_assets_true": 6, "has_assets_false_hits": 0,
            "balloon_sad": "7 件 SAD=0.00",
            "macro_frame": "379x291 技能展开宏指令",
            "ida_helpers": "sub_9A3CAE / sub_4EDAB7",
            "dg_note": "round85 D-G 083 家族入口 = UIUserList 第 3 页签",
        },
        "evidence_dirs": [],
        "prior_notes": [],
        "basis": "has_assets=false 全库 0 命中、has_assets=true 6 处全走 load_window_bg 真 NX 底板；DIY 壳属 D 矩阵 P3 长尾（结构性差异），按 s9 优先 / 校准≠改回原生不重写。",
    },
    {
        "id": "UIItemInventory",
        "label": "物品栏窗",
        "file": "IO/UITypes/UIItemInventory.cpp",
        "file_lines": 2045,
        "verdict": ALIGNED,
        "calibration_refs": [
            "docs/features/corewindow-angel-calibration-2026-11/README.md:94",
            "docs/features/corewindow-angel-calibration-2026-11/README.md:135",
            "docs/features/corewindow-angel-calibration-2026-11/README.md:308",
            "docs/features/drop-angel-calibration-2026-11/README.md:116",
        ],
        "calibration_symbols": [
            "CUIItem::OnCreate @ 0x81c6c9", "CUIItem::SetArrangeButton @ 0x81e3a6",
            "CUIItem::OnChildNotify @ 0x81d01f", "OnGatherItemResult 0xa1e943", "OnSortItemResult 0xa1e96d",
        ],
        "source_anchors": [
            "IO/UITypes/UIItemInventory.cpp:124-127",
            "IO/UITypes/UIItemInventory.cpp:146-163",
            "IO/UITypes/UIItemInventory.cpp:170",
            "IO/UITypes/UIItemInventory.cpp:175-182",
            "IO/UITypes/UIItemInventory.cpp:547-550",
            "IO/UITypes/UIItemInventory.cpp:1461",
        ],
        "constants": {
            "slot_col": 4, "slot_row": 6, "slot_pos": "(8,50)",
            "expanded_sad": 0.000,
            "zero_div_guard": "slot_col<=0||slot_row<=0 → 4/6/(8,50) 整组回退",
            "ida_dec": "tools/_ida_CUIItem_OnCreate.txt / _ida_CUIItem_OnChildNotify.txt / _ida_CUIItem_OnMouseButton.txt",
        },
        "evidence_dirs": [
            "docs/features/ui-v83-window-restore/evidence/w7-equip",
            "docs/features/ui-v83-window-restore/evidence/r27-b3-equip",
        ],
        "prior_notes": [
            "docs/features/development-and-fix-plan-2026-11/notes/wave2-iteminventory-recheck-20261110.md",
            "docs/features/development-and-fix-plan-2026-11/notes/wave2-iteminventory-slotcol-recheck-20261110.md",
        ],
        "basis": "v83 位图实测 slot_col=4 / slot_row=6 / slot_pos(8,50)、展开态逐格 SAD=0.000；083 EA 已在源码注释逐字钉死，除零旁路守卫已销案。",
    },
]

EV_DATA = {
    "round": "round87",
    "task": "五面复核归档（m13177 / m09211 / goal-11388c42）",
    "nature": "纯文档归档；0 源码 / 0 协议 / 0 WZ 改动",
    "source_rounds": "round73-77 + round84 会话内 REA→IDA→IMG 只读取证",
    "authority": "Angel(13339) 083/v83 带符号 IDB；s9 协议优先；校准≠改回原生",
    "face_count": 5,
    "faces": FACES,
    "verdict_distribution": {ALIGNED: 5},
    "constraints": [
        "083/v83 only 禁 095",
        "s9 协议优先",
        "数值重合≠语义重合",
        "校准≠改回原生（不重写 Journey 架构 / 不改已对齐坐标）",
        "WZ/IMG/XML 先 orange-wz MCP（128B WzAESConstant.DEFAULT_KEY，.nx/NoLife 才 65536B key_gms）",
        "门禁 18 REA→IDA→orange-wz→.nx 只读取证（截图仅备选）",
    ],
    "live_backlog": [
        "五屏真机观感截图（用户认领，pending-live-tests 60 节 / 142 处标记）",
        "展开宠物面板原生帧 / BtPet1-3 origin(15,13)（§45 用户认领）",
        "D-F 五按钮 y=234 是否按五屏微调（决策点）",
        "D2 聊天规则待真机",
        "InPacket::available() 393 处 §27-A 真机逐面回归",
    ],
    "zero_change": {
        "source_files_touched": 0,
        "protocol_edits": 0,
        "wz_edits": 0,
        "deliverables": [
            "docs/features/development-and-fix-plan-2026-11/evidence/five-face-review-round87.json",
            "docs/features/development-and-fix-plan-2026-11/notes/wave3-five-face-review-archive-20261111.md",
            "docs/features/development-and-fix-plan-2026-11/README.md (§4 2-35)",
            "docs/features/pending-live-tests/README.md (§46)",
            "tools/test_round87_five_face_archive.py",
            "tools/_fiveface_deliver.py",
        ],
    },
}

NOTE_MD = """# Wave3 · 五面复核归档（round87 / 2026-11-11）

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
"""

DEV_ROW = ("| **2-35** | 文/稳 | **五面复核归档（round87 / m13177 + m09211）**：把 round73-77 / round84 会话内对 "
    "`UIChannel` / `UICharSelect` / `UICommonCreation` / `UIMiscShells` / `UIItemInventory` 的 REA→IDA→IMG 只读取证结论"
    "「**已对齐 / 合法混合面，无需改源码**」结构化落版本库（此前只在压缩证据里，dev-plan 搜「五面\\|归档」0 命中）。"
    "逐面锚点 = 12 面校准 README 行号（login-flow :168/169/170/…、activity-minigame :11/103/104/167、corewindow :94/135/308、drop :116）"
    "+ 五源文件常量/083 EA 注释 + 既有 recheck notes + evidence 子目录。定性 = 真 NX 资源根 / 真 083 立即数 / 真位图，"
    "截图 SAD 仅补 083 无 OnCreate 立即数处（门禁 18 备选合法），**非虚构根同族病**（对照 round76 UIGuild 五证为0 / round78 UIEquipInventory 10 真节点）。"
    "结构性差异按「校准≠改回原生 / s9 优先」登记 P3 长尾，不做架构级重写。**0 源码 / 0 协议 / 0 WZ 改动** | "
    "`evidence/five-face-review-round87.json` + `notes/wave3-five-face-review-archive-20261111.md` + 12 面校准 README + 5 源文件 | "
    "门禁 18 只读取证（本轮为归档，不重做取证）；s9 优先 / 083-only 禁095 | 纯文档（evidence + notes + dev §4 2-35 + PLT §46 + harness + 生成器） | "
    "`tools/test_round87_five_face_archive.py`（静态守卫 5 面结论 / 锚点可达 / 常量命中 / EOL 字节级 / 0 改动 / 幂等 + 场景 5×结论真值表）"
    " + `gen_opcode_matrix.py --check`（GATE OK）+ `build_smoke.bat`（EXITCODE=0，门禁回归非真重编） | "
    "✅ 本轮归档（round87）；五屏真机 60 节/142 处仍由用户认领；**11 个外部源码 WIP 零夹带** |")

S46 = [
    "## 46. round87 · 五面复核归档 + 真机认领清单（#devfix-five-face-review-archive，需用户一起测）",
    "",
    "本轮 **0 源码 / 0 协议 / 0 WZ 改动**：只把 round73-77 / round84 会话内五面只读取证结论结构化归档，不重做取证、不改已对齐坐标。",
    "",
    "| 项 | 面 | 验收动作 | 本轮状态 |",
    "| --- | --- | --- | --- |",
    "| 46-1 | UIChannel 选频道 | 真机开频道窗，核对 plate 三态 / y=99 双钮 / 末排灰板与原生帧一致 | ⬜ 未执行，用户认领 |",
    "| 46-2 | UICharSelect 选角色 | 真机核对 sub_602ADC 标签值坐标表 / 占用1384 空1348 / PAGESIZE=3 三按钮 | ⬜ 未执行，用户认领 |",
    "| 46-3 | UICommonCreation 建角 | 真机核对 init_modern NX 资源驱动 + classic 1280x720 截图锚 / BtViewAll 真位图 | ⬜ 未执行，用户认领 |",
    "| 46-4 | UIMiscShells 杂项壳 | 真机核对 load_window_bg 真 NX 底板 6 处 / 界面气球 7 件 / 宏指令 379x291 | ⬜ 未执行，用户认领 |",
    "| 46-5 | UIItemInventory 物品栏 | 真机核对 slot_col=4 / slot_row=6 / slot_pos(8,50) 展开态逐格对齐 | ⬜ 未执行，用户认领 |",
    "",
    "已知限制：",
    "",
    "- 本归档是只读取证层结论，不能替代实机观感验收；五屏截图（60 节 / 142 处标记）全部由用户认领。",
    "- 五面均判「已对齐 / 合法混合面」：真 NX 资源根 + 真 083 EA 立即数 + 真位图；截图 SAD 仅补 083 无 OnCreate 立即数处（门禁 18 备选合法）。",
    "- 结构性差异（UIMiscShells DIY 壳 / UICommonCreation classic 截图锚）按「校准≠改回原生 / s9 优先」登记 P3 长尾，不做架构级重写。",
    "- 外部程序在途 11 个源码 WIP 原样留工作树，不夹带进本轮提交；本轮 build 为门禁回归（零源码改动，不声称真重编覆盖）。",
]


def rb(p):
    with open(p, "rb") as f:
        return f.read()


def wb(p, b):
    with open(p, "wb") as f:
        f.write(b)


def main():
    # 1) evidence JSON (LF, ensure_ascii=False, 2-space indent)
    ev_text = json.dumps(EV_DATA, ensure_ascii=False, indent=2) + "\n"
    wb(EVID, ev_text.encode("utf-8"))

    # 2) notes (LF)
    wb(NOTE, NOTE_MD.encode("utf-8"))

    # 3) dev-plan: insert 2-35 row after 2-34 row, preserve lone-LF
    dev_b = rb(DEV)
    dev = dev_b.decode("utf-8")
    if "| **2-35** |" not in dev:
        lines = dev.split("\n")
        out = []
        inserted = False
        for ln in lines:
            out.append(ln)
            if ln.startswith("| **2-34** |") and not inserted:
                out.append(DEV_ROW)
                inserted = True
        assert inserted, "2-34 anchor not found"
        wb(DEV, "\n".join(out).encode("utf-8"))

    # 4) PLT: append ## 46, preserve CRLF, single trailing CRLF (no EOF blank line)
    plt_b = rb(PLT)
    plt = plt_b.decode("utf-8")
    if "## 46." not in plt:
        base = plt.rstrip("\r\n")
        sec = "\r\n".join(S46).rstrip("\r\n")
        wb(PLT, (base + "\r\n\r\n" + sec + "\r\n").encode("utf-8"))

    print("FIVEFACE_DELIVER_ALL_OK")
    print("EV_BYTES", os.path.getsize(EVID), "NOTE_BYTES", os.path.getsize(NOTE))
    print("DEV_BYTES", os.path.getsize(DEV), "PLT_BYTES", os.path.getsize(PLT))


if __name__ == "__main__":
    main()
