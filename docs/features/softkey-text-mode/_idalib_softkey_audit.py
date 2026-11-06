# -*- coding: utf-8 -*-
"""idalib binary pattern search: SoftKey node-name refs in BeiDou.exe V17.7 (083 family)."""
import idapro
import idaapi, ida_bytes, idautils, idc, ida_funcs, ida_hexrays, ida_name, ida_segment

OUT = r"E:\project\MapleStory-Client\docs\features\softkey-text-mode\evidence\ida_softkey_dump.txt"
DBS = [
    r"E:\MXD\BeiDou-ClientV17.7\_audit_copy.i64",
    r"E:\MXD\BeiDou-ClientV17.7\suipre.dll.i64",
]
PATS = [b"S\0o\0f\0t\0K\0e\0y\0", b"SoftKey", b"B\0t\0N\0u\0m\0", b"K\0e\0y\0W\0i\0n\0d\0o\0w\0"]

log = []
for db in DBS:
    idapro.open_database(db, False)
    idaapi.auto_wait()

    segs = []
    for sea in idautils.Segments():
        sg = ida_segment.getseg(sea)
        if sg:
            segs.append((sg.start_ea, sg.end_ea, str(sg.name)))
    log.append("\n##### %s segs=%s #####" % (db, [(n, hex(a), hex(b)) for a, b, n in segs]))

    def bsearch_all(pat, _segs=segs):
        hits = []
        for a, b, n in _segs:
            size = b - a
            if size <= 0 or size > 64 * 1024 * 1024:
                continue
            data = ida_bytes.get_bytes(a, size) or b""
            pos = data.find(pat)
            while pos != -1 and len(hits) < 30:
                hits.append(a + pos)
                pos = data.find(pat, pos + 1)
        return hits

    funcs = {}
    for pat in PATS:
        hits = bsearch_all(pat)
        log.append("pat %r hits=%d %s" % (pat, len(hits), [hex(h) for h in hits[:12]]))
        for h in hits:
            for xr in idautils.XrefsTo(h):
                f = ida_funcs.get_func(xr.frm)
                if f:
                    funcs[f.start_ea] = funcs.get(f.start_ea, 0) + 1
    log.append("code funcs referencing patterns: %d" % len(funcs))
    for fea in sorted(funcs)[:6]:
        name = ida_name.get_name(fea)
        log.append("\n===== FUNC %s @ %x (refs=%d) =====" % (name, fea, funcs[fea]))
        try:
            log.append(str(ida_hexrays.decompile(fea))[:60000])
        except Exception as e:
            log.append("DECOMPILE_FAIL %s" % e)
    try:
        idapro.close_database(False)
    except Exception:
        pass

with open(OUT, "w", encoding="utf-8") as f:
    f.write("\n".join(log))
print("WROTE", OUT)
