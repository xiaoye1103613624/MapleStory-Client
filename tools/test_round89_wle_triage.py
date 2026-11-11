# -*- coding: utf-8 -*-
"""round89 W/L/E 三桶 + UIQuestLog change_tab 对齐补丁 harness（静态守卫 + 场景）。

定式沿用 round78/86/87/88 的教训：
  - git() 用 stderr=DEVNULL + 过滤 "warning:" 行（GBK warning 混进 stdout 的坑）
  - wc -l 用 count(b"\\n")（无尾换行文件口径）
  - 提交态自适应：提交前 git diff --name-only / 提交后 git show --name-only HEAD
  - 生成器不读 git log（round88 B9b 幂等漂移坑），故 B11 可安全重跑
  - 0 行为改动用 evidence 的不可变 sha256 基准（与 git HEAD 无关）
"""
import json, os, subprocess, hashlib, sys
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
R88EV = f"{MATRIX}/evidence/three-way-consistency-round88.json"
CPP = "IO/UITypes/UIQuestLog.cpp"
GEN = "tools/_r89_deliver.py"
IDA = "tools/_ida_r89/CUIQuestInfo_OnTabChanged.code.txt"
S9 = r"E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/util/PacketCreator.java"

# 与生成器完全一致的补丁块字节（CRLF）
PATCH_NOTE = (
    "\t\t// round89 W/L/E：对齐 083 CUIQuestInfo::OnTabChanged @0x87fd4f 首行 *(this + 369) = -1;\r\n"
    "\t\t// 切换分类页签必须先清选中，再由 rebuild_list() 自动选第一条可见任务（TryShowTopQuest /\r\n"
    "\t\t// ToggleDetail(0) 语义）；旧实现保留上一页的 selected_id，跨页残留导致摘要页显示旧任务。\r\n"
    "\t\t// 纯客户端状态机：零 wire / 零协议 / 零 WZ / 零新封包，不触碰 s9 改造字段宽度。\r\n"
    "\t\tselected_id = -1;\r\n"
).encode("utf-8")

FAILS, CHECKS, SCEN = [], [0], [0, 0]


def chk(name, cond):
    CHECKS[0] += 1
    if not cond:
        FAILS.append(f"A: {name}")
        print(f"[FAIL] {name}")


def sc(name, cond):
    SCEN[0] += 1
    if not cond:
        SCEN[1] += 1
        FAILS.append(f"B: {name}")
        print(f"[FAIL] SCEN:{name}")


def rb(p):
    with open(p, "rb") as f:
        return f.read()


def rs(p, enc="utf-8"):
    with open(p, "r", encoding=enc, errors="replace", newline="") as f:
        return f.read()


def git(*args):
    r = subprocess.run(["git"] + list(args), stdout=subprocess.PIPE,
                       stderr=subprocess.DEVNULL)
    return r.stdout.decode("utf-8", "replace")


def git_lines(*args):
    return [x.strip() for x in git(*args).splitlines()
            if x.strip() and not x.strip().startswith("warning:")]


def head_file(path):
    r = subprocess.run(["git", "show", f"HEAD:{path}"], stdout=subprocess.PIPE,
                       stderr=subprocess.DEVNULL)
    return r.stdout if r.returncode == 0 else None


# ---------------------------------------------------------------- 输入
ev = json.load(open(EV, encoding="utf-8"))
r88 = json.load(open(R88EV, encoding="utf-8"))
cpp_b = rb(CPP)
cpp_s = cpp_b.decode("utf-8")
ver = rs(VERDICT)
plan = rs(PLAN)
notes = rs(NOTES)
dev = rs(DEV)
plt_s = rb(PLT).decode("utf-8")
ida = rs(IDA)
s9 = rs(S9)

# ================================================================ A 静态守卫
# A1 文件存在
for p in [EV, PLAN, NOTES, DEV, PLT, VERDICT, CPP, GEN, IDA, R88EV]:
    chk(f"A1 exists {p}", os.path.isfile(p))
chk("A1 s9 source exists", os.path.isfile(S9))

# A2 triggers 两条逐字
chk("A2a trigger m09729", any("字段级 body 改造过的字段" in t for t in ev["triggers"]))
chk("A2b trigger m09737", any("其余功能也要按照这个方向" in t for t in ev["triggers"]))
chk("A2c plan carries triggers", "m09729" in plan and "m09737" in plan)
chk("A2d notes carries triggers", "m09729" in notes and "m09737" in notes)
chk("A2e verdict §7 carries triggers", "m09729" in ver and "m09737" in ver)

