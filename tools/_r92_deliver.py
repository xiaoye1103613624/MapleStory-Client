# -*- coding: utf-8 -*-
"""round92 slice1+slice2 幂等生成器：
 - 片1 (skill/D12 / corewindow/D9 -> CUIMacroSysEx::OnSelected) = 编号错配 DOC_ONLY，零源码改动
 - 片2 (login-flow/D9 / statusbar-hud/D12 -> CUIChannelShift::OnKey) = 编号错配，但挖到
   UIChannel::step_channel(±6) uint8_t 下溢真实缺陷 -> 按 083 门控还原（L 桶，零 wire）
产出 6 交付物 + 1 真实补丁；绝不写 commit hash / git log（round88 B9b 教训）。
EOL 字节级保持：UIChannel.cpp / PLT = CRLF；dev / WLE / PLAN / ev / notes = LF。
"""
import os, sys, io, json, hashlib, subprocess

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
ROOT = "E:/project/MapleStory-Client"
os.chdir(ROOT)

CPP = "IO/UITypes/UIChannel.cpp"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
WLE = "docs/features/angel-calibration-d-matrix-2026-11/WLE-TRIAGE-PLAN.md"
PLAN = "docs/features/angel-calibration-d-matrix-2026-11/NEXT-BATCH-PLAN.md"
EV = "docs/features/angel-calibration-d-matrix-2026-11/evidence/wle-implementation-round92.json"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave3-implementation-slice1-2-20261111.md"

# ---- 补丁块（CRLF + tab，逐字对齐 IO/UITypes/UIChannel.cpp:220-235）----
OLD = (
    "\t\t\telse if (keycode == KeyAction::Id::UP)\r\n"
    "\t\t\t{\r\n"
    "\t\t\t\tstep_channel(selected_channel, -static_cast<int>(SLOTS_PER_ROW));\r\n"
    "\t\t\t}\r\n"
    "\t\t\telse if (keycode == KeyAction::Id::DOWN)\r\n"
    "\t\t\t{\r\n"
    "\t\t\t\tstep_channel(selected_channel, static_cast<int>(SLOTS_PER_ROW));\r\n"
    "\t\t\t}\r\n"
    "\t\t\telse if (keycode == KeyAction::Id::LEFT)\r\n"
    "\t\t\t{\r\n"
    "\t\t\t\tstep_channel(selected_channel, -1);\r\n"
    "\t\t\t}\r\n"
    "\t\t\telse if (keycode == KeyAction::Id::RIGHT)\r\n"
    "\t\t\t{\r\n"
    "\t\t\t\tstep_channel(selected_channel, 1);\r\n"
    "\t\t\t}\r\n"
)
NEW = (
    "\t\t\t// round92 按 083 CUIChannelShift::OnKey @0x995863 门控还原：\r\n"
    "\t\t\t//   (a2 != 40 && a2 != 38 || count >= 7) —— UP/DOWN 跨行须频道数 > 单行 6 列；\r\n"
    "\t\t\t//   (a2 != 37 && a2 != 39 || count >= 2) —— LEFT/RIGHT 须至少 2 个频道。\r\n"
    "\t\t\t// 本服 channel_count 常为 3：缺门控时 step_channel(±6) 单次 wrap 后仍为负，\r\n"
    "\t\t\t// uint8_t 下溢成 253..255 / 越界 3..5（tools/test_round92_channel_nav_083.py B1 复现）。\r\n"
    "\t\t\telse if (keycode == KeyAction::Id::UP || keycode == KeyAction::Id::DOWN)\r\n"
    "\t\t\t{\r\n"
    "\t\t\t\tif (channel_count > SLOTS_PER_ROW)\r\n"
    "\t\t\t\t\tstep_channel(selected_channel, keycode == KeyAction::Id::UP\r\n"
    "\t\t\t\t\t\t? -static_cast<int>(SLOTS_PER_ROW) : static_cast<int>(SLOTS_PER_ROW));\r\n"
    "\t\t\t}\r\n"
    "\t\t\telse if (keycode == KeyAction::Id::LEFT || keycode == KeyAction::Id::RIGHT)\r\n"
    "\t\t\t{\r\n"
    "\t\t\t\tif (channel_count > 1)\r\n"
    "\t\t\t\t\tstep_channel(selected_channel, keycode == KeyAction::Id::LEFT ? -1 : 1);\r\n"
    "\t\t\t}\r\n"
)

