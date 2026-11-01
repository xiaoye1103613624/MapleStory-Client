#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
T4 · 传输层逐字节对拍 oracle

做法：把 s9（Java）与客户端（C++）的封包变换**各自逐句**翻译成 Python，
用同一批 (iv, 长度, 明文) 向量跑两侧，再逐字节比较。
不做任何"推断"——每个函数上方都标出被抄写的源文件与行号。
第三方参照：pycryptodome 的 AES-256/ECB（验证客户端手搓 AES 与 s9 的 JCE 是否同一密钥表）。

覆盖 PLAN §4 T4 的全部条目：
  静态 key 扩展 / multiplyBytes IV 展开 / funnyBytes 6 遍 / OFB 块切换 /
  updateIv / getPacketHeader+isValidHeader / 密钥 0xFFFF-83 vs 83

用法：
    python tools/transport_oracle.py                 # 打印对拍报告
    python tools/transport_oracle.py --json <path>   # 同时落盘向量与结果（evidence）

退出码 = 差异条数（0 = 两侧完全一致）。
"""

import argparse
import hashlib
import json
import os
import random
import re
import sys

ROOT = os.path.dirname(os.path.abspath(__file__)) + os.sep + ".."
ROOT = os.path.abspath(ROOT)

CLIENT_CRYPTO = os.path.join(ROOT, "Net", "Cryptography.cpp")
S9_AES = os.path.join(
    ROOT, "..", "BeiDou-Server_s9", "gms-server", "src", "main", "java",
    "org", "gms", "net", "encryption", "MapleAESOFB.java")
S9_CUSTOM = os.path.join(
    ROOT, "..", "BeiDou-Server_s9", "gms-server", "src", "main", "java",
    "org", "gms", "net", "encryption", "MapleCustomEncryption.java")

B = 0xFF          # 8-bit 截断（Java 的 byte 赋值 / C++ 的 int8_t 赋值）
IV_SEND_SEED = [82, 48, 120, 0]     # MapleAESOFB 外部 InitializationVector.generateSend():15
IV_RECV_SEED = [70, 114, 122, 0]    # InitializationVector.generateReceive():20
S9_AES_KEY = bytes([0x13, 0, 0, 0, 0x08, 0, 0, 0, 0x06, 0, 0, 0, 0xB4, 0, 0, 0,
                    0x1B, 0, 0, 0, 0x0F, 0, 0, 0, 0x33, 0, 0, 0, 0x52, 0, 0, 0])  # MapleAESOFB.java:38-47
MBYTES_INIT = [0xF2, 0x53, 0x50, 0xC6]  # MapleAESOFB.java:174 / Cryptography.cpp:174-177

VERSION = 83                       # ServerConstants.VERSION
# ClientCyphers.java:15-16：send 用 (0xFFFF - VERSION)，receive 用 VERSION
# MapleAESOFB.java:97 再把传入值做高低字节交换
S9_SEND_SWAPPED = ((0xFFFF - VERSION) & 0xFF) << 8 | ((0xFFFF - VERSION) >> 8) & 0xFF
S9_RECV_SWAPPED = (VERSION & 0xFF) << 8 | (VERSION >> 8) & 0xFF


# --------------------------------------------------------------------------
# 源文件里的三张表：直接解析，避免手抄出错
# --------------------------------------------------------------------------
def parse_byte_table(path, anchor):
    """从源码里抠出 anchor 之后的第一个 {...} 字节初始化列表。"""
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        text = f.read()
    i = text.index(anchor)
    j = text.index("{", i)
    k = text.index("}", j)
    body = text[j + 1:k]
    body = re.sub(r"/\*.*?\*/", " ", body, flags=re.S)
    vals = []
    for tok in re.findall(r"(?:\(byte\)|\(uint8_t\))?\s*(0[xX][0-9A-Fa-f]+|\d+)", body):
        vals.append(int(tok, 16) if tok.lower().startswith("0x") else int(tok) & 0xFF)
    return bytes(v & 0xFF for v in vals)


def sha256(path):
    with open(path, "rb") as f:
        return hashlib.sha256(f.read()).hexdigest()


FUNNY = parse_byte_table(S9_AES, "funnyBytes = new byte[]")      # MapleAESOFB.java:49-81
SUBBOX = parse_byte_table(CLIENT_CRYPTO, "subbox[256]")          # Cryptography.cpp:304-322
MAPLEKEY = parse_byte_table(CLIENT_CRYPTO, "maplekey[256]")      # Cryptography.cpp:275-293


# ==========================================================================
# s9 侧（Java 转写；byte 一律用 0..255 的无符号视图建模）
#   Java 的 byte 复合赋值 = 运算后截断到低 8 位，与 (& 0xFF) 等价；
#   显式 & 0xFF 的地方逐句保留。
# ==========================================================================
def s9_roll_left(in_byte, count):            # MapleCustomEncryption.java:25-29
    tmp = (in_byte & 0xFF) << (count % 8)
    return ((tmp & 0xFF) | (tmp >> 8)) & B


def s9_roll_right(in_byte, count):           # MapleCustomEncryption.java:31-36
    tmp = ((in_byte & 0xFF) << 8) >> (count % 8)   # >>> ，tmp 恒非负
    return ((tmp & 0xFF) | (tmp >> 8)) & B


def s9_encrypt_data(data):                   # MapleCustomEncryption.java:38-70
    out = bytearray(data)
    n = len(out)
    for j in range(6):
        remember = 0
        dl = n & B
        if j % 2 == 0:
            for i in range(n):
                cur = out[i]
                cur = s9_roll_left(cur, 3)
                cur = (cur + dl) & B
                cur ^= remember
                remember = cur
                cur = s9_roll_right(cur, dl & 0xFF)
                cur = (~cur) & B
                cur = (cur + 0x48) & B
                dl = (dl - 1) & B
                out[i] = cur
        else:
            for i in range(n - 1, -1, -1):
                cur = out[i]
                cur = s9_roll_left(cur, 4)
                cur = (cur + dl) & B
                cur ^= remember
                remember = cur
                cur ^= 0x13
                cur = s9_roll_right(cur, 3)
                dl = (dl - 1) & B
                out[i] = cur
    return bytes(out)


def s9_decrypt_data(data):                   # MapleCustomEncryption.java:72-107
    out = bytearray(data)
    n = len(out)
    for j in range(1, 7):
        remember = 0
        dl = n & B
        if j % 2 == 0:
            for i in range(n):
                cur = out[i]
                cur = (cur - 0x48) & B
                cur = (~cur) & B
                cur = s9_roll_left(cur, dl & 0xFF)
                nxt = cur
                cur ^= remember
                remember = nxt
                cur = (cur - dl) & B
                cur = s9_roll_right(cur, 3)
                out[i] = cur
                dl = (dl - 1) & B
        else:
            for i in range(n - 1, -1, -1):
                cur = out[i]
                cur = s9_roll_left(cur, 3)
                cur ^= 0x13
                nxt = cur
                cur ^= remember
                remember = nxt
                cur = (cur - dl) & B
                cur = s9_roll_right(cur, 4)
                out[i] = cur
                dl = (dl - 1) & B
    return bytes(out)


def s9_multiply_bytes(iv, count, mul):       # MapleAESOFB.java:100-107
    size = count * mul
    return [iv[x % count] for x in range(size)]


def s9_funny_shit(input_byte, in_arr):       # MapleAESOFB.java:186-218
    elina = in_arr[1]
    anna = input_byte
    moritz = (FUNNY[elina & B] - input_byte) & B
    in_arr[0] = (in_arr[0] + moritz) & B
    moritz = in_arr[2] ^ FUNNY[anna & B]
    elina = (elina - (moritz & 0xFF)) & B
    in_arr[1] = elina
    elina = in_arr[3]
    moritz = elina
    elina = (elina - (in_arr[0] & 0xFF)) & B
    moritz = (FUNNY[moritz & B] + input_byte) & B
    moritz ^= in_arr[2]
    in_arr[2] = moritz
    elina = (elina + (FUNNY[anna & B] & 0xFF)) & B
    in_arr[3] = elina
    merry = in_arr[0] & 0xFF
    merry |= (in_arr[1] << 8) & 0xFF00
    merry |= (in_arr[2] << 16) & 0xFF0000
    merry |= (in_arr[3] << 24) & 0xFF000000
    ret = merry
    ret = (ret & 0xFFFFFFFF) >> 0x1D          # >>> 0x1d
    merry = (merry << 3) & 0xFFFFFFFF
    ret = (ret | merry) & 0xFFFFFFFF
    for j in range(4):
        in_arr[j] = (ret >> (8 * j)) & B
    return in_arr


def s9_get_new_iv(old_iv):                   # MapleAESOFB.java:173-179
    in_arr = list(MBYTES_INIT)
    for x in range(4):
        s9_funny_shit(old_iv[x], in_arr)
    return in_arr


def s9_crypt(data, iv, ecb):                 # MapleAESOFB.java:109-135（含 :137-139 updateIv）
    out = bytearray(data)
    remaining = len(out)
    llength = 0x5B0
    start = 0
    while remaining > 0:
        my_iv = s9_multiply_bytes(iv, 4, 4)
        if remaining < llength:
            llength = remaining
        for x in range(start, start + llength):
            if (x - start) % len(my_iv) == 0:
                blk = ecb(bytes(my_iv))       # JCE: AES/ECB/PKCS5Padding 的前 16 字节
                my_iv = list(blk)
            out[x] ^= my_iv[(x - start) % len(my_iv)]
        start += llength
        remaining -= llength
        llength = 0x5B4
    iv[:] = s9_get_new_iv(iv)
    return bytes(out)


def s9_get_packet_header(iv, swapped_version, length):   # MapleAESOFB.java:141-153
    iiv = iv[3] & B
    iiv |= (iv[2] << 8) & 0xFF00
    iiv ^= swapped_version
    mlength = ((length << 8) & 0xFF00) | ((length >> 8) & 0xFF)
    xored = iiv ^ mlength
    return [(iiv >> 8) & B, iiv & B, (xored >> 8) & B, xored & B]


def s9_decode_packet_length(header_int):      # GMSV83PacketProtocol.java:53-57
    length = ((header_int >> 16) & 0xFFFFFFFF) ^ (header_int & 0xFFFF)
    length = ((length << 8) & 0xFF00) | ((length >> 8) & 0xFF)
    return length


def s9_check_packet(packet2, iv, swapped_version):        # MapleAESOFB.java:161-164
    return (((packet2[0] ^ iv[2]) & B) == ((swapped_version >> 8) & B)
            and (((packet2[1] ^ iv[3]) & B) == (swapped_version & B)))


def s9_is_valid_header(header_int, iv, swapped_version):  # MapleAESOFB.java:166-171
    buf = [(header_int >> 24) & B, (header_int >> 16) & B]
    return s9_check_packet(buf, iv, swapped_version)


def s9_get_hello(send_iv, recv_iv):           # PacketCreator.java:619-629（writeShort/Int 均为 LE）
    out = []
    for v in (0x0E, VERSION, 1):
        out += [v & B, (v >> 8) & B]
    out.append(49)
    out += list(recv_iv)
    out += list(send_iv)
    out.append(8)
    return out


# ==========================================================================
# 客户端侧（C++ 转写：Cryptography.cpp）
# ==========================================================================
def cl_rollleft(data, count):                 # Cryptography.cpp:205-210
    mask = (data & 0xFF) << (count % 8)
    return ((mask & 0xFF) | (mask >> 8)) & B


def cl_rollright(data, count):                # Cryptography.cpp:212-217
    mask = ((data & 0xFF) << 8) >> (count % 8)
    return ((mask & 0xFF) | (mask >> 8)) & B


def cl_mapleencrypt(data):                    # Cryptography.cpp:94-121
    out = bytearray(data)
    n = len(out)
    for _ in range(3):
        remember = 0
        dl = n & B
        for i in range(n):
            cur = (cl_rollleft(out[i], 3) + dl) & B
            cur ^= remember
            remember = cur
            cur = cl_rollright(cur, dl & 0xFF)
            out[i] = (((~cur) & B) + 0x48) & B
            dl = (dl - 1) & B
        remember = 0
        dl = n & B
        for i in range(n - 1, -1, -1):
            cur = (cl_rollleft(out[i], 4) + dl) & B
            cur ^= remember
            remember = cur
            out[i] = cl_rollright(cur ^ 0x13, 3)
            dl = (dl - 1) & B
    return bytes(out)


def cl_mapledecrypt(data):                    # Cryptography.cpp:123-150
    out = bytearray(data)
    n = len(out)
    for _ in range(3):
        remember = 0
        dl = n & B
        for i in range(n - 1, -1, -1):
            cur = cl_rollleft(out[i], 3) ^ 0x13
            out[i] = cl_rollright(((cur ^ remember) - dl) & B, 4)
            remember = cur
            dl = (dl - 1) & B
        remember = 0
        dl = n & B
        for i in range(n):
            cur = (~(out[i] - 0x48)) & B
            cur = cl_rollleft(cur, dl & 0xFF)
            out[i] = cl_rollright(((cur ^ remember) - dl) & B, 3)
            remember = cur
            dl = (dl - 1) & B
    return bytes(out)


def cl_gmul(x):                               # Cryptography.cpp:351-354
    # (uint8_t)((int8_t)x >> 7) 在最高位置 1 时是 0xFF（算术右移），不是 1
    return ((x << 1) ^ (0x1B if x & 0x80 else 0)) & B


def cl_addroundkey(state, rnd):               # Cryptography.cpp:271-299
    off = rnd * 16
    return [(state[i] ^ MAPLEKEY[i + off]) & B for i in range(16)]


def cl_subbytes(state):                       # Cryptography.cpp:301-326
    return [SUBBOX[b] for b in state]


def cl_shiftrows(state):                      # Cryptography.cpp:328-349
    s = list(state)
    s[1], s[5], s[9], s[13] = s[5], s[9], s[13], s[1]
    s[2], s[10] = s[10], s[2]
    s[3], s[15], s[11], s[7] = s[15], s[11], s[7], s[3]
    s[6], s[14] = s[14], s[6]
    return s


def cl_mixcolumns(state):                     # Cryptography.cpp:356-375
    s = list(state)
    for i in range(0, 16, 4):
        c0, c1, c2, c3 = s[i], s[i + 1], s[i + 2], s[i + 3]
        m0, m1, m2, m3 = cl_gmul(c0), cl_gmul(c1), cl_gmul(c2), cl_gmul(c3)
        s[i] = (m0 ^ c3 ^ c2 ^ m1 ^ c1) & B
        s[i + 1] = (m1 ^ c0 ^ c3 ^ m2 ^ c2) & B
        s[i + 2] = (m2 ^ c1 ^ c0 ^ m3 ^ c3) & B
        s[i + 3] = (m3 ^ c2 ^ c1 ^ m0 ^ c0) & B
    return s


def cl_aesencrypt(block):                     # Cryptography.cpp:253-269
    st = list(block)
    st = cl_addroundkey(st, 0)
    for rnd in range(1, 14):
        st = cl_subbytes(st)
        st = cl_shiftrows(st)
        st = cl_mixcolumns(st)
        st = cl_addroundkey(st, rnd)
    st = cl_subbytes(st)
    st = cl_shiftrows(st)
    st = cl_addroundkey(st, 14)
    return bytes(st)


def cl_updateiv(iv):                          # Cryptography.cpp:152-203
    mbytes = list(MBYTES_INIT)
    for i in range(4):
        ivbyte = iv[i]
        mbytes[0] = (mbytes[0] + (FUNNY[mbytes[1] & B] - ivbyte)) & B
        mbytes[1] = (mbytes[1] - ((mbytes[2] ^ FUNNY[ivbyte & B]) & 0xFF)) & B
        mbytes[2] = (mbytes[2] ^ (FUNNY[mbytes[3] & B] + ivbyte)) & B
        mbytes[3] = (mbytes[3] + (FUNNY[ivbyte & B] - mbytes[0])) & B
        mask = 0
        mask |= mbytes[0] & 0xFF
        mask |= (mbytes[1] << 8) & 0xFF00
        mask |= (mbytes[2] << 16) & 0xFF0000
        mask |= (mbytes[3] << 24) & 0xFF000000
        mask = ((mask >> 0x1D) | (mask << 3)) & 0xFFFFFFFFFFFFFFFF
        for j in range(4):
            mbytes[j] = (mask >> (8 * j)) & B
    return mbytes


def cl_aesofb(data, iv):                      # Cryptography.cpp:219-251
    out = bytearray(data)
    n = len(out)
    blocklength = 0x5B0
    offset = 0
    while offset < n:
        miv = [iv[i % 4] for i in range(16)]
        remaining = n - offset
        if remaining > blocklength:
            remaining = blocklength
        for x in range(remaining):
            relpos = x % 16
            if relpos == 0:
                miv = list(cl_aesencrypt(bytes(miv)))
            out[x + offset] ^= miv[relpos]
        offset += blocklength
        blocklength = 0x5B4
    iv[:] = cl_updateiv(iv)
    return bytes(out)


def cl_encrypt(data, sendiv):                 # Cryptography.cpp:36-42
    return cl_aesofb(cl_mapleencrypt(data), sendiv)


def cl_decrypt(data, recviv):                 # Cryptography.cpp:44-50
    return cl_mapledecrypt(cl_aesofb(data, recviv))


def cl_create_header(iv, length):             # Cryptography.cpp:52-73
    a = ((iv[3] << 8) | iv[2]) ^ VERSION
    b = a ^ length
    return [a % 0x100, a // 0x100, b % 0x100, b // 0x100]


def cl_check_length(header4, sign_extend=False):   # Cryptography.cpp:75-98
    """客户端 check_length 的当前模型：D8 修复后 Cryptography.cpp:89 是
    static_cast<uint16_t> ⇒ 与 s9 decodePacketLength 一样产出无符号 0..65535。
    sign_extend=True 保留下来建模修复前的 int16_t 缺陷，供对拍做负向对照。"""
    hm = 0
    for i in range(4):
        hm |= (header4[i] & 0xFF) << (8 * i)
    v = (hm >> 16) ^ (hm & 0xFFFF)
    if sign_extend:                       # static_cast<int16_t> 后赋给 size_t ⇒ 负数符号扩展
        v = v - 0x10000 if v >= 0x8000 else v
    return v


# ==========================================================================
# 第三方参照：AES-256 密钥扩展 + pycryptodome
# ==========================================================================
def aes256_expand(key):
    """标准 AES-256 密钥表，按 round-key 主序铺平成 240 字节。"""
    rcon = 1
    w = [list(key[4 * i:4 * i + 4]) for i in range(8)]
    for i in range(8, 60):
        t = list(w[i - 1])
        if i % 8 == 0:
            t = t[1:] + t[:1]
            t = [SUBBOX[b] for b in t]
            t[0] ^= rcon
            rcon = cl_gmul(rcon)
        elif i % 8 == 4:
            t = [SUBBOX[b] for b in t]
        w.append([(w[i - 8][j] ^ t[j]) & B for j in range(4)])
    return bytes(b for word in w for b in word)


try:
    from Crypto.Cipher import AES

    _aes = AES.new(S9_AES_KEY, AES.MODE_ECB)

    def ecb(b16):
        """JCE Cipher.getInstance("AES") = AES/ECB/PKCS5Padding；
        16 字节输入会补一整块 pad，输出 32 字节，前 16 字节即 ECB 结果
        （MapleAESOFB.java:121-122 只 arraycopy 前 16 字节）。
        s9 未引入 BouncyCastle（gms-server/pom.xml 只有 netty 4.1.109），
        故 provider = SunJCE，默认变换成立。"""
        return _aes.encrypt(b16)

    HAVE_PYCRYPTO = True
except Exception:                                    # noqa: BLE001
    HAVE_PYCRYPTO = False

    def ecb(b16):
        raise RuntimeError("pycryptodome 不可用")


def diff_bytes(a, b):
    """返回首个不匹配位置与两侧上下文摘要。"""
    for i, (x, y) in enumerate(zip(a, b)):
        if x != y:
            return (i, x, y)
    if len(a) != len(b):
        return ("len", len(a), len(b))
    return None


def make_lengths():
    lens = [0, 1, 2, 3, 4, 5, 7, 8, 15, 16, 17, 31, 32, 63, 64, 127, 128,
            255, 256, 511, 512, 1023, 1024, 1455, 1456, 1457, 2048, 2911,
            2912, 2913, 4368, 4369, 8191, 16384, 32767, 32768, 40000, 65535]
    rng = random.Random(20261030)
    lens += [rng.randrange(1, 65535) for _ in range(24)]
    return sorted(set(lens))


def make_plaintext(n, rng):
    return bytes(rng.randrange(256) for _ in range(n))


# ==========================================================================
# 对拍套件
# ==========================================================================
def run(vectors_out=None):
    rng = random.Random(20261030)
    results = []

    def record(name, ok, detail="", vectors=None):
        results.append({"check": name, "ok": bool(ok), "detail": detail})
        mark = "OK  " if ok else "DIFF"
        print(f"[{mark}] {name}" + (f" — {detail}" if detail and not ok else ""))
        if vectors is not None and vectors_out is not None:
            vectors_out.append({"check": name, "ok": bool(ok), "detail": detail,
                                "cases": vectors})

    # ---- 0. 源表完整性 -------------------------------------------------
    record("source-tables", len(FUNNY) >= 256 and len(SUBBOX) == 256 and len(MAPLEKEY) >= 240,
           f"funny={len(FUNNY)} subbox={len(SUBBOX)} maplekey={len(MAPLEKEY)}")
    if len(FUNNY) != 256 or len(SUBBOX) != 256 or len(MAPLEKEY) != 256:
        return results

    # ---- 1. 静态密钥扩展：客户端硬编码表 vs s9 32 字节 key 的标准扩展 ----
    ref_schedule = aes256_expand(S9_AES_KEY)
    d = diff_bytes(ref_schedule, MAPLEKEY[:240])
    record("aes-key-schedule(s9 key -> client maplekey)", d is None,
           f"首个差异 {d}" if d else f"{len(ref_schedule)} 字节全等（round0..14）")

    # ---- 2. 手搓 AES vs pycryptodome ---------------------------------
    if HAVE_PYCRYPTO:
        bad = None
        for _ in range(200):
            blk = bytes(rng.randrange(256) for _ in range(16))
            if cl_aesencrypt(blk) != ecb(blk):
                bad = (blk, cl_aesencrypt(blk), ecb(blk))
                break
        record("client-aes == JCE AES-256/ECB", bad is None,
               f"块 {bad[0].hex()} 客户端={bad[1].hex()} JCE={bad[2].hex()}" if bad
               else "200 随机块全等")

    # ---- 3. 自定义加密 6 遍（funnyBytes/roll）--------------------------
    lens = make_lengths()
    enc_bad = dec_bad = rt_bad = None
    enc_cases = []
    for n in lens:
        pt = make_plaintext(n, rng)
        a, b = s9_encrypt_data(pt), cl_mapleencrypt(pt)
        if a != b and enc_bad is None:
            enc_bad = (n, diff_bytes(a, b))
        c, d2 = s9_decrypt_data(pt), cl_mapledecrypt(pt)
        if c != d2 and dec_bad is None:
            dec_bad = (n, diff_bytes(c, d2))
        if cl_mapledecrypt(s9_encrypt_data(pt)) != pt and rt_bad is None:
            rt_bad = n
        if n in (0, 1, 16, 17, 255, 256, 1456, 32768):
            enc_cases.append({"len": n, "plain": pt[:16].hex(),
                              "s9_enc": a[:16].hex(), "client_enc": b[:16].hex()})
    record("custom-encrypt 6 遍 (s9 vs client)", enc_bad is None,
           f"len={enc_bad[0]} 首差 {enc_bad[1]}" if enc_bad else f"{len(lens)} 个长度全等")
    record("custom-decrypt 6 遍 (s9 vs client)", dec_bad is None,
           f"len={dec_bad[0]} 首差 {dec_bad[1]}" if dec_bad else f"{len(lens)} 个长度全等")
    record("cross round-trip (s9enc -> clientdec)", rt_bad is None,
           f"len={rt_bad} 不可逆" if rt_bad else "全部还原成功")

    # ---- 4. updateIv 链 -----------------------------------------------
    iv0 = [0x46, 0x72, 0x7A, 0x00]
    a, b = list(iv0), list(iv0)
    first_diff = None
    chain_cases = []
    for step in range(1, 20001):
        a = s9_get_new_iv(a)
        b = cl_updateiv(b)
        if step in (1, 2, 3, 100, 1000, 20000):
            chain_cases.append({"step": step, "s9": bytes(a).hex(), "client": bytes(b).hex()})
        if a != b and first_diff is None:
            first_diff = (step, list(a), list(b))
    record("updateIv / funnyShit 链 20000 步", first_diff is None,
           f"第 {first_diff[0]} 步起 s9={bytes(first_diff[1]).hex()} client={bytes(first_diff[2]).hex()}"
           if first_diff else "20000 次迭代逐步全等", chain_cases)

    # ---- 5. AES-OFB keystream（含 multiplyBytes 展开与块切换）----------
    ks_bad = None
    iv_bad = None
    ks_cases = []
    for n in lens:
        pt = make_plaintext(n, rng)
        iv_a = [70, 114, 122, n & 0xFF]
        iv_b = list(iv_a)
        got_a = s9_crypt(bytearray(pt), iv_a, ecb) if HAVE_PYCRYPTO else None
        got_b = cl_aesofb(bytearray(pt), iv_b)
        if got_a is not None and got_a != got_b and ks_bad is None:
            ks_bad = (n, diff_bytes(got_a, got_b))
        if iv_a != iv_b and iv_bad is None:
            iv_bad = (n, list(iv_a), list(iv_b))
        if n in (0, 1, 16, 1456, 1457, 2912, 2913, 32768) and got_a is not None:
            ks_cases.append({"len": n, "iv": bytes([70, 114, 122, n & 0xFF]).hex(),
                             "s9_ct": got_a[:12].hex(), "client_ct": got_b[:12].hex(),
                             "s9_iv_next": bytes(iv_a).hex(), "client_iv_next": bytes(iv_b).hex()})
    record("aes-ofb keystream (s9 vs client)", ks_bad is None,
           f"len={ks_bad[0]} 首差 {ks_bad[1]}" if ks_bad else f"{len(lens)} 个长度全等", ks_cases)
    record("aes-ofb 后置 iv", iv_bad is None,
           f"len={iv_bad[0]} s9={bytes(iv_bad[1]).hex()} client={bytes(iv_bad[2]).hex()}"
           if iv_bad else "全部一致")

    # ---- 6. 头部：客户端发 -> 服务端收 --------------------------------
    hd_bad = None
    val_bad = None
    hdr_cases = []
    for n in lens:
        iv = [70, 114, 122, 0x5A]
        hdr = cl_create_header(iv, n)
        # 服务端以 receiveCypher（version 83 交换后 0x5300）解同一 iv
        hint = (hdr[0] << 24) | (hdr[1] << 16) | (hdr[2] << 8) | hdr[3]
        if s9_decode_packet_length(hint) != (n & 0xFFFF) and hd_bad is None:
            hd_bad = (n, hdr, s9_decode_packet_length(hint))
        if not s9_is_valid_header(hint, iv, S9_RECV_SWAPPED) and val_bad is None:
            val_bad = (n, hdr)
        if n in (0, 1, 2, 255, 256, 1456, 32767, 32768, 65535):
            hdr_cases.append({"len": n, "client_header": bytes(hdr).hex(),
                              "s9_decode_len": s9_decode_packet_length(hint),
                              "s9_valid": s9_is_valid_header(hint, iv, S9_RECV_SWAPPED)})
    record("c2s 头部长度可被服务端解出", hd_bad is None,
           f"len={hd_bad[0]} 头={bytes(hd_bad[1]).hex()} 服务端解出={hd_bad[2]}" if hd_bad else "",
           hdr_cases)
    record("c2s 头部通过 isValidHeader", val_bad is None,
           f"len={val_bad[0]} 头={bytes(val_bad[1]).hex()}" if val_bad else "version 0x5300 校验全部通过")

    # ---- 7. 头部：服务端发 -> 客户端收 --------------------------------
    # D8 已修：Cryptography.cpp:89 改为 static_cast<uint16_t>，与 s9
    # decodePacketLength (GMSV83PacketProtocol.java:53-57) 同为无符号 16 位。
    # 负向对照用 sign_extend=True 建模修复前的缺陷：若两种模型在这些长度上完全一致，
    # 说明用例根本没覆盖 32768..65535，这条断言就成了空跑 —— 单独 record 卡住它。
    dl_bad = None
    dl_cases = []
    legacy_diverge = []
    for n in lens:
        iv = list(IV_SEND_SEED)
        iv[3] = 0x31
        hdr = s9_get_packet_header(iv, S9_SEND_SWAPPED, n)
        client_sees = cl_check_length(hdr)
        legacy = cl_check_length(hdr, sign_extend=True)
        if legacy != n:
            legacy_diverge.append((n, legacy))
        if client_sees != n and dl_bad is None:
            dl_bad = (n, hdr, client_sees)
        if n in (1, 255, 256, 1456, 32767, 32768, 40000, 65535):
            dl_cases.append({"len": n, "server_header": bytes(hdr).hex(),
                             "client_check_length": client_sees,
                             "legacy_int16": legacy})
    record("s2c 头部长度客户端解为无符号 0..65535（D8 已修）", dl_bad is None,
           f"len={dl_bad[0]} 头={bytes(dl_bad[1]).hex()} 客户端={dl_bad[2]}" if dl_bad
           else f"{len(lens)} 个长度全等", dl_cases)
    record("D8 断言覆盖 32768..65535（legacy 模型确有偏离）", len(legacy_diverge) > 0,
           "两种模型在所有用例上一致 ⇒ 长度用例没进高位区间，"
           f"{len(lens)} 个用例里 0 个能区分 uint16_t 与 int16_t"
           if not legacy_diverge else
           f"{len(legacy_diverge)} 个长度只有修复前的 int16_t 模型会解错"
           f"（最小 len={legacy_diverge[0][0]} -> {legacy_diverge[0][1]}），断言非空跑")

    # ---- 8. 全链路：客户端加密 -> 服务端解密 ---------------------------
    e2e_bad = None
    e2e_iv_bad = None
    e2e_cases = []
    for n in lens:
        if n > 0x7FFF:
            continue
        pt = make_plaintext(n, rng)
        iv_c = [70, 114, 122, 0x5A]
        iv_s = list(iv_c)
        wire = bytearray(cl_encrypt(pt, iv_c))
        # 服务端：decode -> crypt -> decryptData（GMSV83PacketProtocol.java:30-42）
        got = s9_decrypt_data(s9_crypt(wire, iv_s, ecb))
        if HAVE_PYCRYPTO:
            if got != pt and e2e_bad is None:
                e2e_bad = (n, diff_bytes(got, pt))
            if iv_c != iv_s and e2e_iv_bad is None:
                e2e_iv_bad = (n, list(iv_c), list(iv_s))
        if n in (1, 2, 16, 1456, 32767):
            e2e_cases.append({"len": n, "plain": pt[:12].hex(), "wire": bytes(wire[:12]).hex(),
                              "iv_client_after": bytes(iv_c).hex(),
                              "iv_server_after": bytes(iv_s).hex()})
    record("端到端 c2s（客户端 encrypt -> 服务端 decode）",
           e2e_bad is None and HAVE_PYCRYPTO,
           "缺 pycryptodome，跳过" if not HAVE_PYCRYPTO else
           (f"len={e2e_bad[0]} 首差 {e2e_bad[1]}" if e2e_bad else ""), e2e_cases)
    record("端到端 c2s 后置 IV 同步", e2e_iv_bad is None,
           f"len={e2e_iv_bad[0]} client={bytes(e2e_iv_bad[1]).hex()} "
           f"s9={bytes(e2e_iv_bad[2]).hex()}" if e2e_iv_bad else "")

    # ---- 9. 全链路：服务端加密 -> 客户端解密 ---------------------------
    e2e2_bad = None
    e2e2_cases = []
    for n in lens:
        if n > 0x7FFF:
            continue
        pt = make_plaintext(n, rng)
        iv_s = [82, 48, 120, 0x31]
        iv_c = list(iv_s)
        wire = bytearray(s9_crypt(s9_encrypt_data(pt), iv_s, ecb))
        got = cl_decrypt(wire, iv_c)
        if HAVE_PYCRYPTO:
            if got != pt and e2e2_bad is None:
                e2e2_bad = (n, diff_bytes(got, pt))
        if n in (1, 16, 1456, 32767):
            e2e2_cases.append({"len": n, "plain": pt[:12].hex(), "wire": bytes(wire[:12]).hex(),
                               "iv_server_after": bytes(iv_s).hex(),
                               "iv_client_after": bytes(iv_c).hex()})
    record("端到端 s2c（服务端 encode -> 客户端 decrypt）",
           e2e2_bad is None and HAVE_PYCRYPTO,
           "缺 pycryptodome，跳过" if not HAVE_PYCRYPTO else
           (f"len={e2e2_bad[0]} 首差 {e2e2_bad[1]}" if e2e2_bad else ""), e2e2_cases)

    # ---- 10. handshake 布局 -------------------------------------------
    siv, riv = list(IV_SEND_SEED), list(IV_RECV_SEED)
    hello = s9_get_hello(siv, riv)
    cl_sendiv = hello[7:11]
    cl_recviv = hello[11:15]
    ok = (len(hello) == 16 and cl_sendiv == riv and cl_recviv == siv)
    record("handshake 16 字节布局（Cryptography.cpp:22-31 vs PacketCreator:619-629）", ok,
           f"hello={bytes(hello).hex()} client.sendiv={bytes(cl_sendiv).hex()} "
           f"client.recviv={bytes(cl_recviv).hex()}" if not ok else
           "client.sendiv = 服务端 recvIv('Frz')，client.recviv = 服务端 sendIv('R0x')")

    # ---- 11. 版本常量：0xFFFF-83 vs 83 --------------------------------
    ok = (S9_SEND_SWAPPED == 0xACFF and S9_RECV_SWAPPED == 0x5300)
    record("版本常量换算（ClientCyphers.java:15-16 + MapleAESOFB.java:97）", ok,
           f"send=0x{S9_SEND_SWAPPED:04X} recv=0x{S9_RECV_SWAPPED:04X}"
           "；客户端 Cryptography.cpp:55 只用 83，且只作用于低字节 ⇒ 与 receive 校验等价")

    # ---- 12. 整场会话：300 包连续加解密（OFB 跨包连续性）---------------
    iv_a = [70, 114, 122, 0x5A]
    iv_b = list(iv_a)
    session = []
    stream_bad = None
    for idx in range(300):
        n = rng.choice([2, 3, 7, 16, 17, 100, 1456, 1457, 2912, 4368, 5000, 10000])
        pt = make_plaintext(n, rng)
        a = s9_crypt(bytearray(pt), iv_a, ecb) if HAVE_PYCRYPTO else None
        b = cl_aesofb(bytearray(pt), iv_b)
        if a is not None and (a != b or iv_a != iv_b) and stream_bad is None:
            stream_bad = (idx, n, "密文" if a != b else "IV")
        if idx in (0, 1, 42, 299):
            session.append({"packet": idx, "len": n, "plain": pt[:12].hex(),
                            "s9_ct": a[:12].hex() if a else None, "client_ct": b[:12].hex(),
                            "iv_after": bytes(iv_a).hex()})
    record("整场会话 300 包连续 keystream + IV 链", stream_bad is None and HAVE_PYCRYPTO,
           "缺 pycryptodome，跳过" if not HAVE_PYCRYPTO else
           (f"第 {stream_bad[0]} 包（len={stream_bad[1]}）起 {stream_bad[2]} 分叉" if stream_bad
            else ""), session)

    # 双向：客户端加密 -> 服务端解密，同一 iv 状态连跑 60 包
    iv_c = [70, 114, 122, 0x11]
    iv_s = list(iv_c)
    rt_fail = None
    for idx in range(60):
        n = rng.choice([2, 5, 16, 1456, 1457, 3000, 8192])
        pt = make_plaintext(n, rng)
        wire = cl_encrypt(pt, iv_c)
        got = s9_decrypt_data(s9_crypt(bytearray(wire), iv_s, ecb))
        if got != pt or iv_c != iv_s:
            rt_fail = (idx, n)
            break
    record("双向会话 60 包（客户端 encrypt → 服务端 decode）还原", rt_fail is None,
           f"第 {rt_fail[0]} 包（len={rt_fail[1]}）失败" if rt_fail else "")

    return results


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--json", help="把向量与结果写入该路径")
    args = ap.parse_args()

    print(f"源文件指纹：Cryptography.cpp={sha256(CLIENT_CRYPTO)[:16]} "
          f"MapleAESOFB.java={sha256(S9_AES)[:16]} MapleCustomEncryption.java={sha256(S9_CUSTOM)[:16]}")
    print(f"pycryptodome: {'可用' if HAVE_PYCRYPTO else '不可用（AES 对拍降级）'}")
    print("-" * 72)

    vectors = []
    results = run(vectors)
    diffs = [r for r in results if not r["ok"]]
    print("-" * 72)
    print(f"共 {len(results)} 项对拍，差异 {len(diffs)} 项")
    for r in diffs:
        print(f"  · {r['check']}: {r['detail']}")

    if args.json:
        os.makedirs(os.path.dirname(os.path.abspath(args.json)), exist_ok=True)
        with open(args.json, "w", encoding="utf-8") as f:
            json.dump({
                "generated_by": "tools/transport_oracle.py",
                "sources": {
                    "client/Net/Cryptography.cpp": sha256(CLIENT_CRYPTO),
                    "s9/MapleAESOFB.java": sha256(S9_AES),
                    "s9/MapleCustomEncryption.java": sha256(S9_CUSTOM),
                },
                "constants": {
                    "version": VERSION,
                    "s9_send_swapped": f"0x{S9_SEND_SWAPPED:04X}",
                    "s9_recv_swapped": f"0x{S9_RECV_SWAPPED:04X}",
                    "aes_key": S9_AES_KEY.hex(),
                },
                "checks": results,
                "vectors": vectors,
            }, f, ensure_ascii=False, indent=2)
        print(f"向量已写入 {args.json}")

    sys.exit(min(len(diffs), 99))


if __name__ == "__main__":
    main()
