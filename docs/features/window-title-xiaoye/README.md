# 窗口标题改造（XiaoYE MS v0.0.1 动态标题）

> 状态：客户端已闭环；破功值**依赖服务端下发**（当前 s9 服务端未下发 → 该段省略，见第 8 节）
> 适用客户端/协议版本：MapleStory-Client（Fyms085 分支）/ 北斗 083 系
> 项目名称：MapleStory-Client（e:/project/MapleStory-Client）
> 作者/日期：2026-09-24
> 产物目录：docs/features/window-title-xiaoye/

## 1. 背景与目标

把游戏窗口标题由固定的 `MapleStory` 改为带实时角色信息的动态标题：

```
XiaoYE MS v0.0.1【角色id-角色名】【职业id-职业中文名】【破功值：x】【当前时间：yyyy-MM-dd HH:mm:ss】【在线时长：dd HH:mm:ss】
```

- 角色 id / 角色名 / 职业 id：直接取游戏内数据。
- 职业中文名：取数据的 `String/Skill.img/<职业id>/bookName`（与技能册一致，中文随数据语言）。
- 破功值：服务端 `LIMIT_BREAK_SYNC(0x3730)` 下发的 int64（伤害上限），未同步时省略该段。
- 当前时间 / 在线时长：客户端本地计算，格式见上。

明确不做的事：

- 不改动任何既有玩法逻辑、不改存档、不改其它 UI。
- 不自行发明服务端协议：破功值沿用北斗既有 `LIMIT_BREAK_SYNC` 定义（`PacketCreator.limitBreakSync(long)`）。

## 2. 影响范围

| 层级 | 路径/模块 | 变更类型 |
| --- | --- | --- |
| 服务端 | 无（本次未改；s9 需补 0x3730 才能显示破功值，见第 8 节） | — |
| 脚本 | 无 | — |
| 管理端/前端 | 无 | — |
| WZ/XML | 无（只读 `String/Skill.img`） | — |
| 客户端代码 | `Configuration.h`、`IO/Window.h/.cpp`、`Net/PacketSwitch.h/.cpp`、`Net/Handlers/CommonHandlers.h/.cpp` | 新增/修改 |
| 插件 | 无（行为对齐现有 ijl15 插件 `windowtitle.cpp`，但两套代码互不影响） | — |
| DB/配置 | 无（基础标题写在 `Configuration::TITLE`） | — |

## 3. 最终实现逻辑

### 3.1 主流程

1. `Window::initwindow()` 仍先写入基础标题（`Configuration::TITLE`，已改为 `XiaoYE MS v0.0.1`）。
2. `Window::update()`（主循环每帧调用）额外调用 `Window::update_title()`。
3. `update_title()` 内部按 **500ms** 节流（标题时间只精确到秒，没必要逐帧重建），
   拼出标题与 `applied_title` 不同时才调用 `SDL_SetWindowTitle()`。
4. `build_title()` 依次拼装各段，全部为空/未就绪的段直接跳过。

### 3.2 数据 / 协议 / 节点路径与语义

| 段 | 来源 | 说明 |
| --- | --- | --- |
| 基础标题 | `Configuration::get().get_title()` | `Configuration.h` 中 `TITLE = "XiaoYE MS v0.0.1"` |
| 角色 id | `Stage::get().get_player().get_oid()` | 玩家 `Char`/`MapObject` 的 oid 即角色 id |
| 角色名 | `player.get_stats().get_name()` | 登录时由服务端下发的角色名（UTF-8） |
| 职业 id | `player.get_stats().get_job().get_id()` | |
| 职业中文名 | `JobData::get(jobid).get_name()` → `String/Skill.img/<职业id>/bookName` | 与技能册同名；部署数据为 zh-CN（已确认 `Data/.nxcache/String.nx` 含「英雄」等中文）。取不到时回退 `Job::get_name()`（英文） |
| 破功值 | 服务端 `LIMIT_BREAK_SYNC = 0x3730`，负载 int64 | `LimitBreakSyncHandler` → `Window::set_limit_break()`；`< 0` 表示从未同步，标题省略该段 |
| 当前时间 | 本地时间 `strftime("%Y-%m-%d %H:%M:%S")` | 每秒变化 |
| 在线时长 | `steady_clock` 计时，格式 `dd HH:mm:ss` | 角色上线（oid > 0）时开始；换角色（oid 变化）或退回登录界面（无角色）时重置 |

协议依据（证据）：

- 服务端：`BeiDou-Server/gms-server/src/main/java/org/gms/net/opcodes/SendOpcode.java`
  → `LIMIT_BREAK_SYNC(0x3730) // 角色破功伤害上限同步`；
  `PacketCreator.limitBreakSync(long)` 只写一个 `long`。
- 插件侧同义实现：`BeiDou-ijl15/ezorsia/windowtitle/windowtitle.cpp`（`kBaseTitle = "XiaoYeMS v0.0.1"`、
  `g_limitBreak < 0 → 省略`）、`LimitBreakBridge.cpp`（`0x3730` + `Decode<uint64_t>`）。

