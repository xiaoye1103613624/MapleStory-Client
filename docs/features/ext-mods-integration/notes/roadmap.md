# 分阶段路线图（P0~P5）+ 验证矩阵 + 决策点

## 阶段总览

| 阶段 | 范围 | 主要产物 | 前置 |
| --- | --- | --- | --- |
| **P0** | 全量审计 + 分级矩阵 | `README.md` 分级矩阵（本批） | — |
| **P1** | A 类纯资源落地方法论 + 1 个资源包端到端部署验证 | `classification.md` A 类 + 实测 1 个资源包 | P0 |
| **P2** | B/E 类服务端 Java+SQL 迁移方法论 + 美容院/角色槽位/每日签到样例 diff | `classification.md` B/E 类 + s9 diff 报告 | P0 |
| **P3** | C 类客户端源码重实现方法论 + 扩展装备栏/世界地图tip/NameTag 样例 | `classification.md` C 类 + 映射清单 | P0 |
| **P4** | D 类二进制补丁源码重写 + rea 逆向 99999GW | `classification.md` D 类 + CharStats 改动清单 | P0 |
| **P5** | 跨栈强耦合端到端方案 + 全路线图收口 | 本文件 + 决策点闭环 | P1~P4 |

## P0 — 全量审计（已完成）
- 产出：`README.md` 分级矩阵，剔除 agent 既往产物。
- 工具：`Get-ChildItem -Recurse` 文件类型分布（见执行记录）。

## P1 — A 类纯资源（落地 + 实证）
- 方法论：见 `classification.md` A 类。
- 实证建议：选 **200级武器** 或 **左侧小图标** 作端到端样本：
  1. 落 `Data/Item/`（或 `Data/UI/`）对应目录；
  2. `MapleStory.exe --resource-probe` 校验节点与尺寸（R1/R2）；
  3. 进游戏/对应 UI 逐像素核验（R5）。
- 验收：`build_smoke` EXITCODE=0 + 资源节点存在 + 像素对齐。

## P2 — B/E 类服务端（diff 适配）
- 方法论：见 `classification.md` B/E 类。
- 样例：美容院、角色槽位、每日签到。
- 动作：s9 `PacketCreator.java` + handler + Flyway 迁移 diff；opcode 两端对齐。
- 验收：Java 编译通过；新增迁移 `V<next>__*.sql` 可回放；`debug.packet_dump` 抓包比对。

## P3 — C 类客户端源码（映射到 IO/UITypes）
- 方法论：见 `classification.md` C 类。
- 样例：扩展装备栏、世界地图 tip、NameTag 坐标。
- 动作：原版 `CUI*/CWnd*` → 本工程 `ms::UI*`；资源经 `UiAssetResolve`；坐标取运行时 img（R5）。
- 验收：`build_smoke` EXITCODE=0 + `--resource-probe` 资源核验 + 实机 UI 对照。

## P4 — D 类二进制补丁（源码重写 + rea 逆向）
- 方法论：见 `classification.md` D 类。
- 样例：99999GW（HP/MP 上限）。
- 动作：`rea` 逆向定位编解码语义 → 本工程 `CharStats` uint16→int32 + 全读写站点 + s9 `writeInt` + DB 列宽。
- 验收：同 P2/P3 字节宽一致 + 真连 s9 高血量不溢出/不变负。

## P5 — 跨栈强耦合 + 收口
- 端到端：等级/金币/HP 上限、角色槽位、装备栏/背包扩展、个人伤害统计（见 `README.md §4`）。
- 共同约束：客户端/服务端/DB **同步发布**，字节宽/枚举/常量一致。
- 收口：决策点闭环（下方）。

## 验证矩阵

| 验证项 | 工具/手段 | 判定 |
| --- | --- | --- |
| 客户端构建 | `tools/build_smoke.bat` | EXITCODE=0 |
| 资源节点/尺寸 | `MapleStory.exe --resource-probe`（RT-CATALOG/RT-DUMP） | 节点存在、`_width/_height` 正确（R1/R2） |
| UI 布局/动画 | 运行时 img 逐像素核验（R5） | 无溢出/错位、`none` 容器不误当 bitmap |
| 协议字节宽 | `debug.packet_dump` 真连 s9 抓包 | opcode 命中、字节宽与 s9 一致 |
| 服务端编译 | s9 `mvnw -o package`（Java21） | 编译通过 |
| DB 迁移 | Flyway 回放 `V<next>__*.sql` | 无 DDL 冲突、可回滚 |

## 决策点清单（须用户确认）

| # | 决策 | 建议 |
| --- | --- | --- |
| D1 | D 类取舍：083多分辨率 / 伤害上限 纯客户端 hack | 暂缓（本客户端 SDL3 原生多分辨率；伤害值常已 int） |
| D2 | 融合外观 / 伤害皮肤（完整客户端 mod 工程） | 先确认其目标源码树基线，否则工作量不可估 |
| D3 | Bot UI | **不移植**（疑似外挂） |
| D4 | 资源授权 | 私服改版皮肤/BOSS/地图落盘前须用户授权 |
| D5 | DB 改动 | 未获授权禁止 DDL/DML（仅查询） |
| D6 | 超技能（v83 无原生） | 独立系统级工作，单独立项评估 |
| D7 | 优先级 | 建议顺序：A 类资源 → 跨栈上限(99999/金币/等级) → B 类服务端(美容院/签到/角色槽位) → C 类(装备栏/世界地图tip) → D 类其余 |
