# -*- coding: utf-8 -*-
# filter-branch msg-filter: 若标题行后紧跟 "- " 详情行，补一行空行（git-commit-standard 结构体）。
import sys

text = sys.stdin.read()
lines = text.split("\n")
out = [lines[0]] if lines else []
if len(lines) > 1 and lines[1].startswith("- "):
    out.append("")
out.extend(lines[1:])
sys.stdout.write("\n".join(out))
