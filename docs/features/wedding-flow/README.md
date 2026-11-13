# 结婚 / 婚礼流程闭环（wedding-flow）

> 日期：2026-11-06 ｜ 双端对照（客户端 C++ × s9 Java 源码）｜ 承接「家族/组队/远征/结婚/学院 社交闭环」盘点

## 0. 背景与裁决变更

`parity-gap-2026-11` §2 曾把婚礼判为「🟡 壳 · 裁决=不做玩法」，`s9-protocol-align/notes/gap-inventory.md`
把 `0x89 RING_ACTION` / `0x8A WEDDING_ACTION` 标为「明确不做 / 禁发」。

**本轮复核推翻了该裁决的前提**：先前判断建立在「服务端婚礼不完整」的假设上，逐文件读 s9 源码后证实：

- s9 婚礼后端**完整存在且已注册**（`PacketProcessor.java:250/308/309/310`）；
- Amoria（婚礼镇）NPC 脚本齐备（`scripts/npc/9201008~9201014.js`），预约、发请柬、仪式、
  戒指发放、礼物注册台全流程服务端驱动；
- 事件脚本 `scripts/event/WeddingChapel.js` / `WeddingCathedral.js` 在。

⇒ 缺口 100% 在客户端：**从不发这几个包**。故本切片把客户端做到「能求婚、能订婚、
能收请柬、能进礼台、能解约」；仪式本体仍由服务端 NPC 脚本驱动（客户端无需新增逻辑，
只需正确消费已有的 `WEDDING_PROGRESS` / `MARRIAGE_RESULT` 等 s2c 包）。

> 门禁 7「不发明客户端能力」在这一项上从此**满足**：能力由 s9 源码证明存在。

## 1. 协议事实源（s9 侧逐行证据）

### 1.1 c2s

| opcode | s9 handler | 读包序（源码行） | 本端构造 |
| --- | --- | --- | --- |
| `RING_ACTION = 137` (0x89) | `RingActionHandler` | `byte mode` 分发 | `WeddingPackets.h` |
| ├ mode 0 求婚 | :324 | `readString(name)` + `readInt(boxItemId)` | `WeddingProposalPacket` |
| ├ mode 1 撤回 | :328-330 | 无后续字段 | `WeddingCancelProposalPacket` |
| ├ mode 2 答复 | :334-336 | `readByte(>0=接受)` + `readString(source名)` + `readInt(source cid)` | `WeddingAnswerPacket` |
| ├ mode 3 解约 | :389 | `readInt(wItemId)` | `WeddingBreakPacket` |
| ├ mode 5 请柬寄出 | :393-395 | `readString(guest)` + `readInt(marriageId)` + `readByte(invitationSlot)` | `WeddingInvitePacket` |
| ├ mode 6 请柬打开 | :468-469 | `readInt(slot)` + `readInt(invitationid)` 均 4 字节 | `WeddingOpenInvitationPacket` |
| └ mode 9 心愿单 | :500-508 | `readShort(amount≤10)` + amount × `readString()` | `WeddingWishlistPacket` |
| `WEDDING_ACTION = 138` (0x8A) | `WeddingHandler` | :42 `byte mode` | 同上 |
| ├ mode 6 送礼 | :45-47 | `readShort(slot)` + `readInt(itemid)` + `readShort(qty)` | `WeddingGiftSendPacket` |
| ├ mode 7 取礼 | :110-111 | `readByte(invType 未消费)` + `readByte(itemPos)` | `WeddingGiftTakePacket` |
| └ mode 8 退出礼台 | :154 | 无 | `WeddingRegistryExitPacket` |
| `WEDDING_TALK = 139` (0x8B) | `WeddingTalkHandler` | :37 `byte action`（==1 且非新人 → progress2；否则 ceremony end） | `WeddingTalkPacket` |

**关键：`WEDDING_TALK_MORE` 在 s9 也是 0x8B、同 handler。客户端不重复建枚举**
（否则被 opcode 门禁判为「声明未构造」死项，`gen_opcode_matrix.py` 实测拦截过）。

### 1.2 s2c（既有 handler 的字段修正）

