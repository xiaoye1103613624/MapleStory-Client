# -*- coding: utf-8 -*-
"""Build daily-checkin window/v4 + cell/v1 + addon-equipment window/v4.

Font: NSimSun / 新宋体 via GDI SingleBitPerPixelGridFit → 1bit blit (no AA).
"""
from __future__ import annotations

import json
import os
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

try:
    import ctypes
    from ctypes import wintypes
except ImportError:
    ctypes = None  # type: ignore

ROOT = Path(r"E:\project\MapleStory-Client\docs\features")
CHECKIN_V3 = ROOT / "daily-checkin/ui/window/v3/normal.png"
CHECKIN_V4 = ROOT / "daily-checkin/ui/window/v4"
CELL_V1 = ROOT / "daily-checkin/ui/cell/v1"
EQUIP_REF = Path(
    r"C:\Users\11036\.cursor\projects\e-project-MapleStory-Client\assets"
    r"\c__Users_11036_AppData_Roaming_Cursor_User_workspaceStorage_empty-window_images"
    r"______-d4b1605b-68f8-4f5d-8ab1-38d2c9d7e737.png"
)
EQUIP_V3 = ROOT / "addon-equipment/ui/window/v3/normal.png"
EQUIP_V4 = ROOT / "addon-equipment/ui/window/v4"

CELL_W, CELL_H, HEADER_H = 67, 71, 19
GAP_X, ROW_PITCH = 5, 72
GRID_X0, GRID_Y0 = 7, 59
COLS, ROWS = 7, 4

COL_PINK = (204, 68, 119, 255)
COL_BEIGE = (221, 221, 204, 255)
COL_BLUE = (68, 136, 187, 255)
COL_WHITE = (255, 255, 255, 255)
COL_BLACK = (0, 0, 0, 255)
COL_WM = (64, 84, 104, 255)
COL_SLOT = (187, 204, 221, 255)
COL_CLAIM_GREEN = (34, 170, 68, 255)
MASK_ALPHA = 128  # ~50.2% within 40–55%

FONT_CANDIDATES = [
    r"C:\Windows\Fonts\simsun.ttc",
    r"C:\Windows\Fonts\simsunb.ttf",
    r"C:\Windows\Fonts\nsimsun.ttc",
    r"C:\Windows\Fonts\simhei.ttf",
]
FONT_PX = 13
GLYPH_CHARS = (
    "每日签到回来领取更多奖励！第天0123456789"
    "辅助装备图腾纹章机器人心脏徽章已领"
)
SPACING = 1


def load_font(size: int) -> ImageFont.FreeTypeFont | ImageFont.ImageFont:
    for p in FONT_CANDIDATES:
        if os.path.isfile(p):
            try:
                return ImageFont.truetype(p, size=size, index=0)
            except OSError:
                continue
    raise RuntimeError("No CJK font found (simsun/nsimsun)")


def render_glyph_gdi(ch: str, px: int) -> Image.Image:
    """Render one char via GDI SingleBitPerPixelGridFit if available; else PIL threshold."""
    # Prefer System.Drawing via pythonnet-less approach: use PIL with threshold.
    # Hard binary via large render then threshold is less ideal; use PIL + threshold at native size.
    font = load_font(px)
    # Render on black, white text
    canvas = Image.new("L", (px + 10, px + 10), 0)
    draw = ImageDraw.Draw(canvas)
    draw.text((2, 1), ch, fill=255, font=font)
    # Threshold to 1bit
    bw = canvas.point(lambda v: 255 if v >= 160 else 0, mode="L")
    # Crop ink
    bbox = bw.getbbox()
    if not bbox:
        return Image.new("RGBA", (1, 1), (0, 0, 0, 0))
    cropped = bw.crop(bbox)
    rgba = Image.new("RGBA", cropped.size, (0, 0, 0, 0))
    pxdata = cropped.load()
    out = rgba.load()
    w, h = cropped.size
    for y in range(h):
        for x in range(w):
            if pxdata[x, y] >= 160:
                out[x, y] = (255, 255, 255, 255)
    return rgba


