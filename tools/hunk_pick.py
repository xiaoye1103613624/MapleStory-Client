# -*- coding: utf-8 -*-
"""hunk_pick.py — 交织文件的 hunk 级暂存工具（Wave 9.1 提交拆分用）。

用法: python tools/hunk_pick.py <file> <keep|drop> <marker...>
  从 `git diff <file>` 里按 hunk 体是否含 marker 子串筛选，
  保留集写成 patch 并 `git apply --cached`。marker 匹配 hunk 的新增(+)或删除(-)
  行与上下文行；一个 hunk 命中任一 marker 即选中。只暂存不落地工作树。
判据透明：打印每个 hunk 的序号/头/命中结果，人工可复核后再确认应用。"""
import subprocess
import sys

def main():
    path, mode, markers = sys.argv[1], sys.argv[2], sys.argv[3:]
    diff = subprocess.run(["git", "diff", "--", path], capture_output=True)
    text = diff.stdout.decode("utf-8", "replace")
    lines = text.splitlines(keepends=True)
    header, hunks, cur = [], [], None
    for ln in lines:
        if ln.startswith("@@"):
            if cur is not None:
                hunks.append(cur)
            cur = {"head": ln, "body": []}
        elif cur is None:
            header.append(ln)
        else:
            cur["body"].append(ln)
    if cur is not None:
        hunks.append(cur)

    keep = []
    for i, h in enumerate(hunks):
        hit = any(m in "".join(h["body"]) for m in markers)
        take = hit if mode == "keep" else (not hit)
        print(f"hunk{i+1} {h['head'].splitlines()[0][:40]} markers={'Y' if hit else 'n'} -> {'PICK' if take else 'skip'}")
        if take:
            keep.append(h)

    if not keep:
        print("nothing picked"); return 1
    patch = "".join(header) + "".join(h["head"] + "".join(h["body"]) for h in keep)
    r = subprocess.run(["git", "apply", "--cached", "--recount", "-"],
                       input=patch.encode("utf-8"), capture_output=True)
    print(r.stderr.decode("utf-8", "replace") or "applied-to-index OK")
    return r.returncode

sys.exit(main())
