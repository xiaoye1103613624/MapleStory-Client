# -*- coding: utf-8 -*-
"""round96 A 批前 3 条取证：083 活 IDA 符号 + 反编译

1) skill/D11  BT_HYPER / BT_GUILDSKILL / BT_RIDE 三空钮 -> 083 CUISkill::OnButtonClicked 分支
2) corewindow/D10 CWorldMapDlg 尺寸/侧栏 -> 083 CUIWorldMap / CWorldMapDlg 立即数
3) login-flow/D12 逐字节文档（无需 IDA，读 s9 + 当前端）
"""
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
    with urllib.request.urlopen(req, timeout=120) as r:
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
        if t.startswith("http://127.0.0.1") and ("/" in t):
            try:
                with urllib.request.urlopen(t.strip(), timeout=60) as r:
                    out.append(r.read().decode("utf-8", errors="replace"))
                continue
            except Exception:
                pass
        out.append(t)
    return "\n".join(out)


notify("notifications/initialized")
init = rpc("initialize", {"protocolVersion": "2024-11-05", "capabilities": {},
                          "clientInfo": {"name": "r96", "version": "1"}})
print("[init]", list(init.get("result", {}).keys()) if init else "EMPTY")
notify("notifications/initialized")

QUERIES = [
    ("skill_btn", [{"kind": "functions", "filter": "*CUISkill*OnButtonClicked*", "limit": 20},
                   {"kind": "functions", "filter": "*CUISkill*", "limit": 120}]),
    ("worldmap", [{"kind": "functions", "filter": "*CUIWorldMap*", "limit": 120},
                  {"kind": "functions", "filter": "*WorldMap*", "limit": 120}]),
]

for tag, qs in QUERIES:
    res = call("entity_query", {"queries": qs})
    t = text(res)
    p = os.path.join(OUT, "%s.json" % tag)
    open(p, "w", encoding="utf-8").write(t)
    print("[%s] len=%d -> %s" % (tag, len(t), p))
    try:
        d = json.loads(t)
        for q in d.get("queries", d if isinstance(d, list) else []):
            for it in (q.get("results") or q.get("matches") or []):
                print("   ", it.get("name") or it.get("symbol"), hex(it.get("ea") or it.get("addr") or 0),
                      it.get("size"))
    except Exception as e:
        print("   (parse)", e)

print("R96_IDA_OK")
