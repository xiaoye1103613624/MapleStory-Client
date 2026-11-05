import json, sys, urllib.request, os, re

def fetch(url):
    return urllib.request.urlopen(url, timeout=120).read().decode("utf-8", "replace")

if __name__ == "__main__":
    url = sys.argv[1]
    out = sys.argv[2]
    raw = fetch(url)
    try:
        d = json.loads(raw)
    except Exception:
        open(out, "w", encoding="utf-8").write(raw)
        print("saved raw", out, len(raw))
        sys.exit(0)
    # shape unknown; dump code if present
    def find_code(o):
        if isinstance(o, dict):
            if "code" in o and isinstance(o["code"], str):
                return o
            for v in o.values():
                r = find_code(v)
                if r:
                    return r
        if isinstance(o, list):
            for v in o:
                r = find_code(v)
                if r:
                    return r
        return None
    c = find_code(d)
    if c:
        with open(out, "w", encoding="utf-8") as f:
            f.write(c["code"])
            f.write("\n/* REFS */\n")
            for x in c.get("refs") or []:
                f.write("%s %s %s\n" % (x.get("addr"), x.get("name"), (x.get("string") or "")[:90]))
        print("saved", out, "code len", len(c["code"]))
    else:
        open(out, "w", encoding="utf-8").write(json.dumps(d, ensure_ascii=False, indent=1))
        print("saved json", out)
