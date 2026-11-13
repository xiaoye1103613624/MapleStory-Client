# 阶段 0：REA 接入 + 192 IDA 探测（2026-11-12）

## REA / Cursor

- `npx rea-agents@6.8.0 setup --yes --client cursor --skill-client shared`（Node **26.12.1** 便携）
- 系统 Node `v25.2.1` / `nodejs-24` `v24.14.0` 均不满足 `^22.23.4 \|\| ^24.22.1 \|\| >=26.12.1` → 必须用 Node26 跑 REA
- `~/.cursor/mcp.json`：`rea` 命令改为  
  `C:\Users\11036\tools\node-v26.12.1-win-x64\npx.cmd -y rea-agents@6.8.0 mcp`  
  env：`REA_ANALYSIS_PROVIDER=ida`，`REA_IDA_MCP_CONFIG=C:\Users\11036\.cursor\rea\ida-mcp.json`
- `ida-mcp.json` 指向 `http://127.0.0.1:13340/mcp`（192 实例）
- `rea doctor`：IDA registration **ok / available**；整体 `healthy=false` 因未配 Ghidra/JADX/Kuna（可忽略）

## 192 IDB / MCP

- 路径：`E:\MXD\【2023.8.8江浩生日福利】怀旧岛192整合版\怀旧岛192客户端\冒险岛online192\MapleStory.exe.i64`
- 启动后 MCP：`127.0.0.1:13340`，`module=MapleStory.exe`，`imagebase=0x140000000`，64-bit
- `server_health`：`auto_analysis_ready=true`，`hexrays_ready=true`，`strings_cache_ready=false`
- `survey_binary`（minimal）：
  - `total_functions=3`，`total_strings=10`，`total_segments=8`
  - 段：`___`（巨大 rwx）、`.idata*`、混淆名 `zytsjlrd` / `llbfkknc`、`.pdata`
  - 入口 `start @ 0x149c88000`（壳段）
- `idautils.Strings()` 仅 10 条；对 DualBlade/Evan/VMatrix/中文锚点 **0 hit**
- **局限**：未脱壳前 REA/Hex-Rays **无法**还原技能/职业客户端逻辑；证据改走 WZ/XML（见 `wz-192-skill-inventory.json`）

## 并行仍可用的 IDA

| 端口 | 模块 | 用途 |
| --- | --- | --- |
| 13337 | Maplestory95.exe | v095 对照 |
| 13339 | \`Angel.exe | 本端 s9/v83 协议与 UI 主对照（`mcp.json` 默认 ida-pro-mcp） |
| 13340 | MapleStory.exe (192) | 仅壳信息；脱壳前勿当业务证据 |

## 密钥（192 Skill 分卷）

- `ivBase64=AAAAAA==`（`WZ_LATEST_IV`）+ 128B `DEFAULT_KEY`  
- 已实测可 `load_files` / `query_nodes`（例 `Skill_000.wz` → `1111.img/skill/11111006`）
