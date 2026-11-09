#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Wave2 2-12 round44: UIEnterNumber::handlestring 空串 / 溢出守卫离线 harness.

复刻 C++ 转换语义（不做真实 UI）：
  before: has_only_digits = (find_first_not_of("0123456789") == npos)
          空串 vacuous true -> std::stoi("") 抛 std::invalid_argument；
          纯数字 > INT32_MAX -> std::stoi 抛 std::out_of_range；两者均无 try/catch。
  after : has_only_digits = !empty() && (...)；stoi 包 try/catch 统一走既有提示。

验收：崩溃输入 5 组 before THROW 5 -> after 0；合法/非法输入逐字回归 8/8。
期望值全部由 SCENARIOS 标注推导，不手写死数，避免 harness 自身数错造成假绿/假红。
"""
import py_compile
import sys

ROOT = "."
NOTICE = "IO/UITypes/UINotice.cpp"
TEXTFIELD = "IO/Components/Textfield.cpp"
INVENTORY = "IO/UITypes/UIItemInventory.cpp"
SHOP = "IO/UITypes/UIShop.cpp"
INT32_MAX = (1 << 31) - 1

# ---- 静态守卫：(name, needle, path, want_present) ----
STATIC_GUARDS = [
    ("full_empty_guard", "bool has_only_digits = !numstr.empty()", NOTICE, True),
    ("find_first_not_of", 'numstr.find_first_not_of("0123456789") == std::string::npos', NOTICE, True),
    ("vacuous_comment", "find_first_not_of 返回 npos", NOTICE, True),
    ("entry_comment", "RETURN / 点 OK / Textfield enter_callback", NOTICE, True),
    ("happy_path_comment", "合法数字路径逐字不变", NOTICE, True),
    ("old_vacuous_removed", 'bool has_only_digits = (numstr.find_first_not_of', NOTICE, False),
    ("try_present", "\t\t\ttry", NOTICE, True),
    ("stoi_kept", "num = std::stoi(numstr);", NOTICE, True),
    ("catch_present", "catch (...)", NOTICE, True),
    ("catch_prompt", '"Only numbers are allowed.", okhandler', NOTICE, True),
    ("overflow_comment", "out_of_range", NOTICE, True),
    ("numfield_limit", "int16_t numfield_limit = 10;", NOTICE, True),
    ("textfield_erase", "text.erase(", TEXTFIELD, True),
    ("textfield_change_text", "numfield.change_text(std::to_string(quantity));", NOTICE, True),
    ("softkey_guard", "if (s.empty()) return;", NOTICE, True),
    ("return_entry", "handlestring(numfield.get_text());", NOTICE, True),
    ("ok_entry", "handlestring(numfield.get_text());", NOTICE, True),
    ("enter_callback_entry", "handlestring(numstr);", NOTICE, True),
    ("low_prompt", '"You may only enter a number equal to or higher than 1."', NOTICE, True),
    ("high_prompt", '"You may only enter a number equal to or lower than "', NOTICE, True),
    ("inventory_callsite", "UI::get().emplace<UIEnterNumber>", INVENTORY, True),
    ("shop_callsite", "UI::get().emplace<UIEnterNumber>", SHOP, True),
]


def read(path):
    with open(ROOT + "/" + path, "r", encoding="utf-8", errors="replace") as f:
        return f.read()


def check_static():
    failed = []
    for name, needle, path, want_present in STATIC_GUARDS:
        got = needle in read(path)
        if got != want_present:
            failed.append((name, needle, path, want_present, got))
    return failed


# ---- 转换算法复刻 ----
PROMPT = "Only numbers are allowed."
LOW = "You may only enter a number equal to or higher than 1."


def high(maxv):
    return "You may only enter a number equal to or lower than %d." % maxv


def cpp_stoi(s):
    """std::stoi 等价：空串/非数字 -> invalid_argument；> INT32_MAX -> out_of_range."""
    if not s or not all("0" <= ch <= "9" for ch in s):
        raise ValueError("invalid_argument")
    v = int(s)
    if v > INT32_MAX:
        raise OverflowError("out_of_range")
    return v


def convert(s, maxv, hardened):
    """返回 (kind, value) 或抛 (throw_kind, message)。"""
    if hardened:
        has_only_digits = bool(s) and all("0" <= ch <= "9" for ch in s)
        if not has_only_digits:
            return ("PROMPT", PROMPT)
        try:
            num = cpp_stoi(s)
        except (ValueError, OverflowError):
            # after: catch(...) -> 复用同一提示，不崩交互
            return ("PROMPT", PROMPT)
    else:
        has_only_digits = all("0" <= ch <= "9" for ch in s)  # 空串 vacuous True
        if not has_only_digits:
            return ("PROMPT", PROMPT)
        try:
            num = cpp_stoi(s)
        except ValueError as e:
            return ("THROW", "invalid_argument: " + str(e))
        except OverflowError as e:
            return ("THROW", "out_of_range: " + str(e))

    if num < 1:
        return ("PROMPT_LOW", LOW)
    if num > maxv:
        return ("PROMPT_HIGH", high(maxv))
    return ("ACCEPT", num)


# ---- 13 场景：(label, input, max, exp_before, exp_after, is_regression) ----
SCENARIOS = [
    ("empty_RETURN", "", 5, "THROW", "PROMPT", False),
    ("empty_OK", "", 1, "THROW", "PROMPT", False),
    ("overflow_10digits", "9999999999", 999999999, "THROW", "PROMPT", False),
    ("overflow_INT32MAX+1", "2147483648", 2147483647, "THROW", "PROMPT", False),
    ("overflow_12digits_via_softkey", "999999999999", 2147483647, "THROW", "PROMPT", False),
    ("nondigit_abc", "abc", 5, "PROMPT", "PROMPT", True),
    ("nondigit_12a", "12a", 5, "PROMPT", "PROMPT", True),
    ("nondigit_x", "x", 5, "PROMPT", "PROMPT", True),
    ("zero_low", "0", 5, "PROMPT_LOW", "PROMPT_LOW", True),
    ("one", "1", 5, "ACCEPT", "ACCEPT", True),
    ("max_boundary", "5", 5, "ACCEPT", "ACCEPT", True),
    ("above_max", "6", 5, "PROMPT_HIGH", "PROMPT_HIGH", True),
    ("INT32MAX_legal", "2147483647", 2147483647, "ACCEPT", "ACCEPT", True),
]


def run():
    py_compile.compile(__file__, doraise=True)
    print("PYCOMPILE_OK")
    failed = check_static()
    print("STATIC GUARDS: %d checked, %d failed" % (len(STATIC_GUARDS), len(failed)))
    for name, needle, path, want, got in failed:
        print("  FAIL", name, "needle=", repr(needle), "path=", path,
              "want=", want, "got=", got)
    if failed:
        return 1

    before_throw = 0
    after_throw = 0
    regressions = 0
    mismatch = []
    for label, s, maxv, exp_before, exp_after, is_reg in SCENARIOS:
        kb, vb = convert(s, maxv, hardened=False)
        ka, va = convert(s, maxv, hardened=True)
        if kb == "THROW":
            before_throw += 1
        if ka == "THROW":
            after_throw += 1
        if kb != exp_before or ka != exp_after:
            mismatch.append((label, kb, exp_before, ka, exp_after))
        if is_reg:
            if kb == ka and vb == va:
                regressions += 1
            else:
                mismatch.append((label, "regression", (kb, vb), (ka, va)))
        print("  %-32s before=%-12s after=%-12s %s"
              % (label, kb, ka, "REG" if is_reg else ""))

    expected_before_throw = sum(1 for sc in SCENARIOS if sc[3] == "THROW")
    expected_regressions = sum(1 for sc in SCENARIOS if sc[5])
    print("SUMMARY before_throw=%d (expected %d) after_throw=%d regression=%d/%d mismatch=%d"
          % (before_throw, expected_before_throw, after_throw, regressions,
             expected_regressions, len(mismatch)))
    for m in mismatch:
        print("  MISMATCH", m)
    ok = (not failed) and before_throw == expected_before_throw and after_throw == 0 \
         and regressions == expected_regressions and not mismatch
    print(("PASS: %d static guards + %d scenarios (before THROW %d -> after 0; regression %d/%d)"
           % (len(STATIC_GUARDS), len(SCENARIOS), expected_before_throw,
              regressions, expected_regressions)) if ok else "FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(run())
