# Regenerate map-info panel: WorldMap Border chrome + silver-blue palette.
# Upper mob list/detail (taller) + lower 5×5 backpack-slot drop grid + meso bar.
# Title optically centered in Border top chrome ridge ONLY (never into white well).
# Usage: python _gen_map_info_ui.py
from __future__ import annotations

import json
import os
import shutil
from PIL import Image, ImageDraw, ImageFont

ROOT = os.path.dirname(os.path.abspath(__file__))
BORDER_DIR = os.path.abspath(
    os.path.join(ROOT, "..", "..", "map-mob-drops", "resources", "source", "UIWindow_WorldMap")
)
OUT = ROOT
RUNTIME = os.path.abspath(os.path.join(ROOT, "..", "..", "..", "..", "UI", "MapInfo"))
CLIENT_OUT = r"E:\MXD\BeiDou-Client-xy\UI\MapInfo"

PANEL_W, PANEL_H = 240, 537

# Native Border-sampled palette (same as map-search)
C_OUTLINE = (85, 119, 153, 255)  # #557799
C_MID = (153, 187, 204, 255)  # #99BBCC
C_LIGHT = (187, 204, 221, 255)  # #BBCCDD
C_WELL = (248, 250, 252, 255)
C_WELL_LINE = (160, 170, 185, 255)
C_SLOT = (255, 255, 255, 255)
C_SLOT_LINE = (136, 153, 187, 255)  # #8899BB
C_RECESS = (204, 212, 221, 255)
C_TEXT = (40, 50, 70, 255)
C_TEXT_DIM = (90, 100, 120, 255)
C_TITLE_SHADOW = (255, 255, 255, 180)
C_TAB_OFF = (170, 187, 204, 255)
C_TAB_OFF_LINE = (85, 119, 153, 255)
C_TAB_ON = (255, 255, 255, 255)
C_TAB_ON_LINE = (85, 119, 153, 255)
C_SCROLL = (187, 198, 210, 255)
C_ROW_SEL = (187, 204, 221, 160)
C_ICON_FILL = (230, 236, 242, 255)
C_RATE = (255, 255, 255, 255)
C_RATE_SHADOW = (20, 30, 45, 220)

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

TABS = [
    ("all", "全部"),
    ("equip", "装备"),
    ("consume", "消耗"),
    ("etc", "其他"),
]

# Layout — backpack slot = UIItemInventory disabled icon default 32×32
ROW_H = 78
VISIBLE_MOB_ROWS = 3
ICON_BOX = 40
SLOT_SIZE = 32  # match inventory icon_width/icon_height fallback
SLOT_GAP = 2
GRID_COLS = 5
GRID_ROWS = 5
SCROLL_W = 10
MESO_H = 16
TAB_H = 18


def font(size: int, bold: bool = True):
    cands = [
        r"C:\Windows\Fonts\simsun.ttc",
        r"C:\Windows\Fonts\msyhbd.ttc" if bold else r"C:\Windows\Fonts\msyh.ttc",
        r"C:\Windows\Fonts\msyh.ttc",
        r"C:\Windows\Fonts\simhei.ttf",
    ]
    for p in cands:
        if os.path.exists(p):
            try:
                return ImageFont.truetype(p, size)
            except Exception:
                pass
    return ImageFont.load_default()


def load_tiles():
    tiles = {}
    for k, name in TILE_NAMES.items():
        path = os.path.join(BORDER_DIR, name)
        if not os.path.exists(path):
            raise FileNotFoundError(path)
        tiles[k] = Image.open(path).convert("RGBA")
    return tiles


def tile_h(img, tile, x, y0, y1):
    th = max(tile.height, 1)
    y = y0
    while y < y1:
        sh = min(th, y1 - y)
        piece = tile if sh == th else tile.crop((0, 0, tile.width, sh))
        img.paste(piece, (x, y), piece)
        y += sh


