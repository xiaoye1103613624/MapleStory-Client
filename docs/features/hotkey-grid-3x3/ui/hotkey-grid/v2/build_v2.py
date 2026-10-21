# -*- coding: utf-8 -*-
"""Build hotkey-grid v2 from user reference (pixel-matched 3x3 + opacity + labels)."""
from __future__ import annotations

import os
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

ROOT = Path(r"E:\project\MapleStory-Client\docs\features\hotkey-grid-3x3\ui\hotkey-grid\v2")
REF = ROOT / "ref-quickslot-4x2.png"
OUT = ROOT

CELL_W, CELL_H = 32, 31
GAP_X, GAP_Y = 3, 4
COLS, ROWS = 3, 3
FRAME = 6  # inset matching reference (cells start at 6,6)

# From reference sampling + client OutlinedText / A11M
COL_DARK = (17, 17, 17, 255)
COL_MID = (119, 136, 153, 255)
COL_FILL = (255, 255, 255, 255)
COL_FRAME_EDGE = (68, 68, 85, 255)
COL_FRAME_HI = (255, 255, 255, 255)
# Native key overlay (ref-key-Q): pale lavender fill + white 4-dir outline
COL_LABEL = (204, 204, 221, 255)
COL_LABEL_OUTLINE = (255, 255, 255, 255)
# Client font: fonts/Arial/Arial.ttf @ Text::Font::A11M (11px)
FONT_ARIAL = Path(r"E:\project\MapleStory-Client\fonts\Arial\Arial.ttf")
FONT_SIZE_A11M = 11

LABELS = [
    ["1", "2", "3"],
    ["Insert", "Home", "PageUp"],
    ["Delete", "End", "PageDown"],
]
OPACITIES = [100, 70, 50, 30, 10]


def is_blue_bg(p: tuple[int, int, int, int]) -> bool:
    r, g, b, a = p
    return b > 150 and b > r + 30 and b > g + 20


def crop_board(im: Image.Image) -> Image.Image:
    px = im.load()
    w, h = im.size
    xs, ys = [], []
    for y in range(h):
        for x in range(w):
            if is_blue_bg(px[x, y]):
                continue
            xs.append(x)
            ys.append(y)
    return im.crop((min(xs), min(ys), max(xs) + 1, max(ys) + 1))


def extract_slot(board: Image.Image) -> Image.Image:
    # First slot body at (6,6)-(38,37) => 32x31
    return board.crop((6, 6, 6 + CELL_W, 6 + CELL_H)).convert("RGBA")


def is_near_white(p: tuple[int, int, int, int], thr: int = 230) -> bool:
    r, g, b, a = p
    return a > 0 and r >= thr and g >= thr and b >= thr


def split_slot_fill_chrome(slot: Image.Image) -> tuple[Image.Image, Image.Image]:
    """Split slot into white-ish fill vs chrome (bevel/mid)."""
    fill = Image.new("RGBA", slot.size, (0, 0, 0, 0))
    chrome = Image.new("RGBA", slot.size, (0, 0, 0, 0))
    sp, fp, cp = slot.load(), fill.load(), chrome.load()
    w, h = slot.size
    for y in range(h):
        for x in range(w):
            p = sp[x, y]
            if p[3] == 0:
                continue
            if is_near_white(p):
                fp[x, y] = p
            else:
                cp[x, y] = p
    return fill, chrome


def scrub_blue_bg(im: Image.Image) -> Image.Image:
    out = im.copy()
    px = out.load()
    w, h = out.size
    for y in range(h):
        for x in range(w):
            if is_blue_bg(px[x, y]):
                px[x, y] = (0, 0, 0, 0)
    return out


