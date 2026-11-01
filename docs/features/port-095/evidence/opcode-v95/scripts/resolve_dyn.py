# -*- coding: utf-8 -*-
"""Resolve every COutPacket(opcode) construction point in dec_send/, including casted-this and
variable opcode expressions (trace nearest assignment)."""
import json, glob, os, re

CTOR = re.compile(r"COutPacket::COutPacket\(this: (?:\(COutPacket \*\))?&?[\w\.\[\]]+, nType: ((?:\([^()]*\)\s*)*[\w\.\[\]>-]+)\)")
NUM = re.compile(r"^(\d+)$")
IDENT = re.compile(r"[A-Za-z_]\w*(?:\.\w+|\[\d+\]|\.\w+\[\d+\])*")


def strip_num(e):
    e = e.strip()
    m = NUM.match(e)
    if m:
        return int(m.group(1))
    # (int)v7[0]._m_pStr  /  (char *)x  /  (GW_ItemSlotBase *)107
    m = re.match(r"^\([^)]*\)\s*(\d+)$", e)
    if m:
        return int(m.group(1))
    m = re.match(r"^\([^)]*\)\s*(.+)$", e)
    if m:
        return strip_num(m.group(1))
    return None


def resolve(code, expr, pos):
    v = strip_num(expr)
    if v is not None:
        return v, "literal"
    name = re.sub(r"^\((?:[^()]*)\)\s*", "", expr.strip())
    head = code[:pos]
    lines = head.split("\n")
    # 1) direct:  NAME = <num or cast num> ;
    pat = re.compile(r"(?<![\w\.])" + re.escape(name) + r"\s*=\s*\(?[^;=]*?(\d+)\s*;")
    for ln in reversed(lines[-80:]):
        m = pat.search(ln)
        if m:
            return int(m.group(1)), "assign:" + ln.strip()
    # 2) LOBYTE(NAME) = N   (secure/fuse regs)
    pat2 = re.compile(r"LOBYTE\(" + re.escape(name) + r"\)\s*=\s*(\d+)")
    for ln in reversed(lines[-80:]):
        m = pat2.search(ln)
        if m:
            return int(m.group(1)), "lobyte:" + ln.strip()
    # 3) right-hand cast only:  NAME = (T *)107
    pat3 = re.compile(r"(?<![\w\.])" + re.escape(name) + r"\s*=\s*\([^)]*\)\s*(\d+)\s*;")
    for ln in reversed(lines[-80:]):
        m = pat3.search(ln)
        if m:
            return int(m.group(1)), "cast:" + ln.strip()
    return None, "unresolved"


def disp_name(code, fn):
    m = re.search(r"(\w+(?:::\w+)+)\(", code)
    return m.group(1) if m else fn


rows = []
for path in sorted(glob.glob("dec_send/*.json")):
    fn = os.path.basename(path)[:-5]
    try:
        sc = json.load(open(path, encoding="utf-8"))["result"]["structuredContent"]
    except Exception:
        continue
    code = sc.get("code") or ""
    if "COutPacket::COutPacket" not in code:
        continue
    for m in CTOR.finditer(code):
        expr = m.group(1).strip()
        val, how = resolve(code, expr, m.start())
        rows.append({"fn": fn, "name": disp_name(code, fn), "expr": expr, "op": val, "how": how})

json.dump(rows, open("ctor_sites.json", "w", encoding="utf-8"), ensure_ascii=False, indent=1)
unres = [r for r in rows if r["op"] is None]
print("sites=%d resolved=%d unresolved=%d" % (len(rows), len(rows) - len(unres), len(unres)))
for r in unres:
    print("UNRES", r["fn"], r["name"], "|", r["expr"], "|", r["how"])
