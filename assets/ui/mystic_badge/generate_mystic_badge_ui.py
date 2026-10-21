#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generate mystic badge equipment panel UI assets with Pillow (+ optional PSD via pytoshop).

Canvas size matches the reference image exactly (187x340).
"""

from __future__ import annotations

import os
from typing import List, Tuple

import numpy as np
from PIL import Image, ImageDraw, ImageFilter, ImageFont

OUT_DIR = os.path.dirname(os.path.abspath(__file__))
REF_CANDIDATES = [
    r"C:\Users\11036\.cursor\projects\empty-window\assets\C__Users_11036_.cursor_projects_e-project-MapleStory-Client_assets_c__Users_11036_AppData_Roaming_Cursor_User_workspaceStorage_empty-window_images________-d5b35b40-a3c2-4df3-83ee-9a627ddfd04f.png",
    r"C:\Users\11036\.cursor\projects\e-project-MapleStory-Client\assets\c__Users_11036_AppData_Roaming_Cursor_User_workspaceStorage_empty-window_images________-d5b35b40-a3c2-4df3-83ee-9a627ddfd04f.png",
    os.path.join(OUT_DIR, "..", "..", "..", "assets", "ref_mystic_badge.png"),
]

# Measured from reference: 3 columns x 2 rows of vertical slots
SLOT_W, SLOT_H = 48, 82
SLOT_POSITIONS = [
    (18, 149), (68, 149), (118, 149),
    (18, 239), (68, 239), (118, 239),
]
# middle column is 47px wide in ref; keep visual gap consistent
SLOT_WIDTHS = [48, 47, 48, 48, 47, 48]

TITLE = "神秘徽章装备栏"


def find_ref() -> str:
    for p in REF_CANDIDATES:
        if os.path.isfile(p):
            return p
    raise FileNotFoundError("reference image not found")


def load_font(size: int) -> ImageFont.FreeTypeFont | ImageFont.ImageFont:
    candidates = [
        r"C:\Windows\Fonts\simhei.ttf",
        r"C:\Windows\Fonts\msyh.ttc",
        r"C:\Windows\Fonts\msyhbd.ttc",
        r"C:\Windows\Fonts\simsun.ttc",
    ]
    for path in candidates:
        if os.path.isfile(path):
            try:
                return ImageFont.truetype(path, size=size)
            except OSError:
                continue
    return ImageFont.load_default()


def rounded_rect(
    draw: ImageDraw.ImageDraw,
    xy: Tuple[int, int, int, int],
    radius: int,
    fill=None,
    outline=None,
    width: int = 1,
) -> None:
    draw.rounded_rectangle(xy, radius=radius, fill=fill, outline=outline, width=width)


def make_noise_texture(w: int, h: int, base: Tuple[int, int, int], amp: int = 10) -> Image.Image:
    """Fine grain texture for empty slots."""
    rng = np.random.default_rng(42)
    base_arr = np.array(base, dtype=np.float32)
    noise = rng.integers(-amp, amp + 1, size=(h, w, 1), dtype=np.int16)
    rgb = np.clip(base_arr + noise, 0, 255).astype(np.uint8)
    # slight vertical brushed feel
    for y in range(h):
        shift = int((y % 3) - 1)
        rgb[y] = np.clip(rgb[y].astype(np.int16) + shift * 2, 0, 255).astype(np.uint8)
    alpha = np.full((h, w, 1), 255, dtype=np.uint8)
    return Image.fromarray(np.concatenate([rgb, alpha], axis=2), "RGBA")


def draw_corner_filigree(draw: ImageDraw.ImageDraw, cx: int, cy: int, scale: int, flip_x: int, flip_y: int, color) -> None:
    """Simple ornate corner swirls (light blue, semi-transparent look via lighter stroke)."""
    def p(dx: float, dy: float) -> Tuple[int, int]:
        return (int(cx + flip_x * dx * scale), int(cy + flip_y * dy * scale))

    def box(x0: float, y0: float, x1: float, y1: float) -> List[int]:
        a, b = p(x0, y0)
        c, d = p(x1, y1)
        return [min(a, c), min(b, d), max(a, c), max(b, d)]

    # outer scroll — quadrant depends on corner
    if flip_x > 0 and flip_y > 0:
        start, end = 180, 270
    elif flip_x < 0 and flip_y > 0:
        start, end = 270, 360
    elif flip_x > 0 and flip_y < 0:
        start, end = 90, 180
    else:
        start, end = 0, 90
    draw.arc(box(0, 0, 14, 14), start=start, end=end, fill=color, width=1)
    # inner curls
    pts = [p(2, 1), p(6, 1), p(9, 3), p(10, 7), p(8, 10), p(4, 11), p(2, 8)]
    draw.line(pts, fill=color, width=1)
    draw.arc(box(1, 6, 7, 12), 0, 180, fill=color, width=1)
    eb = box(3, 3, 5, 5)
    draw.ellipse(eb, outline=color)


def draw_rivet(draw: ImageDraw.ImageDraw, x: int, y: int) -> None:
    # dark blue screw head
    draw.ellipse([x - 3, y - 3, x + 3, y + 3], fill=(40, 95, 140, 255), outline=(25, 70, 110, 255))
    draw.ellipse([x - 1, y - 2, x + 2, y + 1], fill=(90, 150, 190, 180))


def make_slot_empty(w: int = SLOT_W, h: int = SLOT_H) -> Image.Image:
    base = (224, 226, 228)
    tex = make_noise_texture(w, h, base, amp=8)
    # mask to rounded rectangle so corners are transparent
    mask = Image.new("L", (w, h), 0)
    ImageDraw.Draw(mask).rounded_rectangle([0, 0, w - 1, h - 1], radius=4, fill=255)
    img = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    img.paste(tex, (0, 0), mask)
    draw = ImageDraw.Draw(img)
    # soft outer edge
    rounded_rect(draw, (0, 0, w - 1, h - 1), radius=4, outline=(175, 180, 185, 255), width=1)
    # inner highlight (top/left bevel)
    draw.line([(3, 2), (w - 4, 2)], fill=(245, 246, 247, 160), width=1)
    draw.line([(2, 3), (2, h - 4)], fill=(245, 246, 247, 120), width=1)
    # footer divider near bottom (as in reference)
    div_y = h - 12
    draw.line([(3, div_y), (w - 4, div_y)], fill=(165, 168, 172, 230), width=1)
    # subtle footer shade
    footer = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    fd = ImageDraw.Draw(footer)
    fd.rounded_rectangle([2, div_y + 1, w - 3, h - 3], radius=2, fill=(200, 202, 205, 45))
    img = Image.alpha_composite(img, footer)
    return img


def make_lock(size: int = 28) -> Image.Image:
    """Vector-style padlock on transparent background."""
    img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    # scale factors for ~28px icon matching slot lock look
    # shackle
    shackle = (160, 160, 160, 255)
    body = (175, 175, 175, 255)
    body_hi = (210, 210, 210, 255)
    keyhole = (130, 130, 130, 255)
    keyhole_hi = (245, 245, 245, 255)

    # U-shackle
    cx = size // 2
    top = 3
    shackle_w = 14
    shackle_h = 11
    left = cx - shackle_w // 2
    right = cx + shackle_w // 2
    # outer arc
    draw.arc([left, top, right, top + shackle_h + 4], 180, 0, fill=shackle, width=3)
    # legs
    draw.line([(left + 1, top + shackle_h // 2 + 2), (left + 1, top + shackle_h + 2)], fill=shackle, width=3)
    draw.line([(right - 1, top + shackle_h // 2 + 2), (right - 1, top + shackle_h + 2)], fill=shackle, width=3)

    # body rounded rect
    by0 = top + shackle_h - 1
    bx0 = cx - 9
    bx1 = cx + 9
    by1 = by0 + 14
    draw.rounded_rectangle([bx0, by0, bx1, by1], radius=3, fill=body, outline=(150, 150, 150, 255))
    # horizontal highlight band
    draw.rectangle([bx0 + 2, by0 + 3, bx1 - 2, by0 + 6], fill=body_hi)

    # keyhole: circle + stem, with bright center
    kx, ky = cx, by0 + 8
    draw.ellipse([kx - 3, ky - 3, kx + 3, ky + 3], fill=keyhole)
    draw.ellipse([kx - 1, ky - 1, kx + 1, ky + 1], fill=keyhole_hi)
    draw.polygon([(kx - 1, ky + 2), (kx + 1, ky + 2), (kx + 2, ky + 6), (kx - 2, ky + 6)], fill=keyhole)
    return img


def make_title_bar(w: int, h: int = 22) -> Image.Image:
    img = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    # white bar with dark outline
    rounded_rect(draw, (0, 0, w - 1, h - 1), radius=1, fill=(255, 255, 255, 255), outline=(60, 65, 75, 255), width=1)
    font = load_font(12)
    # measure text
    bbox = draw.textbbox((0, 0), TITLE, font=font)
    tw, th = bbox[2] - bbox[0], bbox[3] - bbox[1]
    tx = (w - tw) // 2 - bbox[0]
    ty = (h - th) // 2 - bbox[1] - 1
    draw.text((tx, ty), TITLE, font=font, fill=(20, 20, 20, 255))
    return img


def make_main_slot(w: int, h: int) -> Image.Image:
    """Dark blue display area without unlock text."""
    img = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    # outer light-blue border plate
    rounded_rect(draw, (0, 0, w - 1, h - 1), radius=6, fill=(55, 100, 150, 255), outline=(140, 175, 205, 255), width=1)
    # inner deep navy
    inset = 3
    rounded_rect(
        draw,
        (inset, inset, w - 1 - inset, h - 1 - inset),
        radius=5,
        fill=(30, 55, 115, 255),
        outline=(70, 110, 160, 255),
        width=1,
    )
    # very subtle horizontal sheen (no unlock text)
    sheen = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    sd = ImageDraw.Draw(sheen)
    sd.ellipse([w // 2 - 55, h // 2 - 6, w // 2 + 55, h // 2 + 10], fill=(100, 150, 210, 22))
    sheen = sheen.filter(ImageFilter.GaussianBlur(6))
    img = Image.alpha_composite(img, sheen)
    draw = ImageDraw.Draw(img)

    # corner filigree
    decor = (140, 185, 230, 140)
    m = 10
    draw_corner_filigree(draw, m, m, 1, 1, 1, decor)
    draw_corner_filigree(draw, w - 1 - m, m, 1, -1, 1, decor)
    draw_corner_filigree(draw, m, h - 1 - m, 1, 1, -1, decor)
    draw_corner_filigree(draw, w - 1 - m, h - 1 - m, 1, -1, -1, decor)
    return img


def make_grid_bg(w: int, h: int) -> Image.Image:
    img = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    rounded_rect(draw, (0, 0, w - 1, h - 1), radius=6, fill=(75, 140, 185, 255), outline=(110, 165, 205, 255), width=1)
    # inner shade
    rounded_rect(draw, (2, 2, w - 3, h - 3), radius=5, outline=(55, 115, 160, 100), width=1)
    # rivets at four corners
    for rx, ry in [(10, 10), (w - 11, 10), (10, h - 11), (w - 11, h - 11)]:
        draw_rivet(draw, rx, ry)
    return img


def make_panel_frame(w: int, h: int) -> Image.Image:
    """Outer panel chrome / beveled frame."""
    img = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    # soft outer shadow edge
    rounded_rect(draw, (0, 0, w - 1, h - 1), radius=4, fill=(210, 218, 228, 255), outline=(90, 100, 115, 255), width=1)
    # inner bevel
    draw.rectangle([1, 1, w - 2, h - 2], outline=(245, 250, 255, 255))
    draw.rectangle([2, 2, w - 3, h - 3], outline=(150, 165, 180, 200))
    # content well
    rounded_rect(draw, (4, 4, w - 5, h - 5), radius=3, fill=(195, 210, 225, 255))
    return img


def compose_panel(size: Tuple[int, int]) -> dict:
    w, h = size
    layers = {}

    frame = make_panel_frame(w, h)
    layers["layer_frame"] = frame

    # Title bar placement (measured ~y5-26 inside frame)
    title = make_title_bar(w - 14, 20)
    title_layer = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    title_layer.paste(title, (7, 5), title)
    layers["layer_title_bar"] = title_layer

    # Main dark blue area: y~30 to y~133
    main_w, main_h = w - 12, 102
    main = make_main_slot(main_w, main_h)
    main_layer = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    main_layer.paste(main, (6, 30), main)
    layers["layer_main_slot"] = main_layer

    # Grid background: from ~y138 to near bottom
    grid_w, grid_h = w - 12, 190
    grid = make_grid_bg(grid_w, grid_h)
    grid_layer = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    grid_layer.paste(grid, (6, 138), grid)
    layers["layer_grid_bg"] = grid_layer

    # Empty slots
    slots_layer = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    slot_imgs = []
    for (x, y), sw in zip(SLOT_POSITIONS, SLOT_WIDTHS):
        slot = make_slot_empty(sw, SLOT_H)
        slot_imgs.append(slot)
        slots_layer.paste(slot, (x, y), slot)
    layers["layer_slots"] = slots_layer
    layers["slot_empty"] = make_slot_empty(SLOT_W, SLOT_H)
    layers["lock"] = make_lock(28)

    # Composite base (no locks)
    base = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    for key in ("layer_frame", "layer_title_bar", "layer_main_slot", "layer_grid_bg", "layer_slots"):
        base = Image.alpha_composite(base, layers[key])
    layers["panel_base"] = base
    return layers


def rgba_to_channels(img: Image.Image) -> dict:
    from pytoshop import enums

    arr = np.array(img.convert("RGBA"))
    return {
        enums.ChannelId.red: arr[:, :, 0],
        enums.ChannelId.green: arr[:, :, 1],
        enums.ChannelId.blue: arr[:, :, 2],
        enums.ChannelId.transparency: arr[:, :, 3],
    }


def try_write_psd(path: str, size: Tuple[int, int], layers: dict) -> Tuple[bool, str]:
    try:
        from pytoshop import enums
        from pytoshop.user.nested_layers import Image as PsdImage
        from pytoshop.user.nested_layers import nested_layers_to_psd
    except Exception as e:
        return False, f"pytoshop import failed: {e}"

    w, h = size
    order = [
        ("frame", "layer_frame"),
        ("title_bar", "layer_title_bar"),
        ("main_slot", "layer_main_slot"),
        ("grid_bg", "layer_grid_bg"),
        ("slots_empty", "layer_slots"),
        ("lock_icon", "lock"),
    ]
    psd_layers: List = []
    # lock placed at first slot center for reference (hidden optional — keep visible as asset layer offset)
    lock_img = layers["lock"]
    lock_full = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    # place lock sample at top-left outside slots for easy find, or at slot0 center
    lx = SLOT_POSITIONS[0][0] + (SLOT_W - lock_img.width) // 2
    ly = SLOT_POSITIONS[0][1] + (SLOT_H - lock_img.height) // 2
    lock_full.paste(lock_img, (lx, ly), lock_img)

    layer_images = {
        "layer_frame": layers["layer_frame"],
        "layer_title_bar": layers["layer_title_bar"],
        "layer_main_slot": layers["layer_main_slot"],
        "layer_grid_bg": layers["layer_grid_bg"],
        "layer_slots": layers["layer_slots"],
        "lock": lock_full,
    }

    for name, key in order:
        im = layer_images[key]
        # crop to bounding box of non-transparent for smaller layers? keep full canvas for alignment
        psd_layers.append(
            PsdImage(
                name=name,
                visible=(name != "lock_icon"),  # lock as separate optional layer, hidden by default
                opacity=255,
                top=0,
                left=0,
                bottom=h,
                right=w,
                channels=rgba_to_channels(im),
                color_mode=enums.ColorMode.rgb,
            )
        )

    try:
        # pytoshop size is (width, height) despite some docstrings saying otherwise
        psd = nested_layers_to_psd(
            psd_layers,
            color_mode=enums.ColorMode.rgb,
            size=(w, h),
            compression=enums.Compression.zip,
        )
        with open(path, "wb") as f:
            psd.write(f)
        return True, "ok"
    except Exception as e:
        return False, str(e)


def write_readme(path: str, size: Tuple[int, int], psd_ok: bool, psd_note: str) -> None:
    w, h = size
    text = f"""# 神秘徽章装备栏 UI 资源

