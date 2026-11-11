# round89 · W/L/E 三桶总清单 + 一次性 P1 Plan

> 触发：m09729「字段级 body 改造过的字段、类型不影响，但是一样字段的原始逻辑可以对齐一下……保证基础功能一致，扩展功能可以不对齐；」+ m09737「其余功能也要按照这个方向进行开发、修复、完善；」
> 权威源：Angel(13339) 083/v83 带符号 IDB；s9 服务端源码仅约束 W 桶字段宽度；本端 HeavenClient 自研重实现。

## 0. 口径（对 round88 §0/§2.2/§5 的收窄，历史证据链保留）

- **W**（6 个改造字段宽度/类型）→ 保持 s9，零改动。改回 083 原生会平移包中间字段、复现 `LoginParser.cpp:150-174` 白屏。
- **L**（同字段原始逻辑/状态机/分支，不碰 wire）→ 按 Angel 083 最完全反编译还原，保证基础功能一致。
- **E**（s9 私有扩展功能）→ 可不对齐。
- 红线：083/v83 only 禁 095；数值重合≠语义重合；门禁 5 禁想象修改；数值重合≠语义重合。

## 1. W 桶 — 保持 s9 宽度/类型（6 字段 + 27 条 must_keep）

| # | 行 | s9 源码 | 083 差异 | 原因 |
| --- | --- | --- | --- | --- |
| 1 | 192 | `p.writeShort(chr.getLevel());` | 083 原生 level 1B；s9 999 级上限改 2B | s9 改造，保持 |
| 2 | 201 | `p.writeInt(chr.getHp());` | 083 原生 short；s9 99999 上限改 int | s9 改造，保持 |
| 3 | 211 | `p.writeLong(chr.getExp());` | 083 原生 exp 4B；s9 999 级累积经验改 8B | s9 改造，保持 |
| 4 | 213 | `p.writeInt((int) chr.getGachaExp());` | 083 无此字段；s9 扩展通道 | s9 改造，保持 |
| 5 | 217 | `p.writeInt(0);` | 083 无此尾部；s9 spawnpoint 后固定补 int(0) | s9 改造，保持 |
| 6 | 2624 | `public static Packet removeItemFromMap(int objId, int animation, int chrId)` | animation>=2 恒写 4B cid；083 OnPacket v2==4 读 Decode2 delay，数值重合≠语义重合 | s9 改造，保持 |

**碰 wire 的 27 条 must_keep（round88 evidence 直接引用，全部归 W 轨）**：

- `login-flow/D1` K2 [BATCH_0_BASELINE] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `login-flow/D4` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `statusbar-hud/D2` K3 [KEEP_S9] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `corewindow/D3` K3 [KEEP_S9] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `corewindow/D4` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `social-economy/D4` K2 [BATCH_0_BASELINE] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `social-economy/D5` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `activity-minigame/D2` K4 [DOC_ONLY] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `activity-minigame/D5` K2 [BATCH_0_BASELINE] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `activity-minigame/D6` K2 [BATCH_0_BASELINE] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `activity-minigame/D10` K3 [KEEP_S9] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `cashshop/D1` K2 [BATCH_0_BASELINE] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `cashshop/D5` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `cashshop/D8` K4 [DOC_ONLY] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `skill/D1` K2 [BATCH_0_BASELINE] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `skill/D13` K2 [BATCH_0_BASELINE] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `npc/D1` K2 [BATCH_0_BASELINE] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `npc/D2` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `npc/D9` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `npc/D10` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `map/D3` K2 [BATCH_0_BASELINE] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `map/D5` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `map/D14` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `quest/D1` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `quest/D3` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `drop/D1` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `drop/D5` K3 [P3_NO] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束

## 2. L 桶 — 按 Angel 083 还原原始逻辑（reopen 27 + 本轮 1 + drop 2）

### 2.1 本轮唯一落地补丁（零 wire）

- `IO/UITypes/UIQuestLog.cpp::change_tab`：083 `CUIQuestInfo::OnTabChanged @0x87fd4f` 首行 `*(this + 369) = -1;`（清选中）→ 当前端在 `tab = tabid;` 后补 `selected_id = -1;`，由 `rebuild_list()` 自动选第一条可见任务，对齐 `TryShowTopQuest` / `ToggleDetail(0)` 语义。零 wire / 零协议 / 零 WZ / 零新封包。
- `quest/D5` 的 `UIQuestLog::send_key` 空实现**不改**：083 全库 `*Quest*OnKey*` / `*Quest*SendKey*` 符号 total=0，为对齐而发明键盘逻辑违反门禁 5。

### 2.2 round86 一刀切 KEEP 的纠偏对象（reopen 27，逐条 P1 拍板）

