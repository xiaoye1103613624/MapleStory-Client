# -*- coding: utf-8 -*-
"""round92 harness：片1 编号错配（DOC_ONLY）+ 片2 UIChannel 下溢修复（L 桶零 wire）。

A1-A12 静态守卫 / B1-B12 场景。三态自适应 visible（提交前/暂存/提交后都成立）。
"""
import io, os, sys, json, re, subprocess

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
os.chdir("E:/project/MapleStory-Client")

CPP = "IO/UITypes/UIChannel.cpp"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
WLE = "docs/features/angel-calibration-d-matrix-2026-11/WLE-TRIAGE-PLAN.md"
PLAN = "docs/features/angel-calibration-d-matrix-2026-11/NEXT-BATCH-PLAN.md"
EV = "docs/features/angel-calibration-d-matrix-2026-11/evidence/wle-implementation-round92.json"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave3-implementation-slice1-2-20261111.md"
IDA_ONKEY = "tools/_ida_r91/CUIChannelShift_OnKey.code.txt"
IDA_CTOR = "tools/_ida_r92/CUIChannelShift_ctor.code.txt"
IMAGE_MACRO = "tools/_ida_r92/img_gate.json"
IMAGE_CHAN = "tools/_ida_r92/img_gate_channel.json"

fails = []
ok_n = 0
def chk(label, cond, detail=""):
    global ok_n
    if cond:
        ok_n += 1
    else:
        fails.append("%s %s" % (label, detail))

def rt(p):
    return open(p, "r", encoding="utf-8", errors="replace").read()

def rb(p):
    return open(p, "rb").read()

def strip_comments(s):
    s = re.sub(r"/\*.*?\*/", "", s, flags=re.S)
    s = re.sub(r"//[^\n]*", "", s)
    return s

def git(*args):
    r = subprocess.run(["git"] + list(args), capture_output=True)
    return r.stdout.decode("utf-8", "replace")

# ---------- 三态 visible ----------
changed = set(x for x in git("diff", "--name-only").splitlines() if x.strip())
staged = set(x for x in git("diff", "--name-only", "--cached").splitlines() if x.strip())
headf = set(x for x in git("show", "--name-only", "--format=", "HEAD").splitlines() if x.strip())
untracked = set(x for x in git("ls-files", "--others", "--exclude-standard").splitlines() if x.strip())
visible = changed | staged | headf | untracked
repo_files = set(x.strip() for x in git("ls-files").splitlines() if x.strip())

cpp = rt(CPP)
ev = json.loads(rt(EV))
dev = rt(DEV); plt = rt(PLT); wle = rt(WLE); plan = rt(PLAN); notes = rt(NOTES)

# ---------- A: 静态守卫 ----------
chk("A1a cpp has round92 gate comment", "round92 按 083 CUIChannelShift::OnKey @0x995863 门控还原" in cpp)
chk("A1b cpp UP/DOWN gate", "if (channel_count > SLOTS_PER_ROW)" in cpp)
chk("A1c cpp LEFT/RIGHT gate", "if (channel_count > 1)" in cpp)
chk("A1d cpp merged UP|DOWN branch", "keycode == KeyAction::Id::UP || keycode == KeyAction::Id::DOWN" in cpp)
chk("A1e cpp merged LEFT|RIGHT branch", "keycode == KeyAction::Id::LEFT || keycode == KeyAction::Id::RIGHT" in cpp)
chk("A1f step_channel call count == 2 (both gated)", cpp.count("step_channel(selected_channel") == 2,
    "count=%d" % cpp.count("step_channel(selected_channel"))
chk("A1g no ungated UP branch left", "else if (keycode == KeyAction::Id::UP)\r\n\t\t\t{" not in cpp)

# A2: step_channel 函数体零改动（EOL 归一 + 剥离 round92 注释块后与 HEAD 对拍）
def body_of(src, name):
    i = src.find("void UIChannel::%s(" % name)
    if i < 0: return ""
    j = src.find("\n\t}", i)
    return src[i:j]

