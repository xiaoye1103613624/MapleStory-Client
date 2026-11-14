# -*- coding: utf-8 -*-
"""version 修复收口：UICommonCreation modern 分支同类空节点 + draw 统一屏幕绝对坐标。
classic 分支(:487)已是 BLACK+(1061,9) gold reference，两端验收一致，不动。
modern 分支(:98/:101)仍 LEMONGRASS+空节点；:967 用 position+ 会随相机画飞。
"""
import os
BASE = r"E:\project\MapleStory-Client"
P = "IO/UITypes/UICommonCreation.cpp"
patches = [
    (
        '\tvoid UICommonCreation::init_modern(nl::node Login, nl::node Common)\n'
        '\t{\n'
        '\t\tstd::string version_text = Configuration::get().get_version();\n'
        '\t\tversion = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::LEMONGRASS);\n'
        '\t\tbind_text(version, "version.prefix", version_text);\n'
        '\n'
        '\t\tversion_pos = Common["version"]["pos"];',
        '\tvoid UICommonCreation::init_modern(nl::node Login, nl::node Common)\n'
        '\t{\n'
        '\t\tstd::string version_text = Configuration::get().get_version();\n'
        '\t\tversion = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::BLACK, "Ver. " + version_text);\n'
        '\n'
        '\t\tversion_pos = Common["version"]["pos"];\n'
        '\t\tif (!version_pos.x() && !version_pos.y())\n'
        '\t\t\tversion_pos = Point<int16_t>(1061, 9);',
    ),
    (
        '\t\tif (version_pos.x() || version_pos.y() || !classic_mode)\n'
        '\t\t\tversion.draw(position + version_pos - Point<int16_t>(0, 5));',
        '\t\tif (version_pos.x() || version_pos.y() || !classic_mode)\n'
        '\t\t\tversion.draw(version_pos - Point<int16_t>(0, 5));',
    ),
]
p = os.path.join(BASE, P)
raw = open(p, "rb").read()
crlf, lf = raw.count(b"\r\n"), raw.count(b"\n")
if crlf == lf and crlf > 0:
    eol, txt = "\r\n", raw.decode("utf-8").replace("\r\n", "\n")
elif crlf == 0:
    eol, txt = "\n", raw.decode("utf-8")
else:
    raise AssertionError("混合行尾")
for i, (old, new) in enumerate(patches):
    c = txt.count(old)
    assert c == 1, "patch#%d 命中 %d 次" % (i, c)
    txt = txt.replace(old, new, 1)
out = (txt.replace("\n", eol) if eol == "\r\n" else txt).encode("utf-8")
open(p, "wb").write(out)
print("PATCHED %s eol=%s %d patches" % (P, repr(eol), len(patches)))
