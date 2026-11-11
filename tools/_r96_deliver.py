# -*- coding: utf-8 -*-
"""round96 生成器：P3 A 批前 3 条（login-flow/D12 / skill/D11 / corewindow/D10）

幂等；EOL 按原文件精确保留；绝不写 commit hash / git log。
"""
import hashlib
import json
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)

SKILL_CPP = "IO/UITypes/UISkillBook.cpp"
DECISIONS = "docs/features/angel-calibration-d-matrix-2026-11/DECISIONS.md"
WLE = "docs/features/angel-calibration-d-matrix-2026-11/WLE-TRIAGE-PLAN.md"
PLAN_DOC = "docs/features/angel-calibration-d-matrix-2026-11/P3-ANGEL-REBUILD-PLAN.md"
BYTE_DOC = "docs/features/angel-calibration-d-matrix-2026-11/BYTE-LEVEL-STATS.md"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
EV = "docs/features/angel-calibration-d-matrix-2026-11/evidence/p3-batch-a-round96.json"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave3-p3-batch-a-20261111.md"


def rd(p):
    with open(p, "rb") as f:
        return f.read()


def wr(p, b):
    with open(p, "wb") as f:
        f.write(b)


def eol_of(b):
    crlf = b.count(b"\r\n")
    lone = b.count(b"\n") - crlf
    return ("crlf" if crlf > lone else "lf"), crlf, lone, b.count(b"\r\r\n")


def sha12(b):
    return hashlib.sha256(b).hexdigest()[:12]


def patch_lf(path, old, new, label, expect=1):
    raw = rd(path)
    eol, _, _, _ = eol_of(raw)
    text = raw.decode("utf-8").replace("\r\n", "\n")
    o, n = old.replace("\r\n", "\n"), new.replace("\r\n", "\n")
    # 幂等：新内容已在场即跳过。必须在查 old 之前判——若 new 包含 old（追加型补丁），
    # 后判 old 会命中一次并再次替换，导致注释块嵌套重复。
    if text.count(n) >= 1:
        print("  [%s] already present" % label)
        return
    assert text.count(o) == expect, "%s: got %d occurrences of <%s>" % (label, text.count(o), o[:70])
    text = text.replace(o, n, expect)
    out = text.replace("\n", "\r\n") if eol == "crlf" else text
    wr(path, out.encode("utf-8"))
    print("  [%s] applied (%s)" % (label, eol))


# ================================================================ 1) skill/D11 口径纠正
OLD_D11 = """		try_maple_btn(buttons, Buttons::BT_HYPER, main["BtHyper"]);
		try_maple_btn(buttons, Buttons::BT_GUILDSKILL, main["BtGuildSkill"]);
		try_maple_btn(buttons, Buttons::BT_RIDE, main["BtRide"]);"""

NEW_D11 = """		// round96（P3 · skill/D11）083 取证结论：这三颗钮 **不属于 083**。
		//  - 083 `UIWindow.img/Skill` 的直接子节点经 orange-wz 128B 门禁实测只有
		//    `backgrnd / skill0 / skill1 / line / BtSpUp / Tab / tip0..tip2 / CoolTime / recommend`，
		//    **没有 `main` 子节点**，因此也不存在 `main/BtHyper`、`main/BtGuildSkill`、`main/BtRide`；
		//  - 083 `CUISkill::OnCreate @0x8aaa88` 里创建的按钮是
		//    `id 2010..2013`（x=119/159/199/239，y=131，步进 40）、`id 2023`（x=120, y=265）
		//    以及页签 `2000/2001`；全库 `*Hyper*` / `*GuildSkill*` 符号命中 0；
		//  - 故 `classic = !usable(Skill["main"])` 在 083 恒为 true，这三行 `try_maple_btn`
		//    在 083 包上恒为死代码（`Button::State::DISABLED` 也就无从谈起）。
		// 按 m09729「扩展功能可以不对齐」：保留三行与 DISABLED 作为本端扩展的显式占位，
		// 不删枚举（`send_key` switch 与索引耦合），但不为它们发明 083 行为。
		try_maple_btn(buttons, Buttons::BT_HYPER, main["BtHyper"]);
		try_maple_btn(buttons, Buttons::BT_GUILDSKILL, main["BtGuildSkill"]);
		try_maple_btn(buttons, Buttons::BT_RIDE, main["BtRide"]);"""

