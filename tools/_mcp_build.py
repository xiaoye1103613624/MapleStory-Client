#!/usr/bin/env python3
"""通过 MCP (stdio) 驱动 Photoshop：打开能力详情页 PSD 并由 PS 导出 PNG。"""
import json
import os
import subprocess
import sys

EXE = r"C:\Users\11036\AppData\Local\Programs\Python\Python314\Scripts\photoshop-mcp-server.exe"
PSD = r"e:\project\MapleStory-Client\tools\stat_out\stat_page.psd"
OUT_PNG = r"e:\project\MapleStory-Client\tools\stat_out\stat_page_photoshop.png"


class Mcp:
    def __init__(self, exe):
        self.p = subprocess.Popen([exe], stdin=subprocess.PIPE,
                                  stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                  text=True, encoding="utf-8", bufsize=1)
        self._id = 0
        self.send({"jsonrpc": "2.0", "id": 0, "method": "initialize", "params": {
            "protocolVersion": "2024-11-05", "capabilities": {},
            "clientInfo": {"name": "stat-builder", "version": "1"}}})
        info = self.read()["result"]["serverInfo"]
        print("server:", info)
        self.send({"jsonrpc": "2.0", "method": "notifications/initialized"})

    def send(self, obj):
        self.p.stdin.write(json.dumps(obj) + "\n")
        self.p.stdin.flush()

    def read(self, debug=False):
        while True:
            line = self.p.stdout.readline()
            if not line:
                raise RuntimeError("server closed stdout")
            line = line.strip()
            if debug:
                print("   <raw>", repr(line[:120]))
            if not line.startswith("{"):
                continue
            return json.loads(line)

    def call(self, name, args):
        self._id += 1
        self.send({"jsonrpc": "2.0", "id": 100 + self._id, "method": "tools/call",
                   "params": {"name": name, "arguments": args}})
        resp = self.read(debug=os.environ.get("MCP_DEBUG") == "1")
        if "error" in resp:
            raise RuntimeError(resp["error"])
        for c in resp["result"].get("content", []):
            print("[%s] %s" % (name, c.get("text", "").strip()))
        return resp["result"]


def main():
    m = Mcp(EXE)
    m.call("photoshop_get_session_info", {})
    m.call("photoshop_open_document", {"file_path": PSD})
    m.call("photoshop_get_active_document_info", {})
    if os.path.exists(OUT_PNG):
        os.remove(OUT_PNG)
    m.call("photoshop_save_document", {"file_path": OUT_PNG, "format": "png"})
    print("exists:", os.path.exists(OUT_PNG), os.path.getsize(OUT_PNG) if os.path.exists(OUT_PNG) else 0)
    m.p.terminate()


if __name__ == "__main__":
    sys.exit(main())
