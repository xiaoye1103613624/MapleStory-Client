# -*- coding: utf-8 -*-
"""round94 · reopen 残余 17 条 P1/P2/P3 分流（账目轮，零源码改动）

用户 m11928：「处理掉：剩余 reopen 中 21 条属 P1/P2/P3 分流」。
现场先纠算：27 条 reopen 中已被前四轮注销 10 条（round90 2 + round91 3 + round92 4 + round93 1），
残余 17 条本轮逐条分流。产物：
  1) WLE-TRIAGE-PLAN.md §11 分流表
  2) docs/features/development-and-fix-plan-2026-11/README.md §4 表 2-42 行
  3) docs/features/pending-live-tests/README.md §53
  4) docs/features/angel-calibration-d-matrix-2026-11/evidence/reopen-triage-round94.json
  5) docs/features/development-and-fix-plan-2026-11/notes/wave3-reopen-triage-20261111.md

约束：不写 commit hash / git log（round88 B9b）；EOL 按原文件精确保留；幂等。
"""
import io
import json
import os

os.chdir(r"E:/project/MapleStory-Client")

WLE = "docs/features/angel-calibration-d-matrix-2026-11/WLE-TRIAGE-PLAN.md"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
EV = "docs/features/angel-calibration-d-matrix-2026-11/evidence/reopen-triage-round94.json"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave3-reopen-triage-20261111.md"


def rd(p):
    with open(p, "rb") as f:
        return f.read()


def wr(p, b):
    with open(p, "wb") as f:
        f.write(b)


def eol_of(b):
    crlf = b.count(b"\r\n")
    lone = b.count(b"\n") - crlf
    return ("\r\n" if crlf > lone else "\n")


# ---------------------------------------------------------------- 分流数据
# verdict 沿用 round88 canonical 裁定原文，不发明新结论。
ROWS = [
    # face, id, kind, cls, canonical(原文截), bucket, action, why
    ("login-flow", "D3", "K3", "PURE", "骑士团条件解码分支 | 先确认 s9 是否发追加段再补",
     "P2", "等 s9 协议侧确认是否发追加段后再定；本轮不动",
     "缺 s9 侧的追加段证据，按门禁 5 不发明解码分支"),
    ("login-flow", "D12", "K4", "MIXED", "登录发包字段布局 / VAC 编号 | 只做逐字节核对文档、不动代码",
     "P3", "只做逐字节核对文档，代码零改动",
     "字段布局属 W 桶 6 字段范围，改则断联机"),
    ("login-flow", "D13", "K2", "MIXED", "CHARLIST 外层固定 15 槽 vs 按 charcount | 保持 s9",
     "P3", "保持 s9 固定 15 槽",
     "s9 服务端按固定槽发，改按 charcount 会错位"),
    ("statusbar-hud", "D1", "K2", "PURE", "单体 CUIStatusBar → 当前端 10+2 类拆分 | 保持拆分",
     "P3", "保持 10+2 类拆分",
     "合并回单体会重写全部 HUD 类，风险极高且无行为收益"),
    ("corewindow", "D2", "K2", "MIXED", "083 原生语义 vs s9 联机冲突以谁为准 | 保持 s9 联机优先",
     "P3", "保持 s9 联机优先，差异逐条标注",
     "改回 083 原生包形会断联机（LoginParser 白屏实锤）"),
    ("corewindow", "D10", "K2", "MIXED", "CWorldMapDlg 666×524 是否改当前端世界地图 | 保持 s9 布局",
     "P3", "保持 s9 布局，右上玩家列表不判缺失",
     "原生「无在线玩家列表」已坐实，右上列表是 s9 挂接"),
    ("social-economy", "D3", "K2", "PURE", "联盟双入口 + UIGuild 浮窗是否合并回原生 | 保持现状",
     "P3", "保持双入口（round85 已收敛 4 处旁路入口）",
     "083 无独立窗，s9 已用双入口；合并风险极高"),
    ("social-economy", "D9", "K2", "PURE", "UIFriendGroup / 公会徽章编辑 / MapleTV 三整族是否立项 | 保持不做",
     "P3", "明确不做/搁置",
     "083 顶层清单无独立窗、s9 无后端，立项=发明能力"),
    ("social-economy", "D12", "K2", "PURE", "wedding.* 26 键全英文是否译 | 保持决策 B",
     "P3", "保持英文原值、只建键",
     "等 083 String.img / IDA 依据再译，否则自造文案"),
    ("pet-summon", "D2", "K2", "MIXED", "两处 s9 字节丢弃（8B uid→int32、字段读了就丢）| 保持 s9 联机优先",
     "P3", "保持 s9 解包形态",
     "改回原生读 8B 会整体平移后续字段"),
    ("cashshop", "D2", "K2", "PURE", "是否拆 7 子窗 | 不拆，保持单窗",
     "P3", "不拆",
     "单窗功能等价，拆窗是形态重写无收益"),
    ("skill", "D9", "K2", "PURE", "双布局分支 classic 是否收敛为原生单布局 | 保持双布局",
     "P3", "保持双布局",
     "classic 分支服务不同分辨率，收敛会砍可用布局"),
    ("skill", "D11", "K2", "PURE", "BT_HYPER / GUILDSKILL / RIDE 三空钮是否接管 | 保持 DISABLED",
     "P3", "保持 DISABLED",
     "s9 无对应技能通道，接管=发明封包"),
    ("map", "D1", "K2", "MIXED", "SaveFuncKeyMap 89 键差分 vs s9 全量 ChangeKeyMapPacket | 保持 s9 拆法",
     "P3", "保持 s9 全量拆法",
     "s9 走全量包，083 走 89 键差分，改则断联机"),
    ("map", "D4", "K2", "PURE", "083 CWorldMapDlg MFC 大窗 vs 当前端 UIWorldMap 侧栏 | 保持当前端",
     "P3", "保持当前端侧栏 + 付费传送",
     "MFC 大窗依赖 083 窗口框架，重写属架构级"),
    ("map", "D8", "K2", "MIXED", "CHANGE_QUICKSLOT 服务端截断 vs 客户端 8×write_int | 保持既有注记",
     "P3", "保持既有注记，不回改",
     "服务端 (byte)readInt 截断是 s9 真实行为，客户端回改会错位"),
    ("quest", "D5", "K3", "PURE", "是否实现 UIQuestLog::send_key | s9 优先不补",
     "P3", "不补（083 全库 *Quest*OnKey* 命中 0）",
     "083 无 CUIQuestInfo 键盘 handler，补=发明逻辑"),
]