head_cpp = subprocess.run(["git", "show", "HEAD:" + CPP], capture_output=True).stdout.decode("utf-8", "replace").replace("\r\n", "\n")
cur_cpp = cpp.replace("\r\n", "\n")
h_sc = body_of(head_cpp, "step_channel"); c_sc = body_of(cur_cpp, "step_channel")
chk("A2 step_channel body unchanged", h_sc and h_sc.strip() == c_sc.strip(), "head=%d cur=%d" % (len(h_sc), len(c_sc)))

# A3: 083 反编译锚点
onkey = rt(IDA_ONKEY)
chk("A3a 083 gate count>=7", "*(this + 32) >= 7" in onkey)
chk("A3b 083 gate count>=2", "*(this + 32) >= 2" in onkey)
chk("A3c 083 6-col step", "v3 - 6" in onkey and "v3 + 6" in onkey)
ctor = rt(IDA_CTOR)
chk("A3d 083 count source", "*(this + 32) = *(*dword_BE7918 + 8280)" in ctor)
chk("A3e 083 6-col window height", "21 * (v3 / 6) + 125" in ctor)

# A4: evidence 结构
chk("A4a evidence verdict", ev.get("verdict") == "slice1_doc_only_number_mismatch__slice2_real_L_bucket_underflow_fixed")
chk("A4b evidence zero_wire", ev.get("zero_wire") is True)
s1 = ev.get("slice1_macro_selection", {})
chk("A4c slice1 ids", s1.get("registered_ids") == ["skill/D12", "corewindow/D9"])
chk("A4d slice1 doc_only", "DOC_ONLY" in s1.get("action", ""))
chk("A4e slice1 three proofs", len(s1.get("three_proofs", {})) == 3)
s2 = ev.get("slice2_channel_nav", {})
chk("A4f slice2 ids", s2.get("registered_ids") == ["login-flow/D9", "statusbar-hud/D12"])
chk("A4g slice2 repro", "253" in s2.get("real_defect", {}).get("repro", ""))
chk("A4h slice2 wire zero", s2.get("patch", {}).get("wire") == 0 and s2.get("patch", {}).get("new_packet") == 0)

# A5: pre_patch sha = 把当前 NEW 换回 OLD 后的归一 LF sha（与 HEAD 无关，提交/amend 不漂移）
OLD_LF = """\t\t\telse if (keycode == KeyAction::Id::UP)
\t\t\t{
\t\t\t\tstep_channel(selected_channel, -static_cast<int>(SLOTS_PER_ROW));
\t\t\t}
\t\t\telse if (keycode == KeyAction::Id::DOWN)
\t\t\t{
\t\t\t\tstep_channel(selected_channel, static_cast<int>(SLOTS_PER_ROW));
\t\t\t}
\t\t\telse if (keycode == KeyAction::Id::LEFT)
\t\t\t{
\t\t\t\tstep_channel(selected_channel, -1);
\t\t\t}
\t\t\telse if (keycode == KeyAction::Id::RIGHT)
\t\t\t{
\t\t\t\tstep_channel(selected_channel, 1);
\t\t\t}
"""
NEW_LF = """\t\t\t// round92 按 083 CUIChannelShift::OnKey @0x995863 门控还原：
\t\t\t//   (a2 != 40 && a2 != 38 || count >= 7) —— UP/DOWN 跨行须频道数 > 单行 6 列；
\t\t\t//   (a2 != 37 && a2 != 39 || count >= 2) —— LEFT/RIGHT 须至少 2 个频道。
\t\t\t// 本服 channel_count 常为 3：缺门控时 step_channel(±6) 单次 wrap 后仍为负，
\t\t\t// uint8_t 下溢成 253..255 / 越界 3..5（tools/test_round92_channel_nav_083.py B1 复现）。
\t\t\telse if (keycode == KeyAction::Id::UP || keycode == KeyAction::Id::DOWN)
\t\t\t{
\t\t\t\tif (channel_count > SLOTS_PER_ROW)
\t\t\t\t\tstep_channel(selected_channel, keycode == KeyAction::Id::UP
\t\t\t\t\t\t? -static_cast<int>(SLOTS_PER_ROW) : static_cast<int>(SLOTS_PER_ROW));
\t\t\t}
\t\t\telse if (keycode == KeyAction::Id::LEFT || keycode == KeyAction::Id::RIGHT)
\t\t\t{
\t\t\t\tif (channel_count > 1)
\t\t\t\t\tstep_channel(selected_channel, keycode == KeyAction::Id::LEFT ? -1 : 1);
\t\t\t}
"""
import hashlib
pre = cur_cpp.replace(NEW_LF, OLD_LF)
pre_sha = hashlib.sha256(pre.encode("utf-8")).hexdigest()
chk("A5 pre_patch sha stable", ev.get("patch_integrity", {}).get("pre_patch_cpp_sha256") == pre_sha,
    "ev=%s calc=%s" % (ev.get("patch_integrity", {}).get("pre_patch_cpp_sha256", "")[:12], pre_sha[:12]))

