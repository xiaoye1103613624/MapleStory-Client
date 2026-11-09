#!/usr/bin/env python3
"""Offline harness: UINpcTalk SENDSIMPLE keyboard navigation visible-band guard.

Replicates IO/UITypes/UINpcTalk.cpp:733-802 send_key SENDSIMPLE UP/DOWN and the
new ensure_visible() lambda with the SAME geometry as draw():234 /
send_cursor():636. Before-fix code only moves simple_selected; after-fix code
also scrolls offset so the highlighted row stays inside the visible band and
ENTER cannot submit an invisible choice.
"""
import io

LIVE = r"E:/project/MapleStory-Client/IO/UITypes/UINpcTalk.cpp"

# ---- geometry (mirrors UINpcTalk.h constants + change_text slider setup) ----
SCROLL_STEP = 18          # scroll_step = A12M line space
LS = 18                   # Text::linespace(Text::Font::A12M)
TEXT_SLACK = 18
MAX_HEIGHT = 248          # height when show_slider
TEXT_Y_PAD = 7
TOP_H = 100               # simulated UtilDlgEx top plate height
TEXT_DRAW_Y = TOP_H + TEXT_Y_PAD
BAND_TOP = TOP_H - 1
BAND_BOTTOM = BAND_TOP + MAX_HEIGHT - TEXT_SLACK


def rowmax_of(n):
    """change_text : rowmax = (over + scroll_step - 1)/scroll_step + 1."""
    over = n * LS - (MAX_HEIGHT - TEXT_SLACK)
    if over <= 0:
        return 1
    return (over + SCROLL_STEP - 1) // SCROLL_STEP + 1


def vline(sel, offset):
    return TEXT_DRAW_Y + sel * LS - offset * SCROLL_STEP


def visible(sel, offset, show_slider):
    """draw():245 / send_cursor():640 visible-band predicate, verbatim."""
    if not show_slider:
        return True
    v = vline(sel, offset)
    return v >= BAND_TOP and v + LS <= BAND_BOTTOM


def ensure_visible(sel, offset, n, show_slider, rowmax, unitrows=1):
    """Replicates the C++ ensure_visible lambda inserted by _apply_wave2_fix.py."""
    if not show_slider or n == 0 or sel < 0 or sel >= n:
        return offset
    maxoffset = rowmax - unitrows
    if maxoffset < 0:
        maxoffset = 0
    v = vline(sel, offset)
    if v + LS > BAND_BOTTOM:
        while offset < maxoffset and v + LS > BAND_BOTTOM:
            offset += 1
            v = vline(sel, offset)
    elif v < BAND_TOP:
        while offset > 0 and v < BAND_TOP:
            offset -= 1
            v = vline(sel, offset)
    return offset


def press(sel, offset, n, key, guarded, show_slider, rowmax):
    if key == "UP":
        if sel > 0:
            sel -= 1
    elif key == "DOWN":
        if n > 0 and sel + 1 < n:
            sel += 1
    if guarded:
        offset = ensure_visible(sel, offset, n, show_slider, rowmax)
    return sel, offset


# ---- static guard verification (live source, not a summary) ----
with io.open(LIVE, "r", encoding="utf-8") as f:
    src = f.read()

GUARDS = [
    ("lambda_def", "auto ensure_visible = [&]()"),
    ("down_band", "vline + ls > band_bottom"),
    ("up_band", "vline < band_top"),
    ("thumb_sync", "slider.setrows(offset, unitrows, rowmax);"),
    ("draw_guard_kept", "visible = !show_slider"),
    # live :641-642 是分两行（continue 独占一行），按 live 逐字取判据行，不拼摘要。
    ("cursor_guard_kept",
     "if (show_slider && (vline < band_top || vline + ls > band_bottom))"),
]
for name, needle in GUARDS:
    ok = needle in src
    print("[%s] %-22s %s" % ("OK" if ok else "FAIL", name, needle[:46]))
    assert ok, "static guard missing: " + name

calls = src.count("ensure_visible();")
print("[%s] %-22s UP+DOWN calls = %d (expected 2)" %
      ("OK" if calls == 2 else "FAIL", "nav_calls", calls))
assert calls == 2, "ensure_visible() call count must be exactly 2, got %d" % calls