# 已注销（前四轮），用于算术自证：27 - 10 = 17
CLOSED = [
    ("drop", "D2", "round90", "drop-wle-correction", "F 维 P2（缺 COM 渲染栈），非 L 桶"),
    ("drop", "D12", "round90", "drop-wle-correction", "W 桶 wire 宽度分叉（083 读 short delay / s9 写 int cid）"),
    ("corewindow", "D8", "round91", "next-batch-plan", "DOC_ONLY，round75 c07b1a7b 已发货"),
    ("skill", "D4", "round91", "next-batch-plan", "架构级单点裁决（是否新增整窗族）→ 不新增"),
    ("quest", "D8", "round91", "next-batch-plan", "W 桶 wire 条件追加段（sub_721008 控 107 包 4B x/y）"),
    ("corewindow", "D9", "round92", "wle-implementation", "真议题是 32 格→5 槽数量，非选中态状态机"),
    ("skill", "D12", "round92", "wle-implementation", "真议题是 deactivate() 与 ShiftMacroUIState 形态差异"),
    ("login-flow", "D9", "round92", "wle-implementation", "真议题是 60 频道翻页控件（增强 P3）"),
    ("statusbar-hud", "D12", "round92", "wle-implementation", "真议题是 UIMobGage 空 Type（UIBossHP 已覆盖）"),
    ("activity-minigame", "D1", "round93", "citation-fix", "095 注释已按 083 零命中改判本端扩展（E 桶）"),
]

BUCKET_COUNT = {"P1": 0, "P2": 0, "P3": 0}
for r in ROWS:
    BUCKET_COUNT[r[5]] += 1

assert len(ROWS) == 17, len(ROWS)
assert len(CLOSED) == 10, len(CLOSED)
assert len(ROWS) + len(CLOSED) == 27, len(ROWS) + len(CLOSED)
assert sum(BUCKET_COUNT.values()) == 17, BUCKET_COUNT