def rd(p): return open(p, "rb").read()
def wr(p, b): open(p, "wb").write(b)
def rt(p): return open(p, "r", encoding="utf-8").read()
def sha(b): return hashlib.sha256(b).hexdigest()

def head_blob(rel):
    r = subprocess.run(["git", "show", "HEAD:" + rel], capture_output=True)
    return r.stdout  # LF-clean blob (autocrlf)

# ---- 1) 补丁：三态幂等（提交前 HEAD=old / 提交后 HEAD=new 都能重跑）----
raw = rd(CPP)
has_new = NEW.encode("utf-8") in raw
has_old = OLD.encode("utf-8") in raw
assert has_new or has_old, "UIChannel.cpp neither OLD nor NEW block found -> ABORT"

# pre_patch sha：以「未打补丁的归一 HEAD/old 版」为基准，写死进 evidence（复跑复用，防漂移）
blob = head_blob(CPP).replace(b"\r\n", b"\n")          # 归一到 LF
base_lf = blob.decode("utf-8")
if OLD.replace("\r\n", "\n") in base_lf:
    lf_old = OLD.replace("\r\n", "\n"); lf_new = NEW.replace("\r\n", "\n")
    pre_patch = lf_new and base_lf                       # HEAD 是 old
    pre_patch_norm = base_lf.replace(lf_new, lf_old)    # 万一 HEAD 含 new，换回 old
else:
    lf_old = OLD.replace("\r\n", "\n"); lf_new = NEW.replace("\r\n", "\n")
    pre_patch_norm = base_lf.replace(lf_new, lf_old)    # HEAD 已是 new -> 还原 old
PRE_SHA = sha(pre_patch_norm.encode("utf-8"))

if not has_new:
    assert has_old, "lost OLD block"
    wr(CPP, raw.replace(OLD.encode("utf-8"), NEW.encode("utf-8"), 1))
    print("PATCH applied (old -> new)")
else:
    print("PATCH already present (idempotent)")

# ---- 2) evidence（复跑复用 pre_patch_sha256，绝不写 commit hash）----
os.makedirs(os.path.dirname(EV), exist_ok=True)
prev = json.load(open(EV, encoding="utf-8")) if os.path.isfile(EV) else {}
pre_sha_final = prev.get("patch_integrity", {}).get("pre_patch_cpp_sha256", PRE_SHA)

