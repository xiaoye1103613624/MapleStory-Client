"""Resolve WER fault offsets (RVA) in MapleStory.exe to function + source line.

Uses dbghelp against the side-by-side PDB, so the PDB must belong to the same build
(as matched by PE TimeDateStamp; WER reports it as the P3/P6 signature).

    python tools/symbolize_offset.py 0x7fb70 [0x1ec372 ...]

HARD RULE — check 出错模块名称 (the faulting module) in the Event 1000 message BEFORE trusting any
output. `错误偏移` is relative to the *faulting module*, not always to MapleStory.exe. Feeding an
ntdll offset in here still prints a confident-looking MapleStory source line, and it is wrong
(this happened on 2026-10-30: 0xc0000374 / ntdll.dll / 0x118095 resolved to a bogus
UIStoreBank.cpp:56 — the giveaway is the same offset recurring across different exe builds).
Run tools/wer_event_raw.ps1 to dump the full message.
"""
import ctypes
import ctypes.wintypes as wt
import struct
import sys

EXE = r"E:\MXD\BeiDou-Client-xy\MapleStory.exe"
PDB_DIR = r"E:\MXD\BeiDou-Client-xy"

dbgh = ctypes.WinDLL("dbghelp.dll")
kernel = ctypes.WinDLL("kernel32.dll")

SYMOPT_UNDNAME = 0x00000002
SYMOPT_DEFERRED_LOADS = 0x00000004
SYMOPT_LOAD_LINES = 0x00000010
GetCurrentProcess = kernel.GetCurrentProcess
GetCurrentProcess.restype = wt.HANDLE

kernel.LoadLibraryExW.argtypes = [wt.LPCWSTR, wt.HANDLE, wt.DWORD]


class SYMBOL_INFO(ctypes.Structure):
    _fields_ = [
        ("SizeOfStruct", ctypes.c_ulong),
        ("TypeIndex", ctypes.c_ulong),
        ("Reserved", ctypes.c_uint64 * 2),
        ("Index", ctypes.c_ulong),
        ("Size", ctypes.c_ulong),
        ("ModBase", ctypes.c_uint64),
        ("Flags", ctypes.c_ulong),
        ("Value", ctypes.c_uint64),
        ("Tag", ctypes.c_uint64),
        ("StructSize", ctypes.c_ulong),
        ("Source", ctypes.c_uint64),
        ("Length", ctypes.c_ulong),
        ("Function", ctypes.c_uint64),
        ("LineNumber", ctypes.c_ulong),
        ("Address", ctypes.c_uint64),
        ("Name", ctypes.c_char * 1024),
    ]


# SizeOfStruct must describe the header only, up to (not including) the inline name buffer.
SYM_HDR = getattr(SYMBOL_INFO, "Name").offset + 1


class IMAGEHLP_LINE64(ctypes.Structure):
    _fields_ = [
        ("SizeOfStruct", ctypes.c_ulong),
        ("Key", ctypes.c_void_p),
        ("LineNumber", ctypes.c_ulong),
        ("FileName", ctypes.c_char_p),
        ("Address", ctypes.c_uint64),
    ]


dbgh.SymSetOptions.restype = ctypes.c_ulong
dbgh.SymInitializeW.argtypes = [wt.HANDLE, wt.LPCWSTR, wt.BOOL]
dbgh.SymInitializeW.restype = wt.BOOL
dbgh.SymLoadModuleExW.argtypes = [wt.HANDLE, wt.HANDLE, wt.LPCWSTR, wt.LPCWSTR,
                                  ctypes.c_uint64, ctypes.c_ulong, ctypes.c_void_p, ctypes.c_ulong]
dbgh.SymLoadModuleExW.restype = ctypes.c_uint64
SYM_FROM_ADDR = getattr(dbgh, "SymFromAddrW", None) or dbgh.SymFromAddr
SYM_FROM_ADDR.argtypes = [wt.HANDLE, ctypes.c_uint64,
                          ctypes.POINTER(ctypes.c_uint64), ctypes.POINTER(SYMBOL_INFO)]
