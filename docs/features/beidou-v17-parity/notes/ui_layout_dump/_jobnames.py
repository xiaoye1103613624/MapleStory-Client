import io
import os
import sys

sys.path.insert(0, os.path.abspath(r"e:/project/MapleStory-Client/tools"))
import nx_inspect  # noqa: E402

nx = nx_inspect.Nx(r"E:\MXD\BeiDou-Client-xy\Data\.nxcache\String.nx")
out = []

root = nx.find("") if hasattr(nx, "find") else None
out.append("root=%r" % (root,))

# 列出顶层
top = []
for child in nx.children(nx.find("String") if nx.find("String") is not None else 0):
    top.append(nx.name(child))
out.append("children of 'String': %s" % top[:60])

jobimg = nx.find("String/Job.img")
out.append("Job.img=%r" % (jobimg,))

if jobimg is not None:
    for c in nx.children(jobimg):
        cid = nx.name(c)
        nm = c
        for g in nx.children(c):
            if nx.name(g) == "name":
                nm = g
        out.append("job %s -> %r" % (cid, nx.value(nm) if nx.node(nm)[3] == 3 else nx.node(nm)[3]))

with io.open(r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/ui_layout_dump/_jobnames.txt", "w", encoding="utf-8") as f:
    f.write("\n".join(out))
print("ok")
