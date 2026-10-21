# -*- coding: utf-8 -*-
"""Write layered PSD companions for v4/v1 UI assets via pytoshop."""
from pathlib import Path

import numpy as np
from PIL import Image
from pytoshop.enums import ChannelId, ColorMode
from pytoshop.user import nested_layers


def make_image_layer(name, im, opacity=255, visible=True, top=0, left=0):
    arr = np.array(im.convert("RGBA"))
    h, w = arr.shape[:2]
    # Contiguous copies — pytoshop RLE path needs owned C-order buffers.
    channels = {
        ChannelId.red: np.ascontiguousarray(arr[:, :, 0].copy()),
        ChannelId.green: np.ascontiguousarray(arr[:, :, 1].copy()),
        ChannelId.blue: np.ascontiguousarray(arr[:, :, 2].copy()),
        ChannelId.transparency: np.ascontiguousarray(arr[:, :, 3].copy()),
    }
    return nested_layers.Image(
        name=name,
        channels=channels,
        opacity=opacity,
        visible=visible,
        top=top,
        left=left,
        bottom=top + h,
        right=left + w,
        color_mode=ColorMode.rgb,
    )


def save_psd(path: Path, width: int, height: int, layer_list):
    from pytoshop.enums import Compression

    # NOTE: pytoshop docs claim size=(height, width), but nested_layers_to_psd
    # actually writes the tuple as (width, height). Pass PIL (W, H). size=None
    # also works (infers from layer bounds). See mystic_badge generator.
    psd = nested_layers.nested_layers_to_psd(
        layer_list,
        color_mode=ColorMode.rgb,
        size=(width, height),
        compression=Compression.raw,
    )
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, "wb") as f:
        psd.write(f)
    print("wrote", path, path.stat().st_size)


def main():
    ck = Image.open(
        r"E:\project\MapleStory-Client\docs\features\daily-checkin\ui\window\v4\normal.png"
    )
    save_psd(
        Path(
            r"E:\project\MapleStory-Client\docs\features\daily-checkin\ui\window\v4\daily-checkin.psd"
        ),
        ck.width,
        ck.height,
        [make_image_layer("raster/normal", ck)],
    )

    eq = Image.open(
        r"E:\project\MapleStory-Client\docs\features\addon-equipment\ui\window\v4\normal.png"
    )
    save_psd(
        Path(
            r"E:\project\MapleStory-Client\docs\features\addon-equipment\ui\window\v4\addon-equipment.psd"
        ),
        eq.width,
        eq.height,
        [make_image_layer("raster/normal", eq)],
    )

    cell_dir = Path(r"E:\project\MapleStory-Client\docs\features\daily-checkin\ui\cell\v1")
    normal = Image.open(cell_dir / "normal.png")
    mask = Image.open(cell_dir / "mask.png")
    label = Image.open(cell_dir / "claimed-label.png")
    lw, lh = label.size
    lx = (normal.width - lw) // 2
    ly = 19 + ((normal.height - 19) - lh) // 2
    save_psd(
        cell_dir / "cell.psd",
        normal.width,
        normal.height,
        [
            make_image_layer("cell/normal", normal),
            make_image_layer("cell/mask", mask),
            make_image_layer("cell/claimed-label", label, top=ly, left=lx),
        ],
    )
    print("DONE")


if __name__ == "__main__":
    main()
