# -*- coding: utf-8 -*-
import json, ast, shutil
from pathlib import Path

base = Path(__file__).resolve().parent
ev = base / "evidence"
notes = base / "notes"
ev.mkdir(parents=True, exist_ok=True)
notes.mkdir(parents=True, exist_ok=True)

raw = json.loads((ev / "live-pyeval-inventory.json").read_text(encoding="utf-8"))


def parse_result(tag):
    r = raw[tag]["result"]
    if isinstance(r, str):
        return ast.literal_eval(r)
    return r


r37 = parse_result("p13337")
r39 = parse_result("p13339")

clean = {
    "p13337": r37,
    "p13339": r39,
    "collected_at": "2026-11-12",
    "note": "13337 is NOT BeiDou; live idb_path is v83/083/v83.i64",
}
(ev / "live-pyeval-inventory-parsed.json").write_text(
    json.dumps(clean, ensure_ascii=False, indent=2), encoding="utf-8"
)

identity = {
    "warning": "Do NOT equate port 13337 with historical BeiDou.exe.i64. Port reuse only.",
    "p13337": {
        "port": 13337,
        "idb_path": r37["idb_path"],
        "module": r37["root_filename"],
        "input_path": r37["input_path"],
        "input_md5": r37["input_md5"],
        "input_sha256": r37["input_sha256"],
        "input_file_size": r37["input_file_size"],
        "imagebase": r37["imagebase"],
        "role": "current instance under analysis (v83.i64 under 083/)",
    },
    "p13339": {
        "port": 13339,
        "idb_path": r39["idb_path"],
        "module": r39["root_filename"],
        "input_path": r39["input_path"],
        "input_md5": r39["input_md5"],
        "input_sha256": r39["input_sha256"],
        "input_file_size": r39["input_file_size"],
        "imagebase": r39["imagebase"],
        "role": "Angel IDA instance (`Angel.i64) for side-by-side compare",
    },
    "verdict": {
        "same_input_exe": r37["input_md5"] == r39["input_md5"],
        "same_input_path": r37["input_path"] == r39["input_path"],
        "different_idb_files": r37["idb_path"] != r39["idb_path"],
        "image_size_survey": "0xa95000 both (from survey_binary)",
        "func_delta": r39["func_total"] - r37["func_total"],
        "conclusion": (
            "Same Angel.exe (identical MD5/SHA256/size). Two IDA databases of the "
            "same binary with near-identical symbol state; NOT BeiDou vs Angel."
        ),
    },
}
(ev / "identity-live-health.json").write_text(
    json.dumps(identity, ensure_ascii=False, indent=2), encoding="utf-8"
)

compare = {"metric": {}}
for k in [
    "func_total",
    "func_real_named",
    "func_mangled_msvc",
    "class_count",
    "class_methods_sum",
    "cui_class_count",
    "input_md5",
    "input_file_size",
    "strings_iter_count",
]:
    compare["metric"][k] = {
        "p13337": r37.get(k),
        "p13339": r39.get(k),
        "equal": r37.get(k) == r39.get(k),
    }
compare["segments_equal"] = r37["segments"] == r39["segments"]
compare["feature_index_equal"] = r37["feature_index"] == r39["feature_index"]
compare["top_classes_equal"] = r37["top_classes"] == r39["top_classes"]

disk_path = Path(
    r"E:\project\MapleStory-Client\docs\features\ida-v83-13337-instance\evidence\live-health-disk.json"
)
if disk_path.exists():
    disk = json.loads(disk_path.read_text(encoding="utf-8"))
    (ev / "live-health-disk.json").write_text(
        json.dumps(disk, ensure_ascii=False, indent=2), encoding="utf-8"
    )
    compare["disk"] = {
        "p13337_i64": disk["p13337"]["disk"].get("exact"),
        "p13339_i64": disk["p13339"]["disk"].get("exact"),
        "p13337_id0": disk["p13337"]["disk"].get(".id0"),
        "p13339_id0": disk["p13339"]["disk"].get(".id0"),
    }

(ev / "angel-vs-v83i64-compare.json").write_text(
    json.dumps(compare, ensure_ascii=False, indent=2), encoding="utf-8"
)

survey_src = Path(
    r"E:\project\MapleStory-Client\docs\features\ida-v83-13337-instance\evidence\live-survey-minimal.json"
)
if survey_src.exists():
    shutil.copy(survey_src, ev / "live-survey-minimal.json")

