# 性能仪器 + 大场景结构优化 · Wave P 首批（2026-11-04）

> 范围：①64 位 vs 原生 32 位定量评估（PE 实测）；②窗口标题实时 FPS/延迟；③视口剔除 + 伤害数字钳制。
> 产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2026-11-04 15:03 重建，GATE OK / EXITCODE=0）。
> 扫描/评估脚本：`tools/pe_bits.py`（PE 位与 LAA）、`tools/gap_scan_2026_11.py`、`tools/gap_c2s_refined.py`。

## 1. 64 位评估结论（PE 头实测，非推测）

| 二进制 | 机器类型 | DllCharacteristics | LAA | 含义 |
| --- | --- | --- | --- | --- |
| 原生 `BeiDou.exe`（V17.7） | **x86 PE32** | `0x0000` | ❌ | 用户地址空间硬顶 **2GB** |
| 原生 `Gr2D_DX8.dll` | x86 PE32 | `0x0000` | ❌ | 渲染 DLL 同样 2GB |
| 本端 `MapleStory.exe` | **x64 PE32+** | `0x0010` | —（x64 不适用） | 地址空间 128TB |

位宽差距的**诚实账目**：
- **不是** FPS 魔法：纯渲染循环换 64 位不会凭空 +X%（本项目主循环为 GPU 提交 + CPU 取帧，指令级差异 <2%，x64 相对价值在基线保证——SSE2 恒开，无需 32 位的 `/arch:SSE2`，本 vcxproj 实测未设该旗标）。
- **是** 上限解除：原生 32 位无 LAA ⇒ 大地图 atlas 缓存、`.nxcache` 映射、数百怪/数千纹理堆到 ~1.5-2GB 即崩/ textures 拒绝；本端 x64 无此天花板，配合 Wave P 之后的 atlas 配额（见 §4）才有意义。
- 附加收益：x64 调用约定 16 个通用寄存器（vs x86 ~8）、更大指针天然对齐；负面——指针/结构膨胀 ~10-15% 内存占用、缓存密度略降，对本负载（大缓冲流式访问）可忽略。

## 2. 标题实时 FPS / 延迟（已交付）

- **FPS**：`Window::end()` 每次 SwapWindow 计数 +1；`build_title()` 沿用既有 500ms 节流窗折算（`perf_frames/perf_fps_since/perf_fps`，mutable，无新增定时器）。
- **延迟**：内核平滑 RTT，零额外发包。
  - Windows：`WSAIoctl(SIO_TCP_INFO)` → `TCP_INFO_v0.RttUs`（`Net/SocketWinsock.cpp::rtt_us`）。SDK 头门控 `NTDDI>=WIN10_RS2`，工程原本不定义 `_WIN32_WINNT`——已在 `SocketWinsock.h` 顶部钉 `WINVER/_WIN32_WINNT=0x0A00 + NTDDI_VERSION=0x0A000003`，并显式 `#include <mstcpip.h>`。
  - POSIX/Android：`getsockopt(TCP_INFO).tcpi_rtt`（`SocketPosix.cpp`，补 `<netinet/tcp.h>`；本轮未在 Android 交叉构建里复验，列入 §5）。
  - Asio 后端：显式桩返回 false（USE_ASIO 默认关，asio 头未随仓库分发）。
  - 展示：连接可用 → `【FPS:101 延迟:12ms】`；未连接/不支持 → `延迟:--`。
- **真机证据**（登录页，未连服）：
  `MainWindowTitle = XiaoYE MS v0.0.1【FPS:101 延迟:--】【当前时间：2026-11-04 15:05:19】`（PID 8904，FPS 采样成立；`--` 为登录服未启动的预期态）。

## 3. 大场景流畅性（怪物海量/特效/掉落）首批

