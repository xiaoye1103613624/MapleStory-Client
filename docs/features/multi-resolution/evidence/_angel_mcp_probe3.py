# -*- coding: utf-8 -*-
"""Deeper Angel evidence: immediates in OnCreate, screen size globals, no multi-res."""
import json
import pathlib
import urllib.request

URL = "http://127.0.0.1:13339/mcp"
OUT = pathlib.Path(__file__).with_name("angel-resolution-immediates.json")
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
    resp = urllib.request.urlopen(req, timeout=180)
    sid = resp.headers.get("Mcp-Session-Id")
    raw = resp.read().decode("utf-8", "replace")
    return raw, sid


def call(tool, args, id_=1):
    raw, _ = rpc("tools/call", {"name": tool, "arguments": args}, id_=id_)
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
            return {"text": text[:80000]}
    return obj


def main():
    global SID
    raw, sid = rpc(
        "initialize",
        {
            "protocolVersion": "2024-11-05",
            "capabilities": {},
            "clientInfo": {"name": "cursor-agent", "version": "1.0"},
        },
        id_=1,
    )
    SID = sid
    rpc("notifications/initialized")

    out = {"session": SID, "health": call("server_health", {}, 2)}

    # Full OnCreate / ToggleQuickSlot / Draw / ChangeChatWndSize decompiles (truncated later)
    targets = {
        "OnCreate": "0x8d01b2",
        "ToggleQuickSlot": "0x8df4b1",
        "Draw": "0x8d80a4",
        "ChangeChatWndSize": "0x8d616c",
        "GetShortCutIndexByPos": "0x8de8d5",
        "ReDrawQuickslot": "0x8d82a7",
        "IWzGr2D_Initialize": None,
    }

    # Resolve IWzGr2D::Initialize
    look = call("lookup_funcs", {"queries": ["IWzGr2D::Initialize", "CWvsApp::InitializeGr2D"]}, 3)
    out["lookup_gr2d"] = look

    out["decompiles"] = {}
    for i, (name, addr) in enumerate(targets.items()):
        if not addr:
            continue
        print("decompile", name, addr)
        dec = call("decompile", {"addr": addr, "include_addresses": True}, 10 + i)
        code = ""
        if isinstance(dec, dict):
            code = dec.get("code") or dec.get("text") or json.dumps(dec, ensure_ascii=False)
            if isinstance(code, dict):
                code = code.get("code") or json.dumps(code, ensure_ascii=False)
        else:
            code = str(dec)
        out["decompiles"][name] = {"addr": addr, "code": code[:40000], "len": len(code)}

    # py_eval: scan OnCreate for push immediates of design constants
    out["imm_scan"] = call(
        "py_eval",
        {
            "new_locals": True,
            "code": r"""
import idaapi, idautils, idc, ida_funcs, ida_name, ida_ua, ida_bytes
fn = ida_funcs.get_func(0x8d01b2)
want = {800,600,578,151,466,571,647,533,515,543,573,629,685,741,768,618,648,678,708,738}
hits = {}
if fn:
    for ea in idautils.Heads(fn.start_ea, fn.end_ea):
        insn = ida_ua.insn_t()
        if ida_ua.decode_insn(insn, ea) == 0:
            continue
        for i in range(8):
            op = insn.ops[i]
            if op.type == ida_ua.o_void:
                break
            val = None
            if op.type in (ida_ua.o_imm,):
                val = op.value
            elif op.type in (ida_ua.o_displ, ida_ua.o_mem, ida_ua.o_near, ida_ua.o_far):
                val = op.addr
            if val in want:
                hits.setdefault(int(val), []).append(hex(ea))
# also search strings for resolution keywords
import idautils, idc
res_strs = []
for s in idautils.Strings():
    t = str(s)
    if any(k in t for k in ('1920','1280','1366','Resolution','soScreen','UpdateResolution','1024 x','800 x')):
        res_strs.append({'addr': hex(s.ea), 'str': t[:120]})
result = {
  'oncreate_size': fn.size() if fn else None,
  'imm_hits': {str(k): v[:30] for k,v in sorted(hits.items())},
  'resolution_like_strings': res_strs[:50],
  'resolution_like_count': len(res_strs),
}
""",
        },
        50,
    )

    # Also find_regex broader for 800 x / screen
    out["find_800x"] = call("find_regex", {"pattern": "800 x|1024 x|1280 x|1920 x|Screen Resolution|soScreen", "limit": 50}, 60)
    out["find_create_layer"] = call("find_regex", {"pattern": "base/quickSlot|base/backgrnd|LongQuickSlot", "limit": 50}, 61)

    OUT.write_text(json.dumps(out, ensure_ascii=False, indent=2), encoding="utf-8")
    print("WROTE", OUT)
    imm = out.get("imm_scan", {})
    print("imm_scan keys", imm.keys() if isinstance(imm, dict) else type(imm))
    print(json.dumps(imm, ensure_ascii=False)[:4000])
    print("find_800x", out["find_800x"])
    print("find_create_layer", out["find_create_layer"])


if __name__ == "__main__":
    main()
