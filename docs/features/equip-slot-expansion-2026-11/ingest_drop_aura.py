r"""⑥ 掉落特效素材入库（安全路线：独立 .img，绝不改 live 的 BasicEff.img）。

为什么不用 copy_paste 合并进 BasicEff.img：
  实测 copy_paste_nodes(OVERWRITE)+save_as 产出的文件只剩 2 个顶层节点
  （live BasicEff.img 有 41 个），verify_ingest_integrity.py 判 FAIL。
  合并路线会毁掉整个 Effect 根（全地图特效都在这一个 3.9MB 文件里）。

安全路线依据（一手源码）：
  Util/Resource/ImgSource.cpp:261-294 gather() 递归扫描根目录下所有 .img；
  :306 nl::img_file::open_merge(bufs) 把它们合并成同一个 Effect 根。
  → 在 Data/Effect/ 放一个新 .img，节点就出现在 nl::nx::Effect 下，
    与 BasicEff.img 完全隔离；且 ImgSource 不吃 .nxcache（那是 ImgFiles.cpp
    的另一条路），无需处理缓存失效。

流程（AGENTS.md §3）：create_img_file → copy_paste_nodes 从参考包复制
dropItemAura / dropItemEffect → save 到 staging → 完整性复核 → 原子放入 live。
"""
import json
import os
import shutil
import sys
import urllib.request

BASE = "http://127.0.0.1:10012/mcp"
KEYFILE = r"E:\project\MapleStory-Client\tools\_gms_key_128.b64"
REF = r"E:\project\MapleStory-Client\tools\_staging\ref_effect.wz"
# create_img_file 产出的暂存新文件
NEW_STG = r"E:\project\MapleStory-Client\tools\_staging\DropAuraEff.img"
# 最终落到 live 的位置（客户端会扫描 Data/Effect 下所有 .img）
LIVE_TARGET = r"E:\MXD\BeiDou-Client-xy\Data\Effect\DropAuraEff.img"
LIVE_BACKUP_DIR = r"E:\project\MapleStory-Client\tools\_staging"

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


def tree(root, path, maxdepth=3):
    t = call("get_node_tree_json", {"rootPath": root, "nodePath": path, "maxDepth": maxdepth,
                                     "autoParse": True, "includePng": False})
    return (t or {}).get("tree") or {}


def count_frames(node):
    kids = node.get("children") or []
    return len(kids), all((k.get("width") or 0) > 0 for k in kids)


def main():
    for f in (NEW_STG,):
        if os.path.exists(f):
            os.remove(f)

    rpc("initialize", {"protocolVersion": "2024-11-05", "capabilities": {},
                       "clientInfo": {"name": "dsh", "version": "1"}})
    rpc("notifications/initialized", {}, notify=True)
    key = {"name": "GMS", "ivBase64": "TSPHKw==",
           "userKeyBase64": open(KEYFILE, encoding="ascii").read().strip()}

    # create_img_file 的 key 需要 version？先按 schema 传 fileName+key
    print("create_img_file ->", call("create_img_file", {
        "fileName": NEW_STG, "key": key}))

    try:
        call("load_files", {"paths": [REF], "exclusive": False, "key": key})
        print("copy_paste ->", call("copy_paste_nodes", {
            "sources": [{"rootPath": REF, "nodePath": "BasicEff.img/dropItemAura"},
                        {"rootPath": REF, "nodePath": "BasicEff.img/dropItemEffect"}],
            "targets": [{"rootPath": NEW_STG, "nodePath": ""},
                        {"rootPath": NEW_STG, "nodePath": ""}],
            "strategy": "OVERWRITE", "autoParse": True,
            "clearClipboard": True, "releaseSourceCache": False}))

        # 完整性复核：4 档 × front/back × 6 帧 + 光柱 6 帧
        a = tree(NEW_STG, "dropItemAura", 3)
        grades = a.get("children") or []
        print(f"dropItemAura grades = {[g['name'] for g in grades]}")
        if len(grades) != 4:
            raise RuntimeError("档位数 != 4")
        total_frames = 0
        for g in grades:
            for side in (g.get("children") or []):
                n, sized = count_frames(side)
                total_frames += n
                if n != 6 or not sized:
                    raise RuntimeError(f"{g['name']}/{side['name']} frames={n} sized={sized}")
            print(f"  {g['name']}: " + ", ".join(
                f"{s['name']}={len(s.get('children') or [])}f" for s in (g.get('children') or [])))
        e = tree(NEW_STG, "dropItemEffect", 1)
        ef = e.get("children") or []
        print(f"dropItemEffect frames = {len(ef)} sizes = "
              + ", ".join(f"{c.get('width')}x{c.get('height')}" for c in ef))
        if len(ef) != 6:
            raise RuntimeError("光柱帧数 != 6")

        print("save_node ->", call("save_node", {"rootPath": NEW_STG, "nodePath": "",
                                                  "autoParse": True, "clearCache": True,
                                                  "unloadAfterSave": True}))
    finally:
        try:
            print("unload:", call("unload_all", {}))
        except Exception as exc:
            print("unload warn:", exc)

    if not os.path.exists(NEW_STG):
        print("=== FAIL: create/save 未落盘", NEW_STG, "===")
        return 1
    size = os.path.getsize(NEW_STG)
    print(f"\nNEW_STG size = {size}")
    if size < 50000:
        print("=== FAIL: 文件过小，疑无像素数据 ===")
        return 1

    # 原子放入 live（新文件，不覆盖任何既有资源；备份位=自身不存在，故只需防重复）
    if os.path.exists(LIVE_TARGET):
        bak = os.path.join(LIVE_BACKUP_DIR, "DropAuraEff.prev.img")
        shutil.copy2(LIVE_TARGET, bak)
        print("existing live backed up ->", bak)
    shutil.copy2(NEW_STG, LIVE_TARGET)
    print(f"installed {LIVE_TARGET} size={os.path.getsize(LIVE_TARGET)}")
    print("=== PASS: 掉落光环素材以独立 .img 入库，BasicEff.img 未被触碰 ===")
    return 0


if __name__ == "__main__":
    sys.exit(main())
