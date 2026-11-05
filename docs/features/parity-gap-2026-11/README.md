# 功能面差距审计 · 客户端 × s9 服务端 × 原生参考端（2026-11-04）

> 现场重算（不采信旧台账结论）：扫描脚本 `tools/gap_scan_2026_11.py`、`tools/gap_c2s_refined.py`（idiom 修正前以 v2 输出为准）、`tools/_gap_scan_out.json`。
> 事实源：客户端 `Net/PacketSwitch.cpp`/`Net/OutPacket.h`/`IO/UITypes`；服务端 `E:\project\BeiDou-Server_s9\gms-server`（`RecvOpcode/SendOpcode/PacketProcessor`）；原生窗口面 `docs/features/v83-hud-quest/evidence/UIWindow.img.top-level.txt`（112 节点）。
> 口径：协议以 s9 为准（匹配 s9 = 匹配原生）；功能/UI 以原生 + ijl15 参考矩阵（`beidou-v17-parity/notes/01-parity-matrix-and-plan.md`）为准。

## 0. 总读数

| 维度 | 实测 | 结论 |
| --- | --- | --- |
| s2c 入站 | 客户端注册 **301** handler；s9 实际下发 **255** | 差集 **3** 个（见 §1.1），其余全覆盖 |
| c2s 出站 | 客户端枚举 **103** 全部有构造点；s9 生效 handler **173** | 差集 **71**（含 2 个假阳性，见 §1.2）；`LOGIN_EMAIL=3` 客户端独有、无害 |
| 原生 UI 窗口 | `UIWindow.img` 顶层 **112** 节点 | 有壳/有窗 **~107**；真缺 **~5**（见 §2） |
| 扩展功能矩阵（33+ 项） | P0.5 基础系统、W0–W13 版式、聊天、地图对象已闭环 | 纯客户端/双端扩展 **约 20 项未实现**（见 §3） |

---

## 1. 协议层（现场重算）

### 1.1 s2c：s9 会发、客户端没接（3 个，全部属公会副本）

| opcode | s9 名 | 场景 | 缺口性质 |
| --- | --- | --- | --- |
| 287 | `GUILD_BOSS_HEALER_MOVE` | 扎库/公会副本治疗精灵移动 | 公会副本玩法未做（原生有） |
| 288 | `GUILD_BOSS_PULLEY_STATE_CHANGE` | 公会副本滑轮状态 | 同上 |
| 376 | `WORLD_MAP_PLAYERS` | 世界地图玩家分布请求的回包 | 请求侧 278 已接（地图信息面板发），回包不接 → 面板只出掉落不出玩家 |

### 1.2 c2s：s9 有 handler、客户端从不发（71 − 2 假阳性）

先扣掉两个假阳性（扫描正则误报，人工复核）：
- `WORLD_MAP_DROP_REQUEST(278)` — 已接：`UIMapInfoPanel.cpp:231` 发 `WorldMapDropRequestPacket`（枚举表尾无逗号导致漏计）。
- `HIRED_MERCHANT_REQUEST(63)` — 原生 v83 的雇佣商人操作走 `PLAYER_INTERACTION(123)`，本端经 `UIMerchant/PlayerInteractionPackets` 覆盖；63 是遗留通道，不需要补。

其余按 `s9-protocol-align/notes/gap-inventory.md` 三分类 + 本次复核，**按场景细化**：

**A. 玩家可感知、应补（基础设施在，缺发送/接线）**

