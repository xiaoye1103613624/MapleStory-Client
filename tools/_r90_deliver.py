# -*- coding: utf-8 -*-
"""round90 幂等生成器：drop 面 W/L/E 误分类纠偏 + 零行为注释 + 四文档。

触发：m10601「还有未完成的或者你可以自测的任务吗？」→ 按 m09211/m09729/m09737
「全部继续 / 按最完全反编译还原基础功能 / 其余同方向」授权，自测推进下一批时
先复核 round89 §2.3/§5 的 P1 登记，发现 drop/D2、drop/D12 两处误分类。

定性（三方对账，零源码行为改动）：
  D12 = W 桶 wire 宽度分叉（083 2B delay vs s9 4B cid，改回读 short 断联机）
  D2  = F 维 P2（IWzGr2DLayer COM 画布栈当前端没有，48 帧淡出语义等价）
  两者均非 L 桶 → 从 L.p1_registered 注销；168 条历史裁定与 §4 算术不动。

约束：不写 git log / commit hash 进产物（round88 B9b 教训）；EOL 字节级保持；
零署名精确 add；25 个外部在途 WIP 零夹带。
"""
import json
import os

ROOT = "E:/project/MapleStory-Client"


def rb(p):
    with open(os.path.join(ROOT, p), "rb") as f:
        return f.read()


def wb(p, b):
    with open(os.path.join(ROOT, p), "wb") as f:
        f.write(b)


def must(cond, msg):
    if not cond:
        raise AssertionError(msg)


# ---------------------------------------------------------------- 证据常量
IDA_LEAVE = "tools/_ida_r90/OnDropLeaveField.code.txt"
IDA_ENTER = "tools/_ida_r90/OnDropEnterField.code.txt"
S9 = "E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/util/PacketCreator.java"
DROP_CAL = "docs/features/drop-angel-calibration-2026-11/README.md"

IDA_VERBATIM = (
    "v2 = CInPacket::Decode1(this: a2); /*0x5065b0*/ "
    "v71 = CInPacket::Decode4(this: a2); /*0x5065bc*/ "
    "if ( v2 == 2 || v2 == 3 || v2 == 5 ) { *(v7 + 88) = CInPacket::Decode4(this: a2); /*0x506627*/ } "
    "else if ( v2 == 4 ) { /*0x5065eb*/ v5 = CInPacket::Decode2(this: a2); /*0x5065f8*/ "
    "*(v4 + 68) = get_update_time() + v5; /*0x506602*/ *(v79 + 72) = 4; /*0x506608*/ } "
    "if ( v2 == 0 ) { ... IWzGr2DLayer::GetCanvas / RemoveCanvas / Animate ... }"
)

