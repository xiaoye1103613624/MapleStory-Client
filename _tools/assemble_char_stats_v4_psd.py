"""Export v4 reusable UI parts (blank chips) + layout JSON + JSX for layered PSD.

Per ui-photoshop §10: materials strip + Final assembly with editable text layers.
Does NOT bake labels into chips — text is created in Photoshop as Type layers.
"""
from __future__ import annotations

import json
import math
from pathlib import Path

from PIL import Image, ImageDraw

# Reuse geometry/colors from compose script
import compose_char_stats_ui as C

OUT = C.OUT
UI_V4 = OUT / "ui" / "detail" / "v4"
COMP = UI_V4 / "components"
LAYOUT_JSON = UI_V4 / "layout_cn.json"
JSX_PATH = UI_V4 / "assemble_v4.jsx"
PSD_PATH = OUT / "Character_Stats_Detail_V4.psd"
PSD_UI_PATH = UI_V4 / "Character_Stats_Detail_V4.psd"

LH = C.ROW_H_V4 - 2  # 16
LABEL_OUTLINE = (60, 60, 60, 255)
RESIST_OUTLINE = (40, 40, 40, 255)


def _rounded(img: Image.Image, fill, outline, radius: int = 2) -> None:
    draw = ImageDraw.Draw(img)
    w, h = img.size
    draw.rounded_rectangle([0, 0, w - 1, h - 1], radius=radius, fill=fill, outline=outline, width=1)


def export_components() -> dict[str, str]:
    """Write blank reusable sprites; return name→absolute path."""
    COMP.mkdir(parents=True, exist_ok=True)
    paths: dict[str, str] = {}

    def save(name: str, im: Image.Image) -> None:
        p = COMP / f"{name}.png"
        im.save(p)
        paths[name] = str(p).replace("\\", "/")

    # Label chips by semantic color (no text)
    color_map = {
        "combat": C.V4_COMBAT,
        "dmg": C.V4_COMBAT_DK,
        "crit_rate": C.V4_CRIT_RATE,
        "crit_dmg": C.V4_CRIT_DMG,
        "final": C.V4_FINAL,
        "boss": C.V4_BOSS,
        "ignore": C.V4_IGNORE,
        "status": C.V4_STATUS,
        "buff": C.V4_BUFF,
        "stance": C.V4_STANCE,
        "drop": C.V4_DROP,
        "meso": C.V4_MESO,
        "honor": C.V4_HONOR,
        "ailment": C.V4_AILMENT,
        "pink": C.PINK,
        "green": C.GREEN,
        "resist": C.RESIST_GREY,
    }
    for key, fill in color_map.items():
        chip = Image.new("RGBA", (C.LABEL_W_V4, LH), (0, 0, 0, 0))
        _rounded(chip, fill, LABEL_OUTLINE if key != "resist" else RESIST_OUTLINE)
        save(f"label_chip_{key}", chip)

    # Value boxes
    for name, w in (
        ("value_box_80", C.VW_NORMAL_V4),
        ("value_box_240", C.VW_ATK_V4),
        ("value_box_left", C.LEFT_W - C.PAD * 2 - C.LABEL_W_L_V4 - 3),  # 205
    ):
        box = Image.new("RGBA", (w, LH), (0, 0, 0, 0))
        _rounded(box, C.VALUE_BG, C.VALUE_EDGE)
        save(name, box)

    # Resist cell parts
    row_w = C.LEFT_W - C.PAD * 2
    cell_w = (row_w - 4 * (C.RESIST_COLS - 1)) // C.RESIST_COLS
    resist_lab = Image.new("RGBA", (cell_w, C.RESIST_LABEL_H), (0, 0, 0, 0))
    _rounded(resist_lab, C.RESIST_GREY, RESIST_OUTLINE)
    save("resist_label_blank", resist_lab)
    resist_val = Image.new("RGBA", (cell_w, C.RESIST_VALUE_H), (0, 0, 0, 0))
    _rounded(resist_val, C.VALUE_BG, C.VALUE_EDGE)
    save("resist_value_blank", resist_val)

    # Panel chrome blanks (no title text)
    for name, w, h in (
        ("panel_chrome_main", C.LEFT_W, C.measure_left_content_h(len(C.LEFT_IDENTITY_V3["cn"]))),
        ("panel_chrome_detail", C.RIGHT_W_V4, C.measure_left_content_h(len(C.LEFT_IDENTITY_V3["cn"]))),
    ):
        chrome = Image.new("RGBA", (w, h), (0, 0, 0, 0))
        draw = ImageDraw.Draw(chrome)
        draw.rounded_rectangle(
            [0, 0, w - 1, h - 1], radius=3, fill=C.PANEL_BG, outline=C.PANEL_EDGE, width=2
        )
        draw.rectangle([2, 2, w - 3, C.TITLE_H], fill=C.TITLE_BG)
        draw.line([2, C.TITLE_H, w - 3, C.TITLE_H], fill=C.PANEL_EDGE, width=1)
        draw.line([3, h - 4, w - 4, h - 4], fill=C.BEVEL_HI, width=1)
        draw.line([3, h - 3, w - 4, h - 3], fill=C.BEVEL_LO, width=1)
        save(name, chrome)

    return paths