| 场景 | opcode（s9 名） | 现状细节 |
| --- | --- | --- |
| 背包点 Buff 图标右键解除 | 92 `CANCEL_BUFF` / 99 `CANCEL_DEBUFF` | ✅ CANCEL_BUFF 已接（Wave3）；CANCEL_DEBUFF 空包已接（s9 handler 故意空，真解除走药剂 72） |
| 道具效果取消（坐骑/状态道具） | 73 `CANCEL_ITEM_EFFECT` | 有效果系统无取消入口 |
| 能力值窗「自动分配」 | 88 `AUTO_DISTRIBUTE_AP` | `UIStatsInfo` 无该钮；原生有 |
| 技能书双击使用 | 82 `USE_SKILL_BOOK` | `UISkillBook` 是浏览窗，不消费技能书 |
| 传送卷/回城卷 | 85 `USE_RETURN_SCROLL` | 使用回城卷轴无发送路径 |
| 脚本道具（任务道具点击） | 78 `SCRIPTED_ITEM` | 任务脚本给的一次性道具点不出 |
| 捕捉道具（怪物捕捉球） | 81 `USE_CATCH_ITEM` | 无 UI 分支 |
| 召唤袋 | 75 `USE_SUMMON_BAG` | 无发送（召唤兽显示已接） |
| 死亡道具（复活币类） | 53 `USE_DEATHITEM` | 无 |
| HOT 持续治疗请求 | 89 `HEAL_OVER_TIME` | ✅ 2026-11-05 已接自然回复上报（见 heal-over-time） |
| 快捷栏拖拽换键保存 | 183 `CHANGE_QUICKSLOT` | 3×3 快捷格（hotkey-grid）已做，改键**不持久化到服务端** |
| HP/MP 报警阈值存档 | 4096 `SET_HPMPALERT` | ✅ Wave1 已上行+回灌（见 set-hpmpalert）；端到端回灌待 `use_server_auto_pot` |
| 地图内特殊交互 | 100 `CHANGE_MAP_SPECIAL` / 101 `USE_INNER_PORTAL` / 206 `TOUCHING_REACTOR` / 115 `USE_TREASUER_CHEST` / 112 `USE_ITEM_REWARD` | 隐藏传送点/触碰式反应器/宝箱开启不发送；反应堆渲染已修但交互半通 |
| 触怪伤害 | 47 `TOUCH_MONSTER_ATTACK` | ✅ 2026-11-05 身体压力/能量满触怪发 47（见 touch-monster-attack） |
| 断线重连 | 28 `RELOG` | 断线只能重走全登录 |
| 世界地图查看后选角 | 14 `PICK_ALL_CHAR` | ✅ 2026-11-05 修正误发 19→14/32（见 pick-all-char） |
| 远程扭蛋 | 116 `USE_REMOTE` | `UIRemoteGachapon` 有壳，抽一次发不出 |
| 引导点击 | 162 `CLICK_GUIDE` / 197 `NPC_ACTION` / 163 `ARAN_COMBO_COUNTER`(见 B) | 新手引导/监视器 NPC 动作不上报 |
| 组队搜索 | 220 `PARTY_SEARCH_REGISTER` | 已按 r-note 用 `PARTY_SEARCH_START` 代理（s9 0xDC 是空 stub）→ 视同已消化，留档 |

**B. 战斗/环境内部报文（多数由渲染侧顺带补发，优先级低但属真实差集）**

`56 MOB_BANISH_PLAYER`、`109 GRENADE_EFFECT`、`189 AUTO_AGGRO`、`191 FIELD_DAMAGE_MOB`、`192 MOB_DAMAGE_MOB_FRIENDLY`、`194 MOB_DAMAGE_MOB`、`193 MONSTER_BOMB`、`212 LEFT_KNOCKBACK` —— 竹林/道场/怪物互伤/驱逐事件类，客户端只渲染不回报，单机自玩影响小，联机事件图可能不同步。

> **2026-11-04 Wave 4 复核覆写**：本节 9 条逐条读 s9 handler 后，**仅 109/189 为真缺口且已接**；
> 163 属假缺口（本端战神连击早在 Combat.cpp 已发），56/191/193 依赖 Boss 脚本/障碍/道场内容未建，
> 192/194 属怪物 AI 互伤事件、212 属脚本请求击退、47 属 BODY_PRESSURE 技能数据缺失。**判词全表见
> [wave4-battle-packets/README.md](../wave4-battle-packets/README.md)**——未来会话接线一律以该文档为准，勿按本节原表动工。

