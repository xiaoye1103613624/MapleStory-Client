#!/usr/bin/env python3
"""分析 WER 崩溃转储：提取异常信息，并借助 DbgHelp + PDB 解析异常线程栈上的返回地址。

用法：
    python tools/resolution/analyze_crashdump.py <dump> [--pdb-dir E:\\MXD\\BeiDou-Client-xy] [--out <json>]

只读 dump / PDB，不启动进程、不修改系统状态。用于 0xC0000374 这类
「异常点在 ntdll 堆检查、真凶在更早写坏堆的调用栈」的定位。
"""
import argparse
import ctypes
import json
import struct
from ctypes import wintypes
from pathlib import Path

STREAM_THREAD_LIST = 3
STREAM_MODULE_LIST = 4
STREAM_EXCEPTION = 6


class SYMBOL_INFO(ctypes.Structure):
    _fields_ = [
        ("SizeOfStruct", wintypes.ULONG),
        ("TypeIndex", wintypes.ULONG),
        ("Reserved", ctypes.c_uint64 * 2),
        ("Index", wintypes.ULONG),
        ("Size", wintypes.ULONG),
        ("ModBase", ctypes.c_uint64),
        ("Flags", wintypes.ULONG),
        ("Value", ctypes.c_uint64),
        ("Address", ctypes.c_uint64),
        ("Register", wintypes.ULONG),
        ("Scope", wintypes.ULONG),
        ("Tag", wintypes.ULONG),
        ("NameLen", wintypes.ULONG),
        ("MaxNameLen", wintypes.ULONG),
        ("Name", ctypes.c_char * 1),
    ]


def parse_dump(path):
    data = Path(path).read_bytes()
    sig, version, nstreams, dir_rva, checksum, timestamp, flags = struct.unpack_from("<IIIIIIQ", data, 0)
    if sig != 0x504D444D:  # 'MDMP'
        raise ValueError("not a minidump: %08x" % sig)

    streams = {}
    for i in range(nstreams):
        stype, size, rva = struct.unpack_from("<III", data, dir_rva + i * 12)
        streams[stype] = (size, rva)

    modules = []
    if STREAM_MODULE_LIST in streams:
        _, rva = streams[STREAM_MODULE_LIST]
        count = struct.unpack_from("<I", data, rva)[0]
        for i in range(count):
            off = rva + 4 + i * 108
            base, size = struct.unpack_from("<QI", data, off)
            name_rva = struct.unpack_from("<I", data, off + 20)[0]
            nlen = struct.unpack_from("<I", data, name_rva)[0]
            name = data[name_rva + 4:name_rva + 4 + nlen].decode("utf-16-le", "replace")
            modules.append({"base": base, "size": size, "name": name})

    threads = []
    if STREAM_THREAD_LIST in streams:
        _, rva = streams[STREAM_THREAD_LIST]
        count = struct.unpack_from("<I", data, rva)[0]
        for i in range(count):
            off = rva + 4 + i * 48
            tid = struct.unpack_from("<I", data, off)[0]
            stack_start, stack_size, stack_rva = struct.unpack_from("<QII", data, off + 24)
            threads.append({"tid": tid, "stack_start": stack_start,
                            "stack_size": stack_size, "stack_rva": stack_rva})

    exception = {}
    if STREAM_EXCEPTION in streams:
        _, rva = streams[STREAM_EXCEPTION]
        tid = struct.unpack_from("<I", data, rva)[0]
        code, eflags, nested, address = struct.unpack_from("<IIQQ", data, rva + 8)
        exception = {"tid": tid, "code": "0x%08X" % code, "address": address}

    return data, {"streams": sorted(streams), "modules": modules,
                  "threads": threads, "exception": exception}


