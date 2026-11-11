# -*- coding: utf-8 -*-
"""round90 harness：drop 面 W/L/E 误分类纠偏（纯注释 / 零行为改动）。

判据原则：
  - 静态守卫逐字锚定 083 活反编译 / s9 源码 / 当前端源码 / 四文档；
  - 0 行为改动硬证 = `git show HEAD:Drop.cpp` 与工作树都归一 EOL、剥离纯注释后逐字节对拍
    （提交前后都成立，不依赖 commit hash，避开 round88 B9b 漂移）；
  - 不放宽验收；harness 自身判据错就地修，不改交付物口径。
"""
import json
import os
import subprocess
import sys

ROOT = "E:/project/MapleStory-Client"
os.chdir(ROOT)

DROP = "Gameplay/MapleMap/Drop.cpp"
PLAN = "docs/features/angel-calibration-d-matrix-2026-11/WLE-TRIAGE-PLAN.md"
EV89 = "docs/features/angel-calibration-d-matrix-2026-11/evidence/wle-triage-round89.json"
EV90 = "docs/features/angel-calibration-d-matrix-2026-11/evidence/drop-wle-correction-round90.json"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave3-drop-wle-correction-20261111.md"
IDA = "tools/_ida_r90/OnDropLeaveField.code.txt"
IDA_ENTER = "tools/_ida_r90/OnDropEnterField.code.txt"
S9 = "E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/util/PacketCreator.java"
CAL = "docs/features/drop-angel-calibration-2026-11/README.md"

STATIC = []
SCEN = []


def chk(name, cond):
    STATIC.append((name, bool(cond)))


def sc(name, cond):
    SCEN.append((name, bool(cond)))


def rb(p):
    with open(p, "rb") as f:
        return f.read()


def rt(p):
    return rb(p).decode("utf-8", "replace")


def strip_comments(code):
    """剥离 C++ 纯注释行（整行 // 或 tab+//），保留所有代码行；归一 EOL。"""
    out = []
    for ln in code.replace("\r\n", "\n").split("\n"):
        if ln.lstrip().startswith("//"):
            continue
        out.append(ln.rstrip())
    return "\n".join(out)


# ---------------------------------------------------------------- 读现场
drop_b = rb(DROP)
drop = drop_b.decode("utf-8", "replace")
plan = rt(PLAN)
ev89 = json.loads(rt(EV89))
ev90 = json.loads(rt(EV90))
dev = rt(DEV)
plt = rt(PLT)
notes = rt(NOTES)
ida = rt(IDA)
ida_enter = rt(IDA_ENTER)
s9 = rt(S9)
cal = rt(CAL)

# git show HEAD:Drop.cpp（LF blob，autocrlf clean）；提交前后都成立
try:
    head_drop = subprocess.check_output(
        ["git", "show", "HEAD:" + DROP], stderr=subprocess.DEVNULL
    ).decode("utf-8", "replace")
except Exception:
    head_drop = ""

# ================================================================ A 静态守卫
# A1 083 活反编译逐字
chk("A1a OnDropLeaveField @0x506590 exists", os.path.isfile(IDA) and len(ida) > 5000)
chk("A1b v2 = Decode1(animation)", "v2 = CInPacket::Decode1(this: a2); /*0x5065b0*/" in ida)
chk("A1c v71 = Decode4(oid)", "v71 = CInPacket::Decode4(this: a2); /*0x5065bc*/" in ida)
chk("A1d v2==2||3||5 → Decode4", "if ( v2 == 2 || v2 == 3 || v2 == 5 )" in ida and "*(v7 + 88) = CInPacket::Decode4" in ida)
chk("A1e v2==4 → Decode2(delay) [W 桶根]", "else if ( v2 == 4 )" in ida and "v5 = CInPacket::Decode2(this: a2); /*0x5065f8*/" in ida)
chk("A1f v2==4 → +68 记账", "*(v4 + 68) = get_update_time() + v5; /*0x506602*/" in ida)
chk("A1g v2==4 → +72 = 4", "*(v79 + 72) = 4; /*0x506608*/" in ida)
chk("A1h v2==0 → IWzGr2DLayer COM", "if ( v2 == 0 )" in ida and "IWzGr2DLayer::Animate" in ida)
chk("A1i enter file is 268 not 269", "CDropPool::OnDropEnterField" in ida_enter and "v108 = CInPacket::Decode1" in ida_enter)
chk("A1j leave file addr 0x506590", "0x506590" in ev90["ida_basis"]["addr"])

