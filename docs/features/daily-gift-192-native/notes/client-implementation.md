# 每日礼物（Daily Gift）192 原生资源 + 私有协议 0x3721/0x3722 — 客户端实现说明

> 交付人：dg-client（子代理）  
> 日期：2026-11-23  
> 状态：代码已落盘、Release x64 独立 OutDir 构建通过（error=0）；资源入库未做（按约定留给 lead 统一做）。

## 1. 目标与范围

把 slot2「每日礼物」`UIDailyGift` 从假数据灰盒改成接真实私有协议的真窗，并使用 192 原生资源（`UiAssetResolve::window4("dailyGift")`，节点名小写 `dailyGift`，对应 `UIWindow4.img/dailyGift`）。协议双端已定，服务端由另一子代理实现。

- 不涉及 slot3 `UIDailyCheckin` / `DailyCheckinPackets.h` / `0x3717` / `0x3718`（未触碰）。
- 资源**未**复制/替换 live 包：192 原生 `DailyGiftStage.img`（GMS key，verify 通过，764KB，含 `dailyGift` 全套 741×647 `backgrnd`）已在 `E:/project/MapleStory-Client/tools/_staging_dg/`，按 AGENTS.md §3.1 硬门禁交由 lead 统一「关客户端 → 原子替换 → probe」入库。

## 2. 协议（逐字节，双端已定）

- C→S `0x3721`（`OutPacket::Opcode::DAILY_GIFT_ACTION = 14113`）：
  - `byte action(0=OPEN, 1=CLAIM)` + `[byte day(1..28)]`
  - OPEN：请求配置快照；CLAIM：领取第 day 天奖励。
- S→C `0x3722`（`PacketSwitch::DAILY_GIFT_SNAPSHOT`）：
  - `respType=0` 配置快照：`byte currentDay, int claimedMask, byte levelMin, byte questDone, int killCount, byte premiumLeft, int[28] rewardItemIds, int[28] rewardCounts`
  - `respType=1` 领奖结果：`byte resultCode(0成功/1等级不足/2杀怪不足/3已领过/4非当前日/5月限尽), byte day, int itemId, int count`

## 3. 改动文件与行号（逐条）

### 3.1 `Net/OutPacket.h`
- L315–318：新增枚举注释与 `DAILY_GIFT_ACTION = 14113`（0x3721）。

### 3.2 `Net/Packets/DailyGiftPackets.h`（新建，40 行）
- L21–29：`DailyGiftOpenPacket()` → opcode 0x3721 + `write_byte(0)`。
- L32–40：`DailyGiftClaimPacket(int16_t day)` → opcode 0x3721 + `write_byte(1)` + `write_byte(day)`。
- 写法仿 `DailyCheckinPackets.h`，逐字节对齐服务端。

### 3.3 `Net/PacketSwitch.cpp`
- L333：新增 `DAILY_GIFT_SNAPSHOT = 0x3722`（S→C In 枚举）。
- L717：`emplace<DAILY_GIFT_SNAPSHOT, DailyGiftHandler>();`。

### 3.4 `Net/Handlers/WaveAShellHandlers.h` / `.cpp`
- `.h` L699–703：新增 `class DailyGiftHandler : public PacketHandler`（含 `handle(InPacket&)` 声明）。
- `.cpp` L18：新增 `#include "../../IO/UITypes/UIDailyGift.h"`。
- `.cpp` L93：新增 `#include <array>`（`std::array<int32_t,28>` 解析用）。
- `.cpp` L3414–3501：`DailyGiftHandler::handle` 实现：
  - `respType=0`：逐字段读 12B 固定头 + 两组定长 `int[28]`，每字段带 `available()` 守卫，缺包不越界；读完后 `get_element<UIDailyGift>()`，无则 `emplace`、未激活则 `makeactive()`，再 `apply_snapshot(...)`。**因此首包即使窗口未开也能自动开窗**（侧栏点击只发 OPEN）。
  - `respType=1`：读 10B 结果，调 `on_claim_result(result_code, day, item_id, count)`。
  - 未知 `respType`：WARN + 跳尾。
  - 关键节点均为 `LOG(LOG_INFO/...WARN)`（`[DailyGift]` 前缀），无 DBG 残留。