FEATURE_BLURB = {
    "login": "登录/选角/频道：CLogin 等 10 类 / 98 方法",
    "statusbar_hud": "状态栏/HUD：CUIStatusBar 等 7 类 / 108 方法",
    "corewindow": "窗口控件基座：CWnd/CCtrl*/CWndMan 等 22 类 / 181 方法",
    "social": "社交：好友/家族/Messenger/UserList 等 8 类 / 77 方法",
    "pet": "宠物：CPet/CUIPetEquip 等 3 类 / 33 方法",
    "activity": "活动/临时状态：EnergyBar/EventAlarm/TemporaryStat 3 类 / 12 方法",
    "cashshop": "商城：CCashShop 为主 2 类 / 70 方法",
    "skill": "技能 UI：CUISkill* / CSkillInfo 4 类 / 14 方法",
    "npc": "NPC：CNpc/CNpcPool 等 4 类 / 54 方法",
    "map": "地图/副本 Field：CField* 等 32 类 / 207 方法",
    "drop": "掉落：CDropPool 1 类 / 7 方法",
    "quest": "任务：CQuestMan/CUIQuest* 6 类 / 50 方法",
    "packet": "封包：CIn/OutPacket + CClientSocket 3 类 / 35 方法",
    "crypto": "加密：CAESCipher/CIGCipher/CSecurityClient 3 类 / 18 方法",
    "character": "角色：CUserLocal/CUser/CAvatar 等 8 类 / 285 方法",
    "inventory": "背包装备：CItemInfo/CUIItem/CUIEquip 等 6 类 / 92 方法",
    "monsterbook": "怪物书：CUIMonsterBook/CMonsterBookMan 2 类 / 11 方法",
    "medal": "勋章称号任务：CUIMedalQuestInfo* 2 类 / 13 方法",
}

FEATURE_KEYS = {
    "login": ["Login", "CharSelect", "Channel", "World", "Pin"],
    "statusbar_hud": ["StatusBar", "MiniMap", "ToolTip", "KeyConfig"],
    "corewindow": ["CWnd", "Ctrl"],
    "social": ["Guild", "Party", "Friend", "Messenger", "Family"],
    "pet": ["Pet"],
    "activity": ["Event", "Energy", "Temporary"],
    "cashshop": ["CashShop", "Cash"],
    "skill": ["Skill"],
    "npc": ["Npc"],
    "map": ["Field", "WorldMap", "Map"],
    "drop": ["Drop", "Pick"],
    "quest": ["Quest"],
    "packet": ["Packet", "Socket"],
    "crypto": ["Cipher", "AES", "Security"],
    "character": ["User", "Avatar"],
    "inventory": ["Item", "Equip", "Inventory"],
    "monsterbook": ["MonsterBook"],
    "medal": ["Medal", "Title"],
}

index_rows = []
for i, (feat, blurb) in enumerate(FEATURE_BLURB.items(), 1):
    fi37 = r37["feature_index"][feat]
    fi39 = r39["feature_index"][feat]
    keys = FEATURE_KEYS.get(feat, [])
    related = {}
    for k, v in (r37.get("string_anchors") or {}).items():
        text = str(v.get("text", ""))
        if any(x.lower() in k.lower() or x.lower() in text.lower() for x in keys):
            related[k] = v

    ev_feat = {
        "feature": feat,
        "p13337": fi37,
        "p13339": fi39,
        "equal_to_angel_idb": fi37 == fi39,
        "string_anchors_related": related,
        "source_idb": r37["idb_path"],
        "compare_idb": r39["idb_path"],
        "input_md5": r37["input_md5"],
    }
    (ev / f"feature-{feat}.json").write_text(
        json.dumps(ev_feat, ensure_ascii=False, indent=2), encoding="utf-8"
    )

    classes_tbl = "\n".join(
        "| `{}` | {} |".format(c["class"], c["methods"]) for c in fi37["classes"]
    )
    eq = fi37 == fi39
    note = f"""# {feat} — 功能点事实（13337 / v83.i64）

> 采集：2026-11-12 live IDA MCP  
> 当前实例：`port=13337` → `{r37['idb_path']}`  
> 对照 Angel：`port=13339` → `{r39['idb_path']}`  
> **禁止**把 13337 当成历史 BeiDou IDB（端口复用）。

## 一句话结论

{blurb}。与 Angel IDB（13339）该类索引 **{'完全一致' if eq else '存在差异'}**。

## 身份

| 项 | 值 |
|---|---|
| 当前 IDB | `{r37['idb_path']}` |
| module / input | `{r37['root_filename']}` / `{r37['input_path']}` |
| input_md5 | `{r37['input_md5']}` |
| 对照 Angel IDB | `{r39['idb_path']}`（同 MD5） |

## 类与方法统计

| 指标 | 13337 (v83.i64) | 13339 (Angel.i64) |
|---|---:|---:|
| class_count | {fi37['class_count']} | {fi39['class_count']} |
| method_count | {fi37['method_count']} | {fi39['method_count']} |

### 命中类列表（13337）

| 类 | 具名方法数 |
|---|---:|
{classes_tbl}

## 字符串锚点（抽样）

```json
{json.dumps(related, ensure_ascii=False, indent=2)}
```

## 与 Angel 差异

- 本功能点 feature_index：**{'equal' if eq else 'DIFF'}**
- 两端输入 exe MD5 相同 → 业务语义以任一侧具名符号为准均可；本轮两侧符号量同级（3122 mangled / 399 classes）。

## Evidence

- `evidence/feature-{feat}.json`
- `evidence/live-pyeval-inventory-parsed.json`
- `evidence/identity-live-health.json`
"""
    note_path = notes / f"r{i:02d}-{feat}-facts.md"
    note_path.write_text(note, encoding="utf-8")
    index_rows.append(
        (
            feat,
            blurb,
            f"notes/r{i:02d}-{feat}-facts.md",
            f"evidence/feature-{feat}.json",
            fi37["class_count"],
            fi37["method_count"],
            eq,
        )
    )
    print("wrote", feat)

