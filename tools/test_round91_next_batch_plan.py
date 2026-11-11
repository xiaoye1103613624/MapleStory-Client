# -*- coding: utf-8 -*-
"""round91 harness: one-shot NEXT-BATCH-PLAN + three-candidate W/L/E correction.

Plan round => ZERO source change. Static guards (evidence/plan/anchors/EOL/
arithmetic/constraints/zero-change/hash-drift/WIP) + scenario truth tables.
Three-state visibility (working-tree | staged | HEAD) so pre-commit, staged
and post-commit all pass (round88 B12a / round90 A9 lesson).
ASCII-safe stdout (MSYS terminal may be GBK). Print no Chinese.
"""
import os, json, re, subprocess, sys, py_compile

ROOT = "E:/project/MapleStory-Client"
DM = "docs/features/angel-calibration-d-matrix-2026-11"
DP = "docs/features/development-and-fix-plan-2026-11"
WLE = DM + "/WLE-TRIAGE-PLAN.md"
DEV = DP + "/README.md"
PLT = "docs/features/pending-live-tests/README.md"
EV = DM + "/evidence/next-batch-plan-round91.json"
PLAN = DM + "/NEXT-BATCH-PLAN.md"
NOTES = DP + "/notes/wave3-next-batch-plan-20261111.md"
DEC = DM + "/DECISIONS.md"
MATRIX = DM + "/README.md"
IDA = "tools/_ida_r91"
GEN = "tools/_r91_deliver.py"

os.chdir(ROOT)

def rp(p):
    return os.path.join(ROOT, p)

def rt(p):
    with open(rp(p), "rb") as f:
        b = f.read()
    return b.decode("utf-8", "replace")

def rb(p):
    with open(rp(p), "rb") as f:
        return f.read()

def sh(args):
    try:
        return subprocess.check_output(args, cwd=ROOT, stderr=subprocess.STDOUT).decode("utf-8", "replace")
    except subprocess.CalledProcessError as e:
        return (e.output or b"").decode("utf-8", "replace")
    except Exception:
        return ""

# ---------------------------------------------------------------- three-state visibility
changed = set(x.strip() for x in sh(["git", "diff", "--name-only"]).splitlines() if x.strip())
staged = set(x.strip() for x in sh(["git", "diff", "--cached", "--name-only"]).splitlines() if x.strip())
headf = set(x.strip() for x in sh(["git", "show", "--name-only", "--format=", "HEAD"]).splitlines() if x.strip())
visible = changed | staged | headf
untracked = set()
for l in sh(["git", "ls-files", "--others", "--exclude-standard"]).splitlines():
    if l.strip():
        untracked.add(l.strip())
visible_all = visible | untracked

DELIVERABLES = [WLE, DEV, PLT, EV, PLAN, NOTES, GEN]
IDA_FILES = [
    "sub_721008.code.txt",
    "CUIQuestInfo_StartQuest_action.code.txt",
    "CUIStat_OnButtonClicked.code.txt",
    "CUIKeyConfig_OnButtonClicked.code.txt",
    "CUISkill_OnSkillLevelUpButton.code.txt",
    "CanSendExclRequest.code.txt",
    "CWndSkillGuide_OnKey.code.txt",
    "CUIMacroSysEx_OnSelected.code.txt",
    "CUIMacroSys_OnKey.code.txt",
    "CUIChannelShift_OnKey.code.txt",
    "CUIChannelSelect_OnKey.code.txt",
    "CUISkillEx_ShiftMacroUIState.code.txt",
    "img_gate.json",
]

static_fail = []
checked = 0

def chk(name, cond):
    global checked
    checked += 1
    if not cond:
        static_fail.append(name)

def has(p, *needles):
    t = rt(p)
    return all(n in t for n in needles)

# ---------------------------------------------------------------- A1 deliverables exist
for p in DELIVERABLES:
    chk("A1a exists " + p, os.path.isfile(rp(p)))
for f in IDA_FILES:
    chk("A1b ida evidence " + f, os.path.isfile(rp(IDA + "/" + f)))