def render_glyph_table_via_powershell() -> dict[str, Image.Image]:
    """Use embedded PowerShell GDI SingleBit for true hard-edge glyphs; save to temp PNGs."""
    import subprocess
    import tempfile

    tmp = Path(tempfile.mkdtemp(prefix="ms_glyphs_"))
    chars = "".join(dict.fromkeys(GLYPH_CHARS))  # unique preserve order
    # Write char list as UTF-8 file to avoid PS encoding issues
    char_file = tmp / "chars.txt"
    char_file.write_text(chars, encoding="utf-8")
    out_dir = tmp / "glyphs"
    out_dir.mkdir()
    ps = rf"""
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing
$chars = [System.IO.File]::ReadAllText('{char_file}', [System.Text.Encoding]::UTF8)
$outDir = '{out_dir}'
$fontName = '新宋体'
try {{
  $f = New-Object System.Drawing.Font $fontName, {FONT_PX}, ([System.Drawing.FontStyle]::Regular), ([System.Drawing.GraphicsUnit]::Pixel)
}} catch {{
  $fontName = '宋体'
  $f = New-Object System.Drawing.Font $fontName, {FONT_PX}, ([System.Drawing.FontStyle]::Regular), ([System.Drawing.GraphicsUnit]::Pixel)
}}
Write-Host "font=$($f.Name)"
$i = 0
foreach ($ch in $chars.ToCharArray()) {{
  $cell = {FONT_PX} + 8
  $bmp = New-Object System.Drawing.Bitmap $cell, $cell, ([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
  $g = [System.Drawing.Graphics]::FromImage($bmp)
  $g.Clear([System.Drawing.Color]::Black)
  $g.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::SingleBitPerPixelGridFit
  $g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::None
  $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
  $g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::None
  $br = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::White)
  $g.DrawString([string]$ch, $f, $br, 1, 1)
  $br.Dispose(); $g.Dispose()
  $minX=$cell; $minY=$cell; $maxX=-1; $maxY=-1
  for ($y=0; $y -lt $cell; $y++) {{
    for ($x=0; $x -lt $cell; $x++) {{
      if ($bmp.GetPixel($x,$y).R -ge 160) {{
        if ($x -lt $minX) {{ $minX=$x }}; if ($y -lt $minY) {{ $minY=$y }}
        if ($x -gt $maxX) {{ $maxX=$x }}; if ($y -gt $maxY) {{ $maxY=$y }}
      }}
    }}
  }}
  $path = Join-Path $outDir ("g{{0:D3}}.png" -f $i)
  if ($maxX -lt 0) {{
    $gb = New-Object System.Drawing.Bitmap 1, 1, ([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $gb.SetPixel(0,0,[System.Drawing.Color]::FromArgb(0,0,0,0))
    $gb.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $gb.Dispose()
  }} else {{
    $gw = $maxX-$minX+1; $gh = $maxY-$minY+1
    $gb = New-Object System.Drawing.Bitmap $gw, $gh, ([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    for ($y=0; $y -lt $gh; $y++) {{
      for ($x=0; $x -lt $gw; $x++) {{
        $on = ($bmp.GetPixel($minX+$x, $minY+$y).R -ge 160)
        if ($on) {{ $gb.SetPixel($x,$y,[System.Drawing.Color]::White) }}
        else {{ $gb.SetPixel($x,$y,[System.Drawing.Color]::FromArgb(0,0,0,0)) }}
      }}
    }}
    $gb.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $gb.Dispose()
  }}
  $bmp.Dispose()
  $i++
}}
$f.Dispose()
Write-Host "glyphs=$i"
"""
    ps_file = tmp / "mk_glyphs.ps1"
    # Write PS with UTF-8 BOM so Chinese font name works
    ps_file.write_bytes(b"\xef\xbb\xbf" + ps.encode("utf-8"))
    r = subprocess.run(
        ["powershell", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(ps_file)],
        capture_output=True,
    )
    out = r.stdout.decode("utf-8", errors="replace")
    err = r.stderr.decode("utf-8", errors="replace")
    print(out.encode("ascii", errors="replace").decode("ascii"))
    if r.returncode != 0:
        print(err.encode("ascii", errors="replace").decode("ascii"))
        raise RuntimeError(f"glyph PS failed: {r.returncode}")
    glyphs: dict[str, Image.Image] = {}
    for i, ch in enumerate(chars):
        p = out_dir / f"g{i:03d}.png"
        glyphs[ch] = Image.open(p).convert("RGBA").copy()
    return glyphs


