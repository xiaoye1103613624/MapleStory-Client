# 用户待测清单（真机统一验收）

> 维护约定：测不了/未测的写这里，步骤列全；已闭合标 ✅ + 日期 + 证据路径。  
> 本轮（2026-11-05）按用户改口：**不做桌面点测**，仅维护步骤供用户统一测。

相关功能目录：[world-map-players](../world-map-players/) · [use-return-scroll](../use-return-scroll/) · [softkey-tails](../softkey-tails/) · [set-hpmpalert](../set-hpmpalert/) · [pick-all-char](../pick-all-char/) · [heal-over-time](../heal-over-time/) · [touch-monster-attack](../touch-monster-attack/) · [cancel-debuff](../cancel-debuff/) · [guild-bbs-ui](../guild-bbs-ui/) · [guild-boss-hud](../guild-boss-hud/)

---

## 汇总

| # | 项 | 状态 | opcode | 备注 |
| --- | --- | --- | --- | --- |
| 1 | WORLD_MAP_PLAYERS | ⏳ 待测 | c2s 277 / s2c 376 | 大地图选点玩家列表 |
| 2 | USE_RETURN_SCROLL | ⏳ 待测 | c2s 85 | 回城卷双击 |
| 3 | SoftKey：登录遮罩截图 | ⏳ 待测 | — | SoftKey 尾巴 |
| 4 | SoftKey：PersonalShop FM | ⏳ 待测 | 雇佣店创建 | 需自由市场 + 许可证 |
| 5 | SoftKey：Messenger 立绘 | ⏳ 待测 | s2c 313 mode1 | 本机 look + 脚点 0.70；代码已闭环 |
| 6 | SoftKey：Guild BBS 摘要 | ⏳ 待测 | BBS 155 | 公会窗摘要栏（兼容） |
| 7 | SET_HPMPALERT | ⏳ 待测 | c2s 4096 / s2c 扩展 | Wave1 已接线；端到端需服务端配置 |
| 8 | PICK_ALL_CHAR | ⏳ 待测 | c2s 14 / 32 | VAC 选角（本轮修正误发 19） |
| 9 | HEAL_OVER_TIME | ⏳ 待测 | c2s 89 | 自然回复上报 |
| 10 | TOUCH_MONSTER_ATTACK | ⏳ 待测 | c2s 47 | 身体压力/能量满触怪 |
| 11 | CANCEL_DEBUFF | ⏳ 待测 | c2s 99 | s9 handler 空实现；仅协议对齐 |
| 12 | GuildBBS 完整窗 | ⏳ 待测 | c2s 155 / s2c 59 | `UIGuildBBS` 列表/读帖/发帖 |
| 13 | 公会 Boss HUD+物件 | ⏳ 待测 | s2c 287 / 288 | `UIGuildBoss` + `MapGuildBoss` |
| 14 | CharInfo SoftKey 便签 | ⏳ 待测 | c2s 131 | `N` → NoteSend；需商城便签额度 |

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
- **前置**：已登录进图；`UIWindow.img/Messenger` 资源可用；建议再开第二角色测对方立绘。
- **操作**：
  1. 快捷键开 Messenger（状态栏社交 / `V` 等入口，以当前键位为准）→ 应自动建房。
  2. **本机**：0 号 NameBar（灰，左上）上方应立即出现本机 STAND1 立绘（scale≈0.70，脚点在名牌中线上沿）。
  3. SoftKey / 空槽点邀请第二角色 → 对方接受后，对应色 NameBar 上方出现对方立绘 + `Ch.N` 标签。
  4. 发一句聊天 → 下方白框有字；本机立绘不消失。
  5.（可选）抓包：建房后应收到 s2c `MESSENGER=313` **mode=1** `joinMessenger(pos=0)`；对方进房有 mode=0 `addMessengerPlayer`（含 look）。
  6. 截图存 `docs/features/softkey-tails/evidence/messenger-look/`（若脚点悬空/裁切，记偏移再微调 `LOOK_FOOT_*`）。
- **期望**：本机与对方立绘均可见；不悬空、不裁进标题条；关窗再开可重建。
- **道具/地图/opcode**：任意城镇；c2s `MESSENGER=122`；s2c `313`。
- **功能文档**：[softkey-tails](../softkey-tails/) · [notes/messenger-look-2026-11-05.md](../softkey-tails/notes/messenger-look-2026-11-05.md)

