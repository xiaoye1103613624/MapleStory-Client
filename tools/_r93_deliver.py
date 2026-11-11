# -*- coding: utf-8 -*-
"""round93 交付生成器：5 处 095/v95 依据链纠偏（纯注释）+ 四文档 + evidence。

幂等：old 串不存在而 new 串已存在 -> 跳过（already present）。
EOL：源文件按原样 CRLF/LF 保留；新增文档按各自目录既有 EOL。
绝不写 commit hash / git log（round88 B9b 教训）。
"""
import hashlib
import io
import json
import os
import sys

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)


def rd(p):
    with open(p, "rb") as f:
        return f.read().decode("utf-8")


def wr(p, s):
    with open(p, "wb") as f:
        f.write(s.encode("utf-8"))


def sha12(s):
    return hashlib.sha256(s.encode("utf-8")).hexdigest()[:12]


def nl_style(p):
    b = rd(p)
    crlf = b.count("\r\n")
    lf = b.count("\n") - crlf
    return "\r\n" if crlf > lf else "\n"


def eol_audit(p):
    b = rd(p)
    crlf = b.count("\r\n")
    lf = b.count("\n") - crlf
    return dict(bytes=len(b.encode("utf-8")), crlf=crlf, lonelf=lf,
                crcrlf=b.count("\r\r\n"), bom=b.startswith("\ufeff"))


PATCHES = [
    (
        "IO/UIStateGame.cpp",
        "// 095 自定义中文 UI：暂时复用 EVENT 键打开福利中心（后续补独立 SPECIAL 键位 + 服务端 keymap）。",
        "// 本端扩展（E 桶）：福利中心（签到/兑换/排行/转职/传送）非 083 原生窗族 —— "
        "083 活 IDB 全库 `*UISpecial*` / `*Fairy*` / `*Welfare*` / `*CheckIn*` / `*Attendance*` "
        "符号命中均为 0（`tools/_ida_r93/special_family.json`），故按 m09729「扩展功能可以不对齐」"
        "保留且不改行为；暂时复用 EVENT 键打开（后续若补独立键位须同步服务端 keymap）。"
        "旧注释写「095 自定义中文 UI」属依据链污染，round93 按 083 零命中改判本端扩展身份。",
    ),
    (
        "IO/UITypes/UISpecial.h",
        "// 095 自定义中文 UI 入口：福利中心（签到/兑换/排行/转职/传送）。",
        "// 本端扩展（E 桶）入口：福利中心（签到/兑换/排行/转职/传送）。\n"
        "\t// 083 活 IDB 全库 `*UISpecial*` / `*Fairy*` / `*Welfare*` / `*CheckIn*` / `*Attendance*` "
        "符号命中均为 0（`tools/_ida_r93/special_family.json`），非 083 原生窗族；"
        "按 m09729「扩展功能可以不对齐」保留，零行为改动。旧注释「095 自定义中文 UI」= 依据链污染。",
    ),
    (
        "Gameplay/MapleMap/MapPortals.h",
        "// 传送门触发范围（对齐 v95：CPortal::CheckKey 的按键通道 vs ProcessPortal 的碰触通道）：",
        "// 传送门触发范围（083 依据：按键通道 = `CPortalList::FindPortal @0x712ab1`，"
        "碰触通道 = `CPortalList::FindPortal_Collision @0x712b61`，入口 = "
        "`CUserLocal::CheckPortal_Collision @0x94dac6`；旧注释写「v95 CPortal::CheckKey / ProcessPortal」"
        "属依据链污染 —— 083 全库 `*CPortal@@*` / `*CPortal::*` 符号命中为 0，见 "
        "`tools/_ida_r93/portal_family.json`）：",
    ),
    (
        "Gameplay/MapleMap/MapPortals.cpp",
        "// 原生语义（v95 CPortal::CheckKey / ProcessPortal）：",
        "// 原生语义（083 `CPortalList::FindPortal @0x712ab1` 按键通道 / "
        "`CPortalList::FindPortal_Collision @0x712b61` 碰触通道，由 "
        "`CUserLocal::CheckPortal_Collision @0x94dac6` 按 portal type 分派；旧注释「v95 ...」"
        "依据链污染已纠偏）：",
    ),
    (
        "Gameplay/Stage.cpp",
        "// 其余传送门：按上键主动交互（对齐 v95 CPortal::CheckKey）",
        "// 其余传送门：按上键主动交互（083 依据：`CPortalList::FindPortal @0x712ab1` "
        "按键通道命中框 = 水平 ±range / 垂直固定 ±50 band；旧注释「v95 CPortal::CheckKey」"
        "在 083 全库无此符号，属依据链污染，见 `tools/_ida_r93/portal_family.json`）",
    ),
]

