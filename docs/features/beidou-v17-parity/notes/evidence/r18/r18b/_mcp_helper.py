#!/usr/bin/env python3
"""Minimal IDA MCP HTTP client for r18b small-step REA recovery."""
from __future__ import annotations

import json
import sys
import urllib.request

URL = "http://127.0.0.1:13338/mcp"
OUT = r"E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\evidence\r18\r18b"


def rpc(method: str, params: dict | None, req_id: int, session: str | None):
    body = {"jsonrpc": "2.0", "id": req_id, "method": method}
    if params is not None:
        body["params"] = params
    data = json.dumps(body).encode("utf-8")
    headers = {
        "Content-Type": "application/json",
        "Accept": "application/json, text/event-stream",
    }
    if session:
        headers["Mcp-Session-Id"] = session
    req = urllib.request.Request(URL, data=data, headers=headers, method="POST")
    with urllib.request.urlopen(req, timeout=120) as resp:
        new_session = resp.headers.get("Mcp-Session-Id") or session
        raw = resp.read().decode("utf-8", errors="replace")
    # SSE or plain JSON
    payload = None
    if raw.startswith("event:") or "data:" in raw[:80]:
        for line in raw.splitlines():
            if line.startswith("data:"):
                payload = json.loads(line[5:].strip())
                break
    else:
        payload = json.loads(raw)
    return new_session, payload, raw


def main():
    cmd = sys.argv[1] if len(sys.argv) > 1 else "decompile"
    session, init, _ = rpc(
        "initialize",
        {
            "protocolVersion": "2024-11-05",
            "capabilities": {},
            "clientInfo": {"name": "r18b", "version": "1.0"},
        },
        1,
        None,
    )
    open(f"{OUT}/_mcp_init_resp.json", "w", encoding="utf-8").write(json.dumps(init, indent=2))
    # notifications/initialized (no id)
    try:
        body = json.dumps(
            {"jsonrpc": "2.0", "method": "notifications/initialized", "params": {}}
        ).encode()
        headers = {
            "Content-Type": "application/json",
            "Accept": "application/json, text/event-stream",
        }
        if session:
            headers["Mcp-Session-Id"] = session
        urllib.request.urlopen(
            urllib.request.Request(URL, data=body, headers=headers, method="POST"),
            timeout=30,
        ).read()
    except Exception as e:
        print("initialized notify:", e)

    if cmd == "decompile":
        addr = sys.argv[2]
        tag = sys.argv[3] if len(sys.argv) > 3 else addr.replace("0x", "")
        session, result, raw = rpc(
            "tools/call",
            {"name": "decompile", "arguments": {"addr": addr}},
            2,
            session,
        )
        open(f"{OUT}/ida_mcp_raw_{tag}.json", "w", encoding="utf-8").write(raw if raw.startswith("{") else json.dumps(result))
        # unwrap
        code = None
        if isinstance(result, dict):
            sc = result.get("result", {}).get("structuredContent")
            if isinstance(sc, dict) and "code" in sc:
                code = sc["code"]
            else:
                content = result.get("result", {}).get("content") or []
                for c in content:
                    if c.get("type") == "text":
                        try:
                            inner = json.loads(c["text"])
                            code = inner.get("code")
                        except Exception:
                            if c["text"].startswith("void") or c["text"].startswith("int"):
                                code = c["text"]
        if code:
            open(f"{OUT}/rea_decompile_{tag}.txt", "w", encoding="utf-8").write(code)
            print(f"OK {addr} code_len={len(code)}")
        else:
            print("NO_CODE", json.dumps(result)[:500])
            # check for /output uuid
            s = json.dumps(result)
            if "/output/" in s:
                print("has_output_ref")
    elif cmd == "find_regex":
        pattern = sys.argv[2]
        tag = sys.argv[3] if len(sys.argv) > 3 else "regex"
        session, result, raw = rpc(
            "tools/call",
            {"name": "find_regex", "arguments": {"pattern": pattern, "limit": 50}},
            2,
            session,
        )
        open(f"{OUT}/ida_find_regex_{tag}.json", "w", encoding="utf-8").write(
            json.dumps(result, indent=2, ensure_ascii=False)
        )
        # summarize
        matches = []
        try:
            sc = result["result"]["structuredContent"]
            matches = sc.get("matches") or []
            print(f"n={sc.get('n')} matches={len(matches)}")
            for m in matches[:20]:
                print(m)
        except Exception:
            content = (result.get("result") or {}).get("content") or []
            for c in content:
                print(c.get("text", "")[:500])
    elif cmd == "xrefs_to":
        addr = sys.argv[2]
        tag = sys.argv[3] if len(sys.argv) > 3 else addr.replace("0x", "")
        session, result, raw = rpc(
            "tools/call",
            {"name": "xrefs_to", "arguments": {"addrs": [addr]}},
            2,
            session,
        )
        open(f"{OUT}/ida_xrefs_{tag}.json", "w", encoding="utf-8").write(
            json.dumps(result, indent=2, ensure_ascii=False)
        )
        print(json.dumps(result)[:800])
    elif cmd == "list_tools":
        session, result, raw = rpc("tools/list", {}, 2, session)
        open(f"{OUT}/_mcp_tools.json", "w", encoding="utf-8").write(
            json.dumps(result, indent=2)[:200000]
        )
        tools = (result.get("result") or {}).get("tools") or []
        print("tools:", [t.get("name") for t in tools])
    else:
        print("usage: decompile|find_regex|xrefs_to|list_tools")


if __name__ == "__main__":
    main()
