"""Compose extended MapleStory expanded inventory UI.

Keep original window width (no right-edge stretch artifacts).
Double slot grid vertically. Widen empty meso field by +80px inside footer.
"""
from __future__ import annotations

from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

SRC = Path(r"E:\project\MapleStory-Client\docs\features\inventory-ui-ps\source")
OUT = Path(r"E:\project\MapleStory-Client\docs\features\inventory-ui-ps")
MESO_EXTRA = 160  # visibly wider empty meso field (was +80)
TITLE_CN = "背包"
TITLE_FONT = Path(r"C:\Windows\Fonts\simhei.ttf")

SLOT_Y0 = 45
SLOT_Y1 = 257
FOOTER_Y0 = 257
MESO_X0, MESO_X1 = 24, 142
# "mesos" label sits just after the white field
LABEL_X0, LABEL_X1 = 142, 180
MESO_Y0, MESO_Y1 = 267, 281
BORDER_SAFE = 8


def stretch_width(strip: Image.Image, new_w: int) -> Image.Image:
    w, h = strip.size
    if new_w == w:
        return strip.copy()
    edge = min(2, max(1, w // 8))
    left = strip.crop((0, 0, edge, h))
    right = strip.crop((w - edge, 0, w, h))
    mid = strip.crop((edge, 0, w - edge, h)).resize((new_w - 2 * edge, h), Image.Resampling.NEAREST)
    out = Image.new("RGBA", (new_w, h))
    out.paste(left, (0, 0))
    out.paste(mid, (edge, 0))
    out.paste(right, (new_w - edge, 0))
    return out


def rebuild_footer(footer: Image.Image) -> Image.Image:
    """Widen empty meso field; shift 'mesos' label; keep window width & right chrome."""
    fw, fh = footer.size
    fy0 = MESO_Y0 - FOOTER_Y0
    fy1 = MESO_Y1 - FOOTER_Y0
    new_meso_w = (MESO_X1 - MESO_X0) + MESO_EXTRA
    new_meso_x1 = MESO_X0 + new_meso_w  # 222

    out = footer.copy()

    # 1) Cover old meso + label band with footer blue chrome sample (clear digits/old label)
    chrome = footer.crop((200, 0, 201, fh)).resize((new_meso_x1 - MESO_X0 + (LABEL_X1 - LABEL_X0), fh), Image.Resampling.NEAREST)
    out.paste(chrome, (MESO_X0, 0))

    # 2) Paste stretched empty meso field (white box only on meso rows)
    meso = stretch_width(footer.crop((MESO_X0, fy0, MESO_X1, fy1)), new_meso_w)
    out.paste(meso, (MESO_X0, fy0))

    # 3) Shift original "mesos" label to the right of the widened field
    label = footer.crop((LABEL_X0, 0, LABEL_X1, fh))
    label_x = new_meso_x1 + 2
    # Ensure label does not run past the right frame
    max_label_x = fw - BORDER_SAFE - label.size[0]
    if label_x > max_label_x:
        label_x = max(new_meso_x1, max_label_x)
    out.paste(label, (label_x, 0))

    return out


def paint_title_cn(header: Image.Image) -> Image.Image:
    """Replace English 'ITEM INVENTORY' with Chinese title."""
    out = header.copy()
    draw = ImageDraw.Draw(out)
    bg = out.getpixel((90, 12))
    draw.rectangle((6, 5, 120, 18), fill=bg)
    font = ImageFont.truetype(str(TITLE_FONT), 12) if TITLE_FONT.exists() else ImageFont.load_default()
    draw.text((8, 6), TITLE_CN, font=font, fill=(0, 0, 0, 255))
    return out


def clean_right_fringe(im: Image.Image) -> Image.Image:
    """Drop the 1px gray shadow fringe past the true outer border (WZ export artifact)."""
    # Keep through the white outer highlight (x=600); drop gray x=601 + empty x=602.
    if im.size[0] >= 602:
        return im.crop((0, 0, 601, im.size[1]))
    return im


def compose() -> Path:
    base = clean_right_fringe(Image.open(SRC / "FullBackgrnd.png").convert("RGBA"))
    w, h = base.size
    if h != 289 or w not in (601, 603):
        raise SystemExit(f"unexpected FullBackgrnd size {w}x{h}")

    slot_h = SLOT_Y1 - SLOT_Y0
    header = paint_title_cn(base.crop((0, 0, w, SLOT_Y0)))
    slots = base.crop((0, SLOT_Y0, w, SLOT_Y1))
    footer = rebuild_footer(base.crop((0, FOOTER_Y0, w, h)))

    canvas_w = w  # keep original width — no stacked right column
    canvas_h = header.size[1] + slot_h * 2 + footer.size[1]
    canvas = Image.new("RGBA", (canvas_w, canvas_h), (0, 0, 0, 0))

    y = 0
    canvas.paste(header, (0, y), header)
    y += header.size[1]
    canvas.paste(slots, (0, y), slots)
    y += slot_h
    canvas.paste(slots, (0, y), slots)
    y += slot_h
    canvas.paste(footer, (0, y), footer)

    layers_dir = OUT / "layers"
    layers_dir.mkdir(parents=True, exist_ok=True)
    header.save(layers_dir / "01_header.png")
    slots.save(layers_dir / "03_slots.png")
    slots.save(layers_dir / "03_slots_copy.png")
    footer.save(layers_dir / "04_footer_meso.png")
    canvas.save(OUT / "MapleStory_Inventory_Full_Extended.png")

    (OUT / "compose_meta.txt").write_text(
        "source=UI.wz/UIWindow.img/Item/FullBackgrnd\n"
        f"orig={w}x{h}\n"
        f"slot_y=[{SLOT_Y0},{SLOT_Y1}) h={slot_h} duplicated => {slot_h * 2}\n"
        f"meso +{MESO_EXTRA}px EMPTY, window width UNCHANGED ({w})\n"
        f"final={canvas_w}x{canvas_h}\n"
        "no right-edge stretch; no sample digits; no BtCashshop overlay\n",
        encoding="utf-8",
    )
    print("OK", canvas.size, "->", OUT / "MapleStory_Inventory_Full_Extended.png")
    return OUT / "MapleStory_Inventory_Full_Extended.png"


if __name__ == "__main__":
    compose()
