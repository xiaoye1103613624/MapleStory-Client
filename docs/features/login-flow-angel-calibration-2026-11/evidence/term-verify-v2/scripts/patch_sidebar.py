# -*- coding: utf-8 -*-
"""选角色页/职业团页左侧栏四块木牌补齐。
带唯一锚点断言，不猜 tab 缩进（缩进从文件动态取），任一 patch 失败则零写盘。
"""
import os

ROOT = r"E:\project\MapleStory-Client"
FILES = {
    "char_h":   os.path.join(ROOT, "IO", "UITypes", "UICharSelect.h"),
    "char_cpp": os.path.join(ROOT, "IO", "UITypes", "UICharSelect.cpp"),
    "race_h":   os.path.join(ROOT, "IO", "UITypes", "UIRaceSelect.h"),
    "race_cpp": os.path.join(ROOT, "IO", "UITypes", "UIRaceSelect.cpp"),
}

# (op, key, anchor, lines);  lines = [(extra_tabs_relative_to_anchor_indent, content), ...]
# op: IA=insert after anchor line, IB=insert before, RL=replace anchor line
PATCHES = [
    # ---- UICharSelect.h : 4 个装饰成员 ----
    ("IA", "char_h", "Texture charinfo;", [
        (0, "// 左侧栏四块（选角色页 step2）：标题牌 y36-85 / 世界信息条 y99-148 /"),
        (0, "// 查看所有角色 y377-399 / 回到首页 y427-449；照 UICommonCreation.cpp:492-534。"),
        (0, "// 本页 position 是相机值，装饰以屏幕绝对坐标单独绘制，按钮局部=绝对−position。"),
        (0, "Sprite cs_step;"),
        (0, "Sprite cs_world_box;"),
        (0, "Text cs_world_name;"),
        (0, "Text cs_channel;"),
    ]),
    # ---- UICharSelect.cpp : include ----
    ("IA", "char_cpp", '#include "../Components/AreaButton.h"', [
        (0, '#include "../Components/LocalTwoSpriteButton.h"'),
    ]),
    # ---- UICharSelect.cpp : 构造装饰 ----
    ("IA", "char_cpp", 'world_sprites.emplace_back(selectedWorld["ch"][channel_id]);', [
        (0, ""),
        (0, "// 左侧栏四块：照 UICommonCreation.cpp:492-534 已验证 classic 侧栏；"),
        (0, "// stateargs 用屏幕绝对坐标，draw 传 (0,0) 不叠加相机 position。"),
        (0, 'cs_step = Sprite(Common["step"]["3"], Point<int16_t>(0, 33));'),
        (0, 'cs_world_box = Sprite(Common["selectWorld"], Point<int16_t>(0, 96));'),
        (0, ""),
        (0, "std::string cs_wname;"),
        (0, "uint8_t cs_wch = 0xFF;"),
        (0, "if (auto ws = UI::get().get_element<UIWorldSelect>())"),
        (0, "{"),
        (1, "if (const World* w = ws->get_selected_world())"),
        (2, "cs_wname = w->name;"),
        (1, "cs_wch = ws->get_channelid();"),
        (0, "}"),
        (0, "if (cs_wch == 0xFF)"),
        (1, "cs_wch = Configuration::get().get_channelid();"),
        (0, "for (char& c : cs_wname)"),
        (1, "if (c >= 'a' && c <= 'z')"),
        (2, "c = static_cast<char>(c - 'a' + 'A');"),
        (0, "cs_world_name = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, cs_wname);"),
        (0, "cs_channel = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE,"),
        (1, '"CH." + std::to_string(cs_wch + 1));'),
    ]),
    # ---- UICharSelect.cpp : 重定位 BtCharacter = 查看所有角色 ----
    ("RL", "char_cpp",
     'buttons[Buttons::BtCharacter] = std::make_unique<MapleButton>(CharSelect["EditCharList"]["BtCharacter"]);', [
        (0, "// 左侧栏「查看所有角色 / 回到首页」：屏幕绝对 (0,377)/(0,427)，"),
        (0, "// 局部坐标 = 绝对 − position（draw_buttons 会再加回 position）。"),
        (0, "const Point<int16_t> cs_sidebar = Point<int16_t>(0, 0) - position;"),
        (0, ""),
        (0, "LocalPngTexture cs_btva_normal;"),
        (0, "LocalPngTexture cs_btva_pressed;"),
        (0, "constexpr size_t CS_ATLAS_BTVA = 0x43534254ull;  // 'CSBT'"),
        (0, "bool cs_btva_ok = cs_btva_normal.load(CS_ATLAS_BTVA, {"),
        (1, '"UI/Creation/BtViewAll_normal.png",'),
        (1, '"docs/features/login-flow-ui-ida-align/resources/creation/BtViewAll_normal.png" })'),
        (1, "&& cs_btva_pressed.load(CS_ATLAS_BTVA + 1, {"),
        (1, '"UI/Creation/BtViewAll_pressed.png",'),
        (1, '"docs/features/login-flow-ui-ida-align/resources/creation/BtViewAll_pressed.png" });'),
        (0, ""),
        (0, "if (cs_btva_ok)"),
        (1, "buttons[Buttons::BtCharacter] = std::make_unique<LocalTwoSpriteButton>("),
        (2, "std::move(cs_btva_normal), std::move(cs_btva_pressed), cs_sidebar + Point<int16_t>(0, 377));"),
        (0, "else"),
        (1, "buttons[Buttons::BtCharacter] = std::make_unique<MapleButton>("),
        (2, 'Login["WorldSelect"]["BtViewAll"], cs_sidebar + Point<int16_t>(0, 377));'),
    ]),
    # ---- UICharSelect.cpp : 重定位 BtPreview = 回到首页（资产 Common/BtStart）----
    ("RL", "char_cpp",
     'buttons[Buttons::BtPreview] = std::make_unique<MapleButton>(Common["BtPreview"]);', [
        (0, 'buttons[Buttons::BtPreview] = std::make_unique<MapleButton>(Common["BtStart"], cs_sidebar + Point<int16_t>(0, 427));'),
    ]),
    # ---- UICharSelect.cpp : draw 装饰（绝对坐标）----
    ("IB", "char_cpp", "version.draw(position + version_pos - Point<int16_t>(0, 5));", [
        (0, "// 左侧栏四块：屏幕绝对坐标，draw 传 (0,0) 不叠加相机 position。"),
        (0, "cs_step.draw(Point<int16_t>(0, 0), inter);"),
        (0, "cs_world_box.draw(Point<int16_t>(0, 0), inter);"),
        (0, "cs_world_name.draw(Point<int16_t>(99, 111));"),
        (0, "cs_channel.draw(Point<int16_t>(99, 127));"),
        (0, ""),
    ]),

    # ---- UIRaceSelect.h : 4 个装饰成员 ----
    ("IA", "race_h", "Text classic_card_label[CLASSIC_CLASS_COUNT];", [
        (0, "// 左侧栏四块（职业团页 step3）：与正确端一致，照 UICommonCreation"),
        (0, "// 已验证 classic 侧栏；本页 position=(240,40)，装饰以屏幕绝对坐标绘制。"),
        (0, "Sprite rs_step;"),
        (0, "Sprite rs_world_box;"),
        (0, "Text rs_world_name;"),
        (0, "Text rs_channel;"),
    ]),
    # ---- UIRaceSelect.cpp : include ----
    ("IA", "race_cpp", '#include "../../Graphics/Text.h"', [
        (0, '#include "../Components/LocalTwoSpriteButton.h"'),
        (0, '#include "../../Net/Packets/SelectCharPackets.h"'),
    ]),
    # ---- UIRaceSelect.cpp : init_classic 装饰 + BtPreview(ViewAll) + BtStart 重定位 ----
    ("RL", "race_cpp",
     'buttons[Buttons::BtStart] = std::make_unique<MapleButton>(Common["BtStart"], Point<int16_t>(0, 1));', 1, [
        (0, "// 左侧栏四块（职业团页）：标题牌/世界框/查看所有角色/回到首页，"),
        (0, "// 屏幕绝对 (0,33)/(0,96)/(0,377)/(0,427)；本页 position=(240,40)，"),
        (0, "// 装饰以绝对坐标单独绘制，按钮局部坐标 = 绝对 − position。"),
        (0, 'rs_step = Sprite(Common["step"]["3"], Point<int16_t>(0, 33));'),
        (0, 'rs_world_box = Sprite(Common["selectWorld"], Point<int16_t>(0, 96));'),
        (0, ""),
        (0, "std::string rs_wname;"),
        (0, "uint8_t rs_wch = 0xFF;"),
        (0, "if (auto ws = UI::get().get_element<UIWorldSelect>())"),
        (0, "{"),
        (1, "if (const World* w = ws->get_selected_world())"),
        (2, "rs_wname = w->name;"),
        (1, "rs_wch = ws->get_channelid();"),
        (0, "}"),
        (0, "if (rs_wch == 0xFF)"),
        (1, "rs_wch = Configuration::get().get_channelid();"),
        (0, "for (char& c : rs_wname)"),
        (1, "if (c >= 'a' && c <= 'z')"),
        (2, "c = static_cast<char>(c - 'a' + 'A');"),
        (0, "rs_world_name = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, rs_wname);"),
        (0, "rs_channel = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE,"),
        (1, '"CH." + std::to_string(rs_wch + 1));'),
        (0, ""),
        (0, "const Point<int16_t> rs_sidebar = Point<int16_t>(0, 0) - position;"),
        (0, ""),
        (0, "LocalPngTexture rs_btva_normal;"),
        (0, "LocalPngTexture rs_btva_pressed;"),
        (0, "constexpr size_t RS_ATLAS_BTVA = 0x52534254ull;  // 'RSBT'"),
        (0, "bool rs_btva_ok = rs_btva_normal.load(RS_ATLAS_BTVA, {"),
        (1, '"UI/Creation/BtViewAll_normal.png",'),
        (1, '"docs/features/login-flow-ui-ida-align/resources/creation/BtViewAll_normal.png" })'),
        (1, "&& rs_btva_pressed.load(RS_ATLAS_BTVA + 1, {"),
        (1, '"UI/Creation/BtViewAll_pressed.png",'),
        (1, '"docs/features/login-flow-ui-ida-align/resources/creation/BtViewAll_pressed.png" });'),
        (0, ""),
        (0, "if (rs_btva_ok)"),
        (1, "buttons[Buttons::BtPreview] = std::make_unique<LocalTwoSpriteButton>("),
        (2, "std::move(rs_btva_normal), std::move(rs_btva_pressed), rs_sidebar + Point<int16_t>(0, 377));"),
        (0, "else"),
        (1, "buttons[Buttons::BtPreview] = std::make_unique<MapleButton>("),
        (2, 'Login["WorldSelect"]["BtViewAll"], rs_sidebar + Point<int16_t>(0, 377));'),
        (0, ""),
        (0, 'buttons[Buttons::BtStart] = std::make_unique<MapleButton>(Common["BtStart"], rs_sidebar + Point<int16_t>(0, 427));'),
    ]),
    # ---- UIRaceSelect.cpp : classic draw 装饰 ----
    ("IA", "race_cpp", "classic_text_gl.draw(position + Point<int16_t>(315, 52));", [
        (0, ""),
        (0, "// 左侧栏四块：屏幕绝对坐标，draw 传 (0,0) 不叠加相机 position。"),
        (0, "rs_step.draw(Point<int16_t>(0, 0), inter);"),
        (0, "rs_world_box.draw(Point<int16_t>(0, 0), inter);"),
        (0, "rs_world_name.draw(Point<int16_t>(99, 111));"),
        (0, "rs_channel.draw(Point<int16_t>(99, 127));"),
    ]),
    # ---- UIRaceSelect.cpp : button_pressed classic 的 BtPreview ----
    ("IB", "race_cpp", "if (buttonid == Buttons::BtStart)", [
        (0, "if (classic_mode && buttonid == Buttons::BtPreview)"),
        (0, "{"),
        (1, "// 左侧栏「查看所有角色」：VIEW_ALL_CHAR(13)，与 UIWorldSelect 一致。"),
        (1, "ViewAllCharPacket().dispatch();"),
        (1, "return Button::State::NORMAL;"),
        (0, "}"),
        (0, ""),
    ]),
]


