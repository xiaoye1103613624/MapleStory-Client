# -*- coding: utf-8 -*-
"""
round35 自测：UICharSelect::send_key 网格导航边界守卫。

静态核验 2 处守卫逐字在盘 + 算法复刻 10 个边界 case（修复前/修复后对照）。

背景（见 docs/features/development-and-fix-plan-2026-11/notes/wave1-charselect-recheck-20261109.md）：
UICharSelect.cpp:830-894 的网格导航分支与 round34 已修的 UIWorldSelect::send_key
是同一类边界病，但两条守卫的实证口径必须分开，不混着吹：

  守卫 A（characters_count<=0 早退）：
    空列表修复前 UP/DOWN 走 `next_index==selected_character` 的 return 分支，
    **不会** misfire；真实症状是 RIGHT 分支
    `selected_index != characters_count-1`（0 != -1 成立）→ button_pressed(BtPageR)
    对空列表做无意义翻页。故 A 的价值 = 消除空列表下任何 button_pressed。
    （自我批判：开工诊断曾写「空列表 UP 算出 6=BtResetPIC」，被 harness 实证
     证伪，确切症状如上，不硬撑。另：:842 `if(columns>0)` 是既有守卫，
     本轮 A 补的是 button_pressed 误触发，不是补除零。）

  守卫 B（selected_character>=characters_count 夹到末位）：
    remove_character():979-982 在越界时调 select_last_slot()，但该函数只在
    当前页找到 index<characters_count 才 button_pressed；若 selected_page 指向
    已空的页则一次都不按，selected_character 保持残留越界值。随后 UP 分支
    `button_pressed(next_index - COLUMNS + CHARACTER_SLOT0)` 减法落到
    CHARACTER_SLOT0 之前的按钮段，实测 8 = BtPreview（非开工诊断写的 6）。

验收口径（本轮范围，不放宽）：
  - 守卫 A：count<=0 -> no_grid（presses=0）；修复前空列表 presses>0
  - 守卫 B：count>0 且 sel 越界 -> 夹到 [0,count) 且 misfire=0；修复前 >0
  - 非空通用：columns>0、0<=div.rem<columns、misfire 计数 0

注意：page 相对索引语义（绝对 selected_index 当页内 slot 用）是既有另一处
既有设计，不在本轮守卫范围，已在 notes §6 记入真机 backlog，本 harness
不伪称已修。
"""
import io
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "IO", "UITypes", "UICharSelect.cpp")

# UICharSelect.h:88-100 枚举（0-based）
BT_SELECT, BT_NEW, BT_DELETE, BT_PAGEL, BT_PAGER = 0, 1, 2, 3, 4
BT_CHANGEPIC, BT_RESETPIC, BT_CHARACTER, BT_PREVIEW = 5, 6, 7, 8
CHARACTER_SLOT0 = 9
PAGESIZE = 3
COLUMNS = 3
# 网格导航不该产生的「CHARACTER_SLOT0 之前」按钮段（减法误落点）
MISFIRE_IDS = {BT_CHANGEPIC, BT_RESETPIC, BT_CHARACTER, BT_PREVIEW}

GUARDS = [
    ("A_empty_list_early_return",
     "if (characters_count <= 0)\n\t\t\t\t\t\treturn;"),
    ("B_selected_character_clamp",
     "if (selected_character >= static_cast<uint8_t>(characters_count))\n"
     "\t\t\t\t\t\tselected_character = static_cast<uint8_t>(characters_count - 1);"),
]


def cpp_div(a, b):
    """C/C++ std::div：商向零截断；此处 a>=0 故等价 divmod。"""
    return divmod(a, b)


def navigate(count, sel, page, guarded):
    """复刻 UICharSelect::send_key 网格前置分支 + 4 方向 buttonid 计算。

    guarded=True 应用本轮 2 处守卫；False 为修复前对照。
    buttonids 中 "return" 表示该方向未调用 button_pressed；
    presses = 非 "return" 的项数。
    """
    out = {"status": "", "columns": None, "rows": None, "sel": sel,
           "div_quot": None, "div_rem": None, "buttonids": [], "presses": 0}

    if guarded and count <= 0:
        out["status"] = "no_grid"
        return out
    if guarded and sel >= count:
        sel = count - 1
    out["sel"] = sel
    out["status"] = "grid"

    index_total = min(count, (page + 1) * PAGESIZE)
    columns = min(index_total, COLUMNS)
    # C++ uint8_t: (index_total-1) 提升为 int，/3 向零截断，+1
    rows = int((index_total - 1) / COLUMNS) + 1 if index_total > 0 else 1
    out["columns"], out["rows"] = columns, rows

    current_col = 0
    if columns > 0:  # :842 既有守卫
        q, r = cpp_div(sel, columns)
        out["div_quot"], out["div_rem"] = q, r
        current_col = r

    selected_index = sel

    # UP
    if selected_index - COLUMNS < 0:
        next_index = (selected_index - COLUMNS) + rows * COLUMNS
    else:
        next_index = selected_index - COLUMNS
    if next_index == sel:
        out["buttonids"].append(("UP", "return"))
    elif next_index >= index_total:
        out["buttonids"].append(("UP", next_index - COLUMNS + CHARACTER_SLOT0))
    else:
        out["buttonids"].append(("UP", next_index + CHARACTER_SLOT0))

    # DOWN
    if selected_index + COLUMNS >= index_total:
        next_index = current_col
    else:
        next_index = selected_index + COLUMNS
    if next_index == sel:
        out["buttonids"].append(("DOWN", "return"))
    elif next_index > index_total:
        out["buttonids"].append(("DOWN", next_index + COLUMNS + CHARACTER_SLOT0))
    else:
        out["buttonids"].append(("DOWN", next_index + CHARACTER_SLOT0))

    # LEFT
    if selected_index != 0:
        si = selected_index - 1
        if si >= (page + 1) * PAGESIZE - PAGESIZE:
            out["buttonids"].append(("LEFT", si + CHARACTER_SLOT0))
        else:
            out["buttonids"].append(("LEFT", BT_PAGEL))
    else:
        out["buttonids"].append(("LEFT", "return"))

    # RIGHT
    if selected_index != count - 1:
        si = selected_index + 1
        if si < index_total:
            out["buttonids"].append(("RIGHT", si + CHARACTER_SLOT0))
        else:
            out["buttonids"].append(("RIGHT", BT_PAGER))
    else:
        out["buttonids"].append(("RIGHT", "return"))

    out["presses"] = sum(1 for _, b in out["buttonids"] if b != "return")
    return out


