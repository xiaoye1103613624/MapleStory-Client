# -*- coding: utf-8 -*-
"""Build addon-equipment window/v5.

Fixes vs v4:
- Title vertically centered in white bar (y=9..21), matching EN gap≈3
- Slot watermarks re-blitted via GDI SingleBitPerPixelGridFit (grayMid=0)
- Labels: 图腾×4 / 纹章 / 机器人 / 心脏 / 徽章
"""
from __future__ import annotations

import json
import subprocess
import tempfile
from pathlib import Path

from PIL import Image, ImageDraw

ROOT = Path(r"E:\project\MapleStory-Client\docs\features")
EQUIP_REF = Path(
    r"C:\Users\11036\.cursor\projects\e-project-MapleStory-Client\assets"
    r"\C__Users_11036_.cursor_projects_e-project-MapleStory-Client_assets"
    r"_c__Users_11036_AppData_Roaming_Cursor_User_workspaceStorage_empty-window"
    r"_images______-d4b1605b-68f8-4f5d-8ab1-38d2c9d7e737.png"
)
# fallback: lowercase path variant used by prior builds
EQUIP_REF_ALT = Path(
    r"C:\Users\11036\.cursor\projects\e-project-MapleStory-Client\assets"
    r"\c__Users_11036_AppData_Roaming_Cursor_User_workspaceStorage_empty-window_images"
    r"______-d4b1605b-68f8-4f5d-8ab1-38d2c9d7e737.png"
)
OUT = ROOT / "addon-equipment/ui/window/v5"
LAYERS = OUT / "_layers"

COL_WHITE = (255, 255, 255, 255)
COL_BLACK = (0, 0, 0, 255)
COL_WM = (64, 84, 104, 255)
COL_SLOT = (187, 204, 221, 255)

TITLE_BAR = (9, 21)  # inclusive
# 13px glyphs ≈12px tall fill the 13px bar (gap≈0). EN caps ≈8px with gap≈3.
# 10px yields ~9px ink → gap_top=gap_bot=2 inside the white bar.
TITLE_PX = 10
WM_PX = 11
SPACING = 1
TITLE_MIN_GAP_TOP = 2

SLOT_ORIGINS = [
    (11, 32),
    (44, 32),
    (77, 32),
    (110, 32),
    (11, 65),
    (44, 65),
    (77, 65),
    (110, 65),
]
LABELS = ["图腾", "图腾", "图腾", "图腾", "纹章", "机器人", "心脏", "徽章"]
SLOT_W, SLOT_H = 31, 30
WM_OX, WM_OY = 2, 2


def near_black(c: tuple[int, ...]) -> bool:
    return c[0] < 60 and c[1] < 60 and c[2] < 60