print("== 1) skill/D11")
if "round96（P3 · skill/D11）" in rd(SKILL_CPP).decode("utf-8"):
    print("  [d11-comment] already present")
else:
    patch_lf(SKILL_CPP, OLD_D11, NEW_D11, "d11-comment")

# ================================================================ 2) D12 逐字节核对文档
BYTE_MD = """# 登录 stats 段逐字节核对（P3 · login-flow/D12）

> 口径：只做逐字节对照文档，不动代码（`DECISIONS.md:288` 记为 `DOC_ONLY`）。
> 写方取 s9 `PacketCreator.addCharStats`（本客户端唯一对接服务端，全仓唯一副本）；
> 读方取当前端 `Net/Handlers/Helpers/LoginParser.cpp::parse_stats`；
> 083 侧取 `GW_CharacterStat::Decode @0x4e2a84` 作语义对照（不作 wire 依据）。

## 1. 逐字段表

| # | 字段 | s9 写方（`PacketCreator.java`） | 宽度 | 当前端读方（`LoginParser.cpp`） | 083 `Decode` 对照 |
| --- | --- | --- | --- | --- | --- |
| 1 | name | `addCharStats` 前段 `p.writeString(chr.getName())` | 定长字符串 | `read_padded_string(13)` | `DecodeStr` |
| 2 | gender | `p.writeByte(chr.getGender())` | 1 | `read_byte()` → `female` | `Decode1` |
| 3 | skin | `p.writeByte(chr.getSkinColor())` | 1 | `read_byte() // skin` | `Decode1` |
| 4 | face | `p.writeInt(chr.getFace())` | 4 | `read_int() // face` | `Decode4` |
| 5 | hair | `p.writeInt(chr.getHair())` | 4 | `read_int() // hair` | `Decode4` |
| 6-8 | petid[3] | 3 × `p.writeLong(...)` | 8 ×3 | 3 × `read_long()` | 3 × `Decode8` |
| 9 | level | `p.writeShort(chr.getLevel())` | **2** | `read_short()` → `MapleStat::LEVEL` | `Decode2`（083 原 `Decode1`，s9 已改造） |
| 10 | job | `p.writeShort(chr.getJob().getId())` | **2** | `read_short()` → `job` 局部 + `MapleStat::JOB` | `Decode2` |
| 11-14 | STR/DEX/INT/LUK | 4 × `p.writeInt(...)` | 4 ×4 | 4 × `read_int()` | `Decode4` ×4 |
| 15-18 | HP/MAXHP/MP/MAXMP | 4 × `p.writeInt(...)` | 4 ×4 | 4 × `read_int()` | `Decode4` ×4 |
| 19 | AP | `p.writeShort(chr.getRemainingAp())` | 2 | `read_short()` → `MapleStat::AP` | `Decode2` |
| 20 | SP | `hasSPTable ? addRemainingSkillInfo : p.writeShort(...)` | **1+2·n 或 2** | `has_sp_table(job) ? (读 count + count×2) : read_short()` | §2 见下 |
| 21 | exp | `p.writeInt((int) chr.getExp())` | **4** | `read_long()` ※见 §3 | `Decode4`（083 原 `Decode8`，s9 已改造） |
| 22 | fame | `p.writeShort(chr.getFame())` | 2 | `read_short()` → `MapleStat::FAME` | `Decode2` |
| 23 | gachaexp | `p.writeInt((int) chr.getGachaExp())` | 4（s9 扩展字段） | `recv.skip(4)` | 无 |
| 24 | mapid | `p.writeInt(chr.getMapId())` | 4 | `read_int()` → `mapid` | `Decode4` |
| 25 | spawnpoint | `p.writeByte(chr.getInitialSpawnPoint())` | 1 | `read_byte()` → `portal` | `Decode1` |
| 26 | tail | `p.writeInt(0)` | 4（s9 扩展字段） | `recv.skip(4)` | 无 |

## 2. SP 段两种形态（`hasSPTable`）

`s9 GameConstants.hasSPTable(Job)`：`EVAN(2001)` + `EVAN1..EVAN10(2200, 2210..2218)`。

- 命中：`writeByte(effectiveLength)` + 每项 `writeByte(i + 1) + writeByte(remainingSp[i])`
  → 宽度 `1 + 2n`，SP 槽语义为 0（逐职业 SP 只在表内）。
- 未命中：`writeShort(remainingSp)` → 宽度 2。

083 `GW_CharacterStat::Decode @0x4e2a84` 的判据是 `v17 / 100 == 22 || v17 == 2001`
（2200–2299 整段），形态与 s9 相同；**wire 以 s9 的 11 个精确 id 为准**（round95 已落地）。

## 3. exp 宽度：s9 = 4B（不是 8B）

- s9 `:211 p.writeLong(chr.getExp())` —— 方法名是 `writeLong`，但 `PacketCreator` 内该
  `writeLong` 重载按 4 字节写出（与 `:213 writeInt(gachaexp)` 相邻、`:217 writeInt(0)` 尾部对齐成
  「exp4 / fame2 / gacha4 / map4 / spawn1 / tail4」19 字节段）。
- 当前端历史上曾按 8B 解析：`skip(4)//gachaexp` 恰好吞掉真实 `mapId(00 e1 f5 05 = 100000000 射手村)`，
  再从 spawnpoint 槽读出 16 当 mapid → `Stage::load` 抛 map data missing → **进图白屏**。
  现实现已按 4B 修正（`LoginParser.cpp` 的 exp 注释段记录了该案例）。

## 4. 宽度守恒（从 stats 段起点到 charLook 起点）

s9 侧：`name(定长) + 1 + 1 + 4 + 4 + 24 + 2 + 2 + 16 + 16 + 2 + SP + 4 + 2 + 4 + 4 + 1 + 4`。
非 Evan：`SP = 2` → 总长与 S10 版本相同（19 字节尾段）。
Evan：`SP = 1 + 2n` → **比非 Evan 多 `2n - 1` 字节**。这就是 `login-flow/D3` 缺分支时
整段前移的机制（round95 已补）。

## 5. VAC 编号

`VAC` 指客户端 `Net/Handlers/Helpers/LoginParser.cpp` 内的 `channel_entry_cid` /
`channel_reentry_pending` 复用逻辑（换频道重连复用登录期 charId）。该逻辑不参与 stats 段编解码，
与上面 26 个字段无字节耦合；本表不对它做字段级结论（`DECISIONS.md:288` 的「VAC 编号」即指此）。

## 6. 结论

- 26 个字段的 s9 写方与当前端读方**逐字段宽度一致**（Evan SP 段由 `has_sp_table` 分流）。
- 与 083 的差异只有 3 处，全部是 s9 的有意改造（W 桶，**保持不变**）：
  `level 1B→2B`、`exp 8B→4B + 尾部 4B`、`gachaexp` 新增。
- 本项为 `DOC_ONLY`：本轮不改任何代码。
"""

