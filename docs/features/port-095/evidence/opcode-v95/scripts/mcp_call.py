#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Minimal MCP streamable-HTTP caller (UTF-8 safe on Windows).

usage: python mcp_call.py <port> <tool> <args.json|-> [timeout_sec]
"""
import json
import sys
import urllib.request


def rpc(url, payload, sid=None, timeout=300):
    data = json.dumps(payload, ensure_ascii=True).encode("ascii")
    headers = {
        "Content-Type": "application/json",
        "Accept": "application/json, text/event-stream",
    }
    if sid:
        headers["Mcp-Session-Id"] = sid
    req = urllib.request.Request(url, data=data, headers=headers, method="POST")
    resp = urllib.request.urlopen(req, timeout=timeout)
    body = resp.read().decode("utf-8", "replace")
    return resp.headers.get("Mcp-Session-Id"), body


def main():
    port, tool = sys.argv[1], sys.argv[2]
    args_file = sys.argv[3]
    timeout = int(sys.argv[4]) if len(sys.argv) > 4 else 300
    if args_file == "-":
        arguments = {}
    else:
        with open(args_file, "r", encoding="utf-8") as f:
            arguments = json.load(f)
    url = "http://127.0.0.1:%s/mcp" % port

    sid, _ = rpc(url, {
        "jsonrpc": "2.0", "id": 1, "method": "initialize",
        "params": {"protocolVersion": "2025-06-18", "capabilities": {},
                   "clientInfo": {"name": "qoder-cli", "version": "1"}},
    }, timeout=20)
    rpc(url, {"jsonrpc": "2.0", "method": "notifications/initialized"}, sid, timeout=20)
    _, body = rpc(url, {
        "jsonrpc": "2.0", "id": 2, "method": "tools/call",
        "params": {"name": tool, "arguments": arguments},
    }, sid, timeout=timeout)
    sys.stdout.write(body)


if __name__ == "__main__":
    main()
