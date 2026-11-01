#!/usr/bin/env python3
"""Batch IDA MCP searches for r18b Wave0 retry items."""
from __future__ import annotations

import json
import re
import urllib.request

URL = "http://127.0.0.1:13338/mcp"
OUT = r"E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\evidence\r18\r18b"


def rpc(method, params, rid, session):
    body = {"jsonrpc": "2.0", "id": rid, "method": method}
    if params is not None:
        body["params"] = params
    data = json.dumps(body).encode()
    h = {
        "Content-Type": "application/json",
        "Accept": "application/json, text/event-stream",
    }
    if session:
        h["Mcp-Session-Id"] = session
    req = urllib.request.Request(URL, data=data, headers=h, method="POST")
    with urllib.request.urlopen(req, timeout=120) as resp:
        sess = resp.headers.get("Mcp-Session-Id") or session
        raw = resp.read().decode("utf-8", "replace")
    payload = None
    if "data:" in raw[:120]:
        for line in raw.splitlines():
            if line.startswith("data:"):
                payload = json.loads(line[5:].strip())
                break
    else:
        payload = json.loads(raw)
    return sess, payload, raw


def unwrap(result):
    if not isinstance(result, dict):
        return result
    r = result.get("result") or {}
    if "structuredContent" in r:
        return r["structuredContent"]
    content = r.get("content") or []
    for c in content:
        t = c.get("text")
        if not t:
            continue
        try:
            return json.loads(t)
        except Exception:
            return {"text": t}
    return r


def main():
    sess, _, _ = rpc(
        "initialize",
        {
            "protocolVersion": "2024-11-05",
            "capabilities": {},
            "clientInfo": {"name": "r18b-batch", "version": "1"},
        },
        1,
        None,
    )
    try:
        urllib.request.urlopen(
            urllib.request.Request(
                URL,
                data=json.dumps(
                    {
                        "jsonrpc": "2.0",
                        "method": "notifications/initialized",
                        "params": {},
                    }
                ).encode(),
                headers={
                    "Content-Type": "application/json",
                    "Accept": "application/json, text/event-stream",
                    "Mcp-Session-Id": sess,
                },
                method="POST",
            ),
            timeout=30,
        ).read()
    except Exception:
        pass

    rid = 2
    jobs = []

    # ASCII / path fragments
    for pat, tag in [
        ("BtCashShop", "BtCashShop"),
        ("CashShop", "CashShop"),
        ("BtNPT", "BtNPT"),
        ("BtShort", "BtShort"),
        ("BtMenu", "BtMenu"),
        ("KeyConfig", "KeyConfig"),
        ("QuickSlot", "QuickSlot2"),
        ("WorldSelect", "WorldSelect"),
        ("ChannelSelect", "ChannelSelect"),
        ("SelectWorld", "SelectWorld"),
        ("CharSelect", "CharSelect2"),
        ("CUICharSelect", "CUICharSelect"),
        ("Rank", "Rank"),
        ("MakeCharInfo", "MakeCharInfo"),
        ("Login.img", "Login_img"),
    ]:
        jobs.append(("find_regex", {"pattern": pat, "limit": 30}, tag))

    # GBK: 排行不可用 = BF C5 D0 D0 B2 BB BF C9 D3 C3
    jobs.append(
        (
            "find_bytes",
            {"patterns": ["BF C5 D0 D0 B2 BB BF C9 D3 C3"], "limit": 20},
            "gbk_paihang_bukeyong",
        )
    )
    # GBK: 到选择的世界 — 到=B5BD 选=D1A1 择=D4F1 的=B5C4 世=CAC0 界=BDE7
    jobs.append(
        (
            "find_bytes",
            {"patterns": ["B5 BD D1 A1 D4 F1 B5 C4 CA C0 BD E7"], "limit": 20},
            "gbk_dao_xuanze_shijie",
        )
    )
    # UTF-16LE "Rank" / "charInfo" light
    jobs.append(
        (
            "find_bytes",
            {"patterns": ["63 00 68 00 61 00 72 00 49 00 6e 00 66 00 6f 00"], "limit": 20},
            "utf16_charInfo",
        )
    )
    jobs.append(
        (
            "find_bytes",
            {"patterns": ["52 00 61 00 6e 00 6b 00"], "limit": 30},
            "utf16_Rank",
        )
    )
    # push 1000 / push 1005 near code as bytes (x86 little-endian imm32)
    # 68 E8 03 00 00 = push 1000
    jobs.append(
        (
            "find_bytes",
            {"patterns": ["68 E8 03 00 00"], "limit": 50},
            "push_1000",
        )
    )
    jobs.append(
        (
            "find_bytes",
            {"patterns": ["68 ED 03 00 00"], "limit": 50},
            "push_1005",
        )
    )

    summary = {}
    for tool, args, tag in jobs:
        sess, result, raw = rpc("tools/call", {"name": tool, "arguments": args}, rid, sess)
        rid += 1
        data = unwrap(result)
        path = f"{OUT}/ida_{tool}_{tag}.json"
        open(path, "w", encoding="utf-8").write(
            json.dumps(data, indent=2, ensure_ascii=False)[:200000]
        )
        # normalize summary
        n = None
        sample = []
        if isinstance(data, dict):
            if "n" in data:
                n = data.get("n")
                sample = (data.get("matches") or [])[:8]
            elif "matches" in data:
                sample = data["matches"][:8]
                n = len(data["matches"])
            elif "result" in data and isinstance(data["result"], list):
                sample = data["result"][:8]
                n = len(data["result"])
            elif "text" in data:
                sample = [data["text"][:200]]
        summary[tag] = {"n": n, "sample": sample, "file": path}
        print(f"{tag}: n={n} sample={sample[:3]}")

    open(f"{OUT}/r18b_search_summary.json", "w", encoding="utf-8").write(
        json.dumps(summary, indent=2, ensure_ascii=False)
    )


if __name__ == "__main__":
    main()
