# qf_s17_crop.py key x0 y0 x1 y1 scale name
import sys, hashlib, os
from PIL import Image
ROOT = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-qf-work"
MD5 = {
 'img1':'7e736cebeff4d5db35bbfc9ec109ea65','img2':'e4d03c62b88c8eef93904e7c663a79b9',
 'img3':'ca6baeb3ecdf8b0d1ea71eef72455332','img4':'44c1ed4c33415e1fae928645b26bbda8',
 'img5':'6f611be2682cc310d7f59b29ac923e9e','img6':'f7dba52544ae90d7929dd56be6b66cdf',
 'img7':'1a7090c2cadcb4560f4e716fbdd4e1fc','img8':'f7069a850077905ea355a02b10eec84d',
 'img9':'3875b92b444caa39b78bad74611e989d','img10':'558c7c0d545638d3ef612408bcc64ddd',
 'img11':'ae6298dfb5418b929fbfebb93549a0c5','img12':'c1cd8eebec12ca60dbc41bdc3476ba3b',
 'img13':'ef8586a5777a2119f7c3c235df491693','img14':'7d2c9fe479c63a22b789472e374db260',
 'img15':'9ac47a7f7e0e7b1a28e1acc1da07c8ab',
}
key,x0,y0,x1,y1,sc,name = sys.argv[1], *map(int, sys.argv[2:7]), sys.argv[7]
p = ROOT + r"\src\%s.png" % key
assert hashlib.md5(open(p,'rb').read()).hexdigest() == MD5[key]
im = Image.open(p).convert('RGB').crop((x0,y0,x1+1,y1+1))
im = im.resize((im.width*sc, im.height*sc), Image.NEAREST)
d = os.path.join(ROOT,'crop'); os.makedirs(d, exist_ok=True)
fp = os.path.join(d, "%s_%s.png" % (key,name))
im.save(fp)
print(fp, im.size, "src-rect=(%d,%d)-(%d,%d)"%(x0,y0,x1,y1))
