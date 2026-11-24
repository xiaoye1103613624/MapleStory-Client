# 原生证据补证（IDA/REA/服务端源码）— 2026-11-22

> 分支：`feature/dev_0.0.1`
> 性质：只读取证，未修改任何客户端/服务端源码、未改 IDB、未连库。
> 目的：补齐两批共 10 问中三处「资料不足」项，供复查与后续实现引用。
> 工具：REA CLI `rea-agents@6.8.0`（provider=ida）、IDA MCP `http://127.0.0.1:13340/mcp`（session `9914dd04`，指向 `E:\MXD\反编译IDA\v83\`\`Angel.i64`）、`ops_inspect_local_file` / `ops_grep`。
> 区分：**观察**＝反编译/源码直接读到；**推断**＝基于结构的判断。

---

## 补证 1 — 技能命中特效（龙咆哮 1311006）原生 UOL

### 结论（翻案）
Angel v83 原生**没有**视觉 hit 特效播放器，也没有从 SKILLENTRY 按 hit 取 UOL 的函数：
- 全库 UOL getter 仅 8 个，技能相关只有 `SKILLENTRY::GetEffectUOL@0x932d40` / `GetScreenEffectUOL@0x936304` / `GetSpecialUOL@0x411256` / `MOBSKILLLEVELDATA::GetEffectUOL@0x66c215`；**无 GetHitEffectUOL**。
- 全库 `CAnimationDisplayer::Effect_*` 仅 11 个，**无 Effect_SkillHit / ShowSkillHitEffect**。
- `CUser::ShowSkillEffect@0x933990`（2256 行）只调 GetEffectUOL/GetScreenEffectUOL + `Effect_SkillUse`，**全函数不取 hit 节点**。
- `CMob::ShowHitEffect@0x66fb4d` 只做受击姿态状态机（融合值 12–20/21–37/6–8 + `sub_668AAD` + `PrepareActionLayer` + `GetActionDelay`），**不加载 UOL、不播视觉特效**。
- `CMob::OnHit@0x668b83` 中的 hit UOL 是**声音**：`SP_1591_SOUND_SKILLIMG_07D_HIT`(:140)、`SP_3306_SOUND_CASHEFFECTIMG_08D_HIT`(:191) → `IWzResMan::GetObjectA`(:175/:217) → `CSoundMan::PlaySE`(:237)。
- 本端 `Gameplay/Combat/SkillHitEffect.cpp:24/31/43/69/93` 的 `src["hit"]["0"]` 是**本端重建约定**，不能反推原生。

### 意义
「释放用 `Skill/[id]/effect`、命中用 `Skill/[id]/hit`」在本端是**应当成立**的分离设计，与原生「施法侧 / 受击侧分离」语义一致。
边界：只能说「释放节点原生确证、命中视觉节点在本端重建中独立实现」；**原生是否/如何播放 hit 视觉节点未确证**（资料不足）。

### 证据
一手函数/地址详见对话；已落盘证据：`docs/features/skill-angel-calibration-2026-11/evidence/angel-skill-facts.md`（:648/:1378-1385/:1417-1422）、`angel-skill-decompile.json`、`angel-skill-symbol-inventory.json`、`docs/features/mob-lifecycle-angel-review-2026-11/evidence/ida/OnHit_CMob.c:90-93,140,175,191,217,237,244`、`ShowHitEffect_CMob.c:1-55`。字符串缓存仅 1281 条，`find_regex` 对 hit/effect 正则命中 0（README:62 的历史局限一致）。

---

## 补证 2 — 怪物 foothold 平台边缘（CVecCtrlMob 首次反编译）

### 结论（突破 + 翻案）
README §10.5.4 自承「9KB CVecCtrlMob 未逐字节复刻」，本轮首次反编译其核心碰撞函数：
- **原生有 foothold 边缘钳位 + 翻向**（观察）：`CVecCtrlMob::CollisionDetectWalk@0x9bdcb1`、`CollisionDetectFloat@0x9be14f` 依据当前 fh 的 prev/next 邻接算允许 x 区间，20px 内缩（`dbl_AFE890=20.0`），越界 `CVecCtrl::SetInput(this, ±1, 0)` 翻向 —— 即原生版 TURNATEDGES。
- **原生有出生移动矩形**（观察）：`CVecCtrl::SetActive@0x9b12a8` 把 x 夹 `[this+78,+80]`、y 夹 `[this+79,+81]`；`Init_CMob.c:149/164` 出生时取 `GetFoothold(dwSN)` 交给控制器。
- **原生并非只在出生单段平台巡逻**（推断）：state 2 用当前 fh 与相邻 fh 并集区间作钳位，允许沿连续 fh 链行走，仅链末端/不可走邻板翻向。**未发现主动寻找下层平台下跳的代码**。
- **本端 `get_edge()` ≥3 段返回地图墙无原生对应**（判定）：`Gameplay/Physics/FootholdTree.cpp:317-351`，left 有 prev 且有 prev.prev → `return walls.first()`；right 对称 → `walls.second()`。原生按真实 fh 邻接 + 20px 内缩，不存在「第 3 段改夹地图墙」的阈值。这是**本端自造缺陷**，会让长平台上的怪走到地图边界而非平台末端。
- 注意 `get_wall()` 先于 `get_edge()` 执行且 `TURNATEDGES` 命中一次即清（`FootholdTree.cpp:123-145` / `Mob.cpp:320-327`），实际行为会间歇回到地图墙钳位，进一步偏离原生稳定边缘翻向。

