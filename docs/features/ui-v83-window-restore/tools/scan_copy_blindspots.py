"""R27-B1c 第五把尺：找「四条形状都看不见」的展示文案字面量（盲区猎手）。

为什么需要它：scan_display_text.py 按行认四种**具名形状**（change_text("..") /
Text(Text::Font::.., "..") / show_text(Tooltip..) / 软键盘 `}, ".."`），凡文案以
①三元另一支（`packages.empty() ? "No packages." : …`）②拼接片段（`"Rep " + n`）
③折行实参（`change_text(\n  "Pet " + …)`）④赋给中间变量再喂给 label
（`std::string txt = "Guilds (" + …; label.change_text(txt)`）的形式出现就整条漏掉。
B1c-4/5a 已经各踩过一次（当时靠人工补），本尺把「人工补」变成「有账可查」。

判据（宁缺勿滥，噪音交人读）：
  * 字面量里必须**含空格**且含字母 —— 原生资源节点名（`BtOK`/`backgrnd`/`icon1`）不含空格，
    路径类（`Basic.img/Notice4`）含 `/`，都被排除
  * 行里必须有展示汇点（change_text / show_notice / UIYesNo / Text( / 赋给 string）
  * 剔除注释行、LOG( 行、已走 Locale:: 的行、ini 键名样式的字面量
  * 已排除**软键盘提示位**（第三实参），那是 §11 台账第 6 条挂起项，不混进本账
"""
import os
import re

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", ".."))
OUTDIR = os.path.join(ROOT, "docs", "features", "ui-v83-window-restore",
                      "evidence", "r27-b1c-i18n-scan")
OUT = os.path.join(OUTDIR, "blindspots.txt")

SCAN_DIRS = ["IO", "Character", "Data", "Gameplay", "Graphics", "Util", "Net"]
SKIP_FILE_PARTS = ("GraphicsGL.cpp",)

LIT = re.compile(r'"((?:[^"\\]|\\.)+)"')
# 展示汇点：写进 Text/label 的语句，或把文案赋给中间变量
SINK = re.compile(r'(change_text|show_notice|emplace<UIYesNo>|\bset_message\b|= *Text\(|= *OutlinedText\(|std::string \w+ *=|\+= *"|\? *")')
DROP_LINE = re.compile(r'(LOG\(|Locale::|\bnl::|\.img|\.nx|\[\s*")')
# 节点名/路径常带 '/'、'\\'、'.img'；文案几乎不会
BAD_TEXT = re.compile(r'(/[A-Za-z0-9_]|\\.img|\\n)')
KEYLIKE = re.compile(r'^[a-z][a-z0-9_]*\.[a-z0-9_.]*$')
SOFTKEY_POS = re.compile(r'\}\s*,\s*"')


def main():
    rows = []
    for d in SCAN_DIRS:
        base = os.path.join(ROOT, d)
        for dirpath, _dirs, files in os.walk(base):
            for fn in sorted(files):
                if not fn.endswith(".cpp") or any(p in fn for p in SKIP_FILE_PARTS):
                    continue
                path = os.path.join(dirpath, fn)
                rel = os.path.relpath(path, ROOT).replace("\\", "/")
                lines = open(path, encoding="utf-8", errors="replace").read().splitlines()
                for i, line in enumerate(lines, 1):
                    s = line.strip()
                    if s.startswith(("//", "*", "/*")):
                        continue
                    if DROP_LINE.search(line) or not SINK.search(line):
                        continue
                    for m in LIT.finditer(line):
                        t = m.group(1)
                        if " " not in t or not re.search(r"[A-Za-z]", t):
                            continue
                        if KEYLIKE.match(t) or BAD_TEXT.search(t) or len(t) < 4:
                            continue
                        # 软键盘提示位（`}, "…"`) 由第四形状记账，不重复算
                        pre = line[:m.start(1)]
                        if SOFTKEY_POS.search(pre + '"'):
                            continue
                        rows.append((rel, i, t, s))

    by_file = {}
    for rel, ln, txt, src in rows:
        by_file.setdefault(rel, []).append((ln, txt, src))

    os.makedirs(OUTDIR, exist_ok=True)
    with open(OUT, "w", encoding="utf-8", newline="\n") as f:
        f.write("第五把尺：四条形状看不见的展示文案（盲区候选，须人读确认）\n")
        f.write("判据：字面量含空格+字母、行内有展示汇点、剔除 LOG/资源路径/已 Locale/软键盘提示位\n")
        f.write("总计 %d 处候选 / %d 文件\n" % (len(rows), len(by_file)))
        for rel in sorted(by_file, key=lambda r: (-len(by_file[r]), r)):
            f.write("\n[%s] %d 处\n" % (rel, len(by_file[rel])))
            for ln, txt, src in by_file[rel]:
                f.write("  L%-5d %r\n        %s\n" % (ln, txt[:70], src[:150]))
    print("WROTE", OUT)
    print("candidates=%d files=%d" % (len(rows), len(by_file)))


if __name__ == "__main__":
    main()
