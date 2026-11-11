# -*- coding: utf-8 -*-
import json, urllib.request, uuid, os
ENDPOINT="http://127.0.0.1:13339/mcp"; SESSION=str(uuid.uuid4())
def post(p):
    req=urllib.request.Request(ENDPOINT,data=json.dumps(p).encode(),headers={"Content-Type":"application/json","Accept":"application/json, text/event-stream","Mcp-Session-Id":SESSION},method="POST")
    with urllib.request.urlopen(req,timeout=180) as r: raw=r.read().decode("utf-8","replace")
    if not raw.strip(): return {}
    for ln in raw.splitlines():
        if ln.startswith("data:"): raw=ln[5:].strip(); break
    return json.loads(raw) if raw.strip() else {}
def notify(p):
    req=urllib.request.Request(ENDPOINT,data=json.dumps(p).encode(),headers={"Content-Type":"application/json","Accept":"application/json, text/event-stream","Mcp-Session-Id":SESSION},method="POST")
    with urllib.request.urlopen(req,timeout=180) as r: r.read()
post({"jsonrpc":"2.0","id":0,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"r90","version":"1"}}})
notify({"jsonrpc":"2.0","method":"notifications/initialized","params":{}})
def call(name,args): return post({"jsonrpc":"2.0","id":1,"method":"tools/call","params":{"name":name,"arguments":args}})
def txt(res):
    out=[]; r=res.get("result",res)
    for it in r.get("content",[]):
        t=it.get("text")
        if t: out.append(t)
    blob="\n".join(out)
    for tok in blob.split():
        if tok.startswith("http://127.0.0.1") and tok.endswith(".json"):
            with urllib.request.urlopen(tok,timeout=180) as rr: return rr.read().decode("utf-8","replace")
    return blob
os.makedirs("tools/_ida_r90",exist_ok=True)
for nm,a in [("OnDropLeaveField","0x506590"),("OnDropEnterField","0x505900")]:
    s=txt(call("decompile",{"addr":a}))
    try: code=json.loads(s).get("code","")
    except Exception: code=s
    open("tools/_ida_r90/"+nm+".code.txt","w",encoding="utf-8").write(code)
    open("tools/_ida_r90/"+nm+".dec.txt","w",encoding="utf-8").write(s)
    print("DEC",nm,a,"lines",len(code.splitlines()))
code=open("tools/_ida_r90/OnDropLeaveField.code.txt",encoding="utf-8").read()
print("=== OnDropLeaveField animation critical lines ===")
for i,ln in enumerate(code.splitlines(),1):
    if any(k in ln for k in ["== 4","==4","Decode2","Decode4","Decode1","delay","update_time","+ 68","+68","+ 72","+72","IWzGr","Animate","RemoveCanvas"]):
        print(f"{i}: {ln.strip()[:140]}")
