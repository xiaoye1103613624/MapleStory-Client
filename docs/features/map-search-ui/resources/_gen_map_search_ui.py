# Regenerate map-search panel: WorldMap Border chrome + native silver-blue palette.
# Matches mob/drop side panels; exports layout.json for UIMapSearchPanel hitboxes.
# Usage: python _gen_map_search_ui.py
from __future__ import annotations

import json
import os
from PIL import Image, ImageDraw, ImageFont

ROOT = os.path.dirname(os.path.abspath(__file__))
BORDER_DIR = os.path.abspath(
    os.path.join(ROOT, "..", "..", "map-mob-drops", "resources", "source", "UIWindow_WorldMap")
)
OUT = ROOT
RUNTIME = os.path.abspath(os.path.join(ROOT, "..", "..", "..", "..", "UI", "MapSearch"))

PANEL_W, PANEL_H = 260, 537

# Native Border-sampled palette (UIWindow.img/WorldMap/Border)
C_OUTLINE = (85, 119, 153, 255)       # #557799
C_MID = (153, 187, 204, 255)          # #99BBCC
C_LIGHT = (187, 204, 221, 255)        # #BBCCDD
C_WELL = (248, 250, 252, 255)         # same as mob panel well
C_WELL_LINE = (160, 170, 185, 255)
C_SLOT = (255, 255, 255, 255)
C_SLOT_LINE = (136, 153, 187, 255)    # #8899BB
C_RECESS = (204, 212, 221, 255)
C_TEXT = (40, 50, 70, 255)
C_TEXT_DIM = (90, 100, 120, 255)
C_TITLE_SHADOW = (255, 255, 255, 180)
C_TAB_OFF = (170, 187, 204, 255)
C_TAB_OFF_LINE = (85, 119, 153, 255)
C_TAB_ON = (255, 255, 255, 255)
C_TAB_ON_LINE = (85, 119, 153, 255)
C_BTN = (204, 221, 238, 255)
C_BTN_HI = (238, 245, 255, 255)
C_BTN_LINE = (85, 119, 153, 255)
C_BTN_TEXT = (40, 50, 70, 255)
C_BTN_PRESS = (153, 170, 187, 255)
C_BTN_DIS = (210, 215, 220, 255)
C_BTN_DIS_TEXT = (140, 145, 150, 255)
C_SCROLL = (187, 198, 210, 255)

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
    ("map", "地图"),
    ("mob", "怪物"),
    ("drop", "掉落"),
    ("npc", "NPC"),
]


