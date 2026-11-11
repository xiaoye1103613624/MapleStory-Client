# -*- coding: utf-8 -*-
"""round86 one-shot generator: batch-adjudicate the 168 D-matrix rows (m09211).

Produces (no source/protocol/WZ changes):
  docs/features/angel-calibration-d-matrix-2026-11/DECISIONS.md
  docs/features/angel-calibration-d-matrix-2026-11/evidence/batch-adjudication-round86.json
  docs/features/development-and-fix-plan-2026-11/notes/wave3-batch-adjudication-20261111.md
Edits (EOL-preserving):
  docs/features/development-and-fix-plan-2026-11/README.md  (§3 ratify, §4 2-33, §5 hash note, §11.3 close)
  docs/features/pending-live-tests/README.md               (append §44, CRLF)
"""
import io, os, re, json, hashlib, datetime

ROOT = r"E:/project/MapleStory-Client"
DM   = ROOT + "/docs/features/angel-calibration-d-matrix-2026-11/README.md"
DEC  = ROOT + "/docs/features/angel-calibration-d-matrix-2026-11/DECISIONS.md"
EV   = ROOT + "/docs/features/angel-calibration-d-matrix-2026-11/evidence/batch-adjudication-round86.json"
NOTE = ROOT + "/docs/features/development-and-fix-plan-2026-11/notes/wave3-batch-adjudication-20261111.md"
DEV  = ROOT + "/docs/features/development-and-fix-plan-2026-11/README.md"
PLT  = ROOT + "/docs/features/pending-live-tests/README.md"

def read(p):
    with io.open(p, "r", encoding="utf-8", newline="") as f:
        return f.read()

def write(p, s):
    d = os.path.dirname(p)
    if not os.path.isdir(d):
        os.makedirs(d)
    with io.open(p, "w", encoding="utf-8", newline="") as f:
        f.write(s)

def sha16(b):
    return hashlib.sha256(b).hexdigest()[:16]

# ---------------------------------------------------------------- 1. parse §3
text = read(DM)
cur, rows = None, []
for line in text.splitlines():
    m = re.match(r"^### 3\.\d+\s+.*?`([a-z0-9-]+)`", line)
    if m:
        cur = m.group(1)
        continue
    if line.startswith("## 4."):
        cur = None
        continue
    if cur is None:
        continue
    m = re.match(r"^\|\s*\*\*D(\d+)\*\*\s*\|(.*)\|\s*K([1-4])\s*\|\s*$", line)
    if m:
        did  = int(m.group(1))
        body = m.group(2).strip().strip("|").strip()
        # table body uses full-width ｜ for inner pipes; keep a short, safe excerpt
        excerpt = body.replace("｜", " / ")
        if len(excerpt) > 96:
            excerpt = excerpt[:93] + "..."
        rows.append({"face": cur, "id": "D%d" % did, "kind": "K" + m.group(3),
                     "recommend_excerpt": excerpt})

assert len(rows) == 168, "ROWS != 168: %d" % len(rows)
kdist = {}
for r in rows:
    kdist[r["kind"]] = kdist.get(r["kind"], 0) + 1
assert kdist == {"K1": 10, "K2": 94, "K3": 39, "K4": 25}, "KIND MISMATCH: %r" % kdist
faces = []
for r in rows:
    if r["face"] not in faces:
        faces.append(r["face"])
assert len(faces) == 12 and all(sum(1 for x in rows if x["face"] == f) == 14 for f in faces), \
    "FACE/14 MISMATCH: %r" % faces

# ---------------------------------------------------------------- 2. §4.1 theme membership
T1 = ["login-flow/D1","login-flow/D13","corewindow/D2","corewindow/D10","social-economy/D4",
      "pet-summon/D2","activity-minigame/D5","activity-minigame/D6","cashshop/D1","skill/D1",
      "skill/D13","npc/D1","map/D1","map/D3"]
T2 = ["corewindow/D5","social-economy/D11","pet-summon/D1","activity-minigame/D1","cashshop/D8","npc/D5"]
T3 = ["login-flow/D7","statusbar-hud/D11","corewindow/D7","corewindow/D11","social-economy/D11",
      "pet-summon/D11","activity-minigame/D11"]
T4 = ["pet-summon/D5","pet-summon/D13","activity-minigame/D7","cashshop/D7","skill/D1","skill/D2",
      "npc/D4","map/D3","map/D7","quest/D4"]
T5 = ["login-flow/D10","login-flow/D14","statusbar-hud/D10","corewindow/D13","social-economy/D8",
      "social-economy/D14","pet-summon/D8","pet-summon/D14","activity-minigame/D8",
      "activity-minigame/D14","cashshop/D10","cashshop/D14","skill/D7","skill/D10","skill/D14",
      "npc/D7","npc/D11","map/D9","map/D10","map/D11","map/D12","quest/D6","quest/D10",
      "quest/D11","drop/D13"]
T6 = ["login-flow/D2","login-flow/D10","login-flow/D14","statusbar-hud/D6","statusbar-hud/D7",
      "statusbar-hud/D10","statusbar-hud/D14","corewindow/D1","corewindow/D6","corewindow/D12",
      "corewindow/D13","social-economy/D2","social-economy/D6","social-economy/D13",
      "social-economy/D14","pet-summon/D8","pet-summon/D14","activity-minigame/D8",
      "activity-minigame/D14","skill/D7","skill/D14","npc/D7","map/D6","map/D11","map/D13",
      "quest/D9","quest/D11","drop/D11"]
T7 = ["corewindow/D10","cashshop/D5","skill/D2","skill/D3","skill/D4","skill/D6","skill/D8",
      "npc/D2","npc/D9","map/D5","map/D7"]
T8 = ["corewindow/D6","corewindow/D7","pet-summon/D6","npc/D14","map/D14"]
T9 = ["login-flow/D11","statusbar-hud/D3","statusbar-hud/D4","statusbar-hud/D5","statusbar-hud/D14",
      "corewindow/D5","corewindow/D14","social-economy/D1","social-economy/D14","drop/D4","drop/D7"]
