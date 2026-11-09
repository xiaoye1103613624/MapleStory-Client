#!/usr/bin/env python3
"""round43 Wave2 2-11 - UIOptionMenu sButtonUOL 空串 / 非数字守卫离线自测。

缺陷（修复前，IO/UITypes/UIOptionMenu.cpp:96-98）::

    std::string sButtonUOL = graphic["combo:resolution"]["sButtonUOL"].get_string();
    std::string ctype = std::string(1, sButtonUOL.back());
    MapleComboBox::Type type = static_cast<MapleComboBox::Type>(std::stoi(ctype));

``nl::node::get_string(std::string def = "")`` 在 ``m_data == nullptr`` 或节点
类型为 none/vector/bitmap/audio 时返回 *def*（空串）；见
``includes/NoLifeNx/nlnx/node.cpp:199-217``。于是：

* 空串 ``.back()``  -> C++ UB（std::string::back 前置条件非空）
* ``std::stoi("")`` / ``std::stoi("x")`` -> ``std::invalid_argument``

两者都发生在 ``UIOptionMenu::UIOptionMenu()`` 构造期，且 ``:53`` 的
``UiAssetResolve::usable(OptionMenu)`` fallback 只护住整个 ``OptionMenu`` 根，
护不住 ``graphic["combo:resolution"]["sButtonUOL"]`` 子节点被裁剪的情形。

本 harness 不链接游戏、不解析 WZ/IMG/XML，只做两件事：
1. 逐字静态守卫核对 live 源码（11 条）。
2. 复刻修复前/后 ``sButtonUOL -> MapleComboBox::Type`` 的转换算法，用
   13 个输入证明 before 的 UB/THROW 归零、合法数字路径逐字回归一致。

退出码 0 = 全部通过。
"""

from __future__ import annotations

import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
SRC = ROOT / "IO" / "UITypes" / "UIOptionMenu.cpp"
NODE = ROOT / "includes" / "NoLifeNx" / "nlnx" / "node.cpp"
GENDER = ROOT / "IO" / "UITypes" / "UIGender.cpp"
COMBOBOX_H = ROOT / "IO" / "Components" / "MapleComboBox.h"

DEFAULT = 1  # MapleComboBox::Type::DEFAULT == 1 (MapleComboBox.h:34)


