# -*- coding: utf-8 -*-
from __future__ import annotations

import os
from pathlib import Path

OUT = Path(r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\c4-btsort-gather")
IDB = r"E:\MXD\Maplestory95.i64"
os.environ.setdefault("IDAUSR", str(OUT / "_idausr"))
Path(os.environ["IDAUSR"]).mkdir(parents=True, exist_ok=True)

import idapro

print("open", IDB, flush=True)
print("open_rc", idapro.open_database(IDB, run_auto_analysis=False), flush=True)

import ida_funcs
import ida_hexrays
import ida_name
import idaapi
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


targets = {
    "OnGather": 0x7CBB40,
    "OnSort": 0x7CBB70,
    "Toggle": 0x7CBBA0,
    "SetArrangeButton": 0x7CCC90,
    "OnSortItemResult": 0x9F12B0,
    "OnGatherItemResult": 0x9F1280,
    "OnCreate": 0x7CE520,
}

for name, ea in targets.items():
    code = f"/* {demangle(ea)} @ {ea:#x} */\n\n" + decompile_ea(ea)
    (OUT / f"{name}.c").write_text(code, encoding="utf-8")
    print("saved", name, len(code), flush=True)

idapro.close_database(save=False)
print("done", flush=True)