| face/id | kind | 原 verdict | 类别 |
| --- | --- | --- | --- |
| `login-flow/D3` | K3 | `KEEP_S9` | PURE |
| `login-flow/D9` | K4 | `KEEP_CURRENT` | PURE |
| `login-flow/D12` | K4 | `DOC_ONLY` | MIXED |
| `login-flow/D13` | K2 | `BATCH_0_BASELINE` | MIXED |
| `statusbar-hud/D1` | K2 | `BATCH_1_KEEP` | PURE |
| `statusbar-hud/D12` | K2 | `BATCH_1_KEEP` | PURE |
| `corewindow/D2` | K2 | `BATCH_0_BASELINE` | MIXED |
| `corewindow/D8` | K3 | `DOC_ONLY` | PURE |
| `corewindow/D9` | K2 | `BATCH_1_KEEP` | PURE |
| `corewindow/D10` | K2 | `BATCH_0_BASELINE` | MIXED |
| `social-economy/D3` | K2 | `BATCH_1_KEEP` | PURE |
| `social-economy/D9` | K2 | `BATCH_1_KEEP` | PURE |
| `social-economy/D12` | K2 | `BATCH_1_KEEP` | PURE |
| `pet-summon/D2` | K2 | `BATCH_0_BASELINE` | MIXED |
| `activity-minigame/D1` | K4 | `KEEP_WITH_NOTE` | PURE |
| `cashshop/D2` | K2 | `BATCH_1_KEEP` | PURE |
| `skill/D4` | K2 | `BATCH_1_KEEP` | PURE |
| `skill/D9` | K2 | `BATCH_1_KEEP` | PURE |
| `skill/D11` | K2 | `BATCH_1_KEEP` | PURE |
| `skill/D12` | K2 | `BATCH_1_KEEP` | PURE |
| `map/D1` | K2 | `BATCH_0_BASELINE` | MIXED |
| `map/D4` | K2 | `BATCH_1_KEEP` | PURE |
| `map/D8` | K2 | `BATCH_1_KEEP` | MIXED |
| `quest/D5` | K3 | `KEEP_CURRENT` | PURE |
| `quest/D8` | K2 | `BATCH_1_KEEP` | PURE |
| `drop/D2` | K2 | `BATCH_1_KEEP` | PURE |
| `drop/D12` | K2 | `BATCH_1_KEEP` | PURE |

### 2.3 已有 083 活 IDA 证据、登记 P1（本轮不落补丁）

- `drop/D2`（animation=0 消亡动画）+ `drop/D12`（animation=4 延时爆炸帧）：`CDropPool::OnDropEnterField @0x505900` 反编译逐字已落 `tools/_ida_r89/`，分支含 `MakeMoneyAnimation` / `*(v115+18)=3`，按 L 桶下一批还原。
- `quest/D8`、`skill/D4`、`corewindow/D8` 等状态机分支同理，逐面 REA→IDA→IMG 后再落，不凭空写。

## 3. E 桶 — s9 私有扩展（不对齐，5 项逐条带证据）

| 扩展项 | 证据 | 理由 |
| --- | --- | --- |
| GachaExp 扩展通道 | `PacketCreator.java:213 p.writeInt((int) chr.getGachaExp());` | 083 无此字段，s9 私有扩展 |
| 999 级上限 | `PacketCreator.java:192 p.writeShort(chr.getLevel()); // 999 级` | 083 原生 1B level，s9 私有数值扩展 |
| 99999 HP/MP 上限 | `PacketCreator.java:198-205 p.writeInt(chr.getHp()); ×4` | 083 原生 short，s9 私有数值扩展 |
| c2s 107 抓包工具 | `docs/features/pending-live-tests/README.md P3 长尾台账` | s9 侧运营工具，客户端基础功能不含 |
| 095 自定义中文 UI（福利中心） | `IO/UIStateGame.cpp:532 + IO/UITypes/UISpecial.h:9（round86 T2 已判为本端扩展身份标注）` | 本端扩展入口，非 083 原生功能，机械换地址=二次造假 |

E 为保守枚举：未列扩展项默认仍归 L 轨，逐条拍板时可追加，不硬凑数字。

## 4. 168 条路由闭合

| 轨道 | 组成 | 数量 |
| --- | --- | --- |
| **W 轨** | `class=WIRE`（含 must_keep 27） | 38 |
| **L 轨** | `class=PURE` 76 + `MIXED` 14 + `OTHER` 40 | 130 |
| **合计** | 38 + 130 | **168** |

round88 四分类计数 `PURE76/WIRE38/MIXED14/OTHER40` 直接复用（稳定、已证据化），不做语义再分类造假；E 为 L 轨内豁免标记，不参与算术。未决 0。

## 5. 一次性 P1 Plan（本轮 + 后续同方向批次）

1. **本轮（已落地）**：W/L/E 口径修订 + 三桶总清单 + `UIQuestLog::change_tab` 1 行真实对齐补丁（083 反编译逐字为凭）；harness + GATE + 真重编三证 + 零署名原子提交。
2. **下一批（drop 面）**：`CDropPool::OnDropEnterField` 的 animation 分支按 083 还原，REA→IDA→orange-wz(128B)→.nx 只读取证 → 纯逻辑补丁 → harness → GATE → 真重编 → 零署名提交。
3. **逐面推进**：reopen 27 条按 12 面分批，每批先出 083 符号/反编译锚点，证据不足的登记 P2/P3 不发明；每批一片一原子 commit。
4. **W 桶全程零改动**：6 字段宽度/类型 + 27 条 wire KEEP 不碰，避免复现白屏。
5. **真机归口**：五屏截图 60 节/142 处、`InPacket::available()` 393 处 §27-A 逐面回归、D2 聊天规则，由用户认领后销案。

## 6. 限制

- 本轮唯一源码改动 = `UIQuestLog.cpp` 1 行 + 4 行注释；零 wire / 零协议 / 零 WZ / 零新封包。
- 工作树 17 个外部程序在途 WIP 原样保留，精确 add 隔离、零夹带。
- 三桶只对「处置取向」分类，不改写 round86 DECISIONS.md 的 168 条历史裁定。
- 架构级重写（如独立 CUIPartyHP 浮窗、CUIStatusBar 拆 10+2 类）仍须单点裁决，不在自动落地范围。
