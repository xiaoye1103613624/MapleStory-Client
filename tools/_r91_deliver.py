# -*- coding: utf-8 -*-
"""round91 generator: one-shot NEXT-BATCH-PLAN + three-candidate W/L/E correction.

IDEMPOTENT. No commit hash / git-log output is ever written into deliverables
(round88 B9b lesson). EOL-preserving: WLE-PLAN/dev = LF, PLT = CRLF.
Print ASCII-safe only (MSYS stdout may be GBK).
"""
import os, json, hashlib, io

ROOT = "E:/project/MapleStory-Client"
DM = "docs/features/angel-calibration-d-matrix-2026-11"
DP = "docs/features/development-and-fix-plan-2026-11"
WLE = DM + "/WLE-TRIAGE-PLAN.md"
DEV = DP + "/README.md"
PLT = DP.replace("development-and-fix-plan-2026-11", "pending-live-tests") + "/README.md"
EVDIR = DM + "/evidence"
NOTES = DP + "/notes/wave3-next-batch-plan-20261111.md"
EV = EVDIR + "/next-batch-plan-round91.json"
PLAN = DM + "/NEXT-BATCH-PLAN.md"
IDA = "tools/_ida_r91"

def rp(p):
    return os.path.join(ROOT, p)

def readb(p):
    with open(rp(p), "rb") as f:
        return f.read()

def writeb(p, b):
    full = rp(p)
    os.makedirs(os.path.dirname(full), exist_ok=True)
    with open(full, "wb") as f:
        f.write(b)

def lf_text(lines):
    return ("\n".join(lines).rstrip("\n") + "\n").encode("utf-8")

def sha(b):
    return hashlib.sha256(b).hexdigest()[:12]

# ---------------------------------------------------------------- evidence
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
]
for f in IDA_FILES:
    assert os.path.isfile(rp(IDA + "/" + f)), "missing IDA evidence: " + f
for f in ["img_gate.json"]:
    assert os.path.isfile(rp(IDA + "/" + f)), "missing IMG gate: " + f

gate = json.loads(readb(IDA + "/img_gate.json").decode("utf-8"))
assert gate["verdict"] == "GATE_OK_128B_IMG_REACHABLE", gate["verdict"]
assert gate["key_b64_len"] == 172 and gate["iv_b64"] == "TSPHKw=="

# 083 decompiled literals (verbatim, from tools/_ida_r91/*.code.txt)
LIT = {}
def grab(name, needles):
    t = readb(IDA + "/" + name).decode("utf-8", "replace")
    hits = [ln.strip() for ln in t.splitlines() if any(n in ln for n in needles)]
    LIT[name] = hits
    return hits

q_start = grab("CUIQuestInfo_StartQuest_action.code.txt",
               ["if ( sub_721008", "COutPacket::COutPacket(a1: 107)",
                "COutPacket::Encode2(a1: v50)", "COutPacket::Encode2(a1: v51)",
                "CClientSocket::SendPacket"])
q_sub = grab("sub_721008.code.txt", ["IsAutoStartQuest", "sub_720FED", "return 1"])
q_stat = grab("CUIStat_OnButtonClicked.code.txt",
              ["case 2000:", "v2 = 2048", "v2 = 0x2000", "v2 = 64", "v2 = 128",
               "v2 = 256", "v2 = 512", "SendAbilityUpRequest", "AutoApUp"])
q_kc = grab("CUIKeyConfig_OnButtonClicked.code.txt",
            ["if ( a2 == 1 )", "if ( a2 == 2 )", "if ( a2 == 2000 )",
             "a2 != 2001", "a2 == 2002", "SaveFuncKeyMap", "DefaultFuncKeyMap",
             "memset(a1: (dword_BED5A0 + 4)", "UI_Close"])
q_sp = grab("CUISkill_OnSkillLevelUpButton.code.txt",
            ["CanSendExclRequest(a1: 500", "SP_3509_", "SP_3510_", "SP_3511_"])
q_excl = grab("CanSendExclRequest.code.txt", ["get_update_time() - *(this + 2090) >= a2"])
q_guide = grab("CWndSkillGuide_OnKey.code.txt", ["a2 == 27", "CloseSkillGuide"])
q_macro_sel = grab("CUIMacroSysEx_OnSelected.code.txt",
                   ["*(this + 361) = a2", "GetMacroName", "IsShoutMacro", "ResetInfo"])
q_macro_key = grab("CUIMacroSys_OnKey.code.txt", ["a1 == 27", "ProcessBasicUIKey"])
q_ch_shift = grab("CUIChannelShift_OnKey.code.txt", ["case 0x25u", "case 0x26u", "case 0x27u", "case 0x28u", "v8 = v3 - 1"])
q_ch_sel = grab("CUIChannelSelect_OnKey.code.txt", ["SendViewAllCharacterPacket", "wParam == 9", "wParam != 13", "wParam == 27"])
q_shiftmacro = grab("CUISkillEx_ShiftMacroUIState.code.txt", ["*(this + 401) = 2", "*(this + 401) = 0", "IWz", "Animate"])

assert any("if ( sub_721008(a1: *(this + 6)) == 0 )" in x for x in q_start), "quest wire literal missing"
assert any("IsAutoStartQuest" in x for x in q_sub), "sub_721008 literal missing"
assert any("2048" in x for x in q_stat) and any("0x2000" in x for x in q_stat), "CUIStat masks missing"
assert any("CanSendExclRequest(a1: 500" in x for x in q_sp), "500ms gate literal missing"
assert any("get_update_time() - *(this + 2090) >= a2" in x for x in q_excl), "excl literal missing"
assert any("CloseSkillGuide" in x for x in q_guide), "guide literal missing"
assert any("*(this + 361) = a2" in x for x in q_macro_sel), "macro literal missing"
assert any("case 0x25u" in x for x in q_ch_shift), "channelshift literal missing"
assert any("SendViewAllCharacterPacket" in x for x in q_ch_sel), "channelselect literal missing"