print("ROWS 17  CLOSED 10  TOTAL 27  BUCKETS", BUCKET_COUNT)

# ---------------------------------------------------------------- WLE §11
wle_new = """## 11. round94 分流 - reopen 残余 17 条定 P1/P2/P3（m11928）

**算术先自证**：27 条 reopen 中前四轮已注销 **10** 条
（round90 `drop/D2`+`drop/D12`；round91 `corewindow/D8`+`skill/D4`+`quest/D8`；
round92 `corewindow/D9`+`skill/D12`+`login-flow/D9`+`statusbar-hud/D12`；round93 `activity-minigame/D1`），
**残余 17 条**本轮逐条落桶：**P1 = 0 / P2 = 1 / P3 = 16**，`0 + 1 + 16 = 17`。

| # | 面 | ID | 类 | 桶 | 处置 | 不落 P1 的理由 |
| --- | --- | --- | --- | --- | --- | --- |
""" + "\n".join(
    "| %d | `%s` | **%s** | %s | **%s** | %s | %s |" % (i, r[0], r[1], r[2], r[5], r[6], r[7])
    for i, r in enumerate(ROWS, 1)
) + """

**P1 = 0 的判定口径**：P1 门槛是「083 反编译有状态机证据 + 零 wire + 当前端确有缺陷」三条同时成立。
残余 17 条的 canonical 裁定原文全部是「保持 / 不补 / 不做 / 不改」，属**已裁定不实施**而非「待做」，
因此没有一条够格进 P1；把它们塞进 P1 才是 round90/91/92 那类「矩阵文本→P1 登记」同族病。

**P2 = 1（`login-flow/D3`）**：唯一真·待证据项——缺 s9 侧是否发追加段的协议事实，
拿到 s9 抓包即可升 P1，拿到前不发明解码分支。

**P3 = 16**：全部为「不改即正确」的现状保持项，登记在册防止后续被重复翻案。
"""

b = rd(WLE)
eol = eol_of(b)
if "## 11. round94 分流" in b.decode("utf-8", "replace"):
    print("[WLE] already present (idempotent)")
else:
    txt = b.decode("utf-8")
    txt = txt.rstrip("\r\n") + eol + wle_new.replace("\n", eol)
    wr(WLE, txt.encode("utf-8"))
    print("[WLE] appended §11  crlf=%d" % rd(WLE).count(b"\r\n"))

# ---------------------------------------------------------------- dev-plan 2-42
dev_row = ("| **2-42** | round94 reopen 残余 17 条 P1/P2/P3 分流（m11928） | "
           "27 条 reopen 先纠算：前四轮已注销 10 条（round90 2 / round91 3 / round92 4 / round93 1），**残余 17 条**。"
           "本轮逐条落桶 **P1 = 0 / P2 = 1 / P3 = 16**（`0+1+16=17`）：唯一 P2 是 `login-flow/D3`（缺 s9 追加段协议事实，"
           "拿到即升 P1）；16 条 P3 全部是 canonical 已裁定「保持/不补/不做」的现状保持项。"
           "**P1=0 的理由**：P1 门槛「083 状态机证据 + 零 wire + 当前端确有缺陷」三条并发，"
           "残余 17 条无一条满足，硬塞 P1 即 round90/91/92 同族病。"
           "**零源码改动**；算术不动（168 条裁定 / `W 38 + L 130 = 168` / reopen 27 分母不变）。 |")

b = rd(DEV)
eol = eol_of(b)
txt = b.decode("utf-8")
if "| **2-42** |" in txt:
    print("[DEV] already present (idempotent)")
else:
    anchor = "| **2-41** |"
    i = txt.find(anchor)
    assert i >= 0, "2-41 anchor not found"
    j = txt.find(eol if eol == "\r\n" else "\n", i)
    assert j > 0, "2-41 line end not found"
    ins = (eol if eol == "\r\n" else "\n") + dev_row
    txt = txt[:j] + ins + txt[j:]
    wr(DEV, txt.encode("utf-8"))
    print("[DEV] inserted 2-42  crlf=%d lonelf=%d" % (rd(DEV).count(b"\r\n"), rd(DEV).count(b"\n") - rd(DEV).count(b"\r\n")))

