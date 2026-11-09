#!/usr/bin/env python3
"""Offline harness: UIRaceSelect SoftKey numeric-selection visible-window guard.

Static  : verify the 4 guard fragments are literally present in the source.
Dynamic : replicate `bid = uint16(CLASS0 + idx - index_shift)` and the modern
          `button_pressed()` dispatch *verbatim* (functional buttons for bid
          0..4; `if (buttonid >= CLASS0)` has NO upper bound -> select_class()
          reads class_index[bid - CLASS0] which OOBs past INDEX_COUNT).
          Compare before-fix (upper bound only) vs after-fix (visible-window
          bound) over 10 boundary cases.

Fact-driven discipline (round35/36): before-fix MUST show a real defect; a
harness that reports 0 defects before the fix is a fake-green and fails.
Labels must match the live code: lower idx -> functional-button MISFIRE,
higher idx -> class_index[] OOB read (not "no_button").
"""
import io
import sys

SRC = "IO/UITypes/UIRaceSelect.cpp"
src = io.open(SRC, encoding="utf-8").read()

GUARDS = [
    ("cond_lower", "} else if (idx >= index_shift"),
    ("cond_upper", "static_cast<int>(index_shift) + INDEX_COUNT"),
    ("cond_global", "idx < static_cast<int>(order.size())) {"),
    ("comment", "仅当前可见窗口的职业按钮"),
]
for name, needle in GUARDS:
    ok = needle in src
    print("[%s] %-14s %s" % ("OK" if ok else "FAIL", name, needle[:52]))
    if not ok:
        print("FAIL: static guard missing")
        sys.exit(1)

CLASS0 = 5           # Buttons enum: BtStart0 BtPreview1 BtMake2 BtLeft3 BtRight4, CLASS0=5
INDEX_COUNT = 6      # UIRaceSelect.h:66 - visible class slots / class_index[]
ORDER = 20           # representative order.size() (modern race list)
MISFIRE = {
    0: "BtStart(show_worldselect)",
    1: "BtPreview(show_charselect)",
    2: "BtMake(confirm-dialog)",
    3: "BtLeftArrow",
    4: "BtRightArrow",
}


def button_pressed(bid):
    """Mirror UIRaceSelect::button_pressed() modern branch (uint16 buttonid)."""
    if bid in MISFIRE:
        return "MISFIRE:" + MISFIRE[bid]
    # live: if (buttonid >= Buttons::CLASS0) { index = buttonid-CLASS0+index_shift;
    #       select_class(index) -> selected = index-index_shift = buttonid-CLASS0;
    #       class_index[selected] read } -- NO upper bound on buttonid.
    if bid >= CLASS0:
        selected = bid - CLASS0
        if selected >= INDEX_COUNT:
            return "OOB:class_index[%d]" % selected
        return "CLASS:%d" % selected
    return "OOB:no_button"   # defensive; uint16 bid can't be negative


def softkey(idx, index_shift, guarded):
    """Return (status, outcome) for the SoftKey numeric-selection branch."""
    if idx < 0:
        return "ignore", None                       # live: if (idx < 0) return;
    if guarded:
        if not (idx >= index_shift
                and idx < index_shift + INDEX_COUNT
                and idx < ORDER):
            return "suppressed", None
    else:
        if not (idx < ORDER):                      # live: idx < order.size() only
            return "ignore", None
    # live: static_cast<uint16_t>(Buttons::CLASS0 + idx - index_shift)
    bid = (CLASS0 + idx - index_shift) & 0xFFFF
    return "fire", button_pressed(bid)


CASES = [
    # tag, idx, index_shift, kind
    ("A_small_btstart", 0, 5, "MIS"),
    ("B_btmake", 2, 5, "MIS"),
    ("C_btright", 4, 5, "MIS"),
    ("D_far_oob", 15, 5, "OOB"),
    ("E_first_visible", 5, 5, "REG"),
    ("F_last_visible", 10, 5, "REG"),
    ("G_noshift_first", 0, 0, "REG"),
    ("H_noshift_last", 5, 0, "REG"),
    ("I_just_past", 6, 0, "OOB"),
    ("J_shift10_tail", 19, 10, "OOB"),
]

before_bad = 0
after_bad = 0
reg_ok = 0
reg_total = 0
for tag, idx, sh, kind in CASES:
    _, b = softkey(idx, sh, False)
    _, a = softkey(idx, sh, True)
    bbad = b is not None and (b.startswith("MISFIRE") or b.startswith("OOB"))
    abad = a is not None and (a.startswith("MISFIRE") or a.startswith("OOB"))
    if bbad:
        before_bad += 1
    if abad:
        after_bad += 1
    if kind == "REG":
        reg_total += 1
        if a is not None and a == b and a.startswith("CLASS:"):
            reg_ok += 1
    print("[OK] %-20s before=%-30s after=%s" % (tag, b, a))

print("[summary] before_bad=%d after_bad=%d reg_ok=%d/%d"
      % (before_bad, after_bad, reg_ok, reg_total))
ok = (after_bad == 0 and before_bad > 0 and reg_ok == reg_total)
if ok:
    print("PASS: %d static guards + %d cases (before bad=%d -> after bad=%d; regression %d/%d)"
          % (len(GUARDS), len(CASES), before_bad, after_bad, reg_ok, reg_total))
    sys.exit(0)
print("FAIL: guard ineffective / before-fix showed no defect / regression broken")
sys.exit(1)