def gdi_glyphs(chars: str, px: int) -> dict[str, Image.Image]:
    tmp = Path(tempfile.mkdtemp(prefix="ms_v5_g_"))
    char_file = tmp / "chars.txt"
    char_file.write_text(chars, encoding="utf-8")
    out_dir = tmp / "g"
    out_dir.mkdir()
    cf = str(char_file).replace("\\", "/")
    od = str(out_dir).replace("\\", "/")
    ps = f"""
$ErrorActionPreference='Stop'
Add-Type -AssemblyName System.Drawing
$chars=[System.IO.File]::ReadAllText('{cf}',[System.Text.Encoding]::UTF8)
$outDir='{od}'
$fontName='新宋体'
try {{ $f=New-Object System.Drawing.Font $fontName,{px},([System.Drawing.FontStyle]::Regular),([System.Drawing.GraphicsUnit]::Pixel) }}
catch {{ $f=New-Object System.Drawing.Font '宋体',{px},([System.Drawing.FontStyle]::Regular),([System.Drawing.GraphicsUnit]::Pixel) }}
Write-Host "font=$($f.Name) px={px}"
$i=0
foreach($ch in $chars.ToCharArray()){{
  $cell={px}+10
  $bmp=New-Object System.Drawing.Bitmap $cell,$cell,([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
  $g=[System.Drawing.Graphics]::FromImage($bmp)
  $g.Clear([System.Drawing.Color]::Black)
  $g.TextRenderingHint=[System.Drawing.Text.TextRenderingHint]::SingleBitPerPixelGridFit
  $g.SmoothingMode=[System.Drawing.Drawing2D.SmoothingMode]::None
  $g.InterpolationMode=[System.Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
  $g.PixelOffsetMode=[System.Drawing.Drawing2D.PixelOffsetMode]::None
  $br=New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::White)
  $g.DrawString([string]$ch,$f,$br,1,1)
  $br.Dispose(); $g.Dispose()
  $minX=$cell;$minY=$cell;$maxX=-1;$maxY=-1
  for($y=0;$y -lt $cell;$y++){{ for($x=0;$x -lt $cell;$x++){{
    if($bmp.GetPixel($x,$y).R -ge 160){{
      if($x -lt $minX){{$minX=$x}}; if($y -lt $minY){{$minY=$y}}
      if($x -gt $maxX){{$maxX=$x}}; if($y -gt $maxY){{$maxY=$y}}
    }}
  }} }}
  $path=Join-Path $outDir ('g{{0:D3}}.png' -f $i)
  if($maxX -lt 0){{
    $gb=New-Object System.Drawing.Bitmap 1,1,([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $gb.SetPixel(0,0,[System.Drawing.Color]::FromArgb(0,0,0,0))
    $gb.Save($path,[System.Drawing.Imaging.ImageFormat]::Png); $gb.Dispose()
  }} else {{
    $gw=$maxX-$minX+1; $gh=$maxY-$minY+1
    $gb=New-Object System.Drawing.Bitmap $gw,$gh,([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    for($y=0;$y -lt $gh;$y++){{ for($x=0;$x -lt $gw;$x++){{
      if($bmp.GetPixel($minX+$x,$minY+$y).R -ge 160){{ $gb.SetPixel($x,$y,[System.Drawing.Color]::White) }}
      else {{ $gb.SetPixel($x,$y,[System.Drawing.Color]::FromArgb(0,0,0,0)) }}
    }} }}
    $gb.Save($path,[System.Drawing.Imaging.ImageFormat]::Png); $gb.Dispose()
  }}
  $bmp.Dispose(); $i++
}}
$f.Dispose(); Write-Host "glyphs=$i"
"""
    ps_file = tmp / "mk.ps1"
    ps_file.write_bytes(b"\xef\xbb\xbf" + ps.encode("utf-8"))
    r = subprocess.run(
        ["powershell", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(ps_file)],
        capture_output=True,
    )
    print(r.stdout.decode("utf-8", errors="replace").encode("ascii", errors="replace").decode("ascii"))
    if r.returncode != 0:
        raise RuntimeError(r.stderr.decode("utf-8", errors="replace").encode("ascii", errors="replace").decode("ascii"))
    glyphs: dict[str, Image.Image] = {}
    for i, ch in enumerate(chars):
        glyphs[ch] = Image.open(out_dir / f"g{i:03d}.png").convert("RGBA").copy()
    return glyphs


def measure(glyphs: dict[str, Image.Image], text: str, spacing: int = SPACING) -> tuple[int, int]:
    w = h = 0
    first = True
    for ch in text:
        g = glyphs[ch]
        if not first:
            w += spacing
        w += g.size[0]
        h = max(h, g.size[1])
        first = False
    return w, h


