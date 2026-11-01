#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
T12b · 生成 UTF-8 解码对拍向量（Graphics/Utf8.h 的期望值）

oracle = Python 的 str/utf-8 codec：期望码点序列 = list(map(ord, s))，字节序列由
`str.encode('utf-8')` 现算，**不手抄**。

非法输入（落单续字节、截断序列、畸形引导字节）Python 会直接抛异常，没有 oracle 可对，
所以那几条按 `Utf8.h` 顶部写明的**规则**显式给出期望值（规则 = 返回 0 且 consumed=1，
即"什么都不画、也不占推进量"），标签统一带 `rule:` 前缀，与 `str:` 开头的 oracle 条目区分。

标签一律用十六进制而不是原文（`str:` 开头的标签是 UTF-8 字节的 hex），这样生成的表里
**只有注释**含中文；探针编译时和主构建一样带 /utf-8，不会被本机 cp936 shell 搅乱。

用法：
    python tools/probe/gen_utf8_vectors.py
"""

import io
import platform
import sys

OUT = "tools/probe/utf8_vectors.h"

# 合法样本：期望码点序列 = list(map(ord, s))
VALID = [
    "Hello",                      # 纯 ASCII：既有英文布局的回归基线
    "Admin 正在监控 1",            # 真机聊天行（混合）
    "已被监控",                    # 真机 yellow tip
    "对所有人",                    # 聊天频道标签
    "蘑菇王国",
    "新手/排行不可用",              # T12 当初"整串空白"的复现串
    "aé€",              # 1/2/3 字节混排
    "【测试】·—“”",    # 中文标点（全角）
    "한글",                        # 谚文：SimHei 未必有字形，用来验"缺字宽度=0"
    "\U0001f383",                  # 4 字节 emoji：解码合法、字体必缺
    "\x80\u07ff\u0800\uffff",      # UTF-8 各长度段的下边界码点
    "\U00010000\U0010ffff",        # 4 字节段两端
    "Line with  spaces  inside",
    "你好你好你好你好你好你好你好你好你好你好",  # 长中文串：换行路径的输入
]

# 规则样本：(标签, 字节, 期望码点序列, 期望 consumed 序列)
#
# `rule:` = Utf8.h 顶部写明的规则（返回 0 且 consumed=1，什么都不画也不占宽）。
# `lax:`  = 故意**保留**改动前绘制路径的宽松行为：不校验后续字节是不是 0x80..0xBF。
#           旧的内联解码就是 `cp = (cp << 6) | (text[pos+k] & 0x3F)` 直接拼，
#           换成严格校验会改变已有渲染行为（超出"让量出来的和画出来的相等"这一步），
#           所以这里把它作为**已知并记录在案**的语义钉住，而不是当 bug 修。
RULE = [
    ("lone-continuation", b"\x80", [0], [1]),
    ("truncated-after-ascii", b"A\x80", [ord("A"), 0], [1, 1]),
    ("truncated-at-eof", b"\xe2", [0], [1]),
    ("truncated-mid", b"\xe2\x98", [0, 0], [1, 1]),
    ("truncated-4byte", b"\xf0\x9f\x8e", [0, 0, 0], [1, 1, 1]),
    ("control-nul", b"\x00A", [0, ord("A")], [1, 1]),
    ("control-tab", b"\tA", [0, ord("A")], [1, 1]),
    ("del-7f", b"\x7f", [0x7F], [1]),
    ("space-then-truncated", b" \xe2\x98", [ord(" "), 0, 0], [1, 1, 1]),
    # 0xE2 声称是 3 字节序列，尾字节却是 'A'(0x41)：宽松拼接 → U+2601
    ("lax:non-continuation-tail", b"\xe2\x98\x41", [0x2601], [3]),
    # 0xF8 不是合法引导字节，但 >= 0xF0 一律按 4 字节处理；余下 0x80 落单 → 0
    ("lax:lead-f8-as-4byte", b"\xf8\x88\x80\x80\x80", [0x8000, 0], [4, 1]),
    # 过长编码 C0 80：解出码点 0，按"什么都不画"处理，但吃掉 2 个字节
    ("lax:overlong-nul", b"\xc0\x80", [0], [2]),
]


def emit(name, values, ctype):
    fmt = "0x%02X" if ctype == "uint8_t" else "%u"
    body = ", ".join(fmt % v for v in values)
    return "\tstatic const %s %s[] = { %s };\n" % (ctype, name, body)


def main():
    entries = []

    for s in VALID:
        entries.append(("str:" + s.encode("utf-8").hex(), s.encode("utf-8"),
                        [ord(c) for c in s],
                        [len(c.encode("utf-8")) for c in s]))

    for label, raw, cps, consumed in RULE:
        assert len(raw) == sum(consumed), label
        assert len(cps) == len(consumed), label
        entries.append(("rule:" + label, raw, cps, consumed))

    out = io.StringIO()
    out.write("// 自动生成，勿手改：python tools/probe/gen_utf8_vectors.py\n")
    out.write("// 被测对象：Graphics/Utf8.h 的 utf8_next()\n")
    out.write("// Python %s / 样本 %d 条（oracle %d + rule %d）\n"
              % (platform.python_version(), len(entries), len(VALID), len(RULE)))
    out.write("#pragma once\n\n")
    out.write("#include <cstddef>\n#include <cstdint>\n\n")
    out.write("struct Utf8Vector\n{\n")
    out.write("\tconst char* label;\n")
    out.write("\tconst uint8_t* bytes;\n")
    out.write("\tsize_t nbytes;\n")
    out.write("\tconst uint32_t* cps;\n")
    out.write("\tconst uint32_t* consumed;\n")
    out.write("\tsize_t nchars;\n")
    out.write("};\n\n")

    tables = []
    rows = []

    for idx, (label, raw, cps, consumed) in enumerate(entries):
        b, c, s = "v%d_bytes" % idx, "v%d_cps" % idx, "v%d_consumed" % idx
        tables.append(emit(b, list(raw), "uint8_t"))
        tables.append(emit(c, cps, "uint32_t"))
        tables.append(emit(s, consumed, "uint32_t"))
        rows.append('\t{ "%s", %s, %d, %s, %s, %d },'
                    % (label, b, len(raw), c, s, len(cps)))

    out.write("".join(tables))
    out.write("\nstatic const Utf8Vector UTF8_VECTORS[] =\n{\n")
    out.write("\n".join(rows) + "\n")
    out.write("\n};\n\n")
    out.write("static const size_t UTF8_VECTOR_COUNT = %d;\n" % len(entries))

    io.open(OUT, "w", encoding="utf-8", newline="\n").write(out.getvalue())

    # stdout 保持纯 ASCII（本机 shell 是 cp936）
    print("wrote %s: vectors=%d oracle=%d rule=%d bytes=%d"
          % (OUT, len(entries), len(VALID), len(RULE), sum(len(e[1]) for e in entries)))


if __name__ == "__main__":
    sys.exit(main())
