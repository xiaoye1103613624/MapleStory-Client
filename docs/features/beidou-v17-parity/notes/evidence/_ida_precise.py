import json, urllib.request, pathlib, re
ev = pathlib.Path(r"E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\evidence")
session = "d3395606-0963-4657-91f1-84f198f097cc"
url = "http://127.0.0.1:13338/mcp"
_id = 800

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

nodes = [ln.strip() for ln in open(r"E:\project\MapleStory-Client\docs\features\v83-hud-quest\evidence\UIWindow.img.top-level.txt", encoding="utf-8") if ln.strip()]

# Exact path find: UIWindow.img/<node> and UI/UIWindow.img/<node>
exact = {}
for i in range(0, len(nodes), 15):
    batch = nodes[i:i+15]
    targets = []
    for n in batch:
        targets += [f"UIWindow.img/{n}", f"UI/UIWindow.img/{n}", f"UIWindow.img/{n}/", f"/{n}/backgrnd"]
    r = call("find", {"type":"string", "targets": targets, "limit": 3})
    # r is list of per-target results
    if isinstance(r, list):
        for item in r:
            # shapes vary
            t = item.get("target") if isinstance(item, dict) else None
            matches = item.get("matches") if isinstance(item, dict) else None
            if matches is None and isinstance(item, dict):
                # maybe {query, n, matches}
                matches = item.get("results") or item.get("hits")
            if isinstance(item, dict) and matches:
                key = t or item.get("query") or str(item)[:40]
                exact[str(key)] = matches
    print(f"exact batch {i} list_len={len(r) if isinstance(r,list) else type(r)}")

(ev/"ida_exact_path_find.json").write_text(json.dumps(exact, indent=2, ensure_ascii=False)[:200000], encoding="utf-8")
print("exact_keys", len(exact))

# go* UI action strings (classic MapleStory UI script hooks)
go = call("find_regex", {"pattern": r"go[A-Z][A-Za-z0-9_]{2,40}", "limit": 500})
go_matches = go.get("matches", []) if isinstance(go, dict) else []
go_strs = sorted({m.get("string") for m in go_matches if m.get("string","").startswith("go")})
(ev/"ida_go_actions.txt").write_text("\n".join(go_strs), encoding="utf-8")
print("go_actions", len(go_strs))
for s in go_strs[:80]:
    print(" ", s)

# CNM* / CWvs* interesting named exports/strings for systems
cnm = call("find_regex", {"pattern": r"CNM[A-Za-z]{4,40}", "limit": 200})
# filter messenger/guild related noise from ngs
# Better: game UI related words in strings
sys_pat = call("find_regex", {"pattern": r"(Trunk|TradingRoom|PersonalShop|EntrustedShop|StoreBank|MemberShop|Messenger|FamilyTree|Family|Wedding|MonsterCarnival|AriantMatch|Minigame|RpsGame|SpeedQuiz|InitialQuiz|Incubator|Parcel|Duey|Ranking|GuildBBS|CashTradingRoom|HiredMerchant|Fredrick|QuestAlarm|SkillMacro|SkillEx|PartySearch|RemoteGachapon|KarmaScissors|ItemProtector|TreasureBox|SlideMenu|EnergyBar|TemporaryStat|MobGage|createCygnus|CharacterClone|classMatch|muruengRaid|MapleEvent|MonsterKilling|AdminClaim|MateMessage|FadeYesNo|GameMenu|ShortCut|SoftKeyboard|FloatNotice|Megaphone|AvatarMegaphone|ItemMegaphone|ArtMegaphone|MultiLine|MinigameTable|ExceptionItemSearch|itemSearch|Coupon|MacroSkill|SkillUp|GMBoard)", "limit": 500})
sys_matches = sys_pat.get("matches", []) if isinstance(sys_pat, dict) else []
# group by which keyword
from collections import defaultdict
by = defaultdict(list)
keys = ["Trunk","TradingRoom","PersonalShop","EntrustedShop","StoreBank","MemberShop","Messenger","FamilyTree","Family","Wedding","MonsterCarnival","AriantMatch","Minigame","RpsGame","SpeedQuiz","InitialQuiz","Incubator","Parcel","Duey","Ranking","GuildBBS","CashTradingRoom","HiredMerchant","Fredrick","QuestAlarm","SkillMacro","SkillEx","PartySearch","RemoteGachapon","KarmaScissors","ItemProtector","TreasureBox","SlideMenu","EnergyBar","TemporaryStat","MobGage","createCygnus","CharacterClone","classMatch","muruengRaid","MapleEvent","MonsterKilling","AdminClaim","MateMessage","FadeYesNo","GameMenu","ShortCut","SoftKeyboard","FloatNotice","Megaphone","AvatarMegaphone","ItemMegaphone","ArtMegaphone","MultiLine","MinigameTable","ExceptionItemSearch","itemSearch","Coupon","MacroSkill","SkillUp","GMBoard","MonsterBook","Maker","ViciousHammer","VegaSpell","CashGachapon","Delivery","NewYearsCard","Transform","EnchantSkill","AranSkillGuide","PartyRace","DualMobGauge","Radio","UtilDlgEx","UserList","UserInfo","KeyConfig","GameOpt","SysOpt","Channel","WorldMap","MiniMap","Equip","Item","Stat","Skill","Shop","Quest","Notice","Memo","Alliance","Guild"]
for m in sys_matches:
    s = m.get("string","")
    for k in keys:
        if k in s:
            by[k].append(s)
            break
(ev/"ida_system_keyword_strings.json").write_text(json.dumps({k:v[:8] for k,v in sorted(by.items())}, indent=2, ensure_ascii=False), encoding="utf-8")
print("system_keywords_hit", len(by))
for k,v in sorted(by.items(), key=lambda x: -len(x[1])):
    print(f"  {k}\t{len(v)}\t{v[0][:70]}")