#!/usr/bin/env python3
"""One-shot precise patch: UIRaceSelect SoftKey numeric class-selection guard.

Adds a visible-window lower/upper guard so `idx` (a global `order` index) can
only map into the current [index_shift, index_shift + INDEX_COUNT) class-button
window. Avoids (A) buttonid landing on BtStart/BtMake/... functional buttons
and (B) select_class() reading class_index[selected] out of bounds.

Adaptive indent detection (tab/CRLF tolerant) - round34/35/37 lesson: old_string
with tabs + CJK comments via the edit tool is fragile; python exact replace wins.
"""
import io
import re

P = "IO/UITypes/UIRaceSelect.cpp"
s = io.open(P, encoding="utf-8", newline="").read()
crlf = "\r\n" in s
body = s.replace("\r\n", "\n")

# --- guard 1: visible-window condition on the SoftKey numeric branch ---------
old_cond = "} else if (idx < static_cast<int>(order.size())) {"
assert body.count(old_cond) == 1, "cond anchor count=%d" % body.count(old_cond)
m = re.search(r"(?m)^([ \t]*)" + re.escape(old_cond), body)
indent = m.group(1)
cont = indent + "\t\t"
new_cond = ("} else if (idx >= index_shift\n"
            + cont + "&& idx < static_cast<int>(index_shift) + INDEX_COUNT\n"
            + cont + "&& idx < static_cast<int>(order.size())) {")
# old_cond has no leading indent; the original indent stays in front of it.
body = body.replace(old_cond, new_cond, 1)

# --- guard 2: rationale comment right above the button_pressed line ---------
old_press = "button_pressed(static_cast<uint16_t>(Buttons::CLASS0 + idx - index_shift));"
assert body.count(old_press) == 1, "press anchor count=%d" % body.count(old_press)
m2 = re.search(r"(?m)^([ \t]*)" + re.escape(old_press), body)
i2 = m2.group(1)
comment = (i2 + "// 仅当前可见窗口的职业按钮可经 SoftKey 触发；越界 idx 会误触功能按钮或越界读 class_index[]。\n")
body = body.replace(i2 + old_press, comment + i2 + old_press, 1)

out = body.replace("\n", "\r\n") if crlf else body
io.open(P, "w", encoding="utf-8", newline="").write(out)
print("WROTE_OK crlf=%s indent=%d cont=%d" % (crlf, len(indent), len(cont)))
