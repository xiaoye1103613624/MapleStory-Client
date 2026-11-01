import sys, io, struct, time
sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx

CACHE = "E:/MXD/BeiDou-Client-xy/Data/.nxcache/"
out = io.open(r"E:/project/_r23_mapfeat.txt", "w", encoding="utf-8")
m = Nx(CACHE + "Map002.nx")
t0 = time.time()

n = m.node_count
d = m.data
fc = [0] * n
cc = [0] * n
ni = [0] * n
for i in range(n):
    off = 64 + i * 20
    name_i, first, count = struct.unpack_from("<IIH", d, off)
    ni[i] = name_i
    fc[i] = first
    cc[i] = count


def kids(i):
    return range(fc[i], fc[i] + cc[i])


def nm(i):
    return m.strings[ni[i]]


root = None
for c in kids(0):
    if nm(c) == "Map":
        root = c
maps = []
for fo in kids(root):
    for mi in kids(fo):
        maps.append((nm(fo), nm(mi), mi))

# top-level keys per map img
top = {}
# keys under info
info = {}
# keys under a foothold leaf
fh_keys = {}
fh_seen = 0
for folder, name, mi in maps:
    for c in kids(mi):
        k = nm(c)
        top[k] = top.get(k, 0) + 1
        if k == "info":
            for ic in kids(c):
                ik = nm(ic)
                info[ik] = info.get(ik, 0) + 1
        elif k == "foothold" and fh_seen < 400:
            fh_seen += 1
            for lyr in kids(c):
                for chunk in kids(lyr):
                    for fe in kids(chunk):
                        for pc in kids(fe):
                            pk = nm(pc)
                            fh_keys[pk] = fh_keys.get(pk, 0) + 1

out.write("map imgs=%d elapsed=%.1fs\n\n" % (len(maps), time.time() - t0))
out.write("== map-img top-level keys (count of maps having it)\n")
for k, v in sorted(top.items(), key=lambda x: -x[1]):
    out.write("  %-14s %d\n" % (k, v))
out.write("\n== info.* keys\n")
for k, v in sorted(info.items(), key=lambda x: -x[1]):
    out.write("  %-16s %d\n" % (k, v))
out.write("\n== foothold leaf keys (sampled %d maps)\n" % fh_seen)
for k, v in sorted(fh_keys.items(), key=lambda x: -x[1]):
    out.write("  %-10s %d\n" % (k, v))
out.close()
print("done")
