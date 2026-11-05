# -*- coding: utf-8 -*-
"""gap_c2s_refined.py — 精化 c2s 差集：识别三种引用写法
  Opcode::NAME / OutPacket::Opcode::NAME / OutPacket(NAME
对照 s9 PacketProcessor 生效 handler 集，输出「服务端有 handler、客户端从不发」。只读。"""
import re
from pathlib import Path

C = Path(r"E:\project\MapleStory-Client")
S9 = Path(r"E:\project\BeiDou-Server_s9\gms-server")

def read(p):
    try:
        return p.read_text(encoding="utf-8", errors="replace")
    except Exception:
        return ""

opkg = read(C / "Net/OutPacket.h")
send_map = {m.group(1): int(m.group(2), 16) if m.group(2).startswith("0x") else int(m.group(2))
            for m in re.finditer(r"([A-Z0-9_]+)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*,", opkg)}

constructed = set()
for py in C.rglob("*"):
    if py.suffix not in (".h", ".cpp") or not py.is_file():
        continue
    rel = str(py.relative_to(C)).replace("\\", "/")
    if rel.startswith(("docs/", "tools/", "build")) or rel == "Net/OutPacket.h":
        continue
    t = read(py)
    for m in re.finditer(r"(?<![\w:])Opcode::([A-Z0-9_]+)", t):
        if m.group(1) in send_map:
            constructed.add(m.group(1))
    for m in re.finditer(r"OutPacket\(\s*([A-Z0-9_]+)\s*[,)]", t):
        if m.group(1) in send_map:
            constructed.add(m.group(1))

srv = read(S9 / "src/main/java/org/gms/net/opcodes/RecvOpcode.java")
srv_recv = {m.group(1): int(m.group(2), 16) for m in re.finditer(r"(\w+)\((0x[0-9A-Fa-f]+)\)", srv)}
pp = ""
for p in S9.rglob("PacketProcessor.java"):
    t = read(p)
    if "registerHandler" in t:
        pp = t
        break
reg_names = set(re.findall(r"registerHandler\(RecvOpcode\.(\w+)", pp))
reg = {srv_recv[n]: n for n in reg_names if n in srv_recv}

sent = {send_map[n] for n in constructed}
gap = {}
for v in sorted(set(reg) - sent):
    gap[v] = reg[v]
print("client_constructed:", len(constructed), " srv_handlers:", len(reg))
print("c2s_gap:", len(gap))
for v, n in gap.items():
    print(f"  {v:5} 0x{v:04X}  {n}")
