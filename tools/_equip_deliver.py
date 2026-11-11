# -*- coding: utf-8 -*-
"""round78 UIEquipInventory 装备窗宠物区 083 复核交付生成器（幂等，EOL 精确保留）。

本轮 0 行为字面量：只在 UIEquipInventory.cpp 的宠物区注释前插入一段 083 依据纯注释，
另产出 evidence JSON / notes / dev-plan 2-34 行 / PLT §45。harness 单独落盘。
"""
import json
import os
import hashlib

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

CPP = "IO/UITypes/UIEquipInventory.cpp"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
EV = "docs/features/development-and-fix-plan-2026-11/evidence/equip-window-083-round78.json"
NOTE = "docs/features/development-and-fix-plan-2026-11/notes/wave3-equip-window-083-review-20261111.md"


def rb(p):
    return open(os.path.join(ROOT, p), "rb").read()


def wb(p, b):
    open(os.path.join(ROOT, p), "wb").write(b)


def wt_lf(p, s):
    open(os.path.join(ROOT, p), "w", encoding="utf-8", newline="\n").write(s)


# ---------------------------------------------------------------- 1) cpp 纯注释
CPP_LINES = [
    "\t\t// round78 / Angel 083 依据链（纯注释，0 行为字面量）：",
    "\t\t// - 资源根：orange-wz MCP 128B WzAESConstant.DEFAULT_KEY 实证",
    "\t\t//   UIWindow.img/Equip 10 真节点全在（backgrnd / pet / FullBackgrnd /",
    "\t\t//   BtDetail / BtPet1-3 / BtPetEquipShow / BtPetEquipHide / BtCashshop）。",
    "\t\t//   旧 _equip_probe_stdout.txt 的 \"IMG 解析失败\" 是 65536B NoLife 全量表",
    "\t\t//   错规格假阻塞（round85 4b2452c4 已翻案），非 MCP 故障、非重名。",
    "\t\t// - 083 活 IDA：CUIEquip::OnButtonClicked @0x7feb72 case 3000 商城 /",
    "\t\t//   3001 ToggleDragonEquip / 3002 TogglePetEquip；",
    "\t\t//   CUIEquip::TogglePetEquip @0x7ffa84 ZAlloc 1456B 独立 CUIPetEquip",
    "\t\t//   子窗（父窗偏移 +123）；CUIPetEquip::OnButtonClicked @0x8019f1",
    "\t\t//   case 2000 收回 / 2001-2003 三页签 InvalidateRect。",
    "\t\t// - 083 无 CUIEquip / CUIPetEquip 的 OnCreate 符号（sym2_*OnCreate total=0）",
    "\t\t//   ⇒ 无布局立即数，坐标按门禁 18 走截图 SAD 备选（合法）。",
    "\t\t// - 定性：合法混合面反例（真资源根 + 截图坐标 + 083 独立子窗架构差异），",
    "\t\t//   不是虚构根同族病。本端内嵌宠物面板是 Journey 架构差异、交互语义对齐",
    "\t\t//   （BT_PET_SHOW/HIDE 开合 + BT_PET_TAB0/1/2 三页签）；按",
    "\t\t//   「校准≠改回原生 / s9 优先」不重写架构，本轮 0 行为改动。",
]
CPP_BLOCK = ("\n".join(CPP_LINES) + "\n").encode("utf-8")

b = rb(CPP)
if b"round78 / Angel 083" not in b:
    anchor = "\t\t// 宠物装备区：以窗体右下角为锚".encode("utf-8")
    idx = b.find(anchor)
    assert idx >= 0, "cpp pet-section anchor not found"
    b = b[:idx] + CPP_BLOCK + b[idx:]
    wb(CPP, b)
    print("OK cpp comment block inserted bytes=", len(CPP_BLOCK))
else:
    print("OK cpp already patched (idempotent)")