(notes / "r00-identity-and-size.md").write_text(
    f"""# 身份澄清与「大小不一样」解释

> 2026-11-12 live 取证。**13337 ≠ BeiDou**（用户已关闭 BeiDou；端口被新 IDB 占用）。

## 当前两端是谁

| 端口 | idb_path | module | input_md5 | 角色 |
|---|---|---|---|---|
| **13337** | `{r37['idb_path']}` | `{r37['root_filename']}` | `{r37['input_md5']}` | 本轮分析对象（`083/v83.i64`） |
| **13339** | `{r39['idb_path']}` | `{r39['root_filename']}` | `{r39['input_md5']}` | Angel IDA（`` `Angel.i64 ``） |

- 两端 **input_path 相同**：`{r37['input_path']}`
- 两端 **SHA256 相同**：`{r37['input_sha256']}`
- 两端 **input_file_size 相同**：{r37['input_file_size']} bytes（≈9.46 MB）
- `survey_binary.image_size`：**两侧都是 `0xa95000`**（不是历史文档里 BeiDou 的 `0xa94000`）

## 与历史文档的关系

| 旧口径 | 现状 |
|---|---|
| 13337 = BeiDou.exe.i64 | **作废**。BeiDou 已关；13337 现为 `v83/083/v83.i64` |
| 13339 = Angel.i64 | **仍成立**（live 确认） |
| Angel vs BeiDou 差 0x1000 image_size | **不适用于本轮**。本轮两侧 image_size 相同 |

## 「大小不一样」指什么

1. **不是 exe 大小不同**：input MD5/SHA256/size 全等。
2. **不是 PE image_size 不同**：两侧 `0xa95000`。
3. **是 IDB 数据库文件大小不同**（分析库落盘）：
   - `v83.i64` ≈ 28,053,268 B（2026-11-12 新建）
   - `` `Angel.i64 `` ≈ 27,998,358 B（2026-11-08）
   - 工作集 `.id0`：v83 ≈ 109 MB vs Angel ≈ 126 MB（打开时长/分析缓存不同）
4. **函数数几乎相同**：55015 vs 55016（差 1，IDA 边界分析噪声级）
5. **符号状态同级**：MSVC 修饰名 3122、类 399、类方法合计 2984、CUI* 类 77 — 两侧一致

## 结论（校准该用哪边）

- **同一份 Angel.exe 的两份 IDB**，不是「两个不同客户端」。
- 业务校准任选；习惯上 13339 `` `Angel.i64 `` 文档链更长，13337 `v83.i64` 是同符号新鲜实例。
- **不要**再拿历史 BeiDou stripped 结论套到当前 13337。
""",
    encoding="utf-8",
)

