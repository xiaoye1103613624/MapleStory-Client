#!/usr/bin/env python3
"""T1: 从 s9 源码与客户端源码生成 opcode 对照矩阵。

两侧都按 opcode 数值 join（名字天然不同：客户端 MOVE_MONSTER ↔ s9 MOVE_LIFE）。
矩阵是机器可读的事实源，notes/opcode-diff.md 与门禁都由它派生。

用法：
    python gen_opcode_matrix.py            # 生成/覆盖 protocol/opcode-matrix.json
    python gen_opcode_matrix.py --check    # 只校验盘上矩阵是否仍与两侧源码一致（门禁）

--check 退出码（build_smoke.bat 按此拦构建）：
    0 一致，或只有引用行号搬动（GATE STALE，不拦）
    1 协议事实与源码矛盾 —— 必须重新生成并复核 notes/opcode-diff.md
    2 矩阵文件还不存在（环境未初始化，不拦构建）
"""
import argparse
import datetime
import hashlib
import json
import os
import re
import subprocess
import sys

S9 = r"E:\project\BeiDou-Server_s9"
S9_SRC = os.path.join(S9, "gms-server", "src", "main", "java", "org", "gms")
CLIENT = r"E:\project\MapleStory-Client"
OUT_JSON = os.path.join(CLIENT, "docs", "features", "s9-protocol-align",
                        "protocol", "opcode-matrix.json")

F_S9_RECV = os.path.join(S9_SRC, "net", "opcodes", "RecvOpcode.java")
F_S9_SEND = os.path.join(S9_SRC, "net", "opcodes", "SendOpcode.java")
F_S9_PROC = os.path.join(S9_SRC, "net", "PacketProcessor.java")
F_CL_OUT = os.path.join(CLIENT, "Net", "OutPacket.h")
F_CL_IN = os.path.join(CLIENT, "Net", "PacketSwitch.cpp")

# NAME(0x01), // comment   /   NAME = 41,
PAT_JAVA_CONST = re.compile(r"^\s*([A-Z][A-Z0-9_]*)\s*\(\s*(0x[0-9A-Fa-f]+|\d+)\s*\)")
PAT_CPP_CONST = re.compile(r"^\s*([A-Z][A-Z0-9_]*)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*,?")
# new XHandler(...) 与 XHandler.getInstance() 两种注册写法都要吃到
PAT_REGISTER = re.compile(
    r"registerHandler\(\s*RecvOpcode\.([A-Z0-9_]+)\s*,\s*"
    r"(?:new\s+)?([A-Za-z0-9_]+)(?:\.[A-Za-z_]+)?\s*\(")
PAT_METHOD = re.compile(r"private\s+void\s+(register\w+Handlers)\s*\(")
PAT_EMPLACE = re.compile(r"emplace<\s*([A-Z][A-Z0-9_]*)\s*,\s*([A-Za-z0-9_]+)\s*>\s*\(")
PAT_CREATE = re.compile(r"OutPacket\.create\(\s*SendOpcode\.([A-Z0-9_]+)")
# OutPacket 子类的构造器走 super(SendOpcode.X)，同样是真实下发点
PAT_SUPER = re.compile(r"\bsuper\(\s*SendOpcode\.([A-Z0-9_]+)")
# 客户端 OutPacket.h 里已有的 /// s9 RecvOpcode.NAME = 0xNN 断言
PAT_ANNOT = re.compile(r"s9\s+RecvOpcode\.([A-Z0-9_]+)\s*=\s*(0x[0-9A-Fa-f]+|\d+)")
# 客户端把枚举名当实参传给 OutPacket 构造器 / switch 返回点：OutPacket(OutPacket::Opcode::NAME)
PAT_USE = re.compile(r"\bOpcode::([A-Z][A-Z0-9_]*)\b")

SKIP_DIR_TOKENS = (".git", "x64", "build", "docs", "tools", ".vs",
                   "third_party", "vendor", "debug", "release")


