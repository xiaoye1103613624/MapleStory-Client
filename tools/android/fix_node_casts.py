#!/usr/bin/env python3
"""
Mechanical fixer for the NoLifeNx nl::node implicit-conversion ambiguity under
clang. When a node is assigned to an integral/char type, append .get_integer();
when assigned to std::string, wrap with static_cast<std::string>(...).

Reads E:/project/MapleStory-Client/build.log, finds the node-conversion errors,
patches the offending lines in place, and is safe to re-run (skips already-fixed
lines). This only touches the RHS of assignment statements, matching the way the
upstream code intended (node -> integer via get_integer()).
"""
import re
import os

ROOT = "E:/project/MapleStory-Client"
BUILD_LOG = os.path.join(ROOT, "build.log")

pat = re.compile(r'(?P<path>[A-Za-z]:[\\/][^:]+?):(?P<line>\d+):(?P<col>\d+): error:.*?node', re.DOTALL)

# Collect unique (path, line) from the build log.
seen = {}
with open(BUILD_LOG, "r", encoding="utf-8", errors="ignore") as f:
    for m in pat.finditer(f.read()):
        path = os.path.normpath(m.group("path"))
        line = int(m.group("line"))
        seen[(path, line)] = True

assign_re = re.compile(r'^(\s*(?:[^\n]*?)\s*=\s*)(.+);(\s*)$')
string_re = re.compile(r'std::string')

fixed = 0
for (path, line) in seen:
    full = os.path.join(ROOT, path) if not os.path.isabs(path) else path
    full = os.path.normpath(full)
    if not os.path.exists(full):
        print("SKIP (missing):", full)
        continue
    with open(full, "r", encoding="utf-8", errors="ignore") as fh:
        lines = fh.readlines()
    if line < 1 or line > len(lines):
        continue
    src = lines[line - 1]
    if ".get_integer()" in src or "static_cast<std::string>" in src:
        continue  # already patched
    m = assign_re.match(src)
    if not m:
        print("SKIP (no assignment):", full, line, src.rstrip())
        continue
    lhs, rhs, tail = m.group(1), m.group(2), m.group(3)
    # Decide cast type: string target vs integral target.
    if string_re.search(lhs):
        new = f"{lhs}static_cast<std::string>({rhs});{tail}"
    else:
        new = f"{lhs}{rhs}.get_integer();{tail}"
    lines[line - 1] = new
    with open(full, "w", encoding="utf-8") as fh:
        fh.writelines(lines)
    fixed += 1
    print("FIXED:", path, line, "->", new.strip())

print(f"\nPatched {fixed} lines.")
