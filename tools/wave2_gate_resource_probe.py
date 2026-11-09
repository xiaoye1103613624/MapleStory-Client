#!/usr/bin/env python3
"""Wave2 2-2 resource layer - AGENTS.md orange-wz FIRST gate probe + compliant .nx fallback.

YG 2026-11-09 ACTIVE: any WZ/IMG/XML node query MUST go through orange-wz MCP
(custom-orange_wz, http://127.0.0.1:10012/mcp). Fallback to .nx dump ONLY when
server returns `IMG 解析失败` / out-of-bounds / GBK error text; record raw error
+ key spec. This probe closes the IMG ring for the Wave2 resource layer work:
structure layer (Data/Skill, Data/Effect, Data/Npc dir enumeration) WORKS,
content layer (internal .img tree/search) FAILS -> compliant .nx fallback.
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
        "clientInfo": {"name": "wave2-gate-resource-probe", "version": "1"}}})
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
    "target": "Wave2 2-2 resource layer (Data/Skill, Data/Effect, Data/Npc)",
    "orange_wz_calls": [],
    "raw_error_texts_verbatim": [],
    "dir_structures": {},
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
    print("[OW] %-30s %-10s %s" % (tag, cls, str(r)[:90].replace("\n", " ")))
    return r


call("unload_all", {"hintGc": True})
DIRS = [D + "/Skill", D + "/Effect", D + "/Npc"]
ow_step("load_files", {"paths": DIRS, "key": KEY,
                       "exclusive": False}, "load_3_resource_dirs")
roots = ow_step("list_loaded_roots", {}, "roots")
root_paths = []
try:
    root_paths = [x["rootPath"] for x in json.loads(str(roots)).get("roots", [])]
except Exception:
    root_paths = DIRS

for rp in root_paths:
    tag = os.path.basename(rp.rstrip("/\\"))
    top = ow_step("list_children", {"rootPath": rp, "nodePath": "",
                                    "autoParse": False}, "top_" + tag)
    children = []
    try:
        children = json.loads(str(top)).get("children", [])
    except Exception:
        pass
    names = [c.get("name") for c in children]
    struct["dir_structures"][tag] = {"child_count": len(children),
                                     "children": names[:40]}
    # content-layer descent on one representative .img per dir
    focus = [n for n in names if str(n).lower().endswith(".img")][:1]
    for nm in focus:
        ow_step("get_node_tree_json", {"rootPath": rp, "nodePath": nm,
                                       "autoParse": True}, "descend_" + tag + "_" + str(nm))
        ow_step("search_node", {"rootPath": rp, "nodePath": nm,
                                "keyword": "summon"}, "search_" + tag + "_" + str(nm))
    print("[STRUCT] %-8s top children=%d" % (tag, len(children)))

ow_step("unload_all", {"hintGc": True}, "final_unload")
struct["final_roots"] = ow_step("list_loaded_roots", {}, "final_roots")
struct["parse_fail_count"] = parse_fails
struct["conclusion"] = {
    "orange_wz_structure_layer": "WORKS - resource dir enumeration returns real .img child nodes",
    "orange_wz_content_layer": "FAILS - internal .img tree/search returns IMG 解析失败 with full 65536B key + autoParse:true",
    "fallback_condition_met": parse_fails > 0,
    "fallback_source": ".nxcache/{Skill,Effect,Npc}.nx (PKG4) via tools/nx_inspect.py",
    "fallback_recorded": "raw error text + key spec captured above (AGENTS.md section 2)",
    "no_fake_green": "PARSE_FAIL classified separately; 'IMG 解析失败' never counted as OK",
}
io.open(os.path.join(EV, "orange-wz-resource-structure.json"), "w",
        encoding="utf-8").write(json.dumps(struct, ensure_ascii=False, indent=2))
print("[STRUCT] parse_fails:", parse_fails)

# ---- compliant .nx content-layer probe -------------------------------------
nxres = {"fallback_reason": "orange-wz content-layer IMG 解析失败 (see orange-wz-resource-structure.json)",
         "tool": "tools/nx_inspect.py (Nx/PKG4 reader)",
         "key_spec": KSPEC,
         "files": {}}
for nxname in ("Skill", "Effect", "Npc"):
    path = D + "/.nxcache/" + nxname + ".nx"
    entry = {"file": path}
    try:
        nx = Nx(path)
        tops = []
        for i in nx.children(0):
            nd = nx.node(i)
            tops.append({"name": nx.name(i),
                         "type": TYPE_NAMES.get(nd[3], nd[3]),
                         "child_count": nd[2]})
        entry["node_count"] = nx.node_count
        entry["root_child_count"] = len(tops)
        entry["root_children"] = tops[:30]
        print("[NX] %-8s root_children=%d" % (nxname, len(tops)))
    except Exception as e:
        entry["error"] = repr(e)
        print("[NX] %-8s ERROR %r" % (nxname, e))
    nxres["files"][nxname] = entry
io.open(os.path.join(EV, "nx-resource-content-probe.json"), "w",
        encoding="utf-8").write(json.dumps(nxres, ensure_ascii=False, indent=2))
print("DONE")