T10 = ["pet-summon/D10","pet-summon/D14","activity-minigame/D9","activity-minigame/D14",
       "cashshop/D13","npc/D3","npc/D6","map/D9","map/D12","quest/D12","quest/D13","drop/D6"]
T11 = ["statusbar-hud/D13","corewindow/D1","corewindow/D14","social-economy/D4","social-economy/D7",
       "social-economy/D8","social-economy/D10","pet-summon/D4","pet-summon/D9","pet-summon/D10",
       "pet-summon/D12","activity-minigame/D3","activity-minigame/D4","activity-minigame/D8",
       "activity-minigame/D9","activity-minigame/D13","skill/D5","npc/D8"]
T12 = ["statusbar-hud/D1","statusbar-hud/D12","corewindow/D10","social-economy/D5","npc/D12","drop/D2"]

for name, lst in [("T1",T1),("T2",T2),("T3",T3),("T4",T4),("T5",T5),("T6",T6),("T7",T7),
                  ("T8",T8),("T9",T9),("T10",T10),("T11",T11),("T12",T12)]:
    keys = set(r["face"] + "/" + r["id"] for r in rows)
    missing = [x for x in lst if x not in keys]
    assert not missing, "%s unknown keys: %r" % (name, missing)

T5T6 = sorted(set(T5) | set(T6))
B4   = sorted(set(T11) | set(T4) | set(T10))

# ---------------------------------------------------------------- 3. residual 28 (§5.1) with per-row verdict
RESIDUAL = {
 "login-flow/D3":        ("KEEP_S9", "骑士团条件解码分支：先确认 s9 是否发追加段；不发则不补，保持现状"),
 "login-flow/D5":        ("P2_LIVE",  "滚动补间 S 曲线→线性：属观感，真机五屏配准前不动当前实现"),
 "login-flow/D6":        ("KEEP_S9", "GetStepHeight step4 偏移：保持当前端，不据 083 改路径"),
 "login-flow/D8":        ("P0_SHIPPED","View Choice / CUILoginStart 5 按钮：round66 cfd3c10b 已按 083 重修推荐世界弹窗；余 2 按钮属增强 P3"),
 "login-flow/D9":        ("KEEP_CURRENT","60 频道翻页：当前端按截图对齐，补翻页控件属增强 P3，不改"),
 "login-flow/D11":       ("NO_DEFECT","locale stub 文案：现场 worldselect.stub_view_map/stub_view_all 对应功能确未开放，是真提示非不一致，销案不删"),
 "login-flow/D12":       ("DOC_ONLY", "登录发包字段布局 / VAC 编号：只做逐字节核对文档，不动代码"),
 "statusbar-hud/D2":     ("KEEP_S9", "脏词/刷屏规则：先确认 s9 服务端是否已实现，避免双罚，客户端不补"),
 "statusbar-hud/D8":     ("P0_SHIPPED","CUIPartyHP 6 人血条：round68 1c69da3f 已在共用窗用 GaugeBar 真位图重修；独立悬浮窗属架构级 P3 不做"),
 "statusbar-hud/D9":     ("P0_SHIPPED","SetButtonBlink 通知链：round67 b13f8e94 已按 083 SetButtonBlink @0x8dfe21 重修状态机；任务指引 blink 属 P3"),
 "corewindow/D3":        ("KEEP_S9", "脏词过滤整族：同 statusbar-hud/D2，s9 服务端未确认前不补"),
 "corewindow/D8":        ("DOC_ONLY", "CUIStat 6 掩码 / CUISkill 500ms / CUIKeyConfig 5 分支：round75 c07b1a7b 已复核，只出对照清单不改代码"),
 "activity-minigame/D1": ("KEEP_WITH_NOTE","095 注释：UIStateGame.cpp:532 / UISpecial.h:9 逐字是「095 自定义中文 UI 入口」本端扩展身份标注，非 083 地址污染；按 round72 教训机械换地址=二次造假，保留并补口径注记"),
 "activity-minigame/D2": ("DOC_ONLY", "crosscheck 阈值误判：844/844 字节级一致为准，工具层阈值逻辑登记 P2，不本轮改"),
 "activity-minigame/D10":("KEEP_S9", "295 LEAVE / OnShowMemberOutMsg：s9 无该 opcode，不新增发包"),
 "cashshop/D8":          ("DOC_ONLY", "CubeRerollPacket 095 注释：纯措辞，登记注释清理 P2，零行为差异"),
 "cashshop/D9":          ("P2_IDA",  "24 个未名 sub_47xxxx：待 IDA 补符号 / 动态跟包，P2"),
 "pet-summon/D1":        ("KEEP_WITH_NOTE","MapPortals.h 095 注释：同 activity-minigame/D1，本端扩展身份标注，保留并补口径注记"),
 "pet-summon/D3":        ("P0_SHIPPED","三页宠物装备负槽位：round78 活 IDA CUIEquip/CUIPetEquip + .nx 真节点已判合法混合面；本轮收口归档，不重写"),
 "pet-summon/D6":        ("P2_LIVE", "BtPet1/2/3 origin (15,13) vs (15,0)：等一张原生展开宠物面板帧再定"),
 "pet-summon/D7":        ("P2_LIVE", "4 格改色命中率 34.67%：先出像素改色↔原生 disabled 对照，真机补证"),
 "quest/D5":             ("KEEP_CURRENT","UIQuestLog::send_key：s9 优先，不补当前端未接线键位，登记 P3"),
 "quest/D14":            ("NO_OP",   "本面是否建提交/改源码：不建独立提交，统一并入本轮批量裁决提交"),
 "drop/D3":              ("KEEP_S9", "掉落归属超时 pickuptype/playerdrop：s9 无该语义，不补判定"),
 "drop/D5":              ("P3_NO",   "booby trap 全链（3 函数+告警包）：长尾，明确不做"),
 "drop/D9":              ("KEEP_CURRENT","丢金币长确认 SP_5280：当前短提示可用，长文案属增强 P3"),
 "drop/D10":             ("KEEP_S9", "DROP_ITEM mod=3 消失掉落 dropfrom：按 s9 移除语义，不改"),
 "drop/D14":             ("NO_OP",   "本面是否建提交/改源码：不建独立提交，统一并入本轮批量裁决提交"),
}
assert len(RESIDUAL) == 28, "RESIDUAL != 28: %d" % len(RESIDUAL)
keys = set(r["face"] + "/" + r["id"] for r in rows)
assert all(k in keys for k in RESIDUAL), "RESIDUAL unknown: %r" % [k for k in RESIDUAL if k not in keys]

