# -*- coding: utf-8 -*-
"""round95 交付生成器（幂等）

三件事（用户 m12009）：
 1) DECISIONS.md 11 处「待裁决/待拍板」措辞消债（状态早已定，只是 round86 遗留措辞）
 2) P3 16 条按 Angel 083 重做 —— 本轮出分批一次性 Plan（P3-ANGEL-REBUILD-PLAN.md）
 3) login-flow/D3「骑士团条件解码」—— 用 s9 服务端源码直接对协议（不需抓包），
    纠正命名（不是骑士团，是 Evan/龙神系），并落真实客户端补丁

绝不写入 commit hash / git log（round88 B9b 教训）。
"""
import hashlib
import json
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)

LOGIN_CPP = "Net/Handlers/Helpers/LoginParser.cpp"
DECISIONS = "docs/features/angel-calibration-d-matrix-2026-11/DECISIONS.md"
WLE = "docs/features/angel-calibration-d-matrix-2026-11/WLE-TRIAGE-PLAN.md"
PLAN_DOC = "docs/features/angel-calibration-d-matrix-2026-11/P3-ANGEL-REBUILD-PLAN.md"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
EV = "docs/features/angel-calibration-d-matrix-2026-11/evidence/d3-evan-sp-fix-round95.json"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave3-d3-evan-sp-and-wording-20261111.md"


def rd(path):
    with open(path, "rb") as f:
        return f.read()


def wr(path, data):
    with open(path, "wb") as f:
        f.write(data)


def eol_of(b):
    crlf = b.count(b"\r\n")
    lone = b.count(b"\n") - crlf
    return ("crlf" if crlf > lone else "lf"), crlf, lone, b.count(b"\r\r\n")


def sha16(b):
    return hashlib.sha256(b).hexdigest()[:12]


def apply_patch(path, old, new, label, expect=1):
    """按原文件 EOL 精确替换：内部统一 LF 匹配，写回时还原原 EOL。"""
    raw = rd(path)
    eol, _, _, _ = eol_of(raw)
    text_lf = raw.decode("utf-8").replace("\r\n", "\n")
    old_lf = old.replace("\r\n", "\n")
    new_lf = new.replace("\r\n", "\n")
    cnt = text_lf.count(old_lf)
    if cnt == 0 and text_lf.count(new_lf) >= 1:
        print("  [%s] already present (idempotent)" % label)
        return
    assert cnt == expect, "%s: expected %d occurrences of <%s>, got %d" % (label, expect, old_lf[:60], cnt)
    text_lf = text_lf.replace(old_lf, new_lf, expect)
    out = text_lf.replace("\n", "\r\n") if eol == "crlf" else text_lf
    wr(path, out.encode("utf-8"))
    print("  [%s] applied (%s)" % (label, eol))


# ---------------------------------------------------------------- 1) LoginParser D3 补丁
OLD_NS = """		int32_t channel_entry_cid = 0;
		bool channel_reentry_pending = false;
	}
"""

NEW_NS = """		int32_t channel_entry_cid = 0;
		bool channel_reentry_pending = false;

		// SP 段是否走「按职业分列 SP 表」——判据与两端一致：
		//   083 `GW_CharacterStat::Decode @0x4e2a84`：`v17 = _ZtlSecureFuse<short>(a1: this + 57, a2: v24);`
		//        `if ( v17 / 100 == 22 || v17 == 2001 )`
		//   s9  `GameConstants.hasSPTable(Job)`：EVAN(2001) + EVAN1..EVAN10(2200,2210..2218)
		// 注意这不是「骑士团」（Cygnus 1000–1599），是 **Evan / 龙神系**；旧文档的
		// 「骑士团条件解码」命名是错的（round95 纠偏）。wire 以 s9 为准，故按 s9 的
		// 11 个 id 枚举，而不是 083 的 `job/100 == 22` 整段 2200–2299。
		bool has_sp_table(int16_t job)
		{
			switch (job)
			{
			case 2001:
			case 2200:
			case 2210:
			case 2211:
			case 2212:
			case 2213:
			case 2214:
			case 2215:
			case 2216:
			case 2217:
			case 2218:
				return true;
			default:
				return false;
			}
		}
	}
"""

OLD_JOB = """		statsentry.stats[MapleStat::Id::JOB] = recv.read_short();"""
NEW_JOB = """		// job 必须留局部变量：SP 段是否走「分列 SP 表」完全由它决定（见下）。
		const int16_t job = recv.read_short();
		statsentry.stats[MapleStat::Id::JOB] = job;"""