lines = []
lines.append("# IDA 盘点：13337 → `v83/083/v83.i64`（Angel.exe 同源）")
lines.append("")
lines.append("> 采集日期：2026-11-12  ")
lines.append("> 方法：live `server_health` / `survey_binary` / `py_eval`（HTTP 直连 13337 & 13339）  ")
lines.append("> **身份铁律**：端口会复用；以 `idb_path` 为准。当前 **13337 不是 BeiDou**。")
lines.append("")
lines.append("## 0. 一句话结论")
lines.append("")
lines.append(
    "- **13337** 当前挂的是 `E:\\\\MXD\\\\反编译IDA\\\\v83\\\\083\\\\v83.i64`，"
    "module/input 仍是 **`Angel.exe`**（MD5 `cbe5ccba...`）。"
)
lines.append(
    "- **13339** 挂的是同目录上一级的 `` `Angel.i64 ``，**同一份 exe** 的另一份 IDB。"
)
lines.append(
    "- 「大小不一样」主要来自 **IDB/id0 数据库体积与打开时长**，不是不同版本二进制；"
    "`image_size` 两侧均为 `0xa95000`，函数数仅差 1。"
)
lines.append(
    "- 符号可读性：两侧同级（3122 MSVC 修饰名 / 399 类 / 2984 类方法），"
    "**不是**历史「Angel 有符号 vs BeiDou 全 sub_*」。"
)
lines.append("")
lines.append("## 1. 身份与大小（详见 notes）")
lines.append("")
lines.append("| 文档 | 结论 |")
lines.append("|---|---|")
lines.append(
    "| [notes/r00-identity-and-size.md](notes/r00-identity-and-size.md) | "
    "13337=`v83.i64`；与 Angel 同 MD5；大小差在 IDB 文件 |"
)
lines.append(
    "| [evidence/identity-live-health.json](evidence/identity-live-health.json) | live 身份 JSON |"
)
lines.append(
    "| [evidence/angel-vs-v83i64-compare.json](evidence/angel-vs-v83i64-compare.json) | 指标对照 |"
)
lines.append(
    "| [evidence/live-survey-minimal.json](evidence/live-survey-minimal.json) | survey_binary |"
)
lines.append(
    "| [evidence/live-health-disk.json](evidence/live-health-disk.json) | 磁盘 IDB 体积 |"
)
lines.append("")
lines.append("## 2. 全局规模")
lines.append("")
lines.append("| 指标 | 13337 v83.i64 | 13339 Angel.i64 |")
lines.append("|---|---:|---:|")
lines.append(
    "| func_total | {} | {} |".format(r37["func_total"], r39["func_total"])
)
lines.append(
    "| func_real_named | {} | {} |".format(
        r37["func_real_named"], r39["func_real_named"]
    )
)
lines.append(
    "| MSVC mangled | {} | {} |".format(
        r37["func_mangled_msvc"], r39["func_mangled_msvc"]
    )
)
lines.append(
    "| classes | {} | {} |".format(r37["class_count"], r39["class_count"])
)
lines.append(
    "| class methods | {} | {} |".format(
        r37["class_methods_sum"], r39["class_methods_sum"]
    )
)
lines.append(
    "| CUI* classes | {} | {} |".format(
        r37["cui_class_count"], r39["cui_class_count"]
    )
)
lines.append(
    "| strings | {} | {} |".format(
        r37["strings_iter_count"], r39["strings_iter_count"]
    )
)
lines.append("| image_size (survey) | 0xa95000 | 0xa95000 |")
lines.append(
    "| input_md5 | `{}` | `{}` |".format(r37["input_md5"], r39["input_md5"])
)
lines.append("")
lines.append("## 3. 功能点索引（每点独立文档）")
lines.append("")
lines.append(
    "| # | 功能点 | 一句话 | 类/方法 | vs Angel | notes | evidence |"
)
lines.append("|---:|---|---|---:|---|---|---|")
for i, (feat, blurb, np, ep, cc, mc, eq) in enumerate(index_rows, 1):
    lines.append(
        "| {} | `{}` | {} | {}/{} | {} | [{}]({}) | [{}]({}) |".format(
            i,
            feat,
            blurb,
            cc,
            mc,
            "一致" if eq else "差异",
            np,
            np,
            ep,
            ep,
        )
    )
lines.append("")
lines.append("## 4. 校准建议")
lines.append("")
lines.append(
    "1. 当前 13337 与 13339 **都是 Angel 符号 IDB**，可互换作 v83 原生逻辑源。"
)
lines.append(
    "2. 历史「BeiDou stripped 交叉验证」链路若仍需要，须重新打开 BeiDou IDB（会拿到新端口），"
    "**不能**假定还在 13337。"
)
lines.append(
    "3. Cursor 默认 `user-ida-pro-mcp` 可能挂在 13339；分析「用户说的 13337」须 HTTP 直连"
    "或先 `server_health` 读 `idb_path`。"
)
lines.append("")
lines.append("## 5. 段布局（两侧相同）")
lines.append("")
lines.append("```json")
lines.append(json.dumps(r37["segments"], ensure_ascii=False, indent=2))
lines.append("```")
lines.append("")
lines.append("含 `.mackt`（Themida/加壳相关残留段名特征），与历史 Angel 描述一致。")

(base / "README.md").write_text("\n".join(lines) + "\n", encoding="utf-8")
print("DONE", base)
print("features", len(index_rows))