### 6. Guild BBS 摘要
- **前置**：已加入公会。
- **操作**：开公会窗 → 看 `[BBS]` 摘要栏（打开完整 BBS 窗后回包也会同步摘要）。
- **期望**：列表/帖摘要与公告分栏。
- **opcode**：c2s `BBS_OPERATION=155` / s2c `GUILD_BBS_PACKET=59`。

### 12. GuildBBS 完整窗（GuildBBS.img）
- **前置**：角色已加入公会；服务端 `bbs_threads` 可为空。
- **操作**：
  1. 打开公会窗 → 按 `B` → 应弹出 `GuildBBS.img` 大窗（734×526）并自动发 List(page=0)。
  2. 空库：列表为空、无崩；有帖：可见标题行与回复数；有公告（localid=0）时顶栏黄字 `[公告]`。
  3. 单击选中行 → 双击或 `O` → 进读帖页（subject/body/replies）。
  4. `W` 或 BtWrite → SoftKey 标题 → SoftKey 正文 → 发帖（mode 0）；`N` 发公告帖；`1/2/3` 选 icon。
  5. `E` / BtRetouch → 编辑选中帖；`D` 删帖；读帖页 `P` 回复、BtReplyDelete 删回复。
  6. `,` / `.` 或翻页钮切页；Esc 从写/读退回列表，再 Esc 关窗。
  7.（可选）抓包确认 List=`mode2`、Open=`mode3`、Post=`mode0`、Reply=`mode4`。
- **期望**：窗体贴图来自 `Data/UI/GuildBBS.img`；协议字段与 s9 `GuildPackets` 一致；公会窗摘要栏同步更新。
- **道具/地图/opcode**：任意城镇；c2s `155`；s2c `59` mode `0x06`/`0x07`。
- **功能文档**：[guild-bbs-ui](../guild-bbs-ui/)

### 13. 公会 Boss HUD + 地图物件（Healer / Pulley）
- **前置**：
  1. 进 **`990000900`**（莎莲娜王室，地图根有 `healer`/`pulley`）；或任意图仅测 HUD。
  2. s9 当前脚本**不发** `GuildBoss_*`（仅 `GuildPackets` 工厂）→ 需 GM/临时脚本 `broadcastPacket(GuildPackets.GuildBoss_HealerMove(y))` / `PulleyStateChange(state)`，或改 PQ 事件补发。
- **操作**：
  1. 进 `990000900` → 日志应有 `[GuildBoss] map healer path=Map/Obj/guild.img/...` 与 pulley；屏幕可见治疗精灵（约 x=295,y=-564）与滑轮（约 x=-59,y=-394）。
  2. 发 HealerMove(y)（建议 y 在 yMin=-564 .. yMax=-350）→ 精灵 Y 插值移动；顶栏 HUD 显示 `治疗精灵 Y=…`。
  3. 发 PulleyStateChange(state) → 日志 `pulley state=`；HUD 刷新；精灵帧仍循环（原生 state 只门控交互）。
  4. 12s 无新包 HUD 自动隐藏；Esc 立即关 HUD（物件仍在）。
  5. 确认 **不再** 往 QuestAlarm 灌 tip。
  6. 换到无 healer 节点的图再发包 → HUD 仍更新；物件侧 debug「no map object」。
- **期望**：Y/state 与包体一致；`990000900` 上精灵可见且随 287 移动；无崩溃。
- **opcode**：s2c `GUILD_BOSS_HEALER_MOVE=287` body=`short y`；`GUILD_BOSS_PULLEY_STATE_CHANGE=288` body=`byte state`。
- **功能文档**：[guild-boss-hud](../guild-boss-hud/)
- **预留**：pulley 交互上行（原生 `sub_558B45`）未接——无服务端消费证据。

## 14. CharInfo SoftKey 便签（NOTE_ACTION）

- **前置**：打开角色信息窗（点其他角色 / 自己）；s9 `NoteActionHandler` **仅当** `CashShop.getAvailableNotes()>0` 才落库（商城赠送便签额度）。
- **操作**：
  1. CharInfo 聚焦 → 按 `N`。
  2. 已有目标名：SoftKey 输入正文 → 发 `NOTE_ACTION=131` action0。
  3. 无目标名：SoftKey 先收件人名，再正文。
  4.（可选）`UIClaim` SoftKey 同包对照；抓包 body=`byte0 + mapleStr name + mapleStr msg`。
- **期望**：有额度时对方可在 Memo 收到；无额度时客户端仍发包、服务端静默跳过（不崩）。
- **opcode**：c2s `NOTE_ACTION=131`。
- **备注**：与 `UIClaim` 同走 `NoteSendPacket`；本项补的是 CharInfo 快捷键路径。

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
