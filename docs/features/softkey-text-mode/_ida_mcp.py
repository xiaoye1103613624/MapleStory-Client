# -*- coding: utf-8 -*-
"""Minimal Streamable-HTTP MCP client for ida-pro-mcp at 127.0.0.1:13337."""
import json, urllib.request, sys

BASE = "http://127.0.0.1:13337/mcp"
SID = None

def _post(payload, sid=None):
    req = urllib.request.Request(BASE, data=json.dumps(payload).encode(),
                                 headers={"Content-Type": "application/json",
                                          "Accept": "application/json, text/event-stream",
                                          **({"Mcp-Session-Id": sid} if sid else {})})
    with urllib.request.urlopen(req, timeout=120) as r:
        sid_out = r.headers.get("Mcp-Session-Id")
        body = r.read().decode("utf-8", "ignore")
    # parse either plain json or SSE
    if body.startswith("event") or "\ndata: " in body:
        datas = [l[6:] for l in body.splitlines() if l.startswith("data: ")]
        obj = json.loads(datas[-1]) if datas else None
    else:
        obj = json.loads(body) if body else None
    return obj, sid_out

def init():
    global SID
    obj, SID = _post({"jsonrpc":"2.0","id":1,"method":"initialize","params":
                      {"protocolVersion":"2025-06-18","capabilities":{},
                       "clientInfo":{"name":"dsh","version":"1"}}})
    _post({"jsonrpc":"2.0","method":"notifications/initialized"}, SID)
    return obj

def call(tool, args):
    obj, _ = _post({"jsonrpc":"2.0","id":2,"method":"tools/call","params":
                    {"name":tool,"arguments":args}}, SID)
    if obj and "error" in obj:
        return {"error": obj["error"]}
    res = obj.get("result", obj) if obj else {}
    if isinstance(res, dict) and "content" in res:
        txt = "\n".join(c.get("text","") for c in res["content"] if c.get("type")=="text")
        try: return json.loads(txt)
        except Exception: return txt
    return res

def tools():
    obj, _ = _post({"jsonrpc":"2.0","id":3,"method":"tools/list","params":{}}, SID)
    return [t["name"] for t in obj["result"]["tools"]]

if __name__ == "__main__":
    init()
    print(json.dumps(tools(), ensure_ascii=False, indent=0))