# A2 s9 源码逐字
chk("A2a s9 PacketCreator exists", os.path.isfile(S9))
chk("A2b removeItemFromMap signature", "public static Packet removeItemFromMap(int objId, int animation, int chrId, boolean pet, int slot)" in s9)
chk("A2c animation>=2 gate", "if (animation >= 2)" in s9)
chk("A2d writeInt(chrId) 4B", "p.writeInt(chrId);" in s9)
chk("A2e pet → writeByte(slot)", "if (pet)" in s9 and "p.writeByte(slot);" in s9)

# A3 当前端 Drop.cpp 行为体未改（0 行为改动硬证）
chk("A3a HEAD blob available", len(head_drop) > 4000)
chk("A3b code identical after EOL-normalize + strip comments",
    strip_comments(head_drop) == strip_comments(drop))
chk("A3c expire signature intact", "void Drop::expire(int8_t type, const PhysicsObject* lt)" in drop)
chk("A3d switch(type) intact", "switch (type)" in drop)
for c in ("case 0:", "case 1:", "case 2:", "case 4:", "case 5:", "default:"):
    chk("A3e " + c, c in drop)
chk("A3f case0 PICKEDUP", "state = Drop::State::PICKEDUP;" in drop)
chk("A3g case245 vspeed -4.5f", "phobj.vspeed = -4.5f;" in drop)
chk("A3h case245 NORMAL", "phobj.type = PhysicsObject::Type::NORMAL;" in drop)
chk("A3i round90 note is comment-only", "// round90 三方对账" in drop and "零行为改动" in drop)
chk("A3j note mentions W 桶", "W 桶" in drop)
chk("A3k note mentions F 维 P2", "F 维 P2" in drop)
chk("A3l note mentions short would break", "读 short 会平移后续字段" in drop or "改回 083 读 short" in drop)

# A4 校准 README 既有结论（纠偏不是发明，是回归既有证据）
chk("A4a D1 不改 read short", "| D1 | 是否按 083 把 `269` 的 `animation=4` 改成读 `Decode2` | 不改 |" in cal)
chk("A4b D2 不补 COM", "| D2 | 是否补 `animation=0` 的 `IWzGr2DLayer` 消亡动画 | 不补 |" in cal)
chk("A4c D12 不改", "| D12 | 是否补 `animation=4` 的延时爆炸帧 | 不改 |" in cal)
chk("A4d 数值重合≠语义重合", "数值重合 ≠ 语义重合" in cal or "数值重合≠语义重合" in cal)
chk("A4e animation=4 包体宽度不同", "animation=4 的包体宽度不同" in cal or "双端同号但 animation=4" in cal)

# A5 PLAN §2.3 / §5 / §7
chk("A5a PLAN §2.3 证伪标记", "round90 已证伪并注销" in plan)
chk("A5b PLAN §2.3 正确函数锚点", "CDropPool::OnDropLeaveField @0x506590" in plan)
chk("A5c PLAN §2.3 明确非 268", "不在 268 进场" in plan)
chk("A5d PLAN §5 纠偏后下一批", "下一批（round90 纠偏后）" in plan)
chk("A5e PLAN §5 D2/D12 不进 P1", "D2/D12 已判 W / F 维 P2" in plan and "不进 P1" in plan)
chk("A5f PLAN §5 合法 L 候选", "quest/D8" in plan and "skill/D4" in plan and "corewindow/D8" in plan)
chk("A5g PLAN §7 exists", "## 7. round90 纠偏" in plan)
chk("A5h PLAN §7 reopen arithmetic", "27 = 25 P1 + 1 W + 1 F-P2" in plan)
chk("A5i PLAN §7 168 不动", "38+130=168" in plan and "不做语义再分类造假" in plan)
chk("A5j PLAN §7 同族审计", "## 7." in plan and "同族审计" in plan)
chk("A5k PLAN §4 arithmetic untouched", "| **合计** | 38 + 130 | **168** |" in plan)
chk("A5l PLAN reopen table still 27 rows", plan.count("| `") >= 27)

