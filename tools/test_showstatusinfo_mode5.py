# -*- coding: utf-8 -*-
"""SHOW_STATUS_INFO mode 5 语义守卫（round49 / 第 15 例）。

缺陷（改前）：客户端把 mode==5 当 SP（read_short + read_short），
而 s9 PacketCreator.java:1763-1773 getShowMesoGain(gain, inChat=true) 写的是
    writeByte(5) + writeInt(gain) + writeShort(0)
Angel 083 CWvsContext::OnMessage case 5 -> OnIncMoneyMessage@0xa221f3 也是 Decode4(gain)。
=> 每次聊天窗金币提示（任务奖励 / 商店售出 / 传送扣费）都被解析成
   "You have gained SP (+N)" 并弹 UISkillUp 窗。

本 harness 复刻两种读法，对同一份 s9 包体逐字节比对：
  改前 before()：read_short x2 -> 假 SP
  改后 after() ：read_int       -> 真 meso
只消费源码文本 + 内置包体，不启动客户端。

    python tools/test_showstatusinfo_mode5.py

Hard constraint: 083/v83 only. Read-only.
"""
import io
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "Net", "Handlers", "MessagingHandlers.cpp")
SRV = os.path.join("E:\\project", "BeiDou-Server_s9",
                   "gms-server", "src", "main", "java", "org", "gms", "util", "PacketCreator.java")


def read_text(path):
    with io.open(path, "r", encoding="utf-8", errors="replace") as fh:
        return fh.read()


def strip_comments(block):
    """去掉 // 行注释与 /* */ 块注释，只留真正执行的代码。"""
    out = []
    in_block = False
    for line in block.split("\n"):
        if in_block:
            if "*/" in line:
                in_block = False
            continue
        st = line.strip()
        if st.startswith("//"):
            continue
        if st.startswith("/*"):
            if "*/" not in st:
                in_block = True
            continue
        code = line.split("//", 1)[0]
        out.append(code)
    return "\n".join(out)


def block_of(text, header, end_marker="\n\t\t}"):
    i = text.find(header)
    if i < 0:
        return ""
    j = text.find(end_marker, i)
    return text[i:j] if j > 0 else text[i:i + 4000]


# ---------------------------------------------------------------- 静态守卫
def static_guards(text):
    guards = []

    def g(name, cond, detail=""):
        guards.append((name, bool(cond), detail))

    m5 = block_of(text, "else if (mode == 5)")
    code5 = strip_comments(m5)
    g("mode5_block_present", m5, "len=%d" % len(m5))
    g("mode5_reads_int", "int32_t gain = recv.read_int();" in code5, "Decode4(gain)")
    g("mode5_no_read_short", "recv.read_short()" not in code5, "旧 read_short 读法已移除")
    g("mode5_no_sp_text", "gained SP" not in code5, "不再输出 SP 文案（只看代码行）")
    g("mode5_no_skillup", "UISkillUp" not in code5, "不再弹 UISkillUp（只看代码行）")
    g("mode5_meso_text", "You have gained mesos" in code5 and "You have lost mesos" in code5, "金币文案")
    g("mode5_chat_only", "UIChatBar" in code5 and "UIStatusBar" not in code5, "inChat 语义：仅聊天窗")
    g("mode5_red_on_loss", "gain <= 0 ? UIChatBar::MessageType::RED" in code5, "亏损红色")
    g("mode5_drain_tail", "while (recv.available())" in code5, "余量吞尽")
    g("mode5_cites_server", "PacketCreator.java:1763" in m5, "引用 s9 写点")
    g("mode5_cites_angel", "OnIncMoneyMessage@0xa221f3" in m5, "引用 Angel 地址")
    g("mode5_cites_sp_path", "need_skillbook_update" in m5, "说明 SP 另有通道")

    # SP 通道仍在 ChangeStatsHandler（不能被本修一刀切掉）
    ph = read_text(os.path.join(ROOT, "Net", "Handlers", "PlayerHandlers.cpp"))
    nb = block_of(ph, "bool ChangeStatsHandler::need_skillbook_update")
    g("sp_channel_kept", "MapleStat::Id::SP" in nb, "ChangeStatsHandler 仍处理 SP")

    # mode 6 = GP 未被误伤
    m6 = block_of(text, "else if (mode == 6)")
    g("mode6_gp_kept", "You have gained GP" in m6, "mode6 仍为 GP")

    # s9 侧写点原文可对照
    try:
        srv = read_text(SRV)
    except OSError:
        srv = ""
    srv_ok = ("p.writeByte(5)" in srv and "getShowMesoGain(int gain, boolean inChat)" in srv)
    g("server_writes_byte5", srv_ok, "s9 getShowMesoGain inChat 分支 writeByte(5)")
    return guards


