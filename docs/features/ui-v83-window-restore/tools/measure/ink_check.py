# R27-B1b：用与渲染器同口径（FT_LOAD_RENDER 的 bitmap_top）复算各窗绘制点
import freetype

FONT = r"E:/project/MapleStory-Client/fonts/SimHei/simhei.ttf"
RENDER = freetype.FT_LOAD_RENDER

def linespace(f):
    maxrows = 0
    for c in range(32, 128):
        f.load_char(chr(c), RENDER)
        maxrows = max(maxrows, f.glyph.bitmap.rows)
    return int(maxrows * 1.35 + 1)

def maxbt(f, s):
    best = 0
    detail = []
    for ch in s:
        f.load_char(ch, RENDER)
        bt = f.glyph.bitmap_top
        detail.append("%s=%d" % (ch, bt))
        best = max(best, bt)
    return best, detail

ROWS = [
    ("Stat NAME", 11, "Admin", 40),
    ("Stat JOB", 11, "新手", 58),
    ("Stat LEVEL", 11, "1", 87),
    ("Stat HP", 11, "50 / 50", 122),
    ("Stat MP", 11, "5 / 4357", 140),
    ("Stat EXP", 11, "0 (0%)", 158),
    ("Stat FAME", 11, "0", 176),
    ("Chip JOB", 12, "新手", 40),
    ("Chip NAME", 12, "Admin", 56),
    ("Bag MESO", 11, "12345", 284),
    ("ChatBar TARGET", 11, "对所有人", 8),
    ("Skill SP", 12, "0", 274),
]

lines = []
cache = {}
for name, size, text, ink_top in ROWS:
    if size not in cache:
        f = freetype.Face(FONT)
        f.set_pixel_sizes(0, size)
        cache[size] = (f, linespace(f))
    f, ls = cache[size]
    best, detail = maxbt(f, text)
    lines.append("%-16s size%d linespace%d maxbt%d 墨迹顶%d -> draw_y %d   [%s]"
                 % (name, size, ls, best, ink_top, ink_top - (ls - best), detail))

with open(r"E:/project/_r27_ink_check.txt", "w", encoding="utf-8") as fp:
    fp.write("\n".join(lines))
