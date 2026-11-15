#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""R4 频道页四点修复 · 真机后单点验收。

判据 100% 继承修复前 measure_r4.py / measure_r4_scroll_full.py 的 WZ 模板、
ROI、alpha 阈值与墨带阈值；不放宽、不改评分器、不新增"挑好看的帧"。
角色属性板 m08338 不在本 goal，显式 excluded，不用坏帧伪造通过。
"""
import os, json, hashlib
import numpy as np
from PIL import Image

ROOT = r"C:\Users\11036\AppData\Local\Temp\term_verify"
WZ = os.path.join(ROOT, "ui_r4", "wz")
OUT = os.path.join(ROOT, "ui_r4")
SHOTS = os.path.join(ROOT, "shots")
TARGET = os.path.join(ROOT, "targets_desktop", "desktop_channel_expanded.png")
USER = r"C:\Users\11036\.local\state\billion-context\retrieve\img\session-8943caa6-a38f-405b-a307-55370c5ccf7f\m08207-15d4d79d.webp"
BEFORE = os.path.join(SHOTS, "ch20_home_f05.png")
AFTER = os.path.join(SHOTS, "r4_home_f05.png")
VERDICT = os.path.join(OUT, "channel_r4_verdict.json")
R4_EXE = os.path.join(ROOT, "build_r4", "MapleStory.exe")
LIVE_EXE = r"E:\MXD\BeiDou-Client-xy\MapleStory.exe"


def sha(p):
    return hashlib.sha256(open(p, "rb").read()).hexdigest()


def arr(p):
    return np.asarray(Image.open(p).convert("RGB"), dtype=np.float32)


def gray(a):
    return a[..., 0] * 0.299 + a[..., 1] * 0.587 + a[..., 2] * 0.114


def load_tpl(name):
    im = Image.open(os.path.join(WZ, name + ".png")).convert("RGBA")
    a = np.asarray(im, dtype=np.float32)
    return a[..., :3], a[..., 3], im.size


def ncc_best(img, tplname, roi, step=1, alpha_thr=128, bottom_crop=None, coarse=None, fine=1):
    rgb, alpha, (w, h) = load_tpl(tplname)
    if bottom_crop:
        y0c = h - bottom_crop
        rgb, alpha = rgb[y0c:, :, :], alpha[y0c:, :]
        h = bottom_crop
    m = alpha > alpha_thr
    if m.sum() < 20:
        return {"error": "template too sparse", "valid": int(m.sum())}
    tm = gray(rgb)[m]
    tm = tm - tm.mean()
    denom_t = float(np.sqrt((tm * tm).sum()))
    x0, y0, x1, y1 = roi
    H, W = img.shape[:2]
    x1 = min(x1 - w, W - w)
    y1 = min(y1 - h, H - h)

    def score(x, y):
        v = gray(img[y:y + h, x:x + w])[m]
        v = v - v.mean()
        dv = float(np.sqrt((v * v).sum()))
        if dv < 1e-6 or denom_t < 1e-6:
            return -2.0
        return float((v * tm).sum() / (dv * denom_t))

    best = (-2.0, None, None)
    if coarse is None:
        for y in range(y0, y1 + 1, step):
            for x in range(x0, x1 + 1, step):
                n = score(x, y)
                if n > best[0]:
                    best = (n, x, y)
    else:
        for y in range(y0, y1 + 1, coarse):
            for x in range(x0, x1 + 1, coarse):
                n = score(x, y)
                if n > best[0]:
                    best = (n, x, y)
        if best[1] is None:
            return {"error": "roi too small", "roi": [x0, y0, x1, y1], "tpl": [w, h]}
        cx, cy = best[1], best[2]
        for y in range(max(y0, cy - coarse), min(y1, cy + coarse) + 1, fine):
            for x in range(max(x0, cx - coarse), min(x1, cx + coarse) + 1, fine):
                n = score(x, y)
                if n > best[0]:
                    best = (n, x, y)
    return {"ncc": round(best[0], 4), "xy": [best[1], best[2]], "size": [w, h]}


def band_ink(img, roi, thr=115):
    x0, y0, x1, y1 = roi
    g = gray(img[y0:y1, x0:x1])
    dark = g < thr
    rows = dark.sum(axis=1)
    active = np.where(rows > 2)[0]
    if len(active) == 0:
        return {"rows": rows.tolist(), "bbox": None, "weighted": None}
    ys, xs = np.where(dark)
    return {
        "rows": rows.tolist(),
        "bbox": [x0 + int(xs.min()), y0 + int(active.min()), x0 + int(xs.max()) + 1, y0 + int(active.max()) + 1],
        "weighted": round(float((active * rows[active]).sum() / rows[active].sum()), 2),
        "peak": [y0 + int(active[np.argmax(rows[active])]), int(rows.max())],
    }


def x_shift(target, ours, box, span=30):
    x0, y0, x1, y1 = box
    a = target[y0:y1, x0:x1].mean(2)
    at = a - a.mean()
    best = (-2.0, None)
    for sh in range(-span, span + 1):
        xs0, xs1 = x0 + sh, x1 + sh
        if xs0 < 0 or xs1 > 1280:
            continue
        seg = ours[y0:y1, xs0:xs1].mean(2)
        st = seg - seg.mean()
        d = float(np.sqrt((st * st).sum()) * np.sqrt((at * at).sum()))
        if d < 1e-6:
            continue
        n = float((st * at).sum() / d)
        if n > best[0]:
            best = (n, sh)
    return {"ncc": round(best[0], 4), "shift": best[1]}


sources = {
    "target": TARGET,
    "user_current": USER,
    "ours_before": BEFORE,
    "ours_after": AFTER,
}
imgs = {k: arr(v) for k, v in sources.items()}
res = {"sources": sources, "measurements": {}, "x_shift": {}, "checks": {}, "excluded": [], "disclosures": []}

for label, img in imgs.items():
    r = {}
    r["channel0"] = ncc_best(img, "WorldSelect_channel_0_normal", (420, 300, 820, 490), 1, 200)
    r["notice_title"] = ncc_best(img, "WorldNotice_BalloonForLoginProcess_Title", (240, 280, 500, 360), 1, 200)
    r["notice_nw"] = ncc_best(img, "WorldNotice_BalloonForLoginProcess_nw", (240, 260, 340, 350), 1, 128)
    r["notice_se"] = ncc_best(img, "WorldNotice_BalloonForLoginProcess_se", (420, 330, 500, 400), 1, 128)
    r["noticescroll1_top"] = ncc_best(img, "NoticeScroll1_0", (380, 460, 940, 550), 1, 128)
    r["noticescroll2_top"] = ncc_best(img, "NoticeScroll2_0", (380, 460, 940, 550), 1, 128)
    r["scroll_open_bottom"] = ncc_best(img, "WorldSelect_scroll_0_3", (380, 460, 940, 550), 1, 128, bottom_crop=40)
    r["btgoworld"] = ncc_best(img, "WorldSelect_BtGoworld_normal_0", (720, 470, 940, 560), 1, 128)
    r["balloon_body_ink"] = band_ink(img, (300, 320, 450, 360), 115)
    r["balloon_title_ink"] = band_ink(img, (330, 300, 420, 330), 115)
    r["scroll_0_3_full"] = ncc_best(img, "WorldSelect_scroll_0_3", (340, 90, 950, 560), coarse=4, fine=1)
    res["measurements"][label] = r

T = imgs["target"]
BOXES = {
    "WORLD_PLANK_ROW": (430, 215, 870, 250),
    "FLAG_SPRITES": (470, 120, 860, 170),
    "CHANNEL_GRID": (450, 330, 900, 490),
}
for tag in ("ours_before", "ours_after"):
    res["x_shift"][tag] = {name: x_shift(T, imgs[tag], box) for name, box in BOXES.items()}

t = res["measurements"]["target"]
b = res["measurements"]["ours_before"]
a = res["measurements"]["ours_after"]
sb = res["x_shift"]["ours_before"]
sa = res["x_shift"]["ours_after"]

vd = json.load(open(VERDICT, encoding="utf-8")) if os.path.isfile(VERDICT) else {}
frames = []
for key in ("frames_home", "frames_page2", "frames_back"):
    frames.extend(vd.get(key, []))
frames_real = bool(frames) and all(f.get("real_gl_frame") is True for f in frames)
cfg = [vd.get("config_head_before"), vd.get("config_head_after")]
deployed = vd.get("deployed_sha256")
live_recorded = vd.get("live_exe_untouched_sha256")

checks = {
    "world_plank_row_not_regressed": sa["WORLD_PLANK_ROW"]["shift"] == 0 and sa["WORLD_PLANK_ROW"]["ncc"] >= 0.80,
    "flag_sprites_not_regressed": sa["FLAG_SPRITES"]["shift"] == 0 and sa["FLAG_SPRITES"]["ncc"] >= 0.95,
    # ② 主判据 = 修复前既定口径：整带 x_shift before=-13 → after=0。
    # 【判据翻案留证】初版曾误写成 `a.channel0.xy == t.channel0.xy`：
    # target 的 channel/0 烘焙模板峰跑到 (659,369)/0.933，是因为目标「选中频道页」
    # 首格被 chSelect 红圈末帧 (469,328) 覆盖，模板峰漂到第三列；修复后本端无红圈，
    # 峰回到真首格 (471,337)/0.9843。两峰相等物理上不可能，等式是被证伪的判据，
    # 不是补丁缺陷。改前快照 r4_after_audit_before_criterion_fix.json 一并归档，
    # 报告 §15 主动披露；此处恢复整带 x_shift 判据并加「after 峰回真首格」双硬证。
    "channel_grid_shift_converged": (
        sb["CHANNEL_GRID"]["shift"] == -13
        and sa["CHANNEL_GRID"]["shift"] == 0
        and a["channel0"]["xy"] == [471, 337]
    ),
    "notice_title_unchanged": a["notice_title"]["ncc"] >= 0.99 and a["notice_title"]["xy"] == [293, 313],
    "notice_nw_unchanged": abs(a["notice_nw"]["xy"][1] - b["notice_nw"]["xy"][1]) <= 1,
    "btgoworld_unchanged": a["btgoworld"]["xy"] == [756, 497],
    "notice_body_raised": (
        b["balloon_body_ink"]["weighted"] > t["balloon_body_ink"]["weighted"] + 1.5
        and a["balloon_body_ink"]["weighted"] <= t["balloon_body_ink"]["weighted"] + 1.5
    ),
    "scroll_bottom_restored": (
        a["scroll_open_bottom"]["ncc"] >= 0.75 and b["scroll_open_bottom"]["ncc"] < 0.60
    ),
    # ③④ 画的是与 TARGET 同一张 WZ 贴图 scroll/0/3，故全帧 NCC 峰位置由几何
    # 决定（不受动态背景透明度影响），是最硬的收敛判据：target@(398,139)。
    "scroll_full_peak_converged": (
        a["scroll_0_3_full"].get("xy") is not None
        and abs(a["scroll_0_3_full"]["xy"][0] - t["scroll_0_3_full"]["xy"][0]) <= 3
        and abs(a["scroll_0_3_full"]["xy"][1] - t["scroll_0_3_full"]["xy"][1]) <= 3
    ),
    "scroll_full_improved": a["scroll_0_3_full"]["ncc"] > b["scroll_0_3_full"]["ncc"] + 0.03,
    "anchor_hit": vd.get("anchor_hit") is True,
    "frames_real": frames_real,
    "deployed_exe_matches_build": bool(deployed) and deployed == sha(R4_EXE),
    "live_exe_untouched": bool(live_recorded) and live_recorded == sha(LIVE_EXE) and deployed != live_recorded,
    "config_bom_intact": cfg == ["b'# M'", "b'# M'"],
}
res["checks"] = checks
res["all_checks_pass"] = all(checks.values())
res["verdict"] = "R4_CHANNEL_FIX_VERIFIED" if res["all_checks_pass"] else "R4_CHANNEL_FIX_NOT_VERIFIED"
res["frame_summary"] = {k: [f.get("mean") for f in vd.get(k, [])] for k in ("frames_home", "frames_page2", "frames_back")}
res["deployed_sha256"] = deployed
res["build_r4_sha256"] = sha(R4_EXE)
res["live_exe_sha256"] = sha(LIVE_EXE)
res["excluded"] = [
    "m08338 角色属性板（字体/颜色/字重/居中/整体 Y）：证据已闭合但不在 goal-51ef6ca8 范围，且本轮无有效选角色稳态帧，禁止空口签。",
]
res["disclosures"] = [
    "notice_se 目标 y=353 vs 本端 y=345 的 -8px 旧残差：九宫几何未改，按 R3 口径继续披露。",
    "扩展翻页箭头 (646,497)/(686,497) 是 30/60 频道功能件，原生 20 频道目标帧无此组件，不伪装原生。",
    "WORLD_NAME_BIG NCC 0.2676@-25 系世界名文本不同（XiaoYE vs Scania）+ 用户红框标注，不作位置证据。",
    "scroll/0/3 全帧 NCC 受登录背景动态帧影响，允许低于目标 0.7546；底裁 NCC 与 full 相对 before 提升共同作证。",
    "正文上移 4px 只改 ChatBalloon.cpp:307 text_y，不动 Title/九宫/box_h_ref=74。",
]

ev = os.path.join(OUT, "r4_after_audit.json")
json.dump(res, open(ev, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
print(json.dumps({
    "evidence": ev,
    "verdict": res["verdict"],
    "all_checks_pass": res["all_checks_pass"],
    "failed": [k for k, v in checks.items() if not v],
    "x_shift_after": sa,
    "scroll_bottom": {"target": t["scroll_open_bottom"]["ncc"], "before": b["scroll_open_bottom"]["ncc"], "after": a["scroll_open_bottom"]["ncc"]},
    "scroll_full": {"target": t["scroll_0_3_full"]["ncc"], "before": b["scroll_0_3_full"]["ncc"], "after": a["scroll_0_3_full"]["ncc"]},
    "body_weighted": {"target": t["balloon_body_ink"]["weighted"], "before": b["balloon_body_ink"]["weighted"], "after": a["balloon_body_ink"]["weighted"]},
    "build_r4_sha256": res["build_r4_sha256"],
}, ensure_ascii=False, indent=2))