def build_layout_cn(panel_h: int) -> dict:
    """Absolute positions for Final assembly (CN). Text is separate from chips."""
    gap = 6
    detail_x0 = C.LEFT_W + gap
    left_col = C.V4_LEFT_COL["cn"]
    right_col = C.V4_RIGHT_COL["cn"]
    n_rows = max(len(left_col), len(right_col))
    top_y = C.TITLE_H + 4
    bottom_pad = C.CONTENT_BOTTOM_PAD + C.FOOTER_H + 2
    total_rows = 1 + n_rows
    usable = panel_h - top_y - bottom_pad
    min_step = C.ROW_H_V4
    inter = max(0, usable - total_rows * min_step) // (total_rows - 1) if total_rows > 1 else 0
    step = min_step + inter

    items: list[dict] = []

    # Main chrome + title
    items.append(
        {
            "kind": "place",
            "part": "panel_chrome_main",
            "x": 0,
            "y": 0,
            "group": "Final/Main/chrome",
            "name": "chrome_main",
        }
    )
    items.append(
        {
            "kind": "text",
            "text": C.TITLES["cn"][0],
            "x": C.LEFT_W // 2,
            "y": C.TITLE_H // 2 + 4,
            "size": C.FONT_TITLE_V4,
            "align": "CENTER",
            "rgb": list(C.TEXT_DARK[:3]),
            "group": "Final/Main/text",
            "name": "title_main",
        }
    )

    # Detail chrome + title
    items.append(
        {
            "kind": "place",
            "part": "panel_chrome_detail",
            "x": detail_x0,
            "y": 0,
            "group": "Final/Detail/chrome",
            "name": "chrome_detail",
        }
    )
    items.append(
        {
            "kind": "text",
            "text": C.TITLES["cn"][1],
            "x": detail_x0 + C.RIGHT_W_V4 // 2,
            "y": C.TITLE_H // 2 + 4,
            "size": C.FONT_TITLE_V4,
            "align": "CENTER",
            "rgb": list(C.TEXT_DARK[:3]),
            "group": "Final/Detail/text",
            "name": "title_detail",
        }
    )

    # Left identity / core rows
    x0 = C.PAD
    y = top_y
    vw_left = C.LEFT_W - C.PAD * 2 - C.LABEL_W_L_V4 - 3
    for i, label in enumerate(C.LEFT_IDENTITY_V3["cn"]):
        items.append(
            {
                "kind": "place",
                "part": "label_chip_pink",
                "x": x0,
                "y": y,
                "group": "Final/Main/labels",
                "name": f"main_lab_{i}",
            }
        )
        items.append(
            {
                "kind": "place",
                "part": "value_box_left",
                "x": x0 + C.LABEL_W_L_V4 + 2,
                "y": y,
                "group": "Final/Main/values",
                "name": f"main_val_{i}",
            }
        )
        items.append(
            {
                "kind": "text",
                "text": label,
                "x": x0 + 3,
                "y": y + LH // 2 + 3,
                "size": C.FONT_LABEL_V4,
                "align": "LEFT",
                "rgb": list(C.TEXT_WHITE[:3]),
                "group": "Final/Main/text",
                "name": f"main_txt_{i}",
            }
        )
        y += C.ROW_H + 1

    y += 3
    for i, label in enumerate(C.LEFT_CORE["cn"]):
        items.append(
            {
                "kind": "place",
                "part": "label_chip_green",
                "x": x0,
                "y": y,
                "group": "Final/Main/labels",
                "name": f"core_lab_{i}",
            }
        )
        items.append(
            {
                "kind": "place",
                "part": "value_box_left",
                "x": x0 + C.LABEL_W_L_V4 + 2,
                "y": y,
                "group": "Final/Main/values",
                "name": f"core_val_{i}",
            }
        )
        items.append(
            {
                "kind": "text",
                "text": label,
                "x": x0 + 3,
                "y": y + LH // 2 + 3,
                "size": C.FONT_LABEL_V4,
                "align": "LEFT",
                "rgb": list(C.TEXT_WHITE[:3]),
                "group": "Final/Main/text",
                "name": f"core_txt_{i}",
            }
        )
        y += C.ROW_H + 1

    y += 6
    row_w = C.LEFT_W - C.PAD * 2
    cell_gap = 4
    cell_w = (row_w - cell_gap * (C.RESIST_COLS - 1)) // C.RESIST_COLS
    for i, label in enumerate(C.LEFT_RESIST["cn"]):
        col = i % C.RESIST_COLS
        row = i // C.RESIST_COLS
        cx = x0 + col * (cell_w + cell_gap)
        cy = y + row * (C.RESIST_LABEL_H + 2 + C.RESIST_VALUE_H + C.RESIST_CELL_GAP_Y)
        items.append(
            {
                "kind": "place",
                "part": "resist_label_blank",
                "x": cx,
                "y": cy,
                "group": "Final/Main/resist",
                "name": f"resist_lab_{i}",
            }
        )
        items.append(
            {
                "kind": "place",
                "part": "resist_value_blank",
                "x": cx,
                "y": cy + C.RESIST_LABEL_H + 2,
                "group": "Final/Main/resist",
                "name": f"resist_val_{i}",
            }
        )
        items.append(
            {
                "kind": "text",
                "text": label,
                "x": cx + 3,
                "y": cy + C.RESIST_LABEL_H // 2 + 3,
                "size": 10,
                "align": "LEFT",
                "rgb": list(C.TEXT_WHITE[:3]),
                "group": "Final/Main/text",
                "name": f"resist_txt_{i}",
            }
        )

    # Detail: full ATK row
    dx = detail_x0 + C.PAD
    dy = top_y
    atk_label, atk_ck = C.V4_ATK_FULL["cn"]
    items.append(
        {
            "kind": "place",
            "part": f"label_chip_{atk_ck}",
            "x": dx,
            "y": dy,
            "group": "Final/Detail/labels",
            "name": "atk_lab",
        }
    )
    items.append(
        {
            "kind": "place",
            "part": "value_box_240",
            "x": dx + C.LABEL_W_V4 + 2,
            "y": dy,
            "group": "Final/Detail/values",
            "name": "atk_val",
        }
    )
    items.append(
        {
            "kind": "text",
            "text": atk_label,
            "x": dx + 3,
            "y": dy + LH // 2 + 3,
            "size": C.FONT_LABEL_V4,
            "align": "LEFT",
            "rgb": list(C.TEXT_WHITE[:3]),
            "group": "Final/Detail/text",
            "name": "atk_txt",
        }
    )
    dy += step

    pair_w = C.LABEL_W_V4 + 2 + C.VW_NORMAL_V4
    right_x = dx + pair_w + C.COL_GAP_V4
    for i in range(n_rows):
        if i < len(left_col):
            lab, ck = left_col[i]
            items.append(
                {
                    "kind": "place",
                    "part": f"label_chip_{ck}",
                    "x": dx,
                    "y": dy,
                    "group": "Final/Detail/labels",
                    "name": f"dL_lab_{i}",
                }
            )
            items.append(
                {
                    "kind": "place",
                    "part": "value_box_80",
                    "x": dx + C.LABEL_W_V4 + 2,
                    "y": dy,
                    "group": "Final/Detail/values",
                    "name": f"dL_val_{i}",
                }
            )
            items.append(
                {
                    "kind": "text",
                    "text": lab,
                    "x": dx + 3,
                    "y": dy + LH // 2 + 3,
                    "size": C.FONT_LABEL_V4,
                    "align": "LEFT",
                    "rgb": list(C.TEXT_WHITE[:3]),
                    "group": "Final/Detail/text",
                    "name": f"dL_txt_{i}",
                }
            )
        if i < len(right_col):
            lab, ck = right_col[i]
            items.append(
                {
                    "kind": "place",
                    "part": f"label_chip_{ck}",
                    "x": right_x,
                    "y": dy,
                    "group": "Final/Detail/labels",
                    "name": f"dR_lab_{i}",
                }
            )
            items.append(
                {
                    "kind": "place",
                    "part": "value_box_80",
                    "x": right_x + C.LABEL_W_V4 + 2,
                    "y": dy,
                    "group": "Final/Detail/values",
                    "name": f"dR_val_{i}",
                }
            )
            items.append(
                {
                    "kind": "text",
                    "text": lab,
                    "x": right_x + 3,
                    "y": dy + LH // 2 + 3,
                    "size": C.FONT_LABEL_V4,
                    "align": "LEFT",
                    "rgb": list(C.TEXT_WHITE[:3]),
                    "group": "Final/Detail/text",
                    "name": f"dR_txt_{i}",
                }
            )
        dy += step

    # Materials strip (right of Final): full chrome masters + chip strip below
    mat_x = 670
    chip_parts = [
        "label_chip_combat",
        "label_chip_dmg",
        "label_chip_crit_rate",
        "label_chip_crit_dmg",
        "label_chip_final",
        "label_chip_boss",
        "label_chip_ignore",
        "label_chip_status",
        "label_chip_buff",
        "label_chip_stance",
        "label_chip_drop",
        "label_chip_meso",
        "label_chip_honor",
        "label_chip_ailment",
        "label_chip_pink",
        "label_chip_green",
        "label_chip_resist",
        "value_box_80",
        "value_box_240",
        "value_box_left",
        "resist_label_blank",
        "resist_value_blank",
    ]
    materials = [
        {"part": "panel_chrome_main", "x": mat_x, "y": 24, "name": "mat_chrome_main"},
        {
            "part": "panel_chrome_detail",
            "x": mat_x + C.LEFT_W + 8,
            "y": 24,
            "name": "mat_chrome_detail",
        },
    ]
    mx, my = mat_x, 24 + panel_h + 16
    for i, part in enumerate(chip_parts):
        col = i % 4
        row = i // 4
        materials.append(
            {
                "part": part,
                "x": mx + col * (C.LABEL_W_V4 + 10),
                "y": my + row * (LH + 8),
                "name": f"mat_{part}",
            }
        )
    chip_rows = math.ceil(len(chip_parts) / 4)

    final_w = C.LEFT_W + gap + C.RIGHT_W_V4
    canvas_w = mat_x + C.LEFT_W + 8 + C.RIGHT_W_V4 + 16
    canvas_h = max(panel_h + 16, my + chip_rows * (LH + 8) + 24)

    return {
        "canvas": {"w": canvas_w, "h": canvas_h, "final_w": final_w, "final_h": panel_h},
        "items": items,
        "materials": materials,
        "step": step,
        "font": "Microsoft YaHei",
    }


