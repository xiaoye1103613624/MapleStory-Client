# -*- coding: utf-8 -*-
"""Resolve MapleStory.exe RVAs to function name + source line (PDB via dbghelp).

Usage: python _r26_addr.py 0xe39f 0x6ab90 ...
"""
import ctypes
import os
import struct
import sys
from ctypes import wintypes

EXE = r'E:\project\MapleStory-Client\build_check\MapleStory.exe'
BASE = 0x7ff700000000  # synthetic; we query by RVA+BASE


class SYM(ctypes.Structure):
    _fields_ = [('SizeOfStruct', wintypes.DWORD), ('Index', wintypes.DWORD),
                ('Reserved', ctypes.c_ulonglong * 2), ('Size', wintypes.DWORD),
                ('ModBase', ctypes.c_ulonglong), ('Flags', wintypes.DWORD),
                ('Value', ctypes.c_ulonglong), ('Address', ctypes.c_ulonglong),
                ('NameType', wintypes.DWORD), ('Name', ctypes.c_char * 2048)]


class LINE(ctypes.Structure):
    _fields_ = [('SizeOfStruct', wintypes.DWORD), ('Key', ctypes.c_void_p),
                ('LineNumber', wintypes.DWORD), ('FileName', ctypes.c_char_p),
                ('Offset', wintypes.DWORD)]


class PUB(ctypes.Structure):
    _fields_ = [('SizeOfStruct', wintypes.DWORD), ('Address', ctypes.c_ulonglong),
                ('Size', wintypes.DWORD), ('ModBase', ctypes.c_ulonglong),
                ('Reserved1', ctypes.c_ulonglong), ('Reserved2', ctypes.c_ulonglong),
                ('Offset', wintypes.DWORD), ('Reserved3', ctypes.c_ulonglong),
                ('Name', ctypes.c_char * 2048)]


d = ctypes.WinDLL('dbghelp.dll')
k = ctypes.WinDLL('kernel32.dll')
opts = 0x10 | 0x4 | 0x2000  # LOAD_LINES | DEFERRED | FAIL_CRITICAL
d.SymSetOptions(opts)
hp = k.GetCurrentProcess()
assert d.SymInitializeW(hp, os.path.dirname(EXE), False)
d.SymSetOptions.restype = ctypes.c_ulong
d.SymLoadModuleExW.restype = ctypes.c_ulonglong
d.SymLoadModuleExW.argtypes = [wintypes.HANDLE, wintypes.HANDLE, wintypes.LPCWSTR,
                               wintypes.LPCWSTR, ctypes.c_ulonglong, wintypes.DWORD,
                               ctypes.c_void_p, wintypes.DWORD]
h = d.SymLoadModuleExW(hp, None, EXE, None, BASE, 0x494000, None, 0)
print('modbase=0x%x handle=0x%x' % (BASE, h))

for a in sys.argv[1:]:
    rva = int(a, 16) if a.lower().startswith('0x') else int(a)
    addr = BASE + rva
    s = SYM()
    s.SizeOfStruct = 88
    disp = ctypes.c_ulonglong(0)
    name = d.SymFromAddrW(hp, ctypes.c_ulonglong(addr), ctypes.byref(disp), ctypes.byref(s))
    nm = s.Name.decode('mbcs', 'replace') if name else '?'
    p = PUB()
    p.SizeOfStruct = 72
    ok2 = d.SymGetSymFromAddr64(hp, ctypes.c_ulonglong(addr), ctypes.byref(disp), ctypes.byref(p))
    pub = p.Name.decode('mbcs', 'replace') if ok2 else '?'
    li = LINE()
    li.SizeOfStruct = ctypes.sizeof(LINE)
    ld = wintypes.DWORD()
    ok3 = d.SymGetLineFromAddr64(hp, ctypes.c_ulonglong(addr), ctypes.byref(ld), ctypes.byref(li))
    src = '%s:%d' % (os.path.basename(li.FileName.decode('mbcs', 'replace')) if li.FileName else '?',
                     li.LineNumber) if ok3 else '?'
    print('rva=0x%-8x sym=%-70s pub=%-50s line=%s' % (rva, nm[:70], pub[:50], src))
d.SymCleanup(hp)