def font(size: int, bold: bool = True):
    cands = [
        r"C:\Windows\Fonts\simsun.ttc",  # classic Maple-like bitmap feel
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
    """Sharp 1px native-style rectangle (no modern large radius)."""
    dr.rectangle(box, fill=fill, outline=outline, width=width)


def frame_panel(size, tiles):
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
    # Same body gradient as mob/drop side panels
    for y in range(top_h - 2, h - bot_h + 2):
        t = (y - (top_h - 2)) / max(h - top_h - bot_h + 4, 1)
        r = int(210 + 20 * (1 - t))
        g = int(218 + 15 * (1 - t))
        b = int(230 + 10 * (1 - t))
        fd.line([(left_w - 1, y), (w - right_w, y)], fill=(r, g, b, 255))
    img = Image.alpha_composite(img, fill)

    # Single content well (mob-panel style)
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
    """Vertically center title in Border top chrome only (never into white well)."""
    dr = ImageDraw.Draw(img)
    # Same size as mob/drop side-panel titles
    f = font(14, True)
    top_h = int(metrics["top_h"])  # Border NW/N/NE ~33
    bbox = dr.textbbox((0, 0), title, font=f)
    tw, th = bbox[2] - bbox[0], bbox[3] - bbox[1]
    tx = (img.width - tw) // 2 - bbox[0]
    # Center ink inside [2, top_h-2]; well starts at top_h+4
    band_pad = 2
    usable = max(th, top_h - band_pad * 2)
    ty = band_pad + (usable - th) // 2 - bbox[1]
    ink_top = ty + bbox[1]
    ink_bot = ty + bbox[3]
    if ink_bot > top_h - band_pad:
        ty -= ink_bot - (top_h - band_pad)
    if ink_top < band_pad:
        ty += band_pad - ink_top
    dr.text((tx + 1, ty + 1), title, font=f, fill=C_TITLE_SHADOW)
    dr.text((tx, ty), title, font=f, fill=C_TEXT)
    print(f"title '{title}' top_h={top_h} ty={ty} ink=[{ty + bbox[1]},{ty + bbox[3]}]")


def build_search_backgrnd(tiles) -> tuple[Image.Image, dict]:
    img, m = frame_panel((PANEL_W, PANEL_H), tiles)
    draw_title(img, m, "地图搜索")
    dr = ImageDraw.Draw(img)

    wx0, wy0, wx1, wy1 = m["well"]
    # Inner content inset aligned to well (same left/right column)
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
    }

    # --- Search field ---
    search_h = 18
    search_box = (ix0, y, ix1, y + search_h)
    rect(dr, search_box, C_SLOT, C_SLOT_LINE)
    # inner highlight (native bevel)
    dr.line([(ix0 + 1, y + 1), (ix1 - 1, y + 1)], fill=(255, 255, 255, 200))
    layout["search"] = {"x": ix0, "y": y, "w": content_w, "h": search_h}
    y += search_h + 4

    # --- Tab shelf only (inventory-style: Tab sprites overlay, not baked) ---
    tab_h = 18
    shelf = (ix0, y, ix1, y + tab_h + 2)
    rect(dr, shelf, C_LIGHT, C_SLOT_LINE)
    # Equal pitch slots for layout / hitboxes; sprites drawn separately
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
        "h": tab_h,
        "widths": widths,
        "xs": tab_xs,
        "shelf_y": y,
    }
    y += tab_h + 2 + 5

    # --- Nav: back + subtitle (same height, shared baseline) ---
    nav_h = 18
    back_w = 44
    back_box = (ix0, y, ix0 + back_w, y + nav_h)
    rect(dr, back_box, C_RECESS, C_SLOT_LINE)
    # vertical divider
    div_x = ix0 + back_w + 3
    dr.line([(div_x, y + 2), (div_x, y + nav_h - 2)], fill=C_OUTLINE, width=1)
    sub_x = div_x + 4
    sub_box = (sub_x, y, ix1, y + nav_h)
    rect(dr, sub_box, C_SLOT, C_SLOT_LINE)
    layout["back"] = {"x": ix0, "y": y, "w": back_w, "h": nav_h}
    layout["subtitle"] = {"x": sub_x, "y": y, "w": ix1 - sub_x, "h": nav_h}
    y += nav_h + 5

    # --- Detail card ---
    footer_h = 28
    list_header_h = 16
    remain = wy1 - gap - y - footer_h - 4
    detail_h = max(72, int(remain * 0.32))
    list_h = remain - detail_h - list_header_h - 4

    detail_box = (ix0, y, ix1, y + detail_h)
    rect(dr, detail_box, C_SLOT, C_SLOT_LINE)
    icon = 48
    icon_box = (ix0 + 4, y + 4, ix0 + 4 + icon, y + 4 + icon)
    rect(dr, icon_box, C_LIGHT, C_OUTLINE)
    # attr guide lines
    ax = ix0 + 4 + icon + 8
    for i in range(4):
        ly = y + 10 + i * 14
        dr.line([(ax, ly + 8), (ix1 - 6, ly + 8)], fill=C_RECESS, width=1)
    layout["detail"] = {"x": ix0, "y": y, "w": content_w, "h": detail_h, "icon": 48}
    y += detail_h + 4

    # --- List header ---
    hdr = (ix0, y, ix1, y + list_header_h)
    rect(dr, hdr, C_LIGHT, C_SLOT_LINE)
    layout["list_header"] = {"x": ix0, "y": y, "w": content_w, "h": list_header_h}
    y += list_header_h + 2

    # --- List well ---
    list_box = (ix0, y, ix1, y + list_h)
    rect(dr, list_box, C_SLOT, C_SLOT_LINE)
    track_x = ix1 - 10
    rect(dr, (track_x, y + 2, ix1 - 2, y + list_h - 2), C_SCROLL, C_SLOT_LINE)
    layout["list"] = {"x": ix0, "y": y, "w": content_w, "h": list_h}
    y += list_h + 4

    # --- Footer: cost + teleport (shared baseline) ---
    tele_w = 64
    tele_h = 22
    cost_h = tele_h
    # vertically center within footer band
    fy = y + (footer_h - tele_h) // 2
    cost_box = (ix0, fy, ix1 - tele_w - 4, fy + cost_h)
    tele_box = (ix1 - tele_w, fy, ix1, fy + tele_h)
    rect(dr, cost_box, C_SLOT, C_SLOT_LINE)
    rect(dr, tele_box, C_RECESS, C_SLOT_LINE)
    layout["cost"] = {"x": cost_box[0], "y": fy, "w": cost_box[2] - cost_box[0], "h": cost_h}
    layout["teleport"] = {"x": tele_box[0], "y": fy, "w": tele_w, "h": tele_h}

    return img, layout


