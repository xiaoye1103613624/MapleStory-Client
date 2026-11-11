# -*- coding: utf-8 -*-
"""round89 W/L/E 三桶 + 口径修订 + 一次性 P1 Plan + UIQuestLog 真实对齐补丁。

幂等 / EOL 精确保留 / 零 wire 改动 / 不读 git log（round88 B9b 教训）。
证据来源（全部只读取证）：
  - tools/_r88_rows.json（DECISIONS.md §4 解析的 168 行，稳定 ASCII 字段）
  - docs/features/angel-calibration-d-matrix-2026-11/evidence/three-way-consistency-round88.json
  - tools/_ida_r89/CUIQuestInfo_OnTabChanged.code.txt（083 活 IDA 反编译逐字）
  - IO/UITypes/UIQuestLog.cpp（当前端 change_tab / rebuild_list / update_detail）
  - E:/project/BeiDou-Server_s9/.../PacketCreator.java（s9 改造字段逐字）
触发：User said (m09729) 字段级 body 改造过的字段/类型不影响，但一样字段的原始逻辑可以对齐；
      User said (m09737) 其余功能也要按照这个方向进行开发、修复、完善。
"""
import json, os, hashlib
from collections import Counter

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)

MATRIX = "docs/features/angel-calibration-d-matrix-2026-11"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave3-wle-triage-questlog-20261111.md"
EV = f"{MATRIX}/evidence/wle-triage-round89.json"
PLAN = f"{MATRIX}/WLE-TRIAGE-PLAN.md"
VERDICT = f"{MATRIX}/CONSISTENCY-VERDICT.md"
CPP = "IO/UITypes/UIQuestLog.cpp"
R88EV = f"{MATRIX}/evidence/three-way-consistency-round88.json"
ROWS = "tools/_r88_rows.json"
IDA = "tools/_ida_r89/CUIQuestInfo_OnTabChanged.code.txt"
S9 = r"E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/util/PacketCreator.java"

TRIGGERS = [
    "字段级 body 改造过的字段、类型不影响，但是一样字段的原始逻辑可以对齐一下。目的就是为了根据最完全的反编译的客户端，来还原当前客户端原始逻辑和代码，保证基础功能一致，扩展功能可以不对齐；",
    "其余功能也要按照这个方向进行开发、修复、完善；",
]


def rb(p):
    with open(p, "rb") as f:
        return f.read()


def wb(p, b):
    with open(p, "wb") as f:
        f.write(b)


def rs(p, enc="utf-8"):
    with open(p, "r", encoding=enc, errors="replace", newline="") as f:
        return f.read()


def ws(p, s, enc="utf-8"):
    with open(p, "w", encoding=enc, newline="") as f:
        f.write(s)


# ---- 1. 稳定输入校验（缺一即 FAIL，不放宽）----
rows = json.load(open(ROWS, encoding="utf-8"))
assert isinstance(rows, list) and len(rows) == 168, len(rows)
cls = Counter(r["class"] for r in rows)
assert cls == Counter({"PURE": 76, "WIRE": 38, "MIXED": 14, "OTHER": 40}), cls

r88 = json.load(open(R88EV, encoding="utf-8"))
assert r88["classification_168"]["total"] == 168
reopen = r88["reopen_candidates"]
must_keep = r88["must_keep_s9"]
assert len(reopen) == 27 and len(must_keep) == 27

# 只取 ASCII 稳定字段，绝不把 GBK 乱码 rec 写进新产物（round88 教训）
def slim(r):
    return {k: r[k] for k in ("face", "id", "kind", "verdict") if k in r} | (
        {"cls": r["cls"]} if "cls" in r else {})

reopen_slim = [slim(r) for r in reopen]
must_slim = [slim(r) for r in must_keep]

ida = rs(IDA)
assert "*(this + 369) = -1;" in ida and "CUIQuestInfo::OnTabChanged" in ida
assert "TryShowTopQuest" in ida and "ToggleDetail(this, a2: 0)" in ida

