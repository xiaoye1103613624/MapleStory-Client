#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""生成 R3 自洽验收记录并归档证据（不重编、不抓帧、不猜结论）。

口径纪律（PUA harness 治理）：报告「候选完成 + 证据链 + 剩余风险」，
不把 14:38 的终端 ALL_PASS 包装成「纯净二进制最终裁决」。
所有数值都标注来源与限制；被覆盖的原件、环境噪声帧、未修残差全部写明。
"""
import hashlib
import json
import os
import shutil
import time

TEMP = r"C:\Users\11036\AppData\Local\Temp\term_verify"
UI_R3 = os.path.join(TEMP, "ui_r3")
SCRIPTS = os.path.join(TEMP, "scripts")
SHOTS = os.path.join(TEMP, "shots")
TARGETS = os.path.join(TEMP, "targets_desktop")
REPO = r"E:\project\MapleStory-Client"
EVID = os.path.join(REPO, "docs", "features",
                    "login-flow-angel-calibration-2026-11", "evidence",
                    "term-verify-v2", "r3-align")
os.makedirs(EVID, exist_ok=True)
for sub in ("templates", "scripts", "verdicts"):
    os.makedirs(os.path.join(EVID, sub), exist_ok=True)


def sha(p):
    return hashlib.sha256(open(p, "rb").read()).hexdigest() if os.path.exists(p) else None


def cp(src, dst_dir, rename=None):
    if not os.path.exists(src):
        return None
    name = rename or os.path.basename(src)
    dst = os.path.join(dst_dir, name)
    shutil.copy2(src, dst)
    return {"file": name, "sha256": sha(dst), "bytes": os.path.getsize(dst)}


rec = {
    "record": "R3 频道页/角色列表页逐组件对齐验收记录",
    "generated": time.strftime("%Y-%m-%d %H:%M:%S"),
    "verdict": "R3_ALIGNED_WITH_DISCLOSED_RESIDUALS",
    "verdict_note": (
        "静态几何 + 14:38 同代码真机五组件 ALL_PASS 双证；但不签『纯净校准二进制复跑通过』。"
        "理由：live 目录与增量 obj 被别会话并发反复替换，14:33 干净帧 PNG / verdict JSON 原件"
        "已被 15:04-15:22 三次重抓覆盖，仅终端逐项数值在案；15:03 明确隔离编译的 ba324061 "
        "三次抓帧均停在登录页（环境噪声），未产出可用 UI 帧。按 harness 治理口径报告"
        "『候选完成 + 证据链 + 剩余风险』，不把局部通过包装成最终裁决。"
    ),

    "source_truth": {
        "angel": [
            "CUICharDetail_ctor_0x60292f.txt:20-23  CFadeWnd::CreateFadeWnd(a2:183, a3:a1[57]!=0?151:115)",
            "CUICharDetail_Draw_0x602adc.txt  DrawTextA 职业(46,1)/等级(46,19)/人气(136,19)/力量(46,37)/智力(136,37)/敏捷(46,55)/运气(136,55)/排行(36,99)/(115-w,99)",
            "CUIChannelSelect_ctor_0x62b2cf.json  CDialog::CreateDlg(a4:0x21C=540, a5:167, a6:211)",
        ],
        "orange_wz": (
            "E:\\MXD\\BeiDou-Client-xy\\Data\\UI\\Login.img, 128B GMS128 key "
            "(tools/_gms_key_128.b64, head EwAAAFIAAAAqAAAAWwAAAAgA), iv TSPHKw==, "
            "exclusive:false, 只读工具 load_files/get_node_tree_json/get_node_detail"
        ),
        "wz_nodes": {
            "CharSelect/scroll/2": "217x30 d250 -> 217x196 d50 -> 217x195 d50 -> 217x193 (末帧常驻), origin(0,0)",
            "CharSelect/charInfo": "183x115 origin(45,57)",
            "CharSelect/charInfo1": "183x151 origin(45,57)，底部多一行居中「排行榜」，本端 Range(0,36) 裁掉",
            "CharSelect/nameTag/1/0": "8x22",
            "WorldSelect/alert/backgrd": "540x167 (Angel 0x21C/167 对上)",
            "WorldSelect/channel/N/normal": "92x30",
            "WorldSelect/chBackgrn": "449x233 origin(224,116)",
            "WorldSelect/chSelect": "23x11 d100 -> 38x21 d50 -> 42x33 d50 -> 59x33 (末帧)",
            "WorldSelect/chgauge": "73x9 origin(36,4)",
            "WorldSelect/scroll/0": "513x152 d250 -> 513x416 d50 -> 513x408 d50 -> 513x401",
            "WorldNotice/BalloonForLoginProcess": (
                "nw/ne 22x40, sw/se 22x24, n/s 12x40/24, w/e 19/22x9, c 12x9, Title 170x18"
            ),
        },
    },

    "measured_shifts": {
        "method": (
            "ui_r3/measure_r3_final.py 用 WZ 真 PNG 模板做 alpha-mask 滑窗 NCC；"
            "常态帧口径：shots/channel_grid_f05.png mean=152.12 / world_f03.png mean=144.67。"
            "v2_channel_grid.png mean=59.2 是暗过场帧，据此得出的『公告没画』是假结论，已作废。"
        ),
        "target_bbox": {
            "charInfo": [392, 220, 575, 335], "scroll": [372, 161, 589, 354],
            "nameTagL": [469, 430, 477, 452], "noticeTitle": [293, 313, 463, 331],
            "noticeNw": [280, 296, 302, 336],
        },
        "before_shift_ours_minus_target": {
            "charInfo": [-1, -23, -1, -23], "charInfo1": [-1, -23, -1, -23],
            "scroll": [2, -3, 2, -3], "nameTagL": [5, 8, 5, 8],
            "noticeTitle": [-34, -12, -34, -12], "noticeNw": [-28, -1],
        },
        "user_complaints_confirmed": [
            "m06637「羊皮纸靠下/属性板偏」-> charInfo 上移 23px 坐实",
            "m06637「名字名片靠下」-> nameTagL 下移 8px 坐实",
            "m06637「频道页公告 ui」-> noticeTitle 左移 34 / 上移 12 坐实",
        ],
        "linkage": (
            "目标内容板顶 220 - 卷轴顶 161 = 59；本端 197-158=39，差 20 正是 "
            "CS_SCROLL_CONTENT_OFF.y=39 与真值 59 的缺口。故必须联动改两个常量："
            "CHARINFO +23 且 SCROLL_CONTENT +20 => 内容板 +23、卷轴仅 +3。"
        ),
    },

    "fixes": {
        "IO/UITypes/UICharSelect.cpp": [
            "CS_CHARINFO_OFF(-94,-231) -> (-94,-208)  (+23，内容板/值行下移)",
            "CS_SCROLL_CONTENT_OFF(17,39) -> (17,59)  (+20，使卷轴仅下移 3)",
            "nametags[index].draw(charpos + Point<int16_t>(2,25)) -> (2,17)  (-8，名牌上移)",
        ],
        "IO/UITypes/UIWorldSelect.cpp": [
            "WS_NOTICE_GAP 0 -> -36  (公告右移；目标 nw_left=280 = 436-192-(-36))",
        ],
        "IO/Components/ChatBalloon.cpp": [
            "box_w_ref 184 -> 192  (真角件 span=box_w-22 定案，见 real_corners)",
            "title.draw(top+6) -> (top+18)  (12 帧稳态 title 恒 301 vs 目标 313)",
            "title_x = left+(box_w-170)/2 -> left+13  (原生标题非严格居中，与框宽解耦)",
            "box_h_ref 保持 74  (正文已偏低 3.39px，增大 box_h 会经居中公式再下推)",
        ],
    },

    "real_corner_evidence": {
        "script": "scripts/settle_frame_real_corners.py",
        "templates": (
            "orange-wz 重连后 get_node_detail(nodes=[ne,se,e,n]) 取得真 PNG；"
            "spill 内原本只有 Title/nw，早前 settle_ne_mirror.py 用『镜像 nw』近似 ne(ncc0.89) 不可靠，已由真模板取代"
        ),
        "first_measurement_184_era": {
            "TARGET": {"nw": [280, 296], "ne": [450, 296], "se": [450, 352], "span": 170},
            "OURS_box184": {"nw": [285, 295], "ne": [447, 295], "se": [447, 344], "span": 162},
            "span_diff": 8,
        },
        "reasoning": (
            "span = ne_left - nw_left = box_w - 22 (OURS 184-22=162 精确自洽)，"
            "TARGET span=170 => box_w=192。196 由两个左锚点 (box_w-170)/2=13 反推，"
            "属过拟合：真 ne 给出框右缘 472，196 会推到 476 过冲 4px。"
        ),
        "final_frame_14_38": {
            "TARGET": {"nw": [280, 296], "ne": [450, 296], "se": [450, 352], "span": 170},
            "OURS": {"nw": [280, 295], "ne": [454, 295], "se": [454, 344], "span": 174},
            "ne_residual": 4,
            "ncc_curve": "干净单峰：TARGET peak x450 ncc0.9387；OURS peak x454 ncc0.9364、x450 仅 0.4662 => +4 是真实残差非误配",
        },
    },

    "real_device_acceptance": {
        "status": "PARTIAL_TRANSCRIPT_ONLY",
        "terminal_output_14_38": {
            "source": "verify_r3_align.py 终端逐行输出（JSON 原件后被覆盖，逐项数值在对话记录中可核对）",
            "binary_nominal": "768c6e22d3b543fb468f2f0e3cb37ca192e7d4a2451f3434b112478af3c15c39 / 2026-11-15 14:32:41 / 5137408 B (隔离编译 BUILD_EXITCODE=0)",
            "frames": "channel: shots/channel_grid_f05.png (14:33:49, mean151.2 干净频道帧); char: shots/charselect_clean_f09.png (13:32:16)",
            "components": {
                "charInfo": {"bbox": [391, 220, 574, 335], "shift": [-1, 0, -1, 0], "pass": True},
                "scroll": {"bbox": [374, 161, 591, 354], "shift": [2, 0, 2, 0], "pass": True},
                "nameTagL": {"bbox": [473, 430, 481, 452], "shift": [4, 0, 4, 0], "pass": True,
                             "note": "收紧 ROI (465,418,492,462) 后 [4,0]；宽 ROI 曾误配到另一槽 [459,448]，是测量 bug 非代码 bug。x=4 系名字长度导致 CENTER 左片漂移"},
                "noticeTitle": {"bbox": [293, 313, 463, 331], "shift": [0, 0, 0, 0], "pass": True},
                "noticeNw": {"bbox": [280, 295, 302, 335], "shift": [0, -1, 0, -1], "pass": True},
            },
            "ALL_PASS": True, "ALL_STEADY": True,
        },
        "overwritten_originals": (
            "14:33 干净 channel f00-f11 / 13:32 charselect f00-f09 / 14:38 verify_r3_align.json "
            "已被 15:04、15:13、15:22 三次重抓覆盖；本记录不声称这些 PNG/JSON 原件仍存在。"
        ),
        "later_retries_excluded": {
            "attempts": ["15:04 final_capture_eval", "15:13 单跑 channel", "15:22 final_chain_v2"],
            "symptoms": [
                "anchor_hit=False / charlist_hit=False / frames mean≈136.7 std≈70 (登录页底色)",
                "noticeNw shift=+191、charInfo +54,+43 是模板在登录页误配特征，非坐标漂移",
                "packet_dump 15:13 段却有 [SEND] 01 00 ...admin / [RECV] 0a 00 00 ...Scania / [RECV] 0a 00 ff => 协议登录成功，脚本 wait_anchor 从共享 dump 起点读、被并发写入污染",
            ],
            "environment_noise": [
                "别会话 13:53/14:21/14:37 反复改 ItemTooltip.cpp / UIShop.cpp（C2248 半成品）并跑并发构建",
                "14:54 LNK1104 无法打开 x64\\obj-Release\\UIWorldMap.obj 时 MSBuild.exe/cl.exe 正被别会话占用",
                "live exe 被反复替换：ed03d4ef -> dc6e7be4 -> 5390ef8d -> 768c6e22 -> cb1d9751 -> f707f37c -> ba324061",
                "14:34 与 14:47 两次 live MapleStory.exe 瞬时消失导致 no window",
            ],
            "judgement": "环境噪声，不作退化证据；也不伪称这三次复跑通过",
        },
        "isolated_build_15_03": {
            "binary": "ba3240618741ae9e28d7aa363d0dad4147966b25af1ca4c449aca72797ed72a4 / 15:03:36 / 5120512 B",
            "build": "final_atomic_verify.py 隔离编译 BUILD_EXITCODE=0，别会话两文件 sha 全等恢复 (RESTORE_ALL_OK True)",
            "exe_unchanged_during_capture": True,
            "outcome": "抓帧停在登录页，未产出可用 UI 帧；仅证明『当前 192/-36/+13 源码可确定性构建』",
        },
    },

    "disclosed_residuals": [
        "公告框右缘 ne +4px（OURS x454 vs TARGET x450）：真角件干净单峰，非误配；192 由 span 定案，未再调",
        "公告框底 se +8px（OURS y344 vs TARGET y352）：box_h 增大经 text_y 居中公式会把已偏低 3.39px 的正文再下推，正文像素判据噪声大，本轮不动垂直几何",
        "公告正文中心 +3.39px（OURS 345.22 vs TARGET 341.83）：同上，未修",
        "频道页 20 格 vs 服务端仅发 3 频道（数据态差异，非坐标漂移）",
        "CELL_H=32 无本端渲染实证、18px 行距像素侧未收敛、进图帧场景态不可比、职业团 EVAN/DUALBLADE 两卡按用户指示保留 —— 沿用 §11.7 不签项",
    ],

    "code_constants_verified": {
        "IO/Components/ChatBalloon.cpp": {"box_w_ref": 192, "box_h_ref": 74,
                                          "title_x": "left + 13", "title_y": "top + 18"},
        "IO/UITypes/UIWorldSelect.cpp": {"WS_NOTICE_GAP": -36},
        "IO/UITypes/UICharSelect.cpp": {"CS_CHARINFO_OFF": [-94, -208],
                                       "CS_SCROLL_CONTENT_OFF": [17, 59],
                                       "nametags_offset": [2, 17]},
        "bom": "UICharSelect.cpp=EF BB BF；UIWorldSelect.cpp / ChatBalloon.cpp 无 BOM（既有状态，多次编译通过）",
    },
}

# --- 写记录 ---
rec_path = os.path.join(EVID, "verdicts", "r3_acceptance_record.json")
with open(rec_path, "w", encoding="utf-8") as f:
    json.dump(rec, f, indent=2, ensure_ascii=False)
print("WROTE", rec_path)

# --- 归档真角件模板 + 关键测量 JSON + 脚本 ---
copied = []
for fn in sorted(os.listdir(UI_R3)):
    if fn.startswith("tpl2_") and fn.endswith(".png"):
        r = cp(os.path.join(UI_R3, fn), os.path.join(EVID, "templates"))
        if r:
            copied.append(r)

for fn in ["notice_slide_curve.json", "notice_geometry.json", "notice_body.json",
           "measure_r3_final.json", "compute_shifts.json", "body_final_settle.json",
           "nametag_settle_decisive.json", "real_corners_settle.json",
           "final_chain_v2.json", "final_atomic_verify.json"]:
    r = cp(os.path.join(UI_R3, fn), os.path.join(EVID, "verdicts"))
    if r:
        copied.append(r)

for fn in ["measure_r3_final.py", "measure_notice_slide.py", "measure_notice_geometry.py",
           "measure_notice_body.py", "measure_body_final.py", "settle_frame_real_corners.py",
           "settle_nametag_decisive.py", "verify_r3_align.py", "patch_r3_align.py",
           "build_quarantine.py", "final_chain_v2.py", "final_capture_eval.py",
           "build_r3_acceptance.py"]:
    r = cp(os.path.join(SCRIPTS, fn), os.path.join(EVID, "scripts"))
    if r:
        copied.append(r)

# 目标帧引用（不复制大文件，只记 sha + 路径）
targets = {}
for fn in ["desktop_channel_expanded.png", "desktop_channel_collapsed.png",
           "desktop_char_select.png"]:
    p = os.path.join(TARGETS, fn)
    if os.path.exists(p):
        targets[fn] = {"sha256": sha(p), "bytes": os.path.getsize(p),
                       "source_path": p}
with open(os.path.join(EVID, "verdicts", "target_frames_manifest.json"),
          "w", encoding="utf-8") as f:
    json.dump(targets, f, indent=2, ensure_ascii=False)

print("COPIED", len(copied), "files; targets", len(targets))
for c in copied[:6]:
    print("  ", c["file"], c["bytes"])
print("EVID", EVID)
