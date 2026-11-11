# round93 · 095/v95 依据链纠偏（5 处纯注释）

触发：用户 m11680「是否还有未完成、需要拍板、未自测的事项了？」→ 盘点后按 m09211
「全部继续，如果能自测的可以自己启动测试」自测推进 reopen 清单中唯一「该做且能自测」的一项
（`activity-minigame/D1` KEEP_WITH_NOTE），并在同族扫描中扩出另外 4 处。

## 1. 为什么是这 5 处

reopen 27 条里唯一明确「做：只换注释」的只有 `activity-minigame/D1`。沿同一模式
（注释里写「095/v95」当依据、或引 083 里不存在的符号名）全仓扫描，命中 5 处：

| 文件:行 | 旧依据语 |
| --- | --- |
| `IO/UIStateGame.cpp:533` | `// 095 自定义中文 UI：暂时复用 EVENT 键打开福利中心…` |
| `IO/UITypes/UISpecial.h:9` | `// 095 自定义中文 UI 入口：福利中心（签到/兑换/排行/转职/传送）。` |
| `Gameplay/MapleMap/MapPortals.h:40` | `// 传送门触发范围（对齐 v95：CPortal::CheckKey 的按键通道 vs ProcessPortal 的碰触通道）：` |
| `Gameplay/MapleMap/MapPortals.cpp:146` | `// 原生语义（v95 CPortal::CheckKey / ProcessPortal）：` |
| `Gameplay/Stage.cpp:284` | `// 其余传送门：按上键主动交互（对齐 v95 CPortal::CheckKey）` |

## 2. 083 活 IDB 实证

`tools/_r93_ida*.py`（endpoint `http://127.0.0.1:13339/mcp`，notification fire-and-forget）→

- `*UISpecial*` / `*Fairy*` / `*Welfare*` / `*CheckIn*` / `*Attendance*` **全 0**；
  `*Special*` 仅 1 条 `?GetSpecialUOL@SKILLENTRY@@QBE?AVZtl_bstr_t@@J`（技能 UOL，非窗类）
  ⇒ 福利中心不是 083 原生窗族，是**本端扩展（E 桶）**。按 m09729「扩展功能可以不对齐」，保留且零行为改动。
- `*CPortal@@*` / `*CPortal::*` **全 0** —— 旧注释引的 `CPortal::CheckKey` / `ProcessPortal`
  在 083 里**根本不存在**。真锚点在 `CPortalList` 与 `CUserLocal`：
  - `CPortalList::FindPortal @0x712ab1`（按键通道）：
    `dword_BF040C(a1: v9, a2: v7[3]-a5, a3: v7[4]-50, a4: v7[3]+a5, a5: v7[4]+50)`
    —— 水平 ±range、垂直固定 ±50 band。
  - `CPortalList::FindPortal_Collision @0x712b61`（碰触通道）：
    `dword_BF040C(a1: v7, a2: v5[3]-v5[5]/2, a3: v5[4]-v5[6]/2, a4: v5[3]+v5[5]/2, a5: v5[4]+v5[6]/2)`
    —— 按 portal 自身 w/h 半宽半高框。
  - `CUserLocal::CheckPortal_Collision @0x94dac6`（size `0x35c`=860B，入口）：命中后按 `v7[2]` type 分派
    —— `9` → `COutPacket(100)` + `Encode1(field)` + `EncodeStr` + `Encode2(x)` + `Encode2(y)` 脚本门；
    `12` → `sub_94DE22`；其余 → `CField::SendTransferFieldRequest` 或 `TryRegisterTeleport`。

## 3. 为什么不改行为

本端 `MapPortals::find_warp_at(playerpos, scope)` 用 `PortalScope{TOUCH_ONLY, ALL}` 两通道
表达「碰触自动触发 / 上键主动触发」，与 083 的「碰撞框命中后按 type 分派」是**等价抽象**。
把 083 的 type 分派原样搬进来会改触发时机（例如 type 9 脚本门会被自动触发），属**行为级变更**，
本轮判「不搬」，登记为决策点（PLT 52-3）。

## 4. DECISIONS.md:130 的处理

该行写「做：只换注释依据引 Angel 083 `CPortal`」——它引的符号名同样不存在。
但 `DECISIONS.md` 是 round86 的历史裁定表，按既往口径**不改写历史行**，
仅在本 notes 与 `WLE-TRIAGE-PLAN.md` 第 10 节记明；本轮实际依据以第 2 节的真锚点为准。

## 5. 规则升级

承接 round91 第 8 节 / round92 第 9 节：**注释里引 083 符号前，必须先 `entity_query` 实证符号存在**。
符号名不存在（如 `CPortal::CheckKey`）写进注释即依据链污染 —— 本轮由 harness 静态守卫
`tools/test_round93_citation_fix.py` 逐条断言（5 文件零 `095`/`v95` 依据语、
`CPortal::CheckKey`/`ProcessPortal` 零残留、083 真锚点在场）。

算术不动：168 条 DECISIONS 裁定、`W 38 + L 130 = 168`、reopen `27 = 25 P1 + 1 W + 1 F-P2` 全部保持。
