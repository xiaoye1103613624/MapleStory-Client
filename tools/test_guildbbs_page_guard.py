# -*- coding: utf-8 -*-
"""round39 · UIGuildBBS BtPageRight 上界守卫离线自测.

静态核验 5 段守卫逐字在盘 + 动态复刻 page_left/page_right 翻页判定
（修复前无条件右翻，修复后仅当本页满 LIST_MAX_ROWS 才右翻）。
不编译 C++、不连 IDA/服务端，纯算法边界对照。
"""
import io
import sys

CPP = r"E:/project/MapleStory-Client/IO/UITypes/UIGuildBBS.cpp"
HDR = r"E:/project/MapleStory-Client/IO/UITypes/UIGuildBBS.h"
LIST_MAX_ROWS = 12

cpp = io.open(CPP, "r", encoding="utf-8", newline="").read().replace("\r\n", "\n")
hdr = io.open(HDR, "r", encoding="utf-8", newline="").read().replace("\r\n", "\n")

GUARDS = [
    ("right_guard", cpp, "if (static_cast<int>(threads.size()) >= LIST_MAX_ROWS)"),
    ("right_guard_comment", cpp, "与 BtPageLeft 的 page_index > 0 下界对称"),
    ("left_lower_bound_kept", cpp, "if (page_index > 0)"),
    ("list_max_rows_const", hdr, "static constexpr int16_t LIST_MAX_ROWS = 12;"),
    ("request_list_lower_bound_kept", cpp, "if (page_i < 0)"),
]

fail = []
for name, text, needle in GUARDS:
    ok = needle in text
    print("[%s] %-32s %s" % ("OK" if ok else "FAIL", name,
                             "" if ok else "MISSING: " + needle))
    if not ok:
        fail.append(name)


def page_left(page_index, guarded=True):
    """BtPageLeft 未改：page_index>0 才左翻。guarded 参数仅作对照占位。"""
    if page_index > 0:
        return request(page_index - 1)
    return None


def page_right(page_index, n_rows, guarded):
    """复刻修复前/后 BtPageRight。

    guarded=False（修复前）: 无条件 request(page_index+1)。
    guarded=True （修复后）: 本页满 LIST_MAX_ROWS 才 request(page_index+1)。
    """
    if not guarded or n_rows >= LIST_MAX_ROWS:
        return request(page_index + 1)
    return None


def request(p):
    return "request(%d)" % p


def is_bad(n_rows):
    """本页未满却仍发下一页请求 = 必然返回空列表的越界页请求。"""
    return n_rows < LIST_MAX_ROWS


# (id, page_index, n_rows, 归类) —— A..D/G 末页(越界) / E,F 满页(合法回归)
RIGHT_CASES = [
    ("A_empty_page0", 0, 0, "bad"),
    ("B_last_3rows", 0, 3, "bad"),
    ("C_last_11rows", 1, 11, "bad"),
    ("D_last_8rows_p2", 2, 8, "bad"),
    ("E_full_page0", 0, 12, "reg"),
    ("F_full_page2", 2, 12, "reg"),
    ("G_last_3rows_p5", 5, 3, "bad"),
]

before_bad = after_bad = reg_ok = 0
for cid, pidx, n_rows, kind in RIGHT_CASES:
    before = page_right(pidx, n_rows, guarded=False)
    after = page_right(pidx, n_rows, guarded=True)
    if kind == "bad":
        assert is_bad(n_rows)
        # 修复前必发越界请求，修复后必须拦住
        ok = before is not None and after is None
        before_bad += 1 if before is not None else 0
        after_bad += 1 if after is not None else 0
    else:
        # 合法满页翻页：修复前后行为必须完全一致
        ok = before == after
        reg_ok += 1 if ok else 0
    print("[%s] %-20s p=%d rows=%-2d before=%-12s after=%-12s" % (
        "OK" if ok else "FAIL", cid, pidx, n_rows,
        str(before), str(after)))
    if not ok:
        fail.append(cid)

# BtPageLeft 对照（未改，回归）
LEFT_CASES = [("H_left_at_0", 0, None), ("I_left_at_2", 2, "request(1)")]
for cid, pidx, expect in LEFT_CASES:
    got = page_left(pidx)
    ok = got == expect
    print("[%s] %-20s p=%d got=%-12s expect=%s" % (
        "OK" if ok else "FAIL", cid, pidx, str(got), str(expect)))
    reg_ok += 1 if ok else 0
    if not ok:
        fail.append(cid)

# J: 键盘旁路 '.'/']' 与鼠标都走 button_pressed(BtPageRight) —— 静态确认
kbd_ok = ("button_pressed(BtPageRight)" in cpp and "keycode == '.'" in cpp)
print("[%s] %-20s keyboard '.'/']' -> button_pressed(BtPageRight)" % (
    "OK" if kbd_ok else "FAIL", "J_keyboard_route"))
if not kbd_ok:
    fail.append("J_keyboard_route")

print("[summary] before_bad=%d after_bad=%d reg_ok=%d/4" % (
    before_bad, after_bad, reg_ok))
if fail:
    print("FAIL:", fail)
    sys.exit(1)
if not (before_bad > 0 and after_bad == 0 and reg_ok == 4):
    print("FAIL: invariant (before>0, after=0, regression 4/4) not met")
    sys.exit(1)
print("PASS: 5 static guards + 10 cases (before bad=%d -> after %d; regression OK)"
      % (before_bad, after_bad))
sys.exit(0)
