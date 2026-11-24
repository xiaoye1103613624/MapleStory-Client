#!/usr/bin/env python3
r"""掉落光环资源层修复（一次性、自包含、不动 live exe）。

背景（2026-11-21 取证）：
  - live 的 MapleStory.exe 内含的是「数字节点」版本 DropAuras 代码
    （二进制无 Rare/Epic/Unique/Legendary 字面量，只有 DropAuraEff.img + to_string 路径）。
    该版本读 root["dropItemAura"]["1".."5"]，但 live 资源里这些数字节点是
    webp 误导入的「单个静态红色帧」（无 origin/z/delay），导致所有档位显示成
    几乎相同的静态红光、且不会动 —— 这就是用户说的「实际效果存在问题」的真正根因。
  - 正确素材在命名节点 Rare/Epic/Unique/Legendary，每档 front/back 各 6 帧
    带 origin/z/delay，且四档颜色各异（蓝/紫/黄/绿）。

修复：把命名档位节点复制到数字节点（OVERWRITE），使【当前部署的 exe】也能
渲染正确的四色动画光环；源端 DropAuras.cpp/.h 已恢复命名节点实现，
下次干净 rebuild 同样正确。两条代码路径都成立。

流程：load(staging 副本) -> copy_paste(Rare->1,Epic->2,Unique->3,Legendary->4/5)
      -> 复核 -> save_node -> 原子替换 live 资源。禁止 save_as 回写 loaded 根同路径。
"""
import json
import os
import shutil
import urllib.request

BASE = "http://127.0.0.1:10012/mcp"
KEYFILE = r"E:\project\MapleStory-Client\tools\_gms_key_128.b64"
STAGING = r"E:\project\MapleStory-Client\tools\_staging\DropAuraEff.aurafix.img"
# save_as 必须写到与 loaded 根不同的路径（AGENTS.md §3 安全流）
OUTPUT = r"E:\project\MapleStory-Client\tools\_staging\DropAuraEff.aurafix.out.img"
LIVE = r"E:\MXD\BeiDou-Client-xy\Data\Effect\DropAuraEff.img"

sid = None
mid = [0]


def rpc(method, params=None, notify=False):
    global sid
    mid[0] += 1
    body = {"jsonrpc": "2.0", "method": method}
    if not notify:
        body["id"] = mid[0]
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


def tree(node_path):
    t = call("get_node_tree_json", {"rootPath": STAGING, "nodePath": node_path,
                                    "maxDepth": 3, "autoParse": True, "includePng": False})
    return (t or {}).get("tree") or {}


def main():
    # 每次从 live 重新取干净副本，避免上一次失败留下的脏状态
    shutil.copy2(LIVE, STAGING)
    if os.path.exists(OUTPUT):
        os.remove(OUTPUT)
    print("staging copy refreshed from live")

    rpc("initialize", {"protocolVersion": "2024-11-05", "capabilities": {},
                       "clientInfo": {"name": "dsh-aurafix", "version": "1"}})
    rpc("notifications/initialized", {}, notify=True)
    key = {"name": "GMS", "ivBase64": "TSPHKw==",
           "userKeyBase64": open(KEYFILE, encoding="ascii").read().strip()}

    call("load_files", {"paths": [STAGING], "key": key, "exclusive": False})

    # grade 映射：服务端 1..4 = Rare..Epic..Unique..Legendary；数字节点 1..4 对应之。
    # 数字 5 是死槽（服务端 grade 0..4，永不发 5）；填 Legendary 作兼容兜底。
    mapping = [("Rare", "1"), ("Epic", "2"), ("Unique", "3"),
               ("Legendary", "4"), ("Legendary", "5")]
    try:
        print("=== before ===")
        b = tree("dropItemAura")
        for g in b.get("children", []):
            sides = {s["name"]: len(s.get("children") or []) for s in g.get("children", [])}
            print(" ", g["name"], sides)

        # copy_paste_nodes 会把「源节点本身」按源名字挂到 target 下，
        # 所以要逐个粘贴 front/back 子节点（其名字就是 front/back），
        # 用 OVERWRITE 覆盖数字节点里同名的旧静态帧。
        for src_name, dst_num in mapping:
            for side in ("front", "back"):
                res = call("copy_paste_nodes", {
                    "sources": [{"rootPath": STAGING, "nodePath": f"dropItemAura/{src_name}/{side}"}],
                    "targets": [{"rootPath": STAGING, "nodePath": f"dropItemAura/{dst_num}"}],
                    "strategy": "OVERWRITE", "autoParse": True,
                    "clearClipboard": True, "releaseSourceCache": False,
                })
                pasted = (res or {}).get("results", [{}])[0].get("pasted", [])
                print(f"copy {src_name}/{side} -> {dst_num}: {[p.get('name') for p in pasted]}")

        print("=== after (数字节点应各有 front/back 6 帧) ===")
        a = tree("dropItemAura")
        ok = True
        grades = {g["name"]: g for g in a.get("children", [])}
        for num in ("1", "2", "3", "4", "5"):
            g = grades.get(num, {})
            sides = {s["name"]: len(s.get("children") or []) for s in g.get("children", [])}
            # 检查 front 第 0 帧带 origin/z/delay 元数据
            front = next((s for s in g.get("children", []) if s["name"] == "front"), {})
            frames = front.get("children") or []
            meta = [c["name"] for c in (frames[0].get("children") or [])] if frames else []
            print(f"  {num}: {sides} frame0_meta={meta}")
            if sides.get("front") != 6 or sides.get("back") != 6:
                ok = False
        if not ok:
            raise RuntimeError("数字节点复核失败：非 6 帧")

        # save_as 到异路径（不能回写 loaded 根），落盘真正生效
        print("save_as ->", call("save_as", {"rootPath": STAGING, "nodePath": "",
                                            "filePath": OUTPUT, "autoParse": True,
                                            "clearCache": True, "unloadAfterSave": True}))
    finally:
        try:
            print("unload:", call("unload_all", {}))
        except Exception as exc:
            print("unload warn:", exc)

    if not os.path.exists(OUTPUT):
        print("=== FAIL: save_as 输出未落盘 ===")
        return 1
    size = os.path.getsize(OUTPUT)
    print(f"\noutput size = {size}")
    if size < 100000:
        print("=== FAIL: 输出过小 ===")
        return 1

    # 备份 live 后原子替换（客户端未占用时）
    bak = STAGING + ".pre-aurafix.bak"
    if not os.path.exists(bak):
        shutil.copy2(LIVE, bak)
        print("live backed up ->", bak)
    shutil.copy2(OUTPUT, LIVE)
    print(f"installed {LIVE} size={os.path.getsize(LIVE)}")
    print("=== PASS ===")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