| s2c | s9 producer | 修正点 |
| --- | --- | --- |
| `MARRIAGE_REQUEST(72)` mode 0 | `onMarriageRequest` | 原实现只 tip，改为 **UIYesNo** → 发 mode2 |
| `MARRIAGE_REQUEST(72)` mode 9 | `sendWishList` | 包体**为空**；原实现按 int 流计数 → 改为 SoftKey 录 ≤10 条回 mode9 |
| `MARRIAGE_RESULT(73)` msg 11 | `OnMarriageResult(int,Character,bool)` | 与登录 `addRingInfo` marriage 块**逐字段同构** → 走 `parse_marriage_block` 写本地 |
| `MARRIAGE_RESULT(73)` msg 15 | `sendWeddingInvitation` | 尾部 `short`（s9 恒写 1）；Cathedral 判据 = `!=2`（0/1 都是 Cathedral，2=Chapel/Vegas） |
| `WEDDING_GIFT_RESULT(74)` | `onWeddingGiftResult` | **原实现把 int 当 mode 后第一字段，完全错位**。按源码重写各 mode 分支（0x09 names / 0x0A 0x0F long32+items / 0x0B names+long32+items / 0x0C 0x0E 无体）。item 体按 `addItemInfo` 的 itemType 三分支消费 |
| `NOTIFY_MARRIED_PARTNER_MAP_TRANSFER(75)` | `OnNotifyWeddingPartnerTransfer` | R28-B2 同类：原为一条信息**开窗** → 只更新 `MarriageInfo.partner_mapid`，`(0,0)` = 解绑 |

### 1.3 道具号（s9 `ItemId.java:229-284`）

| 用途 | itemid | 栏位 |
| --- | --- | --- |
| 订婚戒指盒（求婚消耗） | `2240000..2240015` | USE |
| 订婚凭据（空盒 + 对戒 8 项） | `4031357..4031364` | ETC |
| 请柬（待寄出） | `4031377`(Chapel) `4031395`(Cathedral) | ETC |
| 请柬（已收到） | `4031406`(Chapel) `4031407`(Cathedral) | ETC |
| 结婚戒指 | `1112803/1112806/1112807/1112809`（`id/10==111280`） | EQUIP |
| 婚礼门票 | `5251000..5251003` | CASH |

求婚盒在 USE 栏（服务端校验 `haveItem(itemid)`），订婚凭据与请柬落 ETC；本端
`WeddingItem::*` 判定函数严格按 `InventoryType` 分栏，故解约入口只在 ETC 页签挂
（`4031357..64`），婚礼戒指解约走婚礼窗 B 键扫 EQUIP/EQUIPPED，**不在 ETC 页签**。

## 2. 客户端改动清单

### 新增
- `Character/MarriageInfo.h` — 订婚/结婚数据层（含 `partner_mapid`）。
- `Net/Packets/WeddingPackets.h` — 137/138/139 全部构造 + `WeddingItem::*` 判定。

### 出站/注册
- `Net/OutPacket.h` — `RING_ACTION=137` `WEDDING_ACTION=138` `WEDDING_TALK=139`（含 s9 出处注释）。

### 登录解析（此前只读不存 → 请柬/送礼拿不到 marriageId）
- `Net/Handlers/Helpers/CharacterParser.{h,cpp}` — `parse_ring3(recv, MarriageInfo&)`
  + 抽共用 `parse_marriage_block`。
- `Net/Handlers/SetFieldHandlers.cpp`、`Helpers/CashShopParser.cpp` — 调用点补 player 引用。
- `Character/Player.{h,cpp}` — `get_marriage()` 存取。

### UI 接线
- `IO/UITypes/UIWedding.{h,cpp}` — 开窗不再自动弹软键盘；
  `I/W`请客（校验关系+请柬，缺则只给 tip）· `T`取礼 · `S`送礼 · `X`退出礼台 ·
  `C`撤回求婚 · `B`解约（先扫背包确认真有凭据/戒指才发）· `R`婚礼祝福 ·
  `M/N/G` 照旧。
- `IO/UITypes/UIItemInventory.cpp` — USE 页签双击订婚戒指盒 → 求婚；
  ETC 页签双击请柬 → 寄客/开请柬；双击订婚凭据 → 解约（UIYesNo 二次确认）。

### 入站
- `Net/Handlers/WaveAShellHandlers.cpp` — 见 §1.2；`WeddingPhotoHandler` 保持
  `s9 需要外部 WvsMapGen，服务端注释「Will no longer continue Wedding Photos」`
  的既有降级（仅 tip，不假造合影）。

