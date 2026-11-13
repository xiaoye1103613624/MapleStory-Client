r"""orange-wz MCP 探针：枚举参考 Effect.wz 的 dropItemAura / dropItemEffect 节点树。

用 ASCII staging 路径（中文目录名传到 Java 侧会变成 `?` → Illegal char，已实测）。
key 规格按 AGENTS.md §3：128B WzAESConstant.DEFAULT_KEY base64 + iv TSPHKw==。
最后必须 unload_all 收口，避免根锁残留。
"""
import base64
import json
import sys
import urllib.request

BASE = "http://127.0.0.1:10012/mcp"
WZ = r"E:\project\MapleStory-Client\tools\_staging\ref_effect.wz"
KEYFILE = r"E:\project\MapleStory-Client\tools\_gms_key_128.b64"

sid = None


def rpc(method, params=None, mid=[0], notify=False):
    global sid
    mid[0] += 1
    body = {"jsonrpc": "2.0", "method": method}
    if not notify:
        body["id"] = mid[0]
    if params is not None:
        body["params"] = params
    data = json.dumps(body).encode()
    req = urllib.request.Request(BASE, data=data, method="POST")
    req.add_header("Content-Type", "application/json")
    req.add_header("Accept", "application/json, text/event-stream")
    if sid:
        req.add_header("Mcp-Session-Id", sid)
    with urllib.request.urlopen(req, timeout=180) as r:
        if notify:
            return r.status
        if sid is None:
            sid = r.headers.get("Mcp-Session-Id") or r.headers.get("mcp-session-id")
        raw = r.read().decode("utf-8", "replace")
        # streamable-http 可能回 SSE
        for line in raw.splitlines():
            if line.startswith("data:"):
                raw = line[5:].strip()
                break
        return json.loads(raw)


def call(name, arguments):
    res = rpc("tools/call", {"name": name, "arguments": arguments})
    r = res.get("result", res)
    if r.get("isError"):
        txt = (r.get("content") or [{}])[0].get("text", "")
        # 服务端错误文本是 GBK，尝试还原
        try:
            txt2 = base64.b64decode(txt).decode("gbk", "replace") if False else txt
        except Exception:
            txt2 = txt
        raise RuntimeError(f"{name} -> isError: {txt2}")
    sc = r.get("structuredContent")
    if sc is None and r.get("content"):
        try:
            sc = json.loads(r["content"][0]["text"])
        except Exception:
            sc = r["content"][0]["text"]
    return sc


def walk(node, depth=0, maxdepth=3, lines=None):
    lines = lines if lines is not None else []
    kids = node.get("children") or []
    meta = f"{'  ' * depth}{node.get('name')} [{node.get('type')}] kids={len(kids)}"
    if node.get("width") is not None:
        meta += f" w={node.get('width')} h={node.get('height')} fmt={node.get('pngFormat')}"
    lines.append(meta)
    if depth < maxdepth:
        for k in kids:
            walk(k, depth + 1, maxdepth, lines)
    return lines


def main():
    rpc("initialize", {"protocolVersion": "2024-11-05", "capabilities": {},
                       "clientInfo": {"name": "dsh-probe", "version": "1.0"}})
    rpc("notifications/initialized", {}, notify=True)
    key = open(KEYFILE, encoding="ascii").read().strip()

    call("load_files", {"paths": [WZ], "exclusive": False,
                        "key": {"name": "GMS", "ivBase64": "TSPHKw==", "userKeyBase64": key}})
    try:
        for path, md in (("BasicEff.img/dropItemAura", 4), ("BasicEff.img/dropItemEffect", 1)):
            tree = call("get_node_tree_json", {"rootPath": WZ, "nodePath": path,
                                               "maxDepth": md, "autoParse": True,
                                               "includePng": False})
            t = tree.get("tree", tree)
            print(f"===== {path} =====")
            for ln in walk(t, maxdepth=md):
                print(ln)
    finally:
        print("unload_all ->", call("unload_all", {}))

    return 0


if __name__ == "__main__":
    sys.exit(main())
