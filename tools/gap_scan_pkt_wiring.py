# -*- coding: utf-8 -*-
"""gap_scan_pkt_wiring.py — 出站包类“已定义 vs 已接线”：
对 Net/OutPacket.h 每个 Opcode，找到 Net/Packets 中引用它的包类；
再看该类在 IO/ Gameplay/ Character/ Graphics/ Net/Handlers/ 有无调用点。
零调用点 = 定义了但从不发（未接线）。只读。"""
import re, json
from pathlib import Path

CLIENT = Path(r"E:\project\MapleStory-Client")
def read(p):
    try: return p.read_text(encoding="utf-8", errors="replace")
    except Exception: return ""

opkg = read(CLIENT / "Net/OutPacket.h")
send_map = {m.group(1): int(m.group(2),16) if m.group(2).startswith("0x") else int(m.group(2))
            for m in re.finditer(r"([A-Z0-9_]+)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*,", opkg)}

# 1) opcode -> packet classes (class X : public OutPacket / subclass, whose ctor references the opcode)
txt_all = {}
for py in CLIENT.rglob("*"):
    if py.suffix in (".h",".cpp") and py.is_file():
        rel = str(py.relative_to(CLIENT)).replace("\\","/")
        txt_all[rel] = read(py)

# build class -> opcode map by scanning `class NAME ... OutPacket(OutPacket::Opcode::OP` proximity
op_to_classes = {}
for rel, txt in txt_all.items():
    if not rel.startswith("Net/"): continue
    # crude: each class block ends at next `class `
    blocks = re.split(r"(?m)^\s*class\s+", txt)
    heads = re.findall(r"(?m)^\s*class\s+([A-Za-z_0-9]+)", txt)
    # pair: split gives blocks[1..] aligned with heads
    for name, blk in zip(heads, blocks[1:]):
        ops = set(re.findall(r"OutPacket::Opcode::([A-Z0-9_]+)", blk)) | set(re.findall(r"OutPacket\(\s*([A-Z0-9_]+)\s*[,)]", blk))
        for op in ops:
            if op in send_map:
                op_to_classes.setdefault(op, set()).add(name)

# 2) call sites outside Net/
CONSUMERS = [rel for rel in txt_all if rel.startswith(("IO/","Gameplay/","Character/","Graphics/","Net/Handlers/","Net/Session","MapleStory.cpp","Configuration"))]
unwired = {}
for op, classes in sorted(op_to_classes.items()):
    hit = False
    for rel in CONSUMERS:
        t = txt_all[rel]
        for c in classes:
            if re.search(r"\b"+re.escape(c)+r"\b\s*[<( ]", t):
                hit = True; break
        if hit: break
    if not hit:
        unwired[op] = sorted(classes)

# 3) opcode defined but no packet class at all
no_class = [op for op in send_map if op not in op_to_classes]

out = {"unwired_opcodes": unwired, "opcode_no_class": sorted(no_class)}
Path(CLIENT/"tools/_gap_pkt_wiring.json").write_text(json.dumps(out, ensure_ascii=False, indent=1), encoding="utf-8")
print(json.dumps(out, ensure_ascii=False, indent=1)[:6000])
