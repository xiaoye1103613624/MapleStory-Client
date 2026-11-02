import io, struct, sys
sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx

out = io.open(r"E:/project/_ms_probe/_utildlgex_tree.txt", "w", encoding="utf-8")
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


def node(i):
    return nx.node(i)


def vec(i):
    _, _, _, ntype, data, w, h = node(i)
    if ntype != 4:
        return None
    off = 64 + i * 20
    return struct.unpack_from("<ii", nx.data, off + 12)


def info(i):
    _, _, _, ntype, data, w, h = node(i)
    o = child(i, "origin")
    return "type=%d %dx%d origin=%s" % (ntype, w, h, vec(o) if o is not None else None)


ui = child(0, "UIWindow.img")
out.write("== UIWindow.img roots ==\n")
for c in kids(ui):
    out.write("  %s\n" % nm(c))

wdl = child(ui, "UtilDlgEx")
out.write("\n== UtilDlgEx (%s) ==\n" % info(wdl))
for c in kids(wdl):
    n = nm(c)
    out.write("%-14s %s\n" % (n, info(c)))
    for st in kids(c):
        out.write("   %-12s %s\n" % (nm(st), info(st)))
        for fr in kids(st):
            out.write("      %-10s %s\n" % (nm(fr), info(fr)))

# 同族窗框：UtilDlg / UtilDlg2 ... 找有没有 261 宽这一档
out.write("\n== 其它 UtilDlg* 家族 ==\n")
for c in kids(ui):
    if nm(c).startswith("UtilDlg"):
        out.write("%s\n" % nm(c))
        for k in kids(c):
            out.write("   %-10s %s\n" % (nm(k), info(k)))

# ComboBox（下拉）在 Basic.img 里的件
basic = child(0, "Basic.img")
if basic is not None:
    out.write("\n== Basic.img 里 Combo / combo 相关根 ==\n")
    for c in kids(basic):
        if "ombo" in nm(c) or "combo" in nm(c):
            out.write("%-14s %s\n" % (nm(c), info(c)))
            for st in kids(c):
                out.write("   %-12s %s\n" % (nm(st), info(st)))
out.close()
print("done")