# ---------------------------------------------------------------- PLT §53
plt_new = """## 53. round94 · reopen 残余 17 条 P1/P2/P3 分流（账目轮 / 零源码改动）

**触发**：User said (m11928)「处理掉：剩余 reopen 中 21 条属 P1/P2/P3 分流」。

**先纠算**：上一轮答复里「21 条」是漏算 round91 的 3 条注销。现场实数：
27 条 reopen 已被前四轮注销 **10** 条 → **残余 17 条**（`27 - 10 = 17`）。

注销台账：

| 轮次 | 注销条数 | 条目 |
| --- | --- | --- |
| round90 | 2 | `drop/D2`（F 维 P2，缺 COM 渲染栈）、`drop/D12`（W 桶 wire 宽度分叉） |
| round91 | 3 | `corewindow/D8`（DOC_ONLY 已发货）、`skill/D4`（架构级 → 不新增）、`quest/D8`（W 桶 wire 条件追加段） |
| round92 | 4 | `corewindow/D9`、`skill/D12`、`login-flow/D9`、`statusbar-hud/D12`（均为**编号错配**，真议题另有其人） |
| round93 | 1 | `activity-minigame/D1`（095 注释已改判本端扩展 E 桶） |

**分流结果**：**P1 = 0 / P2 = 1 / P3 = 16**（算术 `0 + 1 + 16 = 17`）。

- **P2（1 条）**：`login-flow/D3` 骑士团条件解码分支 —— 缺 s9 侧是否发追加段的协议事实；
  拿到 s9 抓包即可升 P1，拿到前按门禁 5 不发明解码分支。
- **P3（16 条）**：`login-flow/D12`、`login-flow/D13`、`statusbar-hud/D1`、`corewindow/D2`、
  `corewindow/D10`、`social-economy/D3`、`social-economy/D9`、`social-economy/D12`、
  `pet-summon/D2`、`cashshop/D2`、`skill/D9`、`skill/D11`、`map/D1`、`map/D4`、`map/D8`、
  `quest/D5` —— 全部是 canonical 已裁定「保持 / 不补 / 不做 / 不改」的现状保持项，登记在册防重复翻案。
- **P1 = 0**：P1 门槛（083 状态机证据 + 零 wire + 当前端确有缺陷）三条并发，残余 17 条无一条满足。

**验证**
- `tools/test_round94_reopen_triage.py`：静态守卫 + 场景全绿（下条命令）。
- `python tools/gen_opcode_matrix.py --check` → `GATE OK`（账目轮零源码改动，不声称真重编）。
- 算术不动：168 条 DECISIONS 裁定 / `PURE76+WIRE38+MIXED14+OTHER40=168` / W 轨 38 + L 轨 130 = 168 /
  reopen 27 分母不变（17 分流 + 10 注销）。
- 外部在途 WIP 零夹带。
"""

b = rd(PLT)
eol = eol_of(b)
txt = b.decode("utf-8")
if "## 53. round94" in txt:
    print("[PLT] already present (idempotent)")
else:
    txt = txt.rstrip("\r\n") + eol + plt_new.replace("\n", eol)
    wr(PLT, txt.encode("utf-8"))
    print("[PLT] appended §53  crlf=%d lonelf=%d" % (rd(PLT).count(b"\r\n"), rd(PLT).count(b"\n") - rd(PLT).count(b"\r\n")))