**C. 宠物/召唤兽/龙 一整族（原生完整玩法面，本端仅显示层）**

| 场景 | opcode |
| --- | --- |
| 召唤宠物上线 | 98 `SPAWN_PET`（s2c 已接渲染，c2s 使用/召唤发不出） |
| 宠物移动/对话/指令/拾取/自动吃药/排除列表 | 167/168/169/170/171/172 |
| 宠物喂食/坐骑喂食 | 76 `PET_FOOD` / 77 `USE_MOUNT_FOOD` |
| 召唤兽移动/攻击/受伤上报/凝视体 | 175/176/177/178 `BEHOLDER` |
| 战神龙移动 | 181 `MOVE_DRAGON` |

结论：**宠物系统 = 能看到、不能用**。这是与原生差距最大的一整块玩法面。

**D. 明确不做（维持既有裁决，不重复立项）**

2 `GUEST_LOGIN`、10 `REGISTER_PIN`、16 `NAME_TRANSFER`、18 `WORLD_TRANSFER`、27 `STRANGE_DATA`、31/32 PIC 两项、118/129 GM 专属（ADMIN_CHAT/ADMIN_LOG）、137/138/139 婚礼/婚戒、161 `CASHSHOP_SURPRISE`、211/213/218 雪球/椰子/嘉年华、253 `MTS_OPERATION`（s9 无拍卖后端）、14099 `CUSTOM_PACKET` 调试通道。

**E. 附带发现（非 opcode 面）**

- 155 `BBS_OPERATION`：本端有 `BbsPackets`（已构造计数内），但**公会 BBS 界面完整度未验收**（原生 `GuildBBS.img` 多页签）。
- 83 `NOTE_ACTION`：留言条（CharInfo 写 note）路径需按 r-note 复核是否真发。
- 238 `USE_ITEMUI`(Raise)、236 `CHECK_CASH`、229 `CASHSHOP_OPERATION` 在 v2 扫描里都判定已接（商城购买 `CashShopBuyPacket` 四处 dispatch 坐实）；但 **MTS/拍卖窗口为壳**（原生窗口在、s9 无服务端）——显式预留。

---

## 2. 原生 UI 窗口面（112 节点对照）

客户端 `IO/UIElement.h::Type` + `IO/UITypes/*.cpp`（78 个窗类）+ `UIMiscShells`（58 个壳窗类）已覆盖绝大多数节点。真缺/半缺：

| 窗口 | 原生节点 | 状态 | 说明 |
| --- | --- | --- | --- |
| 临时属性面板 | `TemporaryStatView` | ❌ 全缺 | Buff 来源/剩余明细悬浮板（原生按 skill 分组显示 buff 条）。与 buff-timer 功能同源不同窗。**2026-11-04 Wave 5 复核**：s9 无独立 TEMPORARY_STAT 封包（SendOpcode/RecvOpcode/handler 全零命中，唯一 encodeTemporary 是 spawn 流 stati）——但本端 `Player::buffs`（掩码+skillid+时长）已在状态包解析链里，**可本地驱动建窗，不等服务端** |
| 称号窗口 | `Title` | ❌ 全缺 | 已获得称号列表+佩戴切换；无 Type、无 handler。~~s9 有 Title 数据下发面，需核对~~ → **证伪（2026-11-04）**：s9 两侧 opcode 与全源码零命中，无数据源。裁决改"保持壳/显式预留"（同 Ranking/MTS），除非你决定给 s9 加 Title 系统（双端批） |
| 公会副本 HUD | 287/288 对应表现 | ❌ | 同 §1.1 |
| 世界地图玩家层 | 376 | 🟡 | 请求发、回包不处理 |
| 排名窗 | `Ranking` | 🟡 壳 | s9 无查询后端 → 空壳（原生是「排行不可用」提示，观感已对齐，功能双侧都空） |
| 拍卖/MTS | `MTS` | 🟡 壳 | 同上，显式预留 |
| 婚礼全套 | `Wedding` | 🟡 壳 | 裁决=不做玩法，仅壳 |