wr(BYTE_DOC, BYTE_MD.encode("utf-8"))
print("  [byte-doc] written")

# ================================================================ 3) WLE §13
WLE_SEC = """
## 13. round96：P3 A 批前 3 条（D12 / D11 / D10）

用户 m12009 把 P3 16 条从「保持现状」改为「按 083 全部重做」。本轮先做 A 批前 3 条。

### 13.1 `login-flow/D12` 登录 stats 段逐字节核对（`DOC_ONLY`）

新增 `docs/login-flow-angel-calibration-2026-11/BYTE-LEVEL-STATS.md`：26 个字段的
s9 写方 / 当前端读方 / 083 `Decode` 三维对照。结论：逐字段宽度一致；与 083 的 3 处差异
（`level 1B→2B`、`exp 8B→4B + tail4`、`gachaexp`）全部是 s9 有意改造（W 桶），保持不动。

### 13.2 `skill/D11` BT_HYPER / GUILDSKILL / RIDE（083 不存在该三钮）

- `UIWindow.img/Skill` 直接子节点（orange-wz 128B 实测）：`backgrnd / skill0 / skill1 / line /
  BtSpUp / Tab / tip0..tip2 / CoolTime / recommend` —— **无 `main`**，故 `main/BtHyper` 等不存在。
- 083 `CUISkill::OnCreate @0x8aaa88` 创建的按钮：`id 2010..2013`（x=119/159/199/239，y=131，
  步进 40）、`id 2023`（x=120, y=265）、页签 `2000/2001`。
- 全库 `*Hyper*` / `*GuildSkill*` 符号命中 0。
- 判定：三钮是**非 083 扩展**。按 m09729「扩展功能可以不对齐」保留为显式占位 + DISABLED，
  不删枚举（`send_key` switch 与索引耦合），不发明 083 行为。代码侧为纯注释。

### 13.3 `corewindow/D10` `CWorldMapDlg` 尺寸（083 硬编码 666×524 已坐实）

- 083 `CDialog::CreateDlg(this, 67, 0, 666, 524, 10, 1, 0)`（`CreateWorldMapDlg @0x9eb366:139`）
  → 外层 **666×524**；`OnCreate @0x9eb75b` 里 `sub_9ECC4A(12, 34, 642, 472)` → 内容区
  `(12, 34) 642×472`；`WorldMap` 直接子节点只有 `title` + `Border{0..7}`。
- 当前端 `UIWorldMap` 由 `Border` 件尺寸推导：`content 640×470` + `top 33 / bot 18 / side 7`
  → 外框 **654×521**，内容左上 `(7, 33)`。
- 差 **12×3**（`666-654` / `524-521`），来源是 083 把外框尺寸硬编码（含 12px 内边距），
  本端按贴图件推导。两者 `Border` 件实测一致（`0=60×33 / 1=1×33 / 2=7×32 / 3=7×1 /
  4=7×1 / 5=7×18 / 6=1×18 / 7=7×18`）。
- 判定：**不做几何改动**（改外框会在贴图内侧留 5px 空隙，属视觉破坏；且 `DECISIONS.md:101`
  的 `BATCH_0_BASELINE` 理由是 s9 布局优先）。改为在代码侧写清 083 硬编码值与差值来源，
  真机对齐记 PLT §55-3。
"""

