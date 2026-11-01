#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
T8 · 在线协议探针（对真实运行的 s9 服务端发封包，验证 T4 的 oracle 不是纸面对账）

为什么要有这个工具
------------------
T4 的 transport_oracle.py 只是「把两侧源码各自转写成 Python 再互比」，
两边都错得一样的话对拍照样全绿。探针把同一套变换直接对着**正在运行的 s9** 跑：
头部被接受、IV 链不漂移、服务端自己发来的包能被我们解开 —— 这才叫裁决。

安全红线（写在代码里，命令行绕不过去）
--------------------------------------
1. 只发两类包：
   a) s9 侧**没有注册 handler** 的 recv opcode —— Client.channelRead() 的 null 分支
      只做 updateLastPacket()（Client.java:207-236），对账号/角色/世界/DB 零影响；
   b) opcode_allowlist.txt 里点名且已人工读源码确认零副作用的（当前只有 PONG）。
2. 出现在 opcode_blocklist.txt 的值一律不发，即使是 a) 类。
3. 两个清单里的「名字 ↔ 值」都现场用 gen_opcode_matrix 的解析器核对 s9 源码，
   对不上直接 ABORT —— 防止手抄错值把危险包放出去。
4. 不做登录、不建 session、不碰数据库。探针连接活过 30s 就会被服务端 getPing() 探测，
   收到 PING 后回 PONG 只是写一个 volatile long。

用法
----
    python tools/probe/s9_probe.py --dry-run                 # 只验门禁与发送计划，不联网
    python tools/probe/s9_probe.py                           # 完整探针（含等 PING，约 70s）
    python tools/probe/s9_probe.py --no-ping                 # 只测 c2s，不等 PING
    python tools/probe/s9_probe.py --ping-rounds 1 --length-probe --negative-control  # 全套，约 180s

退出码 = 未通过**门禁断言**条数（0 = 探针被服务端接受且 s2c 能解密）。
negative control 的结论走 rep.measure()：它推翻假设是产出，不是探针失败。

2026-10-30 实测结论（详见 notes/probe.md，原始数据 evidence/probe/probe-final.*）
------------------------------------------------------------------------------
1. hello 16 字节与 oracle s9_get_hello() 逐字节一致，'Frz'/'R0x' 前缀位置如 F1 所述。
2. s2c 打通：服务端 PING 用 recvCypher 解出 opcode=0x0011，头部过 isValidHeader，
   长度公式两侧相等（版本抵消）。
3. c2s 打通（硬证据）：回 PONG 后活过 checkIfIdle 的 15s 复查窗口 ⇒ 我们的包被
   解密 + 派发 + 执行到 KeepAliveHandler。
4. 包长 2..40000（含 32768/40000 两个 D8 临界值）扫完之后派发仍正常 ⇒ 服务端长度
   公式覆盖 16 位全域，不破坏帧同步；D8 纯客户端缺陷。
5. **坏头部不会断线，且会永久废掉这条连接的入站派发**（后续 PONG 不再被处理，
   最后由 pong 超时踢掉）。所以「连接还活着」永远不能当作「包被接受」的证据 ——
   判定一律走 PONG ack oracle。
