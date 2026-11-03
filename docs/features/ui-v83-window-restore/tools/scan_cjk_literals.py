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

# 逐条给「为什么不接 i18n」的档号，读数才能自己解释自己：
# 只报「还剩 N 处中文」等于把该接的和不该接的混在一个数里，下一批无从判断是不是漏了。
# 判据是「这处不是界面展示文案」；键 = 相对路径，值 = [(字面量, 档号, 理由)]，理由过期就删条目。
EXEMPT = {
    "IO/UITypes/UIGender.cpp": [
        (u"女", "B1c-5c", u'软键盘输入的**比对串**（`t == "女"`），不是展示文案；接了 i18n 会在切语言后让比对失效'),
        (u"男", "B1c-5c", u'同上'),
    ],
    "IO/UITypes/UIMiscShells.cpp": [
        (u"Basic.img/Notice4(t,c,s) 或 ComboBox 三件缺失 —— 弹层只剩文字与按钮", "B1c-6",
         u'诊断日志串，不上界面'),
    ],
    "IO/Window.cpp": [
        (u"破功值同步: ", "待裁决", u'开发者浮层（破功值/在线时长）—— 是否算展示文案等用户裁决，且该文件当前被并行会话占着'),
    ],
}


def exempt_tag(rel, lit):
    for want, tag, _why in EXEMPT.get(rel, ()):
        if want == lit:
            return tag
    return None


# tools -> ui-v83-window-restore -> features -> docs -> 仓库根，所以要上跳四级
root = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", ".."))
out_dir = os.path.join(root, "docs", "features", "ui-v83-window-restore",
    "evidence", "r27-b1c-i18n-scan")
out = []
by_tag = {}
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
        rel = os.path.relpath(p, root).replace("\\", "/")
        tagged = [(ln, txt, exempt_tag(rel, txt) or ("待裁决" if rel in EXEMPT else "待接线"))
                  for ln, txt in hits]
        if tagged:
            total += len(tagged)
            n_free = len([1 for _l, _t, g in tagged if g == "待接线"])
            out.append("[%s] %d 处（其中待接线 %d）" % (rel, len(tagged), n_free))
            for ln, txt, tag in tagged:
                out.append("  L%-5d [%-8s] %s" % (ln, tag, txt))
                by_tag[tag] = by_tag.get(tag, 0) + 1

summary = "分档：" + " | ".join("%s=%d" % (k, by_tag[k]) for k in sorted(by_tag)) + "\n"
io.open(os.path.join(out_dir, "cjk-scan-full.txt"), "w", encoding="utf-8", newline="\n").write(
    "中文串字面量全量扫描（含跨行/拼接/三元盲区）\n总计 %d 处\n%s\n" % (total, summary) + "\n".join(out) + "\n")
print("total=%d -> evidence/r27-b1c-i18n-scan/cjk-scan-full.txt" % total)
