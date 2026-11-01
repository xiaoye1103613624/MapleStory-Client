# -*- coding: utf-8 -*-
"""v95(send: 582 ctor sites / 238 ops) <-> s9 opcode candidate mapping (v2)."""
import json, re, collections

MD = r"E:\project\MapleStory-Client\docs\features\port-095\notes\opcode-v95.md"
md = open(MD, encoding="utf-8").read().split("## 5. 发送")[0]

ROW = re.compile(r"^\|\s*(\d+)(?:\s*[–-]\s*(\d+))?\s*\|\s*(.+?)\s*\|\s*$", re.M)
rng, exact = {}, {}
for m in ROW.finditer(md):
    lo = int(m.group(1)); hi = int(m.group(2)) if m.group(2) else lo
    h = m.group(3).strip().strip("`")
    if h.startswith("|") or h.startswith("opcode") or h.startswith("子") or h == "-":
        continue
    if hi - lo > 40:
        continue
    if lo == hi: exact[lo] = h
    else:
        for n in range(lo, hi + 1): rng.setdefault(n, h)
v95recv = dict(rng); v95recv.update(exact)
v95recv = {k: v for k, v in v95recv.items() if not v.startswith("[case]")}

sites = json.load(open("send_sites_all.json", encoding="utf-8"))
def pretty(n):
    m = re.match(r"\?([^@]+)@([^@]+)@@", n)
    return (m.group(2) + "::" + m.group(1)) if m else n
v95send = collections.defaultdict(set)
for s in sites:
    if s["op"] is not None: v95send[s["op"]].add(pretty(s["name"]))

s9 = json.load(open("s9_opcodes.json", encoding="utf-8"))
STOP = {"on","send","request","result","res","packet","handler","man","dlg","pool","of","by","the","and",
        "wvs","context","stage","ui","c","i","a","to","for","user","local","remote","common","get","set",
        "process","try","doing","check","update","init","new","base","widget","wnd","tab","ctrl","item",
        "items","take","use","load","save","change","ask","show"}

def toks(s):
    s = re.sub(r"^(?:CWvsContext|CField|CUserLocal|CUserRemote|CUserCommon|CUser|CMobPool|CMob|CNpcPool|"
               r"CNpc|CUI\w+|C\w+Manager|C\w+Man|C\w+Dlg|C\w+Pool|C\w+Tab|C\w+Wnd|C\w+)::", "", s)
    s = re.sub(r"^(?:On|Send|Try|Check|Update|Process|Do)", "", s)
    parts = re.findall(r"[A-Z]+(?![a-z])|[A-Z][a-z0-9]*|[a-z0-9]+", s.replace("_", " "))
    return {p.lower() for p in parts if p.lower() not in STOP}

def dice(a, b):
    return 0.0 if not a or not b else 2.0*len(a & b)/(len(a)+len(b))

send_c = {n: {"value": v["value"], "comment": v["comment"], "toks": toks(n)} for n, v in s9["send"].items()}
recv_c = {n: {"value": v["value"], "comment": v["comment"], "handler": v.get("handler",""),
              "toks": toks(n) | toks(v.get("handler",""))} for n, v in s9["recv"].items()}

out = {"recv": [], "send": []}
for op in sorted(v95recv):
    t = toks(v95recv[op])
    cand = sorted(((dice(t, c["toks"]), n) for n, c in send_c.items()), reverse=True)[:4]
    out["recv"].append({"v95": op, "handler": v95recv[op],
                        "cand": [{"s9": n, "v": send_c[n]["value"], "score": round(s,2)}
                                 for s, n in cand if s >= 0.5]})
for op in sorted(v95send):
    fns = sorted(v95send[op]); t = set()
    for n in fns: t |= toks(n)
    cand = sorted(((dice(t, c["toks"]), n) for n, c in recv_c.items()), reverse=True)[:4]
    out["send"].append({"v95": op, "fns": fns,
                        "cand": [{"s9": n, "v": recv_c[n]["value"], "score": round(s,2),
                                  "handler": recv_c[n]["handler"]} for s, n in cand if s >= 0.5]})
json.dump(out, open("map_candidates2.json", "w", encoding="utf-8"), ensure_ascii=False, indent=1)
print("recv rows=%d exact=%d send rows=%d exact=%d" % (
    len(out["recv"]), sum(1 for r in out["recv"] if r["cand"] and r["cand"][0]["score"] == 1.0),
    len(out["send"]), sum(1 for r in out["send"] if r["cand"] and r["cand"][0]["score"] == 1.0)))
