"""门禁：本批新建的 ini 值必须**逐字来自改动前的源码字面量**（值保真）。

为什么需要：决策 B 要求「建键不翻译」，值 = 站点原来那条英文字面量。但改形时把挂在变量后面的
`+ "m"`、`+ "):"` 吃掉、或把两个插值塌成一个，**编译、键门禁、arity 尺全都不会响** —— 界面照跑，
文案静静掉字。R27-B1c-5b 实测抓到两条这样的自伤（`delivery.row_meso`、`delivery.row_item`），
所以这条尺的判据是：值按 `{}` 切出的每一段非占位符片段，必须能在**该键引用处的 HEAD 旧文件**里逐字找到。

用法（只读，git show 取旧版，不改工作树）：
  python tools/check_value_fidelity.py --table evidence/r27-b1c-i18n-scan/keys-5b.txt
  --out 写 UTF-8 报告（本机 stdout 是 cp936）。rc：0 全过 / 1 有不合格 / 2 空跑或判据缺失。

**这是「提交前」的尺**：判据取自 `HEAD`，本批一旦 commit，`HEAD` 就变成改后的源码，
片段自然查无 ⇒ 假阳性（实测：commit 后同一条命令 98 键里 75 条报查无）。要在原子提交**之前**跑并归档读数；
提交之后要复查同一批，加 `--rev <该批的父提交>`。
**父提交要自己数，别默认 `HEAD~1`**：批后若还追了工具/文档提交，`HEAD~1` 就落在批自身或批后，
读数会变成一片假「查无」（R27-B1c-5b 实测踩过：批 = `HEAD~1` 时 `--rev HEAD~1` 报 75 查无，
`--rev HEAD~2` 才 98/98）。这条现在由 `--rev` 自检兜住 —— 见下。

用法示例：
  python tools/check_value_fidelity.py --table <键表> --ini locale/zh-CN.ini [--rev <rev>] [--out F]

已知边界：片段横跨引号拼接（源码写 `"Fee = TradeFee"` `+ tip` `") + 5000"`）时按整段查会失败，
本工具**照实报警交人工**，不做模糊匹配 —— 放宽判据等于取消门禁。
"""
import argparse
import io
import os
import re
import subprocess
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", ".."))
SCAN_DIRS = ["IO", "Character", "Data", "Gameplay", "Graphics", "Net", "Util"]
KEYLIT = re.compile(r'"([a-z0-9_.]+)"')


def read_table(path):
    rows = []
    for ln in io.open(path, encoding="utf-8"):
        ln = ln.rstrip("\n")
        if not ln or ln.startswith("#") or "=" not in ln:
            continue
        k, _, v = ln.partition("=")
        rows.append((k, v))
    return rows


def locate(keys):
    """键字面量在源码里只出现在它的调用点上 ⇒ 按「哪条文件的哪一行引用了该键」定位。"""
    hits = {}
    for d in SCAN_DIRS:
        base = os.path.join(ROOT, d)
        if not os.path.isdir(base):
            continue
        for dirpath, _s, files in os.walk(base):
            for fn in sorted(files):
                if not fn.endswith(".cpp"):
                    continue
                p = os.path.join(dirpath, fn)
                rel = os.path.relpath(p, ROOT).replace("\\", "/")
                text = io.open(p, encoding="utf-8", errors="replace").read()
                for m in KEYLIT.finditer(text):
                    if m.group(1) in keys:
                        hits.setdefault(m.group(1), set()).add(rel)
    return hits


def head(rel, rev):
    wr = subprocess.run(["git", "-C", ROOT, "show", "%s:%s" % (rev, rel)], capture_output=True)
    if wr.returncode != 0:
        return None
    return wr.stdout.decode("utf-8", "replace")


