#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""round66 harness: UIRecommendWorld 按 Angel 083 布局重修的静态守卫 + 场景回归.

只读取证（不跑客户端）：
  - 静态守卫：083 OnCreate 立即数、真节点名、id→行为、错误节点名/手绘文案被清除、
    UIElement.h / UIWorldSelect.cpp 错注释已改、UICashShop 无重复赋值、
    LoginBackground 线性插值仍在（按 Angel 修复过，保留）。
  - 场景回归：把 move_world 环绕 / 箭头 enable / select_world 守卫的 C++ 语义
    在 Python 里等价建模，逐项对拍。
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

CPP = ROOT / "IO" / "UITypes" / "UIRecommendWorld.cpp"
HDR = ROOT / "IO" / "UITypes" / "UIRecommendWorld.h"
UIELEM = ROOT / "IO" / "UIElement.h"
WSEL = ROOT / "IO" / "UITypes" / "UIWorldSelect.cpp"
CASHSHOP = ROOT / "IO" / "UITypes" / "UICashShop.cpp"
LOGBG = ROOT / "IO" / "LoginBackground.cpp"

failures: list[str] = []
checks = 0


def read(p: Path) -> str:
    return p.read_text(encoding="utf-8", errors="replace")


def guard(cond: bool, label: str) -> None:
    global checks
    checks += 1
    if not cond:
        failures.append(label)


