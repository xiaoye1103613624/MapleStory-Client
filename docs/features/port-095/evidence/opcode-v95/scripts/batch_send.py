#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Batch-decompile every caller of COutPacket::COutPacket(int nType) -> send-opcode extraction.

Usage: python batch_send.py [resume]
Writes dec_send/<funcaddr>.json and send_ops.json
"""
import json
import os
import sys
import time
import urllib.request

PORT = 13338
DB = "0dce3e26"
OUTDIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "dec_send")
URL = "http://127.0.0.1:%d/mcp" % PORT


def rpc(payload, sid=None, timeout=180):
    data = json.dumps(payload, ensure_ascii=True).encode("ascii")
    headers = {
        "Content-Type": "application/json",
        "Accept": "application/json, text/event-stream",
    }
    if sid:
        headers["Mcp-Session-Id"] = sid
    req = urllib.request.Request(URL, data=data, headers=headers, method="POST")
    resp = urllib.request.urlopen(req, timeout=timeout)
    return resp.headers.get("Mcp-Session-Id"), resp.read().decode("utf-8", "replace")


def parse_sse(body):
    out = []
    for line in body.splitlines():
        if line.startswith("data:"):
            out.append(line[5:].strip())
    return out[-1] if out else body


def main():
    os.makedirs(OUTDIR, exist_ok=True)
    callers = json.load(open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "send_callers.json"), encoding="utf-8"))
    sid, _ = rpc({"jsonrpc": "2.0", "id": 1, "method": "initialize",
                  "params": {"protocolVersion": "2025-06-18", "capabilities": {},
                             "clientInfo": {"name": "qoder-cli", "version": "1"}}}, timeout=30)
    rpc({"jsonrpc": "2.0", "method": "notifications/initialized"}, sid, timeout=30)

    done = 0
    for addr, name in callers.items():
        dst = os.path.join(OUTDIR, addr.replace("0x", "") + ".json")
        if os.path.exists(dst) and os.path.getsize(dst) > 100:
            continue
        try:
            _, body = rpc({"jsonrpc": "2.0", "id": 2, "method": "tools/call",
                           "params": {"name": "decompile",
                                      "arguments": {"addr": addr, "database": DB,
                                                    "include_addresses": False}}}, sid, timeout=180)
            payload = parse_sse(body)
            json.loads(payload)  # validate
            with open(dst, "w", encoding="utf-8") as f:
                f.write(payload)
            done += 1
        except Exception as exc:  # noqa: BLE001
            with open(dst + ".err", "w", encoding="utf-8") as f:
                f.write("%s: %s\n%s" % (addr, name, exc))
        if done % 50 == 0:
            sys.stderr.write("done=%d\n" % done)
            sys.stderr.flush()
    print("decompiled=%d" % done)


if __name__ == "__main__":
    main()
