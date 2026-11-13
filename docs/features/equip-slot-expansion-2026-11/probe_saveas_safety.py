r"""最小实验：create_child_node(增量新增) + save_as 是否会丢 live 的既有节点？

目的：上一轮 copy_paste_nodes(OVERWRITE)+save_as 产出的 OUT 只剩 2 个顶层节点，
live 41 个全丢。必须先定位是哪一步丢的，再决定入库路线，绝不拿 live 试错。

实验设计（全程 staging，不碰 live）：
  A. staging 副本 → save_as(未改动) → 比较顶层节点数  ⇒ 单独验证 save_as
  B. staging 副本 → create_child_node(新增 1 个 LIST) → save_as → 比较  ⇒ 验证增量新增路径
判据：A 若已丢 ⇒ save_as 对 .img 根保存不安全，改用「独立 .img 文件」路线。
"""
import json
import os
import shutil
import sys
import urllib.request

BASE = "http://127.0.0.1:10012/mcp"
KEYFILE = r"E:\project\MapleStory-Client\tools\_gms_key_128.b64"
LIVE = r"E:\MXD\BeiDou-Client-xy\Data\Effect\BasicEff.img"
STG_A = r"E:\project\MapleStory-Client\tools\_staging\basisave_a.img"
OUT_A = r"E:\project\MapleStory-Client\tools\_staging\basisave_a_out.img"
STG_B = r"E:\project\MapleStory-Client\tools\_staging\basient_b.img"
OUT_B = r"E:\project\MapleStory-Client\tools\_staging\basient_b_out.img"
# C 臂：专测 copy_paste_nodes 本身（不 save_as，只看内存树），定位丢数据的那一步
STG_C = r"E:\project\MapleStory-Client\tools\_staging\basient_c.img"
REF = r"E:\project\MapleStory-Client\tools\_staging\ref_effect.wz"

sid = None
_mid = [0]


def rpc(method, params=None, notify=False):
    global sid
    _mid[0] += 1
    body = {"jsonrpc": "2.0", "method": method}
    if not notify:
        body["id"] = _mid[0]
    if params is not None:
        body["params"] = params
    req = urllib.request.Request(BASE, data=json.dumps(body).encode(), method="POST")
    req.add_header("Content-Type", "application/json")
    req.add_header("Accept", "application/json, text/event-stream")
    if sid:
        req.add_header("Mcp-Session-Id", sid)
    with urllib.request.urlopen(req, timeout=600) as r:
        if notify:
            return r.status
        if sid is None:
            sid = r.headers.get("Mcp-Session-Id")
        raw = r.read().decode("utf-8", "replace")
        for ln in raw.splitlines():
            if ln.startswith("data:"):
                raw = ln[5:].strip()
                break
        return json.loads(raw)


def call(name, args):
    res = rpc("tools/call", {"name": name, "arguments": args})
    if "error" in res:
        raise RuntimeError(f"{name}: {res['error']}")
    r = res.get("result", {})
    if r.get("isError"):
        raise RuntimeError(f"{name} isError: {(r.get('content') or [{}])[0].get('text', '')}")
    return r.get("structuredContent") or (json.loads(r["content"][0]["text"]) if r.get("content") else None)


def toplevel(root):
    t = call("get_node_tree_json", {"rootPath": root, "nodePath": "", "maxDepth": 1,
                                    "autoParse": True, "includePng": False})
    n = (t or {}).get("tree") or {}
    return {c["name"] for c in (n.get("children") or [])}


def prep(dst):
    if os.path.exists(dst):
        os.remove(dst)
    shutil.copy2(LIVE, dst)


def main():
    for f in (STG_A, OUT_A, STG_B, OUT_B):
        if os.path.exists(f):
            os.remove(f)
    prep(STG_A)
    prep(STG_B)
    prep(STG_C)

    rpc("initialize", {"protocolVersion": "2024-11-05", "capabilities": {},
                       "clientInfo": {"name": "dsh", "version": "1"}})
    rpc("notifications/initialized", {}, notify=True)
    key = {"name": "GMS", "ivBase64": "TSPHKw==",
           "userKeyBase64": open(KEYFILE, encoding="ascii").read().strip()}

    try:
        call("load_files", {"paths": [LIVE, STG_A, STG_B, REF], "exclusive": False, "key": key})

        base = toplevel(LIVE)
        stga = toplevel(STG_A)
        print(f"LIVE top={len(base)}  STG_A top={len(stga)}  identical={base == stga}")

        # A) 未改动直接 save_as
        call("save_as", {"rootPath": STG_A, "filePath": OUT_A, "nodePath": "",
                         "autoParse": True, "unloadAfterSave": False, "clearCache": True})
        # OUT_A 需要重新加载才能查
        call("load_files", {"paths": [OUT_A], "exclusive": False, "key": key})
        outa = toplevel(OUT_A)
        print(f"A) save_as(no-op) -> OUT_A top={len(outa)} size={os.path.getsize(OUT_A)}"
              f"  lost={len(base - outa)}")

        # B) 先新增一个 LIST 节点再 save_as
        call("create_child_node", {"rootPath": STG_B, "nodePath": "", "name": "_probe_node",
                                   "type": "LIST", "autoParse": True})
        stgb = toplevel(STG_B)
        print(f"B) after create_child: STG_B top={len(stgb)} contains_probe={'_probe_node' in stgb}")
        call("save_as", {"rootPath": STG_B, "filePath": OUT_B, "nodePath": "",
                         "autoParse": True, "unloadAfterSave": False, "clearCache": True})
        call("load_files", {"paths": [OUT_B], "exclusive": False, "key": key})
        outb = toplevel(OUT_B)
        print(f"B) save_as(+1 node) -> OUT_B top={len(outb)} size={os.path.getsize(OUT_B)}"
              f"  lost={len(base - outb)} probe_kept={'_probe_node' in outb}")

        verdict_a = "SAFE" if len(base - outa) == 0 else "DROPS DATA"
        verdict_b = "SAFE" if len(base - outb) == 0 and "_probe_node" in outb else "DROPS DATA"
        print(f"\n=== save_as 未改动: {verdict_a} | save_as 增量新增: {verdict_b} ===")
        print("=== 结论：copy_paste_nodes(OVERWRITE→root) 是丢数据的那一步 ==="
              if verdict_a == "SAFE" else "=== 结论：save_as 本身不安全，必须走独立 .img 路线 ===")
        return 0 if verdict_a == "SAFE" else 1
    finally:
        print("unload:", call("unload_all", {}))


if __name__ == "__main__":
    sys.exit(main())
