# -*- coding: utf-8 -*-
import re
import sys

path = r"E:\MXD\20dalu_079MS\20dalu\wz\UI.wz\UIWindow.img.xml"
with open(path, "rb") as f:
    text = f.read().decode("utf-8", "replace")

def section(name, nbytes=200000):
    m = re.search(r'<imgdir name="%s">' % name, text)
    if not m:
        print(name, "MISSING")
        return ""
    return text[m.start(): m.start() + nbytes]

def dump_btn(chunk, btn):
    # find first occurrence of button imgdir or canvas
    bm = re.search(r'<imgdir name="%s">' % btn, chunk)
    if not bm:
        bm = re.search(r'<canvas name="%s"[^>]*>' % btn, chunk)
    if not bm:
        print(" ", btn, "MISSING")
        return
    sub = chunk[bm.start(): bm.start() + 2500]
    origins = re.findall(r'<vector name="origin" x="(-?\d+)" y="(-?\d+)"/>', sub)
    dims = re.findall(r'width="(\d+)" height="(\d+)"', sub)
    print(" ", btn, "origins", origins[:8], "dims", dims[:6])

for name in ["KeyConfig", "Item", "Skill", "Equip", "Quest", "QuestAlarm"]:
    chunk = section(name)
    if not chunk:
        continue
    print("====", name, "len", len(chunk))
    # top-level canvases
    for c in re.findall(r'<canvas name="([^"]+)" width="(\d+)" height="(\d+)">', chunk[:5000]):
        print(" canvas", c)
    if name == "KeyConfig":
        for btn in ["BtOK", "BtCancel", "BtDefault", "BtDelete", "BtClose", "backgrnd", "backgrnd2", "backgrnd3"]:
            dump_btn(chunk, btn)
    if name == "Item":
        for btn in ["BtCoin", "BtGather", "BtSort", "BtFull", "BtCashshop", "BtClose", "backgrnd", "Tab"]:
            dump_btn(chunk, btn)
        # Tab children origins
        tm = re.search(r'<imgdir name="Tab">', chunk)
        if tm:
            tchunk = chunk[tm.start(): tm.start()+15000]
            origins = re.findall(r'<vector name="origin" x="(-?\d+)" y="(-?\d+)"/>', tchunk)
            print("  Tab origins sample", origins[:20])
    if name == "Skill":
        for btn in ["BtSpUp", "backgrnd", "Tab", "skill0", "skill1"]:
            dump_btn(chunk, btn)
        tm = re.search(r'<imgdir name="Tab">', chunk)
        if tm:
            tchunk = chunk[tm.start(): tm.start()+15000]
            origins = re.findall(r'<vector name="origin" x="(-?\d+)" y="(-?\d+)"/>', tchunk)
            print("  Tab origins sample", origins[:20])
    if name == "Equip":
        for btn in ["backgrnd", "BtClose", "Tab"]:
            dump_btn(chunk, btn)
    if name == "Quest":
        for btn in ["backgrnd", "BtGiveup", "BtSearch", "Tab"]:
            dump_btn(chunk, btn)
        tm = re.search(r'<imgdir name="Tab">', chunk)
        if tm:
            tchunk = chunk[tm.start(): tm.start()+15000]
            origins = re.findall(r'<vector name="origin" x="(-?\d+)" y="(-?\d+)"/>', tchunk)
            print("  Tab origins sample", origins[:20])
    if name == "QuestAlarm":
        for c in re.findall(r'<canvas name="([^"]+)" width="(\d+)" height="(\d+)">', chunk[:8000]):
            print(" canvas", c)
        origins = re.findall(r'<vector name="origin" x="(-?\d+)" y="(-?\d+)"/>', chunk[:8000])
        print(" origins", origins[:30])