# ---------------------------------------------------------------- evidence
ev = {
    "round": 94,
    "date": "2026-11-11",
    "trigger": "User said (m11928) 处理掉：剩余 reopen 中 21 条属 P1/P2/P3 分流；DECISIONS.md 11 处待拍板是哪些？算术未动指什么计算？",
    "recount": {
        "reopen_total": 27,
        "closed_previous_rounds": 10,
        "residual": 17,
        "arithmetic": "27 - 10 = 17",
        "correction_note": "上一轮口头说「21 条」是漏算 round91 的 3 条注销（corewindow/D8、skill/D4、quest/D8），本轮纠算为 17。",
    },
    "closed_ledger": [
        {"face": c[0], "id": c[1], "round": c[2], "commit_tag": c[3], "reason": c[4]} for c in CLOSED
    ],
    "triage": [
        {
            "n": i,
            "face": r[0],
            "id": r[1],
            "kind": r[2],
            "cls": r[3],
            "canonical": r[4],
            "bucket": r[5],
            "action": r[6],
            "why_not_p1": r[7],
        }
        for i, r in enumerate(ROWS, 1)
    ],
    "bucket_count": BUCKET_COUNT,
    "bucket_arithmetic": "P1 %d + P2 %d + P3 %d = %d" % (
        BUCKET_COUNT["P1"], BUCKET_COUNT["P2"], BUCKET_COUNT["P3"],
        BUCKET_COUNT["P1"] + BUCKET_COUNT["P2"] + BUCKET_COUNT["P3"],
    ),
    "p1_zero_reason": "P1 门槛 = 083 反编译状态机证据 + 零 wire + 当前端确有缺陷，三条并发；残余 17 条 canonical 裁定全为「保持/不补/不做/不改」，无一条满足。硬塞 P1 即 round90/91/92 同族病（矩阵文本→P1 登记层）。",
    "decisions_pending_11": {
        "total_lines": 11,
        "note": "DECISIONS.md 里仍带「待裁决/待拍板」字样的 11 行；这些行的 canonical 裁定状态已定（KEEP_S9 / KEEP_CURRENT / BATCH_4_STANCE / P0_SHIPPED / P2_LIVE），「待裁决」字样属 round86 批量裁决时的历史措辞，非真未决。",
        "lines": [
            {"line": 132, "face": "pet-summon", "id": "D3", "kind": "K3",
             "topic": "三页宠物装备负槽位穿戴（区间 21–29/31–37/39–45 + -100 偏移）",
             "status": "P0_SHIPPED",
             "resolution": "round78 活 IDA CUIEquip/CUIPetEquip + .nx 真节点已判合法混合面；收口归档，不重写"},
            {"line": 133, "face": "pet-summon", "id": "D4", "kind": "K2",
             "topic": "CActionMan 动作帧 / TamingMob 骑宠实体 / 自动说话 / Lv≥15 / 500 冷却整族",
             "status": "BATCH_4_STANCE",
             "resolution": "维持不做；不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3"},
            {"line": 136, "face": "pet-summon", "id": "D7", "kind": "K3",
             "topic": "4 格改色命中率 34.67% 偏低格",
             "status": "P2_LIVE",
             "resolution": "先出「像素改色 ↔ 原生 disabled」对照，真机补证"},
            {"line": 142, "face": "pet-summon", "id": "D13", "kind": "K3",
             "topic": "龙 die1/filpL/R/skill 帧 + 召唤浮动帧 + CQuestMan 联动 + move_ttl 余晖",
             "status": "BATCH_4_STANCE",
             "resolution": "资源在/不在分列；不发明能力，跨面归属归对应面 P3"},
            {"line": 151, "face": "activity-minigame", "id": "D3", "kind": "K2",
             "topic": "小游戏 DIY 合并壳（Omok 胜负/禁手、MemoryGame 牌面、RPS 状态机、Factory 6 类）",
             "status": "BATCH_4_STANCE",
             "resolution": "维持「wire later」壳，单列补齐计划"},
            {"line": 152, "face": "activity-minigame", "id": "D4", "kind": "K2",
             "topic": "活动场地对象族 / 活动 UI 完整控件表 / Tutor·EventAlarm 壳",
             "status": "BATCH_4_STANCE",
             "resolution": "维持 FieldMarker 黄字 + HUD 壳降级"},
            {"line": 158, "face": "activity-minigame", "id": "D10", "kind": "K3",
             "topic": "295 LEAVE 是否补 OnShowMemberOutMsg 语义（SP_4096/4097）",
             "status": "KEEP_S9",
             "resolution": "s9 无该 opcode，不新增发包"},
            {"line": 248, "face": "quest", "id": "D5", "kind": "K3",
             "topic": "是否实现 UIQuestLog::send_key",
             "status": "KEEP_CURRENT",
             "resolution": "s9 优先，不补当前端未接线键位，登记 P3（083 全库 *Quest*OnKey* 命中 0）"},
            {"line": 265, "face": "drop", "id": "D3", "kind": "K3",
             "topic": "是否实现掉落归属超时（pickuptype/playerdrop 判定）",
             "status": "KEEP_S9",
             "resolution": "s9 无该语义，不补判定"},
            {"line": 271, "face": "drop", "id": "D9", "kind": "K3",
             "topic": "是否补丢金币长确认文案（SP_5280）",
             "status": "KEEP_CURRENT",
             "resolution": "当前短提示可用，长文案属增强 P3"},
            {"line": 272, "face": "drop", "id": "D10", "kind": "K3",
             "topic": "是否处理 DROP_ITEM mod=3（消失掉落）的 dropfrom 语义",
             "status": "KEEP_S9",
             "resolution": "按 s9 移除语义，不改"},
        ],
    },
    "arithmetic_explained": {
        "what_it_is": "**客户端校准决策台账的条数闭合**，不是客户端运行期计算，也不是服务端逻辑。",
        "formulas": [
            "168 = PURE 76 + WIRE 38 + MIXED 14 + OTHER 40（round88 四分类，稳定已证据化）",
            "168 = W 轨 38 + L 轨 130（WLE 三桶路由，W 轨 = class:WIRE）",
            "reopen 27 = 25 P1 候选 + 1 W 桶 + 1 F 维 P2（round90 登记层口径）",
            "reopen 27 = 已注销 10 + 残余 17（本轮）",
            "残余 17 = P1 0 + P2 1 + P3 16（本轮）",
        ],
        "why_it_matters": "任一数字被单方面改动（注销却不记账、语义再分类、把 P2 算进 P1），168 分母或 27 分母就断裂，后续所有轮次的进度百分比失真——所以每轮只动「登记层」，分母永不变。",
        "not_this": [
            "不是客户端代码里的任何数值（伤害公式 / 帧率 / 冷却 / 坐标）",
            "不是服务端 s9 的任何计算",
            "不是 GATE 的 opcode 计数（那个是 148/307/182 等，另一套口径）",
        ],
    },
    "zero_source_change": True,
    "zero_wire_change": True,
    "zero_protocol_change": True,
    "zero_wz_change": True,
    "constraints": [
        "083/v83 only，禁引 095",
        "s9 协议优先（仅 W 桶 6 字段宽度/类型）",
        "数值重合 ≠ 语义重合",
        "校准 ≠ 改回原生（L 桶同字段原始逻辑按 083 还原）",
        "门禁 5：禁想象修改，证据不足登记 P2/P3 不发明",
    ],
}

