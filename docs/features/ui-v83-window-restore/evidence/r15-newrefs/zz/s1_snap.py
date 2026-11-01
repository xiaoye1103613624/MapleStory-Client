# zz/s1_snap: snapshot the 15 sources into zz/src (private dir; a concurrent
# session is deleting files in the shared evidence root). md5-verified.
import os, io, json, shutil, hashlib, time
from PIL import Image

BASE = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
ZZ = os.path.join(BASE, "zz")
REFDIR = r"C:\Users\11036\Desktop\正确客户端截图"
os.makedirs(os.path.join(ZZ, "src"), exist_ok=True)
os.makedirs(os.path.join(ZZ, "crop"), exist_ok=True)

NAMES = {
    "img1":  "角色装备栏显示物品信息.png",
    "img2":  "背包栏鼠标悬浮按钮上显示按钮功能提示信息.png",
    "img3":  "怪物手册.png",
    "img4":  "怪物手册2.png",
    "img5":  "NPC对话.png",
    "img6":  "NPC对话2.png",
    "img7":  "NPC对话框二级弹窗.png",
    "img8":  "npc任务提示1.png",
    "img9":  "NPC任务提示2.png",
    "img10": "任务告知ui.png",
    "img11": "正在进行的任务展示.png",
    "img12": "任务栏显示.png",
    "img13": "接收任务后触发的任务提示框.png",
    "img14": "角色头上提示信息.png",
    "img15": "悄悄话框.png",
}
EXPECT = {"img1": (359, 404), "img2": (175, 290), "img3": (540, 357), "img4": (520, 347),
          "img5": (523, 203), "img6": (526, 349), "img7": (527, 351), "img8": (80, 149),
          "img9": (86, 146), "img10": (224, 27), "img11": (553, 394), "img12": (543, 395),
          "img13": (526, 505), "img14": (150, 148), "img15": (261, 117)}
log = io.open(os.path.join(ZZ, "manifest.txt"), "w", encoding="utf-8")
man = {}
for key in sorted(NAMES, key=lambda k: int(k[3:])):
    src = os.path.join(REFDIR, NAMES[key])
    b = open(src, "rb").read()
    dst = os.path.join(ZZ, "src", key + ".png")
    open(dst, "wb").write(b)
    b2 = open(dst, "rb").read()
    h = hashlib.md5(b).hexdigest()
    with Image.open(dst) as im:
        sz = im.size
        mode = im.mode
    man[key] = {"name": NAMES[key], "md5": h, "size": sz, "mode": mode,
                "bytes": len(b), "ts": time.ctime(), "snap_ok": b == b2,
                "size_matches_task": sz == EXPECT[key]}
    log.write("%s file=%s bytes=%d md5=%s mode=%s size=%dx%d size_matches_task=%s snap_ok=%s\n"
              % (key, NAMES[key], len(b), h, mode, sz[0], sz[1], sz == EXPECT[key], b == b2))
json.dump(man, io.open(os.path.join(ZZ, "manifest.json"), "w", encoding="utf-8"), indent=1, ensure_ascii=False)
log.close()
print("SNAP_OK")
