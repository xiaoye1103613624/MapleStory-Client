import io, sys
sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx
nx = Nx("E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx")
out = io.open(r"E:/project/_ms_probe/_notice4_tree2.txt", "w", encoding="utf-8")
def dump(root, label):
    out.write("=== %s ===\n" % label)
    def rec(i, depth=0):
        nm = nx.name(i)
        _, _, _, nt, _, w, h = nx.node(i)
        extra = ""
        if nt == 4:
            extra = " origin=" + str(nx.value(i))
        elif nt == 5:
            extra = " %dx%d" % (w, h)
        out.write("%s%s ntype=%d%s\n" % ("  " * depth, nm, nt, extra))
        for c in nx.children(i):
            rec(c, depth + 1)
    rec(root)
    out.write("\n")
for p in ("Basic.img/Notice4", "Basic.img/Notice3", "Basic.img/ComboBox", "Basic.img/BtComboBox"):
    dump(nx.find(p), p)
out.close(); print("ok")
