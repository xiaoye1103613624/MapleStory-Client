# -*- coding: utf-8 -*-
"""One-shot Angel IDA MCP probe for multi-resolution plan evidence."""
import json
import pathlib
import urllib.request

SID = "a1cae656-6347-41e8-a541-ccf26675e417"
URL = "http://127.0.0.1:13339/mcp"
OUT = pathlib.Path(__file__).with_name("angel-resolution-probe.json")


def call(tool, args, id_=1):
    body = {
        "jsonrpc": "2.0",
        "id": id_,
        "method": "tools/call",
        "params": {"name": tool, "arguments": args},
    }
    req = urllib.request.Request(
        URL,
        data=json.dumps(body).encode(),
        headers={
            "Content-Type": "application/json",
            "Accept": "application/json, text/event-stream",
            "Mcp-Session-Id": SID,
        },
    )
    raw = urllib.request.urlopen(req, timeout=180).read().decode("utf-8", "replace")
    obj = json.loads(raw)
    if obj.get("error"):
        return {"error": obj["error"]}
    result = obj.get("result", {})
    sc = result.get("structuredContent")
    if sc is not None:
        return sc
    content = result.get("content", [])
    if content and content[0].get("type") == "text":
        text = content[0]["text"]
        try:
            return json.loads(text)
        except Exception:
            return {"text": text}
    return obj


def main():
    out = {}
    out["server_health"] = call("server_health", {}, 1)
    out["lookup"] = call(
        "lookup_funcs",
        {
            "queries": [
                "CUIStatusBar::OnCreate",
                "CUIStatusBar::Draw",
                "CUIStatusBar::OnMoveWnd",
                "CUIStatusBar::Create",
                "CQuickSlot::OnCreate",
                "CQuickSlot::Create",
                "CWvsApp::Run",
                "CWvsApp::InitializeGr2D",
                "IWzGr2D::Initialize",
                "CWnd::CreateWnd",
            ]
        },
        2,
    )
    out["func_query_statusbar"] = call(
        "func_query",
        {"queries": {"name_regex": "CUIStatusBar", "count": 80}},
        3,
    )
    out["func_query_quickslot"] = call(
        "func_query",
        {"queries": {"name_regex": "QuickSlot|CQuickSlot", "count": 40}},
        4,
    )
    out["func_query_resolution"] = call(
        "func_query",
        {"queries": {"name_regex": "Resolution|ScreenWidth|SetScreen", "count": 40}},
        5,
    )
    out["find_regex"] = call(
        "find_regex",
        {
            "pattern": (
                "1920|1280|1366|800 x 600|1024 x 768|"
                "quickSlot|StatusBar|UpdateResolution|"
                "soScreenResolution|ScreenWidth|m_nWidth"
            ),
            "limit": 80,
        },
        6,
    )
    out["search_text_statusbar"] = call(
        "search_text",
        {"pattern": "UI/StatusBar.img", "limit": 40, "code_only": False},
        7,
    )
    out["search_text_quickslot"] = call(
        "search_text",
        {"pattern": "quickSlot", "limit": 40, "code_only": False},
        8,
    )
    OUT.write_text(json.dumps(out, ensure_ascii=False, indent=2), encoding="utf-8")
    print("WROTE", OUT)
    print("health module=", out["server_health"].get("module"))
    print("lookup=", json.dumps(out["lookup"], ensure_ascii=False)[:2500])
    print("find_regex=", json.dumps(out["find_regex"], ensure_ascii=False)[:2500])
    print(
        "func_query_statusbar=",
        json.dumps(out["func_query_statusbar"], ensure_ascii=False)[:2500],
    )


if __name__ == "__main__":
    main()