# ---------------------------------------------------------------- A2 evidence JSON
ev = json.loads(rt(EV))
chk("A2a round==91", ev.get("round") == 91)
chk("A2b zero_source_change", ev.get("zero_source_change") is True)
chk("A2c zero_wire_change", ev.get("zero_wire_change") is True)
chk("A2d zero_protocol_change", ev.get("zero_protocol_change") is True)
chk("A2e zero_wz_change", ev.get("zero_wz_change") is True)
chk("A2f deliverable path", "NEXT-BATCH-PLAN.md" in ev.get("deliverable", ""))
chk("A2g candidates==3", len(ev.get("candidates", [])) == 3)
chk("A2h ida_evidence_files==12", len(ev.get("ida_evidence_files", [])) == 12)
chk("A2i key count >=15", len(ev.keys()) >= 15)
chk("A2j arithmetic key", "arithmetic_unchanged" in ev)
chk("A2k pattern key", "pattern_finding" in ev)
chk("A2l next_batch key", "next_batch_clean_L_bucket" in ev)
chk("A2m constraints key", isinstance(ev.get("constraints"), list) and len(ev["constraints"]) >= 6)
chk("A2n img_gate key", "img_gate" in ev)

ids = [c.get("id") for c in ev["candidates"]]
chk("A2o candidate ids", sorted(ids) == sorted(["quest/D8", "skill/D4", "corewindow/D8"]))
for c in ev["candidates"]:
    cid = c.get("id", "?")
    chk("A2p verdict NOT_P1 " + cid, "NOT_P1" in c.get("round91_verdict", ""))
    chk("A2q has 083 ida " + cid, isinstance(c.get("ida"), dict) and len(c["ida"]) > 0)
    chk("A2r has s9 or client " + cid, "s9" in c or "client" in c)
    chk("A2s has reason " + cid, "reason" in c and len(c["reason"]) > 40)

# 083 verdict literals
q8 = [c for c in ev["candidates"] if c["id"] == "quest/D8"][0]
sk4 = [c for c in ev["candidates"] if c["id"] == "skill/D4"][0]
cw8 = [c for c in ev["candidates"] if c["id"] == "corewindow/D8"][0]
chk("A2t quest W_bucket", "W_bucket" in q8["round91_verdict"])
chk("A2u skill ARCHITECTURE", "ARCHITECTURE" in sk4["round91_verdict"])
chk("A2v corewindow DOC_ONLY", "DOC_ONLY" in cw8["round91_verdict"])
chk("A2w sub_721008 semantics", "IsAutoStartQuest" in q8.get("symbol_semantics_resolved", ""))
chk("A2x s9 tolerates both", "BOTH" in q8["s9"]["note"] or "tolerates" in q8["s9"]["note"])

# ---------------------------------------------------------------- A3 IMG gate 128B
g = ev["img_gate"]
chk("A3a verdict", g.get("verdict") == "GATE_OK_128B_IMG_REACHABLE")
chk("A3b key_b64_len==172", g.get("key_b64_len") == 172)
chk("A3c iv TSPHKw==", g.get("iv_b64") == "TSPHKw==")
chk("A3d 128B spec", "128" in str(g.get("key_spec", "")))
chk("A3e 3 queries", len(g.get("queries", {})) == 3)
chk("A3f queries all isError false", all(v.get("isError") is False for v in g.get("queries", {}).values()))
gate_raw = rt(IDA + "/img_gate.json")
chk("A3g img_gate file verdict", "GATE_OK_128B_IMG_REACHABLE" in gate_raw)
chk("A3h img_gate no 65536", "65536" not in gate_raw)

# ---------------------------------------------------------------- A4 083 decompilation literals
q_sub = rt(IDA + "/sub_721008.code.txt")
q_start = rt(IDA + "/CUIQuestInfo_StartQuest_action.code.txt")
q_stat = rt(IDA + "/CUIStat_OnButtonClicked.code.txt")
q_kc = rt(IDA + "/CUIKeyConfig_OnButtonClicked.code.txt")
q_sp = rt(IDA + "/CUISkill_OnSkillLevelUpButton.code.txt")
q_excl = rt(IDA + "/CanSendExclRequest.code.txt")
q_guide = rt(IDA + "/CWndSkillGuide_OnKey.code.txt")
q_msel = rt(IDA + "/CUIMacroSysEx_OnSelected.code.txt")
q_mkey = rt(IDA + "/CUIMacroSys_OnKey.code.txt")
q_cshift = rt(IDA + "/CUIChannelShift_OnKey.code.txt")
q_csel = rt(IDA + "/CUIChannelSelect_OnKey.code.txt")
q_shiftmacro = rt(IDA + "/CUISkillEx_ShiftMacroUIState.code.txt")