CORRECTION = {
    "round": 90,
    "date": "2026-11-11",
    "trigger": "m10601 还有未完成的或者你可以自测的任务吗？（m09211/m09729/m09737 授权全量同方向自测）",
    "verdict": (
        "round89 WLE-TRIAGE-PLAN.md §2.3/§5 与 evidence/wle-triage-round89.json 的 "
        "L.p1_registered 把 drop/D2、drop/D12 登记为「按 083 反编译还原的 L 桶下一批」，"
        "且把函数锚点写成 268 进场 OnDropEnterField @0x505900；三方对账证伪："
        "两条决策点都在 269 离场 OnDropLeaveField @0x506590，且一条是 wire 宽度分叉、"
        "一条是 COM 渲染栈资源缺口，均非 L 桶纯逻辑。"
    ),
    "ida_basis": {
        "endpoint": "http://127.0.0.1:13339/mcp",
        "idb": "E:/MXD/反编译IDA/v83/Angel.i64",
        "func": "CDropPool::OnDropLeaveField",
        "addr": "0x506590",
        "lines": "83-99",
        "file": IDA_LEAVE,
        "enter_func_not_applicable": "CDropPool::OnDropEnterField @0x505900（268 进场，非 269 离场）",
        "enter_file": IDA_ENTER,
        "verbatim": IDA_VERBATIM,
        "on_packet_dispatch": "CDropPool::OnPacket @0x5058db：a1==268 → OnDropEnterField；a1==269 → OnDropLeaveField",
    },
    "s9_basis": {
        "file": "PacketCreator.java:2624-2642",
        "abs": S9,
        "verbatim": "public static Packet removeItemFromMap(int objId, int animation, int chrId, boolean pet, int slot)",
        "wire": "if (animation >= 2) { p.writeInt(chrId); if (pet) { p.writeByte(slot); } }",
        "note": "animation>=2 恒写 4B cid；083 仅 v2==4 读 2B delay → 数值同为 4、语义不同",
    },
    "current_client": {
        "file": "Gameplay/MapleMap/Drop.cpp:119-144",
        "func": "void Drop::expire(int8_t type, const PhysicsObject* lt)",
        "case4": "case 2 / case 4 / case 5：angle.set(0) + state=PICKEDUP + looter=lt + phobj.vspeed=-4.5f + type=NORMAL",
        "case0": "case 0：state=PICKEDUP（48 帧淡出）",
        "handler": "Net/Handlers/MapObjectHandlers.cpp RemoveLootHandler：mode>1 读 int cid（s9 正确解）",
        "verdict": "按 s9 读 int cid 正确，不改 short；零 wire/协议/WZ/新封包改动",
    },
    "reclass": {
        "drop/D12": {
            "was": "L.p1_registered（animation=4 延时爆炸帧，按 083 还原）",
            "now": "W 桶（wire 宽度分叉）",
            "why": "083 v2==4 读 Decode2 delay + 写 +68/+72 记账；s9 写 4B cid。改回 083 读 short 会平移后续字段、复现 LoginParser.cpp:150-174 白屏类错位。W 桶保持 s9。",
            "calibration": "drop-angel-calibration-2026-11/README.md:11/73/81/119/243-244/266/277（D1 不改、D12 不改）",
        },
        "drop/D2": {
            "was": "L.p1_registered（animation=0 消亡动画，按 083 还原）",
            "now": "F 维 P2（资源/渲染栈缺口，非逻辑）",
            "why": "083 v2==0 走 IWzGr2DLayer::Animate/RemoveCanvas/GetCanvas COM 画布栈；当前端无该栈，48 帧淡出业务结果（消失）一致。校准≠改回原生，不补。",
            "calibration": "drop-angel-calibration-2026-11/README.md:82/107/108/260/267（A12/C5/D2 不补、L3 遗留）",
        },
    },
    "reopen_closure": {
        "round89_reopen": 27,
        "p1_remaining": 25,
        "to_W": 1,
        "to_F_P2": 1,
        "arithmetic": "27 = 25 + 1 + 1",
        "note": "168 条 DECISIONS 历史裁定与 WLE-TRIAGE-PLAN.md §4 的 38+130=168 算术不动；仅在 P1 登记层注销 2 条，不做语义再分类造假。",
    },
    "same_family_audit": {
        "checked": "WLE-TRIAGE-PLAN.md §2.3 其余 P1 候选（quest/D8、skill/D4、corewindow/D8）逐条核",
        "result": "状态机/条件分支、不碰 wire，属合法 L 桶候选，无同族误分类",
        "next_batch": "改走 quest/D8 / skill/D4 / corewindow/D8，每批先出 083 符号+反编译锚点再落，门禁 5 禁想象修改",
    },
    "source_change": {
        "behavior_literals": 0,
        "wire": False,
        "protocol": False,
        "wz": False,
        "new_packet": False,
        "drop_cpp": "仅 expire() 前补 083/s9 三方对账纯注释（CRLF 字节级保持）",
    },
    "constraints": [
        "083/v83 only 禁 095",
        "s9 协议优先（W 桶 6 字段宽度/类型 + 27 条 wire KEEP 零改动）",
        "数值重合 ≠ 语义重合",
        "校准 ≠ 改回原生（L 桶同字段原始逻辑按 Angel 083 还原）",
        "门禁 5 禁想象修改 / 门禁 18 REA→IDA→orange-wz→.nx 只读取证",
    ],
}

