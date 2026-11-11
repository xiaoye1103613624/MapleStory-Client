# -*- coding: utf-8 -*-
"""round88 三方一致性确认书生成器（幂等 / EOL 精确保留 / 0 源码改动）。
证据来源：s9 PacketCreator.java 源码逐字 + 客户端 LoginParser.cpp + Q1 EA + opcode matrix + DECISIONS.md §4。
"""
import json, re, os, hashlib
from collections import Counter

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)

S9 = r"E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/util/PacketCreator.java"
MATRIX = "docs/features/angel-calibration-d-matrix-2026-11"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
EV = f"{MATRIX}/evidence/three-way-consistency-round88.json"
VERDICT = f"{MATRIX}/CONSISTENCY-VERDICT.md"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave3-three-way-consistency-20261111.md"

def rb(p):
    with open(p, "rb") as f: return f.read()
def rs(p, enc="utf-8"):
    with open(p, "r", encoding=enc, errors="replace", newline="") as f: return f.read()

# ---- 1. s9 wire divergences（源码逐字核证，缺一即 FAIL）----
s9 = rs(S9)
S9_MARKS = [
    ("level 2B",       "p.writeShort(chr.getLevel());", 192, "083 原生 level 1B；s9 因 999 级上限改 2B"),
    ("HP/MP int",      "p.writeInt(chr.getHp());",      201, "083 原生 short；s9 因 99999 上限改 int"),
    ("exp 8B",         "p.writeLong(chr.getExp());",    211, "083 原生 exp 4B；s9 因 999 级累积经验改 8B"),
    ("GachaExp 4B",    "p.writeInt((int) chr.getGachaExp());", 213, "083 无此字段；s9 扩展通道"),
    ("spawn tail 4B",  "p.writeInt(0);",                217, "083 无此尾部；s9 spawnpoint 后固定补 int(0)"),
    ("hello 83",       "p.writeShort(0x0E);",           632, "握手声明版本 83 → 与 Angel v83 一致"),
]
s9_div = []
for tag, code, approx, why in S9_MARKS:
    assert code in s9, f"s9 source mark missing: {code}"
    s9_div.append(dict(tag=tag, code=code, approx_line=approx, why_083_differs=why))
# spawnpoint 前一行存在性
assert "p.writeByte(chr.getInitialSpawnPoint());" in s9
assert "public static Packet removeItemFromMap(int objId, int animation, int chrId)" in s9

# ---- 2. Q1 同二进制 ----
sc = json.load(open("docs/features/ida-v83-angel-instance/evidence/ida-symbol-compare.json", encoding="utf-8"))
ea = json.load(open("docs/features/ida-v83-angel-instance/evidence/angel-gap-feature-ea.json", encoding="utf-8"))
q1 = dict(
    angel_13339_decorated=sc["13339"]["decorated"],
    beidou_13337_decorated=sc["13337"]["decorated"],
    angel_13339_total=sc["13339"]["total"],
    beidou_13337_total=sc["13337"]["total"],
    same_version_string="MapleStory_Global:%s" in sc["13339"]["version_strings"] and "MapleStory_Global:%s" in sc["13337"]["version_strings"],
    ea_target_classes=ea["target_classes"], ea_matched_classes=ea["matched_classes"], ea_named_funcs=ea["total_named_funcs"],
    conclusion="Angel(13339) 与 BeiDou(13337) 是同一份 v83/083 GMS 二进制的两个 IDA 实例，仅符号还原状态不同；当前端是 HeavenClient 从零自研重实现，与这两个 exe 无代码继承关系。",
)
assert q1["angel_13339_decorated"] == 3122 and q1["beidou_13337_decorated"] == 50
assert q1["same_version_string"] and q1["ea_matched_classes"] == 95

