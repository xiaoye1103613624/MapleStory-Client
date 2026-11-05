# -*- coding: utf-8 -*-
"""Find tab-change / SetArrangeButton callers for per-TI state restore."""
from __future__ import annotations

import os
from pathlib import Path

OUT = Path(r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\c4-btsort-gather")
IDB = r"E:\MXD\Maplestory95.i64"
os.environ.setdefault("IDAUSR", str(OUT / "_idausr"))
Path(os.environ["IDAUSR"]).mkdir(parents=True, exist_ok=True)

import idapro

print("open_rc", idapro.open_database(IDB, run_auto_analysis=False), flush=True)

import ida_funcs
import ida_hexrays
import ida_name
import idaapi
import idautils
import idc


def demangle(ea: int) -> str:
    n = ida_name.get_ea_name(ea) or ""
    d = idc.demangle_name(n, idc.get_inf_attr(idc.INF_SHORT_DN)) if n else None
    return d or n


def decompile_ea(ea: int) -> str:
    f = ida_funcs.get_func(ea)
    if not f:
        return f"/* no func */\n"
    try:
        cfunc = ida_hexrays.decompile(f.start_ea)
    except Exception as e:
        return f"/* fail {e} */\n"
    return (str(cfunc) if cfunc else "/* null */") + "\n"


set_arr = 0x7CCC90
lines = [f"/* xrefs to SetArrangeButton @ {set_arr:#x} */\n"]
callers = set()
for xref in idautils.XrefsTo(set_arr):
    f = ida_funcs.get_func(xref.frm)
    fea = f.start_ea if f else xref.frm
    callers.add(fea)
    lines.append(f"{xref.frm:#x} <- {demangle(fea)} ({fea:#x})\n")
(OUT / "xref_SetArrangeButton.txt").write_text("".join(lines), encoding="utf-8")
print("xrefs", len(lines) - 1, flush=True)

# Decompile any callers that aren't already saved
known = {0x7CCC90, 0x9F1280, 0x9F12B0, 0x7CE520}
for fea in sorted(callers):
    if fea in known:
        continue
    name = demangle(fea).replace("::", "_").replace("(", "_").replace(")", "").replace(" ", "")
    name = "".join(c if c.isalnum() or c in "_-" else "_" for c in name)[:80]
    code = f"/* {demangle(fea)} @ {fea:#x} */\n\n" + decompile_ea(fea)
    (OUT / f"caller_{fea:x}_{name}.c").write_text(code, encoding="utf-8")
    print("saved caller", hex(fea), demangle(fea)[:80], flush=True)

# Resolve aUi_435 / aUi_159 string contents near SetArrangeButton
import ida_bytes
import ida_ua

# From SetArrangeButton refs: look for string pointers used as AddButton path args
# Search strings BtGather / BtSort under UIWindow
hits = []
for s in idautils.Strings():
    try:
        t = str(s)
    except Exception:
        continue
    if "BtGather" in t or "BtSort" in t or "Item/Bt" in t:
        hits.append((s.ea, t))
(OUT / "_bt_strings.txt").write_text("\n".join(f"{ea:#x}\t{t}" for ea, t in hits[:80]) + "\n", encoding="utf-8")
print("bt_strings", len(hits), flush=True)

idapro.close_database(save=False)
print("done", flush=True)
