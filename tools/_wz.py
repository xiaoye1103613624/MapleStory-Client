#!/usr/bin/env python3
"""Minimal reader for BeiDou standalone .img files (classic WZ img-node stream).

Ported from tools/vendor/NoLifeWzToNx-master/NoLifeWzToNx.cpp (metadata only:
node tree + canvas width/height + vector origins; bitmaps are not decoded).
"""
import re
import struct
import sys
import zlib

KEYS_CPP = r"e:/project/MapleStory-Client/tools/vendor/NoLifeWzToNx-master/Keys.cpp"


def load_keys():
    text = open(KEYS_CPP, "r", encoding="utf-8", errors="replace").read()

    def grab(name, size):
        m = re.search(r"uint8_t\s+%s\[[^\]]*\]\s*=\s*\{(.*?)\};" % name, text, re.S)
        if not m:
            raise RuntimeError("key table %s not found" % name)
        nums = re.findall(r"0x[0-9A-Fa-f]+|\b\d+\b", m.group(1))
        vals = [int(n, 16) if n.lower().startswith("0x") else int(n) for n in nums]
        if not vals:
            return bytes(size)
        if len(vals) != size:
            if len(vals) > size or not vals:
                raise RuntimeError("key %s size %d != %d" % (name, len(vals), size))
            vals += [0] * (size - len(vals))  # pad short table
        return bytes(vals)

    return [grab("key_bms", 65536), grab("key_gms", 65536), grab("key_kms", 0x20000)]


CP1252_HIGH = bytes(range(0x80, 0x100))

keys_all = []


class Reader:
    def __init__(self, data):
        self.d = data
        self.o = 0
        self.end = len(data)

    def tell(self):
        return self.o

    def seek(self, n):
        self.o = n

    def skip(self, n):
        self.o += n

    def u8(self):
        v = self.d[self.o]
        self.o += 1
        return v

    def i8(self):
        v = struct.unpack_from("<b", self.d, self.o)[0]
        self.o += 1
        return v

    def i32(self):
        v = struct.unpack_from("<i", self.d, self.o)[0]
        self.o += 4
        return v

    def u32(self):
        v = struct.unpack_from("<I", self.d, self.o)[0]
        self.o += 4
        return v

    def cint(self):
        a = self.i8()
        return a if a != -128 else self.i32()