def blit(
    target: Image.Image,
    glyphs: dict[str, Image.Image],
    text: str,
    x: int,
    y: int,
    color: tuple[int, int, int, int],
    spacing: int = SPACING,
) -> Image.Image:
    """Blit onto target; also return a transparent layer containing only this text."""
    layer = Image.new("RGBA", target.size, (0, 0, 0, 0))
    tp = target.load()
    lp = layer.load()
    tw, th = target.size
    cx = x
    for ch in text:
        g = glyphs[ch]
        gp = g.load()
        gw, gh = g.size
        for yy in range(gh):
            for xx in range(gw):
                if gp[xx, yy][3] < 128:
                    continue
                tx, ty = cx + xx, y + yy
                if 0 <= tx < tw and 0 <= ty < th:
                    tp[tx, ty] = color
                    lp[tx, ty] = color
        cx += gw + spacing
    return layer


def build() -> dict:
    OUT.mkdir(parents=True, exist_ok=True)
    LAYERS.mkdir(parents=True, exist_ok=True)

    ref = EQUIP_REF if EQUIP_REF.is_file() else EQUIP_REF_ALT
    if not ref.is_file():
        raise FileNotFoundError(f"EN ref missing: {EQUIP_REF} / {EQUIP_REF_ALT}")

    title_chars = "辅助装备"
    wm_chars = "图腾纹章机器人心脏徽章"
    title_glyphs = gdi_glyphs(title_chars, TITLE_PX)
    wm_glyphs = gdi_glyphs(wm_chars, WM_PX)

    equip = Image.open(ref).convert("RGBA")
    ep = equip.load()

    # Clear EN title ink in/near title bar
    for y in range(1, 23):
        for x in range(4, equip.width - 4):
            if near_black(ep[x, y]):
                ep[x, y] = COL_WHITE

    # Solid-fill slots (erase EN watermarks completely)
    for sx, sy in SLOT_ORIGINS:
        for yy in range(SLOT_H):
            for xx in range(SLOT_W):
                ep[sx + xx, sy + yy] = COL_SLOT

    frame = equip.copy()
    frame.save(LAYERS / "frame.png")

    et = "辅助装备"
    ew, eh = measure(title_glyphs, et)
    bar_top, bar_bot = TITLE_BAR
    bar_h = bar_bot - bar_top + 1
    # Optical center: prefer EN-like top gap (~3). Equal math center is 2/2;
    # Chinese glyphs carry more top ink, so bias +1px down when room allows.
    title_y = bar_top + max(TITLE_MIN_GAP_TOP, (bar_h - eh) // 2)
    if title_y + eh <= bar_bot:  # room for +1 optical bias
        title_y = min(title_y + 1, bar_bot - eh + 1)
    if title_y + eh - 1 > bar_bot - 1:
        title_y = max(bar_top + TITLE_MIN_GAP_TOP, bar_bot - 1 - eh + 1)
    title_x = (equip.width - ew) // 2
    title_layer = blit(equip, title_glyphs, et, title_x, title_y, COL_BLACK)
    title_layer.save(LAYERS / "title.png")
    print(
        f"title at ({title_x},{title_y}) size={ew}x{eh} "
        f"bar={bar_top}..{bar_bot} gap_top={title_y - bar_top} "
        f"gap_bot={bar_bot - (title_y + eh - 1)}"
    )

    wm_layer = Image.new("RGBA", equip.size, (0, 0, 0, 0))
    probe_w, _ = measure(wm_glyphs, "机器人", spacing=0)
    print(f"wm 机器人 w={probe_w} slot_w={SLOT_W}")
    for (sx, sy), lab in zip(SLOT_ORIGINS, LABELS):
        lw, lh = measure(wm_glyphs, lab, spacing=0)
        tx, ty = sx + WM_OX, sy + WM_OY
        if tx + lw > sx + SLOT_W:
            tx = max(sx, sx + SLOT_W - lw)
        part = blit(equip, wm_glyphs, lab, tx, ty, COL_WM, spacing=0)
        wm_layer.alpha_composite(part)
        print(f"  slot {lab} @({tx},{ty}) {lw}x{lh}")
    wm_layer.save(LAYERS / "watermarks.png")

    png_path = OUT / "normal.png"

    # Hard gate: force binary in title ink + slot fills (no mid-gray AA anywhere on glyphs)
    ep = equip.load()
    for y in range(bar_top, bar_bot + 1):
        for x in range(20, equip.width - 20):  # skip chrome corners
            r, g, b, a = ep[x, y]
            if (r, g, b) == (255, 255, 255):
                continue
            if r + g + b < 80:
                ep[x, y] = COL_BLACK
            else:
                # any leftover mid tone inside title field → white
                ep[x, y] = COL_WHITE
    for sx, sy in SLOT_ORIGINS:
        for y in range(sy, sy + SLOT_H):
            for x in range(sx, sx + SLOT_W):
                r, g, b, a = ep[x, y]
                if (r, g, b) == COL_SLOT[:3]:
                    continue
                # snap any near-ink to exact WM; anything else back to slot bg
                lum = (r + g + b) / 3
                if lum < 140:
                    ep[x, y] = COL_WM
                else:
                    ep[x, y] = COL_SLOT

    # Rebuild clean text layers from final composite (binary only)
    title_clean = Image.new("RGBA", equip.size, (0, 0, 0, 0))
    wm_clean = Image.new("RGBA", equip.size, (0, 0, 0, 0))
    tp = title_clean.load()
    wp = wm_clean.load()
    for y in range(bar_top, bar_bot + 1):
        for x in range(20, equip.width - 20):
            if ep[x, y][:3] == (0, 0, 0):
                tp[x, y] = COL_BLACK
    for sx, sy in SLOT_ORIGINS:
        for y in range(sy, sy + SLOT_H):
            for x in range(sx, sx + SLOT_W):
                if ep[x, y][:3] == COL_WM[:3]:
                    wp[x, y] = COL_WM
    # frame without text for PSD
    frame_only = equip.copy()
    fp = frame_only.load()
    for y in range(equip.height):
        for x in range(equip.width):
            if tp[x, y][3] > 0:
                fp[x, y] = COL_WHITE
            elif wp[x, y][3] > 0:
                fp[x, y] = COL_SLOT
    frame_only.save(LAYERS / "frame.png")
    title_clean.save(LAYERS / "title.png")
    wm_clean.save(LAYERS / "watermarks.png")

    equip.save(png_path)

    # Acceptance metrics
    bar_inks = []
    for y in range(bar_top, bar_bot + 1):
        for x in range(4, equip.width - 4):
            p = ep[x, y]
            if p[0] + p[1] + p[2] < 80:
                bar_inks.append(y)
    gap_top = min(bar_inks) - bar_top if bar_inks else -1
    gap_bot = bar_bot - max(bar_inks) if bar_inks else -1

    gray_mid = 0
    ink_slots = 0
    for sx, sy in SLOT_ORIGINS:
        for y in range(sy, sy + SLOT_H):
            for x in range(sx, sx + SLOT_W):
                r, g, b, a = ep[x, y]
                if (r, g, b) == COL_SLOT[:3]:
                    continue
                if (r, g, b) == COL_WM[:3]:
                    ink_slots += 1
                    continue
                # mid-gray AA
                if 40 < (r + g + b) / 3 < 200:
                    gray_mid += 1

    meta = {
        "size": list(equip.size),
        "title": {"x": title_x, "y": title_y, "w": ew, "h": eh, "gap_top": gap_top, "gap_bot": gap_bot},
        "title_dy_vs_v4": title_y - 4,  # v4 used y=4
        "wm_px": WM_PX,
        "labels": LABELS,
        "grayMid": gray_mid,
        "slotInk": ink_slots,
        "ref": str(ref),
    }
    (OUT / "build_meta.json").write_text(json.dumps(meta, ensure_ascii=False, indent=2), encoding="utf-8")
    print("=== V5 BUILD DONE ===")
    print(json.dumps(meta, ensure_ascii=False, indent=2))
    return meta


if __name__ == "__main__":
    build()