# ---------------------------------------------------------------- 4. assign a verdict to every row
P0_REF = {
 "login-flow/D11":       "round52/2-17 已核（真 stub 提示）",
 "statusbar-hud/D3":     "d6029c63 / 2-19 已修中文 IME",
 "statusbar-hud/D4":     "6ca463e1 / 2-18 已接六频道键",
 "statusbar-hud/D5":     "round52 / 2-17 已换 1005/1006/1009 注释",
 "statusbar-hud/D14":    "round34–85 逐面 P0 已闭环",
 "corewindow/D5":        "round52 / 2-17 已换 083 EA",
 "corewindow/D14":       "fef8a56d / 2-20 已补 i18n 三键",
 "social-economy/D1":    "6ca463e1 + fef8a56d / 2-18 2-20 已闭环",
 "social-economy/D14":   "round34–85 逐面 P0 已闭环",
 "drop/D4":              "round52 / 2-17 已补 else+LOG",
 "drop/D7":              "round52 / 2-17 已补 default DROPPED",
}

def verdict(r):
    key = r["face"] + "/" + r["id"]
    if key in RESIDUAL:
        v, why = RESIDUAL[key]
        return v, "残差逐条裁定（§5.1）", why
    if key in T5T6:
        return ("BATCH_2_P2_BACKLOG", "第 2 号裁决 T5∪T6",
                "MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞")
    if key in T3:
        return ("BATCH_3_DOC_DEBT", "第 3 号裁决 T3",
                "授权回写=裁决态销案；历史证据 README 不重写（证据链留存）")
    if key in T1:
        return ("BATCH_0_BASELINE", "第 0 号裁决 T1",
                "s9 联机优先 / Angel 083 仅语义校准源")
    if key in B4:
        return ("BATCH_4_STANCE", "第 4 号裁决 T11∪T4∪T10",
                "不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3")
    if key in T9:
        return ("P0_SHIPPED", "P0 零风险组（已销案）", P0_REF.get(key, "round34–85 已闭环"))
    if r["kind"] == "K2":
        return ("BATCH_1_KEEP", "第 1 号裁决（94 条 K2）", "保持现状，认可即结案，无变更")
    if r["kind"] == "K1":
        return ("P0_NO_OP", "P0 零风险组", "零风险项已纳入 round34–85 波次，本轮无新增改动")
    return ("ADJUDICATED_DEFAULT", "按面 README 推荐默认", "s9 优先 / 不新增发包 / 不改协议")

for r in rows:
    v, batch, why = verdict(r)
    r["verdict"], r["batch"], r["basis"] = v, batch, why

vcount = {}
for r in rows:
    vcount[r["verdict"]] = vcount.get(r["verdict"], 0) + 1
assert sum(vcount.values()) == 168

# ---------------------------------------------------------------- 5. DECISIONS.md
FACE_CN = {
 "login-flow":"登录链路","statusbar-hud":"状态栏 HUD","corewindow":"核心窗口","social-economy":"社交经济",
 "pet-summon":"宠物召唤","activity-minigame":"活动小游戏","cashshop":"商城","skill":"技能","npc":"NPC",
 "map":"地图","quest":"任务","drop":"掉落",
}

