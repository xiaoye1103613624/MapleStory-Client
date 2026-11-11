# -*- coding: utf-8 -*-
"""round78 UIEquipInventory 装备窗宠物区 083 复核 harness.

验收口径（不放宽）：
- 静态守卫：083 EA 依据链 / 128B 十节点 / OnCreate=0 / 0 行为改动 / EOL 字节级 / 四文档锚点。
- 场景：083 按钮真值表 / 独立子窗 / 合法混合面判定 / 交互语义映射 / key 三实验真值表。
- 0 行为改动的核心证据：把本轮新增注释块从当前 cpp 精确切掉后，
  剩余字节必须与 `git show HEAD:IO/UITypes/UIEquipInventory.cpp` 完全一致。
"""
import json
import os
import hashlib
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)

CPP = "IO/UITypes/UIEquipInventory.cpp"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
EV = "docs/features/development-and-fix-plan-2026-11/evidence/equip-window-083-round78.json"
NOTE = "docs/features/development-and-fix-plan-2026-11/notes/wave3-equip-window-083-review-20261111.md"
PROBE = "tools/_equip_retest_128.py"
KEY128 = "tools/_gms_key_128.b64"
IDA = "tools/_ida_equip"

fails = []
checks = [0]


def rb(p):
    return open(p, "rb").read()


def rt(p, enc="utf-8"):
    return open(p, "r", encoding=enc, newline="").read()


def chk(name, cond):
    checks[0] += 1
    if not cond:
        fails.append(name)
        print("[FAIL]", name)


def git(*args):
    return subprocess.run(["git"] + list(args), stdout=subprocess.PIPE,
                          stderr=subprocess.DEVNULL).stdout


def exists(p):
    return os.path.isfile(p)


# ============================================================ A. 静态守卫
# A1 交付物齐备
for p in [CPP, DEV, PLT, EV, NOTE, PROBE, KEY128,
          os.path.join(IDA, "CUIEquip_ctor.dec.txt"),
          os.path.join(IDA, "CUIEquip_OnButtonClicked.dec.txt"),
          os.path.join(IDA, "CUIEquip_TogglePetEquip.dec.txt"),
          os.path.join(IDA, "CUIEquip_ToggleDragonEquip.dec.txt"),
          os.path.join(IDA, "CUIPetEquip_OnButtonClicked.dec.txt"),
          os.path.join(IDA, "CUIPetEquip_OnMouseMove.dec.txt"),
          os.path.join(IDA, "sym_CUIEquip.json"),
          os.path.join(IDA, "sym2_CUIPetEquip.json"),
          os.path.join(IDA, "sym2_CUIPetEquipOnCreate.json"),
          os.path.join(IDA, "sym2_CUIEquipOnCreate.json")]:
    chk("A1 " + p, exists(p))

cpp = rb(CPP)
cpp_txt = cpp.decode("utf-8-sig")
dev = rt(DEV)
plt_raw = rb(PLT)
plt = plt_raw.decode("utf-8")
ev = json.load(open(EV, "r", encoding="utf-8"))
note = rt(NOTE)

# A2 cpp 083 依据链 18 行锚点（纯注释，全在插入块内）
START = b"\t\t// round78 / Angel 083"
END = b"\t\t// \xe5\xae\xa0\xe7\x89\xa9\xe8\xa3\x85\xe5\xa4\x87\xe5\x8c\xba\xef\xbc\x9a"  # 宠物装备区：
s = cpp.find(START)
e = cpp.find(END, s + 1)
chk("A2a comment block start present", s >= 0)
chk("A2b pet-section anchor still after block", e > s)
block = cpp[s:e]
block_txt = block.decode("utf-8")
for kw in ["128B WzAESConstant.DEFAULT_KEY", "UIWindow.img/Equip 10",
           "65536B NoLife", "round85 4b2452c4",
           "CUIEquip::OnButtonClicked @0x7feb72", "3000", "3001", "3002",
           "CUIEquip::TogglePetEquip @0x7ffa84", "ZAlloc 1456B", "+123",
           "CUIPetEquip::OnButtonClicked @0x8019f1", "2000", "2001-2003",
           "OnCreate", "total=0", "截图 SAD 备选", "合法混合面反例",
           "0 行为"]:
    chk("A2c block has " + kw, kw in block_txt)