ev = {
  "round": 91,
  "date": "2026-11-11",
  "trigger": "m10739 '开始 下一批 的计划' (gate-1 plan round, zero source change)",
  "authority": "Angel.exe(13339) 083/v83 IDB; s9 source only constrains W-bucket wire widths",
  "deliverable": DM + "/NEXT-BATCH-PLAN.md",
  "zero_source_change": True,
  "zero_wire_change": True,
  "zero_protocol_change": True,
  "zero_wz_change": True,
  "img_gate": {
    "endpoint": gate["endpoint"],
    "img": gate["img"],
    "key_spec": gate["key_spec"],
    "key_b64_len": gate["key_b64_len"],
    "key_sha_head": gate["key_sha_head"],
    "iv_b64": gate["iv_b64"],
    "verdict": gate["verdict"],
    "queries": {k: {"isError": v["isError"], "text_len": v["text_len"]}
                for k, v in gate["queries"].items()},
    "img_dependency": gate["img_dependency"],
    "note": gate["img_dependency_note"],
  },
  "ida_evidence_files": [IDA + "/" + f for f in IDA_FILES],
  "candidates": [
    {
      "id": "quest/D8",
      "round89_registration": "L-bucket P1 (WLE-TRIAGE-PLAN.md:96, :123, :152-153)",
      "round91_verdict": "W_bucket_wire_conditional_suffix_KEEP_s9_NOT_P1",
      "matrix_row": DM + "/README.md:304 '| **D8** | 是否把 sub_721008 条件分支搬进当前端 | 不搬 | K2 |'",
      "decisions_row": DM + "/DECISIONS.md:251 '| **D8** | K2 | 是否把 `sub_721008` 条件分支搬进当前端 | 不搬 | `BATCH_1_KEEP` |'",
      "ida": {
        "symbol": "sub_721008 @0x721008",
        "file": IDA + "/sub_721008.code.txt",
        "literal": q_sub,
        "StartQuest": "CQuest::StartQuest @0x71751c COutPacket(107) action1/action2",
        "StartQuest_file": IDA + "/CUIQuestInfo_StartQuest_action.code.txt",
        "wire_literal": [x for x in q_start if "sub_721008" in x or "Encode2(a1: v5" in x or "COutPacket(a1: 107)" in x],
      },
      "s9": {
        "file": "E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/net/server/channel/handlers/QuestActionHandler.java",
        "isNpcNearby": "if (p.available() >= 4) { playerP = new Point(p.readShort(), p.readShort()); } else { playerP = pos; }",
        "note": "s9 tolerates BOTH forms (with/without 4B x/y suffix); current client's no-suffix packet is a legal s9 shape.",
      },
      "client": {
        "file": "Net/Packets/QuestPackets.h",
        "layout": "byte action, short questid [, int npcid for start/complete/scripted] — NEVER writes x/y",
      },
      "reason": "sub_721008 gates a 4B x/y Encode2 suffix on the c2s 107 packet => wire layout, not pure logic. Restoring it changes packet shape (W bucket / protocol-level), and s9 already accepts the current shape. Original 'do not port' verdict stands.",
      "symbol_semantics_resolved": "sub_721008 = IsAutoStartQuest(questid) || sub_720FED(questid); the old '函数名无符号语义待补 L2' note is now closed, but the KEEP verdict is unchanged because it is wire.",
      "action": "no P1 patch; keep s9; record as W-bucket wire-conditional in NEXT-BATCH-PLAN",
    },
    {
      "id": "skill/D4",
      "round89_registration": "L-bucket P1 (WLE-TRIAGE-PLAN.md:96, :123, :152-153)",
      "round91_verdict": "ARCHITECTURE_single_window_decision_KEEP_no_new_window_NOT_P1",
      "matrix_row": DM + "/README.md:243 '| **D4** | 是否新增独立 CWndSkillGuide 子窗 | 不新增 | K2 |'",
      "decisions_row": DM + "/DECISIONS.md:190 '| **D4** | K2 | 是否新增独立 `CWndSkillGuide` 子窗 | 不新增 | `BATCH_1_KEEP` |'",
      "ida": {
        "symbols": ["??0CWndSkillGuide@@QAE@J@Z @0x8aa243", "?OnKey@CWndSkillGuide@@UAEXII@Z @0x8aa4e4"],
        "file": IDA + "/CWndSkillGuide_OnKey.code.txt",
        "literal": q_guide,
      },
      "client": {
        "file": "IO/UITypes/UISkillBook.cpp:797",
        "send_key": "escape -> set_skillpoint(false) / close()",
        "calibration": "docs/features/skill-angel-calibration-2026-11/README.md:79 A4 judged ESC closed-loop CONSISTENT",
      },
      "reason": "This is an architecture-level 'add a whole independent window family or not' decision, NOT a same-field original-logic restoration. WLE-TRIAGE-PLAN.md:133 itself excludes architecture rewrites from automatic landing scope. Original 'do not add' verdict stands.",
      "action": "no P1 patch; keep no-new-window; single-point decision only if user reopens",
    },
    {
      "id": "corewindow/D8",
      "round89_registration": "L-bucket P1 (WLE-TRIAGE-PLAN.md:96, :123, :152-153)",
      "round91_verdict": "DOC_ONLY_already_shipped_round75_NOT_P1",
      "matrix_row": DM + "/README.md:152 '| **D8** | CUIStat 6 掩码 / CUISkill 500ms 门闩 / CUIKeyConfig 5 分支覆盖度？ | 先出逐条对照清单，不改代码；确认 s9 是否需要 | K3 |'",
      "decisions_rows": [DM + "/DECISIONS.md:99", DM + "/DECISIONS.md:293", DM + "/DECISIONS.md:311"],
      "ida": {
        "CUIStat_OnButtonClicked": {"addr": "0x8c522f", "file": IDA + "/CUIStat_OnButtonClicked.code.txt", "masks": q_stat[:8]},
        "CUIKeyConfig_OnButtonClicked": {"addr": "0x8335d0", "file": IDA + "/CUIKeyConfig_OnButtonClicked.code.txt", "branches": q_kc[:10]},
        "CUISkill_OnSkillLevelUpButton": {"addr": "0x8acf1a", "file": IDA + "/CUISkill_OnSkillLevelUpButton.code.txt", "gate": q_sp},
        "CanSendExclRequest": {"addr": "0x485bf7", "file": IDA + "/CanSendExclRequest.code.txt", "literal": q_excl},
      },
      "s9": {
        "DistributeSPHandler": "p.readInt(); int skillid = p.readInt(); AssignSPProcessor.SPAssignAction(c, skillid); — NO 500ms rate limit",
        "AutoAssignHandler": "AssignAPProcessor.APAutoAssignAction(p, c) — s9 Stat 5-byte encoding",
        "KeymapChangeHandler": "s9-private keymap delta form; QuickslotKeyMappedModifiedHandler reads QUICKSLOT_SIZE ints",
      },
      "client": {
        "UIStatsInfo": "IO/UITypes/UIStatsInfo.cpp:729-755 to_code STR/DEX/INT/LUK -> AutoAssignApPacket::StatCode",
        "StatCode": "Net/Packets/PlayerPackets.h:54-60 STR=64, DEX=128, INT=256, LUK=512 (4, not 6: 2048/0x2000 are HP/MP which s9 lacks)",
        "UISkillBook": "IO/UITypes/UISkillBook.cpp:545-560 BT_OKAY while(used>0) spend_sp — no 500ms gate, CORRECT for s9",
        "UIKeyConfig": "IO/UITypes/UIKeyConfig.cpp:731-810 button_pressed 6 cases (CLOSE/CANCEL/DEFAULT/DELETE/OK/KEYSETTING) aligned to 083 5 branches + local adaptation",
      },
      "reason": "K3 DOC_ONLY; round75 c07b1a7b already shipped the comparison list. s9 has no 500ms limit and a different Stat encoding => the 083 literals are reference-only, not a patch target. The 083-vs-client comparison is delivered as evidence this round (zero source change).",
      "action": "no P1 patch; deliver the verbatim 083 comparison inside NEXT-BATCH-PLAN + evidence",
    },
  ],
  "pattern_finding": {
    "title": "THIRD occurrence of paper-registration-before-evidence (same family as round90 drop/D2+D12)",
    "false_claim": "WLE-TRIAGE-PLAN.md:152-153 '同族审计：§2.3 其余候选 quest/D8 / skill/D4 / corewindow/D8 均为状态机/条件分支、不碰 wire，属合法 L 桶候选，无同族误分类'",
    "also": "WLE-TRIAGE-PLAN.md:96 '等状态机分支同理' + :123 '改走真正有 083 状态机证据且零 wire 的 L 桶候选'",
    "evidence": "083 live IDA (12 decompilations) + s9 handlers + client code + DECISIONS.md verdicts all disagree; all three are NOT L-bucket P1.",
    "lesson": "Every 'next batch L-bucket' registration must carry its own 083 decompilation anchor + s9 handler + client literal BEFORE it is written into a plan. Generalize the fix: stop registering batches from the D-matrix text alone.",
  },
  "next_batch_clean_L_bucket": [
    {
      "priority": "P1-first",
      "ids": ["skill/D12", "corewindow/D9"],
      "topic": "macro system selected-state / ESC state machine (pure UI, zero wire)",
      "ida": [
        {"symbol": "CUIMacroSysEx::OnSelected @0x8a984d", "file": IDA + "/CUIMacroSysEx_OnSelected.code.txt", "literal": q_macro_sel[:6]},
        {"symbol": "CUIMacroSys::OnKey @0x8a98dc", "file": IDA + "/CUIMacroSys_OnKey.code.txt", "literal": q_macro_key},
      ],
      "why_clean": "20-line + 8-line pure UI state machine: *(this+361)=a2 / GetMacroName / IsShoutMacro / SetChecked / ResetInfo / ProcessBasicUIKey(27). Zero COutPacket / zero Encode / zero SendPacket.",
      "client_anchor": "macro slots in UIKeyConfig / UISkillBook (32->5 slot adaptation, corewindow/D9)",
      "gate": "REA->IDA->orange-wz(128B)->.nx read-only, then one atomic commit per slice",
    },
    {
      "priority": "P1-second",
      "ids": ["login-flow/D9", "statusbar-hud/D12"],
      "topic": "channel grid keyboard navigation (pure index math, zero wire)",
      "ida": [
        {"symbol": "CUIChannelShift::OnKey @0x995863", "file": IDA + "/CUIChannelShift_OnKey.code.txt", "literal": q_ch_shift[:8]},
        {"symbol": "CUIChannelSelect::OnKey @0x628a1c", "file": IDA + "/CUIChannelSelect_OnKey.code.txt", "literal": q_ch_sel[:6]},
      ],
      "why_clean": "CUIChannelShift = 6-column arrow-key index computation (case 0x25/0x26/0x27/0x28), no packet. CUIChannelSelect MUST be split: navigation = L bucket, SendViewAllCharacterPacket = W bucket (s9 first, keep).",
      "gate": "split L/W before any patch; navigation only",
    },
    {
      "priority": "P2-F-dim",
      "ids": ["skill/D12-macro-canvas"],
      "topic": "CUISkillEx::ShiftMacroUIState COM canvas stack",
      "ida": {"symbol": "CUISkillEx::ShiftMacroUIState @0x8bad5a", "file": IDA + "/CUISkillEx_ShiftMacroUIState.code.txt", "literal": q_shiftmacro[:6]},
      "why_not_P1": "243-line COM IWz canvas/Animate state machine; current client has no COM canvas stack => F-dimension P2, same class as round90 drop/D2. Do not invent.",
    },
  ],
  "arithmetic_unchanged": {
    "decisions_168": "168 historical verdicts untouched",
    "wle_section4": "W 38 + L 130 = 168 unchanged",
    "reopen_27": "27 = 25 P1 + 1 W + 1 F-P2 (round90) unchanged; this round only cancels 3 P1 *registrations*, NOT the 168 classification",
    "note": "Registration layer and historical verdict layer are separate; no semantic reclassification fraud.",
  },
  "constraints": [
    "083/v83 only, 095 forbidden",
    "s9 wire first (W bucket 6 field widths + 27 wire KEEP: zero change)",
    "numeric coincidence != semantic coincidence",
    "L bucket = same-field original logic only, no wire touch",
    "gate-5: no imagined modification (evidence before code)",
    "gate-18: REA->IDA->orange-wz(128B)->.nx read-only first",
    "plan round: ZERO source change, implementation starts after user confirms the plan",
  ],
}
writeb(EV, (json.dumps(ev, ensure_ascii=False, indent=2) + "\n").encode("utf-8"))