（`MonsterBook/Maker/Alliance/Storage/Trade/Merchant/Guild/Party/Buddy/Messenger/Delivery/StoreBank/Incubator…` 均已有实体窗；r12/r802/r804 等已闭环。）

---

## 3. 扩展/定制功能矩阵（ijl15 33 项 + 萧曳自定义）——未实现清单

对照 `01-parity-matrix-and-plan.md`，以**当前源码现场 grep 复核**（不是照抄 10-21 的旧状态）：

### 3.1 纯客户端类

| # | 功能 | 现在 | 证据 |
| --- | --- | --- | --- |
| 8 | 解锁 300 级 | ❌ | `Character/ExpTable.h:26 LEVELCAP = 201`；服务端侧也未开 |
| 12 | 长伤害显示 | ❌ | `Gameplay/Combat/DamageNumber.h:40` 仍 `int32_t damage` |
| 7 | 血量突破上限 | ❌ | 无关键字命中 |
| 18 | 伤害皮肤 | ❌ | `DamageSkin` 0 命中（`UIWindow.img/DamageSkin` 资源在包内） |
| 19 | 每日签到 | ❌（仅 UI 归档） | `docs/features/daily-checkin/ui/v4` 有切图 PSD，**无窗类、无协议、未入库** |
| 20 | 美容院 | ❌ | 无实体窗（换发型实际走 NPC 商店+CashShop 面，非原生美容院窗） |
| 21 | 七彩棱镜染色 | ❌ | `Prism/染色` 0 命中 |
| 23 | 魔方结果窗 | ❌ | 无 |
| 24 | 背包格子锁定 | ❌ | 无 |
| 26 | 天气系统 | 🟡 | s2c `BLOW_WEATHER` 已接（`PacketSwitch:509`）、UIEvent 有雨；**客户端主导的自定义天气（0x373D/373F）未做** |
| 27 | 骑宠预览 | ❌ | 无 |
| 28 | 弹跳技能 | ❌ | 无 |
| 15 | 新角色骰子 | 🟡 | `UICommonCreation.cpp:450-459` 按钮/贴图已建但 **`set_active(false)` 常驻禁用**，随机 roll 逻辑未接 |
| 3/4/5 | 商店/仓库/玩家商店列表加长 | 🟡 | 基础仓库/商店已通（`UIStorage` 全链 dispatch）；「加长/更高列表」未见改动 |
| 6 | 金币超上限 | 🟡 | 读取侧 int64（既有）；显示格式化/上限校验未收口 |
| 9 | 角色槽位扩展 | ❌ | 未见选角槽位扩展改动 |
| 10 | 快速登录 | ✅ | `get_auto_login` + `UILogin.cpp:335-392`（旧矩阵 ❌ 已过期） |
| 11 | 潜能卷轴拖拽 | ❌ | 未接 |
| 13 | 侧边栏工具 | ❌ | 无 |
| 16 | RS 分辨率同步 | 🟡 | 已有 1024/多分辨率修正，未做完整 RS 系 |
| 17 | AllowCashTrade | ❌ | 未接 |

### 3.2 协议联动/双端类

