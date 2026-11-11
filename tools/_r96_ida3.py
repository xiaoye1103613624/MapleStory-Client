# -*- coding: utf-8 -*-
"""round96 探针3：反编译 CUISkill::OnCreate（三空钮 id）+ CWorldMapDlg 尺寸链"""
import json
import os
import urllib.request

EP = "http://127.0.0.1:13339/mcp"
OUT = "tools/_ida_r96"
_id = [0]


def rpc(method, params):
    _id[0] += 1
    body = json.dumps({"jsonrpc": "2.0", "id": _id[0], "method": method, "params": params}).encode()
    req = urllib.request.Request(EP, data=body, headers={"Content-Type": "application/json",
                                                        "Accept": "application/json, text/event-stream"})
    with urllib.request.urlopen(req, timeout=300) as r:
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
    return rpc("tools/call", {"name": name, "arguments": args}).get("result", {})


def text(res):
    out = []
    for c in res.get("content", []) or []:
        t = c.get("text") or ""
        if t.startswith("http://127.0.0.1"):
            try:
                with urllib.request.urlopen(t.strip(), timeout=120) as r:
                    out.append(r.read().decode("utf-8", errors="replace"))
                continue
            except Exception:
                pass
        out.append(t)
    return "\n".join(out)


notify("notifications/initialized")
rpc("initialize", {"protocolVersion": "2024-11-05", "capabilities": {},
                   "clientInfo": {"name": "r96c", "version": "1"}})
notify("notifications/initialized")

TARGETS = [
    ("CUISkill_OnCreate", "0x8aaa88"),
    ("CWorldMapDlg_CreateWorldMapDlg", "0x9eb366"),
    ("CWorldMapDlg_OnCreate", "0x9eb75b"),
    ("CWorldMapDlg_Draw", "0x9eb9c6"),
    ("CWorldMapDlg_MakeBorder", "0x9ebfc3"),
]

for name, addr in TARGETS:
    res = call("decompile", {"addr": addr})
    t = text(res)
    try:
        d = json.loads(t)
        code = d.get("code") or t
    except Exception:
        code = t
    p = os.path.join(OUT, "%s.code.txt" % name)
    open(p, "w", encoding="utf-8").write(code)
    print("[dec] %s %s lines=%d %s" % (name, addr, code.count("\n") + 1, p))

print("R96_IDA3_OK")