# ---------------------------------------------------------------- NEXT-BATCH-PLAN.md (main deliverable)
P = []
P.append("# round91 · 下一批一次性完整 Plan（计划轮 · 零源码改动）")
P.append("")
P.append("> 触发：User said (m10739)「开始 下一批 的计划」；User said (m10738)「记录一次有可见 PUA 输出的交付，不记录评分。」")
P.append("> 门禁 1：先证据 → 分析 → 抉择 → 一次性 Plan；**用户确认计划后才进入实现**。本轮不落任何源码补丁。")
P.append("> 权威源：Angel.exe(13339) 083/v83 带符号 IDB；s9 源码只约束 W 桶字段宽度；本端 HeavenClient 自研重实现。")
P.append("")
P.append("## 0. 结论先行（别把计划当交付）")
P.append("")
P.append("**WLE-TRIAGE-PLAN.md §5/:123 与 §7/:152-153 登记的「下一批 L 桶 3 条」三条全部不成立，不进 P1。** 这是继 round90 `drop/D2`/`drop/D12` 之后**第三次**「纸面登记 → 现场取证证伪」的同族病。本轮把三条的桶属性用 083 活 IDA + s9 源码 + 当前端代码 + DECISIONS 裁定四方钉死，并给出真正干净的 L 桶候选（每条自带 083 反编译锚点）。")
P.append("")
P.append("| 候选 | round89/90 登记 | round91 判定 | 是否进 P1 |")
P.append("| --- | --- | --- | --- |")
P.append("| `quest/D8` | L 桶 P1（搬 `sub_721008` 分支） | **W 桶**：107 包条件追加 4B x/y，wire 布局 | ❌ 保持 s9 |")
P.append("| `skill/D4` | L 桶 P1（新增 `CWndSkillGuide` 子窗） | **架构级**单点裁决：是否新增整窗族 | ❌ 不新增 |")
P.append("| `corewindow/D8` | L 桶 P1（覆盖度对照） | **DOC_ONLY 已发货**（round75 `c07b1a7b`） | ❌ 只补证据 |")
P.append("")
P.append("## 1. 本轮证据链（门禁 18 全走完）")
P.append("")
P.append("| 层 | 证据 | 落盘 |")
P.append("| --- | --- | --- |")
P.append("| REA / IDA | 083 活 IDB `http://127.0.0.1:13339/mcp`，12 份反编译 | `tools/_ida_r91/*.code.txt` |")
P.append("| s9 源码 | `QuestActionHandler.java` / `DistributeSPHandler.java` / `AutoAssignHandler.java` / `KeymapChangeHandler.java` / `QuickslotKeyMappedModifiedHandler.java` | 逐字引于 §2 |")
P.append("| 当前端 | `Net/Packets/QuestPackets.h`、`UISkillBook.cpp`、`UIStatsInfo.cpp`、`UIKeyConfig.cpp` | 逐字引于 §2 |")
P.append("| 历史裁定 | `DECISIONS.md:99/190/251/293/311`、矩阵 `:152/:243/:304` | 逐字引于 §2 |")
P.append("| orange-wz IMG | 128B `WzAESConstant.DEFAULT_KEY` base64（172 字符）+ iv `TSPHKw==`，`UIWindow.img` 的 `KeyConfig`/`Stat`/`Skill` 三根全可达、`isError=false` | `tools/_ida_r91/img_gate.json` → `" + gate["verdict"] + "` |")
P.append("| .nx | 本轮判定不依赖资源布局（见下），无需回退 | — |")
P.append("")
P.append("**IMG 门禁结论**：`GATE_OK_128B_IMG_REACHABLE`（KeyConfig len=548 / Stat len=2138 / Skill len=4242，全 `isError=false`）。三条判定靠**反编译 + s9 源码 + 当前端代码 + 裁定**，均不依赖 IMG 资源布局；本次仅按门禁 18 验证 128B 通道可达且三根存在。**注意口径**：orange-wz MCP 路径用 128B `DEFAULT_KEY`，`.nx`/nlnx/`tools/_wz.py` 才用 65536B `key_gms`（AGENTS.md:23 双规格）。")
P.append("")
P.append("## 2. 三条候选四方对拍（逐条）")
P.append("")
P.append("### 2.1 `quest/D8` — `sub_721008` 是 wire 条件，不是纯逻辑")
P.append("")
P.append("**083 反编译逐字**（`tools/_ida_r91/sub_721008.code.txt`，`0x721008`）：")
P.append("")
P.append("```c")
for x in q_sub:
    P.append("  " + x)
