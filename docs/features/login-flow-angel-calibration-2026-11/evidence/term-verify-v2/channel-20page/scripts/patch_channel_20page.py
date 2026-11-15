from pathlib import Path
import sys

ROOT = Path(r"E:\project\MapleStory-Client")
H = ROOT / "IO" / "UITypes" / "UIWorldSelect.h"
CPP = ROOT / "IO" / "UITypes" / "UIWorldSelect.cpp"

EDITS = {
    H: [],
    CPP: [],
}

EDITS[H].append((
    "header: 20-page constant + comment",
    "\t\t// 取值对齐 Angel CLogin::ChangeStep step1：`sub_5FDD2D(15)` / `sub_5FDD2D(60)`。\n"
    "\t\t// 面板可视槽仍为 30（与 chBackgrn 449×233 网格一致）；>30 时翻页。\n"
    "\t\tstatic constexpr uint16_t WORLD_BUTTON_COUNT = 15U;\n"
    "\t\tstatic constexpr uint16_t CHANNEL_PAGE_SIZE = 30U;\n",
    "\t\t// 取值对齐 Angel CLogin::ChangeStep step1：`sub_5FDD2D(15)` / `sub_5FDD2D(60)`。\n"
    "\t\t// 可视槽以 WZ 真值为准：Login.img/WorldSelect/channel 只有 0..19 共 20 张\n"
    "\t\t// 自带 CH.N 标签的按钮贴图，目标帧也是 4 列×5 行；>20 个逻辑频道时翻页，\n"
    "\t\t// 逻辑容量仍为 CHANNEL_CAPACITY=60（服务端 channel_size=30 / max=60）。\n"
    "\t\tstatic constexpr uint16_t WORLD_BUTTON_COUNT = 15U;\n"
    "\t\tstatic constexpr uint16_t CHANNEL_PAGE_SIZE = 20U;\n",
))

EDITS[H].append((
    "header: page comment",
    "\t\t/// 频道翻页：0 = CH.1..30，1 = CH.31..60。\n"
    "\t\tuint8_t channel_page = 0;\n",
    "\t\t/// 频道翻页：0 = CH.1..20，1 = CH.21..40，2 = CH.41..60。\n"
    "\t\tuint8_t channel_page = 0;\n",
))

EDITS[CPP].append((
    "cpp: include Geometry/SpecialText",
    "#include \"../../Graphics/LocalPngTexture.h\"\n"
    "#include \"../../Graphics/Text.h\"\n",
    "#include \"../../Graphics/Geometry.h\"\n"
    "#include \"../../Graphics/LocalPngTexture.h\"\n"
    "#include \"../../Graphics/SpecialText.h\"\n"
    "#include \"../../Graphics/Text.h\"\n",
))

EDITS[CPP].append((
    "cpp: WS_NOTICE_GAP comment drift",
    "\t\t\t\t\t// 公告板：右缘越过面板左缘 33px（WS_NOTICE_GAP=-33，原生叠在面板前，\n",
    "\t\t\t\t\t// 公告板：右缘越过面板左缘 36px（WS_NOTICE_GAP=-36，原生叠在面板前，\n",
))

EDITS[CPP].append((
    "cpp: change_world capacity comment",
    "\t\t//   - 可视槽 CHANNEL_PAGE_SIZE；逻辑容量 CHANNEL_CAPACITY（Angel ChangeStep 60）。\n"
    "\t\t// 故按容量上限收敛，多余频道忽略并限频告警；>PAGE_SIZE 走翻页。\n",
    "\t\t//   - 可视槽 CHANNEL_PAGE_SIZE=20（WZ channel/0..19 与目标帧 4×5）；逻辑容量 CHANNEL_CAPACITY=60。\n"
    "\t\t// 故按容量上限收敛，多余频道忽略并限频告警；>20 走翻页。\n",
))

