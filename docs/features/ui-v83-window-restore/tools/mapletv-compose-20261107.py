# -*- coding: utf-8 -*-
"""按运行时 NX 的原生偏移合成 MapleTV 双屏预期图（只读，不改游戏数据）。

用法：python docs/features/ui-v83-window-restore/tools/mapletv-compose-20261107.py

证据链：
  Data/.nxcache/Npc.nx  9250025.img/info（MapleTV / msgX,msgY / adX,adY）
  Data/.nxcache/Npc.nx  9250025.img/stand/0（442x570，origin 321,569）
  Data/.nxcache/UI.nx   MapleTV.img/TVbasic/0（240x90）、TVmedia/0/0（240x180）
  s9 gms-server/wz/Npc.wz/927*.img.xml：同类模板的 short 偏移（msgX 是 short）
  原生解密串：UI/MapleTV.img/TVbasic/0、UI/MapleTV.img/TVmedia/%d
"""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[4]
sys.path.insert(0, str(ROOT / "tools"))
from nx_inspect import Nx  # noqa: E402
from PIL import Image  # noqa: E402

NX_DIR = Path(r"E:\MXD\BeiDou-Client-xy\Data\.nxcache")
OUT = ROOT / "docs" / "features" / "ui-v83-window-restore" / "evidence" / "mapletv-20261107"
OUT.mkdir(parents=True, exist_ok=True)


def rgba(nx, node):
    w, h, bgra = nx.bitmap_pixels(node)
    if w is None:
        raise ValueError("not a bitmap")
    return Image.frombytes("RGBA", (w, h), bgra, "raw", "BGRA")


def i16(v):
    """WZ short 被 .nx 读成 0x0000FFxx / 0xFFFFFFxx 时按 int16 还原。

    9250025 的 msgX 原始值 65304(0xFF18) → -232；adX 4294967059(0xFFFFFF13) → -237。
    """
    v = int(v) & 0xFFFF
    return v - 0x10000 if v > 0x7FFF else v


npcs = Nx(NX_DIR / "Npc.nx")
ui = Nx(NX_DIR / "UI.nx")

info = npcs.find("9250025.img/info")
stand = npcs.find("9250025.img/stand/0")

# PKG4 vector 的 x/y 占节点记录 +12..+19 两个 int32（见 nx_inspect.value）。
origin_node = next(c for c in npcs.children(stand) if npcs.name(c) == "origin")
ox, oy = struct.unpack_from("<ii", npcs.data, 64 + origin_node * 20 + 12)

vals = {}
for c in npcs.children(info):
    name = npcs.name(c)
    if name.startswith("MapleTV"):
        vals[name] = npcs.node(c)[4]

msg_x, msg_y = i16(vals["MapleTVmsgX"]), i16(vals["MapleTVmsgY"])
ad_x, ad_y = i16(vals["MapleTVadX"]), i16(vals["MapleTVadY"])

# 立绘以 origin 为锚点，WZ 偏移相对 NPC 锚点；换算到立绘左上角坐标。
msg_lt = (ox + msg_x, oy + msg_y)
ad_lt = (ox + ad_x, oy + ad_y)

stand_img = rgba(npcs, stand)
basic = rgba(ui, ui.find("MapleTV.img/TVbasic/0"))
media = rgba(ui, ui.find("MapleTV.img/TVmedia/0/0"))

stand_img.save(OUT / "tv_before_stand.png")

composite = stand_img.copy()
composite.alpha_composite(basic, msg_lt)
composite.alpha_composite(media, ad_lt)
composite.save(OUT / "tv_after_expected.png")

print("stand origin =", (ox, oy))
print("WZ offsets   = msg", (msg_x, msg_y), "ad", (ad_x, ad_y))
print("screen LT    = msg", msg_lt, "ad", ad_lt)
print("dark boards  = top x89..328 y190..279 / bottom x84..323 y308..487 (PIL 实测)")
print("out          =", OUT)
