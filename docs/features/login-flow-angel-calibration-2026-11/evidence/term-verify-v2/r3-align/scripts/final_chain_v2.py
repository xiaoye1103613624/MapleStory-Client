#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""R3 收口原子链 v2：修锚点超时 -> 抓两页 -> 立刻快照干净帧 -> 复验 -> 绑 exe sha。

真因（15:13 packet_dump 硬证据）：登录协议链完整
  [SEND] 01 00 05 00 admin 05 00 admin   (登录包)
  [RECV] 00 00 ...                        (登录成功)
  [SEND] 0b 00 / [RECV] 0a 00 00 ...Scania / [RECV] 0a 00 ff (serverlist end)
但 capture 脚本 wait_anchor 只等 12s，服务端响应 >12s => anchor_hit=False，
脚本在登录页上点 (485,210) 空坐标，抓到 mean136.7 登录页帧（不是频道页）。
修法：两脚本所有 timeout=12.0 -> 30.0，不改点击坐标与判据。

干净帧抓到后立刻复制到 shots/clean_r3_final/ 并定死 sha，防别会话/后续脚本覆盖。
全程不重编；exe sha 首尾绑定，漂移即披露。
"""
import hashlib
import json
import os
import shutil
import subprocess
import sys
import time

LIVE = r"E:\MXD\BeiDou-Client-xy\MapleStory.exe"
SCRIPTS = r"C:\Users\11036\AppData\Local\Temp\term_verify\scripts"
SHOTS = r"C:\Users\11036\AppData\Local\Temp\term_verify\shots"
UI_R3 = r"C:\Users\11036\AppData\Local\Temp\term_verify\ui_r3"
CLEAN = os.path.join(SHOTS, "clean_r3_final")
VERDICT = os.path.join(UI_R3, "final_chain_v2.json")
os.makedirs(CLEAN, exist_ok=True)
os.makedirs(UI_R3, exist_ok=True)


def sha(p):
    return hashlib.sha256(open(p, "rb").read()).hexdigest() if os.path.exists(p) else None


def jload(p):
    return json.load(open(p, "r", encoding="utf-8", errors="replace"))


def run(args, cwd, timeout=420):
    r = subprocess.run(args, cwd=cwd, capture_output=True, text=True,
                       errors="replace", timeout=timeout)
    return (r.stdout or "") + "\n" + (r.stderr or ""), r.returncode


def mean_of(p):
    import numpy as np
    from PIL import Image
    a = np.asarray(Image.open(p).convert("RGB")).astype(float)
    return round(float(a.mean()), 2), round(float(a.std()), 2)


def main():
    v = {"ts": time.strftime("%Y-%m-%d %H:%M:%S"), "mode": "capture+eval, no rebuild"}
    s0 = sha(LIVE)
    v["exe_start"] = s0
    print("EXE START", s0)

    # 1. 修锚点超时（只改 timeout，不动坐标/判据）
    patched = {}
    for fn in ("capture_channel_grid.py", "capture_charselect_clean.py"):
        p = os.path.join(SCRIPTS, fn)
        b = open(p, "r", encoding="utf-8", errors="replace").read()
        n = b.count("timeout=12.0")
        b2 = b.replace("timeout=12.0", "timeout=30.0")
        open(p, "w", encoding="utf-8", newline="").write(b2)
        patched[fn] = n
    v["patched_timeouts"] = patched
    print("PATCHED", patched)

    # 2. 清场（含服务端会话释放）
    subprocess.run(["taskkill", "/F", "/IM", "MapleStory.exe"], capture_output=True)
    time.sleep(4)

    # 3. 频道页
    caps = {}
    for name, script, sm, pref in [
        ("channel", "capture_channel_grid.py", "channel_grid_summary.json", "channel_grid_f"),
        ("charselect", "capture_charselect_clean.py", "charselect_clean_summary.json", "charselect_clean_f"),
    ]:
        out, rc = run([sys.executable, os.path.join(SCRIPTS, script)], SCRIPTS)
        sp = os.path.join(SHOTS, sm)
        s = jload(sp) if os.path.exists(sp) else {}
        frames = s.get("frames") or []
        # 立刻快照全部帧 + sha
        saved = []
        for f in frames:
            src = os.path.join(SHOTS, f["file"])
            if os.path.exists(src):
                dst = os.path.join(CLEAN, f["file"])
                shutil.copy2(src, dst)
                saved.append({"file": f["file"], "sha": sha(dst),
                              "mean": f.get("mean"), "real": f.get("real_gl_frame")})
        caps[name] = {
            "rc": rc, "ts": s.get("ts"), "frames": len(frames),
            "anchor_hit": s.get("anchor_hit", s.get("serverlist_anchor_hit")),
            "charlist_hit": s.get("charlist_hit"),
            "error": s.get("error"), "snapshots": saved,
        }
        print("CAPTURE", name, "frames", len(frames),
              "anchor", caps[name]["anchor_hit"], "charlist", caps[name]["charlist_hit"])
        time.sleep(1.0)
    v["captures"] = caps

    # 4. 帧定性（f05 / f09）
    frame_quality = {}
    for fn in ("channel_grid_f05.png", "charselect_clean_f09.png"):
        p = os.path.join(CLEAN, fn)
        if os.path.exists(p):
            m, sd = mean_of(p)
            frame_quality[fn] = {"mean": m, "std": sd, "sha": sha(p)}
            print("FRAME", fn, "mean", m, "std", sd)
    v["frame_quality"] = frame_quality

    # 5. 复验（用刚快照的干净帧：把 verify 脚本指向 clean 目录前，
    #    先把干净帧拷回 shots 覆盖被污染的帧，保证脚本读到的就是这一批）
    for f in os.listdir(CLEAN):
        if f.endswith(".png"):
            shutil.copy2(os.path.join(CLEAN, f), os.path.join(SHOTS, f))

    evals = {}
    for name, script in [("verify_r3_align", "verify_r3_align.py"),
                        ("real_corners", "settle_frame_real_corners.py")]:
        out, rc = run([sys.executable, os.path.join(SCRIPTS, script)], SCRIPTS, timeout=300)
        evals[name] = {"rc": rc, "tail": out[-1200:]}
        for line in out.splitlines():
            if "ALL_PASS" in line or "VERDICT" in line or "SPAN " in line:
                print("EVAL", name, line.strip())
    v["evals"] = evals

    # 6. 汇总 + 绑定
    vr = jload(os.path.join(UI_R3, "verify_r3_align.json"))
    rcj = jload(os.path.join(UI_R3, "real_corners_settle.json"))
    v["summary"] = {
        "all_pass": vr.get("all_pass"), "all_steady": vr.get("all_steady"),
        "shifts": {k: d.get("shift_after") for k, d in vr.get("components", {}).items()},
        "ncc": {k: (d.get("rep") or {}).get("ncc") for k, d in vr.get("components", {}).items()},
        "corner_span": {"target": rcj.get("TARGET", {}).get("span_nw_ne"),
                        "ours": rcj.get("OURS_V1", {}).get("span_nw_ne")},
        "corner_ne_x": {"target": rcj.get("TARGET", {}).get("ne", {}).get("pos", [None])[0],
                        "ours": rcj.get("OURS_V1", {}).get("ne", {}).get("pos", [None])[0]},
        "corner_se_y": {"target": rcj.get("TARGET", {}).get("se", {}).get("pos", [None, None])[1],
                        "ours": rcj.get("OURS_V1", {}).get("se", {}).get("pos", [None, None])[1]},
    }
    s1 = sha(LIVE)
    v["exe_end"] = s1
    v["exe_unchanged"] = (s1 == s0)
    clean_channel = (caps["channel"]["anchor_hit"] is True)
    v["verdict"] = ("R3_FINAL_VERIFIED" if vr.get("all_pass") and clean_channel
                    and v["exe_unchanged"] else "PARTIAL_DISCLOSURE")
    json.dump(v, open(VERDICT, "w", encoding="utf-8"), indent=2, ensure_ascii=False)
    print("\nEXE END", s1, "UNCHANGED", v["exe_unchanged"])
    print("VERDICT", v["verdict"])
    print(json.dumps(v["summary"], indent=2, ensure_ascii=False))
    print("SAVED", VERDICT)


if __name__ == "__main__":
    main()
