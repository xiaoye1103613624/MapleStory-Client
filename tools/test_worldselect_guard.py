"""UIWorldSelect::send_key 边界守卫自测（构建级之外的算法闭环）。

背景：docs/features/feature-gap-inventory-2026-11/README.md §16 的 #2/#9 是
2026-11-04 旧账，live 源码已闭环；round34 逐字复核发现 send_key 里三处真实 UB：
  A. world_selected && worldid >= worlds.size()  → worlds[worldid] 越界读
     （clear_worlds() 清空 worlds 但不复位 world_selected，重连瞬间按键即触发）
  B. channel_total == 0 → columns=0 → std::div(x, 0) 除零 (SIGFPE)
  C. channel_total>0 但 channelid >= channel_total（重连后频道变少）→ 导航落到非法按钮段
  D. SoftKey「Channel #」延迟回调里 worlds[worldid] 无守卫（回调时可能已 clear_worlds）

本脚本做两件事，均为可自测部分：
  1) 静态核验：逐字确认 4 处守卫在 UIWorldSelect.cpp 中存在（防止后续被误删）。
  2) 算法复刻：把修复后的导航前置逻辑用 Python 等价复刻，喂边界输入，
     断言不抛 ZeroDivisionError / IndexError、且返回的是「安全退出」而非非法通道 id。

真机键盘导航（登录服在跑、冷启动、上下左右/TAB/ESC）需联机，进
docs/features/pending-live-tests/README.md 人工 backlog，不在此冒充已验证。

用法：python tools/test_worldselect_guard.py
退出码 0 = PASS，1 = FAIL。
"""
from __future__ import annotations

import io
import sys
from pathlib import Path

SRC = Path(__file__).resolve().parent.parent / "IO" / "UITypes" / "UIWorldSelect.cpp"
COLUMNS = 4  # UIWorldSelect.h:81 static constexpr uint8_t COLUMNS = 4U


# ---- 1) 静态核验：4 处守卫逐字在盘 -----------------------------------------
GUARDS = [
    ("A worldid 主路径守卫", "if (worldid >= worlds.size())\n\t\t\t\t\treturn;"),
    ("B 0 频道除零守卫", "if (channel_total == 0)"),
    ("C channelid 收敛守卫", "if (selected_channel >= channel_total)\n\t\t\t\t\tselected_channel = 0;"),
    ("D SoftKey 回调守卫", "if (worldid >= worlds.size()) return;"),
]


def check_guards(text: str) -> list[tuple[str, bool]]:
    return [(name, needle in text) for name, needle in GUARDS]


# ---- 2) 算法复刻：修复后 send_key 前置分支的等价模型 -------------------------
def navigate(worlds_size: int, worldid: int, world_selected: bool,
             channel_total: int, channelid: int, escape: bool):
    """返回 (outcome, columns, selected_channel)；outcome ∈ {'no_world','blocked','escaped','ok'}。

    复刻 UIWorldSelect.cpp:757-793 的控制流（守卫后），不含 button_pressed 副作用。
    """
    if not world_selected:
        return "no_world", None, None

    # guard A
    if worldid >= worlds_size:
        return "blocked", None, None

    selected_channel = 0 if channelid == 0xFF else channelid

    # guard B：0 频道世界，仅 ESC 逃生
    if channel_total == 0:
        return ("escaped", None, None) if escape else ("blocked", None, None)

    # guard C：重连后频道数变小
    if selected_channel >= channel_total:
        selected_channel = 0

    columns = min(channel_total, COLUMNS)
    assert columns > 0, "columns 必须 >0，否则 std::div 除零"  # noqa: S101
    rows = (channel_total - 1) // COLUMNS + 1
    div_quot, div_rem = divmod(selected_channel, columns)
    assert 0 <= div_quot < rows, "quot 越出 rows"  # noqa: S101
    assert 0 <= div_rem < columns, "rem 越出 columns"  # noqa: S101
    return "ok", columns, selected_channel


CASES = [
    # (label, worlds_size, worldid, world_selected, channel_total, channelid, escape, expect)
    ("未选世界", 3, 0, False, 5, 0, False, "no_world"),
    ("A 重连后 worlds 清空", 0, 0, True, 5, 0, False, "blocked"),
    ("A worldid 越界", 2, 7, True, 5, 0, False, "blocked"),
    ("B 0 频道 + 方向键", 1, 0, True, 0, 0, False, "blocked"),
    ("B 0 频道 + ESC 逃生", 1, 0, True, 0, 0, True, "escaped"),
    ("C channelid 旧值越界", 1, 0, True, 3, 9, False, "ok"),
    ("C channelid=0xFF 哨兵", 1, 0, True, 3, 0xFF, False, "ok"),
    ("正常单频道", 1, 0, True, 1, 0, False, "ok"),
    ("正常 30 频道末位", 1, 0, True, 30, 29, False, "ok"),
    ("正常 7 频道中位", 1, 0, True, 7, 5, False, "ok"),
]


def main() -> int:
    buf = io.StringIO()
    ok = True

    def p(s=""):
        print(s, file=buf)

    text = SRC.read_text(encoding="utf-8")
    p("=== 1) 静态守卫核验 ===")
    for name, found in check_guards(text):
        p(f"[{'OK' if found else 'FAIL'}] {name}")
        ok &= found

    p()
    p("=== 2) 边界算法复刻（断言无除零/越界） ===")
    for label, ws, wid, sel, ct, cid, esc, expect in CASES:
        try:
            outcome, cols, sch = navigate(ws, wid, sel, ct, cid, esc)
            good = outcome == expect
            p(f"[{'OK' if good else 'FAIL'}] {label}: "
              f"outcome={outcome} columns={cols} selected_channel={sch} (expect={expect})")
            ok &= good
        except (ZeroDivisionError, AssertionError, IndexError) as e:
            p(f"[FAIL] {label}: 抛出 {type(e).__name__}: {e}")
            ok = False

    p()
    p(("PASS: 4 guards + %d boundary cases" % len(CASES)) if ok else "FAIL")
    out = buf.getvalue()
    sys.stdout.write(out)
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
