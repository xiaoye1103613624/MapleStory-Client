import sys, io, time
sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx

CACHE = "E:/MXD/BeiDou-Client-xy/Data/.nxcache/"
out = io.open(r"E:/project/_r23_force_survey.txt", "w", encoding="utf-8")
t0 = time.time()

for fname in ("Map002.nx",):
    m = Nx(CACHE + fname)
    node_name = [None] * m.node_count
    fc = [0] * m.node_count      # first child
    cc = [0] * m.node_count      # child count
    d = m.data
    import struct
    for i in range(m.node_count):
        off = 64 + i * 20
        name_i, first, count = struct.unpack_from("<IIH", d, off)
        node_name[i] = name_i
        fc[i] = first
        cc[i] = count

    def kids(i):
        return range(fc[i], fc[i] + cc[i])

    def find(nd, nm):
        for c in kids(nd):
            if m.strings[node_name[c]] == nm:
                return c
        return None

    maps = []
    root = find(0, "Map")
    for fo in kids(root):
        for mi in kids(fo):
            maps.append((m.strings[node_name[fo]], m.strings[node_name[mi]], mi))
    out.write("map imgs=%d\n" % len(maps))

    total_maps_with = 0
    total_fh = 0
    rows = []
    for folder, nm, mi in maps:
        fh = find(mi, "foothold")
        if fh is None:
            continue
        cnt = 0
        for lyr in kids(fh):
            for chunk in kids(lyr):
                for fe in kids(chunk):
                    f = find(fe, "force")
                    if f is not None:
                        cnt += 1
        if cnt:
            total_maps_with += 1
            total_fh += cnt
            rows.append("%s/%s force_footholds=%d" % (folder, nm, cnt))
    out.write("maps with conveyor footholds=%d  total conveyor footholds=%d\n" % (total_maps_with, total_fh))
    for r in rows:
        out.write("  %s\n" % r)
    out.write("elapsed=%.1fs\n" % (time.time() - t0))
out.close()
print("done")
