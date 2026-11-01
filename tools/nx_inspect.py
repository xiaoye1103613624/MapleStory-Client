#!/usr/bin/env python3
"""Read-only inspector for the client's NX (PKG4) data files.

Useful when a layout question can only be answered by looking at the actual
artwork (sizes, transparent "page window", node structure) instead of guessing
from the code.

Usage
-----
    python tools/nx_inspect.py list  <file.nx> <depth> <node/path> [more/paths...]
    python tools/nx_inspect.py size  <file.nx> <WxH> [more sizes...]
    python tools/nx_inspect.py image <file.nx> <node/path> <out.png>

Examples
--------
    python tools/nx_inspect.py list  E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx 2 Login.img/Common/frame
    python tools/nx_inspect.py size  E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx 1024x768
    python tools/nx_inspect.py image E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx Login.img/Common/frame frame.png

Notes on the on-disk format (PKG4, as produced by NoLifeNx):
    header: "PKG4", u32 node_count, u32 header_size(64), ...,
            u32 string_count, u32 string_table_offset, ...,
            u32 bitmap_count, u32 bitmap_data_offset(@0x20), u32 bitmap_index(@0x2C)
    node 20B: u32 name_index, u32 first_child, u16 child_count, u16 type,
              u32 data, u16 width, u16 height
    strings start with a u16 length prefix; bitmaps are LZ4 blocks
    (4 byte compressed length, then the block) of 4 bytes/pixel BGRA.
"""
import mmap
import struct
import sys
import zlib

TYPE_NAMES = {
    0: "null", 1: "int", 2: "real", 3: "string",
    4: "vector", 5: "bitmap", 6: "audio",
}


class Nx:
    def __init__(self, path):
        self._fh = open(path, "rb")
        self.data = mmap.mmap(self._fh.fileno(), 0, access=mmap.ACCESS_READ)
        self.node_count = struct.unpack_from("<I", self.data, 4)[0]
        self.string_count, self.string_table = struct.unpack_from("<II", self.data, 0x10)
        self.bitmap_count = struct.unpack_from("<I", self.data, 0x1C)[0]
        self.bitmap_data = struct.unpack_from("<I", self.data, 0x20)[0]
        offsets = struct.unpack_from("<%dQ" % self.string_count, self.data, self.string_table)
        self.strings = [self._read_string(o) for o in offsets]

    def _read_string(self, off):
        length = struct.unpack_from("<H", self.data, off)[0]
        return self.data[off + 2: off + 2 + length].decode("utf-8", "replace")

    def node(self, i):
        off = 64 + i * 20
        name, first_child = struct.unpack_from("<II", self.data, off)
        count, ntype = struct.unpack_from("<HH", self.data, off + 8)
        data = struct.unpack_from("<I", self.data, off + 12)[0]
        width, height = struct.unpack_from("<HH", self.data, off + 16)
        return name, first_child, count, ntype, data, width, height

    def name(self, i):
        index = self.node(i)[0]
        return self.strings[index] if index < len(self.strings) else "?%d" % index

    def children(self, i):
        _, first, count, _, _, _, _ = self.node(i)
        return range(first, first + count) if count else range(0)

    def value(self, i):
        _, _, _, ntype, data, width, height = self.node(i)
        if ntype == 1:
            return str(data)
        if ntype == 2:
            return str(struct.unpack_from("<d", self.data, data)[0])
        if ntype == 3:
            return repr(self.strings[data] if data < len(self.strings) else self._read_string(data))
        if ntype == 4:
            # A PKG4 vector occupies 8 bytes of the node record: x = int32 @ +12
            # (the "data" slot) and y = int32 @ +16 (the "width"+"height" slot).
            # Reading only `data` -- as two int16 halves or as a file offset --
            # makes every origin come out as (x, 0) / (x, -1), which is how the
            # world-map BaseImg origin (320, 235) once looked like (320, 0).
            off = 64 + i * 20
            x, y = struct.unpack_from("<ii", self.data, off + 12)
            return "(%d, %d)" % (x, y)
        if ntype == 5:
            return "bitmap %dx%d" % (width, height)
        if ntype == 6:
            return "audio"
        return ""

    def find(self, path):
        i = 0
        for part in path.split("/"):
            hit = None
            for child in self.children(i):
                if self.name(child) == part:
                    hit = child
                    break
            if hit is None:
                return None
            i = hit
        return i

    def path_of(self, i):
        parent = {}
        for n in range(self.node_count):
            _, first, count, _, _, _, _ = self.node(n)
            for c in range(first, first + count):
                parent[c] = n
        parts = []
        while i > 0:
            parts.append(self.name(i))
            i = parent[i]
        return "/".join(reversed(parts))

    def bitmap_offset(self, i):
        _, _, _, ntype, index, _, _ = self.node(i)
        if ntype != 5:
            return None
        return struct.unpack_from("<Q", self.data, self.bitmap_data + index * 8)[0]

    def bitmap_pixels(self, i):
        """Return (width, height, bgra_bytes) for a bitmap node."""
        _, _, _, ntype, _, width, height = self.node(i)
        if ntype != 5:
            return None, None, None
        off = self.bitmap_offset(i)
        compressed = struct.unpack_from("<I", self.data, off)[0]
        target = 4 * width * height
        raw = lz4_decompress(bytes(self.data[off + 4: off + 4 + compressed]), target)
        return width, height, raw[:target]