画布尺寸：`{w} x {h}`（与参考图一致）

## 文件清单

| 文件 | 说明 |
|------|------|
| `panel_base.png` | 完整面板：标题 + 深蓝主区（无「200级可解锁」）+ 6 空槽（无锁） |
| `slot_empty.png` | 单个空槽（浅灰纹理） |
| `lock.png` | 透明底锁图标 |
| `layer_title_bar.png` | 标题栏图层（全画布透明底） |
| `layer_main_slot.png` | 深蓝主区图层 |
| `layer_grid_bg.png` | 浅蓝网格底板 + 四角 rivet |
| `mystic_badge_panel.psd` | 分层 PSD（若生成成功） |

## 合成方式（自下而上）

1. `layer_frame`（外框，含在 `panel_base` 内；PSD 中为 `frame`）
2. `layer_title_bar` — 标题「神秘徽章装备栏」
3. `layer_main_slot` — 深蓝展示区（无解锁文字）
4. `layer_grid_bg` — 浅蓝装备区背景
5. `layer_slots` / 6× `slot_empty` 贴到槽位坐标
6. （可选）`lock.png` 居中叠到需要锁定的槽上

### 槽位坐标（左上角，相对画布）

"""
    for i, ((x, y), sw) in enumerate(zip(SLOT_POSITIONS, SLOT_WIDTHS)):
        text += f"- 槽 {i}: ({x}, {y}) 尺寸 {sw}x{SLOT_H}\n"
    text += f"""
