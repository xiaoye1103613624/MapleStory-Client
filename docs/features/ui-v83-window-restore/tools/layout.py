"""R27 版式门禁：原生帧 ↔ 客户帧「逐格比对」。

为什么要它：以往验收只查「元素在位」，于是一整轮 11 组偏位缺陷全部漏网。
判据换成可复算的一条：**同一个格子在原生帧上量到的值 = oracle，在客户帧上量到的
值 = 实测，Δ 超过 tol 即判未修复**（check 非 0 退出，可挂进构建脚本）。

坐标一律「窗口相对」：先用窗口自己的 backgrnd 位图在整帧里做模板匹配定出左上角，
再按相对坐标下探针 —— 窗口摆在屏幕哪儿都不影响对账，原生帧与客户帧分辨率不同也不影响。

用法
  python layout.py geom   <file.nx> <root/path> [more...]        # 资产侧几何：origin / 尺寸（格子的真源）
  python layout.py anchor <frame.png> <backgrnd.png> [--png OUT] # 只定锚点（调试用）
  python layout.py bias   <spec.json> [frame]                    # 审锚点：允许模板出界重算 ±2 meanSAD 网格
  python layout.py mark   <spec.json> <frame.png> [--anchor X Y] # 在原生帧量探针 -> 写回 oracle
  python layout.py check  <spec.json> <frame.png> [--png OUT] [--mode M]  # 实测 vs oracle（非 0 = 有格不合格）
  python layout.py probe  <frame.png> x0 y0 x1 y1 --color r,g,b  # 裸量一块区域，用来新写探针

**建 spec 后先跑 bias**：oracle 全是相对锚点的坐标，锚点偏 1px 就整张齐偏 1px。
自动锚点要求模板整张落在帧内，遇到紧裁的原生帧（帧高 == 图高）时 dy 被尺寸约束强制成 0，
这时用 bias 量出的那一格 `mark --anchor X Y` 校正；check/selftest 对 oracle 帧会直接采用
记录锚点（别的帧照旧自动锚定，真机帧的窗口位置本来就该量出来）。

spec = ../evidence/r27-oracle/<window>.json；字段说明见 README.md。
只在某形态下存在的格子（如 Stat 详情板）给探针写 "active_in": ["detail"]，
check 用 --mode 选形态，不适用的探针报 SKIP，不计入不合格。
"""
import argparse
import json
import os
import re
import sys

import numpy as np
from PIL import Image, ImageDraw

# tools/ -> ui-v83-window-restore/ -> features/ -> docs/ -> 仓库根
REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "../../../.."))


# ----------------------------------------------------------------- 图像与匹配

def load_rgb(path):
    return np.asarray(Image.open(resolve(path)).convert("RGB"), dtype=np.float64)


def resolve(path):
    """相对路径先按当前工作目录解，解不到再按仓库根解（spec 里两种写法都允许）。"""
    if os.path.isabs(path) or os.path.exists(path):
        return path
    return os.path.join(REPO, path)


def color_mask(img, color, tol):
    """每通道绝对差 <= tol 的像素掩码。img: HxWx3。"""
    d = np.abs(img - np.asarray(color, dtype=np.float64))
    return d.max(axis=2) <= tol


def find_box(img, color, tol, region=None, min_pix=3, row_min=1, col_min=1):
    """region 内匹配色的 bbox -> (x0,y0,x1,y1,n)；不足 min_pix 个返回 None。

    row_min / col_min 是「该行/列至少要有几个匹配像素才算内容」。不加这两道闸，
    一个杂散暗点就能把 bbox 从文字块撑到整块面板（实测 value_text_bbox 一度量成
    198x227 = 整个搜索区），等于没量到东西。
    """
    x0, y0, x1, y1 = region if region else (0, 0, img.shape[1], img.shape[0])
    sub = img[y0:y1, x0:x1]
    m = color_mask(sub, color, tol)
    n = int(m.sum())
    if n < min_pix:
        return None
    rows = np.nonzero(m.sum(axis=1) >= row_min)[0]
    if not len(rows):
        return None
    cols = np.nonzero(m[rows[0]:rows[-1] + 1].sum(axis=0) >= col_min)[0]
    if not len(cols):
        return None
    return (x0 + int(cols.min()), y0 + int(rows.min()),
            x0 + int(cols.max()) + 1, y0 + int(rows.max()) + 1, n)


def band_rows(img, color, tol, region, min_run):
    """region 内「水平连续同色段 >= min_run」的行号列表（量条带/格线，比目测可靠）。"""
    x0, y0, x1, y1 = region
    out = []
    for y in range(y0, y1):
        row = color_mask(img[y:y + 1, x0:x1], color, tol)[0]
        if row.sum() < min_run:
            continue
        best = cur = 0
        for v in row:
            cur = cur + 1 if v else 0
            if cur > best:
                best = cur
        if best >= min_run:
            out.append(y)
    return out