# ---- 3. opcode GATE 层一致 ----
mx = json.load(open("docs/features/s9-protocol-align/protocol/opcode-matrix.json", encoding="utf-8"))
gate = dict(
    s9_git_head=mx["_meta"]["s9_git_head"],
    counts=mx["_meta"]["counts"],
    meaning="opcode 名/值 + 客户端注册 + s9 handler 三方对账；握手/版本/加解密同形 → 能联机的层一致",
)
assert gate["s9_git_head"].startswith("a78ba681a4")
assert gate["counts"]["client_out_sent"] == 148

# ---- 4. 客户端白屏实锤（原先逻辑错误样本，已修）----
lp = rs("Net/Handlers/Helpers/LoginParser.cpp")
assert "进图白屏" in lp and "statsentry.exp = recv.read_long();" in lp and "recv.skip(4);" in lp
whitescreen = dict(
    file="Net/Handlers/Helpers/LoginParser.cpp:150-174",
    old_bug="旧代码按 8B exp 解析 s9 包 → skip(4) 吞掉真实 mapId(100000000) → 从 spawnpoint 槽读出 16 当 mapid → Stage::load 抛 map data missing → 进图白屏",
    current_fix="recv.read_long() 读 exp + 末尾 recv.skip(4) 吃掉 s9 的 int(0) tail；已与 s9 PacketCreator.java:214-217 逐字段对齐",
    meaning="用户断言『原先功能很多是错的』成立；此例是协议字段宽度错配的典型，已按服务端源码修复而非按 083 原生。",
)

# ---- 5. DECISIONS.md §4 168 行 → 三分类（PURE/WIRE/MIXED/OTHER）----
dec = rs(f"{MATRIX}/DECISIONS.md")
rows, face = [], None
for i, ln in enumerate(dec.split("\n")):
    m = re.match(r"### 4\.\d+ .*?\(`([a-z\-]+)`", ln)
    if m: face = m.group(1)
    m2 = re.match(r"\|\s*\*{0,2}D(\d+)\*{0,2}\s*\|\s*(K\d)\s*\|\s*(.*?)\s*\|\s*`([A-Z0-9_]+)`\s*\|\s*(.*?)\s*\|", ln)
    if m2:
        rows.append(dict(line=i + 1, face=face, id="D" + m2.group(1), kind=m2.group(2),
                         rec=m2.group(3), verdict=m2.group(4), basis=m2.group(5)))
assert len(rows) == 168, len(rows)

WIRE = ['wire','包形','opcode','read_','write_','decode','encode','发包','封包','字节','字段序','c2s','handler','协议','changemap','枚举','0x','包体','charlist','update_skill','maker','funckey','skip(','8 b','4 b','2 b','联机','服务端','收包','解包','收窄','writeint','writeshort','writelong','read_int','read_short','read_long','packet','包']
PURE = ['ui','窗','按钮','排版','布局','坐标','位图','动画','帧','sad','截图','版式','图标','node','img','浮窗','页签','dimension','origin','拖拽','观感','样式','文字','配色','闪烁','tooltip','补间','类树','页','背板','槽','条','皮肤','名牌']
def classify(r):
    s = (r["rec"] + " " + r["basis"]).lower()
    hw, hp = any(k in s for k in WIRE), any(k in s for k in PURE)
    return "MIXED" if (hw and hp) else ("WIRE" if hw else ("PURE" if hp else "OTHER"))
for r in rows: r["class"] = classify(r)
cls = Counter(r["class"] for r in rows)
assert cls == Counter({"PURE":76,"WIRE":38,"MIXED":14,"OTHER":40}), cls

KEEP = {"BATCH_0_BASELINE","BATCH_1_KEEP","KEEP_S9","KEEP_CURRENT","KEEP_WITH_NOTE","NO_DEFECT","NO_OP","DOC_ONLY","P3_NO"}
kept = [r for r in rows if r["verdict"] in KEEP]
kcls = Counter(r["class"] for r in kept)
assert len(kept) == 79 and kcls == Counter({"WIRE":27,"OTHER":25,"PURE":20,"MIXED":7}), (len(kept), kcls)

