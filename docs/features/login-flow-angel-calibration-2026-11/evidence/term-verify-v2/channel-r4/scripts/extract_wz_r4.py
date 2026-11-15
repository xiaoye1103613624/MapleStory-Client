import json, base64, os, io
from PIL import Image

SPILL = r"C:\Users\11036\AppData\Local\Temp\dsh-spill-xgIqGu\session-79253882d1e2\0bbcbb40331d-mcp__orange_wz__get_node_detail.txt"
OUT = r"C:\Users\11036\AppData\Local\Temp\term_verify\ui_r4\wz"
os.makedirs(OUT, exist_ok=True)

with open(SPILL, "r", encoding="utf-8") as f:
    data = json.loads(f.read())

items = []
for r in data["results"]:
    v = r["detail"]["value"]
    b64 = v.get("png")
    if not b64:
        continue
    np = r["nodePath"].replace("/", "_")
    im = Image.open(io.BytesIO(base64.b64decode(b64))).convert("RGBA")
    path = os.path.join(OUT, np + ".png")
    im.save(path)
    a = im.getchannel("A").getextrema()
    items.append((np, im.size, a))
    print(np, im.size, "alpha", a)

# contact sheet for scroll frames + balloon pieces (scaled to fit)
sel = ["WorldSelect_scroll_0_0", "WorldSelect_scroll_0_3", "WorldSelect_scroll_1_0",
       "WorldSelect_BtGoworld_normal_0", "NoticeScroll2_0", "NoticeScroll1_0",
       "WorldNotice_BalloonForLoginProcess_Title", "WorldNotice_BalloonForLoginProcess_nw",
       "WorldNotice_BalloonForLoginProcess_se", "WorldSelect_channel_0_normal"]
TW, TH = 260, 210
sheet = Image.new("RGB", (TW * 2, TH * 5), (245, 245, 245))
from PIL import ImageDraw
d = ImageDraw.Draw(sheet)
for k, name in enumerate(sel):
    p = os.path.join(OUT, name + ".png")
    if not os.path.exists(p):
        continue
    im = Image.open(p).convert("RGBA")
    im.thumbnail((TW - 8, TH - 26))
    bg = Image.new("RGBA", im.size, (255, 255, 255, 255))
    bg.alpha_composite(im)
    x, y = (k % 2) * TW + 4, (k // 2) * TH + 22
    sheet.paste(bg.convert("RGB"), (x, y))
    d.text((x, (k // 2) * TH + 4), name, fill=(0, 0, 0))
sheet.save(os.path.join(OUT, "_sheet.png"))
print("SHEET", os.path.join(OUT, "_sheet.png"), sheet.size)