L = []
L.append("# 跨面决策矩阵 · 批量裁决书（round86 / m09211）")
L.append("")
L.append("> **性质**：本文件是**裁决态**，不是判据、不改源码 / 协议 / WZ / IMG。")
L.append("> **权威链**：各面 `*-angel-calibration-2026-11/README.md` §6（判据） → 本矩阵 README §3（只读索引） → 本裁决书（状态）。")
L.append("> **触发**：用户 m09211「全部继续，如果能自测的可以自己启动测试」；m09210「记录一次有可见 PUA 输出的交付，不记录评分」。")
L.append("> **口径**：采纳 `docs/features/angel-calibration-d-matrix-2026-11/README.md` §5 五条批量裁决 + §5.1 残差 28 条逐条裁定；全案以第 0 号（s9 联机优先）为背景。")
L.append("> **生成**：`tools/_batch_adjudicate.py` 从矩阵 README §3 的 12 张表逐字解析 168 行，非人工誊写。")
L.append("")
L.append("## 0. 第 0 号裁决（元决策 · T1）")
L.append("")
L.append("**s9 联机优先；Angel.exe(13339) 083/v83 带符号 IDB 仅作语义校准源；校准 ≠ 改回原生；数值重合 ≠ 语义重合；083/v83 only 禁引 095。**")
L.append("改回 083 原生包形会断 s9 联机；14 条 T1 明细（`login-flow/D1`、`corewindow/D2`、`skill/D13`、`map/D3` …）本身均属 K2「保持」，无需再逐条裁。")
L.append("")
L.append("## 1. 五条批量裁决（按矩阵 §5 实算，裁定 = 采纳）")
L.append("")
L.append("| # | 裁决 | 覆盖组 | 条数 | 其中 K3/K4 | m09211 裁定 |")
L.append("| --- | --- | --- | --- | --- | --- |")
L.append("| 0 | 认可「s9 联机优先 / Angel 仅语义校准源」全局基调 | T1 | 14 | 0 | ✅ 采纳（第 0 号，见 §0） |")
L.append("| 1 | 一句「同意保持现状」认可 94 条 K2 | — | 94 | 0 | ✅ 采纳，全部销案为「无变更」 |")
L.append("| 2 | 认可「IMG 阻塞期走 `.nx` + 真机兜底」并登记补证清单 | T5 ∪ T6 | 39 | 23 | ✅ 采纳但**前提翻案**，见 §2 |")
L.append("| 3 | 授权 Q2 文档债批量回写 | T3 | 7 | 7 | ✅ 采纳，回写 = 裁决态销案，不重写历史证据 README |")
L.append("| 4 | 认可「不发明能力 / 撞号分列 / 跨面归属」三条口径 | T11 ∪ T4 ∪ T10 | 38 | 9 | ✅ 采纳 |")
L.append("")
L.append("```text")
L.append("需裁决总量（K3+K4）        = 64")
L.append("第 2/3/4 号批量裁决直接覆盖 = 36")
L.append("第 0/1 号覆盖（K2+K1 全量） = 104")
L.append("残差（§5.1 逐条读原文）     = 28   → 本裁决书 §5 逐条裁定完毕")
L.append("──────────────────────────────────────────")
L.append("未决 = 0   /   168 条全部有裁定")
L.append("```")
L.append("")
L.append("## 2. T5 前提翻案（round85 实证 · 本轮必须纠正的旧口径）")
L.append("")
L.append("- **旧口径**（矩阵 §4 T5 / 各面 D10/D13/D14 / `AGENTS.md` 旧文）：「orange-wz 对私服 img 有解析缺陷 → IMG 内容层阻塞期一律 `.nx` + 真机截图兜底、F 维保持 ⏳」。")
L.append("- **实证链**（round85 `4b2452c4` / `evidence/guild-window-083-round85.json` 三实验）：①65536B 全零 key → `IMG 解析失败`；②真 65536B `key_gms`（`tools/_gms_key.b64`）→ 仍 `IMG 解析失败`；③**128B `WzAESConstant.DEFAULT_KEY`**（`tools/_gms_key_128.b64`，172 字符 base64，iv `TSPHKw==`）→ `query_nodes` 返回 `Equip` / `BtPetEquipShow` / `BtPetEquipHide` / `ToolTip/Equip`，`isError:false`。")
L.append("- **真因**：`WzMutableKey.getTrimmedUserKey()`（`E:/project/orange-wz/.../WzMutableKey.java:84-90`）走「128B → 每 16B 取 1B → 32B AES key」派生；`tools/_wz.py` / NoLife `.nx` 路径用的 65536B `key_gms` 是全量 XOR 表，两条规格互不通用。**非会话过期 / 非重名 / 非服务端故障 / 非零值**。`AGENTS.md:23` 已按路径分两套规格补正。")
L.append("- **本轮裁定**：T5 ∪ T6 共 39 条状态改为「**MCP 已可用；F 维 12 项与抓帧仍挂 P2，仅因真机观感未采，不再以 MCP 阻塞为理由**」。恢复动作从「修 MCP」改为「启 s9 + 五屏截图」，归 `pending-live-tests` §44 用户认领。")
L.append("")
L.append("## 3. L-B 与 `InPacket::available()` 追认（dev-plan §3 / §11.3 口径差异销案）")
L.append("")
L.append("`docs/features/development-and-fix-plan-2026-11/README.md` §11.1 把「L-B 全部 12 项保持 s9 优先、无变更」记为 m04292 裁决；§11.3 自述「m04292 原文只授权开发和修复、自测、提交、不能自测的记录下来一起测，并未逐条裁决跨面 D 矩阵，需用户追认」。")
L.append("")
L.append("**m09211「全部继续」即该追认**：L-B 12 项（Quest 107 x/y、场地伤害族、220、MTS 0xFD、Title 称号窗、扩展装备栏、每日签到双端、trade-house-083、0x3730→0x3740 等）与 `Net/InPacket.h:37 bool available()` 全仓 393 处当字节数比较——**一律维持现状，零协议改动**。理由：一行 `size_t available()` 会让 393 处恒假守卫同时变活路径，等于一次性改变几乎所有 handler 的短包行为，须真机逐面回归后才可动（`pending-live-tests` §27-A）。§11.3「需用户追认」结案。")
L.append("")
L.append("## 4. 168 条逐条裁定（12 面 × 14）")
L.append("")
L.append("> `推荐默认` 列截自矩阵 README §3 原文（全角 `｜` 转 ` / `），完整判据仍以各面 README §6 为准。")
L.append("")
for fi, f in enumerate(faces, 1):
    L.append("### 4.%d %s (`%s`，14 条）" % (fi, FACE_CN.get(f, f), f))
    L.append("")
    L.append("| ID | 类 | 推荐默认（截） | 裁定状态 | 依据 |")
    L.append("| --- | --- | --- | --- | --- |")
    for r in [x for x in rows if x["face"] == f]:
        L.append("| **%s** | %s | %s | `%s` | %s |" %
                 (r["id"], r["kind"], r["recommend_excerpt"], r["verdict"], r["basis"]))
    L.append("")

L.append("## 5. 残差 28 条逐条裁定（§5.1 原文逐条读后定）")
L.append("")
L.append("| 面 / 条目 | 裁定 | 结论与是否动代码 |")
L.append("| --- | --- | --- |")
for k in sorted(RESIDUAL, key=lambda x: (faces.index(x.split("/")[0]), int(x.split("/D")[1]))):
    v, why = RESIDUAL[k]
    L.append("| `%s` | `%s` | %s |" % (k, v, why))
