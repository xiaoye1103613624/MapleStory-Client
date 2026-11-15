# -*- coding: utf-8 -*-
"""
m08208 四点频道页：唯一命中补丁（本轮 goal 范围，仅 3 文件）。
- PRECHECK：每条 old 在其文件字节流里恰好出现 1 次 + 全文件花括号净平衡不变；
  任一失败 => 零写盘。
- 逐文件保 BOM/自适应 EOL：本批 3 文件均 head3=2f2f2f、纯 LF，bytes 级替换不改换行。
事实与来源：ui_r4/measure_r4.json、ui_r4/measure_r4_scroll_full.json、
            targets_desktop/desktop_channel_expanded.png、orange-wz Login.img 真节点。
"""
import os
import sys

ROOT = r"E:\project\MapleStory-Client"

WORLDSEL_H = os.path.join(ROOT, "IO", "UITypes", "UIWorldSelect.h")
WORLDSEL_CPP = os.path.join(ROOT, "IO", "UITypes", "UIWorldSelect.cpp")
BALLOON_CPP = os.path.join(ROOT, "IO", "Components", "ChatBalloon.cpp")

EDITS = []


def E(label, path, old, new):
    EDITS.append((label, path, old, new))


# ---------------- ① 公告正文上移 4px ----------------
E(
    "balloon_text_y_minus4",
    BALLOON_CPP,
    "\t\tconst int16_t text_y = static_cast<int16_t>(ct + (cb - ct - textlabel.height()) / 2);\n"
    "\t\ttextlabel.draw(Point<int16_t>(text_x, text_y));",
    "\t\t// m08208 ①：measure_r4.json balloon_body_ink 目标墨带 y337..345，本端\n"
    "\t\t// y341..350（weighted 14.23 vs 16.93）=> 正文比目标低约 4~5px。\n"
    "\t\t// 只上移正文 text_y，不动 Title(:304)/九宫/notice_y，保 noticeTitle NCC=1.0。\n"
    "\t\tconst int16_t text_y = static_cast<int16_t>(ct + (cb - ct - textlabel.height()) / 2 - 4);\n"
    "\t\ttextlabel.draw(Point<int16_t>(text_x, text_y));",
)

# ---------------- ② 区域图标 +13 / 频道格 +13 +1 ----------------
E(
    "world_dlg_map_x_plus13",
    WORLDSEL_CPP,
    "\tstatic constexpr Point<int16_t> WS_WORLD_DLG_MAP(-140, -804);",
    "\tstatic constexpr Point<int16_t> WS_WORLD_DLG_MAP(-127, -804);\n"
    "\t// m08208 ②：用户对照截图指「区域图标偏左」。worldsrc_pos 由本常量派生\n"
    "\t// （世界按钮/名牌/旗标均 +13）；不动 WS_CHANNEL_WND_MAP，以免 panel_c 漂移\n"
    "\t// 卷轴落点 (398,139)、公告 nw(280,296)、GoWorld(756,497) 三处已验收锚点。",
)

E(
    "cell_offset_plus13_plus1",
    WORLDSEL_CPP,
    "\t\treturn Point<int16_t>(22 + CELL_W * d.rem, 56 + CELL_H * d.quot);",
    "\t\t// m08208 ②：channel/0 模板 NCC TARGET@(659,369) vs OURS@(646,368)，\n"
    "\t\t// 目标首格 (471,337) vs 本端 (458,336) => +13/+1。Gauge/红圈/次页标签\n"
    "\t\t// 覆盖均走本函数，联动收敛；panel_topleft=(436,280) 保持不变。\n"
    "\t\treturn Point<int16_t>(35 + CELL_W * d.rem, 57 + CELL_H * d.quot);",
)

# ---------------- ③④ 卷轴打开态常驻资源 ----------------
E(
    "header_add_scroll_open_texture",
    WORLDSEL_H,
    "\t\tTexture worlds_background;\n"
    "\t\tTexture channels_background;",
    "\t\tTexture worlds_background;\n"
    "\t\tTexture channels_background;\n"
    "\t\t/// m08208 ③④：Login.img/WorldSelect/scroll/0/3（513x401，origin(0,0)）打开态末帧，\n"
    "\t\t/// 选中世界且展开动画播完后常驻，替代无卷边的 chBackgrn 平板。\n"
    "\t\tTexture channels_scroll_open;",
)

