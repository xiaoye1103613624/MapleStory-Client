# -*- coding: utf-8 -*-
"""Parse a Windows minidump (x64): exception stream, faulting thread context, module RVAs.

Read-only. Usage: python _r26_dump.py <dmp>
"""
import struct
import sys

EX_CODES = {
    0xC0000005: 'ACCESS_VIOLATION',
    0xC00000FD: 'STACK_OVERFLOW',
    0xC0000094: 'INTEGER_DIVIDE_BY_ZERO',
    0x80000003: 'BREAKPOINT',
    0xE06D7363: 'CPP_EH (C++ throw)',
    0xC0000409: 'FASTFAIL / stack buffer overrun',
    0x40000015: 'FATAL_APP_EXIT',
    0xC000001D: 'ILLEGAL_INSTRUCTION',
    0xC0000096: 'PRIVILEGED_INSTRUCTION',
    0xC000011A: 'FATAL_IO_ERROR',
}
ARCH = {0: 'x86', 9: 'AMD64', 1101: 'AMD64'}

# AMD64 CONTEXT register offsets
REGS = [('rax', 0x78), ('rcx', 0x80), ('rdx', 0x88), ('rbx', 0x90), ('rsp', 0x98),
        ('rbp', 0xA0), ('rsi', 0xA8), ('rdi', 0xB0), ('r8', 0xB8), ('r9', 0xC0),
        ('r10', 0xC8), ('r11', 0xD0), ('r12', 0xD8), ('r13', 0xE0), ('r14', 0xE8),
        ('r15', 0xF0), ('rip', 0xF8)]


def streams_of(data):
    sig, ver, count, dir_rva = struct.unpack_from('<4sIII', data, 0)
    if sig != b'MDMP':
        raise SystemExit('not a minidump: %r' % sig)
    out = {}
    off = dir_rva
    for _ in range(count):
        stype, size, rva = struct.unpack_from('<III', data, off)
        out.setdefault(stype, []).append((size, rva))
        off += 12
    return out


def u16str(data, rva):
    n, = struct.unpack_from('<I', data, rva)
    return data[rva + 4:rva + 4 + n].decode('utf-16-le', 'replace')


def main(path):
    data = open(path, 'rb').read()
    st = streams_of(data)
    print('== %s (%d bytes, %d streams)' % (path.split('/')[-1], len(data), len(st)))

    if 7 in st:
        _, rva = st[7][0]
        arch, = struct.unpack_from('<H', data, rva)
        print('arch=%d (%s)' % (arch, ARCH.get(arch, '?')))

    mods = []
    if 4 in st:
        _, rva = st[4][0]
        n, = struct.unpack_from('<I', data, rva)
        off = rva + 4
        for _ in range(n):
            base, imgsize, csum, tds, name_rva = struct.unpack_from('<QIIII', data, off)
            cv_rva, cv_size = struct.unpack_from('<II', data, off + 24 + 52)
            mods.append((base, imgsize, u16str(data, name_rva), cv_rva, cv_size))
            off += 108  # sizeof(MINIDUMP_MODULE)
        print('modules=%d' % n)

    def owner(addr):
        for base, sz, name, *_ in mods:
            if base <= addr < base + sz:
                return '%s+0x%x' % (name.split('\\')[-1], addr - base)
        return '0x%x' % addr

    for base, sz, name, cv_rva, cv_size in mods:
        if 'MapleStory' in name and cv_size >= 24:
            cv = data[cv_rva:cv_rva + cv_size]
            if cv[:4] == b'RSDS':
                g = cv[4:20]
                age, = struct.unpack_from('<I', cv, 20)
                d1, d2, d3 = struct.unpack_from('<IHH', g, 0)
                print('MAPLESTORY base=0x%x size=0x%x guid=%08X-%04X-%04X-%s-%s age=%d' %
                      (base, sz, d1, d2, d3, g[8:10].hex().upper(), g[10:16].hex().upper(), age))

    if 6 not in st:
        print('!! NO exception stream (clean exit / non-crash dump)')
        return 0

    _, rva = st[6][0]
    tid, = struct.unpack_from('<I', data, rva)
    code, flags = struct.unpack_from('<II', data, rva + 8)
    nparams, = struct.unpack_from('<I', data, rva + 24)
    info = struct.unpack_from('<15Q', data, rva + 32)
    addr, = struct.unpack_from('<Q', data, rva + 152)
    ctx_rva, ctx_size = struct.unpack_from('<II', data, rva + 160)
    print('EXCEPTION thread=%d code=0x%08X (%s) addr=%s' %
          (tid, code, EX_CODES.get(code, '?'), owner(addr)))
    print('  flags=0x%x nparams=%d' % (flags, nparams))
    if code == 0xC0000005 and nparams >= 3:
        print('  access op=%s fault_va=0x%x instr=%s' %
              ({0: 'READ', 1: 'WRITE', 8: 'DEP-EXEC'}.get(info[0], info[0]), info[1], owner(info[2])))
    elif code == 0xE06D7363 and nparams >= 2:
        print('  throw_obj=0x%x code_ptr=0x%x' % (info[1], info[2]))
    else:
        print('  params=%s' % [hex(p) for p in info[:nparams]])

    ctx = data[ctx_rva:ctx_rva + ctx_size]
    print('  CONTEXT (%d bytes):' % ctx_size)
    for nm, o in REGS:
        if o + 8 <= len(ctx):
            v, = struct.unpack_from('<Q', ctx, o)
            tag = '  <-- ' + owner(v) if 0x1000 < v < 0x7FFFFFFFFFFF else ''
            print('    %-4s = 0x%016x%s' % (nm, v, tag))

    # stack scan: first 40 return-address-looking slots above rsp
    _, stack_desc_rva = st[3][0] if 3 in st else (0, 0)
    if 5 in st:  # MemoryList
        mem = []
        _, mrva = st[5][0]
        n, = struct.unpack_from('<I', data, mrva)
        off = mrva + 4
        for _ in range(n):
            a, msz, mrva2 = struct.unpack_from('<QII', data, off)
            mem.append((a, msz, mrva2))
            off += 16

        def read64(a):
            for base_a, msz, mrva2 in mem:
                if base_a <= a <= base_a + msz - 8:
                    return struct.unpack_from('<Q', data, mrva2 + (a - base_a))[0]
            return None

        for nm, o in REGS:
            if nm != 'rsp':
                continue
            sp, = struct.unpack_from('<Q', ctx, o)
            hits = []
            for i in range(0, 512, 8):
                v = read64(sp + i)
                if v is None:
                    continue
                for base, sz, name, *_ in mods:
                    if 'MapleStory' in name and base <= v < base + sz:
                        hits.append('sp+0x%03x -> %s+0x%x' % (i, name.split('\\')[-1], v - base))
                        break
            print('  code on stack (first 20):')
            for h in hits[:20]:
                print('    ' + h)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
