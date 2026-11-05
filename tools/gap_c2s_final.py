# -*- coding: utf-8 -*-
"""gap_c2s_final.py — c2s 差集终版（修正负向断言把 OutPacket::Opcode::NAME
误排除的 bug）。两层判据：
  L1 opcode -> 是否有包类构造引用（两种写法：`Opcode::NAME`（任意前缀）
     与 `OutPacket(NAME`）
  L2 该包类是否被 IO/Gameplay/Character/Net-Handlers 实例化（含 () / <）
只读；输出每 opcode: 构造=是/否, 实例化=是/否, 引用类名。"""
import re
from pathlib import Path

C = Path(r"E:\project\MapleStory-Client")
S9 = Path(r"E:\project\BeiDou-Server_s9\gms-server")

def read(p):
    try:
        return p.read_text(encoding="utf-8", errors="replace")
    except Exception:
        return ""

# ---- client out enum ----
send_map = {m.group(1): int(m.group(2), 16) if m.group(2).startswith("0x") else int(m.group(2))
            for m in re.finditer(r"([A-Z0-9_]+)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*(?:,|(?=\s*\}))",
                                 read(C / "Net/OutPacket.h"))}

txt = {}
for py in C.rglob("*"):
    if py.suffix not in (".h", ".cpp") or not py.is_file():
        continue
    rel = str(py.relative_to(C)).replace("\\", "/")
    if rel.startswith(("docs/", "tools/", "build", "includes/")):
        continue
    txt[rel] = read(py)

# opcode -> referencing files (L1)
ref_files = {}
for rel, t in txt.items():
    if rel == "Net/OutPacket.h":
        continue
    names = set()
    names.update(re.findall(r"Opcode::([A-Z0-9_]+)", t))
    names.update(re.findall(r"OutPacket\(\s*([A-Z0-9_]+)\s*[,)]", t))
    for n in names:
        if n in send_map:
            ref_files.setdefault(n, set()).add(rel)

# packet class -> opcode, via ctor proximity inside Net/Packets
cls2op = {}
op2cls = {}
for rel, t in txt.items():
    if not rel.startswith("Net/Packets"):
        continue
    for m in re.finditer(r"(?m)^\s*class\s+([A-Za-z_0-9]+)[^;]*?;[\s\S]*?(?=(?:^\s*class\s+)|\Z)", t):
        body = m.group(0)
        for n in set(re.findall(r"Opcode::([A-Z0-9_]+)", body)) | set(re.findall(r"OutPacket\(\s*([A-Z0-9_]+)\s*[,)]", body)):
            if n in send_map:
                cls2op[m.group(1)] = n
                op2cls.setdefault(n, set()).add(m.group(1))

# L2: class instantiated outside its declaring header.
# Subclass-aware pass: `class B(...) : public A(...)` where A maps to an opcode
# inherits that opcode (StorageTakePacket -> StoragePacket -> STORAGE).
for _ in range(4):
    changed = False
    for rel, t in txt.items():
        if not rel.startswith("Net/Packets"):
            continue
        for m in re.finditer(r"(?m)^\s*class\s+([A-Za-z_0-9]+)[^;]*?;", t):
            cname = m.group(1)
            if cname in cls2op:
                continue
            base = re.search(r":\s*(?:public\s+)?([A-Za-z_0-9]+)\s*\(", m.group(0)[m.group(1).__len__() + 7:])
            if base and base.group(1) in cls2op:
                cls2op[cname] = cls2op[base.group(1)]
                op2cls.setdefault(cls2op[base.group(1)], set()).add(cname)
                changed = True
    if not changed:
        break

CONSUMER = lambda rel: rel.startswith(("IO/", "Gameplay/", "Character/", "Graphics/", "Net/Handlers/", "Audio/")) or rel in ("MapleStory.cpp",)
op_instantiated = {}
for n, classes in op2cls.items():
    hit = False
    for c in classes:
        for rel in CONSUMER and [r for r in txt if CONSUMER(r)]:
            if re.search(r"\b" + c + r"\s*(?:<|\(|\.)", txt[rel]):
                hit = True
                break
        if hit:
            break
    op_instantiated[n] = hit

# server side
srv_recv = {m.group(1): int(m.group(2), 16) for m in re.finditer(r"(\w+)\((0x[0-9A-Fa-f]+)\)",
            read(S9 / "src/main/java/org/gms/net/opcodes/RecvOpcode.java"))}
pp = ""
for p in S9.rglob("PacketProcessor.java"):
    t = read(p)
    if "registerHandler" in t:
        pp = t
        break
reg = {srv_recv[n]: n for n in set(re.findall(r"registerHandler\(RecvOpcode\.(\w+)", pp)) if n in srv_recv}

sent_fully = {send_map[n] for n, ok in op_instantiated.items() if ok}
gap = {v: reg[v] for v in sorted(set(reg) - sent_fully)}

lines = [f"srv_handlers={len(reg)} client_opcodes={len(send_map)} fully_wired={len(sent_fully)} gap={len(gap)}"]
for v, n in gap.items():
    has_ctor = v in {send_map[k] for k in op2cls}
    lines.append(f"{v:5} 0x{v:04X}  {n:28} ctor={'Y' if has_ctor else 'N'}")
Path(C / "tools/_gap_c2s_final.txt").write_text("\n".join(lines), encoding="utf-8")
print("\n".join(lines))