### 3.5 `IO/UITypes/UIDailyGift.h`
- L34–37：类注释更新为真窗/真协议说明。
- L50：`kKillTarget = 300`。
- L65–71：新增 `apply_snapshot(...)`（定长 `std::array<int32_t,28>` 两数组）与 `on_claim_result(int8_t result_code, int8_t day, int32_t item_id, int32_t count)`；移除旧 `(day, bool, result_code)` 假签名与 `try_claim` / `set_*` 假预留。
- L77–81：`CellVisual` 改为 `Locked / Available / Claimed` 三态。
- L140–145：真数据成员 `reward_item_ids` / `reward_counts` / `reward_icons`（`std::array<Texture,28>`）/ `cells`；新增 `has_snapshot`、`current_day`、`claimed_mask`、`level_min`、`quest_done`、`kill_count`、`premium_left`。
- L184–185：`has_native_bg` + `native_bg`（192 原生整窗贴图）。
- 新增 `Text status_text`（等级门+premiumLeft）与 `result_text`（领奖结果提示）；移除 `fake_kill_current/need` 与 `progress_visual` 之外的假态。
- 私有方法：`refresh_icon_textures()`、`rebuild_cells()`、`refresh_status_texts()`、`request_claim(int16_t day)` 等替换原 `init_fake_data()` / `fake_claim_slot()`。

### 3.6 `IO/UITypes/UIDailyGift.cpp`
- L18–34：include 调整（新增 `ItemData.h`、`DailyGiftPackets.h`、`Locale.h`、`UiAssetResolve.h`、`Graphics/Texture.h` 经头传递）。
- L43–129：构造函数：
  - 28 格初值 `Locked`（无快照不闪假可领）；
  - L70–90：`#ifdef USE_NX` 下 `UiAssetResolve::window4("dailyGift")` 尝试加载 `backgrnd`（节点名小写 `dailyGift`，与 `window4("ShortCutUI")` 范式一致）；有则 `native_bg` + 以贴图真实尺寸设 `dimension/dragarea`，无则 WARN + 回退灰框；
  - L127：`DailyGiftOpenPacket().dispatch()`，构造即请求快照；
  - 关闭钮、28 格 AreaButton、CLAIM AreaButton、New 闪光动画、dimension/dragarea 均按原逻辑（原生 bg 时 dimension 取贴图尺寸）。
- L200–222：`apply_snapshot` 落地 8 字段 + 两数组，`has_snapshot=true`，刷新图标/格子/状态文案。
- L224–234：`refresh_icon_textures` 用 `ItemData::get(id).get_icon(false)` 填 28 格图标（去掉原 L357–358「中部不渲染」限制）。
- L236–256：`rebuild_cells` 按 `claimed_mask` 与 `current_day` 生成 Claimed/Available/Locked；进度目标 = killCount/300。
- L258–263：`refresh_status_texts` 用 `Locale::format("dailygift.level_gate"/"premium_left", ...)` 显示真值。
- L375–408：`draw` 用原生 bg 或灰框 + 原叠层；新增 `status_text`/`result_text` 绘制。
- L424–487：`draw_slot`：真图标居中绘制；可领高亮 / 已领灰蒙+对勾 / 未解锁灰蒙；premium 日金角标保留；闪光仍本地播放但**不预置已领**。
- L491–512：`draw_progress_bar` 显示真 `kill_count / 300`。
- L660–673：`request_claim` 仅对 `Available` 格发 `DailyGiftClaimPacket(day)` + 本地闪光，不预置已领。
- L689–718：`on_claim_result` 按 resultCode 驱动：0 成功置 `claimed_mask`/`cells[day]=Claimed` 并提示；1–5 失败给可见提示且不清状态；全程 `LOG(LOG_INFO/...WARN)`。
- L512 起 `button_pressed`：格子/CLAIM 点击均走 `request_claim`，不再 `fake_claim_slot`。

### 3.7 `locale/en-US.ini` / `locale/zh-CN.ini`
- 更新 `dailygift.rule_3..6`（去掉「假数据/无协议/不渲染」等过时描述）。
- 新增 `dailygift.level_gate`、`dailygift.premium_left`、`dailygift.claim_ok`、`dailygift.claim_fail`（格式化占位 `{}` 经 `Locale::format`）。

