"""门禁：`Locale::format` / `format_all` 调用点的**实参个数**必须等于 ini 值里 `{}` 的个数。

为什么要这条尺：`Util/Locale.h` 的 `format` 只填**第一个** `{}`、`format_all` 按序填，
占位符多于值 ⇒ 界面上留下裸 `{}`；少于值 ⇒ 值直接拼在句尾。两种都**不报错、不崩**，
只有肉眼看得出来。R27-B1c-5b 立这条尺时先做负对照（人为改坏 3 条值）确认它有牙，再跑全站。

用法（只读，不改代码）：
  python tools/check_placeholder_arity.py            # 用 locale/zh-CN.ini 做判据
  python tools/check_placeholder_arity.py --both     # zh 与 en 两份都查（值不同也能各查各的）

已知边界：键名写成变量（`Locale::format(k, …)`，k 是 `const char*`）的调用点认不出，
按「键不在 ini」报出，交人工判定；三元素键（`Locale::text(a ? "k1" : "k2")`）取第一个键。
"""
import argparse
import io
import os
import re
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", ".."))
SCAN_DIRS = ["IO", "Character", "Data", "Gameplay", "Graphics", "Net", "Util"]

CALL = re.compile(r'Locale::(format_all|format)\s*\(')
KEYRE = re.compile(r'"([a-z0-9_.]+)"')
DYNRE = re.compile(r'Locale::enum_key\s*\(\s*"([a-z0-9_.]+)"')


def read_ini(path):
    vals = {}
    if not os.path.isfile(path):
        return vals
    for ln in io.open(path, encoding="utf-8"):
        ln = ln.rstrip("\n")
        if not ln or ln.startswith("#") or "=" not in ln:
            continue
        k, _, v = ln.partition("=")
        vals[k] = v
    return vals


def scan_string(s, i):
    """s[i] 处是引号，返回引号结束后的下标（跳过转义）。"""
    i += 1
    while i < len(s):
        if s[i] == "\\":
            i += 2
            continue
        if s[i] == '"':
            return i + 1
        i += 1
    return i


def match_paren(s, open_idx):
    """s[open_idx]=='('，返回配对 ')' 的下标；找不到 -1。"""
    depth = 0
    i = open_idx
    while i < len(s):
        c = s[i]
        if c == '"':
            i = scan_string(s, i)
            continue
        if c == "(":
            depth += 1
        elif c == ")":
            depth -= 1
            if depth == 0:
                return i
        i += 1
    return -1


def match_brace(s, open_idx):
    depth = 0
    i = open_idx
    while i < len(s):
        c = s[i]
        if c == '"':
            i = scan_string(s, i)
            continue
        if c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return i
        i += 1
    return -1


def top_commas(s):
    """深度 0 的逗号下标列表（字符串字面量与任意括号内的逗号都不算）。"""
    out = []
    depth = 0
    i = 0
    while i < len(s):
        c = s[i]
        if c == '"':
            i = scan_string(s, i)
            continue
        if c in "([{":
            depth += 1
        elif c in ")]}":
            depth -= 1
        elif c == "," and depth == 0:
            out.append(i)
        i += 1
    return out


