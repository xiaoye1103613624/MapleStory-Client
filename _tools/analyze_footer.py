from pathlib import Path
import numpy as np
from PIL import Image

arr = np.array(Image.open(Path(r"E:\project\MapleStory-Client\docs\features\inventory-ui-ps\source\FullBackgrnd.png")).convert("RGBA"))
# footer rows 266-281
for y in [268, 272, 275, 278]:
    row = arr[y]
    # classify roughly
    labels = []
    x = 0
    while x < 603:
        p = row[x]
        if p[3] < 200:
            kind = "a"
        elif p[0] > 250 and p[1] > 250 and p[2] > 250:
            kind = "W"
        elif p[0] > 180 and p[1] > 140 and p[2] < 80:
            kind = "G"  # green-ish button
        elif p[0] > 200 and p[1] > 160 and p[2] < 100:
            kind = "g"
        elif abs(int(p[0]) - int(p[1])) < 20 and p[2] > p[0]:
            kind = "B"  # blue chrome
        else:
            kind = "."
        start = x
        while x < 603:
            p = row[x]
            if p[3] < 200:
                k = "a"
            elif p[0] > 250 and p[1] > 250 and p[2] > 250:
                k = "W"
            elif p[0] > 160 and p[1] > 180 and p[2] < 120 and p[1] > p[0]:
                k = "G"
            elif abs(int(p[0]) - int(p[1])) < 25 and p[2] >= p[0] - 5 and p[0] < 230:
                k = "B"
            else:
                k = "."
            if k != kind:
                break
            x += 1
        labels.append(f"{kind}:{start}-{x}")
    print(f"y={y}", " ".join(labels))

# Find green button bbox
gmask = (arr[257:, :, 1] > arr[257:, :, 0] + 20) & (arr[257:, :, 1] > 150) & (arr[257:, :, 2] < 150)
ys, xs = np.where(gmask)
if len(xs):
    print("green bbox", xs.min(), xs.max(), ys.min() + 257, ys.max() + 257)
