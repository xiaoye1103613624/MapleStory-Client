#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""R4 真机复验 runner：先收敛进程 → 配置护栏 → 原验证脚本 → 收敛 r4 → 字节恢复 → 同一判据复算。

护栏与抓帧脚本彻底解耦：verify_r4_channel.py 一个字不改。
关键顺序（蓝军翻案）：客户端退出时会重写 Settings（22:23 实测 6612B 覆写），
故必须先 taskkill 让所有既有实例把配置写盘落定，再翻 login.auto；
finally 里先收敛自己启动的 r4 实例，再字节级恢复，避免残留进程退出时反覆盖。
"""
import os
import sys
import json
import time
import subprocess

SCRIPTS = r"C:\Users\11036\AppData\Local\Temp\term_verify\scripts"
sys.path.insert(0, SCRIPTS)
import auto_login_guard as guard

VERIFY = os.path.join(SCRIPTS, "verify_r4_channel.py")
AUDIT = os.path.join(SCRIPTS, "r4_after_audit.py")
KILL_NAMES = ["MapleStory.exe", "MapleStory_ch20.exe", "MapleStory_r4.exe"]


def kill(names):
    for n in names:
        subprocess.run(["taskkill", "/F", "/IM", n], capture_output=True)


def main():
    print("=== R4 GUARDED RE-VERIFY ===", flush=True)
    # 1) 先让所有既有客户端退出并写完配置，护栏才不会被退出写盘反覆盖
    kill(KILL_NAMES)
    time.sleep(1.5)
    print("PRE_GUARD_KILL_DONE", flush=True)

    rec = guard.disable()
    print("DISABLED", json.dumps(rec, ensure_ascii=False), flush=True)
    verify_rc = None
    audit_rc = None
    try:
        print("=== STEP 1: verify_r4_channel.py (unmodified) ===", flush=True)
        r = subprocess.run([sys.executable, VERIFY], cwd=r"E:\project\MapleStory-Client")
        verify_rc = r.returncode
        print("VERIFY_RC", verify_rc, flush=True)
    finally:
        # 2) 收敛本次 r4 实例，避免其退出时把 Settings 写回 true
        kill(["MapleStory_r4.exe"])
        time.sleep(0.8)
        ok = guard.restore()
        print("AUTO_LOGIN_RESTORE_OK", ok, flush=True)
        if not ok:
            print("FATAL: config restore failed — inspect", guard.EVIDENCE, flush=True)
    print("=== STEP 2: r4_after_audit.py (same acceptance criteria) ===", flush=True)
    a = subprocess.run([sys.executable, AUDIT], cwd=r"E:\project\MapleStory-Client")
    audit_rc = a.returncode
    print("AUDIT_RC", audit_rc, flush=True)
    return 0 if (verify_rc == 0 and audit_rc == 0) else 1


if __name__ == "__main__":
    sys.exit(main())
