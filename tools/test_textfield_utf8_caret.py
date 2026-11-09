#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
round36 · Textfield UTF-8 caret boundary guard self-test.

Facts (live source, read verbatim 2026-11-09):
- GraphicsGL.cpp:1349-1358  LayoutBuilder::advances is indexed by BYTE; every
  byte of a multi-byte codepoint occupies a slot with the SAME pen origin.
  Textfield::draw:78 uses textlabel.advance(markerpos) -> markerpos is a BYTE
  index, not a codepoint index.
- Textfield.cpp BACK  :192-202 and DELETE :230-238 already step markerpos to
  the nearest codepoint LEAD byte (skip 0x80..0xBF continuation bytes).
- Textfield.cpp add_string:294 inserts with text.insert(markerpos, 1, c): if
  markerpos sits on a continuation byte the inserted byte splits a multi-byte
  character into an INVALID UTF-8 sequence (mojibake / broken glyph).
- Defect (pre-fix): LEFT/RIGHT :174-187 were a bare markerpos--/++; on CJK text
  the caret lands inside a 3-byte codepoint -> split-character on next insert.

Fix: LEFT backs up to the previous LEAD byte; RIGHT skips the current codepoint's
continuation bytes to the next LEAD byte. Same codepoint discipline as BACK/DELETE.

This harness replicates the caret-move algorithm in Python and verifies:
  (1) static: 4 codepoint-boundary guards present in Textfield.cpp
  (2) after-fix: caret ALWAYS lands on a UTF-8 codepoint boundary
  (3) before-fix: caret lands on continuation bytes and insert() splits chars
  (4) ASCII regression: guarded moves are byte-identical to the legacy +/-1