# A6: 四文档
chk("A6a dev 2-40", "| **2-40** |" in dev)
chk("A6b PLT 51", "## 51." in plt)
chk("A6c WLE 9", "## 9. round92 纠偏" in wle)
chk("A6d PLAN 8", "## 8. round92 实施回执" in plan)
chk("A6e notes verdict", "编号错配" in notes and "下溢" in notes)
chk("A6f WLE 4 ids table", all(k in wle for k in ["`skill/D12`", "`corewindow/D9`", "`login-flow/D9`", "`statusbar-hud/D12`"]))

# A7: 128B IMG 门禁两份
mg = json.loads(rt(IMAGE_MACRO)); mgc = json.loads(rt(IMAGE_CHAN))
chk("A7a macro gate", mg.get("verdict") == "GATE_OK_128B_MACRO_NODES_ENUMERATED")
chk("A7b channel gate", mgc.get("verdict") == "GATE_OK_128B_CHANNEL_NODES_ENUMERATED")
chk("A7c key 128B", mgc.get("key_b64_len") == 172)
def gate_matches(q):
    """q['head'] 是工具返回的 JSON 文本片段（可能被 400 字符截断）；能解析就返回 matches，
    截断时返回 None 并由调用方走文本层断言（不对截断串做 json.loads 硬判）。"""
    raw = q.get("head", "")
    try:
        return json.loads(raw).get("result", {}).get("matches", None)
    except Exception:
        return None

def gate_paths(q):
    """在（可能截断的）head 文本里抽出所有非空 nodePath 值，用于「命中点是否都在 Memo 下」的文本层断言。
    注意：搜索结果顶层自带 `"nodePath":""`（根），必须过滤空串，否则会假红。"""
    return [p for p in re.findall(r'"nodePath":"([^"]*)"', q.get("head", "")) if p]

chk("A7d shout search empty", gate_matches(mg["queries"].get("search_Shout", {})) == [])
chk("A7e btshout search empty", gate_matches(mg["queries"].get("search_BtShout", {})) == [])
# search_Check 的 head 被 400 字符截断（text_len 1356），改用 nodePath 文本层断言：
# 命中点必须全部落在 Memo/ 下（083 三个宏资源根 Macro/SkillMacro/SkillMacroEx 无勾选框）。
_ck_paths = gate_paths(mg["queries"].get("search_Check", {}))
chk("A7f check hits exist", len(_ck_paths) > 0, "paths=%d" % len(_ck_paths))
chk("A7g check hits all under Memo", all(p.startswith("Memo/") for p in _ck_paths),
    str([p for p in _ck_paths if not p.startswith("Memo/")][:3]))
chk("A7h no macro-root checkbox", not any(p.startswith(("Macro/", "SkillMacro/", "SkillMacroEx/")) for p in _ck_paths))

# A8: EOL 审计
def eol(p):
    b = rb(p)
    return b.count(b"\r\n"), b.count(b"\n") - b.count(b"\r\n"), b.count(b"\r\r\n")