P.append("```")
P.append("")
P.append("**083 发包链逐字**（`tools/_ida_r91/CUIQuestInfo_StartQuest_action.code.txt`，`CQuest::StartQuest @0x71751c`，action=1 start / action=2 complete）：")
P.append("")
P.append("```c")
for x in q_start:
    if any(k in x for k in ["COutPacket(a1: 107)", "Encode1(this:", "Encode2(a1: *(this + 6))", "Encode4(this:", "if ( sub_721008", "Encode2(a1: v50)", "Encode2(a1: v51)", "Encode4(this: &v52, a2: v30)", "SendPacket"]):
        P.append("  " + x)
P.append("```")
P.append("")
P.append("即：`COutPacket(107)` → `Encode1(action)` → `Encode2(questid)` → `Encode4(npcid)` → **`if (sub_721008(questid)==0) { Encode2(x); Encode2(y); }`** → `SendPacket`。`sub_721008` 语义 = `IsAutoStartQuest(questid) || sub_720FED(questid)`（旧「函数名无符号语义待补 L2」本轮补齐）；自动开始/脚本链接任务**不追加 x/y**，普通任务追加 4B。")
P.append("")
P.append("**s9 逐字**（`QuestActionHandler.java:48-64`）：")
P.append("")
P.append("```java")
P.append("private static boolean isNpcNearby(InPacket p, Character player, Quest quest, int npcId) {")
P.append("    Point playerP; Point pos = player.getPosition();")
P.append("    if (p.available() >= 4) {")
P.append("        playerP = new Point(p.readShort(), p.readShort());")
P.append("        if (playerP.distance(pos) > 1000) { playerP = pos; }")
P.append("    } else { playerP = pos; }")
P.append("    ...")
P.append("}")
P.append("```")
P.append("")
P.append("s9 **同时容忍有/无 x/y 两种包体**（`available()` 在 Java 侧是真实字节数）。当前端 `Net/Packets/QuestPackets.h` 布局 `byte action, short questid [, int npcid]`，从不写 x/y = 合法 s9 形态。")
P.append("")
P.append("**裁定**：`quest/D8` 是 **W 桶 wire 条件追加段**，不是「同字段原始逻辑」。搬进来 = 改 c2s 107 包形 = 协议级改动，且 s9 已接受当前形态。矩阵 `:304`「不搬」+ `DECISIONS.md:251` `BATCH_1_KEEP` 正确。**不进 P1，保持 s9，零改动。**")
P.append("")
P.append("### 2.2 `skill/D4` — 架构级「是否新增整窗族」，不是 L 桶")
P.append("")
P.append("**083 反编译逐字**（`tools/_ida_r91/CWndSkillGuide_OnKey.code.txt`，`0x8aa4e4`）：")
P.append("")
P.append("```c")
for x in q_guide:
    P.append("  " + x)