EDITS[CPP].append((
    "cpp: paged CH.N label overlay",
    "\t\t\t\tPoint<int16_t> panel_topleft = panel_c - Point<int16_t>(CH_PANEL_W / 2, CH_PANEL_H / 2);\n"
    "\n"
    "\t\t\t\tfor (uint16_t i = 0; i < page_vis; i++)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tchannel_gauge[i].draw(\n",
    "\t\t\t\tPoint<int16_t> panel_topleft = panel_c - Point<int16_t>(CH_PANEL_W / 2, CH_PANEL_H / 2);\n"
    "\n"
    "\t\t\t\t// 第 2/3 页复用 WZ 的 channel/0..19 按钮贴图；这些贴图左上角烘焙了\n"
    "\t\t\t\t// CH.1..CH.20，若直接复用会把逻辑频道 CH.21..CH.60 显示成旧标签。\n"
    "\t\t\t\t// 用 WZ 标签区实测底色 (238,204,153) 盖掉 40×15，再叠 A11B 描边字。\n"
    "\t\t\t\t// 第 1 页保持原生烘焙标签不动，避免破坏已验收的 20 格 NCC。\n"
    "\t\t\t\tif (page_base != 0 && page_vis > 0)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tconst ColorBox label_mask(\n"
    "\t\t\t\t\t\t40, 15,\n"
    "\t\t\t\t\t\tColor(238.0f / 255.0f, 204.0f / 255.0f, 153.0f / 255.0f, 1.0f),\n"
    "\t\t\t\t\t\t1.0f\n"
    "\t\t\t\t\t);\n"
    "\t\t\t\t\tOutlinedText label(\n"
    "\t\t\t\t\t\tText::Font::A11B,\n"
    "\t\t\t\t\t\tText::Alignment::LEFT,\n"
    "\t\t\t\t\t\tColor::Name::CREAM,\n"
    "\t\t\t\t\t\tColor::Name::JAMBALAYA\n"
    "\t\t\t\t\t);\n"
    "\n"
    "\t\t\t\t\tfor (uint16_t i = 0; i < page_vis; i++)\n"
    "\t\t\t\t\t{\n"
    "\t\t\t\t\t\tconst uint16_t logical = static_cast<uint16_t>(page_base + i);\n"
    "\t\t\t\t\t\tconst Point<int16_t> cell =\n"
    "\t\t\t\t\t\t\tposition + panel_topleft + channel_cell_offset(i);\n"
    "\n"
    "\t\t\t\t\t\tlabel_mask.draw(DrawArgument(cell, content_alpha));\n"
    "\t\t\t\t\t\tlabel.change_text(\"CH.\" + std::to_string(logical + 1));\n"
    "\n"
    "\t\t\t\t\t\tconst Point<int16_t> label_pos = cell + Point<int16_t>(\n"
    "\t\t\t\t\t\t\t8, label.y_for_ink_top(4));\n"
    "\t\t\t\t\t\tconst DrawArgument label_args(label_pos, content_alpha);\n"
    "\n"
    "\t\t\t\t\t\tlabel.l.draw(DrawArgument(label_pos + Point<int16_t>(-1, 0), content_alpha));\n"
    "\t\t\t\t\t\tlabel.r.draw(DrawArgument(label_pos + Point<int16_t>(1, 0), content_alpha));\n"
    "\t\t\t\t\t\tlabel.t.draw(DrawArgument(label_pos + Point<int16_t>(0, -1), content_alpha));\n"
    "\t\t\t\t\t\tlabel.b.draw(DrawArgument(label_pos + Point<int16_t>(0, 1), content_alpha));\n"
    "\t\t\t\t\t\tlabel.inner.draw(label_args);\n"
    "\t\t\t\t\t}\n"
    "\t\t\t\t}\n"
    "\n"
    "\t\t\t\tfor (uint16_t i = 0; i < page_vis; i++)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tchannel_gauge[i].draw(\n",
))