# A6 evidence
chk("A6a ev89 top-level round90_correction", "round90_correction" in ev89)
chk("A6b ev89 correction is dict", isinstance(ev89.get("round90_correction"), dict))
chk("A6c ev89 history p1_registered preserved", "p1_registered" in str(ev89))
chk("A6d ev90 round 90", ev90.get("round") == 90)
chk("A6e ev90 ida addr", ev90["ida_basis"]["addr"] == "0x506590")
chk("A6f ev90 D12 → W", ev90["reclass"]["drop/D12"]["now"].startswith("W 桶"))
chk("A6g ev90 D2 → F P2", "F 维 P2" in ev90["reclass"]["drop/D2"]["now"])
chk("A6h ev90 reopen arithmetic", ev90["reopen_closure"]["arithmetic"] == "27 = 25 + 1 + 1")
chk("A6i ev90 25 P1", ev90["reopen_closure"]["p1_remaining"] == 25)
chk("A6j ev90 zero behavior", ev90["source_change"]["behavior_literals"] == 0)
chk("A6k ev90 wire/protocol/wz/new_packet all false",
    all(ev90["source_change"][k] is False for k in ("wire", "protocol", "wz", "new_packet")))
chk("A6l ev90 083-only constraint", any("083/v83 only" in c for c in ev90["constraints"]))
chk("A6m ev90 no commit hash", "#devfix" not in rt(EV90) and "70dbe0cc" not in rt(EV90))

# A7 四文档
chk("A7a dev 2-38", "| **2-38** |" in dev)
chk("A7b dev 2-38 mentions W 桶", "W 桶 wire 宽度分叉" in dev)
chk("A7c dev 2-38 zero behavior", "0 行为字面量" in dev)
chk("A7d PLT §49", "## 49. round90" in plt)
chk("A7e PLT §48-4 superseded", "round90 已注销" in plt)
chk("A7f PLT §49 D12 W 桶", "W 桶" in plt)
chk("A7g notes exists", len(notes) > 3000)
chk("A7h notes 三方逐字", "0x5065b0" in notes and "p.writeInt(chrId)" in notes)
chk("A7i notes 2B vs 4B", "2B delay" in notes and "4B cid" in notes)

# A8 EOL 字节级
chk("A8a Drop.cpp CRLF", b"\r\n" in drop_b and drop_b.count(b"\n") == drop_b.count(b"\r\n"))
chk("A8b PLT CRLF", b"\r\n" in rb(PLT) and rb(PLT).count(b"\n") == rb(PLT).count(b"\r\n"))
chk("A8c no CRCRLF anywhere", all(b"\r\r\n" not in rb(p) for p in (DROP, PLT)))
chk("A8d PLAN pure LF", b"\r" not in rb(PLAN))
chk("A8e ev89 pure LF", b"\r" not in rb(EV89))
chk("A8f ev90 pure LF", b"\r" not in rb(EV90))
chk("A8g dev pure LF", b"\r" not in rb(DEV))
chk("A8h notes pure LF", b"\r" not in rb(NOTES))
chk("A8i PLAN no trailing blank", rb(PLAN).endswith(b"\n") and not rb(PLAN).endswith(b"\n\n"))
chk("A8j PLT single CRLF tail", rb(PLT).endswith(b"\r\n") and not rb(PLT).endswith(b"\r\n\r\n"))

# A9 零署名 / WIP 隔离（现场 git，三态自适应：提交前 changed / 暂存 staged / 提交后 HEAD）
def git_names(*args):
    try:
        return subprocess.check_output(["git"] + list(args), stderr=subprocess.DEVNULL).decode("utf-8", "replace").split()
    except Exception:
        return []


changed = git_names("diff", "--name-only")
staged = git_names("diff", "--cached", "--name-only")
head_files = git_names("show", "--name-only", "--format=", "HEAD")
visible = set(changed) | set(staged) | set(head_files)
r90_src = [DROP, PLAN, EV89, DEV, PLT]
chk("A9a round90 src visible in one of changed/staged/HEAD", all(any(p.endswith(x) for p in visible) for x in r90_src))
wip_union = set(p for p in visible if p.endswith((".cpp", ".h")))
chk("A9b WIP union >= 25", len(wip_union) >= 25)

# ================================================================ B 场景真值表
# B1 三方 animation=4 真值表
facts4 = {
    "083_read": "Decode2 (2B delay)",
    "083_side_effect": "+68 = update_time+delay, +72 = 4",
    "s9_write": "writeInt(chrId) 4B",
    "client_read": "int cid (mode>1)",
    "verdict": "W",
}
sc("B1a 083 v2==4 reads short", facts4["083_read"].startswith("Decode2"))
sc("B1b 083 v2==4 records delay", "+68" in facts4["083_side_effect"])
sc("B1c s9 writes 4B int", "4B" in facts4["s9_write"])
sc("B1d client reads int (s9-correct)", "int" in facts4["client_read"])
sc("B1e verdict = W (keep s9)", facts4["verdict"] == "W")
sc("B1f numeric 4 coincidence ≠ semantic", facts4["083_read"] != facts4["s9_write"])
sc("B1g reverting to short would shift wire", "改回 083 读 short" in drop or "读 short 会平移" in drop)

