import struct, io, re
data = open(r'E:\MXD\BeiDou-ClientV17.7\ijl15.dll','rb').read()

def cstr(off):
    end = off
    while end < len(data) and data[end] != 0:
        end += 1
    return data[off:end].decode('gbk', 'replace')

def read_ptr(off):
    return struct.unpack_from('<Q', data, off)[0]

# imagebase guess: find PE header
pe = struct.unpack_from('<I', data, 0x3c)[0]
imgbase = struct.unpack_from('<Q', data, pe + 0x18 + 0x18)[0] if struct.unpack_from('<H', data, pe + 0x18)[0] == 0x20b else struct.unpack_from('<I', data, pe + 0x18 + 0x1c)[0]
print('PE at', hex(pe), 'imagebase', hex(imgbase))

lines = []
# dump pointer table 0x2c840..0x2c9a0 (file offsets; RVA==file offset assumption check via pointers)
for off in range(0x2c840, 0x2c9a0, 8):
    p = read_ptr(off)
    if p == 0:
        lines.append('%s -> 0' % hex(off))
        continue
    # try RVA and VA interpretations
    s_rva = cstr(p) if 0 < p < len(data) else None
    s_va = cstr(p - imgbase) if 0 < p - imgbase < len(data) else None
    lines.append('%s -> ptr=%s rva_str=%r va_str=%r' % (hex(off), hex(p), s_rva, s_va))

with io.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\ijl_jobtable.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(lines))
print('done')
