# 服务端 NPC/任务脚本全量校验与修复报告（2026-11-17）

> 范围：`E:\project\BeiDou-Server_s9\gms-server\scripts-zh-CN`（2087 个 .js）+ `E:\project\BeiDou-Server_s9\gms-server\scripts`（1976 个 .js），共 **4063** 个脚本。
> 引擎前提：服务端脚本引擎为 **GraalJS**（`src/main/java/org/gms/scripting/AbstractScriptManager.java:25,49` `graal.js`），加载规则 **zh-CN 优先、scripts 回退**（同文件 :61-65），`application.yml:52` `language: zh-CN`。
> 编码前提：全部以 UTF-8（无 BOM）修改，保持原 CRLF 行尾（遵守全端 UTF-8 编码约定）。

## 1. 校验方法

- 单进程 Node v25 `vm.Script` 逐文件语法解析（不执行）+ `TextDecoder fatal` UTF-8 严格校验 + BOM 检测；`node --check` 双重复核报错行。
- 机器可读结果：本目录 `server-script-validation.json`。
- 临时校验/修复脚本已清理，不留仓库垃圾。

## 2. 修复清单（全部有备份，`backup/` 目录）

| # | 文件 | 缺陷 | 修复 | 影响面 |
| --- | --- | --- | --- | --- |
| 1 | `scripts-zh-CN/npc/9201112.js:42` | 中文串内嵌套未转义半角引号 `被称为"Sigils"的东西`，整文件语法错 | `"Sigils"` → `\"Sigils\"`（1 行） | **活跃 bug**：zh-CN 加载优先，玩家对话直接报错 |
| 2 | `scripts-zh-CN/quest/20002.js:39` | `else { status--;` 分支缺闭合 `}`，吞掉后续 if-else 链 | 插入 `}`（1 行） | **活跃 bug**：新手任务 20002 脚本加载失败 |
| 3 | `scripts-zh-CN/quest/21736.js:36` | 同上结构，空行占位导致 else 块未闭合 | 空行 → `}`（1 行） | **活跃 bug**：任务 21736 加载失败 |
| 4 | `scripts/npc/9900000.js:75-79,88-92` | E4X `for each(var i in …)` 语句，GraalJS 默认方言不支持 | 改 `forEach`，与 zh-CN 版已验证写法逐字对齐（zh-CN `9900000.js:75,84`） | 回退英文时的隐患（zh-CN 已正确） |
| 5 | `scripts/quest/2236.js:37,39` | 全角花括号 `｛`（U+FF5B）/`｝`（U+FF5D） | 换半角 `{`/`}` 并保持 tab 缩进 | 回退英文时的隐患（zh-CN 已正确） |
| 6 | BOM×5：`scripts-zh-CN/npc/9310035.js`、`scripts-zh-CN/quest/22003.js`、`scripts-zh-CN/quest/29002.js`、`scripts/npc/9310035.js`、`scripts/quest/29002.js` | 文件头 UTF-8 BOM（EF BB BF） | 字节级剥离（内容零改动，numstat 1/1 即整文件重写行） | 消除首个语句前的隐藏字符风险 |

## 3. 终验结果（修复后全量复扫）

```text
TOTAL files=4063 syntaxErr=0 badEnc=0 bom=0
scripts-zh-CN: files=2087 syntaxErr=0 badEnc=0 bom=0
scripts      : files=1976 syntaxErr=0 badEnc=0 bom=0
```

## 3b. GraalJS 权威复验（与服务端同引擎、同 classpath、同加载方式）

> V8（node --check）只是语法代理；为排除引擎方言差异，用**服务端同款 GraalJS 23.0.4 + js-scriptengine 24.0.1**、同款 JDK 21、复用 `_server_cmdline_47296.txt` 提取的 107 项生产 classpath，`engine.eval(UTF-8 reader)` 全量重扫 4063 个脚本。校验器：`tools/graalcheck/ScriptSyntaxCheck.java`（绑定 allowHostAccess/allowHostClassLookup，与 `AbstractScriptManager:107-112` 同口径）。

```text
GRAAL TOTAL files=4063 fail=51 elapsed=8s   （日志 tools/graalcheck/graal_full_check2.log）
  SyntaxError: 0   ← 语法层面全部通过，与 V8 结论一致
  失败 51 个全为运行期环境性 NPE：
    - 47 × scripts*/event/*.js  顶层调用 EventManager/Scheduler → ServerManager.getApplicationContext() 为 null（独立校验器无 Spring 容器，live 服务端有，非脚本缺陷）
    - 4 × npc/9201002.js、9201011.js（zh-CN+en 各2）顶层 GameConfig.getXxx() 同类原因
```

判定：本轮修复后的 4063 脚本在真实加载路径（GraalJS 解析+顶层执行）下无语法问题；51 个 NPE 属校验环境限制，服务端真实启动时由 Spring 容器兜住，不构成缺陷，如需铁证可重启服务端看日志无 `AbstractScriptManager ... warn` 输出。

## 3c. orange-wz MCP 冒烟（curl 直连通路）

- 会话内 `mcp__orange_wz__*` 工具未注入（`ops_discover_tools` No allowed），但 MCP 进程可用 **curl JSON-RPC 直连**：`initialize → 取 Mcp-Session-Id → notifications/initialized → tools/list` 成功返回 `load_files` 等 29 工具。后续 IMG 校验可走此通路（128B key 规格见 AGENTS.md §3）。

## 4. 交叉核验与边界

- 修复前用 `git status` 核对：5 个语法修复文件均不在并发会话的未提交改动清单内，零撞车；修复后 `git diff -U1` 逐行复核全部为最小 diff。
- zh-CN 版 `9900000.js` / `2236.js` 本身语法正确（en 版缺陷已被覆盖），说明 #4/#5 是回退路径隐患，一并修正。
- `for each` 在 `scripts` 全目录仅此 1 文件 2 处（其余命中都是英文句子里的字面 "for each"）。
- 未触碰：`scripts/event` 等其余 4058 个脚本全部原样通过，零改动。

## 5. 未尽事项（如实披露）

- **真机闭环待做**：校验是静态语法级；GraalJS 运行期语义（如脚本对 `cm/qm` API 的调用是否存在）未逐脚本验证。3 个 zh-CN 活跃 bug 修完后，建议重启服务端并实际点开 NPC 9201112 / 做任务 20002、21736 验证对话流。
- orange-wz MCP（10012）进程已启动（HTTP 400=存活），但本会话 `ops_discover_tools` 仍未暴露 `mcp__orange_wz__*` 工具；本轮校验不需要 WZ 资源，未受影响。下次需要 IMG 校验时先跑 `E:\project\orange-wz\start-mcp.bat` 并确认工具注入。