def measure(
    glyphs: dict[str, Image.Image], text: str, spacing: int | None = None
) -> tuple[int, int]:
    sp = SPACING if spacing is None else spacing
    w = h = 0
    first = True
    for ch in text:
        g = glyphs[ch]
        if not first:
            w += sp
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
    outline: tuple[int, int, int, int] | None = None,
    spacing: int | None = None,
) -> None:
    sp = SPACING if spacing is None else spacing
    px = target.load()
    tw, th = target.size
    cx = x
    # outline pass
    if outline is not None:
        ox0 = cx
        for ch in text:
            g = glyphs[ch]
            gp = g.load()
            gw, gh = g.size
            for yy in range(gh):
                for xx in range(gw):
                    if gp[xx, yy][3] < 128:
                        continue
                    for dy in (-1, 0, 1):
                        for dx in (-1, 0, 1):
                            if dx == 0 and dy == 0:
                                continue
                            tx, ty = ox0 + xx + dx, y + yy + dy
                            if 0 <= tx < tw and 0 <= ty < th:
                                px[tx, ty] = outline
            ox0 += gw + sp
    # fill pass
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
                    px[tx, ty] = color
        cx += gw + sp


def fill_rect(im: Image.Image, x: int, y: int, w: int, h: int, color: tuple[int, int, int, int]) -> None:
    draw = ImageDraw.Draw(im)
    draw.rectangle([x, y, x + w - 1, y + h - 1], fill=color)


def near_pink(c: tuple[int, ...]) -> bool:
    return c[0] > 180 and c[1] < 100 and 80 < c[2] < 160


def near_white(c: tuple[int, ...]) -> bool:
    return c[0] > 240 and c[1] > 240 and c[2] > 240


def near_black(c: tuple[int, ...]) -> bool:
    return c[0] < 40 and c[1] < 40 and c[2] < 40


def watermark_dark(c: tuple[int, ...]) -> bool:
    return c[0] < 120 and c[1] < 140 and c[2] < 140 and c[0] > 20


