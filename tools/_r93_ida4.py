# -*- coding: utf-8 -*-
"""round93: 083 传送门触发逻辑反编译（CUserLocal::CheckPortal_Collision 等）。"""
import json, urllib.request, os

BASE = "http://127.0.0.1:13339/mcp"
OUT = "tools/_ida_r93"
os.makedirs(OUT, exist_ok=True)
S = None


def post(body, sid=None, timeout=300):
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


def decompile(addr):
    r, _ = post({"jsonrpc": "2.0", "id": 7, "method": "tools/call",
                 "params": {"name": "decompile", "arguments": {"addr": addr}}}, S)
    t = txt(r)
    try:
        d = json.loads(t)
        if isinstance(d, dict):
            return d.get("code") or d.get("result") or t
        return t
    except Exception:
        return t


r, hs = post({"jsonrpc": "2.0", "id": 1, "method": "initialize",
              "params": {"protocolVersion": "2025-03-26", "capabilities": {},
                         "clientInfo": {"name": "r93d", "version": "1"}}})
S = hs.get("Mcp-Session-Id") if hs else None
post({"jsonrpc": "2.0", "method": "notifications/initialized", "params": {}}, S)

targets = [
    ("CUserLocal_CheckPortal_Collision", "0x94dac6"),
    ("CPortalList_FindPortal_Collision", "0x712b61"),
    ("CPortalList_FindPortal", "0x712ab1"),
]
for name, addr in targets:
    code = decompile(addr)
    p = OUT + "/" + name + ".code.txt"
    with open(p, "w", encoding="utf-8", newline="\n") as f:
        f.write(code if isinstance(code, str) else json.dumps(code, ensure_ascii=False))
    lines = (code if isinstance(code, str) else "").count("\n") + 1
    print("==", name, addr, "lines", lines)
print("R93D_OK")