def call_sites(text):
    """产出 (kind, key, n_values, line, dyn_prefix)。n_values 解析不出来时给 None。"""
    for m in CALL.finditer(text):
        kind = m.group(1)
        op = text.index("(", m.start())
        cl = match_paren(text, op)
        if cl < 0:
            continue
        args = text[op + 1:cl]
        commas = top_commas(args)
        if not commas:
            continue
        # 键段 = 第一个顶层逗号之前。三元素键（`jackpot ? "k1" : "k2"`）在这里有两个引号串，
        # 两个都要查 —— 只查第一个的话，另一支的占位符数写错了看不见。
        head = args[:commas[0]]
        dyn = DYNRE.search(head)
        if dyn:
            # `enum_key("equip.stat.", names[es])`：键名运行时拼，逐个查不了，
            # 改查「该前缀下所有 ini 值的占位符数」是否符合本函数的契约。
            keys, prefix = [], dyn.group(1)
        else:
            keys, prefix = KEYRE.findall(head), None
        line = text[:m.start()].count("\n") + 1
        if kind == "format":
            n = 1
        else:
            tail = args[commas[0] + 1:]
            bi = tail.find("{")
            if bi >= 0 and match_brace(tail, bi) > 0:
                inner = tail[bi + 1:match_brace(tail, bi)]
                n = len(top_commas(inner)) + (1 if inner.strip() else 0)
            else:
                n = len(commas)      # 无花括号：顶层逗号数即值数
        if prefix is not None:
            yield kind, None, n, line, prefix
            continue
        if not keys:
            keys = [None]
        for key in keys:
            yield kind, key, n, line, None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--both", action="store_true", help="zh-CN 与 en-US 两份都查")
    ap.add_argument("--out", default=None, help="把报告写成 UTF-8 文件（stdout 是 cp936，中文会炸）")
    ns = ap.parse_args()

    inis = [os.path.join(ROOT, "locale", "zh-CN.ini")]
    if ns.both:
        inis.append(os.path.join(ROOT, "locale", "en-US.ini"))

    # 反「假绿」闸：ROOT 算错时什么都扫不到，checked=0/bad=0 看起来像全过。
    missing = [p for p in inis if not os.path.isfile(p)]
    if missing or not any(os.path.isdir(os.path.join(ROOT, d)) for d in SCAN_DIRS):
        sys.stderr.write("ROOT=%s 下找不到判据/源码目录，拒绝出报告\n" % ROOT)
        return 2

    lines = []
    bad = 0
    checked = 0
    for ini in inis:
        vals = read_ini(ini)
        rel_ini = os.path.relpath(ini, ROOT).replace("\\", "/")
        for d in SCAN_DIRS:
            for dirpath, _dirs, files in os.walk(os.path.join(ROOT, d)):
                for fn in sorted(files):
                    if not fn.endswith(".cpp"):
                        continue
                    path = os.path.join(dirpath, fn)
                    text = io.open(path, encoding="utf-8", errors="replace").read()
                    rel = os.path.relpath(path, ROOT).replace("\\", "/")
                    for kind, key, n, line, prefix in call_sites(text):
                        if prefix is not None:
                            group = sorted(k for k in vals if k.startswith(prefix))
                            need = 1 if kind == "format" else n
                            if not group:
                                lines.append("%s:%d %s(%s*) 前缀无键" % (rel, line, kind, prefix))
                                bad += 1
                            for k in group:
                                got = vals[k].count("{}")
                                checked += 1
                                if got != need:
                                    lines.append("%s:%d %s(%s) 实参 %d 个 / 占位符 %d 个｜值=%r"
                                                 % (rel, line, kind, k, need, got, vals[k]))
                                    bad += 1
                            continue
                        if key is None:
                            lines.append("%s:%d %s 键是变量，人工判定" % (rel, line, kind))
                            bad += 1
                            continue
                        if key not in vals:
                            lines.append("%s:%d %s(%s) 键不在 %s" % (rel, line, kind, key, rel_ini))
                            bad += 1
                            continue
                        got = vals[key].count("{}")
                        checked += 1
                        if kind == "format" and got != 1:
                            lines.append("%s:%d format(%s) 占位符 %d 个，应恰好 1 个｜值=%r"
                                         % (rel, line, key, got, vals[key]))
                            bad += 1
                        elif kind == "format_all" and n is not None and got != n:
                            lines.append("%s:%d format_all(%s) 实参 %d 个 / 占位符 %d 个｜值=%r"
                                         % (rel, line, key, n, got, vals[key]))
                            bad += 1

    head = "arity 校验调用点 = %d | 不合格 = %d | 判据 = %s" % (
        checked, bad, " + ".join(os.path.relpath(p, ROOT).replace("\\", "/") for p in inis))
    report = head + "\n" + "\n".join(lines) + "\n"
    if checked == 0:
        # 判据在、源码在，却一条调用点都没抓到 = 尺子自己坏了，不是「全过」。
        report += "FALSE-GREEN：0 个调用点被校验，工具失效，勿据此放行\n"

    if ns.out:
        io.open(ns.out, "w", encoding="utf-8", newline="\n").write(report)
        print("WROTE " + ns.out)
    else:
        io.open(sys.stdout.fileno(), "w", encoding="utf-8", closefd=False,
                errors="replace").write(report)
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