wr(EV, (json.dumps(ev, ensure_ascii=False, indent=2) + "\n").encode("utf-8"))
print("[EV] wrote", EV, "keys", len(ev))

# ---------------------------------------------------------------- notes
notes = """# round94 · reopen 残余 17 条 P1/P2/P3 分流（账目轮）

**触发**：User said (m11928)「处理掉：剩余 reopen 中 21 条属 P1/P2/P3 分流；DECISIONS.md 11 处待拍板是哪些？算术未动指的是什么计算？客户端还是服务端？」

## 1. 先纠算："21 条" 是错的

上一轮答复说「剩余 reopen 中 21 条」——漏算 round91 的 3 条注销（`corewindow/D8`、`skill/D4`、`quest/D8`）。
现场实数：27 条 reopen 已被前四轮注销 **10** 条，**残余 17 条**。

| 轮次 | 注销 | 明细 |
| --- | --- | --- |
| round90 | 2 | `drop/D2` → F 维 P2（缺 COM 渲染栈）；`drop/D12` → W 桶 wire 宽度分叉 |
| round91 | 3 | `corewindow/D8` → DOC_ONLY 已发货；`skill/D4` → 架构级不新增；`quest/D8` → W 桶 wire 条件追加段 |
| round92 | 4 | `corewindow/D9`/`skill/D12`/`login-flow/D9`/`statusbar-hud/D12` → 全是**编号错配** |
| round93 | 1 | `activity-minigame/D1` → 095 注释改判本端扩展 |

## 2. 分流结果：P1 = 0 / P2 = 1 / P3 = 16

- **P2（1）**：`login-flow/D3` 骑士团条件解码 —— 缺 s9 是否发追加段的协议事实，拿到抓包即升 P1。
- **P3（16）**：`login-flow/D12`、`login-flow/D13`、`statusbar-hud/D1`、`corewindow/D2`、`corewindow/D10`、
  `social-economy/D3`、`social-economy/D9`、`social-economy/D12`、`pet-summon/D2`、`cashshop/D2`、
  `skill/D9`、`skill/D11`、`map/D1`、`map/D4`、`map/D8`、`quest/D5`。
- **P1 = 0**：P1 门槛三条并发（083 状态机证据 + 零 wire + 当前端确有缺陷），残余 17 条的 canonical 裁定
  原文全是「保持 / 不补 / 不做 / 不改」，属**已裁定不实施**，硬塞 P1 就是 round90/91/92 那类「矩阵文本→P1 登记」同族病。

## 3. DECISIONS.md 11 处「待裁决」的真相

这 11 行虽带「待裁决」「待拍板」字样，但**裁定状态已定**——字样是 round86 批量裁决时留下的历史措辞：

| 行 | 面 | ID | 状态 |
| --- | --- | --- | --- |
| 132 | pet-summon | D3 | `P0_SHIPPED`（round78 已判合法混合面） |
| 133 | pet-summon | D4 | `BATCH_4_STANCE`（维持不做） |
| 136 | pet-summon | D7 | `P2_LIVE`（先出像素改色对照） |
| 142 | pet-summon | D13 | `BATCH_4_STANCE`（资源分列） |
| 151 | activity-minigame | D3 | `BATCH_4_STANCE`（wire later 壳） |
| 152 | activity-minigame | D4 | `BATCH_4_STANCE`（黄字降级） |
| 158 | activity-minigame | D10 | `KEEP_S9`（s9 无 295 LEAVE opcode） |
| 248 | quest | D5 | `KEEP_CURRENT`（P3，083 无 Quest OnKey） |
| 265 | drop | D3 | `KEEP_S9`（s9 无掉落归属超时语义） |
| 271 | drop | D9 | `KEEP_CURRENT`（P3 增强） |
| 272 | drop | D10 | `KEEP_S9`（按 s9 移除语义） |

**结论**：不是 11 个待拍板决策，是 **11 行历史措辞**。真要拍板的是 0 个。
若要让文档自洽，正确做法是把这 11 行的「待裁决」改成对应状态名——属文档债，登记 P3，本轮不动
（改写会动 DECISIONS.md 344 行里 11 行的措辞，风险低但会与 round86 已提交记录不一致，须单列一轮做）。

## 4. 「算术未动」= 客户端校准决策台账的条数闭合

**不是**客户端代码里的任何数值，**不是**服务端逻辑，**不是** GATE 的 opcode 计数。是这张决策表的分母：

| 公式 | 含义 |
| --- | --- |
| `168 = PURE 76 + WIRE 38 + MIXED 14 + OTHER 40` | round88 四分类，每轮复用不复算 |
| `168 = W 轨 38 + L 轨 130` | WLE 三桶路由，W 轨 = `class:WIRE` |
| `reopen 27 = 25 P1 候选 + 1 W 桶 + 1 F 维 P2` | round90 登记层口径 |
| `reopen 27 = 已注销 10 + 残余 17` | 本轮 |
| `残余 17 = P1 0 + P2 1 + P3 16` | 本轮 |

**为什么必须守恒**：任一数字被单方面改动（注销却不记账、语义再分类、把 P2 算进 P1），168 或 27 的分母
就断裂，后续每轮的进度百分比全失真。所以每轮只动「登记层」，**分母永不变**。
这也是为什么 round90/91/92 三次纠偏都只在 P1 登记层注销条目、从不做语义再分类。

## 5. 本轮性质与验证

- **账目轮**：零源码改动 / 零 wire / 零协议 / 零 WZ；产物 = WLE §11 + dev-plan 2-42 + PLT §53 + evidence + 本 notes。
- 外部在途 WIP 零夹带（`IO/UIStateGame.cpp` 的 UIMedal hunk 等全部原样留工作树）。
- 验证见 harness `tools/test_round94_reopen_triage.py` 与 GATE。
"""

# notes 目录惯例为 LF（40 份里 37 份 LF），本文件跟同族 wave3-* 保持一致
wr(NOTES, notes.encode("utf-8"))
print("[NOTES] wrote", NOTES)

print("ROUND94_DELIVER_ALL_OK")
for p in (WLE, DEV, PLT, EV, NOTES):
    bb = rd(p)
    import hashlib
    print("  SHA %s %s  bytes=%d crlf=%d crcrlf=%d" % (
        hashlib.sha256(bb).hexdigest()[:12], p, len(bb), bb.count(b"\r\n"), bb.count(b"\r\r\n")))