ev = {
    "round": 92,
    "date": "2026-11-11",
    "trigger": "m10905 开始下面的计划 -> NEXT-BATCH-PLAN.md §5 实施阶段（片1 + 片2）",
    "verdict": "slice1_doc_only_number_mismatch__slice2_real_L_bucket_underflow_fixed",
    "slice1_macro_selection": {
        "registered_ids": ["skill/D12", "corewindow/D9"],
        "registered_as": "CUIMacroSysEx::OnSelected @0x8a984d 宏选中态状态机 (L 桶 P1)",
        "decisions_truth": {
            "corewindow/D9": "DECISIONS.md:100 真议题 = 32 格 -> 5 槽数量 (SkillMacros::MAX_SLOTS=5 与 s9 SKILL_MACRO(110) 对齐) KEEP",
            "skill/D12": "DECISIONS.md:198 真议题 = 组合栏 deactivate() 与 ShiftMacroUIState 形态差异 KEEP",
        },
        "three_proofs": {
            "img_no_shout_checkbox": "tools/_ida_r92/img_gate.json: search_Shout/shout/BtShout matches=[] ; Check 仅 Memo/check0/1",
            "083_is_com_control_stack": "tools/_ida_r92/CUIMacroSys_OnCreate.code.txt:98-114 ZAllocEx::Alloc(108) CCtrlCheckBox 3011@(166,241) + CCtrlEdit",
            "client_no_checkbox_class": "全仓 Checkbox 实现类 0；仅 IO/UITypes/UIOptionMenu.h:78 注释；shout 只在 Character/SkillMacros.h:14 + SkillMacroPackets.h:26 数据/发包层",
        },
        "client_equivalent": "IO/UITypes/UIMiscShells.cpp:4880-4884 shown_name 变更检测已等价 083 CCtrlEdit::SetText；32 格 COM 编辑窗 vs 5 槽组合栏为形态/数量收缩（corewindow README:13）",
        "action": "DOC_ONLY 编号错配纠偏，零源码改动",
    },
    "slice2_channel_nav": {
        "registered_ids": ["login-flow/D9", "statusbar-hud/D12"],
        "registered_as": "CUIChannelShift::OnKey @0x995863 6 列频道网格键盘导航 (L 桶 P1 次选)",
        "decisions_truth": {
            "login-flow/D9": "DECISIONS.md:62 真议题 = 60 频道翻页控件 (K4 KEEP_CURRENT，补翻页属增强 P3)",
            "statusbar-hud/D12": "DECISIONS.md:84 真议题 = UIMobGage.* 空 Type (MOBGAGE=102) 保持，UIBossHP 已覆盖",
        },
        "real_defect": {
            "anchor_083": "CUIChannelShift::OnKey @0x995863 门控 (a2!=40&&a2!=38 || this+32>=7) / (a2!=37&&a2!=39 || this+32>=2)",
            "count_source_083": "CUIChannelShift::CUIChannelShift @0x994bcf *(this+32) = *(*dword_BE7918+8280) ; v4 = 21*(v3/6)+125 (6 列分行)",
            "client_gap": "IO/UITypes/UIChannel.cpp:179-206 step_channel 单次 wrap；:220-235 send_key 无门控",
            "repro": "channel_count=3, SLOTS_PER_ROW=6, UP(-6) sel=0/1/2 -> uint8_t 253/254/255 ; DOWN(+6) -> 3/4/5 越界",
            "repro_file": "tools/test_round92_channel_nav_083.py B1",
        },
        "patch": {
            "file": CPP,
            "function": "UIChannel::send_key",
            "change": "UP|DOWN 合并分支加 if (channel_count > SLOTS_PER_ROW) ；LEFT|RIGHT 加 if (channel_count > 1)",
            "wire": 0, "protocol": 0, "wz_write": 0, "new_packet": 0,
            "step_channel_body_unchanged": True,
            "math_closed": "count>=7 时 delta=±6 单次 wrap 数学闭合：v∈[-6,count+5]，一次 ±count 落回 [0,count)",
        },
    },
    "pattern_finding": "round91 Plan §4 整批 P1 编号均从矩阵文本推导、未走四方取证 = 矩阵文本->P1 登记层同族病连续第 4 轮（round90 drop / round91 三候选 / round92 四编号）；WLE §8 已写「登记前必附四项取证」却未回扫 Plan §4 -> 本轮升级为 harness 静态守卫",
    "hard_rule_upgrade": "NEXT-BATCH-PLAN.md 中每个 P1 候选必须附 083 反编译 + s9 handler + 当前端字面量 + DECISIONS 四项锚点；harness A12 逐条断言",
    "arithmetic_unchanged": {
        "decisions_168": "168 条历史裁定不动",
        "wle_section4": "W 38 + L 130 = 168 不动",
        "reopen_27": "27 = 25 P1 + 1 W + 1 F-P2 不动（仅 P1 登记层注销 4 个错配编号）",
    },
    "img_gates_128b": {
        "macro": "tools/_ida_r92/img_gate.json GATE_OK_128B_MACRO_NODES_ENUMERATED",
        "channel": "tools/_ida_r92/img_gate_channel.json GATE_OK_128B_CHANNEL_NODES_ENUMERATED",
        "key": "128B WzAESConstant.DEFAULT_KEY base64 tools/_gms_key_128.b64 (172 chars) iv TSPHKw==",
    },
    "ida_evidence_files": [
        "tools/_ida_r92/CUIMacroSysEx_GetIndexByPos.code.txt",
        "tools/_ida_r92/CUIMacroSysEx_OnButtonClicked.code.txt",
        "tools/_ida_r92/CUIMacroSysEx_OnKey.code.txt",
        "tools/_ida_r92/CUIMacroSysEx_OnMouseButton.code.txt",
        "tools/_ida_r92/CUIMacroSysEx_ResetInfo.code.txt",
        "tools/_ida_r92/CUIMacroSys_GetIndexByPos.code.txt",
        "tools/_ida_r92/CUIMacroSys_OnButtonClicked.code.txt",
        "tools/_ida_r92/CUIMacroSys_OnCreate.code.txt",
        "tools/_ida_r92/CUIMacroSys_OnSelected.code.txt",
        "tools/_ida_r92/CUIMacroSys_ResetInfo.code.txt",
        "tools/_ida_r92/CUIChannelShift_OnCreate.code.txt",
        "tools/_ida_r92/CUIChannelShift_Draw.code.txt",
        "tools/_ida_r92/CUIChannelShift_ctor.code.txt",
        "tools/_ida_r92/q__CMacroSysMan_.txt",
        "tools/_ida_r92/img_gate.json",
        "tools/_ida_r92/img_gate_channel.json",
        "tools/_ida_r91/CUIChannelShift_OnKey.code.txt",
        "tools/_ida_r91/CUIChannelSelect_OnKey.code.txt",
    ],
    "patch_integrity": {"pre_patch_cpp_sha256": pre_sha_final, "note": "worktree 归一 LF 后把 NEW 换回 OLD 的 sha；与 HEAD 版本无关，提交/amend 不漂移"},
    "zero_wire": True,
    "constraints": [
        "083/v83 only 禁 095",
        "s9 协议优先（W 桶 6 字段宽度/类型零改动）",
        "校准≠改回原生（L 桶同字段原始逻辑按 083 还原）",
        "数值重合≠语义重合",
        "门禁 5 禁想象修改 / 18 REA→IDA→orange-wz(128B)→.nx 只读取证",
    ],
}
open(EV, "w", encoding="utf-8", newline="\n").write(json.dumps(ev, ensure_ascii=False, indent=2) + "\n")

