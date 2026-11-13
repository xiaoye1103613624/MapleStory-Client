# 社交经济面 · 既有文档四态审计（r05）

> 本文件 = 第二路只读 subagent `076f3b24` 返回 14K 全文的结构化归档 + 主 agent §0 前置复核块。
> 基准：GMS 083 原版（BeiDou.exe 13337 + Data；s9=v83 派生）；095/PLUS2/怀旧岛192 不作参照。
> 术语：F=学院(FAMILY/UIFamily)；G=家族(GUILD，原版 UIUserList 第 3 页签)；
> Academy/BossParty 独立面板 = 095 only = 「不做」，非 083 缺口。
> 取值优先级：**checked-in source > 2026-11 现场重算 > 旧基线**。
> 行尾 [观察]=文档明写/源码逐字/IDA 逐字；[推断]=跨文档归并/冲突裁决；[待实机]=⏳ 需真机。
> 全程只读，未改任何文件。所有路径相对 `E:\project\MapleStory-Client`。

---

## 0. 主 agent 前置复核块（★ 与 r01/r02/r03 互证）

1. **★ 四态清单本身证实「大面积销案」**：Wave A 11 项全 ❌（r16:188-203）
   已被 `r810-commit-split-ledger.md:78` T14 一次补齐 20 个缺失系统窗外壳 + handler +
   出站包（54 path / social 33 / commerce 15）+ `r40-continuous-shell-coverage.md:5`
   15 壳 + `parity-gap-2026-11/README.md:107` + 30+ 源码类销案 → 与 Q2 §9.5
   「不缺窗口文件也不缺出站封包」结论一致。[观察+推断]
2. **★ 与 r02 §1 逐字互证**：`IO/UIStateGame.cpp:361-384` G/BOSSPARTY→
   `UIUserList::Tab::GUILD` 第 3 页签；Angel r01 `CUIUserList::OnCreate` 公会组
   **2030–2041** 为 tab index 2 → 用户 2026-11-07 裁决已在源码 + IDA 双侧闭环。[观察]
3. **★ 与 r03 §0 互证**：`social-loop-audit-2026-11/README.md:20,22,70-75`
   判「韩系导师 Academy 只见 095 线、083 UIWindow.img 顶层 0 命中 → 不做」，
   r03 §0.1 又补「083 顶层清单无 Guild/Alliance/Academy/BossParty 独立窗」→ 三重证据。[观察]
4. **⚠️ 本面新发现 2 个 095 文档污染点**（r02 已记 10 处 `UIMessenger.cpp` v95 注释，
   本清单再补 2 处独立文档）：
   - `docs/features/messenger-btclose-coords/README.md:16` 逐字「`CUIMessenger::SetCtrl 0x7F34C0`
     `(276,6)` 来自 **Maplestory95(095 线)**」→ 按 083 基准应作旁注、须 Angel
     `CUIMessenger::SetCtrl 0x8505e2` 复核（本端 `Basic.img` 无 BtClose3 → `close_btn()` 12×12）。
   - `docs/features/guild-bbs-ui/README.md:20,27` port-095 `CUIGuildBBS::SendLoadList/...`
     → c2s 179（本端 s9=`BBS_OPERATION` 155，编号差属 095 旁注）。
   → 列 D 决策点：**只换注释依据，不改行为/协议**。[观察+推断]
5. **★ 同文件内部冲突 6 处**（§2b 后半）：Q2 `feature-gap-inventory-2026-11/README.md`
   `:175` vs `:342`（好友增删）、`:176` vs `:343/:606`（220 组队搜索）、`:185` vs `:189`
   （婚礼）、`:486` vs `:346/:493`（0x8B）、`:489` vs `:606`（0xFD）、`:597` vs
   `guild-boss-hud/README.md:8`（287/288 已接线）→ 全部按「checked-in source > 旧基线」
   取值，列文档一致性决策点（D11 同型，交用户裁决后统一回写，不擅改 Q2）。[推断]
6. **★ `docs/features/userlist-five-tabs/README.md` 不存在**（目录唯一文档为
   `notes/diagnosis-and-plan.md`，glob + read 双证）→ 文档漂移，不得臆造 README 内容。[观察]
7. **真缺口形态收敛**：与前三面一致 = 「控件已建/坐标齐/点击可达但回调未抽全 +
   显式预留 + 文档一致性」，**不是缺窗口/缺排版/缺 handler**。[推断]

---

## 1. 已校准 ✅（源码/协议/符号闭环，可销旧账）

- `IO/UIStateGame.cpp:361-384` — G/BOSSPARTY→`UIUserList::Tab::GUILD`（第 3 页签），
  逐字注释「// v83 G(17) = GUILD「家族」…不是独立 UIGuild 浮窗」；
  FRIENDS→FRIEND / PARTY→PARTY，`:388-389 emplace<UIUserList>(tab)`。[观察]
- `docs/features/social-loop-audit-2026-11/README.md:100` —「**接线 | G 键**
  （`KeyAction::Id::GUILD`）改开 `UIUserList(Tab::GUILD)`」裁决已在源码落地，
  旧账「独立 UIGuild 浮窗」销案。[观察]
- `docs/features/social-economy-angel-calibration-2026-11/notes/r01-angel-native-social-facts.md:164`
  — Angel `CUIUserList::OnCreate` 家族(公会) tab index 2 控件组 **2030–2041**
  （BtInvite/BtKick/BtWithdraw/BtUp/BtDown/BtWhere/BtWhisper/BtNotice/BtChat/BtInfo/
  BtPartyInvite/BtGuildBBS），逐字互证第 3 页签。[观察]
- `docs/features/social-economy-angel-calibration-2026-11/notes/r02-main-agent-crosscheck-cancel.md:63-65`
  — 判「✅ 已校准，G/BOSSPARTY 与 Angel 083 公会控件组 2030–2041 逐点吻合，
  MEMORY.md critical 术语在源码层已落地」。[观察]
- `r02:95-100` — 5 页签顺序/显隐/逐格坐标 ✅ 已校准；`IO/UITypes/UIUserList.cpp:88-110`
  `TAB_POS[5]={(4,23),(35,22),(66,22),(96,23),(149,22)}` 为「正确端截图 oracle 实测」。[观察]
- `r01:214-217`/`r02:111` — 学院 `CUIFamily/CUIFamilyChart` 与公会
  `CTabGuild/CTabGuildAlliance/CUIGuildBBS` **类树分离**，「学院≠家族」符号层铁证；
  `IO/UITypes/UIFamily.cpp:2` 学院 / `UIGuild.h:31-37` 家族页切片。[观察]
- `docs/features/beidou-v17-parity/notes/r22-messenger-protocol.md:11-22` — c2s
  `MESSENGER=122`(0x7A) mode 0 Open(`int messengerid`,0=建房)/2 Leave/3 Invite
  (`string name`)/5 Decline(`string sender`)/6 Chat(`string text`)；s2c `313`(0x139)。[观察]
- `r02:139-141`/`r01:195-202` — `Net/Packets/MessengerPackets.h:13,24,34,45,56` 五包
  + Angel `CUIMessenger::OnCreate` 逐字 `COutPacket(122)+Encode1(0)+Encode4(a2)`，
  `PacketSwitch.cpp:453` 已注册 → ✅ c2s 122 + mode 集对齐。[观察]
- `docs/features/wedding-flow/README.md:29-55` — 婚礼 c2s `RING_ACTION=137`(0x89)/
  `WEDDING_ACTION=138`(0x8A)/`WEDDING_TALK=139`(0x8B) 全 mode；s2c
  `MARRIAGE_REQUEST=72`/`RESULT=73`/`GIFT_RESULT=74`/`NOTIFY_MARRIED_PARTNER_MAP_TRANSFER=75`/
  `WEDDING_PROGRESS=320`/`CEREMONY_END=321`。[观察]
- `docs/features/social-loop-audit-2026-11/README.md:19`/`parity-gap-2026-11/README.md:105`/
  `feature-gap-inventory-2026-11/README.md:185` — 婚礼 ✅ 双端闭环 2026-11-06，
  「此前判『壳/不做』**前提已证伪**」；`:110` `gen_opcode_matrix.py --check` GATE OK /
  never-constructed=0。[观察]
- `social-loop-audit-2026-11/README.md:17-18` — 组队 ✅（`PARTY_OPERATION=124`/
  `DENY_PARTY_REQUEST=125`+s2c PARTYLIST）；家族 ✅ c2s 145–153 全 9 包 +
  s2c 94–104 全 handler + `UIFamily`/`UIFamilyTree`。[观察]
- `docs/features/s9-protocol-align/notes/s9-social.md:79-85` — 好友 `BUDDYLIST` c2s
  错配 63→`BUDDYLIST_MODIFY=130`(0x82) 已修 2026-11-02；**非对称要点**：s2c
  `PacketSwitch.cpp:158 BUDDYLIST=63` 保持不动（c2s130/s2c63 是 v83 双枚举标准）。[观察]
