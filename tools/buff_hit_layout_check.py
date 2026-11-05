# -*- coding: utf-8 -*-
"""buff_hit_layout_check.py — UIBuffList 右键几何对拍（镜像 draw/is_in_range/rightclick）
draw(): 第 i 格锚点 icpos_i = (X - 32i, Y)，图标 origin(0,32) 底边对齐 →
        可见框 [X-32i, X-32i+32) × [Y-32, Y]
is_in_range: w = cx-X，要求 -32(n-1) <= w < 32 且 -32 <= cy-Y <= 0
rightclick: slot = (31 - w) // 32，索引 draw_order[slot]
断言：每格中心与左右边界命中槽位 == 该格索引；缝外拒绝。"""
import sys

def hit(n, w, dy):
    if n == 0:
        return None
    if not (-32 * (n - 1) <= w < 32 and -32 <= dy <= 0):
        return None
    return (31 - w) // 32

fails = 0
for n in (1, 2, 5, 15):
    for i in range(n):
        # 第 i 格可见框 x ∈ [-32i, -32i+32)
        lo, hi = -32 * i, -32 * i + 32
        cases = {lo: i, lo + 1: i, (lo + hi) // 2: i, hi - 1: i}
        for w, want in cases.items():
            got = hit(n, w, -16)
            if got != want:
                print(f"FAIL n={n} slot={i} w={w} -> {got} want {want}")
                fails += 1
    # 条带外（更左一格、右缘外）拒绝
    assert hit(n, -32 * (n - 1) - 1, -16) is None, f"n={n} left overflow accepted"
    assert hit(n, 32, -16) is None, f"n={n} right overflow accepted"
    assert hit(n, 0, -33) is None, f"n={n} above accepted"

# 与 draw_order 长度一致性：slot 上界 = n-1
for n in (1, 15):
    top = hit(n, -32 * (n - 1), -16)
    if top != n - 1:
        print(f"FAIL n={n} leftmost slot -> {top}")
        fails += 1

print("OK" if fails == 0 else f"FAILS={fails}")
sys.exit(1 if fails else 0)
