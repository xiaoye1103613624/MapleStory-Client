import json, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from _mcp2 import tool

# usage: python dec.py <addr> [outfile]
addr = sys.argv[1]
out = sys.argv[2] if len(sys.argv) > 2 else "tools/_out/dec_%s.txt" % addr.lstrip("0x0")
r = tool("decompile", {"addr": addr})
if isinstance(r, dict):
    code = r.get("code") or json.dumps(r, ensure_ascii=False, indent=1)
    refs = r.get("refs") or []
else:
    code = str(r)
    refs = []
with open(out, "w", encoding="utf-8") as f:
    f.write(code)
    f.write("\n/* REFS */\n")
    for x in refs:
        f.write("%s %s %s\n" % (x.get("addr"), x.get("name"), (x.get("string") or "")[:80]))
print(out, "len", len(code))