### 不能断言的部分
「原生怪物只在出生平台巡逻、不主动跳下」— **资料不足，不能断言**。只能说原生有边缘翻向、未见主动下跳目标；掉落源于 jump/重力失支撑。移动 NPC（Npc.cpp:154/219-253/271-294 只 set 不 re-arm）同样偏离原生矩形约束。

### 证据
`CVecCtrlMob::CollisionDetectWalk@0x9bdcb1`(0x3d5B)、`CollisionDetectFloat@0x9be14f`(0x280B)、`CVecCtrl::SetActive@0x9b12a8`(0x2abB)、`CVecCtrl::CollisionDetectWalk@0x9b3fd1`、`CtrlUpdateActiveMove@0x9bccaf`、`ChaseTargetImp@0x9bbd88`（均为本轮 IDA 现场 decompile，只读）；本端 `FootholdTree.cpp:123-145,317-351,175-204`、`Mob.cpp:174-175,320-327,393-408`、README:279,306-309,320。

---

## 补证 3 — 商城退出后黑夜变白天（服务端发包序列）

### 结论（翻案：服务端补发，原「坐实本端缺陷」不成立）
- 服务端退出商城 = 换频道重连：`ChangeMapHandler.enterFromCashShop()`（:188-208）只发 `getChannelChange`（:204），方法内无天气包。
- 重连后走完整登录入场：`PlayerLoggedinHandler:271` 发 SET_FIELD（getCharInfo）、`:296 addPlayer` → `MapleMap.java:2643` 无条件 `WeatherPackets.sendTo(chr)` 发 0x373D（snap=true）；`WeatherPackets.java:88-93/59-85`。
- 发包时序：SET_FIELD 先、0x373D 后；客户端 SET_FIELD 的 `Stage::clear()`（`SetFieldHandlers.cpp:251` → `Stage.cpp:155`）在前、0x373D 的 apply（`CommonHandlers.cpp:302`）在后 → **正常时序下 apply 不会被 clear 覆盖**。
- 另外 `WeatherTask`/`WeatherService` 每 60s 全服广播兜底（`WeatherTask.java:43-44`，`WeatherService.java:160`）。

### 对客户端 Q1 的定性
「服务端不发 0x373D」证伪；「本端缺陷坐实」不成立。更可能的真实缺陷面（**需运行态确认**）：
- (a) 时序/异步覆盖：`exit_cashshop()` 的本地 `stage.load`（`UICashShop.cpp:1546`）与 SET_FIELD 的 `Stage::clear()` 都可能把天气清成 720 正午；若 0x373D 在 SET_FIELD 前到达，或 clear 经 fade procedure 延迟到 0x373D 之后执行，就会被覆盖成「白天」。
- (b) 0x373D 路由/门禁异常：静态源码未见，需抓运行时包确认。

### 仍不确定
无运行时抓包日志证实 0x373D 到达/顺序（服务端 logs 只有 `[Weather] timing` 启动日志；客户端 `setfield_trace_AB.txt` 无 0x373D 行）。**下一步**：退出商城时抓包/临时日志，确认 0x373D 是否到达、到达相对 SET_FIELD clear 的先后。

### 证据
服务端 `E:\project\BeiDou-Server_s9\gms-server`：`ChangeMapHandler.java:73-77,188-208`、`MapleMap.java:2640-2643`、`WeatherPackets.java:59-93`、`PlayerLoggedinHandler.java:271,296`、`PacketCreator.java:1091-1092`、`WeatherTask.java:43-44`、`SendOpcode.java:363-366`。客户端：`PlayerHandlers.cpp:67-78`、`UICashShop.cpp:1528-1552`、`CashShopHandlers.cpp:827-836`、`Stage.cpp:155`、`SetFieldHandlers.cpp:251`、`WeatherState.cpp:85-88` / `WeatherState.h:59-60`、`CommonHandlers.cpp:259-309`。旧 RCA `docs/features/cashshop-9fix-2026-11/notes/exit-black-screen-rca.md:17` 结论核对属实。

---

## 总账：三处补证的定性变化
| 原「资料不足」项 | 补证后 |
|---|---|
| 命中特效原生 UOL | 原生无视觉 hit 播放器（观察）；本端双路径语义一致，但原生 hit 视觉播放未确证 |
| 怪物只在出生平台巡逻？ | 原生有边缘翻向+出生矩形、无主动下跳证据；本端 `get_edge` ≥3 段返地图墙是自造缺陷；「只在出生平台」不能断言 |
| 商城退出天气 | 服务端确实补发 0x373D；缺陷改判为「客户端时序/本地 stage.load 覆盖」候选，需抓包确认 |