reopen = [dict(face=r["face"], id=r["id"], kind=r["kind"], verdict=r["verdict"], cls=r["class"], rec=r["rec"],
               reason="不碰 wire；round86 用『s9 联机优先』一刀切 KEEP 的理由在此条不成立，应按 Angel 083 语义重新逐条拍板（架构级重写须单独裁决）")
          for r in kept if r["class"] in ("PURE","MIXED")]
must_keep = [dict(face=r["face"], id=r["id"], kind=r["kind"], verdict=r["verdict"], rec=r["rec"],
                  reason="碰协议 wire / opcode；改回 083 原生包形会断 s9 联机，保持 s9 是硬约束")
             for r in kept if r["class"] == "WIRE"]
assert len(reopen) == 27 and len(must_keep) == 27

# ---- 6. 已发货修复链（快照自举，保证跨 commit 字节幂等）----
# git log --oneline 含 commit hash；若每次从现场重算，本轮 commit/amend 会让 evidence/VERDICT
# 漂移（B9b 幂等红）。历史链是时间快照 → evidence 已存在时复用，git log 只由 harness B8 现场对拍。
import subprocess
if os.path.isfile(EV):
    _prev = json.load(open(EV, encoding="utf-8"))
    shipped_align = list(_prev["shipped_align_commits"])
    shipped_devfix_total = int(_prev["shipped_devfix_total"])
    shipped_align_total = len(shipped_align)
    shipped = [None] * shipped_devfix_total
else:
    log = subprocess.run(["git","log","--oneline"], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL).stdout.decode("utf-8","replace")
    shipped = [ln for ln in log.split("\n") if "#devfix" in ln]
    shipped_align = [ln for ln in shipped if any(k in ln for k in ("083","Angel","校准","翻案","收口","重修","复核"))]
    shipped_devfix_total = len(shipped)
    shipped_align_total = len(shipped_align)
assert shipped_devfix_total >= 30 and shipped_align_total >= 15, (shipped_devfix_total, shipped_align_total)

# ---- 7. 主裁决 ----
verdict = dict(
    q1_judgement="不是。『BeiDou==Angel』指两个 IDA 实例同二进制，仅符号还原状态不同；79 条保持的是当前端 HeavenClient 自研逻辑，与这两个 exe 无继承关系。保持的真实理由写在 DECISIONS.md §0：s9 联机优先。",
    angel_vs_s9=dict(
        handshake_layer="一致（s9 hello u16 0x0E / version 83 / u8 49，IV 按 0xFFFF-83；PLAN.md F1）",
        opcode_layer="一致（GATE OK s9 a78ba681a4，client_out_sent=148 / s9_send=314）",
        body_layer="不一致 —— s9 是改造过的 v83：level 2B、HP/MP int、exp 8B、GachaExp 4B、spawn 尾 int(0)、removeItemFromMap 恒写 4B chrId",
        conditional_full_align="前提『Angel 与服务端一致』在 body/wire 层不成立 → 不触发『全部按 Angel 对齐』；照 083 原生包形改回会断联机（LoginParser 白屏即实证）。",
    ),
    beidou_many_bugs=f"成立。LoginParser.cpp:150-174 白屏案例是实锤，且全仓已有 {len(shipped)} 个 #devfix 原子提交、其中 {len(shipped_align)} 个标题带 083/Angel/校准/翻案/收口/重修/复核语义；本会话一直在做『按 Angel 对齐纯客户端层』这件事。",
    round86_gap="round86 把 168 条批量裁决，其中 27 条『保持』属纯客户端/混合层、不碰 wire，用 s9 联机优先一刀切 KEEP 的理由不成立 → 本轮列为重拍板清单（reopen_candidates），不自动改代码（架构级重写须逐条裁决 + 门禁 5 禁想象修改）。",
    align_policy="wire/opcode 层 s9 优先不可动；纯客户端 UI/排版/动画/交互层按 Angel 083 语义继续对齐（本会话 30+ commit 已证）；字段级 body 逐片以 s9 源码为准（PLAN.md §P2 一片一原子 commit）。",
)