### 3.8 `MapleStory.vcxproj`
- L828：新增 `<ClInclude Include="Net\Packets\DailyGiftPackets.h" />`（`DailyGiftHandler` 在既有 `WaveAShellHandlers.cpp` 内，无新 cpp；`.filters` 里 packet 头文件遵循现有约定不登记）。

### 3.9 构建脚本（新增）
- `build_dailygift_20261123.bat`：隔离 `OutDir=E:\project\MapleStory-Client\build-dailygift-192-20261123`，MSBuild Release x64 v142 `/m:1` + `/flp:logfile=build_dailygift_192_20261123.log`。

## 4. 构建验证

```
MSBuild.exe MapleStory.sln /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v142 \
  /p:OutDir=E:\project\MapleStory-Client\build-dailygift-192-20261123\ \
  /m:1 /nologo /v:minimal /flp:logfile=build_dailygift_192_20261123.log
```

- 结果：**EXITCODE=0**，日志统计 `1465 warning(s), 0 error(s)`，耗时 00:03:34.39。
- 产物：`build-dailygift-192-20261123\MapleStory.exe` 已生成；`locale/en-US.ini` / `zh-CN.ini` 已复制到 OutDir。
- 备注：任务提到的并行 `SkillData.h` C2512 未出现（lead 的默认构造修复已在位或无冲突）。全部 warning 均为仓库既有（C4005 USE_NX 重定义、C4100 未用形参、C4189 未用局部变量、C4244/C4267 窄化、LNK4098/LNK4099），非本次引入。
- MSYS 下采用 `-p:` 单横线等价写法无碍；实际脚本用 `/p:` 在 `cmd.exe /c` 下执行成功。

## 5. 资源与后续（留给 lead）

- 192 原生资源 `DailyGiftStage.img`（GMS key，verify 通过，764KB）已由 dg-resource 转 key 落 staging。**客户端代码已具备加载能力**：`window4("dailyGift")` 命中 `UIWindow4.img/dailyGift` 即画原生 741×647 `backgrnd`；未入库时自动回退灰框，功能（协议/图标/状态/进度）仍可联调。
- lead 入库流水线（按 AGENTS.md §3.1）：关客户端 → staging → orange-wz `batch_convert_key(GMS, verify:true)` → `save_as` 异路径 → 原子替换 live → `--resource-probe` / 短启验收（UI 根含 `dailyGift` 且不 0xc0000374）。**禁止** `Copy-Item` 裸拷 `.img`。

## 6. 日志与注释

- 所有入口/出口/分支/异常均上线级日志：handler 收包（`[DailyGift] 0x3722 SNAPSHOT/CLAIM_RES`）、开窗、快照落地、发 CLAIM、领取成功/失败、资源回退 WARN。
- 无 `[DBG-…]` / `LOG_DEBUG` / `console` 等调试残留（本次新增代码全部使用 `LOG_INFO/LOG_WARN`）。
- 关键非显而易见处均有注释：协议字节布局、自动开窗原因、图标 origin、缺包守卫、原生节点名大小写、领奖结果驱动而非本地预置。

## 7. 验收核对

- [x] 0x3721 枚举 + 两个出站包
- [x] 0x3722 枚举 + handler 双 respType 解析 + 自动开窗
- [x] 28 格真图标（`get_icon(false)`）+ 三态（已领/可领/未解锁）
- [x] 进度条 killCount/300、等级门 levelMin、premiumLeft 真值
- [x] 点击发 CLAIM；本地仅闪光，结果由 on_claim_result 驱动
- [x] 构造发 OPEN；关闭/Layout 既有行为不变
- [x] 192 原生资源 `window4("dailyGift")` + 灰框回退（大小写匹配）
- [x] vcxproj 登记新头文件；无新 cpp
- [x] 未触碰 slot3（UIDailyCheckin / 0x3717 / 0x3718）
- [x] 完整注释 + INFO/WARN 日志，无 DBG
- [x] MSBuild Release x64 隔离 OutDir，error=0
- [ ] 资源 live 入库（lead）、真机联调（服务端配合）— 非本任务范围