- `r02:170-192` — `Net/PacketSwitch.cpp` 社交约 40 条已注册（`:158 PARTY_OPERATION=62`/
  `:159 BUDDYLIST=63`/`:161 MESSENGER=313`/`:164 STORAGE=309`/`:170 MEMO_RESULT=41`/
  `:171-175 FAMILY_*`/`:182-184 WEDDING_*`/`:346 GUILD_BBS_PACKET=59`/
  `:395-396 ALLIANCE=66`/`:406-408 GUILD_BOSS 287/288`）→ ✅ 收包 handler 面完整。[观察]
- `feature-gap-inventory-2026-11/README.md:11`/`:318-328` — `client_out_constants/
  client_out_sent` 148/148 全有构造点、917 处 `.dispatch()`、`PacketSwitch.cpp:417-782`
  305 条 emplace；`:342` `BUDDYLIST_MODIFY→BUDDYLIST(130)` ✅ 有发送点
  `BuddyPackets.h:32,43,53`+`UIBuddyList.cpp:94..283`。[观察]
- `feature-gap-inventory-2026-11/README.md:346`/`:493` — `WEDDING_TALK_MORE(0x8B)` ✅
  有等价包 `WEDDING_TALK=139`（`WeddingPackets.h:210`+`UIWedding.cpp:281`+
  `OutPacket.h:317`），撞值不重复建枚举。[观察]
- `docs/features/beidou-v17-parity/notes/r810-commit-split-ledger.md:78` — T14 `f5af924`
  一次补齐「**20 个缺失系统窗外壳 + handler + 出站包**」54 path；`:73` social 33 path、
  `:75` commerce 15 path。[观察]
- `docs/features/beidou-v17-parity/notes/r40-continuous-shell-coverage.md:5` — 15 壳
  「PersonalShop·MemberShop·TradingRoom·EntrustedShop·CashTradingRoom·ItemSearch·
  AranSkillGuide·Transform·MonsterKilling·MapleEvent·SkillEx·Book·QuestAlarm·
  MateMessage·BetaEdition」；`:7` Minigame 已接 PLAYER_INTERACTION ROOM/READY/START/EXIT。[观察]
- `parity-gap-2026-11/README.md:107` —「MonsterBook/Maker/Alliance/Storage/Trade/
  Merchant/Guild/Party/Buddy/Messenger/Delivery/StoreBank/Incubator… 均已有实体窗；
  r12/r802/r804 等已闭环」。[观察]
- `IO/UITypes/*.h` 当前端类存在性 — `UIAlliance.h:35`/`UIBuddyList.h:44`/
  `UIDelivery.h:24`/`UIFamily.h:36`/`UIFamilyTree.h:27`/`UIGuild.h:34`/
  `UIGuildBBS.h:48`/`UIGuildBoss.h:15`/`UIMapleTV.h:6`/`UIMemo.h:23`/
  `UIMerchant.h:33`/`UIMessenger.h:22`/`UIMTS.h:15`/`UIParty.h:48`/
  `UIPartyRace.h:12`/`UIPartySearch.h:13`/`UIStorage.h:39`/`UIStoreBank.h:21`/
  `UIUserList.h:27`/`UIWedding.h:16` + `UIMiscShells.h:442,454,465,476,488,499`
  （PersonalShop/MemberShop/TradingRoom/EntrustedShop/CashTradingRoom/ItemSearch）
  → 30+ 社交经济类存在，Wave A「无窗/无类」旧账销案。[观察]
