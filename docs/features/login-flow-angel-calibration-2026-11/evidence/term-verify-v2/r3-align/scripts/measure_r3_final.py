#!/usr/bin/env python3
"""R3 决定性测量：正确帧 + WZ 真值模板 + 文本墨行投影 + 3x 目视裁图。

帧口径：
  char  : target desktop_char_select.png        / ours ui_final_charselect.png
  chan  : target desktop_channel_expanded.png  / ours channel_grid_f05.png（常态，mean152）
  coll  : target desktop_channel_collapsed.png / ours world_f03.png（常态，mean144）
公告只出现在展开页(chan)，故 noticeTitle/noticeNw 走 chan。
"""
import base64
import io
import json
import os

import numpy as np
from PIL import Image, ImageDraw

TV = r"C:\Users\11036\AppData\Local\Temp\term_verify"
OUT = os.path.join(TV, "ui_r3")
SPILL = (r"C:\Users\11036\AppData\Local\Temp\dsh-spill-xgIqGu"
         r"\session-79253882d1e2"
         r"\c666db3f18b1-mcp__orange_wz__get_node_detail.txt")

FRAMES = {
    "char": ("targets_desktop/desktop_char_select.png", "shots/ui_final_charselect.png"),
    "chan": ("targets_desktop/desktop_channel_expanded.png", "shots/channel_grid_f05.png"),
    "coll": ("targets_desktop/desktop_channel_collapsed.png", "shots/world_f03.png"),
}

# key -> (nodePath, page, roi)
TPLS = {
    "charInfo":    ("CharSelect/charInfo",                      "char", (330, 150, 640, 380)),
    "scroll":      ("CharSelect/scroll/2/3",                    "char", (350, 120, 620, 400)),
    "nameTagL":    ("CharSelect/nameTag/1/0",                   "char", (380, 395, 620, 470)),
    "noticeTitle": ("WorldNotice/BalloonForLoginProcess/Title", "chan", (180, 250, 560, 380)),
    "noticeNw":    ("WorldNotice/BalloonForLoginProcess/nw",    "chan", (200, 250, 560, 400)),
}


def decode():
    doc = json.loads(open(SPILL, encoding="utf-8", errors="replace").read())
    by = {}
    for r in doc["results"]:
        d = r.get("detail", {})
        np_ = d.get("node", {}).get("nodePath") or r.get("nodePath")
        v = d.get("value", {})
        if np_ and v.get("png"):
            by[np_] = (v["png"], v.get("width"), v.get("height"))
    out = {}
    for key, (path, page, roi) in TPLS.items():
        b64, w, h = by[path]
        im = Image.open(io.BytesIO(base64.b64decode(b64))).convert("RGBA")
        p = os.path.join(OUT, "tpl2_%s.png" % key)
        im.save(p)
        out[key] = {"file": p, "size": list(im.size), "wz": [w, h],
                    "page": page, "roi": list(roi)}
    return out


def ncc(t, tm, a, roi):
    x0, y0, x1, y1 = roi
    h, w = tm.shape
    m = tm > 128
    tv = t[m].astype(np.float64); tv -= tv.mean()
    tn = np.sqrt((tv * tv).sum())
    best = (-2.0, None, None)
    for y in range(y0, y1 - h + 1):
        for x in range(x0, x1 - w + 1):
            pv = a[y:y + h, x:x + w][m].astype(np.float64); pv -= pv.mean()
            pn = np.sqrt((pv * pv).sum())
            if pn < 1e-6 or tn < 1e-6:
                continue
            c = float((tv * pv).sum() / (tn * pn))
            if c > best[0]:
                best = (c, x, y)
    return {"ncc": round(best[0], 4), "bbox": None if best[1] is None
            else [best[1], best[2], best[1] + w, best[2] + h]}


def ink_rows(a, x0, x1, y0, y1):
    """暗中性墨行：返回连续 run 段（暗像素数 8..120 的行）。"""
    reg = a[y0:y1, x0:x1]
    lum = reg.mean(2)
    neutral = (reg.max(2) - reg.min(2)) < 70
    dark = (lum < 105) & neutral
    cnt = dark.sum(1)
    rows = [y0 + i for i, v in enumerate(cnt) if 8 <= v <= 120]
    runs = []
    if rows:
        s = pr = rows[0]
        for y in rows[1:]:
            if y == pr + 1:
                pr = y
            else:
                runs.append([s, pr]); s = pr = y
        runs.append([s, pr])
    return runs


def zoom_crop(tp, op, box, name, scale=3):
    T = Image.open(tp).convert("RGB").crop(box)
    O = Image.open(op).convert("RGB").crop(box)
    w, h = T.size
    canvas = Image.new("RGB", (w * scale, h * scale * 2 + 28), (20, 20, 20))
    canvas.paste(T.resize((w * scale, h * scale), Image.NEAREST), (0, 0))
    canvas.paste(O.resize((w * scale, h * scale), Image.NEAREST), (0, h * scale + 28))
    d = ImageDraw.Draw(canvas)
    d.text((4, 2), "TARGET", fill=(255, 90, 90))
    d.text((4, h * scale + 30), "OURS", fill=(90, 255, 90))
    p = os.path.join(OUT, name)
    canvas.save(p)
    return p


def main():
    tpls = decode()
    arrs = {}
    for page, (tp, op) in FRAMES.items():
        arrs[page] = {
            "T": np.asarray(Image.open(os.path.join(TV, tp)).convert("RGB"), np.float32),
            "O": np.asarray(Image.open(os.path.join(TV, op)).convert("RGB"), np.float32),
            "tp": os.path.join(TV, tp), "op": os.path.join(TV, op),
        }
    res = {"templates": {k: {"size": v["size"], "wz": v["wz"]} for k, v in tpls.items()},
           "frames": {k: {"target": os.path.basename(v["tp"]),
                          "ours": os.path.basename(v["op"])} for k, v in arrs.items()},
           "locate": {}, "ink_rows": {}, "crops": {}}

    for key, info in tpls.items():
        arr = np.asarray(Image.open(info["file"]).convert("RGBA"))
        tm, t = arr[..., 3], arr[..., :3].astype(np.float32)
        A = arrs[info["page"]]
        rt = ncc(t, tm, A["T"], tuple(info["roi"]))
        ro = ncc(t, tm, A["O"], tuple(info["roi"]))
        shift = None if not (rt["bbox"] and ro["bbox"]) \
            else [ro["bbox"][i] - rt["bbox"][i] for i in range(4)]
        res["locate"][key] = {"target": rt, "ours": ro, "shift": shift}

    # 属性板墨行（x 390..575，y 190..345）—— 直接量属性值行
    A = arrs["char"]
    res["ink_rows"]["char_detail"] = {
        "target": ink_rows(A["T"], 390, 575, 190, 345),
        "ours": ink_rows(A["O"], 390, 575, 190, 345),
    }

    res["crops"]["char_detail"] = zoom_crop(
        A["tp"], A["op"], (350, 140, 620, 370), "zoom_char_detail.png")
    res["crops"]["name_tag"] = zoom_crop(
        A["tp"], A["op"], (400, 405, 620, 465), "zoom_name_tag.png", 4)
    res["crops"]["notice"] = zoom_crop(
        arrs["chan"]["tp"], arrs["chan"]["op"], (250, 270, 500, 380),
        "zoom_notice.png", 3)

    jp = os.path.join(OUT, "measure_r3_final.json")
    json.dump(res, open(jp, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
    print(json.dumps(res, ensure_ascii=False, indent=2))
    print("JSON", jp)


if __name__ == "__main__":
    main()
