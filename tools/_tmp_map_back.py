#!/usr/bin/env python3
import sys
sys.path.insert(0, r"E:\project\MapleStory-Client\tools")
from nx_inspect import Nx

m = Nx(r"E:\MXD\BeiDou-Client-xy\Data\.nxcache\Map002.nx")
m1 = Nx(r"E:\MXD\BeiDou-Client-xy\Data\.nxcache\Map001.nx")


def clean(v):
    if v is None:
        return ""
    s = str(v)
    if len(s) >= 2 and s[0] == "'" and s[-1] == "'":
        return s[1:-1]
    return s


for mid in ["000000001", "000000002", "000000003", "000000004", "000000000"]:
    print("====", mid)
    back = m.find("Map/Map0/%s.img/back" % mid)
    if not back:
        print(" no back")
        continue
    for c in m.children(back):
        name = m.name(c)
        base = "Map/Map0/%s.img/back/%s/" % (mid, name)

        def v(k):
            n = m.find(base + k)
            return None if n is None else m.value(n)

        bs = clean(v("bS"))
        no = clean(v("no")) or "0"
        typ = clean(v("type")) or "0"
        ani = clean(v("ani")) or "0"
        cx = clean(v("cx"))
        cy = clean(v("cy"))
        ani_s = "ani" if ani not in ("0", "") else "back"
        path = "Back/%s.img/%s/%s" % (bs, ani_s, no) if bs else "(empty bS)"
        bn = m1.find(path) if bs else None
        dim = m1.value(bn) if bn is not None else "MISSING"
        # estimate tiles at 1280x720 if type tiled-ish
        t = int(typ) if typ.isdigit() else -1
        print(
            "  [%s] bS=%r no=%s type=%s cx=%s cy=%s -> %s | %s"
            % (name, bs, no, typ, cx, cy, path, dim)
        )
        if t in (1, 2, 3, 4, 5, 6, 7) and isinstance(dim, str) and dim.startswith("bitmap"):
            # parse WxH
            wh = dim.split()[1]
            w, h = [int(x) for x in wh.split("x")]
            use_cx = int(cx) if cx and cx != "0" else (w if w > 0 else 1)
            use_cy = int(cy) if cy and cy != "0" else (h if h > 0 else 1)
            htile = 1280 // use_cx + 3 if t in (1, 3, 4, 6) else 1
            vtile = 720 // use_cy + 3 if t in (2, 3, 5, 7) else 1
            if t in (1, 4):  # HTILED HMOVEA
                htile = 1280 // use_cx + 3
                vtile = 1
            elif t in (2, 5):
                htile = 1
                vtile = 720 // use_cy + 3
            elif t in (3, 6, 7):
                htile = 1280 // use_cx + 3
                vtile = 720 // use_cy + 3
            draws = htile * vtile
            print("       tiles=%dx%d draws=%d cx=%d cy=%d" % (htile, vtile, draws, use_cx, use_cy))
        elif t in (1, 2, 3, 4, 5, 6, 7) and dim == "MISSING":
            # missing bitmap -> cx=cy=1 explosion
            htile = 1280 // 1 + 3
            vtile = 720 // 1 + 3
            print("       MISSING BITMAP -> potential explosion draws~%d" % (htile * vtile))