s9 = rs(S9)
W_FIELDS = [
    dict(tag="level 2B", code="p.writeShort(chr.getLevel());", line=192, why="083 原生 level 1B；s9 999 级上限改 2B"),
    dict(tag="HP/MP int", code="p.writeInt(chr.getHp());", line=201, why="083 原生 short；s9 99999 上限改 int"),
    dict(tag="exp 8B", code="p.writeLong(chr.getExp());", line=211, why="083 原生 exp 4B；s9 999 级累积经验改 8B"),
    dict(tag="GachaExp 4B", code="p.writeInt((int) chr.getGachaExp());", line=213, why="083 无此字段；s9 扩展通道"),
    dict(tag="spawn tail 4B", code="p.writeInt(0);", line=217, why="083 无此尾部；s9 spawnpoint 后固定补 int(0)"),
    dict(tag="removeItemFromMap 4B cid", code="public static Packet removeItemFromMap(int objId, int animation, int chrId)", line=2624, why="animation>=2 恒写 4B cid；083 OnPacket v2==4 读 Decode2 delay，数值重合≠语义重合"),
]
for w in W_FIELDS:
    assert w["code"] in s9, f"s9 mark missing: {w['code']}"

cpp_b = rb(CPP)
assert b"\r\n" in cpp_b and b"\n" not in cpp_b.replace(b"\r\n", b""), "UIQuestLog.cpp must be pure CRLF"
assert not cpp_b.startswith(b"\xef\xbb\xbf"), "UIQuestLog.cpp has no BOM"

# ---- 2. W/L/E 三桶（处置轨道，非语义再分类；只用稳定字段）----
# W 轨 = 碰 wire / opcode 的 38 条 → 保持 s9 宽度/类型，零改动（含 must_keep 27）。
# L 轨 = 其余 130 条（PURE76 + MIXED14 + OTHER40）→ 同字段原始逻辑/状态机/分支按 Angel 083 还原；
#        其中 reopen 27 条为 round86 一刀切 KEEP 的纠偏对象，逐条 P1 拍板。
# E  = 跨轨豁免标记：s9 私有扩展功能（不对齐），逐项带证据，不参与 168 算术。
W_TRACK = cls["WIRE"]
L_TRACK = cls["PURE"] + cls["MIXED"] + cls["OTHER"]
assert W_TRACK + L_TRACK == 168

E_EXEMPT = [
    dict(item="GachaExp 扩展通道", evidence="PacketCreator.java:213 p.writeInt((int) chr.getGachaExp());", why="083 无此字段，s9 私有扩展"),
    dict(item="999 级上限", evidence="PacketCreator.java:192 p.writeShort(chr.getLevel()); // 999 级", why="083 原生 1B level，s9 私有数值扩展"),
    dict(item="99999 HP/MP 上限", evidence="PacketCreator.java:198-205 p.writeInt(chr.getHp()); ×4", why="083 原生 short，s9 私有数值扩展"),
    dict(item="c2s 107 抓包工具", evidence="docs/features/pending-live-tests/README.md P3 长尾台账", why="s9 侧运营工具，客户端基础功能不含"),
    dict(item="095 自定义中文 UI（福利中心）", evidence="IO/UIStateGame.cpp:532 + IO/UITypes/UISpecial.h:9（round86 T2 已判为本端扩展身份标注）", why="本端扩展入口，非 083 原生功能，机械换地址=二次造假"),
]

# 本轮 L 桶真实落地补丁（唯一一处源码改动，零 wire）
PATCH_NOTE = (
    "\t\t// round89 W/L/E：对齐 083 CUIQuestInfo::OnTabChanged @0x87fd4f 首行 *(this + 369) = -1;\r\n"
    "\t\t// 切换分类页签必须先清选中，再由 rebuild_list() 自动选第一条可见任务（TryShowTopQuest /\r\n"
    "\t\t// ToggleDetail(0) 语义）；旧实现保留上一页的 selected_id，跨页残留导致摘要页显示旧任务。\r\n"
    "\t\t// 纯客户端状态机：零 wire / 零协议 / 零 WZ / 零新封包，不触碰 s9 改造字段宽度。\r\n"
    "\t\tselected_id = -1;\r\n"
)
PATCH_ANCHOR = b"\t\ttab = tabid;\r\n"
PATCH_MARK = b"round89 W/L/E"

# ---- 3. 幂等写：真实补丁（bytes，CRLF 精确保留）----
# 不可变基准：切掉 round89 注释块后的字节 sha256（与 git HEAD 无关，提交后 harness 仍可核 0 行为改动）
_patch_bytes = PATCH_NOTE.encode("utf-8")
_recon = cpp_b.replace(_patch_bytes, b"", 1) if _patch_bytes in cpp_b else cpp_b
PRE_PATCH_SHA256 = hashlib.sha256(_recon).hexdigest()

