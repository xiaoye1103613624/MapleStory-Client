# -*- coding: utf-8 -*-
"""round92 slice2: decompile CUIChannelShift::OnCreate/Draw to pin this+32 semantics."""
import json, urllib.request, os
BASE="http://127.0.0.1:13339/mcp"; OUT="tools/_ida_r92"; S=None
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
r,hs=post({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-03-26","capabilities":{},"clientInfo":{"name":"r92-ida2","version":"1"}}})
S=hs.get("Mcp-Session-Id") if hs else None
post({"jsonrpc":"2.0","method":"notifications/initialized","params":{}},S)
# find tool names
r,_=post({"jsonrpc":"2.0","id":2,"method":"tools/list","params":{}},S)
tools=[t["name"] for t in r.get("result",{}).get("tools",[])]
print("TOOLS", [t for t in tools if "decomp" in t.lower() or "entity" in t.lower() or "query" in t.lower()])
def decompile(addr):
    r,_=post({"jsonrpc":"2.0","id":50,"method":"tools/call","params":{"name":"decompile","arguments":{"addr":addr}}},S)
    t=txt(r)
    try:
        j=json.loads(t); return j.get("code",t)
    except Exception: return t
for nm,addr in [("CUIChannelShift_OnCreate","0x994da4"),("CUIChannelShift_Draw","0x995a35"),("CUIChannelShift_ctor","0x994bcf")]:
    c=decompile(addr)
    open(OUT+"/"+nm+".code.txt","w",encoding="utf-8").write(c)
    open(OUT+"/"+nm+".dec.txt","w",encoding="utf-8").write("/* addr "+addr+" */\n"+c)
    print(nm,addr,"lines",len(c.splitlines()))
post({"jsonrpc":"2.0","id":99,"method":"tools/call","params":{"name":"unload_all","arguments":{"hintGc":False}}},S)
print("R92_IDA2_OK")
