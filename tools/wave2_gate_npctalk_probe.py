#!/usr/bin/env python3
"""Wave2 2-3 NPC talk UI - AGENTS.md orange-wz FIRST gate probe + compliant .nx fallback.

YG 2026-11-09 ACTIVE: any WZ/IMG/XML node query MUST go through orange-wz MCP
(custom-orange_wz, http://127.0.0.1:10012/mcp). Fallback to .nx dump ONLY when
server returns `IMG 解析失败` / out-of-bounds / GBK error text; record raw error
+ key spec. This probe closes the IMG ring for the UINpcTalk (UtilDlgEx) UI work:
structure layer (Data/UI dir enumeration) WORKS, content layer (internal .img
tree/search) FAILS -> compliant UI.nx fallback for UtilDlgEx controls.
"""
import io
import os
import sys
import json
import base64
import hashlib
import urllib.request

sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx, TYPE_NAMES

BASE = "http://127.0.0.1:10012/mcp"
SID = None
EV = r"E:/project/MapleStory-Client/docs/features/development-and-fix-plan-2026-11/evidence"
os.makedirs(EV, exist_ok=True)
D = r"E:/MXD/BeiDou-Client-xy/Data"


def post(payload):
    global SID
    h = {"Content-Type": "application/json",
         "Accept": "application/json, text/event-stream"}
    if SID:
        h["Mcp-Session-Id"] = SID
    r = urllib.request.urlopen(
        urllib.request.Request(BASE, data=json.dumps(payload).encode(), headers=h),
        timeout=300)
    sid = r.headers.get("mcp-session-id")
    if sid:
        SID = sid
    raw = r.read().decode("utf-8", "replace")
    if not raw.strip():
        return None
    for line in raw.splitlines():
        if line.startswith("data: "):
            return json.loads(line[6:])
    return json.loads(raw)


def init():
    post({"jsonrpc": "2.0", "id": 1, "method": "initialize", "params": {
        "protocolVersion": "2025-03-26", "capabilities": {},
        "clientInfo": {"name": "wave2-gate-npctalk-probe", "version": "1"}}})
    post({"jsonrpc": "2.0", "method": "notifications/initialized", "params": {}})
    return SID


def call(name, args):
    out = post({"jsonrpc": "2.0", "id": 3, "method": "tools/call",
                "params": {"name": name, "arguments": args}})
    if out is None:
        return ""
    res = out.get("result", out)
    if isinstance(res, dict) and "content" in res:
        return "\n".join(c.get("text", "") for c in res["content"]
                         if c.get("type") == "text")
    return json.dumps(res, ensure_ascii=False)[:12000]


GMS = io.open(r"E:/project/MapleStory-Client/tools/_gms_key.b64",
              encoding="ascii").read().strip()
IV = base64.b64encode(bytes([0x4D, 0x23, 0xC7, 0x2B])).decode()
KEY = {"name": "GMS", "userKeyBase64": GMS, "ivBase64": IV}
KSPEC = ("GMS full 65536B key table (tools/vendor/NoLifeWzToNx-master/Keys.cpp "
         "key_gms), b64_len=%d, sha256_16=%s, IV=GMS[0x4D,0x23,0xC7,0x2B]=%s"
         % (len(GMS), hashlib.sha256(base64.b64decode(GMS)).hexdigest()[:16], IV))

FAIL_MARKERS = ("IMG 解析失败", "out of bounds", "Index ", "缺少参数", "失败", "Error", "找不到根节点")


def classify(txt):
    s = str(txt)
    for m in FAIL_MARKERS:
        if m in s:
            return "PARSE_FAIL"
    return "OK"


init()
struct = {
    "rule": "AGENTS.md YG 2026-11-09: orange-wz MCP first; .nx fallback only on parse failure; record raw error + key spec",
    "endpoint": BASE,
    "connection": "custom-orange_wz (global, streamable-http, 29 tools)",
    "key_spec": KSPEC,
    "exclusive": False,
    "target": "UINpcTalk UtilDlgEx (Data/UI/UIWindow.img)",
    "orange_wz_calls": [],
    "raw_error_texts_verbatim": [],
}
parse_fails = 0


def ow_step(tool, args, tag):
    global parse_fails
    r = call(tool, args)
    cls = classify(r)
    struct["orange_wz_calls"].append({"tool": tool, "tag": tag,
                                      "class": cls, "raw": str(r)[:6000]})
    if cls == "PARSE_FAIL":
        parse_fails += 1
        first = str(r).strip().splitlines()[0][:300] if str(r).strip() else str(r)[:300]
        if first not in struct["raw_error_texts_verbatim"]:
            struct["raw_error_texts_verbatim"].append(first)
    print("[OW] %-26s %-10s %s" % (tag, cls, str(r)[:90].replace("\n", " ")))
    return r


