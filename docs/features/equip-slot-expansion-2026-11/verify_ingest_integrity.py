r"""⑥ 素材入库完整性对账（替换 live 前的强制门禁）。

背景：copy_paste_nodes(OVERWRITE) + save_as 产出的 OUT 只有 116,221B，
而 staged 输入是 3,992,551B —— 缩小 34 倍。替换前必须证明节点树没有丢，
否则就是在毁 live 的 BasicEff.img（全地图特效都在这一个文件里）。

判据：
  1) OUT 的顶层子节点数 == LIVE 的顶层子节点数（一个都不能少）
  2) OUT 里 LIVE 存在的每个顶层节点名都在（集合相等）
  3) OUT 新增 dropItemAura（4 档 × front/back × 6 帧）与 dropItemEffect（6 帧）
  4) 抽样若干既有节点（含 CANVAS 尺寸）在 OUT 与 LIVE 中一致
"""
import json
import sys
import urllib.request

BASE = "http://127.0.0.1:10012/mcp"
KEYFILE = r"E:\project\MapleStory-Client\tools\_gms_key_128.b64"
LIVE = r"E:\MXD\BeiDou-Client-xy\Data\Effect\BasicEff.img"
OUT = r"E:\project\MapleStory-Client\tools\_staging\BasicEff_aura_out.img"

sid = None
_mids = [0]


def rpc(method, params=None, notify=False):
    global sid
    _mids[0] += 1
    body = {"jsonrpc": "2.0", "method": method}
    if not notify:
        body["id"] = _mids[0]
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


def top_children(root):
    """根级子节点必须走 get_node_tree_json —— list_children(nodePath="") 在
    orange-wz 上恒返回 children:[]（UIWindow.img 实测同现象），用它做判据 = 空转。"""
    t = call("get_node_tree_json", {"rootPath": root, "nodePath": "",
                                    "maxDepth": 1, "autoParse": True, "includePng": False})
    n = (t or {}).get("tree") or {}
    return {c["name"]: c["type"] for c in (n.get("children") or [])}


def canvas_png(root, path):
    """取单个 CANVAS 的 base64 PNG，用于像素级交叉比对。"""
    t = call("get_node_tree_json", {"rootPath": root, "nodePath": path,
                                    "maxDepth": 0, "autoParse": True, "includePng": True})
    n = (t or {}).get("tree") or {}
    return n.get("png") or n.get("base64Png") or n.get("data")


def canvas_shape(root, path):
    t = call("get_node_tree_json", {"rootPath": root, "nodePath": path,
                                    "maxDepth": 1, "autoParse": True, "includePng": False})
    n = (t or {}).get("tree") or {}
    kids = n.get("children") or []
    return (n.get("width"), n.get("height"), len(kids))


def main():
    errs = []
    rpc("initialize", {"protocolVersion": "2024-11-05", "capabilities": {},
                       "clientInfo": {"name": "dsh", "version": "1"}})
    rpc("notifications/initialized", {}, notify=True)
    key = {"name": "GMS", "ivBase64": "TSPHKw==",
           "userKeyBase64": open(KEYFILE, encoding="ascii").read().strip()}
    call("load_files", {"paths": [LIVE, OUT], "exclusive": False, "key": key})
    try:
        a = top_children(LIVE)
        b = top_children(OUT)
        print(f"LIVE top nodes = {len(a)}   OUT top nodes = {len(b)}")

        if len(a) < 10:
            print(f"=== GATE INVALID: LIVE 顶层节点只有 {len(a)} 个，判据未生效 ===")
            return 2
        missing = sorted(set(a) - set(b))
        extra = sorted(set(b) - set(a))
        if missing:
            errs.append(f"OUT 丢失 {len(missing)} 个既有顶层节点: {missing[:20]}")
        print(f"missing={len(missing)} extra={extra}")

        # 3) 新增节点到位
        for need, expect in (("dropItemAura", 4), ("dropItemEffect", 6)):
            if need not in b:
                errs.append(f"OUT 缺少新增节点 {need}")
                continue
            kids = call("list_children", {"rootPath": OUT, "nodePath": need, "autoParse": True})
            n = len((kids or {}).get("children", []))
            if n != expect:
                errs.append(f"{need} 子节点 {n} != {expect}")
            else:
                print(f"  {need}: {n} 子节点 OK")

        # 4) 抽样既有节点的形状一致（含 CANVAS 与 LIST）
        sample = [k for k in sorted(a)][:12]
        checked = 0
        for name in sample:
            la, lb = canvas_shape(LIVE, name), canvas_shape(OUT, name)
            if la != lb:
                errs.append(f"节点 {name} 形状变化 LIVE{la} -> OUT{lb}")
            checked += 1
        print(f"抽样比对 {checked} 个既有顶层节点形状 = {sorted(sample)}")

        # 像素级：挑既有 CANVAS 节点逐个比 base64，不一致即数据被改写
        px_checked = 0
        for name in sorted(a):
            if a[name] != "CANVAS_PROPERTY":
                continue
            pa, pb = canvas_png(LIVE, name), canvas_png(OUT, name)
            if not pa:
                errs.append(f"LIVE canvas {name} 取不到 png，判据不可用")
                continue
            if pa != pb:
                errs.append(f"canvas {name} 像素被改写 (LIVE {len(pa)}B vs OUT {len(pb or '')}B)")
            px_checked += 1
            if px_checked >= 8:
                break
        print(f"像素级比对既有 CANVAS = {px_checked} 个")
        if px_checked == 0:
            errs.append("没有任何 CANVAS 参与比对，像素门禁空转")
    finally:
        print("unload:", call("unload_all", {}))

    if errs:
        print("\n=== FAIL: OUT 不可用于替换 live ===")
        for e in errs:
            print("  " + e)
        return 1
    print("\n=== PASS: OUT 与 LIVE 节点树等价且含新增 aura/effect ===")
    return 0


if __name__ == "__main__":
    sys.exit(main())
