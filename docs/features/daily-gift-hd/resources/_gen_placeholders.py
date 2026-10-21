# Generate Daily Gift placeholder PNGs into this directory tree.
# Usage: python _gen_placeholders.py
from PIL import Image, ImageDraw, ImageFont
import os

root = os.path.dirname(os.path.abspath(__file__))
dirs = ['digits', 'chrome', 'slots', 'progress', 'buttons', 'preview', 'source']
for d in dirs:
    os.makedirs(os.path.join(root, d), exist_ok=True)

def try_font(size):
    for p in [r'C:\\Windows\\Fonts\\msyhbd.ttc', r'C:\\Windows\\Fonts\\msyh.ttc',
              r'C:\\Windows\\Fonts\\arialbd.ttf', r'C:\\Windows\\Fonts\\arial.ttf']:
        if os.path.exists(p):
            try: return ImageFont.truetype(p, size)
            except Exception: pass
    return ImageFont.load_default()

def save(img, rel):
    img.save(os.path.join(root, rel), 'PNG')
    print('wrote', rel)

font_day = try_font(22)
font_date = try_font(18)
for d in range(10):
    s = str(d)
    im = Image.new('RGBA', (20, 28), (0,0,0,0)); dr = ImageDraw.Draw(im)
    bbox = dr.textbbox((0,0), s, font=font_day); tw, th = bbox[2]-bbox[0], bbox[3]-bbox[1]
    x,y = (20-tw)//2-bbox[0], (28-th)//2-bbox[1]
    dr.text((x+1,y+1), s, font=font_day, fill=(0,0,0,160)); dr.text((x,y), s, font=font_day, fill=(255,255,255,255))
    save(im, f'digits/digit_{d}.png')
    im2 = Image.new('RGBA', (16, 22), (0,0,0,0)); dr2 = ImageDraw.Draw(im2)
    bbox = dr2.textbbox((0,0), s, font=font_date); tw, th = bbox[2]-bbox[0], bbox[3]-bbox[1]
    x,y = (16-tw)//2-bbox[0], (22-th)//2-bbox[1]
    dr2.text((x+1,y+1), s, font=font_date, fill=(0,40,80,180)); dr2.text((x,y), s, font=font_date, fill=(255,255,255,255))
    save(im2, f'digits/digit_date_{d}.png')

print('done digits + reopen script for full set if needed')
