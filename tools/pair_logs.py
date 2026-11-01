#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""T9: 客户端 packet_dump.log 与服务端 out.log 的逐包配对管线.

为什么需要它
------------
T4 的 oracle 只能证明「加解密算法与 s9 一致」，T8 的探针只能证明「头部与派发
被 s9 接受」。字段级 body 对账（P2 的 S1..S11）需要第三种证据：同一次真实会话
里，客户端发出的字节与服务端真正读到的字节逐包相等。本工具就是那条取证流水线。

四种日志格式（全部按源码逐字对齐，不凭记忆）
--------------------------------------------
1) 客户端出站/入站（本仓库 Net/Session.cpp:36-56，packet_dump()）::

       [SEND] PacketDump : 01 00 00 00 61 61 ...
       [RECV] PacketDump : 00 00 04 00 00 00 ...

   - 十六进制**小写**、空格分隔、每行末尾还有一个空格（"%02x " 循环）。
   - SEND 在 cryptography.encrypt() 之前打（Session.cpp:166），RECV 在
     cryptography.decrypt() 之后打（Session.cpp:132）⇒ 两侧都是**明文正文**，
     前 2 字节是小端 opcode，**不含 4 字节头**。
   - [SEND]/[RECV] 标记是 T9 新加的；标记之前的旧行（裸 "PacketDump : "）方向
     未知，本工具单独归类，不参与配对。

2) 服务端档 B（debug.packet，gms-server InPacketLogger.java:38-39 /
   OutPacketLogger.java:39-40）::

       ClientSend:LOGIN_PASSWORD [1] (16) <HEX> 01 00 41 42 ... <TEXT> .AB...
       <UnknownPacket> ServerSend:null [ACFF] (4) <HEX> FF AC 00 00 <TEXT> ...
       AC FF ...                   ← 退化行：正文超阈值时只打前 2 字节
                                     InPacketLogger.java:40-41 阈值 3000、格式 "{}..."（无空格）
                                     OutPacketLogger.java:41-42 阈值 50000、格式 "{} ..."（有空格）

   - `<HEX>` 用 HexTool.toHexString()（HexTool.java:41-43，大写、空格分隔）；
     `<TEXT>` 用 HexTool.toStringFromCharset()（HexTool.java:73-84，控制码替成
     '.'，按当前账号 charset 解码）⇒ 这一栏对 T10/T11 的字符集定盘直接可用。
   - 括号里的数字 = packet.getBytes() 长度（readerIndex..writerIndex，含 opcode、
     不含 4 字节头）⇒ 与客户端 packet_dump 的字节数**应严格相等**，本工具做校验。
   - opcode 名来自 OpcodeConstants.send/recvOpcodeNames，查不到时 SLF4J 打成 `null`
     并加 `<UnknownPacket> ` 前缀。
   - **不受 LoggingUtil.isIgnoredRecvPacket 过滤**（InPacketLogger 里没有这个判断）。

3) 服务端档 A（use_debug_show_rcvd_packet，Client.java:216-218 +
   ByteBufInPacket.java:92-108）::

       收到封包 包头ID [0x18] 内容： ByteBufInPacket[1800_5300...000000]

   - 十六进制**连续无空格、大写**，并在 2*readerIndex 处插入 '_'
     （ByteBufInPacket.insertReaderPosition，:104-108）。因为 Client.java:214 已先
     readShort() 取走 opcode，所以正常包的 '_' 恒在第 4 个 hex 字符处。
   - **被过滤的 7 个 opcode 不会出现**（LoggingUtil.java:9-23：41/89/91/107/167/188/197）
     ⇒ 档 A 单独用会漏掉移动/NPC 类高频包，配对时必须知道这一点。
   - 同一条入站包在档 B 开着时会先打 ClientSend 再打这一行 ⇒ 本工具按字节相同做
     镜像去重，不重复计数。

4) log4j2 行前缀（gms-server/src/main/resources/log4j2.xml:6-7）::

       2026-10-30 19:20:00.123 [ INFO] ==> <message>

   解析时先剥掉前缀；前缀之外的时间戳用于给配对结果标注服务端接收时刻。

配对判据
--------
同方向（c2s / s2c）两条 TCP 流内的包序是确定的，所以做「带前瞻窗口的贪心顺序
匹配」：先找 opcode 相同且逐字节相同的 EXACT；窗口内只有 opcode 相同但字节不同
则记 DIFF 并给出**首个差异字节偏移**；opcode 都对不上记 UNMATCHED。窗口被跳过的
服务端记录单独列出（原因：档 A 过滤、超阈值截断、日志开关中途切换等），
PLAN §4 T9 验收要求「配不上的每一条都要解释」。