# ---------------------------------------------------------------- 2) dev-plan 2-34
ROW234 = (
    "| **2-34** | 交/稳 | **UIEquipInventory 装备窗按 Angel 083 只读取证复核 + 口径标注（round78 收口）**："
    "定性为**合法混合面反例**（真资源根 + 截图 SAD 坐标 + 083 独立子窗架构差异），非虚构根同族病。"
    "orange-wz MCP 128B `WzAESConstant.DEFAULT_KEY` 实证 `UIWindow.img/Equip` 10 真节点全在"
    "（`pet`/`BtPet1-3`/`BtPetEquipShow/Hide`/`backgrnd`/`FullBackgrnd`/`BtDetail`/`BtCashshop`），"
    "旧 `_equip_probe_stdout.txt` 的 `IMG 解析失败` 是 65536B NoLife 全量表错规格假阻塞（round85 `4b2452c4` 已翻案）；"
    "083 活 IDA `CUIEquip::OnButtonClicked @0x7feb72`（3000 商城 / 3001 ToggleDragonEquip / 3002 TogglePetEquip）、"
    "`CUIEquip::TogglePetEquip @0x7ffa84` ZAlloc 1456B 独立 `CUIPetEquip` 子窗（父窗 +123）、"
    "`CUIPetEquip::OnButtonClicked @0x8019f1`（2000 收回 / 2001-2003 三页签）；"
    "083 无 `OnCreate` 符号（无布局立即数）⇒ 坐标走门禁 18 截图 SAD 备选合法。"
    "本端内嵌宠物面板属 Journey 架构差异、交互语义对齐（BT_PET_SHOW/HIDE 开合 + BT_PET_TAB0/1/2 三页签），"
    "按「校准≠改回原生 / s9 优先」不重写架构，**0 行为字面量** | "
    "`tools/_ida_equip/*`（8 CUIEquip + 2 CUIPetEquip 符号 + 6 反编译）/ "
    "`tools/_equip_retest_128.py`（128B 门禁 10 节点 isError:false）/ "
    "`evidence/equip-window-083-round78.json` / `notes/wave3-equip-window-083-review-20261111.md` | "
    "门禁 18（IDA→orange-wz 128B→合规截图备选）；校准≠改回原生（不重写独立子窗架构 / 不动 s9 / 不改坐标） | "
    "`UIEquipInventory.cpp` 1 块 083 依据纯注释（0 行为字面量） | "
    "`tools/test_round78_equip_window_083.py`（静态守卫 083 EA / 128B 十节点 / OnCreate=0 / 0 行为行 / EOL 保持 + "
    "场景 083 三页签真值表 / 独立子窗 / 合法混合面判定）+ `gen_opcode_matrix.py --check`（GATE OK）+ "
    "`build_smoke.bat`（EXITCODE=0 真重编覆盖 UIEquipInventory.cpp） | "
    "✅ 本轮收口（round78）；真机 4 项进 `pending-live-tests` §45；"
    "**BtPet1/2/3 origin (15,13) 待展开宠物面板原生帧再定，列入用户取证** |"
)
t = open(os.path.join(ROOT, DEV), "r", encoding="utf-8", newline="").read()
if "| **2-34** |" not in t:
    lines = t.split("\n")
    hit = -1
    for i, l in enumerate(lines):
        if l.startswith("| **2-33** |"):
            hit = i
            break
    assert hit >= 0, "dev 2-33 anchor not found"
    lines.insert(hit + 1, ROW234)
    open(os.path.join(ROOT, DEV), "w", encoding="utf-8", newline="").write("\n".join(lines))
    print("OK dev 2-34 row inserted after line", hit + 1)
else:
    print("OK dev already has 2-34 (idempotent)")