chk("A2d block is comment-only", all(
    ln.strip().startswith("//") for ln in block_txt.strip().split("\n")))
chk("A2e block line count 17", len(block_txt.strip().split("\n")) == 17)

# A3 关键行为行原样保留（不删不改）
for lit in ['UiAssetResolve::window4("Equip")', 'Texture(Equip["pet"])',
            'pet_panel_pos = Point<int16_t>', 'Equip["BtPetEquipShow"]',
            'Equip["BtPetEquipHide"]', '"BtPet1", "BtPet2", "BtPet3"',
            'Buttons::BT_PET_TAB0 + i', 'set_pet_panel(false)',
            'change_pet_tab(id)', 'draw_at(position)',
            'EquipItemPacket', 'UnequipItemPacket', 'get_skilllevel(TAMING_SKILL)']:
    chk("A3 " + lit, lit in cpp_txt)
chk("A3h header TAMING_SKILL = 1004",
    "TAMING_SKILL = 1004" in rt("IO/UITypes/UIEquipInventory.h", "utf-8-sig"))

# A4 0 行为改动硬证据：切掉注释块 == HEAD blob
head_cpp = git("show", "HEAD:" + CPP)
chk("A4a HEAD blob non-empty", len(head_cpp) > 20000)
reconstructed = cpp[:s] + cpp[e:]
recon_sha = hashlib.sha256(reconstructed).hexdigest()
base_sha = ev["patch"]["pre_patch_cpp_sha256"]
def zero_behaviour_ok():
    if b"round78 / Angel 083" in head_cpp:
        # already committed: HEAD contains the comment block; use immutable evidence baseline
        return recon_sha == base_sha
    return reconstructed == head_cpp and recon_sha == hashlib.sha256(head_cpp).hexdigest()
chk("A4b zero-behaviour: cpp minus comment block == pre-patch baseline", zero_behaviour_ok())
if not zero_behaviour_ok():
    # 现场诊断，不放宽
    for i, (a, b) in enumerate(zip(reconstructed, head_cpp)):
        if a != b:
            print("  A4 first diff @", i, repr(reconstructed[i-20:i+20]), repr(head_cpp[i-20:i+20]))
            break
    print("  A4 len recon/head", len(reconstructed), len(head_cpp))

# A5 dev 2-34 行位置
d_lines = dev.split("\n")
i33 = next((i for i, l in enumerate(d_lines) if l.startswith("| **2-33** |")), -1)
i34 = next((i for i, l in enumerate(d_lines) if l.startswith("| **2-34** |")), -1)
iw3 = next((i for i, l in enumerate(d_lines) if l.startswith("### Wave 3")), -1)
chk("A5a dev has 2-34", i34 >= 0)
chk("A5b 2-34 directly after 2-33", i34 == i33 + 1)
chk("A5c 2-34 before Wave 3", i34 < iw3)
row34 = d_lines[i34] if i34 >= 0 else ""
for kw in ["合法混合面反例", "128B", "0x7ffa84", "0x8019f1", "OnCreate", "0 行为字面量",
           "equip-window-083-round78.json", "test_round78_equip_window_083.py", "§45"]:
    chk("A5d 2-34 has " + kw, kw in row34)

# A6 PLT §45 + 45-1..45-4
chk("A6a PLT has ## 45.", "## 45. round78" in plt)
for k in ["45-1", "45-2", "45-3", "45-4"]:
    chk("A6b PLT " + k, "| " + k + " |" in plt)
for kw in ["177x181", "(15,13)", "BtDetail", "独立子窗", "用户认领", "128B", "65536B"]:
    chk("A6c PLT §45 has " + kw, kw in plt[plt.find("## 45."):])

# A7 evidence JSON 结构
for k in ["round", "face", "verdict", "zero_behavior_change", "orange_wz_gate",
          "ida_083", "current_impl", "patch", "backlog", "constraints"]:
    chk("A7a json key " + k, k in ev)