cpp_out = cpp_b
if PATCH_MARK not in cpp_b:
    assert cpp_b.count(PATCH_ANCHOR) == 1, cpp_b.count(PATCH_ANCHOR)
    cpp_out = cpp_b.replace(PATCH_ANCHOR, PATCH_ANCHOR + _patch_bytes, 1)
    wb(CPP, cpp_out)

# ---- 4. 口径修订段：追加 §7（不删 §0/§2.2/§5 历史证据链）----
ver = rs(VERDICT)
SEC7_HEAD = "## 7. round89 口径修订（m09729 / m09737，本节覆盖前文的收窄表述）"
if SEC7_HEAD not in ver:
    sec7 = "\n".join([
        "",
        SEC7_HEAD,
        "",
        "> 触发（m09729）：字段级 body 改造过的字段、类型不影响，但是一样字段的原始逻辑可以对齐一下……保证基础功能一致，扩展功能可以不对齐；",
        "> 触发（m09737）：其余功能也要按照这个方向进行开发、修复、完善；",
        "",
        "**本节不删改 §0 / §2.1 / §2.2 / §5 的历史证据链，只把「不触发全部对齐」这个过宽结论收窄为 W/L/E 三桶。**",
        "",
        "### 7.1 三桶定义",
        "",
        "| 桶 | 范围 | 处置 | 依据 |",
        "| --- | --- | --- | --- |",
        "| **W** | 6 个 s9 改造字段的宽度/类型（level 2B / HP-MP int / exp 8B / GachaExp 4B / spawn tail int0 / removeItemFromMap 4B cid） | **保持 s9，零改动** | `PacketCreator.java:192/201/211/213/217/2624` + `LoginParser.cpp:150-174` 白屏实锤 |",
        "| **L** | 同字段的**原始业务逻辑 / 状态机 / 分支**（不碰 wire） | **按 Angel 083 最完全反编译还原**，保证基础功能一致 | `tools/_ida_r89/CUIQuestInfo_OnTabChanged.code.txt` 等活 IDA |",
        "| **E** | s9 私有扩展功能 | **可不对齐**（豁免） | 见 `WLE-TRIAGE-PLAN.md §3`，5 项逐条带证据 |",
        "",
        "### 7.2 对 §0 / §2.2 的收窄",
        "",
        "- §0「所以『全部按 Angel 对齐』的前提不成立」→ **仅对 W 桶成立**；L 桶前提成立，须按 083 还原。",
        "- §2.2「body/wire 层前提不成立 → 不触发全部对齐」→ **改为**：W 桶 6 个字段宽度/类型不触发（改回断联机复现白屏）；**L 桶同字段原始逻辑按 Angel 083 还原照常触发**，与 wire 层正交。",
        "- 白屏案例（`LoginParser.cpp:150-174`）证明的是「字段宽度不能改回 083」，**不证明「字段上的业务逻辑不能按 083 对齐」**——这两件事必须分开，round88 把后者一起挡掉是口径过宽。",
        "",
        "### 7.3 对 §5 策略的补正",
        "",
        "1. wire / opcode 层：s9 源码优先，不可动（W 桶 38 条 WIRE 轨，含 must_keep 27）。",
        "2. 字段级 body：`PLAN.md §P2` 11 片逐片探针 + 一片一原子 commit，以 s9 ByteBuf 序列为准（W 桶）。",
        "3. **L 桶（新增）**：纯客户端原始逻辑 / 状态机 / 分支按 Angel 083 还原；reopen 27 条逐条 P1 拍板，证据不足的不发明（门禁 5）。本轮首例 = `UIQuestLog::change_tab` 清选中。",
        "4. **E 桶（新增）**：s9 私有扩展功能不对齐，逐项披露、不硬凑。",
        "",
        "### 7.4 168 条路由闭合",
        "",
        "`W 轨 38（WIRE）+ L 轨 130（PURE 76 + MIXED 14 + OTHER 40）= 168`，未决 0；E 为 L 轨内跨轨豁免标记，不参与算术。逐条清单见 `WLE-TRIAGE-PLAN.md` 与 `evidence/wle-triage-round89.json`。",
        "",
        "### 7.5 红线保留",
        "",
        "083/v83 only 禁 095；数值重合≠语义重合；门禁 5 禁想象修改；门禁 18 REA→IDA→orange-wz→.nx 只读取证；本轮零 wire / 零协议 / 零 WZ 改动。",
    ]).rstrip("\n") + "\n"
    ver = ver.rstrip("\n") + "\n" + sec7
    ws(VERDICT, ver)

