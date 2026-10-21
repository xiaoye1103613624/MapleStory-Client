#!/usr/bin/env python3
"""合成"角色能力"详情页（CMS 经典样式，素材取自 UI.nx UIWindow.img/Stat）。

输出:
  tools/stat_out/stat_page.png      成品 1x
  tools/stat_out/stat_page_2x.png   2x 放大
  tools/stat_out/stat_page.psd      Photoshop 分层文档（底图 + 3 个文字图层）

布局（1x 像素风）:
  主面板 backgrnd.png (208x347) @ (0,0)
  detail 面板 backgrnd2.png (218x203) @ (212,0)
  第三栏(复制 backgrnd2 为底) @ (434,0)：暴击/无视防御/BOSS伤害/抗性等
"""
import os
import numpy as np
from PIL import Image, ImageDraw, ImageFont

HERE = os.path.dirname(os.path.abspath(__file__))
ASSET = os.path.join(HERE, "stat_assets")
OUT = os.path.join(HERE, "stat_out")
os.makedirs(OUT, exist_ok=True)

FONT_PATH = r"C:\Windows\Fonts\simsun.ttc"
font = ImageFont.truetype(FONT_PATH, 12)
font_s = ImageFont.truetype(FONT_PATH, 10)

def pick_font(s):
    """长标签自动缩小，避免溢出 58px 标签块。"""
    return font_s if len(s) >= 5 else font

TXT = (87, 87, 87, 255)      # EMPEROR
WHITE = (255, 255, 255, 255)
FILL_BEIGE = (221, 221, 204, 255)

main_img = Image.open(os.path.join(ASSET, "backgrnd.png")).convert("RGBA")
detail_img = Image.open(os.path.join(ASSET, "backgrnd2.png")).convert("RGBA")

W = 434 + detail_img.width
H = main_img.height

# 底图（清字后）与三个文字层
base = Image.new("RGBA", (W, H), (0, 0, 0, 0))
text_main = Image.new("RGBA", (W, H), (0, 0, 0, 0))
text_detail = Image.new("RGBA", (W, H), (0, 0, 0, 0))
text_extra = Image.new("RGBA", (W, H), (0, 0, 0, 0))

def text_l(img, xy, s, fill=TXT, f=None):
    ImageDraw.Draw(img).text(xy, s, font=f or font, fill=fill)

def text_r(img, x_right, y, s, fill=TXT, f=None):
    w = ImageDraw.Draw(img).textlength(s, font=f or font)
    ImageDraw.Draw(img).text((x_right - w, y), s, font=f or font, fill=fill)

def wipe(img, x0, y0, x1, y1, col=FILL_BEIGE):
    p = img.load()
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            p[x, y] = col

# ---------- 主面板 ----------
m = main_img.copy()
MAIN_BOXES = [(31, 46), (49, 74), (77, 92), (95, 110),
              (113, 128), (131, 146), (149, 164), (167, 182)]
main_rows = [
    "萧曳",
    "英雄",
    "200",
    "-",
    "32500 / 32500",
    "10000 / 10000",
    "0.00%",
    "0",
]
for (y0, y1) in MAIN_BOXES:
    wipe(m, 58, y0 + 2, 199, y1 - 2)
# 能力值标题下 AP 框
wipe(m, 90, 198, 120, 212, (238, 238, 238, 255))
# 四行能力值
ABILITY_BOXES = [(243, 258), (261, 276), (279, 294), (297, 312)]
for (y0, y1) in ABILITY_BOXES:
    wipe(m, 58, y0 + 2, 199, y1 - 2)
base.alpha_composite(m, (0, 0))

for (y0, y1), v in zip(MAIN_BOXES, main_rows):
    text_l(text_main, (60, y0 + (y1 - y0 - 12) // 2), v)
text_l(text_main, (94, 199), "0", TXT)
ability_rows = [
    "1053 (500+553)",
    "152 (4+148)",
    "20 (4+16)",
    "35 (5+30)",
]
for (y0, y1), v in zip(ABILITY_BOXES, ability_rows):
    text_l(text_main, (60, y0 + (y1 - y0 - 12) // 2), v)

# ---------- detail / 第三栏 ----------
def prepare_panel(panel_img):
    """擦除标签文字与数值框内部，返回 (清字面板, 每行标签竖条模板)。"""
    p = panel_img.copy()
    strips = []
    for k in range(9):
        yt = 6 + 18 * k
        strip = p.crop((16, yt + 2, 17, yt + 17))
        for x in range(17, 72):
            p.paste(strip, (x, yt + 2))
        wipe(p, 78, yt + 3, 205, yt + 16)
        strips.append(strip)
    return p, strips

DETAIL_ROWS = [
    ("攻击力",   "25000 ~ 35000"),
    ("物理防御", "15000"),
    ("魔法力",   "500 ~ 800"),
    ("魔法防御", "8000"),
    ("命中率",   "8000"),
    ("回避率",   "5000"),
    ("手技",     "25"),
    ("移动速度", "140%"),
    ("跳跃力",   "120%"),
]
EXTRA_ROWS = [
    ("暴击率",       "50%"),
    ("暴击伤害",     "15.00%"),
    ("无视防御",     "30%"),
    ("BOSS攻击力",   "30%"),
    ("伤害",         "0%"),
    ("最终伤害",     "0.00%"),
    ("攻击速度",     "普通 (6)"),
    ("状态异常抗性", "40"),
    ("击退抗性",     "20%"),
]

d, d_strips = prepare_panel(detail_img)
t, t_strips = prepare_panel(detail_img.copy())
base.alpha_composite(d, (212, 0))
base.alpha_composite(t, (434, 0))

def fill_rows(text_layer, x_base, rows, strips):
    for k, (label, val) in enumerate(rows):
        yt = 6 + 18 * k
        f = pick_font(label)
        dy = 2 if len(label) >= 5 else 1
        text_l(text_layer, (x_base + 19, yt + dy), label, WHITE, f)
        text_r(text_layer, x_base + 205, yt + 2, val, TXT)

fill_rows(text_detail, 212, DETAIL_ROWS, d_strips)
fill_rows(text_extra, 434, EXTRA_ROWS, t_strips)

# ---------- 输出 PNG ----------
out_png = os.path.join(OUT, "stat_page.png")
final = base.copy()
final.alpha_composite(text_main)
final.alpha_composite(text_detail)
final.alpha_composite(text_extra)
final.save(out_png)
final.resize((W * 2, H * 2), Image.NEAREST).save(os.path.join(OUT, "stat_page_2x.png"))
print("wrote", out_png, final.size)

# ---------- 输出 PSD ----------
from pytoshop.user import nested_layers
from pytoshop import enums

def to_layer(img, name):
    arr = np.asarray(img)  # HxWx4 RGBA
    return nested_layers.Image(
        name=name,
        visible=True,
        opacity=255,
        top=0,
        left=0,
        channels={
            enums.ChannelId.red: arr[:, :, 0],
            enums.ChannelId.green: arr[:, :, 1],
            enums.ChannelId.blue: arr[:, :, 2],
            enums.ChannelId.transparency: arr[:, :, 3],
        },
    )

layers = [
    to_layer(text_extra, "文字-扩展与抗性"),
    to_layer(text_detail, "文字-属性详情"),
    to_layer(text_main, "文字-主面板"),
    to_layer(base, "底图"),
]
psd = nested_layers.nested_layers_to_psd(
    layers, color_mode=enums.ColorMode.rgb, compression=enums.Compression.raw)
out_psd = os.path.join(OUT, "stat_page.psd")
with open(out_psd, "wb") as f:
    psd.write(f)
print("wrote", out_psd)