def build() -> dict:
    CHECKIN_V4.mkdir(parents=True, exist_ok=True)
    CELL_V1.mkdir(parents=True, exist_ok=True)
    EQUIP_V4.mkdir(parents=True, exist_ok=True)

    print("=== glyphs (GDI SingleBit) ===")
    glyphs = render_glyph_table_via_powershell()
    print(f"glyph count={len(glyphs)}")

    # ---- checkin v4 ----
    print("=== daily-checkin v4 ===")
    checkin = Image.open(CHECKIN_V3).convert("RGBA")
    px = checkin.load()

    # clear title black text
    for y in range(1, 18):
        for x in range(6, 120):
            c = px[x, y]
            if near_black(c):
                px[x, y] = COL_WHITE
    blit(checkin, glyphs, "每日签到", 8, 3, COL_BLACK)

    # Banner: fixed band from v3 layout. Keep orange icon (x<28). Solid-fill then redraw tip.
    banner_top, banner_bot = 26, 51
    icon_max_x = 28
    for y in range(banner_top, banner_bot + 1):
        for x in range(icon_max_x, checkin.width - 6):
            px[x, y] = COL_BLUE
    tip = "每天回来领取更多奖励！"
    _tw, th = measure(glyphs, tip)
    tip_y = banner_top + (banner_bot - banner_top + 1 - th) // 2
    blit(checkin, glyphs, tip, 32, tip_y, COL_WHITE)
    print(f"banner y={banner_top}..{banner_bot} tip_y={tip_y}")

    seen: dict[str, bool] = {}
    for r in range(ROWS):
        for c in range(COLS):
            day = r * COLS + c + 1
            label = f"第{day}天"
            if label in seen:
                raise RuntimeError(f"duplicate {label}")
            seen[label] = True
            cx = GRID_X0 + c * (CELL_W + GAP_X)
            cy = GRID_Y0 + r * ROW_PITCH
            fill_rect(checkin, cx, cy, CELL_W, HEADER_H, COL_PINK)
            lw, lh = measure(glyphs, label)
            tx = cx + (CELL_W - lw) // 2
            ty = cy + (HEADER_H - lh) // 2
            tx = max(cx, min(tx, cx + CELL_W - lw))
            ty = max(cy, min(ty, cy + HEADER_H - lh))
            blit(checkin, glyphs, label, tx, ty, COL_WHITE)
    if len(seen) != 28:
        raise RuntimeError(f"expected 28 days, got {len(seen)}")

    checkin_path = CHECKIN_V4 / "normal.png"
    checkin.save(checkin_path)
    print(f"saved {checkin_path} {checkin.size}")

    # ---- cell v1 ----
    print("=== cell v1 ===")
    cell_blank = Image.new("RGBA", (CELL_W, CELL_H), (0, 0, 0, 0))
    fill_rect(cell_blank, 0, 0, CELL_W, HEADER_H, COL_PINK)
    fill_rect(cell_blank, 0, HEADER_H, CELL_W, CELL_H - HEADER_H, COL_BEIGE)
    cell_blank.save(CELL_V1 / "normal.png")

    cell_day1 = checkin.crop((GRID_X0, GRID_Y0, GRID_X0 + CELL_W, GRID_Y0 + CELL_H))
    cell_day1.save(CELL_V1 / "normal-day1.png")

    mask = Image.new("RGBA", (CELL_W, CELL_H), (68, 68, 68, MASK_ALPHA))
    mask.save(CELL_V1 / "mask.png")

    claim = "已领取"
    cw, ch = measure(glyphs, claim)
    pad = 3
    claim_label = Image.new("RGBA", (cw + pad * 2, ch + pad * 2), (0, 0, 0, 0))
    blit(claim_label, glyphs, claim, pad, pad, COL_CLAIM_GREEN, outline=COL_WHITE)
    claim_label.save(CELL_V1 / "claimed-label.png")

    claimed = Image.new("RGBA", (CELL_W, CELL_H), (68, 68, 68, MASK_ALPHA))
    lw, lh = claim_label.size
    body_top = HEADER_H
    body_h = CELL_H - HEADER_H
    lx = (CELL_W - lw) // 2
    ly = body_top + (body_h - lh) // 2
    lx = max(0, min(lx, CELL_W - lw))
    ly = max(body_top, min(ly, CELL_H - lh))
    claimed.alpha_composite(claim_label, (lx, ly))
    # Ensure claimed text pixels are fully opaque green/white (above mask)
    cp = claimed.load()
    lp = claim_label.load()
    for yy in range(lh):
        for xx in range(lw):
            p = lp[xx, yy]
            if p[3] > 0:
                cp[lx + xx, ly + yy] = (p[0], p[1], p[2], 255)
    claimed.save(CELL_V1 / "claimed.png")

    # preview: blank + blend mask + opaque label
    preview = cell_blank.copy()
    # blend mask
    base = preview.load()
    mp = mask.load()
    for y in range(CELL_H):
        for x in range(CELL_W):
            b = base[x, y]
            m = mp[x, y]
            a = m[3] / 255.0
            base[x, y] = (
                int(m[0] * a + b[0] * (1 - a)),
                int(m[1] * a + b[1] * (1 - a)),
                int(m[2] * a + b[2] * (1 - a)),
                255,
            )
    preview.paste(claim_label, (lx, ly), claim_label)
    preview.save(CELL_V1 / "claimed-preview.png")

    # ---- equip v4 ----
    print("=== addon-equipment v4 ===")
    ref_path = EQUIP_REF if EQUIP_REF.is_file() else EQUIP_V3
    equip = Image.open(ref_path).convert("RGBA")
    ep = equip.load()

    for y in range(1, 19):
        for x in range(4, equip.width - 4):
            c = ep[x, y]
            if near_black(c) or (c[0] < 60 and c[1] < 60 and c[2] < 60):
                ep[x, y] = COL_WHITE
    et = "辅助装备"
    ew, eh = measure(glyphs, et)
    blit(equip, glyphs, et, (equip.width - ew) // 2, 4, COL_BLACK)

    # Slot content rects measured from English ref (inner fill excluding white dividers)
    slot_origins = [
        (11, 32),
        (44, 32),
        (77, 32),
        (110, 32),
        (11, 65),
        (44, 65),
        (77, 65),
        (110, 65),
    ]
    labels = ["图腾", "图腾", "图腾", "图腾", "纹章", "机器人", "心脏", "徽章"]
    slot_w, slot_h = 31, 30
    wm_ox, wm_oy = 2, 2

    # Fully repaint slot interiors to solid slot color (erase EN watermark completely)
    for sx, sy in slot_origins:
        for yy in range(slot_h):
            for xx in range(slot_w):
                px_, py_ = sx + xx, sy + yy
                if px_ >= equip.width or py_ >= equip.height:
                    continue
                ep[px_, py_] = COL_SLOT

    # Watermark: spacing 0; if still too wide for 「机器人」, use 11px glyph subset
    wm_glyphs = glyphs
    wm_sp = 0
    probe_w, _ = measure(glyphs, "机器人", spacing=0)
    if probe_w > slot_w - 2:
        print(f"generating 11px watermark glyphs (机器人 w={probe_w})")
        import subprocess
        import tempfile

        tmp = Path(tempfile.mkdtemp(prefix="ms_wm_"))
        chars = "图腾纹章机器人心脏徽章"
        (tmp / "chars.txt").write_text(chars, encoding="utf-8")
        out_dir = tmp / "g"
        out_dir.mkdir()
        char_file = str(tmp / "chars.txt").replace("\\", "/")
        out_posix = str(out_dir).replace("\\", "/")
        ps = f"""
$ErrorActionPreference='Stop'
Add-Type -AssemblyName System.Drawing
$chars=[System.IO.File]::ReadAllText('{char_file}',[System.Text.Encoding]::UTF8)
$outDir='{out_posix}'
$fontName='新宋体'
try {{ $f=New-Object System.Drawing.Font $fontName,11,([System.Drawing.FontStyle]::Regular),([System.Drawing.GraphicsUnit]::Pixel) }}
catch {{ $f=New-Object System.Drawing.Font '宋体',11,([System.Drawing.FontStyle]::Regular),([System.Drawing.GraphicsUnit]::Pixel) }}
$i=0
foreach($ch in $chars.ToCharArray()){{
  $cell=18
  $bmp=New-Object System.Drawing.Bitmap $cell,$cell,([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
  $g=[System.Drawing.Graphics]::FromImage($bmp)
  $g.Clear([System.Drawing.Color]::Black)
  $g.TextRenderingHint=[System.Drawing.Text.TextRenderingHint]::SingleBitPerPixelGridFit
  $g.SmoothingMode=[System.Drawing.Drawing2D.SmoothingMode]::None
  $br=New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::White)
  $g.DrawString([string]$ch,$f,$br,1,1)
  $br.Dispose(); $g.Dispose()
  $minX=$cell;$minY=$cell;$maxX=-1;$maxY=-1
  for($y=0;$y -lt $cell;$y++){{ for($x=0;$x -lt $cell;$x++){{ if($bmp.GetPixel($x,$y).R -ge 160){{ if($x -lt $minX){{$minX=$x}}; if($y -lt $minY){{$minY=$y}}; if($x -gt $maxX){{$maxX=$x}}; if($y -gt $maxY){{$maxY=$y}} }} }} }}
  $path=Join-Path $outDir ('g{{0:D3}}.png' -f $i)
  if($maxX -lt 0){{ $gb=New-Object System.Drawing.Bitmap 1,1,([System.Drawing.Imaging.PixelFormat]::Format32bppArgb); $gb.SetPixel(0,0,[System.Drawing.Color]::FromArgb(0,0,0,0)); $gb.Save($path,[System.Drawing.Imaging.ImageFormat]::Png); $gb.Dispose() }}
  else {{
    $gw=$maxX-$minX+1; $gh=$maxY-$minY+1
    $gb=New-Object System.Drawing.Bitmap $gw,$gh,([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    for($y=0;$y -lt $gh;$y++){{ for($x=0;$x -lt $gw;$x++){{
      if($bmp.GetPixel($minX+$x,$minY+$y).R -ge 160){{ $gb.SetPixel($x,$y,[System.Drawing.Color]::White) }} else {{ $gb.SetPixel($x,$y,[System.Drawing.Color]::FromArgb(0,0,0,0)) }}
    }} }}
    $gb.Save($path,[System.Drawing.Imaging.ImageFormat]::Png); $gb.Dispose()
  }}
  $bmp.Dispose(); $i++
}}
$f.Dispose(); Write-Host "wm=$i"
"""
        ps_file = tmp / "wm.ps1"
        ps_file.write_bytes(b"\xef\xbb\xbf" + ps.encode("utf-8"))
        r = subprocess.run(
            ["powershell", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(ps_file)],
            capture_output=True,
        )
        if r.returncode != 0:
            raise RuntimeError("wm glyph fail: " + r.stderr.decode("utf-8", errors="replace"))
        wm_glyphs = {}
        for i, ch in enumerate(chars):
            wm_glyphs[ch] = Image.open(out_dir / f"g{i:03d}.png").convert("RGBA").copy()
        probe_w, _ = measure(wm_glyphs, "机器人", spacing=0)
        print(f"wm 机器人 w={probe_w}")

    for (sx, sy), lab in zip(slot_origins, labels):
        lw, lh = measure(wm_glyphs, lab, spacing=wm_sp)
        tx, ty = sx + wm_ox, sy + wm_oy
        if tx + lw > sx + slot_w:
            tx = max(sx, sx + slot_w - lw)
        blit(equip, wm_glyphs, lab, tx, ty, COL_WM, spacing=wm_sp)
        print(f"slot label {lab} at ({tx},{ty}) size={lw}x{lh}")

    equip_path = EQUIP_V4 / "normal.png"
    equip.save(equip_path)
    print(f"saved {equip_path} {equip.size}")

    # grayMid check on pink headers
    gray_mid = 0
    px = checkin.load()
    for r in range(ROWS):
        for c in range(COLS):
            cx = GRID_X0 + c * (CELL_W + GAP_X)
            cy = GRID_Y0 + r * ROW_PITCH
            for y in range(cy, cy + HEADER_H):
                for x in range(cx, cx + CELL_W):
                    p = px[x, y]
                    if near_pink(p) or near_white(p):
                        continue
                    if abs(p[0] - p[1]) < 30 and 40 < p[0] < 220:
                        gray_mid += 1
    print(f"pink-header grayMid={gray_mid}")

    meta = {
        "checkin": str(checkin_path),
        "equip": str(equip_path),
        "cellW": CELL_W,
        "cellH": CELL_H,
        "maskAlpha": MASK_ALPHA,
        "maskOpacityPct": round(MASK_ALPHA / 255 * 100, 1),
        "claimGreenRGB": list(COL_CLAIM_GREEN[:3]),
        "grayMid": gray_mid,
        "days": list(seen.keys()),
        "equipLabels": labels,
    }
    (CHECKIN_V4 / "build_meta.json").write_text(
        json.dumps(meta, ensure_ascii=False, indent=2), encoding="utf-8"
    )
    print("=== BUILD DONE ===")
    print(json.dumps(meta, ensure_ascii=False, indent=2))
    return meta


if __name__ == "__main__":
    build()