def read_text(path):
    """s9 是 UTF-8；客户端注释可能是 UTF-8 或 GBK，按序试。"""
    raw = open(path, "rb").read()
    for enc in ("utf-8", "gbk"):
        try:
            return raw.decode(enc)
        except UnicodeDecodeError:
            continue
    return raw.decode("utf-8", errors="replace")


def sha256(path):
    return hashlib.sha256(open(path, "rb").read()).hexdigest()


def to_int(lit):
    return int(lit, 16) if lit.lower().startswith("0x") else int(lit)


def parse_java_enum(path, enum_name):
    """只取枚举常量段：从 `enum NAME` 到第一个单独分号 / 构造器之前。"""
    lines = read_text(path).splitlines()
    start = None
    for i, ln in enumerate(lines):
        if re.search(r"\benum\s+%s\b" % enum_name, ln):
            start = i + 1
            break
    if start is None:
        sys.exit("ABORT: enum %s not found in %s" % (enum_name, path))
    out = []
    for i in range(start, len(lines)):
        ln = lines[i]
        if re.match(r"^\s*;\s*$", ln) or re.match(r"^\s*(private|public|protected)\s", ln):
            break
        m = PAT_JAVA_CONST.match(ln)
        if m:
            out.append({"name": m.group(1), "value": to_int(m.group(2)), "line": i + 1})
    return out


def parse_cpp_enum(path):
    """取 `enum Opcode : uint16_t { ... };` 块内的 NAME = value。"""
    lines = read_text(path).splitlines()
    start = None
    for i, ln in enumerate(lines):
        if re.search(r"\benum\s+Opcode\b", ln):
            start = i + 1
            break
    if start is None:
        sys.exit("ABORT: enum Opcode not found in %s" % path)
    out = []
    depth = 0
    for i in range(start - 1, len(lines)):
        ln = lines[i]
        depth += ln.count("{") - ln.count("}")
        if i >= start:
            code = re.sub(r"//.*$", "", ln)
            m = PAT_CPP_CONST.match(code)
            if m:
                out.append({"name": m.group(1), "value": to_int(m.group(2)), "line": i + 1})
        if depth == 0 and i >= start:
            break
    return out


def parse_registrations(path):
    """opcode 名 -> 注册信息。

    s9 的 registerHandler 是 `handlers[code.getValue()] = handler` 数组覆盖写
    （PacketProcessor.java:100-105），所以**后注册的才生效**；同名重复注册必须暴露
    出被盖掉的那条，否则报告会把不可达的 handler 当成现役 handler。"""
    method = None
    out = {}
    for i, ln in enumerate(read_text(path).splitlines()):
        mm = PAT_METHOD.search(ln)
        if mm:
            method = mm.group(1)
        m = PAT_REGISTER.search(ln)
        if m:
            entry = out.setdefault(m.group(1), {"handlers": [], "lines": [],
                                                "group": method})
            entry["handlers"].append(m.group(2))
            entry["lines"].append(i + 1)
    for e in out.values():
        e["handler"] = e["handlers"][-1]
        e["line"] = e["lines"][-1]
        e["shadowed_by"] = e["handlers"][:-1] or None
    return out


def parse_emplaces(path):
    """PacketSwitch 构造函数里的 emplace<opcode, Handler>。"""
    out = {}
    for i, ln in enumerate(read_text(path).splitlines()):
        m = PAT_EMPLACE.search(ln)
        if m:
            out.setdefault(m.group(1), []).append({"handler": m.group(2), "line": i + 1})
    return out


def parse_s9_emitted(root):
    """s9 实际创建过的 SendOpcode（定义了但从不 create 的属信息项，不是缺口）。"""
    used = set()
    for dirpath, _, files in os.walk(root):
        for fn in files:
            if fn.endswith(".java"):
                text = read_text(os.path.join(dirpath, fn))
                for pat in (PAT_CREATE, PAT_SUPER):
                    for m in pat.finditer(text):
                        used.add(m.group(1))
    return used


