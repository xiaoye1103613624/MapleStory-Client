# IDA 9.4 / IDAPython inventory dump for ijl15.dll
# Outputs: functions, strings, string->xref map, entries, segments
import json
import idc
import ida_funcs
import idautils
import ida_segment
import ida_entry

OUT = r"C:\Users\11036\AppData\Local\Temp\opencode\ida_ijl15_inventory.json"

funcs = []
for ea in idautils.Functions():
    f = ida_funcs.get_func(ea)
    if f is None:
        continue
    funcs.append({
        "ea": ea,
        "end": f.end_ea,
        "name": ida_funcs.get_func_name(ea),
    })

strings = []
xrefs = []
for s in idautils.Strings():
    try:
        val = str(s)
    except Exception:
        continue
    strings.append({"ea": s.ea, "val": val})
    for x in idautils.XrefsTo(s.ea, 0):
        f = ida_funcs.get_func(x.frm)
        xrefs.append({
            "str_ea": s.ea,
            "from": x.frm,
            "func": (f.start_ea if f else 0),
            "type": x.type,
        })

segs = []
for s in idautils.Segments():
    sg = ida_segment.getseg(s)
    segs.append({"name": ida_segment.get_segm_name(sg), "start": sg.start_ea, "end": sg.end_ea})

ents = []
for i in range(ida_entry.get_entry_qty()):
    ord_ = ida_entry.get_entry_ordinal(i)
    ea = ida_entry.get_entry(ord_)
    name = ida_entry.get_entry_name(ord_)
    ents.append({"ord": ord_, "ea": ea, "name": name})

data = {
    "file": "ijl15.dll",
    "function_count": len(funcs),
    "string_count": len(strings),
    "segments": segs,
    "entries": ents,
    "functions": funcs,
    "strings": strings,
    "string_xrefs": xrefs,
}

with open(OUT, "w", encoding="utf-8") as fh:
    json.dump(data, fh, ensure_ascii=False)

print("[dump] functions=%d strings=%d xrefs=%d -> %s" % (len(funcs), len(strings), len(xrefs), OUT))
idc.qexit(0)
