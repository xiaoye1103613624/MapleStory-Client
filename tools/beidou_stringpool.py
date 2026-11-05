"""BeiDou.exe (V17.7) obfuscated string-pool decoder.

The client stores almost every string -- WZ paths, property names, animation names,
message text -- in an ID-indexed pool instead of as plain literals.  This module
reproduces `sub_406276` / `sub_79E993` / `sub_79ECDE` / `sub_79EBF3` and dumps all
5659 entries as text.

Layout (imagebase 0x400000):
  * pointer table  0xBDC9D4 .. 0xBE2240   5659 LE dwords, descending, -> blob
  * string blob    0xB22030 .. 0xB372FC   [lenByte][XOR payload][0x00]
  * key material   0xB001EC               16 bytes, transformed per entry

Per-entry transform, from `sub_79ECDE(pOut, &unk_B001EC, 16, rot)` where `rot` is
the entry's own length byte read as a **signed char**:

  1. copy the 16 key bytes into a buffer                      (sub_79E780)
  2. rotate by `(rot >> 3) % 16`, then bit-shift left `rot & 7`
     with the escaped high bits wrapping to the front         (sub_79EBF3)
  3. XOR the payload with `key[i % 16]`, stopping at a **raw** 0x00 in the
     ciphertext -- exactly `while (*v4 != 0)` in sub_79ECDE
  4. if a decoded byte is 0x00 the client stores the key byte instead
     (`if (v6 == *v4) *v4 = v6;`), so a decoded NUL never ends the string

Step 3/4 matters: a plain "stop when the decrypted byte is 0" rule silently
truncates names such as `attack1`, `attack7`, `regen`, `MapleStory Error : ...`.

Usage:
    python tools/beidou_stringpool.py [path\\to\\BeiDou.exe] > tools/_out/STRINGS2.tsv
    # columns: id <TAB> string   (latin-1; the pool is byte-oriented)

Analysis write-up: docs/features/native-map-object-re/README.md
"""
import struct
import sys

TABLE = 0xBDC9D4
BLOB_LO, BLOB_HI = 0xB00000, 0xBD7000
KEYBASE = 0xB001EC
KLEN = 16

# (file offset, raw size, virtual address)
_SECTIONS = [
    (0x1000, 0x6EF000, 0x401000),     # .text
    (0x6F0000, 0xE7000, 0xAF0000),    # .rdata
    (0x7D7000, 0x11000, 0xBD7000),    # .data
]


def _v2o(v):
    for rawoff, rawsz, vva in _SECTIONS:
        if vva <= v < vva + rawsz:
            return rawoff + (v - vva)
    return None


class Pool:
    def __init__(self, data):
        self.d = data
        self.key0 = bytearray(self._read(KEYBASE, KLEN))
        self.ptrs = self._pointers()

    def __len__(self):
        return len(self.ptrs)

    def _read(self, v, n):
        o = _v2o(v)
        return self.d[o:o + n] if o is not None else b""

    def _pointers(self):
        out = []
        for i in range(20000):
            b = self._read(TABLE + i * 4, 4)
            if len(b) < 4:
                break
            p = struct.unpack("<I", b)[0]
            if not (BLOB_LO <= p < BLOB_HI):
                break
            out.append(p)
        return out

    def _key(self, rot):
        """sub_79E780 (copy) followed by sub_79EBF3(buf, 16, rot)."""
        k = bytearray(self.key0)
        a2, a3 = KLEN, rot & 0xFFFFFFFF
        if not a2 or not a3:
            return k
        if a3 >= 8:
            off = (a3 >> 3) % a2
            if off:
                k = bytearray(k[(i + off) % a2] for i in range(a2))
            a3 &= 7
        if a3:
            carry = k[0] >> (8 - a3) if a2 > 1 else 0
            out = bytearray(a2)
            for j in range(a2):
                nxt = 0 if j == a2 - 1 else k[j + 1] >> (8 - a3)
                out[j] = (nxt | ((k[j] << a3) & 0xFF)) & 0xFF
            out[a2 - 1] |= carry
            k = out
        return k

    def decode(self, i):
        p = self.ptrs[i]
        ln = self._read(p, 1)[0]
        key = self._key(ln if ln < 128 else ln - 256)   # signed-char rotation
        hi = self.ptrs[i - 1] if i > 0 else p + 400
        raw = self._read(p + 1, max(0, min(hi - (p + 1), 400)))
        out = bytearray()
        for j, c in enumerate(raw):
            if c == 0:                    # stop on the RAW zero (strlen on ciphertext)
                break
            x = (c ^ key[j % KLEN]) & 0xFF
            if x == 0:
                x = key[j % KLEN]         # sub_79ECDE zero substitution
            out.append(x)
        return bytes(out)

    def text(self, i, encoding="latin-1"):
        return self.decode(i).decode(encoding, "replace")


def dump(path):
    pool = Pool(open(path, "rb").read())
    for i in range(len(pool)):
        sys.stdout.write("%d\t%s\n" % (i, pool.text(i).replace("\t", " ")))


if __name__ == "__main__":
    dump(sys.argv[1] if len(sys.argv) > 1 else r"E:\MXD\BeiDou-ClientV17.7\BeiDou.exe")
