import io, sys
sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx
nx = Nx("E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx")
out = io.open(r"E:/project/_ms_probe/_whisp_node.txt", "w", encoding="utf-8")
# 1) 任何名字里含 Whisper / Target 的节点
def walk(i, path, depth):
    nm = nx.name(i)
    p = path + "/" + nm if path else nm
    _, fc, cnt, nt, _, w, h = nx.node(i)
    low = nm.lower()
    if "whisper" in low or "target" in low:
        out.write("HIT %s ntype=%d %dx%d children=%d\n" % (p, nt, w, h, cnt))
    if depth < 3:
        for c in nx.children(i):
            walk(c, p, depth + 1)
walk(0, "", 0)
# 2) UIWindow.img 根列表
out.write("\n=== UIWindow.img roots ===\n")
r = nx.find("UIWindow.img")
for c in nx.children(r):
    _, _, _, nt, _, w, h = nx.node(c)
    out.write("%s ntype=%d %dx%d\n" % (nx.name(c), nt, w, h))
out.close(); print("ok")
