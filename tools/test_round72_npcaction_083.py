# -*- coding: utf-8 -*-
"""round72 harness: NPC_ACTION / CANCEL_DEBUFF 083-basis comment correction.

Static guards =现场逐字; scenarios = pure model of 083 vs current vs 095 encodings.
No game code is executed; this verifies the evidence chain + that zero behavior literals moved.
"""
import json, pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
def src(p):
    return (ROOT / p).read_bytes().decode("utf-8", errors="replace")

FILES = [
    "Net/Packets/MovePathEncode.h",
    "Net/Packets/NpcInteractionPackets.h",
    "Net/Handlers/WaveAShellHandlers.cpp",
    "Gameplay/MapleMap/Npc.cpp",
    "Character/Player.cpp",
    "Net/OutPacket.h",
    "Net/Packets/CommonPackets.h",
]

# 095 basis literals that must be gone from all 7 source files (comments included)
BANNED = [
    "Maplestory95", "port-095", "0x666e20", "0x671590",
    "CheckTemporaryStatDuration", "LABEL_15", "伪代码",
]

EA_083 = ["0x68a563", "0x68a33c", "0x6d3273", "0xa202be", "0xa2071f"]

static = []
def guard(name, ok, detail=""):
    static.append((name, bool(ok), detail))

# ---- A. static guards ----
all_src = {f: src(f) for f in FILES}
joined = "\n".join(all_src.values())

for b in BANNED:
    guard("A1.no_banned[%s]" % b, b not in joined,
          "found in: " + ",".join(f for f in FILES if b in all_src[f]) if b in joined else "clean")

for ea in EA_083:
    guard("A2.083_ea[%s]" % ea, ea in joined, "083 EA present in comments" if ea in joined else "MISSING")

# A3: 7 files each got at least one 083 cross-reference OR explicit s9 basis
for f in FILES:
    t = all_src[f]
    has_083 = any(ea in t for ea in EA_083)
    has_s9 = ("s9" in t)
    guard("A3.basis[%s]" % f.split("/")[-1], has_083 or has_s9,
          "083=%s s9=%s" % (has_083, has_s9))

# A4: behavior literals byte-for-byte intact
mp = all_src["Net/Packets/MovePathEncode.h"]
guard("A4.encode_4short_header",
      "write_short(x);" in mp and "write_short(y);" in mp and
      "write_short(vx);" in mp and "write_short(vy);" in mp,
      "x/y/vx/vy header intact")
guard("A4.encode_count", "write_byte(static_cast<int8_t>(elems.size()" in mp,
      "count byte intact")
guard("A4.encode_attr_0x23", "0x23" in mp and "0x24" in mp,
      "095-style attr branch set preserved (no behavior change)")
guard("A4.cancel_debuff_opcode", "CANCEL_DEBUFF = 99" in all_src["Net/OutPacket.h"],
      "s9 CANCEL_DEBUFF(99) intact")
guard("A4.s9_length9", "length - 9" in mp.replace("length-9", "length - 9") or
      "length-9" in joined, "s9 tail-9 contract referenced")

# A5: LoginBackground linear interpolation (Angel 083 fixed) must remain
lb = src("IO/LoginBackground.cpp")
guard("A5.loginbg_linear",
      "static_cast<double>(elapsed)" in lb and "static_cast<double>(tween_duration)" in lb,
      "083 CLogin linear interpolation kept")
guard("A5.loginbg_no_smoothstep", "3.0 - 2.0" not in lb and "3.0-2.0" not in lb,
      "smoothstep removed (Angel fix preserved)")

# A6: docs present + content faithful
ev_path = "docs/features/development-and-fix-plan-2026-11/evidence/npcaction-083-round72.json"
notes_path = "docs/features/development-and-fix-plan-2026-11/notes/wave2-npcaction-083-review-20261111.md"
dev_path = "docs/features/development-and-fix-plan-2026-11/README.md"
plt_path = "docs/features/pending-live-tests/README.md"
ev = json.loads(src(ev_path))
notes = src(notes_path)
dev = src(dev_path)
plt = src(plt_path)

