"""Wider searches for SkillMacro / BtSpUp / BtFull / skill1."""
from pathlib import Path
import base64, io, re
import numpy as np
from PIL import Image

SCR = Path(r"C:\Users\11036\Desktop\正确客户端截图")
MCP = Path(r"C:\Users\11036\.cursor\projects\e-project-MapleStory-Client\agent-tools")
OUT = Path(
    r"E:\project\MapleStory-Client\docs\features\pixel-ui-angel-restore-2026-11\evidence\w3-eik"
)

# Build blob from dumps containing needed pngs
blob = ""
for d in sorted(MCP.glob("*.txt"), key=lambda p: p.stat().st_mtime, reverse=True)[:40]:
    t = d.read_text(encoding="utf-8", errors="replace")
    if '"png":"iVBOR' in t:
        blob += t + "\n"


def extract(label: str):
    idx = 0
    while True:
        i = blob.find(label, idx)
        if i < 0:
            return None
        m = re.search(r'"png":"(iVBOR[^"]+)"', blob[i : i + 800000])
        if m:
            return Image.open(io.BytesIO(base64.b64decode(m.group(1)))).convert("RGBA")
        idx = i + 1


def mae_at(ref, tmpl, x, y):
    th, tw = tmpl.shape[:2]
    patch = ref[y : y + th, x : x + tw]
    if patch.shape[:2] != (th, tw):
        return 1e9
    a = tmpl[:, :, 3] > 128
    if not np.any(a):
        return 1e9
    d = np.abs(patch[:, :, :3].astype(np.int16) - tmpl[:, :, :3].astype(np.int16))
    return float(d[a].mean())


def search(ref, tmpl, x0, y0, x1, y1, step=1):
    th, tw = tmpl.shape[:2]
    H, W = ref.shape[:2]
    x1 = min(x1, W - tw)
    y1 = min(y1, H - th)
    best = (1e9, -1, -1)
    for y in range(y0, y1 + 1, step):
        for x in range(x0, x1 + 1, step):
            m = mae_at(ref, tmpl, x, y)
            if m < best[0]:
                best = (m, x, y)
    bx, by = best[1], best[2]
    if bx < 0:
        return best
    for y in range(max(y0, by - step), min(y1, by + step) + 1):
        for x in range(max(x0, bx - step), min(x1, bx + step) + 1):
            m = mae_at(ref, tmpl, x, y)
            if m < best[0]:
                best = (m, x, y)
    return best


jobs = [
    ("技能展开宏指令.png", "SkillMacro/backgrnd", 150, 0, 200, 10),
    ("技能框+技能组合框.png", "Skill/backgrnd", 0, 0, 10, 10),
    ("技能框+技能组合框.png", "SkillMacro/backgrnd", 150, 0, 200, 10),
    ("技能展开宏指令.png", "Skill/skill1", 0, 80, 30, 250),
    ("技能展开宏指令.png", "Skill/BtSpUp/normal/0", 120, 90, 160, 250),
    ("背包栏（折叠）.png", "Item/BtFull/normal/0", 100, 0, 160, 20),
    ("背包栏（折叠）.png", "Item/backgrnd", 0, 0, 8, 4),
    ("背包栏展开.png", "Item/FullBackgrnd", 0, 0, 5, 5),  # will fail if W too big
]

# FullBackgrnd is 603 but shot is 602 — crop tmpl to 602 and match
for scr, node, x0, y0, x1, y1 in jobs:
    im = extract(node)
    if im is None:
        print("NO", node)
        continue
    ref = np.array(Image.open(SCR / scr).convert("RGBA"))
    if node.endswith("FullBackgrnd") and im.size[0] > ref.shape[1]:
        im = im.crop((0, 0, ref.shape[1], min(im.size[1], ref.shape[0])))
        print("cropped FullBackgrnd to", im.size)
    mae, x, y = search(ref, np.array(im), x0, y0, x1, y1, step=2)
    print(f"{scr} <- {node}: ({x},{y}) mae={mae:.3f} tmpl={im.size} ref={ref.shape[1]}x{ref.shape[0]}")
    im.save(OUT / (node.replace("/", "_") + ".png"))

# Expected code positions for skill1 rows: CLASSIC (10,87)+ (0,40*i) but sprite offset (1,0)
# => frame coords of skill1: if Q=(10,87) and frame = Q-(1,0) wait comments say Q = frame+(1,0)
# so frame = Q - (1,0) = (9,87) for first row?
print("--- check skill1 at candidate positions ---")
sk1 = extract("Skill/skill1")
ref = np.array(Image.open(SCR / "技能展开宏指令.png").convert("RGBA"))
if sk1:
    t = np.array(sk1)
    for y in [87, 99, 100, 101]:
        for x in [7, 9, 10, 11]:
            print(f"  skill1@({x},{y}) mae={mae_at(ref,t,x,y):.3f}")

sp = extract("Skill/BtSpUp/normal/0")
if sp:
    t = np.array(sp)
    # CLASSIC_SKILL (10,87) + SPUP (130,20) = (140,107); or (125,20)->(135,107)
    # with Q->frame: (139,107) / (134,107)
    for pos in [(140, 107), (135, 107), (132, 119), (139, 107), (134, 107), (141, 107)]:
        print(f"  BtSpUp@{pos} mae={mae_at(ref,t,*pos):.3f}")