def tile_w(img, tile, y, x0, x1):
    tw = max(tile.width, 1)
    x = x0
    while x < x1:
        sw = min(tw, x1 - x)
        piece = tile if sw == tw else tile.crop((0, 0, sw, tile.height))
        img.paste(piece, (x, y), piece)
        x += sw


def save(img: Image.Image, rel: str):
    path = os.path.join(OUT, rel) if not os.path.isabs(rel) else rel
    os.makedirs(os.path.dirname(path), exist_ok=True)
    img.save(path, "PNG")
    print("wrote", path, img.size)
    return path


def rect(dr, box, fill, outline, width=1):
    dr.rectangle(box, fill=fill, outline=outline, width=width)


def frame_panel(size, tiles):
    """Border chrome + single outer well; returns (img, metrics)."""
    w, h = size
    nw, n, ne = tiles["nw"], tiles["n"], tiles["ne"]
    west, east = tiles["w"], tiles["e"]
    sw, s, se = tiles["sw"], tiles["s"], tiles["se"]

    top_h = max(nw.height, n.height, ne.height)
    bot_h = max(sw.height, s.height, se.height)
    left_w = max(nw.width, west.width, sw.width)
    right_w = max(ne.width, east.width, se.width)

    img = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    fill = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    fd = ImageDraw.Draw(fill)
    for y in range(top_h - 2, h - bot_h + 2):
        t = (y - (top_h - 2)) / max(h - top_h - bot_h + 4, 1)
        r = int(210 + 20 * (1 - t))
        g = int(218 + 15 * (1 - t))
        b = int(230 + 10 * (1 - t))
        fd.line([(left_w - 1, y), (w - right_w, y)], fill=(r, g, b, 255))
    img = Image.alpha_composite(img, fill)

    pad = 6
    well = (left_w + pad, top_h + 4, w - right_w - pad - 1, h - bot_h - pad - 1)
    dr = ImageDraw.Draw(img)
    rect(dr, well, C_WELL, C_WELL_LINE)

    img.paste(nw, (0, 0), nw)
    img.paste(ne, (w - ne.width, 0), ne)
    img.paste(sw, (0, h - sw.height), sw)
    img.paste(se, (w - se.width, h - se.height), se)
    tile_w(img, n, 0, nw.width, w - ne.width)
    tile_w(img, s, h - s.height, sw.width, w - se.width)
    tile_h(img, west, 0, nw.height, h - sw.height)
    tile_h(img, east, w - east.width, ne.height, h - se.height)

    metrics = {
        "top_h": top_h,
        "bot_h": bot_h,
        "left_w": left_w,
        "right_w": right_w,
        "well": well,
    }
    return img, metrics


def draw_title(img, metrics, title: str):
    """
    Vertically place title in Border top chrome ridge ONLY.
    Optical center of the bevel sits in the upper ~2/3 of top_h (~33);
    hard-clamp ink so it never enters the white well (well @ top_h+4).
    Matches intent of map-search fix + mob_panel ``ty = max(6, ...)`` but biases up.
    """
    dr = ImageDraw.Draw(img)
    f = font(14, True)
    top_h = int(metrics["top_h"])
    bbox = dr.textbbox((0, 0), title, font=f)
    tw, th = bbox[2] - bbox[0], bbox[3] - bbox[1]
    tx = (img.width - tw) // 2 - bbox[0]

    # Upper chrome ridge only — leave lower ~12px of top_h clear of title ink
    band_top = 3
    band_bot = max(band_top + th, top_h - 12)
    usable = max(th, band_bot - band_top)
    ty = band_top + (usable - th) // 2 - bbox[1]
    # Extra optical lift (Border bevel reads higher than geometric mid)
    ty -= 2

    ink_top = ty + bbox[1]
    ink_bot = ty + bbox[3]
    if ink_bot > band_bot:
        ty -= ink_bot - band_bot
        ink_top = ty + bbox[1]
        ink_bot = ty + bbox[3]
    if ink_top < band_top:
        ty += band_top - ink_top
        ink_top = ty + bbox[1]
        ink_bot = ty + bbox[3]
    # Absolute safety: never reach white well
    well_y = top_h + 4
    if ink_bot >= well_y - 2:
        ty -= ink_bot - (well_y - 3)
        ink_bot = ty + bbox[3]

    dr.text((tx + 1, ty + 1), title, font=f, fill=C_TITLE_SHADOW)
    dr.text((tx, ty), title, font=f, fill=C_TEXT)
    print(
        f"title '{title}' top_h={top_h} ty={ty} ink=[{ty + bbox[1]},{ty + bbox[3]}] "
        f"band=[{band_top},{band_bot}] well_y={well_y}"
    )


