# -*- coding: utf-8 -*-
"""R15 pass 8: match generic chrome (scrollbars / close buttons / checkboxes) from
Basic.img + UIWindow.img/BtUIClose* against every screenshot, and re-run the window's
own subtree with a stricter opaque-pixel floor so tiny transparent art cannot produce
spurious hits."""
import io, os, sys, collections
import numpy as np
from PIL import Image

sys.path.insert(0, r"E:\project\MapleStory-Client\tools")
from nx_inspect import Nx

NXPATH = "E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx"
SRC = r"C:\Users\11036\Desktop\正确客户端截图"
OUT = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
K = 110
MIN_OPAQUE = 120

JOBS = [
    ("勋章框.png", "UIWindow.img/Title"),
    ("好友列表框.png", "UIWindow.img/UserList"),
    ("组员列表框.png", "UIWindow.img/UserList"),
    ("拒绝列表框.png", "UIWindow.img/UserList"),
    ("家族联盟.png", "UIWindow.img/UserList"),
    ("好友、组员、家族、家族联盟、拒绝列表.png", "UIWindow.img/UserList"),
    ("个人请报框.png", "UIWindow.img/UserInfo"),
    ("学院.png", "UIWindow.img/Family"),
]
GENERIC_ROOTS = ["Basic.img/VScr", "Basic.img/VScr2", "Basic.img/VScr3", "Basic.img/VScr4",
                 "Basic.img/VScr5", "Basic.img/VScr6", "Basic.img/VScr7", "Basic.img/VScr8",
                 "Basic.img/HScr", "Basic.img/HScr2", "Basic.img/HScr3", "Basic.img/HScr4",
                 "Basic.img/HScr5", "Basic.img/HScr6",
                 "Basic.img/BtClose", "Basic.img/BtClose2", "Basic.img/CheckBox",
                 "UIWindow.img/BtUIClose", "UIWindow.img/BtUIClose2"]

nx = Nx(NXPATH)


def rgba_of(path):
    i = nx.find(path)
    if i is None:
        return None
    w, h, raw = nx.bitmap_pixels(i)
    if raw is None:
        return None
    a = np.frombuffer(raw, dtype=np.uint8).reshape(h, w, 4)
    return a[:, :, [2, 1, 0, 3]].copy()


def walk_bitmaps(root):
    out = []
    i = nx.find(root)
    if i is None:
        return out

    def rec(node, prefix):
        if nx.node(node)[3] == 5:
            out.append(prefix)
        for c in nx.children(node):
            rec(c, prefix + "/" + nx.name(c))

    rec(i, root)
    return out


def sparse_best(img, tpl, mask):
    H, W = img.shape[:2]
    th, tw = tpl.shape[:2]
    if tw > W or th > H:
        return None
    Hy, Wx = H - th + 1, W - tw + 1
    ys, xs = np.nonzero(mask)
    if len(ys) == 0:
        return None
    step = max(1, len(ys) // K)
    idx = np.arange(0, len(ys), step)[:K]
    acc = np.zeros((Hy, Wx), dtype=np.float64)
    ii = img[:, :, :3].astype(np.int16)
    tt = tpl[:, :, :3].astype(np.int16)
    for k in idx:
        v, u = int(ys[k]), int(xs[k])
        acc += np.abs(ii[v:v + Hy, u:u + Wx] - tt[v, u]).sum(axis=2)
    j = int(np.argmin(acc))
    dy, dx = divmod(j, Wx)
    return dx, dy


def dense(img, tpl, mask, dx, dy):
    H, W = img.shape[:2]
    th, tw = tpl.shape[:2]
    if dx < 0 or dy < 0 or dx + tw > W or dy + th > H:
        return None
    si = img[dy:dy + th, dx:dx + tw, :3].astype(np.int16)
    d = np.abs(si - tpl[:, :, :3].astype(np.int16)).sum(axis=2)
    n = int(mask.sum())
    return (float(((d == 0) & mask).sum()) / n, float(d[mask].mean()),
            float(((d <= 4) & mask).sum()) / n, n)


def main():
    L = []
    ap = L.append
    generic = []
    for r in GENERIC_ROOTS:
        generic += walk_bitmaps(r)
    ap("generic chrome bitmaps collected: %d" % len(generic))
    ap("")
    for shot, root in JOBS:
        img = np.asarray(Image.open(os.path.join(SRC, shot)).convert("RGBA"), dtype=np.uint8)
        H, W = img.shape[:2]
        ap("=" * 100)
        ap("SCREENSHOT %s  %dx%d" % (shot, W, H))
        own = walk_bitmaps(root)
        for label, paths in (("window subtree (%s)" % root, own), ("generic chrome", generic)):
            hits, weak = [], []
            for p in paths:
                if p.split("/")[-1].startswith("backgrnd"):
                    continue
                a = rgba_of(p)
                if a is None:
                    continue
                m = a[..., 3] >= 250
                n = int(m.sum())
                if n < 20:
                    continue
                sb = sparse_best(img, a, m)
                if sb is None:
                    continue
                dx, dy = sb
                d = dense(img, a, m, dx, dy)
                if d is None:
                    continue
                exact, mae, w4, npx = d
                rec = (p, a.shape[1], a.shape[0], dx, dy, exact, mae, w4, npx)
                if exact >= 0.97 and mae <= 3.0:
                    (hits if npx >= MIN_OPAQUE else weak).append(rec)
            hits.sort(key=lambda t: (t[4], t[3]))
            weak.sort(key=lambda t: (t[4], t[3]))
            ap("  --- %s : %d accepted, %d below %d-opaque-px floor ---" % (label, len(hits), len(weak), MIN_OPAQUE))
            for p, w, h, dx, dy, exact, mae, w4, npx in hits:
                ap("    %-62s %dx%-4d x[%d..%d] y[%d..%d]  exact=%.4f mae=%.3f n=%d"
                   % (p, w, h, dx, dx + w - 1, dy, dy + h - 1, exact, mae, npx))
            if weak:
                ap("    (weak, tiny art - not trusted on its own)")
                for p, w, h, dx, dy, exact, mae, w4, npx in weak[:14]:
                    ap("    %-62s %dx%-4d x[%d..%d] y[%d..%d]  exact=%.4f mae=%.3f n=%d"
                       % (p, w, h, dx, dx + w - 1, dy, dy + h - 1, exact, mae, npx))
        ap("")
    with io.open(os.path.join(OUT, "_p8_chrome.txt"), "w", encoding="utf-8") as f:
        f.write("\n".join(L) + "\n")
    print("DONE")


if __name__ == "__main__":
    main()
