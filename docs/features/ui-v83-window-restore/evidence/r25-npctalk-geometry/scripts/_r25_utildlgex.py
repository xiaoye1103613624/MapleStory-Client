import io, struct, sys
sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx

out = io.open(r"E:/project/_r25_utildlgex.txt", "w", encoding="utf-8")
nx = Nx("E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx")


def nm(i):
    return nx.name(i)


def kids(i):
    return list(nx.children(i))


def child(i, name):
    for c in kids(i):
        if nm(c) == name:
            return c
    return None


def vec(i):
    _, _, _, ntype, data, w, h = nx.node(i)
    if ntype != 4:
        return None
    off = 64 + i * 20
    return struct.unpack_from("<ii", nx.data, off + 12)


def info(i):
    _, _, _, ntype, data, w, h = nx.node(i)
    o = child(i, "origin")
    return "type=%d %dx%d origin=%s" % (ntype, w, h, vec(o) if o is not None else None)


wdl = child(child(0, "UIWindow.img"), "UtilDlgEx")
for c in kids(wdl):
    n = nm(c)
    if not n.startswith("Bt"):
        continue
    out.write("%s\n" % n)
    for st in kids(c):
        for fr in kids(st):
            out.write("   %-12s %-6s %s\n" % (n, nm(st), info(fr)))
out.close()
print("done")
