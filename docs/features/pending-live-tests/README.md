# 用户待测清单（真机统一验收）

> 维护约定：测不了/未测的写这里，步骤列全；已闭合标 ✅ + 日期 + 证据路径。  
> 本轮（2026-11-05）按用户改口：**不做桌面点测**，仅维护步骤供用户统一测。

相关功能目录：[world-map-players](../world-map-players/) · [use-return-scroll](../use-return-scroll/) · [softkey-tails](../softkey-tails/) · [set-hpmpalert](../set-hpmpalert/) · [pick-all-char](../pick-all-char/) · [heal-over-time](../heal-over-time/) · [touch-monster-attack](../touch-monster-attack/) · [cancel-debuff](../cancel-debuff/)

---

## 汇总

| # | 项 | 状态 | opcode | 备注 |
| --- | --- | --- | --- | --- |
| 1 | WORLD_MAP_PLAYERS | ⏳ 待测 | c2s 277 / s2c 376 | 大地图选点玩家列表 |
| 2 | USE_RETURN_SCROLL | ⏳ 待测 | c2s 85 | 回城卷双击 |
| 3 | SoftKey：登录遮罩截图 | ⏳ 待测 | — | SoftKey 尾巴 |
| 4 | SoftKey：PersonalShop FM | ⏳ 待测 | 雇佣店创建 | 需自由市场 + 许可证 |
| 5 | SoftKey：Messenger 立绘 | ⏳ 待测 | — | NameBar 脚点 |
| 6 | SoftKey：Guild BBS 摘要 | ⏳ 待测 | BBS 155 | SoftKey `B` |
| 7 | SET_HPMPALERT | ⏳ 待测 | c2s 4096 / s2c 扩展 | Wave1 已接线；端到端需服务端配置 |
| 8 | PICK_ALL_CHAR | ⏳ 待测 | c2s 14 / 32 | VAC 选角（本轮修正误发 19） |
| 9 | HEAL_OVER_TIME | ⏳ 待测 | c2s 89 | 自然回复上报 |
| 10 | TOUCH_MONSTER_ATTACK | ⏳ 待测 | c2s 47 | 身体压力/能量满触怪 |
| 11 | CANCEL_DEBUFF | ⏳ 待测 | c2s 99 | s9 handler 空实现；仅协议对齐 |

---

## 1. WORLD_MAP_PLAYERS（大地图玩家列表）

- **前置**：客户端已登录进图；同图或其它频道有 ≥1 在线角色；服务端 `feature/integration_client`。
- **操作**：
  1. 打开世界地图（默认 `W` / 小地图入口）。
  2. 点选一个有人的地图节点。
  3. 观察右上「在线: N」列表（名/等级/频道）。
  4. 切换到另一节点 → 旧列表应清空。
  5. SoftKey `L` 手动刷新当前点。
- **期望**：选中 mapId 与回包一致；陈旧回包丢弃；人数 ≤40。
- **道具/地图/opcode**：任意城镇即可；c2s `WORLD_MAP_PLAYERS=277` body=`int mapId`；s2c `376`。

## 2. USE_RETURN_SCROLL（回城卷）

- **前置**：背包消耗栏有 `2030000..2030099` 回城/传送卷（不要用 `2030100` 防驱逐卷）。
- **操作**：
  1. 打开背包 → 双击回城卷。
  2. （可选）服务端 `!showpackets` 确认收包头 `0x55`。
- **期望**：扣 1 个道具并传送回城；opcode **85** 而非 72。
- **道具/地图/opcode**：道具 `2030xxx`；任意野外图；c2s `USE_RETURN_SCROLL=85`（body 同 USE_ITEM：ts/slot/itemId）。

## 3–6. SoftKey 尾巴

### 3. 登录遮罩截图
- **前置**：停在登录/选角可截屏场景；截图键默认 SCROLL_LOCK。
- **操作**：按截图键 → 查 `Screenshots/`（或系统设置指定目录）是否生成 PNG。
- **期望**：有时间戳文件名；成功日志一条。