data = dict(
    round="88", date="2026-11-11",
    trigger=dict(m09543="记录一次有可见 PUA 输出的交付，不记录评分。",
                 m09544="没有按Angel修改且保持原先逻辑的，是因为BeiDou和Angel一样的原因？如果不是的话，需要确认Angel是否和服务端一致，如果一致就全部按Angel 反编译逻辑对齐；原先BeiDou有很多功能都是错误的；"),
    verdict=verdict,
    s9_wire_divergences=s9_div, s9_packetcreator_single_copy=True,
    handshake_and_gate=dict(handshake=gate["meaning"], gate_head=gate["s9_git_head"], counts=gate["counts"]),
    q1_same_binary=q1,
    client_whitescreen_case=whitescreen,
    classification_168=dict(**{k: cls[k] for k in ("PURE","WIRE","MIXED","OTHER")}, total=168,
                            arithmetic="76+38+14+40=168"),
    kept_79=dict(WIRE=27, OTHER=25, PURE=20, MIXED=7, total=79,
                 arithmetic="27+25+20+7=79；KEEP 集合=BATCH_0_BASELINE/BATCH_1_KEEP/KEEP_S9/KEEP_CURRENT/KEEP_WITH_NOTE/NO_DEFECT/NO_OP/DOC_ONLY/P3_NO"),
    reopen_candidates=reopen,
    must_keep_s9=must_keep,
    shipped_align_commits=shipped_align,
    shipped_devfix_total=shipped_devfix_total,
    shipped_align_total=shipped_align_total,
    zero_source_change=True,
    zero_change_note="本轮只读取证 + 判定，未改任何 .cpp/.h/.c/.cc/协议/WZ；工作树 11 个外部程序在途 WIP 原样保留、本轮一个字节未碰。",
    live_backlog=[
        "真机五屏截图 60 节/142 处（用户认领）",
        "reopen_candidates 27 条需逐条拍板（架构级重写单点裁决，非本轮自动改）",
        "InPacket::available() 393 处 §27-A 真机逐面回归",
        "PLAN.md §P2 字段级 body 11 片逐片探针 + 一片一原子 commit",
    ],
    constraints=["083/v83 only 禁095","s9 协议优先","数值重合≠语义重合","校准≠改回原生",
                 "orange-wz MCP 用 128B DEFAULT_KEY；.nx 才用 65536B key_gms",
                 "门禁 5 禁想象修改 / 18 REA→IDA→orange-wz→.nx 只读取证"],
)

os.makedirs(os.path.dirname(EV), exist_ok=True)
with open(EV, "w", encoding="utf-8", newline="") as f:
    json.dump(data, f, ensure_ascii=False, indent=1)