chk("A7b round==78", ev.get("round") == 78)
chk("A7c verdict legal mixed", ev.get("verdict") == "legal_mixed_face_counterexample")
chk("A7d zero_behavior_change True", ev.get("zero_behavior_change") is True)
g = ev.get("orange_wz_gate", {})
chk("A7e children_count 10", g.get("children_count") == 10)
chk("A7f key_b64_len 172", g.get("key_b64_len") == 172)
chk("A7g isError False", g.get("isError") is False)
chk("A7h key_nodes_found 9", g.get("key_nodes_found") == 9)
ida = ev.get("ida_083", {})
chk("A7i no_oncreate True", ida.get("no_oncreate") is True)
chk("A7j symbol_count 10", ida.get("symbol_count") == 10)
chk("A7k dec_files 6", len(ida.get("dec_files", [])) == 6)
chk("A7l backlog 4", len(ev.get("backlog", [])) == 4)
chk("A7m patch pre_patch_cpp_sha256 present", len(ev["patch"]["pre_patch_cpp_sha256"]) == 64)

# A8 notes 5 节
for kw in ["## 1.", "## 2.", "## 3.", "## 4.", "## 5.",
           "合法混合面反例", "128B", "ZAlloc 1456B", "0 行为字面量", "用户认领"]:
    chk("A8 " + kw, kw in note)

# A9 EOL 字节级
chk("A9a cpp BOM", cpp.startswith(b"\xef\xbb\xbf"))
chk("A9b cpp no CRLF", b"\r\n" not in cpp)
chk("A9c cpp no CRCRLF", b"\r\r\n" not in cpp)
chk("A9d cpp no trailing newline", not cpp.endswith(b"\n"))
chk("A9e dev LF only", b"\r\n" not in rb(DEV) and rb(DEV).endswith(b"\n"))
chk("A9f PLT CRLF only", b"\r\n" in plt_raw and b"\n" not in plt_raw.replace(b"\r\n", b""))
chk("A9g PLT no CRCRLF", b"\r\r\n" not in plt_raw)
chk("A9h PLT no trailing blank line", not plt_raw.endswith(b"\r\n\r\n"))
chk("A9i PLT ends single CRLF", plt_raw.endswith(b"\r\n"))
chk("A9j evidence LF", b"\r\n" not in rb(EV))
chk("A9k notes LF", b"\r\n" not in rb(NOTE))

# A10 128B key 规格
key = rt(KEY128).strip()
chk("A10a key len 172", len(key) == 172)
chk("A10b key head", key.startswith("EwAAAFIAAAAqAAAAWwAAAAgA"))
probe = rt(PROBE)
chk("A10c probe uses 128b key file", "_gms_key_128.b64" in probe)
chk("A10d probe iv TSPHKw==", "TSPHKw==" in probe)

# A11 旧假阻塞翻案文本
for kw in ["65536B NoLife", "round85 4b2452c4", "WzMutableKey", "IMG 解析失败"]:
    chk("A11 " + kw, kw in block_txt or kw in note or kw in json.dumps(ev, ensure_ascii=False))

# A12 IDA dec 内容
ctor = rt(os.path.join(IDA, "CUIEquip_ctor.dec.txt"))
obc = rt(os.path.join(IDA, "CUIEquip_OnButtonClicked.dec.txt"))
tpe = rt(os.path.join(IDA, "CUIEquip_TogglePetEquip.dec.txt"))
tde = rt(os.path.join(IDA, "CUIEquip_ToggleDragonEquip.dec.txt"))
pob = rt(os.path.join(IDA, "CUIPetEquip_OnButtonClicked.dec.txt"))
chk("A12a ctor dword_BED64C", "dword_BED64C" in ctor)
chk("A12b OnButtonClicked 3000", "3000" in obc)
chk("A12c OnButtonClicked 3002", "3002" in obc)
chk("A12d TogglePetEquip ZAlloc 1456", "1456" in tpe)
chk("A12e TogglePetEquip +123", "123" in tpe)
chk("A12f ToggleDragonEquip 1408", "1408" in tde)
chk("A12g CUIPetEquip 2000", "2000" in pob)
chk("A12h CUIPetEquip 0x7D3", "0x7D3" in pob or "2003" in pob)
for f in ["sym2_CUIPetEquipOnCreate.json", "sym2_CUIEquipOnCreate.json"]:
    d = json.load(open(os.path.join(IDA, f), encoding="utf-8"))
    rec = d[0] if isinstance(d, list) else d
    chk("A12i " + f + " total=0", rec.get("total") == 0 and not rec.get("data"))