| # | 功能 | 现在 | 备注 |
| --- | --- | --- | --- |
| 29 | 套装系统 | ❌ | 无数据消费点 |
| 30 | 团队 Buff 显示 | ❌ | 0x3713 通道未启 |
| 31 | 个人伤害统计 | ❌ | `DPT_TRACKER(0x3714)` 枚举在表、无 handler 实体 |
| 32 | 他人装备详情 | ❌ | 0x3726/27 未分配 |
| 33 | 装备成长 Tip | ❌ | — |
| 34 | 融合外观注能 | ❌ | — |
| 35 | 收纳背包 | ❌ | 0x3724/25 未分配 |
| 36 | 怪物图鉴 | 🟡 | r12 已交付 v83 卡册（设封面/列表）；D7 期望的**增强图鉴**未做 |
| 37 | 窗口现金商城 | ❌ | 全屏版可用且购买已通；窗口形态未做，0x3730 冲突规划（→0x3740）未执行 |
| 38 | 扩展装备栏（辅助装备/神秘徽章） | ❌（仅 UI 稿） | `addon-equipment/ui/v5`、`mystic_badge` 切图归档；无窗类、无槽位、无服务端结构 |
| 39 | 抽奖 XyLottery | ❌ | 双端皆无（s9 grep 无后端）；商城壳里的「彩票」是 CashGachapon 面 |
| — | 交易所 trade-house-083 | ❌（仅设计） | `docs/features/trade-house-083/` 有 DESIGN/PLAN-ui，无代码、s9 无后端 |
| — | 高清每日礼物 | ✅ | `UIDailyGift` 已交付（daily-gift-hd） |
| — | 地图信息/怪物/掉落面板 | ✅ | UIMapInfoPanel/UIMapMobPanel/UIMapDropPanel（含 278 掉落请求） |
| — | 地图搜索 | ✅ | UIMapSearchPanel |
| — | 家族三系创角 | ✅ | job-families / EA11719 提交线 |

---

## 4. 平台与工程面

| 项 | 状态 | 缺口 |
| --- | --- | --- |
| Windows | ✅ 主验 | — |
| Android arm64 | 🟡 | APK 可构建；**端到端真机点测未做**（masterplan M5 验收未勾） |
| Linux / iOS | ⬜ | 明示「随后」，未启动 |
| 中文 IME 输入 | ⬜ | s9-align 裁决「先显示后输入」，软键盘替代（T13 预留） |
| i18n pending-copy | 🟡 | R27-B1c 台账尚存待补键行（138 行版），软键盘档豁免 |
| 联机冒烟 | 🟡 | 聊天 s9 回显矩阵、地图对象波1-3、攻击布局 13 用例等均有「待真机确认」清单未逐项勾绿 |
| 工作树 | ⚠ 214 文件未提交 | GM 命令窗 + 字体兜底 + 商城按钮等并发在途 hunk（归属清单见 commit-hunks-B4b.md），未原子提交 |
| 调试残留 | ⚠ | `GraphicsGL.cpp dbgatlas → atlas_dbg.log` 仍在 Release 链路，待验证轮收尾删（r810 §5） |

---

## 5. 建议收敛顺序（决策点）

1. **P0 玩法硬缺口**：宠物/召唤族（§1.2-C，显示已通、补发送+命令 UI 即活）→ Buff 解除/AP 自动分配/技能书/回城卷（§1.2-A 里成本最低的五个）。
2. **P0.5 一致性小票**：`SET_HPMPALERT(0x1000)` 接存档、`CHANGE_QUICKSLOT` 持久化、`WORLD_MAP_PLAYERS(376)` 回包、`PICK_ALL_CHAR` 派发 —— 每项半日内可闭环。
3. **P1 原生窗补壳转实体**：`TemporaryStatView`、`Title`。
4. **P2 扩展矩阵**：按用户价值排（伤害皮肤 → 背包格锁 → 窗口商城 → 签到入库 → 扩展装备栏〔需双端，槽位结构先裁决〕→ 图鉴增强）。
5. **工程债**：真机冒烟清单逐项勾绿；工作树 214 文件按功能线原子提交；`atlas_dbg.log` 删除。

> 待你抉择：①首批范围取上文 1+2 还是含 3？②扩展装备栏/交易所是否立项（需动服务端结构）？③300 级/长伤害是否随本轮开（牵经验表+服务端上限）？

---

## 7. 扩展矩阵 ❌ 项批量复核（2026-11-05，同 §6 方法论）

对本轮 §3 矩阵里仍标 ❌ 的项做"代码面是否真缺"的现场复核（grep 全 UI/Gameplay/Character/Data）：