## 与参考图差异

- 已去除「200级可解锁」文字
- 6 个槽默认空槽，不绘制锁
- 锁单独导出为 `lock.png`
- 角部装饰为简化卷草纹（非位图抠取）

## PSD

- 状态：{'成功' if psd_ok else '失败'}
- 说明：{psd_note}

## 生成

```bash
python assets/ui/mystic_badge/generate_mystic_badge_ui.py
```
"""
    with open(path, "w", encoding="utf-8") as f:
        f.write(text)


def main() -> None:
    ref_path = find_ref()
    ref = Image.open(ref_path).convert("RGBA")
    size = ref.size
    print(f"reference: {ref_path}")
    print(f"size: {size[0]}x{size[1]}")

    os.makedirs(OUT_DIR, exist_ok=True)
    layers = compose_panel(size)

    exports = {
        "panel_base.png": layers["panel_base"],
        "slot_empty.png": layers["slot_empty"],
        "lock.png": layers["lock"],
        "layer_title_bar.png": layers["layer_title_bar"],
        "layer_main_slot.png": layers["layer_main_slot"],
        "layer_grid_bg.png": layers["layer_grid_bg"],
    }
    for name, im in exports.items():
        p = os.path.join(OUT_DIR, name)
        im.save(p)
        print(f"wrote {p} {im.size}")

    psd_path = os.path.join(OUT_DIR, "mystic_badge_panel.psd")
    psd_ok, psd_note = try_write_psd(psd_path, size, layers)
    if psd_ok:
        print(f"wrote {psd_path}")
    else:
        print(f"PSD failed: {psd_note}")

    readme = os.path.join(OUT_DIR, "README.md")
    write_readme(readme, size, psd_ok, psd_note)
    print(f"wrote {readme}")

    # cleanup debug crops if any
    for dbg in ("_dbg_title.png", "_dbg_main.png", "_dbg_grid.png", "_dbg_slot.png"):
        p = os.path.join(OUT_DIR, dbg)
        if os.path.isfile(p):
            os.remove(p)

    # quick acceptance: size match
    assert layers["panel_base"].size == size
    print("ACCEPT: canvas size matches reference")
    print("DONE")


if __name__ == "__main__":
    main()
