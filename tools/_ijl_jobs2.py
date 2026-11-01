import io
data = open(r'E:\MXD\BeiDou-ClientV17.7\ijl15.dll','rb').read()

def strings_in(start, end):
    out = []
    off = start
    while off < end:
        if data[off] == 0:
            off += 1
            continue
        e = off
        while e < end and data[e] != 0:
            e += 1
        s = data[off:e].decode('gbk', 'replace')
        out.append((off, s))
        off = e + 1
    return out

lines = []
for off, s in strings_in(0x2c600, 0x2cd00):
    lines.append('%s %s' % (hex(off), s))
with io.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\ijl_jobs_region.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(lines))
print('done')