OLD_NAV = (
    "\t\t\t\t// 键盘导航按**逻辑频道**（跨页）；可视页不足时先翻页再点槽。\n"
    "\t\t\t\tuint16_t channel_total = pending_channel_limit;\n"
    "\t\t\t\tuint16_t selected_channel = (channelid == 0xFF) ? 0 : static_cast<uint16_t>(channelid);\n"
    "\n"
    "\t\t\t\t// 服务端偶发下发 0 频道的畸形世界：columns=0 会让 std::div 除零(UB)，\n"
    "\t\t\t\t// 行数与上下导航随之失真。该世界没有可导航的频道网格，仅保留退出逃生键。\n"
    "\t\t\t\tif (channel_total == 0)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tif (escape)\n"
    "\t\t\t\t\t{\n"
    "\t\t\t\t\t\tworld_selected = false;\n"
    "\t\t\t\t\t\tchannel_slide = 1.0f;\n"
    "\t\t\t\t\t\tpanel_expand = 1.0f;\n"
    "\t\t\t\t\t\tclear_selected_world();\n"
    "\t\t\t\t\t}\n"
    "\n"
    "\t\t\t\t\treturn;\n"
    "\t\t\t\t}\n"
    "\n"
    "\t\t\t\tif (selected_channel >= channel_total)\n"
    "\t\t\t\t\tselected_channel = 0;\n"
    "\n"
    "\t\t\t\tauto press_logical_channel = [this](uint16_t logical)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tconst uint8_t pg = static_cast<uint8_t>(logical / CHANNEL_PAGE_SIZE);\n"
    "\t\t\t\t\tif (pg != channel_page)\n"
    "\t\t\t\t\t\tset_channel_page(pg);\n"
    "\t\t\t\t\tconst uint16_t slot = static_cast<uint16_t>(logical - channel_page_base());\n"
    "\t\t\t\t\tbutton_pressed(static_cast<uint16_t>(Buttons::BtChannel0 + slot));\n"
    "\t\t\t\t};\n"
    "\n"
    "\t\t\t\tuint8_t columns = static_cast<uint8_t>(std::min<uint16_t>(channel_total, COLUMNS));\n"
    "\t\t\t\tuint8_t rows = static_cast<uint8_t>((channel_total - 1) / COLUMNS + 1);\n"
    "\n"
    "\t\t\t\tdiv_t div = std::div(static_cast<int>(selected_channel), static_cast<int>(columns));\n"
    "\t\t\t\tint32_t current_col = div.rem;\n"
    "\n"
    "\t\t\t\tif (keycode == KeyAction::Id::UP)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tint next_channel = static_cast<int>(selected_channel) - COLUMNS;\n"
    "\t\t\t\t\tif (next_channel < 0)\n"
    "\t\t\t\t\t\tnext_channel += rows * COLUMNS;\n"
    "\t\t\t\t\tif (static_cast<uint16_t>(next_channel) >= channel_total)\n"
    "\t\t\t\t\t\tnext_channel = current_col;\n"
    "\t\t\t\t\tif (static_cast<uint16_t>(next_channel) == selected_channel)\n"
    "\t\t\t\t\t\treturn;\n"
    "\t\t\t\t\tpress_logical_channel(static_cast<uint16_t>(next_channel));\n"
    "\t\t\t\t}\n"
    "\t\t\t\telse if (keycode == KeyAction::Id::DOWN)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tuint16_t next_channel = static_cast<uint16_t>(selected_channel + COLUMNS);\n"
    "\t\t\t\t\tif (next_channel >= channel_total)\n"
    "\t\t\t\t\t\tnext_channel = static_cast<uint16_t>(current_col);\n"
    "\t\t\t\t\tif (next_channel == selected_channel)\n"
    "\t\t\t\t\t\treturn;\n"
    "\t\t\t\t\tpress_logical_channel(next_channel);\n"
    "\t\t\t\t}\n"
    "\t\t\t\telse if (keycode == KeyAction::Id::LEFT || keycode == KeyAction::Id::TAB)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tif (selected_channel != 0)\n"
    "\t\t\t\t\t\tselected_channel--;\n"
    "\t\t\t\t\telse\n"
    "\t\t\t\t\t\tselected_channel = static_cast<uint16_t>(channel_total - 1);\n"
    "\n"
    "\t\t\t\t\tpress_logical_channel(selected_channel);\n"
    "\t\t\t\t}\n"
    "\t\t\t\telse if (keycode == KeyAction::Id::RIGHT)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tif (selected_channel != channel_total - 1)\n"
    "\t\t\t\t\t\tselected_channel++;\n"
    "\t\t\t\t\telse\n"
    "\t\t\t\t\t\tselected_channel = 0;\n"
    "\n"
    "\t\t\t\t\tpress_logical_channel(selected_channel);\n"
    "\t\t\t\t}\n"
)

