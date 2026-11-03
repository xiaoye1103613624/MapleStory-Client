# -*- coding: utf-8 -*-
# R28-B4 取证：在汉化壳 ijl15.dll 里定位「系统设置」两个下拉的条目表。
# 结论要能复跑，所以整段落到一个脚本里，输出写 UTF-8 文件（stdout 是 cp936，不能打印 CJK）。
import re, io

DLL = "E:/MXD/BeiDou-ClientV17.7/ijl15.dll"
NEEDLES = [
    "\u4e0d\u663e\u793a",            # 不显示
    "\u53ea\u663e\u793a\u540d\u5b57",  # 只显示名字
    "\u53ea\u663e\u793aHP",           # 只显示HP
    "\u540d\u5b57\u548cHP\u90fd\u663e\u793a",  # 名字和HP都显示
    "MapleStory\u6587\u4ef6\u5939",     # MapleStory文件夹
    "\u684c\u9762",                    # 桌面
]

data = open(DLL, "rb").read()
out = io.open("E:/project/_r28/ijl15_scan.txt", "w", encoding="utf-8")
out.write("== %s size=%d\n" % (DLL, len(data)))

hits = {}
for n in NEEDLES:
    gbk = n.encode("gbk")
    off = data.find(gbk)
    out.write("%-24s gbk@%s count=%d\n" % (n, hex(off) if off >= 0 else "NOT-FOUND",
                                           len(re.findall(re.escape(gbk), data))))
    if off >= 0:
        hits[n] = off

# 以「怪物情报」四条的首条为锚，把其后以 \0 分隔的连续串整段读出，验证顺序
def dump_run(start, span=160):
    raw = data[start:start + span]
    parts = raw.split(b"\x00")
    lines = []
    consumed = 0
    for p in parts:
        if not p:
            consumed += 1
            continue
        try:
            text = p.decode("gbk")
        except Exception:
            text = "<undecodable %s>" % p.hex()
        lines.append("  @%s  %r" % (hex(start + consumed), text))
        consumed += len(p) + 1
    return lines

anchor_mob = hits.get("\u4e0d\u663e\u793a")
if anchor_mob is not None:
    out.write("\n-- run from 0x%x (mob info table) --\n" % anchor_mob)
    out.write("\n".join(dump_run(anchor_mob)) + "\n")

anchor_path = hits.get("MapleStory\u6587\u4ef6\u5939")
if anchor_path is not None:
    out.write("\n-- run from 0x%x (screenshot folder table) --\n" % anchor_path)
    out.write("\n".join(dump_run(anchor_path)) + "\n")

out.close()
print("done")
