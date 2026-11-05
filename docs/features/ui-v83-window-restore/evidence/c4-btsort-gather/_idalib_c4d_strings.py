# -*- coding: utf-8 -*-
"""Resolve aUi_435 / aUi_159 string paths used by SetArrangeButton."""
from __future__ import annotations

import os
from pathlib import Path

OUT = Path(r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\c4-btsort-gather")
IDB = r"E:\MXD\Maplestory95.i64"
os.environ.setdefault("IDAUSR", str(OUT / "_idausr"))
Path(os.environ["IDAUSR"]).mkdir(parents=True, exist_ok=True)

import idapro

print("open_rc", idapro.open_database(IDB, run_auto_analysis=False), flush=True)

import ida_bytes
import ida_name
import ida_ua
import idautils
import idc

# From SetArrangeButton @ 0x7ccc90 — dump immediates / data refs in the function
ea = 0x7CCC90
end = idc.find_func_end(ea)
lines = [f"/* SetArrangeButton disasm string refs {ea:#x}-{end:#x} */\n"]
for head in idautils.Heads(ea, end):
    for xref in idautils.DataRefsFrom(head):
        s = idc.get_strlit_contents(xref)
        if s:
            try:
                t = s.decode("utf-8", errors="replace")
            except Exception:
                t = repr(s)
            lines.append(f"{head:#x} -> {xref:#x} '{t}'\n")
        else:
            name = ida_name.get_ea_name(xref) or ""
            if name:
                lines.append(f"{head:#x} -> {xref:#x} name={name}\n")

(OUT / "SetArrangeButton_strrefs.txt").write_text("".join(lines), encoding="utf-8")
print("".join(lines), flush=True)

# Also resolve aUi_435 / aUi_159 by name
for n in ("aUi_435", "aUi_159", "aUiWindow_img_Item_BtGather", "aBtGather"):
    ea2 = ida_name.get_name_ea(idc.BADADDR, n)
    if ea2 != idc.BADADDR:
        s = idc.get_strlit_contents(ea2)
        print(n, hex(ea2), s, flush=True)

# brute: names containing BtGather/BtSort
for ea3, name in idautils.Names():
    if "BtGather" in name or "BtSort" in name or name.startswith("aUi_") and ("Gather" in (idc.get_strlit_contents(ea3) or b"").decode("latin1", "ignore") or "Sort" in (idc.get_strlit_contents(ea3) or b"").decode("latin1", "ignore")):
        s = idc.get_strlit_contents(ea3)
        print("NAME", name, hex(ea3), s, flush=True)

idapro.close_database(save=False)
print("done", flush=True)