class Img:
    def __init__(self, data):
        self.r = Reader(data)
        self.strings = [""]
        self.u8key = None
        self.u16key = None
        self.nodes = []  # dict: name, children(list of idx), type, extra
        self.root = self._new_node("")

    def _new_node(self, name):
        n = {"name": name, "children": [], "type": "none", "w": 0, "h": 0,
             "origin": None, "value": None, "bmp_off": None}
        self.nodes.append(n)
        return len(self.nodes) - 1

    def add_string(self, s):
        for i, v in enumerate(self.strings):
            if v == s:
                return i
        self.strings.append(s)
        return len(self.strings) - 1

    def read_enc_string(self):
        r = self.r
        ln = r.i8()
        if ln == 0:
            return ""
        if ln > 0:
            slen = r.u32() if ln == 127 else ln
            raw = self.r.d[self.r.o:self.r.o + slen * 2]
            r.skip(slen * 2)
            mask = 0xAAAA
            out = []
            for i in range(min(slen, 0x10000)):
                ch = struct.unpack_from("<H", raw, i * 2)[0] ^ self.u16key[i] ^ mask
                mask += 1
                out.append(ch)
            for i in range(0x10000, slen):
                ch = struct.unpack_from("<H", raw, i * 2)[0] ^ mask
                mask += 1
                out.append(ch)
            return "".join(chr(c) for c in out)
        slen = r.u32() if ln == -128 else -ln
        raw = self.r.d[self.r.o:self.r.o + slen]
        r.skip(slen)
        mask = 0xAA
        buf = bytearray(slen)
        for i in range(min(slen, 0x10000)):
            buf[i] = raw[i] ^ self.u8key[i] ^ mask
            mask += 1
        for i in range(0x10000, slen):
            buf[i] = raw[i] ^ mask
            mask += 1
        if any(b >= 0x80 for b in buf):
            return bytes(buf).decode("cp1252", "replace")
        return buf.decode("ascii", "replace")

    def read_prop_string(self, p_offset):
        a = self.r.u8()
        if a in (0x00, 0x73):
            return self.read_enc_string()
        if a in (0x01, 0x1B):
            o = self.r.i32() + p_offset
            p = self.r.tell()
            self.r.seek(o)
            s = self.read_enc_string()
            self.r.seek(p)
            return s
        raise RuntimeError("Unknown property string type: %d @%d" % (a, self.r.tell()))

    def deduce_key(self, keys):
        r = self.r
        ln = r.i8()
        if ln >= 0:
            raise RuntimeError("deduce_key: positive length")
        slen = r.u32() if ln == -128 else -ln
        os_ = r.d[r.o:r.o + slen]
        for key in keys:
            mask = 0xAA
            valid = True
            for i in range(slen):
                c = os_[i] ^ key[i] ^ mask
                mask += 1
                if c < 0x20 or c >= 0x80:
                    valid = False
                    break
            if valid:
                self.u8key = key
                self.u16key = key
                break
        if not self.u8key:
            raise RuntimeError("Failed to identify the locale")
        r.skip(slen)

    def parse(self, keys):
        r = self.r
        p = r.tell()
        if r.u8() == 1:
            raise RuntimeError("lua script img not supported")
        # deduce_key runs at p+1 (after the first type byte), then re-seek to p.
        self.deduce_key(keys)
        r.seek(p)
        self.extended_property(self.root, p)

    def extended_property(self, node_idx, p_offset):
        r = self.r
        st = self.read_prop_string(p_offset)
        n = self.nodes[node_idx]
        if st == "Property":
            r.skip(2)
            self.sub_property(node_idx, p_offset)
        elif st == "Canvas":
            r.skip(1)
            if r.u8() == 1:
                r.skip(2)
                self.sub_property(node_idx, p_offset)
            nn = self.nodes[node_idx]
            nn["type"] = "bitmap"
            nn["bmp_off"] = r.tell()  # payload starts at width cint
            nn["w"] = r.cint()
            nn["h"] = r.cint()
        elif st == "Shape2D#Vector2D":
            n["type"] = "vector"
            n["origin"] = (r.cint(), r.cint())
        elif st == "Shape2D#Convex2D":
            count = r.cint()
            kids = []
            for i in range(count):
                kids.append(self._new_node(str(i)))
                self.extended_property(kids[-1], p_offset)
            n["children"] = kids
        elif st == "Sound_DX8":
            n["type"] = "audio"
            r.skip(1)
            r.cint()
            r.cint()
        elif st == "UOL":
            r.skip(1)
            n["type"] = "uol"
            n["value"] = self.read_prop_string(p_offset)
        else:
            raise RuntimeError("Unknown sub property type: %r @%d" % (st, r.tell()))

    def sub_property(self, node_idx, p_offset):
        r = self.r
        count = r.cint()
        n = self.nodes[node_idx]
        kids = []
        for i in range(count):
            name = self.read_prop_string(p_offset)
            t = r.u8()
            idx = self._new_node(name)
            nn = self.nodes[idx]
            kids.append(idx)
            if t == 0x00:
                nn["type"] = "integer"
                nn["value"] = i
            elif t in (0x0B, 0x02):
                nn["type"] = "integer"
                nn["value"] = struct.unpack_from("<H", r.d, r.o)[0]
                r.skip(2)
            elif t == 0x03:
                nn["type"] = "integer"
                nn["value"] = r.cint()
            elif t == 0x04:
                nn["type"] = "real"
                num = r.u8()
                nn["value"] = struct.unpack_from("<f", r.d, r.o)[0] if num == 0x80 else struct.unpack_from("<b", r.d, r.o - 1)[0]
                if num == 0x80:
                    r.skip(4)
            elif t == 0x05:
                nn["type"] = "real"
                nn["value"] = struct.unpack_from("<d", r.d, r.o)[0]
                r.skip(8)
            elif t == 0x08:
                nn["type"] = "string"
                nn["value"] = self.read_prop_string(p_offset)
            elif t == 0x09:
                p = r.i32() + r.tell()
                self.extended_property(idx, p_offset)
                r.seek(p)
            elif t == 0x13:
                nn["type"] = "integer"
                nn["value"] = r.cint()
            elif t == 0x14:
                nn["type"] = "integer"
                num = r.u8()
                if num == 0x80:
                    nn["value"] = struct.unpack_from("<q", r.d, r.o)[0]
                    r.skip(8)
                else:
                    nn["value"] = struct.unpack_from("<b", r.d, r.o - 1)[0]
            else:
                raise RuntimeError("Unknown sub property type: %d @%d" % (t, r.tell()))
        n["children"] = kids

    # ---- navigation ----
    def find(self, path):
        idx = self.root
        for part in path.split("/"):
            if not part:
                continue
            hit = None
            for c in self.nodes[idx]["children"]:
                if self.nodes[c]["name"] == part:
                    hit = c
                    break
            if hit is None:
                return None
            idx = hit
        return idx

    def decode_bitmap(self, idx):
        """Decode a Canvas node into (w, h, RGBA bytes). Mirrors
        wz_file::decode_bitmap in Util/WzRuntime.cpp (zlib payload, fmt1=2)."""
        n = self.nodes[idx]
        if n["type"] != "bitmap":
            return None
        r = Reader(self.r.d)
        r.seek(n["bmp_off"])
        w = r.cint()
        h = r.cint()
        fmt1 = r.cint()
        fmt2 = r.u8()
        r.u32()
        clen = r.u32()
        r.u8()
        if fmt2 != 0:
            raise RuntimeError("unsupported secondary format %d" % fmt2)
        payload = self.r.d[r.o:r.o + clen]

        # fmt1=1/257 → 4444 (2 B/px); 513 → 565; 2 → 8888 (4 B/px).
        bytes_pp = 2 if fmt1 in (1, 257, 513) else 4
        limit = w * h * bytes_pp

        def try_inflate(buf, limit):
            """Inflate a zlib stream but stop once `limit` output bytes exist.

            These v083-style canvases carry twice the declared pixel bytes
            (8 B/px); the client only consumes the first w*h*4 (see
            wz_file::decode_bitmap's avail_out). Stopping early also avoids
            the trailing Adler-32 mismatch on the unused tail."""
            d = zlib.decompressobj()
            out = bytearray()
            try:
                for i in range(0, len(buf), 8192):
                    out += d.decompress(buf[i:i + 8192], limit - len(out))
                    if len(out) >= limit or d.eof:
                        break
            except zlib.error:
                pass
            return bytes(out) if len(out) >= limit else None

        def decrypt_blocks(buf, key):
            out = bytearray()
            src = 0
            while src + 4 <= len(buf):
                block = struct.unpack_from("<I", buf, src)[0]
                src += 4
                if block == 0:
                    break
                if src + block > len(buf):
                    return None
                for i in range(block):
                    out.append(buf[src + i] ^ key[i])
                src += block
            return bytes(out)

        raw = try_inflate(payload, limit)
        if raw is None:
            # WZ block-XOR encryption: [u32 len][len bytes] chunks, key restarts per chunk.
            for key in ([self.u8key] if self.u8key else []) + [k for k in keys_all]:
                dec = decrypt_blocks(payload, key)
                if not dec:
                    continue
                raw = try_inflate(dec, limit)
                if raw is not None:
                    break
        if raw is None or len(raw) < limit:
            # Some canvases (mz-encrypted tails / odd headers) won't fully
            # inflate — keep whatever came out and pad the rest transparent.
            if raw is None:
                # last resort: no inflate at all
                raise RuntimeError("zlib inflate failed (node=%s)" % n["name"])
            raw = raw + bytes(limit - len(raw))
        if bytes_pp == 2:
            # Expand 4444 (fmt1=1/257) or 565 (fmt1=513) into RGBA.
            out = bytearray(w * h * 4)
            if fmt1 == 513:
                for p in range(w * h):
                    v = raw[p * 2] | (raw[p * 2 + 1] << 8)
                    r5 = (v >> 11) & 0x1F
                    g6 = (v >> 5) & 0x3F
                    b5 = v & 0x1F
                    out[p * 4] = r5 * 255 // 31
                    out[p * 4 + 1] = g6 * 255 // 63
                    out[p * 4 + 2] = b5 * 255 // 31
                    out[p * 4 + 3] = 255
            else:
                for p in range(w * h):
                    v = raw[p * 2] | (raw[p * 2 + 1] << 8)
                    a4 = (v >> 12) & 0xF
                    r4 = (v >> 8) & 0xF
                    g4 = (v >> 4) & 0xF
                    b4 = v & 0xF
                    out[p * 4] = r4 * 255 // 15
                    out[p * 4 + 1] = g4 * 255 // 15
                    out[p * 4 + 2] = b4 * 255 // 15
                    out[p * 4 + 3] = a4 * 255 // 15
            return w, h, bytes(out)
        if fmt1 == 2:
            return w, h, raw[:limit]
        raise RuntimeError("unsupported pixel format %d" % fmt1)

    def dump(self, idx, depth=0, maxdepth=6):
        n = self.nodes[idx]
        pad = "  " * depth
        extra = ""
        if n["type"] == "bitmap":
            extra = " [bitmap %dx%d]" % (n["w"], n["h"])
        elif n["type"] == "vector":
            extra = " [vector %s]" % (n["origin"],)
        elif n["value"] is not None:
            extra = " [%s %r]" % (n["type"], n["value"])
        print("%s%s%s" % (pad, n["name"], extra))
        if depth >= maxdepth:
            return
        for c in n["children"]:
            self.dump(c, depth + 1, maxdepth)


