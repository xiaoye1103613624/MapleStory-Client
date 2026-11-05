# -*- coding: utf-8 -*-
"""Headless idalib: Maplestory95 OnGather/OnSort + CUIItem button click."""
from __future__ import annotations

import json
import os
import sys
from pathlib import Path

OUT = Path(r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\c4-btsort-gather")
IDB = r"E:\MXD\Maplestory95.i64"
OUT.mkdir(parents=True, exist_ok=True)

os.environ.setdefault("IDAUSR", str(OUT / "_idausr"))
Path(os.environ["IDAUSR"]).mkdir(parents=True, exist_ok=True)

import idapro

print("open", IDB, flush=True)
rc = idapro.open_database(IDB, run_auto_analysis=False)
print("open_rc", rc, flush=True)

import ida_funcs
import ida_hexrays
import ida_name
import ida_xref
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
        return f"/* no func at {ea:#x} */\n"
    try:
        cfunc = ida_hexrays.decompile(f.start_ea)
    except Exception as e:
        return f"/* decompile failed {ea:#x}: {e} */\n"
    if not cfunc:
        return f"/* null cfunc {ea:#x} */\n"
    return str(cfunc) + "\n"


def save(name: str, text: str) -> None:
    p = OUT / name
    p.write_text(text, encoding="utf-8")
    print("saved", p, "chars", len(text), flush=True)


# Resolve named symbols
want = [
    "?OnGatherItemResult@CWvsContext@@IAEXAAVCInPacket@@@Z",
    "?OnSortItemResult@CWvsContext@@IAEXAAVCInPacket@@@Z",
    "?SendGatherItemRequest@CWvsContext@@QAEXJ@Z",
    "?SendSortItemRequest@CWvsContext@@QAEXJ@Z",
    "?OnButtonClicked@CUIItem@@UAEXI@Z",
    "?OnCreate@CUIItem@@UAEXPAX@Z",
]

resolved = {}
for mangled in want:
    ea = ida_name.get_name_ea(idaapi.BADADDR, mangled)
    resolved[mangled] = ea
    print("sym", mangled, hex(ea) if ea != idaapi.BADADDR else "MISSING", flush=True)

# Fallback: search by demangled substring
if any(v == idaapi.BADADDR for v in resolved.values()):
    for ea in idautils.Functions():
        d = demangle(ea)
        if not d:
            continue
        if "OnGatherItemResult" in d and "CWvsContext" in d:
            resolved["OnGatherItemResult"] = ea
        if "OnSortItemResult" in d and "CWvsContext" in d:
            resolved["OnSortItemResult"] = ea
        if "SendGatherItemRequest" in d and "CWvsContext" in d:
            resolved["SendGatherItemRequest"] = ea
        if "SendSortItemRequest" in d and "CWvsContext" in d:
            resolved["SendSortItemRequest"] = ea
        if "OnButtonClicked" in d and "CUIItem" in d and "CUIItemMaker" not in d and "CUIItemUpgrade" not in d:
            resolved["CUIItem::OnButtonClicked"] = ea

save("_resolved.json", json.dumps({k: (hex(v) if isinstance(v, int) and v != idaapi.BADADDR else str(v)) for k, v in resolved.items()}, indent=2))

# Decompile core handlers
for key, mangled in [
    ("OnGatherItemResult", "?OnGatherItemResult@CWvsContext@@IAEXAAVCInPacket@@@Z"),
    ("OnSortItemResult", "?OnSortItemResult@CWvsContext@@IAEXAAVCInPacket@@@Z"),
    ("SendGatherItemRequest", "?SendGatherItemRequest@CWvsContext@@QAEXJ@Z"),
    ("SendSortItemRequest", "?SendSortItemRequest@CWvsContext@@QAEXJ@Z"),
    ("CUIItem_OnButtonClicked", "?OnButtonClicked@CUIItem@@UAEXI@Z"),
]:
    ea = resolved.get(mangled, idaapi.BADADDR)
    if ea == idaapi.BADADDR:
        ea = resolved.get(key.replace("CUIItem_OnButtonClicked", "CUIItem::OnButtonClicked"), idaapi.BADADDR)
    if ea == idaapi.BADADDR:
        # try alternate keys from demangle scan
        for k, v in resolved.items():
            if key.split("_")[-1] in k or key in k:
                ea = v
                break
    if ea == idaapi.BADADDR:
        save(f"{key}.c", f"/* MISSING {key} */\n")
        continue
    code = f"/* {demangle(ea)} @ {ea:#x} */\n\n" + decompile_ea(ea)
    save(f"{key}.c", code)

# Xrefs TO SendGather / SendSort → callers (expect CUIItem::OnButtonClicked)
for label, mangled in [
    ("xref_SendGather", "?SendGatherItemRequest@CWvsContext@@QAEXJ@Z"),
    ("xref_SendSort", "?SendSortItemRequest@CWvsContext@@QAEXJ@Z"),
]:
    ea = resolved.get(mangled, idaapi.BADADDR)
    if ea == idaapi.BADADDR:
        continue
    lines = [f"/* xrefs to {demangle(ea)} @ {ea:#x} */\n"]
    for xref in idautils.XrefsTo(ea):
        f = ida_funcs.get_func(xref.frm)
        fea = f.start_ea if f else xref.frm
        lines.append(f"{xref.frm:#x} from {demangle(fea)} ({fea:#x}) type={xref.type}\n")
    save(f"{label}.txt", "".join(lines))

# Also try CUIItem related names containing Gather/Sort/Button
hits = []
for ea in idautils.Functions():
    d = demangle(ea)
    if not d:
        continue
    if "CUIItem" in d and any(k in d for k in ("Gather", "Sort", "Button", "Toggle", "Enable", "SetButton")):
        if "CUIItemMaker" in d or "CUIItemUpgrade" in d:
            continue
        hits.append((ea, d))
save("_cuitem_related.txt", "\n".join(f"{ea:#x}\t{d}" for ea, d in sorted(hits)[:200]) + "\n")

idapro.close_database(save=False)
print("done", flush=True)