# ---- 5. W/L/E 三桶总清单 + 一次性 P1 Plan ----
plan_lines = [
    "# round89 · W/L/E 三桶总清单 + 一次性 P1 Plan",
    "",
    "> 触发：m09729「字段级 body 改造过的字段、类型不影响，但是一样字段的原始逻辑可以对齐一下……保证基础功能一致，扩展功能可以不对齐；」+ m09737「其余功能也要按照这个方向进行开发、修复、完善；」",
    "> 权威源：Angel(13339) 083/v83 带符号 IDB；s9 服务端源码仅约束 W 桶字段宽度；本端 HeavenClient 自研重实现。",
    "",
    "## 0. 口径（对 round88 §0/§2.2/§5 的收窄，历史证据链保留）",
    "",
    "- **W**（6 个改造字段宽度/类型）→ 保持 s9，零改动。改回 083 原生会平移包中间字段、复现 `LoginParser.cpp:150-174` 白屏。",
    "- **L**（同字段原始逻辑/状态机/分支，不碰 wire）→ 按 Angel 083 最完全反编译还原，保证基础功能一致。",
    "- **E**（s9 私有扩展功能）→ 可不对齐。",
    "- 红线：083/v83 only 禁 095；数值重合≠语义重合；门禁 5 禁想象修改；数值重合≠语义重合。",
    "",
    "## 1. W 桶 — 保持 s9 宽度/类型（6 字段 + 27 条 must_keep）",
    "",
    "| # | 行 | s9 源码 | 083 差异 | 原因 |",
    "| --- | --- | --- | --- | --- |",
]
for i, w in enumerate(W_FIELDS, 1):
    plan_lines.append(f"| {i} | {w['line']} | `{w['code']}` | {w['why']} | s9 改造，保持 |")
plan_lines += [
    "",
    "**碰 wire 的 27 条 must_keep（round88 evidence 直接引用，全部归 W 轨）**：",
    "",
]
for r in must_slim:
    plan_lines.append(f"- `{r['face']}/{r['id']}` {r['kind']} [{r['verdict']}] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束")
plan_lines += [
    "",
    "## 2. L 桶 — 按 Angel 083 还原原始逻辑（reopen 27 + 本轮 1 + drop 2）",
    "",
    "### 2.1 本轮唯一落地补丁（零 wire）",
    "",
    "- `IO/UITypes/UIQuestLog.cpp::change_tab`：083 `CUIQuestInfo::OnTabChanged @0x87fd4f` 首行 `*(this + 369) = -1;`（清选中）→ 当前端在 `tab = tabid;` 后补 `selected_id = -1;`，由 `rebuild_list()` 自动选第一条可见任务，对齐 `TryShowTopQuest` / `ToggleDetail(0)` 语义。零 wire / 零协议 / 零 WZ / 零新封包。",
    "- `quest/D5` 的 `UIQuestLog::send_key` 空实现**不改**：083 全库 `*Quest*OnKey*` / `*Quest*SendKey*` 符号 total=0，为对齐而发明键盘逻辑违反门禁 5。",
    "",
    "### 2.2 round86 一刀切 KEEP 的纠偏对象（reopen 27，逐条 P1 拍板）",
    "",
    "| face/id | kind | 原 verdict | 类别 |",
    "| --- | --- | --- | --- |",
]
for r in reopen_slim:
    plan_lines.append(f"| `{r['face']}/{r['id']}` | {r['kind']} | `{r['verdict']}` | {r.get('cls','PURE/MIXED')} |")
