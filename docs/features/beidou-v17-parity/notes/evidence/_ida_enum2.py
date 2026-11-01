import json, urllib.request, pathlib, re, collections
ev = pathlib.Path(r"E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\evidence")
session = "d3395606-0963-4657-91f1-84f198f097cc"
url = "http://127.0.0.1:13338/mcp"
_id = 200

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
        print("ERR", name, j["error"]); return None
    result = j.get("result", {})
    content = result.get("content")
    if content and isinstance(content, list) and content and "text" in content[0]:
        text = content[0]["text"]
        try:
            return json.loads(text)
        except Exception:
            return text
    return result

# Paginate all UIWindow.img / UI/ paths
all_matches = []
offset = 0
while True:
    r = call("find_regex", {"pattern": r"UI/(UIWindow|CashShop|Login|StatusBar|Basic|DialogImage|GuildBBS|ITC|MapleTV|NameTag|Logo|MapLogin|BuffIcon|tutorial|ChatBalloon|GuildMark|Npt_GuideLine|MapleEzorsia)\.img/[^\x00]{1,80}", "limit": 500, "offset": offset})
    if not r: break
    matches = r.get("matches") or []
    all_matches.extend(matches)
    print(f"page offset={offset} n={r.get('n')} got={len(matches)} done={r.get('cursor',{}).get('done')}")
    if r.get("cursor",{}).get("done") or not matches:
        break
    offset += len(matches)
    if offset > 5000: break

# Also simpler UIWindow.img pattern
offset=0
while True:
    r = call("find_regex", {"pattern": r"UIWindow\.img/[A-Za-z0-9_]+", "limit": 500, "offset": offset})
    if not r: break
    matches = r.get("matches") or []
    all_matches.extend(matches)
    print(f"uiw offset={offset} n={r.get('n')} got={len(matches)} done={r.get('cursor',{}).get('done')}")
    if r.get("cursor",{}).get("done") or not matches:
        break
    offset += len(matches)

# Dedup by string
seen=set(); uniq=[]
for m in all_matches:
    s=m.get("string") or m.get("text") or str(m)
    if s not in seen:
        seen.add(s); uniq.append(m)

(ev/"ida_all_ui_path_strings.json").write_text(json.dumps(uniq, indent=2, ensure_ascii=False), encoding="utf-8")

# Extract top-level UIWindow nodes referenced in exe
nodes=collections.Counter()
for m in uniq:
    s=m.get("string","")
    mm=re.search(r"UIWindow\.img/([A-Za-z0-9_]+)", s)
    if mm: nodes[mm.group(1)] += 1
(ev/"ida_uiwindow_nodes_in_exe.txt").write_text("\n".join(f"{k}\t{v}" for k,v in nodes.most_common()), encoding="utf-8")
print("unique_strings", len(uniq), "nodes", len(nodes))
for k,v in nodes.most_common():
    print(f"  {k}\t{v}")

# xrefs for a sample of key node strings (first addr per node)
sample_addrs=[]
for node in ["Maker","MonsterBook","ViciousHammer","VegaSpell","CashGachapon","PartySearch","Delivery","Trunk","TradingRoom","Messenger","Family","Wedding","Ranking","Incubator","Minigame","RpsGame","MonsterCarnival","EntrustedShop","StoreBank","Alliance","GuildUnion","UserList","SkillEx","SkillMacro","QuestAlarm","PersonalShop","MapleTV","Memo","DualMobGauge","PartyRace","AranSkillGuide","Radio","NewYearsCard","Transform","EnchantSkill","Book"]:
    for m in uniq:
        s=m.get("string","")
        if f"UIWindow.img/{node}" in s or f"/{node}/" in s:
            sample_addrs.append((node, m["addr"], s[:80]))
            break

xref_summary={}
for node, addr, s in sample_addrs:
    xr = call("xrefs_to", {"addrs": addr, "limit": 20})
    count=0; fns=[]
    if isinstance(xr, dict):
        # various shapes
        items = xr.get(addr) or xr.get("xrefs") or xr.get("result") or xr
        if isinstance(items, dict) and "xrefs" in items:
            items = items["xrefs"]
        if isinstance(items, list):
            count=len(items)
            for it in items[:5]:
                if isinstance(it, dict):
                    fns.append(it.get("fn") or it.get("func") or it.get("from") or it.get("addr") or str(it))
                else:
                    fns.append(str(it))
        else:
            fns=[str(items)[:200]]
    xref_summary[node]={"addr":addr,"string":s,"xref_n":count,"sample":fns}
    print(f"xref {node} @{addr} n={count} sample={fns[:2]}")

(ev/"ida_uiwindow_xrefs_sample.json").write_text(json.dumps(xref_summary, indent=2, ensure_ascii=False), encoding="utf-8")