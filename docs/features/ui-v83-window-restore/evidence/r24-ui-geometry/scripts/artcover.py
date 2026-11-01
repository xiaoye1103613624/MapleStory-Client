"""R24: how much of the "NPC/monster not showing up" class is a missing-art problem.

Cross the life inventory of every map img against the art roots that actually
ship in this pack, so the gap is a number instead of a guess.
"""
import sys, io, struct, time
sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx

CACHE = "E:/MXD/BeiDou-Client-xy/Data/.nxcache/"
OUT = r"E:/project/_r24_artcover.txt"

out = io.open(OUT, "w", encoding="utf-8")
t0 = time.time()


class Tab:
    """Flattened node table for one nx file (same 20-byte record layout)."""

    def __init__(self, path):
        self.nx = Nx(path)
        n = self.nx.node_count
        d = self.nx.data
        self.ni = [0] * n
        self.fc = [0] * n
        self.cc = [0] * n
        self.tv = [0] * n
        for i in range(n):
            off = 64 + i * 20
            name_i, first, count, ntype = struct.unpack_from("<IIHH", d, off)
            self.ni[i] = name_i
            self.fc[i] = first
            self.cc[i] = count
            self.tv[i] = ntype

    def close(self):
        del self.nx

    def nm(self, i):
        return self.nx.strings[self.ni[i]]

    def kids(self, i):
        return range(self.fc[i], self.fc[i] + self.cc[i])

    def child(self, i, name):
        for c in self.kids(i):
            if self.nm(c) == name:
                return c
        return None

    def text(self, i):
        # type 3 = string. nx.value() repr()s its output, so read the string table
        # slot straight off the node record (data field @ +12).
        if self.tv[i] != 3:
            return None
        data = struct.unpack_from("<I", self.nx.data, 64 + i * 20 + 12)[0]
        s = self.nx.strings
        return s[data] if data < len(s) else None

    def all_ids(self):
        """Every '<digits>.img' child name (art roots and map folders alike)."""
        ids = set()
        for i in range(1, self.nx.node_count):
            nm = self.nm(i)
            if nm.endswith(".img") and nm[:-4].isdigit():
                ids.add(nm[:-4])
        return ids


mobs = {}
npcs = {}
maps_with_life = 0
total_maps = 0

mp = Tab(CACHE + "Map002.nx")
maproot = mp.child(0, "Map")
for folder in mp.kids(maproot):
    for mi in mp.kids(folder):
        total_maps += 1
        life = mp.child(mi, "life")
        if life is None:
            continue
        maps_with_life += 1
        mapname = mp.nm(mi)
        for entry in mp.kids(life):
            ide = mp.child(entry, "id")
            ety = mp.child(entry, "type")
            if ide is None or ety is None:
                continue
            t = mp.text(ety)
            i = mp.text(ide)
            if not isinstance(t, str) or not isinstance(i, str):
                continue
            bucket = mobs if t == "m" else npcs if t == "n" else None
            if bucket is None:
                continue
            bucket.setdefault(i, set()).add(mapname)
mp.close()


def art_ids(path):
    tab = Tab(path)
    ids = set()
    # depth 1 only: Mob.nx / Npc.nx key their entries as <7-digit>.img off the root
    for c in tab.kids(0):
        nm = tab.nm(c)
        if nm.endswith(".img") and nm[:-4].isdigit():
            ids.add(nm[:-4])
    tab.close()
    return ids


mob_art = art_ids(CACHE + "Mob.nx")
npc_art = art_ids(CACHE + "Npc.nx")

out.write("map imgs=%d  with life=%d  elapsed=%.1fs\n\n"
          % (total_maps, maps_with_life, time.time() - t0))
out.write("art roots: Mob.nx=%d  Npc.nx=%d\n\n" % (len(mob_art), len(npc_art)))

for label, bucket, art in (("MOB", mobs, mob_art), ("NPC", npcs, npc_art)):
    missing = {k: v for k, v in bucket.items() if k not in art}
    maps_hit = set()
    for v in missing.values():
        maps_hit |= v
    out.write("== %s\n" % label)
    out.write("  distinct ids placed on maps : %d\n" % len(bucket))
    out.write("  ids with no art in this pack: %d (%.1f%%)\n"
              % (len(missing), 100.0 * len(missing) / max(1, len(bucket))))
    out.write("  maps containing >=1 such id : %d\n" % len(maps_hit))
    sample = sorted(missing.items(), key=lambda kv: -len(kv[1]))[:15]
    out.write("  top missing by map spread   : %s\n\n"
              % ", ".join("%s(%d)" % (k, len(v)) for k, v in sample))

out.close()
print("done")
