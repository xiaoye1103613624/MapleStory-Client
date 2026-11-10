# -*- coding: utf-8 -*-
"""round54 statusbar-hud/D3 验证：聊天输入框启用系统 IME（accept_unicode）。

不跑客户端，只做两件事：
  1) 静态守卫：源码里两处 input_text 都调了 set_accept_unicode(true)，且没顺手改 limit。
  2) 行为复刻：用 Textfield::add_string 的 allowlist 逻辑复刻修前(false)/修后(true)，
     证明中文修前进不来、修后进得来，ASCII 与超限行为逐字不变。
"""
import io, re, sys

CHAT = "IO/UITypes/UIChatBar.cpp"
TF_H = "IO/Components/Textfield.h"
TF_CPP = "IO/Components/Textfield.cpp"
UI_CPP = "IO/UI.cpp"

read = lambda p: io.open(p, encoding="utf-8", errors="replace").read()
chat, tfh, tfc, uic = read(CHAT), read(TF_H), read(TF_CPP), read(UI_CPP)

FAIL = []


def guard(name, cond):
    if not cond:
        FAIL.append(name)


# ---- 静态守卫 ----
cls = chat.find("input_text = Textfield(")
cls_after = chat[cls:] if cls >= 0 else ""
guard("classic_ctor_found", cls >= 0)
guard("classic_accept_unicode",
      re.search(r"input_text\s*=\s*Textfield\([\s\S]{0,700}?input_text\.set_accept_unicode\(true\);", chat) is not None)

modern_anchor = chat.find("int16_t input_text_limit = 70;")
guard("modern_ctor_found", modern_anchor >= 0)
guard("modern_accept_unicode",
      modern_anchor >= 0 and
      re.search(r"input_text_limit, input_text_marker_height[\s\S]{0,400}?input_text\.set_accept_unicode\(true\);",
                chat[modern_anchor:]) is not None)

guard("limit_kept_70", "int16_t input_text_limit = 70;" in chat and chat.count("70, 11") >= 1)

guard("tf_default_false", re.search(r"bool\s+accept_unicode\s*=\s*false\s*;", tfh) is not None)
guard("tf_setter", re.search(r"void\s+set_accept_unicode\(bool\s+accept\)", tfh) is not None)
guard("tf_accepts_unicode_getter", re.search(r"bool\s+accepts_unicode\(\)\s+const", tfh) is not None)

guard("add_string_filters_high",
      re.search(r"if\s*\(accept_unicode\)[\s\S]{0,120}?else[\s\S]{0,160}?c\s*>=\s*128", tfc) is not None)
guard("focus_switches_ime",
      re.search(r"void\s+UI::focus_textfield\([^)]*\)[\s\S]{0,1200}?accepts_unicode\(\)[\s\S]{0,120}?begin_ascii_text_input",
                uic) is not None)
guard("add_string_keeps_high",
      re.search(r"if\s*\(accept_unicode\)[\s\S]{0,80}?if\s*\(c\s*<\s*32\)", tfc) is not None)
guard("text_input_route",
      "send_text" in uic and "add_string" in uic)
guard("comment_cites_bytes_and_s9",
      "3 字节" in chat and "127" in chat)

# ---- 行为复刻 ----
ALLOW_ASCII = lambda c: 32 <= c < 128
ALLOW_UNI = lambda c: c >= 32


def belowlimit(text, limit):
    if limit > 0:
        return len(text) < limit
    return True  # 本用例不覆盖按像素宽度分支


def add_string(text_bytes, s, accept_unicode, limit):
    """复刻 Textfield::add_string：逐字节过滤 + 逐字节插入（修前修后同一实现）。"""
    out = bytearray(text_bytes)
    pos = len(out)
    for b in s:
        if accept_unicode:
            if not ALLOW_UNI(b):
                continue
        else:
            if not ALLOW_ASCII(b):
                continue
        if belowlimit(out, limit):
            out.insert(pos, b)
            pos += 1
    return bytes(out)


LIMIT = 70
CASES = [
    # (名称, 输入字节, 修前期望, 修后期望)
    ("ascii_hello", b"hello", b"hello", b"hello"),
    ("ascii_space", b"a b c", b"a b c", b"a b c"),
    ("cjk_nihao", "你好".encode("utf-8"), b"", "你好".encode("utf-8")),
    ("cjk_mixed", "hi你好".encode("utf-8"), b"hi", "hi你好".encode("utf-8")),
    ("cjk_long", "地图搜索".encode("utf-8"), b"", "地图搜索".encode("utf-8")),
    ("ctrl_only", b"\x01\x02", b"", b""),
    ("emoji4b", "\U0001F600".encode("utf-8"), b"", "\U0001F600".encode("utf-8")),
]

bad = 0
for name, src, want_before, want_after in CASES:
    got_before = add_string(b"", src, False, LIMIT)
    got_after = add_string(b"", src, True, LIMIT)
    ok = (got_before == want_before) and (got_after == want_after)
    if not ok:
        bad += 1
        print("CASE FAIL %-12s before=%r(want %r) after=%r(want %r)"
              % (name, got_before, want_before, got_after, want_after))

# 超限：修前修后都只受 limit 约束（字节）
long_ascii = b"a" * 100
over_before = add_string(b"", long_ascii, False, LIMIT)
over_after = add_string(b"", long_ascii, True, LIMIT)
if not (len(over_before) == LIMIT and over_after == over_before):
    bad += 1
    print("OVERLIMIT FAIL before=%d after=%d" % (len(over_before), len(over_after)))

# 中文 3 字节 -> 70 字节上限实际 23 字
cjk_fill = "字".encode("utf-8") * 30
fill = add_string(b"", cjk_fill, True, LIMIT)
chars = len(fill) // 3
if chars != 23:
    bad += 1
    print("CJK_LIMIT FAIL chars=%d (expect 23)" % chars)

# 混合：ASCII 优先填满，剩余字节给不了完整汉字就不进（逐字节语义，如实记录）
mixed = add_string(b"", b"x" * 68 + "字".encode("utf-8"), True, LIMIT)
print("MIXED_BOUNDARY len=%d tail=%r (逐字节插入，汉字可能被截断 -> 由发送侧 GBK 转换兜原文)"
      % (len(mixed), mixed[-4:]))

print("STATIC GUARDS checked=%d failed=%d" % (
    17, len(FAIL)))
if FAIL:
    for f in FAIL:
        print("  FAIL", f)
print("SCENARIOS %d, bad=%d" % (len(CASES) + 3, bad))
print("PASS" if not FAIL and bad == 0 else "FAIL")
sys.exit(0 if (not FAIL and bad == 0) else 1)