# ---- 3) notes（新建 LF）----
notes = """# round92 实施片 1 + 片 2 · 编号错配纠偏 + UIChannel 下溢修复（2026-11-11）

> 触发 m10905「开始下面的计划」→ 进入 NEXT-BATCH-PLAN.md §5 实施阶段。
> 本轮不再从矩阵文本推导批次，先四方取证再定性。

## 1. 片 1（skill/D12 / corewindow/D9 → 宏选中态）= DOC_ONLY 编号错配

- round91 Plan §4 把 `CUIMacroSysEx::OnSelected @0x8a984d` 挂到这两个编号下，判为「L 桶纯逻辑补丁」。
- DECISIONS 原文：`corewindow/D9`（:100）真议题是 32 格→5 槽数量、`skill/D12`（:198）真议题是
  `deactivate()` 与 `ShiftMacroUIState` 形态差异 —— **都不是宏选中态状态机**。
- 三项现场证据坐实「无干净补丁」：
  1. orange-wz 128B 门禁 `search_Shout/shout/BtShout` 全 `matches:[]`，`Check` 仅 `Memo/check0/1`
     → 083 三个宏资源根（`Macro`/`SkillMacro`/`SkillMacroEx`）都无 shout 勾选框位图；
  2. `CUIMacroSys::OnCreate @0x8a8504:98-114` 是 COM 控件栈（`ZAllocEx::Alloc(108)`
     `CCtrlCheckBox` id=3011 @(166,241) + `CCtrlEdit`），当前端无 COM 控件体系；
  3. 当前端全仓 Checkbox 实现类 0（仅 `UIOptionMenu.h:78` 注释），`shout` 只在数据层/发包层出现。
- 当前端 `UISkillMacroEx` 是 5 槽技能组合栏（corewindow README:13 明确「不是 62 类一一对应」），
  `UIMiscShells.cpp:4880-4884` 已用 `shown_name` 变更检测等价实现 083 `CCtrlEdit::SetText`。
- **判定：零源码改动**，仅登记编号错配。

## 2. 片 2（login-flow/D9 / statusbar-hud/D12 → 频道网格）= 编号错配 + 真实 L 桶缺陷

- DECISIONS 原文：`login-flow/D9`（:62）= 60 频道翻页控件（P3 增强 KEEP）、
  `statusbar-hud/D12`（:84）= `UIMobGage.*` 空 Type（保持）——也都不是键盘导航。
- 但对应窗 `CUIChannelShift`（083 游戏内换频，6 列）↔ 当前端 `UIChannel`（`SLOTS_PER_ROW=6`）
  的门控对拍挖到真实缺陷：
  - 083 `CUIChannelShift::OnKey @0x995863` 门控 `count>=2` 才放 LEFT/RIGHT、`count>=7` 才放 UP/DOWN；
    `CUIChannelShift::CUIChannelShift @0x994bcf` 中 `*(this+32)=*(*dword_BE7918+8280)` 即频道总数，
    `v4 = 21*(v3/6)+125` 按 6 列分行决定窗高。
  - 当前端 `UIChannel::step_channel`（:179-206）单次 wrap + `send_key`（:220-235）无门控。
    本服 `channel_count=3` 时 UP(-6) 让 `sel-6+3` 仍为负 → `uint8_t` 下溢 253/254/255，
    DOWN(+6) 得 3/4/5 越界（`tools/test_round92_channel_nav_083.py` B1 逐值复现）。
- **补丁**：`UIChannel::send_key` 的 UP|DOWN 合并分支加 `if (channel_count > SLOTS_PER_ROW)`、
  LEFT|RIGHT 加 `if (channel_count > 1)`，逐字还原 083 门控语义；`step_channel` 函数体零改动。
  `count>=7` 时 `delta=±6` 单次 wrap 数学闭合（B3 穷举证明），无需改取模算法。
- 零 wire / 零协议 / 零 WZ 改写 / 零新封包。

## 3. 同族病升级为 harness 守卫

- 「矩阵文本→P1 登记层错配」已连续第 4 轮（round90 drop → round91 三候选 → round92 四编号）。
- round91 WLE §8 已写硬规则「登记前必附 083 反编译 + s9 handler + 当前端字面量 + DECISIONS 四项」，
  但**同批生成的 Plan §4 自己没回扫** —— 规则写了没落地等于没写。
- 本轮 WLE §9 写死：任何 P1 候选登记必须四项齐备；harness A12 对 NEXT-BATCH-PLAN.md §4
  三个候选逐条断言 083 锚点存在。

## 4. 算术与限制

- 168 条 DECISIONS 历史裁定 / WLE §4 `38+130=168` / reopen `27=25 P1+1 W+1 F-P2` **全不动**，
  仅在 P1 登记层注销 4 个错配编号（不改语义再分类）。
- W 桶 6 字段宽度/类型 + 27 条 wire KEEP 零改动；`quest/D8`/`skill/D4`/`corewindow/D8` 零改动。
- 真机五屏观感（含本服 3 频道换频窗 ↑/↓、多频道服跨行）由用户认领，见 PLT §51。
"""
open(NOTES, "w", encoding="utf-8", newline="\n").write(notes)

