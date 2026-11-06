import idautils, re

S = list(idautils.Strings())
pat = re.compile(r"soft|key|case|num|tab|del|next|ok|cancel|backgrnd", re.I)
hits = sorted({str(s) for s in S if pat.search(str(s))})
print("MATCH", len(hits))
for h in hits[:120]:
    print("  ", h)
