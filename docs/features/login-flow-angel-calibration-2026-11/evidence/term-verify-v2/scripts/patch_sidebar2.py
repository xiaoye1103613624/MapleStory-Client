# -*- coding: utf-8 -*-
"""蓝军自检收窄修正（只改 UICharSelect，UIRaceSelect 第一版保持不动）：

问题：第一版把 UICharSelect 的 BtPreview 贴图换成「回到首页」(Common/BtStart,
(0,427))，但 button_pressed(BtPreview) 仍是「回选世界页 set_step(1)」，且 ESC
send_key 也调 BtPreview → 贴图与行为不一致，违反用户「点击后页面变化清楚」。

修正：新增专用枚举 BtHome（插在 CHARACTER_SLOT0 之前，仍 < SLOT0 故 send_cursor
第一遍会响应），保留 BtPreview 原贴图/原逻辑/ESC；BtHome 走与
UIWorldSelect.cpp:1350-1363 一致的 reconnect→登录页。

不给 UIRaceSelect 插枚举：CLASS0 被 modern 分支 order[...-CLASS0] / for(i<=CLASS0)
按绝对值依赖，shift 会炸；classic 下 BtPreview 本就空闲，第一版复用安全。
UICharSelect 的 CHARACTER_SLOT0 全部是相对引用（+i / -SLOT0 / >=SLOT0），
grep 无硬编码 9，插入安全。
"""
import os

ROOT = r"E:\project\MapleStory-Client"
F = {
    "ch": os.path.join(ROOT, "IO", "UITypes", "UICharSelect.h"),
    "cc": os.path.join(ROOT, "IO", "UITypes", "UICharSelect.cpp"),
}

PATCHES = [
    # --- 枚举：BtPreview 之后、CHARACTER_SLOT0 之前插 BtHome ---
    ("IB", "ch", "\t\t\tCHARACTER_SLOT0", [
        (0, "BtHome,          // 左侧栏「回到首页」：reconnect → 登录页（区别于 BtPreview 回选世界页）"),
    ]),
    # --- includes ---
    ("IA", "cc", '#include "UILoginNotice.h"', [
        (0, '#include "UILogin.h"'),
    ]),
    ("IA", "cc", '#include "../../Graphics/Geometry.h"', [
        (0, '#include "../../Net/Session.h"'),
    ]),
    # --- 恢复 BtPreview 原构造 + 新增 BtHome ---
    ("RL", "cc",
     'buttons[Buttons::BtPreview] = std::make_unique<MapleButton>(Common["BtStart"], cs_sidebar + Point<int16_t>(0, 427));', [
        (0, 'buttons[Buttons::BtPreview] = std::make_unique<MapleButton>(Common["BtPreview"]);'),
        (0, 'buttons[Buttons::BtHome] = std::make_unique<MapleButton>(Common["BtStart"], cs_sidebar + Point<int16_t>(0, 427));'),
    ]),
    # --- button_pressed 新增 case BtHome ---
    ("IB", "cc", "\t\t\tcase Buttons::BtCharacter:", [
        (0, "case Buttons::BtHome:"),
        (0, "{"),
        (1, "// 左侧栏「回到首页」：与 UIWorldSelect.cpp:1350-1363 BtStart 一致，"),
        (1, "// 重置会话后回登录页；BtPreview/ESC 仍保持原有「回选世界页」行为。"),
        (1, "Sound(Sound::Name::SCROLLUP).play();"),
        (1, "Session::get().reconnect();"),
        (1, "if (LoginBackground::get().available())"),
        (2, "LoginBackground::get().set_step(0);"),
        (1, "UI::get().remove(UIElement::Type::CHARSELECT);"),
        (1, "UI::get().remove(UIElement::Type::LOGINWAIT);"),
        (1, "UI::get().remove(UIElement::Type::LOGINNOTICE);"),
        (1, "UI::get().emplace<UILogin>();"),
        (1, "break;"),
        (0, "}"),
    ]),
]


def read(k):
    with open(F[k], "r", encoding="utf-8", newline="") as f:
        return f.read()


def locate(work, anchor):
    cnt = work.count(anchor)
    assert cnt == 1, "count=%d for %r" % (cnt, anchor[:80])
    idx = work.index(anchor)
    ls = work.rfind("\n", 0, idx) + 1
    lead = work[ls:idx]
    assert lead.strip() == "", "not line-start lead=%r" % lead
    le = work.find("\n", idx + len(anchor))
    if le < 0:
        le = len(work)
    return ls, le, lead


def render(lead, lines):
    return "\n".join(lead + "\t" * e + c for e, c in lines)


def main():
    by = {}
    for op, k, anchor, lines in PATCHES:
        by.setdefault(k, []).append((op, anchor, lines))
    out = {}
    for k, ps in by.items():                      # 全在内存，任一失败零写盘
        raw = read(k)
        crlf = "\r\n" in raw
        work = raw.replace("\r\n", "\n")
        for op, anchor, lines in ps:
            ls, le, lead = locate(work, anchor)
            block = render(lead, lines)
            if op == "IA":
                work = work[:le] + "\n" + block + work[le:]
            elif op == "IB":
                work = work[:ls] + block + "\n" + work[ls:]
            elif op == "RL":
                work = work[:ls] + block + work[le:]
        new = work.replace("\n", "\r\n") if crlf else work
        assert new != raw, "NO CHANGE " + k
        out[k] = new
    for k, new in out.items():
        with open(F[k], "w", encoding="utf-8", newline="") as f:
            f.write(new)
        print("PATCHED", os.path.relpath(F[k], ROOT))
    print("ALL_OK", len(out), "files /", len(PATCHES), "patches")


if __name__ == "__main__":
    main()