### i18n
- `locale/zh-CN.ini` / `en-US.ini` — 新增 24 个 `wedding.*` 键。
  ⚠ 值暂为英文（遵 R27-B1c 用户决策 B「保持英文原值，只建键」），等有 v83
  String.img / IDA 依据再换中文；婚礼请柬文案此前是「硬编码 Whisper」，
  现已改为真实道具流程，`r27-b1c-i18n-scan/pending-copy.md` 的待决项随之消解。

## 3. 验收状态

| 层 | 判据 | 结果 |
| --- | --- | --- |
| 协议门禁 | `gen_opcode_matrix.py --check` | ✅ GATE OK（s9 3d5ccc6f8e），`client enums never constructed: 0`，collision 如实标注 `139:{TALK,TALK_MORE}` |
| 编译 | MSBuild Release\|x64 全 TU | ✅ **0 编译错误** |
| 链接 | 写 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` | ⚠ `LNK1104` — 客户端进程 PID 34476 / 21908 正在运行占用 |
| 真机流程 | 求婚→接受→预约→请柬→仪式→戒指→礼台 | ⏳ 待链接产物可覆盖后按 §4 点测 |

链接阻塞属「插件同步与进程占用」，本会话**首次遇到**，按门禁 14 需用户抉择处理
方式（A 自行关闭 / B 允许结束进程后覆盖 / C 中止保留构建产物）。

## 4. 真机冒烟清单（待执行）

1. 双号（A 男 / B 女，同图，均 ≥50 级，A 持 2240000）：A 双击戒指盒 → 输入 B 名 → B 收 YesNo → 接受 → 双方 `MARRIAGE_RESULT(11)`，本地 MarriageInfo `status=1`、`relationship_id>0`。
2. 拒绝路径：B 点 No → A 收 `msg 0`，A `marriageItemId` 清。
3. A 持 4031377 + 已订婚 → 双击 → 输入 C 名 → C 收 `4031406`（或 Duey 信）+ `MARRIAGE_RESULT(15)` 显示新人。
4. C 双击 4031406 → `RING_ACTION mode6` → 新人信息窗。
5. NPC 9201008 预约 → 9201010 录心愿单（SoftKey `a,b,c`）→ `mode9` → 9201014 礼台：B 送礼 / 新人取礼 / X 退出。
6. 解约：双击 `4031357..64` 或婚礼窗 B → `mode3` → `OnNotifyWeddingPartnerTransfer(0,0)` → 本地 MarriageInfo 清空。
7. R28 回归：冷启动登录链抓帧，`UIWedding` / `UIFamily` / `UIGuild` / `UIAlliance` **不得**出现在任何未主动打开的帧里。

## 5. 显式预留（不在本切片）

- **合影**：s9 `onTakePhoto` 需外部 WvsMapGen 服务（源码自陈放弃）→ 仅 tip。
- **婚礼服装/门票获取**：走 CashShop / NPC 商店既有链路，本切片不新增商城逻辑。
- **Amoria 地图物件与婚礼天气**（`MapHelper/weather/wedding/*`）：资源面未盘点，
  如仪式地图渲染异常另立资源切片。
- **学院 / 远征**：v95+ 系统（095 移植线证据），非 GMS083 原版能力 → 按 083 基准判「不做」，
  见 [social-loop-audit-2026-11 §五](../social-loop-audit-2026-11/)。

## 6. 对照与资源基准（用户裁决 2026-11-06）

> **功能参照 = GMS083 原版**：083 参照端 = `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe`
> （IDB 已加载，r12 起 REA/IDA 一直用它）；UIWindow.img 顶层 112 节点清单
> （`v83-hud-quest/evidence`）即原版功能面权威表 —— `Wedding/Family/FamilyTree/UserList`
> 均在表内，本切片所有窗型、道具、流程以该表与 s9(v83 派生) 源码为准。
>
> **禁止拿 095 当参照**：`port-095`/浩浩岛095/PLUS2 是独立在研移植线；本轮证据链中凡出现
> v95 反编译符号（`ExpeditionIntermediary`、v95 opcode 表）仅作旁注，不构成需求或依据。
>
> **找资源同基准**：需要补道具图标/婚礼时装/门票贴图时，用 083 适配来源
> （`E:\MXD\适用于083时装`、`E:\MXD\动漫皮肤083.zip`、GMS113 原版 Data）或小册子按
> 083 版本号筛选；095/192 系资源不入库。本轮未新增资源（请柬 40313xx 图标沿用原生
> Etc.img，已在包内）。