def make_tab(label: str, enabled: bool, tw: int, th: int = 18) -> Image.Image:
    """Inventory-like tab: white raised when on, silver recessed when off."""
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
        # recessed bottom shadow
        dr.line([(1, th - 2), (tw - 2, th - 2)], fill=(120, 140, 160, 180))

    f = font(11, True)
    bbox = dr.textbbox((0, 0), label, font=f)
    lw, lh = bbox[2] - bbox[0], bbox[3] - bbox[1]
    tx = (tw - lw) // 2 - bbox[0]
    ty = (th - lh) // 2 - bbox[1]
    dr.text((tx, ty), label, font=f, fill=text)
    return img


def make_button(label: str, state: str, bw: int, bh: int) -> Image.Image:
    img = Image.new("RGBA", (bw, bh), (0, 0, 0, 0))
    dr = ImageDraw.Draw(img)

    styles = {
        "normal": (C_BTN, C_BTN_LINE, C_BTN_TEXT, C_BTN_HI, 0),
        "mouseOver": (C_BTN_HI, C_OUTLINE, C_TEXT, (255, 255, 255, 255), 0),
        "pressed": (C_BTN_PRESS, C_OUTLINE, C_TEXT, C_MID, 1),
        "disabled": (C_BTN_DIS, (150, 155, 160, 255), C_BTN_DIS_TEXT, C_BTN_DIS, 0),
    }
    fill, outline, text, hi, shift = styles[state]
    rect(dr, (0, 0, bw - 1, bh - 1), fill, outline)
    if state != "pressed" and state != "disabled":
        dr.line([(1, 1), (bw - 2, 1)], fill=hi)
        dr.line([(1, 1), (1, bh - 2)], fill=hi)
    if state == "pressed":
        dr.rectangle((1, 1, bw - 2, bh - 2), outline=(60, 80, 100, 100))

    f = font(11, True)
    bbox = dr.textbbox((0, 0), label, font=f)
    lw, lh = bbox[2] - bbox[0], bbox[3] - bbox[1]
    tx = (bw - lw) // 2 - bbox[0] + shift
    ty = (bh - lh) // 2 - bbox[1] + shift
    if state != "disabled":
        dr.text((tx + 1, ty + 1), label, font=f, fill=C_TITLE_SHADOW)
    dr.text((tx, ty), label, font=f, fill=text)
    return img


def make_detail_icon_frame() -> Image.Image:
    img = Image.new("RGBA", (48, 48), (0, 0, 0, 0))
    dr = ImageDraw.Draw(img)
    rect(dr, (0, 0, 47, 47), C_LIGHT, C_OUTLINE)
    return img


def make_list_row_selected(width: int = 216, height: int = 28) -> Image.Image:
    img = Image.new("RGBA", (width, height), (0, 0, 0, 0))
    dr = ImageDraw.Draw(img)
    rect(dr, (0, 0, width - 1, height - 1), (187, 204, 221, 160), C_OUTLINE)
    return img


