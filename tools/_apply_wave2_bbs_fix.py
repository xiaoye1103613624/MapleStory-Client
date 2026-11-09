# -*- coding: utf-8 -*-
"""round39 · Wave2 2-7: UIGuildBBS::button_pressed BtPageRight 上界守卫.

与 BtPageLeft 的 `if (page_index > 0)` 下界对称：当前页未满即末页，
不发必然返回空列表的越界页请求。动态探测 tab 缩进，避免 edit 工具对
tab+中文注释 old_string 失配（round34/35/37/38 教训）。
"""
import io
import re
import sys

SRC = r"E:/project/MapleStory-Client/IO/UITypes/UIGuildBBS.cpp"

raw = io.open(SRC, "r", encoding="utf-8", newline="").read()
crlf = "\r\n" in raw
src = raw.replace("\r\n", "\n")

m = re.search(
    r"(?m)^([ \t]*)case BtPageRight:\n"
    r"([ \t]*)request_list\(page_index \+ 1\);\n"
    r"([ \t]*)break;",
    src,
)
if not m:
    print("ANCHOR_NOT_FOUND")
    sys.exit(2)

ind_case, ind_body, ind_break = m.group(1), m.group(2), m.group(3)
OLD = m.group(0)
NEW = (
    ind_case + "case BtPageRight:\n"
    + ind_body + "// 与 BtPageLeft 的 page_index > 0 下界对称：当前页未满即末页，\n"
    + ind_body + "// 不发必然返回空列表的越界页请求（否则 selected_row 被清成 -1、\n"
    + ind_body + "// 窗口空白、page_index 停在越界值，只能再点「上一页」回退）。\n"
    + ind_body + "if (static_cast<int>(threads.size()) >= LIST_MAX_ROWS)\n"
    + ind_body + "\trequest_list(page_index + 1);\n"
    + ind_break + "break;"
)

found = src.count(OLD)
if found != 1:
    print("OLD_FOUND", found)
    sys.exit(3)

out = src.replace(OLD, NEW, 1)
if crlf:
    out = out.replace("\n", "\r\n")
io.open(SRC, "w", encoding="utf-8", newline="").write(out)
print("WROTE_OK crlf=%s case_indent=%d body_indent=%d" % (
    crlf, len(ind_case), len(ind_body)))