P.append("```")
P.append("")
P.append("全库 `*CWndSkillGuide*` 仅 2 符号（ctor `0x8aa243` / `OnKey 0x8aa4e4`）。当前端 `IO/UITypes/UISkillBook.cpp:797` `send_key` escape→`set_skillpoint(false)`/`close()` 已闭环，`docs/features/skill-angel-calibration-2026-11/README.md:79` A4 已判 ✅ 一致。")
P.append("")
P.append("**裁定**：这是「要不要新增一整个独立子窗族」的**架构级单点裁决**，不是「同字段原始逻辑还原」。`WLE-TRIAGE-PLAN.md:133` 自己写明「架构级重写…不在自动落地范围」。矩阵 `:243`「不新增」+ `DECISIONS.md:190` `BATCH_1_KEEP` 正确。**不进 P1；只有用户单点重开才评估。**")
P.append("")
P.append("### 2.3 `corewindow/D8` — K3 DOC_ONLY，round75 已发货对照清单")
P.append("")
P.append("083 三份反编译本轮全部拉齐落盘：")
P.append("")
P.append("| 083 函数 | 地址 | 逐字要点 | 文件 |")
P.append("| --- | --- | --- | --- |")
P.append("| `CUIStat::OnButtonClicked` | `0x8c522f` | 10 case：6 掩码 `2048/0x2000/64/128/256/512`→`SendAbilityUpRequest`；2006 ToggleDetail；2007/8 AutoApUp(1)；2009 AutoApUp(0) | `tools/_ida_r91/CUIStat_OnButtonClicked.code.txt` |")
P.append("| `CUIKeyConfig::OnButtonClicked` | `0x8335d0` | 5 分支：1=Save+UI_Close(5) / 2=InvalidateRect(StatusBar)+UI_Close(5) / 2000=DoModal→Default+ReDrawQuickslot / 2001=DoModal→memset(445)+InvalidateRect / 2002=sub_836C9B | `tools/_ida_r91/CUIKeyConfig_OnButtonClicked.code.txt` |")
P.append("| `CUISkill::OnSkillLevelUpButton` | `0x8acf1a` | `CanSendExclRequest(500,0)` 独占门闩 + `SP_3509/3510/3511` 三档前置 | `tools/_ida_r91/CUISkill_OnSkillLevelUpButton.code.txt` |")
P.append("| `CWvsContext::CanSendExclRequest` | `0x485bf7` | `" + (q_excl[0] if q_excl else "get_update_time() - *(this + 2090) >= a2") + "` | `tools/_ida_r91/CanSendExclRequest.code.txt` |")
P.append("")
P.append("**s9 对拍**：`DistributeSPHandler` 只 `readInt(); readInt(skillid)` → `AssignSPProcessor.SPAssignAction`，**无 500ms 限速**；`AutoAssignHandler` → `APAutoAssignAction` 走 s9 Stat 5-byte 编码；`KeymapChangeHandler` / `QuickslotKeyMappedModifiedHandler` 是 s9 私有形态。")
P.append("")
P.append("**当前端对拍**：`Net/Packets/PlayerPackets.h:48-72` 注释已明写「本包 type 编码与 SPEND_AP 的 `MapleStat::codes` **不是同一套**（s9 Stat 5-byte 编码），勿复用」，`StatCode` 只 4 个（`STR=64/DEX=128/INT=256/LUK=512`）——083 的 `2048/0x2000` 是 HP/MP 系，s9 没有；`UISkillBook.cpp:545-560` BT_OKAY 无 500ms 门闩**正是 s9 正确实现**；`UIKeyConfig.cpp:731-810` 6 个 case 对齐 083 5 分支 + 本端适配。")
P.append("")
P.append("**裁定**：矩阵 `:152` K3「先出逐条对照清单，**不改代码**」+ `DECISIONS.md:99/293/311`「round75 `c07b1a7b` 已复核，只出对照清单不改代码」= **DOC_ONLY 已发货**。本轮把 083 逐字对照补进 §2.3 表 + evidence（这正是原裁决要的产出），**零源码改动，不进 P1**。")
P.append("")
P.append("## 3. 同族病模式（P8 揪头发：一类问题出去）")
P.append("")
P.append("WLE-TRIAGE-PLAN.md 三处纸面登记被现场取证证伪：")
P.append("")
P.append("- `:96`「`quest/D8`、`skill/D4`、`corewindow/D8` 等状态机分支同理，逐面 REA→IDA→IMG 后再落」")
P.append("- `:123`「改走真正有 083 状态机证据且**零 wire** 的 L 桶候选」")
P.append("- `:152-153`「**同族审计**：…均为状态机/条件分支、不碰 wire，属合法 L 桶候选，**无同族误分类**」")
P.append("")
P.append("**根因**：从 D 矩阵「推荐默认文本」直接抄进 P1 登记层，没有先跑该条的 083 反编译 + s9 handler + 当前端字面量。round90 已在 `drop/D2`/`D12` 上犯过一次，round91 是**第三次**（§7 那句「无同族误分类」本身又埋了一次）。")
P.append("")
P.append("**举一反三（写进 Plan 的硬规则）**：任何「下一批 L 桶」登记，必须先附 ① 083 符号 + 反编译文件路径与逐字关键行 ② s9 对应 handler 的 body ③ 当前端对应字面量 ④ DECISIONS 原裁定，四项齐了才准写进 §5。**不允许从矩阵文本直接推导批次。**")
P.append("")
P.append("## 4. 下一批真正干净的 L 桶候选（每条自带 083 锚点）")
P.append("")
P.append("| 优先 | 决策点 | 083 符号 / 反编译 | 为何干净 | 门槛 |")
P.append("| --- | --- | --- | --- | --- |")
P.append("| **P1 首选** | `skill/D12` / `corewindow/D9` 宏系统选中态 / ESC | `CUIMacroSysEx::OnSelected @0x8a984d`（20 行）、`CUIMacroSys::OnKey @0x8a98dc`（8 行） | 纯 UI 状态机：`*(this+361)=a2` → `GetMacroName` → `IsShoutMacro` → `SetChecked` → `ResetInfo`；ESC→`ProcessBasicUIKey(27,a2)`。**零 `COutPacket` / 零 `Encode` / 零 `SendPacket`** | REA→IDA→orange-wz(128B)→.nx 只读取证后，纯逻辑补丁，一片一原子 commit |")
P.append("| **P1 次选** | `login-flow/D9` / `statusbar-hud/D12` 频道网格键盘导航 | `CUIChannelShift::OnKey @0x995863`（85 行）、`CUIChannelSelect::OnKey @0x628a1c`（135 行） | `CUIChannelShift` 是 6 列方向键纯索引计算（case `0x25/0x26/0x27/0x28`），零发包。**`CUIChannelSelect` 必须拆**：导航段=L，`SendViewAllCharacterPacket` 段=W（s9 优先保持） | **先拆 L/W 再动手**，只做导航 |")
P.append("| **P2 / F 维** | 宏画布态 | `CUISkillEx::ShiftMacroUIState @0x8bad5a`（243 行） | COM `IWz` 画布 / `Animate` 栈，当前端无 COM 画布栈 ⇒ 与 round90 `drop/D2` 同族 F 维 | **不补**，登记 P2，禁想象修改 |")
P.append("")
P.append("反编译全文：`tools/_ida_r91/CUIMacroSysEx_OnSelected.code.txt`、`CUIMacroSys_OnKey.code.txt`、`CUIChannelShift_OnKey.code.txt`、`CUIChannelSelect_OnKey.code.txt`、`CUISkillEx_ShiftMacroUIState.code.txt`。")
P.append("")
P.append("## 5. 一次性执行 Plan（**待用户确认后**逐片实施）")
P.append("")
P.append("| # | 批次 | 动作 | 验收信号 |")
P.append("| --- | --- | --- | --- |")
P.append("| 1 | 本轮（已落地） | 三条候选四方取证 + 桶属性纠偏 + 本一次性 Plan + evidence/notes/dev 2-39/PLT §50 + harness + GATE + build 门禁回归 + 零署名提交 | 计划轮**零源码改动**；harness 全绿；`GATE OK s9 a78ba681a4`；提交签名行 0 |")
P.append("| 2 | 下一批 · 片 1 | `skill/D12`/`corewindow/D9` 宏选中态：先 REA→IDA→orange-wz(128B)→.nx 只读取证，再落纯逻辑补丁（零 wire） | 083 `CUIMacroSysEx::OnSelected` 逐字为凭；harness + GATE + 真重编三证（touch + build_log size/mtime + EXE mtime + 覆盖）+ 零署名原子提交 |")
P.append("| 3 | 下一批 · 片 2 | `CUIChannelShift` 6 列导航纯索引计算；`CUIChannelSelect` 先拆 L/W，只落 L 段 | 同上；`SendViewAllCharacterPacket` 段零改动 |")
P.append("| 4 | 逐面推进 | reopen 剩余 P1 按 12 面分批，**每批先补 §3 四项证据** | 每片一原子 commit；证据不足登记 P2/P3 不发明 |")
P.append("| 5 | 全程 | W 桶 6 字段宽度/类型 + 27 条 wire KEEP **零改动**；`quest/D8`/`skill/D4`/`corewindow/D8` 零改动 | 不复现 `LoginParser.cpp:150-174` 白屏 |")
P.append("| 6 | 真机归口 | 五屏截图 60 节/142 处、`InPacket::available()` 393 处 §27-A、D2 聊天规则 | 用户认领后销案 |")
P.append("")
P.append("## 6. 明确不做（写死，防止半路扩张）")
P.append("")
P.append("- ❌ 不改 `Net/Packets/QuestPackets.h` 给 107 包追加 x/y（W 桶 / 断联机风险）")
P.append("- ❌ 不新增 `CWndSkillGuide` 独立子窗（架构级，需单点裁决）")
P.append("- ❌ 不给 `UISkillBook.cpp` 补 500ms 门闩（s9 无限速，补了反而是错）")
P.append("- ❌ 不把 083 的 `2048/0x2000` 掩码塞进 `AutoAssignApPacket::StatCode`（编码体系不同）")
P.append("- ❌ 不在计划轮改任何 `.cpp` / `.h`（门禁 1：确认计划后才实现）")
P.append("- ❌ 不写 commit hash / `git log` 进 evidence 或 Plan（round88 B9b 漂移教训）")
P.append("")
P.append("## 7. 算术与限制")
P.append("")
P.append("- 168 条 `DECISIONS.md` 历史裁定**不动**；WLE §4 `38 + 130 = 168` **不动**；reopen `27 = 25 P1 + 1 W + 1 F-P2`（round90）**不动**。本轮只在 **P1 登记层**注销 3 条，不做语义再分类造假（登记层 ≠ 裁定层）。")
P.append("- 本轮 **0 源码行为改动 / 0 wire / 0 协议 / 0 WZ / 0 新封包**；`tools/_ida_r91/` 只读取证留痕。")
P.append("- 外部程序在途 WIP 原样留工作树，精确 `git add` 隔离、零夹带。")
P.append("- 真机五屏截图 60 节/142 处、`InPacket::available()` 393 处 §27-A 逐面回归、D2 聊天规则，仍由用户认领。")
P.append("- 下一批实施前须用户确认本 Plan；确认后按 §5 逐片推进，每片一片一原子 commit。")
P.append("")
writeb(PLAN, lf_text(P))

