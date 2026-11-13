# -*- coding: utf-8 -*-
"""把桌面「正确客户端截图」里登录链路 + 创建角色页目标帧按 ASCII 名归拢，避中文名/编码坑。"""
import os, shutil, json
from PIL import Image

SRC = r"C:\Users\11036\Desktop\正确客户端截图"
DST = r"C:\Users\11036\AppData\Local\Temp\term_verify\targets_desktop"
os.makedirs(DST, exist_ok=True)

MAP = {
    "登录界面.png": "desktop_login.png",
    "选择频道页.png": "desktop_channel_collapsed.png",
    "选中频道页.png": "desktop_channel_expanded.png",
    "频道变更.png": "desktop_channel_change.png",
    "选择角色页.png": "desktop_char_select.png",
    "进入游戏页.png": "desktop_ingame.png",
    "创建角色页.png": "desktop_racecreate_race.png",
    "创建冒险家角色页.png": "desktop_racecreate_adventurer.png",
    "创建骑士团页.png": "desktop_racecreate_knight.png",
    "创建骑士团2.png": "desktop_racecreate_knight2.png",
    "创建战神.png": "desktop_racecreate_aran.png",
    "创建战神2.png": "desktop_racecreate_aran2.png",
}

names = os.listdir(SRC)
print("total files:", len(names))
ok, miss = [], []
for cn, ascii_name in MAP.items():
    if cn in names:
        shutil.copy2(os.path.join(SRC, cn), os.path.join(DST, ascii_name))
        im = Image.open(os.path.join(DST, ascii_name))
        ok.append({"src": cn, "dst": ascii_name, "size": list(im.size),
                   "bytes": os.path.getsize(os.path.join(DST, ascii_name))})
    else:
        miss.append(cn)
print("copied:", len(ok), "missing:", len(miss))
for r in ok:
    print("  OK", r["src"], "->", r["dst"], r["size"], r["bytes"])
for cn in miss:
    print("  MISS repr:", repr(cn))
    for n in names:
        if cn[:3] in n:
            print("     candidate", repr(n))

with open(os.path.join(DST, "_manifest.json"), "w", encoding="utf-8") as f:
    json.dump({"source_dir": SRC, "copied": ok, "missing": miss}, f,
              ensure_ascii=False, indent=2)
print("manifest written")