patch_report = []
for path, old, new in PATCHES:
    s = rd(path)
    eol = "\r\n" if s.count("\r\n") > (s.count("\n") - s.count("\r\n")) else "\n"
    old_e = old.replace("\n", eol)
    new_e = new.replace("\n", eol)
    if old_e in s:
        s2 = s.replace(old_e, new_e, 1)
        wr(path, s2)
        patch_report.append({"path": path, "action": "applied", "sha12": sha12(s2)})
        print("[patch] applied   ", path)
    elif new_e in s:
        patch_report.append({"path": path, "action": "already", "sha12": sha12(s)})
        print("[patch] already   ", path)
    else:
        raise SystemExit("PATCH ANCHOR NOT FOUND: " + path)


DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
dev = rd(DEV)
dev_eol = nl_style(DEV)
ROW_241 = (
    "| **2-41** | round93 095/v95 依据链纠偏（m11680 盘点后自测推进） | "
    "**5 处注释把依据写成「095/v95」或引了 083 里不存在的符号名，逐条按活 IDB 纠偏（纯注释，零行为/零 wire）**："
    "① `IO/UIStateGame.cpp:533` + ② `IO/UITypes/UISpecial.h:9` —— 「095 自定义中文 UI：福利中心」→ "
    "083 全库 `*UISpecial*`/`*Fairy*`/`*Welfare*`/`*CheckIn*`/`*Attendance*` 命中均 0，改判"
    "**本端扩展（E 桶）**（m09729「扩展功能可以不对齐」）；③ `Gameplay/MapleMap/MapPortals.h:40` + "
    "④ `Gameplay/MapleMap/MapPortals.cpp:146` + ⑤ `Gameplay/Stage.cpp:284` —— 「对齐 v95 CPortal::CheckKey / "
    "ProcessPortal」→ 083 全库 `*CPortal@@*`/`*CPortal::*` 命中 0（旧引用连 083 都不存在），改为真锚点 "
    "`CPortalList::FindPortal @0x712ab1`（按键通道：水平 ±range / 垂直固定 ±50 band，逐字 "
    "`dword_BF040C(a1: v9, a2: v7[3]-a5, a3: v7[4]-50, a4: v7[3]+a5, a5: v7[4]+50)`）+ "
    "`CPortalList::FindPortal_Collision @0x712b61`（碰触通道：按 portal w/h 半宽半高框 `v5[5]/2`、`v5[6]/2`）+ "
    "入口 `CUserLocal::CheckPortal_Collision @0x94dac6`（860B，按 `v7[2]` type 分派：`9`→`COutPacket(100)` 脚本门 / "
    "`12`→`sub_94DE22` / 其余→`CField::SendTransferFieldRequest` 或 `TryRegisterTeleport`） | "
    "`evidence/portal-special-citation-round93.json` + `notes/wave3-citation-fix-20261111.md` + "
    "`tools/_ida_r93/*`（portal_family.json / special_family.json / 3 份反编译）| "
    "门禁 5 禁想象修改（不把 083 type-dispatch 搬进本端 PortalScope 两通道抽象）/ 18 REA→IDA 只读取证；"
    "083/v83 only 禁 095 | 5 处纯注释（`IO/UIStateGame.cpp` / `IO/UITypes/UISpecial.h` / "
    "`Gameplay/MapleMap/MapPortals.h` / `Gameplay/MapleMap/MapPortals.cpp` / `Gameplay/Stage.cpp`）+ 四文档 | "
    "`tools/test_round93_citation_fix.py`（静态守卫：5 文件零 095/v95 依据语 + 083 真锚点在场 + "
    "`CPortal::CheckKey`/`ProcessPortal` 零残留 + 零行为字面量三态对拍）+ `gen_opcode_matrix.py --check`（GATE OK）+ "
    "`build_smoke.bat`（EXITCODE=0 真重编覆盖 5 文件） | "
    "✅ 本轮收口；`DECISIONS.md:130` 的「引 Angel 083 CPortal」同属符号名错误，已在 notes 记明不改 DECISIONS 历史行；"
    "真机五屏 60 节/142 处仍由用户认领 |"
)
if "| **2-41** |" not in dev:
    anchor = "| **2-40** |"
    idx = dev.index(anchor)
    line_end = dev.index(dev_eol, idx)
    dev = (dev[:line_end + len(dev_eol)]
           + ROW_241.replace("\n", dev_eol) + dev_eol
           + dev[line_end + len(dev_eol):])
    wr(DEV, dev)
    print("[doc] dev-plan 2-41 inserted")
