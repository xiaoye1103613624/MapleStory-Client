import json, urllib.request, pathlib
ev = pathlib.Path(r"E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\evidence")
session = "d3395606-0963-4657-91f1-84f198f097cc"
url = "http://127.0.0.1:13338/mcp"
_id = 100

def call(name, arguments=None, timeout=120):
    global _id
    _id += 1
    body = {
        "jsonrpc": "2.0",
        "id": _id,
        "method": "tools/call",
        "params": {"name": name, "arguments": arguments or {}},
    }
    data = json.dumps(body).encode("utf-8")
    req = urllib.request.Request(url, data=data, headers={
        "Content-Type": "application/json",
        "Accept": "application/json",
        "Mcp-Session-Id": session,
    })
    with urllib.request.urlopen(req, timeout=timeout) as resp:
        raw = resp.read().decode("utf-8", "replace")
    (ev / f"ida_{name}.json").write_text(raw, encoding="utf-8")
    j = json.loads(raw)
    if "error" in j:
        print(f"ERR {name}: {j['error']}")
        return j
    # unwrap content text if present
    result = j.get("result", {})
    content = result.get("content")
    if content and isinstance(content, list) and content and "text" in content[0]:
        text = content[0]["text"]
        try:
            parsed = json.loads(text)
            (ev / f"ida_{name}_parsed.json").write_text(json.dumps(parsed, indent=2, ensure_ascii=False), encoding="utf-8")
            print(f"OK {name}: parsed keys={list(parsed)[:12] if isinstance(parsed, dict) else type(parsed)} size={len(text)}")
            return parsed
        except Exception:
            (ev / f"ida_{name}_text.txt").write_text(text, encoding="utf-8")
            print(f"OK {name}: text len={len(text)}")
            return text
    print(f"OK {name}: result type={type(result)}")
    return result

# 1) survey
call("survey_binary", {})

# 2) find_regex UIWindow
call("find_regex", {"pattern": r"UIWindow\.img/[A-Za-z0-9_]+", "limit": 300})

# 3) CUI*
call("find_regex", {"pattern": r"CUI[A-Za-z0-9_]+", "limit": 300})

# 4) list funcs matching UI
call("func_query", {"query": "CUI", "limit": 100})

# 5) find_regex for major systems as path fragments
for pat, tag in [
    (r"UIWindow\.img/(Trunk|TradingRoom|PersonalShop|Messenger|Family|Wedding|MonsterBook|Maker|Ranking)", "systems1"),
    (r"UIWindow\.img/(ViciousHammer|VegaSpell|CashGachapon|Incubator|Delivery|PartySearch|EntrustedShop)", "systems2"),
    (r"UIWindow\.img/(Minigame|RpsGame|SpeedQuiz|InitialQuiz|MonsterCarnival|AriantMatch|MapleTV)", "systems3"),
]:
    call("find_regex", {"pattern": pat, "limit": 100})
    # rename last file
    src = ev / "ida_find_regex_parsed.json"
    if src.exists():
        src.rename(ev / f"ida_find_{tag}.json")