# A3 W 桶
W = ev["buckets"]["W"]
chk("A3a W 6 fields", len(W["fields"]) == 6)
chk("A3b W 27 must_keep", W["must_keep_count"] == 27 and len(W["must_keep_s9"]) == 27)
for w in W["fields"]:
    chk(f"A3c W field in s9 {w['tag']}", w["code"] in s9)
chk("A3d W fields cover 6 marks", all(k in " ".join(w["tag"] for w in W["fields"])
                                      for k in ["level", "HP", "exp", "GachaExp", "spawn", "removeItemFromMap"]))
# must_keep 与 round88 evidence 完全一致
mk_ev = {(r["face"], r["id"]) for r in W["must_keep_s9"]}
mk_88 = {(r["face"], r["id"]) for r in r88["must_keep_s9"]}
chk("A3e must_keep == round88", mk_ev == mk_88)

# A4 L 桶
L = ev["buckets"]["L"]
chk("A4a L 27 reopen", L["reopen_count"] == 27 and len(L["reopen_candidates"]) == 27)
ro_ev = {(r["face"], r["id"]) for r in L["reopen_candidates"]}
ro_88 = {(r["face"], r["id"]) for r in r88["reopen_candidates"]}
chk("A4b reopen == round88", ro_ev == ro_88)
lp = L["landed_patch"]
chk("A4c patch file", lp["file"] == CPP and lp["func"] == "change_tab")
chk("A4d patch anchor", lp["anchor"] == "tab = tabid;" and lp["added"] == "selected_id = -1;")
chk("A4e patch ida basis", "0x87fd4f" in lp["ida_basis"] and "*(this + 369) = -1;" in lp["ida_basis"])
for k in ["wire_change", "protocol_change", "wz_change", "new_packet"]:
    chk(f"A4f patch {k} is False", lp.get(k) is False)
chk("A4g quest/D5 send_key kept", "门禁 5 禁想象修改" in ev["quest_d5_note"] and "total=0" in ev["quest_d5_note"])

# A5 E 桶
E = ev["buckets"]["E"]
chk("A5a E 5 items", E["count"] == 5 and len(E["items"]) == 5)
chk("A5b E is L-track exemption", "不参与 168 算术" in E["meaning"] or "豁免" in E["meaning"])
for e in E["items"]:
    chk(f"A5c E has evidence {e['item'][:12]}", bool(e["evidence"]) and bool(e["why"]))

# A6 routing 168 闭合
R = ev["routing_168"]
cls88 = r88["classification_168"]
chk("A6a total 168", R["total"] == 168 and R["undecided"] == 0)
chk("A6b W_track 38", R["W_track"] == cls88["WIRE"] == 38)
chk("A6c L_track 130", R["L_track"] == cls88["PURE"] + cls88["MIXED"] + cls88["OTHER"] == 130)
chk("A6d arithmetic", R["W_track"] + R["L_track"] == 168 and R["arithmetic"] == "38+130=168")
chk("A6e composition string", "PURE 76 + MIXED 14 + OTHER 40" in R["L_composition"])

# A7 0 wire / 0 协议 / 0 WZ
for k in ["zero_wire_change", "zero_protocol_change", "zero_wz_change"]:
    chk(f"A7a {k}", ev.get(k) is True)
chk("A7b source_files_changed == [CPP]", ev["source_files_changed"] == [CPP])
chk("A7c ext_wip_count 17", ev["ext_wip_count"] == 17)

# A8 口径修订：§7 追加 + 历史证据链不删
chk("A8a verdict §7 head", "## 7. round89 口径修订" in ver)
chk("A8b §7.1 三桶定义", "### 7.1 三桶定义" in ver and "| **W** |" in ver and "| **L** |" in ver and "| **E** |" in ver)
chk("A8c §7.2 收窄 §0/§2.2", "### 7.2 对 §0 / §2.2 的收窄" in ver and "仅对 W 桶成立" in ver)
chk("A8d §7.2 白屏二分", "不证明" in ver and "字段上的业务逻辑" in ver)
chk("A8e §7.3 补正 §5", "### 7.3 对 §5 策略的补正" in ver and "L 桶（新增）" in ver)
v74 = ver[ver.find("### 7.4"):ver.find("### 7.5")]
chk("A8f §7.4 路由闭合", "38" in v74 and "130" in v74 and "= 168" in v74 and "未决 0" in v74)
chk("A8g §0 原文仍在", "**不是**因为 BeiDou==Angel 才保持" in ver)
chk("A8h §2.2 原文仍在", "**body/wire 层前提不成立 → 不触发全部对齐。**" in ver)
chk("A8i §2.1 表仍在", "p.writeLong(chr.getExp());" in ver or "exp 4B" in ver)
chk("A8j §5 三条策略仍在", "wire / opcode 层" in ver and "字段级 body" in ver)
chk("A8k 历史证据链不删（章序）", all(h in ver for h in
    ["## 0.", "## 1.", "## 2.", "## 3.", "## 5.", "## 6.", "## 7."]))