# ---------------------------------------------------------------- 1 PLAN (LF)
PLAN = "docs/features/angel-calibration-d-matrix-2026-11/WLE-TRIAGE-PLAN.md"
plan_b = rb(PLAN)
plan = plan_b.decode("utf-8")
must("\r" not in plan, "PLAN must be LF")

OLD23 = (
    "- `drop/D2`（animation=0 消亡动画）+ `drop/D12`（animation=4 延时爆炸帧）："
    "`CDropPool::OnDropEnterField @0x505900` 反编译逐字已落 `tools/_ida_r89/`，"
    "分支含 `MakeMoneyAnimation` / `*(v115+18)=3`，按 L 桶下一批还原。"
)
NEW23 = (
    "- ~~`drop/D2` / `drop/D12` 按 L 桶下一批还原~~ **round90 已证伪并注销（见 §7）**："
    "两条决策点都在 **269 离场** `CDropPool::OnDropLeaveField @0x506590`，"
    "不在 268 进场 `OnDropEnterField @0x505900`；逐字 `tools/_ida_r90/OnDropLeaveField.code.txt:83-99` "
    "= `v2=Decode1(animation)` → `Decode4(oid)` → `if(v2==2||3||5) Decode4` / "
    "`else if(v2==4){ Decode2(delay); *(v4+68)=get_update_time()+delay; *(v79+72)=4; }` / "
    "`v2==0` 走 `IWzGr2DLayer` COM 消亡动画。s9 `PacketCreator.java:2624-2642` 对 `animation>=2` "
    "恒写 `writeInt(chrId)` 4B ⇒ **D12 = W 桶 wire 宽度分叉**（083 2B delay vs s9 4B cid，"
    "数值重合≠语义重合，改回读 short 会平移字段、断联机）、**D2 = F 维 P2**"
    "（COM 画布栈当前端没有，48 帧淡出语义等价）。两者均非 L 桶，从 `L.p1_registered` 注销；"
    "168 条历史裁定与 §4 算术不动。"
)
must(OLD23 in plan or "round90 已证伪并注销" in plan, "PLAN §2.3 anchor missing")
if OLD23 in plan:
    plan = plan.replace(OLD23, NEW23, 1)

OLD5 = (
    "2. **下一批（drop 面）**：`CDropPool::OnDropEnterField` 的 animation 分支按 083 还原，"
    "REA→IDA→orange-wz(128B)→.nx 只读取证 → 纯逻辑补丁 → harness → GATE → 真重编 → 零署名提交。"
)
NEW5 = (
    "2. **下一批（round90 纠偏后）**：drop 面 D2/D12 已判 W / F 维 P2、**不进 P1**；"
    "改走真正有 083 状态机证据且零 wire 的 L 桶候选（`quest/D8` / `skill/D4` / `corewindow/D8`），"
    "每批先出 083 符号+反编译锚点，证据不足登记 P2/P3 不发明，"
    "再 REA→IDA→orange-wz(128B)→.nx 只读取证 → 纯逻辑补丁 → harness → GATE → 真重编 → 零署名提交。"
)
must(OLD5 in plan or "下一批（round90 纠偏后）" in plan, "PLAN §5 anchor missing")
if OLD5 in plan:
    plan = plan.replace(OLD5, NEW5, 1)

