import re, io
data = open(r'E:\MXD\BeiDou-ClientV17.7\ijl15.dll','rb').read()
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
with io.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\ijl_strings.txt', 'w', encoding='utf-8') as f:
    f.write('unique chinese runs: %d\n' % len(out))
    for off, s in out:
        f.write('%s %s\n' % (hex(off), s))
print('done', len(out))