def parse_client_annotations(path):
    """OutPacket.h 里 /// s9 RecvOpcode.NAME = 0xNN 的人工断言。"""
    out = {}
    pending = []
    for ln in read_text(path).splitlines():
        for m in PAT_ANNOT.finditer(ln):
            pending.append((m.group(1), to_int(m.group(2))))
        ce = PAT_CPP_CONST.match(re.sub(r"//.*$", "", ln))
        if ce and pending:
            out[ce.group(1)] = pending
            pending = []
    return out


def strip_c_comments(text):
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    return re.sub(r"//[^\n]*", "", text)


def cpp_enum_region(path):
    """返回 `enum Opcode { ... };` 在文件中的行区间（0-based, 闭区间）。"""
    lines = read_text(path).splitlines()
    start = next(i for i, ln in enumerate(lines)
                 if re.search(r"\benum\s+Opcode\b", ln))
    depth = 0
    for i in range(start, len(lines)):
        depth += lines[i].count("{") - lines[i].count("}")
        if depth == 0 and i > start:
            return start, i
    return start, len(lines) - 1


def parse_client_uses(root, decl_path, enum_region):
    """枚举名 -> 构造点列表。排除声明自身与注释，避免把文档行当使用。

    这是 s9_emitted 的镜像维度：证明客户端真的会把这个 opcode 放上链路。
    """
    lo, hi = enum_region
    used = {}
    for dirpath, dirnames, files in os.walk(root):
        norm = dirpath.replace(os.sep, "/").lower()
        dirnames[:] = [d for d in dirnames
                       if not any(t in ("/" + d).lower() for t in SKIP_DIR_TOKENS)]
        for fn in files:
            if not fn.endswith((".cpp", ".h")):
                continue
            p = os.path.join(dirpath, fn)
            lines = strip_c_comments(read_text(p)).splitlines()
            if os.path.normcase(p) == os.path.normcase(decl_path):
                # 挖空声明段但保留行号，引用行号才可直接点进去
                lines = ["" if lo <= i <= hi else ln
                         for i, ln in enumerate(lines)]
            rel = os.path.relpath(p, root).replace(os.sep, "/")
            for i, ln in enumerate(lines):
                for m in PAT_USE.finditer(ln):
                    used.setdefault(m.group(1), []).append(
                        {"file": rel, "line": i + 1})
    return used


def dup_map(rows):
    """数值撞车表。key 转字符串：JSON 会把整数 key 变成字符串，
    不统一的话 --check 的语义比较会把自己刚写出的文件判成漂移。"""
    m = {}
    for r in rows:
        m.setdefault(str(r["value"]), []).append(r["name"])
    return {v: names for v, names in m.items() if len(names) > 1}