def write_jsx(layout: dict, part_paths: dict[str, str]) -> None:
    """Generate ExtendScript that builds layered PSD with Materials + Final.

    Uses open+duplicate (more reliable than Place on CN PS) and ASCII-escaped JSON.
    """

    def j(s: str) -> str:
        return s.replace("\\", "/").replace('"', '\\"')

    parts_js = "{\n" + ",\n".join(f'  "{k}": "{j(v)}"' for k, v in part_paths.items()) + "\n}"
    # ensure_ascii → \\uXXXX so evalFile encoding cannot break Chinese
    layout_js = json.dumps(layout, ensure_ascii=True)

    jsx = f"""// Auto-generated: assemble char-stats v4 layered PSD
#target photoshop
app.displayDialogs = DialogModes.NO;
var _ru = app.preferences.rulerUnits;
app.preferences.rulerUnits = Units.PIXELS;

var PARTS = {parts_js};
var LAYOUT = {layout_js};
var PSD_OUT = "{j(str(PSD_PATH))}";
var PSD_UI = "{j(str(PSD_UI_PATH))}";

function ensureGroup(doc, pathParts) {{
  var parent = doc;
  for (var i = 0; i < pathParts.length; i++) {{
    var name = pathParts[i];
    var set = null;
    for (var k = 0; k < parent.layerSets.length; k++) {{
      if (parent.layerSets[k].name === name) {{ set = parent.layerSets[k]; break; }}
    }}
    if (!set) {{
      set = parent.layerSets.add();
      set.name = name;
    }}
    parent = set;
  }}
  return parent;
}}

var SRC_CACHE = {{}};
function getSrcDoc(filePath) {{
  if (SRC_CACHE[filePath]) return SRC_CACHE[filePath];
  var f = new File(filePath);
  if (!f.exists) {{ throw new Error("missing " + filePath); }}
  SRC_CACHE[filePath] = app.open(f);
  return SRC_CACHE[filePath];
}}
function closeSrcCache() {{
  for (var k in SRC_CACHE) {{
    try {{ SRC_CACHE[k].close(SaveOptions.DONOTSAVECHANGES); }} catch (e) {{}}
  }}
  SRC_CACHE = {{}};
}}

function pastePngAt(doc, filePath, name, x, y, groupPath) {{
  var src = getSrcDoc(filePath);
  app.activeDocument = src;
  src.activeLayer.duplicate(doc, ElementPlacement.PLACEATBEGINNING);
  app.activeDocument = doc;
  var layer = doc.activeLayer;
  layer.name = name;
  var b = layer.bounds;
  var curX = b[0].value;
  var curY = b[1].value;
  layer.translate(x - curX, y - curY);
  if (groupPath && groupPath.length) {{
    var g = ensureGroup(doc, groupPath);
    layer.move(g, ElementPlacement.INSIDE);
  }}
  return layer;
}}

function addText(doc, text, name, x, y, sizePt, align, rgb, groupPath, fontName) {{
  var g = (groupPath && groupPath.length) ? ensureGroup(doc, groupPath) : doc;
  var layer = g.artLayers.add();
  layer.kind = LayerKind.TEXT;
  layer.name = name;
  var ti = layer.textItem;
  ti.kind = TextType.POINTTEXT;
  ti.contents = text;
  ti.size = sizePt;
  try {{ ti.font = fontName; }} catch (e1) {{
    try {{ ti.font = "MicrosoftYaHei"; }} catch (e2) {{
      try {{ ti.font = "SimHei"; }} catch (e3) {{}}
    }}
  }}
  var c = new SolidColor();
  c.rgb.red = rgb[0]; c.rgb.green = rgb[1]; c.rgb.blue = rgb[2];
  ti.color = c;
  if (align === "CENTER") ti.justification = Justification.CENTER;
  else if (align === "RIGHT") ti.justification = Justification.RIGHT;
  else ti.justification = Justification.LEFT;
  ti.position = [x, y];
  return layer;
}}

var W = LAYOUT.canvas.w;
var H = LAYOUT.canvas.h;
var doc = app.documents.add(W, H, 72, "Character_Stats_Detail_V4", NewDocumentMode.RGB, DocumentFill.TRANSPARENT);
app.activeDocument = doc;

ensureGroup(doc, ["Materials"]);
ensureGroup(doc, ["Final"]);

var mats = LAYOUT.materials;
for (var mi = 0; mi < mats.length; mi++) {{
  var m = mats[mi];
  var mp = PARTS[m.part];
  if (!mp) continue;
  pastePngAt(doc, mp, m.name, m.x, m.y, ["Materials"]);
}}

addText(doc, "Materials / components", "mat_banner", 670, 16, 10, "LEFT", [80,80,90], ["Materials"], LAYOUT.font);

var items = LAYOUT.items;
for (var ii = 0; ii < items.length; ii++) {{
  var it = items[ii];
  var gpath = it.group ? String(it.group).split("/") : [];
  if (it.kind === "place") {{
    var pp = PARTS[it.part];
    if (!pp) throw new Error("no part " + it.part);
    pastePngAt(doc, pp, it.name, it.x, it.y, gpath);
  }} else if (it.kind === "text") {{
    addText(doc, it.text, it.name, it.x, it.y, it.size, it.align, it.rgb, gpath, LAYOUT.font);
  }}
}}

function savePsd(path) {{
  var f = new File(path);
  var opts = new PhotoshopSaveOptions();
  opts.embedColorProfile = true;
  opts.alphaChannels = true;
  opts.layers = true;
  doc.saveAs(f, opts, true, Extension.LOWERCASE);
}}

savePsd(PSD_OUT);
savePsd(PSD_UI);
closeSrcCache();
app.preferences.rulerUnits = _ru;

"OK|" + W + "x" + H + "|items=" + items.length + "|mats=" + mats.length;
"""
    JSX_PATH.write_text(jsx, encoding="utf-8")


def main() -> None:
    UI_V4.mkdir(parents=True, exist_ok=True)
    paths = export_components()
    panel_h = C.measure_left_content_h(len(C.LEFT_IDENTITY_V3["cn"]))
    layout = build_layout_cn(panel_h)
    LAYOUT_JSON.write_text(json.dumps(layout, ensure_ascii=False, indent=2), encoding="utf-8")
    write_jsx(layout, paths)
    print("OK components", len(paths), "->", COMP)
    print("OK layout items", len(layout["items"]), "materials", len(layout["materials"]))
    print("OK jsx", JSX_PATH)
    print("canvas", layout["canvas"])


if __name__ == "__main__":
    main()