# ---- 8. CONSISTENCY-VERDICT.md（主答复，用户最想要的结论）----
L = []
A = L.append
A("# Angel / 当前端 / s9 服务端 三方一致性确认书（round88）")
A("")
A("> 触发（m09544）：「没有按Angel修改且保持原先逻辑的，是因为BeiDou和Angel一样的原因？如果不是的话，需要确认Angel是否和服务端一致，如果一致就全部按Angel 反编译逻辑对齐；原先BeiDou有很多功能都是错误的；」")
A("")
A("## 0. 一句话结论")
A("")
A("**不是**因为 BeiDou==Angel 才保持；Angel 与服务端**握手/opcode 层一致、字段级 body 层不一致**（s9 是改造过的 v83），所以「全部按 Angel 对齐」的前提不成立、照 083 原生包形改回会断联机。但「原先功能很多是错的」**成立且一直在修**——纯客户端层按 Angel 083 语义对齐已发货 30+ 个原子提交。")
A("")
A("## 1. 判断题：79 条「保持原逻辑」的真实理由")
A("")
A("| 问 | 答 |")
A("| --- | --- |")
A("| BeiDou 和 Angel 一样吗？ | 一样，但只是**同一份 v83/083 GMS 二进制的两个 IDA 实例**（Angel 13339 带 3122 修饰名 / BeiDou 13337 只有 50；EA 95/97 类、809 命名函数逐字节一致）。 |")
A("| 保持的是谁的逻辑？ | 当前端 **HeavenClient（从零自研重实现，README.md 自证）**，不是 BeiDou.exe 的代码。二者无继承关系。 |")
A("| 所以「因为一样所以保持」成立吗？ | **不成立**。这是把「符号还原状态」和「代码来源」混为一谈。 |")
A("| 真实理由 | `DECISIONS.md §0`：**s9 联机优先，Angel 083 仅作语义校准源；校准≠改回原生；数值重合≠语义重合**。 |")
A("")
A("## 2. Angel 与 s9 服务端是否一致（分层判定，答案不同）")
A("")
A("| 层 | 判定 | 证据 |")
A("| --- | --- | --- |")
A("| 握手 / 版本 / 加解密 | ✅ 一致 | `PacketCreator.java:630-635 getHello` 声明 `u16 0x0E / version 83 / u8 49`；IV 按 `0xFFFF-83`；`PLAN.md F1` 握手已对齐；`GMSV83PacketProtocol` ↔ `Cryptography.cpp` T4 逐字节对拍通过。 |")
A("| opcode 名/值 + 注册 | ✅ 一致 | `GATE OK (s9 a78ba681a4)`：`client_out_sent=148`、`s9_recv_constants=182`、`s9_send_constants=314`、`s9_send_emitted=253` 三方对账。 |")
A("| **字段级 body** | ❌ **不一致** | **s9 是改造过的 v83**，见下表。 |")
A("")
A("### 2.1 s9 相对 083 原生的 wire 改造（源码逐字，`PacketCreator.java`）")
A("")
A("| # | 行 | s9 源码 | 083 原生 | 改造原因（源码注释） |")
A("| --- | --- | --- | --- | --- |")
A("| 1 | 192 | `p.writeShort(chr.getLevel());` | level 1B | 等级上限 999，writeByte >255 溢出 |")
A("| 2 | 198-205 | `p.writeInt(chr.getHp());` ×4 | short | 血量上限 99999 超 short(65535) |")
A("| 3 | **211** | **`p.writeLong(chr.getExp());`** | **exp 4B** | 999 级累积经验远超 int32，必须 8 字节 |")
A("| 4 | 213 | `p.writeInt((int) chr.getGachaExp());` | 无此字段 | s9 GachaExp 扩展通道 |")
A("| 5 | 217 | `p.writeInt(0);`（spawnpoint 后） | 无尾部 | s9 固定补 int(0) |")
A("| 6 | 2624 | `removeItemFromMap` animation≥2 恒写 4B chrId | `CDropPool::OnPacket v2==4` 读 Decode2 delay | 数值重合≠语义重合 |")
A("")
A("全仓只有**一份** `PacketCreator.java`（find 单命中）——不存在「另一份和 083 一样的服务端」可切。")
A("")
A("### 2.2 条件指令判定")
A("")
A("> 用户条件：「如果一致就全部按 Angel 反编译逻辑对齐」。")
A("")
A("**body/wire 层前提不成立 → 不触发全部对齐。** 把 exp 8B→4B、level 2B→1B、删 GachaExp/tail 改回 083 原生，会把 s9 包的中间字段整体平移、直接复现白屏。")
A("")
A("## 3. 「原先功能很多是错的」——成立，实锤 + 已修链")
A("")
A("`Net/Handlers/Helpers/LoginParser.cpp:150-174` 自述白屏案例：旧代码按 8B exp 解析 s9 包 → `skip(4)` 吞掉真实 `mapId=100000000` → 从 spawnpoint 槽读出 `16` 当 mapid → `Stage::load` 抛 `map data missing` → **进图白屏**。现代码 `recv.read_long()` + 末尾 `recv.skip(4)` 已与 s9 `:214-217` 逐字段对齐。")
A("")
A("按 Angel 083 翻案重修已发货（节选，全 30+ 见 evidence `shipped_align_commits`）：")
A("")
for s in shipped_align[:14]:
    A(f"- `{s}`")
