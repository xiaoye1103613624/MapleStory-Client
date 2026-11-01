# -*- coding: utf-8 -*-
"""Merge v95 COutPacket construction sites into one table.

Inputs (all produced by the scripts beside this file):
  ctor_sites.json        502 sites, decompiled bodies (resolve_dyn.py)
  trunc_ctor_sites.json   66 sites in 8 functions whose decompile is truncated (extract_trunc_send.py)
  trunc2_ctor_sites.json  12 sites in 3 functions missing from the first pass (extract_trunc2.py)
  + 2 manual sites in CClientSocket::OnConnect (push observed with insn_query)

Outputs:
  send_sites_all.json  [{fn,name,op,how,src}]  every ctor call site
  send_ops.json        {opcode: [fn,...]}
"""
import io, json, os, re, collections

BASE = os.path.dirname(os.path.abspath(__file__))

PUSH = re.compile(r"^\s*push\s+(?:([0-9A-F]+)h|(\d+))\s*(?:;.*)?$", re.I)


def resolve_push(prev):
    """Nearest push <imm> before the ctor call -> (opcode, matched line)."""
    for line in reversed(prev or []):
        m = PUSH.match(line)
        if m:
            return (int(m.group(1), 16) if m.group(1) else int(m.group(2))), line.strip()
    return None, None


rows = []

# 1) decompiled bodies (literal / assignment resolution done by resolve_dyn.py)
for r in json.load(io.open(os.path.join(BASE, "ctor_sites.json"), encoding="utf-8")):
    if r["op"] is None:
        continue
    rows.append({"fn": r["fn"], "name": r["name"], "op": int(r["op"]),
                 "how": r["how"], "src": "decompile"})

# 2) sites inside truncated functions
for fname in ("trunc_ctor_sites.json", "trunc2_ctor_sites.json"):
    p = os.path.join(BASE, fname)
    if not os.path.exists(p):
        continue
    last = {}  # fn -> (op, addr) of the nearest preceding resolved site
    batch = sorted(json.load(io.open(p, encoding="utf-8")),
                   key=lambda r: int(r.get("addr", "0x0"), 16))
    for r in batch:
        op = r.get("op")
        how = "resolved"
        if op is None:
            op, line = resolve_push(r.get("prev"))
            how = line or "unresolved"
        if op is None and r["fn"] in last:
            # jump-table branches share one `push <op>` upstream (e.g. 0x6282da -> 0x628304)
            op, src_addr = last[r["fn"]]
            how = "shared with %s" % src_addr
        if op is not None:
            last[r["fn"]] = (op, r.get("addr"))
        rows.append({"fn": r["fn"], "name": r.get("name") or r["fn"], "op": op,
                     "how": str(how), "src": "insn_query"})

# 3) manual: CClientSocket::OnConnect (2 ctor sites pulled with insn_query)
if os.path.exists(os.path.join(BASE, "onconnect_manual.json")):
    rows += json.load(io.open(os.path.join(BASE, "onconnect_manual.json"), encoding="utf-8"))

rows = [r for r in rows if r["op"] is not None]

# normalise addresses: "42ca60" / "0x42ca60" -> "0x42ca60"
for r in rows:
    a = str(r["fn"])
    r["fn"] = "0x%x" % int(a, 16)

# function names: fill mangled form for entries that only carry an address
callers = {}
if os.path.exists(os.path.join(BASE, "send_callers.json")):
    callers = json.load(io.open(os.path.join(BASE, "send_callers.json"), encoding="utf-8"))
for r in rows:
    if r["name"] == r["fn"] and r["fn"] in callers:
        r["name"] = callers[r["fn"]]

json.dump(rows, io.open(os.path.join(BASE, "send_sites_all.json"), "w", encoding="utf-8"),
          ensure_ascii=False, indent=1)

ops = collections.defaultdict(list)
for r in rows:
    ops[r["op"]].append(r["fn"])
json.dump({str(k): sorted(set(v)) for k, v in sorted(ops.items())},
          io.open(os.path.join(BASE, "send_ops.json"), "w", encoding="utf-8"),
          ensure_ascii=False, indent=1)

print("sites=%d ops=%d" % (len(rows), len(ops)))
for src in ("decompile", "insn_query"):
    print("  %-11s %d" % (src, sum(1 for r in rows if r["src"] == src)))
