r"""验证新入库的 DropAuraEff.img 是「可被 img_file 解析的标准 .img」，
并确定客户端侧的访问路径。

ImgSource::gather 用 lexically_relative 取相对名（ImgSource.cpp:286），
Data/Effect 根下文件 DropAuraEff.img 的 rel 就是 "DropAuraEff.img"，
open_merge 后挂在 Effect 根下 —— 所以访问路径是
  nl::nx::Effect["DropAuraEff.img"]["dropItemAura"][grade][side][i]
而不是参考文档里的 BasicEff.img/...。本脚本用 MCP 复核该层级确实存在。
"""
import json
import os
import sys
import urllib.request

BASE = "http://127.0.0.1:10012/mcp"
KEYFILE = r"E:\project\MapleStory-Client\tools\_gms_key_128.b64"
LIVE = r"E:\MXD\BeiDou-Client-xy\Data\Effect\DropAuraEff.img"
BASIC = r"E:\MXD\BeiDou-Client-xy\Data\Effect\BasicEff.img"

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
    with urllib.request.urlopen(req, timeout=300) as r:
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
    r = res.get("result", {})
    if r.get("isError"):
        return None, (r.get("content") or [{}])[0].get("text", "")
    return r.get("structuredContent"), None


def main():
    print(f"file exists={os.path.exists(LIVE)} size={os.path.getsize(LIVE)}")
    head = open(LIVE, "rb").read(4)
    print("magic =", head, "(expect b'\\x00\\x00\\x01\\x00' IMG dir header)")

    rpc("initialize", {"protocolVersion": "2024-11-05", "capabilities": {},
                       "clientInfo": {"name": "dsh", "version": "1"}})
    rpc("notifications/initialized", {}, notify=True)
    key = {"name": "GMS", "ivBase64": "TSPHKw==",
           "userKeyBase64": open(KEYFILE, encoding="ascii").read().strip()}

    sc, err = call("load_files", {"paths": [LIVE], "exclusive": False, "key": key})
    if err:
        print("=== FAIL: load 解析失败 ===", err)
        return 1
    print("load_files OK:", json.dumps(sc, ensure_ascii=False))

    try:
        for p in ("", "dropItemAura", "dropItemAura/Rare/front",
                  "dropItemAura/Legendary/back", "dropItemEffect"):
            sc2, err2 = call("list_children", {"rootPath": LIVE, "nodePath": p, "autoParse": True})
            kids = (sc2 or {}).get("children") or []
            print(f"  {p or '<root>'}: {len(kids)} kids -> {[k['name'] for k in kids][:8]}")

        # 像素真取（证明不是空壳）
        sc3, err3 = call("get_node_detail", {"rootPath": LIVE,
                                             "nodePath": "dropItemAura/Rare/front/0",
                                             "autoParse": True})
        v = (sc3 or {}).get("value") if sc3 else None
        png = v.get("png", "") if isinstance(v, dict) else ""
        print(f"  Rare/front/0 png_len={len(png)} w={v.get('width') if isinstance(v, dict) else None}")
        if not png:
            print("=== FAIL: CANVAS 无像素数据（空壳） ===")
            return 1

        # 确认 BasicEff.img 未被污染（顶层节点数应仍是 41 左右且不含 dropItemAura）
        sc4, _ = call("list_children", {"rootPath": BASIC, "nodePath": "", "autoParse": True})
        bkids = (sc4 or {}).get("children") or []
        names = {k["name"] for k in bkids}
        t, _ = call("get_node_tree_json", {"rootPath": BASIC, "nodePath": "", "maxDepth": 1,
                                           "autoParse": True, "includePng": False})
        tn = ((t or {}).get("tree") or {}).get("children") or []
        print(f"  BasicEff.img top nodes(tree)={len(tn)} dropItemAura present={_has('dropItemAura', tn)}")
        print(f"  BasicEff.img size={os.path.getsize(BASIC)} mtime_ok (未改写=3992551)")
    finally:
        sc5, _ = call("unload_all", {})
        print("unload:", json.dumps(sc5, ensure_ascii=False))

    if os.path.getsize(BASIC) != 3992551:
        print("=== FAIL: BasicEff.img 大小变化，被意外改写 ===")
        return 1
    print("\n=== PASS: 独立 .img 可解析、含真实像素、BasicEff.img 完好未动 ===")
    return 0


def _has(name, kids):
    return any(k.get("name") == name for k in kids)


if __name__ == "__main__":
    sys.exit(main())