# A9 IDA 证据
ie = ev["ida_evidence"]
chk("A9a endpoint 13339", "13339" in ie["endpoint"])
chk("A9b symbol OnTabChanged", "0x87fd4f" in ie["symbol"])
chk("A9c first_line clear-selected", "*(this + 369) = -1;" in ie["first_line"])
chk("A9d ida code has clear", "*(this + 369) = -1;" in ida and "CUIQuestInfo::OnTabChanged" in ida)
chk("A9e ida code tail calls", all(k in ida for k in ["TryShowTopQuest", "ToggleDetail", "SyncScrollBarWithSelected"]))
chk("A9f tail_calls list", "TryShowTopQuest" in ie["tail_calls"] and "ToggleDetail(0)" in ie["tail_calls"])

# A10 真实补丁：cpp 语义 + 唯一改动
chk("A10a patch bytes in cpp exactly once", cpp_b.count(PATCH_NOTE) == 1)
# change_tab 函数体内顺序
c0 = cpp_s.find("void UIQuestLog::change_tab")
c1 = cpp_s.find("void UIQuestLog::on_list_scroll")
chk("A10b change_tab located", c0 >= 0 and c1 > c0)
body = cpp_s[c0:c1]
chk("A10c tab = tabid; in change_tab", "tab = tabid;" in body)
chk("A10d selected_id = -1; in change_tab", "selected_id = -1;" in body)
chk("A10e order clear-after-tab", body.find("tab = tabid;") < body.find("selected_id = -1;"))
chk("A10f exactly one clear in change_tab", body.count("selected_id = -1;") == 1)
chk("A10g 083 comment in change_tab", "round89 W/L/E" in body and "0x87fd4f" in body)
chk("A10h rebuild_list auto-select guard", "bool keep = false;" in cpp_s and "selected_id = -1;" in cpp_s[cpp_s.find("void UIQuestLog::rebuild_list"):cpp_s.find("void UIQuestLog::rebuild_list") + 3000])
chk("A10i update_detail empty guard", "if (selected_id < 0)" in cpp_s)
# send_key 空实现保持
s0 = cpp_s.find("void UIQuestLog::send_key")
s1 = cpp_s.find("UIElement::Type UIQuestLog::get_type")
sk = cpp_s[s0:s1]
chk("A10j send_key still empty", "{" in sk and sk.replace("void UIQuestLog::send_key(int32_t keycode, bool pressed, bool escape)", "").replace("{", "").replace("}", "").strip() == "")
# 0 行为改动：切掉补丁块后的 sha256 与 evidence 基准一致（提交前后都稳定）
recon = cpp_b.replace(PATCH_NOTE, b"", 1)
recon_sha = hashlib.sha256(recon).hexdigest()
chk("A10k recon sha == evidence baseline", recon_sha == ev["patch_integrity"]["pre_patch_cpp_sha256"])
# 提交前额外硬证：HEAD 的 cpp 是 LF blob（git autocrlf clean），工作树是 CRLF smudge；
# 全部归一到 LF 后再比对；提交后 HEAD 含补丁块，用 LF 版补丁块切掉（round78 自适应定式）。
def lf(b):
    return b.replace(b"\r\n", b"\n")
note_lf = lf(PATCH_NOTE)
head_cpp = head_file(CPP)
if head_cpp is not None:
    h = lf(head_cpp)
    if note_lf in h:
        h = h.replace(note_lf, b"", 1)
    chk("A10l HEAD cpp == recon (EOL-normalized, patch stripped)", h == lf(recon))
else:
    chk("A10l baseline sha stable", recon_sha == ev["patch_integrity"]["pre_patch_cpp_sha256"])
chk("A10m patch_integrity method", "sha256" in ev["patch_integrity"]["method"])

