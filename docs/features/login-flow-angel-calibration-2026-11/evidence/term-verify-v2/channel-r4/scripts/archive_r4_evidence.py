#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""把 R4 四点修复的脚本/判据/帧/对照图归档到仓库证据目录（纯拷贝，不改判词）。

用法：python archive_r4_evidence.py
输出：E:\\project\\MapleStory-Client\\docs\\features\\login-flow-angel-calibration-2026-11\\evidence\\term-verify-v2\\channel-r4\\
"""
import os
import shutil

ROOT = r"C:\Users\11036\AppData\Local\Temp\term_verify"
DST = r"E:\project\MapleStory-Client\docs\features\login-flow-angel-calibration-2026-11\evidence\term-verify-v2\channel-r4"
SCRIPT_NAMES = [
    "extract_wz_r4.py", "measure_r4.py", "measure_r4_scroll_full.py",
    "measure_r5_charinfo.py", "patch_r4_channel_charinfo.py",
    "build_r4_isolated.bat", "verify_channel20.py", "verify_r4_channel.py",
    "auto_login_guard.py", "run_r4_guarded.py", "r4_after_audit.py",
    "archive_r4_evidence.py",
]
VERDICT_NAMES = [
    "measure_r4.json", "measure_r4_scroll_full.json",
    "channel_r4_verdict.json", "r4_after_audit.json", "auto_login_guard.json",
    "r4_after_audit_before_criterion_fix.json",
]
SHOT_NAMES = [
    "r4_home_f00.png", "r4_home_f05.png", "r4_page2_f05.png", "r4_back_f03.png",
    "ch20_home_f05.png",
]
# 对照图/蓝军翻案图在 ui_r4/ 而不在 shots/；measure_r5_charinfo.json 在 ui_r5/。
CMP_NAMES = [
    "cmp_full_r4.png", "cmp_bottom_r4.png", "cmp_notice_r4.png",
    "cmp_world_row_r4.png", "cmp_r4_input_check.png", "r4_after_center_zoom.png",
    "cmp_r4_bottom_before_after.png", "cmp_r4_notice_before_after.png",
]
WZ_NAMES = [
    "_sheet.png", "WorldSelect_scroll_0_0.png", "WorldSelect_scroll_0_3.png",
    "WorldSelect_scroll_1_0.png", "WorldSelect_BtGoworld_normal_0.png",
    "WorldNotice_BalloonForLoginProcess_Title.png",
    "WorldNotice_BalloonForLoginProcess_nw.png",
    "WorldNotice_BalloonForLoginProcess_se.png",
    "WorldSelect_channel_0_normal.png", "NoticeScroll1_0.png", "NoticeScroll2_0.png",
]

groups = {
    "scripts": (os.path.join(ROOT, "scripts"), SCRIPT_NAMES),
    "verdicts": (os.path.join(ROOT, "ui_r4"), VERDICT_NAMES),
    "shots": (os.path.join(ROOT, "shots"), SHOT_NAMES),
    "compare": (os.path.join(ROOT, "ui_r4"), CMP_NAMES),
    "wz": (os.path.join(ROOT, "ui_r4", "wz"), WZ_NAMES),
}
copied = []
missing = []
for sub, (src_dir, names) in groups.items():
    d = os.path.join(DST, sub)
    os.makedirs(d, exist_ok=True)
    for n in names:
        s = os.path.join(src_dir, n)
        if os.path.isfile(s):
            shutil.copy2(s, os.path.join(d, n))
            copied.append(sub + "/" + n)
        else:
            missing.append(sub + "/" + n)

# 跑错屏证据链整目录留档（auto_login_trap/ 是蓝军翻案的关键物证）
trap_src = os.path.join(ROOT, "ui_r4", "auto_login_trap")
trap_dst = os.path.join(DST, "auto_login_trap")
if os.path.isdir(trap_src):
    os.makedirs(trap_dst, exist_ok=True)
    for n in os.listdir(trap_src):
        s = os.path.join(trap_src, n)
        if os.path.isfile(s):
            shutil.copy2(s, os.path.join(trap_dst, n))
            copied.append("auto_login_trap/" + n)

print("COPIED", len(copied))
for c in copied:
    print("  ", c)
print("MISSING", len(missing))
for m in missing:
    print("  ", m)