# ---- 4) dev README §4 插 2-40（LF，幂等）----
dev = rt(DEV)
if "| **2-40** |" not in dev:
    lines = dev.split("\n")
    idx = next((i for i, l in enumerate(lines) if l.startswith("| **2-39** |")), None)
    assert idx is not None, "2-39 anchor missing"
    row = ("| **2-40** | round92 实施片1+片2（m10905） | **片1** `skill/D12`/`corewindow/D9` 编号错配"
           "（真议题=5槽数量/deactivate 形态，083 是 32 格 COM 编辑窗，当前端 shown_name 已等价 SetText）→ DOC_ONLY 零改动；"
           "**片2** `login-flow/D9`/`statusbar-hud/D12` 编号错配（真议题=60 频道翻页/UIMobGage 空 Type），"
           "但 `CUIChannelShift::OnKey @0x995863` 门控对拍挖到 `IO/UITypes/UIChannel.cpp::step_channel(±6)` "
           "uint8_t 下溢（channel_count=3 → 253/254/255）→ `send_key` 加 083 `count>6`/`count>1` 门控（L 桶，零 wire）| "
           "evidence `wle-implementation-round92.json` / notes `wave3-implementation-slice1-2-20261111.md` / PLT §51 / 真重编三证 |")
    lines.insert(idx + 1, row)
    open(DEV, "w", encoding="utf-8", newline="\n").write("\n".join(lines))

