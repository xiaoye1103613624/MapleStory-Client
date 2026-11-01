import re
from pathlib import Path

sc = Path(r"E:\project\MapleStory-Client\IO\UITypes\UIMiscShells.cpp").read_text(
    encoding="utf-8", errors="replace"
)

def extract(name):
    # crude: from void Name::send_key to next void at column 0 that is not nested
    pat = rf"void {name}::send_key\(.*?\n(?=void |\Z)"
    m = re.search(pat, sc, re.S)
    return m.group(0) if m else ""

def keys_in(body):
    keys = set()
    for x in re.findall(r"case\s+'([^'\\]|\\.)'", body):
        keys.add(x[-1] if x.startswith("\\") else x)
    for x in re.findall(r"keycode\s*==\s*'([^'\\]|\\.)'", body):
        keys.add(x[-1] if x.startswith("\\") else x)
    return keys

letters = set("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
digits = set("0123456789")
syms = set("`~!@#$%^&*()_+-=[]{}\\|;:'\",.<>/?")

for name in ("UIShortCut", "UIGameMenu"):
    body = extract(name)
    keys = keys_in(body)
    used_letters = {k.upper() for k in keys if k.isalpha()}
    used_digits = {k for k in keys if k.isdigit()}
    used_syms = {k for k in keys if not k.isalnum()}
    print(f"=== {name} ===")
    print("letters FREE:", "".join(sorted(letters - used_letters)) or "(none)")
    print("digits FREE:", "".join(sorted(digits - used_digits)) or "(none)")
    print("syms FREE:", "".join(sorted(syms - used_syms)) or "(none)")
    print("letters used:", "".join(sorted(used_letters)))
    print("syms used:", "".join(sorted(used_syms)))
    print()

# also check ENTER_MTS / Owl / BBS presence
for tok in ("EnterMts", "RemoteStore", "WorldMapPlayers", "OpenItemUi", "WaterOfLife",
            "Bbs", "OwlAction", "OwlWarp", "ENTER_MTS", "BBS_OPERATION", "OWL_"):
    hits = [i+1 for i,l in enumerate(sc.splitlines()) if tok in l]
    print(f"{tok}: lines {hits[:8]}{'...' if len(hits)>8 else ''} count={len(hits)}")
