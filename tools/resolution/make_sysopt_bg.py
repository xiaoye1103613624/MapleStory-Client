#!/usr/bin/env python3
"""多分辨率 · 系统设置窗底板（整帧复制，可复跑）

背景
----
用户指定 `reference_分辨率.png`（299x396）作为系统设置窗（UISysOpt）的新 UI 图。
它是**完整窗口截帧**（含运行时叠加的滑条 / 勾选 / 下拉 / 按钮，以及底部「窗口大小」行）。

经核对：v83 原生就是「整帧底板 + 代码交互控件覆盖」模式（原版
`UIWindow.img/SysOpt/backgrnd` 本就含控件烘焙，UISysOpt 代码再画交互态覆盖之），
因此直接整帧当 backgrnd 不会引入双绘；此前为「避免双绘」做的剥离版反而把控件坐标
对齐搞乱（滑条压字、下拉溢出）。本次决策改为**整帧复制**，控件坐标由
`IO/UITypes/UIOptionMenu.h` 的常量表对照整图逐像素校准。

产物
----
  UI/SysOpt/sysopt_backgrnd.png                          （运行时 LocalPngTexture 首选路径）
  docs/features/multi-resolution/ui/uisysopt/v1/sysopt_backgrnd.png （归档副本）
  docs/features/multi-resolution/patches/make_sysopt_bg.log         （复制记录）
"""
import os
import shutil
from PIL import Image

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
DOC_DIR = os.path.join(ROOT, "docs/features/multi-resolution/ui/uisysopt/v1")
REF = os.path.join(DOC_DIR, "reference_分辨率.png")   # 仓库内用户原图归档（整帧）
UI_OUT = os.path.join(ROOT, "UI/SysOpt/sysopt_backgrnd.png")
DOC_OUT = os.path.join(DOC_DIR, "sysopt_backgrnd.png")
PATCH_DIR = os.path.join(ROOT, "docs/features/multi-resolution/patches")
LOG = os.path.join(PATCH_DIR, "make_sysopt_bg.log")


def main():
    ref = Image.open(REF).convert("RGBA")
    if ref.size != (299, 396):
        raise RuntimeError("用户指定新 UI 图尺寸异常: %s" % (ref.size,))

    os.makedirs(os.path.dirname(UI_OUT), exist_ok=True)
    os.makedirs(DOC_DIR, exist_ok=True)
    os.makedirs(PATCH_DIR, exist_ok=True)

    # 整帧复制：底板即用户指定图，所有静态排版（标签、边框、配色）由图提供，
    # 代码只在其上叠加可交互控件（滑条/勾选/下拉/OK·Cancel）的实时态。
    ref.save(UI_OUT)
    ref.save(DOC_OUT)

    with open(LOG, "w", encoding="utf-8", newline="\n") as fh:
        fh.write("# make_sysopt_bg.py 整帧复制日志\n")
        fh.write("ref=%s size=%s\n" % (REF, ref.size))
        fh.write("out=%s size=%s\n" % (UI_OUT, ref.size))
        fh.write("mode=full-frame-copy (no control stripping; v83 native overlay)\n")

    print("REF", ref.size, "->", UI_OUT, DOC_OUT)
    print("wrote", UI_OUT, DOC_OUT, LOG)


if __name__ == "__main__":
    main()