else:
    print("[doc] dev-plan 2-41 already present")

PLT = "docs/features/pending-live-tests/README.md"
plt = rd(PLT)
plt_eol = nl_style(PLT)
SEC52 = "\n".join([
    "## 52. round93 · 095/v95 依据链纠偏（5 处纯注释）",
    "",
    "- **52-1** 福利中心（EVENT 键）打开/关闭行为与旧版完全一致（本轮零行为改动，只改注释依据）。",
    "- **52-2** 传送门行为不变：碰触类（TOUCH / SCRIPTED_TOUCH）行走经过即触发，其余门按上键触发；弹跳（SPRING1/2）不受影响。",
    "- **52-3** 若后续要把 083 `CUserLocal::CheckPortal_Collision @0x94dac6` 的「按 portal type 分派」"
    "（`9` 脚本门 / `12` / 其余传送）搬进本端 `MapPortals::find_warp_at` 两通道抽象，属行为级变更，"
    "须先立决策点（当前判「不搬」：本端 PortalScope 是等价抽象，搬会改触发时机）。",
    "- **52-4** 硬规则：注释里引 083 符号必须先跑 `entity_query` 确认符号真实存在；"
    "`*CPortal@@*`/`*CPortal::*` 这类旧引用在 083 全库命中为 0，写进注释即依据链污染。",
    "- 限制：① 纯注释 5 处，零行为 / 零 wire / 零协议 / 零 WZ；② `DECISIONS.md:130` 历史行不改写（只在本节记明）；"
    "③ 真机五屏截图仍由用户认领。",
    "",
])
if "## 52. round93" not in plt:
    plt = plt.rstrip("\r\n") + plt_eol + SEC52.replace("\n", plt_eol)
    wr(PLT, plt)
    print("[doc] PLT 52 appended")
else:
    print("[doc] PLT 52 already present")