"""

import argparse
import datetime as dt
import json
import os
import re
import select
import socket
import struct
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.abspath(os.path.join(HERE, ".."))
CLIENT_ROOT = os.path.abspath(os.path.join(TOOLS, ".."))
sys.path.insert(0, TOOLS)

# 唯一实现来源：探针绝不自己重写一份编解码，否则「探针通过」不等于「oracle 正确」。
import transport_oracle as O          # noqa: E402
import gen_opcode_matrix as G         # noqa: E402

F_BLOCK = os.path.join(HERE, "opcode_blocklist.txt")
F_ALLOW = os.path.join(HERE, "opcode_allowlist.txt")
EVIDENCE_DIR = os.path.join(CLIENT_ROOT, "docs", "features", "s9-protocol-align",
                            "evidence", "probe")

PING_OPCODE = 0x0011      # SendOpcode.PING(0x11) -> PacketCreator.getPing()
PONG_OPCODE = 0x0018      # RecvOpcode.PONG(0x18)  -> KeepAliveHandler（白名单）
HELLO_LEN = 16            # PacketCreator.getHello():619-629
HELLO_TAIL_OPC = 0x0E     # hello 正文里自带 opcode（未加密，直接由 socketChannel 写出）

# 默认探测目标里，除白名单外，这些是「s9 定义了常量但没注册 handler」的值。
# 由 resolve_targets() 现场算出，这里只固定顺序偏好（小值先跑，出问题好定位）。
LIST_PAT = re.compile(r"^\s*(0x[0-9A-Fa-f]{2,6})\s+(\*|[A-Z][A-Z0-9_]*)", re.S)


# --------------------------------------------------------------------------
# 门禁：清单解析 + 与 s9 源码交叉核对
# --------------------------------------------------------------------------
def load_list(path):
    """读 '0xNNNN  NAME  # 说明' 形式的清单，返回 {value: name}。"""
    out = {}
    if not os.path.exists(path):
        return out
    for raw in open(path, "r", encoding="utf-8").read().splitlines():
        line = raw.split("#", 1)[0]
        m = LIST_PAT.match(line)
        if m:
            out[int(m.group(1), 16)] = m.group(2)
    return out


def s9_recv_tables():
    """{name: value} 与「有有效 handler 的值集合」。

    s9 用 handlers[code.getValue()] = handler 数组覆盖写（PacketProcessor.java:100-105），
    所以判定「有没有 handler」必须按**最终生效值**，不能看常量名。"""
    recv = G.parse_java_enum(G.F_S9_RECV, "RecvOpcode")
    regs = G.parse_registrations(G.F_S9_PROC)
    name2val = {e["name"]: e["value"] for e in recv}
    handled = set()
    handler_of = {}
    for name, info in regs.items():
        if name in name2val:
            v = name2val[name]
            handled.add(v)
            handler_of[v] = info["handler"]
    return name2val, handled, handler_of


def verify_lists(name2val, handled, block, allow):
    """清单里的名字/值必须与源码一致；名字有 handler 却出现在黑名单外要额外警告。"""
    errs = []
    for path, lst in (("blocklist", block), ("allowlist", allow)):
        for val, name in lst.items():
            if name == "*":
                # 只按值否决：s9 可能根本没有这个常量（例如客户端独有的 LOGIN_EMAIL）。
                # 名字校验跳过，但值本身必须是合法 16 位 opcode。
                if not (0 <= val <= 0xFFFF):
                    errs.append("%s: 0x%04X 超出 16 位 opcode" % (path, val))
                continue
            if name not in name2val:
                errs.append("%s: %s 不是 s9 RecvOpcode 常量（清单可能过期/抄错）" % (path, name))
            elif name2val[name] != val:
                errs.append("%s: %s 值不符 清单=0x%04X 源码=0x%04X"
                            % (path, name, val, name2val[name]))
    for val, name in allow.items():
        if val not in handled:
            errs.append("allowlist: 0x%04X %s 在 s9 并无 handler，白名单条目已无必要"
                        % (val, name))
    return errs


def resolve_targets(name2val, handled, handler_of, block, allow, override):
    """算出本次允许发送的 opcode。

    结构安全性来自「无 handler ⇒ 服务端只 updateLastPacket()」，
    白名单是唯一被豁免的例外（已逐条读源码确认零副作用）。"""
    val2names = {}
    for n, v in name2val.items():
        val2names.setdefault(v, []).append(n)

    def label(v):
        names = val2names.get(v)
        return ",".join(names) if names else "<s9 未定义>"

    if override:
        cand = [int(x, 16) for x in override.split(",")]
    else:
        cand = sorted(v for v in name2val.values() if v not in handled)
        cand += sorted(allow)

    kept, skipped, seen = [], [], set()
    for v in cand:
        if v in seen:
            continue
        seen.add(v)
        nm = label(v)
        if not (0 <= v <= 0xFFFF):
            skipped.append((v, nm, "超出 16 位 opcode"))
        elif v in block:
            skipped.append((v, nm, "blocklist"))
        elif v in allow:
            kept.append((v, nm, "allowlist:" + handler_of.get(v, "?")))
        elif v in handled:
            skipped.append((v, nm, "有 handler(%s) 且不在白名单" % handler_of.get(v, "?")))
        else:
            kept.append((v, nm, "no-handler"))
    return kept, skipped


# --------------------------------------------------------------------------
# 传输层（全部走 O.s9_*，与 oracle 同一份实现）
# --------------------------------------------------------------------------
class ProbeTransport:
    """以客户端身份与 s9 对话。

    hello 里的两个 IV 语义（GMSV83PacketProtocol.java:74-76 + ClientCyphers.java:14-18）：
      recvIv -> 服务端 receiveCypher 用，即**我们发出**的包由它解密 -> 我们的 out IV
      sendIv -> 服务端 sendCypher  用，即**我们收到**的包由它加密   -> 我们的 in  IV
    版本常量同理：服务端解我们的包用 RecvOpcode 侧版本 swap16(83)=0x5300，
    服务端发包用 (0xFFFF-83) 的 swap16=0xACFF。客户端 create_header 只写裸 83
    是因为 0x5300 低字节为 0，两个公式等价（T4 已字节级证实）。"""

    def __init__(self, send_iv, recv_iv):
        self.iv_out = list(recv_iv)
        self.iv_in = list(send_iv)
        self.sent = 0
        self.recvd = 0

    def pack(self, body):
        hdr = bytes(O.s9_get_packet_header(self.iv_out, O.S9_RECV_SWAPPED, len(body)))
        enc = bytearray(O.s9_encrypt_data(bytes(body)))
        enc = O.s9_crypt(enc, self.iv_out, O.ecb)     # 就地推进 iv_out
        self.sent += 1
        return hdr + enc

    def unpack(self, hdr4, body):
        hbe = struct.unpack(">I", hdr4)[0]            # 服务端 in.readInt() 是 BE
        ok = O.s9_is_valid_header(hbe, self.iv_in, O.S9_SEND_SWAPPED)
        length_s9 = O.s9_decode_packet_length(hbe)
        dec = O.s9_crypt(bytearray(bytes(body)), self.iv_in, O.ecb)   # 就地推进 iv_in
        plain = O.s9_decrypt_data(dec)
        self.recvd += 1
        opcode = struct.unpack("<H", plain[:2])[0] if len(plain) >= 2 else None
        # 顺带记录「真实 C++ 客户端会把这条包头解成什么长度」，用于 D8 现场判定
        return {
            "hdr": hdr4,
            "hdr_hex": " ".join("%02x" % b for b in hdr4),
            "header_ok": ok,
            "length_s9": length_s9,
            "length_client_buggy": O.cl_check_length(hdr4, sign_extend=True),
            "length_client_fixed": O.cl_check_length(hdr4, sign_extend=False),
            "opcode": opcode,
            "plain": plain,
        }


# --------------------------------------------------------------------------
# socket 辅助
# --------------------------------------------------------------------------
class Closed(Exception):
    pass


def recv_exact(sock, n, deadline):
    buf = b""
    while len(buf) < n:
        left = deadline - time.monotonic()
        if left <= 0:
            raise TimeoutError("需要 %d 字节，只收到 %d" % (n, len(buf)))
        r, _, _ = select.select([sock], [], [], min(left, 1.0))
        if not r:
            continue
        chunk = sock.recv(n - len(buf))
        if not chunk:
            raise Closed("服务端在收 %d 字节前关闭了连接" % n)
        buf += chunk
    return buf


def poll_inbound(sock, tr, sec):
    """非阻塞式探测：在 sec 秒内若有完整包到达则解出来。

    先用头部算长度、收到完整 body 后才 unpack —— unpack 会推进 in IV，
    半包/重复解都会让 IV 链错位，那样后续「服务端包解不开」就成了我们自己的假阳性。"""
    out = []
    end = time.monotonic() + sec
    while time.monotonic() < end:
        r, _, _ = select.select([sock], [], [], 0.05)
        if not r:
            continue
        head = recv_exact(sock, 4, end + 5)
        n = O.s9_decode_packet_length(struct.unpack(">I", head)[0])
        body = recv_exact(sock, n, end + 5) if n else b""
        out.append(tr.unpack(head, body))
    return out


def recv_one(sock, tr, deadline):
    """阻塞等一个完整的服务端包（含 body）。返回 (解析结果, 原始字节)。"""
    head = recv_exact(sock, 4, deadline)
    n = O.s9_decode_packet_length(struct.unpack(">I", head)[0])
    body = recv_exact(sock, n, deadline + 10) if n else b""
    return tr.unpack(head, body), head + body


def wait_and_drain(sock, tr, sec):
    """在 sec 秒内持续读并把到达的包都解出来；返回 (是否还活着, 包列表)。

    必须真的把包解掉：只用 select 看一眼就 recv 丢弃的话 in IV 不推进，
    之后「服务端包解不开」会变成我们自己的假阳性。"""
    got = []
    end = time.monotonic() + sec
    while time.monotonic() < end:
        r, _, _ = select.select([sock], [], [], 0.25)
        if not r:
            continue
        try:
            head = sock.recv(4)
        except OSError:
            return False, got
        if head == b"":
            return False, got
        if len(head) < 4:
            head += recv_exact(sock, 4 - len(head), end + 10)
        n = O.s9_decode_packet_length(struct.unpack(">I", head)[0])
        body = recv_exact(sock, n, end + 10) if n else b""
        got.append(tr.unpack(head, body))
    return True, got


def pack_without_advancing(tr, body, corrupt_first_byte=False):
    """按「这个包不会被服务端消费」的方式造包：不动 tr.iv_out。

    s9 decode() 的顺序是 readInt → isValidHeader（不过就抛）→ 算长 → readBytes → crypt，
    而 updateIv 只发生在 crypt 里（MapleAESOFB.java:137-139）。
    所以被头校验拒掉的包**不应推进任何一侧的 IV**；要验证这条就得先按这个假设造包，
    再看后续包能不能被正常处理。"""
    iv = list(tr.iv_out)
    hdr = bytearray(O.s9_get_packet_header(iv, O.S9_RECV_SWAPPED, len(body)))
    hdr_ok = bytes(hdr)
    if corrupt_first_byte:
        hdr[0] ^= 0xFF
    enc = O.s9_crypt(bytearray(O.s9_encrypt_data(body)), list(iv), O.ecb)
    return bytes(hdr) + enc, hdr_ok


def ping_ack_round(sock, tr, rep, args, tag, on_packet=None, ack_is_gate=True):
    """唯一的「c2s 包真被处理」oracle：等 PING → 回 PONG → 看能否活过 15s 复查窗口。

    Client.checkIfIdle()（Client.java:1183-1201）发 PING 后 15s 复查 lastPong，
    lastPong < pingedAt 就 closeMapleSession() → finally closeSession() → ioChannel.close()。
    而 KeepAliveHandler 只做 c.pongReceived()（写一个 volatile long）。
    ⇒ 活过 ack_wait 秒 ⟺ 我们的 PONG 被解密、被派发、被执行（三件事缺一不可）。
    ⇒ 反过来，"没被踢" 永远不能当成"被接受"：见 negative_control 的实测结论。

    返回 True=派发成功；False=这条连接的入站派发已经废了（或被踢/或解不开）。
    """
    try:
        rep.log("# [%s] 等服务端 PING（最长 %.0fs）..." % (tag, args.ping_wait))
        n, raw = recv_one(sock, tr, time.monotonic() + args.ping_wait)
    except (Closed, TimeoutError, struct.error) as e:
        rep.check(False, "[%s] 等 PING 失败：连接关闭或解包出错" % tag, str(e))
        return False
    if on_packet:
        on_packet(raw, n)
    rep.log("# [%s]   hdr=%s  len(正确)=%d  len(客户端现状int16_t)=%d  opcode=%s  plain=%s"
            % (tag, n["hdr_hex"], n["length_s9"], n["length_client_buggy"],
               "0x%04X" % n["opcode"] if n["opcode"] is not None else "-",
               hexs(n["plain"])))
    rep.check(n["header_ok"], "[%s] PING 头部通过 sendCypher isValidHeader" % tag,
              "iv_in=%s" % hexs(bytes(tr.iv_in), mask_idx=(3,)))
    rep.check(n["length_s9"] == n["length_client_fixed"],
              "[%s] 长度：服务端公式 == 客户端公式（版本抵消）" % tag)
    if not rep.check(n["opcode"] == PING_OPCODE,
                     "[%s] 解密后 opcode == 0x0011（s2c 端到端打通）" % tag,
                     "got %s" % (hex(n["opcode"]) if n["opcode"] is not None else "-")):
        return False
    try:
        sock.sendall(tr.pack(struct.pack("<H", PONG_OPCODE)))
        rep.log("# [%s]   已回 PONG，观察 %.0fs 内是否被空闲踢掉" % (tag, args.ack_wait))
    except OSError as e:
        rep.check(False, "[%s] 发 PONG 失败" % tag, str(e))
        return False
    ack_alive, extra = wait_and_drain(sock, tr, args.ack_wait)
    for x in extra:
        if on_packet:
            on_packet(x["hdr"] + bytes(x["plain"]), x)
        rep.log("# [%s]   ack 窗口内又收到 opcode=%s len=%d"
                % (tag, "0x%04X" % x["opcode"] if x["opcode"] is not None else "-",
                   x["length_s9"]))
    ack = rep.check if ack_is_gate else rep.measure
    return ack(
        ack_alive,
        "[%s] 回 PONG 后活过 %.0fs => PONG 被解密并派发到 KeepAliveHandler" % (tag, args.ack_wait),
        "c2s 真被处理的硬证据；未被踢不等于被处理")


# --------------------------------------------------------------------------
# 取证输出（IV 随机字节脱敏）
# --------------------------------------------------------------------------
def hexs(b, mask_idx=()):
    return " ".join("**" if i in mask_idx else "%02x" % v
                    for i, v in enumerate(bytearray(b)))


class Report:
    def __init__(self):
        self.lines = []
        self.checks = []      # (ok, name, detail)  ← 门禁：不符合预期就是缺陷
        self.measures = []    # (ok, name, detail)  ← 实测结论：打印但不计入退出码

    def log(self, s=""):
        # 控制台在 cp936 下会把 UTF-8 中文显示成乱码，故 stdout 走 ASCII 转义，
        # 完整中文只保留在 .txt 证据文件里。
        self.lines.append(s)
        print(s.encode("ascii", "backslashreplace").decode("ascii"))

    def check(self, ok, name, detail=""):
        self.checks.append((bool(ok), name, detail))
        self.log("%s  %s%s" % ("[OK  ]" if ok else "[FAIL]", name,
                                ("  " + detail) if detail else ""))
        return ok

    def measure(self, ok, name, detail=""):
        """记录一条「服务端就是这样」的实测结论，即便它违反我们的直觉也不算探针失败。

        negative control 的价值恰恰在于它推翻了「不断线 = 接受了包」这个假设；
        把它记成 FAIL 会让退出码永远是红字，反而掩盖真正的缺陷。
        """
        self.measures.append((bool(ok), name, detail))
        self.log("%s  %s%s" % ("[MEAS]" if ok else "[MEAS]", name,
                                ("  " + detail) if detail else ""))
        return ok

    def dump(self, path):
        text = "\n".join(self.lines) + "\n"
        with open(path, "w", encoding="utf-8", newline="\n") as f:
            f.write(text)


# --------------------------------------------------------------------------
# 可证伪性对照（negative control）
# --------------------------------------------------------------------------
def negative_control(args, rep, targets):
    """故意把头部版本号打坏，看 s9 到底怎么反应 —— 并给出真正的「包被接受」oracle。

    为什么必须做：前面「连发 N 包后连接仍存活」只有在服务端会拒绝错误头时才是证据；
    如果服务端对坏头既不断线也不再处理任何包，那「连接活着」毫无信息量。

    真正的 oracle 是 PONG 的可观测副作用：
      Client.checkIfIdle() 发 PING 后 15s 复查 lastPong，没被 PONG 刷新就
      closeMapleSession() → finally closeSession() → ioChannel.close()
      （Client.java:1183-1201 / 265-281 / 291-293）。
      KeepAliveHandler 是 handler 里最干净的一个：只 c.pongReceived() 写一个 volatile long。
    于是「收到 PING 后回 PONG，再活过 ack_wait 秒」= 服务端确实把我们的包
    解密 + 派发 + 执行到了 handler，这是双向的硬证据，而不只是没被踢。

    安全性：只打坏**头部**，绝不打坏正文 —— 正文首两字节就是 opcode，打坏它可能命中
    172 个已注册 handler 里的任意一个（其中大量会写库/改世界）；而坏头在
    GMSV83PacketProtocol.java:33-35 的 isValidHeader 就被拒，走不到派发。

    2026-10-30 实测：坏头既不断线、又让后续包彻底不再派发。原因是 netty 4.1.109 的
    ByteToMessageDecoder.channelRead 把 decode() 抛出的 Exception 重新包成
    DecoderException（见 evidence/probe/netty-ByteToMessageDecoder-channelRead.txt），
    于是 Client.java:253 的 `cause instanceof InvalidPacketHeaderException` 恒为 false，
    既不 closeSession 也不打日志；ReplayingDecoder 又不推进 readerIndex ⇒ 坏头一直卡在
    解码缓冲头部，之后每次读都重新抛，入站派发永久废掉，最后只能被 pong 超时踢掉。
    """
    dead = [v for v, nm, w in targets if not w.startswith("allowlist")]
    probe_v = dead[0] if dead else PONG_OPCODE
    body = struct.pack("<H", probe_v)

    sock = socket.create_connection((args.host, args.port), timeout=10)
    sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    try:
        hello = recv_exact(sock, HELLO_LEN, time.monotonic() + 10)
    except (Closed, TimeoutError) as e:
        rep.check(False, "negative control：读 hello", str(e))
        sock.close()
        return
    tr = ProbeTransport(list(hello[11:15]), list(hello[7:11]))

    # B) 第一件事就发坏头（连 IV 都不让它推进），看服务端是否当场杀会话
    wire_bad, hdr_ok = pack_without_advancing(tr, body, corrupt_first_byte=True)
    try:
        sock.sendall(wire_bad)
    except OSError as e:
        rep.check(False, "negative control：发送坏头", str(e))
        sock.close()
        return
    rep.log("# 已发坏头 %s（同一 IV 下的正确头是 %s）；故意没推进本地 out IV"
            % (hexs(wire_bad[:4]), hexs(hdr_ok)))
    alive, got = wait_and_drain(sock, tr, 5.0)
    if not alive:
        rep.measure(True, "坏头被服务端当场拒绝并断线（最初的假设成立：连接存活=接受）")
        sock.close()
        return
    rep.measure(False, "坏头后 5s 内服务端没有断线",
                "=> s9 的 InvalidPacketHeaderException 分支（Client.java:253）没走到，"
                "「连接存活」不能当接受证据")

    # C) 坏头之后按「坏包不消费 IV」的模型继续通信：等 PING，回 PONG，看能不能活过 15s
    ok = ping_ack_round(sock, tr, rep, args, "坏头之后", ack_is_gate=False,
                        on_packet=lambda raw, n: rep.log(
                            "#   （坏头后收到的包 hdr=%s opcode=%s）"
                            % (n["hdr_hex"],
                               "0x%04X" % n["opcode"] if n["opcode"] is not None else "-")))
    if ok:
        rep.log("# 判定：clean-drop —— 坏包被丢弃且字节流完好，后续包照常处理")
    else:
        rep.log("# 判定：stream-jam —— 一个错误头部永久废掉这条连接的入站派发")
    sock.close()


# --------------------------------------------------------------------------
# 主流程
# --------------------------------------------------------------------------
def main():
    ap = argparse.ArgumentParser(description="T8 s9 protocol probe")
    ap.add_argument("--host", default="127.0.0.1")
    ap.add_argument("--port", type=int, default=8484,
                    help="s9 login-port（application.yml:69）")
    ap.add_argument("--send", default=None,
                    help="逗号分隔的十六进制 opcode，覆盖默认探测集（仍受门禁约束）")
    ap.add_argument("--no-ping", action="store_true",
                    help="不等服务端 PING（省 40s，但拿不到 s2c 端到端证据）")
    ap.add_argument("--ping-wait", type=float, default=40.0)
    ap.add_argument("--ping-rounds", type=int, default=2,
                    help="收几次 PING；>1 时每次回 PONG，顺带证明 IV 链能跨往返不漂")
    ap.add_argument("--ack-wait", type=float, default=20.0,
                    help="回 PONG 后观察存活秒数；须 > Client.checkIfIdle 的 15s 复查窗口")
    ap.add_argument("--length-probe", action="store_true",
                    help="追加包长扫描（含 32768/40000），验证服务端能解 >=32768 的头")
    ap.add_argument("--negative-control", action="store_true",
                    help="另开一条连接做可证伪性对照：坏头部必须被服务端当场拒绝")
    ap.add_argument("--dry-run", action="store_true", help="不联网，只验门禁与计划")
    ap.add_argument("--tag", default=None, help="证据文件名后缀")
    args = ap.parse_args()

    if not O.HAVE_PYCRYPTO:
        sys.exit("ABORT: 探针需要 pycryptodome（与 s9 的 JCE AES/ECB 对齐），请先 pip install pycryptodome")

    rep = Report()
    now = dt.datetime.now(dt.timezone.utc)
    stamp = args.tag or now.strftime("%Y%m%dT%H%M%SZ")
    os.makedirs(EVIDENCE_DIR, exist_ok=True)

    name2val, handled, handler_of = s9_recv_tables()
    block, allow = load_list(F_BLOCK), load_list(F_ALLOW)

    rep.log("# T8 s9 protocol probe  (UTC %s)" % now.isoformat(timespec="seconds"))
    rep.log("# oracle module: tools/transport_oracle.py  (同一份实现，无本地重写)")
    rep.log("# version=%d  send_cypher_ver=0x%04X  recv_cypher_ver=0x%04X"
            % (O.VERSION, O.S9_SEND_SWAPPED, O.S9_RECV_SWAPPED))

    errs = verify_lists(name2val, handled, block, allow)
    if errs:
        rep.log("# 清单校验失败：")
        for e in errs:
            rep.log("#   " + e)
        rep.dump(os.path.join(EVIDENCE_DIR, "probe-%s.txt" % stamp))
        sys.exit(99)
    rep.log("# 门禁清单校验通过：blocklist=%d allowlist=%d" % (len(block), len(allow)))

    targets, skipped = resolve_targets(name2val, handled, handler_of, block, allow, args.send)
    rep.log("# s9 RecvOpcode 常量=%d  有效 handler=%d" % (len(name2val), len(handled)))
    rep.log("# 计划发送 %d 个 opcode：" % len(targets))
    for v, nm, why in targets:
        rep.log("#   0x%04X %-28s %s" % (v, nm, why))
    rep.log("# 被拦下 %d 个：" % len(skipped))
    for v, nm, why in skipped:
        rep.log("#   0x%04X %-28s SKIP(%s)" % (v, nm, why))

    if args.dry_run:
        rep.log("# dry-run：未联网")
        rep.dump(os.path.join(EVIDENCE_DIR, "probe-%s.txt" % stamp))
        return 0

    alive = True
    evidence = []

    sock = socket.create_connection((args.host, args.port), timeout=10)
    rep.log("# connected %s:%d" % (args.host, args.port))
    sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)

    # ---- 阶段 1：handshake 取证 ----
    try:
        hello = recv_exact(sock, HELLO_LEN, time.monotonic() + 10)
    except (Closed, TimeoutError) as e:
        rep.check(False, "读取 16 字节 hello", str(e))
        rep.dump(os.path.join(EVIDENCE_DIR, "probe-%s.txt" % stamp))
        sock.close()
        sys.exit(1)
    evidence.append(("hello s->c (unencrypted)", hello))
    rep.log("# hello: %s" % hexs(hello, mask_idx=(10, 14)))
    recv_iv = list(hello[7:11])
    send_iv = list(hello[11:15])
    rep.check(hello[0:2] == struct.pack("<H", HELLO_TAIL_OPC),
              "F1 hello opcode == 0x000E (LE, 正文内)")
    rep.check(hello[2:4] == struct.pack("<H", O.VERSION),
              "F1 hello version == %d" % O.VERSION,
              "got %s" % hexs(hello[2:4]))
    rep.check(hello[4:6] == struct.pack("<H", 1), "F1 hello flag == 0x0001")
    rep.check(hello[6] == 49, "F1 hello byte6 == 49 (0x31)")
    rep.check(hello[7:10] == bytes(O.IV_RECV_SEED[:3]),
              "F1 recvIv 前缀 == 'Frz' (70 114 122)",
              "got %s" % hexs(hello[7:10]))
    rep.check(hello[11:14] == bytes(O.IV_SEND_SEED[:3]),
              "F1 sendIv 前缀 == 'R0x' (82 48 120)",
              "got %s" % hexs(hello[11:14]))
    rep.check(hello[15] == 8, "F1 hello byte15 == 8")

    tr = ProbeTransport(send_iv, recv_iv)

    # 我们自己的预期 hello（若服务端布局不同，这里会先炸出来）
    expect = bytes(O.s9_get_hello(send_iv, recv_iv))
    rep.check(expect == hello, "hello 与 oracle s9_get_hello() 逐字节一致",
              "expect %s" % hexs(expect, mask_idx=(10, 14)))

    # ---- 阶段 2：连发探测包 ----
    bodies = {}
    for i, (v, nm, why) in enumerate(targets):
        pad = (i * 7) % 13
        bodies[v] = struct.pack("<H", v) + bytes((0xA0 + j) & 0xFF for j in range(pad))

    for v, nm, why in targets:
        body = bodies[v]
        wire = tr.pack(body)
        evidence.append(("sent 0x%04X len=%d" % (v, len(body)), wire))
        try:
            sock.sendall(wire)
        except OSError as e:
            rep.check(False, "send 0x%04X %s" % (v, nm), str(e))
            alive = False
            break
        time.sleep(0.15)
        try:
            inbound = poll_inbound(sock, tr, 0.25)
        except (Closed, TimeoutError) as e:
            rep.check(False, "0x%04X %s 之后连接被服务端关闭" % (v, nm), str(e))
            alive = False
            break
        for n in inbound:
            # 探测阶段本不该有回包；有就是服务端主动发了东西，值得记下来
            evidence.append(("inbound during probe s->c", n["hdr"] + bytes(n["plain"])))
            rep.log("#   意外回包 hdr=%s opcode=%s plain=%s"
                    % (n["hdr_hex"],
                       "0x%04X" % n["opcode"] if n["opcode"] is not None else "-",
                       hexs(n["plain"])))
    if alive:
        rep.log("# 连发 %d 个探测包后连接仍存活（sent=%d iv_out=%s）"
                % (len(targets), tr.sent, hexs(bytes(tr.iv_out), mask_idx=(3,))))
        rep.log("#   注意：这只是「没被 TCP 层打断」，弱证据；真凭据在阶段 3 的 PONG oracle")

    # ---- 阶段 3：s2c 端到端（服务端 PING）+ c2s 派发证据（PONG 存活）----
    s2c_lengths = []

    def sink(raw, n):
        evidence.append(("s->c", raw))
        s2c_lengths.append(n["length_s9"])

    if not args.no_ping and alive:
        for r in range(max(1, args.ping_rounds)):
            if not ping_ack_round(sock, tr, rep, args, "主连接 PONG#%d" % (r + 1), sink):
                alive = False
                break

    # ---- 阶段 4：包长扫描（D8 边界）----
    # 「没断线」在这里已经不算证据（negative control 证明了坏包也不断线），
    # 所以扫完必须再用同一个 PONG oracle 复查：大长度包头有没有把流搞坏。
    if args.length_probe and alive:
        rep.log("# 包长扫描：证明服务端能解 len>=32768 的头（客户端侧 D8 是收方向缺陷）")
        dead = [v for v, nm, w in targets if v not in allow]
        probe_v = dead[0] if dead else PONG_OPCODE
        lp_alive = True
        # body 最短 2 字节：opcode 本身就占 2 字节（LE，写在正文内）
        for L in (2, 3, 255, 256, 4095, 4096, 8192, 32767, 32768, 40000):
            body = struct.pack("<H", probe_v) + bytes((L & 0xFF) for _ in range(max(0, L - 2)))
            assert len(body) == L
            wire = tr.pack(body)
            try:
                sock.sendall(wire)
            except OSError as e:
                rep.check(False, "length %d send" % L, str(e))
                lp_alive = False
                break
            time.sleep(0.2)
            try:
                poll_inbound(sock, tr, 0.2)
            except (Closed, TimeoutError) as e:
                rep.check(False, "length %d：服务端断连（未能解该头）" % L, str(e))
                lp_alive = False
                break
            rep.log("#   len=%-6d 头部=%s  未断连" % (L, hexs(wire[:4])))
        if lp_alive:
            rep.log("# 未断连不代表被吃下：用 PONG oracle 复查这条流的入站派发是否还完好")
            if ping_ack_round(sock, tr, rep, args, "包长扫描之后", sink):
                rep.check(True, "含 32768/40000 的包长扫描之后派发仍正常："
                                "服务端长度公式覆盖 16 位全域，大包头不破坏帧同步")
            else:
                alive = False

    try:
        sock.close()
    except OSError:
        pass

    # ---- 阶段 5：可证伪性对照 ----
    if args.negative_control:
        rep.log("# ---- 阶段 5：negative control（坏头必须被拒）----")
        negative_control(args, rep, targets)

    # ---- 证据落盘 ----
    hx = os.path.join(EVIDENCE_DIR, "probe-%s.hex" % stamp)
    with open(hx, "w", encoding="utf-8", newline="\n") as f:
        f.write("# T8 probe evidence  UTC %s  target %s:%d\n" % (now.isoformat(timespec="seconds"),
                                                                args.host, args.port))
        f.write("# IV 随机字节（recvIv[3] / sendIv[3]）以 ** 脱敏；其余为原始字节。\n")
        for label, data in evidence:
            f.write("%-34s %s\n" % (label + ":", hexs(data)))
    js = os.path.join(EVIDENCE_DIR, "probe-%s.json" % stamp)
    with open(js, "w", encoding="utf-8", newline="\n") as f:
        json.dump({
            "utc": now.isoformat(timespec="seconds"),
            "target": "%s:%d" % (args.host, args.port),
            "version": O.VERSION,
            "sent_packets": tr.sent,
            "recvd_packets": tr.recvd,
            "s2c_lengths_observed": s2c_lengths,
            "targets": [{"opcode": "0x%04X" % v, "name": nm, "why": w}
                        for v, nm, w in targets],
            "skipped": [{"opcode": "0x%04X" % v, "name": nm, "why": w}
                        for v, nm, w in skipped],
            "checks": [{"ok": ok, "name": n, "detail": d} for ok, n, d in rep.checks],
            "measurements": [{"ok": ok, "name": n, "detail": d}
                             for ok, n, d in rep.measures],
        }, f, ensure_ascii=False, indent=2)
    txt = os.path.join(EVIDENCE_DIR, "probe-%s.txt" % stamp)
    rep.dump(txt)

    fails = sum(1 for ok, _, _ in rep.checks if not ok)
    rep.log("")
    rep.log("# %d checks, %d failed; %d measured" % (len(rep.checks), fails,
                                                     len(rep.measures)))
    rep.log("# evidence: %s | %s | %s" % (os.path.basename(hx), os.path.basename(js),
                                          os.path.basename(txt)))
    return min(fails, 99)


if __name__ == "__main__":
    sys.exit(main())