# ---------------------------------------------------------------- WLE-TRIAGE-PLAN.md append §8
wle_b = readb(WLE)
wle = wle_b.decode("utf-8")
if "## 8. round91" not in wle:
    S = []
    S.append("## 8. round91 纠偏 — 下一批 3 条候选的四方对账（m10739）")
    S.append("")
    S.append("触发：用户「开始 下一批 的计划」→ 按门禁 1 先证据后计划，对 §2.3 :96 / §5 :123 /")
    S.append("§7 :152-153 登记的 3 条「合法 L 桶候选」做 083 活 IDA + s9 源码 + 当前端 + DECISIONS 四方复核。")
    S.append("**结论：三条全部不成立，不进 P1。这是继 round90 之后第三次同族误登记。**")
    S.append("")
    S.append("| 项 | round89/90 登记 | round91 判定 | 决定性证据 |")
    S.append("| --- | --- | --- | --- |")
    S.append("| `quest/D8` | L 桶 P1（搬 `sub_721008` 分支） | **W 桶 wire 条件追加段**，保持 s9 | 083 `CQuest::StartQuest @0x71751c` `COutPacket(107)` → `if (sub_721008(questid)==0){ Encode2(x); Encode2(y); }`；`sub_721008 = IsAutoStartQuest || sub_720FED`；s9 `QuestActionHandler.isNpcNearby` `if (p.available() >= 4)` 容忍有/无 x/y；当前端 `Net/Packets/QuestPackets.h` 从不写 x/y = 合法 s9 形态 |")
    S.append("| `skill/D4` | L 桶 P1（新增 `CWndSkillGuide` 子窗） | **架构级单点裁决**，不新增 | 083 全库仅 2 符号（ctor `0x8aa243` / `OnKey 0x8aa4e4` `a2==27`→`CloseSkillGuide`）；当前端 `UISkillBook.cpp:797` ESC 已闭环；矩阵 `:243` / `DECISIONS.md:190` 已裁「不新增」；§6 自排除架构重写 |")
    S.append("| `corewindow/D8` | L 桶 P1（覆盖度对照） | **DOC_ONLY 已发货**（round75 `c07b1a7b`） | 083 三份反编译（`CUIStat::OnButtonClicked 0x8c522f` 6 掩码 / `CUIKeyConfig::OnButtonClicked 0x8335d0` 5 分支 / `CUISkill::OnSkillLevelUpButton 0x8acf1a` + `CanSendExclRequest 0x485bf7` 500ms）；s9 `DistributeSPHandler` 无 500ms 限速 + `AssignAPProcessor` 不同编码；当前端 `PlayerPackets.h:48-72` 已注明两套编码勿复用；矩阵 `:152` / `DECISIONS.md:99/293/311` 裁「只出对照清单不改代码」 |")
    S.append("")
    S.append("**作废旧结论（历史文本保留，以本节为准）**：§2.3 :96「等状态机分支同理」、§5 :123「改走真正有 083 状态机证据且零 wire 的 L 桶候选（`quest/D8`/`skill/D4`/`corewindow/D8`）」、§7 :152-153「同族审计…均为状态机/条件分支、不碰 wire，属合法 L 桶候选，无同族误分类」。")
    S.append("")
    S.append("**同族病根因与硬规则**：从 D 矩阵「推荐默认文本」直接抄进 P1 登记层，没先跑该条的 083 反编译 + s9 handler + 当前端字面量 + DECISIONS 原裁定。今后任何「下一批 L 桶」登记必须先附这四项，不允许从矩阵文本推导批次。")
    S.append("")
    S.append("**真正干净的下一批（每条自带 083 反编译锚点，详见 `NEXT-BATCH-PLAN.md §4`）**：")
    S.append("")
    S.append("- P1 首选 `skill/D12`/`corewindow/D9` 宏选中态：`CUIMacroSysEx::OnSelected @0x8a984d`（20 行纯 UI，`*(this+361)=a2`/`GetMacroName`/`IsShoutMacro`/`ResetInfo`）+ `CUIMacroSys::OnKey @0x8a98dc`（ESC→`ProcessBasicUIKey(27,a2)`），零 `COutPacket`/零 `Encode`/零 `SendPacket`。")
    S.append("- P1 次选 `login-flow/D9`/`statusbar-hud/D12` 频道网格：`CUIChannelShift::OnKey @0x995863` 6 列方向键纯索引；`CUIChannelSelect::OnKey @0x628a1c` **须先拆 L/W**（导航=L，`SendViewAllCharacterPacket`=W 保持 s9）。")
    S.append("- P2/F 维 `CUISkillEx::ShiftMacroUIState @0x8bad5a`（243 行 COM 画布栈，当前端无 COM ⇒ 不补）。")
    S.append("")
    S.append("**算术不动**：168 条历史裁定 / §4 `38+130=168` / reopen `27 = 25 P1 + 1 W + 1 F-P2` 全不变；本轮只在 P1 **登记层**注销 3 条，不做语义再分类造假。")
    S.append("**限制**：0 源码 / 0 wire / 0 协议 / 0 WZ / 0 新封包；计划轮，实施待用户确认一次性 Plan。")
    new_wle = wle.rstrip("\n") + "\n\n" + "\n".join(S).rstrip("\n") + "\n"
    writeb(WLE, new_wle.encode("utf-8"))

