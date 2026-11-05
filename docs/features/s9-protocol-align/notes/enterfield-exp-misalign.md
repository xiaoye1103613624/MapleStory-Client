# 进图白屏 / 小地图失效根因：SET_FIELD `addCharStats` exp 宽度错配

> 日期：2026-11-05 ｜ 状态：**已定案 / 已修复 / 已重编 / 已实机验证闭环**
> 事实源：`BeiDou-Server_s9`（javaclasses 运行实例 java PID 44760）、`packet_dump.log` 真实抓包字节、本仓库客户端源码、部署目录 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` 实机进图抓帧。

## 0. 现象（用户负反馈）

登录 admin 进游戏后：**整屏纯白、看不到角色、左上角小地图只有一个「大地图」收缩条，没有地区图标和地图预览**。同账号 / 同 mapid / 同 Data 用原生客户端登录 admin 一切正常 ⇒ 资源齐全，锅在客户端代码，不是素材代次问题。

## 1. 结论先行

三个症状是**同一条上游故障链**，外加一个独立的小地图门禁误杀：

```
s9 PacketCreator.addCharStats 用 writeInt(exp)=4B + 尾部 writeInt(0)=4B
        ↕ 客户端 LoginParser::parse_stats 按 read_long(exp)=8B 且无尾部 skip（S10/主线口径）
中间字段整体平移 4B：skip(4)//gachaexp 吞掉真实 mapId 100000000，
再从 spawnpoint 槽读出 16 当 mapid
        → Stage::load_map 找不到 Map/Map0/000000016.img，抛 "map data missing"
        → 异常被 SetFieldHandlers.cpp catch 静默吞，Stage::load:state=ACTIVE 永不执行
        → Stage::draw() 整层 return（背景/瓦片/怪/角色全不画）
        → GraphicsGL.flush() 每帧 glClearColor(1,1,1,1) 露纯白
        → Stage::get_mapid() 无有效值 → UIMiniMap 落到 MIN 收缩条
```

小地图还有第二处独立缺陷：`UIMiniMap.cpp` 拿 `miniMap["width"]/["height"]`（**世界坐标范围** 7444×1391）去比 2048 门禁，恒判过大 → `has_map=false` → 地区图标/预览/两行字全不画。两处叠加，所以「白屏 + 小地图没有」一起出现。

## 2. 决定性证据

### 2.1 运行时落盘垫片（`setfield_trace.log`）

Release 下 `MapleStory.h:77` `#define LOG(level,message) void(0)`，`SetFieldHandlers.cpp:47 enter_game_log` 走 `LOG(LOG_DEBUG)` ⇒ 异常信息**永不落盘**，这是最初查不到实锤的原因。先加文件垫片（`Net/Handlers/SetFieldHandlers.cpp:42/47-55`，`#include <fstream>` + `static std::ofstream trace("setfield_trace.log", std::ios::app)`）再复现，抓到：

```
[SetField] fade done map=16 portal=0 enter_game=1
[SetField] ensure_map_roots ok
[SetField] Stage::load FAILED: map data missing        ← 真凶
[SetField] UI State::GAME ok
[SetField] transfer_player ok
```

修复后同一路径：

```
[SetField] fade done map=100000000 portal=16 enter_game=1
[SetField] ensure_map_roots ok
[SetField] Stage::load ok                              ← 闭环
[SetField] UI State::GAME ok
[SetField] transfer_player ok
```

### 2.2 抓包逐字节（`packet_dump.log`，opcode 0x0b CHARLIST / 0x7d SET_FIELD）

把 s9 `PacketCreator.addCharStats`（`E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/util/PacketCreator.java:174-214`）套在真实 CHARLIST 包上（little-endian）：

