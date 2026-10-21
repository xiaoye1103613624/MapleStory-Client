# REA 逆向工具本地集成报告

日期：2026-10-19 · 版本：rea-agents 6.7.0 · 状态：**已集成并验证通过**

## 一、集成全景

```
CodeBuddy Agent（对话中直接调用 REA 工具）
   └─ C:\Users\11036\.codebuddy\mcp.json  ← rea 条目（stdio）
        └─ D:\software\nodejs-24\node.exe  rea.mjs mcp   （Node 24.14.0，专用运行时）
             └─ REA_IDA_MCP_CONFIG → C:\Users\11036\.rea\ida-mcp.json
                  └─ idalib-mcp.exe --stdio --max-workers 1   （headless supervisor）
                       └─ IDA Professional 9.4（idalib）
```

## 二、各组件位置

| 组件 | 位置 | 说明 |
|---|---|---|
| REA CLI/MCP | `D:\software\nodejs-24\node_modules\rea-agents`（bin：`D:\software\nodejs-24\rea.cmd`） | npm 全局安装，6.7.0 |
| Node 运行时 | `D:\software\nodejs-24` | 24.14.0；系统 node 25.2.1 **不被 REA 支持**（仅 22.19+/24.11+/26+），故专用 |
| CodeBuddy MCP 配置 | `C:\Users\11036\.codebuddy\mcp.json` | rea 条目已加入；备份 `mcp.json.rea-backup` |
| IDA provider 注册 | `C:\Users\11036\.rea\ida-mcp.json` | headless 模式 |
| REA 专用 Python venv | `C:\Users\11036\.rea\ida-mcp-env` | ida-pro-mcp（git 快照 c133c385）+ idapro 0.0.11 |
| REA 工作流 skill | `C:\Users\11036\.codebuddy\skills\reverse-engineer-anything` | SKILL.md + 6 篇 references |
| IDA Pro 9.4 | `D:\software\IDA Professional 9.4` | 现有安装，未改动 |

## 三、验证结果（全部通过）

| 验证项 | 结果 |
|---|---|
| `rea --version` | 6.7.0 |
| MCP 握手（initialize + tools/list） | server: rea 6.7.0，protocol 2025-03-26，**207 个工具** |
| `rea doctor`（带 provider env） | ida provider **configured=True available=True**；node/host healthy |
| `rea analyze` where.exe | PE x86_64，103 个函数、149 个字符串，带证据链 |
| `rea search` where.exe | 过程名检索正常（命中 `wmain` 等） |
| `rea function` / disasm | 反汇编正常返回 |
| `rea decompile` where.exe `wmain` | **伪代码正常返回**（hexrays_ready） |

> doctor 中 hopper/ghidra/jadx/kuna missing 及其他客户端的 registration drift 为预期项——本方案只接 IDA；CodeBuddy 不在 REA setup 支持的客户端列表，采用手动注册。

## 四、关键兼容性结论（重要）

- 你全局的 **ida-pro-mcp 2.0.0 不是 PyPI 版**（PyPI 最高 1.4.0），是某次 git 构建。它虽有 supervisor API（idb_open/idb_list/idb_close），但其 `decompile` 工具**不接受 `offset`/`max_lines` 分页参数**，而 REA 6.7.0 的 headless 适配器按分页契约调用 → 直接反编译会报 `unexpected parameters: ['offset','max_lines']`。
- 解决：REA 走**隔离 venv**（`.rea\ida-mcp-env`），安装 REA 文档指定的 git 快照 **c133c385**（同为 2.0.0 版本号，但 decompile 带分页契约）。
- **你全局的 2.0.0 保持原样**，继续服务你的 IDA GUI 工作流（`probe_ida.ps1` → 127.0.0.1:13337），两套互不干扰。

## 五、使用方式

**CodeBuddy 对话中**（需先重启 CodeBuddy 使 MCP 生效）：
> "用 REA 分析 D:\xxx\program.exe，看 xx 功能怎么实现的，给出证据"

**终端 CLI**：
```powershell
$env:REA_ANALYSIS_PROVIDER = 'ida'
$env:REA_IDA_MCP_CONFIG   = 'C:\Users\11036\.rea\ida-mcp.json'
& 'D:\software\nodejs-24\rea.cmd' analyze   'D:\target.exe' --provider ida --json
& 'D:\software\nodejs-24\rea.cmd' decompile 'D:\target.exe' wmain --provider ida --json
& 'D:\software\nodejs-24\rea.cmd' search    'D:\target.exe' 'pattern' --kind procedures --provider ida --json
```

常用命令：`analyze`（总览）、`decompile`/`function`（函数）、`search`（字符串/过程名）、`inspect-native-api`、`doctor`。

## 六、维护备忘

- **更新 REA**：`& 'D:\software\nodejs-24\rea.cmd' update`（npm registry 可达）
- **网络**：github.com 直连不通；需要源码/快照时走 `gh-proxy.com` 代理
- **回滚 MCP 注册**：`mcp.json.rea-backup` 还原即可
- **大目标**：idalib 分析大二进制（数十 MB）耗时以分钟计，属正常
- venv 固定在 c133c385 快照，升级 ida-pro-mcp 前需确认 decompile 仍带 offset/max_lines 分页契约