| 改动 | 位置 | 逻辑 |
| --- | --- | --- |
| **视口剔除** | `Gameplay/MapleMap/MapObjects.cpp::draw` | 锚点屏幕坐标 = 世界坐标 + 相机偏移；越出 `视口±512px` 直接跳过 `mmo->draw`。覆盖 mobs/drops/chars/npcs（四个 `MapObjects` 集合共用）。原实现屏幕外对象每帧照常取帧动画+提交 quad——密集图的最大白烧。512 余量盖住名牌/血条/掉落标签，插值抖动不吃进。 |
| **伤害数字钳制** | `Gameplay/Combat/Combat.cpp::apply_damage_effect` | `damagenumbers` 上界 160，超限丢最老（最老已近淡出尾声，观感无损）。AoE 多段群怪时列表不再瞬时堆几百项，update/draw 尖峰有界。 |

既有可复用旋钮（本轮确认存在，未改）：`window.vsync`（垂直同步）、`ui.sysopt.quality`（0-100 特效密度上限，过半同步开 VSync）、`Constants::TIMESTEP=8ms`（125Hz 逻辑帧）+ 主循环 `MAX_FRAME_UPDATES=4` 补帧钳（防滚雪球黑屏，既有）。

## 4. 后续（仍属 Wave P/0 队列，未在本批）

1. **基线数字**：perf-budget `notes/baseline.md` 填写（标题 FPS 已是仪器；仍要 [BOOT] 分段与进图首帧数——Debug/文件垫片）。
2. **atlas LRU 逐出**替代换图整表 `unload_atlas()`（进图黑屏结构性根治）。
3. **进图预传 back/tiles**（选角应答已知 map id）。
4. 特效侧：MapEffect 每图仅 1 活动槽（`MapEffect.h` 实测单实例），如需多特效并发另行评估；掉落名标签已随剔除受益。

## 5. 验收状态

- [x] `tools/build_smoke.bat` → GATE OK（opcode 矩阵与 s9 `44d0d5d9ca` 一致）/ MSBuild EXITCODE=0（Release x64）
- [x] 部署 exe 时间戳 15:03；真机进程标题含 FPS 段（上引实测串）
- [ ] 登录进图后 `延迟` 显示数值（需 s9 服务在跑；连接建立即出）
- [ ] Android/CMake 线复验 SocketPosix rtt_us（本批只改源码未跑交叉构建）
- [ ] 密集场景前后帧率对比（拿本轮标题仪器在怪多地图读数即得；基线批补正式数字）

## 6. 触碰面与并发提示

- 本批改动文件：`IO/Window.{h,cpp}`、`Net/Session.{h,cpp}`、`Net/SocketWinsock.{h,cpp}`、`Net/SocketPosix.{h,cpp}`（**该文件另有并发会话 DBG-SOCK 移除在途 hunk，提交时需拆**）、`Net/SocketAsio.h`、`Gameplay/MapleMap/MapObjects.cpp`、`Gameplay/Combat/Combat.cpp`。
- 未提交（等用户指令）；未动服务端、未动资源。

## 7. 长跑内存审计与换图丝滑第二批（2026-11-04 续，全部现场复核）

> 口径：子代理审计先证后用——其"UI 关闭不删除"结论被现场证伪（`UIStateGame::remove` L822 实为 `element.release()`；`Audio.cpp` 有 4 处 `BASS_StreamFree`），未入账。以下只收 file:line 复核过的条目。

### 7.1 坐实的持续增长（已修）

| # | 位置 | 症状 | 修复 |
| --- | --- | --- | --- |
| L1 | `Util/Resource/ImgSource.cpp` load_single_map | **每次换图 append 解压后的地图树，只增不减**（close() 仅退出时调）→ 挂机反复换图/回城单调增长 | 单图 file 从 s_files（混标准根，不可头部淘汰）拆入 `s_map_files` **有界 LRU 环（MAP_KEEP=3）**：当前图+前两张宽限，超出销毁最老；上界 3×单图体积恒定 |
| L2 | 同文件 ensure_map | 回城/折返**同图回访照样磁盘读+解压**（换图时长最大可消除项） | 环内命中 → `RootTable::bind` 重绑即返 + 槽提队尾；快路径必须在 load_single_map **之前**（第一版放读盘后=白优化，已纠） |