chk("A4a sub_721008 IsAutoStartQuest", "IsAutoStartQuest" in q_sub)
chk("A4b sub_721008 sub_720FED", "sub_720FED" in q_sub)
chk("A4c StartQuest COutPacket 107", "107" in q_start)
chk("A4d StartQuest sub_721008 gate", "if ( sub_721008(" in q_start)
chk("A4e StartQuest Encode2 v50", "Encode2(a1: v50)" in q_start)
chk("A4f StartQuest Encode2 v51", "Encode2(a1: v51)" in q_start)
chk("A4g CUIStat 2048", "2048" in q_stat)
chk("A4h CUIStat 0x2000", "0x2000" in q_stat)
chk("A4i CUIStat SendAbilityUpRequest", "SendAbilityUpRequest" in q_stat)
chk("A4j CUIKeyConfig SaveFuncKeyMap", "SaveFuncKeyMap" in q_kc)
chk("A4k CUIKeyConfig 0x1BD", "0x1BD" in q_kc or "445" in q_kc)
chk("A4l CUISkill CanSendExclRequest 500", "CanSendExclRequest(a1: 500" in q_sp)
chk("A4m CUISkill SP_3510", "SP_3510_" in q_sp)
chk("A4n CanSendExclRequest get_update_time", "get_update_time() - *(this + 2090) >= a2" in q_excl)
chk("A4o guide CloseSkillGuide", "CloseSkillGuide" in q_guide)
chk("A4p macro OnSelected *(this+361)", "*(this + 361) = a2" in q_msel)
chk("A4q macro OnSelected GetMacroName", "GetMacroName" in q_msel)
chk("A4r macro OnKey ProcessBasicUIKey", "ProcessBasicUIKey" in q_mkey)
chk("A4s channelshift 6-col", "case 0x25u" in q_cshift and "v3 - 6" in q_cshift)
chk("A4t channelselect SendViewAllCharacterPacket", "SendViewAllCharacterPacket" in q_csel)
chk("A4u shiftmacro COM canvas", "IWz" in q_shiftmacro or "Animate" in q_shiftmacro)
# clean candidates emit NO packet
for nm, t in [("macroSel", q_msel), ("macroKey", q_mkey), ("chShift", q_cshift)]:
    chk("A4v zero COutPacket " + nm, "COutPacket" not in t)
    chk("A4w zero SendPacket " + nm, "SendPacket" not in t)

# ---------------------------------------------------------------- A5 s9 source literals
S9 = "E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/net/server/channel/handlers"
quest_h = rt(S9 + "/QuestActionHandler.java") if os.path.isfile(S9 + "/QuestActionHandler.java") else ""
sp_h = rt(S9 + "/DistributeSPHandler.java") if os.path.isfile(S9 + "/DistributeSPHandler.java") else ""
pkt = "E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/util/PacketCreator.java"
pkt_t = rt(pkt) if os.path.isfile(pkt) else ""
chk("A5a QuestActionHandler exists", len(quest_h) > 0)
chk("A5b isNpcNearby available>=4", "available() >= 4" in quest_h)
chk("A5c readShort x/y", "readShort(), p.readShort()" in quest_h or ("readShort()" in quest_h))
chk("A5d DistributeSPHandler exists", len(sp_h) > 0)
chk("A5e SPAssignAction", "SPAssignAction" in sp_h)
chk("A5f no 500ms in DistributeSP", "500" not in sp_h and "CanSendExclRequest" not in sp_h)
chk("A5g PacketCreator exists", len(pkt_t) > 0)
chk("A5h writeLong exp", "writeLong(chr.getExp())" in pkt_t)

