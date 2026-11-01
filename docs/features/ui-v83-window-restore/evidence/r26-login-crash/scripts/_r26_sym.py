# -*- coding: utf-8 -*-
"""Symbolize a Windows x64 minidump against the build_check PDB.

Field order used (verified against raw bytes):
  MINIDUMP_EXCEPTION_STREAM: u32 tid, u32 pad, EXCEPTION_RECORD64, LOCDESC ctx
  EXCEPTION_RECORD64: u32 code, u32 flags, u64 rec, u64 addr, u32 nparams, u32 pad, u64 info[15]

Usage: python _r26_sym.py <dmp>
"""
import ctypes
import os
import struct
import sys
from ctypes import wintypes

EX_CODES = {
    0xC0000005: 'ACCESS_VIOLATION', 0xC00000FD: 'STACK_OVERFLOW',
    0xE06D7363: 'CPP_EH (C++ throw)', 0xC0000409: 'FASTFAIL',
    0xC0000374: 'STATUS_HEAP_CORRUPTION', 0x80000003: 'BREAKPOINT',
    0xC0000094: 'INT_DIV_BY_ZERO', 0xC000001D: 'ILLEGAL_INSTRUCTION',
}
REGS = [('rax', 0x78), ('rcx', 0x80), ('rdx', 0x88), ('rbx', 0x90), ('rsp', 0x98),
        ('rbp', 0xA0), ('rsi', 0xA8), ('rdi', 0xB0), ('r8', 0xB8), ('r9', 0xC0),
        ('r10', 0xC8), ('r11', 0xD0), ('r12', 0xD8), ('r13', 0xE0), ('r14', 0xE8),
        ('r15', 0xF0), ('rip', 0xF8)]

EXE = r'E:\project\MapleStory-Client\build_check\MapleStory.exe'

SYMOPT_UNDNAME = 0x2
SYMOPT_DEFERRED_LOADS = 0x4
SYMOPT_LOAD_LINES = 0x10
SYMOPT_EXACT_SYMBOLS = 0x200
SYMOPT_FAIL_CRITICAL_ERRORS = 0x2000


class IMAGEHLP_LINE64(ctypes.Structure):
    _fields_ = [('SizeOfStruct', wintypes.DWORD), ('Key', ctypes.c_void_p),
                ('LineNumber', wintypes.DWORD), ('FileName', ctypes.c_char_p),
                ('Offset', wintypes.DWORD)]


class SYMBOL_INFO(ctypes.Structure):
    _pack_ = 8
    _fields_ = [('SizeOfStruct', wintypes.DWORD), ('Index', wintypes.DWORD),
                ('Reserved', ctypes.c_ulonglong * 2), ('Size', wintypes.DWORD),
                ('ModBase', wintypes.DWORD), ('Flags', wintypes.DWORD),
                ('Value', ctypes.c_ulonglong), ('Address', ctypes.c_ulonglong),
                ('NameType', wintypes.DWORD), ('Name', ctypes.c_wchar * 1024)]


class DBHH:
    def __init__(self, exe_path):
        self.d = ctypes.WinDLL('dbghelp.dll')
        self.k = ctypes.WinDLL('kernel32.dll')
        self.d.SymSetOptions(SYMOPT_UNDNAME | SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS
                             | SYMOPT_FAIL_CRITICAL_ERRORS)
        self.hp = self.k.GetCurrentProcess()
        if not self.d.SymInitializeW(self.hp, os.path.dirname(exe_path), False):
            raise SystemExit('SymInitializeW failed %d' % ctypes.GetLastError())
        self.loaded = {}
        self.exe_path = exe_path

    def load(self, base, size, img_path):
        if base in self.loaded:
            return True
        h = self.d.SymLoadModuleExW(self.hp, None, img_path, None,
                                    ctypes.c_ulonglong(base), size, None, 0)
        if h:
            self.loaded[base] = h
        return bool(h)

    def line(self, base, addr):
        li = IMAGEHLP_LINE64()
        li.SizeOfStruct = ctypes.sizeof(IMAGEHLP_LINE64)
        disp = wintypes.DWORD()
        if not self.d.SymGetLineFromAddr64(self.hp, ctypes.c_ulonglong(addr),
                                           ctypes.byref(disp), ctypes.byref(li)):
            return None
        fn = li.FileName.decode('mbcs', 'replace') if li.FileName else '?'
        return '%s:%d (+%d)' % (os.path.basename(fn), li.LineNumber, disp.value)

    def sym(self, base, addr):
        si = SYMBOL_INFO()
        si.SizeOfStruct = 64
        disp = ctypes.c_ulonglong(0)
        if not self.d.SymFromAddrW(self.hp, ctypes.c_ulonglong(addr),
                                   ctypes.byref(disp), ctypes.byref(si)):
            return None
        return u'%s+0x%x' % (si.Name, disp.value)