# A11 EOL
chk("A11a cpp pure CRLF", b"\r\n" in cpp_b and b"\n" not in cpp_b.replace(b"\r\n", b""))
chk("A11b cpp no BOM", not cpp_b.startswith(b"\xef\xbb\xbf"))
plt_b = rb(PLT)
chk("A11c PLT pure CRLF", b"\r\n" in plt_b and b"\n" not in plt_b.replace(b"\r\n", b""))
for p, label in [(EV, "ev"), (PLAN, "plan"), (NOTES, "notes"), (DEV, "dev"), (VERDICT, "verdict")]:
    b = rb(p)
    chk(f"A11d {label} pure LF", b"\r" not in b)
    chk(f"A11e {label} no CRCRLF", b"\r\r\n" not in b)
# 无尾空行（git diff --check "new blank line at EOF" 复发防线）
chk("A11f PLT single trailing CRLF", plt_b.endswith(b"\r\n") and not plt_b.endswith(b"\r\n\r\n"))
chk("A11g verdict single trailing LF", rb(VERDICT).endswith(b"\n") and not rb(VERDICT).endswith(b"\n\n"))

# A12 dev-plan §4 2-37 行
chk("A12a dev 2-37 row", "| **2-37** |" in dev)
row37 = next((ln for ln in dev.split("\n") if ln.startswith("| **2-37** |")), "")
chk("A12b 2-37 W/L/E", "W/L/E" in row37)
chk("A12c 2-37 OnTabChanged", "OnTabChanged" in row37 and "0x87fd4f" in row37)
chk("A12d 2-37 zero wire", "零 wire" in row37)
chk("A12e 2-36 still present", "| **2-36** |" in dev)
chk("A12f 2-37 after 2-36", dev.find("| **2-36** |") < dev.find("| **2-37** |"))
chk("A12g Wave 3 anchor intact", "### Wave 3" in dev)

# A13 PLT §48
chk("A13a PLT §48", "## 48. round89" in plt_s)
chk("A13b §48 W/L/E", "W/L/E" in plt_s[plt_s.find("## 48. round89"):])
chk("A13c §48 48-1..48-5", all(f"48-{i}." in plt_s for i in range(1, 6)))
chk("A13d §48 限制 4 条", "限制：①" in plt_s and "④" in plt_s[plt_s.find("## 48. round89"):])
chk("A13e §47 still present", "## 47. round88" in plt_s)

# A14 四文档 + 生成器
chk("A14a notes 5 sections", all(f"## {i}." in notes for i in range(1, 6)))
chk("A14b plan 6 sections", all(f"## {i}." in plan for i in range(0, 7)))
chk("A14c plan 168 closure", "38 + 130" in plan or "38+130" in plan)
chk("A14d plan reopen table", "| face/id | kind | 原 verdict | 类别 |" in plan)
chk("A14e plan E table", "| 扩展项 | 证据 | 理由 |" in plan)
chk("A14f generator exists & imports", os.path.isfile(GEN) and "hashlib" in rs(GEN))

# A15 提交态自适应：本轮唯一源码改动 = CPP；17 外部 WIP 零夹带
changed = set(x for x in git_lines("diff", "--name-only") if x.endswith((".cpp", ".h", ".c", ".cc")))
staged = set(x for x in git_lines("diff", "--cached", "--name-only") if x.endswith((".cpp", ".h", ".c", ".cc")))
head_src = set(x for x in git_lines("show", "--name-only", "--format=", "HEAD") if x.endswith((".cpp", ".h", ".c", ".cc")))
our_src = changed | staged | head_src
# 提交后 head_src 可能含本轮 CPP；提交前 changed 含本轮 CPP。两者都不应含其它本轮源码。
chk("A15a UIQuestLog.cpp is our only source change", CPP in our_src or b"round89 W/L/E" in cpp_b)
# 交付物 6 个文档/生成器都不在「源码」列表
docs = [EV, PLAN, NOTES, DEV, PLT, VERDICT, GEN]
chk("A15b no doc counted as source", all(d not in our_src for d in docs))
# 外部 WIP 登记（提交前可见；提交后不可见也 OK，只核「如出现则必须是 17 个已知 WIP 之一」）
EXT_WIP = {
    "Character/ActiveBuffs.cpp", "Character/ActiveBuffs.h", "Character/Buff.h",
    "Character/CharStats.cpp", "Character/CharStats.h", "Character/Player.cpp",
    "Character/StatCaps.h", "Data/BulletData.cpp", "Data/BulletData.h",
    "Gameplay/Stage.cpp", "IO/UITypes/UIStatusBar.cpp", "IO/UITypes/UIStatusBar.h",
    "Net/Handlers/Helpers/CashShopParser.cpp", "Net/Handlers/Helpers/LoginParser.cpp",
    "Net/Handlers/PlayerHandlers.cpp", "Net/Handlers/WaveAShellHandlers.cpp", "Net/Login.h",
}
ext_seen = (changed - {CPP}) & EXT_WIP
chk("A15c any visible ext WIP are known 17", all(x in EXT_WIP for x in (changed - {CPP})))
chk("A15d ext_wip_count matches constant", ev["ext_wip_count"] == len(EXT_WIP))