def build():
    s9_recv = parse_java_enum(F_S9_RECV, "RecvOpcode")
    s9_send = parse_java_enum(F_S9_SEND, "SendOpcode")
    s9_reg = parse_registrations(F_S9_PROC)
    # 运行时生效的是「注册行号最大」的那条（handlers[value] = handler 覆盖写），
    # 跨枚举名的撞值也按这条规则判定，不能只看名字。
    reg_sites = []
    for r in s9_recv:
        e = s9_reg.get(r["name"])
        if e:
            for h, l in zip(e["handlers"], e["lines"]):
                reg_sites.append({"s9_name": r["name"], "value": r["value"],
                                  "handler": h, "line": l})
    s9_effective = {}
    for s in reg_sites:
        cur = s9_effective.get(s["value"])
        if cur is None or s["line"] > cur["line"]:
            s9_effective[s["value"]] = s
    s9_emitted = parse_s9_emitted(S9_SRC)
    cl_out = parse_cpp_enum(F_CL_OUT)
    cl_in = parse_cpp_enum(F_CL_IN)
    cl_emplace = parse_emplaces(F_CL_IN)
    cl_annot = parse_client_annotations(F_CL_OUT)
    cl_uses = parse_client_uses(CLIENT, F_CL_OUT, cpp_enum_region(F_CL_OUT))

    s9_recv_by_val = {}
    s9_recv_by_name = {}
    for r in s9_recv:
        s9_recv_by_val.setdefault(r["value"], []).append(r)
        s9_recv_by_name.setdefault(r["name"], r)
    s9_send_by_val = {}
    for r in s9_send:
        s9_send_by_val.setdefault(r["value"], []).append(r)
    cl_out_by_val = {}
    for r in cl_out:
        cl_out_by_val.setdefault(r["value"], []).append(r["name"])
    cl_in_by_val = {}
    for r in cl_in:
        cl_in_by_val.setdefault(r["value"], []).append(r["name"])

    s9_recv_dups = dup_map(s9_recv)
    s9_send_dups = dup_map(s9_send)
    cl_in_dups = {str(v): n for v, n in cl_in_by_val.items() if len(n) > 1}
    cl_out_dups = {str(v): n for v, n in cl_out_by_val.items() if len(n) > 1}

    # ---- c2s：客户端会发的包，服务端收不收 ----
    c2s = []
    for r in cl_out:
        s9hits = s9_recv_by_val.get(r["value"], [])
        # 认这条值上运行时真正生效的那条注册（可能是别的枚举名注册的）
        eff = s9_effective.get(r["value"])
        s9name = eff["s9_name"] if eff else (s9hits[0]["name"] if s9hits else None)
        reg = {"handler": eff["handler"], "group": s9_reg[eff["s9_name"]]["group"],
               "line": eff["line"], "shadowed_by": s9_reg[eff["s9_name"]]["shadowed_by"]} \
            if eff else None
        sites = cl_uses.get(r["name"], [])
        vkey = str(r["value"])
        if not s9hits:
            # 从未被构造过的枚举只是文档残留，不上链路，不构成协议缺陷
            status = "CLIENT_ONLY" if sites else "CLIENT_ONLY_DEAD"
        elif reg is None:
            status = "MATCHED_NO_HANDLER"
        elif vkey in s9_recv_dups:
            status = "S9_DUP_VALUE"
        elif vkey in cl_out_dups:
            status = "CLIENT_DUP_VALUE"
        else:
            status = "MATCHED"
        ann = cl_annot.get(r["name"], [])
        annot_ok = None
        if ann:
            # 断言里写的是 s9 名字，就去 s9 枚举查它的真值，而不是靠 join 结果自证
            annot_ok = all(s9_recv_by_name.get(a[0], {}).get("value") == a[1]
                           for a in ann)
        c2s.append({
            "client_name": r["name"], "client_value": r["value"], "client_line": r["line"],
            "s9_name": s9name, "s9_value": r["value"] if s9hits else None,
            "s9_line": s9hits[0]["line"] if s9hits else None,
            "s9_names_at_value": s9_recv_dups.get(vkey, []),
            "s9_handler": reg["handler"] if reg else None,
            "s9_reg_line": reg["line"] if reg else None,
            "s9_group": reg["group"] if reg else None,
            "s9_shadowed_by": reg["shadowed_by"] if reg else None,
            "status": status,
            "client_sent": bool(sites),
            "client_sent_count": len(sites),
            "client_sent_at": sites[:6],
            "client_annotations": [{"s9_name": a[0], "value": a[1]} for a in ann],
            "annotation_consistent": annot_ok,
        })

    # ---- s2c：服务端会发的包，客户端处理不处理 ----
    s2c = []
    for r in s9_send:
        clnames = cl_in_by_val.get(r["value"], [])
        emitted = r["name"] in s9_emitted
        # emplace_impl 是覆盖写：同一数值的最后一次注册才是真正生效的 handler
        entries = [e for n in clnames for e in cl_emplace.get(n, [])]
        winner = max(entries, key=lambda e: e["line"])["handler"] if entries else None
        if clnames:
            status = "HANDLED" if entries else "ENUM_NO_HANDLER"
            if len(entries) > 1:
                status = "CLIENT_DUP_VALUE"
        else:
            status = "UNHANDLED_EMITTED" if emitted else "DEFINED_NOT_EMITTED"
        if str(r["value"]) in s9_send_dups:
            status = "S9_DUP_VALUE"
        s2c.append({
            "s9_name": r["name"], "s9_value": r["value"], "s9_line": r["line"],
            "s9_emitted": emitted,
            "client_names": clnames,
            "client_handlers": [e["handler"] for e in entries],
            "client_handler_wins": winner,
            "client_shadowed_handlers": [e["handler"] for e in entries
                                         if e["handler"] != winner],
            "status": status,
        })

    # 客户端注册了、但枚举里找不到值（不该发生）/ 枚举有值但没注册
    enum_no_impl = sorted(n for n in cl_in_by_val_all(cl_in)
                          if n not in cl_emplace)

    try:
        commit = subprocess.check_output(
            ["git", "-C", S9, "rev-parse", "HEAD"], text=True).strip()
    except Exception:
        commit = "UNKNOWN"

    data = {
        "_meta": {
            "generated_by": "tools/gen_opcode_matrix.py",
            "generated_at": datetime.datetime.now().isoformat(timespec="seconds"),
            "s9_git_head": commit,
            "source_hashes": {
                os.path.relpath(p, S9): sha256(p) for p in
                (F_S9_RECV, F_S9_SEND, F_S9_PROC)},
            "client_source_hashes": {
                os.path.relpath(p, CLIENT).replace("\\", "/"): sha256(p) for p in
                (F_CL_OUT, F_CL_IN)},
            "counts": {
                "s9_recv_constants": len(s9_recv),
                "s9_recv_distinct_values": len(s9_recv_by_val),
                "s9_send_constants": len(s9_send),
                "s9_send_distinct_values": len(s9_send_by_val),
                "s9_send_emitted": len(s9_emitted),
                "s9_registered_handlers": len(s9_reg),
                "s9_registration_sites": len(reg_sites),
                "client_out_constants": len(cl_out),
                "client_out_sent": sum(1 for r in c2s if r["client_sent"]),
                "client_in_constants": len(cl_in),
                "client_in_registrations": sum(len(v) for v in cl_emplace.values()),
            },
            "collisions": {
                "s9_recv": s9_recv_dups, "s9_send": s9_send_dups,
                "client_in": cl_in_dups, "client_out": cl_out_dups,
            },
            "s9_shadowed_registrations": {
                n: {"effective": e["handler"], "shadowed": e["shadowed_by"],
                    "lines": e["lines"]}
                for n, e in s9_reg.items() if e["shadowed_by"]},
            "client_enum_without_registration": enum_no_impl,
        },
        "c2s": c2s,
        "s2c": s2c,
    }
    return data


