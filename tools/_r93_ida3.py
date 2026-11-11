# -*- coding: utf-8 -*-
"""round93: 083 CPortal 族符号（正确解析 results 数组）。"""
import json, urllib.request, os

BASE = "http://127.0.0.1:13339/mcp"
OUT = "tools/_ida_r93"
os.makedirs(OUT, exist_ok=True)
S = None


def post(body, sid=None, timeout=180):
    data = json.dumps(body).encode("utf-8")
    req = urllib.request.Request(BASE, data=data, method="POST")
    req.add_header("Content-Type", "application/json")
    req.add_header("Accept", "application/json, text/event-stream")
    if sid:
        req.add_header("Mcp-Session-Id", sid)
    with urllib.request.urlopen(req, timeout=timeout) as r:
        raw = r.read().decode("utf-8", "replace")
        hs = r.headers
    if not raw.strip():
        return {}, hs
    for ln in raw.splitlines():
        if ln.startswith("data:"):
            raw = ln[5:].strip()
            break
    try:
        return json.loads(raw), hs
    except Exception:
        return {"_raw": raw[:400]}, hs


def txt(res):
    o = []
    rr = res.get("result", res) if isinstance(res, dict) else {}
    for it in rr.get("content", []):
        if isinstance(it, dict) and it.get("text"):
            o.append(it["text"])
    return "\n".join(o)


def q(flt, limit=80):
    r, _ = post({"jsonrpc": "2.0", "id": 99, "method": "tools/call",
                 "params": {"name": "entity_query",
                            "arguments": {"queries": [{"kind": "functions", "filter": flt, "limit": limit}]}}}, S)
    t = txt(r)
    d = json.loads(t)
    if isinstance(d, list):
        res = d
    elif isinstance(d, dict):
        res = d.get("results") or []
    else:
        res = []
    if not res:
        return 0, []
    qq = res[0] if isinstance(res[0], dict) else {}
    rows = qq.get("data") or []
    return qq.get("total", 0), [(x.get("addr"), x.get("name"), x.get("size")) for x in rows if isinstance(x, dict)]


r, hs = post({"jsonrpc": "2.0", "id": 1, "method": "initialize",
              "params": {"protocolVersion": "2025-03-26", "capabilities": {},
                         "clientInfo": {"name": "r93c", "version": "1"}}})
S = hs.get("Mcp-Session-Id") if hs else None
post({"jsonrpc": "2.0", "method": "notifications/initialized", "params": {}}, S)

out = {}
for flt in ["*CPortal@@*", "*CPortal::*", "*Portal*"]:
    n, rows = q(flt)
    out[flt] = {"total": n, "rows": rows[:30]}
    print("==", flt, "total", n)
    for a, nm, sz in rows[:16]:
        print("   ", a, sz, nm)

with open(OUT + "/portal_family.json", "w", encoding="utf-8") as f:
    json.dump(out, f, ensure_ascii=False, indent=2)
print("R93C_OK")