def main(dmp):
    data = open(dmp, 'rb').read()
    sig, ver, count, dir_rva = struct.unpack_from('<4sIII', data, 0)
    st = {}
    off = dir_rva
    for _ in range(count):
        t, sz, rva = struct.unpack_from('<III', data, off)
        st.setdefault(t, (sz, rva))
        off += 12

    # modules
    mods = []
    if 4 in st:
        sz, rva = st[4]
        n, = struct.unpack_from('<I', data, rva)
        off = rva + 4
        for _ in range(n):
            base, isz = struct.unpack_from('<QI', data, off)
            name_rva, = struct.unpack_from('<I', data, off + 20)
            nl, = struct.unpack_from('<I', data, name_rva)
            name = data[name_rva + 4:name_rva + 4 + nl].decode('utf-16-le', 'replace')
            mods.append((base, isz, name))
            off += 108

    def owner(a):
        for b, s, nm in mods:
            if b <= a < b + s:
                return b, a - b, nm
        return None, None, None

    ms = [m for m in mods if 'MapleStory' in m[2]][0]
    print('MAPLESTORY base=0x%x size=0x%x' % (ms[0], ms[1]))

    dbh = DBHH(EXE.encode('utf-8'))
    ok = dbh.load(ms[0], ms[1], EXE)
    print('SymLoadModuleEx -> %s (err=%s)' % (ok, ctypes.GetLastError() if not ok else 0))

    if len(sys.argv) > 2:
        for a in sys.argv[2:]:
            rva = int(a, 16) if a.lower().startswith('0x') else int(a, 16)
            addr = ms[0] + rva
            print('  rva=0x%-8x sym=%-60s line=%s' %
                  (rva, dbh.sym(ms[0], addr) or '(none)', dbh.line(ms[0], addr) or '(none)'))
        return 0

    def show(a, tag):
        b, rva, nm = owner(a)
        if rva is None:
            print('  %-10s 0x%016x  (outside known modules)' % (tag, a))
            return
        short = nm.split('\\')[-1]
        extra = ''
        if b == ms[0]:
            extra = '  |  %s  |  %s' % (dbh.sym(ms[0], a) or '?', dbh.line(ms[0], a) or '?')
        print('  %-10s 0x%016x  %s+0x%x%s' % (tag, a, short, rva, extra))

    # memory for stack scan
    ranges = []
    if 5 in st:
        sz, rva = st[5]
        n, = struct.unpack_from('<I', data, rva)
        off = rva + 4
        for _ in range(n):
            a, msz, mrva = struct.unpack_from('<QII', data, off)
            ranges.append((a, msz, mrva))
            off += 16

    def read64(a):
        for base_a, msz, mrva in ranges:
            if base_a <= a and a + 8 <= base_a + msz:
                return struct.unpack_from('<Q', data, mrva + (a - base_a))[0]
        return None

    if 6 not in st:
        print('no exception stream')
        return 0
    sz, rva = st[6]
    tid, = struct.unpack_from('<I', data, rva)
    code, flags = struct.unpack_from('<II', data, rva + 8)
    eaddr, = struct.unpack_from('<Q', data, rva + 24)
    nparams, = struct.unpack_from('<I', data, rva + 32)
    info = struct.unpack_from('<15Q', data, rva + 40)
    ctx_size, ctx_rva = struct.unpack_from('<II', data, rva + 8 + 152)
    print('EXCEPTION tid=%d code=0x%08X (%s) nparams=%d ctx=%d@0x%x' %
          (tid, code, EX_CODES.get(code, '?'), nparams, ctx_size, ctx_rva))
    show(eaddr, 'rip(ex)')
    if code == 0xC0000005 and nparams >= 2:
        print('  access=%s fault_va=0x%x' %
              ({0: 'READ', 1: 'WRITE', 8: 'DEP-EXECUTE'}.get(info[0], info[0]), info[1]))
    else:
        print('  params=%s' % [hex(p) for p in info[:nparams]])

    ctx = data[ctx_rva:ctx_rva + ctx_size]
    for nm, o in REGS:
        v, = struct.unpack_from('<Q', ctx, o)
        if v and nm in ('rip', 'rsp', 'rbp', 'rax', 'rcx', 'rdx', 'rbx', 'rsi', 'rdi'):
            show(v, nm)
        else:
            print('  %-8s 0x%016x' % (nm, v))

    sp, = struct.unpack_from('<Q', ctx, 0x98)
    print('--- stack scan from rsp (code words in MapleStory.exe) ---')
    found = 0
    for i in range(0, 0x1000, 8):
        v = read64(sp + i)
        if v is None:
            continue
        b, rva2, nm = owner(v)
        if b != ms[0]:
            continue
        # heuristic: must be a plausible return address (not a data pointer)
        print('  sp+0x%04x  rva=0x%x  |  %s  |  %s' %
              (i, rva2, dbh.sym(ms[0], v) or '?', dbh.line(ms[0], v) or '?'))
        found += 1
        if found > 40:
            break
    if not found:
        print('  (no MapleStory.exe return addresses captured)')
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