# ---- 5) PLT 追加 §51（CRLF，幂等，rstrip 后单 CRLF；必须二进制，避免通用换行把整文件掰成 LF）----
plt_b = rd(PLT)
plt = plt_b.decode("utf-8")
if "## 51." not in plt:
    sec = [
        "## 51. round92 · 实施片1+片2（编号错配纠偏 + UIChannel 下溢修复）",
        "",
        "- **51-1** 本服 `channel_count=3` 打开换频窗，按 ↑/↓ 不再出现 253/254/255 幽灵选中帧或 3/4/5 越界高亮；左右键与 ESC 行为不变。",
        "- **51-2** 多频道服（>=7）换频窗 ↑/↓ 跨行正常、6 列落位与 083 `CUIChannelShift` 一致。",
        "- **51-3** 片1 宏组合栏选中态真机观感（DOC_ONLY 未改，`shown_name` 变更检测保持）。",
        "- **51-4** 硬规则：NEXT-BATCH-PLAN.md 任何 P1 候选登记须附 083 反编译 + s9 handler + 当前端字面量 + DECISIONS 四项（harness A12 守卫）。",
        "- 限制：① 零 wire / 零协议 / 零 WZ 改写 / 零新封包；② 4 个错配编号只在 P1 登记层注销，168 条 DECISIONS 裁定与 reopen 27 算术不动；③ W 桶 6 字段 + 27 wire KEEP 零改动；④ 真机五屏截图仍由用户认领。",
    ]
    base = plt.rstrip("\r\n")
    open(PLT, "w", encoding="utf-8", newline="").write(base + "\r\n" + "\r\n".join(sec) + "\r\n")

