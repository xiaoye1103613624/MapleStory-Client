"""Extract canvas by exact nodePath + expected width from newest MCP dumps."""
from pathlib import Path
import base64, io, re, json
from PIL import Image
import numpy as np

SCR = Path(r"C:\Users\11036\Desktop\正确客户端截图")
OUT = Path(
    r"E:\project\MapleStory-Client\docs\features\pixel-ui-angel-restore-2026-11\evidence\w3-eik"
)
OUT.mkdir(parents=True, exist_ok=True)
MCP = Path(r"C:\Users\11036\.cursor\projects\e-project-MapleStory-Client\agent-tools")

want = {
    "SkillMacro/backgrnd": 207,
    "Skill/backgrnd": 175,
    "Skill/skill1": 141,
    "Skill/BtSpUp/normal/0": 12,
    "Item/BtFull/normal/0": 12,
    "Item/backgrnd": 175,
    "Equip/BtPetEquipShow/normal/0": 67,
}


def try_extract(blob: str, node: str, width: int):
    # Prefer JSON-ish: "nodePath":"...SkillMacro/backgrnd" ... "width":207 ... "png":
    pat = (
        rf'"nodePath"\s*:\s*"[^"]*{re.escape(node)}"[\s\S]{{0,2000}}?'
        rf'"width"\s*:\s*{width}[\s\S]{{0,500}}?"png"\s*:\s*"(iVBOR[^"]+)"'
    )
    m = re.search(pat, blob)
    if not m:
        # alternate order width before nodePath nearby
        pat2 = (
            rf'"width"\s*:\s*{width}[\s\S]{{0,300}}?"png"\s*:\s*"(iVBOR[^"]+)"'
            rf'[\s\S]{{0,400}}?"nodePath"\s*:\s*"[^"]*{re.escape(node)}"'
        )
        m = re.search(pat2, blob)
    if not m:
        return None
    im = Image.open(io.BytesIO(base64.b64decode(m.group(1)))).convert("RGBA")
    if im.size[0] != width:
        return None
    return im


found = {}
for d in sorted(MCP.glob("*.txt"), key=lambda p: p.stat().st_mtime, reverse=True)[:20]:
    blob = d.read_text(encoding="utf-8", errors="replace")
    for node, w in list(want.items()):
        if node in found:
            continue
        im = try_extract(blob, node, w)
        if im is not None:
            found[node] = im
            im.save(OUT / (node.replace("/", "_") + ".png"))
            print("OK", node, im.size, "from", d.name)
    if len(found) == len(want):
        break

print("found", len(found), "/", len(want))


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


ref = np.array(Image.open(SCR / "技能展开宏指令.png").convert("RGBA"))
print("ref", ref.shape)

if "Skill/backgrnd" in found:
    t = np.array(found["Skill/backgrnd"].crop((0, 0, 175, min(289, ref.shape[0]))))
    print("Skill bg", search(ref, t, 0, 0, 3, 3, 1))
if "SkillMacro/backgrnd" in found:
    t = np.array(found["SkillMacro/backgrnd"].crop((0, 0, 207, min(289, ref.shape[0]))))
    print("Macro bg", search(ref, t, 165, 0, 180, 5, 1))
if "Skill/skill1" in found:
    t = np.array(found["Skill/skill1"])
    print("skill1 best", search(ref, t, 0, 80, 30, 230, 1))
if "Skill/BtSpUp/normal/0" in found:
    t = np.array(found["Skill/BtSpUp/normal/0"])
    print("BtSpUp best", search(ref, t, 120, 95, 155, 220, 1))
    for pos in [(140, 107), (135, 107), (132, 119), (139, 107), (130, 107), (137, 107)]:
        print(" ", pos, mae_at(ref, t, *pos))

refi = np.array(Image.open(SCR / "背包栏（折叠）.png").convert("RGBA"))
if "Item/BtFull/normal/0" in found:
    t = np.array(found["Item/BtFull/normal/0"])
    print("BtFull best", search(refi, t, 110, 0, 160, 12, 1))
    for pos in [(124, 4), (127, 4), (124, 3), (127, 3)]:
        print(" ", pos, mae_at(refi, t, *pos))
if "Item/backgrnd" in found:
    t = np.array(found["Item/backgrnd"])
    print("Item bg best", search(refi, t, 0, 0, 6, 2, 1))
