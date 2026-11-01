import sys, io
sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx, TYPE_NAMES

CACHE = "E:/MXD/BeiDou-Client-xy/Data/.nxcache/"
out = io.open(r"E:/project/_r23_life.txt", "w", encoding="utf-8")
m = Nx(CACHE + "Map002.nx")


def sv(i):
    n = m.node(i)
    if n[3] == 1:
        v = n[4]
        return v - 0x100000000 if v > 0x7FFFFFFF else v
    if n[3] == 3:
        return m.value(i)
    return n[3]


for mid in ("000000001", "000000002", "000000003"):
    p = "Map/Map0/%s.img" % mid
    out.write("\n===== MAP %s\n" % mid)
    fh = {}
    force = {}
    f = m.find(p + "/foothold")
    for lyr in m.children(f):
        for chunk in m.children(lyr):
            for e in m.children(chunk):
                props = {m.name(k): sv(k) for k in m.children(e)}
                props["layer"] = m.name(lyr)
                fh[m.name(e)] = props
                if "force" in props:
                    force[m.name(e)] = props["force"]
    life = m.find(p + "/life")
    if life is None:
        out.write("  no life\n")
        continue
    out.write("  %-10s %-8s %6s %6s %6s %5s %3s %5s %s\n" % ("idx", "type", "x", "y", "cy", "fh", "f", "mTime", "fhY(x)"))
    for e in m.children(life):
        props = {m.name(k): sv(k) for k in m.children(e)}
        fhid = str(props.get("fh", 0))
        fhp = fh.get(fhid)
        fhy = None
        if fhp:
            x = props.get("x", 0)
            if fhp["x1"] <= x <= fhp["x2"]:
                fhy = fhp["y1"] + (x - fhp["x1"]) * (fhp["y2"] - fhp["y1"]) / max(1, (fhp["x2"] - fhp["x1"]))
        out.write("  %-10s %-8s %6s %6s %6s %5s %3s %5s %s  fhProps=%s\n" % (
            m.name(e), repr(props.get("type")), props.get("x"), props.get("y"),
            props.get("cy"), fhid, props.get("f"), props.get("mobTime"),
            ("%.1f" % fhy) if fhy is not None else "-",
            ("L%s x[%s..%s] y[%s..%s]%s" % (fhp["layer"], fhp["x1"], fhp["x2"], fhp["y1"], fhp["y2"],
                                            (" FORCE=%s" % force[fhid]) if fhid in force else "")) if fhp else "NO-FH"))
out.close()
print("done")