# ---------------------------------------------------------------- notes
N = []
N.append("# round91 · 下一批一次性 Plan 取证笔记（wave3）")
N.append("")
N.append("日期：2026-11-11　触发：m10739「开始 下一批 的计划」+ m10738「记录一次有可见 PUA 输出的交付，不记录评分。」")
N.append("")
N.append("## 1. 做了什么")
N.append("")
N.append("对 WLE-TRIAGE-PLAN.md 登记的「下一批 L 桶 3 条」做四方复核（083 活 IDA 12 份反编译 / s9 5 个 handler / 当前端 4 个文件 / DECISIONS 5 行裁定），证伪三条全部；按门禁 1 产出一次性 Plan（计划轮零源码改动）。")
N.append("")
N.append("## 2. 取证清单")
N.append("")
N.append("| 工具 | 命令 / 文件 | 结果 |")
N.append("| --- | --- | --- |")
N.append("| `tools/_r91_ida.py` | 083 活 IDB symbol query + 6 处 decompile | `tools/_ida_r91/*.code.txt` |")
N.append("| `tools/_r91_ida2.py` | 10 组候选符号密度 | `*CUIChannel*` 6 / `*CUIMacro*` 17 / `*ShiftMacroUIState*` 1 |")
N.append("| `tools/_r91_ida3.py` | 5 处新候选 decompile | 宏 / 频道网格反编译落盘 |")
N.append("| `tools/_r91_img_gate.py` | orange-wz 128B `DEFAULT_KEY` + `UIWindow.img` 三根 | **GATE_OK_128B_IMG_REACHABLE** |")
N.append("| `tools/_r91_deliver.py` | 幂等生成器 | evidence / Plan / WLE §8 / dev 2-39 / PLT §50 / notes |")
N.append("")
N.append("## 3. 三条判定（一句话各一条）")
N.append("")
N.append("- `quest/D8`：`sub_721008` 控的是 c2s 107 包尾部 4B x/y 的**条件追加**，是 wire 不是逻辑；s9 用 `available() >= 4` 同时容忍两种形态，当前端无后缀合法 ⇒ W 桶保持。")
N.append("- `skill/D4`：问的是「要不要新增一整个独立子窗族」，架构级单点裁决；083 全库只 2 个符号，当前端 ESC 已闭环 ⇒ 不新增。")
N.append("- `corewindow/D8`：K3 DOC_ONLY，round75 已发货；083 的 500ms 门闩 / 6 掩码 / 5 分支是参考值，s9 无限速且 Stat 编码不同，当前端实现**正是 s9 正确形态** ⇒ 只补逐字对照，不改代码。")
N.append("")
N.append("## 4. 同族病（第三次）")
N.append("")
N.append("round90 挖出 `drop/D2`+`D12` 误登记，round91 又挖出 §7 自己写的「无同族误分类」也是误判。根因=从矩阵推荐默认文本直接抄进 P1 登记层。硬规则已写进 `NEXT-BATCH-PLAN.md §3` 与 WLE §8：登记前必须附 083 反编译 + s9 handler + 当前端字面量 + DECISIONS 原裁定四项。")
N.append("")
N.append("## 5. 限制与验收")
N.append("")
N.append("- 0 源码 / 0 wire / 0 协议 / 0 WZ / 0 新封包；计划轮，用户确认 Plan 后才实现。")
N.append("- 交付物不写 commit hash / `git log`（round88 B9b）。")
N.append("- IMG 门禁：128B `WzAESConstant.DEFAULT_KEY` base64（172 字符）+ iv `TSPHKw==`；`.nx` 路径才用 65536B `key_gms`。")
N.append("- 验收：harness 静态守卫 + 场景全绿、`python tools/gen_opcode_matrix.py --check` GATE OK、build 门禁回归、零署名精确 add 提交、feedback 无 rating、goal 保持 active。")
N.append("")
writeb(NOTES, lf_text(N))