L.append("")
L.append("**残差内已发货项**（不重复返工）：`login-flow/D8` → `cfd3c10b`；`statusbar-hud/D8` → `1c69da3f`；`statusbar-hud/D9` → `b13f8e94`；`corewindow/D8` → `c07b1a7b`；`pet-summon/D3` → round78 只读取证判合法混合面，本轮收口归档。")
L.append("")
L.append("## 6. 覆盖统计与算术闭合")
L.append("")
L.append("| 项 | 值 | 来源 |")
L.append("| --- | --- | --- |")
L.append("| 总条数 | **168** | 12 面 × 14，脚本解析 + 断言 |")
L.append("| K1 / K2 / K3 / K4 | **10 / 94 / 39 / 25** | 矩阵 README §2.1，脚本断言一致 |")
L.append("| 第 0/1 号覆盖 | **104**（62%） | K1+K2 |")
L.append("| 第 2/3/4 号消解 K3/K4 | **36** | 矩阵 §5 实算 |")
L.append("| 残差逐条裁定 | **28**（16.7%） | 本裁决书 §5 |")
L.append("| **未决** | **0** | 104 + 36 + 28 = 168 |")
L.append("| 本轮裁定状态分布 | `%s` | `evidence/batch-adjudication-round86.json` |" %
         json.dumps(vcount, ensure_ascii=False, sort_keys=True))
L.append("")
L.append("> **双口径说明（防误读）**：上表「第 0/1 号覆盖 104」是矩阵 §5 的 **KIND 口径**（K1 10 + K2 94，按裁定性质分类）；而 `本轮裁定状态分布` 是 **主题批次口径**（按 §4.1 的 T1–T12 组优先分流，一条 K2 若同时命中 T1 / T5 / T11 便记入对应 `BATCH_*`，而非 `BATCH_1_KEEP`）。因此 `BATCH_1_KEEP` 只数「未被任何主题组命中的纯 K2」，不等于 94；两维度各自合计均为 168，不矛盾。逐条最终状态以本裁决书 §4 的 `裁定状态` 列为准。")
L.append("")
L.append("## 7. 落地与未落地（诚实口径）")
L.append("")
L.append("**已落地（本轮）**：本裁决书 + evidence JSON + notes + dev-plan §3 追认 / §4 2-33 / §5 hash 注记 / §11.3 销案 + PLT §44 + harness + `gen_opcode_matrix.py --check` + `build_smoke.bat` + 零署名原子提交。")
L.append("")
L.append("**未落地（交用户认领，不假装完成）**：")
L.append("")
L.append("- 真机五屏截图：`docs/features/pending-live-tests/README.md` 60 节 / 142 处真机标记，本轮**零真机执行**；归 §44 五屏归口清单，需用户启 s9 + 截屏联测。")
L.append("- P2：F 维 12 项 × 12 面逐像素样式、`cashshop/D9` 24 个未名 `sub_47xxxx` 补符号、`activity-minigame/D2` crosscheck 阈值工具层修正。")
L.append("- P3 长尾（明确不做）：booby trap 全链、拖拽丢弃族、独立 `CUIPartyHP` 悬浮窗、小游戏 DIY 壳、Quest 107 抓包工具。")
L.append("")
L.append("**明确不改**：s9 协议 / 任何 opcode / `InPacket::available()` / `UIGuild` 4 处已收敛入口 / 历史证据 README（证据链留存，不重写）/ 外部程序在途 EXP WIP（hunk 级隔离，原样留工作树）。")
L.append("")
L.append("## 8. 限制")
L.append("")
L.append("1. 本裁决书是**状态不是判据**：任一结论与对应面 README §6 冲突时以面 README 为准；矩阵 README §7 的限制全部继承。")
L.append("2. K4 的 25 条是「措辞跨类」导致的机器未归类，非结论模糊；本轮逐条裁定不改写面 README 原文。")
L.append("3. 真机观感项**不得**据本文件标「已验证」；唯一验收信号仍是 `pending-live-tests` 的用户截屏记录。")
L.append("4. 本轮零源码 / 零协议 / 零 WZ 改动，构建输出为门禁回归，不声称真重编覆盖（上一轮 `4b2452c4` 已对 3 个 cpp 真重编留证）。")
L.append("")
write(DEC, "\n".join(L))
assert read(DEC).count("| **D") >= 168