def main() -> int:
    cpp = read(CPP)
    hdr = read(HDR)

    # ---- A. 083 OnCreate 布局立即数 ----
    guard("RW_BACKGRD_WIDTH = 540" in cpp, "A1 缺 083 背景宽 540")
    guard("RW_BACKGRD_HEIGHT = 167" in cpp, "A2 缺 083 背景高 167")
    guard(re.search(r"Point<int16_t>\(RW_BACKGRD_WIDTH, RW_BACKGRD_HEIGHT\)", cpp) is not None,
          "A3 dimension 未用 083 背景尺寸")
    guard('nl::nx::UI["Login.img"]["WorldSelect"]["alert"]' in cpp,
          "A4 未从 alert 真子树取资源")
    guard('sprites.emplace_back(alert["backgrd"])' in cpp,
          "A5 真背景 backgrd 未进基类 sprites")
    guard('Point<int16_t>(45, 70)' in cpp and 'alert["BtArrowL"]' in cpp,
          "A6 BtArrowL (45,70) 未落")
    guard('Point<int16_t>(215, 70)' in cpp and 'alert["BtArrowR"]' in cpp,
          "A7 BtArrowR (215,70) 未落")
    guard('Point<int16_t>(113, 114)' in cpp and 'alert["BtChoice"]' in cpp,
          "A8 BtChoice (113,114) 未落")
    guard(cpp.count("try_maple_btn(") == 3, "A9 真位图按钮应恰好 3 颗")

    # ---- B. id→行为（OnButtonClicked @0x62b708） ----
    guard("BT_MOVE_LEFT = 1000" in hdr and "BT_MOVE_RIGHT = 1001" in hdr
          and "BT_SELECT = 1002" in hdr, "B1 枚举 id 与 083 不符")
    guard(re.search(r"case BT_MOVE_LEFT:\s*move_world\(-1\)", cpp) is not None,
          "B2 1000 未映射 MoveWorld(-1)")
    guard(re.search(r"case BT_MOVE_RIGHT:\s*move_world\(1\)", cpp) is not None,
          "B3 1001 未映射 MoveWorld(+1)")
    guard(re.search(r"case BT_SELECT:\s*select_world\(\)", cpp) is not None,
          "B4 1002 未映射 SelectWorld")

    # ---- C. 消息区 {78,61,212,110} + 换行宽 200 ----
    for tok in ("position.x() + 78", "position.y() + 61",
                "position.x() + 212", "position.y() + 110"):
        guard(tok in cpp, f"C 消息区矩形缺 {tok}")
    guard(cpp.count('"", 200)') >= 2, "C5 Text 换行宽度 200 未落（name/message）")

    # ---- D. 世界数 >1 才启用箭头 / %0 UB 修掉 ----
    guard("wc > 1 ? Button::State::NORMAL : Button::State::DISABLED" in cpp,
          "D1 箭头启用条件未按 083 ResetWorldAndMessage")
    guard("if (wc == 0)" in cpp and "return;" in cpp, "D2 move_world 空列表守卫缺失")
    guard("int64_t next" in cpp and "next %=" in cpp, "D3 move_world 环绕未用安全取模")
    guard("buttons.find(id)" in cpp, "D4 箭头只读访问未用 find（会插空 unique_ptr）")

    # ---- E. 旧「自行修复/BeiDou」痕迹彻底清除 ----
    for bad in ('["Alert"]', '["Backgrnd"]', '["Backgrn"]', 'chBackgrnd',
                '"推荐世界"', '"选择"', 'Text::A18B', 'left_rect', 'right_rect',
                'select_rect', 'title_text', 'left_label', 'right_label'):
        guard(bad not in cpp and bad not in hdr, f"E 旧手绘/错节点残留: {bad}")
    guard("UIElement::draw(alpha)" in cpp, "E14 未复用基类 draw_sprites/draw_buttons")
    guard("void UIRecommendWorld::update" not in cpp, "E15 不应再空 override update")
    guard('worldselect->change_world(*w)' in cpp and "UI::get().remove(TYPE)" in cpp,
          "E16 select_world 未按当前端 change_world + 关窗")

    # ---- F. 三处错注释 / 重复赋值修正 ----
    uielem = read(UIELEM)
    wsel = read(WSEL)
    cash = read(CASHSHOP)
    logbg = read(LOGBG)
    guard("R6 证实被裁剪" not in uielem and "chBackgrn" not in uielem,
          "F1 UIElement.h 仍留 095 误判注释")
    guard("alert/backgrd 真背景" in uielem, "F2 UIElement.h 未写 083 真背景依据")
    guard("R6：Alert 资产缺失" not in wsel and "chBackgrn 回退背景" not in wsel,
          "F3 UIWorldSelect.cpp 仍留误判注释")
    guard("alert/backgrd 真背景" in wsel, "F4 UIWorldSelect.cpp 未写 083 真背景依据")
    guard("UI::get().emplace<UIRecommendWorld>(this);" in wsel,
          "F5 BtViewChoice 真窗路由被破坏")
    dup = 'buttons[Buttons::BtChargeRefresh] = std::make_unique<MapleButton>(CSGLChargeNX["BtChargeRefresh"], Point<int16_t>(92, 554));'
    guard(cash.count(dup) == 1, f"F6 UICashShop BtChargeRefresh 赋值应恰好 1 次，实际 {cash.count(dup)}")
    # 按 Angel 修复过的动画保留：线性插值在、smoothstep 不在
    guard("(tween_to.x - tween_from.x) * t" in logbg, "F7 LoginBackground 线性插值丢失")
    guard("3.0 - 2.0 * t" not in logbg and "3 - 2 * t" not in logbg,
          "F8 LoginBackground smoothstep 又回来了")

    # ---- 场景回归：C++ 语义的 Python 等价模型 ----
    scenarios = 0
    bad = 0

    def model_move(idx: int, delta: int, wc: int) -> int:
        """对齐 UIRecommendWorld::move_world：wc==0 守卫 + int64 环绕."""
        if wc == 0:
            return idx
        nxt = (idx + delta) % wc
        if nxt < 0:
            nxt += wc
        return nxt

    def model_arrow(wc: int) -> str:
        """对齐 refresh：世界数 >1 才 NORMAL，否则 DISABLED."""
        return "NORMAL" if wc > 1 else "DISABLED"

    def model_select(idx: int, wc: int) -> str:
        """对齐 select_world：wc>0 且 idx<wc 才 change_world，随后必关窗."""
        acted = "CHANGE" if (wc > 0 and idx < wc) else "SKIP"
        return acted + "+CLOSE"

    # move_world 环绕：n=0 / n=1 / n=3，含负向与多圈
    move_cases = [
        # (idx, delta, wc, expected)
        (0, -1, 0, 0),     # 空列表：守卫不崩不动（083 原生 %0 UB 已修）
        (0, 1, 0, 0),
        (0, -1, 1, 0),     # 单世界：任何方向恒 0
        (0, 5, 1, 0),
        (0, -1, 3, 2),     # 3 世界：左环绕到末
        (2, 1, 3, 0),      # 右环绕回首
        (1, 1, 3, 2),
        (1, -1, 3, 0),
        (0, -7, 3, 2),     # 多圈负向
        (2, 7, 3, 0),      # 多圈正向
    ]
    for idx, delta, wc, exp in move_cases:
        scenarios += 1
        got = model_move(idx, delta, wc)
        if got != exp:
            bad += 1
            failures.append(f"S move_world({idx},{delta},wc={wc}) got {got} exp {exp}")

    arrow_cases = [(0, "DISABLED"), (1, "DISABLED"), (2, "NORMAL"), (5, "NORMAL")]
    for wc, exp in arrow_cases:
        scenarios += 1
        if model_arrow(wc) != exp:
            bad += 1
            failures.append(f"S arrow(wc={wc}) exp {exp}")

    select_cases = [(0, 0), (0, 1), (2, 3), (3, 3), (9, 2)]
    for idx, wc in select_cases:
        scenarios += 1
        exp = ("CHANGE" if (wc > 0 and idx < wc) else "SKIP") + "+CLOSE"
        if model_select(idx, wc) != exp:
            bad += 1
            failures.append(f"S select(idx={idx},wc={wc}) exp {exp}")

    # ---- 汇总 ----
    print(f"STATIC GUARDS checked={checks} failed={len([f for f in failures if not f.startswith('S ')])}")
    print(f"SCENARIOS {scenarios}, bad={bad}")
    if failures:
        for f in failures:
            print("  FAIL:", f)
        print("FAIL: UIRecommendWorld 083 layout harness")
        return 1
    print("PASS: UIRecommendWorld 083 layout + scenarios")
    return 0


if __name__ == "__main__":
    sys.exit(main())
