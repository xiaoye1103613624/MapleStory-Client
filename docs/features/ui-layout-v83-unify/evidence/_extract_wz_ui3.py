# -*- coding: utf-8 -*-
import re
path = r"E:\MXD\20dalu_079MS\20dalu\wz\UI.wz\UIWindow.img.xml"
with open(path, "rb") as f:
    text = f.read().decode("utf-8", "replace")
m = re.search(r'<imgdir name="KeyConfig">', text)
kc = text[m.start(): m.start()+250000]
# key folder
km = re.search(r'<imgdir name="key">', kc)
key = kc[km.start(): km.start()+80000]
# sample key origins for 2,41,59,82
for kid in ['2','41','42','57','59','82','87']:
    cm = re.search(r'<canvas name="%s" width="(\d+)" height="(\d+)">' % kid, key)
    if not cm:
        print('key', kid, 'missing')
        continue
    sub = key[cm.start(): cm.start()+200]
    print('key', kid, cm.groups(), re.findall(r'origin" x="(-?\d+)" y="(-?\d+)"', sub)[:1])

# icon origins
im = re.search(r'<imgdir name="icon">', kc)
icon = kc[im.start(): im.start()+5000]
print('icon0', re.search(r'<canvas name="0"[^>]*>.*?</canvas>', icon, re.S).group(0)[:180])

# StatusBar quickSlot origin from StatusBar xml in docs
sbpath = r"E:\project\MapleStory-Client\docs\features\v83-hud-quest\evidence\StatusBar.img.xml"
with open(sbpath,'r',encoding='utf-8') as f:
    sb=f.read()
qm=re.search(r'<canvas name="quickSlot"[^>]*>.*?</canvas>', sb, re.S)
print('QS', qm.group(0) if qm else None)

# Item/Equip/Skill/Quest backgrnd origins
for name in ['Item','Equip','Skill','Quest']:
    m=re.search(r'<imgdir name="%s">'%name, text)
    chunk=text[m.start():m.start()+5000]
    bm=re.search(r'<canvas name="backgrnd" width="(\d+)" height="(\d+)">(.*?)</canvas>', chunk, re.S)
    if bm:
        print(name, 'bg', bm.group(1), bm.group(2), re.findall(r'origin" x="(-?\d+)" y="(-?\d+)"', bm.group(3)))