def draw_scroll_track(dr, x0, y0, x1, y1):
    """Right-edge scrollbar track inside a list/grid well."""
    track = (x1 - SCROLL_W - 1, y0 + 2, x1 - 2, y1 - 2)
    rect(dr, track, C_SCROLL, C_SLOT_LINE)
    return track


def make_slot(size: int = SLOT_SIZE) -> Image.Image:
    img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    dr = ImageDraw.Draw(img)
    rect(dr, (0, 0, size - 1, size - 1), C_SLOT, C_SLOT_LINE)
    dr.line([(1, 1), (size - 2, 1)], fill=(255, 255, 255, 220))
    dr.line([(1, 1), (1, size - 2)], fill=(255, 255, 255, 180))
    return img


def draw_rate_overlay(dr, sx: int, sy: int, text: str, f):
    """Quantity-style rate badge at bottom-right of a slot (preview / runtime pattern)."""
    bbox = dr.textbbox((0, 0), text, font=f)
    tw, th = bbox[2] - bbox[0], bbox[3] - bbox[1]
    tx = sx + SLOT_SIZE - 2 - tw - bbox[0]
    ty = sy + SLOT_SIZE - 2 - th - bbox[1]
    dr.text((tx + 1, ty + 1), text, font=f, fill=C_RATE_SHADOW)
    dr.text((tx, ty), text, font=f, fill=C_RATE)