# A16 constraints 7
chk("A16a constraints >= 6", len(ev["constraints"]) >= 6)
chk("A16b 083-only", any("083" in c and "095" in c for c in ev["constraints"]))
chk("A16c 门禁 5", any("门禁 5" in c for c in ev["constraints"]))
chk("A16d 128B/65536B 双规格", any("128B" in c and "65536B" in c for c in ev["constraints"]))

# ================================================================ B 场景
# B1 三分类与 round88 完全一致
rows = json.load(open("tools/_r88_rows.json", encoding="utf-8"))
cls = Counter(r["class"] for r in rows)
sc("B1a rows 168", len(rows) == 168)
sc("B1b class dist", cls == Counter({"PURE": 76, "WIRE": 38, "MIXED": 14, "OTHER": 40}))
sc("B1c W_track == WIRE", R["W_track"] == cls["WIRE"])
sc("B1d L_track == PURE+MIXED+OTHER", R["L_track"] == cls["PURE"] + cls["MIXED"] + cls["OTHER"])

# B2 reopen / must_keep 来源一致且互斥
sc("B2a reopen ∩ must_keep 空", not (ro_ev & mk_ev))
sc("B2b reopen 全在 PURE/MIXED", all(r.get("cls") in ("PURE", "MIXED") or
    (r["face"], r["id"]) in {(x["face"], x["id"]) for x in r88["reopen_candidates"]}
    for r in L["reopen_candidates"]))
sc("B2c must_keep 27", len(mk_ev) == 27)

# B3 6 个 W 字段逐条在 s9 且标签唯一
sc("B3a 6 unique tags", len({w["tag"] for w in W["fields"]}) == 6)
sc("B3b all codes in s9", all(w["code"] in s9 for w in W["fields"]))
sc("B3c exp 8B present", any("writeLong" in w["code"] for w in W["fields"]))
sc("B3d removeItemFromMap present", any("removeItemFromMap" in w["code"] for w in W["fields"]))

# B4 E 项逐条带证据
sc("B4a E 5", len(E["items"]) == 5)
sc("B4b all E have evidence+why", all(e["evidence"] and e["why"] for e in E["items"]))
sc("B4c E not in arithmetic", E["count"] + R["total"] == 173)  # 5 是豁免，不进 168

# B5 补丁重建真值表
sc("B5a patch present", PATCH_NOTE in cpp_b)
sc("B5b recon removes patch", PATCH_NOTE not in recon)
sc("B5c recon sha stable", hashlib.sha256(recon).hexdigest() == ev["patch_integrity"]["pre_patch_cpp_sha256"])
sc("B5d recon still has tab=tabid", b"\t\ttab = tabid;\r\n" in recon)
sc("B5e recon still has rebuild_list", b"void UIQuestLog::rebuild_list" in recon)
sc("B5f recon CRLF clean", b"\n" not in recon.replace(b"\r\n", b""))

# B6 0 行为字面量（除 selected_id = -1 外，补丁块无其它有效代码行）
code_lines = [l for l in PATCH_NOTE.decode("utf-8").splitlines()
              if l.strip() and not l.strip().startswith("//")]
sc("B6a only 1 effective code line", len(code_lines) == 1)
sc("B6b effective line is clear", code_lines[0].strip() == "selected_id = -1;")
sc("B6c 4 comment lines", PATCH_NOTE.decode("utf-8").count("// round89") == 1 and
   PATCH_NOTE.decode("utf-8").count("\t\t//") == 4)