def read_ini(path):
    vals = {}
    for ln in io.open(path, encoding="utf-8"):
        ln = ln.rstrip("\n")
        if not ln or ln.startswith("#") or "=" not in ln:
            continue
        k, _, v = ln.partition("=")
        vals[k] = v
    return vals


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--table", required=True, help="本批键表：key=英文原值（# 注释行忽略）")
    ap.add_argument("--ini", default=None,
                    help="真源 ini（如 locale/zh-CN.ini）：给了就按 **ini 里的值** 查保真，"
                         "键表只当「本批键清单」用 —— 否则表与 ini 漂了也查不出来")
    ap.add_argument("--rev", default="HEAD",
                    help="原文取自哪个版本。默认 HEAD ⇒ 本尺是**提交前**的闸；提交后要复查这批，"
                         "传该批的父提交（先 git log 数出本批那条，别默认 HEAD~1 —— "
                         "批后追过提交就会指错，工具会自判 WRONG-REV 并 rc=2）")
    ap.add_argument("--out", default=None)
    ns = ap.parse_args()

    table = os.path.abspath(ns.table)
    if not os.path.isfile(table):
        sys.stderr.write("判据表不存在：%s\n" % table)
        return 2
    rows = read_table(table)
    if not rows:
        sys.stderr.write("判据表为空 ⇒ 拒绝出「全过」报告：%s\n" % table)
        return 2

    ini = None
    if ns.ini:
        inip = os.path.join(ROOT, ns.ini) if not os.path.isabs(ns.ini) else ns.ini
        if not os.path.isfile(inip):
            sys.stderr.write("判据 ini 不存在：%s\n" % inip)
            return 2
        ini = read_ini(inip)

    hits = locate(set(k for k, _ in rows))
    cache = {}

    # --rev 自检：这批键是本批**新造**的字面量，所以「该键字面量已出现在 rev 里」= rev 含改动后源码。
    # 落在这上面时片段必然成片查无（实测 HEAD~1 指错 ⇒ 75/98 假报警），比查无更该先说清楚。
    stale = []
    for key, _ in rows:
        for rel in sorted(hits.get(key, ())):
            if rel not in cache:
                cache[rel] = head(rel, ns.rev) or ""
            if '"%s"' % key in cache[rel]:
                stale.append(key)
                break
    rev_post = len(stale) * 2 > len(rows)

    ok, frag_fail, unref, noini = 0, [], [], []
    for key, val in rows:
        audit = ini[key] if ini is not None else val
        if ini is not None and key not in ini:
            noini.append(key)
            continue
        home = sorted(hits.get(key, ()))
        if not home:
            unref.append((key, audit))
            continue
        frags = [f for f in audit.split("{}") if f.strip()]
        bad = []
        for fr in frags:
            found = False
            for rel in home:
                if rel not in cache:
                    cache[rel] = head(rel, ns.rev) or ""
                old = cache[rel]
                if fr in old or fr.encode("utf-8").decode("cp1252", "replace") in old:
                    found = True
                    break
            if not found:
                bad.append(fr)
        if bad:
            frag_fail.append((key, audit, bad, home))
        else:
            ok += 1

    lines = []
    for key, val, bad, home in frag_fail:
        lines.append("  %s  值=%r  片段在 %s 查无=%r  代码在=%s"
                     % (key, val, ns.rev, bad, ",".join(os.path.basename(h) for h in home)))
    for key, val in unref:
        lines.append("  %s = %r  —— 有值但源码无引用（本批漏接线或键名写错）" % (key, val))
    for key in noini:
        lines.append("  %s —— 键表列了、ini 里没有" % key)
    head_line = "值保真（原文取 %s）：%s %d 键 | 片段全部命中 = %d | 片段查无 = %d | 源码无引用 = %d | ini 缺键 = %d" % (
        ns.rev, "ini 真源" if ini is not None else "键表", len(rows), ok, len(frag_fail), len(unref), len(noini))
    report = head_line + "\n"
    if rev_post:
        report += ("WRONG-REV：%d/%d 个键的字面量已存在于 %s ⇒ %s 含**改动后**源码，"
                   "上面「片段查无」不成立。判据要用该批的**父提交**：先 `git log --oneline` 数出"
                   "本批那条，再传 --rev <它前面一条>（批后若追了提交，HEAD~1 不是父提交）\n"
                   % (len(stale), len(rows), ns.rev, ns.rev))
    report += "\n".join(lines) + "\n"
    if ok == 0 and not rev_post:
        report += "FALSE-GREEN：0 条值被校验，工具失效，勿据此放行\n"

    if ns.out:
        io.open(ns.out, "w", encoding="utf-8", newline="\n").write(report)
        print("WROTE " + ns.out)
    else:
        io.open(sys.stdout.fileno(), "w", encoding="utf-8", closefd=False,
                errors="replace").write(report)
    if rev_post:
        return 2
    return 1 if (frag_fail or unref or noini) else 0


if __name__ == "__main__":
    sys.exit(main())