def line_blocks(img, color, tol, region, min_pix=4, min_h=3, gap=1):
    """把区域内有墨的行聚成「文本行块」，返回 [(块起始绝对 y, 块高, 峰值行像素数), ...]。

    band_rows 只回答「哪些行有色」，量不出**行距**；而行距正是 G2（基线/行距虚高）
    唯一能被机器判出来的形式，所以必须先分块再取块首。

    min_pix 滤掉抗锯齿散点（实测原生 Stat 帧里 1~2 个噪点会自成一个"假行"），
    gap 允许块内夹一行空白（如只有标点的一行），min_h 滤掉单行噪点块。
    """
    x0, y0, x1, y1 = region
    counts = []
    for y in range(y0, y1):
        counts.append(int(color_mask(img[y:y + 1, x0:x1], color, tol)[0].sum()))
    ink = [i for i, c in enumerate(counts) if c >= min_pix]
    out = []
    start = prev = None
    for i in ink:
        if start is None:
            start = prev = i
        elif i - prev <= gap:
            prev = i
        else:
            out.append((start, prev - start + 1, max(counts[start:prev + 1])))
            start = prev = i
    if start is not None:
        out.append((start, prev - start + 1, max(counts[start:prev + 1])))
    return [(y0 + s, h, pk) for s, h, pk in out if h >= min_h]


def _fft_corr(img, kern):
    """img 与 kern 的有效互相关：out[y,x] = Σ_i,j img[y+i,x+j]*kern[i,j]，kern 不翻转。

    用「kern 上下左右翻转后做循环相关」实现标准模板滑动（SSD 展开式里要用）。
    """
    fh, fw = img.shape
    kh, kw = kern.shape
    H = 1 << (fh + kh - 1).bit_length()
    W = 1 << (fw + kw - 1).bit_length()
    F = np.fft.rfft2(img, (H, W))
    K = np.fft.rfft2(kern[::-1, ::-1], (H, W))
    full = np.fft.irfft2(F * K, (H, W))
    # 相关结果的 (kh-1, kw-1) 处起是首个完整重叠位置
    return full[kh - 1:fh, kw - 1:fw]


def anchor_in(f, tmpl, alpha_min=250, verbose=False):
    """在 ndarray 帧（或帧的一块裁剪）里找 tmpl 位图的左上角。

    只统计 alpha>=alpha_min 的像素：nx 位图的半透明描边在窗口叠放后颜色会变，
    把这类像素计入分数会让正确锚点的 rmse 虚高（实测 backgrnd 由 0 涨到 80）。

    SSD(x,y) = Σ f²·a - 2Σ f·(t·a) + Σ (t·a)²   三项全部用 FFT 相关一次算完。

    判成功用**吻合像素比 agree**，不是 rmse：窗口底衬之上必然还要画按钮/数字/名条，
    实测原生 Stat/backgrnd 有 21% 的像素被覆盖（集中在属性值区 y=200..340），
    按 rmse 设阈值会把正确锚点误判成「这张帧不能用」。
    反过来说，agree 掉到反常的低也正好暴露 G3 那一类缺陷（同点重复画底 / 多画一层）。

    返回 (x, y, rmse, agree)；模板比帧大或实心像素不足返回 None。
    """
    t = np.asarray(Image.open(resolve(tmpl)).convert("RGBA"), dtype=np.float64)
    if t.ndim == 2:  # nx_inspect 会把灰度位图存成 2D：整块视作不透明
        t = np.dstack([t, t, t, np.full_like(t, 255.0)])
    a = (t[:, :, 3] >= alpha_min).astype(np.float64)
    if a.sum() < 16:
        return None
    tm = t[:, :, :3] * a[:, :, None]
    h, w = a.shape
    if h > f.shape[0] or w > f.shape[1]:
        return None
    ssd = None
    for c in range(3):
        term = _fft_corr(f[:, :, c] ** 2, a) - 2.0 * _fft_corr(f[:, :, c], tm[:, :, c]) + (tm[:, :, c] ** 2).sum()
        ssd = term if ssd is None else ssd + term
    n = a.sum()
    iy, ix = np.unravel_index(np.argmin(ssd), ssd.shape)
    rmse = float(np.sqrt(max(ssd[iy, ix], 0.0) / n))
    agree = _agreement(f, t, a, int(ix), int(iy))
    if verbose:
        s = np.sort(ssd.ravel())[:5]
        print("   top5 rmse: %s" % ["%.1f" % float(np.sqrt(max(v, 0) / n)) for v in s])
        print("   anchor agree=%.3f" % agree)
    return (int(ix), int(iy), rmse, agree)


