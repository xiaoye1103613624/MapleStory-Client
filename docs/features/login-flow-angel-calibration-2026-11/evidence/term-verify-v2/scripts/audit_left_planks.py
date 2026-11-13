# -*- coding: utf-8 -*-
"""逐页量化登录链路「左侧木牌导航栏」ROI 的对齐度，并核桌面目标帧与旧 targets 是否同源。
纯 numpy+PIL，零均值 NCC；不臆测像素内容，只给可复现数值。
"""
import os, json, hashlib
import numpy as np
from PIL import Image

TV = r"C:\Users\11036\AppData\Local\Temp\term_verify"
DESK = os.path.join(TV, "targets_desktop")
OLD = os.path.join(TV, "targets")
SHOTS = os.path.join(TV, "shots")
OUT = os.path.join(TV, "ui_left_planks")
os.makedirs(OUT, exist_ok=True)


def load(p):
    return np.asarray(Image.open(p).convert("RGB"), dtype=np.float64)


def ncc(a, b):
    """零均值 NCC，逐通道后取灰度+RGB 均值；尺寸不一致返回 None。"""
    if a.shape != b.shape:
        return None
    out = {}
    for key, aa, bb in (("gray", a.mean(2), b.mean(2)),
                        ("R", a[:, :, 0], b[:, :, 0]),
                        ("G", a[:, :, 1], b[:, :, 1]),
                        ("B", a[:, :, 2], b[:, :, 2])):
        x = aa - aa.mean()
        y = bb - bb.mean()
        d = np.sqrt((x * x).sum() * (y * y).sum())
        out[key] = float((x * y).sum() / d) if d > 0 else 0.0
    return out


def edge_ncc(a, b):
    """水平梯度结构相似度，剔除色差，只看线条/字/木牌边缘是否同位。"""
    ga = np.abs(np.diff(a.mean(2), axis=1))
    gb = np.abs(np.diff(b.mean(2), axis=1))
    x = ga - ga.mean()
    y = gb - gb.mean()
    d = np.sqrt((x * x).sum() * (y * y).sum())
    return float((x * y).sum() / d) if d > 0 else 0.0


# 页面配对：本端帧 / 目标帧（桌面新帧）
PAIRS = [
    ("login", "v2_login_typed.png", "desktop_login.png"),
    ("channel_collapsed", "v2_world_collapsed.png", "desktop_channel_collapsed.png"),
    ("channel_expanded", "v2_channel_grid.png", "desktop_channel_expanded.png"),
    ("char_select", "v2_racecreate_settled.png", "desktop_char_select.png"),
    ("racecreate", "racecreate_click_f06.png", "desktop_racecreate_race.png"),
]

# ROI：左侧木牌导航栏（书页左边框+挂载木牌），右上版本号，全图
ROIS = {
    "left_plank_strip": (0, 0, 160, 470),   # x0,y0,x1,y1 左侧整条木牌栏
    "left_plank_top": (0, 0, 160, 100),     # 顶部标题牌
    "left_plank_bottom": (0, 380, 160, 470),# 底部「查看所有角色/回到首页」
    "version_topright": (1050, 0, 1280, 55),
    "full": (0, 0, 1280, 720),
}

res = {"pairs": [], "rois": ROIS, "same_source_check": {}}

# 先核桌面 7 张 vs 旧 targets 7 张 sha256（判断是否同一口径）
name_map = {
    "desktop_login.png": "login.png",
    "desktop_channel_collapsed.png": "channel_select.png",
    "desktop_channel_expanded.png": "channel_selected.png",
    "desktop_char_select.png": "char_select.png",
    "desktop_ingame.png": "ingame.png",
    "desktop_keymap.png": "keymap.png",
    "desktop_seik.png": "seik.png",
}
for d, o in name_map.items():
    dp, op = os.path.join(DESK, d), os.path.join(OLD, o)
    if os.path.exists(dp) and os.path.exists(op):
        hd = hashlib.sha256(open(dp, "rb").read()).hexdigest()
        ho = hashlib.sha256(open(op, "rb").read()).hexdigest()
        res["same_source_check"][o] = {
            "desktop_sha256": hd, "old_target_sha256": ho, "identical": hd == ho,
            "desktop_bytes": os.path.getsize(dp), "old_bytes": os.path.getsize(op),
        }

for page, shot, tgt in PAIRS:
    sp, tp = os.path.join(SHOTS, shot), os.path.join(DESK, tgt)
    if not (os.path.exists(sp) and os.path.exists(tp)):
        res["pairs"].append({"page": page, "error": "missing",
                             "shot": sp, "target": tp})
        continue
    a, b = load(sp), load(tp)
    if a.shape != b.shape:
        res["pairs"].append({"page": page, "error": "size_mismatch",
                             "shot_shape": list(a.shape), "target_shape": list(b.shape)})
        continue
    entry = {"page": page, "shot": shot, "target": tgt, "shape": list(a.shape), "roi": {}}
    for rn, (x0, y0, x1, y1) in ROIS.items():
        ra, rb = a[y0:y1, x0:x1], b[y0:y1, x0:x1]
        e = {"ncc": ncc(ra, rb), "edge_ncc": edge_ncc(ra, rb),
             "shot_mean": float(ra.mean()), "target_mean": float(rb.mean()),
             "shot_std": float(ra.std()), "target_std": float(rb.std())}
        entry["roi"][rn] = e
    # 左侧栏木牌「存在性」判据：目标端该 ROI 有较高方差（木牌+字），若本端方差远低则疑似缺失
    lp = entry["roi"]["left_plank_strip"]
    entry["left_plank_verdict"] = {
        "target_std": lp["target_std"], "shot_std": lp["shot_std"],
        "std_ratio": float(lp["shot_std"] / lp["target_std"]) if lp["target_std"] > 0 else None,
        "gray_ncc": lp["ncc"]["gray"], "edge_ncc": lp["edge_ncc"],
        "likely_present_both": bool(lp["shot_std"] > 12 and lp["target_std"] > 12),
    }
    res["pairs"].append(entry)

with open(os.path.join(OUT, "left_plank_audit.json"), "w", encoding="utf-8") as f:
    json.dump(res, f, ensure_ascii=False, indent=2)

print("=== 同源核对(桌面7张 vs 旧targets7张) ===")
for k, v in res["same_source_check"].items():
    print(f"  {k}: identical={v['identical']} bytes {v['desktop_bytes']} vs {v['old_bytes']}")
print("\n=== 逐页左侧木牌栏 ROI (0,0,160,470) ===")
for e in res["pairs"]:
    if "error" in e:
        print(f"  {e['page']}: ERROR {e['error']}")
        continue
    lp = e["left_plank_verdict"]
    print(f"  {e['page']:20s} gray={lp['gray_ncc']:+.4f} edge={lp['edge_ncc']:+.4f} "
          f"std shot/tgt={lp['shot_std']:.1f}/{lp['target_std']:.1f} "
          f"ratio={lp['std_ratio']:.2f} both_present={lp['likely_present_both']}")
print("\n=== 各 ROI gray NCC 全表 ===")
for e in res["pairs"]:
    if "error" in e:
        continue
    print(f"  {e['page']}:")
    for rn, v in e["roi"].items():
        print(f"    {rn:20s} gray={v['ncc']['gray']:+.4f} edge={v['edge_ncc']:+.4f}")
print("\nOUT:", os.path.join(OUT, "left_plank_audit.json"))
