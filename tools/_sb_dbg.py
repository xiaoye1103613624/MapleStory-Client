#!/usr/bin/env python3
import sys, struct, zlib
sys.path.insert(0, r"e:/project/MapleStory-Client/tools")
import _wz

keys = _wz.load_keys()
_wz.keys_all = keys
data = open(r"E:/MXD/BeiDou-Client-xy/Data/UI/StatusBar.img", "rb").read()
img = _wz.Img(data)
img.parse(keys)

n = img.nodes[img.find("base/backgrnd2")]
print("meta:", {k: v for k, v in n.items() if k != "children"})
r = _wz.Reader(data)
r.seek(n["bmp_off"])
w = r.cint(); h = r.cint(); fmt1 = r.cint(); fmt2 = r.u8()
r.skip(4)
clen = r.u32()
r.skip(1)
print("w=%d h=%d fmt1=%d fmt2=%d clen=%d off=%d" % (w, h, fmt1, fmt2, clen, r.o))
payload = data[r.o:r.o + clen]
print("payload head:", payload[:16].hex())

def has_zlib(b):
    if len(b) < 2:
        return False
    cmf, flg = b[0], b[1]
    return (cmf & 0x0F) == 8 and (cmf >> 4) <= 7 and ((cmf << 8) + flg) % 31 == 0

print("direct zlib header?", has_zlib(payload))

def decrypt_blocks(buf, key):
    out = bytearray(); src = 0
    while src + 4 <= len(buf):
        block = struct.unpack_from("<I", buf, src)[0]; src += 4
        if block == 0:
            break
        if src + block > len(buf):
            return None
        for i in range(block):
            out.append(buf[src + i] ^ key[i])
        src += block
    return bytes(out)

limit = w * h * 4
for name, key in [("bms", keys[0]), ("gms", keys[1]), ("kms", keys[2])]:
    dec = decrypt_blocks(payload, key)
    if not dec:
        print(name, "decrypt fail")
        continue
    print(name, "decrypted head:", dec[:8].hex(), "zlib?", has_zlib(dec))
    try:
        d = zlib.decompressobj()
        out = d.decompress(dec, limit)
        print(name, "inflated", len(out), "of", limit)
        if len(out) >= limit:
            open(r"E:/MXD/BeiDou-Client-xy/_sb_bg2.png", "wb").write(b"")
            from PIL import Image
            im = Image.frombytes("RGBA", (w, h), out[:limit])
            bg = Image.new("RGBA", (w, h), (200, 200, 200, 255))
            bg.alpha_composite(im)
            bg.resize((w * 2, h * 2), Image.NEAREST).save(r"E:/MXD/BeiDou-Client-xy/_sb_bg2.png")
            print("SAVED")
            break
    except Exception as e:
        print(name, "inflate err:", e)