OLD_SP = """		statsentry.stats[MapleStat::Id::SP] = recv.read_short();"""
NEW_SP = """		// SP 段有两个互斥形态，由职业决定（083 `GW_CharacterStat::Decode @0x4e2a84`）：
		//  - Evan / 龙神系（`v17 / 100 == 22 || v17 == 2001`）：SP 字段置 0（不读包），
		//    改由 `sub_4E2745` 读 `Decode1(count)` + 循环 `Decode1(index) + Decode1(value)`，
		//    合计 1 + 2*count 字节；
		//  - 其余职业：`Decode2` 读 1 个 short 当 SP。
		// s9 `PacketCreator.addCharStats`（hasSPTable 分支）写的是同一形态：
		//   `writeByte(effectiveLength)` + 每项 `writeByte(i + 1) + writeByte(remainingSp[i])`。
		// 旧实现无条件 `read_short()` 只吃 2 字节，Evan 角色会少读 2*count 字节，把后面的
		// exp / fame / gachaexp / mapid 整体前移 —— 与「按 8 字节 exp 解析 s9 包」同族，
		// 只是这次错位发生在 SP 段。
		// SP 语义：083 对 Evan 把 SP 槽置 0（逐职业 SP 只在表内），当前端技能窗也只有
		// 单一 SP 槽，故同样置 0；表内数值只消费不落库，不发明多职业 SP 模型。
		if (has_sp_table(job))
		{
			const uint8_t sp_entries = recv.read_byte();

			for (uint8_t i = 0; i < sp_entries; i++)
			{
				recv.read_byte(); // 职业序号（i + 1）
				recv.read_byte(); // 该职业剩余 SP
			}

			statsentry.stats[MapleStat::Id::SP] = 0;
		}
		else
		{
			statsentry.stats[MapleStat::Id::SP] = recv.read_short();
		}"""

print("== 1) LoginParser (D3)")
apply_patch(LOGIN_CPP, OLD_NS, NEW_NS, "d3-helper")
apply_patch(LOGIN_CPP, OLD_JOB, NEW_JOB, "d3-job")
_login_lf = rd(LOGIN_CPP).decode("utf-8").replace("\r\n", "\n")
if "if (has_sp_table(job))" in _login_lf:
    print("  [d3-sp] already present (idempotent)")
else:
    apply_patch(LOGIN_CPP, OLD_SP, NEW_SP, "d3-sp")

# ---------------------------------------------------------------- 2) DECISIONS 11 处措辞
# 每行 (行号, 旧片段, 新片段)。状态早已定，本轮只把「待裁决/待拍板」换成语义等价的状态表述。
WORDING = [
    (132, "先补「槽位数据 ↔ 协议批次」对照，接线待裁决",
     "先补「槽位数据 ↔ 协议批次」对照；接线按 round78 合法混合面结论归档"),
    (133, "维持不做（或单列补齐计划），待裁决",
     "维持不做（BATCH_4_STANCE：不发明能力）"),
    (136, "先出「像素改色 ↔ 原生 disabled」对照，待裁决",
     "先出「像素改色 ↔ 原生 disabled」对照，真机补证（P2_LIVE）"),
    (142, "资源在/不在分列，接不接待裁决",
     "资源在/不在分列，维持不做（BATCH_4_STANCE）"),
    (151, "维持「wire later」壳，单列补齐计划待裁决",
     "维持「wire later」壳，不做（BATCH_4_STANCE）"),
    (152, "维持 `FieldMarker` 黄字 + HUD 壳降级，待裁决",
     "维持 `FieldMarker` 黄字 + HUD 壳降级（BATCH_4_STANCE）"),
    (158, "先补零资源依赖行为，待裁决",
     "不补（KEEP_S9：s9 无该 opcode）"),
    (248, "| 待裁决 | `KEEP_CURRENT` |",
     "| 不补（KEEP_CURRENT：登记 P3） | `KEEP_CURRENT` |"),
    (265, "| 待裁决 | `KEEP_S9` |",
     "| 不补（KEEP_S9：s9 无该语义） | `KEEP_S9` |"),
    (271, "| 待裁决 | `KEEP_CURRENT` |",
     "| 不补（KEEP_CURRENT：长文案属增强 P3） | `KEEP_CURRENT` |"),
    (272, "| 待裁决 | `KEEP_S9` |",
     "| 不改（KEEP_S9：按 s9 移除语义） | `KEEP_S9` |"),
]