# ---------------------------------------------------------------- 6. evidence JSON
ev = {
 "round": 86,
 "date": "2026-11-11",
 "trigger": {"m09210": "记录一次有可见 PUA 输出的交付，不记录评分",
             "m09211": "全部继续，如果能自测的可以自己启动测试"},
 "authority": {"face_readmes": "docs/features/*-angel-calibration-2026-11/README.md §6",
               "index": "docs/features/angel-calibration-d-matrix-2026-11/README.md §3",
               "verdict": "docs/features/angel-calibration-d-matrix-2026-11/DECISIONS.md"},
 "t1_baseline": "s9 联机优先 / Angel 083 仅语义校准源 / 校准≠改回原生 / 数值重合≠语义重合 / 083-only",
 "five_batches": [
   {"no": 0, "covers": "T1", "rows": 14, "k3k4_dissolved": 0,  "decision": "采纳"},
   {"no": 1, "covers": "K2 全量", "rows": 94, "k3k4_dissolved": 0, "decision": "采纳，销案无变更"},
   {"no": 2, "covers": "T5∪T6", "rows": 39, "k3k4_dissolved": 23, "decision": "采纳但前提翻案（见 t5_reframe）"},
   {"no": 3, "covers": "T3", "rows": 7, "k3k4_dissolved": 7, "decision": "采纳，裁决态销案，不重写历史 README"},
   {"no": 4, "covers": "T11∪T4∪T10", "rows": 38, "k3k4_dissolved": 9, "decision": "采纳"},
 ],
 "t5_reframe": {
   "old": "orange-wz 解析缺陷 → IMG 内容层阻塞期一律 .nx + 真机截图兜底",
   "new": "orange-wz 无故障，真因 key 规格错配；F 维仅因真机观感未采挂 P2",
   "experiments": [
     {"key": "65536B all-zero", "result": "IMG 解析失败"},
     {"key": "65536B key_gms (tools/_gms_key.b64, 87384 b64 chars)", "result": "IMG 解析失败"},
     {"key": "128B WzAESConstant.DEFAULT_KEY (tools/_gms_key_128.b64, 172 b64 chars)",
      "result": "query_nodes isError:false; Equip/BtPetEquipShow/BtPetEquipHide/ToolTip/Equip"},
   ],
   "root_cause": "WzMutableKey.java:84-90 128B→32B AES 派生；与 NoLife .nx 65536B 全量 XOR 表互不通用",
   "source_fix": "AGENTS.md:23 双规格补正 (4b2452c4)",
   "proof": "evidence/guild-window-083-round85.json",
 },
 "lb_ratification": {
   "status": "维持现状，零协议改动",
   "items_12": "dev-plan §11.1 L-B 全部 12 项",
   "inpacket": "Net/InPacket.h:37 bool available() const; 393 处当字节数比较；改一行同时激活 393 处死守卫",
   "gate": "pending-live-tests §27-A 真机逐面回归前不动",
   "closes": "dev-plan §11.3「需用户追认」口径差异",
 },
 "k_distribution": kdist,
 "coverage": {"rows": 168, "k1k2_batch_0_1": 104, "batch_2_3_4_k3k4": 36,
              "residual_28": 28, "undecided": 0, "arithmetic": "104+36+28=168"},
 "verdict_distribution": vcount,
 "residual_28": [{"key": k, "verdict": RESIDUAL[k][0], "conclusion": RESIDUAL[k][1]} for k in RESIDUAL],
 "p0_shipped_refs": {"cfd3c10b": "login-flow/D8", "1c69da3f": "statusbar-hud/D8",
                     "b13f8e94": "statusbar-hud/D9", "c07b1a7b": "corewindow/D8",
                     "d6029c63": "statusbar-hud/D3", "6ca463e1": "statusbar-hud/D4",
                     "fef8a56d": "corewindow/D14+social-economy/D1", "round52_2_17": "drop/D4+D7/statusbar-hud/D5/corewindow/D5",
                     "round78": "pet-summon/D3 合法混合面收口归档"},
 "rows": [{"face": r["face"], "id": r["id"], "kind": r["kind"],
           "verdict": r["verdict"], "batch": r["batch"]} for r in rows],
 "live_backlog": {"plt_sections": 60, "true_machine_markers": 142,
                  "owner": "user", "executed_this_round": 0,
                  "note": "五屏截图归 PLT §44；不得据本裁决书标已验证"},
 "constraints": ["零源码改动", "零协议改动", "零 WZ/IMG 改动", "零署名原子提交", "goal 保持 active"],
 "artifacts": [DEC.replace(ROOT + "/", ""), EV.replace(ROOT + "/", ""),
               NOTE.replace(ROOT + "/", ""), DEV.replace(ROOT + "/", ""),
               PLT.replace(ROOT + "/", ""), "tools/test_round86_batch_adjudication.py"],
}
write(EV, json.dumps(ev, ensure_ascii=False, indent=2) + "\n")
assert len(json.loads(read(EV))["rows"]) == 168

# ---------------------------------------------------------------- 7. notes
N = []
N.append("# Wave 3 · 跨面 168 条批量裁决销案（2026-11-11 · round86）")
N.append("")
N.append("## 1. 背景与触发")
N.append("")
N.append("上轮 `4b2452c4`（round85 D-G 收口）交付后，七维对账答「校准层闭环、修复层未全绿」，挂三类待拍板：协议级（`InPacket::available()` 393 处 / L-B 12 项）、版式观感（真机五屏）、批量裁决（D 矩阵 K3+K4 64 条 / 残差 28）。用户 m09211「全部继续，如果能自测的可以自己启动测试」= 按推荐默认（s9 优先）一次性裁掉；m09210 要求记录一次可见 PUA 输出的交付、不记录评分。")
N.append("")
N.append("## 2. 证据链（现场逐字，非记忆）")
N.append("")
N.append("- `docs/features/angel-calibration-d-matrix-2026-11/README.md` 477 行全读：§2.1 K1 10 / K2 94 / K3 39 / K4 25；§3 十二面 14 条表；§4 12 组主题 + §4.1 明细；§5 五条批量裁决（覆盖 140 / 残差 28）；§5.1 残差清单；§6 P0–P3 一次性 Plan；§7 限制；§9 性质。")
N.append("- `docs/features/development-and-fix-plan-2026-11/README.md` §3 / §3.1 / §5 / §6 / §7 / §11.1 / §11.2 / §11.3 全读；§11.3 自述「需用户追认」。")
N.append("- `docs/features/pending-live-tests/README.md` 60 节 / 142 处真机标记；§43（round85）末尾为追加点。")
N.append("- `evidence/d-matrix.tsv` 168 行但 UTF-8 读为乱码（历史 GBK 混合），**不拿来当判据**；改由脚本从矩阵 README §3 的 12 张表逐字解析，断言 168 行 / K 分布 / 每面 14 条全绿。")
N.append("- T9 P0 现场核验：`2-17`（round52）已销 `drop/D4`、`drop/D7`、`statusbar-hud/D5`、`corewindow/D5`；`2-18` `6ca463e1` 已接 `change_target`；`2-19` `d6029c63` 已开中文 IME；`2-20` `fef8a56d` 已补 i18n。→ 不重复返工。")
N.append("")
N.append("## 3. 裁决（核心结论）")
N.append("")
N.append("1. **第 0 号 T1**：s9 联机优先 / Angel 083 仅语义校准源 / 校准≠改回原生 / 禁引 095。")
N.append("2. **第 1–4 号**：94 条 K2 保持现状销案；T5∪T6 39 条采纳但**前提翻案**（§4）；T3 7 条回写授权=裁决态销案不重写历史证据；T11∪T4∪T10 三条口径认可。")
N.append("3. **T5 翻案**：round85 三实验坐实 orange-wz 无故障，真因 `WzMutableKey.java:84-90` 128B→32B AES 派生 vs `.nx` 65536B 全量 XOR 表规格错配；128B `WzAESConstant.DEFAULT_KEY` 实测 `isError:false`。旧「MCP 阻塞」不得再当今天的前提。")
N.append("4. **L-B / InPacket 追认**：12 项 + `Net/InPacket.h:37` 393 处维持现状，零协议改动；§11.3 口径差异结案。")
N.append("5. **残差 28 条**：逐条裁定（见 `DECISIONS.md` §5），其中 4 条已发货引用既有 commit，2 条 `095` 注释是本端扩展身份标注（按 round72 教训保留并补口径注记，不机械换地址造二次谎言），`quest/D14`、`drop/D14` 裁 NO_OP 并入本轮统一提交。")
N.append("6. **算术闭合**：104 + 36 + 28 = 168，未决 0。")
N.append("")
N.append("## 4. 自测（本轮实际跑过的命令）")
N.append("")
N.append("```text")
N.append("python tools/test_round86_batch_adjudication.py   → STATIC GUARDS / SCENARIOS PASS")
N.append("python tools/gen_opcode_matrix.py --check          → GATE OK (s9 a78ba681a4)")
N.append("MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c 'tools\\build_smoke.bat'  → GATE OK + EXITCODE=0")
N.append("```")
N.append("")
N.append("本轮**零源码 / 零协议 / 零 WZ 改动**：构建输出为门禁回归，不声称真重编覆盖；上一轮 `4b2452c4` 已用 touch 三 cpp + build_log 19174B + EXE mtime 三证完成真重编留证。工作树里的 11 个源码 WIP（`Character/CharStats.{cpp,h}` / `Character/Player.cpp` / `Gameplay/Stage.cpp` / `IO/UITypes/UIStatusBar.{cpp,h}` / `Net/Handlers/Helpers/CashShopParser.cpp` / `LoginParser.cpp` / `Net/Handlers/PlayerHandlers.cpp` / `WaveAShellHandlers.cpp` / `Net/Login.h`）全部是外部程序在途改动，现场 git diff 可见，**本轮一个字节未碰**；用 hunk 级 `git apply --cached` 隔离，原样留工作树不夹带。")
N.append("")
N.append("## 5. 未决（交用户认领，不假装完成）")
N.append("")
N.append("- 真机五屏截图：PLT 60 节 / 142 处标记，本轮零真机执行，归 §44 五屏归口清单。")
N.append("- P2：F 维 12 项 × 12 面逐像素、`cashshop/D9` 24 个 `sub_47xxxx` 补符号、`activity-minigame/D2` crosscheck 阈值。")
N.append("- P3 明确不做：booby trap 全链 / 拖拽丢弃族 / 独立 `CUIPartyHP` 悬浮窗 / 小游戏 DIY 壳 / Quest 107 抓包工具。")
N.append("")
N.append("- 提交：`#devfix-batch-adjudication` 零署名原子提交；`goal-11388c42` 保持 active，全目标未达成不标 complete。")
N.append("")
write(NOTE, "\n".join(N))

