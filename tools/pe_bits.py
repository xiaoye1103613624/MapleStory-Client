# -*- coding: utf-8 -*-
"""pe_bits.py — 读 PE 头：机器类型 / DllCharacteristics（LAA、DYNAMICBASE、NX、CET）"""
import struct
import os


def bits(path):
    d = open(path, 'rb').read(64 * 1024)
    off = struct.unpack_from('<I', d, 0x3C)[0]
    mach, cn = struct.unpack_from('<HH', d, off + 4)
    opt = off + 24
    magic = struct.unpack_from('<H', d, opt)[0]
    pe32plus = (magic == 0x20b)
    # PE32 的 DllCharacteristics 在 opt+70，PE32+ 在 opt+74；SubSystem 统一 opt+68
    dllc_off = opt + (74 if pe32plus else 70)
    dllc = struct.unpack_from('<H', d, dllc_off)[0]
    subsystem = struct.unpack_from('<H', d, opt + 68)[0]
    return dict(
        machine={0x14c: 'x86(32)', 0x8664: 'x64', 0xaa64: 'ARM64'}.get(mach, hex(mach)),
        pe='PE32+' if pe32plus else 'PE32',
        chars=ch_count(cn),
        dllc='0x%04x' % dllc,
        LAA=bool(dllc & 0x20),
        DYNAMICBASE=bool(dllc & 0x40),
        NX_COMPAT=bool(dllc & 0x100),
        CETSHADOW=bool(dllc & 0x4000),
        subsystem=subsystem,
    )


def ch_count(cn):
    # 段数用于估算调试/符号体积
    return cn


for p in [r'E:\MXD\BeiDou-ClientV17.7\BeiDou.exe',
          r'E:\MXD\BeiDou-ClientV17.7\Gr2D_DX8.dll',
          r'E:\MXD\BeiDou-ClientV17.7\Data\Gr2D_DX8.dll',
          r'E:\MXD\BeiDou-Client-xy\MapleStory.exe']:
    if os.path.exists(p):
        print(os.path.basename(p), bits(p))
    else:
        print(os.path.basename(p), 'MISSING')