def cl_in_by_val_all(rows):
    return {r["name"] for r in rows}


def summarize(data):
    from collections import Counter
    c = data["_meta"]["counts"]
    print("s9   : recv=%(s9_recv_constants)d consts / %(s9_recv_distinct_values)d values  "
          "send=%(s9_send_constants)d consts / %(s9_send_emitted)d emitted  "
          "handlers=%(s9_registered_handlers)d" % c)
    print("     : client out=%(client_out_constants)d consts / %(client_out_sent)d sent  "
          "in=%(client_in_constants)d consts / %(client_in_registrations)d handlers" % c)
    coll = data["_meta"]["collisions"]
    print("     : collisions s9_recv=%s s9_send=%s client_in=%s client_out=%s" % (
        coll["s9_recv"], coll["s9_send"], coll["client_in"], coll["client_out"]))
    print("c2s  %s" % dict(Counter(r["status"] for r in data["c2s"])))
    print("s2c  %s" % dict(Counter(r["status"] for r in data["s2c"])))
    stale = [r for r in data["c2s"] if r["annotation_consistent"] is False]
    print("stale client s9-assertion comments: %d" % len(stale))
    dead = [r for r in data["c2s"] if not r["client_sent"]]
    print("client enums never constructed   : %d" % len(dead))
    sh = data["_meta"].get("s9_shadowed_registrations", {})
    print("s9 shadowed registrations        : %d  %s" % (len(sh), sh))


