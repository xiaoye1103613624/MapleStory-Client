import json
from pathlib import Path

ev = Path(__file__).resolve().parent
raw = (ev / "rea_decompile_MakerUI.json").read_text(encoding="utf-8-sig")
j = json.loads(raw)


def walk(o):
    if isinstance(o, dict):
        code = o.get("code")
        if isinstance(code, str) and "sub_822350" in code:
            return code
        for v in o.values():
            r = walk(v)
            if r:
                return r
    elif isinstance(o, list):
        for v in o:
            r = walk(v)
            if r:
                return r
    return None


code = walk(j) or "MISSING"
(ev / "rea_MakerUI_sub_822350.c").write_text(code, encoding="utf-8")
print("code_len", len(code))
