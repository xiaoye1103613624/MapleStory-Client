"""W10 probe: saturated-colour clusters in the reference quest screenshot title rows."""
import os
from collections import defaultdict

from PIL import Image

BASE = os.path.dirname(os.path.abspath(__file__))
EV = os.path.abspath(os.path.join(BASE, "..", "docs", "features", "ui-v83-window-restore",
                                  "evidence", "w10-quest"))
sh = Image.open(r"C:\Users\11036\Desktop\正确客户端截图\任务栏.png").convert("RGBA")
px = sh.load()
out = open(os.path.join(EV, "_probe_title.txt"), "w", encoding="utf-8")

regions = {"left-title": (190, 0, 245, 25), "right-title": (500, 0, 548, 25),
           "left-tabrow": (0, 18, 245, 45), "right-banner": (243, 18, 548, 125)}

for name, (x0, y0, x1, y1) in regions.items():
    groups = defaultdict(list)
    for y in range(y0, y1):
        for x in range(x0, x1):
            r, g, b = px[x, y][:3]
            if max(r, g, b) - min(r, g, b) > 40:  # saturated
                groups[(r // 34 * 34, g // 34 * 34, b // 34 * 34)].append((x, y))
    print("== %s" % name, file=out)
    for c, pts in sorted(groups.items(), key=lambda kv: -len(kv[1]))[:8]:
        xs = [p[0] for p in pts]
        ys = [p[1] for p in pts]
        print("   %s n=%d  x %d..%d  y %d..%d" % (c, len(pts), min(xs), max(xs), min(ys), max(ys)), file=out)

# tab box edges: black vertical lines within the tab band
print("\n== black columns y 24..40", file=out)
cols = [x for x in range(0, 245) if all(sum(px[x, y][:3]) < 120 for y in range(26, 40))]
print("   ", cols, file=out)
print("== black rows x 6..60 (tab band vertical extent)", file=out)
rows = [y for y in range(15, 60) if all(sum(px[x, y][:3]) < 150 for x in range(10, 60))]
print("   ", rows, file=out)
out.close()
print("done")