VOLATILE_META = ("generated_at", "s9_git_head", "source_hashes",
                 "client_source_hashes")
# 只是 path:line 引用漂移（同一事实搬了位置）不值得拦构建；协议事实变了才拦
LINE_FIELDS = {"client_line", "s9_line", "s9_reg_line", "client_sent_at"}


def row_diff(old, new, key):
    """按 key 对齐两张表，返回 (硬差异, 软差异)：
    硬 = 协议事实变了（值/名字/handler/status），报告结论失效；
    软 = 只有行号搬动，重新生成即可。"""
    o = {r[key[0]] + "/" + str(r.get(key[1], "")): r for r in old}
    n = {r[key[0]] + "/" + str(r.get(key[1], "")): r for r in new}
    hard, soft = [], []
    for k in sorted(set(o) | set(n)):
        if k not in o:
            hard.append("  + new row %s" % k)
        elif k not in n:
            hard.append("  - dropped row %s" % k)
        elif o[k] != n[k]:
            fields = sorted(f for f in o[k] if o[k][f] != n[k].get(f))
            line_only = all(f in LINE_FIELDS for f in fields)
            (soft if line_only else hard).append("  ~ %s  %s" % (k, ", ".join(fields)))
    return hard, soft


def check(path, data):
    """门禁：协议事实与盘上矩阵不一致就判失败（拦构建）；
    只有引用行号搬动或源码 hash 变了但事实没变，则提示重新生成，不拦。"""
    if not os.path.exists(path):
        print("GATE SKIP: %s missing - run gen_opcode_matrix.py first" % path)
        return 2
    stored = json.load(open(path, encoding="utf-8"))
    cur = build()
    hard = []
    soft = []
    for name, table, key in (("c2s", "c2s", ("client_name", "client_value")),
                             ("s2c", "s2c", ("s9_name", "s9_value"))):
        h, s = row_diff(stored[table], cur[table], key)
        hard += ["[%s]%s" % (name, d) for d in h]
        soft += ["[%s]%s" % (name, d) for d in s]
    for k in stored["_meta"]:
        if k in VOLATILE_META or stored["_meta"][k] == cur["_meta"].get(k):
            continue
        hard.append("  ~ _meta.%s" % k)

    stale_src = [kk for k in ("source_hashes", "client_source_hashes")
                 for kk, v in stored["_meta"][k].items()
                 if cur["_meta"][k].get(kk) != v]

    if hard:
        print("GATE FAIL: opcode matrix contradicts the sources (%d fact change(s)):"
              % len(hard))
        for d in hard[:40]:
            print(d)
        if len(hard) > 40:
            print("  ... %d more" % (len(hard) - 40))
        print("run: python tools/gen_opcode_matrix.py   "
              "then re-verify notes/opcode-diff.md")
        return 1
    if soft or stale_src:
        why = ", ".join(soft[:10]) if soft else \
            "source files changed but derived facts are identical"
        print("GATE STALE (not blocking): %s" % why)
        if stale_src:
            print("  hashed sources changed: %s" % ", ".join(stale_src))
        print("run: python tools/gen_opcode_matrix.py")
        return 0
    print("GATE OK: opcode matrix matches both sources (s9 %s)"
          % stored["_meta"]["s9_git_head"][:10])
    return 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--check", action="store_true", help="只做门禁校验，不写文件")
    ap.add_argument("--out", default=OUT_JSON)
    args = ap.parse_args()

    data = build()
    if args.check:
        sys.exit(check(args.out, data))

    os.makedirs(os.path.dirname(args.out), exist_ok=True)
    text = json.dumps(data, ensure_ascii=False, indent=1, sort_keys=True)
    open(args.out, "w", encoding="utf-8", newline="\n").write(text + "\n")
    print("wrote %s" % args.out)
    summarize(data)


if __name__ == "__main__":
    main()