用法
----
    python tools/pair_logs.py --server E:/project/BeiDou-Server_s9/logs/out.log \
                              --client E:/MXD/BeiDou-Client-xy/packet_dump.log \
                              --report docs/features/s9-protocol-align/evidence/pairing/login.md
    python tools/pair_logs.py --self-test      # 离线自检三种格式与判据（无真实日志时可跑）

stdout 只打 ASCII（本仓库的 shell 按 cp936 解 UTF-8，中文会花），中文报告一律落盘。
"""

import argparse
import json
import os
import re
import sys
import tempfile
from collections import Counter
from dataclasses import dataclass

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT = os.path.dirname(SCRIPT_DIR)
DEFAULT_MATRIX = os.path.join(REPO_ROOT, "docs", "features", "s9-protocol-align",
                              "protocol", "opcode-matrix.json")

# log4j2: %d{yyyy-MM-dd HH:mm:ss.SSS} [%5level] ==> %m%n
PREFIX_RE = re.compile(r"^(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{3}) "
                       r"\[[ A-Z]{5}\] ==> (.*)$")

# Net/Session.cpp packet_dump()。真实行每字节后都带一个空格（"%02x " 循环），
# 这里把行尾空格做成可选，容忍手工整理过的样本行。
CLIENT_RE = re.compile(r"^(?:\[(SEND|RECV)\] )?PacketDump : "
                       r"((?:[0-9a-fA-F]{2}[ ]?)*)\s*$")

# InPacketLogger.java:38-39 / OutPacketLogger.java:39-40
B_RE = re.compile(r"^(?P<unknown><UnknownPacket> )?(?P<dir>ClientSend|ServerSend):"
                  r"(?P<name>\S+) \[(?P<ophex>[0-9A-Fa-f]+)\] \((?P<len>\d+)\) <HEX> (?P<rest>.*)$")
HEX_SPACED_RE = re.compile(r"^(?:[0-9A-Fa-f]{2}(?: [0-9A-Fa-f]{2})*)?$")

# 退化行：InPacketLogger.java:41 打 "{}..."（无空格，阈值 3000），
# OutPacketLogger.java:42 打 "{} ..."（有空格，阈值 50000）⇒ 空格必须可选。
TRUNC_RE = re.compile(r"^(?P<ophex>[0-9A-F]{2} [0-9A-F]{2}) ?\.\.\.$")

# Client.java:216-218 + ByteBufInPacket.java:92-108
A_RE = re.compile(r"^收到封包 包头ID \[0x(?P<ophex>[0-9A-Fa-f]+)\] 内容：\s*"
                  r"ByteBufInPacket\[(?P<hex>[0-9A-Fa-f_]*)\]$")

# MonitoredChrLogger.java:66-76 —— 「!monitor」(gm3) 的**逐角色入站全 hex**日志。
# 它是这几档里唯一给完整字节的：不读 GameConfig ⇒ 免重启、免改库、纯内存 HashSet。
# 行式 "{account}-{char} {opcode十进制}-{hex}"，hex 由 HexTool.toHexString 产出
# （HexTool.java:41-43 = 大写 + 空格分隔），body 为空时打 "<empty>"。
# ⚠ 这一行的 hex **不含 opcode**：packet.getBytes() = ByteBufUtil.getBytes(byteBuf)
#   从 readerIndex 起（ByteBufInPacket.java:18-20），而 Client.java:213 已经
#   readShort() 吃掉那 2 字节 ⇒ 这里按小端把 opcode 补回前缀，才与客户端 dump 同形可比。
MON_RE = re.compile(r"^(?P<account>[^\s]+?)-(?P<char>[^\s]+?) (?P<opid>\d+)-"
                    r"(?P<hex>(?:[0-9A-Fa-f]{2}(?: [0-9A-Fa-f]{2})*|<empty>))$")

TEXT_SEP = " <TEXT> "


def _ascii(s):
    """shell 里只允许 ASCII；非 ASCII 用 backslashreplace 露出码点。"""
    return s.encode("ascii", "backslashreplace").decode("ascii")


def spaced_hex_to_bytes(h):
    h = h.strip()
    if not h:
        return b""
    return bytes.fromhex(h.replace(" ", ""))


@dataclass
class Pkt:
    direction: str          # 'c2s' | 's2c' | '' (unknown)
    source: str             # 'client' | 'server'
    kind: str               # client_dump / server_in(B) / server_out(B) / server_recv_text(A) / server_trunc
    data: bytes
    file: str
    line_no: int
    stamp: str = ""
    name: str = ""          # 日志里服务端自己标的 opcode 名
    declared_len: int = -1  # 档 B 括号里的长度
    reader_pos: int = -1    # 档 A 的 '_' 位置（字节单位）
    text: str = ""          # 档 B 的 <TEXT>
    raw: str = ""

    @property
    def opcode(self):
        # opcode 在正文内小端 2 字节（LoggingUtil.readFirstShort = readShortLE）
        if len(self.data) >= 2:
            return int.from_bytes(self.data[:2], "little")
        return None


def parse_client(path):
    pkts, legacy = [], 0
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        for no, line in enumerate(f, 1):
            m = CLIENT_RE.match(line.rstrip("\r\n"))
            if not m:
                continue
            direction, hexstr = m.group(1), m.group(2)
            if direction is None:
                legacy += 1          # T9 之前的行没有方向标记
            data = spaced_hex_to_bytes(hexstr)
            pkts.append(Pkt("c2s" if direction == "SEND" else
                            "s2c" if direction == "RECV" else "",
                            "client", "client_dump", data, os.path.basename(path),
                            no, raw=line.strip()))
    return pkts, legacy


def _msg_of(line):
    m = PREFIX_RE.match(line.rstrip("\r\n"))
    return (m.group(1), m.group(2)) if m else ("", line.rstrip("\r\n"))


def parse_server(path):
    pkts, unparsed = [], 0
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        for no, line in enumerate(f, 1):
            stamp, msg = _msg_of(line)
            if not msg:
                continue
            mt = TRUNC_RE.match(msg)
            if mt:
                # 退化行没有方向信息，单列一类，不参与配对
                pkts.append(Pkt("", "server", "server_trunc",
                                spaced_hex_to_bytes(mt.group("ophex")),
                                os.path.basename(path), no, stamp,
                                name="(truncated over threshold)", raw=line.strip()))
                continue
            mb = B_RE.match(msg)
            if mb:
                rest = mb.group("rest")
                idx = rest.rfind(TEXT_SEP)
                hexpart, text = (rest[:idx], rest[idx + len(TEXT_SEP):]) \
                    if idx >= 0 else (rest, "")
                if not HEX_SPACED_RE.match(hexpart.strip()):
                    unparsed += 1
                    continue
                direction = "c2s" if mb.group("dir") == "ClientSend" else "s2c"
                kind = "server_in" if direction == "c2s" else "server_out"
                pkts.append(Pkt(direction, "server", kind,
                                spaced_hex_to_bytes(hexpart),
                                os.path.basename(path), no, stamp,
                                name="" if mb.group("unknown") else mb.group("name"),
                                declared_len=int(mb.group("len")),
                                text=text, raw=line.strip()))
                continue
            ma = A_RE.match(msg)
            if ma:
                hexraw = ma.group("hex")
                pos = hexraw.find("_")
                pkts.append(Pkt("c2s", "server", "server_recv_text",
                                bytes.fromhex(hexraw.replace("_", "")),
                                os.path.basename(path), no, stamp,
                                name="(档A)",
                                reader_pos=-1 if pos < 0 else pos // 2,
                                raw=line.strip()))
                continue
            mm = MON_RE.match(msg)
            if mm:
                opid = int(mm.group("opid"))
                body = (b"" if mm.group("hex") == "<empty>"
                        else spaced_hex_to_bytes(mm.group("hex")))
                pkts.append(Pkt("c2s", "server", "server_monitored",
                                opid.to_bytes(2, "little") + body,
                                os.path.basename(path), no, stamp,
                                text="%s/%s" % (mm.group("account"), mm.group("char")),
                                raw=line.strip()))
                continue
            unparsed += 1
    return pkts, unparsed


def dedupe_server_inbound(server_pkts):
    """同一条入站包可能被多档日志各打一行，按相邻字节相同去镜像。

    真实顺序固定：InPacketLogger 的 ClientSend（档 B，pipeline 里 addBefore("Client")）
    → Client.channelRead 的「收到封包」（档 A）→ MonitoredChrLogger 的「account-char id-hex」
    （!monitor，档 F，Client.java:223 在 A 之后）。三者正文相同时只留最早那行，
    因为它带 declared_len 与 <TEXT>，信息量最大。
    """
    kept, dropped = [], 0
    prev_in = None
    for p in server_pkts:
        if p.direction == "c2s":
            mirror = (prev_in is not None and prev_in.data == p.data and
                      ((p.kind == "server_recv_text" and prev_in.kind == "server_in") or
                       (p.kind == "server_monitored" and
                        prev_in.kind in ("server_in", "server_recv_text"))))
            if mirror:
                dropped += 1
                prev_in = p
                continue
            if p.kind != "server_trunc":
                prev_in = p
        kept.append(p)
    return kept, dropped


def load_opcode_names(path):
    """从 T1 生成的矩阵取 value→name，给日志里没名字的包补名。"""
    if not os.path.exists(path):
        return {}, {}
    with open(path, "r", encoding="utf-8") as f:
        d = json.load(f)
    out_names, in_names = {}, {}
    for r in d.get("c2s", []):
        if isinstance(r.get("s9_value"), int):
            out_names[r["s9_value"]] = r.get("s9_name") or out_names.get(r["s9_value"], "")
        if isinstance(r.get("client_value"), int):
            out_names.setdefault(r["client_value"], r.get("client_name") or "")
    for r in d.get("s2c", []):
        if isinstance(r.get("s9_value"), int):
            in_names[r["s9_value"]] = r.get("s9_name") or in_names.get(r["s9_value"], "")
    return out_names, in_names


def first_diff(a, b):
    for i in range(min(len(a), len(b))):
        if a[i] != b[i]:
            return i
    return None if len(a) == len(b) else min(len(a), len(b))


def pair_stream(clients, servers, window=50):
    """带前瞻窗口的顺序匹配；返回 (pairs, unmatched_client, unmatched_server)。

    护栏：客户端 dump 无时间戳，服务端档 B/F 的 PONG 之类高频包会先于内容包被消费，
    纯贪心会「越过」后面仍要用的服务端记录，把本该 EXACT 的内容包误判成未配对
    （2026-10-31 档 F 首份数据里 4 条 WHISPER/MULTI_CHAT 就是这么丢的）。
    扫描遇到 opcode 不匹配、但该 opcode 在后面未处理的客户端行里还会出现时，停止前扫。
    """
    pairs, um_c, um_s = [], [], []
    rem = Counter(c.opcode for c in clients if c.opcode is not None)
    si = 0
    for c in clients:
        if c.opcode is None:
            um_c.append((c, "malformed: <2 bytes"))
            continue
        rem[c.opcode] -= 1
        best = None          # (index, 'EXACT'|'DIFF')
        for j in range(si, min(len(servers), si + window)):
            s = servers[j]
            if s.opcode is None:
                continue
            if s.opcode != c.opcode:
                if rem[s.opcode] > 0:
                    break
                continue
            if s.data == c.data:
                best = (j, "EXACT")
                break
            if best is None:
                best = (j, "DIFF")
        if best:
            j, kind = best
            for k in range(si, j):
                um_s.append((servers[k], "skipped before pair at L%d" % servers[j].line_no))
            d = first_diff(c.data, servers[j].data) if kind == "DIFF" else None
            pairs.append((c, servers[j], kind, d))
            si = j + 1
        else:
            um_c.append((c, "no server record with same opcode in window"))
    for k in range(si, len(servers)):
        um_s.append((servers[k], "left over at end of stream"))
    return pairs, um_c, um_s


def length_check(pairs):
    """档 B 括号里的 declared_len 必须等于客户端正文字节数。"""
    bad = []
    for c, s, kind, d in pairs:
        if s.declared_len >= 0 and s.declared_len != len(c.data):
            bad.append((c, s))
    return bad


def _fmt_pair(idx, c, s, kind, d, names):
    op = c.opcode
    nm = s.name or (names or {}).get(op, "") or "?"
    diff = ""
    if kind == "DIFF":
        diff = " first-diff@byte %d: client=%02X server=%02X len %d/%d" % (
            d, c.data[d] if d < len(c.data) else 0,
            s.data[d] if d < len(s.data) else 0, len(c.data), len(s.data))
    return "| %d | 0x%02X | %s | %s | %d | %d | %s | %s | %s |" % (
        idx, op, nm, kind, len(c.data), len(s.data),
        "C:L%d" % c.line_no,
        "S:L%d %s" % (s.line_no, s.stamp), diff.strip())


TABLE_HEAD = ("| # | opcode | name | match | client len | server len | client line "
              "| server line/time | note |\n"
              "|---|---|---|---|---|---|---|---|---|\n")


def build_report(args, summary, blocks):
    lines = ["# T9 日志配对报告", "",
             "- 客户端：%s" % (args.client or "(未提供)"),
             "- 服务端：%s" % (args.server or "(未提供)"),
             "- 前瞻窗口：%d 包；档 A/B 镜像去重：%d 行" % (args.window, summary["deduped"]),
             "",
             "## 结论", ""]
    for k in ("c2s", "s2c"):
        b = summary[k]
        lines.append("- **%s**: 配对 %d（EXACT %d / DIFF %d），客户端未配对 %d，"
                     "服务端未配对 %d，长度不符 %d" % (
                         k, b["pairs"], b["exact"], b["diff"], b["um_client"],
                         b["um_server"], b["len_bad"]))
    lines.append("- 客户端无方向标记的历史行（T9 之前格式）：%d" % summary["legacy"])
    lines.append("- 服务端超阈值截断行（入站 3000 / 出站 50000，无法配对）：%d" % summary["trunc"])
    lines.append("- 服务端无法识别的行：%d" % summary["unparsed"])
    if summary["notes"]:
        lines.append("")
        lines.append("### 需要注意")
        for n in summary["notes"]:
            lines.append("- %s" % n)
    for title, body in blocks:
        lines += ["", "## %s" % title, "", body]
    return "\n".join(lines) + "\n"


def run(args):
    names_out, names_in = load_opcode_names(args.opcode_matrix)
    summary = {"deduped": 0, "legacy": 0, "trunc": 0, "unparsed": 0, "notes": [],
               "c2s": {}, "s2c": {}}
    blocks = []

    client_pkts = []
    if args.client:
        client_pkts, summary["legacy"] = parse_client(args.client)
    server_pkts, summary["unparsed"] = parse_server(args.server) if args.server else ([], 0)
    server_pkts, summary["deduped"] = dedupe_server_inbound(server_pkts)
    summary["trunc"] = sum(1 for p in server_pkts if p.kind == "server_trunc")

    print(_ascii("[pair_logs] client pkts=%d server pkts=%d (trunc=%d, dedup=%d, unparsed=%d)"
                 % (len(client_pkts), len(server_pkts), summary["trunc"],
                    summary["deduped"], summary["unparsed"])))

    for direction, names in (("c2s", names_out), ("s2c", names_in)):
        cl = [p for p in client_pkts if p.direction == direction]
        sv = [p for p in server_pkts if p.direction == direction]
        if args.client:
            pairs, um_c, um_s = pair_stream(cl, sv, args.window)
        else:
            pairs, um_c, um_s = [], [], [(p, "no client dump supplied") for p in sv]
        lb = length_check(pairs)
        summary[direction] = {
            "pairs": len(pairs),
            "exact": sum(1 for p in pairs if p[2] == "EXACT"),
            "diff": sum(1 for p in pairs if p[2] == "DIFF"),
            "um_client": len(um_c), "um_server": len(um_s), "len_bad": len(lb),
        }
        tbl = TABLE_HEAD
        for i, (c, s, kind, d) in enumerate(pairs, 1):
            tbl += _fmt_pair(i, c, s, kind, d, names) + "\n"
        blocks.append(("%s 配对明细" % direction, tbl))

        if um_c:
            t = "| client line | opcode | name | len | hex | 原因 |\n|---|---|---|---|---|---|\n"
            for c, why in um_c:
                t += "| L%d | %s | %s | %d | %s | %s |\n" % (
                    c.line_no, "0x%02X" % c.opcode if c.opcode is not None else "?",
                    names.get(c.opcode, "") if c.opcode is not None else "",
                    len(c.data), c.data.hex(" ").upper(), why)
            blocks.append(("%s 客户端未配对" % direction, t))
        if um_s:
            t = "| server line | time | kind | opcode | name | len | 原因 |\n|---|---|---|---|---|---|---|\n"
            for s, why in um_s:
                t += "| L%d | %s | %s | %s | %s | %d | %s |\n" % (
                    s.line_no, s.stamp, s.kind,
                    "0x%02X" % s.opcode if s.opcode is not None else "?",
                    s.name or (names.get(s.opcode, "") if s.opcode is not None else ""),
                    len(s.data), why)
            blocks.append(("%s 服务端未配对（每一条都要给解释）" % direction, t))
        if lb:
            t = "| client line | server line | opcode | client len | declared len |\n" \
                "|---|---|---|---|---|\n"
            for c, s in lb:
                t += "| L%d | L%d | 0x%02X | %d | %d |\n" % (
                    c.line_no, s.line_no, c.opcode, len(c.data), s.declared_len)
            blocks.append(("%s 长度不一致（档 B 括号值 vs 客户端正文字节数）" % direction, t))

    if summary["legacy"]:
        summary["notes"].append(
            "客户端有 %d 行是 T9 之前的 packet_dump 格式（没有 [SEND]/[RECV] 标记），"
            "无法判方向，已全部排除配对；请用重新构建后的客户端重跑会话。" % summary["legacy"])
    if summary["trunc"]:
        summary["notes"].append(
            "服务端有 %d 行正文超阈值被截断成前 2 字节（入站阈值 3000、出站 50000），"
            "只可能核对 opcode，不可能逐字节配对。" % summary["trunc"])
    if not args.client:
        summary["notes"].append("本次未提供客户端 packet_dump.log，只做了服务端日志分类。")
    if summary["c2s"].get("um_server", 0) or summary["s2c"].get("um_server", 0):
        summary["notes"].append(
            "存在服务端未配对记录：常见原因是档 A 过滤掉 7 个高频 opcode、日志开关中途"
            "切换、客户端 dump 晚于会话启动，或客户端根本没发过该包（服务端自发）。")

    report = build_report(args, summary, blocks)
    if args.report:
        os.makedirs(os.path.dirname(os.path.abspath(args.report)), exist_ok=True)
        with open(args.report, "w", encoding="utf-8") as f:
            f.write(report)
        print(_ascii("[pair_logs] report -> %s" % args.report))
    else:
        sys.stdout.write(_ascii(report))

    total_pairs = summary["c2s"]["pairs"] + summary["s2c"]["pairs"]
    total_diff = summary["c2s"]["diff"] + summary["s2c"]["diff"]
    total_lenbad = summary["c2s"]["len_bad"] + summary["s2c"]["len_bad"]
    print(_ascii("[pair_logs] pairs=%d diff=%d len_mismatch=%d um_client=%d um_server=%d"
                 % (total_pairs, total_diff, total_lenbad,
                    summary["c2s"]["um_client"] + summary["s2c"]["um_client"],
                    summary["c2s"]["um_server"] + summary["s2c"]["um_server"])))
    return 0 if (total_lenbad == 0 and (not args.strict or total_diff == 0)) else 1


# --------------------------------------------------------------------------
# self test：三种格式各造样例行，钉住解析器与判据（真实配对日志要等 T7/档 B）
# --------------------------------------------------------------------------

ST_CLIENT = """\
[SEND] PacketDump : 01 00 00 00 61 61 61 00 00 00 00 00
[RECV] PacketDump : 00 00 04 00 00 00 64
PacketDump : de ad be ef
[SEND] PacketDump : 0c 00 11 22
[RECV] PacketDump : 18 00
[SEND] PacketDump : 18 00 99
"""

# 顺序按真实管线排：InPacketLogger 的 ClientSend 先出，Client.channelRead 的
# "收到封包" 后出；MOVE_LIFE 那条专门用来暴露「档 A 过滤 7 个高频 opcode」。
ST_SERVER = """\
2026-10-30 19:20:00.123 [ INFO] ==> ClientSend:LOGIN_PASSWORD [1] (12) <HEX> 01 00 00 00 61 61 61 00 00 00 00 00 <TEXT> ....aaa.....
2026-10-30 19:20:00.123 [ INFO] ==> 收到封包 包头ID [0x1] 内容：ByteBufInPacket[0100_00006161610000000000]
2026-10-30 19:20:00.124 [ INFO] ==> ServerSend:LOGIN_STATUS [0] (7) <HEX> 00 00 04 00 00 00 64 <TEXT> .......
2026-10-30 19:20:00.125 [ INFO] ==> ClientSend:PONG [18] (3) <HEX> 18 00 AA <TEXT> ...
2026-10-30 19:20:00.125 [ INFO] ==> 收到封包 包头ID [0x18] 内容：ByteBufInPacket[1800_]
2026-10-30 19:20:00.126 [ INFO] ==> <UnknownPacket> ServerSend:null [FFAC] (4) <HEX> AC FF 00 00 <TEXT> ....
2026-10-30 19:20:00.127 [ INFO] ==> AB CD ...
2026-10-30 19:20:00.128 [ INFO] ==> ClientSend:MOVE_LIFE [BC] (4) <HEX> BC 00 01 02 <TEXT> ....
2026-10-30 19:20:00.129 [ WARN] ==> 这条不属于封包日志，应计入 unparsed
"""

# MonitoredChrLogger.java:66-76 的真实行式（hex 大写空格分隔、opcode 是十进制、
# 且 hex 不含 opcode —— ByteBufInPacket.getBytes() 从 readerIndex 起）。
# 第二行是 body 为空的包（如 PONG），日志里打 "<empty>"。
ST_MON = """\
2026-10-30 19:20:00.130 [ INFO] ==> admin-Admin 1-00 00 61 61 61 00 00 00 00 00
2026-10-30 19:20:00.131 [ INFO] ==> admin-Admin 24-<empty>
2026-10-30 19:20:00.132 [ INFO] ==> admin-Admin 207-<empty>
"""


def self_test():
    """断言解析与配对判据；任何一条不符即 exit(1) 并打印定位。"""
    fails = []

    def ck(cond, label):
        if not cond:
            fails.append(label)
        print(_ascii("  [%s] %s" % ("ok" if cond else "FAIL", label)))

    with tempfile.TemporaryDirectory() as td:
        cpath = os.path.join(td, "packet_dump.log")
        spath = os.path.join(td, "out.log")
        with open(cpath, "w", encoding="utf-8") as f:
            f.write(ST_CLIENT)
        with open(spath, "w", encoding="utf-8") as f:
            f.write(ST_SERVER)

        client, legacy = parse_client(cpath)
        server, unparsed = parse_server(spath)
        server, deduped = dedupe_server_inbound(server)

        print(_ascii("[self-test] client=%d server=%d dedup=%d unparsed=%d legacy=%d"
                     % (len(client), len(server), deduped, unparsed, legacy)))

        ck(len(client) == 6, "client dump lines parsed = 6 (SEND/RECV/bare all read)")
        ck(legacy == 1, "1 bare 'PacketDump :' line flagged as legacy (no direction)")
        ck([p.direction for p in client] == ["c2s", "s2c", "", "c2s", "s2c", "c2s"],
           "direction map: SEND=c2s, RECV=s2c, bare=''")
        ck(client[0].opcode == 1, "client opcode read little-endian from body[:2]")

        kinds = [p.kind for p in server]
        ck(kinds == ["server_in", "server_out", "server_in", "server_recv_text",
                     "server_out", "server_trunc", "server_in"],
           "server kinds after dedupe: %s" % kinds)
        ck(deduped == 1, "ClientSend + 收到封包 mirror of identical bytes deduped once")
        ck(len(server) == 7, "server records after dedupe = 7")
        ck(unparsed == 1, "non-packet line counted, not silently dropped (unparsed=%d)" % unparsed)
        ck(sum(1 for p in server if p.kind == "server_trunc") == 1,
           "'AB CD ...' classified as truncated (no direction)")
        # 两个 logger 的退化行格式不一样：入站 "{}..." 无空格，出站 "{} ..." 有空格。
        ck(TRUNC_RE.match("AB CD...") and TRUNC_RE.match("AB CD ..."),
           "both truncation spellings matched (InPacketLogger.java:41 no-space, "
           "OutPacketLogger.java:42 space)")
        ck(not TRUNC_RE.match("北斗 v83 正在启动..."),
           "startup banner must NOT be mistaken for a truncated packet")
        ck(any(p.name == "" and "AC FF" in p.data.hex(" ").upper() for p in server),
           "<UnknownPacket> prefix -> name cleared")
        a = [p for p in server if p.kind == "server_recv_text"]
        ck(len(a) == 1 and a[0].reader_pos == 2 and a[0].data == bytes.fromhex("1800"),
           "surviving A-class line: '_' stripped -> reader_pos=2 (opcode consumed at "
           "Client.java:214), contiguous uppercase hex decoded; got %s"
           % [(p.reader_pos, p.data.hex()) for p in a])
        # 另一条 档A 行（12 字节登录包）已被镜像去重；这里直接验证它解出的字节
        # 与 ClientSend 行完全相同 —— 那正是去重的依据。
        ck(bytes.fromhex("0100_00006161610000000000".replace("_", "")) == client[0].data,
           "A-class 12-byte login line decodes identical to ClientSend line (dedupe basis)")

        cl_in = [p for p in client if p.direction == "c2s"]
        sv_in = [p for p in server if p.direction == "c2s"]
        pairs, um_c, um_s = pair_stream(cl_in, sv_in, 50)
        ck(len(pairs) == 2, "c2s pairs=%d (expect 2)" % len(pairs))
        ck([k for _, _, k, _ in pairs] == ["EXACT", "DIFF"],
           "c2s match kinds = EXACT then DIFF (got %s)" % [k for _, _, k, _ in pairs])
        ck(pairs[1][3] == 2, "DIFF first-diff byte offset = 2 (18 00 99 vs 18 00 AA)")
        ck(len(um_c) == 1 and um_c[0][0].opcode == 12,
           "c2s unmatched client = op 0x0C only")
        ck(sorted(p.opcode for p, _ in um_s) == [24, 188],
           "c2s unmatched server = 档A leftover(0x18) + MOVE_LIFE(188) : %s"
           % sorted(p.opcode for p, _ in um_s))
        ck(length_check(pairs) == [], "no declared_len vs client-length violation in c2s pairs")

        cl_out = [p for p in client if p.direction == "s2c"]
        sv_out = [p for p in server if p.direction == "s2c"]
        pairs2, um_c2, um_s2 = pair_stream(cl_out, sv_out, 50)
        ck(len(pairs2) == 1 and pairs2[0][2] == "EXACT",
           "s2c: 00 00 04 00 00 00 64 matches ServerSend:LOGIN_STATUS byte-exactly")
        ck(len(um_c2) == 1 and len(um_s2) == 1,
           "s2c unmatched = 1 client (op 0x18) + 1 server (<UnknownPacket>)")

        lb = length_check([(cl_in[0], Pkt("c2s", "server", "server_in",
                                         cl_in[0].data, "x", 9, declared_len=99),
                             "EXACT", None)])
        ck(len(lb) == 1, "length_check flags declared_len(99) != client body(12)")

        # ---- 档 F（!monitor，MonitoredChrLogger.java:66-76）：单独一份文件，
        #      不去动上面钉死的 server=7 计数 ----
        mpath = os.path.join(td, "out-monitor.log")
        with open(mpath, "w", encoding="utf-8") as f:
            f.write(ST_MON)
        mon, mon_unparsed = parse_server(mpath)
        ck(len(mon) == 3 and mon_unparsed == 0 and
           all(p.kind == "server_monitored" for p in mon),
           "档 F: 3 records, kind=server_monitored, unparsed=0 (got n=%d unparsed=%d)"
           % (len(mon), mon_unparsed))
        ck([p.opcode for p in mon] == [1, 24, 207],
           "档 F opcode comes from the decimal field (got %s)" % [p.opcode for p in mon])
        ck(mon[0].data == client[0].data,
           "档 F hex has NO opcode (getBytes() starts at readerIndex) -> parser restores the "
           "LE 2-byte prefix and lands byte-identical to the client SEND dump")
        ck(mon[1].data == bytes.fromhex("1800"), "档 F '<empty>' body -> opcode-only record")
        ck(all(p.direction == "c2s" for p in mon), "档 F is inbound-only")
        ck([p.text for p in mon] == ["admin/Admin"] * 3,
           "档 F keeps account/char in .text so a mixed-character log is visible")
        mpairs, _, _ = pair_stream([client[0]], [mon[0]], 50)
        ck(len(mpairs) == 1 and mpairs[0][2] == "EXACT",
           "档 F pairs EXACT against the client login SEND")
        b_login = [p for p in server if p.kind == "server_in" and p.opcode == 1]
        a_login = [p for p in server if p.kind == "server_recv_text"]
        ck(len(b_login) == 1 and len(a_login) == 1, "fixtures for the mirror test exist")
        for prev, tag in ((b_login[0], "B->E"), (a_login[0], "A->E")):
            kept_x, drop_x = dedupe_server_inbound(
                [prev] + [p for p in mon if p.opcode == prev.opcode][:1])
            ck(drop_x == 1 and not any(p.kind == "server_monitored" for p in kept_x),
               "%s mirror collapses into the earlier, richer record (dropped=%d)" % (tag, drop_x))
        ee, ee_drop = dedupe_server_inbound([mon[1], mon[1]])
        ck(ee_drop == 0 and len(ee) == 2,
           "two adjacent identical 档 F lines are TWO real packets, must not collapse "
           "(dropped=%d)" % ee_drop)

        ns = argparse.Namespace(client=cpath, server=spath,
                                report=os.path.join(td, "rep.md"), window=50,
                                opcode_matrix=DEFAULT_MATRIX, strict=False)
        ck(run(ns) == 0, "run() exits 0: DIFF alone is not a failure")
        ns.strict = True
        ck(run(ns) == 1, "--strict makes the DIFF case fail the run")

    if fails:
        print(_ascii("[self-test] FAILED: " + "; ".join(fails)))
        return 1
    print(_ascii("[self-test] all assertions passed"))
    return 0


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--server", help="服务端 logs/out.log（含档 A 或档 B 封包行）")
    ap.add_argument("--client", help="客户端 packet_dump.log（exe 同目录）")
    ap.add_argument("--report", help="把 markdown 报告写到这里（UTF-8）")
    ap.add_argument("--window", type=int, default=50,
                    help="前瞻窗口（包数）：档 A 过滤/开关切换造成的错位靠它吸收")
    ap.add_argument("--opcode-matrix", default=DEFAULT_MATRIX,
                    help="T1 生成的 opcode-matrix.json，用于补 opcode 名")
    ap.add_argument("--strict", action="store_true",
                    help="DIFF 也算失败（默认只把长度不符当失败）")
    ap.add_argument("--self-test", action="store_true", dest="selftest",
                    help="跑内置样例，验证三种日志格式的解析器")
    args = ap.parse_args()
    if args.selftest:
        return self_test()
    if not args.server and not args.client:
        ap.error("need --server and/or --client (or --self-test)")
    return run(args)


if __name__ == "__main__":
    sys.exit(main())