### 3.3 模块与扩展点

- `PacketSwitch` 原先用长度 500 的数组存 handler，装不下 `0x37xx` 这类北斗自定义 opcode。
  本次新增稀疏表 `extended_handlers`（`unordered_map<uint16_t, unique_ptr<PacketHandler>>`），
  `emplace<O, T>()` 用 `integral_constant` 标签分发：`< 500` 进数组，否则进稀疏表（避免常量条件告警 C4127）。
  `forward()` 先按 opcode 选容器取 handler，找不到时沿用原有的 UNHANDLED / OUTOFBOUNDS 告警。
- 后续其它北斗自定义包（0x37xx）可直接照此 `emplace<OP, XxxHandler>()` 注册。

## 4. 资源与同步

- 只读 `String/Skill.img`，不写任何 WZ/IMG。
- 多语言：职业名随数据的 `String` 根（zh-CN 数据 → 中文；英文数据 → 英文），无需额外 i18n 表。
- 部署目录 `E:\MXD\BeiDou-Client-xy` 无需新增资源文件。

## 5. 编码与 i18n

- 工程已开 `/utf-8`（vcxproj `AdditionalOptions`），源码中文常量为 UTF-8。
- `SDL_SetWindowTitle()` 接受 UTF-8，标题可正常显示中文。
- 角色名/职业名来自数据与封包，原样拼入标题，未做转码。

## 6. 产物清单

| 类型 | 路径 | 说明 |
| --- | --- | --- |
| 文档 | `docs/features/window-title-xiaoye/README.md` | 本文件 |
| 代码 | `IO/Window.h`、`IO/Window.cpp` | 标题拼装、节流刷新、破功值保存 |
| 代码 | `Net/PacketSwitch.h`、`Net/PacketSwitch.cpp` | 扩展 opcode 表 + 0x3730 注册 + OpcodeName |
| 代码 | `Net/Handlers/CommonHandlers.h/.cpp` | `LimitBreakSyncHandler` |
| 配置 | `Configuration.h` | `TITLE = "XiaoYE MS v0.0.1"` |
| 构建索引 | `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（`tools\build_smoke.bat`，EXITCODE=0） | **vcxproj 的 `OutDir` 就是部署目录**，构建即直出，无需 robocopy（`x64\Release` 下是旧产物，切勿拷贝覆盖） |

## 7. 验证记录

- 构建：`tools\build_smoke.bat` → `EXITCODE=0`（Release/x64，MSBuild v142 工具集）。
  改动相关文件无新增告警（原 `PacketSwitch.h` 的 C4127 已用标签分发消除）。
- 部署：`tools\build_smoke.bat` 产物直出 `E:\MXD\BeiDou-Client-xy`（vcxproj `OutDir`）。
  首次同步时部署目录 exe 被客户端占用（PID 13976），按用户选择「自行关闭客户端后再同步」；
  重新构建后 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` = 2,379,776 字节 / 2026-09-25 16:26:45（已刷新）。
  注：曾用 `x64\Release`（2026-09-22 旧产物）误覆盖，已由重新构建修正。
- 运行验证：**未跑通**——沙箱无可用登录服务端（需 8484 端口真实握手），
  标题实际效果需用户在本机启动服务端 + 客户端后肉眼确认。
- 建议手工验证点：
  1. 登录界面：标题应为 `XiaoYE MS v0.0.1【当前时间：…】`（无角色段、无在线时长段）。
  2. 进入游戏：出现【角色id-角色名】【职业id-中文职业名】【在线时长：00 00:00:01】并每秒递增。
  3. 换角色回登录：在线时长段消失，再进角色从 00 开始。
  4. 破功值：服务端下发 0x3730 后才出现【破功值：x】。

## 8. 预留、风险与回滚

- **破功值为显式预留**：用户实际运行的 `BeiDou-Server_s9` 的 `SendOpcode` 中没有 `LIMIT_BREAK_SYNC`，
  因此该段当前不会显示（与 ijl15 插件"未同步则省略"一致）。
  如需显示，可在 s9 增加 `PacketCreator.limitBreakSync(long)` 并在角色上线后下发（BeiDou-Server 已有同款实现可抄）。
- 风险面极小：只新增标题拼装与一个包处理；未改动既有 handler 行为。
- 回滚：还原 `Configuration.h` 的 `TITLE` 并撤掉 `Window::update()` 中的 `update_title()` 即可退回静态标题。

## 9. 对外交流摘要

客户端窗口标题改为 `XiaoYE MS v0.0.1` + 角色/职业/破功值/当前时间/在线时长分段显示；
职业中文名直接读 `String/Skill.img` 的 bookName（随数据语言），破功值走北斗既有 `LIMIT_BREAK_SYNC(0x3730)`；
为接收 0x37xx 自定义包，PacketSwitch 增加了稀疏 handler 表。
当前 s9 服务端未下发该包，破功值段暂不显示。