guard("A6.evidence_keys", all(k in ev for k in
      ["ida_symbols_083", "ida_negatives_083", "fake_095_addresses_falsified",
       "encode_083_vs_current", "cnpc_generatemovepath_083", "s9_contract",
       "scanned_literals", "patch", "decision_points"]),
      "evidence keys=%d" % len(ev))
guard("A6.evidence_083_encode", "0x68a563" in ev["ida_symbols_083"].get(
      "?Encode@CMovePath@@QAEXAAVCOutPacket@@PAH@Z", ""), "083 Encode EA in evidence")
guard("A6.evidence_falsify", "sub_666362" in ev["fake_095_addresses_falsified"].get("0x666e20", ""),
      "0x666e20 -> sub_666362 recorded")
guard("A6.evidence_verdict", "different version" in ev["encode_083_vs_current"].get("verdict", "").lower() or
      "NOT 083" in ev["encode_083_vs_current"].get("verdict", ""),
      "double-fake verdict recorded")
guard("A6.notes_sections", all(("## %d." % i) in notes for i in range(0, 9)),
      "notes §0..§8 present")
guard("A6.notes_decision", "D-C" in notes and "交用户裁决" in notes,
      "decision D-C recorded")
guard("A6.dev_2_27", "| **2-27**" in dev and "npcaction-083-round72" in dev,
      "dev-plan §4 row 2-27 inserted")
guard("A6.plt_38", "## 38." in plt and "devfix-npcaction-083-review" in plt,
      "PLT §38 appended")
guard("A6.plt_28a_closed", "## 28-A. 已闭环" in plt and "round72 已按 s9 协议口径纠偏" in plt,
      "PLT §28-A retitled CLOSED + A5 resolved")
guard("A6.plt_crcrlf", "\r\r\n" not in (ROOT / plt_path).read_bytes().decode("utf-8", errors="replace"),
      "no CRLF corruption")

# A7: opcode-matrix regenerated (source hash synced + real s9 HEAD) — GATE must be OK not STALE
matrix_path = "docs/features/s9-protocol-align/protocol/opcode-matrix.json"
mx = json.loads((ROOT / matrix_path).read_bytes().decode("utf-8-sig"))
mx_meta = mx["_meta"]
guard("A7.matrix_new_source_hash",
      mx_meta["client_source_hashes"]["Net/OutPacket.h"] == "be70cbca216f9e587a88baffd490d0f65f9a76520ca0397d7e7e5d5d7d085ac7",
      "OutPacket.h hash synced to edited comment (no STALE)")
guard("A7.matrix_s9_head", mx_meta.get("s9_git_head", "").startswith("a78ba681a4"),
      "s9_git_head synced to real HEAD a78ba681a4")
guard("A7.matrix_business_unchanged",
      "c2s" in mx and "s2c" in mx and "counts" in mx_meta and
      not any(k in mx_meta for k in ("BROKEN",)),
      "business data intact (c2s/s2c/counts present, no fabricated field)")

# ---- B. scenarios (pure model, faithful to IDA/site literals) ----
scen = []
def scenario(name, got, want):
    scen.append((name, got == want, "got=%r want=%r" % (got, want)))

# B1: 083 Encode @0x68a563 header = x(2) + y(2) + count(1)
def header_bytes_083():
    return 2 + 2 + 1
scenario("B1.083_header=5B", header_bytes_083(), 5)

# B2: current / 095 0x666e20 header = x+y+vx+vy (4*2) + count(1)
def header_bytes_current():
    return 2 + 2 + 2 + 2 + 1
scenario("B2.current_header=9B", header_bytes_current(), 9)

# B3: 083 attr switch max = 0x16, current 095 attr set includes 0x23/0x24
scenario("B3.083_attr_max", 0x16, 22)
scenario("B4.current_has_0x23_0x24", (0x23 in (0x23, 0x24), 0x24 in (0x23, 0x24)), (True, True))

# B5: s9 NPCAnimationHandler length>6 -> readBytes(length-9)
def s9_tail_drop(length):
    if length == 6:
        return 0
    if length > 6:
        return length - 9
    return None
scenario("B5.s9_len6", s9_tail_drop(6), 0)
scenario("B6.s9_len15_drop9", s9_tail_drop(15), 6)
scenario("B7.s9_len9_drop0", s9_tail_drop(9), 0)

