import re
data = open(r'E:\MXD\BeiDou-ClientV17.7\ijl15.dll','rb').read()
print('size', len(data))
runs = []
for m in re.finditer(rb'(?:[\x81-\xfe][\x40-\xfe]){2,}', data):
    s = m.group().decode('gbk', 'replace')
    if re.fullmatch(r'[\u4e00-\u9fff（）()]+', s):
        runs.append((m.start(), s))
seen = set()
out = []
for off, s in runs:
    if s not in seen:
        seen.add(s)
        out.append((off, s))
print('unique chinese runs:', len(out))
for off, s in out[:150]:
    print(hex(off), s)