E(
    "ctor_assign_scroll_open",
    WORLDSEL_CPP,
    "\t\tchannels_background = WorldSelect[\"chBackgrn\"];",
    "\t\tchannels_background = WorldSelect[\"chBackgrn\"];\n"
    "\t\t// m08208 ③④：orange-wz 真节点 WorldSelect/scroll/0/3 = 513x401「卷轴打开态」\n"
    "\t\t// （顶部木轴 + 羊皮纸主体 + 底部卷起木轴）。目标帧全帧 alpha-NCC=0.7546@\n"
    "\t\t// (398,139)（ui_r4/measure_r4_scroll_full.json）。播完常驻此帧即补上底部卷起\n"
    "\t\t// 羊皮纸 UI 与「到选择的世界去」按钮下背景，且保持卷轴为打开态。\n"
    "\t\tchannels_scroll_open = WorldSelect[\"scroll\"][\"0\"][\"3\"];",
)

E(
    "draw_else_use_scroll_open",
    WORLDSEL_CPP,
    "\t\t\telse\n"
    "\t\t\t{\n"
    "\t\t\t\t// 卷轴展开：按进度自上而下纵向揭示底板。源裁剪（露出顶部 reveal 行）\n"
    "\t\t\t\t// 与目标高度同步收缩，保持 1:1 不拉伸；未用贴图拉伸是因为木框会被压扁。\n"
    "\t\t\t\tconst int16_t full_h = channels_background.get_dimensions().y();\n"
    "\t\t\t\tint16_t reveal = static_cast<int16_t>(full_h * panel_expand);\n"
    "\n"
    "\t\t\t\tif (reveal < 1)\n"
    "\t\t\t\t\treveal = 1;\n"
    "\n"
    "\t\t\t\tif (reveal < full_h)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\t// 自上而下卷轴展开：目标矩形必须是「完整面板矩形」(左上角 panel_c)，\n"
    "\t\t\t\t\t// 由 Range 同时裁剪源与目标底边。源取顶部 reveal 行、目标高度亦为\n"
    "\t\t\t\t\t// reveal，1:1 纵向揭示。若把 reveal 当作 DrawArgument 的 stretch 高度，\n"
    "\t\t\t\t\t// 目标矩形先被压成 reveal 高、再被 Range 二次裁掉 (full_h-reveal)，\n"
    "\t\t\t\t\t// 实际目标高度 = 2*reveal - full_h（中段为负/零）→ 整段动画不绘制，\n"
    "\t\t\t\t\t// 仅 panel_expand 到 1 时整板瞬现，表现为「卷轴不滚动」。\n"
    "\t\t\t\t\tchannels_background.draw(\n"
    "\t\t\t\t\t\tDrawArgument(position + panel_c),\n"
    "\t\t\t\t\t\tRange<int16_t>(0, static_cast<int16_t>(full_h - reveal))\n"
    "\t\t\t\t\t);\n"
    "\t\t\t\t}\n"
    "\t\t\t\telse\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tchannels_background.draw(position + panel_c);\n"
    "\t\t\t\t}\n"
    "\t\t\t}",
    "\t\t\telse if (channels_scroll_open.is_valid())\n"
    "\t\t\t{\n"
    "\t\t\t\t// m08208 ③④：真帧 scroll/0/3 打开态在目标帧 alpha-NCC=0.7546@(398,139)。\n"
    "\t\t\t\t// 选中世界动画播完后常驻此帧（不再切回 chBackgrn 平板），落点与展开\n"
    "\t\t\t\t// 动画共用同一 scroll_tl；稳态 panel_expand=1 => 整帧绘制，无跳变。\n"
    "\t\t\t\tconst Point<int16_t> scroll_tl =\n"
    "\t\t\t\t\tpanel_c - Point<int16_t>(CH_PANEL_W / 2, CH_PANEL_H / 2) + WS_SCROLL_LOCAL;\n"
    "\t\t\t\tconst int16_t full_h = channels_scroll_open.get_dimensions().y();\n"
    "\t\t\t\tint16_t reveal = static_cast<int16_t>(full_h * panel_expand);\n"
    "\n"
    "\t\t\t\tif (reveal < 1)\n"
    "\t\t\t\t\treveal = 1;\n"
    "\n"
    "\t\t\t\tif (reveal < full_h)\n"
    "\t\t\t\t\tchannels_scroll_open.draw(\n"
    "\t\t\t\t\t\tDrawArgument(position + scroll_tl),\n"
    "\t\t\t\t\t\tRange<int16_t>(0, static_cast<int16_t>(full_h - reveal))\n"
    "\t\t\t\t\t);\n"
    "\t\t\t\telse\n"
    "\t\t\t\t\tchannels_scroll_open.draw(position + scroll_tl);\n"
    "\t\t\t}\n"
    "\t\t\telse\n"
    "\t\t\t{\n"
    "\t\t\t\t// 无 scroll/0/3 资源时保留原 chBackgrn 纵向揭示，保证不崩、不黑屏。\n"
    "\t\t\t\tconst int16_t full_h = channels_background.get_dimensions().y();\n"
    "\t\t\t\tint16_t reveal = static_cast<int16_t>(full_h * panel_expand);\n"
    "\n"
    "\t\t\t\tif (reveal < 1)\n"
    "\t\t\t\t\treveal = 1;\n"
    "\n"
    "\t\t\t\tif (reveal < full_h)\n"
    "\t\t\t\t\tchannels_background.draw(\n"
    "\t\t\t\t\t\tDrawArgument(position + panel_c),\n"
    "\t\t\t\t\t\tRange<int16_t>(0, static_cast<int16_t>(full_h - reveal))\n"
    "\t\t\t\t\t);\n"
    "\t\t\t\telse\n"
    "\t\t\t\t\tchannels_background.draw(position + panel_c);\n"
    "\t\t\t}",
)