def build_outer_frame(board: Image.Image, tw: int, th: int) -> Image.Image:
    """9-slice-ish outer frame from reference board (6px rim)."""
    bw, bh = board.size
    f = FRAME
    frame = Image.new("RGBA", (tw, th), (0, 0, 0, 0))

    # Corners from reference
    tl = scrub_blue_bg(board.crop((0, 0, f, f)))
    tr = scrub_blue_bg(board.crop((bw - f, 0, bw, f)))
    bl = scrub_blue_bg(board.crop((0, bh - f, f, bh)))
    br = scrub_blue_bg(board.crop((bw - f, bh - f, bw, bh)))
    frame.paste(tl, (0, 0), tl)
    frame.paste(tr, (tw - f, 0), tr)
    frame.paste(bl, (0, th - f), bl)
    frame.paste(br, (tw - f, th - f), br)

    # Edges (stretch middle segments)
    top = scrub_blue_bg(board.crop((f, 0, bw - f, f))).resize((tw - 2 * f, f), Image.NEAREST)
    bot = scrub_blue_bg(board.crop((f, bh - f, bw - f, bh))).resize((tw - 2 * f, f), Image.NEAREST)
    left = scrub_blue_bg(board.crop((0, f, f, bh - f))).resize((f, th - 2 * f), Image.NEAREST)
    right = scrub_blue_bg(board.crop((bw - f, f, bw, bh - f))).resize((f, th - 2 * f), Image.NEAREST)
    frame.paste(top, (f, 0), top)
    frame.paste(bot, (f, th - f), bot)
    frame.paste(left, (0, f), left)
    frame.paste(right, (tw - f, f), right)
    return frame


def build_panel_base(
    slot: Image.Image, board: Image.Image
) -> tuple[Image.Image, Image.Image, Image.Image]:
    """Return (full, fills_only, chrome_only) for 3x3 panel with outer frame."""
    inner_w = COLS * CELL_W + (COLS - 1) * GAP_X
    inner_h = ROWS * CELL_H + (ROWS - 1) * GAP_Y
    tw = FRAME + inner_w + FRAME
    th = FRAME + inner_h + FRAME

    fills = Image.new("RGBA", (tw, th), (0, 0, 0, 0))
    chrome = Image.new("RGBA", (tw, th), (0, 0, 0, 0))

    # Dark plate under slots (chrome) — inset inside frame rim
    plate = Image.new("RGBA", (inner_w + 2, inner_h + 2), COL_DARK)
    chrome.paste(plate, (FRAME - 1, FRAME - 1))

    # Outer frame sampled from reference
    chrome.alpha_composite(build_outer_frame(board, tw, th), (0, 0))

    slot_fill, slot_chrome = split_slot_fill_chrome(slot)

    for r in range(ROWS):
        for c in range(COLS):
            x = FRAME + c * (CELL_W + GAP_X)
            y = FRAME + r * (CELL_H + GAP_Y)
            chrome.alpha_composite(slot_chrome, (x, y))
            fills.alpha_composite(slot_fill, (x, y))

    full = Image.alpha_composite(chrome, fills)
    return full, fills, chrome


def apply_color_opacity(img: Image.Image, opacity_pct: int) -> Image.Image:
    """Keep RGB (原色), only scale alpha — whole panel fades together (not white-over-dark→灰)."""
    if opacity_pct >= 100:
        return img
    out = img.copy()
    alpha = out.split()[3].point(lambda a: int(a * opacity_pct / 100.0 + 0.5))
    out.putalpha(alpha)
    return out


def find_font(size: int) -> ImageFont.ImageFont:
    """Prefer client Arial (native Text::Font path); fallback system fonts."""
    candidates = [
        str(FONT_ARIAL),
        r"C:\Windows\Fonts\arial.ttf",
        r"C:\Windows\Fonts\Tahoma.ttf",
    ]
    for path in candidates:
        if os.path.exists(path):
            try:
                return ImageFont.truetype(path, size)
            except OSError:
                continue
    return ImageFont.load_default()


def draw_outlined_text(
    img: Image.Image,
    xy: tuple[int, int],
    text: str,
    font: ImageFont.ImageFont,
    fill=COL_LABEL,
    outline=COL_LABEL_OUTLINE,
) -> None:
    """Match Graphics/SpecialText.h OutlinedText: L/R/T/B only (no diagonals)."""
    draw = ImageDraw.Draw(img)
    x, y = xy
    for dx, dy in ((-1, 0), (1, 0), (0, -1), (0, 1)):
        draw.text((x + dx, y + dy), text, font=font, fill=outline)
    draw.text((x, y), text, font=font, fill=fill)


