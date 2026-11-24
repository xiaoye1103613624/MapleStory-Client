#!/usr/bin/env python3
"""Apply generated equip potline/spark/soul locale entries, preserving UTF-8 no BOM.

Replaces existing equip.potline.*/spark.*/soul.* lines with the full generated set.
Backs up originals to .bak_eqploc before writing. Verifies strict UTF-8 read-back.
"""
import json, shutil

D = json.load(open(r"E:\project\MapleStory-Client\tools\_eqploc.json", encoding="utf-8"))
FILES = {
    "zh": r"E:\project\MapleStory-Client\locale\zh-CN.ini",
    "en": r"E:\project\MapleStory-Client\locale\en-US.ini",
}
PREFIXES = ("equip.potline.", "equip.spark.", "equip.soul.")

for lang, path in FILES.items():
    raw = open(path, "rb").read()
    assert not raw.startswith(b"\xef\xbb\xbf"), f"BOM in {path}"
    text = raw.decode("utf-8")  # strict
    lines = text.splitlines(keepends=True)
    kept = [ln for ln in lines if not ln.lstrip().startswith(PREFIXES)]
    new_entries = [ln + "\r\n" for ln in D[lang]]
    shutil.copy2(path, path + ".bak_eqploc")
    # 在末尾追加（locale 解析按 key=value，顺序无关）
    out = "".join(kept)
    if out and not out.endswith("\n"):
        out += "\r\n"
    out += "".join(new_entries)
    open(path, "wb").write(out.encode("utf-8"))
    chk = open(path, "rb").read().decode("utf-8")
    cnt = sum(1 for ln in chk.splitlines() if ln.startswith(PREFIXES))
    print(f"{lang}: wrote {len(D[lang])} entries, total prefix lines={cnt}, utf8=OK")
print("DONE")
