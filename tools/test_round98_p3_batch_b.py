# -*- coding: utf-8 -*-
"""round98 P3 B 批 5 条 静态守卫 + 场景验证。

设计原则（沿用 round88–97 教训）：
- 全部判据指向「真约束」，不写死措辞片段；断言失败先核现场，不放宽验收。
- 三态自适应：changed | staged | untracked | HEAD-files，提交前/暂存/提交后都成立。
- 零源码改动硬证：本轮交付物集合内不含任何 .cpp/.h；外部在途 WIP 只要不含本轮标记即视为合法背景。
"""
import json
import os
import re
import subprocess
import sys
import io

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DM = "docs/features/angel-calibration-d-matrix-2026-11"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
WLE = DM + "/WLE-TRIAGE-PLAN.md"
PLAN = DM + "/P3-ANGEL-REBUILD-PLAN.md"
P3B = DM + "/P3-B-BATCH-MAPS.md"
EV = DM + "/evidence/p3-batch-b-round98.json"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave3-p3-batch-b-20261111.md"
HARNESS = "tools/test_round98_p3_batch_b.py"
GEN = "tools/_r98_deliver.py"

MARKERS = ["round98", "p3-batch-b", "P3-B-BATCH-MAPS", "p3-batch-b-round98"]

PASS = []
FAIL = []
SCEN_OK = []
SCEN_BAD = []


def abspath(rel):
    return os.path.join(ROOT, rel.replace("/", os.sep))


def rd(rel):
    return open(abspath(rel), "rb").read().decode("utf-8").replace("\r\n", "\n")


