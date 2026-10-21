# -*- coding: utf-8 -*-
import re
path = r"E:\MXD\20dalu_079MS\20dalu\wz\UI.wz\UIWindow.img.xml"
with open(path, "rb") as f:
    text = f.read().decode("utf-8", "replace")

def find_dir(parent_chunk, name):
    m = re.search(r'<imgdir name="%s">' % name, parent_chunk)
    return m.start() if m else -1

# KeyConfig precise
m = re.search(r'<imgdir name="KeyConfig">', text)
kc = text[m.start(): m.start()+250000]
# first canvas backgrnd
bm = re.search(r'<canvas name="backgrnd" width="(\d+)" height="(\d+)">', kc)
print("KeyConfig backgrnd", bm.group(1), bm.group(2) if bm else None)
sub = kc[bm.start(): bm.start()+400]
print(sub[:400])
print("origin", re.findall(r'origin" x="(-?\d+)" y="(-?\d+)"', sub))

# BtOK position - look for CreateCtrl patterns won't be in XML; check if button has lt/rb
for btn in ["BtOK","BtCancel","BtDefault","BtDelete"]:
    bm = re.search(r'<imgdir name="%s">' % btn, kc)
    sub = kc[bm.start(): bm.start()+1200]
    print(btn, "first canvas", re.search(r'<canvas name="0" width="(\d+)" height="(\d+)">', sub).groups())
    print(" ", re.findall(r'origin" x="(-?\d+)" y="(-?\d+)"', sub)[:4])

# Item Tab enabled 0 origin and also check BtCoin positions via looking at production vs classic
m = re.search(r'<imgdir name="Item">', text)
item = text[m.start(): m.start()+120000]
# Tab/enabled/0
tm = re.search(r'<imgdir name="Tab">.*?<imgdir name="enabled">.*?<canvas name="0" width="(\d+)" height="(\d+)">(.*?)</canvas>', item, re.S)
if tm:
    print("Item Tab0", tm.group(1), tm.group(2), re.findall(r'origin" x="(-?\d+)" y="(-?\d+)"', tm.group(3)))

# Skill backgrnd exact
m = re.search(r'<imgdir name="Skill">', text)
sk = text[m.start(): m.start()+80000]
bm = re.search(r'<canvas name="backgrnd" width="(\d+)" height="(\d+)">', sk)
print("Skill backgrnd", bm.groups(), re.findall(r'origin" x="(-?\d+)" y="(-?\d+)"', sk[bm.start():bm.start()+300]))

# Quest list vs flat
m = re.search(r'<imgdir name="Quest">', text)
q = text[m.start(): m.start()+100000]
print("Quest has list?", bool(re.search(r'<imgdir name="list">', q[:5000])))
print("Quest top canvases", re.findall(r'<canvas name="([^"]+)" width="(\d+)" height="(\d+)">', q[:3000])[:15])
# Tab positions - enabled canvases
for i in range(3):
    pat = r'<imgdir name="Tab">.*?<imgdir name="enabled">.*?<canvas name="%d" width="(\d+)" height="(\d+)">(.*?)</canvas>' % i
    tm = re.search(pat, q, re.S)
    if tm:
        print("Quest Tab", i, tm.group(1), tm.group(2), re.findall(r'origin" x="(-?\d+)" y="(-?\d+)"', tm.group(3)))
