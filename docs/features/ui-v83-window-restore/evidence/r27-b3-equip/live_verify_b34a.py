# -*- coding: utf-8 -*-
"""R27-B3-4a 真机自证：技能窗「绘制格 == 命中格」（tooltip 连通块判据版）。

判据（对应 CHECKLIST D7c 的手感项，做成可复算的像素判据）：
  J1 图标本格内取 3 点（+4 / 格心 / +28）-> 都弹出 tooltip。
  J2 本格外的「行空隙」(y=+36) 与「列空隙」(x=+36) -> 不弹 tooltip。
      改造前 UISkillBook 的命中是 (y-base)/ROW_HEIGHT 整段除法，40px 行距全算一格
      -> 这 8px 空隙会误认成下一行；现在只认 Icon 自己的本格（32x32）。
  J3 tooltip 报的就是压住的那一格，而不是邻格：
      J3a（过闸）同格 3 点的 tooltip 宽高一致，且框的位移 == 光标的位移（逐像素相等）
          -> 三处是同一个框，只是跟着光标走。
      J3b（诊断，不过闸）同探针位跨行比，看离自己那行的参考帧是否最近。
          相邻两行的三行式 tooltip 会落在半透明底噪里（实测余量 -0.0003 ~ +0.0303）：
          tooltip 半透明，差集里混了「底下压的是什么」，像素级判据分不开两颗技能，
          所以这里只报数，不当闸。
      J3c（内容判据）落 4 张 tooltip 裁片 + 4 张行名裁片，读图确认
          tooltip 标题与框内技能图标 == 该行技能。
          本轮读数：精灵的祝福 / 蜗牛投掷术 / 团队治疗 / 疾风步，与行序逐一对上。

tooltip 用「与基线帧的差集中最大的连通块」定位：地图背景动图是散点，
tooltip 是整块 -> 面积闸能分开。该连通块的二值形状即 tooltip 的像素签名。

窗口落位不猜：Settings 里 ui.position.skill=(560,99)（view 坐标），
抓帧坐标 = view + 非客户区边框 (8,31)；格子与渲染同源
（skill_cell() + SKILL_META_OFFSET）。
"""
import json
import subprocess
import sys
import time

import numpy as np
from PIL import Image

sys.path.insert(0, 'E:/project')
import _drive  # noqa: E402

HWND = int(sys.argv[1]) if len(sys.argv) > 1 else _drive.find_hwnd()
OUT = 'E:/project/_r27_b34a_hover'

BORDER = (8, 31)                # PrintWindow 帧 = 客户区 + 边框
WIN_VIEW = (560, 99)            # Settings: ui.position.skill
CLASSIC_OFFSET = (7, 99)        # UISkillBook.h CLASSIC_SKILL_OFFSET（行底落点）
META_OFFSET = (2, 2)            # UISkillBook.h SKILL_META_OFFSET（图标本格 = 行底 + META）
ROW_HEIGHT = 40                 # UISkillBook.h ROW_HEIGHT
CELL = 32                       # ms::Icon::cell_width/height
ROWS = 4                        # 新手可见技能行数

TIP_MIN_AREA = 1200             # 连通块面积闸（小于它 = 没有 tooltip）
T_BBOX_SIZE = 2                 # J3a：同格 tooltip 宽高差上限（像素）
T_ANCHOR = 1                    # J3a：框位移与光标位移的容差（像素）

IN_PROBES = (('in-TL', 4, 4), ('in-C', 16, 16), ('in-BR', CELL - 4, CELL - 4))


def cap(tag):
    out = '%s_%s.png' % (OUT, tag)
    subprocess.run([sys.executable, 'E:/project/_cap2.py', out, str(HWND)],
                   check=True, capture_output=True)
    return np.asarray(Image.open(out).convert('RGB')).astype(np.int16), out


def cell_frame(row):
    """图标本格左上（抓帧坐标）= 窗口 + CLASSIC + META + (0, ROW_HEIGHT*row)。"""
    return (WIN_VIEW[0] + BORDER[0] + CLASSIC_OFFSET[0] + META_OFFSET[0],
            WIN_VIEW[1] + BORDER[1] + CLASSIC_OFFSET[1] + META_OFFSET[1] + ROW_HEIGHT * row)


def move(fx, fy):
    ox, oy = _drive.client_origin(HWND)
    _drive.user32.SetCursorPos(ox + fx - BORDER[0], oy + fy - BORDER[1])
    time.sleep(0.30)


