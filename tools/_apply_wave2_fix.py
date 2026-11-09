#!/usr/bin/env python3
"""One-shot precise patch: UINpcTalk::send_key SENDSIMPLE keyboard navigation
must keep the highlighted choice inside the visible band (Wave2 2-3).

The OLD/NEW templates below are written with a 1-tab base; the live file nests
this block one level deeper. Instead of hard-coding the shift, probe the real
anchor indentation and re-indent both templates uniformly. A hand-copied
anchor from a summary is exactly the kind of drift that produces a fake fix.
"""
import io
import re

P = r"E:/project/MapleStory-Client/IO/UITypes/UINpcTalk.cpp"
with io.open(P, "r", encoding="utf-8", newline="") as f:
    raw = f.read()
crlf = "\r\n" in raw
txt = raw.replace("\r\n", "\n")

# Probe the live base indentation of the SENDSIMPLE block.
m = re.search(r"(?m)^(\t+)if \(type == TalkType::SENDSIMPLE\)\n", txt)
assert m, "SENDSIMPLE anchor line not found"
base = m.group(1)
TPL_BASE = "\t"
assert base.startswith(TPL_BASE), "unexpected base indent %r" % base
extra = base[len(TPL_BASE):]
print("ANCHOR_BASE", len(base), "EXTRA", len(extra))


def reindent(block, prefix):
    return "".join(prefix + ln for ln in block.splitlines(True))


OLD = (
"\tif (type == TalkType::SENDSIMPLE)\n"
"\t{\n"
"\t\tif (keycode == KeyAction::Id::UP)\n"
"\t\t{\n"
"\t\t\tif (simple_selected > 0)\n"
"\t\t\t\t--simple_selected;\n"
"\t\t\treturn;\n"
"\t\t}\n"
"\t\tif (keycode == KeyAction::Id::DOWN)\n"
"\t\t{\n"
"\t\t\tif (!simple_choices.empty()\n"
"\t\t\t\t&& simple_selected + 1 < static_cast<int32_t>(simple_choices.size()))\n"
"\t\t\t\t++simple_selected;\n"
"\t\t\treturn;\n"
"\t\t}\n"
"\t\t// 回车：等同下项钮（IDA sub_9AC0B6 Enter → 提交 list[cur]）\n"
"\t\tif (keycode == KeyAction::Id::RETURN)\n"
"\t\t{\n"
"\t\t\tif (dispatch_simple_selection())\n"
"\t\t\t{\n"
"\t\t\t\tclear_input_field();\n"
"\t\t\t\tdeactivate();\n"
"\t\t\t}\n"
"\t\t\treturn;\n"
"\t\t}\n"
"\t}\n"
)

NEW = (
"\tif (type == TalkType::SENDSIMPLE)\n"
"\t{\n"
"\t\t// Wave2 2-3：键盘导航把高亮移到滚出可视带的行时，draw() 的选中虚线因\n"
"\t\t// visible=false 不画、send_cursor 也点不到，但回车 dispatch_simple_selection()\n"
"\t\t// 仍会提交这个不可见项——与 round34 UIWorldSelect / round35 UICharSelect /\n"
"\t\t// round36 Textfield 同族：主体路径有守卫，旁路分支漏守卫。\n"
"\t\t// 这里用与 draw()/send_cursor() 同源的可视带判据把选中项滚回视野，并同步滑块 thumb。\n"
"\t\tauto ensure_visible = [&]()\n"
"\t\t{\n"
"\t\t\tif (!show_slider\n"
"\t\t\t\t|| simple_choices.empty()\n"
"\t\t\t\t|| simple_selected < 0\n"
"\t\t\t\t|| static_cast<size_t>(simple_selected) >= simple_choices.size())\n"
"\t\t\t\treturn;\n"
"\n"
"\t\t\tconst SimpleChoice& c = simple_choices[static_cast<size_t>(simple_selected)];\n"
"\t\t\tconst int16_t ls = Text::linespace(Text::Font::A12M);\n"
"\t\t\tconst int16_t band_top = static_cast<int16_t>(top.height() - 1);\n"
"\t\t\tconst int16_t band_bottom = static_cast<int16_t>(band_top + height - TEXT_SLACK);\n"
"\t\t\tint16_t maxoffset = static_cast<int16_t>(rowmax - unitrows);\n"
"\t\t\tif (maxoffset < 0)\n"
"\t\t\t\tmaxoffset = 0;\n"
"\n"
"\t\t\t// vline 与 draw():234 / send_cursor():636 逐字同源。\n"
"\t\t\tint16_t vline = static_cast<int16_t>(text_draw_y + c.pos.y() - offset * scroll_step);\n"
"\t\t\tif (vline + ls > band_bottom)\n"
"\t\t\t{\n"
"\t\t\t\twhile (offset < maxoffset && vline + ls > band_bottom)\n"
"\t\t\t\t{\n"
"\t\t\t\t\t++offset;\n"
"\t\t\t\t\tvline = static_cast<int16_t>(text_draw_y + c.pos.y() - offset * scroll_step);\n"
"\t\t\t\t}\n"
"\t\t\t}\n"
"\t\t\telse if (vline < band_top)\n"
"\t\t\t{\n"
"\t\t\t\twhile (offset > 0 && vline < band_top)\n"
"\t\t\t\t{\n"
"\t\t\t\t\t--offset;\n"
"\t\t\t\t\tvline = static_cast<int16_t>(text_draw_y + c.pos.y() - offset * scroll_step);\n"
"\t\t\t\t}\n"
"\t\t\t}\n"
"\t\t\t// Slider::row 与 UINpcTalk::offset 同源；setrows 只回写 row，不触发 onmoved。\n"
"\t\t\tslider.setrows(offset, unitrows, rowmax);\n"
"\t\t};\n"
"\n"
"\t\tif (keycode == KeyAction::Id::UP)\n"
"\t\t{\n"
"\t\t\tif (simple_selected > 0)\n"
"\t\t\t\t--simple_selected;\n"
"\t\t\tensure_visible();\n"
"\t\t\treturn;\n"
"\t\t}\n"
"\t\tif (keycode == KeyAction::Id::DOWN)\n"
"\t\t{\n"
"\t\t\tif (!simple_choices.empty()\n"
"\t\t\t\t&& simple_selected + 1 < static_cast<int32_t>(simple_choices.size()))\n"
"\t\t\t\t++simple_selected;\n"
"\t\t\tensure_visible();\n"
"\t\t\treturn;\n"
"\t\t}\n"
"\t\t// 回车：等同下项钮（IDA sub_9AC0B6 Enter → 提交 list[cur]）\n"
"\t\tif (keycode == KeyAction::Id::RETURN)\n"
"\t\t{\n"
"\t\t\tif (dispatch_simple_selection())\n"
"\t\t\t{\n"
"\t\t\t\tclear_input_field();\n"
"\t\t\t\tdeactivate();\n"
"\t\t\t}\n"
"\t\t\treturn;\n"
"\t\t}\n"
"\t}\n"
)

OLD = reindent(OLD, extra)
NEW = reindent(NEW, extra)

cnt = txt.count(OLD)
print("OLD_FOUND", cnt)
assert cnt == 1, "anchor not unique after reindent: %d" % cnt
out = txt.replace(OLD, NEW, 1)
if crlf:
    out = out.replace("\n", "\r\n")
with io.open(P, "w", encoding="utf-8", newline="") as f:
    f.write(out)
print("WROTE_OK crlf=%s +lines=%d" % (crlf, len(NEW.splitlines()) - len(OLD.splitlines())))