悬垂风险已逐项核查：`nl::Texture` 存 `nl::bitmap` **值拷贝**（Texture.h L64 注释坐拔 data() 全局静态缓冲）；Stage 侧 `tilesobjs/backgrounds/physics/mapinfo/portals` 每次 `load_map` 值语义重建，旧图节点随替换销毁——旧 file 被逐时最长已隔 3 次换图，宽限窗覆盖 atlas/贴图残留引用。

### 7.2 复核过的"不是问题"（防下次重怀疑）

- `UIStateGame::remove` 真删除；`MobDropData` 为 `insert_or_assign` 按 mob_id 覆写（**有界**，种类天花板）；`MonsterBook::add_card` 是 `cards[card]=level` 覆写（有界）；`TimedQueue::update` 到点全 pop（有界）；`GraphicsGL::clear()` 有商城/退出 3 个运行期调用点 + getoffset 满压自动回收；`Combat::damagenumbers` 本批已加 160 钳。
- 施法上行**没有**人为延迟：`Combat.cpp:175` 攻击结果即时 `AttackPacket.dispatch()`（400ms 队列只是他人攻击的显示调度）。

### 7.3 换图/传送淡入窗口实测参数（结构性调优，本轮）

淡出 + 淡入各走 `1/step` 个 TIMESTEP(8ms) 拍。真实现场是 4 处独立 `fadestep = 0.025f`（各 40 拍 = 320ms/侧，合计 640ms 黑场仪式），**无** UIChannel 0.015 那处（上一稿文档笔误，已按 grep 校准）。

| 位置 | 场景 | 原值 | 现值 | 效果 |
| --- | --- | --- | --- | --- |
| `Net/Handlers/SetFieldHandlers.cpp:89` | 进游戏/换图/传送主链 | 0.025 | **0.04** | 每侧 320→200ms，总黑场 640→400ms |
| `Net/Handlers/CashShopHandlers.cpp:650` | 进商城 | 0.025 | **0.04** | 同上 |
| `IO/UITypes/UICashShop.cpp:313` | 出商城 | 0.025 | **0.04** | 同上 |
| `IO/UITypes/UIQuit.cpp:206` | 退出确认 | 0.025 | 0.025（**不动**） | 退出场景无丝滑诉求，最小改动 |

保留黑场是为遮装图卡顿（配合 §7.1 L2 近张重绑，回访同图近乎无感）；0.04 是"遮得住 + 等不到"的当前平衡点，真机若觉闪可回调 0.035。切频道走 SetField 主链（同一 transition），已随之提速。

### 7.4 验证

- `build_smoke`：GATE OK / EXITCODE=0（16:35 部署 exe）；scratch OutDir 链接探针同样绿（占用冲突期证明代码可构建）。
- 真机：auto-login 端到端标题 `【1-Admin】…【FPS:101 延迟:--】`（登录→选角→进图全链在新剔除/钳制/环缓存下存活）；测毕已恢复 `auto: false`。
- 待长跑量化（下批 Wave 0.1 一并）：8h 挂机 + 100 次换图的工作集曲线、boot_perf 分段基线。

## 8. Wave 0.1 基线首采 + 进图 15× 提速（2026-11-04 晚，真机 A/B）

### 8.1 仪器

`Util/PerfLog.{h,cpp}`：Release 可用的分段毫秒日志（`debug.perf_log` 开关，默认 false，
落 `boot_perf.log`，5000 行封顶）。存在理由：Release 下 LOG() 是 no-op（坐实），
`[BOOT]` 分段从未有真数——perf-budget 基线空白的根因。插桩点：
`ensure_map` 冷路径毫秒 + SetField 淡入回调五段（unload/roots/stage/unlock/total 累计）。

