# #4 个人伤害统计 · 阶段 0 证据解析与原生移植方案（2026-11-18）

## 1. 结论先行
当前是"**服务端完整、客户端骨架通但 UI 缺位**"的半成品。补 UI 渲染层即可闭环，**不需重排 opcode、不需改服务端、不碰 live 资源**——属于可立即开工、不涉及资源改写的原生移植工作。

## 2. 三层链路证据（逐条行号，点到面）

### 2.1 服务端发包（BeiDou-Server_s9，已完整）
| 项 | 位置 | 内容 |
|---|---|---|
| 控制收包 | `RecvOpcode.java:25` | `CUSTOM_PACKET(0x3713)` |
| 统计发包 | `SendOpcode.java:358` | `DPT_TRACKER(0x3714)` |
| 三态分发 | `net/server/handlers/CustomPacketHandler.java:41-48` | `DAMAGE_RANK_OPEN/RESET/CLOSE` → `chr.damageRankOpen()/Reset()/Close()` |
| 角色状态 | `client/Character.java:5599-5627` | `damageRankOpen()`(激活+全量快照)、`damageRankClose()`、`damageRankReset()`(清运行时状态) |
| 记录入口 | `Character.java:5629` | `dptRecordObservedPlayerDamage(attacker, dmg)` |
| 发包构造 | `util/PacketCreator.java:341-368` | 三种包（下详） |

**包体格式（权威，服务端侧）**：
```
mode 0 = dptReset()  (:341-345)  → byte 0，无后续
mode 1 = dptPlayerUpdate() (:347-355) → byte 1, int charId, int jobId, string name, long totalDamage
mode 2 = dptSkillUpdate() (:357-368) → byte 2, int skillId, string name,
         long deltaDamage, long totalDamage, long maxDamage, long minDamage, int count
```

### 2.2 客户端现状（MapleStory-Client，骨架通、UI 缺）
| 项 | 位置 | 内容 |
|---|---|---|
| opcode 声明 | `Net/PacketSwitch.cpp:328` | `DPT_TRACKER = 0x3714` |
| handler 注册 | `Net/PacketSwitch.cpp:686` | `emplace<DPT_TRACKER, DptTrackerHandler>()` |
| handler 实现 | `Net/Handlers/WaveAShellHandlers.cpp:3304-3347` | `DptTrackerHandler::handle`：解析 mode 0/1/2，字段与服务端对齐（read_byte/int/string/long） |
| **缺口** | `WaveAShellHandlers.cpp:3338-3346` | 处理结果仅 `UIAdmin::set_message(tip)` 调试提示 + `LOG(DEBUG)`，无排行榜窗口、无 F12 开关、无玩家/技能双页、无拖拽/滚动/最小化 |

### 2.3 参考包（Kaentake 注入 DLL，逻辑可翻译）
`E:/MXD/扩展改动/个人伤害统计/统计1/Tengutake/src/`
| 文件 | 可复用逻辑 |
|---|---|
| `uiDamageRank.h:11-50` | `CDamageRankData` 数据结构：`PlayerEntry{charId,name,job,totalDamage}`、`SkillEntry{skillId,skillName,delta/total/max/minDamage,count}`，`std::map` 按 id 索引 + `Snapshot*()` 排序快照 |
| `uiDamageRank.h:52-91` | `CUIDamageRank`：`Mode{Player,Skill}`、按钮 id（reset1003/switch1004/auto1005/minmax1002/close1000）、`HandleTrackerPacket`、`AnimateTick`、鼠标四件套 |
| `uiDamageRank.h:104-200` | 渲染函数族（`BuildCanvas`/`DrawPlayerPage`/`DrawSkillPage`/`DrawGaugeBar`/`DrawScrollBar`）+ 滚动/翻页/命中测试全套 |
| `damagerank_client_readme.md:29-46` | 控制包 subtype 1=open/2=reset/3=close（**与 s9 的 CustomPacketHandler 三态对齐**） |

## 3. 原生移植方案（对齐当前客户端框架）

### 3.1 数据结构（照搬，无协议风险）
新建 `Character/DamageRank.h`，移植 `CDamageRankData`：
- `PlayerEntry` / `SkillEntry` 字段与 §2.3 一致，数值类型用 `int64_t`（对应协议 long）。
- `std::map<int32_t, ...>` 存储 + `Snapshot*()` 返回按 totalDamage 降序的 `std::vector`。

### 3.2 UI 类（对齐 `UIMonsterBook` 范式，见 `IO/UITypes/UIMonsterBook.h:46-61`）
新建 `IO/UITypes/UIDamageRank.h/.cpp`：
- 继承 `UIDragElement<PosDAMAGERANK>`（ dragged 窗口，对应参考包拖拽需求）。
- 覆写 `draw(float)/update()/button_pressed(uint16_t)/send_cursor(bool,Point)/send_key(int32_t,bool,bool)/send_scroll(double)/get_type()`——**这正是 `UIElement.h:60-130` 的契约**。
- 按钮 id 沿用参考包 1000/1002/1003/1004/1005（`UIMonsterBook.h:92-102` 证明原生用同类 enum 段）。
- 双页：`Mode::Player` 显示 name/job/damage 条形榜，`Mode::Skill` 显示 skillName/总伤/最大/最小/次数；`send_scroll` 驱动滚动（参考 `UIMonsterBook::send_scroll`）。

### 3.3 状态接入
- `UIElement::Type` 枚举（`IO/UIElement.h:65+`）新增 `DAMAGERANK` 项。
- 位置策略：`LayoutPolicy::SavedPosClamped`（`UIElement.h:44`，保留拖拽坐标 + 越界夹回）。
- F12 开关：走现有 `send_key` 分发路径（参考包 `F12test.cpp`，需再核当前客户端按键框架落点——**阶段 1 首件事**）。
- handler 改造：`WaveAShellHandlers.cpp:3340-3346` 的 `UIAdmin::set_message(tip)` 改为喂给 `UIDamageRank`（`upsert` 玩家/技能 + `reload`），保留 DEBUG 日志到收口时清理。

### 3.4 资源（不碰 live，先预留）
窗口板/滚动条/按钮/字体优先复用 `UIWindow.img` 既有节点（如 Basic.img 的滚动条、通用按钮）；若缺专用美术，按 skill 门禁 16 走 Photoshop MCP 出图并归档到 `docs/features/.../ui/`，**本阶段不导入 live**。

## 4. 验证门禁
- 客户端 VS 构建 error=0 / lnk=0（贴输出）。
- 协议回归：mode 0/1/2 各造一包，确认 UI 落地字段（**真机发包未验收须如实声明**）。
- 不新建 opcode（0x3714 双端已一致），不改服务端，不动 live 资源。

## 5. 阶段拆解（下一步）
1. 核按键框架落点 + UIElement::Type 注册点 + UIDragElement/Pos 定义位置（阶段 1 开工前定位）。
2. 落 `Character/DamageRank.h` 数据结构。
3. 落 `IO/UITypes/UIDamageRank.h/.cpp` UI 与渲染。
4. 改 handler 接入 + 加 Type 枚举 + F12。
5. 构建验证 + 真机勾验（用户执行）。