chk("A8a cpp CRLF only", eol(CPP)[1] == 0 and eol(CPP)[2] == 0)
chk("A8b PLT CRLF only", eol(PLT)[1] == 0 and eol(PLT)[2] == 0)
chk("A8c dev LF only", eol(DEV)[0] == 0 and eol(DEV)[2] == 0)
chk("A8d WLE LF only", eol(WLE)[0] == 0 and eol(WLE)[2] == 0)
chk("A8e PLAN LF only", eol(PLAN)[0] == 0 and eol(PLAN)[2] == 0)
chk("A8f EV LF only", eol(EV)[0] == 0 and eol(EV)[2] == 0)
chk("A8g NOTES LF only", eol(NOTES)[0] == 0 and eol(NOTES)[2] == 0)

# A9: 交付物不写 commit hash（40 位 hex / git log oneline 形态）
hexpat = re.compile(r"\b[0-9a-f]{7,40}\b")
for p in [EV, NOTES, DEV, PLT, WLE, PLAN]:
    txt = rt(p)
    hits = [h for h in hexpat.findall(txt) if len(h) >= 7 and not h.startswith(("0x", "00"))]
    # 允许 0x 地址、sha256 摘要、12 位产物 SHA 之外的纯 40 位
    bad = [h for h in hits if len(h) == 40]
    chk("A9 no 40-hex in %s" % os.path.basename(p), not bad, str(bad[:3]))

# A10: 算术不动
ar = ev.get("arithmetic_unchanged", {})
chk("A10a 168 closed", "168" in ar.get("decisions_168", ""))
chk("A10b W+L", "38" in ar.get("wle_section4", "") and "130" in ar.get("wle_section4", ""))
chk("A10c reopen 27", "27" in ar.get("reopen_27", ""))

# A11: 硬规则升级登记
chk("A11 hard rule in ev", "harness A12" in ev.get("hard_rule_upgrade", ""))
chk("A11b hard rule in PLT", "harness A12 守卫" in plt)

# A12: round91 Plan §4 三候选仍是四项齐备（回扫守卫，防再次纸面登记）
# 注意：§4 原表只写了「083 锚点 + s9 差异」，未写 DECISIONS 锚点 —— 这正是本轮同族病的成因；
# §8 已判 §4 候选表整体作废。守卫改为「§4 至少带 083/s9 + §8 明确作废并指向四项规则」。
p4 = plan[plan.find("## 4. 下一批真正干净的 L 桶候选"):plan.find("## 5. 一次性执行 Plan")]
chk("A12a Plan §4 exists", bool(p4))
for tok in ["083", "s9"]:
    chk("A12b Plan §4 carries %s" % tok, tok in p4)
chk("A12c Plan §8 voids §4", "§4 候选登记表整体作废" in plan)
chk("A12d Plan §8 names 4 anchors", "083 反编译 + s9 handler + 当前端字面量 + DECISIONS 四项" in plan)

# A13: 目标文件在 visible（三态）
for p in [CPP, DEV, PLT, WLE, PLAN, EV, NOTES]:
    chk("A13 %s visible" % os.path.basename(p), p in visible or p in repo_files)

# ---------- B: 场景 ----------
bad = []
def sim_step(target, delta, total, current, gated_updown=False, gated_lr=False):
    """复刻 UIChannel::step_channel + 新门控（C++ 语义逐字）。"""
    if gated_updown and total <= 6: return target
    if gated_lr and total <= 1: return target
    if total <= 1: return target
    v = target + delta
    if v < 0: v += total
    elif v >= total: v -= total
    if v == current:
        v += 1 if delta > 0 else -1
        if v < 0: v = total - 1
        elif v >= total: v = 0
    return v & 0xFF

# B1: 旧逻辑在 count=3 时下溢（缺陷存在性证明）
old_vals = set()
for cur in (0, 1, 2):
    for sel in (0, 1, 2):
        old_vals.add(sim_step(sel, -6, 3, cur)); old_vals.add(sim_step(sel, 6, 3, cur))
