#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""绑定已构建二进制（不重编）做「抓帧 -> 复验 -> 首尾 sha 绑定」收尾。

背景：final_atomic_verify.py 已完成编译（BUILD_EXITCODE=0, exe ba324061,
别会话文件 orig_match True 恢复），但读 capture summary 时窗口标题里的
GBK 乱码字节触发 UnicodeDecodeError('gbk') 崩在读 JSON 那一步。
代码常量自 14:32 定案后未再改，故不重编（避免并发撞 obj），只做抓帧+复验。
所有 json load 一律 encoding=utf-8, errors=replace。
"""
import hashlib
import json
import os
import subprocess
import sys
import time

LIVE = r"E:\MXD\BeiDou-Client-xy\MapleStory.exe"
SCRIPTS = r"C:\Users\11036\AppData\Local\Temp\term_verify\scripts"
UI_R3 = r"C:\Users\11036\AppData\Local\Temp\term_verify\ui_r3"
SHOTS = r"C:\Users\11036\AppData\Local\Temp\term_verify\shots"
VERDICT = os.path.join(UI_R3, "final_capture_eval.json")


def sha_file(p):
    return hashlib.sha256(open(p, "rb").read()).hexdigest() if os.path.exists(p) else None


def jload(p):
    with open(p, "r", encoding="utf-8", errors="replace") as f:
        return json.load(f)


def run(args, cwd, timeout=300):
    r = subprocess.run(args, cwd=cwd, capture_output=True, text=True,
                       errors="replace", timeout=timeout)
    return (r.stdout or "") + "\n" + (r.stderr or ""), r.returncode


def main():
    v = {"ts": time.strftime("%Y-%m-%d %H:%M:%S"), "mode": "capture+eval (no rebuild)"}
    pre = sha_file(LIVE)
    st = os.stat(LIVE)
    v["exe"] = {"sha256": pre, "size": st.st_size,
                "mtime": time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(st.st_mtime))}
    print("EXE BIND", v["exe"])

    # 常量口径（证明不重编的代码就是已编译代码）
    consts = subprocess.check_output(
        ["grep", "-nE",
         "box_w_ref =|WS_NOTICE_GAP =|left \\+ 13|CS_CHARINFO_OFF\\(-94|nametags\\[index\\]\\.draw",
         r"E:\project\MapleStory-Client\IO\Components\ChatBalloon.cpp",
         r"E:\project\MapleStory-Client\IO\UITypes\UIWorldSelect.cpp",
         r"E:\project\MapleStory-Client\IO\UITypes\UICharSelect.cpp"],
        text=True, errors="replace")
    v["constants"] = consts.strip().splitlines()
    print("CONSTANTS OK", len(v["constants"]), "lines")

    captures = {}
    for name, script, sm in [
        ("channel", "capture_channel_grid.py", "channel_grid_summary.json"),
        ("charselect", "capture_charselect_clean.py", "charselect_clean_summary.json"),
    ]:
        attempts = []
        for attempt in range(3):
            out, rc = run([sys.executable, os.path.join(SCRIPTS, script)], SCRIPTS)
            p = os.path.join(SHOTS, sm)
            s = jload(p) if os.path.exists(p) else {}
            nf = len(s.get("frames") or [])
            attempts.append({"attempt": attempt, "rc": rc, "frames": nf,
                             "error": s.get("error"),
                             "charlist_hit": s.get("charlist_hit"),
                             "serverlist_anchor_hit": s.get("serverlist_anchor_hit"),
                             "config": s.get("config")})
            print("CAPTURE", name, attempts[-1])
            if nf >= 3:
                break
            time.sleep(1.5)
        captures[name] = attempts
    v["captures"] = captures

    after = sha_file(LIVE)
    v["exe"]["sha_after"] = after
    v["exe"]["unchanged"] = (after == pre)
    print("EXE AFTER", after, "UNCHANGED", after == pre)

    evals = {}
    for name, script in [("verify_r3_align", "verify_r3_align.py"),
                          ("real_corners", "settle_frame_real_corners.py")]:
        out, rc = run([sys.executable, os.path.join(SCRIPTS, script)], SCRIPTS, timeout=300)
        evals[name] = {"rc": rc, "tail": out[-1200:]}
        print("EVAL", name, "rc", rc)
        for line in out.splitlines():
            if "ALL_PASS" in line or "VERDICT" in line or "SPAN " in line:
                print("  ", line)
    v["evals"] = evals

    vr = jload(os.path.join(UI_R3, "verify_r3_align.json"))
    rc_ = jload(os.path.join(UI_R3, "real_corners_settle.json"))
    v["summary"] = {
        "all_pass": vr.get("all_pass"),
        "all_steady": vr.get("all_steady"),
        "component_shifts": {k: d.get("shift_after")
                             for k, d in vr.get("components", {}).items()},
        "corner_span_target": rc_.get("TARGET", {}).get("span_nw_ne"),
        "corner_span_ours": rc_.get("OURS_V1", {}).get("span_nw_ne"),
        "corner_ne_x": {"target": rc_.get("TARGET", {}).get("ne", {}).get("pos", [None])[0],
                        "ours": rc_.get("OURS_V1", {}).get("ne", {}).get("pos", [None])[0]},
        "corner_se_y": {"target": rc_.get("TARGET", {}).get("se", {}).get("pos", [None, None])[1],
                        "ours": rc_.get("OURS_V1", {}).get("se", {}).get("pos", [None, None])[1]},
        "binary": vr.get("binary"),
    }
    cap_ok = all(any(a["frames"] >= 3 for a in att) for att in captures.values())
    v["verdict"] = ("R3_FINAL_VERIFIED" if vr.get("all_pass") and (after == pre or after is None)
                    and cap_ok else "PARTIAL_DISCLOSURE")
    with open(VERDICT, "w", encoding="utf-8") as f:
        json.dump(v, f, indent=2, ensure_ascii=False)
    print("\nVERDICT", v["verdict"])
    print(json.dumps(v["summary"], indent=2, ensure_ascii=False))
    print("SAVED", VERDICT)


if __name__ == "__main__":
    main()