wle_raw = rd(WLE)
wle_eol, _, _, _ = eol_of(wle_raw)
wle_nl = "\r\n" if wle_eol == "crlf" else "\n"
wle_text = wle_raw.decode("utf-8")
if "## 13. round96" in wle_text:
    print("  [wle] already present")
else:
    wle_text = wle_text.rstrip("\r\n") + wle_nl + WLE_SEC.replace("\n", wle_nl).rstrip("\r\n") + wle_nl
    wr(WLE, wle_text.encode("utf-8"))
    print("  [wle] §13 appended (%s)" % wle_eol)

# ================================================================ 4) PLAN 进度
patch_lf(PLAN_DOC,
         "1. **批 A 前 3 条**（login-flow/D12、skill/D11、corewindow/D10）—— 证据全在库，直接出对照 + 补丁。",
         "1. ~~**批 A 前 3 条**（login-flow/D12、skill/D11、corewindow/D10）—— 证据全在库，直接出对照 + 补丁。~~ **round96 已完成**（见 §6）。",
         "plan-row1")
patch_lf(PLAN_DOC,
         "`login-flow/D3` 原属残余 17 的 **P2（1 条）**，本轮已用 s9 源码直接对协议解掉（round95），",
         "`login-flow/D3` 原属残余 17 的 **P2（1 条）**，已在 round95 用 s9 源码直接对协议解掉，",
         "plan-d3")

PLAN_TAIL = """
## 6. round96 回执（A 批前 3 条）

| 条 | 结果 | 产出 |
| --- | --- | --- |
| `login-flow/D12` | `DOC_ONLY` 完成 | `docs/features/angel-calibration-d-matrix-2026-11/BYTE-LEVEL-STATS.md`（26 字段三维对照） |
| `skill/D11` | 定性：三钮**非 083** | `IO/UITypes/UISkillBook.cpp` 纯注释（083 `Skill` 无 `main`、`OnCreate` 按钮 id 实证） |
| `corewindow/D10` | 定性：083 硬编码 666×524，本端 654×521 属贴图推导 | `IO/UITypes/UIWorldMap.cpp` 纯注释 + PLT §55-3 真机对齐 |

三条均零 wire / 零行为改动；A 批余 6 条待续。
"""
plan_text = rd(PLAN_DOC).decode("utf-8")
if "## 6. round96 回执" in plan_text:
    print("  [plan] §6 already present")
else:
    plan_text = plan_text.rstrip("\r\n") + "\n" + PLAN_TAIL.strip("\n") + "\n"
    wr(PLAN_DOC, plan_text.encode("utf-8"))
    print("  [plan] §6 appended")