NEW_NAV = (
    "\t\t\t\t// 键盘导航按**逻辑频道**（跨页）；方向键只在当前可视页 4 列网格内走，\n"
    "\t\t\t\t// SoftKey / 历史自动选线仍可跨页翻到对应 logical。\n"
    "\t\t\t\tconst uint16_t channel_total = pending_channel_limit;\n"
    "\n"
    "\t\t\t\t// 服务端偶发下发 0 频道的畸形世界：columns=0 会让 std::div 除零(UB)，\n"
    "\t\t\t\t// 行数与上下导航随之失真。该世界没有可导航的频道网格，仅保留退出逃生键。\n"
    "\t\t\t\tif (channel_total == 0)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tif (escape)\n"
    "\t\t\t\t\t{\n"
    "\t\t\t\t\t\tworld_selected = false;\n"
    "\t\t\t\t\t\tchannel_slide = 1.0f;\n"
    "\t\t\t\t\t\tpanel_expand = 1.0f;\n"
    "\t\t\t\t\t\tclear_selected_world();\n"
    "\t\t\t\t\t}\n"
    "\n"
    "\t\t\t\t\treturn;\n"
    "\t\t\t\t}\n"
    "\n"
    "\t\t\t\tconst uint16_t page_base = channel_page_base();\n"
    "\t\t\t\tconst uint16_t page_vis = channel_page_visible();\n"
    "\n"
    "\t\t\t\tif (page_vis == 0)\n"
    "\t\t\t\t\treturn;\n"
    "\n"
    "\t\t\t\tuint16_t selected_channel =\n"
    "\t\t\t\t\t(channelid == 0xFF) ? page_base : static_cast<uint16_t>(channelid);\n"
    "\n"
    "\t\t\t\tif (selected_channel >= channel_total\n"
    "\t\t\t\t\t|| selected_channel < page_base\n"
    "\t\t\t\t\t|| selected_channel >= page_base + page_vis)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tselected_channel = page_base;\n"
    "\t\t\t\t}\n"
    "\n"
    "\t\t\t\tauto press_logical_channel = [this](uint16_t logical)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tif (logical >= pending_channel_limit)\n"
    "\t\t\t\t\t\treturn;\n"
    "\n"
    "\t\t\t\t\tconst uint8_t pg = static_cast<uint8_t>(logical / CHANNEL_PAGE_SIZE);\n"
    "\t\t\t\t\tif (pg != channel_page)\n"
    "\t\t\t\t\t\tset_channel_page(pg);\n"
    "\t\t\t\t\tconst uint16_t slot = static_cast<uint16_t>(logical - channel_page_base());\n"
    "\t\t\t\t\tbutton_pressed(static_cast<uint16_t>(Buttons::BtChannel0 + slot));\n"
    "\t\t\t\t};\n"
    "\n"
    "\t\t\t\tauto press_current_page_slot = [this, page_base, &press_logical_channel](uint16_t page_slot)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tpress_logical_channel(static_cast<uint16_t>(page_base + page_slot));\n"
    "\t\t\t\t};\n"
    "\n"
    "\t\t\t\tconst uint16_t slot = static_cast<uint16_t>(selected_channel - page_base);\n"
    "\t\t\t\tconst uint8_t columns = COLUMNS;\n"
    "\t\t\t\tconst uint8_t rows =\n"
    "\t\t\t\t\tstatic_cast<uint8_t>((page_vis - 1) / columns + 1);\n"
    "\n"
    "\t\t\t\tdiv_t div = std::div(static_cast<int>(slot), static_cast<int>(columns));\n"
    "\t\t\t\tint32_t current_col = div.rem;\n"
    "\t\t\t\tconst int32_t current_row = static_cast<int32_t>(slot / columns);\n"
    "\n"
    "\t\t\t\tauto move_vertical = [&](int32_t delta_row)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tint32_t row = current_row;\n"
    "\n"
    "\t\t\t\t\tfor (int32_t tries = 0; tries < rows; ++tries)\n"
    "\t\t\t\t\t{\n"
    "\t\t\t\t\t\trow = (row + delta_row + rows) % rows;\n"
    "\t\t\t\t\t\tconst int32_t candidate = row * columns + current_col;\n"
    "\n"
    "\t\t\t\t\t\tif (candidate >= 0 && candidate < page_vis)\n"
    "\t\t\t\t\t\t{\n"
    "\t\t\t\t\t\t\tpress_current_page_slot(static_cast<uint16_t>(candidate));\n"
    "\t\t\t\t\t\t\treturn;\n"
    "\t\t\t\t\t\t}\n"
    "\t\t\t\t\t}\n"
    "\t\t\t\t};\n"
    "\n"
    "\t\t\t\tif (keycode == KeyAction::Id::UP)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tmove_vertical(-1);\n"
    "\t\t\t\t}\n"
    "\t\t\t\telse if (keycode == KeyAction::Id::DOWN)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tmove_vertical(1);\n"
    "\t\t\t\t}\n"
    "\t\t\t\telse if (keycode == KeyAction::Id::LEFT || keycode == KeyAction::Id::TAB)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tint32_t next_slot = static_cast<int32_t>(slot) - 1;\n"
    "\n"
    "\t\t\t\t\tif (next_slot < 0)\n"
    "\t\t\t\t\t\tnext_slot = static_cast<int32_t>(page_vis - 1);\n"
    "\n"
    "\t\t\t\t\tpress_current_page_slot(static_cast<uint16_t>(next_slot));\n"
    "\t\t\t\t}\n"
    "\t\t\t\telse if (keycode == KeyAction::Id::RIGHT)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tuint16_t next_slot = static_cast<uint16_t>(slot + 1);\n"
    "\n"
    "\t\t\t\t\tif (next_slot >= page_vis)\n"
    "\t\t\t\t\t\tnext_slot = 0;\n"
    "\n"
    "\t\t\t\t\tpress_current_page_slot(next_slot);\n"
    "\t\t\t\t}\n"
)