SEC7 = """

## 7. round90 纠偏 — drop 面三方对账（m10601）

触发：用户「还有未完成的或者你可以自测的任务吗？」→ 按 m09211/m09729/m09737
「全部继续 / 按最完全反编译还原基础功能 / 其余同方向」授权，自测推进下一批时
先复核 §2.3/§5 的 P1 登记，发现两处误分类。**先把上一轮自己埋的错挖出来，再开工。**

| 项 | round89 登记 | round90 判定 | 决定性证据 |
| --- | --- | --- | --- |
| `drop/D12` animation=4 | L 桶 P1（按 083 还原延时爆炸帧） | **W 桶**（wire 宽度分叉，保持 s9） | 083 `OnDropLeaveField @0x506590:94-99` `Decode2(delay)`+`*(v4+68)`；s9 `removeItemFromMap` `animation>=2` 恒 `writeInt(chrId)` 4B |
| `drop/D2` animation=0 | L 桶 P1（按 083 还原消亡动画） | **F 维 P2**（渲染栈缺口，不补） | 083 `v2==0` 走 `IWzGr2DLayer::Animate/RemoveCanvas/GetCanvas`；当前端无 COM 画布栈，48 帧淡出业务结果一致 |
| 函数锚点 | `OnDropEnterField @0x505900`（268 进场） | `OnDropLeaveField @0x506590`（269 离场） | `CDropPool::OnPacket @0x5058db` 分派 268/269；反编译落 `tools/_ida_r90/` |

**reopen 27 闭合**：`27 = 25 P1 + 1 W + 1 F-P2`。168 条 `DECISIONS.md` 历史裁定
与 §4 的 `38+130=168` 算术**不动**——只在 P1 登记层注销 2 条，不做语义再分类造假
（round88 四分类是「推荐默认文本」归类，round90 是「物理 wire 事实」判定，两层不互相覆盖）。

**同族审计**：§2.3 其余候选 `quest/D8` / `skill/D4` / `corewindow/D8` 均为状态机/条件分支、
不碰 wire，属合法 L 桶候选，无同族误分类；逐面取证后按同一方法推进。

**限制**：零源码行为改动（`Drop.cpp` 仅补三方对账纯注释）/ 零 wire / 零协议 / 零 WZ /
零新封包；25 个外部程序在途 WIP 原样留工作树、精确 add 隔离；真机五屏截图 60 节/142 处仍由用户认领。
"""
must(plan.endswith("\n"), "PLAN must end with single LF")
if "## 7. round90 纠偏" not in plan:
    plan = plan.rstrip("\n") + "\n" + SEC7.rstrip("\n") + "\n"
wb(PLAN, plan.encode("utf-8"))

# ---------------------------------------------------------------- 2 evidence（顶层插入键，历史不删）
EV89 = "docs/features/angel-calibration-d-matrix-2026-11/evidence/wle-triage-round89.json"
ev89_b = rb(EV89)
must(b"\r" not in ev89_b, "round89 evidence must be LF")
if b'"round90_correction"' not in ev89_b:
    corr_json = json.dumps(CORRECTION, ensure_ascii=False, indent=2)
    corr_block = '  "round90_correction": ' + corr_json.replace("\n", "\n  ") + ",\n"
    head = b"{\n"
    must(ev89_b.startswith(head), "round89 evidence must start with '{\\n'")
    ev89_b = head + corr_block.encode("utf-8") + ev89_b[len(head):]
    wb(EV89, ev89_b)
# 合法性自检
json.loads(rb(EV89).decode("utf-8"))

# ---------------------------------------------------------------- 3 新 evidence (LF)
EV90 = "docs/features/angel-calibration-d-matrix-2026-11/evidence/drop-wle-correction-round90.json"
ev90 = dict(CORRECTION)
ev90["artifact_role"] = "round90 主证据（round89 evidence 顶层 round90_correction 为同内容索引副本）"
wb(EV90, (json.dumps(ev90, ensure_ascii=False, indent=2) + "\n").encode("utf-8"))