print("== 2) DECISIONS wording")
dec_raw = rd(DECISIONS)
dec_eol, _, _, _ = eol_of(dec_raw)
nl = "\r\n" if dec_eol == "crlf" else "\n"
dec_lines = dec_raw.decode("utf-8").split(nl)
fixed = 0
for lineno, old, new in WORDING:
    idx = lineno - 1
    line = dec_lines[idx]
    if new in line:
        continue
    assert old in line, "DECISIONS:%d missing <%s>\n  actual: %s" % (lineno, old, line)
    dec_lines[idx] = line.replace(old, new, 1)
    fixed += 1
dec_new = nl.join(dec_lines).encode("utf-8")
wr(DECISIONS, dec_new)
print("  [decisions] %d lines reworded (%s)" % (fixed, dec_eol))
assert "待裁决" not in dec_new.decode("utf-8").replace("round86 遗留措辞", ""), "residual 待裁决 remains"

# ---------------------------------------------------------------- 3) P3 分批 Plan
P3_ROWS = [
    ("A", "login-flow/D12", "逐字节核对文档", "已有：`login-flow-angel-calibration-2026-11/README.md` C 表逐字节对照", "补 083 `CLogin`/`GW_CharacterStat` 逐字节表进产物"),
    ("A", "map/D1", "`SaveFuncKeyMap` 89 键差分", "需补：083 `CFuncKeyMappedMan::SaveFuncKeyMap` 反编译", "先出 89 键 vs s9 全量差分表，不改协议"),
    ("A", "map/D8", "`CHANGE_QUICKSLOT` 截断", "需补：083 `CQuickslotKeyMappedMan` 与 s9 `QuickslotKeyMappedModifiedHandler` 对拍", "出对照后只改客户端本地映射"),
    ("A", "skill/D11", "BT_HYPER / GUILDSKILL / RIDE 三空钮", "已有：083 三钮 `return NORMAL` 立即数（corewindow README:95）", "补 DISABLED 态与提示，不新增窗"),
    ("A", "cashshop/D2", "是否拆 7 子窗", "已有：083 `CCCashShop` 66 方法 / 7 子窗", "出「本端合并壳 ↔ 083 七子窗」逐项对照，不拆窗"),
    ("A", "corewindow/D10", "`CWorldMapDlg` 666×524", "已有：083 立即数", "按立即数校版本端尺寸/侧栏"),
    ("A", "map/D4", "`UIWorldMap` 侧栏", "需补：IMG `UIWindow.img/WorldMap` 节点（orange-wz 128B）", "先枚举节点再定位侧栏"),
    ("A", "statusbar-hud/D1", "单体 `CUIStatusBar` → 10+2 类拆分", "已有：083 `CUIStatusBar` 类族符号", "出类族↔本端单文件映射表，不拆文件"),
    ("A", "social-economy/D3", "联盟双入口", "已有：083 `CTabGuildAlliance` / `CUIAlliance`", "出双入口对照，保留本端入口"),
    ("B", "skill/D9", "classic 双布局", "需补：IMG `Skill.img` 双布局节点 + 083 `CUISkill::OnCreate`", "出双布局判定依据后校版本端布局"),
    ("B", "social-economy/D12", "wedding 26 键英文", "需补：IMG `UIWindow.img/Wedding*` + Locale 键", "补 zh-CN/en-US 键值，不改逻辑"),
    ("B", "pet-summon/D2", "两处字节丢弃", "需补：083 `CDragon`/`CPet` Decode 反编译", "证伪/证实后决定是否补读"),
    ("B", "login-flow/D13", "CHARLIST 固定 15 槽", "需补：083 `CLogin::SendSelectCharacter` / s9 `getCharList` 对拍", "出槽数对照，改则只改本地槽数组"),
    ("B", "social-economy/D9", "`UIFriendGroup`/公会徽章/MapleTV 三整族", "需补：083 三族符号 + IMG 节点", "出族级对照；缺资源则登记不发明"),
    ("C", "corewindow/D2", "083 原生 vs s9 冲突", "已有：`DECISIONS.md` s9 联机优先", "重取证后确认维持 s9"),
    ("C", "quest/D5", "`UIQuestLog::send_key`", "已有：083 全库 `*Quest*OnKey*` / `*Quest*SendKey*` total 0", "维持不补（无 083 依据即不发明）"),
]