def template_anchor(frame, tmpl, alpha_min=250, verbose=False):
    """anchor_in 的「按路径」入口：frame 是图片文件路径。"""
    return anchor_in(load_rgb(resolve(frame)), tmpl, alpha_min, verbose)


def overlap_sad(f, tmpl_path, x, y, alpha_min=250):
    """把模板贴在帧的 (x,y)，**允许出界**，只比重叠的实心像素：返回 (meanSAD, agree, n)。

    anchor_in 硬性要求模板整张落在帧内。紧裁的原生帧（帧高 == 图高，实测
    native_bag_fold 179x289 vs Item/backgrnd 175x289）于是只能取 dy=0 —— 竖直锚点
    是被「装得下」这个约束强制出来的，不是内容对上的结果。Δ=1 的系统性伪影就来自这里，
    量它必须换成这个允许出界的口径。
    """
    t = np.asarray(Image.open(resolve(tmpl_path)).convert("RGBA"), dtype=np.float64)
    if t.ndim == 2:
        t = np.dstack([t, t, t, np.full_like(t, 255.0)])
    a = (t[:, :, 3] >= alpha_min)
    h, w = a.shape
    y0t, y1t = max(0, -y), min(h, f.shape[0] - y)
    x0t, x1t = max(0, -x), min(w, f.shape[1] - x)
    if y1t <= y0t or x1t <= x0t:
        return None
    m = a[y0t:y1t, x0t:x1t]
    n = int(m.sum())
    if n < 16:
        return None
    art = t[:, :, :3][y0t:y1t, x0t:x1t] * m[:, :, None]
    sub = f[y0t + y:y1t + y, x0t + x:x1t + x].astype(np.float64)
    d = np.abs(sub - art).max(axis=2)
    return (float(d[m].mean()), float((d[m] <= 24.0).mean()), n)


def _agreement(f, t, a, x, y, tol=24):
    """锚点处「与模板逐通道差 <= tol」的实心像素占比（0..1）。"""
    h, w = a.shape
    sub = f[y:y + h, x:x + w, :][a > 0]
    want = t[:, :, :3][a > 0]
    if sub.shape[0] == 0:
        return 0.0
    return float((np.abs(sub - want).max(axis=1) <= tol).mean())


# ----------------------------------------------------------------- 探针执行

def run_probe(img, p, anchor):
    """量一个探针，返回窗口相对坐标结果 dict；找不到返回 None。"""
    ax, ay = anchor
    reg = (max(0, p["region"][0] + ax), max(0, p["region"][1] + ay),
           min(img.shape[1], p["region"][2] + ax), min(img.shape[0], p["region"][3] + ay))
    color = p.get("color")
    tol = p.get("tol", 40)
    kind = p.get("kind", "blob")
    if kind == "tmpl":
        # 直接在搜索区里模板匹配这张 nx 位图：位置是机器量出来的，不靠人猜颜色
        sub = img[reg[1]:reg[3], reg[0]:reg[2]]
        r = anchor_in(sub, p["tmpl"])
        if not r or r[3] < p.get("min_agree", 0.8):
            return None
        return {"x": reg[0] - ax + r[0], "y": reg[1] - ay + r[1], "w": 0, "h": 0, "n": 0,
                "agree": round(r[3], 3)}
    if kind == "blob":
        hit = find_box(img[reg[1]:reg[3], reg[0]:reg[2]], color, tol, None, p.get("min_pix", 3),
                       p.get("row_min", 1), p.get("col_min", 1))
        if not hit:
            return None
        return {"x": reg[0] - ax + hit[0], "y": reg[1] - ay + hit[1],
                "w": hit[2] - hit[0], "h": hit[3] - hit[1], "n": hit[4]}
    if kind == "band":
        rows = band_rows(img, color, tol, reg, p.get("min_run", 8))
        if not rows:
            return None
        return {"x": reg[0] - ax, "y": rows[0] - ay, "w": 0, "h": len(rows), "n": len(rows),
                "rows": [r - ay for r in rows]}
    if kind == "lines":
        blocks = line_blocks(img, color, tol, reg, p.get("min_pix", 4),
                             p.get("min_h", 3), p.get("gap", 1))
        if not blocks:
            return None
        starts = [b[0] - ay for b in blocks]
        return {"x": reg[0] - ax, "y": starts[0], "w": 0, "h": len(blocks), "n": sum(b[2] for b in blocks),
                "rows": starts, "heights": [b[1] for b in blocks]}
    raise ValueError("unknown probe kind: %s" % kind)


def fmt(v):
    if v is None:
        return "MISSING"
    if "heights" in v:  # lines：行首 + 行距 + 墨高，行距是 G2 的直接判据
        pitch = [b - c for b, c in zip(v["rows"][1:], v["rows"][:-1])]
        return "n=%d rows=%s pitch=%s ink=%s" % (v["h"], v["rows"], pitch, v["heights"])
    if "rows" in v:
        return "y=%d n=%d rows=%s" % (v["y"], v["h"], v["rows"][:10])
    if "agree" in v:
        return "(%d,%d) agree=%.3f" % (v["x"], v["y"], v["agree"])
    return "(%d,%d) %dx%d" % (v["x"], v["y"], v["w"], v["h"])


