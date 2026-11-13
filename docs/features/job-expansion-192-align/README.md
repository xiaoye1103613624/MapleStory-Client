# 多职业 / 五转 / V / 双刀 / 龙神扩展（192 对齐）

> 状态：**阶段 0 完成；波次 1 预备**（REA 已接入；192 IDB 强加壳 → 逻辑取证改 WZ/XML）  
> 本端部署（用户确认）：`E:\MXD\BeiDou-Client-xy`（见 [notes/client-path.md](notes/client-path.md)）  
> 波次1 骑士团四转 + 波次2 双刀/龙神（独立创角 job=3/4 + 原生转职）已落地，见 `evidence/wave2-dual-evan-result.md`、`notes/pending-live-tests.md`。  
> 硬约束：**不占用、不覆盖本端已有骑士团 1–3 / 战神等旧技能 ID 与数值**

## 1. 阶段 0 结论

### 1.1 REA → Cursor

| 项 | 结果 |
| --- | --- |
| 包 | `rea-agents@6.8.0` |
| Node | 系统 Node 25 / 自带 24.14 **不满足**引擎线；已下载便携 **Node v26.12.1** → `C:\Users\11036\tools\node-v26.12.1-win-x64` |
| MCP | `C:\Users\11036\.cursor\mcp.json` 增加 `rea`（`npx.cmd` 指向 Node26） |
| Provider | `REA_ANALYSIS_PROVIDER=ida`，`REA_IDA_MCP_CONFIG=C:\Users\11036\.cursor\rea\ida-mcp.json` → `http://127.0.0.1:13340/mcp` |
| Skill | `C:\Users\11036\.agents\skills\reverse-engineer-anything`（setup 安装） |
| 备份 | `mcp.json.rea.backup` |

**需你本地**：Cursor 重载 MCP 后，工具列表应出现 `rea`。当前会话动态工具目录未必已热加载。

### 1.2 192 客户端 IDA

| 项 | 值 |
| --- | --- |
| IDB | `...\怀旧岛192客户端\冒险岛online192\MapleStory.exe.i64` |
| MCP | `127.0.0.1:13340`（pid 随本次启动） |
| arch | **64-bit**，`imagebase=0x140000000` |
| 分析就绪 | `auto_analysis_ready=true`，`hexrays_ready=true` |
| **可反编译性** | **否（强加壳）**：`total_functions=3`，`total_strings=10`，段名混淆（`zytsjlrd` 等），入口在壳段 `start` |

与早前「开启游戏.vmp.exe」同类：**不能指望从 192 主程序静态还原技能状态机 / 封包分发**。  
行为权威改为：

1. **资源**：192 `Skill_*.wz`（LATEST IV）+ `MapleServer192\wz\Skill.wz\*.xml` + String  
2. **协议/旧逻辑**：本端已校准的 Angel/s9（`13339`）与必要时 Maplestory95（`13337`）  
3. **REA**：配置已通；对 192 主 exe 提问前须先脱壳/内存 dump，否则结论无意义  

清单见 [evidence/wz-192-skill-inventory.json](evidence/wz-192-skill-inventory.json)、[evidence/rea-ida-192-phase0.md](evidence/rea-ida-192-phase0.md)。

### 1.3 本端缺口（相对 192 资源）

| 内容 | 本端 Skill.nx | 192 |
| --- | --- | --- |
| 骑士团 1–3 | 有（原生名伤） | 有（原生，勿覆盖） |
| 骑士团 4 | **空壳** | 有（如 `11121000` 希纳斯骑士） |
| 战神 | 齐 | 齐 |
| 双刀 430–434 | **无** | 有 |
| 龙神 2001/22xx | **无** | 有 |
| V `400001xxx` | **无** | String + `40000.img.xml` 等 |

20dalu「五转占用骑士团 ID」→ **永久否决**。

## 2. 选定路径（F1）

1. **波次 1**：仅填本端骑士团四转空壳（自 192 XML/WZ），不动 1–3；战神残差验收  
2. **波次 2**：双刀、龙神整包新号段 + Job/技能书/`can_use`  
3. **波次 3**：V/五转用 `400001xxx` 或自定义空闲 ID；禁止映射到 `11xxxxxx` 旧骑士团技能  

## 3. 下一步

- Cursor 重载 MCP 验证 `rea`  
- 波次 1：orange-wz 从 192 抽取 `1112/1212/1312/1412/1512` → staging → 本端 Data（另存原子替换）  
- 192 主程序脱壳不在本波范围；需要逻辑级行为时另开 dump 任务  