EDITS[CPP].append(("cpp: current-page keyboard navigation", OLD_NAV, NEW_NAV))

EDITS[CPP].append((
    "cpp: RETURN first channel on current page",
    "\t\t\t\t\t\tif (pending_channel_limit == 0)\n"
    "\t\t\t\t\t\t\treturn;\n"
    "\t\t\t\t\t\tpress_logical_channel(0);\n",
    "\t\t\t\t\t\tif (pending_channel_limit == 0)\n"
    "\t\t\t\t\t\t\treturn;\n"
    "\t\t\t\t\t\tpress_logical_channel(page_base);\n",
))


def load_normalized(path: Path):
    raw = path.read_bytes()
    bom = b"\xef\xbb\xbf" if raw.startswith(b"\xef\xbb\xbf") else b""
    body = raw[len(bom):]
    crlf = body.count(b"\r\n")
    lf = body.count(b"\n")
    if crlf and crlf != lf:
        raise RuntimeError(f"{path}: mixed EOL, abort")
    eol = b"\r\n" if crlf else b"\n"
    text = body.decode("utf-8").replace("\r\n", "\n")
    return bom, eol, text


def save_preserving(path: Path, bom: bytes, eol: bytes, text: str):
    out = text.replace("\n", eol.decode("ascii"))
    path.write_bytes(bom + out.encode("utf-8"))


staged = {}
for path, ops in EDITS.items():
    bom, eol, text = load_normalized(path)
    working = text
    report = []
    for label, old, new in ops:
        count = working.count(old)
        if count != 1:
            raise RuntimeError(f"PRECHECK FAIL {path.name}: {label} count={count}")
        working = working.replace(old, new, 1)
        report.append(label)
    staged[path] = (bom, eol, working)
    print(f"PRECHECK OK {path.name}: {len(report)} edits")

for path, (bom, eol, text) in staged.items():
    save_preserving(path, bom, eol, text)
    print(f"PATCHED {path}")

print("PATCHED 2 files ALL_OK")