def largest_blob(base, cur, near):
    """返回 (bbox, area, sig)：与基线差集中、光标邻域内最大的连通域。

    sig = 该连通域按 bbox 裁切后的二值图，即 tooltip 的像素签名。
    """
    d = np.abs(base - cur).sum(2)
    mask = d > 60
    # 光标周围 260x220 才可能是它的 tooltip，别的区域（会动的地图）不参与
    x0, y0 = max(near[0] - 40, 0), max(near[1] - 40, 0)
    x1, y1 = min(x0 + 300, mask.shape[1]), min(y0 + 260, mask.shape[0])
    sub = mask[y0:y1, x0:x1]
    if not sub.any():
        return None, 0, None

    seen = np.zeros(sub.shape, bool)
    best = (0, None, None)
    ys, xs = np.where(sub)
    order = np.argsort(ys * 4096 + xs)
    for idx in order:
        sy, sx = int(ys[idx]), int(xs[idx])
        if seen[sy, sx]:
            continue
        stack = [(sy, sx)]
        seen[sy, sx] = True
        pts = []
        while stack:
            cy, cx = stack.pop()
            pts.append((cy, cx))
            for dy in (-1, 0, 1):
                for dx in (-1, 0, 1):
                    ny, nx = cy + dy, cx + dx
                    if 0 <= ny < sub.shape[0] and 0 <= nx < sub.shape[1] \
                            and sub[ny, nx] and not seen[ny, nx]:
                        seen[ny, nx] = True
                        stack.append((ny, nx))
        if len(pts) > best[0]:
            a = np.array(pts)
            comp = np.zeros(sub.shape, bool)
            comp[a[:, 0], a[:, 1]] = True
            box = (int(a[:, 1].min()), int(a[:, 0].min()),
                   int(a[:, 1].max()), int(a[:, 0].max()))
            best = (len(pts), (box[0] + x0, box[1] + y0, box[2] + x0, box[3] + y0),
                    comp[box[1]:box[3] + 1, box[0]:box[2] + 1])
    area, bbox, sig = best
    if area < TIP_MIN_AREA or bbox is None:
        return None, int(area), None
    return bbox, int(area), sig


def _pad(m, h, w):
    out = np.zeros((h, w), bool)
    out[:m.shape[0], :m.shape[1]] = m
    return out


def mask_disagree(a, b):
    """两块 tooltip 签名按左上对齐后的像素不一致比例（相对并集）。"""
    if a is None or b is None:
        return 9.0
    h = max(a.shape[0], b.shape[0])
    w = max(a.shape[1], b.shape[1])
    pa = _pad(a, h, w)
    pb = _pad(b, h, w)
    union = int((pa | pb).sum())
    if union == 0:
        return 9.0
    return float((pa ^ pb).sum()) / float(union)


def sheet(boxes, src_tag, path):
    """把各行裁片竖排成一张可读图（J3c 取证件）。

    src_tag='baseline' 取无 tooltip 的底帧（行名在那儿），否则取对应探针帧。
    """
    tiles = []
    for row, (x0, y0, x1, y1) in boxes:
        src = '%s_baseline.png' % OUT if src_tag == 'baseline' \
            else '%s_r%d_%s.png' % (OUT, row, src_tag)
        img = Image.open(src).convert('RGB')
        tiles.append(img.crop((max(x0, 0), max(y0, 0),
                               min(x1, img.width), min(y1, img.height))))
    out = Image.new('RGB', (max(t.width for t in tiles) + 8,
                            sum(t.height for t in tiles) + 6 * len(tiles) + 4),
                    (20, 20, 20))
    y = 4
    for t in tiles:
        out.paste(t, (4, y))
        y += t.height + 6
    out.save(path)
    return path