def write_png(path, w, h, bgra):
    import struct as _s
    rows = bytearray()
    for y in range(h):
        rows.append(0)
        line = bgra[y * w * 4:(y + 1) * w * 4]
        for x in range(w):
            b, g, r_, a = line[x * 4:x * 4 + 4]
            rows += bytes((r_, g, b, a))

    def chunk(tag, payload):
        return (_s.pack(">I", len(payload)) + tag + payload
                + _s.pack(">I", zlib.crc32(tag + payload) & 0xFFFFFFFF))

    png = b"\x89PNG\r\n\x1a\n"
    png += chunk(b"IHDR", _s.pack(">IIBBBBB", w, h, 8, 6, 0, 0, 0))
    png += chunk(b"IDAT", zlib.compress(bytes(rows)))
    png += chunk(b"IEND", b"")
    with open(path, "wb") as f:
        f.write(png)


def main():
    global keys_all
    keys = load_keys()
    keys_all = keys
    path = sys.argv[1]
    target = sys.argv[2] if len(sys.argv) > 2 else ""
    depth = int(sys.argv[3]) if len(sys.argv) > 3 else 6
    data = open(path, "rb").read()
    img = Img(data)
    img.parse(keys)

    if len(sys.argv) > 4 and sys.argv[4].endswith(".png"):
        idx = img.find(target)
        if idx is None:
            print("NOT FOUND", target)
            return 1
        w, h, bgra = img.decode_bitmap(idx)
        write_png(sys.argv[4], w, h, bgra)
        print("wrote", sys.argv[4], "%dx%d" % (w, h))
        return 0
    if target and target != "ROOT":
        idx = img.find(target)
        if idx is None:
            print("NOT FOUND", target)
            return 1
        img.dump(idx, 0, depth)
    else:
        img.dump(img.root, 0, depth)
    return 0


if __name__ == "__main__":
    sys.exit(main())
