# r15a_s2_probe: is the P-mode "alpha" real or a PIL/tRNS artifact?
import os, io, struct, zlib, json
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
REFDIR = r"C:\Users\11036\Desktop\正确客户端截图"
log = io.open(os.path.join(OUTDIR, "_b_probe.txt"), "w", encoding="utf-8")

def chunks(path):
    d = open(path, "rb").read()
    assert d[:8] == b"\x89PNG\r\n\x1a\n"
    i = 8
    out = []
    while i < len(d):
        ln = struct.unpack(">I", d[i:i+4])[0]
        typ = d[i+4:i+8].decode("ascii", "replace")
        data = d[i+8:i+8+ln]
        out.append((typ, data))
        i += 12 + ln
    return out

for name in ["怪物手册.png", "怪物手册2.png", "角色装备栏显示物品信息.png",
             "背包栏鼠标悬浮按钮上显示按钮功能提示信息.png", "NPC对话.png"]:
    p = os.path.join(REFDIR, name)
    log.write("== %s ==\n" % name)
    cs = chunks(p)
    for typ, data in cs:
        if typ == "IHDR":
            w, h, depth, ctype, comp, filt, inter = struct.unpack(">IIBBBBB", data)
            log.write("  IHDR w=%d h=%d depth=%d colortype=%d interlace=%d\n" % (w, h, depth, ctype, inter))
        elif typ == "PLTE":
            log.write("  PLTE n=%d entries, first6=%s\n" % (len(data) // 3, list(data[:18])))
        elif typ == "tRNS":
            nz = sum(1 for b in data if b != 255)
            zeros = sum(1 for b in data if b == 0)
            log.write("  tRNS len=%d count(b!=255)=%d count(b==0)=%d first16=%s\n"
                      % (len(data), nz, zeros, list(data[:16])))
        elif typ == "IDAT":
            log.write("  IDAT len=%d\n" % len(data))
    with Image.open(p) as im:
        log.write("  PIL mode=%s info=%s\n" % (im.mode, {k: (len(repr(v)) if isinstance(v, (bytes, bytearray)) else v) for k, v in im.info.items()}))
        tr = im.info.get("transparency")
        if isinstance(tr, int):
            log.write("  transparency index=%d\n" % tr)
        elif isinstance(tr, bytes):
            log.write("  transparency bytes len=%d hist: zeros=%d non255=%d\n" % (len(tr), tr.count(0), sum(1 for x in tr if x != 255)))
        idx = im.getextrema()
        log.write("  P extrema=%s\n" % (idx,))
        rgba = im.convert("RGBA")
        # how many distinct (r,g,b,a) palette entries survive
        c = rgba.getcolors(maxcolors=1000000)
        c.sort(reverse=True)
        log.write("  distinct RGBA=%d top=%s\n" % (len(c), c[:8]))
        # manual decode ignoring tRNS: build RGB via palette only
        pal = im.getpalette()[:768]
        px = list(im.getdata())
        from collections import Counter
        cc = Counter(px)
        log.write("  top P indices=%s\n" % cc.most_common(8))
        # composite onto black with real tRNS and save a crop to inspect visually
        base = Image.new("RGB", im.size, (255, 0, 255))
        base.paste(im.convert("RGB"), (0, 0))
        base.crop((0, 0, min(220, im.size[0]), min(150, im.size[1]))).save(
            os.path.join(OUTDIR, "_b_probe_%s" % name.replace(".png", "") + "_magenta.png"))
        # alpha-over-magenta version
        a = rgba.getchannel("A")
        log.write("  alpha extrema=%s ; alpha hist top=%s\n" % (a.getextrema(), sorted(Counter(a.getdata()).items(), key=lambda kv: -kv[1])[:6]))
log.close()
print("OK")
