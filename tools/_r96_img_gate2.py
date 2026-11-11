# -*- coding: utf-8 -*-
"""round96 128B IMG 门禁（重跑版）：把「预期内否定结果」与「脚本错误」分开记录。

- 预期内否定结果 → findings（如 Skill/main 不存在，正是 skill/D11 的证据）
- 脚本/协议错误 → errors（如 op: find 不支持）
"""
import base64
import json
import urllib.request

BASE = "http://127.0.0.1:10012/mcp"
IMG = "E:/MXD/BeiDou-Client-xy/Data/UI/UIWindow.img"
IV_B64 = "TSPHKw=="
KEYFILE = "tools/_gms_key_128.b64"
OUT = "tools/_ida_r96/img_gate2.json"
HDR = {"Content-Type": "application/json",
       "Accept": "application/json, text/event-stream",
       "MCP-Protocol-Version": "2025-03-26"}

KEY = open(KEYFILE, "rb").read().decode("ascii").strip()
BODY = {"name": "gms", "ivBase64": IV_B64, "userKeyBase64": KEY}

_id = [0]
SESSION = [None]


def post(payload, url=None):
    data = json.dumps(payload).encode("utf-8")
    req = urllib.request.Request(url or BASE, data=data, headers=HDR, method="POST")
    with urllib.request.urlopen(req, timeout=120) as r:
        raw = r.read()
        sid = r.headers.get("mcp-session-id")
        if sid:
            SESSION[0] = sid
        if not raw.strip():
            return {}
        return json.loads(raw.decode("utf-8", "replace"))


def notify(payload):
    h = dict(HDR)
    if SESSION[0]:
        h["mcp-session-id"] = SESSION[0]
    req = urllib.request.Request(BASE, data=json.dumps(payload).encode("utf-8"), headers=h, method="POST")
    try:
        with urllib.request.urlopen(req, timeout=30) as r:
            r.read()
    except Exception:
        pass


def rpc(method, params=None):
    _id[0] += 1
    pl = {"jsonrpc": "2.0", "id": _id[0], "method": method}
    if params is not None:
        pl["params"] = params
    h = dict(HDR)
    if SESSION[0]:
        h["mcp-session-id"] = SESSION[0]
    req = urllib.request.Request(BASE, data=json.dumps(pl).encode("utf-8"), headers=h, method="POST")
    with urllib.request.urlopen(req, timeout=180) as r:
        raw = r.read()
        sid = r.headers.get("mcp-session-id")
        if sid:
            SESSION[0] = sid
        return json.loads(raw.decode("utf-8", "replace"))


def call(name, args):
    res = rpc("tools/call", {"name": name, "arguments": args})
    return res.get("result", res)


def txt(res):
    out = []
    for c in res.get("content", []) or []:
        if c.get("type") == "text":
            out.append(c.get("text", ""))
    return "\n".join(out)


rpc("initialize", {"protocolVersion": "2025-03-26", "capabilities": {},
                   "clientInfo": {"name": "r96-gate2", "version": "1"}})
notify({"jsonrpc": "2.0", "method": "notifications/initialized"})

load = call("load_files", {"paths": [IMG], "key": BODY, "exclusive": False})
print("[load] isError", load.get("isError"))

queries = {}
findings = []
errors = []

# 1) 三个关键根的直接子节点
for path in ["Skill", "WorldMap"]:
    r = call("query_nodes", {"op": "list_children", "rootPath": IMG, "nodePath": path})
    t = txt(r)
    queries[path] = {"isError": bool(r.get("isError")), "text": t}
    print("[%s] isError %s len %d" % (path, r.get("isError"), len(t)))

# 2) Skill/main 是否存在（skill/D11 的核心证据）
r = call("query_nodes", {"op": "list_children", "rootPath": IMG, "nodePath": "Skill/main"})
t = txt(r)
queries["Skill/main"] = {"isError": bool(r.get("isError")), "text": t}
if r.get("isError") and "节点路径不存在" in t:
    findings.append("Skill/main 不存在 —— 083 Skill 无 main 子节点，故 main/BtHyper|BtGuildSkill|BtRide 恒空")
else:
    errors.append("Skill/main 期望不存在但返回: %s" % t[:200])

# 3) WorldMap/WorldMapSearch 是否存在（本端旧搜索轨恒空的原因）
r = call("query_nodes", {"op": "list_children", "rootPath": IMG, "nodePath": "WorldMap/WorldMapSearch"})
t = txt(r)
queries["WorldMap/WorldMapSearch"] = {"isError": bool(r.get("isError")), "text": t}
if r.get("isError") and "节点路径不存在" in t:
    findings.append("WorldMap/WorldMapSearch 不存在 —— 本端 UIWorldMap 旧搜索轨引用它恒为空（已默认隐藏）")
else:
    errors.append("WorldMap/WorldMapSearch 期望不存在但返回: %s" % t[:200])


def children_of(key):
    t = queries[key]["text"]
    try:
        d = json.loads(t)
    except Exception:
        return None
    node = d.get("result") or d
    ch = node.get("children")
    if not ch:
        return None
    return [c["name"] for c in ch]


skill_ch = children_of("Skill")
wm_ch = children_of("WorldMap")
print("[Skill] children", skill_ch)
print("[WorldMap] children", wm_ch)

if skill_ch and "main" not in skill_ch:
    findings.append("Skill 直接子节点 %d 个且无 main：" % len(skill_ch) + " / ".join(skill_ch))
if wm_ch:
    findings.append("WorldMap 直接子节点 %d 个：%s" % (len(wm_ch), " / ".join(wm_ch)))

call("unload_all", {})

verdict = "GATE_OK_128B_SKILL_NO_MAIN_WORLDMAP_TWO_NODES" if not errors else "GATE_ERRORS_PRESENT"
out = {
    "endpoint": BASE,
    "img": IMG,
    "iv_b64": IV_B64,
    "key_spec": "128B WzAESConstant.DEFAULT_KEY (orange-wz MCP path)",
    "key_b64_len": len(KEY),
    "load_isError": bool(load.get("isError")),
    "role": "Skill/WorldMap 节点枚举 + 预期内否定结果分类（findings 而非 errors）",
    "queries": queries,
    "children": {"Skill": skill_ch, "WorldMap": wm_ch},
    "findings": findings,
    "errors": errors,
    "verdict": verdict,
}
with open(OUT, "w", encoding="utf-8") as f:
    f.write(json.dumps(out, ensure_ascii=False, indent=2) + "\n")
print("\nVERDICT", verdict)
print("FINDINGS", len(findings), "ERRORS", len(errors))
print("WROTE", OUT)