# ---- 6) WLE 追加 §9（LF，幂等）----
wle = rt(WLE)
if "## 9." not in wle:
    sec9 = """## 9. round92 纠偏 — 实施片1+片2 的编号错配（m10905）

**结论：round91 Plan §4 的 4 个 P1 编号全部错配 —— 同族病连续第 4 轮，且是系统性的一批，不是按片各算一次。**

| Plan §4 编号 | round91 挂的锚点 | DECISIONS 真议题 | 本轮判定 |
| --- | --- | --- | --- |
| `skill/D12` | `CUIMacroSysEx::OnSelected` 选中态 | `deactivate()` vs `ShiftMacroUIState` 形态差异 | DOC_ONLY 编号错配 |
| `corewindow/D9` | `CUIMacroSysEx::OnSelected` 选中态 | 32 格 → 5 槽数量 | DOC_ONLY 编号错配 |
| `login-flow/D9` | `CUIChannelShift::OnKey` 网格导航 | 60 频道翻页控件（P3） | 编号错配 + 挖到真缺陷 |
| `statusbar-hud/D12` | `CUIChannelShift::OnKey` 网格导航 | `UIMobGage.*` 空 Type | 编号错配 |

- 片1 三证（128B 门禁 `Shout` 零命中 / 083 `CCtrlCheckBox` COM 栈 / 当前端无 Checkbox 实现类）坐实
  「083 是 32 格 COM 编辑窗、当前端是 5 槽组合栏」，`shown_name` 已等价 `SetText` → **零源码改动**。
- 片2 对应窗 `CUIChannelShift`(083) ↔ `UIChannel`(当前端 6 列) 门控对拍，挖到 `step_channel(±6)`
  在 `channel_count=3` 时 uint8_t 下溢 → `send_key` 加 083 `count>6`/`count>1` 门控（**本轮唯一真实 L 桶补丁**）。
- **规则回扫（P8 自我批判）**：§8 已写「登记前必附四项取证」，但同批 Plan §4 未回扫即发货。
  本轮升级：任何 P1 候选登记必须四项齐备，且由 harness 静态守卫逐条断言，不靠人肉记忆。
- 168 条 DECISIONS / §4 `W 38 + L 130 = 168` / reopen `27 = 25 P1 + 1 W + 1 F-P2` **全不动**；
  仅在 P1 登记层注销 4 个错配编号，不做语义再分类。W 桶 6 字段 + 27 wire KEEP 零改动。
"""
    open(WLE, "w", encoding="utf-8", newline="\n").write(wle.rstrip("\n") + "\n" + sec9)

# ---- 7) NEXT-BATCH-PLAN 追加 §8（LF，幂等，实施回执）----
plan = rt(PLAN)
if "## 8." not in plan:
    sec8 = """## 8. round92 实施回执（m10905「开始下面的计划」）

> 本轮按 §5 推进片 1、片 2；结论是「先四方取证再定性」救了两次返工。

- **片 1**（§4 P1 首选 `skill/D12`/`corewindow/D9` 宏选中态）：四方取证证伪 → 编号错配 DOC_ONLY，
  零源码改动。083 `CUIMacroSysEx` 是 32 格 COM 编辑窗（`CCtrlCheckBox`/`CCtrlEdit`/`CUIToolTip`），
  当前端 `UISkillMacroEx` 是 5 槽组合栏，`shown_name` 变更检测已等价 `SetText`；orange-wz 128B 门禁
  `Shout` 零命中、当前端无 Checkbox 实现类 → 无干净补丁可落。
- **片 2**（§4 P1 次选 `login-flow/D9`/`statusbar-hud/D12` 频道网格）：编号同样错配，但
  `CUIChannelShift::OnKey @0x995863` 门控（`count>=7` 放 ↑/↓、`count>=2` 放 ←/→）对拍当前端
  `UIChannel::send_key` 挖到真实缺陷 —— `step_channel(±6)` 在 `channel_count=3` 时单次 wrap 后仍负、
  `uint8_t` 下溢 253..255 / 越界 3..5。**已落 L 桶纯逻辑补丁**（`send_key` 加 083 门控，
  `step_channel` 函数体零改动，零 wire）。
- **§4 候选登记表整体作废**：4 个编号均系从矩阵文本推导、未走四方取证。下一批必须重新按
  「083 反编译 + s9 handler + 当前端字面量 + DECISIONS 四项」逐条立项（WLE §9 硬规则 + harness 守卫）。
- 真机归口见 PLT §51；168 条裁定 / reopen 27 算术 / W 桶 6 字段 + 27 wire KEEP 全不动。
"""
    open(PLAN, "w", encoding="utf-8", newline="\n").write(plan.rstrip("\n") + "\n" + sec8)

# ---- 收口：6 SHA + 自检 ----
print("ROUND92_DELIVER_ALL_OK")
for p in [CPP, DEV, PLT, WLE, PLAN, EV, NOTES]:
    b = rd(p)
    print("  SHA %s %s" % (sha(b)[:12], p))
print("PRE_PATCH_SHA %s" % pre_sha_final[:16])