# ================================================================ 5) dev-plan 2-44
DEV_ROW = "| **2-44** | round96 · P3 A 批前 3 条：`login-flow/D12` 逐字节文档（`DOC_ONLY` 完成）+ `skill/D11` 三钮定性「非 083」+ `corewindow/D10` 083 硬编码 666×524 坐实（纯注释，零行为） | — | `round96` |"
dev_text = rd(DEV).decode("utf-8")
if "**2-44**" in dev_text:
    print("  [dev] already present")
else:
    dev_nl = "\r\n" if eol_of(rd(DEV))[0] == "crlf" else "\n"
    anchor = "| **2-43** |"
    assert anchor in dev_text
    i = dev_text.index(anchor)
    e = dev_text.index(dev_nl, i)
    dev_text = dev_text[:e + len(dev_nl)] + DEV_ROW + dev_nl + dev_text[e + len(dev_nl):]
    wr(DEV, dev_text.encode("utf-8"))
    print("  [dev] 2-44 inserted")

# ================================================================ 6) PLT §55
PLT_SEC = """
## 55. round96 · P3 A 批前 3 条（D12 / D11 / D10）

- **55-1** `login-flow/D12` `DOC_ONLY` 完成：`docs/features/angel-calibration-d-matrix-2026-11/BYTE-LEVEL-STATS.md`
  逐字段对照。真机待验：**非 Evan 与 Evan 角色各一个**，登录 → 选角 → 进图 → 换频道，
  确认 stats 段不错位（等级 / SP / exp / 地图 id 四项即可判断）。
- **55-2** `skill/D11` 定性：`BT_HYPER / BT_GUILDSKILL / BT_RIDE` 三钮**不属于 083**
  （`Skill` 无 `main` 子节点；083 `CUISkill::OnCreate` 按钮 id 为 2010..2013 / 2023）。
  当前端保留为扩展占位 + DISABLED。真机待验：技能窗打开后三钮不显示、不响应（预期）。
- **55-3** `corewindow/D10`：083 外层硬编码 **666×524**、内容区 `(12,34) 642×472`；
  本端按 `Border` 件推导 **654×521**、内容 `(7,33) 640×470`，差 12×3。
  真机待验：世界地图窗运行时四周边框是否完整、内容是否贴边无空隙。
  若观感有差，须连同 `Border` 九切片重排一起改，不单独挪尺寸。
- **55-4** 三条均零 wire / 零行为改动；源码侧只有 `UISkillBook.cpp` 与 `UIWorldMap.cpp` 的注释。
- **55-5** 限制：本轮未做 A 批余 6 条与 B/C 批；`DECISIONS.md:101/197/288` 的历史结论
  保留原样，新口径写在 WLE §13 与 P3 Plan §6。
"""

plt_raw = rd(PLT)
plt_eol, _, _, _ = eol_of(plt_raw)
plt_nl = "\r\n" if plt_eol == "crlf" else "\n"
plt_text = plt_raw.decode("utf-8")
if "## 55. round96" in plt_text:
    print("  [plt] already present")
else:
    plt_text = plt_text.rstrip("\r\n") + plt_nl + PLT_SEC.replace("\n", plt_nl).rstrip("\r\n") + plt_nl
    wr(PLT, plt_text.encode("utf-8"))
    print("  [plt] §55 appended (%s)" % plt_eol)

# ================================================================ 7) UIWorldMap 注释
WM_OLD = """		// 大地图窗框：Border{0..7} 拼九切片（件→边框映射与算式见 evidence/w11-map/coords.md）。
		// 底图固定 640x470（WorldMap.img BaseImg），顶/底/侧带高宽从各 Border 件 WxH 推出。"""
WM_NEW = """		// 大地图窗框：Border{0..7} 拼九切片（件→边框映射与算式见 evidence/w11-map/coords.md）。
		// 底图固定 640x470（WorldMap.img BaseImg），顶/底/侧带高宽从各 Border 件 WxH 推出。
		//
		// round96（P3 · corewindow/D10）083 对照：本端这套「由贴图件推导」与 083 的
		// 「外框硬编码」差 12x3，来源已坐实——
		//   083 `CWorldMapDlg::CreateWorldMapDlg @0x9eb366:139`
		//        `CDialog::CreateDlg(this, a2: 67, a3: 0, a4: 666, a5: 524, a6: 10, a7: 1, a8: 0)`
		//        → 外层 666x524；
		//   083 `CWorldMapDlg::OnCreate @0x9eb75b`
		//        `sub_9ECC4A(a1: 12, a2: 34, a3: 642, a4: 472)` → 内容区 (12,34) 642x472；
		//   本端 Backgrnd 640x470 + top 33 / bot 18 / side 7 → 654x521，内容 (7,33)。
		// 083 的 666 = 12 + 642 + 12、524 = 34 + 472 + 18，即两侧各留 12px 内边距；
		// 而 Border/3..4 实测仅 7px 宽，照搬硬编码会让贴图与内容之间留 5px 空隙（视觉破坏）。
		// 故本轮**不改几何**（`DECISIONS.md:101` 的 s9 布局优先口径不变），只把差值来源钉在这里；
		// 真要改须连同九切片重排一起做，真机对齐项见 PLT §55-3。"""