"""
import io
import re
import sys

SRC = "IO/Components/Textfield.cpp"


def read_src():
    with io.open(SRC, "r", encoding="utf-8") as f:
        return f.read()


def static_guards():
    """4 codepoint-boundary loops must be present verbatim."""
    s = read_src()
    guards = [
        ("LEFT lead-byte rewind", r"size_t start = markerpos - 1;"),
        ("LEFT skip continuation", r"while \(start > 0 && \(static_cast<unsigned char>\(text\[start\]\) & 0xC0\) == 0x80\)"),
        ("RIGHT skip continuation", r"while \(end < text\.size\(\) && \(static_cast<unsigned char>\(text\[end\]\) & 0xC0\) == 0x80\)"),
        ("RIGHT advance to next lead", r"markerpos = end;"),
        ("BACK existing guard", r"size_t start = static_cast<size_t>\(markerpos\) - 1;"),
        ("DELETE existing guard", r"size_t end = static_cast<size_t>\(markerpos\) \+ 1;"),
    ]
    ok = 0
    for label, pat in guards:
        found = re.search(pat, s) is not None
        print("[%s] static guard: %s" % ("OK" if found else "FAIL", label))
        ok += 1 if found else 0
    return ok, len(guards)


# ---- byte-level caret model -------------------------------------------------

def is_lead_boundary(b, pos):
    """True if pos is a legal UTF-8 codepoint LEAD boundary (not a cont byte)."""
    return pos == 0 or pos == len(b) or (b[pos] & 0xC0) != 0x80


def utf8_valid(b):
    try:
        b.decode("utf-8")
        return True
    except UnicodeDecodeError:
        return False


def move(b, markerpos, direction, guarded):
    """Replicate Textfield::send_key LEFT/RIGHT markerpos update."""
    if direction == "LEFT":
        if markerpos <= 0:
            return markerpos
        if not guarded:
            return markerpos - 1
        start = markerpos - 1
        while start > 0 and (b[start] & 0xC0) == 0x80:
            start -= 1
        return start
    else:  # RIGHT
        if markerpos >= len(b):
            return markerpos
        if not guarded:
            return markerpos + 1
        end = markerpos + 1
        while end < len(b) and (b[end] & 0xC0) == 0x80:
            end += 1
        return end


def insert_split_demo(b, markerpos):
    """add_string inserts 1 ASCII byte at markerpos; does it break UTF-8?"""
    mutated = b[:markerpos] + b"X" + b[markerpos:]
    return utf8_valid(mutated)


def walk(b, start, direction, steps, guarded):
    """Return list of caret positions after each keypress."""
    pos = start
    out = []
    for _ in range(steps):
        pos = move(b, pos, direction, guarded)
        out.append(pos)
    return out


# ---- test cases -------------------------------------------------------------

def cases():
    ascii_b = b"hello"                      # 1-byte codepoints only
    cjk_b = "你好".encode("utf-8")           # 6 bytes, leads at 0 and 3
    mix_b = "a你b".encode("utf-8")           # 5 bytes, leads at 0,1,4
    emo_b = "中😀文".encode("utf-8")          # 3 + 4 + 3 bytes, leads 0,3,7
    return [
        # (id, group, bytes, start, direction, steps)
        ("C01", "REG", ascii_b, 5, "LEFT", 5),
        ("C02", "REG", ascii_b, 0, "RIGHT", 5),
        ("C03", "CJK", cjk_b, 6, "LEFT", 6),
        ("C04", "CJK", cjk_b, 0, "RIGHT", 6),
        ("C05", "CJK", cjk_b, 3, "LEFT", 2),
        ("C06", "MIX", mix_b, 5, "LEFT", 5),
        ("C07", "MIX", mix_b, 0, "RIGHT", 5),
        ("C08", "EMO", emo_b, len(emo_b), "LEFT", len(emo_b)),
        ("C09", "EMO", emo_b, 0, "RIGHT", len(emo_b)),
        ("C10", "REG", ascii_b, 2, "RIGHT", 3),
    ]


def main():
    print("=== 1. static guards in %s ===" % SRC)
    g_ok, g_total = static_guards()
    if g_ok != g_total:
        print("FAIL: static guards %d/%d" % (g_ok, g_total))
        return 1

    print("\n=== 2. caret boundary + split-insert (fix before/after) ===")
    total_bad_after = 0
    total_bad_before = 0
    total_split_before = 0
    regression_ok = True
    for cid, group, b, start, direction, steps in cases():
        before = walk(b, start, direction, steps, guarded=False)
        after = walk(b, start, direction, steps, guarded=True)
        bad_before = [p for p in before if not is_lead_boundary(b, p)]
        bad_after = [p for p in after if not is_lead_boundary(b, p)]
        # split demo: take the first move's landing spot
        p_before = before[0] if before else start
        p_after = after[0] if after else start
        split_before = not insert_split_demo(b, p_before)
        split_after = not insert_split_demo(b, p_after)
        total_bad_before += len(bad_before)
        total_bad_after += len(bad_after)
        total_split_before += 1 if split_before else 0
        if group == "REG" and before != after:
            regression_ok = False  # ASCII path must be byte-identical
        ok = (len(bad_after) == 0) and (not split_after)
        print("[%s] %s %s %-5s %-6s before(land=%s,bad=%d,split=%s) "
              "-> after(land=%s,bad=%d,split=%s)" % (
                  "OK" if ok else "FAIL", cid, group, direction, steps,
                  before[:4], len(bad_before), split_before,
                  after[:4], len(bad_after), split_after))
        if not ok:
            return 1

    print("\n=== 3. three independent closures ===")
    print("[guard LEFT/RIGHT] after-fix carets on continuation bytes: "
          "%d (expected 0)" % total_bad_after)
    print("[before-fix evidence] before-fix carets on continuation bytes: "
          "%d" % total_bad_before)
    split_cases = sum(
        1 for cid, group, b, start, direction, steps in cases()
        if group != "REG" and not insert_split_demo(
            b, walk(b, start, direction, steps, guarded=False)[0])
    )
    print("[before-fix evidence] CJK/MIX/EMO first-move insert() splits: "
          "%d (expected >0)" % split_cases)
    print("[regression] ASCII guarded == unguarded: %s" % regression_ok)

    passed = (g_ok == g_total and total_bad_after == 0
              and total_bad_before > 0 and split_cases > 0
              and regression_ok)
    print("\n%s: %d static guards + %d boundary cases "
          "(before bad=%d/split=%d -> after bad=0; ASCII regression %s)" % (
              "PASS" if passed else "FAIL", g_ok, len(cases()),
              total_bad_before, split_cases, "OK" if regression_ok else "BROKEN"))
    return 0 if passed else 1


if __name__ == "__main__":
    sys.exit(main())