def read(path: pathlib.Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")


# ---------------------------------------------------------------------------
# 1. 静态守卫：逐字核对 live 源码，防「看起来改了」的假绿
# ---------------------------------------------------------------------------
STATIC_GUARDS = [
    ("default_init",
     "MapleComboBox::Type type = MapleComboBox::Type::DEFAULT;",
     SRC, True, "修复后先给 DEFAULT 兜底初值"),
    ("empty_guard",
     "if (!sButtonUOL.empty())",
     SRC, True, "空串守卫必须存在"),
    ("digit_guard",
     "ctype >= '0' && ctype <= '9'",
     SRC, True, "末位数字范围守卫必须存在"),
    ("stoi_guarded",
     "std::stoi(std::string(1, ctype))",
     SRC, True, "stoi 只在数字守卫内对单字符调用"),
    ("old_ub_line_removed",
     "std::string ctype = std::string(1, sButtonUOL.back());",
     SRC, False, "修复前裸 .back() 构造 ctype 的行已删"),
    ("old_stoi_line_removed",
     "std::stoi(ctype)",
     SRC, False, "修复前裸 std::stoi(ctype) 已删"),
    ("comment_get_string_empty",
     "get_string() 在节点缺失",
     SRC, True, "注释说明空串来源"),
    ("comment_node_ref",
     "nlnx/node.cpp:199-217",
     SRC, True, "注释标注 nl::node 证据位置"),
    ("comment_gender_ref",
     "UIGender.cpp:56",
     SRC, True, "注释标注 DEFAULT 回退的同类先例"),
    ("node_get_string_default",
     "if (!m_data)\n            return def;",
     NODE, True, "get_string 在缺失节点时返回默认（空串）"),
    ("gender_uses_default",
     "MapleComboBox::Type::DEFAULT",
     GENDER, True, "UIGender 本身即用 DEFAULT 构造 ComboBox"),
]

# 枚举合法性（DEFAULT=1, BROWN=3..）由 MapleComboBox.h 定义，本轮 L-A 不裁决
COMBOBOX_ENUM = [
    ("combobox_enum_default", "DEFAULT = 1", COMBOBOX_H, True),
]


def run_static() -> list[str]:
    failures: list[str] = []
    for name, needle, path, want_present, *_ in STATIC_GUARDS + COMBOBOX_ENUM:
        if not path.is_file():
            failures.append(f"{name}: missing file {path}")
            continue
        found = needle in read(path)
        if found != want_present:
            failures.append(
                f"{name}: expected present={want_present}, got {found} in {path.name}"
            )
    return failures


# ---------------------------------------------------------------------------
# 2. 算法复刻：修复前（UB / throw） vs 修复后（守卫 + DEFAULT 兜底）
# ---------------------------------------------------------------------------
def before(sbuttonuol: str):
    """复刻修复前 C++ 语义；返回 int 或 'UB' / 'THROW'。"""
    # std::string ctype = std::string(1, sButtonUOL.back());
    if len(sbuttonuol) == 0:
        return "UB"  # std::string::back() on empty -> undefined behaviour
    ctype = sbuttonuol[-1]
    # MapleComboBox::Type type = static_cast<...>(std::stoi(ctype));
    if not ("0" <= ctype <= "9"):
        return "THROW"  # std::stoi("x") -> std::invalid_argument
    return int(ctype)


def after(sbuttonuol: str) -> int:
    """复刻修复后语义；恒返回合法 int，不 UB、不抛。"""
    # MapleComboBox::Type type = MapleComboBox::Type::DEFAULT;
    result = DEFAULT
    # if (!sButtonUOL.empty()) { char ctype = ...; if (digit) type = stoi(...); }
    if len(sbuttonuol) > 0:
        ctype = sbuttonuol[-1]
        if "0" <= ctype <= "9":
            result = int(ctype)
    return result


# 覆盖：空串 / 非数字末位 / 合法数字 / 数字 0（非枚举但原式照透传）/ 边界
CASES = [
    "",                       # 节点缺失 -> get_string() 返回空串
    "x",                      # 非数字单字符
    "abc/",                   # UOL 尾部残缺
    "ComboBox",               # 无数字后缀
    "ComboBox1",              # DEFAULT
    "ComboBox3",              # BROWN
    "ComboBox9",              # GREENNEG
    "ComboBox0",              # 非枚举值：原式透传，本轮不裁决
    "/UI/Basic.img/ComboBox1",
    "1",
    "3",
    "7",
    "ComboBox2",              # 原枚举无 2；原式透传，本轮不裁决
]


def run_scenarios() -> tuple[list[str], int, int, int]:
    failures: list[str] = []
    before_bad = 0
    after_bad = 0
    regression_ok = 0

    for s in CASES:
        b = before(s)
        a = after(s)

        if b in ("UB", "THROW"):
            before_bad += 1
        if not isinstance(a, int):
            after_bad += 1
            failures.append(f"case {s!r}: after returned non-int {a!r}")

        # happy-path 回归：修复前正常返回 int 的输入，修复后必须逐字一致
        if isinstance(b, int):
            if a != b:
                failures.append(f"case {s!r}: regression before={b} after={a}")
            else:
                regression_ok += 1

        print(f"  {s!r:32s} before={str(b):5s} after={a}")

    if before_bad == 0:
        failures.append("before_bad == 0: 缺陷未被复现，harness 标尺失效")
    if after_bad != 0:
        failures.append(f"after_bad == {after_bad}: 修复后仍有 UB/THROW")
    return failures, before_bad, after_bad, regression_ok


def main() -> int:
    static_fail = run_static()
    print(f"STATIC GUARDS: {len(STATIC_GUARDS) + len(COMBOBOX_ENUM)} checked, "
          f"{len(static_fail)} failed")
    for f in static_fail:
        print("  FAIL", f)

    print("SCENARIOS:")
    scen_fail, before_bad, after_bad, regression_ok = run_scenarios()
    for f in scen_fail:
        print("  FAIL", f)

    total_fail = len(static_fail) + len(scen_fail)
    regression_total = sum(1 for s in CASES if isinstance(before(s), int))
    if total_fail:
        print(f"FAIL: {total_fail} problem(s)")
        return 1

    print(f"PASS: {len(STATIC_GUARDS) + len(COMBOBOX_ENUM)} static guards + "
          f"{len(CASES)} scenarios "
          f"(before UB/THROW {before_bad} -> after {after_bad}; "
          f"regression {regression_ok}/{regression_total})")
    return 0


if __name__ == "__main__":
    sys.exit(main())