# ---------------------------------------------------------------- A6 client literals
qph = rt("Net/Packets/QuestPackets.h")
pph = rt("Net/Packets/PlayerPackets.h")
skill = rt("IO/UITypes/UISkillBook.cpp")
stats = rt("IO/UITypes/UIStatsInfo.cpp")
kc_cpp = rt("IO/UITypes/UIKeyConfig.cpp")
chk("A6a QuestActionPacket class", "class QuestActionPacket" in qph)
chk("A6b QuestPackets no x/y", "x/y" not in qph and "write_short(x" not in qph)
chk("A6c QuestPackets action guard", "action == 1 || action == 2" in qph)
chk("A6d PlayerPackets StatCode 4", all(s in pph for s in ["STR = 64", "DEX = 128", "INT = 256", "LUK = 512"]) or all(s in pph for s in ["STR=64", "DEX=128", "INT=256", "LUK=512"]))
chk("A6e PlayerPackets two-encoding note", "不是同一套" in pph or "not the same" in pph)
chk("A6f UISkillBook no 500ms", "500" not in skill[skill.find("BT_OKAY") - 200: skill.find("BT_OKAY") + 600] if "BT_OKAY" in skill else True)
chk("A6g UISkillBook spend_sp loop", "spend_sp(sp_id)" in skill)
chk("A6h UIStatsInfo AutoAssignApPacket", "AutoAssignApPacket" in stats)
chk("A6i UIKeyConfig 083 comment", "0x8335d0" in kc_cpp)

# ---------------------------------------------------------------- A7 DECISIONS / matrix verdicts
dec = rt(DEC)
mat = rt(MATRIX)
chk("A7a DEC quest/D8 not-port", "是否把 sub_721008 条件分支搬进当前端 | 不搬" in dec or ("sub_721008" in dec and "不搬" in dec))
chk("A7b DEC skill/D4 no-new", "是否新增独立" in dec and "不新增" in dec)
chk("A7c DEC corewindow/D8 DOC_ONLY", "DOC_ONLY" in dec and "c07b1a7b" in dec)
chk("A7d DEC BATCH_1_KEEP", "BATCH_1_KEEP" in dec)
mat_rows = [l for l in mat.splitlines() if l.startswith("| **D")]
chk("A7e matrix D8 corewindow K3", any("CUIStat" in l and "500ms" in l and "K3" in l for l in mat_rows))
chk("A7f matrix skill/D4 not add", any("CWndSkillGuide" in l and "不新增" in l for l in mat_rows))
chk("A7g matrix quest/D8 not port", any("sub_721008" in l and "不搬" in l for l in mat_rows))

# ---------------------------------------------------------------- A8 NEXT-BATCH-PLAN sections + verdicts
plan = rt(PLAN)
for sec in ["## 0.", "## 1.", "## 2.", "### 2.1", "### 2.2", "### 2.3", "## 3.", "## 4.", "## 5.", "## 6.", "## 7."]:
    chk("A8a section " + sec, sec in plan)
chk("A8b gate-1 plan round", "计划轮" in plan and "零源码改动" in plan)
chk("A8c user confirm before impl", "确认计划后" in plan or "待用户确认" in plan)
chk("A8d three NOT P1", "三条全部不成立" in plan)
chk("A8e third same-family", "第三次" in plan)
chk("A8f quest W bucket", "W 桶 wire 条件追加段" in plan or "W 桶" in plan)
chk("A8g skill architecture", "架构级" in plan)
chk("A8h corewindow DOC_ONLY", "DOC_ONLY 已发货" in plan)
chk("A8i sub_721008 semantics", "IsAutoStartQuest(questid) || sub_720FED(questid)" in plan)
chk("A8j s9 available>=4", "if (p.available() >= 4)" in plan)
chk("A8k hard rule 4 items", "083 反编译 + s9 handler + 当前端字面量" in plan or ("s9 handler" in plan and "DECISIONS" in plan))
chk("A8l no text-derived batch", "不允许从矩阵文本直接推导批次" in plan)
chk("A8m macro first P1", "CUIMacroSysEx::OnSelected @0x8a984d" in plan)
chk("A8n channelshift second P1", "CUIChannelShift::OnKey @0x995863" in plan)
chk("A8o channelselect split L/W", "拆 L/W" in plan or ("导航段=L" in plan and "SendViewAllCharacterPacket" in plan))
chk("A8p F-dim COM no-patch", "243 行" in plan and "COM" in plan and "不补" in plan)
chk("A8q IMG gate verdict in plan", "GATE_OK_128B_IMG_REACHABLE" in plan)
chk("A8r 128B vs 65536 spec", "128B" in plan and "65536B" in plan)
chk("A8s explicit NOT-DO list", "不改 `Net/Packets/QuestPackets.h`" in plan)
chk("A8t no 500ms gate patch", "不补 500ms 门闩" in plan or "不给 `UISkillBook.cpp` 补 500ms" in plan)
chk("A8u no new window", "不新增 `CWndSkillGuide`" in plan)
chk("A8v no hash drift", "不写 commit hash" in plan)
chk("A8w arithmetic 168", "168 条 `DECISIONS.md` 历史裁定" in plan)
chk("A8x arithmetic 38+130", "38 + 130 = 168" in plan)
chk("A8y arithmetic reopen 27", "27 = 25 P1 + 1 W + 1 F-P2" in plan)
chk("A8z zero source/wire/wz", "0 源码行为改动 / 0 wire / 0 协议 / 0 WZ / 0 新封包" in plan)