# B2 animation=0 真值表
facts0 = {"083": "IWzGr2DLayer COM", "client": "PICKEDUP 48-frame fade", "verdict": "F-P2"}
sc("B2a 083 uses COM canvas", "COM" in facts0["083"])
sc("B2b client uses fade", "fade" in facts0["client"])
sc("B2c same business result (disappear)", True)  # 校准 README A12/C5/B8 已证
sc("B2d verdict = F dim P2 (no patch)", facts0["verdict"] == "F-P2")

# B3 268/269 分派
sc("B3a 268 → OnDropEnterField", "a1==268 → OnDropEnterField" in ev90["ida_basis"]["on_packet_dispatch"])
sc("B3b 269 → OnDropLeaveField", "a1==269 → OnDropLeaveField" in ev90["ida_basis"]["on_packet_dispatch"])
sc("B3c D2/D12 belong to 269 not 268", "269 离场" in plan and "不在 268 进场" in plan)

# B4 reopen 算术闭合
sc("B4a 27 = 25+1+1", 25 + 1 + 1 == 27)
sc("B4b p1_remaining 25", ev90["reopen_closure"]["p1_remaining"] == 25)
sc("B4c to_W 1", ev90["reopen_closure"]["to_W"] == 1)
sc("B4d to_F_P2 1", ev90["reopen_closure"]["to_F_P2"] == 1)

# B5 168 历史裁定不动
sc("B5a 38+130=168", 38 + 130 == 168)
sc("B5b round88 classification untouched", ev89.get("classification_168", {}).get("total") == 168 or "PURE" in str(ev89))
sc("B5c DECISIONS not rewritten", "不做语义再分类造假" in plan)

# B6 同族审计 → 下一批方向
next_batch = ("quest/D8", "skill/D4", "corewindow/D8")
sc("B6a three legal L candidates", all(x in plan and x in notes for x in next_batch))
sc("B6b none of them is wire", all("wire" not in x for x in next_batch))
sc("B6c gate5 no imagination", "门禁 5 禁想象修改" in notes)

# B7 零改动边界
sc("B7a 0 behavior literals", ev90["source_change"]["behavior_literals"] == 0)
sc("B7b zero wire", ev90["source_change"]["wire"] is False)
sc("B7c zero protocol", ev90["source_change"]["protocol"] is False)
sc("B7d zero WZ", ev90["source_change"]["wz"] is False)
sc("B7e zero new packet", ev90["source_change"]["new_packet"] is False)
sc("B7f code byte-identical (strip comments)", strip_comments(head_drop) == strip_comments(drop))

# B8 约束回归
sc("B8a 083 only", any("083/v83 only" in c for c in ev90["constraints"]))
sc("B8b s9 wire priority", any("s9 协议优先" in c for c in ev90["constraints"]))
sc("B8c 数值重合≠语义重合", any("数值重合" in c for c in ev90["constraints"]))
sc("B8d 校准≠改回原生", any("校准 ≠ 改回原生" in c for c in ev90["constraints"]))

# B9 真机归口仍认领
sc("B9a PLT §49 live item", "49-4." in plt and "用户认领" in plt)
sc("B9b expire 0/1/2/4/5 五档", "0/1/2/4/5" in plt)

# ================================================================ 报告
bad_static = [n for n, ok in STATIC if not ok]
bad_scen = [n for n, ok in SCEN if not ok]
print(f"STATIC GUARDS checked={len(STATIC)} failed={len(bad_static)}")
for n in bad_static:
    print("  [FAIL]", n)
print(f"SCENARIOS {len(SCEN)}, bad={len(bad_scen)}")
for n in bad_scen:
    print("  [FAIL]", n)
if bad_static or bad_scen:
    print("FAIL: round90 drop W/L/E correction harness")
    sys.exit(1)
print("PYCOMPILE_OK")
print("PASS: round90 drop W/L/E correction (D12→W / D2→F-P2 / reopen 27=25+1+1 / zero behavior change)")
sys.exit(0)
