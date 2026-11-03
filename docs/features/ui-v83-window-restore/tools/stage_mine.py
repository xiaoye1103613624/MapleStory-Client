"""blob 级精确 staging：只把「我的 hunk」写进 index，工作树里并行会话的在途改动一律不动。

为什么不用 `git add <file>`：
  共享工作树里 IO/UIElement.h / IO/UIStateGame.cpp / locale/*.ini 同时带着另一个会话的未提交
  改动（GAMEOPT/SYSOPT 枚举、ESC 开目录、sysopt 下拉文案）。`git add` 会把别人的半成品塞进
  我的原子提交；`git stash` 在共享工作树里禁用（会连对方的改动一起卷走）。
  所以走 hunk：`git apply --cached` 逐块把我的 hunk 打进 index，工作树一个字节都不改。

为什么按 hunk 而不是按行：
  一段中文注释只有第二行才带 marker 时，逐行判会把同一件事拆成双方。hunk 是 git 自己的分隔，
  隔得够远的改动天然分开；真混进同一个 hunk 就 rc=3 退出交人拆，不猜。

用法：
  python stage_mine.py <file> --mine M [M ...] --theirs T [T ...]   # 按 hunk
  python stage_mine.py --append <file> --from "<我的块起始行>"        # 纯追加（ini：双方挤在同一 hunk）
  STAGE_DRY=1 前缀 = 只报告不写 index。
"""
import os
import re
import subprocess
import sys


def sh(*args, **kw):
    return subprocess.run(list(args), capture_output=True, **kw)


def split_diff(text):
    """返回 (文件头, [(hunk 头, hunk 原文)])。"""
    lines = text.splitlines(keepends=True)
    starts = [i for i, ln in enumerate(lines) if ln.startswith("@@")]
    if not starts:
        return "".join(lines), []
    header = "".join(lines[:starts[0]])
    hunks = []
    for n, s in enumerate(starts):
        e = starts[n + 1] if n + 1 < len(starts) else len(lines)
        hunks.append((lines[s].rstrip("\n"), "".join(lines[s:e])))
    return header, hunks


def added_of(hunk_text):
    return [ln for ln in hunk_text.splitlines() if ln.startswith("+")]


def classify(hunk_text, mine, theirs):
    body = added_of(hunk_text)
    m = any(any(k in ln for k in mine) for ln in body)
    t = any(any(k in ln for k in theirs) for ln in body)
    if m and t:
        return None
    return m


def cmd_hunk(path, mine, theirs):
    wr = sh("git", "diff", "--", path)
    text = wr.stdout.decode("utf-8", "replace")
    if not text.strip():
        print("%s：工作树与 index 无差异（可能已 staged）" % path)
        return 0
    header, hunks = split_diff(text)
    marks = [classify(h, mine, theirs) for _, h in hunks]
    if None in marks:
        print("!! %s：有 hunk 里双方改动混在一起，交人拆（不猜）" % path)
        for (hd, tx), mk in zip(hunks, marks):
            if mk is not None:
                continue
            print("   %s" % hd)
            for ln in added_of(tx):
                who = "MINE  " if any(k in ln for k in mine) else "THEIRS"
                print("     %s %s" % (who, ln[1:].rstrip()[:88]))
        return 3

    keep = [(hd, tx) for (hd, tx), mk in zip(hunks, marks) if mk]
    print("%s：hunk 共 %d，我的 %d，对方 %d"
          % (path, len(hunks), len(keep), len(hunks) - len(keep)))
    for (hd, _), mk in zip(hunks, marks):
        print("  %-6s %s" % ("MINE" if mk else "theirs", hd))
    if not keep:
        return 0
    if os.environ.get("STAGE_DRY") == "1":
        for hd, tx in keep:
            print("--- DRY，将 apply --cached ---\n%s" % hd)
        return 0
    for hd, tx in keep:
        patch = (header + tx).encode("utf-8")
        ar = sh("git", "apply", "--cached", "-", input=patch)
        if ar.returncode:
            print("!! apply --cached 失败：%s\n%s" % (hd, ar.stderr.decode("utf-8", "replace")))
            return 1
        print("   applied %s" % hd)
    print("=> %s 已按 hunk 写入 index（工作树未改动）" % path)
    return 0


def cmd_append(path, start_marker):
    """ini 这类「双方改动落在同一个 hunk 尾部」的纯追加文件：
    直接构造 index blob = HEAD 全文 + 我的那块，再 update-index。"""
    head = sh("git", "show", "HEAD:" + path)
    if head.returncode:
        print(head.stderr.decode("utf-8", "replace"))
        return 1
    work = open(path, "rb").read().decode("utf-8")
    wl = work.splitlines(keepends=True)
    at = [i for i, ln in enumerate(wl) if start_marker in ln]
    if len(at) != 1:
        print("!! %s：起始行 %r 命中 %d 次，需唯一" % (path, start_marker, len(at)))
        return 3
    i = at[0]
    # 我的块前面若有空行，从空行起取，保证追加块与 HEAD 末行之间空一行。
    while i > 0 and wl[i - 1].strip() == "":
        i -= 1
    block = "".join(wl[i:])
    base = head.stdout.decode("utf-8")
    blob = (base.rstrip("\r\n") + "\r\n\r\n" + block.rstrip("\r\n") + "\r\n").encode("utf-8") \
        if "\r\n" in base else (base.rstrip("\n") + "\n\n" + block.rstrip("\n") + "\n").encode("utf-8")
    print("%s：追加自 %r，共 %d 行" % (path, start_marker, block.count("\n")))
    if os.environ.get("STAGE_DRY") == "1":
        print("=> DRY：未写 index\n--- 追加块 ---\n%s" % block)
        return 0
    wr = sh("git", "hash-object", "-w", "--stdin", input=blob)
    if wr.returncode:
        print(wr.stderr.decode("utf-8", "replace"))
        return 1
    sha = wr.stdout.decode().strip()
    ls = sh("git", "ls-files", "-s", "--", path).stdout.decode().split()
    mode = ls[0] if ls else "100644"
    ur = sh("git", "update-index", "--cacheinfo", "%s,%s,%s" % (mode, sha, path))
    if ur.returncode:
        print(ur.stderr.decode("utf-8", "replace"))
        return 1
    print("=> %s  index=%s（工作树未改动）" % (path, sha[:8]))
    return 0


def main():
    argv = sys.argv[1:]
    if not argv:
        print(__doc__)
        return 2
    if argv[0] == "--append":
        return cmd_append(argv[1], argv[argv.index("--from") + 1])
    return cmd_hunk(argv[0], argv[argv.index("--mine") + 1:argv.index("--theirs")],
                    argv[argv.index("--theirs") + 1:])


if __name__ == "__main__":
    sys.exit(main())
