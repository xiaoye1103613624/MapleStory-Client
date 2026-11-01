# -*- coding: utf-8 -*-
"""R15 pass 7: for each screenshot, locate the window frame, then match EVERY bitmap
that lives under that window's UI.nx node against the screenshot.

Method
------
frame   : ring-band template match of the window `backgrnd` (see _p5) -> (fdx,fdy)
control : sparse (K px) sum-of-abs match of the control bitmap over every integer
          offset where it fits inside the image; the best offset is then re-scored
          DENSELY over all fully-opaque template pixels.
          ACCEPT when exact-equal fraction >= 0.97 and dense MAE <= 3.0.
All numbers are measured, none assumed.
"""
import io, os, sys, collections
import numpy as np
from PIL import Image

sys.path.insert(0, r"E:\project\MapleStory-Client\tools")
from nx_inspect import Nx

NXPATH = "E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx"
SRC = r"C:\Users\11036\Desktop\正确客户端截图"
OUT = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"

JOBS = [
    ("勋章框.png", "UIWindow.img/Title", "UIWindow.img/Title/backgrnd"),
    ("好友列表框.png", "UIWindow.img/UserList", "UIWindow.img/UserList/backgrnd3"),
    ("组员列表框.png", "UIWindow.img/UserList", "UIWindow.img/UserList/backgrnd"),
    ("拒绝列表框.png", "UIWindow.img/UserList", "UIWindow.img/UserList/backgrnd"),
    ("家族联盟.png", "UIWindow.img/UserList", "UIWindow.img/UserList/backgrnd2"),
    ("好友、组员、家族、家族联盟、拒绝列表.png", "UIWindow.img/UserList", "UIWindow.img/UserList/backgrnd2"),
    ("个人请报框.png", "UIWindow.img/UserInfo", "UIWindow.img/UserInfo/backgrnd"),
    ("学院.png", "UIWindow.img/Family", "UIWindow.img/Family/backgrnd"),
]
K_SAMPLES = 110
ACC_EXACT = 0.97
ACC_MAE = 3.0

nx = Nx(NXPATH)


def bgra_to_rgba(w, h, raw):
    a = np.frombuffer(raw, dtype=np.uint8).reshape(h, w, 4)  # stored BGRA
    return a[:, :, [2, 1, 0, 3]].copy()


def load_node(path):
    i = nx.find(path)
    if i is None:
        return None, None
    w, h, raw = nx.bitmap_pixels(i)
    if raw is None:
        return None, None
    return bgra_to_rgba(w, h, raw), nx.value(i)


def walk(root):
    """yield (path, is_bitmap, size_str, origin_str)"""
    out = []
    i = nx.find(root)
    if i is None:
        return out

    def rec(node, prefix):
        _, _, _, ntype, _, w, h = nx.node(node)
        origin = ""
        for c in nx.children(node):
            if nx.node(c)[3] == 4:
                origin = nx.value(c)
        out.append((prefix, ntype, "%dx%d" % (w, h) if ntype == 5 else "", origin))
        for c in nx.children(node):
            rec(c, prefix + "/" + nx.name(c))

    rec(i, root)
    return out


def dense(img, tpl, mask, dx, dy):
    H, W = img.shape[:2]
    th, tw = tpl.shape[:2]
    if dx < 0 or dy < 0 or dx + tw > W or dy + th > H:
        return None
    si = img[dy:dy + th, dx:dx + tw, :3].astype(np.int16)
    st = tpl[:, :, :3].astype(np.int16)
    d = np.abs(si - st).sum(axis=2)
    n = int(mask.sum())
    exact = float(((d == 0) & mask).sum()) / n
    mae = float(d[mask].mean())
    within4 = float(((d <= 4) & mask).sum()) / n
    return exact, mae, within4, n


