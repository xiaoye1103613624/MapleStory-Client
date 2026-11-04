#!/usr/bin/env python3
"""多分辨率 · 生成 UISysOpt 299x396 纯底板（可复跑校验）

背景
----
用户指定 `E:/MXD/扩展改动/083多分辨率/分辨率.png`（299x396）作为系统设置窗的新 UI 图，
它比运行时 `UIWindow.img/SysOpt/backgrnd`（299x365）多 31px，即新增「窗口大小」行。

该 PNG 是**完整窗口截帧**（含运行时叠加的滑条 / 勾选 / 下拉 / 按钮，以及底部
y≈370..390 的蓝色下拉选中态），整张直接当 backgrnd 会与 UISysOpt 代码里叠加的控件
双绘重影（项目红线 R5）。因此：
  * 前 365 行一律取运行时纯底板 `UIWindow.img_SysOpt_backgrnd.png`（已导出 PNG）；
  * 新增 31 行取参考图 y=364..371 的浅灰背景条带平铺（该区域不含蓝色控件）；
  * 「窗口大小」行标签、分辨率下拉、确定/取消仍由代码按插件坐标绘制
    （Combo 76,338 166x18；OK/Cancel y=372，见 rs.cpp L1973/L3313）。

产物
----
  UI/SysOpt/sysopt_backgrnd.png                          （运行时 LocalPngTexture 首选路径）
  docs/features/multi-resolution/ui/uisysopt/v1/sysopt_backgrnd.png （归档副本）
  docs/features/multi-resolution/ui/uisysopt/v1/reference_分辨率.png（用户指定原图）
  docs/features/multi-resolution/patches/make_sysopt_bg.log         （像素差异统计）
"""
import os
import shutil

from PIL import Image

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
BASE = os.path.join(
    ROOT, "docs/features/ui-v83-window-restore/evidence/r15-newrefs/_nx_all",
    "UIWindow.img_SysOpt_backgrnd.png")
REF = "E:/MXD/扩展改动/083多分辨率/分辨率.png"

UI_OUT = os.path.join(ROOT, "UI/SysOpt/sysopt_backgrnd.png")
DOC_DIR = os.path.join(ROOT, "docs/features/multi-resolution/ui/uisysopt/v1")
DOC_OUT = os.path.join(DOC_DIR, "sysopt_backgrnd.png")
REF_COPY = os.path.join(DOC_DIR, "reference_分辨率.png")
PATCH_DIR = os.path.join(ROOT, "docs/features/multi-resolution/patches")
LOG = os.path.join(PATCH_DIR, "make_sysopt_bg.log")


def main():
    base = Image.open(BASE).convert("RGBA")
    ref = Image.open(REF).convert("RGB")

    if base.size != (299, 365):
        raise RuntimeError("运行时 SysOpt 底板尺寸异常: %s" % (base.size,))
    if ref.size != (299, 396):
        raise RuntimeError("用户指定新 UI 图尺寸异常: %s" % (ref.size,))

    w, h = base.size
    new_h = ref.height
    out = Image.new("RGBA", (w, new_h), (238, 238, 238, 255))
    out.paste(base, (0, 0))

    # y=364..371 是参考图里「新增行之前」的浅灰背景（不含蓝色下拉选中态），
    # 左列自带 1px 黑边框；平铺到新增的 31 行，保持与上方底板同色系。
    band = ref.crop((0, 364, w, 372)).convert("RGBA")
    for y in range(h, new_h):
        row = band.crop((0, (y - h) % band.height, w, (y - h) % band.height + 1))
        out.paste(row, (0, y))

    # 底边 1px 黑框，与参考图 y=395 一致。
    for x in range(w):
        out.putpixel((x, new_h - 1), (0, 0, 0, 255))

    os.makedirs(os.path.dirname(UI_OUT), exist_ok=True)
    os.makedirs(DOC_DIR, exist_ok=True)
    os.makedirs(PATCH_DIR, exist_ok=True)
    out.save(UI_OUT)
    out.save(DOC_OUT)
    shutil.copy2(REF, REF_COPY)

    # 差异统计：证明参考图前 365 行不是纯底板（含控件），故必须剥离。
    try:
        import numpy as np
        a = np.array(base.convert("RGB")).astype(int)
        r = np.array(ref).astype(int)
        diff_px = int((np.abs(a - r[:h]).sum(2) > 30).sum())
        diff_ratio = round(diff_px / float(w * h), 4)
        blue_rows = [(y, tuple(int(v) for v in np.median(r[y, 10:290], axis=0)))
                     for y in (372, 380, 388)]
    except ImportError:
        diff_px, diff_ratio, blue_rows = -1, -1, []

    with open(LOG, "w", encoding="utf-8", newline="\n") as fh:
        fh.write("# make_sysopt_bg.py 像素处理日志\n")
        fh.write("base=%s size=%s\n" % (BASE, base.size))
        fh.write("ref=%s size=%s\n" % (REF, ref.size))
        fh.write("out=%s size=%s\n" % (UI_OUT, out.size))
        fh.write("rows_reused_from_runtime_base=0..364 (299x365, no runtime controls)\n")
        fh.write("rows_appended=365..395 (31 px, light-gray band from ref y=364..371)\n")
        fh.write("ref_vs_base_diff_px(>30)=%d ratio=%s\n" % (diff_px, diff_ratio))
        fh.write("blue_selection_bar(med RGB)=%s -> NOT baked (runtime Combo state)\n" % blue_rows)
        fh.write("label/dropdown/OK-Cancel drawn by code (Combo 76,338 166x18; BTN_Y=372)\n")

    print("BASE", base.size, "REF", ref.size, "OUT", out.size)
    print("diff_px", diff_px, "ratio", diff_ratio)
    print("wrote", UI_OUT, DOC_OUT, REF_COPY, LOG)


if __name__ == "__main__":
    main()