def compose_preview(panel: Image.Image, layout: dict, tabs: dict, back_n: Image.Image, tele_n: Image.Image) -> Image.Image:
    prev = panel.copy()
    xs = layout["tab"]["xs"]
    widths = layout["tab"]["widths"]
    ty = layout["tab"]["y"]
    for i, (key, _) in enumerate(TABS):
        name = f"{key}_enabled" if i == 0 else f"{key}_disabled"
        t = tabs[name]
        tx = xs[i] + (widths[i] - t.width) // 2
        prev.paste(t, (tx, ty), t)

    b = layout["back"]
    prev.paste(back_n, (b["x"] + (b["w"] - back_n.width) // 2, b["y"] + (b["h"] - back_n.height) // 2), back_n)

    t = layout["teleport"]
    prev.paste(tele_n, (t["x"] + (t["w"] - tele_n.width) // 2, t["y"] + (t["h"] - tele_n.height) // 2), tele_n)
    return prev


def main():
    tiles = load_tiles()
    for d in ("chrome", "Tab/enabled", "Tab/disabled", "buttons", "preview"):
        os.makedirs(os.path.join(OUT, d), exist_ok=True)
    os.makedirs(os.path.join(RUNTIME, "Tab", "enabled"), exist_ok=True)
    os.makedirs(os.path.join(RUNTIME, "Tab", "disabled"), exist_ok=True)
    os.makedirs(RUNTIME, exist_ok=True)

    panel, layout = build_search_backgrnd(tiles)
    save(panel, "chrome/search_panel_backgrnd.png")
    save(make_detail_icon_frame(), "chrome/detail_icon_frame.png")
    save(make_list_row_selected(), "chrome/list_row_selected.png")

    # WZ-like Tab/enabled|disabled split (inventory pattern)
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

    states = ("normal", "mouseOver", "pressed", "disabled")
    back_imgs = {}
    tele_imgs = {}
    bw, bh = layout["back"]["w"], layout["back"]["h"]
    tw, th = layout["teleport"]["w"], layout["teleport"]["h"]
    for st in states:
        b = make_button("返回", st, bw, bh)
        t = make_button("传送", st, tw, th)
        save(b, f"buttons/BtBack_{st}.png")
        save(t, f"buttons/BtTeleport_{st}.png")
        back_imgs[st] = b
        tele_imgs[st] = t

    alone = compose_preview(panel, layout, tabs, back_imgs["normal"], tele_imgs["normal"])
    save(alone, "preview/search_panel_alone.png")

    old_search_w = 147
    ctx_w = old_search_w + 8 + PANEL_W
    ctx = Image.new("RGBA", (ctx_w, PANEL_H), (55, 62, 75, 255))
    dr = ImageDraw.Draw(ctx)
    dr.rectangle((0, 0, old_search_w - 1, PANEL_H - 1), fill=(40, 40, 45, 255), outline=(90, 90, 100, 255))
    f = font(12)
    dr.text((28, PANEL_H // 2 - 8), "原搜索栏", font=f, fill=(180, 180, 190, 255))
    ctx.paste(alone, (old_search_w + 8, 0), alone)
    save(ctx, "preview/dock_context.png")

    with open(os.path.join(OUT, "layout.json"), "w", encoding="utf-8") as fp:
        json.dump(layout, fp, ensure_ascii=False, indent=2)
    print("layout", layout)

    # Runtime: chrome flat + Tab/enabled|disabled (inventory layout) + buttons
    for name in ("search_panel_backgrnd.png", "detail_icon_frame.png", "list_row_selected.png"):
        Image.open(os.path.join(OUT, "chrome", name)).save(os.path.join(RUNTIME, name))
    for key, _ in TABS:
        Image.open(os.path.join(OUT, "Tab", "enabled", f"{key}.png")).save(
            os.path.join(RUNTIME, "Tab", "enabled", f"{key}.png")
        )
        Image.open(os.path.join(OUT, "Tab", "disabled", f"{key}.png")).save(
            os.path.join(RUNTIME, "Tab", "disabled", f"{key}.png")
        )
    for st in states:
        for prefix in ("BtBack", "BtTeleport"):
            name = f"{prefix}_{st}.png"
            Image.open(os.path.join(OUT, "buttons", name)).save(os.path.join(RUNTIME, name))

    print("done PANEL", PANEL_W, PANEL_H, "RUNTIME", RUNTIME)


if __name__ == "__main__":
    main()