# ---------------------------------------------------------------- 4 Drop.cpp 纯注释 (CRLF)
DROP = "Gameplay/MapleMap/Drop.cpp"
drop_b = rb(DROP)
must(b"\r\n" in drop_b and b"\r\r\n" not in drop_b, "Drop.cpp must be CRLF clean")
if b"round90 " not in drop_b:
    anchor = b"\tvoid Drop::expire(int8_t type, const PhysicsObject* lt)\r\n\t{\r\n"
    must(anchor in drop_b, "Drop.cpp expire anchor missing")
    note_lines = [
        "\t\t// round90 三方对账（Angel 083 只读取证；零行为改动，勿据此改 wire）：",
        "\t\t// 083 CDropPool::OnDropLeaveField @0x506590:83-99 逐字 ——",
        "\t\t//   v2 = Decode1(animation); Decode4(oid);",
        "\t\t//   if (v2 == 2 || v2 == 3 || v2 == 5) Decode4(looter cid);",
        "\t\t//   else if (v2 == 4) { Decode2(delay); *(v4+68) = get_update_time() + delay; *(v79+72) = 4; }",
        "\t\t//   v2 == 0 走 IWzGr2DLayer::Animate / RemoveCanvas / GetCanvas COM 消亡动画。",
        "\t\t// s9 PacketCreator.java:2624-2642 removeItemFromMap 对 animation >= 2 恒写 writeInt(chrId) 4B。",
        "\t\t// => animation=4 是 wire 宽度分叉（083 2B delay vs s9 4B cid），数值同为 4、语义不同；",
        "\t\t//    按 s9 读 int cid 才正确（W 桶；改回 083 读 short 会平移后续字段、断联机）。",
        "\t\t// => animation=0 的 COM 画布特效当前端无 IWzGr2DLayer 栈，48 帧淡出语义等价（F 维 P2，不补）。",
        "\t\t// 证据：tools/_ida_r90/OnDropLeaveField.code.txt +",
        "\t\t//       drop-angel-calibration-2026-11/README.md:11/73/81/119/243-244/266-277。",
    ]
    note = ("\r\n".join(note_lines) + "\r\n").encode("utf-8")
    drop_b = drop_b.replace(anchor, anchor + note, 1)
    wb(DROP, drop_b)
must(b"round90 " in rb(DROP), "Drop.cpp round90 note missing")

# ---------------------------------------------------------------- 5 dev-plan 2-38 (LF)
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
dev_b = rb(DEV)
must(b"\r" not in dev_b, "dev-plan must be LF")
if b"| **2-38** |" not in dev_b:
    k = dev_b.find(b"| **2-37** |")
    must(k >= 0, "dev-plan 2-37 anchor missing")
    eol = dev_b.find(b"\n", k)
    must(eol >= 0, "dev-plan 2-37 line end missing")
    row = (
        "| **2-38** | calm | **round90 drop 面 W/L/E 误分类纠偏（m10601 自测推进）**："
        "复核 round89 `WLE-TRIAGE-PLAN.md §2.3/§5` 的下一批 P1 登记，发现 `drop/D12`、`drop/D2` "
        "两处判错桶且函数锚点指错（应为 269 离场 `CDropPool::OnDropLeaveField @0x506590`，非 268 进场）。"
        "083 活反编译 `tools/_ida_r90/OnDropLeaveField.code.txt:83-99` 逐字 `v2==4 → Decode2(delay)` + "
        "`*(v4+68)=get_update_time()+delay` / `v2==0 → IWzGr2DLayer COM`；s9 `PacketCreator.java:2624-2642` "
        "对 `animation>=2` 恒写 `writeInt(chrId)` 4B ⇒ **D12 = W 桶 wire 宽度分叉**（数值重合≠语义重合，"
        "改回读 short 断联机）、**D2 = F 维 P2**（COM 画布栈缺失，48 帧淡出语义等价），均非 L 桶。"
        "`Drop.cpp::expire` 仅补三方对账纯注释（**0 行为字面量 / 0 wire / 0 协议 / 0 WZ**）；"
        "168 条历史裁定与 §4 算术不动，reopen `27 = 25 P1 + 1 W + 1 F-P2` 闭合 | "
        "`evidence/drop-wle-correction-round90.json` + `notes/wave3-drop-wle-correction-20261111.md` + "
        "`tools/_ida_r90/*` + `drop-angel-calibration-2026-11/README.md:11/73/81/119/243-244/266-277` | "
        "门禁 5 禁想象修改 / 18 REA→IDA 只读取证；W 桶保持 s9；校准≠改回原生 | "
        "`WLE-TRIAGE-PLAN.md §2.3/§5/§7` + `wle-triage-round89.json` 顶层 `round90_correction` + "
        "`Drop.cpp` 纯注释 + 四文档 | `tools/test_round90_drop_wle_correction.py`（静态守卫 + 场景）+ "
        "`gen_opcode_matrix.py --check`（GATE OK）+ `build_smoke.bat`（EXITCODE=0 真重编覆盖 Drop.cpp） | "
        "✅ 本轮纠偏收口；下一批改走 `quest/D8` / `skill/D4` / `corewindow/D8` 合法 L 桶候选；"
        "真机五屏 60 节/142 处仍由用户认领；**25 个外部在途 WIP 零夹带** |"
    ).encode("utf-8") + b"\n"
    dev_b = dev_b[:eol + 1] + row + dev_b[eol + 1:]
    wb(DEV, dev_b)
