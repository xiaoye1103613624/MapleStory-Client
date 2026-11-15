#!/usr/bin/env python3
"""本轮（m07417 收口）唯一验收口径整合：不新建像素判据、不放宽阈值。

输入全部是已落盘证据：
  ui_r3/channel20_verdict.json        真机翻页（新二进制 fc690d3b）
  ui_r3/channel20_final_audit.json    CH.17 遮挡收敛 + 次页标签覆盖
  ui_r3/verify_r3_noregress.json      R3 五组件复算（含坏帧误报）
  build_channel20.log / exe sha       BUILD_EXITCODE=0
诚实口径：
  - R3 频道页两组件（noticeTitle/noticeNw）用新帧 ch20_home_f05.png，pass=true，applicable。
  - R3 角色页三组件（charInfo/scroll/nameTagL）输入帧 charselect_clean_f09.png
    是登录页「密码错误！请确认！」弹窗（read_image 目视 + sha ef1f80ec…），
    历史 verify_r3_align.json 本就 all_pass=False、charInfo shift[54,43]。
    标 INVALID_INPUT / applicable=false，不计入本轮 pass 也不计退化；
    本轮 UICharSelect.cpp 对 HEAD 零改动（sha efca4007…==efca4007…），
    逻辑上无退化路径，但不用坏帧伪造通过。
  - pager_region 新落点 vs 目标 mean_abs=54.59 高于旧 45.85：扩展箭头
    原生帧没有，从「压 CH.17 负载条」改到「压空白木轴」，如实披露仍非原生。
    真收敛指标是 gauge_peer_gap 5.79 → -0.43 / ch17_occlusion_resolved=true。
"""
import hashlib
import json
import os
import re

TV = r"C:\Users\11036\AppData\Local\Temp\term_verify"
UI = os.path.join(TV, "ui_r3")
REPO = r"E:\project\MapleStory-Client"


def jload(p):
    return json.load(open(p, encoding="utf-8"))


def sha(p):
    return hashlib.sha256(open(p, "rb").read()).hexdigest()


def main():
    live = jload(os.path.join(UI, "channel20_verdict.json"))
    audit = jload(os.path.join(UI, "channel20_final_audit.json"))
    nore = jload(os.path.join(UI, "verify_r3_noregress.json"))

    build_log = os.path.join(TV, "build_channel20.log")
    log = open(build_log, "r", encoding="utf-8", errors="replace").read()
    errors = re.findall(r"error C\d+|error LNK\d+|fatal error|error MSB\d+", log)
    exe = os.path.join(TV, "build_channel20", "MapleStory.exe")

    char_src = os.path.join(REPO, "IO", "UITypes", "UICharSelect.cpp")
    frames_home = live.get("frames_home", [])
    frames_p2 = live.get("frames_page2", [])
    frames_back = live.get("frames_back", [])

    applicable_r3 = {}
    excluded_r3 = {}
    for k, v in nore["components"].items():
        if k in ("noticeTitle", "noticeNw"):
            applicable_r3[k] = {"ncc": v["ncc"], "shift": v["shift"], "pass": v["pass"]}
        else:
            excluded_r3[k] = {
                "applicable": False,
                "reason": "input frame charselect_clean_f09.png is login WRONG_PASSWORD dialog, "
                          "not char-select page; UICharSelect.cpp unchanged vs HEAD this round",
                "raw_shift": v["shift"], "raw_pass": v["pass"],
            }

    checks = {
        "build_exit0": len(errors) == 0,
        "exe_sha_locked": sha(exe) == live.get("deployed_sha256"),
        "live_exe_untouched": live.get("live_exe_untouched_sha256", "").startswith("295932c5"),
        "config_bom_intact": live.get("config_head_before") == "b'# M'" == live.get("config_head_after"),
        "serverlist_anchor": bool(live.get("anchor_hit")),
        "home_frames_real": len(frames_home) >= 5 and all(f.get("real_gl_frame") and 148 < f["mean"] < 155 for f in frames_home),
        "page2_frames_real": len(frames_p2) >= 5 and all(f.get("real_gl_frame") and 148 < f["mean"] < 155 for f in frames_p2),
        "back_frames_real": len(frames_back) >= 3 and all(f.get("real_gl_frame") and 148 < f["mean"] < 155 for f in frames_back),
        "ch17_occlusion_resolved": bool(audit.get("ch17_occlusion_resolved")),
        "label_override_active": bool(audit.get("label_override_active")),
        "r3_channel_not_regress": all(v["pass"] for v in applicable_r3.values()),
    }

    res = {
        "verdict": "CHANNEL20_PAGER_FIX_VERIFIED" if all(checks.values())
                   else "CHANNEL20_PAGER_FIX_INCOMPLETE",
        "scope": "channel page 20/page + 30-channel pager + pager relocation + R3 channel components only",
        "build": {"exit0": checks["build_exit0"], "errors": errors[:5],
                  "exe_sha256": sha(exe), "exe_size": os.path.getsize(exe)},
        "deployed_sha256": live.get("deployed_sha256"),
        "live_exe_untouched_sha256": live.get("live_exe_untouched_sha256"),
        "real_device": {
            "anchor_hit": live.get("anchor_hit"),
            "home_mean_range": [frames_home[0]["mean"], frames_home[-1]["mean"]] if frames_home else None,
            "page2_mean_range": [frames_p2[0]["mean"], frames_p2[-1]["mean"]] if frames_p2 else None,
            "back_mean_range": [frames_back[0]["mean"], frames_back[-1]["mean"]] if frames_back else None,
            "pager_clicks": [live.get("pager_prev_click"), live.get("pager_next_click")],
        },
        "occlusion": {
            "gauge_peer_gap_before": audit["gauge_peer_gap_before"],
            "gauge_peer_gap_after": audit["gauge_peer_gap_after"],
            "ch17_occlusion_resolved": audit["ch17_occlusion_resolved"],
            "old_pager_inside_CH17": audit["pager"]["old_inside_CH17"],
            "new_pager_inside_CH17": audit["pager"]["new_inside_CH17"],
        },
        "label_override": audit["label_override"],
        "r3_channel_components_applicable": applicable_r3,
        "r3_char_components_excluded": excluded_r3,
        "char_page_source": {"file": "IO/UITypes/UICharSelect.cpp",
                             "worktree_sha256": sha(char_src),
                             "note": "not modified this round; no pixel re-verify (input frame invalid)"},
        "disclosed_residuals": [
            "翻页左右箭头是本服 30/60 频道扩展，原生 20 格目标帧没有；新落点仍在木轴上与目标 mean_abs=54.59，非原生、不签像素一致。",
            "角色页 R3 三组件本轮无可比输入帧（历史 charselect_clean_f09.png 是登录弹窗），不做像素复算；源码零改动，无退化路径但未复验。",
            "CELL_H=32 / 18px 行距 / 纯净校准二进制 / 进图场景态不可比 / 职业团 EVAN+DUALBLADE 按用户指示不删，均沿用历史不签项。",
        ],
        "checks": checks,
        "all_checks_pass": all(checks.values()),
    }
    jp = os.path.join(UI, "channel20_final_verdict.json")
    json.dump(res, open(jp, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
    print(json.dumps({"verdict": res["verdict"], "all_checks_pass": res["all_checks_pass"],
                      "checks": checks, "occlusion": res["occlusion"],
                      "r3_channel": applicable_r3}, ensure_ascii=False, indent=2))
    print("JSON", jp)


if __name__ == "__main__":
    main()
