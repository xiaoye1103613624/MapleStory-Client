"""W10 reference-screenshot crop helper (read-only)."""
import os
from PIL import Image

BASE = os.path.dirname(os.path.abspath(__file__))
PNG = os.path.abspath(os.path.join(BASE, "..", "docs", "features", "ui-v83-window-restore",
                                   "evidence", "w10-quest", "pngs"))
sh = Image.open(r"C:\Users\11036\Desktop\正确客户端截图\任务栏.png").convert("RGBA")


def crop(name, box, scale=4):
    im = sh.crop(box)
    im = im.resize((im.width * scale, im.height * scale), Image.NEAREST)
    im.save(os.path.join(PNG, name))
    print("wrote", name, box, im.size)


crop("ref-a-tabrow.png", (0, 0, 245, 60), 4)
crop("ref-b-listhead.png", (0, 45, 245, 150), 4)
crop("ref-c-bottomleft.png", (0, 330, 245, 396), 4)
crop("ref-d-bottomright.png", (243, 330, 548, 396), 4)
crop("ref-e-topright.png", (180, 0, 548, 40), 4)
crop("ref-f-detailhead.png", (243, 0, 548, 130), 3)
