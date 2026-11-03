"""R27-B1c 清点：全仓「非位图展示文案」硬编码站点，按是否可达 / 是否 Wave-A 壳窗分桶。

只读扫描，不改代码。判定依据全部来自代码本身：
  * Wave-A 壳窗 = 类名出现在 Net/Handlers/WaveAShellHandlers.cpp 里（该文件专管未实现壳窗）
  * 站点是否「已 i18n」= 引号紧挨 Locale::* 之后，或字面量本身就是 ini 键名
输出 UTF-8 文件（stdout 是 cp936，中文会炸）。
"""
import os
import re

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", ".."))
OUTDIR = os.path.join(ROOT, "docs", "features", "ui-v83-window-restore",
                      "evidence", "r27-b1c-i18n-scan")
OUT = os.path.join(OUTDIR, "inventory.txt")

# 展示文案的常见落点：Text 构造 / change_text / show_text / 软键盘提示位
PATTERNS = [
    ("change_text", re.compile(r'change_text\(\s*"((?:[^"\\]|\\.)*)"')),
    ("Text ctor", re.compile(r'Text\s*\(\s*Text::Font::\w+\s*,[^;]*?"((?:[^"\\]|\\.)*)"')),
    ("Tooltip", re.compile(r'show_text\(\s*Tooltip::Parent::\w+\s*,\s*"((?:[^"\\]|\\.)*)"')),
    # `emplace<UISoftKey>(ok, cancel, "<提示语>"[, pos])` 的第三实参是要显示在软键盘上的
    # 文案，前三种形状全看不见它 —— R27-B1c-5a 数出 16 处才知道这条尺缺了一档（用户
    # 2026-11-03 拍板：扩形状、重算全站基线）。按「上一个实参是闭包 ⇒ `}, "串"`」收，
    # 不认调用名：跨行调用（实参折到下一行）仍会漏，这与 Text ctor 档的已知盲区同一条。
    ("SoftKey hint", re.compile(r'\}\s*,\s*"((?:[^"\\]|\\.)*)"\s*[,)]')),
]

SCAN_DIRS = ["IO", "Character", "Data", "Gameplay", "Graphics", "Util", "Net"]
SKIP_FILE_PARTS = ("GraphicsGL.cpp",)  # dbgatlas 探针，非展示文案

LOCALED = re.compile(r'Locale::(text|text_or|tr|format|enum_text|enum_key)\s*\($')
KEYLIKE = re.compile(r'^[a-z][a-z0-9_]*\.[a-z0-9_.]*$')
EMPLACE = re.compile(r'(?:emplace|get_element)<([A-Za-z_][A-Za-z0-9_]*)>')


def cjk(s):
    return any(0x4E00 <= ord(c) <= 0x9FFF for c in s)


def already_localized(line, start, txt):
    return bool(LOCALED.search(line[:start])) or bool(KEYLIKE.match(txt))


def wave_a_shells():
    path = os.path.join(ROOT, "Net", "Handlers", "WaveAShellHandlers.cpp")
    out = set()
    if not os.path.isfile(path):
        return out
    for m in EMPLACE.finditer(open(path, encoding="utf-8", errors="replace").read()):
        out.add(m.group(1))
    return out


def main():
    shells = wave_a_shells()
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
                    if line.lstrip().startswith(("//", "*", "/*")):
                        continue
                    for shape, pat in PATTERNS:
                        for m in pat.finditer(line):
                            txt = m.group(1)
                            if not txt.strip() or already_localized(line, m.start(1), txt):
                                continue
                            rows.append((rel, i, txt, "CJK" if cjk(txt) else "LATIN", shape))

    latin = [r for r in rows if r[3] == "LATIN"]
    cjk_rows = [r for r in rows if r[3] == "CJK"]
    by_shape = {}
    for r in rows:
        by_shape[r[4]] = by_shape.get(r[4], 0) + 1

    def bucket(rel):
        cls = os.path.basename(rel)[:-4]
        return "壳窗" if (cls in shells or cls == "UIMiscShells") else "在用窗"

    groups = {}
    for rel, ln, txt, kind, shape in latin:
        groups.setdefault((bucket(rel), rel), []).append((ln, txt, shape))
    shell_n = sum(len(v) for k, v in groups.items() if k[0] == "壳窗")
    live_n = sum(len(v) for k, v in groups.items() if k[0] == "在用窗")

    os.makedirs(OUTDIR, exist_ok=True)
    with open(OUT, "w", encoding="utf-8", newline="\n") as f:
        f.write("R27-B1c 展示文案硬编码站点清点\n")
        f.write("模式：change_text(\"..\") / Text(Text::Font::.. , \"..\") / show_text(Tooltip::Parent::.., \"..\") / "
                "软键盘提示位（`}, \"..\"` 第三实参）\n")
        f.write("已 i18n 的行（引号在 Locale::* 之后，或字面量本身是 ini 键名）不计入。\n")
        f.write("Wave-A 壳窗（依据 Net/Handlers/WaveAShellHandlers.cpp 里 emplace 到的类名）：\n  %s\n\n"
                % " ".join(sorted(shells)))
        f.write("总计 %d 处 | 英文 %d（壳窗 %d / 在用窗 %d）| 中文 %d\n"
                % (len(rows), len(latin), shell_n, live_n, len(cjk_rows)))
        f.write("按形状分档（全站，中英合并）：%s\n"
                % " / ".join("%s=%d" % (k, by_shape[k]) for k, _ in PATTERNS if k in by_shape))

        for b in ("在用窗", "壳窗"):
            f.write("\n\n== 英文原文 · %s ==\n" % b)
            for (bk, rel) in sorted(k for k in groups if k[0] == b):
                f.write("\n[%s] %d 处\n" % (rel, len(groups[(bk, rel)])))
                for ln, txt, shape in groups[(bk, rel)]:
                    f.write("  L%-5d [%-12s] %s\n" % (ln, shape, txt[:90]))

        f.write("\n\n== 中文硬编码（直接搬 key，无「编造原生文案」风险）==\n")
        by_file2 = {}
        for rel, ln, txt, _kind, shape in cjk_rows:
            by_file2.setdefault(rel, []).append((ln, txt, shape))
        for rel in sorted(by_file2):
            f.write("\n[%s] %d 处\n" % (rel, len(by_file2[rel])))
            for ln, txt, shape in by_file2[rel]:
                f.write("  L%-5d [%-12s] %s\n" % (ln, shape, txt[:90]))

    print("WROTE", OUT)
    print("sites=%d latin=%d shell=%d live=%d cjk=%d shells=%d shapes=%s"
          % (len(rows), len(latin), shell_n, live_n, len(cjk_rows), len(shells),
             ",".join("%s=%d" % (k, by_shape[k]) for k, _ in PATTERNS if k in by_shape)))


if __name__ == "__main__":
    main()