def read(key):
    with open(FILES[key], "r", encoding="utf-8", newline="") as f:
        return f.read()


def norm(raw):
    return (raw.replace("\r\n", "\n"), "\r\n" in raw)


def denorm(work, crlf):
    return work.replace("\n", "\r\n") if crlf else work


def locate(work, anchor, nth=None):
    cnt = work.count(anchor)
    if nth is None:
        assert cnt == 1, "ANCHOR count=%d for %r" % (cnt, anchor[:72])
        idx = work.index(anchor)
    else:
        assert cnt >= nth, "ANCHOR count=%d < nth=%d for %r" % (cnt, nth, anchor[:72])
        idx = -1
        for _ in range(nth):
            idx = work.index(anchor, idx + 1)
    ls = work.rfind("\n", 0, idx) + 1
    lead = work[ls:idx]
    assert lead.strip() == "", "ANCHOR not at line-start, lead=%r" % lead
    le = work.find("\n", idx)
    if le < 0:
        le = len(work)
    return idx, ls, le, lead


def render(lead, lines):
    return "\n".join(lead + "\t" * e + c for e, c in lines)


def apply(work, patches):
    for item in patches:
        op, anchor, lines = item[0], item[1], item[2]
        nth = item[3] if len(item) > 3 else None
        idx, ls, le, lead = locate(work, anchor, nth)
        block = render(lead, lines)
        if op == "IA":
            work = work[:le] + "\n" + block + work[le:]
        elif op == "IB":
            work = work[:ls] + block + "\n" + work[ls:]
        elif op == "RL":
            work = work[:ls] + block + work[le:]
        else:
            raise ValueError(op)
    return work


def main():
    by = {}
    for item in PATCHES:
        op, key, anchor = item[0], item[1], item[2]
        if len(item) == 5:
            nth, lines = item[3], item[4]
        else:
            nth, lines = None, item[3]
        by.setdefault(key, []).append((op, anchor, lines, nth))
    out = {}
    for key, ps in by.items():                 # 先全在内存跑，任一失败零写盘
        raw = read(key)
        work, crlf = norm(raw)
        out[key] = (denorm(apply(work, ps), crlf), raw)
    for key, (new, raw) in out.items():
        assert new != raw, "NO CHANGE " + key
        with open(FILES[key], "w", encoding="utf-8", newline="") as f:
            f.write(new)
        print("PATCHED", os.path.relpath(FILES[key], ROOT), "+%d bytes" % (len(new) - len(raw)))
    print("ALL_OK", len(out), "files /", len(PATCHES), "patches")


if __name__ == "__main__":
    main()