must(b"| **2-38** |" in rb(DEV), "dev-plan 2-38 missing")

# ---------------------------------------------------------------- 6 PLT §48-4 标记 + §49 (CRLF)
PLT = "docs/features/pending-live-tests/README.md"
plt_b = rb(PLT)
must(b"\r\n" in plt_b and b"\r\r\n" not in plt_b, "PLT must be CRLF clean")
OLD484 = (
    "48-4. 真机归口（用户认领）：drop animation=0/4 分支还原后的消亡/爆炸帧观感"
    "（承接 §48 下一批 P1）。"
).encode("utf-8")
NEW484 = (
    "48-4. ~~drop animation=0/4 分支还原后的消亡/爆炸帧观感~~ "
    "**round90 已注销（见 §49）**：D12 判 W 桶 wire 宽度分叉、D2 判 F 维 P2，"
    "两者均不还原、不需要该观感验收；真机仍认领 `Drop::expire` 0/1/2/4/5 五档消失行为回归。"
).encode("utf-8")
if OLD484 in plt_b:
    plt_b = plt_b.replace(OLD484, NEW484, 1)
if b"## 49. round90" not in plt_b:
    sec_lines = [
        "## 49. round90 · drop 面 W/L/E 误分类纠偏（纯注释 / 零行为改动）",
        "",
        "49-1. 触发：m10601「还有未完成的或者你可以自测的任务吗？」→ 按 m09211/m09729/m09737 授权自测推进下一批，先复核 round89 §2.3/§5 的 P1 登记，挖出两处误分类。",
        "49-2. 决定性证据：083 活反编译 `tools/_ida_r90/OnDropLeaveField.code.txt:83-99`（269 离场，非 268 进场）= `v2==4 → Decode2(delay)` + `*(v4+68)=get_update_time()+delay`、`v2==0 → IWzGr2DLayer COM`；s9 `PacketCreator.java:2624-2642` 对 `animation>=2` 恒写 `writeInt(chrId)` 4B。",
        "49-3. 判定：`drop/D12` = **W 桶**（wire 宽度分叉，保持 s9 读 int cid，改回 083 读 short 平移字段断联机）；`drop/D2` = **F 维 P2**（COM 画布栈缺失，48 帧淡出语义等价，不补）。均从 `L.p1_registered` 注销；168 条历史裁定与 §4 算术不动，reopen `27 = 25 P1 + 1 W + 1 F-P2` 闭合。",
        "49-4. 真机归口（用户认领）：`Drop::expire` 0/1/2/4/5 五档消失行为回归；承接 §45/§46/§47/§48 其余真机项不变。",
        "49-5. 验收：harness 全绿 + GATE OK + 真重编三证（build_log / EXE / Drop.cpp 覆盖）+ 零署名提交 + 25 个外部在途 WIP 零夹带 + feedback 无 rating。",
        "",
        "限制：① `Drop.cpp` 仅补三方对账纯注释，0 行为字面量 / 零 wire / 零协议 / 零 WZ / 零新封包；② 25 个外部在途 WIP 原样留工作树；③ 下一批改走 `quest/D8` / `skill/D4` / `corewindow/D8` 合法 L 桶候选，门禁 5 禁想象修改；④ 架构级重写仍单点裁决、真机五屏截图 60 节/142 处仍由用户认领。",
    ]
    sec = ("\r\n".join(sec_lines) + "\r\n").encode("utf-8")
    plt_b = plt_b.rstrip(b"\r\n") + b"\r\n\r\n" + sec