plan_lines += [
    "",
    "### 2.3 已有 083 活 IDA 证据、登记 P1（本轮不落补丁）",
    "",
    "- `drop/D2`（animation=0 消亡动画）+ `drop/D12`（animation=4 延时爆炸帧）：`CDropPool::OnDropEnterField @0x505900` 反编译逐字已落 `tools/_ida_r89/`，分支含 `MakeMoneyAnimation` / `*(v115+18)=3`，按 L 桶下一批还原。",
    "- `quest/D8`、`skill/D4`、`corewindow/D8` 等状态机分支同理，逐面 REA→IDA→IMG 后再落，不凭空写。",
    "",
    "## 3. E 桶 — s9 私有扩展（不对齐，5 项逐条带证据）",
    "",
    "| 扩展项 | 证据 | 理由 |",
    "| --- | --- | --- |",
]
for e in E_EXEMPT:
    plan_lines.append(f"| {e['item']} | `{e['evidence']}` | {e['why']} |")
plan_lines += [
    "",
    "E 为保守枚举：未列扩展项默认仍归 L 轨，逐条拍板时可追加，不硬凑数字。",
    "",
    "## 4. 168 条路由闭合",
    "",
    "| 轨道 | 组成 | 数量 |",
    "| --- | --- | --- |",
    "| **W 轨** | `class=WIRE`（含 must_keep 27） | 38 |",
    "| **L 轨** | `class=PURE` 76 + `MIXED` 14 + `OTHER` 40 | 130 |",
    "| **合计** | 38 + 130 | **168** |",
    "",
    "round88 四分类计数 `PURE76/WIRE38/MIXED14/OTHER40` 直接复用（稳定、已证据化），不做语义再分类造假；E 为 L 轨内豁免标记，不参与算术。未决 0。",
    "",
    "## 5. 一次性 P1 Plan（本轮 + 后续同方向批次）",
    "",
    "1. **本轮（已落地）**：W/L/E 口径修订 + 三桶总清单 + `UIQuestLog::change_tab` 1 行真实对齐补丁（083 反编译逐字为凭）；harness + GATE + 真重编三证 + 零署名原子提交。",
    "2. **下一批（drop 面）**：`CDropPool::OnDropEnterField` 的 animation 分支按 083 还原，REA→IDA→orange-wz(128B)→.nx 只读取证 → 纯逻辑补丁 → harness → GATE → 真重编 → 零署名提交。",
    "3. **逐面推进**：reopen 27 条按 12 面分批，每批先出 083 符号/反编译锚点，证据不足的登记 P2/P3 不发明；每批一片一原子 commit。",
    "4. **W 桶全程零改动**：6 字段宽度/类型 + 27 条 wire KEEP 不碰，避免复现白屏。",
    "5. **真机归口**：五屏截图 60 节/142 处、`InPacket::available()` 393 处 §27-A 逐面回归、D2 聊天规则，由用户认领后销案。",
    "",
    "## 6. 限制",
    "",
    "- 本轮唯一源码改动 = `UIQuestLog.cpp` 1 行 + 4 行注释；零 wire / 零协议 / 零 WZ / 零新封包。",
    "- 工作树 17 个外部程序在途 WIP 原样保留，精确 add 隔离、零夹带。",
    "- 三桶只对「处置取向」分类，不改写 round86 DECISIONS.md 的 168 条历史裁定。",
    "- 架构级重写（如独立 CUIPartyHP 浮窗、CUIStatusBar 拆 10+2 类）仍须单点裁决，不在自动落地范围。",
]
ws(PLAN, "\n".join(plan_lines).rstrip("\n") + "\n")