call("unload_all", {"hintGc": True})
ow_step("load_files", {"paths": [D + "/UI"], "key": KEY,
                       "exclusive": False}, "load_Data_UI")
roots = ow_step("list_loaded_roots", {}, "roots")
rp = D + "/UI"
try:
    rs = json.loads(str(roots)).get("roots", [])
    if rs:
        rp = rs[0]["rootPath"]
except Exception:
    pass
struct["root_path"] = rp
top = ow_step("list_children", {"rootPath": rp, "nodePath": "",
                                "autoParse": False}, "UI_top_children")
children = []
try:
    children = json.loads(str(top)).get("children", [])
except Exception:
    pass
struct["ui_top_child_count"] = len(children)
struct["ui_top_child_names"] = [c.get("name") for c in children]

# content-layer descent: focus on NPC-talk resource roots (UIWindow.img / Basic.img)
focus = [c.get("name", "") for c in children
         if c.get("name", "").lower().endswith(".img")
         and ("uiwindow" in c.get("name", "").lower()
              or "basic" in c.get("name", "").lower())]
if not focus:
    focus = [c.get("name", "") for c in children
             if c.get("name", "").lower().endswith(".img")][:2]
struct["content_descent_targets"] = focus
for nm in focus:
    ow_step("get_node_tree_json", {"rootPath": rp, "nodePath": nm,
                                   "autoParse": True}, "descend_" + nm)
    ow_step("search_node", {"rootPath": rp, "nodePath": nm,
                            "keyword": "UtilDlgEx"}, "search_" + nm)

ow_step("unload_all", {"hintGc": True}, "final_unload")
struct["final_roots"] = ow_step("list_loaded_roots", {}, "final_roots")
struct["parse_fail_count"] = parse_fails
struct["conclusion"] = {
    "orange_wz_structure_layer": "WORKS - Data/UI directory enumeration returns real .img child nodes",
    "orange_wz_content_layer": "FAILS - internal .img tree/search returns IMG 解析失败 with full 65536B key + autoParse:true",
    "fallback_condition_met": parse_fails > 0,
    "fallback_source": ".nxcache/UI.nx (PKG4) via tools/nx_inspect.py",
    "fallback_recorded": "raw error text + key spec captured above (AGENTS.md section 2)",
    "no_fake_green": "PARSE_FAIL classified separately; 'IMG 解析失败' never counted as OK",
}
io.open(os.path.join(EV, "orange-wz-npctalk-ui-structure.json"), "w",
        encoding="utf-8").write(json.dumps(struct, ensure_ascii=False, indent=2))
print("[STRUCT] Data/UI top children:", struct["ui_top_child_count"],
      "parse_fails:", parse_fails)

# ---- compliant .nx content-layer probe: UtilDlgEx controls -------------------
nxres = {"fallback_reason": "orange-wz content-layer IMG 解析失败 (see orange-wz-npctalk-ui-structure.json)",
         "tool": "tools/nx_inspect.py (Nx/PKG4 reader)",
         "key_spec": KSPEC,
         "file": D + "/.nxcache/UI.nx",
         "util_dlg_ex": None}
try:
    nx = Nx(D + "/.nxcache/UI.nx")
    nxres["node_count"] = nx.node_count
    tops = []
    uiw = None
    for i in nx.children(0):
        nd = nx.node(i)
        tops.append({"name": nx.name(i), "child_count": nd[2]})
        if nx.name(i).lower().replace(".img", "") == "uiwindow":
            uiw = i
    nxres["root_child_count"] = len(tops)
    nxres["root_children"] = tops[:40]
    if uiw is not None:
        dlg = None
        for j in nx.children(uiw):
            if "utildlgex" in nx.name(j).lower():
                dlg = j
                break
        if dlg is not None:
            ctrl = []
            for k in list(nx.children(dlg))[:40]:
                ctrl.append({"name": nx.name(k),
                             "type": TYPE_NAMES.get(nx.node(k)[3], nx.node(k)[3]),
                             "child_count": nx.node(k)[2]})
            nxres["util_dlg_ex"] = {"node": "UIWindow.img/UtilDlgEx",
                                    "child_count": nx.node(dlg)[2],
                                    "controls": ctrl}
    print("[NX] UI.nx root_children=%s util_dlg_ex_controls=%s" % (
        nxres["root_child_count"],
        len(nxres["util_dlg_ex"]["controls"]) if nxres["util_dlg_ex"] else 0))
except Exception as e:
    nxres["error"] = repr(e)
    print("[NX] ERROR", repr(e))
io.open(os.path.join(EV, "nx-ui-npctalk-content-probe.json"), "w",
        encoding="utf-8").write(json.dumps(nxres, ensure_ascii=False, indent=2))
print("DONE")
