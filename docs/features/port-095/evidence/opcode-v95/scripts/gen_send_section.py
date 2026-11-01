# -*- coding: utf-8 -*-
"""Refresh §5 (send table) of notes/opcode-v95.md from the merged site table."""
import io, json, os, re, collections

MD = r"E:\project\MapleStory-Client\docs\features\port-095\notes\opcode-v95.md"
SITES = os.path.join(os.path.dirname(os.path.abspath(__file__)), "send_sites_all.json")

sites = json.load(io.open(SITES, encoding="utf-8"))


def pretty(n):
    m = re.match(r"\?([^@]+)@([^@]+)@@", n)
    return (m.group(2) + "::" + m.group(1)) if m else n


per = collections.defaultdict(dict)  # op -> {fn: name}
for s in sites:
    per[s["op"]].setdefault(s["fn"], pretty(s["name"]))

rows = []
for op in sorted(per):
    cells = ["`%s` (%s)" % (name, fn) for fn, name in sorted(per[op].items(), key=lambda kv: kv[1])]
    more = " …（共 %d 个函数）" % len(cells) if len(cells) > 4 else ""
    rows.append("| %d | %s%s |" % (op, "<br>".join(cells[:4]), more))

block = """## 5. 发送（客户端 → 服务端）opcode

方法：对 `??0COutPacket@@QAE@J@Z`（`0x68d090`）取全部 **582** 处 xref（462 个函数）。

- 502 处来自 `dec_send/` 完整反编译，抽取 `COutPacket::COutPacket(this: &p, nType: N)` 常量（含 `(COutPacket *)&p` 形式与变量回溯，见 `resolve_dyn.py`）；
- 80 处位于 11 个**反编译被截断或漏采**的函数（`0x5408e0`/`0x654300`/`0x6b1fc0`/`0x92a240`/`0x9343c0`/`0x937330`/`0x9eb3e0`/`0xa0d3b0` + `0x5dbef0`/`0x627e20`/`0x9a6dd0`），用 `insn_query` 回查紧邻 ctor 的 `push <imm>` 补齐；跳转表分支共用同一 `push` 的（`0x6282da` → `0x6282e5`/`0x628304`）按共享处理。

共解析出 **238** 个不同发送 opcode。旧版 217 的两处错误：15 个“动态 opcode”实为 `this: (COutPacket *)&pkt` 的强制转换被正则漏匹配；80 个构造点因函数未采集/反编译截断而丢失。

相对旧表的 12 个新增 opcode：**20、32、52、126、139、152、158、219、230、231、234、258**（明细与语义见 [opcode-map-v95-vs-s9.md §5.1](opcode-map-v95-vs-s9.md)）。

每个 opcode 最多列 4 个发送函数；完整数据：

- `evidence/opcode-v95/send_sites_all.json`：逐个构造点（`fn` / `name` / `op` / `how` / `src`）
- `evidence/opcode-v95/send_ops.json`：`{opcode: [函数地址]}`

| opcode | 发送函数（调用点所在函数 0x 地址） |
|---|---|
%s

原始数据：`evidence/opcode-v95/send_ops.json`（opcode → 函数地址集合）。

""" % "\n".join(rows)

md = io.open(MD, encoding="utf-8").read()
i = md.index("## 5. 发送")
j = md.index("## 6. 与当前 s9")
new = md[:i] + block + md[j:]
io.open(MD, "w", encoding="utf-8", newline="\n").write(new)
print("ops=%d sites=%d; doc lines %d -> %d" % (len(per), len(sites), md.count("\n") + 1, new.count("\n") + 1))
