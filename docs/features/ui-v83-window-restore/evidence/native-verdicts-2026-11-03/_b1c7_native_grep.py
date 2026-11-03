"""B1c-7 §4.1 取证 v2：'>>' / '>> ' 是不是原生独立串。

方法：从 exe 抽两类「真串」——
  (1) ASCII：nul 结尾、>=2 个可打印字符、且起始位置前一个字节是 0（串边界）；
  (2) UTF-16LE：00 00 结尾、全偶数位可打印、前驱 00 00。
再查其中有没有整串等于/包含 '>>' 的。附 0x73BAD0 的 hexdump 判对齐。
"""
import io, sys, re
sys.stdout.reconfigure(encoding='utf-8', errors='replace')

d = io.open(r'E:\project\_ida_beidou\BeiDou.exe', 'rb').read()
POOL = range(0x6EC000, 0x74A000)

print('--- hexdump 0x73BAC0-0x73BAF0 ---')
seg = d[0x73BAC0:0x73BAF0]
print(' '.join('%02X' % b for b in seg))

# ASCII 真串
ascii_strs = []
i = 0x6EC000
while i < 0x74A000:
    if d[i - 1] == 0 or i == 0x6EC000:
        m = re.match(rb'[\x20-\x7e]{2,}', d[i:0x74A000])
        if m and d[i + m.end():i + m.end() + 1] in (b'', b'\x00'):
            ascii_strs.append((i, m.group(0).decode('ascii')))
            i += m.end() + 1
            continue
    i += 1
hits = [(o, s) for o, s in ascii_strs if '>>' in s]
print('ASCII 串总数=%d  含 ">>" 的=%d' % (len(ascii_strs), len(hits)))
for o, s in hits[:10]:
    print('  0x%X %r' % (o, s[:80]))

# UTF-16LE 真串
u16_strs = []
i = 0x6EC000
while i < 0x74A000 - 2:
    if d[i] != 0 and d[i + 1] == 0 and d[i - 1] == 0 and d[i - 2] == 0 if i >= 2 else False:
        pass
    # 起点 = 前一对为 00 00
    if i >= 2 and d[i - 2:i] == b'\x00\x00' and d[i] != 0 and d[i + 1] == 0:
        m = re.match(rb'(\x20-\x7e\x00){2,}', d[i:0x74A000]) if False else \
            re.match(rb'(?:[\x20-\x7e]\x00){2,}', d[i:0x74A000])
        if m and (i + m.end() + 2 > len(d) or d[i + m.end():i + m.end() + 2] in (b'', b'\x00\x00')):
            try:
                u16_strs.append((i, m.group(0).decode('utf-16-le')))
            except Exception:
                pass
            i += m.end() + 2
            continue
    i += 2
h2 = [(o, s) for o, s in u16_strs if '>>' in s or '＞＞' in s]
print('UTF-16 串总数=%d  含 ">>" 的=%d' % (len(u16_strs), len(h2)))
for o, s in h2[:10]:
    print('  0x%X %r' % (o, s[:60]))