WLE = "docs/features/angel-calibration-d-matrix-2026-11/WLE-TRIAGE-PLAN.md"
wle = rd(WLE)
wle_eol = nl_style(WLE)
SEC10_LINES = [
    "## 10. round93 纠偏 - 5 处 095/v95 依据链（m11680 盘点后自测）",
    "",
    "**结论：reopen 清单里唯一「该做且能自测」的一条 `activity-minigame/D1`（KEEP_WITH_NOTE）扩展成 5 处同类。**",
    "",
    "| 文件:行 | 旧依据 | 083 实测 | 判定 |",
    "| --- | --- | --- | --- |",
    "| `IO/UIStateGame.cpp:533` | 095 自定义中文 UI：福利中心 | `*UISpecial*`/`*Fairy*`/`*Welfare*`/`*CheckIn*`/`*Attendance*` 全 0 | 本端扩展（E 桶） |",
    "| `IO/UITypes/UISpecial.h:9` | 095 自定义中文 UI 入口 | 同上 | 本端扩展（E 桶） |",
    "| `Gameplay/MapleMap/MapPortals.h:40` | v95 `CPortal::CheckKey` / `ProcessPortal` | `*CPortal@@*`/`*CPortal::*` 全 0；真锚点 `CPortalList::FindPortal @0x712ab1` / `FindPortal_Collision @0x712b61` | 依据链错（符号名不存在） |",
    "| `Gameplay/MapleMap/MapPortals.cpp:146` | v95 同上 | 同上 | 依据链错 |",
    "| `Gameplay/Stage.cpp:284` | v95 `CPortal::CheckKey` | 同上 | 依据链错 |",
    "",
    "- 五处均为**注释**，零行为 / 零 wire / 零协议 / 零 WZ；`MapPortals` 两通道抽象（PortalScope）保留，"
    "不把 083 的 type 分派搬进来（那属行为级，另立决策点）。",
    "- `DECISIONS.md:130` 写「引 Angel 083 `CPortal`」同样引了不存在的符号名 —— 该行是历史裁定，不改写，仅在此记明。",
    "- 规则升级（承接 8/9 节）：注释引 083 符号前必须 `entity_query` 实证，符号名不存在即不得写入。",
    "- reopen `27 = 25 P1 + 1 W + 1 F-P2` 算术不动；168 条 DECISIONS 裁定不动（本轮只改注释依据）。",
    "",
]
SEC10 = "\n".join(SEC10_LINES)
if "## 10. round93" not in wle:
    wle = wle.rstrip("\r\n") + wle_eol + wle_eol + SEC10.replace("\n", wle_eol)
    wr(WLE, wle)
    print("[doc] WLE 10 appended")
else:
    print("[doc] WLE 10 already present")

EV = "docs/features/development-and-fix-plan-2026-11/evidence/portal-special-citation-round93.json"
ev = {
    "round": "round93",
    "date": "2026-11-11",
    "trigger": "m11680 还有未完成/待拍板/未自测的事项 -> 自测推进 reopen 唯一可自测项 activity-minigame/D1 并扩展同类",
    "scope": "comment_only_citation_fix",
    "zero_source_behavior": True,
    "zero_change_note": "5 处纯注释改写；零 wire / 零协议 / 零 WZ / 零新封包 / 零行为字面量",
    "patches": patch_report,
    "ida_evidence": {
        "special_family": {
            "file": "tools/_ida_r93/special_family.json",
            "queries": {
                "*Special*": "1 (GetSpecialUOL@SKILLENTRY, 技能 UOL 非窗类)",
                "*UISpecial*": 0,
                "*Fairy*": 0,
                "*Welfare*": 0,
                "*CheckIn*": 0,
                "*Attendance*": 0,
            },
            "verdict": "083 无福利中心窗族 -> 本端扩展 E 桶",
        },
        "portal_family": {
            "file": "tools/_ida_r93/portal_family.json",
            "queries": {"*CPortal@@*": 0, "*CPortal::*": 0, "*Portal*": 11},
            "real_anchors": {
                "CPortalList::FindPortal @0x712ab1": "按键通道：dword_BF040C(a1: v9, a2: v7[3]-a5, a3: v7[4]-50, a4: v7[3]+a5, a5: v7[4]+50)，水平 +-range、垂直固定 +-50",
                "CPortalList::FindPortal_Collision @0x712b61": "碰触通道：dword_BF040C(a1: v7, a2: v5[3]-v5[5]/2, a3: v5[4]-v5[6]/2, a4: v5[3]+v5[5]/2, a5: v5[4]+v5[6]/2)",
                "CUserLocal::CheckPortal_Collision @0x94dac6": "size 0x35c=860B；按 v7[2] type 分派：9 -> COutPacket(100) 脚本门 Encode1(field)+EncodeStr+Encode2(x)+Encode2(y)；12 -> sub_94DE22；其余 -> CField::SendTransferFieldRequest 或 TryRegisterTeleport",
            },
            "decompiled": [
                "tools/_ida_r93/CUserLocal_CheckPortal_Collision.code.txt (190 行)",
                "tools/_ida_r93/CPortalList_FindPortal.code.txt (32 行)",
                "tools/_ida_r93/CPortalList_FindPortal_Collision.code.txt (27 行)",
            ],
            "verdict": "旧注释引用 CPortal::CheckKey / ProcessPortal 在 083 全库不存在 -> 依据链错，换真锚点",
        },
    },
    "decision_points": {
        "portal_type_dispatch": "是否把 083 CheckPortal_Collision 的 type 分派搬进本端 MapPortals::find_warp_at？本轮判不搬：本端 PortalScope（TOUCH_ONLY/ALL）是等价抽象，搬会改触发时机 -> 行为级，另立决策点",
        "decisions_130": "DECISIONS.md:130 写引 Angel 083 CPortal 同属符号名错误；该行是历史裁定不改写，仅在 notes/WLE 10 节记明",
    },
    "arithmetic_unchanged": {
        "decisions_rows": 168,
        "wle_section4": "W 38 + L 130 = 168",
        "reopen": "27 = 25 P1 + 1 W + 1 F-P2",
        "note": "本轮只改注释依据，不动任何裁定与算术",
    },
    "constraints": [
        "083/v83 only，禁 095 依据",
        "门禁 5 禁想象修改：不把不存在的 083 符号写进注释",
        "门禁 18 REA->IDA->orange-wz->.nx 只读取证",
        "W 桶 6 字段宽度/类型 + 27 条 wire KEEP 零改动",
        "注释引 083 符号前必须 entity_query 实证",
    ],
    "live_backlog": "真机五屏 60 节/142 处由用户认领；InPacket::available() 393 处 27-A 逐面回归待真机",
}
wr(EV, json.dumps(ev, ensure_ascii=False, indent=2) + "\n")
print("[doc] evidence written", sha12(rd(EV)))

NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave3-citation-fix-20261111.md"
notes = """# round93 · 095/v95 依据链纠偏（5 处纯注释）

触发：用户 m11680「是否还有未完成、需要拍板、未自测的事项了？」→ 盘点后按 m09211
「全部继续，如果能自测的可以自己启动测试」自测推进 reopen 清单中唯一「该做且能自测」的一项
（`activity-minigame/D1` KEEP_WITH_NOTE），并在同族扫描中扩出另外 4 处。

## 1. 为什么是这 5 处

reopen 27 条里唯一明确「做：只换注释」的只有 `activity-minigame/D1`。沿同一模式
（注释里写「095/v95」当依据、或引 083 里不存在的符号名）全仓扫描，命中 5 处：

| 文件:行 | 旧依据语 |
| --- | --- |
| `IO/UIStateGame.cpp:533` | `// 095 自定义中文 UI：暂时复用 EVENT 键打开福利中心…` |
| `IO/UITypes/UISpecial.h:9` | `// 095 自定义中文 UI 入口：福利中心（签到/兑换/排行/转职/传送）。` |
| `Gameplay/MapleMap/MapPortals.h:40` | `// 传送门触发范围（对齐 v95：CPortal::CheckKey 的按键通道 vs ProcessPortal 的碰触通道）：` |
| `Gameplay/MapleMap/MapPortals.cpp:146` | `// 原生语义（v95 CPortal::CheckKey / ProcessPortal）：` |
| `Gameplay/Stage.cpp:284` | `// 其余传送门：按上键主动交互（对齐 v95 CPortal::CheckKey）` |

## 2. 083 活 IDB 实证

`tools/_r93_ida*.py`（endpoint `http://127.0.0.1:13339/mcp`，notification fire-and-forget）→

- `*UISpecial*` / `*Fairy*` / `*Welfare*` / `*CheckIn*` / `*Attendance*` **全 0**；
  `*Special*` 仅 1 条 `?GetSpecialUOL@SKILLENTRY@@QBE?AVZtl_bstr_t@@J`（技能 UOL，非窗类）
  ⇒ 福利中心不是 083 原生窗族，是**本端扩展（E 桶）**。按 m09729「扩展功能可以不对齐」，保留且零行为改动。
- `*CPortal@@*` / `*CPortal::*` **全 0** —— 旧注释引的 `CPortal::CheckKey` / `ProcessPortal`
  在 083 里**根本不存在**。真锚点在 `CPortalList` 与 `CUserLocal`：
  - `CPortalList::FindPortal @0x712ab1`（按键通道）：
    `dword_BF040C(a1: v9, a2: v7[3]-a5, a3: v7[4]-50, a4: v7[3]+a5, a5: v7[4]+50)`
    —— 水平 ±range、垂直固定 ±50 band。
  - `CPortalList::FindPortal_Collision @0x712b61`（碰触通道）：
    `dword_BF040C(a1: v7, a2: v5[3]-v5[5]/2, a3: v5[4]-v5[6]/2, a4: v5[3]+v5[5]/2, a5: v5[4]+v5[6]/2)`
    —— 按 portal 自身 w/h 半宽半高框。
  - `CUserLocal::CheckPortal_Collision @0x94dac6`（size `0x35c`=860B，入口）：命中后按 `v7[2]` type 分派
    —— `9` → `COutPacket(100)` + `Encode1(field)` + `EncodeStr` + `Encode2(x)` + `Encode2(y)` 脚本门；
    `12` → `sub_94DE22`；其余 → `CField::SendTransferFieldRequest` 或 `TryRegisterTeleport`。

## 3. 为什么不改行为

本端 `MapPortals::find_warp_at(playerpos, scope)` 用 `PortalScope{TOUCH_ONLY, ALL}` 两通道
表达「碰触自动触发 / 上键主动触发」，与 083 的「碰撞框命中后按 type 分派」是**等价抽象**。
把 083 的 type 分派原样搬进来会改触发时机（例如 type 9 脚本门会被自动触发），属**行为级变更**，
本轮判「不搬」，登记为决策点（PLT 52-3）。

## 4. DECISIONS.md:130 的处理

该行写「做：只换注释依据引 Angel 083 `CPortal`」——它引的符号名同样不存在。
但 `DECISIONS.md` 是 round86 的历史裁定表，按既往口径**不改写历史行**，
仅在本 notes 与 `WLE-TRIAGE-PLAN.md` 第 10 节记明；本轮实际依据以第 2 节的真锚点为准。

## 5. 规则升级

承接 round91 第 8 节 / round92 第 9 节：**注释里引 083 符号前，必须先 `entity_query` 实证符号存在**。
符号名不存在（如 `CPortal::CheckKey`）写进注释即依据链污染 —— 本轮由 harness 静态守卫
`tools/test_round93_citation_fix.py` 逐条断言（5 文件零 `095`/`v95` 依据语、
`CPortal::CheckKey`/`ProcessPortal` 零残留、083 真锚点在场）。

算术不动：168 条 DECISIONS 裁定、`W 38 + L 130 = 168`、reopen `27 = 25 P1 + 1 W + 1 F-P2` 全部保持。
"""
wr(NOTES, notes)
print("[doc] notes written", sha12(rd(NOTES)))

print("--- EOL audit ---")
for p in ["IO/UIStateGame.cpp", "IO/UITypes/UISpecial.h", "Gameplay/MapleMap/MapPortals.h",
          "Gameplay/MapleMap/MapPortals.cpp", "Gameplay/Stage.cpp",
          DEV, PLT, WLE, EV, NOTES]:
    a = eol_audit(p)
    print("EOL %-28s crlf=%-5d lonelf=%-4d crcrlf=%d bom=%s bytes=%d" % (
        p.split("/")[-1], a["crlf"], a["lonelf"], a["crcrlf"], a["bom"], a["bytes"]))

print("ROUND93_DELIVER_ALL_OK")
