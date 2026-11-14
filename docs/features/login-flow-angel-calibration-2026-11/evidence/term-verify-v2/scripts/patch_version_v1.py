# -*- coding: utf-8 -*-
"""四页登录链版本号修复：LEMONGRASS+空节点(画飞/不可见) → BLACK+屏幕绝对右上(1061,9)。
证据：orange-wz 实读 Common 12 子节点无 version；5 页 ROI 暗像素投影目标 130px/本端 0；
UICommonCreation.cpp:487-490 建角页 gold reference (1061,9)+BLACK+A12B 两端已验证一致。
draw 去掉 position+：UICharSelect/UIRaceSelect position 为相机偏移会把 HUD 画飞，
版本号本质是屏幕绝对坐标（与创建页一致）。gold reference 创建页不动。
"""
import os, sys

BASE = r"E:\project\MapleStory-Client"

# 每个 (old,new) 用 \n；脚本先归一 CRLF→LF 做替换，写盘再还原 CRLF。
PATCHES = {
    "IO/UITypes/UILogin.cpp": [
        (
            '\t\tversion = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::LEMONGRASS);\n'
            '\t\t// 前缀进绑定表：版本号在会话内恒定，「Ver. 」才是会被语言切换换掉的那半截。\n'
            '\t\tbind_text(version, "version.prefix", version_text);\n'
            '\t\tversion_pos = nl::nx::UI["Login.img"]["Common"]["version"]["pos"];\n'
            '\n'
            '\t\tnl::node Login = nl::nx::UI["Login.img"];\n'
            '\t\tversion_pos = Login["Common"]["version"]["pos"];',
            '\t\tversion = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::BLACK, "Ver. " + version_text);\n'
            '\n'
            '\t\tnl::node Login = nl::nx::UI["Login.img"];\n'
            '\t\tversion_pos = Login["Common"]["version"]["pos"];\n'
            '\t\t// BeiDou live 端缺 Common/version 节点；正确端「Ver. 0.83」是屏幕右上角固定 HUD。\n'
            '\t\tif (!version_pos.x() && !version_pos.y())\n'
            '\t\t\tversion_pos = Point<int16_t>(1061, 9);',
        ),
        (
            '\t\tversion.draw(position + version_pos - Point<int16_t>(0, 5));',
            '\t\tversion.draw(version_pos - Point<int16_t>(0, 5));',
        ),
    ],
    "IO/UITypes/UIWorldSelect.cpp": [
        (
            '\t\tstd::string version_text = Configuration::get().get_version();\n'
            '\t\tversion = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::LEMONGRASS, "Ver. " + version_text);\n'
            '\t\tversion_pos = nl::nx::UI["Login.img"]["Common"]["version"]["pos"];',
            '\t\tstd::string version_text = Configuration::get().get_version();\n'
            '\t\tversion = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::BLACK, "Ver. " + version_text);\n'
            '\t\tversion_pos = nl::nx::UI["Login.img"]["Common"]["version"]["pos"];\n'
            '\t\tif (!version_pos.x() && !version_pos.y())\n'
            '\t\t\tversion_pos = Point<int16_t>(1061, 9);',
        ),
        (
            '\t\tversion.draw(position + version_pos - Point<int16_t>(0, 5));',
            '\t\tversion.draw(version_pos - Point<int16_t>(0, 5));',
        ),
    ],
    "IO/UITypes/UICharSelect.cpp": [
        (
            '\t\tstd::string version_text = Configuration::get().get_version();\n'
            '\t\tversion = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::LEMONGRASS);\n'
            '\t\tbind_text(version, "version.prefix", version_text);\n'
            '\n'
            '\t\tnl::node Login = nl::nx::UI["Login.img"];\n'
            '\t\tnl::node Common = Login["Common"];\n'
            '\t\tversion_pos = Common["version"]["pos"];',
            '\t\tstd::string version_text = Configuration::get().get_version();\n'
            '\t\tversion = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::BLACK, "Ver. " + version_text);\n'
            '\n'
            '\t\tnl::node Login = nl::nx::UI["Login.img"];\n'
            '\t\tnl::node Common = Login["Common"];\n'
            '\t\tversion_pos = Common["version"]["pos"];\n'
            '\t\tif (!version_pos.x() && !version_pos.y())\n'
            '\t\t\tversion_pos = Point<int16_t>(1061, 9);',
        ),
        (
            '\t\tversion.draw(position + version_pos - Point<int16_t>(0, 5));',
            '\t\tversion.draw(version_pos - Point<int16_t>(0, 5));',
        ),
    ],
    "IO/UITypes/UIRaceSelect.cpp": [
        (
            '\t\tstd::string version_text = Configuration::get().get_version();\n'
            '\t\tversion = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::LEMONGRASS);\n'
            '\t\tbind_text(version, "version.prefix", version_text);\n'
            '\t\tversion_pos = nl::nx::UI["Login.img"]["Common"]["version"]["pos"];',
            '\t\tstd::string version_text = Configuration::get().get_version();\n'
            '\t\tversion = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::BLACK, "Ver. " + version_text);\n'
            '\t\tversion_pos = nl::nx::UI["Login.img"]["Common"]["version"]["pos"];\n'
            '\t\tif (!version_pos.x() && !version_pos.y())\n'
            '\t\t\tversion_pos = Point<int16_t>(1061, 9);',
        ),
        (
            '\tvoid UIRaceSelect::init_classic()\n'
            '\t{\n'
            '\t\tnl::node Login = nl::nx::UI["Login.img"];\n'
            '\t\tnl::node Common = Login["Common"];\n'
            '\t\tnl::node RaceSelect = Login["RaceSelect"];',
            '\tvoid UIRaceSelect::init_classic()\n'
            '\t{\n'
            '\t\tstd::string version_text = Configuration::get().get_version();\n'
            '\t\tversion = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::BLACK, "Ver. " + version_text);\n'
            '\t\tversion_pos = Point<int16_t>(1061, 9);\n'
            '\n'
            '\t\tnl::node Login = nl::nx::UI["Login.img"];\n'
            '\t\tnl::node Common = Login["Common"];\n'
            '\t\tnl::node RaceSelect = Login["RaceSelect"];',
        ),
        (
            '\t\t\trs_channel.draw(Point<int16_t>(99, 127));\n'
            '\n'
            '\t\t\tUIElement::draw_buttons(inter);',
            '\t\t\trs_channel.draw(Point<int16_t>(99, 127));\n'
            '\t\t\tversion.draw(version_pos - Point<int16_t>(0, 5));\n'
            '\n'
            '\t\t\tUIElement::draw_buttons(inter);',
        ),
        (
            '\t\tversion.draw(position + version_pos - Point<int16_t>(0, 5));',
            '\t\tversion.draw(version_pos - Point<int16_t>(0, 5));',
        ),
    ],
}

