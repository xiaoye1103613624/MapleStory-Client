# -*- coding: utf-8 -*-
"""round96 探针2：CUISkill 按钮回调 + WorldMap 窗族符号 + 反编译"""
import json
import os
import sys
import urllib.request

EP = "http://127.0.0.1:13339/mcp"
OUT = "tools/_ida_r96"
os.makedirs(OUT, exist_ok=True)
_id = [0]


def rpc(method, params):
    _id[0] += 1
    body = json.dumps({"jsonrpc": "2.0", "id": _id[0], "method": method, "params": params}).encode()
    req = urllib.request.Request(EP, data=body, headers={"Content-Type": "application/json",
                                                        "Accept": "application/json, text/event-stream"})
    with urllib.request.urlopen(req, timeout=180) as r:
        raw = r.read().decode("utf-8", errors="replace")
    if not raw.strip():
        return {}
    if raw.lstrip().startswith("event:"):
        for line in raw.splitlines():
            if line.startswith("data:"):
                raw = line[5:].strip()
                break
    return json.loads(raw)


def notify(method, params=None):
    _id[0] += 1
    body = json.dumps({"jsonrpc": "2.0", "method": method, "params": params or {}}).encode()
    req = urllib.request.Request(EP, data=body, headers={"Content-Type": "application/json",
                                                        "Accept": "application/json, text/event-stream"})
    try:
        with urllib.request.urlopen(req, timeout=30) as r:
            r.read()
    except Exception:
        pass


def call(name, args):
    r = rpc("tools/call", {"name": name, "arguments": args})
    return r.get("result", r)


def text(res):
    out = []
    for c in res.get("content", []) or []:
        t = c.get("text") or ""
        if t.startswith("http://127.0.0.1"):
            try:
                with urllib.request.urlopen(t.strip(), timeout=60) as r:
                    out.append(r.read().decode("utf-8", errors="replace"))
                continue
            except Exception:
                pass
        out.append(t)
    return "\n".join(out)


notify("notifications/initialized")
rpc("initialize", {"protocolVersion": "2024-11-05", "capabilities": {},
                   "clientInfo": {"name": "r96b", "version": "1"}})
notify("notifications/initialized")

FILTERS = [
    "*CUISkill*OnChildNotify*",
    "*CUISkill*OnMouseButton*",
    "*CUISkill*OnButton*",
    "*CUISkill@@*",
    "*CWorldMap*",
    "*WorldMapDlg*",
    "*WorldMap*",
]

allrows = {}
for f in FILTERS:
    res = call("entity_query", {"queries": [{"kind": "functions", "filter": f, "limit": 200}]})
    t = text(res)
    try:
        d = json.loads(t)
        rows = d[0].get("data") or []
    except Exception:
        rows = []
    allrows[f] = rows
    print("=== %s total=%d" % (f, len(rows)))
    for it in rows[:60]:
        print("   %s  %s  size=%s" % (it.get("addr"), it.get("name"), it.get("size")))

json.dump(allrows, open(os.path.join(OUT, "filters.json"), "w", encoding="utf-8"),
          ensure_ascii=False, indent=1)

# 拉若干关键反编译
TARGETS = []
for f in ("*CUISkill*OnButton*", "*CUISkill*OnChildNotify*", "*CUISkill*OnMouseButton*"):
    for it in allrows.get(f, []):
        TARGETS.append((it["name"], it["addr"]))

seen = set()
for name, addr in TARGETS:
    if addr in seen:
        continue
    seen.add(addr)
    res = call("decompile", {"addr": addr})
    t = text(res)
    try:
        d = json.loads(t)
        code = d.get("code") or t
    except Exception:
        code = t
    safe = name.replace("?", "").replace("@", "_").replace("$", "_")[:60]
    p = os.path.join(OUT, "%s_%s.code.txt" % (safe, addr.replace("0x", "")))
    open(p, "w", encoding="utf-8").write(code)
    print("[dec] %s %s -> %d bytes %s" % (name, addr, len(code), p))

print("R96_IDA2_OK")