def cmp_probe(oracle, actual, keys=("x", "y"), size_keys=(), tol=2):
    """逐键求差 -> (worst_abs_delta, detail)。任一侧 MISSING 记 999。

    band/lines 探针额外比 rows 列表本身：网格「行距对不对」和「行数齐不齐」是两个独立
    缺陷（少一行 = 面板矮了；行距漂 = 每格累加偏移），只看首行会漏。
    行首按**逐行差 ≤ tol** 判，不要求逐字节相等：文字行的「墨迹顶」受抗锯齿影响，
    两端字体锐度不同会让探测器落在相邻一行上（r27 实测：原生 9 行墨、客户同名行 7 行，
    中心同为 y=40，但墨迹顶探测器读数差 1px）——那是探测器口径，不是版式缺陷。
    """
    if oracle is None or actual is None:
        return (999, "oracle=%s actual=%s" % (fmt(oracle), fmt(actual)))
    worst, det = 0, []
    for k in keys:
        d = int(actual.get(k, 0) - oracle.get(k, 0))
        det.append("%s%+d" % (k, d))
        worst = max(worst, abs(d))
    for k in size_keys:
        worst = max(worst, abs(int(actual.get(k, 0) - oracle.get(k, 0))))
    if "rows" in oracle:
        o, a = oracle["rows"], actual.get("rows", [])
        if len(o) != len(a):
            pitch = lambda r: [b - c for b, c in zip(r[1:], r[:-1])]  # noqa: E731
            det.append("行数 %d!=%d(行距=%s/%s)" % (len(a), len(o), pitch(a), pitch(o)))
            worst = max(worst, 99)
        else:
            dmax = max(abs(int(x) - int(y)) for x, y in zip(a, o))
            det.append("行首Δmax=%d" % dmax)
            if dmax > tol:
                det.append("超差行=%s" % [(i, int(x) - int(y))
                                        for i, (x, y) in enumerate(zip(a, o)) if abs(x - y) > tol])
            worst = max(worst, dmax)
    return (worst, " ".join(det))


# ----------------------------------------------------------------- spec 读写

def load_spec(path):
    with open(resolve(path), "r", encoding="utf-8") as fh:
        return json.load(fh)


_NUMARR = re.compile(r"\[[\s\n]*-?\d+(?:,[\s\n]*-?\d+)*[\s\n]*\]")


def _collapse(m):
    nums = [t for t in re.split(r"[^0-9-]+", m.group(0)) if t]
    return "[" + ", ".join(nums) + "]"


def dump_spec(path, spec):
    """写回 spec：数字数组（region / color / oracle_anchor）压成一行，保持可读的 git diff。"""
    text = _NUMARR.sub(_collapse, json.dumps(spec, ensure_ascii=False, indent=2))
    with open(path if os.path.isabs(path) else os.path.abspath(path), "w", encoding="utf-8") as fh:
        fh.write(text + "\n")


def probe_active(p, mode):
    """该探针在「mode 形态的窗口」里是否应当存在。

    同一张窗有些格子只在某个开关状态下画出来（Stat 的详情板 = 「详细信息」模式）。
    这类探针在别的形态下必然 MISSING，按缺失判 FAIL 等于让门禁永远红着，
    于是「真缺陷」和「这轮不适用」再也分不开 —— 所以缺 active_in 之外一律报 SKIP。
    """
    ins = p.get("active_in")
    return not ins or mode in ins


