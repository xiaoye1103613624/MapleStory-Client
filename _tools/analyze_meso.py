from pathlib import Path
import numpy as np
from PIL import Image

arr = np.array(Image.open(Path(r"E:\project\MapleStory-Client\docs\features\inventory-ui-ps\source\FullBackgrnd.png")).convert("RGBA"))
h, w = arr.shape[:2]
for y in range(257, h):
    row = arr[y]
    white = (row[:, 0] > 250) & (row[:, 1] > 250) & (row[:, 2] > 250) & (row[:, 3] > 200)
    xs = np.where(white)[0]
    if len(xs):
        # contiguous runs
        runs, s = [], None
        for i in range(w):
            if white[i] and s is None:
                s = i
            elif not white[i] and s is not None:
                runs.append((s, i))
                s = None
        if s is not None:
            runs.append((s, w))
        print(f"y={y} runs={runs}")