# ---------------------------------------------------------------- dev-plan §4 row 2-39
dev_b = readb(DEV)
dev = dev_b.decode("utf-8")
if "| **2-39** |" not in dev:
    lines = dev.split("\n")
    idx = None
    for i, l in enumerate(lines):
        if l.startswith("| **2-38** |"):
            idx = i
            break
    assert idx is not None, "dev-plan 2-38 anchor missing"
    row = ("| **2-39** | round91 下一批一次性 Plan（m10739） | "
           "**三方取证证伪 WLE §5/:123 + §7/:152-153 登记的 3 条「L 桶 P1」**："
           "`quest/D8` = W 桶（083 `CQuest::StartQuest @0x71751c` `COutPacket(107)` 条件追加 4B x/y，"
           "s9 `QuestActionHandler.isNpcNearby` 容忍两形态）；"
           "`skill/D4` = 架构级（083 `CWndSkillGuide::OnKey @0x8aa4e4` 仅 2 符号，当前端 ESC 已闭环）；"
           "`corewindow/D8` = DOC_ONLY 已发货（round75 `c07b1a7b`，083 6 掩码/500ms/5 分支属参考值，"
           "s9 `DistributeSPHandler` 无限速）。三次同族误登记（round90 drop + round91 三条）根因=从矩阵文本抄 P1；"
           "硬规则=登记前必附 083 反编译+s9 handler+当前端字面量+DECISIONS 四项。"
           "下一批改走真干净 L 桶 `CUIMacroSysEx::OnSelected @0x8a984d` / `CUIMacroSys::OnKey @0x8a98dc` / "
           "`CUIChannelShift::OnKey @0x995863`（零 COutPacket/Encode/SendPacket）。"
           "交付 `NEXT-BATCH-PLAN.md` + evidence/notes/WLE §8/PLT §50/harness；"
           "orange-wz 128B 门禁 `GATE_OK_128B_IMG_REACHABLE`；**计划轮 0 源码改动，实施待用户确认 Plan** |")
    lines.insert(idx + 1, row)
    writeb(DEV, ("\n".join(lines).rstrip("\n") + "\n").encode("utf-8"))

# ---------------------------------------------------------------- PLT §50 (CRLF)
plt_b = readb(PLT)
plt = plt_b.decode("utf-8")
if "## 50. round91" not in plt:
    S = []
    S.append("## 50. round91 · 下一批一次性 Plan（3 条候选纠偏 / 计划轮零源码改动）")
    S.append("")
    S.append("触发：m10739「开始 下一批 的计划」+ m10738「记录一次有可见 PUA 输出的交付，不记录评分。」")
    S.append("主交付：`docs/features/angel-calibration-d-matrix-2026-11/NEXT-BATCH-PLAN.md`（门禁 1：确认计划后才实现）。")
    S.append("")
    S.append("50-1. **3 条候选全部不进 P1**：`quest/D8` = W 桶 wire 条件追加段（083 `CQuest::StartQuest @0x71751c` `COutPacket(107)` → `if (sub_721008(questid)==0){Encode2(x);Encode2(y);}`；`sub_721008 = IsAutoStartQuest || sub_720FED`；s9 `QuestActionHandler.java:48-64` `if (p.available() >= 4)` 容忍有/无 x/y；当前端 `Net/Packets/QuestPackets.h` 无后缀=合法 s9 形态）→ 保持 s9 零改动；`skill/D4` = 架构级（`CWndSkillGuide::OnKey @0x8aa4e4` 全库 2 符号，当前端 `UISkillBook.cpp:797` ESC 闭环）→ 不新增；`corewindow/D8` = K3 DOC_ONLY 已发货（round75 `c07b1a7b`），083 6 掩码/500ms/5 分支属参考值，s9 `DistributeSPHandler` 无 500ms 限速 + `AssignAPProcessor` 不同编码，当前端 `PlayerPackets.h:48-72` 已注明勿复用 → 只补逐字对照。")
    S.append("50-2. **同族病第三次**：WLE-TRIAGE-PLAN.md `:96/:123/:152-153` 三处纸面登记（含「无同族误分类」）被 083 活 IDA 12 份反编译 + s9 5 handler + 当前端 + DECISIONS 四方证伪。硬规则写进 Plan §3 / WLE §8：任何「下一批 L 桶」登记前必附 083 反编译 + s9 handler + 当前端字面量 + DECISIONS 原裁定四项，不从矩阵文本推导批次。")
    S.append("50-3. **真干净下一批（自带 083 锚点）**：P1 首选 `skill/D12`/`corewindow/D9` 宏选中态 `CUIMacroSysEx::OnSelected @0x8a984d`（20 行纯 UI）+ `CUIMacroSys::OnKey @0x8a98dc`（ESC→`ProcessBasicUIKey(27,a2)`），零 `COutPacket`/`Encode`/`SendPacket`；P1 次选 `CUIChannelShift::OnKey @0x995863` 6 列方向键纯索引，`CUIChannelSelect::OnKey @0x628a1c` 须先拆 L/W（导航=L / `SendViewAllCharacterPacket`=W 保持 s9）；P2/F 维 `CUISkillEx::ShiftMacroUIState @0x8bad5a`（243 行 COM 画布栈，当前端无 COM 不补）。")
    S.append("50-4. **门禁与算术**：orange-wz 128B `WzAESConstant.DEFAULT_KEY` base64（172 字符）+ iv `TSPHKw==` 对 `UIWindow.img` 的 `KeyConfig`/`Stat`/`Skill` 三根全 `isError=false` → `GATE_OK_128B_IMG_REACHABLE`（`tools/_ida_r91/img_gate.json`）；168 条 DECISIONS 历史裁定 / WLE §4 `38+130=168` / reopen `27 = 25 P1 + 1 W + 1 F-P2` **全不动**，仅 P1 登记层注销 3 条；`GATE OK s9 a78ba681a4`；harness 全绿；零署名原子提交。")
    S.append("50-5. **限制**：计划轮 **0 源码 / 0 wire / 0 协议 / 0 WZ / 0 新封包**；`tools/_ida_r91/` 只读取证留痕；工作树外部在途 WIP 原样保留、精确 add 隔离；真机五屏截图 60 节/142 处、`InPacket::available()` 393 处 §27-A 逐面回归、D2 聊天规则仍由用户认领；下一批实施前须用户确认一次性 Plan。")
    S.append("")
    seg = "\r\n".join(S).rstrip("\r\n")
    new_plt = plt.rstrip("\r\n") + "\r\n" + seg + "\r\n"
    writeb(PLT, new_plt.encode("utf-8"))

# ---------------------------------------------------------------- verify
for p in [WLE, DEV, PLT, EV, PLAN, NOTES]:
    assert os.path.isfile(rp(p)), "missing deliverable: " + p
b_json = json.loads(readb(EV).decode("utf-8"))
assert len(b_json["candidates"]) == 3
assert len(b_json["ida_evidence_files"]) == 12
assert b_json["zero_source_change"] is True

hashes = {p: sha(readb(p)) for p in [WLE, DEV, PLT, EV, PLAN, NOTES]}
print("ROUND91_DELIVER_ALL_OK")
for k, v in hashes.items():
    print("  SHA", v, k)