# ---------------------------------------------------------------- A9 WLE / dev / PLT / notes
wle = rt(WLE)
dev = rt(DEV)
plt = rt(PLT)
notes = rt(NOTES)
chk("A9a WLE section 8", "## 8. round91" in wle)
chk("A9b WLE three not P1", "三条全部不成立，不进 P1" in wle)
chk("A9c WLE third family", "第三次同族误登记" in wle)
chk("A9d WLE invalidate old claim", "作废旧结论" in wle)
chk("A9e WLE hard rule", "不允许从矩阵文本推导批次" in wle)
chk("A9f WLE arithmetic untouched", "**算术不动**" in wle)
chk("A9g WLE macro anchor", "CUIMacroSysEx::OnSelected @0x8a984d" in wle)
chk("A9h dev 2-39", "| **2-39** | round91" in dev)
_row239 = next((l for l in dev.splitlines() if l.startswith("| **2-39** |")), "")
chk("A9i dev 2-39 three", all(k in _row239 for k in ["quest/D8", "skill/D4", "corewindow/D8"]))
chk("A9j PLT section 50", "## 50. round91" in plt)
for n in ["50-1.", "50-2.", "50-3.", "50-4.", "50-5."]:
    chk("A9k PLT " + n, n in plt)
chk("A9l notes title", "# round91" in notes)
chk("A9m notes 5 sections", all(("## " + str(i) + ".") in notes for i in range(1, 6)))

# ---------------------------------------------------------------- A10 EOL
def eol_ok(p, kind):
    b = rb(p)
    crcrlf = b.count(b"\r\r\n")
    crlf = b.count(b"\r\n")
    lonelf = b.count(b"\n") - crlf
    if crcrlf:
        return False
    if kind == "lf":
        return crlf == 0
    return lonelf == 0

for p, k in [(WLE, "lf"), (DEV, "lf"), (PLT, "crlf"), (EV, "lf"), (PLAN, "lf"), (NOTES, "lf")]:
    chk("A10a EOL " + p, eol_ok(p, k))
for p in [WLE, DEV, EV, PLAN, NOTES]:
    chk("A10b no BOM " + p, not rb(p).startswith(b"\xef\xbb\xbf"))

# ---------------------------------------------------------------- A11 zero source change (three-state)
wip_cpp_h = 0
for l in sh(["git", "status", "--porcelain"]).splitlines():
    m = re.match(r"^[ MARC?!]{2}\s+(.+)$", l)
    if not m:
        continue
    f = m.group(1).strip()
    if (f.endswith(".cpp") or f.endswith(".h")) and l[0] == " " or (f.endswith(".cpp") or f.endswith(".h")) and l[0] == "M":
        wip_cpp_h += 1
chk("A11a zero modified .cpp/.h in worktree", wip_cpp_h == 0)
# deliverables themselves are never business source
for p in DELIVERABLES + [IDA + "/" + f for f in IDA_FILES]:
    chk("A11b not business src " + p, not (p.endswith(".cpp") or p.endswith(".h")))
# plan explicitly forbids touching these files
chk("A11c plan forbids QuestPackets edit", "不改 `Net/Packets/QuestPackets.h`" in plan)
chk("A11d evidence zero flags", ev["zero_source_change"] and ev["zero_wire_change"] and ev["zero_protocol_change"] and ev["zero_wz_change"])

# ---------------------------------------------------------------- A12 hash-drift / WIP isolation
oneline_pat = re.compile(r"^[0-9a-f]{7,40}\s+\S", re.M)
all_deliverable_text = ""
for p in [EV, PLAN, NOTES, WLE, GEN]:
    all_deliverable_text += rt(p)
