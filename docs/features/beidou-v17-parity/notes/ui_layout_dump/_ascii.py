import io
import sys
import numpy as np
from PIL import Image

path = sys.argv[1]
cols = int(sys.argv[2]) if len(sys.argv) > 2 else 100
rows = int(sys.argv[3]) if len(sys.argv) > 3 else 46

A = np.asarray(Image.open(path).convert("RGB")).astype(np.float32)
H, W = A.shape[:2]
lum = A.mean(axis=2)

ch, cw = H / float(rows), W / float(cols)
chars = " .:-=+*#%@"
lines = []
for r in range(rows):
    y0, y1 = int(r * ch), max(int(r * ch) + 1, int((r + 1) * ch))
    line = []
    for c in range(cols):
        x0, x1 = int(c * cw), max(int(c * cw) + 1, int((c + 1) * cw))
        v = lum[y0:y1, x0:x1].mean()
        line.append(chars[min(9, int(v / 25.6))])
    lines.append("%3d|%s" % (int(r * ch), "".join(line)))

with io.open(path + ".txt", "w", encoding="utf-8") as f:
    f.write("\n".join(lines))
print("wrote", path + ".txt")
