"""B5-2 复核：全 UI.nx 递归找「~261 宽」的位图窗框件，并列出 UIWindow.img 下的对话框族。

§5 的旧扫描限 depth<=3，怕漏掉更深层；这里去掉深度限制。
注意：nx_inspect 里位图是 ntype==5（不是 canvas==2），origin 是 ntype==4 的
「+12=x、+16=y」两个 int32，直接用 nx.value() 取，别再手算偏移。
"""
import io
import sys

sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx

OUT = io.open(r"E:/project/_ms_probe/_utildlg_widen.txt", "w", encoding="utf-8")
nx = Nx("E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx")

W_MIN, W_MAX = 200, 340


def nm(i):
    return nx.name(i)


def kids(i):
    return list(nx.children(i))


def origin_of(i):
    for c in kids(i):
        if nm(c) == "origin":
            return nx.value(c)
    return None


def bitmap_of(i):
    _, _, _, ntype, _, w, h = nx.node(i)
    return (w, h) if ntype == 5 else None


def walk(i, path):
    for c in kids(i):
        cn = nm(c)
        if cn == "origin":
            continue
        p = path + "/" + cn
        bm = bitmap_of(c)
        if bm and W_MIN <= bm[0] <= W_MAX:
            OUT.write("BITMAP %-58s %dx%d origin=%s\n" % (p, bm[0], bm[1], origin_of(c)))
        walk(c, p)


ui = nx.find("UIWindow.img")
OUT.write("== UIWindow.img 下的 UtilDlg* 家族 ==\n")
for c in kids(ui):
    if nm(c).startswith("UtilDlg") or nm(c) in ("FadeYesNo",):
        OUT.write("%s\n" % nm(c))
        for k in kids(c):
            OUT.write("   %-12s %s\n" % (nm(k), nx.value(k) if bitmap_of(k) else "(dir)"))
            for st in kids(k):
                bm = bitmap_of(st)
                OUT.write("      %-10s %s origin=%s\n" % (
                    nm(st), "%dx%d" % bm if bm else nx.value(st), origin_of(st)))

OUT.write("\n== 全树（不限深度）宽 %d..%d 的位图 ==\n" % (W_MIN, W_MAX))
walk(0, "")
OUT.close()
print("written")