⚠ 仪器自身踩过的坑（都当场修）：① xy-config 解析器只支持**整行注释**，行尾内联
注释会让值失效（第一次采不到数即此因）；② 嵌套 lambda 要读外层按值捕获名，
捕获列表必须 `[&]`（C3493）。

### 8.2 基线首采（修复前，同机同图 mapid=1）

```
[PERF] SETFIELD after_ensure_map_roots cum_ms=7507   ← 整轮 92%
[PERF] SETFIELD after_stage_load      cum_ms=8121
[PERF] SETFIELD total                 cum_ms=8138
```

**结论：换图黑屏/进图卡顿的真凶不是贴图上传，是淡入回调里同步跑 ensure_map_roots
（Mob/Skill/Quest/Sound/String 等全部剩余根的磁盘读+解析），7.5 秒。**

### 8.3 修复：第二阶段后台预热（remaining 根挪入登录期空闲窗口）

- `prewarm_game_roots` 后台线程追加 `ensure_remaining()`（登录→选角→玩家浏览
  角色的十几秒是纯空闲；选角根就绪点不变，game_roots_ready 挪到全量之后）。
- 并发安全补齐（这是本改动唯一的真风险，逐项做）：
  - `ImgSource::ensure()` 全程罩 `s_load_mtx`（判重+解析+绑定原子化——原先
    裸 `std::set/vector` 双线程读写是 UB）；`ensure_map` 的 LRU/绑定段同罩。
  - `ensure_remaining` 循环内 poll 走 `s_pump_enabled` 门（后台线程禁泵 SDL）。
  - `close()` 先等 `s_bg_loading` 落定再持锁清表（退出撞后台装载的竞态防护）。

### 8.4 A/B 复测（修复后，同机同图）

```
[PERF] SETFIELD after_ensure_map_roots cum_ms=0      ← 7507 → 0（根已全预热）
[PERF] SETFIELD after_stage_load       cum_ms=524
[PERF] SETFIELD total                  cum_ms=539    ← 8138 → 539，15.1×
```

**GM `!warp` 真换图二次验证（第一次进图之外的独立样本）**：

```
[PERF] SETFIELD after_unload_atlas   map=100000000 cum_ms=0
[PERF] SETFIELD after_ensure_map_roots map=100000000 cum_ms=0
[PERF] IMG mapload cold mapid=100000000 ms=2         ← 目标图已被预热环/绑定覆盖
[PERF] SETFIELD total map=100000000 cum_ms=313       ← 换图回调全程 0.31s
```

（SendKeys 驱动在跑实例聊天栏 `!warp 100000000` 实测，非重建会话。）

进图淡入回调 8.1s → **0.54s**；叠加 §7.3 fade 提速后，玩家感知黑场 ≈ 200ms 淡出
+ 0.5s 装载 + 200ms 淡入，其中装载段首次小于黑场总时长（装图被完全遮住）。
`mapload cold ms=0` 行是选角期 ensure_map(1) 已被后台预热绑过、本轮走近张重绑的读数。

### 8.5 采集期环境处置

`debug.perf_log` 与 `auto` 为本次采集临时打开（部署机 xy-config.yaml），
**测毕均已恢复 false**。基线原始 12 行日志归档
[evidence/boot_perf_2026-11-04.txt](evidence/boot_perf_2026-11-04.txt)。
构建 GATE OK / EXITCODE=0（17:26 部署）；A/B 全程真机 auto-login 无人值守跑通。

## 10. 启动分段首采 + 「延迟:--」根因定案（2026-11-04 晚，BootTimer 接入 perf 后）

### 10.1 `[BOOT]` 分段首次拿到真数（此前 Release 永不可见）