### 4. PersonalShop FM
- **前置**：角色在自由市场地图；持有雇佣商人许可证（514xxxx / 503xxxx）。
- **操作**：SoftKey 输入店名 → 开店。
- **期望**：发出 `HiredMerchantCreatePacket`；店面出现。

### 5. Messenger 立绘
- **前置**：可开 Messenger 窗。
- **操作**：开 Messenger → 看 NameBar 立绘脚点与缩放（约 0.70）。
- **期望**：立绘不悬空、不裁切；可截图对照再微调。

### 6. Guild BBS 摘要
- **前置**：已加入公会。
- **操作**：SoftKey `B` → 开公会窗看 `[BBS]` 摘要栏。
- **期望**：列表/帖摘要与公告分栏；完整 GuildBBS.img 多页签仍属后续 UI 票。
- **opcode**：c2s `BBS_OPERATION=155`。

## 7. SET_HPMPALERT

- **前置**：进游戏；系统设置可改 HP/MP 报警滑条。端到端回灌需 s9 `use_server_auto_pot=true`（**碰库，需用户授权**）。
- **操作**：
  1. 打开系统设置 → 调 HP/MP 报警百分比 → 确定。
  2. 查 `boot_perf.log` / debug：`[PERF] HPA out hp=… mp=…`。
  3.（可选开回灌）重登 → 期望 `[PERF] HPA hp=…`。
- **期望**：上行 step 0..19；本地闪铃仍按落盘百分比。
- **opcode**：c2s `SET_HPMPALERT=4096` body=`byte hp_step, byte mp_step`。

## 8. PICK_ALL_CHAR（VAC 选角）

- **前置**：登录后进入「查看全部角色」。
- **操作**：
  1. 点选角色 → BtEnter。
  2. 若该世界要 PIC → 输入 PIC 后确认。
- **期望**：进频道选角成功；抓包为 **14**（无 PIC）或 **32**（有 PIC），**不是** 19。
- **opcode**：`PICK_ALL_CHAR=14` body=`int cid, int world, macs, hwid`；`VIEW_ALL_WITH_PIC=32` body=`pic, cid, world, macs, hwid`。

## 9. HEAL_OVER_TIME

- **前置**：进图且存活；建议桑拿/高 recovery 图（`info/recovery>1`）更易观察。
- **操作**：站立等待 ≥10s → 查日志 `[Stage] HEAL_OVER_TIME` / `[PERF] HOT`；HP/MP 应上涨（未满时）。
- **期望**：healHP ≤ `77*recovery*1.5`；healMP∈(0,1000)。
- **opcode**：c2s `HEAL_OVER_TIME=89` body=`skip(8)+short hp+short mp`。

## 10. TOUCH_MONSTER_ATTACK

- **前置**：角色有 BODY_PRESSURE buff，**或** ENERGY_CHARGE≥15000；身边有怪。
- **操作**：贴近怪物碰撞 → 期望怪掉血；日志 `[Stage] TOUCH_MONSTER_ATTACK`。
- **期望**：服务端仅在上述条件结算；无条件发了也会被丢。
- **opcode**：c2s `TOUCH_MONSTER_ATTACK=47`（body 同 CLOSE_ATTACK / parseDamage）。

## 11. CANCEL_DEBUFF

- **前置**：身上有疾病（封印/虚弱/毒/黑暗/晕眩等），且客户端拿到了带 duration 的 GIVE_BUFF。
- **操作**：等待本地 duration 归零。
- **期望**：发空包 opcode 99；s9 **当前不处理**（空 handler），仅确认客户端有发送/无崩溃。真解除仍走药剂 USE_ITEM(72)。
- **opcode**：c2s `CANCEL_DEBUFF=99`（空 body）。
