# -*- coding: utf-8 -*-
"""负对照：证明升级后的 Equip.json 分得出「按格改色」与「盖一层纯色」。

背景（README §3/§6.3）：原生粉格**不是平涂**，是同一张 backgrnd 按格逐像素调色，
网点/渐变全保留（主色只占 ~47%）。旧 oracle 只有 blob/band 探针，比的是 bbox ——
平涂一块 (255,187,187) 能给出**完全相同的框**，所以平涂实现能蒙过门禁。
新加的 `tint` 探针比的是逐像素等式（rate=与 tint(美术) 相等的占比），这条补上后
门禁必须当场把四种「看起来像粉色」的写法全判死，否则 B3-5 不算完成。

四种合成都是拿**已入库的原生 oracle 帧**改出来的（不依赖客户端、可离线复跑）：
  flat   —— 整格铺一个纯色（两种取值：精确目标色 / 该格众数色=对作弊最有利的那档）
  none   —— 不改色，格子还原成冷灰美术原样
  phase3 —— 改色取样的美术低 3px，即调色原点没加 README §8 那个 (0,3) 补偿
  over3  —— 改色对但矩形高了 3px，把格子上沿的白描边一起染了
判据：check 对每种合成都要当场报 FAIL。**分工要如实说**：flat/none/phase3 由新加的
`tint` 尺抓（逐像素等式），over3 由原有的白框/网格 `blob`/`band` 尺抓 —— 所以「平涂能蒙过」
这个洞是 tint 尺补上的，不是 bbox 尺本来就能抓。

用法：python flatfill_control.py        # 打印逐档读数，同时写 flatfill_control.txt
"""
import collections
import os
import subprocess
import sys

import numpy as np
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
# r27-b3-equip -> evidence -> ui-v83-window-restore -> features -> docs -> 仓库根
ROOT = os.path.abspath(os.path.join(HERE, "../../../../../"))
LAYOUT = os.path.join(ROOT, "docs/features/ui-v83-window-restore/tools/layout.py")
SPEC = "docs/features/ui-v83-window-restore/evidence/r27-oracle/Equip.json"
FRAME = "docs/features/ui-v83-window-restore/evidence/r27-oracle/frames/native_equip.png"
ART = "docs/features/ui-v83-window-restore/evidence/r27-oracle/tmpl/Equip_backgrnd.png"
OUT = os.path.join(HERE, "flatfill_control.txt")
TMP = os.path.join(HERE, "_flatfill_%s.png")

# 调色矩形 = 槽位 + (0,3)、32x32（README §6.4b），probe 区与此一致
CELLS = [(5, 233), (38, 233)]


def tint_pixel(c):
    """与 C++ Graphics/MemoryTexture.cpp 同一份调色核（README §6.3/§8 的穷举门禁验过）。"""
    r, g, b = (int(v) for v in c)
    return (17 * ((3 * r + 306) // 68), 17 * ((3 * g + 33) // 68), 17 * ((3 * b + 33) // 68))


def decode(raw):
    return raw.decode(os.environ.get("PYTHONIOENCODING", "utf-8"), "replace")


def main():
    src = os.path.join(ROOT, FRAME)
    arr = np.asarray(Image.open(src).convert("RGB")).astype(np.uint8)
    art = np.asarray(Image.open(os.path.join(ROOT, ART)).convert("RGB")).astype(np.uint8)

    def modal(x, y):
        box = collections.Counter(map(tuple, arr[y:y + 32, x:x + 32].reshape(-1, 3)))
        return box.most_common(1)[0][0]

    variants = {}

    # flat：整格铺纯色。铺该格众数色是对作弊者最有利的一档（bbox 与主色占比都和原生一致）
    for tag, color in (("flat_target", (255, 187, 187)),
                       ("flat_modal", modal(*CELLS[0]))):
        a = arr.copy()
        for x, y in CELLS:
            a[y:y + 32, x:x + 32] = color
        variants[tag] = a

    # none：不改色 —— 把冷灰美术原样贴回去
    a = arr.copy()
    for x, y in CELLS:
        a[y:y + 32, x:x + 32] = art[y:y + 32, x:x + 32]
    variants["none"] = a

    # phase3：改色矩形取样的美术比正确的低 3px（= 调色原点直接拿 iconpositions、
    # 没加 README §8 那个 (0,3) 补偿的真实形状）
    a = arr.copy()
    for x, y in CELLS:
        box = art[y - 3:y + 29, x:x + 32]
        tinted = np.array([tint_pixel(c) for c in box.reshape(-1, 3)], dtype=np.uint8)
        a[y:y + 32, x:x + 32] = tinted.reshape(box.shape)
    variants["phase3"] = a

    # over3：改色本身对，但矩形高了 3px（把格子上沿的白描边也染了）
    a = arr.copy()
    for x, y in CELLS:
        box = art[y - 3:y + 29, x:x + 32]
        tinted = np.array([tint_pixel(c) for c in box.reshape(-1, 3)], dtype=np.uint8)
        a[y - 3:y + 29, x:x + 32] = tinted.reshape(box.shape)
    variants["over3"] = a

    lines = ["负对照：合成帧过 layout.py check（期望：每档都有探针当场 FAIL）",
             "帧源 %s  众数色=%s" % (FRAME, tuple(int(v) for v in modal(*CELLS[0]))), ""]
    rc = 0
    caught_by = {}
    env = dict(os.environ, PYTHONIOENCODING="utf-8")  # 子进程默认 cp936，按 utf-8 解会变乱码
    for tag, a in variants.items():
        path = TMP % tag
        Image.fromarray(a.astype(np.uint8)).save(path)
        p = subprocess.run([sys.executable, LAYOUT, "check", SPEC, path],
                           capture_output=True, cwd=ROOT, env=env)
        os.remove(path)
        rows = decode(p.stdout).splitlines()
        rows = [r.strip() for r in rows if r.startswith("  ")]
        fails = [r for r in rows if "FAIL" in r]
        tint_fail = any("_tint" in r for r in fails)
        caught_by[tag] = "tint" if tint_fail else ("bbox" if fails else "NONE")
        bad = not fails
        rc |= 1 if bad else 0
        lines.append("%-11s check 退出码=%d 抓到=%s 由哪把尺抓到=%s"
                     % (tag, p.returncode, "是" if not bad else "否!", caught_by[tag]))
        lines.extend("    " + r for r in (fails if fails else rows))
    lines.append("")
    lines.append("分工：平涂/漏涂由新加的 tint 尺抓到（逐像素等式），多染 3 行由原有的白框/网格尺抓到。")
    lines.append("结论：%s" % ("四种写法全部被门禁判死，B3-5 成立" if rc == 0
                              else "有档漏检，门禁仍分不出平涂"))
    text = "\n".join(lines) + "\n"
    with open(OUT, "w", encoding="utf-8") as fh:
        fh.write(text)
    sys.stdout.buffer.write(text.encode("gbk", "replace"))
    return rc


if __name__ == "__main__":
    sys.exit(main())
