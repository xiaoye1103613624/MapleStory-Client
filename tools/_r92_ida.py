# -*- coding: utf-8 -*-
"""round92 slice1: 083 macro family full decompile (read-only)."""
import json,urllib.request,uuid,os
E="http://127.0.0.1:13339/mcp";S=str(uuid.uuid4())
def post(p):
 r=urllib.request.Request(E,data=json.dumps(p).encode(),headers={"Content-Type":"application/json","Accept":"application/json, text/event-stream","Mcp-Session-Id":S},method="POST")
 with urllib.request.urlopen(r,timeout=240) as x:raw=x.read().decode("utf-8","replace")
 if not raw.strip():return {}
 for ln in raw.splitlines():
  if ln.startswith("data:"):raw=ln[5:].strip();break
 return json.loads(raw) if raw.strip() else {}
def notify(p):
 r=urllib.request.Request(E,data=json.dumps(p).encode(),headers={"Content-Type":"application/json","Accept":"application/json, text/event-stream","Mcp-Session-Id":S},method="POST")
 with urllib.request.urlopen(r,timeout=240) as x:x.read()
post({"jsonrpc":"2.0","id":0,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"r92d","version":"1"}}});notify({"jsonrpc":"2.0","method":"notifications/initialized","params":{}})
_id=[2]
def call(n,a):
 _id[0]+=1
 return post({"jsonrpc":"2.0","id":_id[0],"method":"tools/call","params":{"name":n,"arguments":a}})
def txt(res):
 out=[]
 for it in res.get("result",res).get("content",[]):
  if it.get("text"):out.append(it["text"])
 blob="\n".join(out)
 for t in blob.split():
  if t.startswith("http://127.0.0.1") and (".json" in t or ".txt" in t):
   try:
    with urllib.request.urlopen(t,timeout=240) as rr:return rr.read().decode("utf-8","replace")
   except Exception:pass
 return blob
os.makedirs("tools/_ida_r92",exist_ok=True)
ANCHORS=[
 ("CUIMacroSys_OnCreate","0x8a8504"),
 ("CUIMacroSysEx_ResetInfo","0x8a9788"),
 ("CUIMacroSysEx_GetIndexByPos","0x8a97c4"),
 ("CUIMacroSysEx_OnMouseButton","0x8a98fe"),
 ("CUIMacroSysEx_OnKey","0x8b8eef"),
 ("CUIMacroSys_OnSelected","0x8b8e63"),
 ("CUIMacroSys_OnButtonClicked","0x8aa134"),
 ("CUIMacroSys_ResetInfo","0x8b8daa"),
 ("CUIMacroSys_GetIndexByPos","0x8b8de3"),
 ("CUIMacroSysEx_OnButtonClicked","0x8b9747"),
]
for nm,a in ANCHORS:
 try:
  s=txt(call("decompile",{"addr":a}))
  try:c=json.loads(s).get("code","")
  except Exception:c=s
  open("tools/_ida_r92/"+nm+".code.txt","w",encoding="utf-8").write(c)
  open("tools/_ida_r92/"+nm+".dec.txt","w",encoding="utf-8").write(s)
  print("DEC",nm,a,"lines",len(c.splitlines()))
  for ln in c.splitlines():
   if any(k in ln for k in ["a2 ==","a2==","a3 ==","case ","if (","switch","return","Encode","Send","SetChecked","SetText","OnSelected","ResetInfo","GetIndexByPos"]) and ";" in ln:
    print("   ",ln.strip()[:120])
 except Exception as e:print("FAIL",nm,repr(e)[:120])
# symbol query for CMacroSysMan family
q=call("entity_query",{"queries":[{"kind":"functions","filter":"*CMacroSysMan*","limit":60}]})
s=txt(q)
open("tools/_ida_r92/q__CMacroSysMan_.txt","w",encoding="utf-8").write(s)
print("Q CMacroSysMan len",len(s))
