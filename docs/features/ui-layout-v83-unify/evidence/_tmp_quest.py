# -*- coding: utf-8 -*-
import re

uw = r"E:\MXD\20dalu_079MS\20dalu\wz\UI.wz\UIWindow.img.xml"
t2 = open(uw, "rb").read().decode("utf-8", "replace")
m = re.search(r'<imgdir name="Quest">', t2)
chunk = t2[m.start() : m.start() + 100000]
print("top canvases", re.findall(r'<canvas name="([^"]+)" width="(\d+)" height="(\d+)">', chunk)[:30])
print("top imgdirs", re.findall(r'<imgdir name="([^"]+)">', chunk)[:40])
for n in ["notice0", "notice1", "notice2", "backgrnd", "backgrnd2"]:
    bm = re.search(r'<canvas name="%s" width="(\d+)" height="(\d+)">' % n, chunk)
    if not bm:
        print(n, "missing")
        continue
    o = re.findall(r'origin" x="(-?\d+)" y="(-?\d+)"', chunk[bm.start() : bm.start() + 250])
    print(n, bm.groups(), o[:1])
