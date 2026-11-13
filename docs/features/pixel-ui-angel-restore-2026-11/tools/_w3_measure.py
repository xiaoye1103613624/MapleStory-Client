# Wave3 E/I/K screenshot measure (one-shot)
from pathlib import Path
import numpy as np
from PIL import Image

base = Path(r"C:\Users\11036\Desktop\正确客户端截图")


def info(p: Path):
    im = Image.open(p).convert("RGBA")
    a = np.array(im)
    mask = a[:, :, 3] > 128
    ys, xs = np.where(mask)
    if len(xs) == 0:
        print(p.name, im.size, "empty")
        return a
    print(
        f"{p.name}: size={im.size} opaque=({xs.min()},{ys.min()})-({xs.max()},{ys.max()}) "
        f"w={xs.max()-xs.min()+1} h={ys.max()-ys.min()+1}"
    )
    return a


for n in [
    "角色装备.png",
    "背包栏（折叠）.png",
    "背包栏展开.png",
    "技能框+技能组合框.png",
    "技能框（带提示）.png",
    "技能展开宏指令.png",
    "打开了S-E-I-K页面.png",
]:
    p = base / n
    if p.exists():
        info(p)

eq = np.array(Image.open(base / "角色装备.png").convert("RGBA"))
print("equip shape", eq.shape)
# vertical energy for pet seam
diff = np.abs(eq[:, 1:, :3].astype(int) - eq[:, :-1, :3].astype(int)).mean(axis=(0, 2))
cands = [(x, float(diff[x])) for x in range(165, 185) if diff[x] > 8]
print("equip seam cands", sorted(cands, key=lambda t: -t[1])[:8])

sk = np.array(Image.open(base / "技能框+技能组合框.png").convert("RGBA"))
diff2 = np.abs(sk[:, 1:, :3].astype(int) - sk[:, :-1, :3].astype(int)).mean(axis=(0, 2))
cands2 = sorted(
    ((x, float(diff2[x])) for x in range(150, 220)), key=lambda t: -t[1]
)[:12]
print("skill seam top", cands2)

# Template-match BtPetEquipShow from known crop region using edge of pet panel
# Estimate: main=175, pet starts ~172
print("equip right panel width", eq.shape[1] - 172)

# Item folded height vs WZ 289
item = np.array(Image.open(base / "背包栏（折叠）.png").convert("RGBA"))
print("item fold", item.shape)
itemf = np.array(Image.open(base / "背包栏展开.png").convert("RGBA"))
print("item full", itemf.shape)