# ---- 6. evidence JSON（不含 git log / commit hash，保证跨 commit 幂等）----
ev = dict(
    round=89,
    date="2026-11-11",
    triggers=TRIGGERS,
    buckets=dict(
        W=dict(meaning="s9 改造字段宽度/类型 + 碰 wire 的 38 条 → 保持 s9 零改动",
               fields=W_FIELDS, must_keep_s9=must_slim, must_keep_count=27),
        L=dict(meaning="同字段原始逻辑/状态机/分支 → 按 Angel 083 最完全反编译还原",
               reopen_candidates=reopen_slim, reopen_count=27,
               landed_patch=dict(file="IO/UITypes/UIQuestLog.cpp", func="change_tab",
                                 anchor="tab = tabid;", added="selected_id = -1;",
                                 ida_basis="CUIQuestInfo::OnTabChanged @0x87fd4f *(this + 369) = -1;",
                                 ida_file="tools/_ida_r89/CUIQuestInfo_OnTabChanged.code.txt",
                                 wire_change=False, protocol_change=False, wz_change=False, new_packet=False),
               p1_registered=["drop/D2 animation=0 消亡动画", "drop/D12 animation=4 延时爆炸帧", "quest/D8 条件分支"]),
        E=dict(meaning="s9 私有扩展功能 → 可不对齐（L 轨内豁免标记，不参与 168 算术）",
               items=E_EXEMPT, count=5),
    ),
    routing_168=dict(W_track=38, L_track=130, L_composition="PURE 76 + MIXED 14 + OTHER 40",
                     total=168, arithmetic="38+130=168", undecided=0,
                     source="round88 evidence classification_168（稳定、已证据化）"),
    quest_d5_note="083 全库 *Quest*OnKey* / *Quest*SendKey* 符号 total=0；UIQuestLog::send_key 空实现保持，门禁 5 禁想象修改",
    ida_evidence=dict(endpoint="http://127.0.0.1:13339/mcp",
                      symbol="?OnTabChanged@CUIQuestInfo@@IAEXJ@Z 0x87fd4f size 0x7a",
                      first_line="*(this + 369) = -1; /*0x87fd57*/",
                      tail_calls=["LoadData", "SetScrollBar", "TryShowTopQuest", "ToggleDetail(0)", "SyncScrollBarWithSelected"]),
    current_impl_before=dict(file=CPP, func="change_tab", lines="694-709",
                             defect="切页签保留上一页 selected_id，跨页残留导致摘要页显示旧任务",
                             safety="update_detail() :625 if (selected_id < 0) return；rebuild_list() :584 自动选第一条可见任务"),
    patch_integrity=dict(pre_patch_cpp_sha256=PRE_PATCH_SHA256,
                        method="切掉 round89 注释块后的字节 sha256；与 git HEAD 无关，提交后 harness 仍可核唯一源码改动"),
    zero_wire_change=True,
    zero_protocol_change=True,
    zero_wz_change=True,
    source_files_changed=[CPP],
    ext_wip_count=17,
    constraints=["083/v83 only 禁 095", "s9 协议优先（收窄：仅 W 桶 6 字段宽度/类型）",
                 "数值重合≠语义重合", "校准≠改回原生（收窄：L 桶同字段原始逻辑按 Angel 083 还原）",
                 "门禁 5 禁想象修改", "门禁 18 REA→IDA→orange-wz→.nx 只读取证",
                 "orange-wz MCP 128B WzAESConstant.DEFAULT_KEY / .nx 65536B key_gms 双规格"],
)
ws(EV, json.dumps(ev, ensure_ascii=False, indent=2) + "\n")

# ---- 7. notes ----
notes = "\n".join([
    "# round89 · W/L/E 三桶 + UIQuestLog 真实对齐补丁（wave3）",
    "",
    "## 1. 触发与口径",
    "",
    "- m09729：字段级 body 改造过的字段/类型不影响，但一样字段的原始逻辑可以对齐；目的是按最完全反编译客户端还原当前客户端原始逻辑和代码，保证基础功能一致，扩展功能可以不对齐。",
    "- m09737：其余功能也按这个方向开发、修复、完善。",
    "- round88 §2.2「body 层不一致→不触发全部对齐」过宽 → 收窄为 W（6 字段保持 s9）/ L（同字段原始逻辑按 083 还原）/ E（扩展不对齐）三桶，历史证据链不删。",
    "",
    "## 2. 本轮真实补丁（零 wire）",
    "",
    "- `IO/UITypes/UIQuestLog.cpp::change_tab`：`tab = tabid;` 后补 `selected_id = -1;`。",
    "- 083 依据：`CUIQuestInfo::OnTabChanged @0x87fd4f` 首行 `*(this + 369) = -1;`（`tools/_ida_r89/CUIQuestInfo_OnTabChanged.code.txt` 逐字），随后 `LoadData/SetScrollBar/…/TryShowTopQuest/ToggleDetail(0)/SyncScrollBarWithSelected`。",
    "- 当前端 `rebuild_list()` 在 `selected_id` 不在新列表时自动选第一条可见任务；`update_detail()` 对 `selected_id < 0` 有空守卫 → 补这一行即复现 083 语义且无副作用。",
    "- `quest/D5 send_key` 空实现保持：083 全库无 Quest 键盘 handler 符号，门禁 5 禁想象修改。",
    "",
    "## 3. 三桶闭合",
    "",
    "- W 轨 38（WIRE，含 must_keep 27）+ L 轨 130（PURE76+MIXED14+OTHER40）= 168，未决 0。",
    "- E 桶 5 项 s9 私有扩展为 L 轨内豁免标记，不参与算术。",
    "- reopen 27 条逐条登记 P1；drop/D2、D12 已有 083 反编译锚点，下一批还原。",
    "",
    "## 4. 自测与交付",
    "",
    "- harness `tools/test_round89_wle_triage.py`；GATE `gen_opcode_matrix.py --check`；真重编 `tools\\build_smoke.bat`（touch UIQuestLog.cpp 后 build_log/EXE/覆盖三证）。",
    "- 精确 add 零署名提交；17 个外部在途 WIP 原样隔离；feedback 无 rating；goal 保持 active。",
    "",
    "## 5. 限制",
    "",
    "- 本轮 1 行源码 + 4 行注释，零 wire / 零协议 / 零 WZ / 零新封包。",
    "- 架构级重写与真机观感项仍单点裁决 / 用户认领。",
]) + "\n"
ws(NOTES, notes)