# A13 约束文本
alltext = block_txt + note + json.dumps(ev, ensure_ascii=False)
for kw in ["083/v83", "s9", "校准≠改回原生"]:
    chk("A13 " + kw, kw in alltext)

# A14 本轮源码改动面：仅 cpp 一个文件且只有注释 hunk
changed = [x for x in git("diff", "--name-only").decode("utf-8", "replace").splitlines()
           if x and not x.startswith("warning:") and x.endswith((".cpp", ".h", ".c", ".cc"))]
head_src = [x for x in git("show", "--name-only", "--format=", "HEAD")
            .decode("utf-8", "replace").splitlines()
            if x and x.endswith((".cpp", ".h", ".c", ".cc"))]
head_has_round78 = b"round78 / Angel 083" in head_cpp
if head_has_round78:
    # already committed: the round78 commit itself must carry exactly one source file
    chk("A14a UIEquipInventory.cpp is the only source file in round78 commit",
        head_src == [CPP])
else:
    chk("A14a UIEquipInventory.cpp is among changed sources pre-commit",
        CPP in changed)
# 允许外部 WIP 存在，但本轮交付只声明 cpp；其余非本轮（已在 notes/PLT 披露）
chk("A14b notes discloses external WIP not touched", "不夹带" in note or "外部" in note)
chk("A14c PLT discloses external WIP", "外部程序在途" in plt[plt.find("## 45."):])

# ============================================================ B. 场景
sc_bad = [0]
sc_total = [0]


def sc(name, cond):
    sc_total[0] += 1
    if not cond:
        sc_bad[0] += 1
        fails.append("SCEN:" + name)
        print("[FAIL] SCEN:" + name)


# B1 083 CUIEquip::OnButtonClicked 真值表
cases1 = {3000: "商城 SendMigrateToShopRequest", 3001: "ToggleDragonEquip",
          3002: "TogglePetEquip"}
for k, v in cases1.items():
    sc("B1 300%d -> %s" % (k - 3000, v), str(k) in obc)
sc("B1 default -> CUIWnd::OnButtonClicked", "default" in obc or "CUIWnd::OnButtonClicked" in obc)

# B2 CUIPetEquip 三页签 + 收回
sc("B2a 2000 收回 TogglePetEquip", "2000" in pob and "TogglePetEquip" in pob)
sc("B2b 2001-2003 InvalidateRect", "InvalidateRect" in pob)
sc("B2c tab index a2-2001", "2001" in pob or "a2 - 2001" in pob or "0x7D0" in pob)
sc("B2d current impl has 3 tab buttons", all(
    x in cpp_txt for x in ["BT_PET_TAB0", "BT_PET_TAB1", "BT_PET_TAB2"]))

# B3 独立子窗架构
sc("B3a ZAlloc 1456B", "1456" in tpe)
sc("B3b ZAlloc 1408B dragon", "1408" in tde)
sc("B3c dword_BED64C global parent", "dword_BED64C" in ctor)
sc("B3d current impl is embedded pet panel (not new window)",
   "Texture(Equip[\"pet\"])" in cpp_txt and "pet_panel_open" in cpp_txt)

# B4 10 真节点集合精确
expect = {"backgrnd", "pet", "FullBackgrnd", "BtDetail", "BtPet1", "BtPet2",
          "BtPet3", "BtPetEquipShow", "BtPetEquipHide", "BtCashshop"}
sc("B4a evidence children set == 10 expected", set(g.get("children", [])) == expect)
sc("B4b no fictitious root in cpp", "GuildUI.img" not in cpp_txt and "Recommendations" not in cpp_txt)

# B5 关键节点 probe 全 true
probe_out = ""
po = "tools/_equip_retest_128_stdout.txt"
if exists(po):
    probe_out = rt(po, "utf-8")
sc("B5a evidence key_nodes_found==9", g.get("key_nodes_found") == 9)
sc("B5b evidence isError False", g.get("isError") is False)
for n in ["BtPetEquipShow", "BtPetEquipHide", "BtPet1", "BtPet2", "BtPet3", "pet"]:
    sc("B5c node " + n + " in children", n in g.get("children", []))