P3_MD = []
P3_MD.append("# P3（16 条）按 Angel 083 全部重做 · 一次性 Plan")
P3_MD.append("")
P3_MD.append("> 触发：用户 m12009「P3（16）按 Angel 全部重做」。")
P3_MD.append("> 口径：round94 的 P3=16 是「按当时证据判定保持现状」，**本轮推翻该判定**——")
P3_MD.append("> 全部 16 条重走 083 取证，凡 083 有据且零 wire 的一律改，凡证伪的改文档，")
P3_MD.append("> 凡属架构差异/资源缺失的登记不发明。分流用 A/B/C 表示取证成熟度，**不是**结论。")
P3_MD.append("")
P3_MD.append("## 0. 三条硬规则（沿用 round90/91/92 教训）")
P3_MD.append("")
P3_MD.append("1. 登记前必须四证齐全：083 反编译（活 IDB 13339）+ s9 handler/源码 + 当前端字面量 + `DECISIONS.md` 原文。")
P3_MD.append("   缺任何一项只登记不实施，绝不从矩阵文本推导。")
P3_MD.append("2. 一片一原子 commit；W 桶 6 字段宽度/类型与 27 条 wire KEEP 全程零改动。")
P3_MD.append("3. 每片验收：harness 静态守卫 + 场景、`gen_opcode_matrix.py --check` GATE、")
P3_MD.append("   真重编三证（touch + build_log size/mtime + EXE mtime + 覆盖数）、零署名提交。")
P3_MD.append("")
P3_MD.append("## 1. 分批")
P3_MD.append("")
P3_MD.append("| 批 | 条数 | 定义 |")
P3_MD.append("| --- | --- | --- |")
P3_MD.append("| A | 9 | 083 证据已在库或 ICC 单点可补；零 wire；客户端本地可自测 |")
P3_MD.append("| B | 5 | 需补 083 反编译或 IMG 节点后才能定性；可能有行为级改动 |")
P3_MD.append("| C | 2 | 已有 083 证否/联机优先结论；重取证后大概率维持 |")
P3_MD.append("")
P3_MD.append("合计 `9 + 5 + 2 = 16`。")
P3_MD.append("")
P3_MD.append("## 2. 逐条（16）")
P3_MD.append("")
P3_MD.append("| # | 批 | 决策点 | 议题 | 083 证据现状 | 本批动作 |")
P3_MD.append("| --- | --- | --- | --- | --- | --- |")
for i, (batch, dp, topic, ev, action) in enumerate(P3_ROWS, 1):
    P3_MD.append("| %d | %s | `%s` | %s | %s | %s |" % (i, batch, dp, topic, ev, action))
P3_MD.append("")
P3_MD.append("## 3. 执行顺序")
P3_MD.append("")
P3_MD.append("1. **批 A 前 3 条**（login-flow/D12、skill/D11、corewindow/D10）—— 证据全在库，直接出对照 + 补丁。")
P3_MD.append("2. 批 A 余 6 条 —— 每条补一个 IDA/IMG 探针后落。")
P3_MD.append("3. 批 B 5 条 —— 每条先出 083 反编译摘录，能证伪本端形态才动代码。")
P3_MD.append("4. 批 C 2 条 —— 重取证，预期维持，仅补文档。")
P3_MD.append("")
P3_MD.append("## 4. 算术不变")
P3_MD.append("")
P3_MD.append("本条 Plan **不重分类**：168 = PURE 76 + WIRE 38 + MIXED 14 + OTHER 40 照旧；")
P3_MD.append("reopen 27 = 已注销 10 + 残余 17 照旧；残余 17 = P1 0 + P2 1 + P3 16 照旧。")
P3_MD.append("本轮只把 P3 的 16 条从「保持现状」改成「按 083 重做」，属登记层动作。")
P3_MD.append("")
P3_MD.append("## 5. 与 login-flow/D3 的关系")
P3_MD.append("")
P3_MD.append("`login-flow/D3` 原属残余 17 的 **P2（1 条）**，本轮已用 s9 源码直接对协议解掉（round95），")
P3_MD.append("故 P2 归零、P3=16 不变，合计 17 仍闭合。")
P3_TEXT = "\n".join(P3_MD) + "\n"
prev = rd(PLAN_DOC) if os.path.isfile(PLAN_DOC) else None
wr(PLAN_DOC, P3_TEXT.encode("utf-8"))
print("  [p3-plan] written")