wb(PLT, plt_b)
must(b"## 49. round90" in rb(PLT), "PLT §49 missing")

# ---------------------------------------------------------------- 7 notes (LF)
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave3-drop-wle-correction-20261111.md"
notes = """# round90 · drop 面 W/L/E 误分类纠偏（只读取证 + 零行为注释）

> 触发：m10601「还有未完成的或者你可以自测的任务吗？」
> 授权：m09211「全部继续，如果能自测的可以自己启动测试」+ m09729/m09737「按最完全反编译还原基础功能、其余功能同方向开发修复完善」
> 权威源：Angel(13339) 083/v83 带符号 IDB；s9 服务端源码仅约束 W 桶字段宽度。

## 1. 为什么先复核而不是直接按 §5 开工

round89 `WLE-TRIAGE-PLAN.md §5` 把「下一批（drop 面）」写成
「`CDropPool::OnDropEnterField` 的 animation 分支按 083 还原 → 纯逻辑补丁」，
§2.3 把 `drop/D2`（animation=0 消亡动画）+ `drop/D12`（animation=4 延时爆炸帧）
登记为 L 桶 P1。自测推进前先按门禁 18 拉活 IDA 对账，发现**函数锚点、桶归类、可行性三处都错**：

- 两条决策点都在 **269 离场** `CDropPool::OnDropLeaveField @0x506590`，
  `CDropPool::OnPacket @0x5058db` 逐字 `a1==268 → OnDropEnterField` / `a1==269 → OnDropLeaveField`；
  268 进场 `OnDropEnterField @0x505900` 管的是掉落物**生成**（`v108=Decode1(mode)` 等），不是离场动画。
- `drop/D12` 改回 083 会改 wire 读取宽度 → 属于 W 桶硬约束，不是 L 桶纯逻辑。
- `drop/D2` 缺的是 COM 渲染栈 → 属于 F 维 P2 资源层，不是逻辑分支。

## 2. 三方逐字证据

### 2.1 Angel 083 活反编译 `tools/_ida_r90/OnDropLeaveField.code.txt:83-99`

```
  v2 = CInPacket::Decode1(this: a2); /*0x5065b0*/
  v71 = CInPacket::Decode4(this: a2); /*0x5065bc*/
  if ( v2 == 2 || v2 == 3 || v2 == 5 ) /*0x5065e4*/
    *(v7 + 88) = CInPacket::Decode4(this: a2); /*0x506627*/
  else if ( v2 == 4 ) /*0x5065eb*/
  {
    v5 = CInPacket::Decode2(this: a2); /*0x5065f8*/
    *(v4 + 68) = get_update_time() + v5; /*0x506602*/
    *(v79 + 72) = 4; /*0x506608*/
  }
  ...
  if ( v2 == 0 ) /*0x506647*/  → IWzGr2DLayer::GetCanvas / RemoveCanvas / Animate
```

### 2.2 s9 `PacketCreator.java:2624-2642`

```
public static Packet removeItemFromMap(int objId, int animation, int chrId, boolean pet, int slot) {
    OutPacket p = OutPacket.create(SendOpcode.REMOVE_ITEM_FROM_MAP);
    p.writeByte(animation);
    p.writeInt(objId);
    if (animation >= 2) {
        p.writeInt(chrId);
        if (pet) { p.writeByte(slot); }
```

`animation>=2` **恒写 4B cid**；083 仅 `v2==4` 读 **2B delay** 并记账到 `+68/+72`。
数值同为 4、语义完全不同 → 数值重合≠语义重合。

### 2.3 当前端 `Gameplay/MapleMap/Drop.cpp:119-144`

`case 2 / 4 / 5` 同组 `PICKEDUP + vspeed=-4.5f`，`case 0` 仅 `PICKEDUP`（48 帧淡出）；
`Net/Handlers/MapObjectHandlers.cpp` 的 `RemoveLootHandler` 在 `mode > 1` 读 `int cid`。
与 s9 逐字段对齐，**按 s9 读 int 是正确解**。

## 3. 判定与闭合

| 项 | round89 | round90 | 依据 |
| --- | --- | --- | --- |
| `drop/D12` | L 桶 P1 | **W 桶**（保持 s9，零改动） | 083 2B delay vs s9 4B cid；改回读 short 平移后续字段、复现 `LoginParser.cpp:150-174` 白屏类错位 |
| `drop/D2` | L 桶 P1 | **F 维 P2**（不补） | `IWzGr2DLayer` COM 画布栈当前端没有；业务结果（消失）一致 |
| 锚点 | `OnDropEnterField @0x505900` | `OnDropLeaveField @0x506590` | `OnPacket @0x5058db` 268/269 分派 |

**reopen 27 闭合**：`27 = 25 P1 + 1 W + 1 F-P2`。
`DECISIONS.md` 168 条历史裁定与 `WLE-TRIAGE-PLAN.md §4` 的 `38+130=168` **不动**——
round88 四分类是「推荐默认文本」归类，round90 是「物理 wire 事实」判定，两层不互相覆盖，
只在 P1 登记层注销 2 条，不做语义再分类造假。

## 4. 同族审计（一类问题出去）

§2.3 其余候选 `quest/D8`（`sub_721008` 条件分支）、`skill/D4`（`CWndSkillGuide` 子窗）、
`corewindow/D8`（`CUIStat` 6 掩码 / `CUISkill` 500ms 门闩 / `CUIKeyConfig` 5 分支）
均为状态机/条件分支、不碰 wire，属**合法 L 桶候选**，无同族误分类。
下一批改走这三条，每批先出 083 符号 + 反编译锚点，证据不足登记 P2/P3，门禁 5 禁想象修改。

## 5. 改动清单与限制

- `WLE-TRIAGE-PLAN.md` §2.3 / §5 措辞纠偏 + 追加 §7 纠偏段。
- `evidence/wle-triage-round89.json` 顶层追加 `round90_correction`（历史 `p1_registered` 不删，标记 superseded）。
- 新增 `evidence/drop-wle-correction-round90.json`、本 notes、dev-plan §4 `2-38`、PLT §48-4 标记 + §49。
- `Gameplay/MapleMap/Drop.cpp::expire` 前补三方对账**纯注释**（CRLF 字节级保持，0 行为字面量）。
- 零 wire / 零协议 / 零 WZ / 零新封包；25 个外部程序在途 WIP 原样留工作树、精确 add 隔离。
- 真机五屏截图 60 节/142 处仍由用户认领。
"""
wb(NOTES, notes.encode("utf-8"))

# ---------------------------------------------------------------- 收口自检
for p in (PLAN, EV89, EV90, DROP, DEV, PLT, NOTES):
    must(os.path.isfile(os.path.join(ROOT, p)), "missing " + p)
json.loads(rb(EV90).decode("utf-8"))
must(b"\r" not in rb(PLAN) and b"\r" not in rb(EV89) and b"\r" not in rb(EV90), "LF artifact polluted")
must(b"\r" not in rb(DEV) and b"\r" not in rb(NOTES), "LF artifact polluted")
must(b"\r\r\n" not in rb(DROP) and b"\r\r\n" not in rb(PLT), "CRCRLF trap")
print("ROUND90_DELIVER_ALL_OK")
for p in (PLAN, EV89, EV90, DROP, DEV, PLT, NOTES):
    print("  ", p, len(rb(p)), "B")