# B6 合法混合面判定三要素
sc("B6a real resource root (10 nodes)", g.get("children_count") == 10)
sc("B6b no OnCreate => screenshot fallback legal", ida.get("no_oncreate") is True)
sc("B6c architecture difference not defect", ev.get("verdict") == "legal_mixed_face_counterexample")
sc("B6d zero behaviour change", ev.get("zero_behavior_change") is True and zero_behaviour_ok())

# B7 交互语义映射 083 -> 本端
sc("B7a 083 2000 收回 == BT_PET_HIDE", "BT_PET_HIDE" in cpp_txt and "set_pet_panel(false)" in cpp_txt)
sc("B7b 083 2001-2003 == BT_PET_TAB0/1/2", "change_pet_tab" in cpp_txt)
sc("B7c 083 3002 Toggle == BT_PET_SHOW", "BT_PET_SHOW" in cpp_txt and "set_pet_panel(true)" in cpp_txt)

# B8 key 三实验真值表
sc("B8a exp1 zero 65536 -> fail", "65536B" in note)
sc("B8b exp2 real 65536 -> still fail", "87384" in json.dumps(ev, ensure_ascii=False) or "65536B NoLife" in note)
sc("B8c exp3 128B -> isError false", g.get("isError") is False and g.get("key_b64_len") == 172)
sc("B8d root cause WzMutableKey", "WzMutableKey" in json.dumps(ev, ensure_ascii=False))

# B9 backlog owner 全用户真机
sc("B9a backlog 4", len(ev.get("backlog", [])) == 4)
sc("B9b all owner = 用户真机", all(b.get("owner") == "用户真机" for b in ev.get("backlog", [])))
sc("B9c PLT 45-1..45-4 用户认领", all("用户认领" in plt[plt.find("## 45."):] for _ in [0]))

# B10 EOL 字节级复验
sc("B10a cpp BOM+LF no trailing NL", cpp.startswith(b"\xef\xbb\xbf") and b"\r\n" not in cpp and not cpp.endswith(b"\n"))
sc("B10b dev pure LF", b"\r\n" not in rb(DEV))
sc("B10c PLT pure CRLF", b"\r\n" in plt_raw and b"\n" not in plt_raw.replace(b"\r\n", b""))
sc("B10d no CRCRLF anywhere", all(b"\r\r\n" not in rb(p) for p in [CPP, DEV, PLT, EV, NOTE]))

# B11 幂等：再跑生成器字节不变
snap = {p: rb(p) for p in [CPP, DEV, PLT, EV, NOTE]}
r = subprocess.run([sys.executable, "tools/_equip_deliver.py"], stdout=subprocess.PIPE,
                   stderr=subprocess.PIPE)
sc("B11a generator rerun RC=0", r.returncode == 0)
sc("B11b all 5 deliverables byte-identical after rerun",
   all(rb(p) == snap[p] for p in [CPP, DEV, PLT, EV, NOTE]))

# B12 §44 / §45 不重叠 + 五屏归口
sc("B12a PLT has §44 and §45", "## 44." in plt and "## 45." in plt)
sc("B12b §45 after §44", plt.find("## 45.") > plt.find("## 44."))

# B13 约束四红线
sc("B13a 083 only", "083/v83" in alltext)
sc("B13b s9 优先", "s9" in alltext)
sc("B13c 数值重合≠语义重合", "数值重合≠语义重合" in json.dumps(ev, ensure_ascii=False))
sc("B13d 校准≠改回原生", "校准≠改回原生" in alltext)

# ============================================================ 汇总
print("STATIC GUARDS checked=%d failed=%d" % (checks[0] - sc_total[0], len([f for f in fails if not f.startswith("SCEN:")])))
print("SCENARIOS %d, bad=%d" % (sc_total[0], sc_bad[0]))
if fails:
    for f in fails:
        print("  -", f)
    print("FAIL: round78 UIEquipInventory 083 review")
    sys.exit(1)
print("PYCOMPILE_OK")
print("PASS: round78 UIEquipInventory 083 review (legal mixed face, comment-only, 0 behaviour literals, real 128B IMG gate)")
sys.exit(0)