SYM_FROM_ADDR.restype = wt.BOOL
dbgh.SymGetLineFromAddr64.argtypes = [wt.HANDLE, ctypes.c_uint64,
                                      ctypes.POINTER(ctypes.c_ulong), ctypes.POINTER(IMAGEHLP_LINE64)]
dbgh.SymGetLineFromAddr64.restype = wt.BOOL


def nt(path):
    """Return (e_lfanew, magic, image_base, size_of_image) from the PE headers."""
    with open(path, "rb") as f:
        blob = f.read(0x400)
    e_lfanew = struct.unpack_from("<I", blob, 0x3C)[0]
    opt = e_lfanew + 24                      # 4-byte signature + 20-byte file header
    magic = struct.unpack_from("<H", blob, opt)[0]
    if magic != 0x20B:
        raise SystemExit("not PE32+ (magic=0x%x) - offsets below assume x64" % magic)
    return (e_lfanew, magic,
            struct.unpack_from("<Q", blob, opt + 24)[0],   # OptionalHeader64.ImageBase
            struct.unpack_from("<I", blob, opt + 52)[0])   # OptionalHeader64.SizeOfImage


def pe_timestamp(path):
    with open(path, "rb") as f:
        d = f.read(0x400)
        e_lfanew = struct.unpack_from("<I", d, 0x3C)[0]
        return struct.unpack_from("<I", d, e_lfanew + 8)[0]  # FileHeader.TimeDateStamp


def main(argv):
    offs = [int(a, 16) if a.lower().startswith("0x") else int(a) for a in argv] or [0x7FB70]
    _, _, pref_base, size_img = nt(EXE)
    ts = pe_timestamp(EXE)
    print("exe preferred imagebase=0x%x  TimeDateStamp=0x%08x  SizeOfImage=0x%x" % (pref_base, ts, size_img))
    print("WER module timestamp must equal 0x%08x for these symbols to be valid" % ts)

    dbgh.SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES)
    h = GetCurrentProcess()
    if not dbgh.SymInitializeW(h, PDB_DIR, False):
        print("SymInitializeW failed rc=%d" % ctypes.GetLastError())
        return 1
    dbgh.SymCleanup.argtypes = [wt.HANDLE]
    # hFile=NULL + DllBase=0 => dbghelp reads the PE itself and uses its preferred base.
    mod = dbgh.SymLoadModuleExW(h, None, EXE, None, 0, 0, None, 0)
    if not mod:
        print("SymLoadModuleExW failed rc=%d  (pdb not loaded / mismatch)" % ctypes.GetLastError())
        return 1
    base = mod
    print("module base from dbghelp = 0x%x (preferred = 0x%x)" % (mod, pref_base))

    rc = 0
    for off in offs:
        va = base + off
        sym = SYMBOL_INFO()
        sym.SizeOfStruct = SYM_HDR
        disp = ctypes.c_uint64(0)
        line = IMAGEHLP_LINE64()
        line.SizeOfStruct = ctypes.sizeof(IMAGEHLP_LINE64)
        ldisp = ctypes.c_ulong(0)
        print("\nRVA 0x%x -> VA 0x%x" % (off, va))
        if SYM_FROM_ADDR(h, va, ctypes.byref(disp), ctypes.byref(sym)):
            print("   func: %s + 0x%x" % (sym.Name.decode("utf-8", "replace"), disp.value))
        else:
            print("   func: unresolved rc=%d" % ctypes.GetLastError())
            rc = 1
        if dbgh.SymGetLineFromAddr64(h, va, ctypes.byref(ldisp), ctypes.byref(line)):
            print("   line: %s:%d (+0x%x bytes)" %
                  (line.FileName.decode("utf-8", "replace"), line.LineNumber, ldisp.value))
        else:
            print("   line: unresolved rc=%d" % ctypes.GetLastError())
            rc = 1
    dbgh.SymCleanup(h)
    return rc


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