# 先在内存全部替换并断言，任一失败零写盘。
staged = {}
for rel, patches in PATCHES.items():
    p = os.path.join(BASE, rel)
    with open(p, "rb") as f:
        raw = f.read()
    crlf = raw.count(b"\r\n")
    lf = raw.count(b"\n")
    # 逐文件自适应：纯 CRLF 保持 CRLF、纯 LF 保持 LF，混合行尾直接拒绝，
    # 避免把整文件 diff 污染成另一种换行符。
    if crlf == lf and crlf > 0:
        eol = "\r\n"; txt = raw.decode("utf-8").replace("\r\n", "\n")
    elif crlf == 0:
        eol = "\n"; txt = raw.decode("utf-8")
    else:
        raise AssertionError("%s 混合行尾 (crlf=%d lf=%d)" % (rel, crlf, lf))
    for i, (old, new) in enumerate(patches):
        c = txt.count(old)
        assert c == 1, "%s patch#%d 命中 %d 次(应 1):\n%s" % (rel, i, c, old[:120])
        txt = txt.replace(old, new, 1)
    staged[p] = (txt.replace("\n", eol) if eol == "\r\n" else txt).encode("utf-8")
    print("OK %-38s %d patches eol=%s" % (rel, len(patches), repr(eol)))

for p, data in staged.items():
    with open(p, "wb") as f:
        f.write(data)
print("PATCHED %d files" % len(staged))