# ---- 8. dev-plan §4 2-37 行（LF，幂等）----
dev = rs(DEV)
ROW37 = "| **2-37** | round89 W/L/E 三桶口径修订 + `UIQuestLog::change_tab` 按 083 `OnTabChanged @0x87fd4f` 清选中（L 桶首例，零 wire） | `d9e4b676` 后续原子提交 / evidence `wle-triage-round89.json` | 纯客户端状态机，W 桶 6 字段保持 s9，E 桶扩展不对齐 |"
if "**2-37**" not in dev:
    anchor = "| **2-36** |"
    idx = dev.find(anchor)
    assert idx >= 0
    eol = dev.find("\n", idx)
    assert eol >= 0
    dev = dev[:eol + 1] + ROW37 + "\n" + dev[eol + 1:]
    ws(DEV, dev)

# ---- 9. PLT §48（CRLF，rstrip 后单 CRLF 收尾，避免 new blank line at EOF）----
plt_b = rb(PLT)
assert b"\r\n" in plt_b and b"\n" not in plt_b.replace(b"\r\n", b""), "PLT must be pure CRLF"
plt_s = plt_b.decode("utf-8")
if "## 48. round89" not in plt_s:
    s48 = [
        "## 48. round89 · W/L/E 三桶 + UIQuestLog 清选中（L 桶首例）",
        "",
        "48-1. 口径修订：round88 §0/§2.2/§5 的「不触发全部对齐」收窄为 W（6 字段保持 s9）/ L（同字段原始逻辑按 083 还原）/ E（扩展不对齐）；CONSISTENCY-VERDICT.md 追加 §7，历史证据链不删。",
        "48-2. 真机归口（用户认领）：切换任务分类页签后，摘要页是否显示新页第一条任务、不再残留上一页选中项；展开态/空列表/无可见任务三种边界。",
        "48-3. 真机归口（用户认领）：宠物装备面板等 L 桶后续批次的观感对齐（承接 §45 四项）。",
        "48-4. 真机归口（用户认领）：drop animation=0/4 分支还原后的消亡/爆炸帧观感（承接 §48 下一批 P1）。",
        "48-5. 验收：harness 全绿 + GATE OK + 真重编三证（build_log/EXE/UIQuestLog.cpp 覆盖）+ 零署名提交 + 17 个外部在途 WIP 零夹带。",
        "",
        "限制：① 本轮 1 行源码 + 4 行注释，零 wire / 零协议 / 零 WZ；② 17 个外部在途 WIP 原样留工作树；③ reopen 27 条为 P1 待逐条拍板，非本轮全改；④ 架构级重写单点裁决、真机五屏截图 60 节/142 处仍由用户认领。",
    ]
    block = "\r\n".join(s48).rstrip("\r\n") + "\r\n"
    plt_s = plt_s.rstrip("\r\n") + "\r\n\r\n" + block
    wb(PLT, plt_s.encode("utf-8"))

# ---- 10. 收口输出（稳定 SHA，不含 git）----
out = [EV, PLAN, NOTES, DEV, PLT, VERDICT, CPP]
print("ROUND89_DELIVER_ALL_OK")
for p in out:
    b = rb(p)
    print(f"  {hashlib.sha256(b).hexdigest()[:12]} {len(b):>7}B  {p}")
