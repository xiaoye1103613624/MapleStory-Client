# -*- coding: utf-8 -*-
"""Angel analyze_function + fixed immediate scan for multi-res plan."""
import json
import pathlib
import re
import urllib.request

URL = "http://127.0.0.1:13339/mcp"
OUT = pathlib.Path(__file__).with_name("angel-resolution-analysis.json")
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
    resp = urllib.request.urlopen(req, timeout=300)
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
            return {"text": text[:100000]}
    return obj


def extract_interesting(code: str):
    lines = []
    for line in code.splitlines():
        if re.search(
            r"800|600|578|151|466|571|647|533|515|543|573|629|685|741|768|"
            r"618|648|678|708|738|CreateCtrl|GetCanvas|CreateLayer|RelMove|"
            r"quickSlot|QuickSlot|uWidth|uHeight|1000|1005|1006|1007|2000",
            line,
            re.I,
        ):
            lines.append(line)
    return lines


def main():
    global SID
    _, sid = rpc(
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
    print("module", out["health"].get("module"))

    addrs = {
        "CUIStatusBar::OnCreate": "0x8d01b2",
        "CUIStatusBar::ToggleQuickSlot": "0x8df4b1",
        "CUIStatusBar::Draw": "0x8d80a4",
        "CUIStatusBar::ChangeChatWndSize": "0x8d616c",
        "CUIStatusBar::GetShortCutIndexByPos": "0x8de8d5",
        "CSlideNotice::OnCreate": "0x7e1686",
    }

    out["analyze"] = {}
    for i, (name, addr) in enumerate(addrs.items()):
        print("analyze_function", name)
        af = call("analyze_function", {"addr": addr}, 10 + i)
        # Keep compact: constants, strings, decompile head
        if isinstance(af, dict):
            code = af.get("decompiled") or af.get("pseudocode") or af.get("code") or ""
            if not code and "text" in af:
                code = af["text"]
            if isinstance(code, dict):
                code = code.get("code") or json.dumps(code)
            # also try nested
            if not code:
                for k in ("analysis", "result", "function"):
                    if isinstance(af.get(k), dict):
                        code = (
                            af[k].get("decompiled")
                            or af[k].get("pseudocode")
                            or af[k].get("code")
                            or ""
                        )
                        if code:
                            break
            compact = {
                "addr": addr,
                "name": name,
                "strings": af.get("strings") or af.get("string_refs"),
                "constants": af.get("constants") or af.get("immediates"),
                "callers": af.get("callers"),
                "callees": af.get("callees"),
                "interesting_lines": extract_interesting(str(code)[:80000]),
                "code_head": str(code)[:15000],
                "raw_keys": list(af.keys())[:40],
            }
            out["analyze"][name] = compact
        else:
            out["analyze"][name] = {"addr": addr, "raw": str(af)[:5000]}

    # Fixed imm scan
    out["imm_scan"] = call(
        "py_eval",
        {
            "new_locals": True,
            "code": r"""
import idaapi, idautils, idc, ida_funcs, ida_ua
fn = ida_funcs.get_func(0x8d01b2)
want = {800,600,578,151,466,571,647,533,515,543,573,629,685,741,768,618,648,678,708,738,407,22}
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
            if op.type == ida_ua.o_imm:
                val = int(op.value)
            if val in want:
                hits.setdefault(val, []).append(hex(ea))
# string scan
res_strs = []
for s in idautils.Strings():
    try:
        text = str(s)
    except Exception:
        continue
    low = text.lower()
    if ('1920' in text or '1280' in text or '1366' in text or 'resolution' in low
            or 'soscreen' in low or 'updateresolution' in low or '800 x' in low
            or '1024 x' in low):
        res_strs.append({'addr': hex(int(s.ea)), 'str': text[:160]})
# also check for GetScreenWidth-like names
names = []
for ea, name in idautils.Names():
    if any(k in name for k in ('Resolution','ScreenWidth','ScreenHeight','soScreen','QuickSlot','StatusBar')):
        names.append({'ea': hex(ea), 'name': name})
result = {
  'oncreate_size': int(fn.size()) if fn else None,
  'imm_hits': {str(k): v[:40] for k,v in sorted(hits.items())},
  'resolution_like_strings': res_strs[:80],
  'resolution_like_count': len(res_strs),
  'interesting_names_count': len(names),
  'interesting_names_sample': names[:80],
}
""",
        },
        100,
    )

    # Also decompile full OnCreate via decompile (may be large)
    print("full decompile OnCreate")
    dec = call("decompile", {"addr": "0x8d01b2", "include_addresses": True}, 200)
    code = ""
    if isinstance(dec, dict):
        code = dec.get("code") or ""
        if isinstance(code, dict):
            code = code.get("code") or json.dumps(code)
        if not code:
            code = dec.get("text") or json.dumps(dec, ensure_ascii=False)[:50000]
    out["oncreate_interesting"] = extract_interesting(str(code))
    out["oncreate_code_len"] = len(str(code))
    out["oncreate_head"] = str(code)[:20000]

    OUT.write_text(json.dumps(out, ensure_ascii=False, indent=2), encoding="utf-8")
    print("WROTE", OUT)
    print("imm", json.dumps(out.get("imm_scan"), ensure_ascii=False)[:5000])
    print("oncreate interesting lines", len(out["oncreate_interesting"]))
    for line in out["oncreate_interesting"][:60]:
        print(line)


if __name__ == "__main__":
    main()
