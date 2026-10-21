from pathlib import Path
import numpy as np
from PIL import Image

img = Image.open(Path(r"E:\project\MapleStory-Client\docs\features\inventory-ui-ps\source\FullBackgrnd.png")).convert("RGBA")
arr = np.array(img)
h, w = arr.shape[:2]
print("size", w, h)

# Dump average color per row
for y in range(h):
    row = arr[y]
    op = row[row[:, 3] > 200]
    mean = op[:, :3].mean(axis=0) if len(op) else np.zeros(3)
    # variance across x for structure
    var = op[:, :3].var() if len(op) else 0
    print(f"{y:3d} mean={tuple(int(x) for x in mean)} var={var:.1f} a={int(row[:,3].mean())}")

print("--- sample pixels ---")
for y, x in [(10, 20), (42, 100), (50, 20), (80, 40), (250, 40), (270, 40), (275, 100), (280, 200)]:
    print(y, x, tuple(arr[y, x]))