| 字段 | offset | 值 |
|---|---|---|
| cid | 4 | 1 |
| name | 8 | `Admin` |
| level | 55 | 1 |
| hp/maxhp/mp/maxmp | 66..72 | 50/50/5/5 |
| **exp (`writeInt` 4B)** | **78** | **0** |
| fame (`writeShort` 2B) | 82 | 0 |
| gachaExp (`writeInt` 4B) | 84 | 0 |
| **mapId (`writeInt` 4B)** | **88** | **100000000 = `00 e1 f5 05`**（射手村） |
| spawnPoint (`writeByte`) | 92 | 16 |
| trailingInt (`writeInt(0)`) | 93 | 0 |
| charLook 起点 | 97 | face 20000 |

- 归档成功会话（21:17，旧服务端）：`@88=10 27 00 00`(gachaExp=10000)、`@92=01 00 00 00`(mapId=1) → 解析正确。
- 本次白屏会话：`@88=00 e1 f5 05`(真实 mapId 100000000)、`@92=10 00 00 00`(16) → 客户端按 8 字节 exp 解析时，`skip(4)//gachaexp` 恰好吞掉 `00 e1 f5 05`，`read_int()` 从 @92 得到 16。
- 两包 `face@23=20000` / `hair@27=30030` / charLook 起点 @97 完全一致 ⇒ stats 头 97 字节结构相同，仅 VALUE 变化，排除「包体结构整体变了」的误判。

### 2.3 三版本服务端 exp 宽度对照

| 源码 | exp 写法 | 尾部 |
|---|---|---|
| **BeiDou-Server_s9**（当前运行） | `PacketCreator.java:208 writeInt(chr.getExp())` = **4B** | `:213 writeInt(0)` = 4B |
| BeiDou-Server_S10 | `:215 writeLong(...)` = 8B | `:220` 尾部 4B |
| BeiDou-Server 主线 | `:212 writeLong` = 8B | `:217` 尾部 4B |

`docs/features/login-flow-ui-ida-align/notes/charlist-parse-fix.md` 记的「exp 8 字节 readLong」是 **S10/主线口径，对 s9 失效**。两种布局从 exp 到 charLook 恰好都是 **19 字节**，所以读错宽度**不抛 PacketError、只把中间字段整体平移 4 字节**——这是最隐蔽的地方：登录/选角/聊天/HUD 全正常，唯独进图 mapid 是错的。

### 2.4 小地图数据齐全（推翻「没实现 / 缺资源」）

`tools/nx_inspect.py` 实测 `Map002.nx Map/Map1/100000000.img/miniMap`：`canvas [bitmap] 465x86`、`width 7444`/`height 1391`（世界坐标）、`centerX 1068`/`centerY 661`/`mag 4`；`info/town=1`、`mapMark='Henesys'`、`hideMinimap=0`；`Map.nx MapHelper.img/mark/Henesys 38x38` 图标、`UI.nx UIWindow.img/MiniMap` 全套切片全在。O(n) 全量扫描 Map002.nx：2213 个 canvas 位图里边长 >2048 的 **0 个**；4456 个 width/height 元数据里 1242 个 >2048（全是世界坐标，被旧门禁误杀）⇒ 旧 `meta_bad` 的 2048 条件 **100% 纯误伤、零正面收益**。

## 3. 修复清单（6 文件，127 插入 / 22 删除）