def measure_all(spec, frame, force_anchor=None, mode=None, anchor_override=None):
    """锚定 + 量全部探针。锚定失败返回 (None, None) —— 该帧判不了几何。

    mode=None（mark / selftest 用）表示「全部照量」，因为 oracle 是从内容齐全的那张
    原生帧上取的；只有 check 才按形态跳过不适用探针。

    force_anchor 只给 selftest 用：注入偏移时可能顺手把锚点图也搬烂（band/lines 要清空
    整个探针区，面积一大 agree 就掉到阈值下），此时退回 spec 里已记录的锚点继续量探针，
    否则「门禁能不能抓偏移」这件事本身会被锚定失败掩盖成崩溃。

    anchor_override 是 mark 的 --anchor：紧裁帧的自动锚点不可信（见 overlap_sad / bias），
    用 bias 量出来的那一格显式供进来，探针值仍然是从帧上量出来的。
    """
    img = load_rgb(resolve(frame))
    if anchor_override:
        anchor = tuple(anchor_override)
        print("anchor=(%d,%d) <- 指定的锚点，未做自动模板匹配（紧裁帧的 dy 会被尺寸约束强制，见 bias）" % anchor)
        return anchor, _measure(img, spec, anchor, mode)
    a = template_anchor(frame, spec["anchor_tmpl"])
    min_agree = spec.get("anchor_min_agree", 0.7)
    if not a or a[3] < min_agree:
        if not force_anchor:
            print("!! 窗口锚定失败（agree=%s，阈值 %.2f）：这张帧只能看内容、不能量版式"
                  "（抓帧被压暗时先跑 bright.py，见 notes/open-issues.md）"
                  % ("%.3f" % a[3] if a else "None", min_agree))
            return None, None
        print("!! 窗口锚定失败（agree=%s，阈值 %.2f）→ 改用 spec 记录的锚点 %s 量探针"
              % ("%.3f" % a[3] if a else "None", min_agree, tuple(force_anchor)))
        anchor = tuple(force_anchor)
    else:
        anchor = (a[0], a[1])
        print("anchor=(%d,%d) agree=%.3f rmse=%.1f  <- %s" % (anchor[0], anchor[1], a[3], a[2], spec["anchor_tmpl"]))
    return anchor, _measure(img, spec, anchor, mode)


def _measure(img, spec, anchor, mode):
    res = {}
    for p in spec["probes"]:
        if mode and not probe_active(p, mode):
            continue
        try:
            res[p["name"]] = run_probe(img, p, anchor)
        except Exception as exc:  # 单格异常不拖垮整轮
            print("   probe %s 出错: %s" % (p["name"], exc))
            res[p["name"]] = None
    return res


# ----------------------------------------------------------------- 子命令

def cmd_geom(argv):
    sys.path.insert(0, os.path.join(REPO, "tools"))
    from nx_inspect import Nx
    nx = Nx(resolve(argv[0]))
    for path in argv[1:]:
        i = nx.find(path)
        if i is None:
            print("== %s ==  NOT FOUND" % path)
            continue
        print("== %s ==" % path)
        for c in nx.children(i):
            _, _, _, ntype, data, w, h = nx.node(c)
            origin = nx.value(c) if ntype == 4 else ("=%s" % nx.value(c) if ntype == 1 else "")
            print("  %-18s type=%d size=%dx%d origin/val=%s" % (nx.name(c), ntype, w, h, origin))
    return 0


def cmd_anchor(args):
    r = template_anchor(args.frame, args.tmpl, verbose=True)
    if not r:
        print("无法匹配（模板过大 / 全透明）")
        return 2
    print("anchor=(%d, %d)  rmse=%.2f  agree=%.3f  tmpl=%s" % (r[0], r[1], r[2], r[3], args.tmpl))
    if args.png:
        im = Image.open(resolve(args.frame)).convert("RGB")
        t = Image.open(resolve(args.tmpl))
        d = ImageDraw.Draw(im)
        d.rectangle([r[0], r[1], r[0] + t.width - 1, r[1] + t.height - 1], outline=(0, 255, 0))
        im.save(os.path.abspath(args.png))
        print("saved %s" % args.png)
    return 0


def cmd_bias(args):
    """审计 spec 记录的锚点：允许模板出界，重算 ±span 网格的 meanSAD。

    为什么要这条：oracle 只能被「量」出来，但**锚点本身也是量出来的**，而紧裁的原生帧
    上自动锚点的 dy 是被尺寸约束强制成 0 的。锚点偏 1px，整张 spec 的每一格就齐刷刷
    偏 1px —— 三个不同窗口同时报 Δ=1 时，追的是门禁，不是客户端常量。
    """
    spec = load_spec(args.spec)
    frame = args.frame or spec.get("oracle_frame")
    if not frame or not os.path.exists(resolve(frame)):
        print("需要 --frame 或 spec 里的 oracle_frame")
        return 2
    f = load_rgb(resolve(frame))
    rec = tuple(spec["oracle_anchor"])
    span = args.span
    grid = {}
    for dy in range(-span, span + 1):
        row = []
        for dx in range(-span, span + 1):
            r = overlap_sad(f, spec["anchor_tmpl"], rec[0] + dx, rec[1] + dy)
            row.append(r)
        grid[dy] = row
    best = min(((r[0], dx, dy) for dy in grid for (dx, r) in
                zip(range(-span, span + 1), grid[dy]) if r), key=lambda t: t[0], default=None)
    print("spec=%s  frame=%s  记录锚点=(%d,%d)" % (args.spec, frame, rec[0], rec[1]))
    print("   meanSAD（允许出界，越小越对）：")
    print("        dy\\dx " + "".join("%8d" % dx for dx in range(-span, span + 1)))
    for dy in range(-span, span + 1):
        cells = []
        for r in grid[dy]:
            cells.append("%8.2f" % r[0] if r else "     ---")
        tag = "  <= 记录" if dy == 0 else ""
        print("   %+5d%s%s" % (dy, "  ".join(cells), tag))
    rec_cell = grid[0][span]
    print("   记录格 meanSAD=%.2f agree=%.3f n=%d" % (rec_cell or (float("nan"),) * 3))
    if best and (best[1], best[2]) != (0, 0):
        print("!! 最优偏移 (dx=%+d, dy=%+d) meanSAD=%.2f -> 锚点应为 (%d,%d)"
              % (best[1], best[2], best[0], rec[0] + best[1], rec[1] + best[2]))
        print("   校正：mark --anchor %d %d（探针值仍从帧上量，只是参照点换成量出来的那一格）"
              % (rec[0] + best[1], rec[1] + best[2]))
        return 1
    print("   OK：记录锚点就是最优格，Δ=1 不是锚点伪影")
    return 0


