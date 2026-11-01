import json, urllib.request, pathlib
ev = pathlib.Path(r"E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\evidence")
session = "d3395606-0963-4657-91f1-84f198f097cc"
url = "http://127.0.0.1:13338/mcp"
_id = 500

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
        except Exception: return {"_text": text}
    return result

# Read top-level nodes
nodes = [ln.strip() for ln in open(r"E:\project\MapleStory-Client\docs\features\v83-hud-quest\evidence\UIWindow.img.top-level.txt", encoding="utf-8") if ln.strip()]
print("nodes", len(nodes))

# Batch find string presence for each node name (exact / as path fragment)
# Use find type=string with batches
results = {}
batch_size = 20
for i in range(0, len(nodes), batch_size):
    batch = nodes[i:i+batch_size]
    # targets as path forms and bare names
    targets = []
    for n in batch:
        targets.append(n)
        targets.append(f"UIWindow.img/{n}")
        targets.append(f"UI/UIWindow.img/{n}")
    r = call("find", {"type":"string", "targets": targets, "limit": 5})
    results[f"batch_{i}"] = r
    print(f"batch {i}: keys={list(r)[:8] if isinstance(r,dict) else type(r)}")

(ev/"ida_find_node_strings_raw.json").write_text(json.dumps(results, indent=2, ensure_ascii=False)[:500000], encoding="utf-8")

# Also find_regex for bare notable names with word boundaries-ish
presence = {}
for n in nodes:
    # skip tiny / generic
    if n in ("Reset","Title","Book","Radio","plant","raise","counsel","Macro","Notice","Report","Admin","Claim"):
        # still check
        pass
    r = call("find_regex", {"pattern": n.replace(".", r"\."), "limit": 5})
    matches = []
    if isinstance(r, dict):
        matches = r.get("matches") or []
    # filter: string equals node or contains UIWindow.img/node or ends with /node
    kept=[]
    for m in matches:
        s = m.get("string","")
        if s == n or f"/{n}" in s or f"UIWindow.img/{n}" in s or s.startswith(n+"/"):
            kept.append(s)
    presence[n] = {"hit": len(kept)>0 or (len(matches)>0 and any(n in m.get("string","") for m in matches)), "samples": [m.get("string") for m in matches[:3]], "kept": kept[:5]}
    if presence[n]["hit"] or kept:
        print(f"HIT {n}: {presence[n]['samples'][:2]}")

(ev/"ida_node_string_presence.json").write_text(json.dumps(presence, indent=2, ensure_ascii=False), encoding="utf-8")
hit_nodes = [n for n,v in presence.items() if v["hit"] or v["kept"]]
print("HIT_COUNT", len(hit_nodes))
print("\n".join(hit_nodes))