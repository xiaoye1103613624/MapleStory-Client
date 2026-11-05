# -*- coding: utf-8 -*-
"""gap_scan_2026_11.py v2 — 现场重算 客户端 × s9服务端 × 原生参考端 功能差距（只读）"""
import json, re
from pathlib import Path

CLIENT = Path(r"E:\project\MapleStory-Client")
SERVER = Path(r"E:\project\BeiDou-Server_s9\gms-server")

def read(p):
    try:
        return p.read_text(encoding="utf-8", errors="replace")
    except Exception:
        return ""

# ---------- client s2c (recv) ----------
psw = read(CLIENT / "Net/PacketSwitch.cpp")
recv_enum = re.findall(r"([A-Z0-9_]+)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*,", psw)
recv_map = {n: int(v, 16) if str(v).startswith("0x") else int(v) for n, v in recv_enum}
regs = re.findall(r"emplace<\s*([A-Z0-9_]+)\s*,\s*([A-Za-z_0-9]+)\s*>\s*\(\s*\)", psw)
registered = {}          # value -> (opname, handler)
for opname, cls in regs:
    if opname in recv_map:
        registered[recv_map[opname]] = (opname, cls)

# ---------- client c2s (send) ----------
opkg = read(CLIENT / "Net/OutPacket.h")
send_enum = re.findall(r"([A-Z0-9_]+)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*,", opkg)
send_map = {n: int(v, 16) if str(v).startswith("0x") else int(v) for n, v in send_enum}
constructed = set()
for py in CLIENT.rglob("*"):
    if py.suffix not in (".h", ".cpp") or not py.is_file():
        continue
    rel = str(py.relative_to(CLIENT)).replace("\\", "/")
    if rel.startswith(("docs/", "tools/")) or rel == "Net/OutPacket.h":
        continue
    txt = read(py)
    # idiom A: OutPacket::Opcode::NAME (any call site, incl. subclass delegating ctors)
    for m in re.finditer(r"OutPacket::Opcode::([A-Z0-9_]+)", txt):
        if m.group(1) in send_map:
            constructed.add(m.group(1))
    # idiom B: ctor with unqualified name: OutPacket(NAME, ...) / OutPacket(NAME)
    for m in re.finditer(r"OutPacket\(\s*([A-Z0-9_]+)\s*[,)]", txt):
        if m.group(1) in send_map:
            constructed.add(m.group(1))
sent_values = {send_map[n] for n in constructed}

# ---------- server ----------
recv_java = read(SERVER / "src/main/java/org/gms/net/opcodes/RecvOpcode.java")
send_java = read(SERVER / "src/main/java/org/gms/net/opcodes/SendOpcode.java")
srv_recv = {m.group(1): int(m.group(2), 16) for m in re.finditer(r"(\w+)\((0x[0-9A-Fa-f]+)\)", recv_java)}
srv_send = {m.group(1): int(m.group(2), 16) for m in re.finditer(r"(\w+)\((0x[0-9A-Fa-f]+)\)", send_java)}

pp = ""
for py in SERVER.rglob("PacketProcessor.java"):
    t = read(py)
    if "registerHandler" in t:
        pp = t
        break
reg_names = set(re.findall(r"registerHandler\(RecvOpcode\.(\w+)", pp))
srv_reg = {srv_recv[n] for n in reg_names if n in srv_recv}

real_emit = set()
for py in SERVER.rglob("*.java"):
    if py.name in ("SendOpcode.java", "RecvOpcode.java"):
        continue
    t = read(py)
    for m in re.finditer(r"SendOpcode\.(\w+)", t):
        if m.group(1) in srv_send:
            real_emit.add(srv_send[m.group(1)])

# ---------- diffs ----------
def names_for_recv(v):
    return [n for n, x in srv_recv.items() if x == v]
def names_for_send(v):
    return [n for n, x in srv_send.items() if x == v]

c2s_gap = {v: names_for_recv(v) for v in sorted(srv_reg - sent_values)}
s2c_gap = {v: names_for_send(v) for v in sorted(real_emit - set(registered))}
client_only_out = sorted(sent_values - srv_reg)
client_only_recv = sorted(set(registered) - real_emit)

# ---------- UI 覆盖（人工整理于结果说明，这里只出协议面） ----------
out = {
    "counts": {
        "client_recv_enum": len(recv_map),
        "client_registered": len(registered),
        "client_out_enum": len(send_map),
        "client_out_constructed": len(constructed),
        "srv_recv_total": len(srv_recv),
        "srv_recv_handlers": len(srv_reg),
        "srv_send_total": len(srv_send),
        "srv_send_emitted": len(real_emit),
    },
    "c2s_gap": c2s_gap,
    "s2c_gap": s2c_gap,
    "client_only_out": client_only_out,
    "client_only_recv": client_only_recv,
}
Path(CLIENT / "tools/_gap_scan_out.json").write_text(json.dumps(out, ensure_ascii=False, indent=1), encoding="utf-8")
print(json.dumps(out["counts"], ensure_ascii=False))
print("c2s_gap", len(c2s_gap), "s2c_gap", len(s2c_gap))