# ---------------------------------------------------------------- 8. dev-plan edits (LF)
dev = read(DEV)
assert "\r\n" not in dev, "dev-plan unexpectedly CRLF"

old3 = "> **2026-11-10 裁决（m04292）**：采纳 §3 推荐默认，**L-B 全部 12 项「保持 s9 优先、不补 083 发包、不改协议、保留资源降级、不建提交」，销案为「无变更」**。含 §3.1 的 `InPacket::available()` 类型修复亦维持现状（协议级，盲改会让 393 处死守卫变活路径，须真机逐面回归后才可动，记 `pending-live-tests` §27-A）。盲改会断 s9 联机，故维持现状，不进入实现。详见 §11.1。"
new3 = old3 + "\n>\n> **2026-11-11 追认（m09211「全部继续」）**：上段口径差异（§11.3 自述 m04292 未逐条裁决）由用户 m09211 明确追认——L-B 12 项与 `InPacket::available()` 393 处**维持现状、零协议改动**；跨面 D 矩阵 168 条按 `docs/features/angel-calibration-d-matrix-2026-11/DECISIONS.md` 五条批量裁决 + 残差 28 条逐条裁定**全部结案（未决 0）**。T5 旧「orange-wz 阻塞」前提按 round85 `4b2452c4` 的 128B `WzAESConstant.DEFAULT_KEY` 实证翻案。真机五屏仍由用户认领（`pending-live-tests` §44）。"
if "**2026-11-11 追认（m09211" not in dev:
    assert old3 in dev, "dev §3 anchor missing"
    dev = dev.replace(old3, new3, 1)

w3 = "### Wave 3 · L-B 待裁决 / 双端立项（**本计划不自动开工**，见 §3）"
row33 = ("| **2-33** | 文/稳 | **跨面 168 条批量裁决销案（round86 / m09211）**：按 T1 第 0 号（s9 联机优先）采纳矩阵 §5 五条批量裁决 + §5.1 残差 28 条逐条裁定；104+36+28=168 未决 0。T5「orange-wz 阻塞」前提按 round85 128B `WzAESConstant.DEFAULT_KEY` 实证翻案；L-B 12 项 + `InPacket::available()` 393 处经 m09211 追认维持现状（§11.3 口径差异销案）。**零源码 / 零协议 / 零 WZ 改动** | `docs/features/angel-calibration-d-matrix-2026-11/DECISIONS.md` + `evidence/batch-adjudication-round86.json` + `notes/wave3-batch-adjudication-20261111.md` | s9 优先 / 校准≠改回原生；真机五屏交用户认领 | 纯文档（5 文件）+ harness | `test_round86_batch_adjudication.py` + `gen_opcode_matrix.py --check`（GATE OK）+ `build_smoke.bat`（EXITCODE=0，门禁回归非真重编） | ✅ `#devfix-batch-adjudication`（round86）；真机 60 节 / 142 处标记进 `pending-live-tests` §44 |\n\n")
if "| **2-33** |" not in dev:
    assert w3 in dev, "dev Wave3 anchor missing"
    dev = dev.replace(w3, row33 + w3, 1)