def split_at_capitals(text: str) -> list[str]:
    """PageUp -> ['Page','Up']; Insert -> ['Insert']."""
    parts: list[str] = []
    cur = ""
    for i, ch in enumerate(text):
        if i > 0 and ch.isupper() and cur:
            parts.append(cur)
            cur = ch
        else:
            cur += ch
    if cur:
        parts.append(cur)
    return parts


def text_width(text: str, font: ImageFont.ImageFont) -> int:
    draw = ImageDraw.Draw(Image.new("RGBA", (64, 32)))
    bbox = draw.textbbox((0, 0), text, font=font)
    return bbox[2] - bbox[0]


def layout_label(text: str, max_w: int = 26) -> tuple[list[str], ImageFont.ImageFont]:
    """Prefer A11M (11px); wrap at capitals if needed; shrink only if still too wide."""
    for sz in (FONT_SIZE_A11M, 10, 9, 8, 7, 6):
        font = find_font(sz)
        if text_width(text, font) <= max_w:
            return [text], font
        parts = split_at_capitals(text)
        if len(parts) >= 2 and all(text_width(p, font) <= max_w for p in parts):
            return parts, font
    font = find_font(6)
    parts = split_at_capitals(text)
    if len(parts) >= 2:
        return parts, font
    return [text], font


def make_labels_layer(size: tuple[int, int]) -> Image.Image:
    layer = Image.new("RGBA", size, (0, 0, 0, 0))
    line_gap = 0  # A11M linespace is tight like client
    for r, row in enumerate(LABELS):
        for c, text in enumerate(row):
            cell_x = FRAME + c * (CELL_W + GAP_X)
            cell_y = FRAME + r * (CELL_H + GAP_Y)
            # top-left of white face (match ref Q overlay)
            tx = cell_x + 2
            ty = cell_y + 0
            lines, font = layout_label(text)
            y = ty
            for line in lines:
                draw_outlined_text(layer, (tx, y), line, font)
                ascent, descent = font.getmetrics()
                y += max(ascent + descent - 2, FONT_SIZE_A11M - 1) + line_gap
    return layer


def main() -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    ref = Image.open(REF).convert("RGBA")
    board = crop_board(ref)
    board.save(OUT / "_ref_board_cropped.png")
    slot = extract_slot(board)
    slot.save(OUT / "_slot_tile.png")

    full100, fills, chrome = build_panel_base(slot, board)
    chrome.save(OUT / "_layer_chrome.png")
    fills.save(OUT / "_layer_fills.png")

    labels = make_labels_layer(full100.size)
    labels.save(OUT / "_layer_labels.png")

    meta = {
        "size": full100.size,
        "cell": [CELL_W, CELL_H],
        "gap": [GAP_X, GAP_Y],
        "frame": FRAME,
        "labels": LABELS,
    }
    print("panel", meta)

    base_empty = Image.alpha_composite(chrome, fills)
    base_labeled = Image.alpha_composite(base_empty, labels)

    for o in OPACITIES:
        empty = apply_color_opacity(base_empty, o)
        empty.save(OUT / f"hotkey-grid-{o}.png")
        labeled = apply_color_opacity(base_labeled, o)
        labeled.save(OUT / f"hotkey-grid-keys-{o}.png")
        print("wrote", o, "alpha-scale whole panel (原色透明)")

    # Checkerboard sheet so translucent white stays readable as white, not gray
    sheet_w = full100.size[0] * 5 + 8 * 4
    sheet_h = full100.size[1] * 2 + 12
    sheet = Image.new("RGBA", (sheet_w, sheet_h), (0, 0, 0, 0))
    sp = sheet.load()
    for y in range(sheet_h):
        for x in range(sheet_w):
            c = 200 if ((x // 8) + (y // 8)) % 2 == 0 else 240
            sp[x, y] = (c, c, c, 255)
    for i, o in enumerate(OPACITIES):
        x = i * (full100.size[0] + 8)
        sheet.alpha_composite(Image.open(OUT / f"hotkey-grid-{o}.png"), (x, 0))
        sheet.alpha_composite(Image.open(OUT / f"hotkey-grid-keys-{o}.png"), (x, full100.size[1] + 12))
    sheet.save(OUT / "_preview_sheet.png")
    print("done", OUT)


if __name__ == "__main__":
    main()