def main():
    _drive.user32.SetProcessDPIAware()
    if not _drive.foreground(HWND):
        raise SystemExit('ABORT: 抢不到前台，未发送任何输入')

    report = []

    def say(s):
        # stdout 是 cp936：不可编码的字符降级打印，report 仍保留原文
        print(s.encode('gbk', 'replace').decode('gbk'), flush=True)
        report.append(s)

    move(120, 600)
    base, base_path = cap('baseline')
    say('baseline=%s 窗口(抓帧)=%s 本格行起点=%s'
        % (base_path, (WIN_VIEW[0] + BORDER[0], WIN_VIEW[1] + BORDER[1]),
           [cell_frame(r) for r in range(ROWS)]))

    results = []
    sigs = {}
    for row in range(ROWS):
        cx, cy = cell_frame(row)
        probes = [(name, cx + dx, cy + dy) for name, dx, dy in IN_PROBES]
        probes += [('gap-Y', cx + 16, cy + 36), ('gap-X', cx + 36, cy + 16)]
        for name, fx, fy in probes:
            move(fx, fy)
            cur, _ = cap('r%d_%s' % (row, name))
            bbox, area, sig = largest_blob(base, cur, (fx, fy))
            sigs[(row, name)] = sig
            results.append(dict(row=int(row), probe=name, frame=[int(fx), int(fy)],
                                area=area, present=bbox is not None,
                                bbox=[int(v) for v in bbox] if bbox else None,
                                tipsize=[int(bbox[2] - bbox[0] + 1),
                                         int(bbox[3] - bbox[1] + 1)] if bbox else None))
            say('row%d %-6s 帧=(%d,%d) tooltip=%-4s 面积=%6d bbox=%s 尺寸=%s'
                % (row, name, fx, fy, '有' if bbox else '无', area, bbox,
                   ('%dx%d' % (bbox[2] - bbox[0] + 1, bbox[3] - bbox[1] + 1)) if bbox else '-'))
        move(120, 600)
        time.sleep(0.2)

    say('')
    say('=== 判定 ===')
    ok = True
    names = [p[0] for p in IN_PROBES]
    for row in range(ROWS):
        ins = [r for r in results if r['row'] == row and r['probe'] in names]
        gaps = [r for r in results if r['row'] == row and r['probe'].startswith('gap-')]
        all_in = all(r['present'] for r in ins)
        no_gap = all(not r['present'] for r in gaps)
        say('J1/J2 row%d: 格内3点全出tooltip=%s  空隙2点都不出=%s  (空隙面积=%s)'
            % (row, all_in, no_gap, [g['area'] for g in gaps]))
        ok = ok and all_in and no_gap

        ref = next(r for r in ins if r['probe'] == 'in-C')
        for other in ('in-TL', 'in-BR'):
            o = next(r for r in ins if r['probe'] == other)
            if not o['bbox'] or not ref['bbox']:
                say('   J3a row%d %-6s: 缺 bbox X' % (row, other))
                ok = False
                continue
            d_box = (o['bbox'][1] - ref['bbox'][1], o['bbox'][0] - ref['bbox'][0])
            d_cur = (o['frame'][1] - ref['frame'][1], o['frame'][0] - ref['frame'][0])
            size_d = max(abs(o['tipsize'][0] - ref['tipsize'][0]),
                         abs(o['tipsize'][1] - ref['tipsize'][1]))
            anchor_ok = (abs(d_box[0] - d_cur[0]) <= T_ANCHOR
                         and abs(d_box[1] - d_cur[1]) <= T_ANCHOR)
            say('   J3a row%d %-6s: 尺寸差=%d(<=%d) 框位移=%s 光标位移=%s 锚定=%s'
                % (row, other, size_d, T_BBOX_SIZE, d_box, d_cur,
                   'ok' if anchor_ok else 'X'))
            ok = ok and size_d <= T_BBOX_SIZE and anchor_ok

    # J3b（诊断，不过闸）：同探针位跨行比，看自己那行的参考帧是否最近。
    # 实测相邻两行的三行式 tooltip 落在半透明底噪里（余量 -0.0003 ~ +0.03），
    # 所以像素 1-NN 只能当参考，不能当判据；内容判据走 J3c 读图。
    say('   J3b 1-NN（诊断，不过闸；参考列 = 各行 in-C 的 tooltip 签名）:')
    j3b = []
    for probe in ('in-TL', 'in-BR'):
        for row in range(ROWS):
            ds = [(mask_disagree(sigs[(row, probe)], sigs[(rr, 'in-C')]), rr)
                  for rr in range(ROWS)]
            same = min(d for d, rr in ds if rr == row)
            cross = min(d for d, rr in ds if rr != row)
            j3b.append(dict(probe=probe, row=int(row), same=round(same, 4),
                            cross=round(cross, 4), margin=round(cross - same, 4),
                            win=bool(same < cross)))
            say('      %-6s row%d: 同格=%.4f  最近跨行=%.4f  余量=%+.4f %s'
                % (probe, row, same, cross, cross - same, '近' if same < cross else '搭'))

    bbox_of = {(r['row'], r['probe']): r['bbox'] for r in results}
    tips = sheet([(r, bbox_of[(r, 'in-C')]) for r in range(ROWS)],
                 'in-C', OUT + '_tips.png')
    names_sheet = sheet([(r, (cell_frame(r)[0] + 34, cell_frame(r)[1] - 8,
                              cell_frame(r)[0] + 200, cell_frame(r)[1] + 16))
                         for r in range(ROWS)],
                        'baseline', OUT + '_rownames.png')
    say('   J3c 裁片=%s , %s（tooltip 标题须 == 该行技能名，读图确认）' % (tips, names_sheet))

    say('总判定: %s' % ('PASS' if ok else 'FAIL'))

    with open(OUT + '.json', 'w', encoding='utf-8') as fh:
        json.dump(dict(window_view=WIN_VIEW, border=BORDER,
                       cells=[dict(row=int(r), cell=[int(v) for v in cell_frame(r)])
                              for r in range(ROWS)],
                       probes=results, j3b=j3b,
                       verdict='PASS' if ok else 'FAIL'), fh,
                  ensure_ascii=False, indent=1)
    with open(OUT + '.txt', 'w', encoding='utf-8') as fh:
        fh.write('\n'.join(report) + '\n')
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