def misfires(res):
    return [(d, b) for d, b in res["buttonids"]
            if isinstance(b, int) and b in MISFIRE_IDS]


# 10 个边界 case：(说明, count, sel, page, group)
#   A = 空列表（守卫 A 覆盖）；B = sel 越界（守卫 B 覆盖）；N = 常规回归
CASES = [
    ("新账号空列表 4 方向", 0, 0, 0, "A"),
    ("重连空列表残留页码", 0, 0, 2, "A"),
    ("删光后残留 sel=5 count=1", 1, 5, 0, "B"),
    ("删光后残留 sel=5 count=1 复验", 1, 5, 0, "B"),
    ("重连残留 sel=9 count=2", 2, 9, 0, "B"),
    ("单页 3 角色 sel=2 UP", 3, 2, 0, "N"),
    ("单页 3 角色 sel=0 DOWN", 3, 0, 0, "N"),
    ("多页 7 角色末位 sel=6", 7, 6, 2, "N"),
    ("4 角色 sel=3 page=1 RIGHT", 4, 3, 1, "N"),
    ("满槽 15 角色 sel=14 page=4", 15, 14, 4, "N"),
]


def main():
    ok = True
    text = io.open(SRC, encoding="utf-8", newline="").read()

    print("=== 1. 静态守卫核验 ===")
    for name, guard in GUARDS:
        hit = text.count(guard)
        print("[%s] %s (count=%d)" % ("OK" if hit == 1 else "FAIL", name, hit))
        if hit != 1:
            ok = False

    print("=== 2. 边界 case（修复前 vs 修复后对照）===")
    agg = {g: {"before_press": 0, "after_press": 0,
               "before_mis": 0, "after_mis": 0}
           for g in ("A", "B", "N")}
    for i, (desc, count, sel, page, group) in enumerate(CASES, 1):
        before = navigate(count, sel, page, guarded=False)
        after = navigate(count, sel, page, guarded=True)
        bf, af = misfires(before), misfires(after)

        agg[group]["before_press"] += before["presses"]
        agg[group]["after_press"] += after["presses"]
        agg[group]["before_mis"] += len(bf)
        agg[group]["after_mis"] += len(af)

        problems = []
        if after["status"] == "no_grid":
            if count > 0:
                problems.append("count>0 却 no_grid")
            if after["presses"] != 0:
                problems.append("no_grid 仍有 presses=%d" % after["presses"])
        else:
            if not (after["columns"] and after["columns"] > 0):
                problems.append("columns=%s" % after["columns"])
            if not (0 <= after["sel"] < count):
                problems.append("sel=%d 未夹到 [0,%d)" % (after["sel"], count))
            if after["div_rem"] is not None and not (0 <= after["div_rem"] < after["columns"]):
                problems.append("div_rem=%s 越界" % after["div_rem"])
            if af:
                problems.append("after_misfire=%s" % af)
        tag = "OK" if not problems else "FAIL"
        if problems:
            ok = False
        print("[%s] C%02d[%s] %-30s before(press=%d,mis=%s) -> after(%s,press=%d,mis=%s)"
              % (tag, i, group, desc, before["presses"], str(bf),
                 after["status"], after["presses"], str(af)))
        if problems:
            print("       -> %s" % "; ".join(problems))

    print("=== 3. 两条守卫分别闭环 ===")
    # 守卫 A：空列表修复前有 button_pressed 误翻页，修复后 0
    a = agg["A"]
    print("[守卫 A] 空列表 presses: 修复前=%d -> 修复后=%d"
          % (a["before_press"], a["after_press"]))
    if not (a["before_press"] > 0 and a["after_press"] == 0):
        print("[FAIL] 守卫 A 未消除空列表误翻页")
        ok = False
    else:
        print("[OK] 守卫 A 消除全部 %d 次空列表误翻页" % a["before_press"])

    # 守卫 B：越界 sel 修复前 misfire，修复后 0
    b = agg["B"]
    print("[守卫 B] 越界 sel misfire: 修复前=%d -> 修复后=%d"
          % (b["before_mis"], b["after_mis"]))
    if not (b["before_mis"] > 0 and b["after_mis"] == 0):
        print("[FAIL] 守卫 B 未消除越界误触发")
        ok = False
    else:
        print("[OK] 守卫 B 消除全部 %d 处越界误触发" % b["before_mis"])

    # 常规回归：修复前后 misfire 均为 0（守卫不改变正常导航）
    n = agg["N"]
    print("[回归] 常规 case misfire: 修复前=%d -> 修复后=%d"
          % (n["before_mis"], n["after_mis"]))
    if n["after_mis"] != 0:
        print("[FAIL] 常规导航被守卫破坏")
        ok = False

    print("PASS: 2 guards + 10 boundary cases (A:press %d->0, B:misfire %d->0)"
          % (a["before_press"], b["before_mis"]) if ok else "FAIL: see above")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
