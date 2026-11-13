# OmniParser 接入说明（Wave 0）

## 本机路径

| 项 | 路径 |
|---|---|
| MCP 代理 | `E:/project/MapleStory-Client/tools/vendor/OmniParser-MCP/` |
| 官方后端（建议） | `E:/project/OmniParser`（由 setup_backend 克隆） |
| HTTP | `http://127.0.0.1:8010` |
| Cursor MCP | `~/.cursor/mcp.json` → `omniparser` stdio 项 |

## 安装（一次性）

```powershell
cd E:\project\MapleStory-Client\tools\vendor\OmniParser-MCP
python setup_backend.py --omni-home E:\project\OmniParser --device cuda --hf-endpoint https://hf-mirror.com
# 无 GPU：加 --device cpu
# 仅启动（已装好）：python setup_backend.py --omni-home E:\project\OmniParser --start-only
```

依赖：`pip install 'mcp>=1.0,<2.0' httpx`（MCP 代理薄依赖）。

## Cursor mcp.json 片段

```json
"omniparser": {
  "command": "C:\\Users\\11036\\AppData\\Local\\Programs\\Python\\Python314\\python.exe",
  "args": ["E:\\project\\MapleStory-Client\\tools\\vendor\\OmniParser-MCP\\omniparser_mcp.py"],
  "env": {
    "OMNIPARSER_API_URL": "http://127.0.0.1:8010",
    "OMNIPARSER_HOME": "E:\\project\\OmniParser",
    "OMNIPARSER_DEVICE": "cuda"
  }
}
```

重载 MCP 后应见 `parse_screen` / `omniparser_status`。

## 落盘约定

对 `evidence/screens/<name>.png` 解析后写入：

`notes/layout-<窗体>.json`（schema 见 skill「截图兜底」）

转换脚本：`tools/omni_to_layout.py`（本功能目录）。

## 后端未就绪时

主路径仍走 IDA + orange-wz；`layout.py` 手标探针可独立验收。OmniParser 仅消解主路径仍差的残余视觉差。