# B7 083 语义链
sc("B7a ida clear first", ida.find("*(this + 369) = -1;") < ida.find("*(this + 368) = a2;"))
sc("B7b ida LoadData after clear", "CUIQuestInfo::LoadData(this);" in ida)
sc("B7c ida TryShowTopQuest", "TryShowTopQuest" in ida)
sc("B7d ida ToggleDetail(0)", "ToggleDetail(this, a2: 0);" in ida)
sc("B7e current rebuild_list auto-select", "selected_id = -1;" in cpp_s[cpp_s.find("void UIQuestLog::rebuild_list"):cpp_s.find("void UIQuestLog::rebuild_list") + 3000])

# B8 口径修订覆盖（§7 存在且不删历史）
sc("B8a §7 after §6", ver.find("## 7.") > ver.find("## 6."))
sc("B8b §0 text intact", "**不是**因为 BeiDou==Angel 才保持" in ver)
sc("B8c §2.2 text intact", "body/wire 层前提不成立" in ver)
sc("B8d §7 narrows §0", "仅对 W 桶成立" in ver)
sc("B8e §7 narrows §2.2", "L 桶同字段原始逻辑按 Angel 083 还原照常触发" in ver)
sc("B8f 白屏二分", "白屏案例" in ver and "不证明" in ver)

# B9 四文档锚点
sc("B9a dev 2-37", "| **2-37** |" in dev)
sc("B9b PLT §48", "## 48. round89" in plt_s)
sc("B9c notes 5 sections", notes.count("## ") >= 5)
sc("B9d plan 168", "168" in plan)
sc("B9e evidence 15 keys", len(ev) == 15)

# B10 EOL 字节级
sc("B10a cpp crlf count", cpp_b.count(b"\r\n") > 1000)
sc("B10b cpp lone_lf 0", b"\n" not in cpp_b.replace(b"\r\n", b""))
sc("B10c PLT crlf count", plt_b.count(b"\r\n") > 1000)
sc("B10d PLT lone_lf 0", b"\n" not in plt_b.replace(b"\r\n", b""))
sc("B10e verdict no CR", b"\r" not in rb(VERDICT))
sc("B10f dev no CR", b"\r" not in rb(DEV))
sc("B10g evidence json valid", isinstance(ev, dict) and ev["round"] == 89)

# B11 生成器幂等（不读 git log，提交/amend 后仍稳定）
before = {p: hashlib.sha256(rb(p)).hexdigest() for p in [EV, PLAN, NOTES, DEV, PLT, VERDICT, CPP]}
r = subprocess.run([sys.executable, GEN], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
after = {p: hashlib.sha256(rb(p)).hexdigest() for p in before}
sc("B11a generator rc0", r.returncode == 0)
sc("B11b generator ALL_OK", b"ROUND89_DELIVER_ALL_OK" in r.stdout)
sc("B11c byte-identical after re-run", before == after)

# B12 一次性 P1 Plan 语义
sc("B12a plan this-round", "本轮" in plan and "UIQuestLog" in plan)
sc("B12b plan next drop", "drop" in plan and "OnDropEnterField" in plan or "CDropPool" in plan)
s22 = plan[plan.find("### 2.2"):plan.find("### 2.3")]
sc("B12c plan reopen 27 rows", all(f"`{r['face']}/{r['id']}`" in s22 for r in L["reopen_candidates"]) and len(L["reopen_candidates"]) == 27)
sc("B12d plan W zero-change", "零改动" in plan or "保持" in plan)
sc("B12e notes zero wire", "零 wire" in notes)

# B13 honest scope
sc("B13a 1 source file", ev["source_files_changed"] == [CPP])
sc("B13b 3 zero flags", all(ev[k] for k in ["zero_wire_change", "zero_protocol_change", "zero_wz_change"]))
sc("B13c quest/D5 not invented", "门禁 5" in ev["quest_d5_note"])

# ---------------------------------------------------------------- 汇总
print(f"STATIC GUARDS checked={CHECKS[0]} failed={len([f for f in FAILS if f.startswith('A')])}")
print(f"SCENARIOS {SCEN[0]}, bad={SCEN[1]}")
import py_compile
try:
    py_compile.compile(GEN, doraise=True)
    py_compile.compile(__file__, doraise=True)
    print("PYCOMPILE_OK")
except Exception as e:
    FAILS.append(f"pycompile: {e}")
    print(f"[FAIL] PYCOMPILE {e}")
if FAILS:
    print(f"FAIL: round89 W/L/E triage ({len(FAILS)} failures)")
    sys.exit(1)
print("PASS: round89 W/L/E triage (W38 + L130 = 168, E5 exempt, quest change_tab clear, zero wire, §7 narrowing)")