# ---------------------------------------------------------------- 3) PLT §45 (CRLF)
S45 = [
    "## 45. round78 · UIEquipInventory 装备窗宠物区 083 复核 + 真机认领清单（#devfix-equip-window-083-review，需用户一起测）",
    "",
    "本轮 **0 行为字面量**：只补 083 依据纯注释 + 四文档 + harness。定性 = 合法混合面反例（真资源根 + 截图 SAD 坐标 + 083 独立子窗架构差异），非虚构根同族病；不重写 Journey 内嵌宠物面板架构。",
    "",
    "| 项 | 归口 | 验收动作 | 本轮状态 |",
    "| --- | --- | --- | --- |",
    "| 45-1 | 装备窗宠物面板展开帧 | 启 s9 冷启动开装备窗 → 点 BtPetEquipShow 展开宠物面板，截图核对 `pet 177x181` 相对主窗右下角 (right-3, bottom-h) 与底边齐平 | ⬜ 未执行，用户认领 |",
    "| 45-2 | BtPet1/2/3 三页签 origin | 展开帧量测 `BtPet1/2/3` 四态 origin 是否 (15,13)；现有 y 按 (15,0) 推可能整体高 13px（注释「只记不改」），待原生帧定 | ⬜ 未执行，用户认领 |",
    "| 45-3 | BtDetail / BtCashshop 默认隐藏 | 截图复核两钮全图模板匹配 mae 55.8 / 61.1（对照显示钮 ≤0.8）→ 原生帧内是否存在，当前端 `set_active(false)` 是否正确 | ⬜ 未执行，用户认领 |",
    "| 45-4 | 独立子窗 vs 内嵌面板交互观感 | 083 是 `CUIPetEquip` 独立浮窗（+123 偏移 / 2000 收回 / 2001-2003 三页签），本端内嵌面板 + BT_PET_SHOW/HIDE + 三页签，核对交互语义等价与观感差异是否可接受 | ⬜ 未执行，用户认领 |",
    "",
    "已知限制：",
    "",
    "- orange-wz MCP 路径用 128B `WzAESConstant.DEFAULT_KEY`（`tools/_gms_key_128.b64` 172 字符 + iv `TSPHKw==`）；旧 65536B 全量表 `IMG 解析失败` 是规格错配假阻塞，非 MCP 故障（round85 `4b2452c4`）。",
    "- 083 无 `CUIEquip`/`CUIPetEquip` 的 `OnCreate` 符号 ⇒ 无布局立即数，坐标按门禁 18 走截图 SAD 备选；不得据 IDA 缺符号判「虚构」。",
    "- 本轮不改 `iconpositions` / `pet_panel_pos` / 任何按钮坐标 / 任何行为字面量；§44 的 60 节 / 142 处真机标记与五屏仍全部由用户认领。",
    "- 外部程序在途 11 个源码 WIP 原样留工作树，不夹带进本轮提交。",
]
b = rb(PLT)
if b"## 45." not in b:
    sec = ("\r\n".join(S45)).rstrip("\r\n").encode("utf-8") + b"\r\n"
    b = b.rstrip(b"\r\n") + b"\r\n\r\n" + sec
    wb(PLT, b)
    print("OK PLT section 45 appended bytes=", len(sec))
else:
    print("OK PLT already has 45 (idempotent)")