A("")
A("这些都曾是「参照 BeiDou 或自行修复」的错版，按 Angel 083 真资源/真立即数重修。")
A("")
A("## 4. round86 一刀切的口径缺口（本轮纠偏，不自动改代码）")
A("")
A("168 条按「碰不碰 wire」重分类：`PURE 76 / WIRE 38 / MIXED 14 / OTHER 40 = 168`。")
A("")
A("79 条「保持」拆开：`WIRE 27 / OTHER 25 / PURE 20 / MIXED 7 = 79`。")
A("")
A("| 类 | 条数 | 处置 |")
A("| --- | --- | --- |")
A("| WIRE 27 | 27 | **保持 s9 是硬约束**（改回 083 断联机），理由成立。清单见 evidence `must_keep_s9`。 |")
A("| PURE 20 + MIXED 7 | **27** | **理由不成立**：不碰 wire，却被 round86 用「s9 联机优先」一刀切 KEEP。列 `reopen_candidates`，按 Angel 083 语义**逐条重新拍板**；架构级重写（单体窗合并 / 7 子窗拆分 / 双布局收敛）须单点裁决，门禁 5 禁想象修改，本轮零源码改动。 |")
A("| OTHER 25 | 25 | 文档/措辞/低优先类，维持。 |")
A("")
A("### 4.1 reopen 清单（27 条，face/id）")
A("")
for r in reopen:
    A(f"- `{r['face']}/{r['id']}` {r['kind']} [{r['verdict']}][{r['cls']}] {r['rec'][:70].rstrip()}")
A("")
A("## 5. 对齐策略（今后按此执行）")
A("")
A("1. **wire / opcode 层**：s9 源码优先，不可动（27 条 must_keep）。")
A("2. **字段级 body**：`PLAN.md §P2` 11 片逐片探针 + 一片一原子 commit，以 s9 ByteBuf 序列为准。")
A("3. **纯客户端 UI/排版/动画/交互**：按 Angel 083 语义继续对齐（本会话一直在做；27 条 reopen 逐条拍板后 P1 实施）。")
A("")
A("## 6. 限制 / 真机归口")
A("")
A("- 本轮 0 源码 / 0 协议 / 0 WZ 改动，只读取证 + 判定；工作树 11 个外部程序在途 WIP 原样保留。")
A("- 27 条 reopen 是「需重新拍板」不是「已改」；架构级重写不在本轮自动落地。")
for b in data["live_backlog"]:
    A(f"- {b}")
A("")
with open(VERDICT, "w", encoding="utf-8", newline="") as f:
    f.write("\n".join(L).rstrip("\n") + "\n")

# ---- 9. notes ----
N = ["# round88 三方一致性确认（wave3）", "",
     "触发：m09543（可见 PUA 输出、不记评分）+ m09544（BeiDou==Angel 质疑 / Angel 与服务端一致性 / 若一致全部按 Angel 对齐 / 原 BeiDou 多错）。", "",
     "## 结论", "",
     "- 保持原逻辑**不是**因为 BeiDou==Angel：Q1 同二进制=两个 IDA 实例，当前端 HeavenClient 是自研重实现；真实理由 DECISIONS.md §0 = s9 联机优先。",
     "- Angel vs s9：握手/opcode 一致（hello version 83、GATE OK a78ba681a4），字段级 body 不一致（s9 改造 v83：exp 8B / level 2B / HP-MP int / GachaExp 4B / spawn tail）。",
     "- 条件「若一致则全部按 Angel 对齐」在 body 层前提不成立 → 不触发；改回 083 原生包形断联机（LoginParser 白屏实证）。",
     "- 「原 BeiDou 多错」成立：LoginParser 白屏实锤 + 30+ 按 Angel 083 翻案重修 commit。", "",
     "## round86 纠偏", "",
     "168 三分类 PURE76/WIRE38/MIXED14/OTHER40；79 KEEP = WIRE27/OTHER25/PURE20/MIXED7。",
     "WIRE27 保持 s9 成立；PURE20+MIXED7=27 条不碰 wire 被一刀切 KEEP 的理由不成立 → 列 reopen 逐条拍板，本轮零源码改动。", "",
     "## 对齐策略", "",
     "wire s9 优先 / 字段 body 逐片以 s9 为准（PLAN.md §P2）/ 纯客户端层按 Angel 083 语义继续对齐。", "",
     "## 限制", "",
     "0 源码 / 0 协议 / 0 WZ 改动；11 个外部在途 WIP 原样留工作树；27 条 reopen 待逐条裁决，架构级重写不在本轮自动落地。", "",
     "证据：`../evidence/three-way-consistency-round88.json`；主答复：`../../angel-calibration-d-matrix-2026-11/CONSISTENCY-VERDICT.md`。", ""]