| 文件 | 行 | 改动 | 性质 |
|---|---|---|---|
| `Net/Handlers/Helpers/LoginParser.cpp` | 154-173 | `statsentry.exp = recv.read_int();`（原 `read_long()`）+ 末尾新增 `recv.skip(4);`（s9 尾部 int）；保留 `read_short fame` / `skip(4)//gachaexp` / `read_int mapid` / `read_byte portal` | **根本修复**：mapid 从 16 回到 100000000 |
| `IO/UITypes/UIMiniMap.cpp` | 211-214 | `meta_bad = !miniMap || canvas_w<=0 || canvas_h<=0`（删掉两个 `>kMaxCanvasEdge`）；`bmp_bad` 仍对 465×86 位图保留 2048 门禁 | 恢复地区图标 / 预览 / 两行字 / MAX 态 |
| `Gameplay/Stage.cpp` | 48-69 | `state = State::ACTIVE` 提前到 `load_map` 之后、`respawn` 之前；地图数据就绪即激活，respawn 异常不再拖住激活态 | 纵深防御 |
| `Graphics/GraphicsGL.cpp` | 1591 | `glClearColor(1,1,1,1)` → `glClearColor(0,0,0,1)` 中性黑兜底 | 空帧不再刺眼纯白 |
| `Gameplay/MapleMap/MapBackgrounds.cpp` | 196/202 | 默认构造 `black(true)`（修未初始化 UB）；`if (black || backgrounds.empty())` 强制铺底 | 保证 Stage 层全屏 quad；对应 `img-evidence-audit.md:49` 未落地的修复 |
| `Net/Handlers/SetFieldHandlers.cpp` | 42/47-55 | `enter_game_log` 加 `std::ofstream` 文件落盘垫片 | 常驻进图诊断（替代 Release 下 no-op 的 LOG） |

## 4. 构建 / 部署 / 验证

- MSBuild Release/x64/v142，6 个翻译单元强制重编（删 obj），**EXITCODE=0，0 error**，协议门禁 `GATE OK: opcode matrix matches both sources (s9 44d0d5d9ca)`。
- 产物 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` 07:54:06 / 4677632 B；`LoginParser.obj` 07:53:21 晚于源码 07:41:20；exe 内含 `setfield_trace.log` 字面量。
- 并发会话曾在同一工作区反复重编、产物一度比修复还旧（4622336 B / 07:37），这是「修了还是白屏」质疑的直接来源——判据必须是**源码 → obj → exe 时间戳链 + exe 特征串**，不能只看构建日志。
- 实机抓帧（`login.auto=true` 自动进图）：射手村地图 / 蘑菇屋 / 角色 Admin 正常渲染；左上角小地图 MAX 态显示「金银岛 / 射手村」两行字 + 金色盾徽地区图标 + 465×86 场景缩略预览 + 玩家标记。取证后 `login.auto` / `packet_dump` 已恢复 `false`。
- 未做 git commit（无授权）。

## 5. 固化判据 / 防复发

1. **协议字段口径以当前对接服务端为准**：`docs/features/s9-protocol-align/` 方向 = 对齐 s9；`login-flow-ui-ida-align` / `charlist-parse-fix.md` 的字节表是 S10/主线历史口径，引用前必须核对服务端 `PacketCreator.addCharStats` 的 exp / tail 宽度。
2. **同宽度错配最危险**：字段宽度变化但总长不变时解析不报错，只是字段平移——用 `packet_dump` 逐字节比对锚点（face/hair/charLook 起点）比看「有没有抛异常」可靠。
3. **Release 运行时观测不能依赖 LOG**：`MapleStory.h:77` 让所有 LOG 变 no-op，异常信息要落盘就得用文件垫片或 Debug 构建。`setfield_trace.log` 常驻保留。
4. **世界坐标 ≠ 位图尺寸**：`miniMap["width"]/["height"]` 是地图世界范围，atlas 门禁只能作用于 `canvas` 位图的 `bmp_w/bmp_h`。
5. exe/源码一致性核对：`vcxproj OutDir=E:\MXD\BeiDou-Client-xy\` 直出部署目录，`run_smoke.bat` 里 `copy x64/Release/...` 那条源路径不存在、不影响；并发会话重建会覆盖产物，验收前查时间戳链。

## 6. 相关文档

- 本切片 `notes/s3-enterfield.md` §2.2/§4 曾记「SET_FIELD 实机渲染成功、无字段级错配」——该结论基于旧会话（mapid=1），已被本次 s9 exp 宽度错配推翻，见本文件。
- `docs/features/login-flow-regression-fix/notes/img-evidence-audit.md:49` 提出的「黑兜底 + 空背景铺底」本次才真正落地。
- `docs/features/ui-v83-window-restore/notes/open-issues.md` C1/B2「本图 has_map=false」是 2048 门禁误杀期间的过期观察，已失效。
