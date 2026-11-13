#!/usr/bin/env python3
"""HudLayout formula smoke (mirrors Util/HudLayout.h)."""
HUD_DESIGN_W = 800
QS08, QS26, QS32 = 151, 466, 571


def is_qs_wide(vw, slots32=False):
    qs = QS32 if slots32 else QS26
    return vw >= HUD_DESIGN_W + qs


def qs_panel_w(vw, slots32=False):
    if not is_qs_wide(vw, slots32):
        return QS08
    return QS32 if slots32 else QS26


def hud_content_w(vw, slots32=False):
    if not is_qs_wide(vw, slots32):
        return HUD_DESIGN_W
    return HUD_DESIGN_W + qs_panel_w(vw, slots32)


def hud_center_x(vw, content_w):
    return (vw - content_w) // 2 if vw > content_w else 0


def qs_x_in_group(vw, slots32=False):
    qs = qs_panel_w(vw, slots32)
    if not is_qs_wide(vw, slots32):
        grid = HUD_DESIGN_W if vw > HUD_DESIGN_W else vw
        return grid - qs
    return HUD_DESIGN_W


def check():
    # narrow: short QS, no center
    assert not is_qs_wide(1024)
    assert qs_panel_w(1024) == QS08
    assert hud_content_w(1024) == 800
    assert hud_center_x(1024, 800) == (1024 - 800) // 2
    assert qs_x_in_group(1024) == 800 - QS08
    # 1280 still short of 800+466=1266? 1280 >= 1266 → wide
    assert is_qs_wide(1280)
    assert qs_panel_w(1280) == QS26
    assert hud_content_w(1280) == 800 + QS26
    cx = hud_center_x(1280, 800 + QS26)
    assert cx == (1280 - (800 + QS26)) // 2
    assert qs_x_in_group(1280) == 800
    # qs and 800 bar do not overlap horizontally in group space
    assert qs_x_in_group(1280) >= HUD_DESIGN_W
    print("HUDLAYOUT_OK")


if __name__ == "__main__":
    check()
