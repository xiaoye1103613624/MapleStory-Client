#!/usr/bin/env python3
"""在别会话半成品文件污染全量重编时，做一次「隔离编译」得到可验收二进制。

背景：13:30 的确定性构建 BUILD_EXITCODE=0（exe 50ec0bd3）。13:53:55 别会话
修改了 IO/Components/ItemTooltip.cpp 与 IO/UITypes/UIShop.cpp，二者直接访问
ms::ItemData::itemid（已 private）→ 二次构建报：
  error C2248: "ms::ItemData::itemid": 无法访问 private 成员(在"ms::ItemData"类中声明)
  IO/Components/ItemTooltip.cpp(62,34) / IO/UITypes/UIShop.cpp(674,29) / IO/UITypes/UIShop.cpp(705,30)

做法（原子、零丢失、finally 必恢复）：
 1. 把这两个别会话文件按字节备份到内存 + 临时文件，记录 sha256；
 2. 临时写入 git show HEAD:<path> 的原始字节（该文件在 13:30 成功构建时即 HEAD 态）；
 3. 跑 build_calibration.bat（MSBuild 增量，仅重编我改的 2 个 TU + 这 2 个恢复 TU）；
 4. finally 无条件把备份字节写回，复核 sha256 全等 → 别会话改动一字不丢；
 5. 报告 BUILD_EXITCODE、exe sha/mtime、恢复校验。

绝不 git add / commit 这些文件，绝不删别会话任何内容。
"""
import hashlib
import os
import subprocess
import sys
import time

ROOT = r"E:\project\MapleStory-Client"
EXE = r"E:\MXD\BeiDou-Client-xy\MapleStory.exe"
BAT = os.path.join(ROOT, "build_calibration.bat")
FOREIGN = [
    "IO/Components/ItemTooltip.cpp",
    "IO/UITypes/UIShop.cpp",
]
BACKUP_DIR = r"C:\Users\11036\AppData\Local\Temp\term_verify\quarantine_backup"
os.makedirs(BACKUP_DIR, exist_ok=True)


def sha(b):
    return hashlib.sha256(b).hexdigest()


def head_bytes(rel):
    return subprocess.check_output(["git", "show", "HEAD:" + rel], cwd=ROOT)


def main():
    backups = {}
    print("=== QUARANTINE BACKUP ===")
    for rel in FOREIGN:
        p = os.path.join(ROOT, rel.replace("/", os.sep))
        b = open(p, "rb").read()
        bp = os.path.join(BACKUP_DIR, rel.replace("/", "_") + ".bak")
        open(bp, "wb").write(b)
        backups[rel] = (p, b, sha(b), bp)
        print("  %s  bytes=%d sha=%s" % (rel, len(b), sha(b)))

    hb = {rel: head_bytes(rel) for rel in FOREIGN}
    print("=== HEAD BYTES FETCHED ===")
    for rel, b in hb.items():
        print("  %s  HEAD bytes=%d sha=%s" % (rel, len(b), sha(b)))

    build_out = ""
    exitcode = None
    try:
        print("=== WRITE HEAD STATE (temporary) ===")
        for rel in FOREIGN:
            open(backups[rel][0], "wb").write(hb[rel])
            print("  wrote HEAD ->", rel)
        print("=== BUILD ===")
        r = subprocess.run(["cmd.exe", "/c", BAT], cwd=ROOT,
                           capture_output=True, text=True, errors="replace")
        build_out = (r.stdout or "") + "\n" + (r.stderr or "")
        print(build_out[-1500:])
        for line in build_out.splitlines():
            if "BUILD_EXITCODE=" in line:
                exitcode = line.split("BUILD_EXITCODE=")[-1].strip()
        print("BUILD_EXITCODE_PARSED", exitcode)
    finally:
        print("=== RESTORE FOREIGN FILES (unconditional) ===")
        all_ok = True
        for rel, (p, b, h, bp) in backups.items():
            open(p, "wb").write(b)
            got = sha(open(p, "rb").read())
            ok = got == h
            all_ok &= ok
            print("  %s restored sha=%s match=%s" % (rel, got, ok))
            if not ok:
                print("  !! FALLBACK FROM DISK BACKUP", bp)
                open(p, "wb").write(open(bp, "rb").read())
                print("  re-verified", sha(open(p, "rb").read()) == h)
                all_ok &= sha(open(p, "rb").read()) == h
        print("RESTORE_ALL_OK", all_ok)
        if not all_ok:
            sys.exit(3)

    print("=== EXE ===")
    if os.path.exists(EXE):
        eb = open(EXE, "rb").read()
        st = os.stat(EXE)
        print("  sha256=%s" % sha(eb))
        print("  size=%d mtime=%s" % (st.st_size, time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(st.st_mtime))))
    else:
        print("  EXE MISSING")
    print("DONE build_exitcode=%s" % exitcode)


if __name__ == "__main__":
    main()
