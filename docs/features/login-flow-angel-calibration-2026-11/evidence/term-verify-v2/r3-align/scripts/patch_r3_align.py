#!/usr/bin/env python3
"""R3 最小补丁：按 WZ 真值模板 + 两端目标帧实测，修三处独立锚点。

证据（ui_r3/measure_r3_final.json + compute_shifts.json）：
  charInfo  目标 bbox [392,220,...] / 本端 [391,197,...]  → 内容板+值行上移 23
  scroll    目标 y=161 / 本端 y=158                        → 卷轴只差 3，必须保持
  nameTag   目标 y=430 / 本端 y=438                        → 名牌下移 8
  notice    目标 Title (293,313) / 本端 (259,301)          → 公告整体左34上12

联动关系（UICharSelect.cpp:547/563）：
  detail_pos = charpos + CS_CHARINFO_OFF
  scroll_y   = detail_y - CS_SCROLL_CONTENT_OFF.y
  要 detail_y 197→220 (+23) 且 scroll_y 158→161 (+3)：
    CS_CHARINFO_OFF.y        -231 → -208  (+23)
    CS_SCROLL_CONTENT_OFF.y    39 →   59  (+20)
  X 两端仅差 1px，不动。

公告（UIWorldSelect.cpp:746-752）：
  notice_x = panel_left - width - WS_NOTICE_GAP
  notice_y = panel_top  + 15 + notice_dy
  右移 33 / 下移 12：
    WS_NOTICE_GAP  0 → -33（与 :743 原生注释「越过面板左缘 33px」一致）
    +15 → +27

保留 UTF-8 BOM / 原 EOL，逐文件自适应；全部断言唯一命中后才写盘。
"""
import os
import sys

ROOT = r"E:\project\MapleStory-Client"

PATCHES = {
    r"IO\UITypes\UICharSelect.cpp": [
        (b"static constexpr Point<int16_t> CS_CHARINFO_OFF(-94, -231);",
         b"static constexpr Point<int16_t> CS_CHARINFO_OFF(-94, -208);"),
        (b"static constexpr Point<int16_t> CS_SCROLL_CONTENT_OFF(17, 39);",
         b"static constexpr Point<int16_t> CS_SCROLL_CONTENT_OFF(17, 59);"),
        (b"nametags[index].draw(charpos + Point<int16_t>(2, 25));",
         b"nametags[index].draw(charpos + Point<int16_t>(2, 17));"),
    ],
    r"IO\UITypes\UIWorldSelect.cpp": [
        (b"static constexpr int16_t WS_NOTICE_GAP = 0;",
         b"static constexpr int16_t WS_NOTICE_GAP = -33;"),
        (b"panel_c.y() - CH_PANEL_H / 2 + 15 + notice_dy",
         b"panel_c.y() - CH_PANEL_H / 2 + 27 + notice_dy"),
    ],
}


def patch(path, pairs):
    full = os.path.join(ROOT, path)
    data = open(full, "rb").read()
    bom = b""
    if data.startswith(b"\xef\xbb\xbf"):
        bom, data = data[:3], data[3:]
    crlf = data.count(b"\r\n")
    lf = data.count(b"\n") - crlf
    if crlf and lf:
        raise AssertionError("%s 混合 EOL (crlf=%d lf=%d)" % (path, crlf, lf))
    eol = b"\r\n" if crlf else b"\n"
    out = data
    for old, new in pairs:
        n = out.count(old)
        if n != 1:
            raise AssertionError("%s anchor 命中 %d 次: %r" % (path, n, old[:70]))
        out = out.replace(old, new, 1)
    if out == data:
        raise AssertionError("%s 无变化" % path)
    open(full, "wb").write(bom + out)
    head = open(full, "rb").read(3)
    return {"file": path, "eol": "CRLF" if crlf else "LF",
            "bom_kept": head == b"\xef\xbb\xbf", "bom_head": head.hex(),
            "bytes_delta": len(out) - len(data)}


def main():
    # 先全量校验，任一失败零写盘
    for path, pairs in PATCHES.items():
        full = os.path.join(ROOT, path)
        data = open(full, "rb").read()
        for old, _ in pairs:
            if data.count(old) != 1:
                raise AssertionError("PRECHECK %s: %r 命中 %d 次"
                                     % (path, old[:70], data.count(old)))
    res = [patch(p, q) for p, q in PATCHES.items()]
    print("PATCHED", len(res), "files")
    for r in res:
        print(" ", r)
    print("ALL_OK")


if __name__ == "__main__":
    main()