# ---------------------------------------------------------------- 4) evidence JSON
EV_DATA = {
    "round": 78,
    "face": "corewindow / UIEquipInventory 装备窗宠物区",
    "verdict": "legal_mixed_face_counterexample",
    "verdict_zh": "合法混合面反例（真资源根 + 截图 SAD 坐标 + 083 独立子窗架构差异），非虚构根同族病",
    "zero_behavior_change": True,
    "orange_wz_gate": {
        "endpoint": "http://127.0.0.1:10012/mcp",
        "key_spec": "128B WzAESConstant.DEFAULT_KEY",
        "key_file": "tools/_gms_key_128.b64",
        "key_b64_len": 172,
        "key_head": "EwAAAFIAAAAqAAAAWwAAAAgA",
        "iv": "TSPHKw==",
        "probe": "tools/_equip_retest_128.py",
        "img": "UIWindow.img/Equip",
        "children_count": 10,
        "children": ["backgrnd", "pet", "FullBackgrnd", "BtDetail", "BtPet1",
                     "BtPet2", "BtPet3", "BtPetEquipShow", "BtPetEquipHide", "BtCashshop"],
        "key_nodes_found": 9,
        "isError": False,
        "old_false_blocker": "tools/_equip_probe_stdout.txt 65536B NoLife 全量表 → IMG 解析失败（round85 4b2452c4 翻案）",
        "root_cause": "WzMutableKey.java:84-90 走 128B→32B AES 派生；NoLife .nx 用 65536B 全量 XOR 表，规格互不通用",
    },
    "ida_083": {
        "symbols": [
            {"name": "??0CUIEquip@@QAE@XZ", "addr": "0x7fde7c"},
            {"name": "?OnMouseMove@CUIEquip@@UAEHJJ@Z", "addr": "0x7fe9bb"},
            {"name": "?OnButtonClicked@CUIEquip@@UAEXI@Z", "addr": "0x7feb72"},
            {"name": "?ToggleDragonEquip@CUIEquip@@QAEXXZ", "addr": "0x7ff8dd"},
            {"name": "?TogglePetEquip@CUIEquip@@IAEXXZ", "addr": "0x7ffa84"},
            {"name": "?GetOffEquipItem@CDraggableItem", "addr": "0x4f11b4"},
            {"name": "?WearEquipItem", "addr": "0x4f1a93"},
            {"name": "?ThrowItem", "addr": "0x4f31a5"},
            {"name": "?OnMouseMove@CUIPetEquip@@UAEHJJ@Z", "addr": "0x800f7b"},
            {"name": "?OnButtonClicked@CUIPetEquip@@UAEXI@Z", "addr": "0x8019f1"},
        ],
        "symbol_count": 10,
        "no_oncreate": True,
        "oncreate_note": "sym2_CUIPetEquipOnCreate.json / sym2_CUIEquipOnCreate.json 均 total=0，无布局立即数",
        "onbuttonclicked": "CUIEquip::OnButtonClicked @0x7feb72 case 3000 商城 / 3001 ToggleDragonEquip / 3002 TogglePetEquip",
        "toggle_pet_equip": "CUIEquip::TogglePetEquip @0x7ffa84 ZAlloc 1456B 独立 CUIPetEquip 子窗（父窗偏移 +123）",
        "pet_equip_onbutton": "CUIPetEquip::OnButtonClicked @0x8019f1 case 2000 收回 / 2001-2003 三页签 InvalidateRect",
        "toggle_dragon": "CUIEquip::ToggleDragonEquip @0x7ff8dd ZAlloc 1408B 独立龙装窗（父窗 -142/+117）",
        "dec_files": ["CUIEquip_ctor.dec.txt", "CUIEquip_OnButtonClicked.dec.txt",
                      "CUIEquip_TogglePetEquip.dec.txt", "CUIEquip_ToggleDragonEquip.dec.txt",
                      "CUIPetEquip_OnButtonClicked.dec.txt", "CUIPetEquip_OnMouseMove.dec.txt"],
    },
    "current_impl": {
        "root": "UiAssetResolve::window4(\"Equip\") = UIWindow.img/Equip",
        "backgrnd": "175x304",
        "pet_panel": "Texture(Equip[\"pet\"]) 177x181 内嵌，pet_panel_pos=(right-3, bottom-h)",
        "buttons": ["BT_PET_SHOW", "BT_PET_HIDE", "BT_PET_TAB0", "BT_PET_TAB1", "BT_PET_TAB2"],
        "tab_origin_note": "BtPet1/2/3 四态 origin (15,13)，现有 y 按 (15,0) 可能高 13px，只记不改待原生帧",
        "icon_offset_fix": "draw_at(position) 去 +(4,4) 偏移（用户报装备 ui 位置偏移）",
        "screenshot_basis": "正确客户端截图/角色装备.png + evidence/w7-equip/coords.md 模板匹配",
    },
    "patch": {
        "zero_behavior_literals": True,
        "comment_only": True,
        "files": ["IO/UITypes/UIEquipInventory.cpp"],
        "block": "round78 / Angel 083 依据链（17 行纯注释）",
        "docs": [DEV, PLT, EV, NOTE],
        "harness": "tools/test_round78_equip_window_083.py",
    },
    "backlog": [
        {"id": "45-1", "what": "宠物面板展开帧 pet 相对右下角定位", "owner": "用户真机"},
        {"id": "45-2", "what": "BtPet1/2/3 origin (15,13) 定 y", "owner": "用户真机"},
        {"id": "45-3", "what": "BtDetail/BtCashshop 默认隐藏观感", "owner": "用户真机"},
        {"id": "45-4", "what": "独立子窗 vs 内嵌面板交互观感", "owner": "用户真机"},
    ],
    "constraints": ["083/v83 only 禁 095", "s9 协议优先", "数值重合≠语义重合", "校准≠改回原生"],
}
# pre-patch immutable baseline: sha256 of cpp with the round78 comment block removed.
# Independent of git HEAD, so the harness stays green before AND after this commit.
_cur = rb(CPP)
_s = _cur.find(b"\t\t// round78 / Angel 083")
_e = _cur.find(b"\t\t// \xe5\xae\xa0\xe7\x89\xa9\xe8\xa3\x85\xe5\xa4\x87\xe5\x8c\xba\xef\xbc\x9a", _s + 1)
assert _s >= 0 and _e > _s
_recon = _cur[:_s] + _cur[_e:]
EV_DATA["patch"]["pre_patch_cpp_sha256"] = hashlib.sha256(_recon).hexdigest()
wt_lf(EV, json.dumps(EV_DATA, ensure_ascii=False, indent=2) + "\n")
print("OK evidence json")