E(
    "send_cursor_scroll_bounds",
    WORLDSEL_CPP,
    "\t\tRectangle<int16_t> channels_bounds = Rectangle<int16_t>(\n"
    "\t\t\tposition + panel_topleft,\n"
    "\t\t\tposition + panel_topleft + channels_background.get_dimensions()\n"
    "\t\t\t);",
    "\t\t// m08208：卷轴打开态 513x401 从 scroll_tl=(398,139) 覆盖到 (911,540)，完全\n"
    "\t\t// 包含 chBackgrn 面板 (436,280)..(885,513)；用它做命中框，点顶轴/底卷边\n"
    "\t\t// 不会误判为「面板外取消选中」。无资源时回退原面板矩形。\n"
    "\t\tconst Point<int16_t> scroll_tl =\n"
    "\t\t\tpanel_c - Point<int16_t>(CH_PANEL_W / 2, CH_PANEL_H / 2) + WS_SCROLL_LOCAL;\n"
    "\t\tRectangle<int16_t> channels_bounds = channels_scroll_open.is_valid()\n"
    "\t\t\t? Rectangle<int16_t>(\n"
    "\t\t\t\tposition + scroll_tl,\n"
    "\t\t\t\tposition + scroll_tl + channels_scroll_open.get_dimensions())\n"
    "\t\t\t: Rectangle<int16_t>(\n"
    "\t\t\t\tposition + panel_topleft,\n"
    "\t\t\t\tposition + panel_topleft + channels_background.get_dimensions());",
)

E(
    "btn_pressed_comment_scroll_open",
    WORLDSEL_CPP,
    "\t\t\t// 局部 (-38,-141)；播完由 update() 置 scroll_playing=false 转交 chBackgrn。",
    "\t\t\t// 局部 (-38,-141)；播完由 update() 置 scroll_playing=false 转交\n"
    "\t\t\t// scroll/0/3 打开态常驻（m08208 ③④，不再切回 chBackgrn 平板）。",
)


def main():
    data = {}
    for _, path, _, _ in EDITS:
        if path not in data:
            with open(path, "rb") as f:
                data[path] = f.read()

    ok = True
    for label, path, old, new in EDITS:
        n = data[path].count(old.encode("utf-8"))
        if n != 1:
            print("PRECHECK FAIL %-38s count=%d  %s" % (label, n, path))
            ok = False
        else:
            print("PRECHECK OK   %-38s count=1" % label)
        # 每条替换的花括号净变化必须为 0（防止把 if(world_selected) 的闭合括号重复吃掉）
        ob = old.count("{") - old.count("}")
        nb = new.count("{") - new.count("}")
        if ob != nb:
            print("BRACE  FAIL %-38s old_delta=%+d new_delta=%+d" % (label, ob, nb))
            ok = False
    if not ok:
        print("ABORT: zero write (PRECHECK / BRACE not clean)")
        sys.exit(1)

    for label, path, old, new in EDITS:
        o = old.encode("utf-8")
        nn = new.encode("utf-8")
        assert data[path].count(o) == 1, label
        data[path] = data[path].replace(o, nn, 1)
        print("APPLIED       %s" % label)

    # 全文件花括号净平衡：模拟替换后与原文件必须一致
    for path, buf in data.items():
        with open(path, "rb") as f:
            orig = f.read()
        ob = orig.count(b"{") - orig.count(b"}")
        nb = buf.count(b"{") - buf.count(b"}")
        if ob != nb:
            print("FILE BRACE FAIL %s orig=%+d after=%+d" % (path, ob, nb))
            sys.exit(1)
        assert buf[:3] == orig[:3], (path, "BOM changed")
        assert buf.count(b"\r\n") == orig.count(b"\r\n"), (path, "CRLF changed")
        print("BYTES OK      %s  brace=%+d crlf=%d" % (path, nb, buf.count(b"\r\n")))

    for path, buf in data.items():
        with open(path, "wb") as f:
            f.write(buf)
        print("WROTE %s (%d bytes)" % (path, len(buf)))

    print("PATCHED %d files ALL_OK" % len(data))


if __name__ == "__main__":
    main()