def sparse_best(img, tpl, mask):
    H, W = img.shape[:2]
    th, tw = tpl.shape[:2]
    if tw > W or th > H:
        return None
    Hy, Wx = H - th + 1, W - tw + 1
    ys, xs = np.nonzero(mask)
    n = len(ys)
    if n == 0:
        return None
    step = max(1, n // K_SAMPLES)
    idx = np.arange(0, n, step)[:K_SAMPLES]
    acc = np.zeros((Hy, Wx), dtype=np.float64)
    ii = img[:, :, :3].astype(np.int16)
    tt = tpl[:, :, :3].astype(np.int16)
    for k in idx:
        v, u = int(ys[k]), int(xs[k])
        acc += np.abs(ii[v:v + Hy, u:u + Wx] - tt[v, u]).sum(axis=2)
    j = int(np.argmin(acc))
    dy, dx = divmod(j, Wx)
    return int(dx), int(dy), float(acc[dy, dx] / (len(idx) * 3))


def main():
    L = []
    ap = L.append
    for shot, root, frame in JOBS:
        img = np.asarray(Image.open(os.path.join(SRC, shot)).convert("RGBA"), dtype=np.uint8)
        H, W = img.shape[:2]
        fa, _ = load_node(frame)
        fmask = fa[..., 3] >= 250
        fb = dense_best = None
        # frame alignment via ring band
        ys, xs = np.nonzero(fmask)
        obb = (int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max()))
        x0, y0, x1, y1 = obb
        ring = np.zeros((H, W), dtype=bool)
        rm = np.zeros(fa.shape[:2], dtype=bool)
        rm[y0:y0 + 4, x0:x1 + 1] = True
        rm[y1 - 3:y1 + 1, x0:x1 + 1] = True
        rm[y0:y1 + 1, x0:x0 + 4] = True
        rm[y0:y1 + 1, x1 - 3:x1 + 1] = True
        rm &= fmask
        best = None
        for dy in range(-14, 15):
            for dx in range(-14, 15):
                th, tw = fa.shape[:2]
                ax0, ax1 = max(0, dx), min(W, dx + tw)
                ay0, ay1 = max(0, dy), min(H, dy + th)
                if ax1 <= ax0 or ay1 <= ay0:
                    continue
                si = img[ay0:ay1, ax0:ax1, :3].astype(np.int16)
                st = fa[ay0 - dy:ay1 - dy, ax0 - dx:ax1 - dx, :3].astype(np.int16)
                sm = rm[ay0 - dy:ay1 - dy, ax0 - dx:ax1 - dx]
                if sm.sum() < 200:
                    continue
                m = float(np.abs(si - st).sum(axis=2)[sm].mean())
                if best is None or m < best[0]:
                    best = (m, dx, dy, int(sm.sum()))
        ap("=" * 100)
        ap("SCREENSHOT %s  %dx%d   frame node = %s (%dx%d)" % (shot, W, H, frame, fa.shape[1], fa.shape[0]))
        if best is None:
            ap("  FRAME NOT LOCATED")
            continue
        fm, fdx, fdy, fn = best
        ap("  frame alignment: dx=%+d dy=%+d  ring MAE=%.3f over n=%d ring px" % (fdx, fdy, fm, fn))
        ap("  window outer rect (bitmap extent) x[%d..%d] y[%d..%d]" % (fdx, fdx + fa.shape[1] - 1, fdy, fdy + fa.shape[0] - 1))
        ap("  window OPAQUE rect              x[%d..%d] y[%d..%d]  = %dx%d"
           % (fdx + obb[0], fdx + obb[2], fdy + obb[1], fdy + obb[3],
              obb[2] - obb[0] + 1, obb[3] - obb[1] + 1))
        ap("")
        ap("  --- control matches under %s ---" % root)
        nodes = walk(root)
        hits = []
        for path, ntype, sz, origin in nodes:
            if ntype != 5:
                continue
            base = path.split("/")[-1]
            if base.startswith("backgrnd"):
                continue
            a, _ = load_node(path)
            if a is None:
                continue
            m = a[..., 3] >= 250
            if m.sum() < 40:
                continue
            sb = sparse_best(img, a, m)
            if sb is None:
                continue
            dx, dy, smae = sb
            d = dense(img, a, m, dx, dy)
            if d is None:
                continue
            exact, mae, w4, npx = d
            if exact >= ACC_EXACT and mae <= ACC_MAE:
                hits.append((path, a.shape[1], a.shape[0], dx, dy, exact, mae, w4, npx, origin))
        hits.sort(key=lambda t: (t[4], t[3]))
        ap("  %-58s %-9s %-14s %-7s %-14s %s" % ("node", "size", "bbox x0,y0..x1,y1", "exact", "MAE", "within4"))
        for path, w, h, dx, dy, exact, mae, w4, npx, origin in hits:
            ap("  %-58s %-9s %-16s exact=%.4f mae=%.3f within4=%.4f n=%d origin=%s  bbox x[%d..%d] y[%d..%d]"
               % (path, "%dx%d" % (w, h), "%d,%d..%d,%d" % (dx, dy, dx + w - 1, dy + h - 1),
                  exact, mae, w4, npx, origin, dx, dx + w - 1, dy, dy + h - 1))
        if not hits:
            ap("  (none accepted)")
        ap("")
    with io.open(os.path.join(OUT, "_p7_buttons.txt"), "w", encoding="utf-8") as f:
        f.write("\n".join(L) + "\n")
    print("DONE")


if __name__ == "__main__":
    main()