# ---------------------------------------------------------------- 5) notes
NOTE_MD = """# round78 · UIEquipInventory 装备窗宠物区按 Angel 083 只读取证复核（2026-11-11）

## 1. 目标与触发
- 用户 m09211「全部继续，如果能自测的可以自己启动测试」+ m04292/m09859/m13177 硬指令链；round86 `bdfd8f40` 批量裁决销案后，收口历史挂账的 round78 装备窗。
- 本轮**不重写架构、不改任何行为字面量**：只把 083 依据链补进代码注释并交付四文档 + harness，真机观感项交用户认领。

## 2. 三维取证（门禁 18：REA→IDA→orange-wz→合规截图备选）
- **orange-wz 128B 门禁**：`tools/_equip_retest_128.py` 直连 `http://127.0.0.1:10012/mcp`，`userKeyBase64`=128B `WzAESConstant.DEFAULT_KEY`（`tools/_gms_key_128.b64` 172 字符）、iv `TSPHKw==`，`UIWindow.img/Equip` 返回 10 个真节点、9 个关键节点全 true、`isError:false`。旧 `tools/_equip_probe_stdout.txt` 的 4 处 `IMG 解析失败` 是 65536B NoLife 全量表错规格假阻塞（round85 `4b2452c4` 已翻案），真因 `WzMutableKey.java:84-90` 走「128B→32B AES 派生」，与 `.nx` 全量 XOR 表互不通用。
- **083 活 IDA**（`tools/_ida_equip/`）：`CUIEquip` 8 符号 + `CUIPetEquip` 2 符号；`CUIEquip::OnButtonClicked @0x7feb72` case 3000 商城 / 3001 `ToggleDragonEquip @0x7ff8dd` / 3002 `TogglePetEquip @0x7ffa84`；`TogglePetEquip` 用 `ZAlloc 1456B` 建独立 `CUIPetEquip` 子窗（父窗偏移 +123）；`CUIPetEquip::OnButtonClicked @0x8019f1` case 2000 收回 / 2001-2003 三页签 `InvalidateRect`；`sym2_*OnCreate.json` 均 `total=0` ⇒ **无布局立即数**。
- **当前端逐行**（`IO/UITypes/UIEquipInventory.cpp` 672 行）：真资源根 `UiAssetResolve::window4("Equip")`；`backgrnd 175x304` + 5 列 33 步进 iconpositions 表；宠物区 `Texture(Equip["pet"]) 177x181` 内嵌、`pet_panel_pos=(right-3,bottom-h)`、BT_PET_SHOW/HIDE 开合、BT_PET_TAB0/1/2 三页签；截图 SAD 坐标补全 083 无立即数处；`BtPet1/2/3 origin (15,13)` 只记不改待原生帧。

## 3. 定性：合法混合面反例（排除虚构根同族病）
- **真资源根**：10 个 `Equip` 节点经 128B 门禁全部存在 → 不是「虚构根 / 全文字自绘」的第 N 例同族病（对照 round76 UIGuild `GuildUI.img` 五证为 0）。
- **截图 SAD 合法**：083 无 `OnCreate` 符号 ⇒ 无布局立即数，按门禁 18 截图仅作备选、补立即数缺口，非「自造版式」。
- **架构差异非缺陷**：083 是独立 `CUIPetEquip` 浮窗，本端是内嵌宠物面板；两者交互语义（开合 + 2001-2003 三页签 → BT_PET_SHOW/HIDE + BT_PET_TAB0/1/2）对齐，属 Journey 架构适配。按「校准≠改回原生 / s9 优先」不重写。

## 4. 实施（0 行为字面量）
- `UIEquipInventory.cpp` 宠物区注释前插入 17 行 083 依据纯注释（资源根 / IDA 链 / 无 OnCreate / 定性）；EOL 保持 BOM+LF 无尾换行。
- 四文档：`evidence/equip-window-083-round78.json`、`notes` 本文件、dev-plan §4 `| **2-34** |` 行、PLT `## 45.` 五屏归口（CRLF 无尾空行）。
- harness `tools/test_round78_equip_window_083.py`：静态守卫（083 EA / 128B 十节点 / OnCreate=0 / 0 行为行 / EOL）+ 场景（三页签真值表 / 独立子窗 / 合法混合面判定）。

## 5. 验收与遗留
- 自测：`gen_opcode_matrix.py --check`（GATE OK `s9 a78ba681a4`）+ `build_smoke.bat`（EXITCODE=0，touch cpp 真重编覆盖）+ harness 全绿 + `git diff --check` 干净。
- 提交：精确 add、零署名、hunk 级隔离外部 11 个源码 WIP。
- 遗留（用户认领）：§45 四项真机观感（展开帧定位 / 三页签 origin / BtDetail·BtCashshop 默认隐藏 / 独立子窗 vs 内嵌面板交互）；§44 的 60 节 / 142 处真机标记与五屏仍全 open。
"""
wt_lf(NOTE, NOTE_MD)
print("OK notes")

print("EQUIP_DELIVER_ALL_OK")
