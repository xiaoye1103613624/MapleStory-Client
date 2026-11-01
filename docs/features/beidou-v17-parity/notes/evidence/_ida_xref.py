import json, urllib.request, pathlib
ev = pathlib.Path(r"E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\evidence")
session = "d3395606-0963-4657-91f1-84f198f097cc"
url = "http://127.0.0.1:13338/mcp"
_id = 900

def call(name, arguments=None, timeout=180):
    global _id
    _id += 1
    body = {"jsonrpc":"2.0","id":_id,"method":"tools/call","params":{"name":name,"arguments":arguments or {}}}
    req = urllib.request.Request(url, data=json.dumps(body).encode(), headers={
        "Content-Type":"application/json","Accept":"application/json","Mcp-Session-Id":session})
    with urllib.request.urlopen(req, timeout=timeout) as resp:
        raw = resp.read().decode("utf-8","replace")
    j = json.loads(raw)
    if "error" in j:
        return {"_error": j["error"]}
    result = j.get("result", {})
    content = result.get("content")
    if content and isinstance(content, list) and content and "text" in content[0]:
        text = content[0]["text"]
        try: return json.loads(text)
        except Exception: return {"_text": text[:20000]}
    return result

# data_ref / code_ref for Maker gauge string
addrs = {
  "Maker": "0xb39260",
  "ViciousHammer": "0xb394e8",
  "VegaSpell": "0xb395d4",
  "CashGachapon": "0xb3de68",
  "MonsterBook_BtRegister": None,
  "PartySearch": None,
  "Delivery": None,
  "goPartySearch": None,
  "goAllianceINVITE": None,
}

# resolve missing via find string
for name in ["goPartySearch","goAllianceINVITE","goAllianceTALK"]:
    r = call("find", {"type":"string", "targets":[name], "limit": 3})
    print(name, r if not isinstance(r,list) else r[:1])
    if isinstance(r, list) and r:
        item=r[0]
        # extract addr
        matches=item.get("matches") if isinstance(item,dict) else None
        if isinstance(matches, list) and matches:
            m0=matches[0]
            addr = m0 if isinstance(m0,str) else (m0.get("addr") if isinstance(m0,dict) else None)
            addrs[name]=addr
        elif isinstance(item,dict):
            # maybe target -> [addrs]
            for k,v in item.items():
                if isinstance(v,list) and v and isinstance(v[0],str) and v[0].startswith("0x"):
                    addrs[name]=v[0]; break

# Also from exact_path_find
exact=json.loads((ev/"ida_exact_path_find.json").read_text(encoding="utf-8"))
for k,v in exact.items():
    if k.startswith("UI/UIWindow.img/") and isinstance(v,list) and v:
        node=k.split("/")[-1]
        addrs.setdefault(node, v[0])

# MonsterBook / Delivery / PartySearch from ascii extract paths
for pat,key in [
    (r"UIWindow\.img/MonsterBook", "MonsterBook"),
    (r"UIWindow\.img/Delivery", "Delivery"),
    (r"UIWindow\.img/PartySearch", "PartySearch"),
    (r"UIWindow\.img/Radio", "Radio"),
    (r"UIWindow\.img/EnchantSkill", "EnchantSkill"),
    (r"UIWindow\.img/NewYearsCard", "NewYearsCard"),
    (r"UIWindow\.img/AranSkillGuide", "AranSkillGuide"),
    (r"UIWindow\.img/Book", "Book"),
]:
    r=call("find_regex", {"pattern": pat, "limit": 3})
    ms=(r or {}).get("matches") or []
    if ms:
        addrs[key]=ms[0]["addr"]
        print("resolved", key, ms[0]["addr"], ms[0].get("string","")[:60])

xref_out={}
for name,addr in addrs.items():
    if not addr: continue
    # try xrefs_to and find data_ref
    xr = call("xrefs_to", {"addrs": [addr], "limit": 30})
    fr = call("find", {"type":"data_ref", "targets":[addr], "limit": 30})
    xref_out[name]={"addr":addr, "xrefs_to":xr, "data_ref":fr}
    # summarize
    def countish(x):
        if x is None: return 0
        if isinstance(x, list): return len(x)
        if isinstance(x, dict):
            if "_error" in x: return f"err:{x['_error']}"
            if "xrefs" in x and isinstance(x["xrefs"], list): return len(x["xrefs"])
            # map addr->list
            for vv in x.values():
                if isinstance(vv, list): return len(vv)
            return list(x.keys())[:5]
        return type(x).__name__
    print(f"{name} @{addr} xrefs={countish(xr)} data_ref={countish(fr)}")

(ev/"ida_xrefs_key_ui.json").write_text(json.dumps(xref_out, indent=2, ensure_ascii=False)[:400000], encoding="utf-8")

# Decompile first xref function if any
decomp={}
for name, blob in xref_out.items():
    candidates=[]
    for src in (blob.get("xrefs_to"), blob.get("data_ref")):
        if isinstance(src, list):
            for it in src:
                if isinstance(it, dict):
                    a=it.get("addr") or it.get("from") or it.get("fn_addr") or it.get("function")
                    if a: candidates.append(str(a))
                elif isinstance(it, str) and it.startswith("0x"):
                    candidates.append(it)
        elif isinstance(src, dict):
            for vv in src.values():
                if isinstance(vv, list):
                    for it in vv:
                        if isinstance(it, dict):
                            a=it.get("addr") or it.get("from") or it.get("fn_addr")
                            if a: candidates.append(str(a))
                        elif isinstance(it, str) and ("0x" in it or it.startswith("sub_")):
                            candidates.append(it)
    if candidates:
        addr=candidates[0]
        d=call("decompile", {"addr": addr, "include_addresses": False})
        text = d.get("_text") if isinstance(d, dict) and "_text" in d else (d if isinstance(d,str) else json.dumps(d)[:5000])
        if isinstance(d, dict) and "code" in d: text=d["code"]
        decomp[name]={"fn":addr, "code": (text or "")[:4000]}
        print("decompiled", name, "fn", addr, "len", len(text or ""))
        if len(decomp)>=4: break

(ev/"ida_decompile_samples.json").write_text(json.dumps(decomp, indent=2, ensure_ascii=False), encoding="utf-8")