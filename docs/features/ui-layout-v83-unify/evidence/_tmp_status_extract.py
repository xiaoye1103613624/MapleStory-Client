# -*- coding: utf-8 -*-
import re
import os

path = r"E:\MXD\20dalu_079MS\20dalu\wz\UI.wz\StatusBar.img.xml"
text = open(path, "rb").read().decode("utf-8", "replace")

m = re.search(r'<canvas name="quickSlot"[^>]*>.*?</canvas>', text, re.S)
print("quickSlot snippet:", m.group(0)[:500] if m else None)

for btn in [
    "EquipKey", "InvenKey", "StatKey", "SkillKey", "KeySet",
    "QuickSlot", "QuickSlotD", "BtShop", "BtMenu", "BtNPT",
    "BtShort", "BtWhisper", "BtClaim",
]:
    bm = re.search(r'<imgdir name="%s">' % btn, text)
    if not bm:
        bm = re.search(r'<canvas name="%s"[^>]*>' % btn, text)
    if not bm:
        print(btn, "MISSING")
        continue
    sub = text[bm.start(): bm.start() + 1200]
    origins = re.findall(r'origin" x="(-?\d+)" y="(-?\d+)"', sub)
    dims = re.findall(r'width="(\d+)" height="(\d+)"', sub)
    print(btn, "origins", origins[:6], "dims", dims[:4])

uw = r"E:\MXD\20dalu_079MS\20dalu\wz\UI.wz\UIWindow.img.xml"
t2 = open(uw, "rb").read().decode("utf-8", "replace")

m = re.search(r'<imgdir name="QuestAlarm">', t2)
print("QuestAlarm", "FOUND" if m else "MISSING")
if m:
    chunk = t2[m.start(): m.start() + 8000]
    print(re.findall(r'<(?:canvas|imgdir) name="([^"]+)"', chunk)[:40])
    bm = re.search(r'<canvas name="backgrnd" width="(\d+)" height="(\d+)">', chunk)
    if bm:
        print(
            "QA bg",
            bm.groups(),
            re.findall(r'origin" x="(-?\d+)" y="(-?\d+)"', chunk[bm.start(): bm.start() + 200]),
        )

m = re.search(r'<imgdir name="Quest">', t2)
chunk = t2[m.start(): m.start() + 50000]
bm = re.search(r'<canvas name="backgrnd" width="(\d+)" height="(\d+)">', chunk)
print("Quest bg", bm.groups() if bm else None)
if bm:
    print(
        " origin",
        re.findall(r'origin" x="(-?\d+)" y="(-?\d+)"', chunk[bm.start(): bm.start() + 200]),
    )

m = re.search(r'<imgdir name="Item">', t2)
ic = t2[m.start(): m.start() + 80000]
for btn in ["BtCoin", "BtGather", "BtSort", "BtFull", "BtSmall", "BtCashshop"]:
    bm = re.search(r'<imgdir name="%s">' % btn, ic)
    if not bm:
        print(btn, "no")
        continue
    sub = ic[bm.start(): bm.start() + 800]
    print(btn, "vecs", re.findall(r'<vector name="([^"]+)" x="(-?\d+)" y="(-?\d+)"', sub)[:8])

# KeyConfig backgrnd2/3 exact children
m = re.search(r'<imgdir name="KeyConfig">', t2)
kc = t2[m.start(): m.start() + 30000]
print("KC children", re.findall(r'<imgdir name="([^"]+)">', kc)[:30])
print("KC canvases", re.findall(r'<canvas name="([^"]+)" width="(\d+)" height="(\d+)">', kc)[:20])

# Skill Tab origins and BtSpUp
m = re.search(r'<imgdir name="Skill">', t2)
sk = t2[m.start(): m.start() + 60000]
print("Skill children", re.findall(r'<imgdir name="([^"]+)">', sk)[:25])
for name in ["BtSpUp", "BtMacro", "BtHyper"]:
    bm = re.search(r'<imgdir name="%s">' % name, sk)
    print(name, "found" if bm else "missing")
    if bm:
        sub = sk[bm.start(): bm.start() + 600]
        print(" ", re.findall(r'<vector name="([^"]+)" x="(-?\d+)" y="(-?\d+)"', sub)[:6])
