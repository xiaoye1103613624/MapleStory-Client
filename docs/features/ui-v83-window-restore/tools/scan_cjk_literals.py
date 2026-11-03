# -*- coding: utf-8 -*-
"""中文串字面量全量扫描（多行/拼接/三元都能抓到），用于核 B1c-4 真实站点数。

为什么还要第二个扫描器：`scan_display_text.py` 按行匹配 `change_text("..")` /
`Text(Text::Font::.. , "..")` / `show_text(..)` 三种形状，所以「跨行折的实参」「三元里的
另一支」「`s += " 频道" + n` 这种拼接片段」全都不在它的眼里 —— B1c-4 的 cjk 读数 18 就是
这么来的，而真实中文站点是 45。本扫描不问形状，只问「这行代码（注释除外）里有没有带汉字
的字符串字面量」，两个读数差就是盲区。

用法：python docs/features/ui-v83-window-restore/tools/scan_cjk_literals.py
产物直接落在 evidence/r27-b1c-i18n-scan/cjk-scan-full.txt（不在仓库根留临时件）。
"""
import io
import os
import re

CJK = re.compile(u'[\\u4e00-\\u9fff\\u3000-\\u303f\\uff00-\\uffef]')
STR = re.compile(r'"((?:[^"\\\n]|\\.)*)"')

# tools -> ui-v83-window-restore -> features -> docs -> 仓库根，所以要上跳四级
root = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", ".."))
out_dir = os.path.join(root, "docs", "features", "ui-v83-window-restore",
    "evidence", "r27-b1c-i18n-scan")
out = []
total = 0
for dirpath, dirnames, filenames in os.walk(os.path.join(root, "IO")):
    dirnames[:] = [d for d in dirnames if d not in (".git",)]
    for fn in filenames:
        if not fn.endswith((".cpp", ".h")):
            continue
        p = os.path.join(dirpath, fn)
        s = io.open(p, encoding="utf-8", errors="replace", newline="").read()
        # 只算代码里的字面量：注释行（// 或 /* ）先剔除
        lines = s.split("\n")
        hits = []
        in_block = False
        for i, line in enumerate(lines, 1):
            st = line.strip()
            if in_block:
                if "*/" in st:
                    in_block = False
                continue
            if st.startswith("/*"):
                if "*/" not in st:
                    in_block = True
                continue
            if st.startswith("//"):
                continue
            code = st.split("//")[0]
            for m in STR.finditer(code):
                if CJK.search(m.group(1)):
                    hits.append((i, m.group(1)))
        if hits:
            total += len(hits)
            rel = os.path.relpath(p, root).replace("\\", "/")
            out.append("[%s] %d 处" % (rel, len(hits)))
            for ln, txt in hits:
                out.append("  L%-5d %s" % (ln, txt))

io.open(os.path.join(out_dir, "cjk-scan-full.txt"), "w", encoding="utf-8", newline="\n").write(
    "中文串字面量全量扫描（含跨行/拼接/三元盲区）\n总计 %d 处\n\n" % total + "\n".join(out) + "\n")
print("total=%d -> evidence/r27-b1c-i18n-scan/cjk-scan-full.txt" % total)
