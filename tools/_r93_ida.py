# -*- coding: utf-8 -*-
"""round93 probe: 083 是否存在 UISpecial / 福利中心(签到/兑换/排行/转职/传送) 窗族。"""
import json, urllib.request, os
BASE="http://127.0.0.1:13339/mcp"; OUT="tools/_ida_r93"; S=None
os.makedirs(OUT,exist_ok=True)
def post(body,sid=None,timeout=180):
    data=json.dumps(body).encode("utf-8")
    req=urllib.request.Request(BASE,data=data,method="POST")
    req.add_header("Content-Type","application/json"); req.add_header("Accept","application/json, text/event-stream")
    if sid: req.add_header("Mcp-Session-Id",sid)
    with urllib.request.urlopen(req,timeout=timeout) as r:
        raw=r.read().decode("utf-8","replace"); hs=r.headers
    if not raw.strip(): return {},hs
    for ln in raw.splitlines():
        if ln.startswith("data:"): raw=ln[5:].strip(); break
    try: return json.loads(raw),hs
    except Exception: return {"_raw":raw[:400]},hs
def txt(res):
    o=[]; rr=res.get("result",res) if isinstance(res,dict) else {}
    for it in rr.get("content",[]):
        if isinstance(it,dict) and it.get("text"): o.append(it["text"])
    return "\n".join(o)
r,hs=post({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-03-26","capabilities":{},"clientInfo":{"name":"r93","version":"1"}}})
S=hs.get("Mcp-Session-Id") if hs else None
post({"jsonrpc":"2.0","method":"notifications/initialized","params":{}},S)
res={}
for flt in ["*Special*","*UISpecial*","*Fairy*","*Welfare*","*CheckIn*","*Attendance*"]:
    r,_=post({"jsonrpc":"2.0","id":10+len(res),"method":"tools/call","params":{"name":"entity_query","arguments":{"queries":[{"kind":"functions","filter":flt,"limit":40}]}}},S)
    t=txt(r)
    try:
        d=json.loads(t)
        q=d["results"][0] if isinstance(d,dict) and d.get("results") else d
        n=q.get("total",0)
        names=[x.get("name") for x in (q.get("data") or [])][:8]
    except Exception:
        n="?"; names=[t[:120]]
    res[flt]={"total":n,"sample":names}
    print(flt,"total=",n,names[:4])
open(OUT+"/special_family.json","w",encoding="utf-8").write(json.dumps(res,ensure_ascii=False,indent=2))
print("R93_IDA_OK")