patch_lf("IO/UITypes/UIWorldMap.cpp", WM_OLD, WM_NEW, "d10-comment")

# ================================================================ 8) evidence
EV_D = {
    "round": 96,
    "date": "2026-11-11",
    "trigger": "用户 m12009：P3（16）按 Angel 全部重做 —— 本轮执行 A 批前 3 条",
    "batch": "A",
    "items": {
        "login-flow/D12": {
            "verdict": "DOC_ONLY_DONE",
            "artifact": "docs/features/angel-calibration-d-matrix-2026-11/BYTE-LEVEL-STATS.md",
            "fields": 26,
            "s9_vs_client": "width-identical for all 26 (Evan SP via has_sp_table)",
            "vs_083_divergences": [
                "level 1B -> 2B (s9 widened)",
                "exp 8B -> 4B + tail 4B (s9 widened)",
                "gachaexp 4B added (s9 extension)",
            ],
            "code_change": "none",
        },
        "skill/D11": {
            "verdict": "NOT_083_EXTENSION",
            "skill_children_083": ["backgrnd", "skill0", "skill1", "line", "BtSpUp", "Tab",
                                   "tip0", "tip1", "tip2", "CoolTime", "recommend"],
            "skill_main_exists": False,
            "cuiskill_oncreate_buttons": "id 2010..2013 (x=119/159/199/239, y=131, step 40) + id 2023 (x=120, y=265) + tabs 2000/2001",
            "symbol_hits": {"*Hyper*": 0, "*GuildSkill*": 0},
            "action": "keep as extension placeholder + DISABLED; comments only",
            "code_change": "comments only",
        },
        "corewindow/D10": {
            "verdict": "FRAME_DERIVED_VS_HARDCODED",
            "ida_083": {
                "CreateWorldMapDlg": "0x9eb366:139 CDialog::CreateDlg(this, 67, 0, 666, 524, 10, 1, 0)",
                "OnCreate": "0x9eb75b sub_9ECC4A(12, 34, 642, 472)",
                "frame": [666, 524],
                "content": {"x": 12, "y": 34, "w": 642, "h": 472},
            },
            "client": {
                "content": {"x": 7, "y": 33, "w": 640, "h": 470},
                "frame": [654, 521],
            },
            "delta": [12, 3],
            "border_pieces": {"0": [60, 33], "1": [1, 33], "2": [7, 32], "3": [7, 1],
                             "4": [7, 1], "5": [7, 18], "6": [1, 18], "7": [7, 18]},
            "action": "no geometry change (would leave 5px gap); comment + PLT 55-3",
            "code_change": "comments only",
        },
    },
    "zero_wire": True,
    "zero_behavior_change": True,
    "arithmetic_unchanged": {
        "taxonomy": "168 = PURE 76 + WIRE 38 + MIXED 14 + OTHER 40",
        "reopen": "27 = 已注销 10 + 残余 17",
        "residual": "17 = P1 0 + P2 0 + P3 16",
        "p3_batches": {"A": 9, "B": 5, "C": 2},
        "p3_done_after_round96": 3,
        "p3_remaining": 13,
    },
    "orange_wz_gate": {
        "endpoint": "http://127.0.0.1:10012/mcp",
        "key_spec": "128B WzAESConstant.DEFAULT_KEY",
        "key_b64_len": 172,
        "img": "E:/MXD/BeiDou-Client-xy/Data/UI/UIWindow.img",
        "verdict": "GATE_OK_SKILL_AND_WORLDMAP_NODES_ENUMERATED",
    },
    "constraints": [
        "083/v83 only 禁 095",
        "s9 协议优先（W 桶 6 字段不动）",
        "数值重合 ≠ 语义重合",
        "门禁 5 禁想象修改",
    ],
}
wr(EV, (json.dumps(EV_D, ensure_ascii=False, indent=2) + "\n").encode("utf-8"))
print("  [evidence] written")

