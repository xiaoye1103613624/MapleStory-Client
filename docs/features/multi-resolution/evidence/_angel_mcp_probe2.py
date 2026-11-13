# -*- coding: utf-8 -*-
"""Angel IDA MCP probe v2 — re-init session + light queries + targeted decompiles."""
import json
import pathlib
import urllib.request

URL = "http://127.0.0.1:13339/mcp"
OUT = pathlib.Path(__file__).with_name("angel-resolution-probe.json")
SID = None


def rpc(method, params=None, id_=None):
    body = {"jsonrpc": "2.0", "method": method}
    if id_ is not None:
        body["id"] = id_
    if params is not None:
        body["params"] = params
    headers = {
        "Content-Type": "application/json",
        "Accept": "application/json, text/event-stream",
    }
    if SID:
        headers["Mcp-Session-Id"] = SID
    req = urllib.request.Request(URL, data=json.dumps(body).encode(), headers=headers)
    resp = urllib.request.urlopen(req, timeout=120)
    global_sid = resp.headers.get("Mcp-Session-Id")
    raw = resp.read().decode("utf-8", "replace")
    return raw, global_sid, resp.headers


def call(tool, args, id_=1):
    raw, _, _ = rpc(
        "tools/call",
        {"name": tool, "arguments": args},
        id_=id_,
    )
    obj = json.loads(raw)
    if obj.get("error"):
        return {"error": obj["error"]}
    result = obj.get("result", {})
    sc = result.get("structuredContent")
    if sc is not None:
        return sc
    content = result.get("content", [])
    if content and content[0].get("type") == "text":
        text = content[0]["text"]
        try:
            return json.loads(text)
        except Exception:
            return {"text": text[:50000]}
    return obj


def main():
    global SID
    raw, sid, _ = rpc(
        "initialize",
        {
            "protocolVersion": "2024-11-05",
            "capabilities": {},
            "clientInfo": {"name": "cursor-agent", "version": "1.0"},
        },
        id_=1,
    )
    SID = sid
    print("session", SID)
    rpc("notifications/initialized")

    out = {"session": SID}
    out["server_health"] = call("server_health", {}, 2)
    print("module", out["server_health"].get("module"), out["server_health"].get("idb_path"))

    out["lookup"] = call(
        "lookup_funcs",
        {
            "queries": [
                "CUIStatusBar::OnCreate",
                "CUIStatusBar::ToggleQuickSlot",
                "CUIStatusBar::Draw",
                "CUIStatusBar::OnMoveWnd",
                "CQuickSlot::OnCreate",
                "CQuickSlot::CreateWnd",
                "CWvsApp::InitializeGr2D",
                "IWzGr2D::Initialize",
                "CWnd::CreateWnd",
                "CSlideNotice::OnCreate",
            ]
        },
        3,
    )
    print("lookup done")

    out["func_query_statusbar"] = call(
        "func_query",
        {"queries": {"name_regex": "CUIStatusBar", "count": 60}},
        4,
    )
    print("statusbar funcs", out["func_query_statusbar"])

    out["func_query_quickslot"] = call(
        "func_query",
        {"queries": {"name_regex": "QuickSlot|CQuickSlot", "count": 40}},
        5,
    )
    print("quickslot funcs", out["func_query_quickslot"])

    out["func_query_resolution"] = call(
        "func_query",
        {"queries": {"name_regex": "Resolution|UpdateResolution|ScreenWidth", "count": 40}},
        6,
    )
    print("resolution funcs", out["func_query_resolution"])

    out["find_regex"] = call(
        "find_regex",
        {
            "pattern": "1920|1280|1366|UpdateResolution|soScreenResolution|quickSlot|StatusBar\\.img",
            "limit": 100,
        },
        7,
    )
    print("find_regex n=", out["find_regex"])

    # Targeted decompiles for key addresses from prior calibration (0x8d01b2 OnCreate)
    addrs = []
    lookup = out["lookup"]
    # lookup may be list or dict depending on MCP version
    if isinstance(lookup, dict):
        # try common shapes
        items = lookup.get("result") or lookup.get("functions") or lookup.get("data") or []
        if isinstance(items, list):
            for it in items:
                if isinstance(it, dict) and it.get("addr"):
                    addrs.append((it.get("name") or it.get("fn") or "?", it["addr"]))
        else:
            for k, v in lookup.items():
                if isinstance(v, dict) and v.get("addr"):
                    addrs.append((k, v["addr"]))
                elif isinstance(v, str) and v.startswith("0x"):
                    addrs.append((k, v))
    elif isinstance(lookup, list):
        for it in lookup:
            if isinstance(it, dict):
                addr = it.get("addr") or it.get("address")
                name = it.get("name") or it.get("fn") or it.get("query")
                if addr:
                    addrs.append((name, addr))

    # Always include known Angel OnCreate / ToggleQuickSlot from prior docs
    known = [
        ("CUIStatusBar::OnCreate", "0x8d01b2"),
        ("CUIStatusBar::ToggleQuickSlot", "0x8df4b1"),
        ("CSlideNotice::OnCreate", "0x7e1686"),
    ]
    for name, addr in known:
        if not any(a == addr for _, a in addrs):
            addrs.append((name, addr))

    out["decompiles"] = {}
    for i, (name, addr) in enumerate(addrs[:8]):
        print("decompile", name, addr)
        dec = call("decompile", {"addr": addr}, 100 + i)
        # keep truncated for plan evidence
        if isinstance(dec, dict):
            code = dec.get("code") or dec.get("text") or json.dumps(dec, ensure_ascii=False)
        else:
            code = str(dec)
        out["decompiles"][name] = {
            "addr": addr,
            "code_head": code[:12000],
            "code_len": len(code),
        }

    # Constants / immediates related to 800 design grid via py_eval
    out["py_constants"] = call(
        "py_eval",
        {
            "code": (
                "import idautils,idc,ida_bytes,ida_funcs,ida_name\n"
                "hits=[]\n"
                "# scan CUIStatusBar::OnCreate for immediates 800/600/578/151/466\n"
                "ea=ida_name.get_name_ea(ida_idaapi.BADADDR,'CUIStatusBar::OnCreate')\n"
                "if ea==ida_idaapi.BADADDR: ea=0x8d01b2\n"
                "fn=ida_funcs.get_func(ea)\n"
                "imm_hits={}\n"
                "if fn:\n"
                "  for h in idautils.Heads(fn.start_ea, fn.end_ea):\n"
                "    for i in range(ida_ua.UA_MAXOP):\n"
                "      op=idc.get_operand_value(h,i)\n"
                "      if op in (800,600,578,151,466,647,533,515,543,741,768):\n"
                "        imm_hits.setdefault(op,[]).append(hex(h))\n"
                "result={'oncreate_ea':hex(ea),'imm_hits':{str(k):v[:20] for k,v in imm_hits.items()}}\n"
            ),
            "new_locals": True,
        },
        200,
    )

    OUT.write_text(json.dumps(out, ensure_ascii=False, indent=2), encoding="utf-8")
    print("WROTE", OUT)
    print("decompiles keys", list(out["decompiles"].keys()))


if __name__ == "__main__":
    main()
