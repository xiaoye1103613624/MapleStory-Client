# -*- coding: utf-8 -*-
import json, urllib.request, uuid, ast
from pathlib import Path

CODE = r"""
def _scan():
    import idautils, ida_funcs
    want = [
        'CLogin','CUICharSelect','CUIStatusBar','CUIMiniMap','CWnd','CWndMan',
        'CUIUserList','CPet','CCashShop','CUISkill','CNpc','CField','CMapLoadable',
        'CDropPool','CQuestMan','CUIQuestInfo','COutPacket','CInPacket','CClientSocket',
        'CAESCipher','CUserLocal','CUser','CAvatar','CItemInfo','CUIItem','CUIMonsterBook',
        'CUIMedalQuestInfo','CTemporaryStatView','CUIMessenger','CWvsContext'
    ]
    out = {c: [] for c in want}
    for ea in idautils.Functions():
        name = ida_funcs.get_func_name(ea) or ''
        if not name.startswith('?'):
            continue
        for c in want:
            if ('@%s@@' % c) in name or name.startswith('??0%s@@' % c) or name.startswith('??1%s@@' % c):
                out[c].append({'ea': hex(ea), 'name': name[:160]})
                break
    for c in list(out):
        sample = out[c][:25]
        out[c] = {'count_sampled': len(sample), 'sample': sample}
    return out
_scan()
"""


def call_port(port, name, args=None, timeout=600):
    session = str(uuid.uuid4())
    init = {
        "jsonrpc": "2.0",
        "id": 0,
        "method": "initialize",
        "params": {
            "protocolVersion": "2025-06-18",
            "capabilities": {},
            "clientInfo": {"name": "probe", "version": "1"},
        },
    }
    req = urllib.request.Request(
        "http://127.0.0.1:%d/mcp" % port,
        data=json.dumps(init).encode(),
        headers={
            "Content-Type": "application/json",
            "Accept": "application/json, text/event-stream",
            "Mcp-Session-Id": session,
        },
        method="POST",
    )
    try:
        with urllib.request.urlopen(req, timeout=30) as r:
            r.read()
    except Exception:
        pass
    payload = {
        "jsonrpc": "2.0",
        "id": 1,
        "method": "tools/call",
        "params": {"name": name, "arguments": args or {}},
    }
    req = urllib.request.Request(
        "http://127.0.0.1:%d/mcp" % port,
        data=json.dumps(payload).encode(),
        headers={
            "Content-Type": "application/json",
            "Accept": "application/json, text/event-stream",
            "Mcp-Session-Id": session,
        },
        method="POST",
    )
    with urllib.request.urlopen(req, timeout=timeout) as r:
        raw = r.read().decode("utf-8", "replace")
    for line in raw.splitlines():
        if line.startswith("data:"):
            raw = line[5:].strip()
            break
    obj = json.loads(raw)
    res = obj.get("result", obj)
    texts = []
    for item in res.get("content", []) or []:
        if item.get("text"):
            texts.append(item["text"])
    blob = "\n".join(texts)
    for tok in blob.split():
        if tok.startswith("http://127.0.0.1") and tok.endswith(".json"):
            with urllib.request.urlopen(tok, timeout=timeout) as r:
                return r.read().decode("utf-8", "replace")
    return blob


raw = call_port(13337, "py_eval", {"code": CODE, "new_locals": True})
j = json.loads(raw)
result = j.get("result", j)
if isinstance(result, str):
    result = ast.literal_eval(result)

base = Path(
    r"E:\project\MapleStory-Client\docs\features\ida-v83-083-v83-i64-inventory-2026-11\evidence"
)
(base / "key-class-method-eas-13337.json").write_text(
    json.dumps(result, ensure_ascii=False, indent=2), encoding="utf-8"
)

feat_map = {
    "login": ["CLogin", "CUICharSelect"],
    "statusbar_hud": ["CUIStatusBar", "CUIMiniMap"],
    "corewindow": ["CWnd", "CWndMan"],
    "social": ["CUIUserList", "CUIMessenger"],
    "pet": ["CPet"],
    "activity": ["CTemporaryStatView"],
    "cashshop": ["CCashShop"],
    "skill": ["CUISkill"],
    "npc": ["CNpc"],
    "map": ["CField", "CMapLoadable"],
    "drop": ["CDropPool"],
    "quest": ["CQuestMan", "CUIQuestInfo"],
    "packet": ["COutPacket", "CInPacket", "CClientSocket"],
    "crypto": ["CAESCipher"],
    "character": ["CUserLocal", "CUser", "CAvatar"],
    "inventory": ["CItemInfo", "CUIItem"],
    "monsterbook": ["CUIMonsterBook"],
    "medal": ["CUIMedalQuestInfo"],
}
for feat, classes in feat_map.items():
    p = base / ("feature-%s.json" % feat)
    data = json.loads(p.read_text(encoding="utf-8"))
    data["key_method_eas"] = {c: result.get(c) for c in classes}
    p.write_text(json.dumps(data, ensure_ascii=False, indent=2), encoding="utf-8")

print("ok CLogin", result.get("CLogin", {}).get("count_sampled"))