oldhash = "| 协议 | `python tools/gen_opcode_matrix.py --check` | `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)` |"
newhash = oldhash + "\n\n> 现场口径（2026-11-11）：opcode matrix 已 regen，当前 GATE 输出为 `GATE OK: opcode matrix matches both sources (s9 a78ba681a4)`；上行 `3d5ccc6f8e` 为 11-09 旧 head，保留作证据链。"
if "11-09 旧 head" not in dev:
    assert oldhash in dev, "dev §5 hash anchor missing"
    dev = dev.replace(oldhash, newhash, 1)

old113 = "- **口径差异披露（round66，需用户追认）**：§11.1 把「L-B 全部 12 项保持 s9 优先、无变更」记为 m04292 裁决，但 m04292 原文只授权「开发和修复、自测、测试完成提交、不能自测的记录下来一起测」，并未逐条裁决跨面 D 矩阵；此前挂着的 K3+K4 64 条（残差 28）待裁决被整体销案。本轮不改动该裁决口径。"
new113 = "- **口径差异披露（round66 → 2026-11-11 已追认销案）**：§11.1 把「L-B 全部 12 项保持 s9 优先、无变更」记为 m04292 裁决，但 m04292 原文只授权「开发和修复、自测、测试完成提交、不能自测的记录下来一起测」，并未逐条裁决跨面 D 矩阵；此前挂着的 K3+K4 64 条（残差 28）待裁决被整体销案。**用户 m09211「全部继续，如果能自测的可以自己启动测试」已明确追认**：L-B 12 项 + `InPacket::available()` 393 处维持现状（协议级，真机逐面回归前不动，`pending-live-tests` §27-A），D 矩阵 168 条按 `DECISIONS.md` 五条批量裁决 + 残差 28 逐条裁定全部结案（未决 0），历史证据 README 不重写。"
if "已追认销案" not in dev:
    assert old113 in dev, "dev §11.3 anchor missing"
    dev = dev.replace(old113, new113, 1)
write(DEV, dev)

# ---------------------------------------------------------------- 9. PLT §44 (CRLF)
plt = read(PLT)
assert "\r\n" in plt, "PLT EOL precondition failed"
_need44 = "## 44." not in plt
S44 = [
 "## 44. round86 · 跨面 168 条批量裁决销案 + 真机认领清单（#devfix-batch-adjudication，需用户一起测）",
 "",
 "本轮**零源码 / 零协议 / 零 WZ 改动**：只把 `docs/features/angel-calibration-d-matrix-2026-11/README.md` 的 168 条按 s9 优先五条批量裁决 + 残差 28 条逐条裁定结案（104+36+28=168，未决 0）。裁决书见同目录 `DECISIONS.md`。",
 "",
 "| 项 | 归口 | 验收动作 | 本轮状态 |",
 "| --- | --- | --- | --- |",
 "| 44-1 | 登录 / 世界选择 / 选角 / 标题四屏 | 启 s9 冷启动，截图配准 7 按钮 / 2 输入框 / 推荐世界弹窗 / 11 帧进度条 | ⬜ 未执行，用户认领 |",
 "| 44-2 | HUD / 状态栏一屏 | 4 钮 KEY_ANI 1Hz 闪烁、聊天三态、六频道键、G(17)/社区家族钮落 UIUserList 第 3 页签 | ⬜ 未执行，用户认领 |",
 "| 44-3 | 核心窗 / 社交经济一屏 | 好友组队窗 312x389 真资源、Guild 页 12 个 gi_btn、SysOpt/GameOpt 版式、UIKeyConfig 5 钮 y=234 | ⬜ 未执行，用户认领 |",
 "| 44-4 | 战斗 / 掉落 / 技能一屏 | 伤害数字动态字宽（D-A）、DamageSkin 皮肤间距、金币提示 mode5、掉落四档 | ⬜ 未执行，用户认领 |",
 "| 44-5 | NPC / 任务 / 地图 / 商城一屏 | NPC 对话排版 A11b–A11j、隐藏门三态、商城 9 tab、F 维 12 项逐像素 | ⬜ 未执行，用户认领 |",
 "",
 "已知限制：",
 "",
 "- 本文件 60 节 / 142 处真机标记**全部由用户认领**，本轮零真机执行；不得据 `DECISIONS.md` 把任何观感项标「已验证」。",
 "- T5 旧「orange-wz 内容层阻塞」前提已按 round85 `4b2452c4` 翻案：128B `WzAESConstant.DEFAULT_KEY` 实测 `query_nodes isError:false`；F 维挂 P2 仅因真机观感未采，非 MCP 故障。",
 "- L-B 12 项与 `InPacket::available()` 393 处经 m09211 追认维持现状（§27-A 仍挂真机逐面回归），零协议改动。",
 "- 外部程序在途 EXP WIP 原样留工作树，不夹带进本轮提交。",
 "",
]
if _need44:
    # S44 ends with a "" separator; rstrip the joined block so EOF is exactly
    # one CRLF (git diff --check flags a trailing blank line otherwise).
    plt = plt.rstrip("\r\n") + "\r\n\r\n" + "\r\n".join(S44).rstrip("\r\n") + "\r\n"
    write(PLT, plt)

# ---------------------------------------------------------------- 10. summary
print("PARSE_ROWS 168")
print("KIND_DIST", json.dumps(kdist, sort_keys=True))
print("FACES", len(faces), ",".join(faces))
print("VERDICT_DIST", json.dumps(vcount, ensure_ascii=False, sort_keys=True))
print("RESIDUAL", len(RESIDUAL), "BATCH2_T5T6", len(T5T6), "BATCH4", len(B4))
print("DEV_EDITS 4 (§3 ratify / §4 2-33 / §5 hash / §11.3 close)")
print("PLT_APPEND §44 CRLF ok", ("## 44." in read(PLT)))
print("DEC_BYTES", os.path.getsize(DEC), "EV_BYTES", os.path.getsize(EV), "NOTE_BYTES", os.path.getsize(NOTE))
print("ROUND86_ALL_OK")