- `docs/features/guild-boss-hud/README.md:7-9,16`/`r02:184` — 公会 Boss HUD
  `UIGuildBoss` ✅ + 287(`short y`)/288(`byte state`) 接线 ✅ + `MapGuildBoss` 动画态机 ✅；
  **据此 `feature-gap-inventory-2026-11/README.md:144`(C17「客户端无 handler」) 与
  `:597`(§16 #12「s2c 287/288 无 handler ❌维持」) 属与源码相反的旧账，应销案**。[观察+推断]
- `r01:81-88` — Angel↔BeiDou EA 交叉 **752/752 全命中 / 0 缺失**，
  `"same binary layout (752/752 EAs identical; differences are IDA analysis coverage, not code)"`。[观察]
- `social-loop-audit-2026-11/README.md:27-30` — 公会/联盟本轮补完 s2c 0x03 邀请 YesNo、
  0x0D 公会名全块解析 `set_guild_names`、0x02/0x0E/0x18/0x1A/0x1D 静默消费、
  `AllianceAcceptPacket(0x04)`；`:104-107` GuildOperation 逐字段 0x05/0x07/0x08/0x0E/0x10/0x1A。[观察]
- `docs/features/userlist-five-tabs/notes/diagnosis-and-plan.md:65-66`/
  `social-loop-audit-2026-11/README.md:113-115` —「`GuildUnion` 不是"只有美术、无内容"
  ——它有 20 个直接子节点（12 按钮 + 7 位图 + 独立 `backgrnd`）」；第 4 页签家族联盟
  2026-11-08 已按运行时节点还原，**不再"只画不切页"**，推翻
  `feature-gap-inventory-2026-11/README.md:159`(C32 ❌只有美术)。[观察]
- `diagnosis-and-plan.md:145-155` — 实施记录：`UIUserList.h` `Tab::UNION` + 40+ 按钮枚举；
  `UIUserList.cpp` `bind_runtime_assets()` 按 158 节点树绑定 **44 个按钮 + 5 个 Slider**、
  draw 五分发、三张互斥底；`:152-155` GATE OK / EXITCODE=0 / exe 2026-11-08 01:41
  4,920,832 B。[观察]

---

## 2. 未闭环 🟡/❌（真缺 / 显式预留 / 文档漂移）

### 2a 真缺 / 显式预留（行为层）
- `feature-gap-inventory-2026-11/README.md:158` — C31 `UIUserList::change_target(int32_t)` /
  `BT_GUILD_CHAT` 本地回显 🟡 预留空实现（`IO/UITypes/UIUserList.cpp:914-920`）；
  `social-loop-audit-2026-11/README.md:116-117`「`UIChatBar::change_target(int32_t)`
  仍是全客户端共用的预留空实现」；`r02:229` 判「❌ 真缺，跨面共性，HUD 面
  `UIChatBar.h:123` 同款 6 频道快捷键死链」。[观察]
- `feature-gap-inventory-2026-11/README.md:343`/`:356-367`/`:606` —
  `PARTY_SEARCH_REGISTER(0xDC/220)` ❌ 完全无（`Net/OutPacket.h` 无 220，仅
  `PARTY_SEARCH_START=222@:222`/`UPDATE=223@:223`）；`IO/UITypes/UIPartySearch.cpp:246`
  「s9 空 stub → Start 代理」；§9.5.3 #3 + §16 ★ 真「完全无」P1。[观察]
- `feature-gap-inventory-2026-11/README.md:347`/`:489`/`:606`/
  `parity-gap-2026-11/README.md:89,104`/`s9-protocol-align/notes/gap-inventory.md:111`
  — `MTS_OPERATION(0xFD)` c2s 完全无（仅 `ENTER_MTS=156@OutPacket.h:291`；
  `PacketSwitch.cpp:325` 348 是 s2c；`WaveAShellHandlers.cpp:3551` 仅 LOG）；
  `UIMTS.cpp:86-95` draw 仅 5 行 Text、`:97-99 void UIMTS::update() {}` 空体；
  s9 无拍卖后端 →「MTS/拍卖窗口为壳」显式预留。[观察]
- `feature-gap-inventory-2026-11/README.md:143`/`:600`/
  `parity-gap-2026-11/README.md:87`/`docs/features/guild-bbs-ui/README.md:27`
  — 公会 BBS 完整度（734×526 多页签）🟡 未验收；`BbsPackets` 有但「界面完整度未验收」，
  按钮落位「无 WZ layout 节点…真机可微调」。[观察]
- `feature-gap-inventory-2026-11/README.md:145`/`guild-boss-hud/README.md:28`/
  `pending-live-tests/README.md:230` — guild pulley 交互上行（原生 `sub_558B45`）
  🟡 显式预留，「交互键发包…本轮未接——无服务端脚本消费证据」。[观察]
- `feature-gap-inventory-2026-11/README.md:156` — C29 `UIParty` 邀请/离开/队长/开除四钮
  🟡 显式预留（本包 UI.nx 根下无 `Party.img`，v83 也无独立队伍窗根；仍有 SoftKey L/E）。[观察]
- `feature-gap-inventory-2026-11/README.md:157` — C30 `UIStorage`
  `BtArrange/BtMesoIn/BtMesoOut` 🟡 显式预留（本包无 `Storage.img`，v83 真身
  `UIWindow.img/Trunk` 463×318，「无参考截图也无 IDA 证据」）。[观察]
- `feature-gap-inventory-2026-11/README.md:160`/`parity-gap-2026-11/README.md:132`/`:161`
  — C33 商店/仓库/玩家商店列表加长 🟡「基础仓库/商店已通（UIStorage 全链 dispatch）；
  加长/更高列表未见改动」；C34 r40 5 壳「深度未达 C 层」。[观察]
- `feature-gap-inventory-2026-11/README.md:235` — F4 MapleTV 开播态 🟡 显式预留
  「`SEND_TV` 只保留提示窗，待 REA/IDA 取证后另立 Plan」；
  `pending-live-tests/README.md:474-475` 同句逐字。[观察]
- `feature-gap-inventory-2026-11/README.md:181` — D7 每日签到 ❌ 壳在双端协议缺
  （`UIDailyGift` 615 行 28 天 7×4，ColorBox 灰框假领取，`grep .dispatch` 零命中，
  s9 两侧零 DAILY/SIGN/CHECKIN opcode）。[观察]
- `feature-gap-inventory-2026-11/README.md:183`/`parity-gap-2026-11/README.md:156`/
  `docs/features/trade-house-083/DESIGN.md:3-4,531-537` — D9 交易所 `trade-house-083`
  ❌ 仅设计（有 DESIGN/PLAN-ui、无代码、s9 无后端）；「本版明确不做（预留）：
  客户端 `UITradeHouse` 等业务代码 / 服务端 Handler / 定时结标任务 / DDL / DML /
  Photoshop PSD/PNG / opcode 最终数值锁定」。[观察]
- `feature-gap-inventory-2026-11/README.md:184` — D10 抽奖 XyLottery ❌ 双端皆无。[观察]
- `feature-gap-inventory-2026-11/README.md:177`/`social-loop-audit-2026-11/README.md:30`
  — D3 公会改职衔 `0x08` 出站 🟡「服务端有 handler，客户端缺钮，低优先」；
  创建联盟「s9 走 NPC 脚本 `createAlliance`，非 c2s，端内无需新包」。[观察]
- `feature-gap-inventory-2026-11/README.md:179`/`parity-gap-2026-11/README.md:88`/
  `pending-live-tests/README.md:234` — D5 `NOTE_ACTION(83/131)` 留言条 🟡 需复核
  （s9 `NoteActionHandler` **仅当** `CashShop.getAvailableNotes()>0` 才落库）；
  「路径需按 r-note 复核是否真发」。[观察]
- `docs/features/wedding-flow/README.md:130-132` — 合影预留（s9 `onTakePhoto` 需外部
  WvsMapGen → 仅 tip）；婚礼服装/门票走 CashShop；「Amoria 地图物件与婚礼天气资源面未盘点」；
  `feature-gap-inventory-2026-11/README.md:185` 同。[观察]
- `feature-gap-inventory-2026-11/README.md:189` — D15 `UIWedding`「本端不做判定，只发
  action=1」🟡 壳·玩法裁定（`IO/UITypes/UIWedding.cpp:280`）。[观察]
- `feature-gap-inventory-2026-11/README.md:356-367`(§9.5.3 #9) — 收包仅
  deactivate/LOG/return：`WEDDING_CEREMONY_END(321)`/`MONSTER_CARNIVAL_LEAVE(295)`/
  `MTS_OPERATION(348,347)`/`FIELD_EFFECT(138)`（`WaveAShellHandlers.cpp:507,710,3551,3565`、
  `TestingHandlers.cpp:186`）。[观察]
- `r02:230` — `UIGuild` 独立浮窗（083 原生无，`UIGuild.h:37 TYPE=GUILD` +
  `UIStateGame.cpp:363` 自证）列 [s9 适配] 保留双入口裁决点。[推断]
- `r02:143-164` — ⚠️ `IO/UITypes/UIMessenger.cpp` **10 处 `v95` 注释**作为行为依据
  （`:136/:159/:168/:237/:278/:290/:410/:486/:537/:549`，blink/chatBalloon/SetCtrl/
  OnChat/Update），依据链不合 083 基准，应换引 Angel `CUIMessenger`（26 方法），
  **只换注释依据、不改行为/协议**。[观察+推断]
- `diagnosis-and-plan.md:162-178` — §8「**显式预留（不假装闭环）**」8 条：落位目视初值
  未 `layout.py` 逐格量测(Δ>2px 需复拍)、oracle `check` 未跑、列表仅空态
  (0/25、0/0、0/26)、按钮语义映射按节点名推断、`BT_GUILD_CHANGE`/`PartyHP` 未落位
  未接线、disabled 态需数据驱动、friend_line/page_selected_row 未参与绘制。[观察]

### 2b 文档漂移 / Q2 旧账（与 checked-in source 相反，逐条列行号，勿再据此立项）
- `feature-gap-inventory-2026-11/README.md:36` — 推断结论：r16 / r00:95-143 /
  r14:117-158 / 01:59-122 的 ❌ 大表是「2026-10-21 旧账，大量已销案，须区分
  『窗口类/壳存在』与『行为闭环』」。[推断]
- `docs/features/beidou-v17-parity/notes/r16-rea-coverage-gap.md:188-203` — Wave A 11 项
  全 ❌（Messenger:192 / PartySearch:193 / Delivery:194 / StoreBank:195 /
  ViciousHammer:196 / VegaSpell:197 / CashGachapon:198 / RemoteGachapon:199 /
  Family:200 / Memo:201；`:203`「全部未达 C」）→ 已被 r810:78 + r40:5 +
  `parity-gap:107` + 30+ 源码类销案（「无窗」层面），REA B→C 证据债另计。[观察+推断]
- `r16-rea-coverage-gap.md:207-219` — Activities 全 ❌（MonsterCarnival:211 /
  Minigame:212 / Wedding:213 / PartyRace:214 / DualMobGauge:215 / MapleTV:217
  「A | ❌ | P4 后置」）「整批未达 C；主链验收不阻塞于此」。[观察]
- `r00-baseline-parity.md:91-141` — 旧基线：组队 62/201 未注册、好友 63 未实现、
  仓库 Trunk/StoreBank 无(:99)、Family 无(:103)、Messenger 无(:104)、MapleTV 无(:106)、
  Delivery 无(:107)、Memo 无(:109)、`STORAGE(309)`/`MESSENGER(313)`/`FAMILY_*(94–104)`/
  `PLAYER_INTERACTION(314)`/`GUILD_OPERATION(65)` 全 ❌(:128-141) → 均被 r02 §5
  源码 handler 注册销案。[观察]
- `r14-native-feature-catalog.md:117-121,127,130,151-154,176-191` — 旧 catalog
  「Messenger 无 UI/无包、Family 无 UI/无包、Memo 无 UI、Delivery 无、PartySearch 无、
  RemoteGachapon 无、StoreBank 无、Wedding 无、MTS 无、BBS 无」→ 源码类存在性销案。[观察]
- `r17-full-native-analysis-and-dependency-plan.md:227,255,267,272,277-279,296,308-315,
  388-397` — 旧依赖计划「待 REA 升层 / 无构造 EA / 构造 EA 待 xref」证据债 → Angel r01
  已给 `CUIMessenger(26)`/`CUIFamily(3)`/`CUIFamilyChart(8)`/`CUIPartySearch::StartSearch
  0xa12073` 等具名 EA，EA 债销案。[观察+推断]
- `feature-gap-inventory-2026-11/README.md:175`(§6 D1) vs `:342`(§9.5.2 #9) — 同文件
  冲突：D1「好友增删 🟡 需先补 UI」被 §9.5.2「✅ 有发送点」推翻，按 checked-in source
  取 ✅。[推断]
- `feature-gap-inventory-2026-11/README.md:176`(§6 D2「🟡 已代理消化」) vs `:343`
  (§9.5.2「❌ 完全无」)/`:606`(★真缺 P1) — 同文件冲突，220 真无，取「真缺但用
  222/223 代理」。[推断]
- `feature-gap-inventory-2026-11/README.md:185`(D11 婚礼 ✅) vs `:189`(D15 UIWedding
  🟡 壳) — 同文件口径差，按 `:582` §9.5 覆写规则「原 ❌ 降级为玩法深度/真机验收」。[推断]
- `feature-gap-inventory-2026-11/README.md:486`(§14 0x008B 明确不做) vs `:346`/`:493`
  (有等价包 139 已落地) — 旧账，销案。[推断]
- `feature-gap-inventory-2026-11/README.md:489`(§14 0x00FD 明确不做) vs `:606`
  (§9.5.3 #4 真缺·仅 UI 有) — 口径冲突，按 `:493` ②「同时是真缺」取值。[推断]
- `feature-gap-inventory-2026-11/README.md:597`(§16 #12 公会副本 HUD ❌维持) vs
  `guild-boss-hud/README.md:8`/`r02:184`(287/288 已接线) — 与源码相反旧账，销案。[观察+推断]
- `feature-gap-inventory-2026-11/README.md:582` — §9.5 覆写逐字「原 TOP20 里
  #1/#3/#4/#5/#6/#7/#10 …已被协议层权威重算 + 源码逐字 `.dispatch()` 推翻…
  **原 ❌ 一律降级为「玩法深度 / 真机验收」，勿再按补发包立项**」。[观察]
- `docs/features/s9-protocol-align/notes/gap-inventory.md:119` — 汇总「**明确不做
  （共 19）**：0x0002…0x0089 0x008A 0x008B…0x00FD 0x3713」把 0x89/0x8A 列入，
  但同文件 `:81`/`:82` 已改判「2026-11-06 已接」→ 汇总行属旧账。[观察+推断]
- `parity-gap-2026-11/README.md:83`(§1.2-D 明确不做把 137/138/139 婚礼婚戒列入)
  vs `:105`/`wedding-flow:7-8`（✅ 双端闭环）— `feature-gap-inventory-2026-11/
  README.md:69-74` 冲突取值表①已裁决取 ✅。[推断]
- `docs/features/corewindow-angel-calibration-2026-11/README.md:14,17,295,353`/
  `docs/features/statusbar-hud-angel-calibration-2026-11/README.md:269` — Q2 清单
  C14/C34/C35/§16 #2/#9 旧账（`:141` C14「5 tooltip 指向不存在 UIToolTip.img/Item」
  已由 `EquipTooltip.cpp:53` 销案；`:594` §16 #9 引用已失效
  `UIWorldSelect.cpp:697-698` 现为 `clear_selected_world()`）；`:295`「**交用户裁决后
  统一回写（本面只出销案证据，不擅改 Q2 文档）**」。[观察+推断]
- `r02:232-233` —「Wave A 壳窗深度（Deliver/StoreBank/FamilyTree/Memo/MTS 等）待
  r04/r05 复核；Q2 清单已大面积销案」「旧文档四态未同步：`feature-gap-inventory-2026-11`
  §6/§14/§16 部分仍标 ❌」。[观察]
- `docs/features/userlist-five-tabs/README.md`（任务点名）— **该文件不存在**，目录唯一
  文档为 `notes/diagnosis-and-plan.md`（glob + read 双证），属文档漂移，不得臆造
  README 内容。[观察]

---

## 3. 待实机 ⏳（需真机/双角色/截图，未验证）

- `pending-live-tests/README.md:42`(§#32)/`wedding-flow/README.md:113,120-126`/
  `social-loop-audit-2026-11/README.md:66` — 结婚全链 7 步 ⏳（双号 A 男/B 女 同图
  ≥50 级 A 持 2240000；拒绝 msg0；4031377→4031406；NPC 9201008 预约→9201010
  心愿单→9201014 礼台；解约 4031357..64；R28 回归冷启动抓帧 UIWedding/UIFamily/
  UIGuild/UIAlliance 不得出现在未主动打开帧）；exe 23:16 已部署。[待实机]
- `pending-live-tests/README.md:43`(§#33)/`feature-gap-inventory-2026-11/README.md:562`
  — 联盟补完 ⏳ 需双公会（盟主邀请→对端 YesNo→0x0D 公会名列表→N 改公告；
  c2s 143/144、s2c 66）。[待实机]
- `pending-live-tests/README.md:44`(§#34)/`feature-gap-inventory-2026-11/README.md:563`
  — 组队/家族 R28 回归抓帧 ⏳（s2c 62/95/96…冷启动登录链）。[待实机]
- `pending-live-tests/README.md:45`(§#35)/`feature-gap-inventory-2026-11/README.md:564`
  — 五死键接线 ⏳（F 学院家族/H 密语/G 共用窗第 3 页签家族/O 组队搜索/L 预期无反应；
  exe 2026-11-07 00:02 已部署）。[待实机]
- `pending-live-tests/README.md:46`(§#36) — 玩家列表 & 家族五页签共用窗还原 ⏳
  （按 G 开窗逐页复拍：好友 0/25+分组行+12 钮／组员 0/0+9 钮／家族 [0/0]+none 行+
  4x3 钮+滚动条／家族联盟「还未加入联盟。」+12 钮／拒绝 0/26+增删；TAB 循环五格；
  三张互斥底图不得叠底；c2s 130/124/GUILD/143；exe 2026-11-08 01:41 已部署）。[待实机]
- `pending-live-tests/README.md:17`(§#4),`:155-163`/`feature-gap-inventory-2026-11/
  README.md:187`(D13) — PersonalShop FM CREATE ⏳（前置 FM 房间 `910xxxxxx` 且
  >910000000 + 现金栏 `514*`(个人店)/`503*`(雇佣商人)；非 FM 双击→红字
  「Stores only in the free market」不发包；`PLAYER_INTERACTION(123)` CREATE 字段序
  `00 | createType(4=514/5=503) | mapleStr title | 00 00 00 | int cashItemId`；
  MiniRoomError 15 → 红字不得误开交易窗）。[待实机]
- `pending-live-tests/README.md:18`(§#5)/`feature-gap-inventory-2026-11/README.md:186`
  (D12),`:549` — Messenger 立绘 ✅ 本机+对方 s2c 313 mode1（0.70 / foot -10，
  2026-11-06 p9 双角色同房）；`:173-174` 脚点悬空/裁切需微调 `LOOK_FOOT_*`。[观察+待实机]
- `pending-live-tests/README.md:19`(§#5b)/`feature-gap-inventory-2026-11/README.md:549`
  — Messenger blink/chatBalloon 🟡「p9 已同房打字；单帧未稳抓泡/blink → 手点补拍」
  （`Messenger/blink` + `chatBalloon/0..4`，mode6 payload `Name0/Name1` 无冒号）。[待实机]
- `pending-live-tests/README.md:21,27`(§#6/#12),`:202-214`/
  `feature-gap-inventory-2026-11/README.md:550` — Guild BBS 摘要 + 完整窗 ⏳
  （c2s `BBS_OPERATION=155`/s2c `GUILD_BBS_PACKET=59`；`GuildBBS.img` 大窗 734×526
  自动发 List(page=0)；W 发帖/N 公告/1/2/3 icon/E 编辑/D 删/P 回复；List=mode2/
  Open=mode3/Post=mode0/Reply=mode4；s2c mode 0x06/0x07）。[待实机]
- `pending-live-tests/README.md:28`(§#13),`:216-230`/
  `feature-gap-inventory-2026-11/README.md:554` — 公会 Boss HUD+物件 ⏳
  （进 `990000900`；「s9 当前脚本**不发** `GuildBoss_*`（仅 `GuildPackets` 工厂）
  → 需 GM/临时脚本 `broadcastPacket(GuildPackets.GuildBoss_HealerMove(y))` /
  `PulleyStateChange(state)`」；治疗精灵 x=295,y=-564 / 滑轮 x=-59,y=-394 /
  yMin=-564..yMax=-350；12s 无新包 HUD 自动隐藏）。[待实机]
- `pending-live-tests/README.md:29`(§#14),`:232-242`/
  `feature-gap-inventory-2026-11/README.md:555` — CharInfo SoftKey 便签 ⏳
  （N→`NOTE_ACTION=131` action0，body=`byte0 + mapleStr name + mapleStr msg`；
  无商城额度时客户端仍发包、服务端静默跳过）。[待实机]
- `pending-live-tests/README.md:470-472`/`feature-gap-inventory-2026-11/README.md:236`
  (F5),`:565` — MapleTV 双屏/点 NPC·角色·玩家/装备对比 ⏳（进 FM `910000000`
  木框黑板 NPC 9250025；「pass2/4/5b/6 点击坐标均未命中目标…属驱动坐标问题非功能
  结论」；测试号无可穿戴道具）。[待实机]
- `diagnosis-and-plan.md:105-106,128,164-166,138` — 五页签 oracle `layout.py mark/bias`
  逐格探针未跑、`check` 未跑（需用户屏前复拍 5 页客户端帧）；`:128` 真机冒烟 5 页签
  逐页复拍 + 按钮发包抓 `packet_dump.log`；逐字「真机复拍需用户在屏前；本环境历史
  抓屏全黑，不假装已验证」。[待实机]
- `social-loop-audit-2026-11/README.md:111`/`feature-gap-inventory-2026-11/README.md:567`
  — 真机冒烟未跑：「需用户在屏前按 G → 看第 3 页签渲染、点成员行选中、逐个点按钮发包；
  本环境无法自截实机画面（历史抓屏全黑问题），不假装已验证」；「社交五窗按 G 逐项点选」⏳。[待实机]
- `wedding-flow/README.md:112,115-116` — MSBuild Release|x64 ✅ 0 编译错误，但链接
  ⚠ `LNK1104` 客户端进程 PID 34476/21908 占用；「链接阻塞首次遇到，按门禁 14 需用户
  抉择」；`:113` 真机流程 ⏳。[待实机]
- `pending-live-tests/README.md:67-70`,`feature-gap-inventory-2026-11/README.md:531-539`
  — 四类全局阻塞阻全部社交实机：A GM 命令注入失败（`IO/UI.cpp:264-269` 吞 `!` 与
  空格，SendKeys/Ctrl+V/Unicode SendInput 均未产 `0x31` 命令包）、A′ harness 遗留
  4 前置全缺（lv1/job0/ap0、背包 0 件）、B 需第二角色/公会/自由市场/商城便签额度
  （「需受控 DML 造数据…**未执行**」）、C `game_config.use_server_auto_pot=false`、
  D 并行会话高频改同一仓库/重建 exe/结束客户端进程。[观察+待实机]

---

## 4. 推测（[推断]：裁决 / 归并 / 无一手证据，非实测闭环）

- `feature-gap-inventory-2026-11/README.md:69-74` — §2.4 六处冲突取值裁决（①婚礼取✅
  ④0x89/0x8A 取改判✅ ⑤WORLD_MAP_PLAYERS 取真机闭环 ⑥协议 c2s 148 为准）；
  `:676-679` 本文性质「观察=文档明写判定或源码逐字行；推断=跨文档去重/大类归并/
  冲突裁决/优先级排序」。[推断]
- `feature-gap-inventory-2026-11/README.md:612-633` — Angel 083 价值与使用约束：
  「符号详细 ≠ 功能更多」（3122 修饰名/361 类/2612 方法 vs BeiDou 50/10/0；函数总数
  55016 vs 55655；imagebase 均 0x400000；版本串逐字相同）；`:633`「控件 id/坐标/包序
  先在 Angel 定位再回 13337 交叉验证，冲突以 13337 + 实机 WZ/IMG 为准；Angel 字符串
  缓存仅 1281(BeiDou 3889)；端口会变，每次先跑 `server_health` 读 `idb_path`」。[推断]
- `r16-rea-coverage-gap.md:13-14,184,203,278-280` —「**「没反编译完」≠「55654 个函数
  都没反」**」「**「有窗」≠「已反编译完」**」「全部未达 C」「端侧 ✅ 只表示『当前能玩到
  某种程度』**不**表示 REA 已达 C」—— REA A/B/C 证据分层，端侧闭环≠REA 达 C。[推断]
- `social-loop-audit-2026-11/README.md:20,22,70-75`,
  `feature-gap-inventory-2026-11/README.md:499-501`,`wedding-flow/README.md:134-135`
  — 基准裁决：韩系导师 `Academy` 只见 095 线符号（083 `UIWindow.img` 顶层 0 命中、
  s9 零命中）；远征独立招募 `BossParty`/`ExpeditionIntermediary` 是 095 证据、
  GMS113 UserList 无第 6 页 →「按 083 基准判『不做』」「玩法维持现状（队伍+NPC+事件，
  本端已可参与）」；v83 中文「学院」= 家族(action 26 FAMILY) `UIFamily` 已闭环。[推断]
- `diagnosis-and-plan.md:9,34-39` —「**主因是 IMG 节点数据整套没有按运行时校验，不是
  REA 反编译失败**」；`:36`「`BeiDou.exe` 内 `UIWindow.img/UserList` 1 处
  （file off `0x73C2DF`）、`UserList` 4 处；`.text` 内**无**该字符串 VA 的 4 字节
  立即数引用 ⇒ 按名查表 / 脚本拼路径，布局常量不在立即数里」；`:38-39`「布局的
  ground truth 只能是运行时 IMG 节点树，落位校准只能靠原生截图」；`:34` REA provider
  = `unreadable_output` 当前不可用。[推断]
- `diagnosis-and-plan.md:170-172` —「**按钮语义映射按节点名推断**：`BtMate`/`BtMod`/
  `BtMessage`/`BtGroupWhisper`/`GroupMod` 等与中文按钮文字的对应关系**无原生证据**，
  暂统一落到已核对的 `BuddyAdd`/`Whisper`/`FindPlayer` 包，需实机逐钮确认」。[推断]
- `r02:218-221` — 拆分模式「Angel 单体窗 + 一批 Tab/Room 子类 → 当前端多窗 +
  UIMiscShells 20+ 壳，**不是 56 类一一对应**，属结构性重写」[s9 适配/形态差异]；
  `:246`「**「校准 ≠ 改回原生」四面一致**」；`:251-259` 冲突取值表（G 键/第 3 格美术/
  底板/Messenger 依据/Tab 编号）。[推断]
- `r01:288-298` — 限制声明：`CUIUserList::OnCreate(59169B)`/`Draw(35089B)` 等 5 个
  大函数「**未逐行人工核完**」；277 函数是精选非全部 752；IMG 层 orange-wz MCP 未
  接入 Host，节点路径依赖 `.nx` 引用 + `tools/nx_inspect.py` + 正确端截图替代链。[推断]
- `docs/features/messenger-btclose-coords/README.md:16` — `CUIMessenger::SetCtrl
  0x7F34C0` `(276,6)`/`BtClose3` EA 来自 **`Maplestory95`(095 线)**，按 083 基准应作
  旁注、须 Angel 13339 复核（本端 `Basic.img` 无 BtClose3 → `close_btn()` 12×12）。[推断]
- `docs/features/guild-bbs-ui/README.md:20,27` — port-095 `CUIGuildBBS::SendLoadList/
  View/OnRegister/OnComment/OnDelete` → c2s 179（本端 s9=`BBS_OPERATION` 155，编号差
  属 095 旁注）；`:19`「IDA BeiDou.exe.i64 **无 `CUIGuildBBS` 符号（私服剥符）**」；
  「按钮落位无 WZ layout 节点：按底板量测常量，真机可微调」。[推断]
- `docs/features/s9-protocol-align/notes/s9-social.md:65-66` —「GUILD/ALLIANCE/FAMILY/
  BBS/MESSENGER 均遵循 v83『`write_byte(operation) + 各操作字段`』范式，**抽样核验，
  未对 30+ 子命令逐条开列**」—— 子命令字段层未全覆盖。[推断]
- `corewindow-angel-calibration-2026-11/README.md:295,311,320`/
  `statusbar-hud-angel-calibration-2026-11/README.md:269,308` — Q2 旧账「**交用户裁决后
  统一回写**（本面只出销案证据，不擅改 Q2 文档）」「本轮不擅自改 Q2 文档（属另一交付物），
  列入决策点 D11」。[推断]

---

## 5. 关键表述逐字引（待确认/待实测/需实机/无证据/证伪/未照搬/结构不同/明确不做）

- 「待确认」— `docs/features/trade-house-083/DESIGN.md:3`「**下一里程碑：D6A UI 出图
  （待确认 `PLAN-ui.md` 后开 Photoshop；仍不写业务代码）**」；
  `docs/features/trade-house-083/PLAN-ui.md:27`「**须用户确认本 Plan 后**才开工」。
- 「待实测/需实机」— `feature-gap-inventory-2026-11/README.md:235`「`SEND_TV` 只保留
  提示窗，**待 REA/IDA 取证后另立 Plan**」；`pending-live-tests/README.md:474-475`
  「开播态…属显式预留…**待 REA/IDA 取证后另立 Plan**」；
  `diagnosis-and-plan.md:138`「真机复拍需用户在屏前；本环境历史抓屏全黑，
  **不假装已验证**」。
- 「无证据」— `diagnosis-and-plan.md:170-172`「对应关系**无原生证据**」；
  `guild-boss-hud/README.md:28`「**无服务端脚本消费证据**，显式预留」；
  `feature-gap-inventory-2026-11/README.md:157`「**无参考截图也无 IDA 证据**」；
  `parity-gap-2026-11/README.md:100`「**证伪（2026-11-04）**: s9 两侧 opcode 与全源码
  零命中，**无数据源**。裁决改"保持壳/显式预留"」。
- 「证伪」— `social-loop-audit-2026-11/README.md:19`「此前判「壳/不做」**前提已证伪**」；
  `:6`「能证伪的标证伪」；`feature-gap-inventory-2026-11/README.md:30-32` 两假阳性
  （HIRED_MERCHANT_REQUEST 63 走 PLAYER_INTERACTION 覆盖、WORLD_MAP_DROP_REQUEST 278 已接）。
- 「未照搬」— `r01:33-34`「判定红线沿用前三面：**「校准 ≠ 改回原生」**」；
  `r02:99-100`「当前端 `TAB_POS` 是**正确端截图 oracle 实测**（比 Angel IDA 坐标更贴
  实机），按三面一致红线『校准 ≠ 改回原生』保留」；`r16:278-280`「端侧 ✅…**不**表示
  REA 已达 C」。
- 「结构不同」— `r01:16-19`「v83 原生社交经济是『少数几个大单体窗 + 一批 Tab/Room
  子类』…**而非「一个系统一个独立 UI 类」**」；`r02:218-221`「**不是 56 类一一对应**，
  属结构性重写」；`diagnosis-and-plan.md:11-12`「沿用开源 Journey v83 节点命名，与北斗
  运行时 `UIWindow.img/UserList` 节点树**不同名**」。
- 「明确不做」— `social-loop-audit-2026-11/README.md:70-75`「判定为『**不做**』」
  「**玩法维持现状**…独立招募面板非 083 原版 UI，**不做**」；
  `wedding-flow/README.md:134-135`「学院 / 远征：v95+ 系统…非 GMS083 原版能力 →
  按 083 基准判『**不做**』」；`s9-protocol-align/notes/gap-inventory.md:41`
  「**明确不做**：项目范围/默认配置不含（PIN 关闭、现金/婚恋/活动小游戏/GM 专属/
  NoOp/改名转档等），附理由。」；`trade-house-083/DESIGN.md:529-537`
  「## 10. 本版明确不做（预留）」；
  `r14-native-feature-catalog.md:247`「**明确不做（本线）：** ijl15 与
  `自定义功能实现` 全部扩展。」；`feature-gap-inventory-2026-11/README.md:350`
  `RELOG=0x1C` 注释「已裁决**明确不做**，不占枚举位」。
- 「未闭环/预留」— `diagnosis-and-plan.md:162`「**显式预留（不假装闭环）**」；
  `r02:232`「**待 r04/r05 复核**；Q2 清单已大面积销案」。

---

## 6. 社交经济常量表（常量名 → 值 → 来源行号）

### 6.A 协议 opcode / mode / 字段
| 常量 | 值 | 来源 |
|---|---|---|
| GENERAL_CHAT / MULTI_CHAT / WHISPER / SPOUSE_CHAT / MESSENGER(c2s) / PARTY_OPERATION / DENY_PARTY_REQUEST / GUILD_OPERATION / DENY_GUILD_REQUEST / NOTE_ACTION / ALLIANCE_OPERATION / DENY_ALLIANCE_REQUEST | 49 / 119 / 120 / 121 / 122 / 124 / 125 / 126 / 127 / 131 / 143 / 144 | `s9-social.md:12-24` |
| OPEN_FAMILY_PEDIGREE / OPEN_FAMILY / ADD_FAMILY / SEPARATE_FAMILY_BY_SENIOR / SEPARATE_FAMILY_BY_JUNIOR / ACCEPT_FAMILY / USE_FAMILY / CHANGE_FAMILY_MESSAGE / FAMILY_SUMMON_RESPONSE | 145 / 146 / 147 / 148 / 149 / 150 / 151 / 152 / 153 | `s9-social.md:25-33` |
| BBS_OPERATION / ENTER_MTS / USE_SOLOMON_ITEM / USE_GACHA_EXP / REMOTE_STORE / PARTY_SEARCH_START / PARTY_SEARCH_UPDATE | 155 / 156 / 157 / 158 / 59 / 222 / 223 | `s9-social.md:34-39` |
| BUDDYLIST_MODIFY(c2s) / BUDDYLIST(s2c) | 130(0x82) / 63(0x3F) 非对称 | `s9-social.md:79-85`、`feature-gap-inventory-2026-11/README.md:342` |
| MESSENGER c2s / s2c | 122(0x7A) / 313(0x139) | `r22-messenger-protocol.md:11-12` |
| MESSENGER mode | 0 Open(`int messengerid`,0=建房)/2 Leave/3 Invite(`string name`)/5 Decline(`string sender`)/6 Chat(`string text`) | `r22:18-22`、`Net/Packets/MessengerPackets.h:13,24,34,45,56`(见 `r02:123-128`) |
| MESSENGER s2c mode | 0/7 parse_look+name/channel；1 set_local_pos；2 clear_member；3 UIYesNo；4/5 note；6 append_chat | `r22:28-33` |
| PARTY_SEARCH_REGISTER | 220(0xDC) 完全无 / s9 空 stub | `feature-gap-inventory-2026-11/README.md:343`、`r23-partysearch-protocol.md:15` |
| PARTY_SEARCH_START / UPDATE | 222 `int min,max,members,jobs` / 223 空体→Stop（**与 `s9-social.md:74-76`「222 空包 / 223 write_string+byte」冲突**） | `r23:13-14` vs `s9-social.md:74-76` |
| PARTY_OPERATION(s2c) mode 4 邀请 | 62 + `"PS: "` 前缀 | `r23:16` |
| PARTY_OPERATION c2s | 124 case1 创建/2 离开/3 加入 `readInt(partyid)`/4 邀请 `readString(name)`；DENY=125 | `s9-social.md:60-62` |
| RING_ACTION / WEDDING_ACTION / WEDDING_TALK(c2s) | 137(0x89) / 138(0x8A) / 139(0x8B) | `wedding-flow/README.md:29,37,41` |
| RING_ACTION mode | 0 求婚 `string name+int boxItemId`；1 撤回；2 答复 `byte+string+int`；3 解约 `int wItemId`；5 请柬寄出 `string guest+int marriageId+byte slot`；6 请柬打开 `int slot+int invitationid`；9 心愿单 `short amount≤10 + amount×string` | `wedding-flow/README.md:30-36` |
| WEDDING_ACTION mode | 6 送礼 `short slot+int itemid+short qty`；7 取礼 `byte invType+byte itemPos`；8 退出礼台 | `wedding-flow/README.md:38-40` |
| 婚礼 s2c | MARRIAGE_REQUEST=72 / RESULT=73 / GIFT_RESULT=74 / NOTIFY_MARRIED_PARTNER_MAP_TRANSFER=75 / WEDDING_PROGRESS=320 / CEREMONY_END=321 / WEDDING_PHOTO=43 | `wedding-flow/README.md:50-55`、`r33-partyrace-wedding.md:4`、`r02:177` |
| WEDDING_TALK_MORE | 0x8B 同 handler；客户端用等价 WEDDING_TALK=139，不重复建枚举 | `wedding-flow/README.md:43-44`、`feature-gap-inventory-2026-11/README.md:346` |
| MARRIAGE_RESULT msg15 Cathedral 判据 | `!=2`（0/1 Cathedral，2 Chapel/Vegas）；尾部 short s9 恒写 1 | `wedding-flow/README.md:53` |
| WEDDING_GIFT_RESULT 分支 | 0x09/0x0A/0x0F/0x0B/0x0C/0x0E（「原实现把 int 当 mode 后第一字段，完全错位」重写） | `wedding-flow/README.md:54` |
| BBS_OPERATION / GUILD_BBS_PACKET | 155 / 59（List=mode2/Open=mode3/Post=mode0/Reply=mode4；s2c 0x06/0x07） | `pending-live-tests/README.md:196-213`、`guild-bbs-ui/README.md:21` |
| GUILD_BOSS_HEALER_MOVE / PULLEY_STATE_CHANGE | 287 body=`short y` / 288 body=`byte state` | `pending-live-tests/README.md:228`、`guild-boss-hud/README.md:16` |
| ALLIANCE_OPERATION / GUILD/ALLIANCE(s2c) | c2s 143/144；s2c 66（对接 s9）；GUILD_OPERATION 65；邀请 YesNo 0x03、公会名 0x0D | `pending-live-tests/README.md:43`、`r02:183`、`social-loop-audit-2026-11/README.md:27-30` |
| GUILD_OPERATION 子命令 | 0x05 邀请/0x07 离会 `int cid+string name`/0x08 逐出/0x0E 改职位 `int cid+byte newRank`(newRank∈(2,5])/0x10 公告 `string`(限 100 字)/0x1A 全量 | `social-loop-audit-2026-11/README.md:104-107` |
| GuildModel 增量 | 0x1A 全量 + 0x27/0x2C/0x2F/0x3D/0x40/0x44 | `social-loop-audit-2026-11/README.md:96` |
| NOTE_ACTION | 131(83) action0；body=`byte0 + mapleStr name + mapleStr msg`；s9 `getAvailableNotes()>0` 才落库 | `pending-live-tests/README.md:234-239` |
| PLAYER_INTERACTION CREATE | 123 字段序 `00 \| createType(4=514/5=503) \| mapleStr title \| 00 00 00 \| int cashItemId`；ROOM sub=4(个人店)/5(雇佣商人) | `pending-live-tests/README.md:160-161` |
| MTS_OPERATION / ENTER_MTS / MTS_OPERATION(s2c) | 0xFD(c2s 无) / 156 / 348(s2c) / 347 | `feature-gap-inventory-2026-11/README.md:347` |
| WORLD_MAP_PLAYERS / 请求 | 376 / 277 真机 `SEND 277→RECV 376` 闭环 | `pending-live-tests/README.md:14,89` |
| 婚礼道具号 | 订婚戒指盒 2240000..2240015(USE)；凭据 4031357..4031364(ETC)；请柬待寄 4031377(Chapel)/4031395(Cathedral)；请柬已收 4031406/4031407；结婚戒指 1112803/1112806/1112807/1112809(`id/10==111280`)；门票 5251000..5251003(CASH) | `wedding-flow/README.md:61-66` |

### 6.B 控件坐标 / 尺寸 / 容量
| 常量 | 值 | 来源 |
|---|---|---|
| Messenger 关闭钮 | 展开态 `(276,6)` 12×12 `BtClose3`（Maplestory95 `0x7F34C0`，095 旁注）；最小态 `(191,3)` 本端不接；旧实现 `dim.x()-21`(=274)+`BtUIClose` 32×15 越窗 | `messenger-btclose-coords/README.md:16-23`、`pending-live-tests/README.md:190-194` |
| Messenger 立绘 | scale≈0.70、脚点名牌中线上沿、`LOOK_FOOT_*` 微调、槽位最多 3 | `pending-live-tests/README.md:170-174`/`feature-gap-inventory-2026-11/README.md:186`/`r22:40` |
| GuildBBS 窗 | `backgrnd` 734×526；`backgrnd2/3` 327×288 | `guild-bbs-ui/README.md:17`、`pending-live-tests/README.md:205` |
| UIStorage v83 真身 | `UIWindow.img/Trunk` 463×318 | `feature-gap-inventory-2026-11/README.md:157` |
| UserList 三张互斥底 | `backgrnd`/`backgrnd2`/`backgrnd3` 均 312×389，不透明占比 .988；meanSAD 好友=bg3(11.59)/联盟=bg2(7.61)/家族=bg2(25.06)/拒绝=bg1(11.07)/组员=bg1(23.50) | `diagnosis-and-plan.md:50-53,78-80,103-104` |
| UserList 页签条 | 相对锚 x=0/31/62/92/145、y=0/-1/-1/0/-1；`TAB_POS[5]={(4,23),(35,22),(66,22),(96,23),(149,22)}`；Angel ctrl 2001 `(a4:1,a5:6,a6:23,a7:270)` | `r02:74-81`、`r01:157`、`r02:93` |
| UserList 行/控件尺寸 | 列表行高 18px；x=a2:281/312；friend0/4 281×27；party0/4 281×27、party1/5 279×18；blacklist0 281×27/1 281×18/2 281×6；BtAdd/BtDelete 68×17；guildmark 20×20；GuildUnion backgrnd 233×304 | `r01:148-149`、`diagnosis-and-plan.md:59-63` |
| CUIUserList tab | `this+369`=当前页签 index 0..4；控件组 2010–2020(好友)/2200–2208(组员)/2030–2041(家族)/2050,2058-2060(联盟)/2150,2151(拒绝)；`DestroyGuildButton(0..5)` 权限显隐 | `r01:142,148,160-169` |
| Angel 公会按钮坐标(部分) | BtInvite(9,316)/BtWithdraw(80,316)/BtWhere(159,316)/BtWhisper(238,316)/BtKick(159,336)/BtUp(9,336)/BtDown(80,336)/BtChat(238,336)/BtInfo(238,363)/BtGuildBBS(80,363)/BtNotice(287,80) | `r01:164` |
| 好友/拒绝计数 | 好友 0/25（当前硬编码 0/50）；拒绝 0/26；家族/联盟 0/0；家族 `[0/0]` `[1/1]`；在线 2 行/离线 5 行无 Slider | `diagnosis-and-plan.md:72-76`、`social-loop-audit-2026-11/README.md:118` |
| 好友/组员底部按钮 | 好友 10 个、家族/组员/联盟 4 列×3 行；bind_runtime_assets 绑 44 按钮 + 5 Slider（158 节点树） | `diagnosis-and-plan.md:72-76,146` |
| PartySearch 默认 | lv±15、全职业 mask；`PLAYER_UPDATE(223)` 误名→`PARTY_SEARCH_UPDATE`，`PlayerUpdatePacket::dispatch` 改 no-op | `r23:23,32` |
| GuildBoss 地图常量 | healer x=295 / yMin=-564 / yMax=-350 / rise=1 / fall=10 / `guild.img/syarenian/boss/0`；pulley x=-59 / y=-394 / `boss/1`；12s 无新包 HUD 隐藏；field[506]=yMin,field[507]=state | `guild-boss-hud/evidence/ida-healer-pulley.md:43-51,14,34-35`、`pending-live-tests/README.md:221-224` |
| PersonalShop FM | 房间 `910xxxxxx` 且 >910000000；许可证 `514*`(个人店)/`503*`(雇佣商人)；MiniRoomError 15 | `pending-live-tests/README.md:156,162` |
| 每日签到 | `UIDailyGift` 28 天 7×4 | `feature-gap-inventory-2026-11/README.md:181` |
| trade-house | `auction_anti_snipe_sec=30`、已拍栏 50 格 | `trade-house-083/DESIGN.md:548-549` |

### 6.C Angel 083 带符号 EA（+BeiDou 无符号锚点）
| 类/方法 | EA | 来源 |
|---|---|---|
| `CUIMessenger` 26 方法 | `0x84db06..0x851d3b`；`OnCreate 0x84ddb0`/`OnButtonClicked 0x84e607`/`Update 0x84e930`/`SetCtrl 0x8505e2`/`OnPacket 0x8511fc`/`OnChat 0x8515d7`/`ProcessChat 0x851d3b` | `angel-gap-feature-ea.json:3106-3235`、`feature-gap-inventory-2026-11/README.md:618-631` |
| `CUIUserList` 10 方法 | `OnCreate 0x919b73`/`Draw 0x91beab`(r01 记 `0x91bec3`)/`OnMouseButton 0x91d515`/`SetButton 0x91e073`/`SetScrollBar 0x91e1eb`/`CreateAllianceButton 0x91eba3`/`DestroyGuildButton 0x91eb52` | `angel-gap-feature-ea.json:3894-3944`、`r01:134-137` |
| `CUIGuildBBS` 17 | `0x8133a9..0x817369`（OnCreate 0x8133a9/SendLoadListRequest 0x816b69/OnGuildBBSPacket 0x816c32/FormatDate 0x817329） | `angel-gap-feature-ea.json:2706-2791` |
| 学院 `CUIFamily` 3 / `CUIFamilyChart` 8 | `0x80862b..0x8091d2` / `0x809ccc..0x80bb81` | `angel-gap-feature-ea.json:2647-2704` |
| `CUIPartySearch::StartSearch` | `0xa12073` | `angel-gap-feature-ea.json:4150-4155` |
| `CTabFriend` 12 / `CFriend` 6 | `0x90952e..` / `0xa3ff8b..` | `angel-gap-feature-ea.json:3832-3892,4157-4187` |
| `CField_Wedding` 4 / `CField_GuildBoss` 2 | `0x5814fd..0x581b78` / `Init 0x5587b3`、`BasicActionAttack 0x558b45` | `angel-gap-feature-ea.json:1432-1452,1352-1362` |
| `CCashShop` 66 / `CUICashItemGachapon` 3 | `0x468f3e..0x47c75e` / `0x999e4d..0x99aa92` | `angel-gap-feature-ea.json:170-500,4050-4065` |
| `CUIMTS` / `CAdminShopDlg` 14 / `CUIStatusBar` 41 / `CUIPartyHP::Create` / `CUIMapleTV::GetText` | `0x6fb07b..` / `0x423db7..0x427a29` / `0x8d01b2..` / `0x91f934` / `0x7f0dc9` | `angel-gap-feature-ea.json:6-76,1137-1232,661-866,3946-3951,2543-2548` |
| BeiDou 无符号 B 锚点 | UserList `sub_917546 0x60a`；PartySearch/Alliance `sub_49D0B6 0x833`+`sub_49D90D 0x66c`+`goPartySearch 0xaf2748`；ViciousHammer `sub_82A9BC 0x3bc`；VegaSpell `sub_82BB83 0x708`；CashGachapon `sub_999E4D 0x43c`；PartyRace `sub_56BD3C 0x860`；DualMobGauge `sub_56C9B7 0x866`；Delivery 叶串 `0xafcc7c` | `r16:63-73,247-254`,`r17:368-379` |
| GuildBoss BeiDou | `sub_558B0F`(OnPacket)/`sub_558C0C`/`sub_558C3B`/`sub_5587B3`/`sub_558455`/`sub_558612`；`healer 0xAF4A68`/`pulley 0xAF4A58`/`yMin 0xAF4A60`；vtable `0xAF3F3C`；StringPool 977=`x` 978=`y` | `guild-boss-hud/evidence/ida-healer-pulley.md:9-35` |
| IDB 基线 | Angel 13339=`E:\MXD\反编译IDA\v83\Angel.i64`(strings 1281)；BeiDou 13337=`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64`(strings 3889)；imagebase 均 `0x400000`；函数总数 55016/55655；EA 交叉 752/752 | `feature-gap-inventory-2026-11/README.md:612,633`、`r01:42-49,81-88` |

---

## 7. Angel 新校准应优先复核的 15 个点

1. **`PARTY_SEARCH_START(222)`/`UPDATE(223)` 字段冲突** — `r23:13-14` 记「222
   `int min,max,members,jobs` / 223 空体→Stop」，`s9-social.md:74-76` 记「222 空包 /
   223 `write_string(text)+write_byte(unk)`」。用 Angel `CUIPartySearch::StartSearch
   0xa12073` + `TabPartySearch`(7 方法) 定序，回 13337 `sub_49D0B6 0x833` 交叉，
   按实机 WZ/包序裁决，勿盲改。（`feature-gap-inventory-2026-11/README.md:225` 已列冲突取值）
2. **`PARTY_SEARCH_REGISTER(220/0xDC)` 是否原生真无构造点** — Angel 全表确认是否任何
   `COutPacket(220)`；当前端 `:343,606` 判「完全无 + s9 空 stub + 用 222/223 代理」，
   属 §9.5.3 #3 / §16 ★ 真缺 P1。
3. **Messenger 关闭钮 `(276,6)`/`BtClose3` 换 083 依据** — `messenger-btclose-coords/
   README.md:16` EA 取自 `Maplestory95 0x7F34C0`（095 线，违反基准）；改用 Angel
   `CUIMessenger::SetCtrl 0x8505e2` 复核展开态/最小态坐标与本包 `Basic.img` 无
   BtClose3 的回退（`close_btn()` 12×12）。
4. **`UIMessenger.cpp` 10 处 v95 注释污染** — `r02:143-164` 逐行列出
   `:136/:159/:168/:237/:278/:290/:410/:486/:537/:549`，用 Angel `CUIMessenger`
   (26 方法)/`CChatBalloon`(`SP_1388..1425`/`SP_1858_MESSENGER_BLINK`/
   `SP_1860_CHATBALLOON`)/`OnPacket` case 6 `OnChat` 换注释依据，**零风险、不改行为/协议**。
5. **CUIUserList tab index 0..4 vs 当前端 `UNION` 排末尾** — `r02:83-100,251-259`：
   原生连续 `this+369` 0..4 + `SetButton` 全隐藏/单组显示；当前端 `UIUserList.h:45-58`
   `UNION` 排末尾 + `tab_button()` 显式映射。判形态差异（非缺陷），Angel
   `Draw 0x91bec3`/`SetButton 0x91e073` 复核是否要收敛编号。
6. **公会按钮组 2030–2041 坐标与权限显隐** — `r01:164,168-169` 12 按钮逐字坐标 +
   `DestroyGuildButton(0/2/3)`/`(1/4/5)` 分级销毁；对当前端 `UIUserList.cpp:447
   draw_guild_tab`/12 按钮路由/`guild-tab-implementation` 逐键对表，补
   `BT_GUILD_CHANGE`(BtChange) 与 `PartyHP` 未落位（`diagnosis-and-plan.md:175`）。
7. **五页签三张互斥底归属** — `diagnosis-and-plan.md:78-80,103-104` 家族与联盟共用
   bg2（家族 meanSAD 25.06 偏高，「家族页归属尚缺一次量测」）；用 Angel `OnCreate`
   `SP_2090` 系列 + `SP_2670/2671 TAB_ENABLED/DISABLED` 复核，并跑 5 张 oracle 帧
   `layout.py`（`:105-106,164-166` 未跑）。
8. **好友/拒绝列表容量常量** — `diagnosis-and-plan.md:73-74` 好友 `0/25`（当前硬编码
   `0/50`）、拒绝 `0/26`；用 Angel `CTabFriend`(12)/`CBlackListDlg`(3)/`CFriend`(6)
   定容量上限与 `BtShowAll/BtShowOnline`（当前误接红 `BtChat`）筛选语义。
9. **GuildUnion 12 按钮语义映射** — `diagnosis-and-plan.md:170-174`「按钮语义映射按
   节点名推断：`BtMate`/`BtMod`/`BtMessage`/`BtGroupWhisper`/`GroupMod` 无原生证据」；
   用 Angel `CTabGuildAlliance`(7)/`CreateAllianceButton 0x91eba3`/`CWndAllianceGrade`(3)
   定 action 字节，替换「统一打开 UIAlliance 窗」兜底。
10. **婚礼 c2s 字段与 s2c 错位修正** — `wedding-flow/README.md:36,53-54` 心愿单
    `short amount≤10`、Cathedral `!=2`、`WEDDING_GIFT_RESULT(74)`「原实现把 int 当 mode
    后第一字段，完全错位」重写 0x09/0x0A/0x0F/0x0B/0x0C/0x0E；用 Angel
    `CField_Wedding`(4: `OnPacket 0x5814fd`/`OnWeddingProgress 0x58153d`/
    `OnWeddingCeremonyEnd 0x581b49`/`SetBlessEffect 0x581b78`) + s9 双端复核，
    联动 #32 七步实机。
11. **`WEDDING_TALK_MORE(0x8B)` 撞值** — `wedding-flow/README.md:43-44`「s9 也是
    0x8B、同 handler，客户端不重复建枚举」；`feature-gap-inventory-2026-11/
    README.md:346,486,493` 判等价 `WEDDING_TALK=139` 已落地；Angel 复核原生是否真有
    独立 0x8B 包还是同链，闭环 §14 旧账。
12. **GuildBoss pulley 交互上行 `sub_558B45`** — `guild-boss-hud/README.md:28`「未接
    ——无服务端脚本消费证据」；`ida-healer-pulley.md:34-35` `field[507]=state`、
    `sub_558B45` 交互时 state≠0 跳过发包；用 Angel `CField_GuildBoss::BasicActionAttack
    0x558b45` 定 c2s 包序，复核 287 `short y`/288 `byte state` 字段。
13. **`MTS_OPERATION(0xFD)` / ITC 拍卖原生行为面** — 当前端 `UIMTS.cpp:86-99` 仅
    5 行 Text + `update(){}` 空体（`feature-gap-inventory-2026-11/README.md:128,347`）；
    用 Angel `CUIMTS 0x6fb07b..`/`CAdminShopDlg`(14)/`CUIAdminShopWishList`(7)/
    `CUIShopScanResult`(2) 定列表/出价/搜索行为，判「s9 无后端→仅壳」是否维持明确不做。
14. **Trade/MiniRoom/PersonalShop 变体壳深度** — Angel `CTrunkDlg`(22)/
    `CTradingRoomDlg`(17)/`CMiniRoomBaseDlg`(15)/`CPersonalShopDlg`(10)/`CShopDlg`(9)/
    `CCashTradingRoomDlg`(5)（`r01:225-232`）；对当前端 `UIMiscShells.h:442,454,465,476,488`
    5 壳 + `UIMerchant`/`UIStorage`，核 `feature-gap-inventory-2026-11/README.md:160-161`
    (C33/C34「深度未达 C 层」)、`r02:232`(「待 r04/r05 复核」)。
15. **083 术语/键位与 095 剔除复核** — `r01:262-269` Angel 083 无 `Academy`/
    `ExpeditionIntermediary` 类；用 Angel 键位映射复核 `UIStateGame.cpp:356-378` 的
    F(学院 FAMILY)/G(家族 GUILD)/H(密语)/O(组队搜索)/L(预期无反应) 五死键
    （pending #35），并确认 `CUIFamily`(F,action 26) 与 `CTabGuild`(G) 两类树分离，
    杜绝把 095 导师 Academy/BossParty 当 083 缺口
    （`social-loop-audit-2026-11/README.md:20,22`、
    `feature-gap-inventory-2026-11/README.md:499-501`）。

---

**交付说明**：四态共 ~90 条，逐条带相对路径:行号 + [观察|推断|待实机]；
Wave A ❌ 11 项（r16:188-203）已由 r810:78 T14 20 壳 + r40:5 15 壳 +
`parity-gap:107` + 30+ 源码类销案；G 键第 3 页签（`UIStateGame.cpp:361-384`）
与 Angel r01/r02 闭环；r22/r23/r33 协议态、userlist 五页签落地态、
Q2 §6/§14/§16 旧账均逐条列出。全程只读，未改任何文件、未构建、未实机、未提交。
