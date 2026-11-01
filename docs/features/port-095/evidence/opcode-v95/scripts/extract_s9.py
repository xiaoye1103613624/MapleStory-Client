# -*- coding: utf-8 -*-
"""Extract s9 (server) + v83 client opcode tables for cross-mapping."""
import json, os, re

S9 = r"E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms"
CLIENT = r"E:\project\MapleStory-Client\Net\PacketSwitch.cpp"
ENUM_RE = re.compile(r"^\s*([A-Z][A-Z0-9_]+)\s*\(\s*(0x[0-9A-Fa-f]+|\d+)\s*\)\s*,?\s*(?://\s*(.*))?$")


def parse_enum(path):
    out, cur = {}, None
    for line in open(path, encoding="utf-8", errors="replace"):
        if line.strip().startswith("//"):
            cur = line.split("//", 1)[1].strip() or cur
            continue
        m = ENUM_RE.match(line)
        if m:
            name, val, cmt = m.group(1), int(m.group(2), 0), (m.group(3) or "").strip()
            out[name] = {"value": val, "comment": cmt}
            cur = None
    return out


recv = parse_enum(os.path.join(S9, "net", "opcodes", "RecvOpcode.java"))
send = parse_enum(os.path.join(S9, "net", "opcodes", "SendOpcode.java"))

pp = open(os.path.join(S9, "net", "PacketProcessor.java"), encoding="utf-8", errors="replace").read()
reg = re.findall(r"registerHandler\(\s*RecvOpcode\.([A-Z0-9_]+)\s*,\s*new\s+(\w+)\s*\(", pp)
for name, handler in reg:
    if name in recv:
        recv[name]["handler"] = handler

# client PacketSwitch.cpp: enum Opcode : uint16_t { NAME = 0x.., } + case Opcode::NAME / case NAME
c = open(CLIENT, encoding="utf-8", errors="replace").read()
cenum = dict(re.findall(r"^\s*([A-Z][A-Z0-9_]+)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*,", c, re.M))
cenum = {k: int(v, 0) for k, v in cenum.items()}
# handler per registration: emplace<OPCODE_NAME, XxxHandler>();
chand = dict(re.findall(r"emplace<\s*([A-Z][A-Z0-9_]+)\s*,\s*(\w+)\s*>\s*\(", c))

json.dump({"recv": recv, "send": send, "client": cenum, "client_handler": chand},
          open("s9_opcodes.json", "w", encoding="utf-8"), ensure_ascii=False, indent=1)
print("recv=%d (with handler %d) send=%d client=%d (with handler %d)" %
      (len(recv), sum(1 for v in recv.values() if v.get("handler")),
       len(send), len(cenum), len(chand)))
