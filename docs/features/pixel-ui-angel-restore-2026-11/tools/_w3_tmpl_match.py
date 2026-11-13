"""Template-match key WZ canvases onto correct screenshots; print best (x,y,mae)."""
from __future__ import annotations

import base64
import io
import json
import re
from pathlib import Path

import numpy as np
from PIL import Image

# Re-fetch via saved MCP pngs: decode from latest agent-tools by nodePath labels
MCP = Path(
    r"C:\Users\11036\.cursor\projects\e-project-MapleStory-Client\agent-tools"
)
# Prefer writing canvases from orange-wz via a small local dump if present;
# else extract from last get_node_detail response files.

SCR = Path(r"C:\Users\11036\Desktop\正确客户端截图")
OUT = Path(
    r"E:\project\MapleStory-Client\docs\features\pixel-ui-angel-restore-2026-11\evidence\w3-eik"
)
OUT.mkdir(parents=True, exist_ok=True)


def extract_png(blob: str, label: str) -> Image.Image | None:
    # find nodePath then nearby "png":"iVBOR...
    idx = blob.find(label)
    if idx < 0:
        return None
    m = re.search(r'"png":"(iVBOR[^"]+)"', blob[idx : idx + 500000])
    if not m:
        return None
    data = base64.b64decode(m.group(1))
    return Image.open(io.BytesIO(data)).convert("RGBA")


def mae_at(ref: np.ndarray, tmpl: np.ndarray, x: int, y: int) -> float:
    th, tw = tmpl.shape[:2]
    patch = ref[y : y + th, x : x + tw]
    if patch.shape[:2] != (th, tw):
        return 1e9
    a = tmpl[:, :, 3] > 128
    if not np.any(a):
        return 1e9
    d = np.abs(patch[:, :, :3].astype(np.int16) - tmpl[:, :, :3].astype(np.int16))
    return float(d[a].mean())


def search(ref: np.ndarray, tmpl: np.ndarray, x0=0, y0=0, x1=None, y1=None, step=1):
    th, tw = tmpl.shape[:2]
    H, W = ref.shape[:2]
    if x1 is None:
        x1 = W - tw
    if y1 is None:
        y1 = H - th
    best = (1e9, -1, -1)
    for y in range(y0, max(y0, y1) + 1, step):
        for x in range(x0, max(x0, x1) + 1, step):
            m = mae_at(ref, tmpl, x, y)
            if m < best[0]:
                best = (m, x, y)
    # refine neighborhood
    bx, by = best[1], best[2]
    for y in range(max(0, by - step), min(H - th, by + step) + 1):
        for x in range(max(0, bx - step), min(W - tw, bx + step) + 1):
            m = mae_at(ref, tmpl, x, y)
            if m < best[0]:
                best = (m, x, y)
    return best


# Load MCP dump that has SkillMacro + Skill + Equip pet
dumps = sorted(MCP.glob("*.txt"), key=lambda p: p.stat().st_mtime, reverse=True)
blob = ""
for d in dumps[:15]:
    t = d.read_text(encoding="utf-8", errors="replace")
    if "SkillMacro/backgrnd" in t and '"png":"iVBOR' in t:
        blob = t
        print("using dump", d.name)
        break
if not blob:
    # fallback: concatenate recent
    for d in dumps[:5]:
        blob += d.read_text(encoding="utf-8", errors="replace")

pairs = [
    ("技能展开宏指令.png", "Skill/backgrnd", 0, 0, 5, 5),
    ("技能展开宏指令.png", "SkillMacro/backgrnd", 160, 0, 185, 5),
    ("技能框+技能组合框.png", "Skill/backgrnd", 0, 0, 5, 5),
    ("技能框+技能组合框.png", "SkillMacro/backgrnd", 160, 0, 185, 5),
    ("角色装备.png", "Equip/BtPetEquipShow/normal/0", 90, 270, 120, 290),
    ("角色装备.png", "Equip/BtPet1/disabled/0", 170, 260, 200, 285),
    ("背包栏（折叠）.png", "Item/backgrnd", 0, 0, 5, 5),
    ("背包栏展开.png", "Item/FullBackgrnd", 0, 0, 5, 5),
]

# Need Item canvases too — may be in older dump
for d in dumps[:30]:
    t = d.read_text(encoding="utf-8", errors="replace")
    if "Item/FullBackgrnd" in t and '"png":"iVBOR' in t:
        blob += t
        break

results = []
for scr_name, node, x0, y0, x1, y1 in pairs:
    tmpl = extract_png(blob, node)
    if tmpl is None:
        print("NO TMPL", node)
        continue
    ref = np.array(Image.open(SCR / scr_name).convert("RGBA"))
    mae, x, y = search(ref, np.array(tmpl), x0, y0, x1, y1, step=2)
    line = f"{scr_name} <- {node}: best=({x},{y}) mae={mae:.3f} tmpl={tmpl.size}"
    print(line)
    results.append(line)
    tmpl.save(OUT / (node.replace("/", "_") + ".png"))

(OUT / "tmpl_match.txt").write_text("\n".join(results) + "\n", encoding="utf-8")
print("wrote", OUT)