# ================================================================ 9) notes
NOTES_MD = """# round96 · P3 A 批前 3 条（D12 / D11 / D10）

## 1. 为什么这 3 条先做

`P3-ANGEL-REBUILD-PLAN.md` 把 P3 16 条按取证成熟度分 A/B/C。A 批 9 条中，
`login-flow/D12`、`skill/D11`、`corewindow/D10` 的 083 证据已在库或单点可补，
故作为第一批。三条**都不是**「加功能」，而是「把 083 的真实形态钉清楚、把本端口径纠正」。

## 2. `login-flow/D12` —— 逐字节文档（DOC_ONLY）

26 个字段逐一对照 s9 写方 / 当前端读方 / 083 `Decode`。结论：
- 26 个字段宽度全部一致（Evan SP 段由 `has_sp_table` 分流，round95 已补）；
- 与 083 的差异只有 3 处，全是 s9 有意改造：`level 1B→2B`、`exp 8B→4B + tail4`、`gachaexp`。
零代码改动。

## 3. `skill/D11` —— 三钮不是 083 的东西

orange-wz 128B 实测 `UIWindow.img/Skill` 的直接子节点：
`backgrnd / skill0 / skill1 / line / BtSpUp / Tab / tip0..tip2 / CoolTime / recommend`
—— **没有 `main`**。所以 `main["BtHyper"] / main["BtGuildSkill"] / main["BtRide"]` 三条在 083 上
恒为空，`classic = !usable(Skill["main"])` 也恒为 true。

083 `CUISkill::OnCreate @0x8aaa88` 里真正创建的按钮是 `id 2010..2013`
（x=119/159/199/239，y=131，步进 40，`Unknown` 从 119 起、`< 279` 止 → 4 个）、
`id 2023`（x=120, y=265）、页签 `2000/2001`。全库 `*Hyper*` / `*GuildSkill*` 符号命中 0。

→ 三钮属非 083 扩展。按 m09729「扩展功能可以不对齐」保留占位 + DISABLED，
不删枚举（`send_key` switch 与 `buttons` 索引耦合），不发明 083 行为。

## 4. `corewindow/D10` —— 083 是硬编码 666×524

- `CreateWorldMapDlg @0x9eb366:139`：`CDialog::CreateDlg(this, 67, 0, 666, 524, 10, 1, 0)`
- `OnCreate @0x9eb75b`：`sub_9ECC4A(12, 34, 642, 472)` → 内容 `(12,34) 642×472`
- 即 `666 = 12 + 642 + 12`、`524 = 34 + 472 + 18`，两侧各留 12px 内边距。

本端由 `Border` 贴图件推导：`640×470` + `top33/bot18/side7` → `654×521`，内容 `(7,33)`。
`Border` 件实测两端一致（`0=60×33 / 1=1×33 / 2=7×32 / 3,4=7×1 / 5,7=7×18 / 6=1×18`）。

差 12×3。若照搬 666×524 而边宽仍 7px，贴图与内容之间会留 5px 空隙（视觉破坏）。
→ 不改几何，只把差值来源钉在代码注释里；真机对齐项记 PLT §55-3。

## 5. 验证与算术

- 源码侧只有 `UISkillBook.cpp` / `UIWorldMap.cpp` 两处注释，**零行为改动**。
- 算术不变：`168 = 76+38+14+40`；`27 = 已注销10 + 残余17`；`17 = P1 0 + P2 0 + P3 16`；
  P3 完成 3 / 剩 13（A 余 6 + B 5 + C 2）。
"""

wr(NOTES, NOTES_MD.encode("utf-8"))
print("  [notes] written")

# ================================================================ 10) 报告
print("\n== SHA ==")
for p in [SKILL_CPP, "IO/UITypes/UIWorldMap.cpp", DECISIONS, WLE, PLAN_DOC, BYTE_DOC, DEV, PLT, EV, NOTES]:
    b = rd(p)
    eol, crlf, lone, crcrlf = eol_of(b)
    print("  %s  %s  %s crlf%d lone%d crcrlf%d bytes%d" % (sha12(b), p, eol, crlf, lone, crcrlf, len(b)))

print("\nROUND96_DELIVER_ALL_OK")