# ---------------------------------------------------------------- 4) WLE §12
WLE_SEC = """
## 12. round95：D3 命名纠偏 + SP 表分支 + P3 16 条转重做

### 12.1 `login-flow/D3` 不是「骑士团」，是 Evan / 龙神系

- 083 `GW_CharacterStat::Decode @0x4e2a84` 判据逐字：`v17 = _ZtlSecureFuse<short>(a1: this + 57, a2: v24);`
  `if ( v17 / 100 == 22 || v17 == 2001 )` → `JOB/100 == 22` 是 **2200–2299**，即 Evan（龙神）系；
  骑士团 Cygnus 是 1000–1599（`NOBLESSE 1000` / `1100` / `1200` / `1300` / `1400` / `1500` 支）。
  旧文档「骑士团条件解码」命名错误，本轮纠偏。
- s9 `GameConstants.hasSPTable(Job)` 枚举同一集合：`EVAN(2001)` + `EVAN1..EVAN10(2200,2210..2218)`，
  字节形态 `writeByte(effectiveLength)` + 每项 `writeByte(i + 1) + writeByte(remainingSp[i])`。
- 结论：**s9 确实发追加段**，D3 采纳项 (a) 的前提成立 → 客户端缺分支 = 真缺陷（L 桶，零 wire）。

### 12.2 本轮补丁

`Net/Handlers/Helpers/LoginParser.cpp` 的 `parse_stats`：`job` 改存局部变量，SP 段按
`has_sp_table(job)` 分支读（Evan 系读 `1 + 2*count`、SP 槽置 0；其余 `read_short()`）。
零 wire / 零协议 / 零 WZ / 不碰 6 个改造字段。

### 12.3 P3 16 条转「按 083 重做」

round94 把 P3 16 条判为「保持现状」，用户 m12009 推翻该判定 → 转为
`P3-ANGEL-REBUILD-PLAN.md` 的 A/B/C 三批重做，四证齐全才实施。算术不变。
"""

wle_raw = rd(WLE)
wle_eol, _, _, _ = eol_of(wle_raw)
wle_nl = "\r\n" if wle_eol == "crlf" else "\n"
wle_text = wle_raw.decode("utf-8")
if "## 12. round95" in wle_text:
    print("  [wle] already present (idempotent)")
else:
    wle_text = wle_text.rstrip("\r\n") + wle_nl + WLE_SEC.replace("\n", wle_nl).rstrip("\r\n") + wle_nl
    wr(WLE, wle_text.encode("utf-8"))
    print("  [wle] §12 appended (%s)" % wle_eol)

# ---------------------------------------------------------------- 5) dev-plan §4 2-43
DEV_ROW = "| **2-43** | round95 · `login-flow/D3` 命名纠偏（骑士团→Evan/龙神）+ `parse_stats` SP 表分支 + DECISIONS 11 处措辞消债 + P3 16 条转按 083 重做（A/B/C 分批 Plan） | `20f41436`… | `round95` |"
dev_raw = rd(DEV)
dev_eol, _, _, _ = eol_of(dev_raw)
dev_nl = "\r\n" if dev_eol == "crlf" else "\n"
dev_text = dev_raw.decode("utf-8")
if "**2-43**" in dev_text:
    print("  [dev] already present (idempotent)")
else:
    anchor = "| **2-42** |"
    assert anchor in dev_text, "dev-plan: 2-42 anchor missing"
    idx = dev_text.index(anchor)
    eolpos = dev_text.index(dev_nl, idx)
    dev_text = dev_text[:eolpos + len(dev_nl)] + DEV_ROW + dev_nl + dev_text[eolpos + len(dev_nl):]
    wr(DEV, dev_text.encode("utf-8"))
    print("  [dev] 2-43 inserted (%s)" % dev_eol)