# ---- scenario A: long menu, DOWN across the whole list ----
def run_down(n, guarded, show_slider=True, start=0):
    rm = rowmax_of(n)
    sel, off = start, 0
    states = []
    for _ in range(n - 1 - start):
        sel, off = press(sel, off, n, "DOWN", guarded, show_slider, rm)
        states.append((sel, off, visible(sel, off, show_slider)))
    return sel, off, states


def run_up(n, guarded, sel, off, show_slider=True):
    rm = rowmax_of(n)
    states = []
    for _ in range(sel):
        sel, off = press(sel, off, n, "UP", guarded, show_slider, rm)
        states.append((sel, off, visible(sel, off, show_slider)))
    return sel, off, states


CASES = []

# A1: 20-choice long menu (auction NPC 9900001 style).
n = 20
sel_b, off_b, st_b = run_down(n, guarded=False)
sel_a, off_a, st_a = run_down(n, guarded=True)
bad_b = sum(1 for s, o, v in st_b if not v)
bad_a = sum(1 for s, o, v in st_a if not v)
print("[OK] A1 long-menu DOWN: before(invisible=%d, end off=%d) -> after(invisible=%d, end off=%d)"
      % (bad_b, off_b, bad_a, off_a))
assert bad_b > 0, "before-fix must expose invisible selections"
assert bad_a == 0, "after-fix must keep every selection visible"
assert off_a >= off_b, "after-fix must scroll offset"
CASES.append(("A1", bad_b, bad_a))

# A2: roll back UP from the after-fix end state.
sel_u, off_u, st_u = run_up(n, guarded=True, sel=sel_a, off=off_a)
bad_u = sum(1 for s, o, v in st_u if not v)
print("[OK] A2 roll-back UP: invisible=%d end(sel=%d, off=%d)" % (bad_u, sel_u, off_u))
assert bad_u == 0 and sel_u == 0 and off_u == 0, "UP must roll offset back to 0"
CASES.append(("A2", 0, 0))

# B: short menu, no slider -> guard early-returns, behaviour identical.
def run_short(n, guarded):
    rm = rowmax_of(n)
    sel, off = 0, 0
    for _ in range(n - 1):
        sel, off = press(sel, off, n, "DOWN", guarded, False, rm)
    return sel, off

for n_short in (2, 3, 5, 8):
    sb, ob = run_short(n_short, False)
    sa, oa = run_short(n_short, True)
    print("[OK] B short n=%d: before(sel=%d,off=%d) == after(sel=%d,off=%d)"
          % (n_short, sb, ob, sa, oa))
    assert (sb, ob) == (sa, oa), "no-slider regression at n=%d" % n_short
    CASES.append(("B%d" % n_short, 0, 0))

# C: empty / single choice / maxoffset clamp.
for n_edge in (0, 1, 2):
    rm = rowmax_of(n_edge)
    sel, off = 0, 0
    for _ in range(12):
        sel, off = press(sel, off, n_edge, "DOWN", True, True, rm)
    assert off == 0 and sel <= max(n_edge - 1, 0), "edge clamp n=%d" % n_edge
    print("[OK] C edge n=%d: sel=%d off=%d (no runaway)" % (n_edge, sel, off))
    CASES.append(("C%d" % n_edge, 0, 0))

# D: before-fix ENTER would submit an invisible row (the actual bug).
n = 20
sel_b, off_b, st_b = run_down(n, guarded=False)
invisible_enter = [(s, o) for s, o, v in st_b if not v]
assert invisible_enter, "before-fix must have an invisible ENTER target"
print("[OK] D before-fix invisible ENTER targets: %d (e.g. sel=%d off=%d)"
      % (len(invisible_enter), invisible_enter[0][0], invisible_enter[0][1]))
CASES.append(("D", len(invisible_enter), 0))

total_before = sum(b for _, b, _ in CASES)
total_after = sum(a for _, _, a in CASES)
print("[guard] DOWN/UP selections landing outside visible band: before=%d -> after=%d"
      % (total_before, total_after))
print("[regression] no-slider + edge cases unchanged: True")
print("PASS: 7 static guards + %d scenarios (before bad=%d -> after bad=0; regression OK)"
      % (len(CASES), total_before))