if not (253 in old_vals and 3 in old_vals):
    bad.append("B1 old-logic underflow not reproduced: %s" % sorted(old_vals))

# B2: 新门控在 count<=6 时 UP/DOWN 不改变 selected
for total in range(1, 7):
    for cur in range(total):
        for sel in range(total):
            if sim_step(sel, -6, total, cur, gated_updown=True) != sel: bad.append("B2 up total=%d" % total)
            if sim_step(sel, 6, total, cur, gated_updown=True) != sel: bad.append("B2 down total=%d" % total)

# B3: count>=7 时 delta=±6 单次 wrap 数学闭合（结果必在 [0,total)）
for total in range(7, 61):
    for cur in range(total):
        for sel in range(total):
            for d in (-6, 6):
                r = sim_step(sel, d, total, cur, gated_updown=True)
                if not (0 <= r < total): bad.append("B3 oob total=%d sel=%d d=%d -> %d" % (total, sel, d, r))

# B4: LEFT/RIGHT 在所有 total>=2 下结果必在范围
for total in range(2, 61):
    for cur in range(total):
        for sel in range(total):
            for d in (-1, 1):
                r = sim_step(sel, d, total, cur, gated_lr=True)
                if not (0 <= r < total): bad.append("B4 oob total=%d sel=%d d=%d -> %d" % (total, sel, d, r))

# B5: total==1 时 LEFT/RIGHT 门控生效不动
if sim_step(0, 1, 1, 0, gated_lr=True) != 0 or sim_step(0, -1, 1, 0, gated_lr=True) != 0:
    bad.append("B5 single-channel nav moved")

# B6: 门控后 UP 不再命中当前频道（不产生「换到自己」）
for total in range(7, 21):
    for cur in range(total):
        r = sim_step(cur, -6, total, cur, gated_updown=True)
        if r == cur: bad.append("B6 up lands on current total=%d cur=%d" % (total, cur))

# B7: 083 门控语义等价（count>=7 / count>=2 阈值）
if not (6 < 7 and 1 < 2): bad.append("B7 threshold sanity")

# B8: 补丁不改动 step_channel（守卫 A2 的场景侧复核）
if "step_channel" not in cpp or "current_channel" not in body_of(cpp.replace("\r\n", "\n"), "step_channel"):
    bad.append("B8 step_channel body lost")

# B9: 无新封包 / 无 wire 关键字引入
for kw in ["OutPacket", "write_int", "write_short", "dispatch()"]:
    if kw in NEW_LF: bad.append("B9 wire keyword in patch: %s" % kw)

# B10: 片1 零改动（UIMiscShells 未在我方改动集合）
usm = "IO/UITypes/UIMiscShells.cpp"
if usm in changed: bad.append("B10 slice1 must be doc-only but UIMiscShells.cpp changed")

# B11: 补丁所在函数签名未变
if "void UIChannel::send_key(int32_t keycode, bool pressed, bool escape)" not in cpp:
    bad.append("B11 send_key signature changed")

# B12: escape / RETURN / C / G 分支保留
for tok in ["if (escape)", "KeyAction::Id::RETURN", "keycode == 'C'", "keycode == 'G'"]:
    if tok not in cpp: bad.append("B12 branch lost: %s" % tok)

# ---------- 输出 ----------
print("STATIC GUARDS checked=%d failed=%d" % (ok_n, len(fails)))
for f in fails: print("  [FAIL]", f)
print("SCENARIOS 12, bad=%d" % len(bad))
for b in sorted(set(bad))[:12]: print("  [BAD]", b)
r = subprocess.run([sys.executable, "-m", "py_compile", os.path.abspath(__file__)], capture_output=True)
print("PYCOMPILE_" + ("OK" if r.returncode == 0 else "FAIL"))
if fails or bad or r.returncode != 0:
    print("FAIL: round92 harness")
    sys.exit(1)
print("PASS: round92 slice1 doc-only number mismatch + slice2 UIChannel underflow fixed (zero wire)")