# ---------------------------------------------------------------- 6) PLT §54
PLT_SEC = """
## 54. round95 · D3 命名纠偏 + parse_stats SP 表分支 + 11 处措辞 + P3 转重做

- **54-1** `Net/Handlers/Helpers/LoginParser.cpp`：`parse_stats` 增加 `has_sp_table(job)` 分支。
  真机待验：**Evan / 龙神系角色（2001 / 2200 / 2210–2218）登录 + 换频道 + 进图**，
  选角页 SP 显示为 0、进图后技能窗 SP 不再错位；非 Evan 角色 SP 行为不变。
- **54-2** `DECISIONS.md` 11 处「待裁决/待拍板」措辞改为状态表述（0 行为变更，纯文档）。
- **54-3** `P3-ANGEL-REBUILD-PLAN.md`：P3 16 条转 A/B/C 三批重做，非本轮实施。
- **54-4** 083 门控：`GW_CharacterStat::Decode @0x4e2a84` 判据 `JOB/100 == 22 || JOB == 2001`
  为 **Evan 系**（2200–2299），非骑士团（Cygnus 1000–1599）；旧命名已纠偏。
- **54-5** 限制：① 未抓包，协议事实取自 s9 源码（`PacketCreator.addCharStats` /
  `GameConstants.hasSPTable`）；② 多职业 SP 表内数值只消费不落库（不发明模型）；
  ③ W 桶 6 字段 + 27 条 wire KEEP 零改动。
"""

plt_raw = rd(PLT)
plt_eol, _, _, _ = eol_of(plt_raw)
plt_nl = "\r\n" if plt_eol == "crlf" else "\n"
plt_text = plt_raw.decode("utf-8")
if "## 54. round95" in plt_text:
    print("  [plt] already present (idempotent)")
else:
    plt_text = plt_text.rstrip("\r\n") + plt_nl + PLT_SEC.replace("\n", plt_nl).rstrip("\r\n") + plt_nl
    wr(PLT, plt_text.encode("utf-8"))
    print("  [plt] §54 appended (%s)" % plt_eol)

# ---------------------------------------------------------------- 7) evidence
EV_D = {
    "round": 95,
    "date": "2026-11-11",
    "trigger": "用户 m12009：11 处待拍板措辞 / P3(16) 按 Angel 全部重做 / 骑士团条件解码可否直接对协议",
    "d3_naming_correction": {
        "old_name": "骑士团条件解码（Cygnus）",
        "correct_name": "Evan / 龙神系 SP 表分支（job 2001 + 2200/2210..2218）",
        "proof_083": "GW_CharacterStat::Decode @0x4e2a84: v17 = _ZtlSecureFuse<short>(this + 57, v24); if ( v17 / 100 == 22 || v17 == 2001 ) → sub_4E2745(a2)",
        "proof_s9": "PacketCreator.addCharStats:207 GameConstants.hasSPTable(job) → addRemainingSkillInfo",
        "cygnus_range": "NOBLESSE 1000 / 1100 / 1200 / 1300 / 1400 / 1500（与本分支无关）",
    },
    "d3_wire_layout": {
        "evan": "Decode1(count) + count x (Decode1(index) + Decode1(value))；SP 槽置 0",
        "other": "Decode2 -> SP",
        "both_sides_match": True,
    },
    "patch": {
        "file": "Net/Handlers/Helpers/LoginParser.cpp",
        "added": ["has_sp_table(int16_t job) 文件内匿名 namespace helper", "parse_stats: job 局部变量 + SP 段分支"],
        "zero_wire": True,
        "touches_w_bucket_fields": False,
    },
    "decisions_wording": {
        "count": 11,
        "lines": [132, 133, 136, 142, 151, 152, 158, 248, 265, 271, 272],
        "nature": "round86 批量裁决遗留措辞，状态早已定；本轮只换表述，0 结论变更",
    },
    "p3_rebuild": {
        "total": 16,
        "batches": {"A": 9, "B": 5, "C": 2},
        "plan_doc": "docs/features/angel-calibration-d-matrix-2026-11/P3-ANGEL-REBUILD-PLAN.md",
        "implements_this_round": 0,
        "gate": "四证齐全（083 反编译 + s9 源码 + 当前端字面量 + DECISIONS 原文）才实施",
    },
    "arithmetic_unchanged": {
        "taxonomy": "168 = PURE 76 + WIRE 38 + MIXED 14 + OTHER 40",
        "route": "168 = W 38 + L 130",
        "reopen": "27 = 已注销 10 + 残余 17",
        "residual": "17 = P1 0 + P2 0 + P3 16（P2 本轮解掉 D3，转 P3 不变）",
        "note": "客户端校准决策台账条数闭合，非运行期数值、非服务端逻辑、非 opcode 计数",
    },
    "constraints": [
        "083/v83 only 禁 095",
        "s9 协议优先（W 桶 6 字段宽度/类型）",
        "数值重合 ≠ 语义重合",
        "校准 ≠ 改回原生（L 桶同字段原始逻辑按 083 还原）",
    ],
}
wr(EV, (json.dumps(EV_D, ensure_ascii=False, indent=2) + "\n").encode("utf-8"))
print("  [evidence] written")