```
[PERF] BOOT window=564ms / patch=0ms / resource=8225ms / ui=345ms
[PERF] IMG mapload cold mapid=16 ms=0     ← 后台预热已把单图绑好
[PERF] QSI custom=0                        ← Wave1 快捷栏下行接收正常
[PERF] SETFIELD total map=16 cum_ms=17     ← 进图淡入回调（预热后）
```

`Util/Patch/BootTimer.h` 析构里补打 `perf::mark`（4 个用点全在 MapleStory.cpp），
`[BOOT]` 段从此 Release 可采。**冷启动 9.4s 里 resource 占 8.2s（87%）**——
优化头牌回到 resource（gather/解析），补丁线 0ms、进图线 17ms 均已无欠账。
本轮只把仪器接全，刀按优先级排队（Wave 0.2 第二子弹，未动）。

### 10.2 「延迟:--」进图后仍 --：对照实验定案（tools/_rtt_probe.c 留档）

| 实验 | 结果 | 结论 |
| --- | --- | --- |
| A `SIO_TCP_INFO`+v0，in=NULL | wsa=**10022** | 复现游戏内失败 |
| B `SIO_TCP_INFO`+v1，in=&IPPROTO_TCP | wsa=**10022** | in-buffer 用法假设**证伪** |
| C `getsockopt(TCP_MAXSEG)` 同 socket | **r=0，mss=65495** | socket 与查询机制全活着 |

**根因：回环链路（127.0.0.1，单机部署常态）tcpip.sys 不持有该连接，
SIO_TCP_INFO 必然 WSAEINVAL。** 本机自连时延迟字段永久 `--`（回环延迟本身
也无网络意义）；连 LAN/远端服时调用有效。机器实测 Win11 25H2——此前注释写
"OS 版本不足"是错的，已按实测改写（rtt_us 失败路径另有 3 次限量的
`perf::mark("RTT", …)` 诊断码，现场再遇 `--` 一眼定位）。
单机也想出数 = 应用层回声 RTT，s9 通道已勘察：CUSTOM_PACKET 回显要 GM4+，
干净做法需动服务端——**未做，显式预留**。

## 11. Wave 0.2 第二子弹收案：gather 剪枝（修了实 bug，但收益不可测）（2026-11-05 晨）

### 11.1 过程三段

1. **首版剪枝（off-by-one，已修）**：`gather` 对 `Data/Map` 下潜前跳过顶层 `Map/`
   子树（97MB/5364 文件被 `load_map_helpers` 读完即弃）。判据写成
   `it.depth() == 1` —— **MSVC 实测根直属子项 depth 从 0 起**（dprobe：
   `Back` 是第一个条目 depth=0），条件永不命中，剪形虚设。
   复采 `Map=5733ms` 与基线 `5417ms` 持平才暴露，A/B 噪声逼出了真 bug——
   没有这轮"看起来失败"的测量，off-by-one 会一直躺在代码里。
2. **修复版**：`depth()==0` + `disable_recursion_pending`；独立探针
   （prune_check.exe）实测判据：跳过 87MB/5364 文件、保留 549MB ——
   裁剪量占 Map 根 IO 的 ~14%。
3. **"修后复采"错账（今日晨纠正）**：昨夜 23:26 那轮 `Map=7595/Sound=3305/
   Character=7218` 全线普涨，当时判为"机器噪声、剪枝不可分辨"——**归因错了
   一半**：今晨核对 exe 时间戳，那轮跑的仍是 22:46 的 **off-by-one 失效版**
   （真修复 07:55 才首次编成），且当时并发会话正构建（07:37 build_verify、
   夜间 C1041×272 为证）。Sound/Character 普涨那部分仍是并发/缓存噪声不假，
   但"剪枝不可分辨"的结论把**未测的东西当成了测过**——不可采信。

### 11.2 真·修后复采（07:55:19 构建 depth()==0 版，07:59 单实例清洁测量）