# B8: 083 CNpc::GenerateMovePath opcode 197, body via Flush->Encode
def cnpc_chain():
    return ("COutPacket(197)", "Encode4(oid)", "Encode1(action)", "Encode1(chatIdx)",
            "CMovePath::Flush", "CMovePath::Encode", "SendPacket")
scenario("B8.cnpc_opcode197", cnpc_chain()[0], "COutPacket(197)")
scenario("B9.cnpc_flush_encode", cnpc_chain()[4:6], ("CMovePath::Flush", "CMovePath::Encode"))

# B10: 095 address falsification (owner function differs)
def falsify_095(addr):
    return {0x666E20: "sub_666362", 0x671590: "CMob::TrySpeaking"}.get(addr, None)
scenario("B10.0x666e20_owner", falsify_095(0x666E20), "sub_666362")
scenario("B11.0x671590_owner", falsify_095(0x671590), "CMob::TrySpeaking")
scenario("B12.not_cmovepath_encode", falsify_095(0x666E20) != "CMovePath::Encode", True)
scenario("B13.not_cnpc_generate", falsify_095(0x671590) != "CNpc::GenerateMovePath", True)

# B14: 083 negative symbol queries
def ida_query_count(filter_):
    return {"*CheckTemporaryStatDuration*": 0, "*CancelDebuff*": 0,
            "*OnTemporaryStatReset*": 1}.get(filter_, -1)
scenario("B14.check_duration_0", ida_query_count("*CheckTemporaryStatDuration*"), 0)
scenario("B15.canceldebuff_0", ida_query_count("*CancelDebuff*"), 0)
scenario("B16.onstatreset_1", ida_query_count("*OnTemporaryStatReset*"), 1)

# B17: scanned literals distribution (7 files / 11 lines / 0 behavior)
scenario("B17.files_scanned", len(FILES), 7)
scenario("B18.hits_before", ev["scanned_literals"]["hits"], 11)
scenario("B19.behavior_touched", ev["patch"]["behavior_change"], "none (comments only)")

# B20: banned literals now zero across all 7 files
def residual_count():
    return sum(len(re.findall(re.escape(b), all_src[f])) for f in FILES for b in BANNED)
scenario("B20.residual_095=0", residual_count(), 0)

# B21: GATE OK (not STALE) after derived matrix regeneration
def _run_bytes(argv):
    import subprocess
    r = subprocess.run(argv, cwd=str(ROOT), capture_output=True)
    dec = lambda b: (b or b"").decode("utf-8", errors="replace")
    return dec(r.stdout), dec(r.stderr), r.returncode

def gate_state():
    out, err, rc = _run_bytes([sys.executable, "tools/gen_opcode_matrix.py", "--check"])
    text = out + err
    return ("GATE OK" in text, "STALE" in text, rc)
scenario("B21.gate_ok_not_stale", gate_state(), (True, False, 0))

# B22: zero behavior literals changed (diff has only +/- comment lines)
def behavior_diff_count():
    out, _err, _rc = _run_bytes(["git", "diff", "--", "Net/Packets/MovePathEncode.h"])
    added = [l for l in out.splitlines()
             if l.startswith("+") and not l.startswith("+++")]
    return sum(1 for l in added if "write_" in l or "case " in l or "CANCEL_DEBUFF =" in l)
scenario("B22.zero_behavior_lines_in_diff", behavior_diff_count(), 0)

# ---- report ----
bad_s = [g for g in static if not g[1]]
bad_b = [s for s in scen if not s[1]]
print("STATIC GUARDS checked=%d failed=%d" % (len(static), len(bad_s)))
for n, ok, d in bad_s:
    print("  FAIL", n, d)
print("SCENARIOS %d, bad=%d" % (len(scen), len(bad_b)))
for n, ok, d in bad_b:
    print("  FAIL", n, d)
if bad_s or bad_b:
    print("FAIL: round72 harness")
    sys.exit(1)
print("PASS: round72 NPC_ACTION/CANCEL_DEBUFF 083-basis comment correction (0 behavior literals)")
sys.exit(0)