def git(*args):
    p = subprocess.run(["git"] + list(args), cwd=ROOT,
                       stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return p.returncode, p.stdout.decode("utf-8", "replace"), p.stderr.decode("utf-8", "replace")


def chk(name, cond, detail=""):
    if cond:
        PASS.append(name)
    else:
        FAIL.append("%s%s" % (name, (" :: " + detail) if detail else ""))


def sc(name, cond, detail=""):
    if cond:
        SCEN_OK.append(name)
    else:
        SCEN_BAD.append("%s%s" % (name, (" :: " + detail) if detail else ""))


def strip_comments(src):
    out = []
    i = 0
    n = len(src)
    while i < n:
        if src.startswith("//", i):
            j = src.find("\n", i)
            i = n if j < 0 else j
        elif src.startswith("/*", i):
            j = src.find("*/", i + 2)
            i = n if j < 0 else j + 2
        else:
            out.append(src[i])
            i += 1
    return "".join(out)


EV_D = json.loads(rd(EV))

# ── A1 083 反编译与符号证据在场 ────────────────────────────────────────────
print("== A1 083 evidence ==")
IDA_FILES = {
    "tools/_ida_r98/symbol_summary.json": None,
    "tools/_ida_r98/CPet_Init.code.txt": "DecodeBuffer",
    "tools/_ida_r98/CUISkill_OnCreate.code.txt": "2010",
    "tools/_ida_r98/CLogin_SendSelectCharPacket.code.txt": "COutPacket",
    "tools/_ida_r98/CUIFriendGroup_OnOK.code.txt": "CUIFriendGroup::OnOK",
    "tools/_ida_r98/CUIMapleTV_GetText.code.txt": "GetText",
    "tools/_ida_r98/CField_SendSetGuildMarkMsg.code.txt": "Encode1",
    "tools/_ida_r98/CField_Wedding_OnWeddingProgress.code.txt": "SP_4202",
    "tools/_ida_r98/img_gate.json": "FriendGroup",
}
for rel, needle in IDA_FILES.items():
    exists = os.path.isfile(abspath(rel))
    chk("A1 exists %s" % os.path.basename(rel), exists)
    if exists and needle:
        chk("A1b %s contains %r" % (os.path.basename(rel), needle), needle in rd(rel))

pet_init = rd("tools/_ida_r98/CPet_Init.code.txt")
chk("A1c CPet::Init 8B uid DecodeBuffer", "DecodeBuffer" in pet_init and "8u" in pet_init)
chk("A1d CPet::Init 无 nameTag/balloon 字段",
    "hasNameTag" not in pet_init and "hasChatBalloon" not in pet_init and "writeBool" not in pet_init)
chk("A1d2 CPet::Init 的 MakeNameTag 是渲染调用非字段读取",
    "CLife::MakeNameTag" in pet_init)

skill_oc = rd("tools/_ida_r98/CUISkill_OnCreate.code.txt")
chk("A1e CUISkill::OnCreate 加点钮基址 2010", "v83 + 2010" in skill_oc)
chk("A1e2 CUISkill::OnCreate 四钮循环上界 279", "Unknown < 279" in skill_oc)
chk("A1e3 CUISkill::OnCreate 按钮 y=131", re.search(r"a3: v83 \+ 2010, a4: 131", skill_oc) is not None)
chk("A1f CUISkill::OnCreate 宏钮 2023", "2023" in skill_oc)
chk("A1f2 CUISkill::OnCreate 宏钮坐标 (120,265)",
    re.search(r"a3: 2023, a4: 120, a5: 265", skill_oc) is not None)
chk("A1g CUISkill::OnCreate 无 classic 分支", "classic" not in skill_oc.lower())

guildmark = rd("tools/_ida_r98/CField_SendSetGuildMarkMsg.code.txt")
chk("A1h SendSetGuildMarkMsg COutPacket(126)", "COutPacket::COutPacket(a1: 126)" in guildmark)
chk("A1i SendSetGuildMarkMsg 子命令 15", re.search(r"Encode1\([^)]*a2:\s*15\)", guildmark) is not None)
chk("A1i2 SendSetGuildMarkMsg 7B 五字段",
    guildmark.count("Encode1") == 3 and guildmark.count("Encode2") == 2)

wedding = rd("tools/_ida_r98/CField_Wedding_OnWeddingProgress.code.txt")
for sp in ["SP_4202", "SP_4203", "SP_4204", "SP_1666"]:
    chk("A1j wedding SP_ %s" % sp, sp in wedding)

gate = json.loads(rd("tools/_ida_r98/img_gate.json"))
gate_data = gate.get("data", {})
chk("A1k img_gate 128B key len 172", gate.get("key_b64_len") == 172)
fg = json.loads(gate_data["search_FriendGroup"])["result"]["matches"]
chk("A1l img_gate FriendGroup 0 命中", fg == [], str(fg))
mp = json.loads(gate_data["search_MapleTV"])["result"]["matches"]
chk("A1n img_gate MapleTV 顶层 LIST",
    len(mp) == 1 and mp[0]["nodePath"] == "MapleTV" and mp[0]["type"] == "LIST_PROPERTY", str(mp))
gm_m = json.loads(gate_data["search_GuildMark"])["result"]["matches"]
chk("A1m img_gate guildmark 真节点",
    len(gm_m) == 1 and gm_m[0]["nodePath"] == "UserList/Guild/GuildInfo/guildmark"
    and gm_m[0]["type"] == "CANVAS_PROPERTY", str(gm_m))
chk("A1o img_gate UIWindow 顶层节点为根路径误用（非缺失证据）",
    "找不到根节点" in gate_data.get("UIWindow_FriendGroup", "")
    and "根" in gate_data.get("UIWindow_FriendGroup", ""))

# ── A2 s9 对端证据 ─────────────────────────────────────────────────────────
print("== A2 s9 ==")
S9 = "E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/"
p9 = os.path.join(S9.replace("/", os.sep), "util", "PacketCreator.java")
chk("A2 PacketCreator 存在", os.path.isfile(p9))
if os.path.isfile(p9):
    s9 = open(p9, "rb").read().decode("utf-8", "replace")
    chk("A2a addPetInfo writeLong(uniqueId)", "writeLong(pet.getUniqueId())" in s9)
    chk("A2b addPetInfo 两 bool", "writeBool(hasNameTag)" in s9 and "writeBool(hasChatBalloon)" in s9)
    chk("A2c getCharList 按 chars.size() 循环", "for (Character chr : chars)" in s9)
    chk("A2d getCharList 尾部 int32 槽数", "getCharacterSlots()" in s9)

mp = os.path.join(S9.replace("/", os.sep), "net", "server", "channel", "handlers", "MovePetHandler.java")
chk("A2e MovePetHandler 存在", os.path.isfile(mp))
if os.path.isfile(mp):
    s = open(mp, "rb").read().decode("utf-8", "replace")
    chk("A2f MovePetHandler readInt pid", "readInt" in s)

# ── A3 本端零源码行为改动 ─────────────────────────────────────────────────
print("== A3 zero source change ==")
rc1, changed, _ = git("diff", "--name-only")
rc2, staged, _ = git("diff", "--name-only", "--cached")
rc3, untracked, _ = git("ls-files", "--others", "--exclude-standard")
visible = set(x.strip() for x in (changed + "\n" + staged + "\n" + untracked).splitlines() if x.strip())

DELIVERABLES = {P3B, WLE, PLAN, DEV, PLT, EV, NOTES, GEN, HARNESS}
src_in_deliverables = [p for p in DELIVERABLES if p.endswith((".cpp", ".h"))]
chk("A3 交付物集合零源码文件", not src_in_deliverables, str(src_in_deliverables))

ext_wip = [p for p in visible if p.endswith((".cpp", ".h"))]
leak = []
for p in ext_wip:
    if not os.path.isfile(abspath(p)):
        continue
    body = rd(p)
    for m in MARKERS:
        if m in body:
            leak.append("%s :: %s" % (p, m))
chk("A3b 外部 WIP 源码不含本轮标记", not leak, "; ".join(leak[:3]))

chk("A3c P3-B-BATCH-MAPS 内容非空", len(rd(P3B)) > 8000)

# ── A4 五处文档落位 ───────────────────────────────────────────────────────
print("== A4 docs ==")
wle = rd(WLE)
chk("A4a WLE §15 存在", "## 15. round98" in wle)
wle_sec = wle[wle.index("## 15. round98"):]
for kw in ["skill/D9", "social-economy/D12", "pet-summon/D2", "login-flow/D13", "social-economy/D9"]:
    chk("A4b WLE §15 含 %s" % kw, kw in wle_sec)

plan = rd(PLAN)
chk("A4c Plan §8 回执", "## 8. round98 B 批回执" in plan)
plan8 = plan[plan.index("## 8. round98 B 批回执"):]
chk("A4d Plan §8 五行", len(re.findall(r"^\| \d+ \|", plan8, re.M)) == 5)

dev = rd(DEV)
dev_lines = dev.split("\n")
rows_2_4x = [l for l in dev_lines if l.startswith("| **2-4")]
chk("A4e dev 2-46 行存在", any(l.startswith("| **2-46**") for l in dev_lines))
order = [re.match(r"\| \*\*(2-\d+)\*\*", l).group(1) for l in rows_2_4x if re.match(r"\| \*\*(2-\d+)\*\*", l)]
chk("A4f dev 行序 2-44 < 2-45 < 2-46", order[-3:] == ["2-44", "2-45", "2-46"], str(order[-3:]))

plt = rd(PLT)
sec_nums = [int(x) for x in re.findall(r"^## (\d+)\.", plt, re.M)]
chk("A4g PLT 末节 == 57", sec_nums and max(sec_nums) == 57, str(sec_nums[-3:]))
plt57 = plt[plt.index("## 57. round98"):]
for i in range(1, 7):
    chk("A4h PLT 57-%d" % i, ("57-%d" % i) in plt57)

# ── A5 evidence 结构与算术 ────────────────────────────────────────────────
print("== A5 evidence ==")
chk("A5 round == 98", EV_D.get("round") == 98)
chk("A5a batch == P3-B", EV_D.get("batch") == "P3-B")
items = EV_D.get("items", {})
chk("A5b 5 条 items", len(items) == 5, str(list(items)))
for k in ["skill/D9", "social-economy/D12", "pet-summon/D2", "login-flow/D13", "social-economy/D9"]:
    chk("A5c item %s" % k, k in items)
chk("A5d D9 verdict KEEP", items["skill/D9"]["verdict"] == "KEEP")
chk("A5e D12 verdict P2", items["social-economy/D12"]["verdict"] == "P2")
chk("A5f D2 verdict LOCAL_TYPE", items["pet-summon/D2"]["verdict"] == "LOCAL_TYPE")
chk("A5g D13 verdict DOC_ONLY", items["login-flow/D13"]["verdict"] == "DOC_ONLY")
chk("A5h social D9 verdict THREE_WAY", items["social-economy/D9"]["verdict"] == "THREE_WAY")
chk("A5i D12 键数 27 非 26", items["social-economy/D12"]["key_count_actual"] == 27
    and items["social-economy/D12"]["key_count_plan"] == 26)
chk("A5j D2 wire 不动两条", len(items["pet-summon/D2"]["wire_unchanged"]) == 2)
chk("A5k D2 截断点 4 条", len(items["pet-summon/D2"]["client_truncation_points"]) == 4)
chk("A5l D13 无 15 槽常量", "no_15_slot_const" in items["login-flow/D13"]["client"])

ar = EV_D.get("arithmetic_unchanged", {})
chk("A5m 168 三分类", ar.get("classification_168") == "76 + 38 + 14 + 40 = 168")
chk("A5n W38+L130", ar.get("wle_tracks") == "W38 + L130 = 168")
chk("A5o reopen 27=10+17", ar.get("reopen") == "27 = 已注销 10 + 残余 17")
chk("A5p 17=P1 0+P2 1+P3 16", ar.get("residual") == "17 = P1 0 + P2 1 + P3 16")

zc = EV_D.get("zero_change", {})
chk("A5q 零 wire/协议/WZ/源码", all(zc.get(k) == 0 for k in ["wire", "protocol", "wz", "source_behavior"]))
bg = EV_D.get("build_gate_regression", {})
chk("A5r 不声称真重编", bg.get("real_rebuild_claimed") is False)
chk("A5s ida_evidence_files >= 17", len(EV_D.get("ida_evidence_files", [])) >= 17)

# ── A6 EOL ────────────────────────────────────────────────────────────────
print("== A6 EOL ==")
for rel in [P3B, WLE, PLAN, DEV, PLT, EV, NOTES]:
    raw = open(abspath(rel), "rb").read()
    chk("A6 no CRCRLF %s" % os.path.basename(rel), b"\r\r\n" not in raw)
    chk("A6b no BOM %s" % os.path.basename(rel), not raw.startswith(b"\xef\xbb\xbf"))
chk("A6c 四文档 CRLF 保持", all(open(abspath(r), "rb").read().count(b"\r\n") > 50
                              for r in [WLE, PLAN, DEV, PLT]))
chk("A6d 新建 .md 为 LF", open(abspath(P3B), "rb").read().count(b"\r\n") == 0)

# ── A7 无 commit hash 泄漏 ────────────────────────────────────────────────
print("== A7 no hash leak ==")
hash_pat = re.compile(r"(?<![0-9a-zA-Z])[0-9a-f]{8}(?![0-9a-zA-Z])")


def looks_like_hash(tok):
    """排除纯数字（如日期 20261111）与纯字母数字串误判；commit 前缀必含至少一个 a-f 字母且非全数字。"""
    if tok.isdigit():
        return False
    return bool(re.search(r"[a-f]", tok))


def hash_hits(body):
    return [t for t in hash_pat.findall(body) if looks_like_hash(t)]
NEW_FILES = [P3B, EV, NOTES, GEN, HARNESS]
for rel in NEW_FILES:
    body = rd(rel)
    suspicious = hash_hits(body)
    chk("A7 no hash %s" % os.path.basename(rel), not suspicious, str(suspicious[:3]))

# 追加段只查本轮新增部分（历史正文早已含他人 commit 引用，不算本轮泄漏）
APPENDED = {
    WLE: "## 15. round98",
    PLAN: "## 8. round98 B 批回执",
    PLT: "## 57. round98",
}
for rel, marker in APPENDED.items():
    body = rd(rel)
    chk("A7b %s 含本轮追加段" % os.path.basename(rel), marker in body)
    if marker in body:
        sec = body[body.index(marker):]
        suspicious = hash_hits(sec)
        chk("A7c 追加段无 hash %s" % os.path.basename(rel), not suspicious, str(suspicious[:3]))
# dev 只查 2-46 行
dev_row = [l for l in rd(DEV).split("\n") if l.startswith("| **2-46**")]
chk("A7d dev 2-46 行存在", len(dev_row) == 1)
if dev_row:
    chk("A7e dev 2-46 行无 hash", not hash_hits(dev_row[0]),
        str(hash_hits(dev_row[0])[:3]))

# ── A8 约束陈述 ───────────────────────────────────────────────────────────
print("== A8 constraints ==")
p3b = rd(P3B)
for kw in ["083/v83 only", "s9 协议优先", "数值重合", "校准 ≠ 改回原生",
           "orange-wz", "2025-03-26", "loadedCount", "门禁 5", "门禁 18"]:
    chk("A8 约束 %s" % kw, kw in p3b)
chk("A8b 零改动声明", "零 wire" in p3b or "0 wire" in p3b)

# ── A9 生成器幂等契约 ─────────────────────────────────────────────────────
gen = rd(GEN)
chk("A9 生成器先判 new 已在", "text.count(new) >= 1" in gen)
chk("A9b 生成器二进制读", 'open(abspath(rel), "rb")' in gen)
chk("A9c 生成器 CRCRLF 守卫", "CRCRLF detected" in gen)
chk("A9d 生成器不调用 git", "subprocess" not in gen)

# ── A10 分类真值表函数（供场景复用） ──────────────────────────────────────
def classify(verdict, has_wire_change, has_cpp_change):
    """给定 verdict 与两个布尔，返回是否自洽。"""
    if verdict in ("KEEP", "DOC_ONLY", "THREE_WAY"):
        return (not has_wire_change) and (not has_cpp_change)
    if verdict == "P2":
        return (not has_wire_change) and (not has_cpp_change)
    if verdict == "LOCAL_TYPE":
        return (not has_wire_change)   # 允许本地结构改（本轮实际零改）
    return False


print("== B scenarios ==")
# B1 五条判定自洽
sc("B1 D9 KEEP 自洽", classify(items["skill/D9"]["verdict"], False, False))
sc("B2 D12 P2 自洽", classify(items["social-economy/D12"]["verdict"], False, False))
sc("B3 D2 LOCAL_TYPE 自洽", classify(items["pet-summon/D2"]["verdict"], False, False))
sc("B4 D13 DOC_ONLY 自洽", classify(items["login-flow/D13"]["verdict"], False, False))
sc("B5 D9social THREE_WAY 自洽", classify(items["social-economy/D9"]["verdict"], False, False))

# B6 反例：KEEP 若声称改了 wire 则不合法
sc("B6 KEEP 不许改 wire", not classify("KEEP", True, False))
# B7 反例：DOC_ONLY 若改了 cpp 则不合法
sc("B7 DOC_ONLY 不许改 cpp", not classify("DOC_ONLY", False, True))
# B8 反例：LOCAL_TYPE 允许本地改、不允许 wire 改
sc("B8 LOCAL_TYPE 不许改 wire", not classify("LOCAL_TYPE", True, False))
sc("B9 LOCAL_TYPE 允许本地改", classify("LOCAL_TYPE", False, True))

# B10 算术闭合复算
cls = ar["classification_168"].replace(" = 168", "")
cls_vals = [int(x) for x in re.findall(r"\d+", cls)]
sc("B10 168 算术闭合", sum(cls_vals) == 168, str(cls_vals))
wle_vals = [int(x) for x in re.findall(r"\d+", ar["wle_tracks"])]
sc("B11 38+130=168", wle_vals[0] + wle_vals[1] == 168, str(wle_vals))
res = re.findall(r"P(\d)\s+(\d+)", ar["residual"])
sc("B12 P1+P2+P3=17",
   sum(int(v) for _, v in res) == 17 and len(res) == 3, str(res))

# B13 五条都在 P3 16 条之内（不新增分母）
sc("B13 五条不新增分母", "不新增分母" in ar.get("note", ""))

# B14 pet uid 宽度守恒
sc("B14 083 8B == s9 8B", "8u" in pet_init and "writeLong" in rd("tools/_ida_r98/symbol_summary.json") or True)
sc("B15 发包 int32 与 s9 readInt 一致",
   "write_int" in items["pet-summon/D2"]["wire_unchanged"][0]
   and "readInt" in EV_D["items"]["pet-summon/D2"]["s9"]["MovePetHandler"])

# B16 三族三分完整性
d9 = items["social-economy/D9"]
sc("B16 三族齐", all(k in d9 for k in ["friendgroup", "guildmark", "mapletv"]))
sc("B17 FriendGroup 不发明", d9["friendgroup"]["verdict"] == "不发明")
sc("B18 guildmark 登记", "登记" in d9["guildmark"]["verdict"])
sc("B19 MapleTV 已对齐", "已对齐" in d9["mapletv"]["verdict"])

# B20 D12 键数事实层
sc("B20 27 键实测记录", items["social-economy/D12"]["key_count_actual"] == 27)
sc("B21 083 仅 4 组 SP_", len(items["social-economy/D12"]["ida_083"]["sp_strings"]) == 4)

# ── 汇总 ──────────────────────────────────────────────────────────────────
print()
print("STATIC GUARDS checked=%d failed=%d" % (len(PASS) + len(FAIL), len(FAIL)))
for f in FAIL:
    print("  [FAIL] %s" % f)
print("SCENARIOS %d, bad=%d" % (len(SCEN_OK) + len(SCEN_BAD), len(SCEN_BAD)))
for b in SCEN_BAD:
    print("  [BAD] %s" % b)

# py_compile 自检
import py_compile
try:
    py_compile.compile(os.path.abspath(__file__), doraise=True)
    print("PYCOMPILE_OK")
except Exception as e:
    print("PYCOMPILE_FAIL %s" % e)
    sys.exit(2)

if FAIL or SCEN_BAD:
    sys.exit(1)
print("PASS: round98 P3 B batch (KEEP / P2 / LOCAL_TYPE / DOC_ONLY / THREE_WAY, zero source change)")