def build_info_backgrnd(tiles) -> tuple[Image.Image, dict]:
    img, m = frame_panel((PANEL_W, PANEL_H), tiles)
    draw_title(img, m, "地图信息")
    dr = ImageDraw.Draw(img)

    wx0, wy0, wx1, wy1 = m["well"]
    gap = 5
    ix0 = wx0 + gap
    ix1 = wx1 - gap
    y = wy0 + gap
    content_w = ix1 - ix0

    layout: dict = {
        "panel_w": PANEL_W,
        "panel_h": PANEL_H,
        "well": list(m["well"]),
        "content_x": ix0,
        "content_right": ix1,
        "row_h": ROW_H,
        "visible_mob_rows": VISIBLE_MOB_ROWS,
        "icon_box": ICON_BOX,
        "slot_size": SLOT_SIZE,
        "slot_gap": SLOT_GAP,
        "scroll_w": SCROLL_W,
        "grid_cols": GRID_COLS,
        "grid_rows": GRID_ROWS,
        "inventory_slot_note": "SLOT_SIZE=32 matches UIItemInventory disabled icon default (icon_width/height)",
    }

    # Reserve bottom: meso + lower 5×5, then upper takes the rest (taller list/detail)
    meso_label_w = 40
    meso_band_h = MESO_H + 4
    meso_y = wy1 - gap - meso_band_h

    grid_pad = 3
    lower_need = grid_pad * 2 + GRID_ROWS * SLOT_SIZE + (GRID_ROWS - 1) * SLOT_GAP
    tab_block = TAB_H + 2 + 5  # shelf + gap below tabs into lower
    # y after upper; compute upper_h so lower fits exactly 5×5
    # stack from bottom: meso <- lower <- tabs <- upper
    lower_bottom = meso_y - 4
    lower_top = lower_bottom - lower_need
    tab_y = lower_top - 5 - (TAB_H + 2)
    upper_bottom = tab_y - 5
    upper_h = upper_bottom - y
    if upper_h < VISIBLE_MOB_ROWS * 60:
        # fallback: shrink row feeling but keep 5×5
        upper_h = max(180, upper_bottom - y)

    # --- Upper: taller mob list/detail well ---
    upper_box = (ix0, y, ix1, y + upper_h)
    rect(dr, upper_box, C_SLOT, C_SLOT_LINE)
    upper_track = draw_scroll_track(dr, ix0, y, ix1, y + upper_h)
    list_inner_w = content_w - SCROLL_W - 4
    layout["upper"] = {
        "x": ix0,
        "y": y,
        "w": content_w,
        "h": upper_h,
        "list_w": list_inner_w,
        "row_h": ROW_H,
        "visible_rows": VISIBLE_MOB_ROWS,
        "fields": ["name", "level", "type", "hp", "exp", "attrs", "resists"],
        "scroll": {
            "x": upper_track[0],
            "y": upper_track[1],
            "w": upper_track[2] - upper_track[0],
            "h": upper_track[3] - upper_track[1],
        },
    }
    y = y + upper_h + 5

    # --- Tab shelf ---
    shelf = (ix0, y, ix1, y + TAB_H + 2)
    rect(dr, shelf, C_LIGHT, C_SLOT_LINE)
    n_tabs = len(TABS)
    tab_w = content_w // n_tabs
    tab_xs, widths = [], []
    x = ix0
    for i in range(n_tabs):
        tw = tab_w if i < n_tabs - 1 else (ix1 - x)
        tab_xs.append(x)
        widths.append(tw)
        x += tw
    layout["tab"] = {
        "x": ix0,
        "y": y + 1,
        "h": TAB_H,
        "widths": widths,
        "xs": tab_xs,
        "shelf_y": y,
    }
    y += TAB_H + 2 + 5

    # --- Lower: fixed 5×5 drop grid ---
    lower_h = meso_y - 4 - y
    if lower_h < lower_need:
        lower_h = lower_need
        meso_y = y + lower_h + 4
    lower_box = (ix0, y, ix1, y + lower_h)
    rect(dr, lower_box, C_SLOT, C_SLOT_LINE)
    lower_track = draw_scroll_track(dr, ix0, y, ix1, y + lower_h)

    grid_right = ix1 - SCROLL_W - 3
    grid_left = ix0 + 3
    grid_top = y + 3
    usable_w = grid_right - grid_left
    cols = GRID_COLS
    rows_vis = GRID_ROWS
    grid_w = cols * SLOT_SIZE + (cols - 1) * SLOT_GAP
    grid_h = rows_vis * SLOT_SIZE + (rows_vis - 1) * SLOT_GAP
    gx0 = grid_left + max(0, (usable_w - grid_w) // 2)
    # Vertically center 5×5 inside lower well if extra space
    gy0 = grid_top + max(0, ((y + lower_h - 3) - grid_top - grid_h) // 2)

    slot_positions = []
    for r in range(rows_vis):
        for c in range(cols):
            sx = gx0 + c * (SLOT_SIZE + SLOT_GAP)
            sy = gy0 + r * (SLOT_SIZE + SLOT_GAP)
            rect(dr, (sx, sy, sx + SLOT_SIZE - 1, sy + SLOT_SIZE - 1), C_SLOT, C_SLOT_LINE)
            dr.line([(sx + 1, sy + 1), (sx + SLOT_SIZE - 2, sy + 1)], fill=(255, 255, 255, 200))
            slot_positions.append({"x": sx, "y": sy, "col": c, "row": r})

    layout["lower"] = {
        "x": ix0,
        "y": y,
        "w": content_w,
        "h": lower_h,
        "cols": cols,
        "rows_visible": rows_vis,
        "slot_size": SLOT_SIZE,
        "slot_gap": SLOT_GAP,
        "grid_x": gx0,
        "grid_y": gy0,
        "slots": slot_positions,
        "rate_overlay": {
            "style": "quantity_corner",
            "anchor": "bottom_right",
            "data": "MobDropEntry.chance → percent text at runtime (preview uses placeholders)",
            "note": "wire to real drop rate later; do not bake rates into chrome PNG",
        },
        "scroll": {
            "x": lower_track[0],
            "y": lower_track[1],
            "w": lower_track[2] - lower_track[0],
            "h": lower_track[3] - lower_track[1],
        },
    }

    # --- Bottom meso bar (backpack gold field) ---
    fy = meso_y + (meso_band_h - MESO_H) // 2
    meso_field_w = content_w - meso_label_w - 4
    meso_box = (ix0, fy, ix0 + meso_field_w, fy + MESO_H)
    rect(dr, meso_box, C_SLOT, C_SLOT_LINE)
    dr.line([(ix0 + 1, fy + 1), (ix0 + meso_field_w - 1, fy + 1)], fill=(255, 255, 255, 200))
    f = font(11, True)
    label = "mesos"
    bbox = dr.textbbox((0, 0), label, font=f)
    lw, lh = bbox[2] - bbox[0], bbox[3] - bbox[1]
    lx = ix0 + meso_field_w + 4 - bbox[0]
    ly = fy + (MESO_H - lh) // 2 - bbox[1]
    dr.text((lx, ly), label, font=f, fill=C_TEXT_DIM)
    layout["meso"] = {
        "x": ix0,
        "y": fy,
        "w": meso_field_w,
        "h": MESO_H,
        "label_x": lx,
        "label": label,
        "text_align": "right",
        "note": "runtime draws meso amount; do not bake digits into chrome",
        "drop_interval_note": (
            "MobDropEntry has min_qty/max_qty (range-capable) but no dedicated meso_min/meso_max; "
            "stub loader empty — gold drop interval not populated in this client build"
        ),
    }

    return img, layout


def make_tab(label: str, enabled: bool, tw: int, th: int = TAB_H) -> Image.Image:
    img = Image.new("RGBA", (tw, th), (0, 0, 0, 0))
    dr = ImageDraw.Draw(img)
    if enabled:
        fill, outline, hi, text = C_TAB_ON, C_TAB_ON_LINE, (255, 255, 255, 255), C_TEXT
    else:
        fill, outline, hi, text = C_TAB_OFF, C_TAB_OFF_LINE, C_MID, C_TEXT_DIM

    rect(dr, (0, 0, tw - 1, th - 1), fill, outline)
    dr.line([(1, 1), (tw - 2, 1)], fill=hi)
    if enabled:
        dr.line([(1, 1), (1, th - 2)], fill=(255, 255, 255, 220))
    else:
        dr.line([(1, th - 2), (tw - 2, th - 2)], fill=(120, 140, 160, 180))

    f = font(11, True)
    bbox = dr.textbbox((0, 0), label, font=f)
    lw, lh = bbox[2] - bbox[0], bbox[3] - bbox[1]
    tx = (tw - lw) // 2 - bbox[0]
    ty = (th - lh) // 2 - bbox[1]
    dr.text((tx, ty), label, font=f, fill=text)
    return img


def make_list_row_selected(width: int, height: int = ROW_H) -> Image.Image:
    img = Image.new("RGBA", (width, height), (0, 0, 0, 0))
    dr = ImageDraw.Draw(img)
    rect(dr, (0, 0, width - 1, height - 1), C_ROW_SEL, C_OUTLINE)
    return img


def compose_preview(panel: Image.Image, layout: dict, tabs: dict) -> Image.Image:
    prev = panel.copy()
    xs = layout["tab"]["xs"]
    widths = layout["tab"]["widths"]
    ty = layout["tab"]["y"]
    for i, (key, _) in enumerate(TABS):
        name = f"{key}_enabled" if i == 0 else f"{key}_disabled"
        t = tabs[name]
        tx = xs[i] + (widths[i] - t.width) // 2
        prev.paste(t, (tx, ty), t)

    dr = ImageDraw.Draw(prev)
    f = font(11, True)
    meso = layout["meso"]
    sample = "12,345,678"
    bbox = dr.textbbox((0, 0), sample, font=f)
    tw = bbox[2] - bbox[0]
    th = bbox[3] - bbox[1]
    tx = meso["x"] + meso["w"] - 4 - tw - bbox[0]
    ty = meso["y"] + (meso["h"] - th) // 2 - bbox[1]
    dr.text((tx, ty), sample, font=f, fill=C_TEXT)

    # Sample mob rows with attrs/resists placeholders (preview only)
    u = layout["upper"]
    row_h = layout.get("row_h", ROW_H)
    f12 = font(12, True)
    f10 = font(10, False)
    f9 = font(9, False)
    samples = [
        ("蓝蜗牛", "Lv.1  普通", "HP 15   EXP 3", "属性: 无", "抗性: 无"),
        ("红蜗牛", "Lv.2  普通", "HP 40   EXP 5", "属性: 火弱", "抗性: —"),
        ("蘑菇王", "Lv.35  BOSS", "HP 20000  EXP 2100", "属性: —", "抗性: 冰/毒"),
    ]
    for i, (name, meta, stats, attrs, resists) in enumerate(samples):
        ry = u["y"] + i * row_h
        if ry + row_h > u["y"] + u["h"]:
            break
        if i > 0:
            dr.line(
                [(u["x"] + 1, ry), (u["x"] + u["list_w"] - 1, ry)],
                fill=C_RECESS,
                width=1,
            )
        iy = ry + (row_h - ICON_BOX) // 2
        rect(
            dr,
            (u["x"] + 4, iy, u["x"] + 4 + ICON_BOX, iy + ICON_BOX),
            C_ICON_FILL,
            C_OUTLINE,
        )
        ix = u["x"] + 4 + ICON_BOX + 6
        dr.text((ix, ry + 4), name, font=f12, fill=C_TEXT)
        dr.text((ix, ry + 20), meta, font=f10, fill=C_TEXT_DIM)
        dr.text((ix, ry + 34), stats, font=f10, fill=C_TEXT_DIM)
        dr.text((ix, ry + 48), attrs, font=f9, fill=C_TEXT_DIM)
        dr.text((ix, ry + 60), resists, font=f9, fill=C_TEXT_DIM)

    # Sample drop icons + rate overlays (preview only — not baked into chrome)
    lo = layout["lower"]
    f_rate = font(9, True)
    rate_samples = ["45%", "10%", "2%", "0.1%", "8%"]
    for i, slot in enumerate(lo["slots"][:5]):
        sx, sy = slot["x"], slot["y"]
        # fake item tile
        rect(
            dr,
            (sx + 4, sy + 4, sx + SLOT_SIZE - 5, sy + SLOT_SIZE - 5),
            C_ICON_FILL,
            C_OUTLINE,
        )
        draw_rate_overlay(dr, sx, sy, rate_samples[i % len(rate_samples)], f_rate)

    return prev


def main():
    tiles = load_tiles()
    for d in ("chrome", "Tab/enabled", "Tab/disabled", "preview"):
        os.makedirs(os.path.join(OUT, d), exist_ok=True)
    os.makedirs(os.path.join(RUNTIME, "Tab", "enabled"), exist_ok=True)
    os.makedirs(os.path.join(RUNTIME, "Tab", "disabled"), exist_ok=True)
    os.makedirs(RUNTIME, exist_ok=True)

    panel, layout = build_info_backgrnd(tiles)
    save(panel, "chrome/info_panel_backgrnd.png")
    save(make_slot(), "chrome/slot.png")

    list_w = layout["upper"]["list_w"]
    save(make_list_row_selected(list_w), "chrome/list_row_selected.png")

    tab_h = layout["tab"]["h"]
    widths = layout["tab"]["widths"]
    tabs = {}
    for i, (key, label) in enumerate(TABS):
        tw = widths[i]
        dis = make_tab(label, False, tw, tab_h)
        en = make_tab(label, True, tw, tab_h)
        save(dis, f"Tab/disabled/{key}.png")
        save(en, f"Tab/enabled/{key}.png")
        tabs[f"{key}_disabled"] = dis
        tabs[f"{key}_enabled"] = en

    layout["tab"]["sprite_w"] = widths
    layout["tab"]["sprite_h"] = tab_h

    alone = compose_preview(panel, layout, tabs)
    save(alone, "preview/info_panel_alone.png")

    border_w = 400
    gap = 8
    ctx_w = PANEL_W + gap + border_w
    ctx = Image.new("RGBA", (ctx_w, PANEL_H), (55, 62, 75, 255))
    dr = ImageDraw.Draw(ctx)
    dr.rectangle(
        (PANEL_W + gap, 0, ctx_w - 1, PANEL_H - 1),
        fill=(70, 95, 120, 255),
        outline=(90, 120, 150, 255),
    )
    f = font(12)
    dr.text(
        (PANEL_W + gap + 140, PANEL_H // 2 - 8),
        "世界地图 Border",
        font=f,
        fill=(200, 220, 235, 255),
    )
    ctx.paste(alone, (0, 0), alone)
    save(ctx, "preview/dock_context.png")

    with open(os.path.join(OUT, "layout.json"), "w", encoding="utf-8") as fp:
        json.dump(layout, fp, ensure_ascii=False, indent=2)
    print(
        "layout upper_h",
        layout["upper"]["h"],
        "lower",
        layout["lower"]["cols"],
        "x",
        layout["lower"]["rows_visible"],
        "slot",
        layout["slot_size"],
    )

    Image.open(os.path.join(OUT, "chrome", "info_panel_backgrnd.png")).save(
        os.path.join(RUNTIME, "info_panel_backgrnd.png")
    )
    Image.open(os.path.join(OUT, "chrome", "slot.png")).save(os.path.join(RUNTIME, "slot.png"))
    Image.open(os.path.join(OUT, "chrome", "list_row_selected.png")).save(
        os.path.join(RUNTIME, "list_row_selected.png")
    )
    for key, _ in TABS:
        Image.open(os.path.join(OUT, "Tab", "enabled", f"{key}.png")).save(
            os.path.join(RUNTIME, "Tab", "enabled", f"{key}.png")
        )
        Image.open(os.path.join(OUT, "Tab", "disabled", f"{key}.png")).save(
            os.path.join(RUNTIME, "Tab", "disabled", f"{key}.png")
        )
    shutil.copy2(os.path.join(OUT, "layout.json"), os.path.join(RUNTIME, "layout.json"))

    client_ui = os.path.dirname(CLIENT_OUT)
    if os.path.isdir(client_ui):
        os.makedirs(CLIENT_OUT, exist_ok=True)
        os.makedirs(os.path.join(CLIENT_OUT, "Tab", "enabled"), exist_ok=True)
        os.makedirs(os.path.join(CLIENT_OUT, "Tab", "disabled"), exist_ok=True)
        for name in ("info_panel_backgrnd.png", "slot.png", "list_row_selected.png", "layout.json"):
            shutil.copy2(os.path.join(RUNTIME, name), os.path.join(CLIENT_OUT, name))
        for key, _ in TABS:
            shutil.copy2(
                os.path.join(RUNTIME, "Tab", "enabled", f"{key}.png"),
                os.path.join(CLIENT_OUT, "Tab", "enabled", f"{key}.png"),
            )
            shutil.copy2(
                os.path.join(RUNTIME, "Tab", "disabled", f"{key}.png"),
                os.path.join(CLIENT_OUT, "Tab", "disabled", f"{key}.png"),
            )
        print("synced CLIENT", CLIENT_OUT)
    else:
        print("skip CLIENT (parent missing):", client_ui)

    print("done PANEL", PANEL_W, PANEL_H, "RUNTIME", RUNTIME)


if __name__ == "__main__":
    main()