def cmd_mark(args):
    spec = load_spec(args.spec)
    anchor, res = measure_all(spec, args.frame, anchor_override=args.anchor)
    if res is None:
        return 2
    for p in spec["probes"]:
        p["oracle"] = res[p["name"]]
    # oracle 帧要随 spec 一起入库，绝对路径换机器就失效；落在仓库内时存仓库相对路径。
    rf = resolve(args.frame)
    rel = os.path.relpath(rf, REPO).replace("\\", "/")
    spec["oracle_frame"] = rf if rel.startswith("..") else rel
    spec["oracle_anchor"] = list(anchor)
    dump_spec(resolve(args.spec), spec)
    print("已写入 oracle：%d 个探针 -> %s" % (len(spec["probes"]), args.spec))
    for p in spec["probes"]:
        print("  %-26s %s" % (p["name"], fmt(p["oracle"])))
    return 0


def recorded_anchor_if_oracle(spec, frame):
    """量的是 spec 自己那张 oracle 帧时，直接用记录好的锚点；别的帧返回 None（照常自动锚定）。

    锚点本身也是量出来的，而紧裁的原生帧上自动锚点不可信：anchor_in 不许模板出界，
    帧高 == 图高时 dy 被尺寸约束强制成 0（bias 子命令专治这个）。mark 已经用校正过的
    锚点写下 oracle，对同一张帧再自动重锚等于把校正抹掉 —— 实测 Δ 凭空多出 1px，
    三条探针整列齐偏，追的不是客户端常数而是门禁自己。真机帧的窗口位置未知，必须自动锚。
    """
    of = spec.get("oracle_frame")
    if not of or "oracle_anchor" not in spec:
        return None
    try:
        if os.path.samefile(resolve(frame), resolve(of)):
            return tuple(spec["oracle_anchor"])
    except OSError:
        return None
    return None


def cmd_check(args):
    spec = load_spec(args.spec)
    anchor, res = measure_all(spec, args.frame, mode=args.mode,
                              anchor_override=recorded_anchor_if_oracle(spec, args.frame))
    if res is None:
        return 2
    tol = spec.get("tol", 2)
    fails, skipped, rows = 0, 0, []
    for p in spec["probes"]:
        if not probe_active(p, args.mode):
            skipped += 1
            rows.append((p["name"], fmt(p.get("oracle")), "-", "-", "SKIP(不在 %s 形态)" % args.mode))
            continue
        orc, act = p.get("oracle"), res.get(p["name"])
        if orc is None:
            rows.append((p["name"], "-", fmt(act), "-", "NO-ORACLE"))
            continue
        keys = tuple(p.get("keys", ("x", "y")))
        size_keys = tuple(p.get("size_keys", ()))
        d, det = cmp_probe(orc, act, keys, size_keys, tol)
        bad = (d > tol) or act is None
        fails += 1 if bad else 0
        rows.append((p["name"], fmt(orc), fmt(act), "\u0394=%d %s" % (d, det), "FAIL" if bad else "ok"))
    w = max(len(r[0]) for r in rows) + 2
    print("\n%s  tol=%dpx  形态=%s  锚点=(%d,%d)  帧=%s"
          % (spec["window"], tol, args.mode, anchor[0], anchor[1], args.frame))
    print("-" * 104)
    for name, o, a, delta, verdict in rows:
        print("  %-*s oracle=%-26s 实测=%-24s %-24s %s" % (w, name, o, a, delta, verdict))
    print("-" * 104)
    print("不合格 %d / %d（不适用跳过 %d）" % (fails, len(rows) - skipped, skipped))
    if args.png:
        annotate(spec, args.frame, args.png, anchor, res, rows)
        print("对照图 %s" % args.png)
    return 1 if fails else 0


