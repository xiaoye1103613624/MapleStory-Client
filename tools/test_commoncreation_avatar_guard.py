#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""round42: UICommonCreation::init_modern avatarSel loop guard.

Evidence:
  IO/UITypes/UICommonCreation.cpp:163  nl::node avatarSel = CustomizeChar["avatarSel"];
  includes/NoLifeNx/nlnx/node.hpp:118 size_t size() const;
  old loop: for (size_t i = 0; i < avatarSel.size() - 1; i++)
  new loop: for (size_t i = 0; i + 1 < avatarSel.size(); i++)

On a 64-bit build, empty avatarSel makes size()-1 wrap to SIZE_MAX and the
constructor hangs in a practically infinite loop.  The new predicate has no
subtraction on the bound and is exactly equivalent for size>=1.
"""
from __future__ import annotations

import io
import sys

CPP = "IO/UITypes/UICommonCreation.cpp"
NODE_HPP = "includes/NoLifeNx/nlnx/node.hpp"
SIZE_MAX = (1 << 64) - 1


def read_text(path: str) -> str:
    with io.open(path, "rb") as f:
        raw = f.read()
    return raw.decode("utf-8").replace("\r\n", "\n")


def modern_chunk(s: str) -> str:
    a = s.find('nl::node avatarSel = CustomizeChar["avatarSel"];')
    b = s.find("for (size_t i = 0; i + 1 < avatarSel.size(); i++)", a)
    if a < 0 or b < 0 or b <= a:
        return ""
    return s[a:b]


def no_avatar_empty_gate(s: str) -> bool:
    chunk = modern_chunk(s)
    return bool(chunk) and "if (!avatarSel)" not in chunk and "avatarSel.empty()" not in chunk


STATIC_GUARDS = [
    ("new_loop", lambda s: "for (size_t i = 0; i + 1 < avatarSel.size(); i++)" in s),
    ("old_loop_removed", lambda s: "for (size_t i = 0; i < avatarSel.size() - 1; i++)" not in s),
    ("comment_underflow", lambda s: "size()-1 是无符号下溢" in s),
    ("comment_equiv", lambda s: "size>=1 时与原式逐字等价" in s),
    ("avatarSel_source", lambda s: 'nl::node avatarSel = CustomizeChar["avatarSel"];' in s),
    ("loop_body_node_access", lambda s: 'avatarSel[i]["normal"]' in s),
    ("node_size_unsigned", lambda h: "size_t size() const;" in h),
    ("classic_guard_present", lambda s: "if (!NewChar)" in s and "[Creation] missing" in s),
    ("no_avatar_empty_gate", no_avatar_empty_gate),
]


def before_bound(n: int) -> int:
    """C++ size_t bound: n - 1, with unsigned wrap for n==0."""
    return n - 1 if n > 0 else SIZE_MAX


def before_iters(n: int):
    b = before_bound(n)
    if b == SIZE_MAX:
        return None  # HANG: do not actually iterate
    return list(range(b))


def after_iters(n: int):
    """for (size_t i = 0; i + 1 < n; i++) -> 0..n-2"""
    return list(range(max(0, n - 1)))


CASES = [
    ("empty_resource_node", 0),
    ("single_child", 1),
    ("two_children", 2),
    ("three_children", 3),
    ("five_children", 5),
    ("eight_children", 8),
    ("twelve_children", 12),
    ("thirty_two_children", 32),
]


def main() -> int:
    cpp = read_text(CPP)
    node_hpp = read_text(NODE_HPP)

    failed = []
    for name, pred in STATIC_GUARDS:
        ok = pred(node_hpp) if name == "node_size_unsigned" else pred(cpp)
        print("[%s] %s" % ("OK" if ok else "FAIL", name))
        if not ok:
            failed.append(name)
    if failed:
        print("STATIC GUARDS FAILED: %s" % ", ".join(failed))
        return 1

    before_hang = 0
    after_empty_zero = 0
    regression_ok = 0
    regression_total = 0

    for name, n in CASES:
        bnd = before_bound(n)
        b = before_iters(n)
        a = after_iters(n)
        if n == 0:
            before_hang += int(bnd == SIZE_MAX and b is None)
            after_empty_zero += int(a == [])
            ok = bnd == SIZE_MAX and b is None and a == []
            print("[%s] %-20s n=%-2d before bound=SIZE_MAX(HANG) -> after iters=0"
                  % ("OK" if ok else "FAIL", name, n))
        else:
            regression_total += 1
            ok = (b == a and len(a) == n - 1)
            regression_ok += int(ok)
            print("[%s] %-20s n=%-2d before=%s -> after=%s"
                  % ("OK" if ok else "FAIL", name, n, b, a))
        if not ok:
            failed.append(name)

    print("[underflow] empty-node before HANG=%d -> after zero-iteration=%d"
          % (before_hang, after_empty_zero))
    print("[regression] happy-path identical sequences: %d/%d" % (regression_ok, regression_total))
    if failed or before_hang != 1 or after_empty_zero != 1 or regression_ok != regression_total:
        print("FAIL: static/dynamic evidence mismatch")
        return 1
    print("PASS: %d static guards + %d scenarios (before HANG 1 -> after 0; regression %d/%d)"
          % (len(STATIC_GUARDS), len(CASES), regression_ok, regression_total))
    return 0


if __name__ == "__main__":
    sys.exit(main())
