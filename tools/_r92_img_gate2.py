# -*- coding: utf-8 -*-
"""round92 slice2 orange-wz 128B gate: UIWindow.img/Channel plate nodes (read-only)."""
import json, urllib.request, urllib.error, os
BASE="http://127.0.0.1:10012/mcp"
IMG=r"E:/MXD/BeiDou-Client-xy/Data/UI/UIWindow.img"
KEYFILE=r"E:/project/MapleStory-Client/tools/_gms_key_128.b64"
IV_B64="TSPHKw=="; S=None
def post(body,sid=None):
    data=json.dumps(body).encode("utf-8")
    req=urllib.request.Request(BASE,data=data,method="POST")
    req.add_header("Content-Type","application/json"); req.add_header("Accept","application/json, text/event-stream"); req.add_header("MCP-Protocol-Version","2025-03-26")
    if sid: req.add_header("Mcp-Session-Id",sid)
    try:
        with urllib.request.urlopen(req,timeout=180) as r: raw=r.read().decode("utf-8","replace"); hs=r.headers
    except urllib.error.HTTPError as e: raw=e.read().decode("utf-8","replace"); hs=e.headers
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
with open(KEYFILE,"r",encoding="utf-8") as f: KEY=f.read().strip()
log={"endpoint":BASE,"img":IMG,"iv_b64":IV_B64,"key_b64_len":len(KEY),"key_spec":"128B WzAESConstant.DEFAULT_KEY","queries":{},"verdict":"","errors":[]}
r,hs=post({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-03-26","capabilities":{},"clientInfo":{"name":"r92-img-gate2","version":"1"}}}); S=hs.get("Mcp-Session-Id") if hs else None
log["init_ok"]=bool(r.get("result")); post({"jsonrpc":"2.0","method":"notifications/initialized","params":{}},S)
r,_=post({"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"load_files","arguments":{"paths":[IMG],"key":{"name":"gms","ivBase64":IV_B64,"userKeyBase64":KEY},"exclusive":False}}},S)
log["load_isError"]=r.get("result",{}).get("isError",False); log["load_text"]=txt(r)[:300]
for kw in ["Channel","channel0","channel1","channel2","BtChange","BtCancel"]:
    op="list_children" if kw=="Channel" else "search"
    args={"op":op,"rootPath":IMG,"maxDepth":1} if op=="list_children" else {"op":op,"rootPath":IMG,"keyword":kw,"searchIn":"name","maxDepth":5}
    if op=="list_children": args["nodePath"]="Channel"
    r,_=post({"jsonrpc":"2.0","id":10+len(log["queries"]),"method":"tools/call","params":{"name":"query_nodes","arguments":args}},S)
    t=txt(r); ie=r.get("result",{}).get("isError",False)
    log["queries"][kw]={"isError":ie,"text_len":len(t),"head":t[:400]}
    if "解析失败" in t or "out of bounds" in t or ie: log["errors"].append(kw+": "+t[:160])
post({"jsonrpc":"2.0","id":99,"method":"tools/call","params":{"name":"unload_all","arguments":{"hintGc":False}}},S)
ok=(not log["load_isError"]) and not log["errors"]
log["verdict"]="GATE_OK_128B_CHANNEL_NODES_ENUMERATED" if ok else "GATE_FAIL"
os.makedirs("tools/_ida_r92",exist_ok=True)
with open("tools/_ida_r92/img_gate_channel.json","w",encoding="utf-8") as f: json.dump(log,f,ensure_ascii=False,indent=2)
print(json.dumps({"verdict":log["verdict"],"load_isError":log["load_isError"],"queries":{k:{"isError":v["isError"],"len":v["text_len"]} for k,v in log["queries"].items()},"errors":log["errors"]},ensure_ascii=False))