def annotate(spec, frame, out, anchor, res, rows):
    """绿框=oracle 实测=蓝；不合格实测=红，探针区=淡蓝，窗口锚=黄。给人眼复核用。"""
    im = Image.open(resolve(frame)).convert("RGB")
    d = ImageDraw.Draw(im)
    bad = {r[0] for r in rows if r[4] in ("FAIL", "NO-ORACLE")}
    t = Image.open(resolve(spec["anchor_tmpl"]))
    d.rectangle([anchor[0], anchor[1], anchor[0] + t.width - 1, anchor[1] + t.height - 1], outline=(255, 255, 0))
    for p in spec["probes"]:
        x0, y0 = p["region"][0] + anchor[0], p["region"][1] + anchor[1]
        x1, y1 = p["region"][2] + anchor[0], p["region"][3] + anchor[1]
        d.rectangle([x0, y0, x1, y1], outline=(120, 120, 255))
        _mark(d, p.get("oracle"), anchor, (0, 255, 0))
        _mark(d, res.get(p["name"]), anchor, (255, 0, 0) if p["name"] in bad else (0, 200, 255))
        d.text((x0, max(0, y0 - 11)), p["name"], fill=(255, 255, 0))
    # 输出路径不能走 resolve()：目标还不存在时会退回仓库根，落到错地方
    im.save(os.path.abspath(out))


def _mark(d, v, anchor, color):
    """有尺寸画框；tmpl/band 探针只标一个十字，避免 0x0 矩形看不见。"""
    if not v:
        return
    x, y = v["x"] + anchor[0], v["y"] + anchor[1]
    if v.get("w") or v.get("h"):
        d.rectangle([x, y, x + v["w"], y + v["h"]], outline=color)
    else:
        d.line([x - 4, y, x + 4, y], fill=color)
        d.line([x, y - 4, x, y + 4], fill=color)
        d.rectangle([x - 4, y - 4, x + 4, y + 4], outline=color)


def cmd_probe(args):
    img = load_rgb(args.frame)
    color = [int(v) for v in args.color.split(",")]
    print(fmt(find_box(img, color, args.tol, tuple(args.region), args.min_pix)))
    return 0


def fill_far(base, color, tol):
    """把擦除色推到「不会命中探针色」的那一侧。

    注入偏移要先把原内容擦掉；如果擦出来的实色本身就在探针色的容差里，那这块纯色
    会被探针当成新内容量一遍，报出来的 Δ 与注入量毫无关系（实测 Equip/grid_top_edge
    注入 1/14/32 全报 Δ=-12 —— 那 12 就是纯色块顶到探针区上沿的距离）。
    """
    base = [int(v) for v in base]
    if any(abs(base[c] - color[c]) > tol + 8 for c in range(3)):
        return tuple(base)
    c = max(range(3), key=lambda i: max(255 - color[i], color[i]))
    v = color[c] + tol + 9 if color[c] + tol + 9 <= 255 else color[c] - tol - 9
    base[c] = max(0, min(255, int(v)))
    return tuple(base)