# ---------------------------------------------------------------- 包体复刻
def make_packet(gain):
    """s9 getShowMesoGain(gain, inChat=true) after opcode 0x27:
    byte 5 | int gain | short 0"""
    body = bytearray()
    body.append(5 & 0xFF)
    body += (gain & 0xFFFFFFFF).to_bytes(4, "little", signed=False)
    body += (0).to_bytes(2, "little", signed=False)
    return bytes(body)


class Reader(object):
    def __init__(self, data):
        self.d = data
        self.pos = 0

    def available(self):
        return len(self.d) - self.pos

    def read_byte(self):
        v = self.d[self.pos]
        self.pos += 1
        return v - 256 if v > 127 else v

    def read_short(self):
        v = int.from_bytes(self.d[self.pos:self.pos + 2], "little", signed=True)
        self.pos += 2
        return v

    def read_int(self):
        v = int.from_bytes(self.d[self.pos:self.pos + 4], "little", signed=True)
        self.pos += 4
        return v

    def skip_byte(self):
        self.pos += 1


def before(r):
    """改前：mode==5 当 SP。"""
    sp = r.read_short() if r.available() >= 2 else 0
    if r.available() >= 2:
        r.read_short()
    return {"kind": "sp", "value": sp, "text": "You have gained SP (+%d)" % sp}


def after(r):
    """改后：mode==5 当聊天窗金币。"""
    gain = r.read_int()
    while r.available():
        r.skip_byte()
    return {
        "kind": "meso",
        "value": gain,
        "text": ("You have lost mesos (%d)" % gain) if gain <= 0
                else ("You have gained mesos (+%d)" % gain),
    }


CASES = [
    # (gain, 期望 after.value, 期望 after 是否 gained, before 是否会给出错误 SP)
    (1000, 1000, True),
    (1, 1, True),
    (-500, -500, False),
    (2147483647, 2147483647, True),
    (-2147483648, -2147483648, False),
    (233, 233, True),
]


def main():
    text = read_text(SRC)
    guards = static_guards(text)
    failed = [x for x in guards if not x[1]]
    for name, ok, detail in guards:
        if not ok:
            sys.stdout.write("GUARD FAIL %-24s %s\n" % (name, detail))
    sys.stdout.write("STATIC GUARDS: %d checked, %d failed\n" % (len(guards), len(failed)))

    bad_before = 0
    bad_after = 0
    regression_ok = 0
    for gain, want_val, want_gain in CASES:
        pkt = make_packet(gain)
        # 真实 handler 先 mode = recv.read_byte() 才分派，harness 必须同样先消费
        rb = Reader(pkt)
        rb.read_byte()
        b = before(rb)
        ra = Reader(pkt)
        ra.read_byte()
        a = after(ra)
        b_wrong = (b["kind"] != "meso") or (b["value"] != gain)
        a_right = (a["kind"] == "meso" and a["value"] == want_val
                   and (("gained" in a["text"]) == want_gain))
        if b_wrong:
            bad_before += 1
        if not a_right:
            bad_after += 1
            sys.stdout.write("AFTER WRONG gain=%d got=%s\n" % (gain, a))
        # 改后语义稳定（同输入两次一致）
        ra2 = Reader(pkt)
        ra2.read_byte()
        a2 = after(ra2)
        if a == a2:
            regression_ok += 1
        sys.stdout.write("  gain=%-12d before=%-28s after=%s\n"
                         % (gain, "%s:%d" % (b["kind"], b["value"]), a["text"]))

    total = len(CASES)
    sys.stdout.write("SUMMARY bad_before=%d bad_after=%d stable=%d/%d\n"
                     % (bad_before, bad_after, regression_ok, total))
    ok = (not failed) and bad_before == total and bad_after == 0 and regression_ok == total
    if ok:
        sys.stdout.write("PASS: %d static guards + %d scenarios "
                         "(before mislabels %d/%d -> after 0; stable %d/%d)\n"
                         % (len(guards), total, bad_before, total, regression_ok, total))
        return 0
    sys.stdout.write("FAIL\n")
    return 1


if __name__ == "__main__":
    sys.exit(main())