```
[PERF] ROOT Map=3074ms        ← 失效版 5417/5733，−43%/−47%
[PERF] ROOT Sound=2021ms      ← 剪枝不触碰：1893~3305 区间内，正常
[PERF] ROOT Character=3368ms  ← 同上（4993~7218 区间下沿）
[PERF] BOOT resource=5814ms   ← 基线 8090/8378，−28%~−31%
```

**判定：剪枝有效。**降幅（−2.4s）超纯 IO 占比（87MB/636MB≈14%）的解释力，
但量级可自洽：被跳过的不只是 87MB 字节，还有 **5364 次 ifstream 开合 +
目录项 stat + vector 增长 + open_merge 对文件数的超线性合并开销**——
文件数才是这个剪枝真正砍掉的东西。**Wave 0.2 结论由"关闭"改判为"完成"。**
教训入账：A/B 前必须核对 **exe 构建时间戳 ≥ 最后一次源码修改时间**，
否则测的是旧二进制（本轮差点把真修复错杀成"不可分辨"）。

修复本身保留，理由是语义可证明而非计时：被跳过的字节 100% 会进
`load_map_helpers` 的 `Map/` 前缀过滤器后丢弃（纯死 IO + 纯死堆分配），
x64 下也是白烧；剪掉它不改变任何节点内容（单图走 `load_single_map` 直开）。

### 11.3 若日后要真提速启动（预留路线，未做）

大头是 helpers 本体 549MB（Obj 362 + Back 157）的解析，可选：
① `open_merge` 前多根 gather 并行（线程=min(核数,根数)，解析仍在锁内）；
② `.nxcache` 式解析结果落盘复用（首建后启动只 mmap）——收益最大但改动面最大；
③ boot 根再收窄（Map 不可移除：登录背景本体在 Map001["Back"]["login.img"]，
   已 grep 证伪过一次"装饰件"假设）。测量仪器（ROOT 计时 + debug.perf_log）
   常备，换机器/接 SSD 阵列后数字才可分辨时再回来开刀。

## 9. 长跑烟雾测：工作集平直曲线（2026-11-04 晚，修复后 exe）

方法：auto-login 进图 → 静置 20s → GM `!warp` 往返 100000000↔100000104 四次，
每次后采 WorkingSet/PrivateBytes（SendKeys 驱动，无人工）。

| 采样点 | WS(MB) | Private(MB) |
| --- | --- | --- |
| 进图后 | 3296 | — |
| 静置 20s | 3296 | 3778 |
| warp A | 3296 | 3779 |
| warp B | 3296 | 3778 |
| warp A2 | 3297 | 3779 |
| warp B2 | **3296** | **3778** |

**判定：4 次换图零增长（±1MB=采样噪声）——§7.1 L1 泄漏修复 + MAP_KEEP 环生效。**
修复前同场景每换一图净增一张解压树（数 MB~数十 MB），无上限。

### 9.1 体量说明（3.3GB 不是回归，是设计代价，且反证 64 位收益）

第二阶段全根预热把 34 根解压树常驻（+ GPU atlas + .nxcache 映射）= WS ~3.3GB。
- 原生 32 位无 LAA（PE 实测 `DllCharacteristics=0x0000`，2GB 硬顶）**装不下这套
  全预热** —— 这正是"64 位到底提升什么"的定量回答：把「进图 7.5s 同步装载」
  变成可行（挪进后台常驻），而不是让同一算法跑快点。
- 若某机型内存吃紧，退路明确（未做，预留）：`prewarm` 第二阶段改按需根装载
  （回到 §8.2 的 7.5s 首进图），或全根环做 LRU。单机自用机器内存充足，默认全预热。

### 9.2 未跑项（如实）

8h 连续挂机长跑（烟雾测是 2 分钟前置信号，替代不了）；GPU-only 内存曲线
（nvidia-smi 未采）；商城反复进出的 CashShopParser 向量回收已核为
`insert_or_assign` 语义（有界），未实机量化。