def lz4_decompress(src, dst_size):
    """Minimal LZ4 block decompressor (enough for NoLifeNx bitmap payloads)."""
    out = bytearray()
    i = 0
    while i < len(src):
        token = src[i]
        i += 1
        length = token >> 4
        if length == 15:
            while True:
                b = src[i]
                i += 1
                length += b
                if b != 255:
                    break
        out += src[i:i + length]
        i += length
        if i + 2 > len(src) or len(out) >= dst_size:
            break
        offset = src[i] | (src[i + 1] << 8)
        i += 2
        if offset == 0:
            raise ValueError("invalid LZ4 offset")
        match = token & 0xF
        if match == 15:
            while True:
                b = src[i]
                i += 1
                match += b
                if b != 255:
                    break
        start = len(out) - offset
        for _ in range(match + 4):
            out.append(out[start])
            start += 1
    return bytes(out)


def write_png(path, width, height, bgra):
    rows = bytearray()
    for y in range(height):
        rows.append(0)
        line = bgra[y * width * 4:(y + 1) * width * 4]
        for x in range(width):
            b, g, r, a = line[x * 4:x * 4 + 4]
            rows += bytes((r, g, b, a))

    def chunk(tag, payload):
        return (struct.pack(">I", len(payload)) + tag + payload
                + struct.pack(">I", zlib.crc32(tag + payload) & 0xFFFFFFFF))

    png = b"\x89PNG\r\n\x1a\n"
    png += chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0))
    png += chunk(b"IDAT", zlib.compress(bytes(rows)))
    png += chunk(b"IEND", b"")
    with open(path, "wb") as f:
        f.write(png)


def cmd_list(nx, args):
    depth = int(args[0])
    for path in args[1:]:
        index = nx.find(path)
        print("=====", path)
        if index is None:
            print("NOT FOUND")
            continue

        def walk(node, level):
            line = "%s [%s] %s" % (
                nx.name(node),
                TYPE_NAMES.get(nx.node(node)[3], nx.node(node)[3]),
                nx.value(node)
            )
            print("  " * level + line.rstrip())
            if level < depth:
                for child in nx.children(node):
                    walk(child, level + 1)

        walk(index, 0)


def cmd_size(nx, args):
    wanted = set()
    for arg in args:
        w, h = arg.lower().split("x")
        wanted.add((int(w), int(h)))
    hits = 0
    for i in range(nx.node_count):
        _, _, _, ntype, _, width, height = nx.node(i)
        if ntype == 5 and (width, height) in wanted:
            print("%d x %d %s" % (width, height, nx.path_of(i)))
            hits += 1
    print("total", hits)


def cmd_image(nx, args):
    path, out = args[0], args[1]
    index = nx.find(path)
    if index is None:
        print("NOT FOUND", path)
        return
    width, height, pixels = nx.bitmap_pixels(index)
    if pixels is None:
        print("NOT A BITMAP", path)
        return
    write_png(out, width, height, pixels)
    alpha = pixels[3::4]
    print("wrote", out, "%dx%d alpha=%d..%d" % (width, height, min(alpha), max(alpha)))


def main():
    if len(sys.argv) < 4:
        print(__doc__)
        return 1

    nx = Nx(sys.argv[2])
    command = sys.argv[1]

    if command == "list":
        cmd_list(nx, sys.argv[3:])
    elif command == "size":
        cmd_size(nx, sys.argv[3:])
    elif command == "image":
        cmd_image(nx, sys.argv[3:])
    else:
        print(__doc__)
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