def cmd_selftest(args):
    """把 oracle 帧里的一块内容整体下移 N 像素，验证 check 报出来的 Δ 正好等于 N。

    门禁自己也得有证据：如果注入 14px 偏移它却报 ok，那「逐格比对」就是假的，
    后面 B2..B7 全部建立在它上面。默认注入三种（1=亚像素容差内应判 ok，14/60=应 FAIL）。
    """
    spec = load_spec(args.spec)
    frame = resolve(spec.get("oracle_frame")) if spec.get("oracle_frame") else getattr(args, "frame", None)
    if not frame or not os.path.exists(frame):
        print("需要 spec 里已有 oracle_frame（先跑一次 mark）")
        return 2
    im = Image.open(frame).convert("RGB")
    src = np.asarray(im).astype(np.int16)
    anchor = tuple(spec["oracle_anchor"])
    probe = next(p for p in spec["probes"] if p["name"] == args.probe)
    orc = probe["oracle"]
    if not orc:
        print("探针 %s 没有 oracle" % args.probe)
        return 2
    color = [int(v) for v in probe.get("color", [0, 0, 0])]
    tol = int(probe.get("tol", 0))
    # 被搬动的内容 = 该探针在 oracle 帧上的 bbox
    bx0, by0 = anchor[0] + orc["x"], anchor[1] + orc["y"]
    bx1, by1 = bx0 + orc["w"], by0 + orc["h"]
    # lines / band 的 oracle 只存行号，w/h 是「行数」不是像素尺寸——照 bbox 搬等于没搬，
    # 实测注入 16px 仍报 Δ=0（假 ok）。这两类改成搬整个探针区：区里所有行一起平移，
    # 才是「窗口常量整体写错 N 像素」的真实形状。
    if args.probe_kind in ("lines", "band"):
        rx0, ry0, rx1, ry1 = probe["region"]
        bx0, by0, bx1, by1 = anchor[0] + rx0, anchor[1] + ry0, anchor[0] + rx1, anchor[1] + ry1
    # tmpl 的 oracle 只有一个点，w/h=0 ⇒ 搬 0 像素等于没搬（实测注入仍报 Δ=0）。
    # 这类要搬的是**模板自身那一块**。
    if args.probe_kind == "tmpl":
        tw, th = Image.open(resolve(probe["tmpl"])).size
        bx1, by1 = bx0 + tw, by0 + th
    # 锚点可以带负分量（bias 校正过的紧裁帧，实测 Item = (3,-1)），这时探针区映射到帧上是
    # 负行号，而 numpy 的 src[-1:288] 是**空切片** —— median 出 NaN、fill_far 直接崩。
    # 量具的坐标一律先夹进帧内（run_probe 本来也这么钳）。
    bx0, by0 = max(0, bx0), max(0, by0)
    bx1, by1 = min(src.shape[1], bx1), min(src.shape[0], by1)
    if bx1 <= bx0 or by1 <= by0:
        print("注入区被钳空：这条探针的内容不落在这张帧里，换探针或换帧")
        return 2
    if args.probe_kind == "blob":
        bg = np.asarray(Image.open(resolve(spec["anchor_tmpl"])).convert("RGB")).astype(np.int16)
        fill = fill_far(np.median(bg.reshape(-1, 3), axis=0), color, tol)
    else:
        fill = fill_far(np.median(src[by0:by1, bx0:bx1].reshape(-1, 3), axis=0), color, tol)
    ok = True
    for shift in [int(v) for v in args.shifts.split(",")]:
        arr = src.copy()
        block = arr[by0:by1, bx0:bx1].copy()
        arr[by0:by1, bx0:bx1] = fill
        y0, y1 = by0 + shift, by1 + shift
        if y1 > arr.shape[0]:
            y1 = arr.shape[0]
        arr[y0:y1, bx0:bx1] = block[:y1 - y0]
        tmp = os.path.abspath("._selftest_%d.png" % shift)
        Image.fromarray(arr.astype(np.uint8)).save(tmp)
        _, res = measure_all(spec, tmp, anchor_override=anchor)
        got = res.get(args.probe)
        os.remove(tmp)
        if got is None:
            print("  shift=%-3d -> MISSING（内容被推离探针区，也算检出）" % shift)
            reported = 999
        else:
            reported = got["y"] - orc["y"]
        verdict = "检出" if abs(reported - shift) <= 1 or reported == 999 else "漏检!"
        if verdict != "检出":
            ok = False
        print("  shift=%-3d 注入  实测 Δ=%-4d  %s" % (shift, reported, verdict))
    print("自测%s" % ("通过：门禁能按像素复现注入偏移" if ok else "失败：门禁不可信，先修工具"))
    return 0 if ok else 1


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)

    sub.add_parser("geom").add_argument("argv", nargs="+")

    a = sub.add_parser("anchor")
    a.add_argument("frame")
    a.add_argument("tmpl")
    a.add_argument("--png")

    m = sub.add_parser("mark")
    m.add_argument("spec")
    m.add_argument("frame")
    m.add_argument("--anchor", nargs=2, type=int, metavar=("X", "Y"),
                   help="用 bias 量出来的锚点覆盖自动锚点（紧裁帧的 dy 会被自动锚点强制成 0）")

    b = sub.add_parser("bias")
    b.add_argument("spec")
    b.add_argument("frame", nargs="?", default=None)
    b.add_argument("--span", type=int, default=2)

    c = sub.add_parser("check")
    c.add_argument("spec")
    c.add_argument("frame")
    c.add_argument("--png")
    c.add_argument("--mode", default="default",
                   help="窗口形态（对照探针的 active_in；抓的是详细模式帧就传 detail）")

    p = sub.add_parser("probe")
    p.add_argument("frame")
    p.add_argument("region", nargs=4, type=int)
    p.add_argument("--color", required=True)
    p.add_argument("--tol", type=int, default=40)
    p.add_argument("--min-pix", dest="min_pix", type=int, default=3)

    s = sub.add_parser("selftest")
    s.add_argument("spec")
    s.add_argument("--probe", default="panel_text_bbox")
    s.add_argument("--probe-kind", dest="probe_kind", default="blob")
    s.add_argument("--shifts", default="1,14,60")

    ns = ap.parse_args()
    if ns.cmd == "geom":
        return cmd_geom(ns.argv)
    if ns.cmd == "anchor":
        return cmd_anchor(ns)
    if ns.cmd == "mark":
        return cmd_mark(ns)
    if ns.cmd == "bias":
        return cmd_bias(ns)
    if ns.cmd == "check":
        return cmd_check(ns)
    if ns.cmd == "selftest":
        return cmd_selftest(ns)
    return cmd_probe(ns)


if __name__ == "__main__":
    sys.exit(main())