| 项 | 复核判据 | 结论 |
| --- | --- | --- |
| #15 新角色骰子 | `UICommonCreation.cpp:1260` handler→`randomize_look()`（1292 随机四围+外观）、`:1511` 建角各步 enable、`:664` classic 绘制 | **假缺口·已通**（矩阵 ❌ 应改 ✅）。注：该文件正被并发会话重写为「classic 仅命名」形态，本判据基于当前 HEAD |
| #18 伤害皮肤 | `UICharInfo.cpp:456` 注释已坐实：运行时 `UI.nx/UIWindow.img` 无 `DamageSkin` 节点、Effect 侧亦无 | **真缺**（且缺资源，非纯代码） |
| #20 美容院 | 无实体窗（换发型走 NPC 商店/CashShop 面） | 真缺（原生美容院窗未建） |
| #21 七彩棱镜 / #23 魔方结果窗 / #24 背包格锁 / #27 骑宠预览 / #28 弹跳技能 / #13 侧边栏工具 / #17 AllowCashTrade | 全仓 grep `Prism`/`Cube`/`LockSlot`/`MountPreview`/`BounceSkill`/`SideBar`/`AllowCashTrade` **0 命中**（SideBar 唯一命中在 UIWorldSelect 属无关词） | 真缺 |
| #19 每日签到 | 仅 UI 归档（`daily-checkin/ui/v4` PSD+PNG），无窗类无协议 | 真缺（素材已备） |

**净结论**：❌ 项里除 #15（假缺口）外皆为真缺；真缺中 #19 有现成素材（可直接进 Wave 6 首批）、#20/#21/#23/#24/#27/#28 属纯客户端（不需协议）、#13/#17 待定语义。**下一步 Wave 6 建议首打 #19 签到入库**（素材齐、纯客户端、可独立验收）。

### 7.1 #19「每日签到」复核（同日续，第 9 处假缺口 + 真实缺口二合一）

现场三条证据把这一项彻底定死：

1. **窗已存在**：`UIDailyGift.cpp`（615 行 / h `.h` 176 行）已是 28 天 7×4 签到窗，从
   `UIEvent::239` 可开、可关、带 hover/动画——**不是"未实现"**。
2. **素材是重绘件**：该窗现用 `ColorBox`（MINESHAFT/DARKGREY/EMPEROR/SILVER…）灰框占位；
   `docs/features/daily-checkin/ui/window/v4/`（513×346 PNG + 分层 PSD + cell 四态）
   正是给它的**皮肤件**，不是新功能 → 本项正确形态 = **"给已有窗换皮"**。
3. **协议不存在（双端）**：s9 `SendOpcode`/`RecvOpcode` 无任何 DAILY/SIGN/CHECKIN opcode，
   无对应 handler（仅 BosslogDaily/FamilyDailyReset 的定时任务，与签到无关）；客户端
   `UIDailyGift` 领取动作 `grep \.dispatch` **零命中**——领取是纯本地假动作。

**处置（分层，不含猜测）**
- 可做（纯客户端，不需用户拍板）：换皮 = 用 `daily-checkin v4` 资产替换 ColorBox。
  但这是**版式/观感改动**，按门禁 18 应走 Photoshop+IMG 通道并需确认 v4 即目标形态
  （PSD 在库、`build_v4.py` 可复算），且当前 `UIDailyGift` 采用程序绘制，
  换成贴图会改动其既有观感 → **属"改已有功能"，须用户点头**。
- 不可做：接协议。s9 无协议 ⇒ 按门禁 7「不发明客户端能力」，**维持假领取并显式标注**。
- 若要真签到：需**服务端新建**（DB 表 + opcode + handler），属双端立项，须用户决策。

**结论**：#19 从"Wave 6 首批候选"降级为**双端立项才可闭环**，且第一步（换皮）也要
用户确认观感目标；不再列为可自助开发项。