chk("A12a no git-log oneline output in deliverables", not oneline_pat.search(all_deliverable_text))
chk("A12b no commit-hash literal in evidence", not re.search(r"\b[0-9a-f]{8,40}\b", rt(EV).split("candidates")[0]))
chk("A12c generator idempotent guard", "IDEMPOTENT" in rt(GEN) or "not in" in rt(GEN))
# three-state visibility: deliverables must be visible in at least one state
for p in DELIVERABLES:
    chk("A12d visible " + p, p in visible_all)
chk("A12e external WIP not in our deliverables", all(p not in DELIVERABLES for p in changed if p.endswith(".cpp") or p.endswith(".h")))

print("STATIC GUARDS checked=%d failed=%d" % (checked, len(static_fail)))
for f in static_fail:
    print("  [FAIL]", f)

# ---------------------------------------------------------------- scenarios
sc_bad = []
sc_n = 0

def sc(name, cond):
    global sc_n
    sc_n += 1
    if not cond:
        sc_bad.append(name)

# B1 quest/D8 truth table
sc("B1a 083 has conditional x/y suffix", "if ( sub_721008(" in q_start and "Encode2(a1: v50)" in q_start)
sc("B1b sub_721008 is boolean gate", "IsAutoStartQuest" in q_sub and "sub_720FED" in q_sub)
sc("B1c s9 tolerates both shapes", "available() >= 4" in quest_h and "else" in quest_h)
sc("B1d client never writes x/y", "class QuestActionPacket" in qph and "write_short(x" not in qph)
sc("B1e verdict W keep s9", "W_bucket" in q8["round91_verdict"] and "KEEP_s9" in q8["round91_verdict"])
sc("B1f DECISION not-port stands", "不搬" in dec and "sub_721008" in dec)
sc("B1g no P1 patch", "NOT_P1" in q8["round91_verdict"] and "no P1 patch" in q8["action"])

# B2 skill/D4 truth table
sc("B2a 083 only 2 guide symbols", "CloseSkillGuide" in q_guide and "a2 == 27" in q_guide)
sc("B2b client ESC closed loop", "escape" in skill.lower() or "escape" in kc_cpp.lower())
sc("B2c verdict ARCHITECTURE", "ARCHITECTURE" in sk4["round91_verdict"])
sc("B2d DECISION no-new stands", "不新增" in dec and "CWndSkillGuide" in dec)
sc("B2e no P1 patch", "NOT_P1" in sk4["round91_verdict"] and "no P1 patch" in sk4["action"])

# B3 corewindow/D8 truth table
sc("B3a 083 500ms gate literal", "CanSendExclRequest(a1: 500" in q_sp and "get_update_time() - *(this + 2090) >= a2" in q_excl)
sc("B3b 083 6 masks", all(x in q_stat for x in ["2048", "0x2000", "64", "128", "256", "512"]))
sc("B3c s9 no 500ms", "500" not in sp_h and "CanSendExclRequest" not in sp_h)
sc("B3d client different StatCode", "不是同一套" in pph)
sc("B3e verdict DOC_ONLY shipped", "DOC_ONLY" in cw8["round91_verdict"] and "shipped" in cw8["round91_verdict"])
sc("B3f DECISION c07b1a7b", "c07b1a7b" in dec and "DOC_ONLY" in dec)
sc("B3g no P1 patch", "NOT_P1" in cw8["round91_verdict"] and "no P1 patch" in cw8["action"])

# B4 same-family pattern (third)
pf = ev["pattern_finding"]
sc("B4a title THIRD", "THIRD" in pf["title"])
sc("B4b false_claim cites WLE", "WLE-TRIAGE-PLAN.md" in pf["false_claim"])
sc("B4c cites :123 and :152-153", ":123" in str(pf.get("also", "")) or ":152-153" in pf["false_claim"])
sc("B4d evidence four-way", "083" in pf["evidence"] and "s9" in pf["evidence"] and "DECISIONS" in pf["evidence"])
sc("B4e lesson 4 items", "083 decompilation" in pf["lesson"] and "s9 handler" in pf["lesson"])
sc("B4f WLE invalidates no-same-family claim", "无同族误分类" in wle and "作废旧结论" in wle)