class Symbolizer:
    def __init__(self, exe, pdb_dir, base, size):
        self.dbghelp = ctypes.WinDLL("DbgHelp.dll", use_last_error=True)
        self.kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
        self.dbghelp.SymInitialize.argtypes = [wintypes.HANDLE, ctypes.c_char_p, wintypes.BOOL]
        self.dbghelp.SymLoadModuleEx.argtypes = [wintypes.HANDLE, wintypes.HANDLE,
                                                 ctypes.c_char_p, ctypes.c_char_p,
                                                 ctypes.c_uint64, wintypes.DWORD,
                                                 ctypes.c_void_p, wintypes.DWORD]
        self.dbghelp.SymFromAddr.argtypes = [wintypes.HANDLE, ctypes.c_uint64,
                                             ctypes.POINTER(ctypes.c_uint64),
                                             ctypes.POINTER(SYMBOL_INFO)]
        self.dbghelp.SymSetOptions.argtypes = [wintypes.DWORD]
        self.dbghelp.SymInitialize.restype = wintypes.BOOL
        self.dbghelp.SymLoadModuleEx.restype = ctypes.c_uint64
        self.dbghelp.SymFromAddr.restype = wintypes.BOOL

        self.proc = self.kernel32.GetCurrentProcess()
        self.dbghelp.SymSetOptions(0x00000004 | 0x00000002)  # UNDNAME | LOAD_LINES
        search = (str(pdb_dir) + ";" + str(exe.parent)).encode("mbcs")
        self.dbghelp.SymInitialize(self.proc, search, False)
        self.base = self.dbghelp.SymLoadModuleEx(
            self.proc, None, str(exe).encode("mbcs"), None, base, size, None, 0)
        self.buf = ctypes.create_string_buffer(ctypes.sizeof(SYMBOL_INFO) + 2048)

    def resolve(self, address):
        sym = ctypes.cast(self.buf, ctypes.POINTER(SYMBOL_INFO)).contents
        ctypes.memset(ctypes.byref(sym), 0, ctypes.sizeof(sym))
        sym.SizeOfStruct = ctypes.sizeof(SYMBOL_INFO)
        sym.MaxNameLen = 2048
        disp = ctypes.c_uint64(0)
        if not self.dbghelp.SymFromAddr(self.proc, address, ctypes.byref(disp), ctypes.byref(sym)):
            return None
        raw = ctypes.string_at(ctypes.addressof(sym) + SYMBOL_INFO.Name.offset, sym.NameLen)
        return {"address": "0x%X" % address, "rva": "0x%X" % (address - self.base),
                "symbol": raw.decode("utf-8", "replace").split("\x00")[0],
                "displacement": int(disp.value)}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("dump")
    ap.add_argument("--exe", default=r"E:\MXD\BeiDou-Client-xy\MapleStory.exe")
    ap.add_argument("--pdb-dir", default=r"E:\MXD\BeiDou-Client-xy")
    ap.add_argument("--out", default="")
    ap.add_argument("--max", type=int, default=40)
    args = ap.parse_args()

    data, info = parse_dump(args.dump)
    main_mod = next((m for m in info["modules"] if m["name"].lower().endswith("maplestory.exe")), None)
    result = {"dump": args.dump, "exception": info["exception"],
              "main_module": main_mod, "module_count": len(info["modules"]),
              "streams": info["streams"], "stack_symbols": []}

    if main_mod:
        exe = Path(args.exe)
        symbolizer = Symbolizer(exe, Path(args.pdb_dir), main_mod["base"], main_mod["size"])
        tid = info["exception"].get("tid")
        thread = next((t for t in info["threads"] if t["tid"] == tid), None)
        if thread is None and info["threads"]:
            thread = info["threads"][0]
        if thread:
            lo, hi = main_mod["base"], main_mod["base"] + main_mod["size"]
            stack = data[thread["stack_rva"]:thread["stack_rva"] + thread["stack_size"]]
            seen = set()
            for off in range(0, len(stack) - 8, 8):
                value = struct.unpack_from("<Q", stack, off)[0]
                if not (lo <= value < hi) or value in seen:
                    continue
                seen.add(value)
                sym = symbolizer.resolve(value)
                if sym:
                    sym["stack_offset"] = off
                    result["stack_symbols"].append(sym)
                    if len(result["stack_symbols"]) >= args.max:
                        break

    text = json.dumps(result, ensure_ascii=False, indent=2)
    if args.out:
        out = Path(args.out)
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(text + "\n", encoding="utf-8")
        print(json.dumps({"written": str(out), "exception": result["exception"],
                          "symbols": len(result["stack_symbols"])}, ensure_ascii=False))
    else:
        print(text)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
