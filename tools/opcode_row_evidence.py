#!/usr/bin/env python3
"""T2 helper: for the rows of interest in the opcode matrix, print the evidence
needed to classify them (construction site, enclosing packet class, and whether
that class is instantiated anywhere outside its own declaration).

A packet class that is never instantiated is a dead UI path: the opcode exists in
the enum and in a ctor, but the client can never put it on the wire. That is the
difference between a real protocol mismatch and stale source text.

用法:
    python tools/opcode_row_evidence.py                 # CLIENT_ONLY / MATCHED_NO_HANDLER
    python tools/opcode_row_evidence.py --status s2c    # 也打印 s2c 异常行
"""
import argparse
import json
import os
import re
import sys

CLIENT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MATRIX = os.path.join(CLIENT, "docs", "features", "s9-protocol-align",
                      "protocol", "opcode-matrix.json")

SKIP = (".git", "/x64", "/build", "/docs", "/tools", ".vs", "debug", "release",
        "third_party", "vendor")
PAT_CLASS = re.compile(r"\bclass\s+([A-Za-z_][A-Za-z0-9_]*)")

_cache = {}


def lines_of(rel):
    if rel not in _cache:
        text = open(os.path.join(CLIENT, rel.replace("/", os.sep)),
                    encoding="utf-8", errors="replace").read()
        _cache[rel] = text
    return _cache[rel]


def code_of(rel):
    """Source with line comments removed - reference counting must ignore docs."""
    return re.sub(r"//[^\n]*", "", lines_of(rel))


def client_files():
    for dirpath, dirnames, files in os.walk(CLIENT):
        norm = dirpath.replace(os.sep, "/").lower()
        dirnames[:] = [d for d in dirnames if d.lower() not in
                       (".git", "x64", "build", "debug", "release", ".vs")]
        if any(t in norm for t in SKIP):
            continue
        for fn in files:
            if fn.endswith((".cpp", ".h")):
                yield os.path.relpath(os.path.join(dirpath, fn),
                                      CLIENT).replace(os.sep, "/")


def enclosing_class(rel, line):
    ls = lines_of(rel).splitlines()
    for i in range(min(line, len(ls)) - 1, -1, -1):
        m = PAT_CLASS.search(ls[i])
        if m:
            return m.group(1)
    return None


def instantiations(cls, decl_rel):
    """Uses of `cls` outside its own declaration / ctor definition."""
    hits = []
    pat = re.compile(r"\b%s\b" % re.escape(cls))
    own_decl = re.compile(r"\b(class|struct)\s+%s\b|\b%s\s*\(" % (cls, cls))
    for rel in client_files():
        body = code_of(rel)
        for i, ln in enumerate(body.splitlines(), start=1):
            if not pat.search(ln):
                continue
            if rel == decl_rel and own_decl.search(ln):
                continue          # 声明与构造器定义自身，不算使用点
            hits.append("%s:%d" % (rel, i))
    return hits


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--status", default="c2s",
                    help="c2s | s2c | all")
    ap.add_argument("--names", default="", help="comma separated client names")
    args = ap.parse_args()

    data = json.load(open(MATRIX, encoding="utf-8"))
    wanted = {n.strip() for n in args.names.split(",") if n.strip()}

    if args.status in ("c2s", "all"):
        for r in data["c2s"]:
            if wanted:
                if r["client_name"] not in wanted:
                    continue
            elif r["status"] not in ("CLIENT_ONLY", "CLIENT_ONLY_DEAD",
                                     "MATCHED_NO_HANDLER"):
                continue
            print("=== %-24s val=%-6s status=%s  s9=%s handler=%s annot=%s" % (
                r["client_name"], r["client_value"], r["status"], r["s9_name"],
                r["s9_handler"], r["annotation_consistent"]))
            seen = []
            for s in r["client_sent_at"]:
                cls = enclosing_class(s["file"], s["line"])
                if cls in seen:
                    continue
                seen.append(cls)
                inst = instantiations(cls, s["file"])
                print("    ctor %-28s @ %s:%d   instances=%d  %s" % (
                    cls, s["file"], s["line"], len(inst),
                    ", ".join(inst[:5]) if inst else "(none - never built)"))
            if not seen:
                print("    no construction site (dead enum)")

    if args.status in ("s2c", "all"):
        for r in data["s2c"]:
            if r["status"] in ("UNHANDLED_EMITTED", "ENUM_NO_HANDLER"):
                print("--- s2c %-34s val=%-6s emitted=%s status=%s client=%s" % (
                    r["s9_name"], r["s9_value"], r["s9_emitted"], r["status"],
                    r["client_names"]))


if __name__ == "__main__":
    sys.exit(main())
