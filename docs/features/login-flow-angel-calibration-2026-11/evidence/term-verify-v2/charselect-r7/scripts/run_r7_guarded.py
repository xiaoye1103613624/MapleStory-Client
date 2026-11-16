#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""R7 runner：收敛进程 -> 关 auto-login 护栏 -> verify_r7_charselect -> 恢复。"""
import os, sys, json, time, subprocess
SCRIPTS = r"C:\Users\11036\AppData\Local\Temp\term_verify\scripts"
sys.path.insert(0, SCRIPTS)
import auto_login_guard as guard
VERIFY = os.path.join(SCRIPTS, "verify_r7_charselect.py")
KILL = ["MapleStory.exe", "MapleStory_r4.exe"]
def kill(names):
    for n in names: subprocess.run(["taskkill","/F","/IM",n], capture_output=True)
def main():
    print("=== R7 GUARDED VERIFY ===", flush=True)
    kill(KILL); time.sleep(1.5)
    rec = guard.disable()
    print("DISABLED", json.dumps(rec, ensure_ascii=False), flush=True)
    rc = None
    try:
        r = subprocess.run([sys.executable, VERIFY], cwd=r"E:\project\MapleStory-Client")
        rc = r.returncode
        print("VERIFY_RC", rc, flush=True)
    finally:
        kill(["MapleStory_r4.exe"]); time.sleep(0.8)
        ok = guard.restore()
        print("AUTO_LOGIN_RESTORE_OK", ok, flush=True)
    return 0 if (rc == 0 and ok) else 1
if __name__ == "__main__":
    sys.exit(main())
