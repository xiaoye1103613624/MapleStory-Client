# r15a overview: size/mode/alpha/corner-colour for the 15 assigned screenshots.
# Console is cp936: NOTHING non-ASCII is printed; everything goes to a UTF-8 file.
import os, io, json
from PIL import Image

REFDIR = r"C:\Users\11036\Desktop\正确客户端截图"
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"

NAMES = [
    "角色装备栏显示物品信息.png",
    "背包栏鼠标悬浮按钮上显示按钮功能提示信息.png",
    "怪物手册.png",
    "怪物手册2.png",
    "NPC对话.png",
    "NPC对话2.png",
    "NPC对话框二级弹窗.png",
    "npc任务提示1.png",
    "NPC任务提示2.png",
    "任务告知ui.png",
    "正在进行的任务展示.png",
    "任务栏显示.png",
    "接收任务后触发的任务提示框.png",
    "角色头上提示信息.png",
    "悄悄话框.png",
]

def med3(im, x, y):
    # 3x3 median-ish sample (mean) around (x,y) clamped
    px = im.load()
    w, h = im.size
    vals = []
    for dy in (-1, 0, 1):
        for dx in (-1, 0, 1):
            xx = min(max(x + dx, 0), w - 1)
            yy = min(max(y + dy, 0), h - 1)
            vals.append(px[xx, yy])
    n = len(vals)
    if len(vals[0]) == 4:
        r = sum(v[0] for v in vals) // n
        g = sum(v[1] for v in vals) // n
        b = sum(v[2] for v in vals) // n
        a = sum(v[3] for v in vals) // n
        return (r, g, b, a)
    return tuple(sum(v[i] for v in vals) // n for i in range(3))

log = io.open(os.path.join(OUTDIR, "_a_overview.txt"), "w", encoding="utf-8")
paths = {}
for i, name in enumerate(NAMES, 1):
    p = os.path.join(REFDIR, name)
    paths["img%d" % i] = p
    with Image.open(p) as raw:
        fmt, mode, size = raw.format, raw.mode, raw.size
        im = raw.convert("RGBA")
        w, h = im.size
        corners = {
            "TL": med3(im, 1, 1), "TR": med3(im, w - 2, 1),
            "BL": med3(im, 1, h - 2), "BR": med3(im, w - 2, h - 2),
            "T_mid": med3(im, w // 2, 1), "B_mid": med3(im, w // 2, h - 2),
            "L_mid": med3(im, 1, h // 2), "R_mid": med3(im, w - 2, h // 2),
        }
        alphas = im.getchannel("A").getextrema()
        # count of fully-transparent pixels
        adata = im.getchannel("A").tobytes()
        ntrans = sum(1 for b in adata if b == 0)
        log.write("img%d name=%s\n" % (i, name))
        log.write("   fmt=%s raw_mode=%s converted=RGBA size=%dx%d alpha_extrema=%s n_alpha0=%d (%.1f%%)\n"
                  % (fmt, mode, w, h, alphas, ntrans, 100.0 * ntrans / (w * h)))
        for k in ("TL", "TR", "BL", "BR", "T_mid", "B_mid", "L_mid", "R_mid"):
            log.write("   corner %-6s = %s\n" % (k, corners[k]))
        # colour statistics of opaque pixels
        px = im.load()
        from collections import Counter
        c = Counter()
        for yy in range(0, h, 2):
            for xx in range(0, w, 2):
                r, g, b, a = px[xx, yy]
                if a > 0:
                    c[(r // 16 * 16, g // 16 * 16, b // 16 * 16)] += 1
        log.write("   top10 quantized colours (RGBA//16, count, sampled every 2px): %s\n"
                  % c.most_common(10))
json.dump(paths, io.open(os.path.join(OUTDIR, "_a_paths.json"), "w", encoding="utf-8"), indent=1, ensure_ascii=False)
log.close()
print("OK")
