# Rebuild left/right side panels by tiling official UIWindow.img/WorldMap/Border pieces.
# Output: standalone chrome PNGs only (no center map).
from __future__ import annotations

import os
from PIL import Image, ImageDraw, ImageFont

ROOT = os.path.dirname(os.path.abspath(__file__))
TILE_DIR = os.path.join(ROOT, "source", "UIWindow_WorldMap")
OUT_CHROME = os.path.join(ROOT, "chrome")
OUT_PREVIEW = os.path.join(ROOT, "preview")
RUNTIME = os.path.abspath(os.path.join(ROOT, "..", "..", "..", "..", "UI", "MapMobDrops"))

MOB_SIZE = (168, 537)
DROP_SIZE = (210, 537)

# Border indices from UIWindow.img/WorldMap/Border
# 0 nw, 1 n, 2 ne, 3 w, 4 e, 5 sw, 6 s, 7 se  (no center fill tile)
TILE_NAMES = {
    "nw": "WorldMap_Border_0.png",
    "n": "WorldMap_Border_1.png",
    "ne": "WorldMap_Border_2.png",
    "w": "WorldMap_Border_3.png",
    "e": "WorldMap_Border_4.png",
    "sw": "WorldMap_Border_5.png",
    "s": "WorldMap_Border_6.png",
    "se": "WorldMap_Border_7.png",
}


def load_tiles():
    tiles = {}
    for key, name in TILE_NAMES.items():
        path = os.path.join(TILE_DIR, name)
        if not os.path.exists(path):
            raise FileNotFoundError(path)
        tiles[key] = Image.open(path).convert("RGBA")
    return tiles


def font(size: int):
    for p in [
        r"C:\Windows\Fonts\msyhbd.ttc",
        r"C:\Windows\Fonts\msyh.ttc",
        r"C:\Windows\Fonts\simhei.ttf",
    ]:
        if os.path.exists(p):
            try:
                return ImageFont.truetype(p, size)
            except Exception:
                pass
    return ImageFont.load_default()


def tile_h(img: Image.Image, tile: Image.Image, x: int, y0: int, y1: int):
    """Vertical tile from y0 to y1 at x (uses tile height as step)."""
    th = max(tile.height, 1)
    y = y0
    while y < y1:
        slice_h = min(th, y1 - y)
        piece = tile if slice_h == th else tile.crop((0, 0, tile.width, slice_h))
        img.paste(piece, (x, y), piece)
        y += slice_h


def tile_w(img: Image.Image, tile: Image.Image, y: int, x0: int, x1: int):
    """Horizontal tile from x0 to x1 at y."""
    tw = max(tile.width, 1)
    x = x0
    while x < x1:
        slice_w = min(tw, x1 - x)
        piece = tile if slice_w == tw else tile.crop((0, 0, slice_w, tile.height))
        img.paste(piece, (x, y), piece)
        x += slice_w


def build_panel(size, title: str, tiles) -> Image.Image:
    w, h = size
    nw, n, ne = tiles["nw"], tiles["n"], tiles["ne"]
    west, east = tiles["w"], tiles["e"]
    sw, s, se = tiles["sw"], tiles["s"], tiles["se"]

    img = Image.new("RGBA", (w, h), (0, 0, 0, 0))

    top_h = max(nw.height, n.height, ne.height)
    bot_h = max(sw.height, s.height, se.height)
    left_w = max(nw.width, west.width, sw.width)
    right_w = max(ne.width, east.width, se.width)

    # Inner fill — classic light parchment / silver-blue window body
    fill = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    fd = ImageDraw.Draw(fill)
    # soft vertical gradient fill inside frame
    for y in range(top_h - 2, h - bot_h + 2):
        t = (y - (top_h - 2)) / max(h - top_h - bot_h + 4, 1)
        # light blue-gray like Border body
        r = int(210 + 20 * (1 - t))
        g = int(218 + 15 * (1 - t))
        b = int(230 + 10 * (1 - t))
        fd.line([(left_w - 1, y), (w - right_w, y)], fill=(r, g, b, 255))
    img = Image.alpha_composite(img, fill)

    # Content well (slightly recessed white list area)
    pad = 6
    well = (
        left_w + pad,
        top_h + 4,
        w - right_w - pad - 1,
        h - bot_h - pad - 1,
    )
    dr = ImageDraw.Draw(img)
    dr.rounded_rectangle(well, radius=4, fill=(248, 250, 252, 255), outline=(160, 170, 185, 255), width=1)

    # Corners
    img.paste(nw, (0, 0), nw)
    img.paste(ne, (w - ne.width, 0), ne)
    img.paste(sw, (0, h - sw.height), sw)
    img.paste(se, (w - se.width, h - se.height), se)

    # Edges
    tile_w(img, n, 0, nw.width, w - ne.width)
    tile_w(img, s, h - s.height, sw.width, w - se.width)
    tile_h(img, west, 0, nw.height, h - sw.height)
    tile_h(img, east, w - east.width, ne.height, h - se.height)

    # Title centered in top chrome band (dark text like WORLD map title style)
    f = font(14)
    bbox = dr.textbbox((0, 0), title, font=f)
    tw, th = bbox[2] - bbox[0], bbox[3] - bbox[1]
    tx = (w - tw) // 2 - bbox[0]
    ty = max(6, (top_h - th) // 2 - bbox[1])
    # subtle shadow
    dr.text((tx + 1, ty + 1), title, font=f, fill=(255, 255, 255, 180))
    dr.text((tx, ty), title, font=f, fill=(40, 50, 70, 255))

    return img


def save(img: Image.Image, path: str):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    img.save(path, "PNG")
    print("wrote", path, img.size)


def main():
    tiles = load_tiles()
    os.makedirs(OUT_CHROME, exist_ok=True)
    os.makedirs(OUT_PREVIEW, exist_ok=True)
    os.makedirs(RUNTIME, exist_ok=True)

    mob = build_panel(MOB_SIZE, "怪物", tiles)
    drop = build_panel(DROP_SIZE, "掉落", tiles)

    save(mob, os.path.join(OUT_CHROME, "mob_panel_backgrnd.png"))
    save(drop, os.path.join(OUT_CHROME, "drop_panel_backgrnd.png"))
    save(mob, os.path.join(RUNTIME, "mob_panel_backgrnd.png"))
    save(drop, os.path.join(RUNTIME, "drop_panel_backgrnd.png"))

    # Preview: left + right only (no center map)
    gap = 16
    sheet = Image.new("RGBA", (mob.width + gap + drop.width, max(mob.height, drop.height)), (60, 70, 85, 255))
    sheet.paste(mob, (0, 0), mob)
    sheet.paste(drop, (mob.width + gap, 0), drop)
    save(sheet, os.path.join(OUT_PREVIEW, "side_panels_only.png"))

    print("done")


if __name__ == "__main__":
    main()
