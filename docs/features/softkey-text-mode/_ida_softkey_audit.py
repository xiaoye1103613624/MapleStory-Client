# -*- coding: utf-8 -*-
"""Headless audit: locate UI SoftKey construction/layout functions in BeiDou.exe and dump pseudocode."""
import idautils, idc, idaapi, ida_hexrays, ida_name, ida_funcs

OUT = r"E:\project\MapleStory-Client\docs\features\softkey-text-mode\evidence\ida_softkey_dump.txt"
lines = []

idaapi.auto_wait()

# 1) collect candidate strings
cand_eas = []
for s in idautils.Strings():
    t = str(s)
    if ("SoftKey" in t) or ("softkey" in t.lower()) or (t == "BtNum") or (t == "BtHighCase"):
        cand_eas.append((s.ea, t))
        lines.append("STR @%x : %s" % (s.ea, t))

# 2) xrefs -> owning functions
funcs = set()
for ea, t in cand_eas:
    for xr in idautils.XrefsTo(ea):
        f = ida_funcs.get_func(xr.frm)
        if f:
            funcs.add(f.start_ea)

lines.append("candidate funcs: %d" % len(funcs))

# 3) decompile each
for fea in sorted(funcs):
    name = ida_name.get_name(fea)
    lines.append("\n===== FUNC %s @ %x =====" % (name, fea))
    try:
        cf = ida_hexrays.decompile(fea)
        lines.append(str(cf))
    except Exception as e:
        lines.append("DECOMPILE_FAIL: %s" % e)
    if len(lines) > 60000:
        break

with open(OUT, "w", encoding="utf-8") as f:
    f.write("\n".join(lines))
print("WROTE", OUT)
qexit(0)