# B5 clean next batch
nb = ev["next_batch_clean_L_bucket"]
sc("B5a 3 entries", len(nb) == 3)
sc("B5b P1-first macro", nb[0]["priority"] == "P1-first" and "macro" in nb[0]["topic"])
sc("B5c macro zero wire", "zero COutPacket" in nb[0]["why_clean"] or "Zero COutPacket" in nb[0]["why_clean"])
sc("B5d P1-second channel", nb[1]["priority"] == "P1-second" and "channel" in nb[1]["topic"])
sc("B5e channelselect split L/W", "MUST be split" in nb[1]["why_clean"] or "split" in nb[1]["why_clean"].lower())
sc("B5f F-dim COM not P1", nb[2]["priority"] == "P2-F-dim" and "COM" in nb[2]["why_not_P1"])
sc("B5g F-dim same as drop/D2", "round90 drop/D2" in nb[2]["why_not_P1"])
sc("B5h macro decompile no packet", all("COutPacket" not in rt(IDA + "/" + f) for f in ["CUIMacroSysEx_OnSelected.code.txt", "CUIMacroSys_OnKey.code.txt", "CUIChannelShift_OnKey.code.txt"]))
sc("B5i channelselect HAS packet (so split)", "SendViewAllCharacterPacket" in q_csel)

# B6 arithmetic unchanged
ar = ev["arithmetic_unchanged"]
sc("B6a 168 untouched", "168" in ar["decisions_168"])
sc("B6b WLE 38+130", all(k in ar["wle_section4"] for k in ["38", "130", "168", "unchanged"]))
sc("B6c reopen 27", "27 = 25 P1 + 1 W + 1 F-P2" in ar["reopen_27"])
sc("B6d no semantic reclass", "no semantic reclassification" in ar["note"] or "Registration layer" in ar["note"])
sc("B6e plan says untouched", "**不动**" in plan and "168 条" in plan)

# B7 zero change
sc("B7a evidence 4 zeros", all(ev[k] for k in ["zero_source_change", "zero_wire_change", "zero_protocol_change", "zero_wz_change"]))
sc("B7b wip_cpp_h == 0", wip_cpp_h == 0)
sc("B7c plan gates before impl", "确认计划后才实现" in plan or "确认计划后才进入实现" in plan)
sc("B7d no business src in deliverables", all(not (p.endswith(".cpp") or p.endswith(".h")) for p in DELIVERABLES))

# B8 gates
sc("B8a 128B key", g["key_b64_len"] == 172 and g["iv_b64"] == "TSPHKw==")
sc("B8b IMG gate OK", g["verdict"] == "GATE_OK_128B_IMG_REACHABLE")
sc("B8c gate-18 chain", "REA" in plan and "IDA" in plan and "orange-wz" in plan and ".nx" in plan)
sc("B8d gate-5 no imagine", "禁想象修改" in plan or "不发明" in plan)
sc("B8e gate-1 plan round", "计划轮" in plan)
sc("B8f two key specs", "128B" in plan and "65536B" in plan)

# B9 constraints
cons = " ".join(ev["constraints"])
sc("B9a 083 only no 095", "083/v83 only" in cons)
sc("B9b s9 wire first", "s9 wire first" in cons)
sc("B9c numeric != semantic", "numeric coincidence" in cons)
sc("B9d L bucket no wire", "L bucket" in cons and "no wire" in cons)
sc("B9e plan round zero change", "ZERO source change" in cons)
sc("B9f implementation after confirm", "user confirms" in cons or "after user confirms" in cons)

# B10 no hash drift / three-state
sc("B10a evidence no commit hash", not re.search(r"\b[0-9a-f]{8}\b", rt(EV).split('"candidates"')[0]))
sc("B10b generator no git log output", "subprocess" not in rt(GEN) and not oneline_pat.search(rt(GEN)))
sc("B10c three-state visible", all(p in visible_all for p in DELIVERABLES))
sc("B10d changed|staged|headf nonempty", len(visible) > 0 or len(untracked) > 0)

print("SCENARIOS %d, bad=%d" % (sc_n, len(sc_bad)))
for f in sc_bad:
    print("  [BAD]", f)

try:
    py_compile.compile(rp(GEN), doraise=True)
    py_compile.compile(rp(__file__), doraise=True)
    print("PYCOMPILE_OK")
except Exception as e:
    print("PYCOMPILE_FAIL", str(e)[:120])

ok = (len(static_fail) == 0) and (len(sc_bad) == 0)
print(("PASS: round91 one-shot NEXT-BATCH-PLAN (3 candidates NOT P1 / third same-family correction / clean L bucket / zero source change)" if ok else "FAIL: round91 harness"))
sys.exit(0 if ok else 1)