with open(NOTES, "w", encoding="utf-8", newline="") as f:
    f.write("\n".join(N))

# ---- 10. dev-plan §4 2-36 行（LF，幂等）----
dev = rs(DEV)
ROW36 = "| **2-36** | round88 三方一致性确认（m09544）：Angel/BeiDou 同二进制≠保持理由；Angel 与 s9 握手·opcode 一致、body 层不一致（s9 改造 v83）；27 条不碰 wire 的 KEEP 列 reopen 逐条拍板。零源码改动，只读取证。证据 `angel-calibration-d-matrix-2026-11/CONSISTENCY-VERDICT.md` + `evidence/three-way-consistency-round88.json`。 |"
if "**2-36**" not in dev:
    anchor = "| **2-35** |"
    i = dev.find(anchor)
    assert i >= 0
    j = dev.find("\n", i)
    dev = dev[:j + 1] + ROW36 + "\n" + dev[j + 1:]
    with open(DEV, "w", encoding="utf-8", newline="") as f:
        f.write(dev)

# ---- 11. PLT §47（CRLF，幂等，勿留尾空行）----
plt_b = rb(PLT)
plt = plt_b.decode("utf-8")
S47 = [
 "## 47. round88 · 三方一致性确认（只读取证 / 零源码改动）",
 "",
 "47-1 **Q1 复核**：Angel(13339)/BeiDou(13337) 同一份 v83 二进制两 IDA 实例；当前端 HeavenClient 自研重实现，79 条保持与「两者一样」无因果，真实理由 s9 联机优先。",
 "47-2 **握手/opcode 一致**：s9 hello version 83、GATE OK `a78ba681a4`（client_out_sent 148 / s9_send 314）。",
 "47-3 **body 层不一致**：s9 改造 v83（exp 8B / level 2B / HP-MP int / GachaExp 4B / spawn tail int0 / removeItemFromMap 恒写 4B cid）→「全部按 Angel 对齐」前提不成立，改回断联机。",
 "47-4 **原逻辑错误已证**：`LoginParser.cpp:150-174` 白屏案例 + 30+ 按 Angel 083 翻案重修 commit。",
 "47-5 **reopen 27 条**（PURE20+MIXED7，不碰 wire 却被一刀切 KEEP）列 `three-way-consistency-round88.json::reopen_candidates`，须逐条拍板；架构级重写须单点裁决，本轮不自动改。",
 "",
 "限制：① 0 源码/0 协议/0 WZ 改动，只读取证；② 11 个外部在途 WIP 原样留工作树；③ 真机五屏截图 60 节/142 处仍由用户认领；④ reopen 27 条为「待裁决」非「已改」。",
]
if "## 47." not in plt:
    base = plt.rstrip("\r\n")
    plt = base + "\r\n\r\n" + "\r\n".join(S47) + "\r\n"
    with open(PLT, "wb") as f:
        f.write(plt.encode("utf-8"))

# ---- 12. 幂等 SHA ----
def sha(p): return hashlib.sha256(rb(p)).hexdigest()[:10]
print("ROUND88_DELIVER_ALL_OK")
for p in (EV, VERDICT, NOTES, DEV, PLT):
    print(" ", sha(p), p)
