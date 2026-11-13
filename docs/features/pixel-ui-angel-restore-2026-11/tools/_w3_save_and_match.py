"""Save latest MCP canvases and match skill1 / BtSpUp / SkillMacro / BtFull."""
from pathlib import Path
import base64, io, re
import numpy as np
from PIL import Image

SCR = Path(r"C:\Users\11036\Desktop\正确客户端截图")
OUT = Path(
    r"E:\project\MapleStory-Client\docs\features\pixel-ui-angel-restore-2026-11\evidence\w3-eik"
)
OUT.mkdir(parents=True, exist_ok=True)
MCP = Path(r"C:\Users\11036\.cursor\projects\e-project-MapleStory-Client\agent-tools")

blob = ""
for d in sorted(MCP.glob("*.txt"), key=lambda p: p.stat().st_mtime, reverse=True)[:8]:
    blob += d.read_text(encoding="utf-8", errors="replace") + "\n"


def extract_last(label: str):
    last = None
    idx = 0
    while True:
        i = blob.find(label, idx)
        if i < 0:
            break
        m = re.search(r'"png":"(iVBOR[^"]+)"', blob[i : i + 900000])
        if m:
            last = Image.open(io.BytesIO(base64.b64decode(m.group(1)))).convert("RGBA")
        idx = i + 1
    return last


def mae_at(ref, tmpl, x, y):
    th, tw = tmpl.shape[:2]
    if y + th > ref.shape[0] or x + tw > ref.shape[1] or x < 0 or y < 0:
        return 1e9
    patch = ref[y : y + th, x : x + tw]
    a = tmpl[:, :, 3] > 128
    if not np.any(a):
        return 1e9
    d = np.abs(patch[:, :, :3].astype(np.int16) - tmpl[:, :, :3].astype(np.int16))
    return float(d[a].mean())


def search(ref, tmpl, x0, y0, x1, y1, step=1):
    th, tw = tmpl.shape[:2]
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


for label in [
    "SkillMacro/backgrnd",
    "Skill/skill1",
    "Skill/BtSpUp/normal/0",
    "Item/BtFull/normal/0",
    "Skill/backgrnd",
]:
    im = extract_last(label)
    if im is None:
        print("missing", label)
        continue
    path = OUT / (label.replace("/", "_") + ".png")
    im.save(path)
    print("saved", path.name, im.size)

ref = np.array(Image.open(SCR / "技能展开宏指令.png").convert("RGBA"))
print("ref", ref.shape)

sm = extract_last("SkillMacro/backgrnd")
sk = extract_last("Skill/backgrnd")
s1 = extract_last("Skill/skill1")
sp = extract_last("Skill/BtSpUp/normal/0")

if sk:
    # crop height to fit
    t = np.array(sk.crop((0, 0, 175, min(289, ref.shape[0]))))
    print("Skill/backgrnd", search(ref, t, 0, 0, 5, 5, 1))
if sm:
    t = np.array(sm.crop((0, 0, 207, min(289, ref.shape[0]))))
    print("SkillMacro/backgrnd", search(ref, t, 160, 0, 185, 5, 1))

if s1:
    t = np.array(s1)
    print("skill1 sweep best", search(ref, t, 0, 80, 40, 240, 2))
    for y in range(85, 110):
        for x in (7, 8, 9, 10, 11):
            m = mae_at(ref, t, x, y)
            if m < 5:
                print(f"  skill1 hit ({x},{y}) mae={m:.3f}")

if sp:
    t = np.array(sp)
    print("BtSpUp sweep best", search(ref, t, 120, 95, 155, 230, 1))
    # code: (10,87)+(130,20)=(140,107); r15 (7,99)+(125,20)=(132,119)
    for pos in [
        (140, 107),
        (135, 107),
        (132, 119),
        (139, 107),
        (141, 107),
        (130, 107),
        (125, 119),
        (137, 107),
        (138, 107),
    ]:
        print(f"  BtSpUp{pos} mae={mae_at(ref,t,*pos):.3f}")

# Item BtFull on fold shot (offset +3 for crop)
refi = np.array(Image.open(SCR / "背包栏（折叠）.png").convert("RGBA"))
bf = extract_last("Item/BtFull/normal/0")
if bf:
    t = np.array(bf)
    # code W-51=124 at y=4; crop has +3 => screen 127
    print("BtFull sweep", search(refi, t, 110, 0, 160, 15, 1))
    for pos in [(124, 4), (127, 4), (124, 3), (127, 3), (125, 4)]:
        print(f"  BtFull{pos} mae={mae_at(refi,t,*pos):.3f}")