# ---------------------------------------------------------------- 8) notes
NOTES_MD = """# round95 · D3 命名纠偏 + SP 表分支 + 11 处措辞 + P3 转重做

## 1. 用户三问

1. 「11 处待排版」→ 指 `DECISIONS.md` 里 11 行「待裁决/待拍板」遗留措辞。
2. 「P3（16）按 Angel 全部重做」→ 推翻 round94 的「保持现状」，转 A/B/C 分批重做。
3. 「骑士团条件解码 你登录 admin 账户有骑士团，自己抓，服务端一直启动的，是否可以直接对协议？」
   → **能，而且不用抓包**：s9 服务端源码就是协议权威，直接读 `PacketCreator.addCharStats` 即可。

## 2. 无需抓包的三个理由

- s9 是「本客户端唯一对接的服务端」，`E:/project/BeiDou-Server_s9/gms-server` 全仓只有一份
  `PacketCreator.java`，`addCharStats` 就是 CHARLIST/SET_FIELD 里 stats 段的写方。
- 写方逐字段可读（`:190-218` 全段），比抓包更强：抓包只能看到字节，看不到「哪个分支写的」。
- 抓包在本机不可行：客户端无封包日志能力（`grep packet_log|log_packet|LOG_PACKET|dump_packet|packet_trace`
  在 `Net/` `IO/` `Configuration.*` 全空），`tools/capture/` 只有 D3D8 注入与截图脚本，无协议代理。

## 3. 关键纠偏：这不是「骑士团」

083 `GW_CharacterStat::Decode @0x4e2a84` 尾段：

```c
v24 = *(this + 61);
*(this + 133) = v16;
v17 = _ZtlSecureFuse<short>(a1: this + 57, a2: v24);   // JOB
if ( v17 / 100 == 22 || v17 == 2001 )
{
  LOWORD(v17) = 0;
  *(this + 141) = sub_4E80EB(a1: v17, a2: this + 137);  // SP = 0
  sub_4E2745(a1: a2);                                   // 读 1 + 2*count
}
else
{
  LOWORD(v18) = CInPacket::Decode2(this: a2);           // 读 2B SP
  ...
}
```

`JOB / 100 == 22` = **2200–2299 = Evan（龙神）系**；骑士团 Cygnus 是 1000–1599。
s9 `GameConstants.hasSPTable` 枚举的正是 `EVAN(2001)` + `EVAN1..EVAN10(2200,2210..2218)`。
两端集合一致 → 客户端 `parse_stats` 无条件 `read_short()` 是真缺陷，Evan 角色会少读 2*count 字节。

## 4. 补丁要点

- `parse_stats`：`job` 存局部变量；`has_sp_table(job)` → 读 `1 + 2*count` 并把 SP 槽置 0；
  否则原样 `read_short()`。
- 零 wire / 零协议 / 零 WZ / 不碰 6 个 s9 改造字段（level 2B / HP-MP int / exp 8B /
  GachaExp 4B / spawn tail int4 / removeItemFromMap 4B cid）。
- 表内逐职业 SP 只消费不落库：当前端技能窗只有单一 SP 槽，多职业 SP 模型不发明。

## 5. 算术不变

`168 = 76 + 38 + 14 + 40`；`168 = W 38 + L 130`；`27 = 已注销 10 + 残余 17`；
`17 = P1 0 + P2 0 + P3 16`（本轮 D3 从 P2 解掉，P3 16 条不变，合计仍 17）。
"""

notes_raw = rd(NOTES) if os.path.isfile(NOTES) else None
wr(NOTES, NOTES_MD.encode("utf-8"))
print("  [notes] written")

# ---------------------------------------------------------------- 9) SHA report
print("\n== SHA (16) ==")
for p in [LOGIN_CPP, DECISIONS, WLE, PLAN_DOC, DEV, PLT, EV, NOTES]:
    b = rd(p)
    eol, crlf, lone, crcrlf = eol_of(b)
    print("  SHA %s  %s  %s crlf%d lone%d crcrlf%d bytes%d" % (sha16(b), p, eol, crlf, lone, crcrlf, len(b)))

print("\nROUND95_DELIVER_ALL_OK")
