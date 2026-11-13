# r05 — 第五功能面「宠物·召唤兽·龙」既有文档四态审计

> 来源：第二路只读 subagent `65dd0c6a-8c19-4792-a1f6-805120ac98be` 完整清单（2026-11-08 22:15），主 agent 加 §0 复核块。
> 标注：[观察]=文档/源码/伪代码逐字可读；[推断]=由观察推出；[待实机]=需真机/IMG/抓包。全部路径为仓库相对路径+行号。
> 基准 083/v83；全程只读、未改任何文件；未引 095 线符号作 083 依据。

## §0 主 agent 前置复核块（7 条）

1. **本面校准在本轮之前尚未成文**：subagent 采样时 `docs/features/pet-summon-angel-calibration-2026-11/` 只有 6 个 evidence、无 README/notes；本轮已补齐 r01/r02/r03/r04/r05 + README（本文件为 r05），该结论已过时。[观察]
2. **Angel=BeiDou 同 EA 再证**：`evidence/angel-beidou-petsummon-ea-crosscheck.json:2-10` 逐字 `"angel_funcs":968,"beidou_present":968,"beidou_sub_named":966,"beidou_real_named":2,"beidou_missing":0,"missing_eas":[],"conclusion":"same binary layout (968/968 EAs identical; differences are IDA analysis coverage, not code)"` → 六面一致（登录 327/328、HUD 547/548、核心窗口 439/441、社交 752/752、宠物 968/968）。[观察]
3. **协议权威口径固定**：`docs/features/s9-protocol-align/README.md:45`「opcode/协议以 s9 为准（R3）：原生 BeiDou.exe 连的就是 s9，故『匹配 s9』=『匹配原生』」；`docs/features/feature-gap-inventory-2026-11/README.md:63` 证据优先级 `opcode-matrix.json(2026-11-08T00:56:27, s9 head 3d5ccc6f8e) > 源码逐字 .dispatch()/emplace > 现场重算 > 真机 > Wave 销案 > gap-inventory.md(旧 head 44d0d5d9ca/client_out_sent=103)`；`:13`「协议层状态一律以 §9.5 三态为准，§9.1–§9.4 旧账已作废」。[观察]
4. **红线「校准 ≠ 改回原生」沿用**：`docs/features/login-flow-angel-calibration-2026-11/README.md:26-27`「类冲突改回 083 原生会直接断掉与 s9 服务端联机，因此一律标注『s9 适配、须裁决』，不自动判当前端为错、不盲改」；`:148`；`corewindow README:286` D2、「保持 s9 联机优先」；`social-economy README:309-310` D3/D4。本面冲突点（PARTY_SEARCH_REGISTER 220、c2s/s2c 撞值、s9 addPetInfo 字节序）一律按此处理。[观察]
5. **095 门禁**：`docs/features/feature-gap-inventory-2026-11/README.md:499-501`「只在 095 线符号出现；083 UIWindow.img 顶层 0 命中」「不作本线参照与入库源（一律取 083 适配来源）」；本面 `port-095/evidence/opcode-v95/dec/_OnPacket_CSummonedPool__QAEXJAAVCInPacket___Z.c` 仅登记排除，不作依据；主 agent 另发现 `Gameplay/MapleMap/MapPortals.h:40`「对齐 v95：CPortal::CheckKey…」1 处注释污染，处置=只换注释依据引 Angel 083、不改行为。[观察]
6. **★ 8 类场标重点核对结论（本面最硬增量）**：`map-object-audit/evidence/codebuddy-baseline.txt:173,193,199-203` 的 8 类**不是全部仍仅 FieldMarker**——该 baseline 是较旧审计读数。Summon（`baseline:188` 已列已实现 + `beidou-v17-parity/notes/r400-mts-summon.md:4` `MapSummon::spawn_summon/set_summon_stance`）、Dragon（`map-object-audit/README.md:40` G5 ✅ MapDragon 真对象 Mob/8840000.img）、Door（`:41` G6 ✅）、动态 Portal（`:42` G7 ✅）、Catch/ItemFX/CharBox（`:46` G11 ✅ BasicEff/Catch、Cash/0537、Cash/0503、ItemEff）均已被覆写；**仅 MonsterStatus(G9) 仍文本场标**（`baseline:197,205` + `map-object-audit/README.md:44`「Mob.img 2057 文件无 status/level 节点」+`:65/:73`「参照端资源同缺」显式预留）；另 G8 出生特效、G4 雾区贴图同属资源缺源显式预留。→ 与 r04（源码盘点）G5/G6/G7/G11 源码侧「真对象 + 缺帧回退占位并存」三方互证。[观察+推断]
7. **文档口径不一致待统一（列入决策点）**：`feature-gap-inventory-2026-11/README.md:220` E20 仍写「❌ 真缺·部分已修（G11/G6/G7），余者仅 FieldMarker 文本」，**未把 G5 Dragon 计入已修**；而 `map-object-audit/README.md:40` 已把 G5 标 ✅已修 → 两档对「已修集合」不一致，本面 README 统一按最新版图（G5/G6/G7/G11 已修 + 基类文本兜底仍在）取值，交 D 决策点后回写（不擅改 Q2）。另 `feature-gap-inventory-2026-11/README.md:197`「无真渲染/8 包/MapObjectHandlers.cpp:208,213,218」+`:627`「28/6/2/17/10」+`parity-gap-2026-11/README.md:79`「能看到、不能用」均为旧读数/文档债，本面 README 按 checked-in source 取值。[观察+推断]

## §1 已校准（✅ 有证据闭环）

- `docs/features/feature-gap-inventory-2026-11/README.md:388` — §9.5.5 逐字「E1/E2/E4/E5/E6/E10/E11/E12/E15/E16/E17/E18 的『❌ c2s 发不出 / 需先补 UI』判定**作废** —— 这些 opcode 现在都有 .dispatch() 发送点」[观察]
- `:322` client_out_constants/client_out_sent **148/148**（149 命名常量，PLAYER_UPDATE=223 是 PARTY_SEARCH_UPDATE 别名 `Net/OutPacket.h:225`）；`:323` client_in 306/305；`:324` s9_registered_handlers 174；`:325` 238 处 Net/Packets/*.h 构造点；`:326` 917 处 .dispatch() [观察]
- `:195` 逐字「本节已被 §9.5 覆写…2026-11-08 opcode-matrix.json 实测 client_out_sent=148，子代理 15 条抽样 + 主 agent 逐字复核证明 E1/E2/E4/E5/E6/E10/E11/E12/E15/E16/E17/E18 全部有 .dispatch() 发送点」；`:196` s2c 渲染层已接 `Net/PacketSwitch.cpp:565-599` [观察]
- `docs/features/s9-protocol-align/protocol/opcode-matrix.json:2409-2668` — c2s MOVE_PET/PET_CHAT/PET_COMMAND/PET_LOOT/PET_AUTO_POT/PET_EXCLUDE_ITEMS/MOVE_SUMMON/SUMMON_ATTACK/DAMAGE_SUMMON/BEHOLDER/MOVE_DRAGON **全部 client_sent:true / status:"MATCHED"**；`:6577-6792` s2c 宠物族全 s9_emitted:true / status:"HANDLED" [观察]
- `feature-gap-inventory-2026-11/README.md:340-341` — #7 PET_FOOD(76) ✅（`PetPackets.h:34` + `InventoryPackets.h:139` 动态分派）；#8 SPAWN_PET(98/0x62) ✅ `UIItemInventory.cpp:830 SpawnPetPacket(static_cast<int8_t>(slot), lead).dispatch();` [观察]
- `:369-382` — §9.5.4 UI↔发包三态：宠物 Pet「✓（无独立 UIPet.h，内嵌）/ ✓ / 两者都有」，证据 `UIMiscShells.cpp:6857,6870,6897,6908` / `UIItemInventory.cpp:828-831` / `PetPackets.h:19,34,62,77,93,108,123,139` / `Stage.cpp:491` / `Player.cpp:203` [观察]
- `docs/features/native-pet-restore/README.md:7-15` — 对账表：SPAWN_PET 0x62/98→CASH 500* 双击；PET_FOOD 76→USE 212*；USE_MOUNT_FOOD 77→USE 226*；MOVE_PET 167→Player::update 跟随；PET_CHAT 168/PET_COMMAND 169→UIUtilDlgExPet SoftKey T/C；PET_LOOT 170→Stage::check_drops；PET_AUTO_POT 171→SoftKey P；PET_EXCLUDE_ITEMS 172→SoftKey E 逗号 id 列表 [观察]
- `docs/features/native-summon-dragon-c2s/README.md:7-11` — MOVE_SUMMON 175→MapFieldMarkers::report_owned_c2s 跟随；SUMMON_ATTACK 176→近怪 1s CD；DAMAGE_SUMMON 177→UIAdmin `dmgsummon oid dmg mid`；BEHOLDER 178→UIAdmin `beholder oid skillId`；MOVE_DRAGON 181→本角色龙跟随 [观察]
- `docs/features/map-object-audit/README.md:40` — **G5 Dragon ✅已修**「文本场标 ⇒ MapDragon 真对象：Mob/8840000.img 帧集（stand6/move18/hit/die1 探针在案；8840001..14 与参照端同缺 ⇒ 全档同帧）；stance 奇偶=朝向（同 Mob.cpp:189）；MOVE_DRAGON 走 MovementParser 尾态驱动 move 段+400ms 余晖回 stand；die1/filpL/R 有意不接」[观察]
- `:41` G6 Door ✅已修（UseDoorPacket 原 byte+byte ⇒ s9 DoorHandler 读 int+byte 永远匹配不到门（只回 blocked6）——改 int(ownerid)+byte；1500ms 冷却）；`:42` G7 动态 Portal ✅已修（位置派生键 0x55000000|(x<<16)|y）；`:46` G11 ✅已修（2026-11-06）「Catch=BasicEff/Catch/{Success,Fail}；黑板=Cash/0537 sample；店招=Cash/0503 signboard；ItemFX=ItemEff」[观察]
- `docs/features/beidou-v17-parity/notes/r400-mts-summon.md:4` — MapSummon：`spawn_summon` / `set_summon_stance`；SpawnSummon / SummonSkill 接线 [观察]
- `docs/features/pet-summon-angel-calibration-2026-11/evidence/angel-petsummon-facts.md:673-678` — **Angel 083 一手坐实**：`CVecCtrlPet::EndUpdateActive` 内 `COutPacket::COutPacket(a1: 167);` + `EncodeBuffer(a1: (v5 + 160), a2: 8u);` + `CMovePath::Flush` + `CClientSocket::SendPacket` → MOVE_PET=167 + 8B uid 前缀 [观察]
- `evidence/angel-petsummon-facts.md:739-744` — `CUIPetEquip::OnButtonClicked` `if ( a2 == 2000 ) CUIEquip::TogglePetEquip` / `else if ( a2 > 0x7D0 && a2 <= 0x7D3 ) *(this + 360) = a2 - 2001` → 三页 id 2001/2002/2003 → index 0/1/2 [观察]
- `docs/features/npc-talk-utildlgex-align/evidence/a11m-askpet-dlgtype6-full-ui.md:10-34` — AskPet dlgType=6 布局闭环：窗 419×297、底图 UIWindow.img/UtilDlgEx_Pet/backgrnd 419×296、BtPrev(15,191) id=0x2000 / BtNext(341,191) id=0x2001 / Yes(314,272) id=1 / No/Exit id=2、nameTag 173×19 字号12、正文区 left=110 area_h=100、立绘 stand 脚点(209,218)、OK=lastMsg+1+int64 / Cancel=action=0、亲密 max=30000 饱食 max=100、ColorBox 宽 173 [观察]
- `docs/features/npc-talk-utildlgex-align/evidence/a11l-askpet-mirror-ida.md:10-16` — AskPet 入站 `7474A2`=string+byte n+n×(long uniqueId+byte pad)；AskPetAll `74775C` 多 byte flag；本机反查 `46D301(invType=5, uniqueId)`；出站 OK action=1+8 字节 uniqueId / Cancel=0 / 空列表=2；s9 NPCMoreTalkHandler lastMsg 9/10 读 long [观察]
- `docs/features/npc-talk-utildlgex-align/evidence/a11g-tip-817-821-pet-inset-ida.md:42-49` — Pet `speaker&6` inset=25 一手取证：`sub_746A8B/sub_746DFF: this[441]=(speaker&6)!=0`；`sub_9ACC20: content_left=this[441]?25:157`；Pet speaker=2→inset=25 [观察]
- `docs/features/beidou-v17-parity/notes/r01-module-map.md:87,142` — 骑宠预览 `ApplyPatches@UserInfoMount` 0x1009AA30 ✅ [观察]

## §2 未闭环（❌ 真缺 / 🟡 部分待验收）

- `feature-gap-inventory-2026-11/README.md:219` — **E19** AskPet 亲密/饱食原生位图条 ❌ 真缺·无 gage 资源 → ColorBox 占位（`pending-live-tests/README.md:263,407`）[观察]
- `:220` — **E20** Summon/Dragon/Door/Portal/MonsterStatus/Catch/ItemFX/CharBox 场标→真精灵 ❌ 真缺·部分已修（G11/G6/G7），余者仅 FieldMarker 文本（`Gameplay/Stage.h:80`；`codebuddy-baseline.txt:173,193,199-203`；`map-object-audit/README.md:40-46`）[观察]
- `:221` — **E21** 宠物装备三页穿戴/卸下（BtPet1/2/3）❌ 真缺·显式预留，`IO/UITypes/UIEquipInventory.cpp:622` 逐字「Equip/pet 下无 Slots 子节点，槽位数据待协议批次」[观察]
- `:222` — **E22** PARTY_SEARCH_REGISTER(220/0xDC) 真发包 ❌ 真缺·客户端无枚举，`UIPartySearch.cpp:246` [观察]
- `:343` — §9.5.2 #10 逐字「PARTY_SEARCH_REGISTER(0xDC/220) ❌ 完全无（Net/OutPacket.h 无 220，仅 PARTY_SEARCH_START=222@:222 / UPDATE=223@:223；UIPartySearch.cpp:246「s9 空 stub → Start 代理」，真发包 :133/:146）」；opcode-matrix.json grep PARTY_SEARCH_REGISTER 零命中 [观察]
- `:207-209` — E7 PET_LOOT(170)/E8 PET_AUTO_POT(171)/E9 PET_EXCLUDE_ITEMS(172) 🟡「有构造类、运行时 dispatch 未逐条证」（`PetPackets.h:108/123/139`）[观察]
- `:213-214` — E13 DAMAGE_SUMMON(177)/E14 BEHOLDER(178) 🟡 同上（`SummonDragonPackets.h`）[观察]
- `:301` — 逐字「**唯一真剩**：0x00DC PARTY_SEARCH_REGISTER(220) 客户端无枚举无发包…；0xAA/AB/AC/B1/B2 五条为「🟡 有构造类、运行时 dispatch 未逐条证」」[观察]
- `:388` — 真缺口收窄为「E19 亲密/饱食位图条（无 gage 资源）、E20 场标→真精灵、宠物装备三页穿戴（UIEquipInventory.cpp:622 BtPet1/2/3 预留）、PARTY_SEARCH_REGISTER 缺」[观察]
- `map-object-audit/README.md:43-44` — G8 Mob 出生特效 P2「只存储不播放…部署 Mob 命名空间无 Mob/SpecialEff.img（catalog_Mob.json 2057 文件零命中）」；G9 怪物状态图标 P2「Mob.img 2057 个文件无 status/level 节点（catalog 扫描零命中）」[观察]
- `wave4-battle-packets/README.md:24` — 逐字「BEHOLDER(178) | s9 只对 AURA_OF_BEHOLDER 结算；本端暗骑 aura 充能面未接 | 随技能补全批 |」[观察]
- `feature-gap-inventory-2026-11/README.md:197` — 源码侧「无独立 UIPet.h、无真渲染」`Gameplay/Stage.h:80 // Field placeholders (summon/mist/door/kite/dragon) until full sprites` [观察]
- `pending-live-tests/README.md:341,355,368,382` — 「**仍预留**：Pet/Mirror 完整 UI；KeyDown cancel」多处同类 [观察]
- `parity-gap-2026-11/README.md:100` — 称号窗口「**证伪（2026-11-04）**：s9 两侧 opcode 与全源码零命中，无数据源。裁决改『保持壳/显式预留』」[观察]

## §3 待实机（⏳ / 真机挂起）

- `pending-live-tests/README.md:35` — 「| 22 | NPC tip 817–821 + Pet inset A11g | ⏳ 待测 | — | 校验 tip 样式；Pet speaker=2 输入 inset=25 |」[待实机]
- `:36` — 「| 23 | ScriptMessage OnPacket + tip 820 A11h | ⏳ 待测 | s2c 304 | 0/1/2/3/4/12 回归；未接 UI -1 收口 |」[待实机]
- `:262` — 「**A11g tip + Pet inset（2026-11-05 已接线，待真机）**：校验失败弹 tip 817–821（A12 白+#355A84 描边）；Pet `speaker&6` 输入 inset=25」[待实机]
- `:400` — 「**前置**：AskPet 需脚本发 msgtype 9/10（本仓 s9 少见）；Mirror 点维度镜 NPC 9010022」[待实机]
- `:405` — 「**期望**：不再吞包 -1；脚本不挂死；现金宠等级/条在 uniqueId 命中时显示。」[待实机]
- `feature-gap-inventory-2026-11/README.md:559` — 「19/22/23/24/25 … ⏳ 全部『已接线，待真机』；BoxText 多行局限」[待实机]
- `:560` — 「29/30/31 G11 场标贴图 / 风筝 iconReward / G10 金银宝箱 ⏳ 需对应 s2c 触发场景」[待实机]
- `:535` — 阻塞 A「GM 命令自动注入失败：! 与空格被 `IO/UI.cpp:264-269` 文本映射吞掉；SendKeys/Ctrl+V/Unicode SendInput 三种方式均未产 0x31 命令包」[待实机]
- `:536,567` — A′「角色 lv1/job0/ap0、背包 0 件、无 BODY_PRESSURE/疾病 buff/回城卷/技能书/召唤袋」；「本环境历史抓屏全黑，不假装已验证」[待实机]
- `pending-live-tests/README.md:71,110` — 「快照角色仍 lv1 / job0 / ap0 / 装备 8 件，无回城卷/技能书/召唤袋」「USE_RETURN_SCROLL(85) / USE_SKILL_BOOK(82) / 召唤袋：背包空，无对应道具」[待实机]
- `native-pet-restore/README.md:26` — 验收「召唤可见且服务端在场；喂食 212*；指令/对话 SoftKey；靠近掉落可拾」[待实机]

## §4 推测（[推断]）

- `feature-gap-inventory-2026-11/README.md:224` — 逐字「**类 E 收敛结论（推断）**：整族 c2s 已 148/148 全覆盖，旧账『与原生差距最大的一整块玩法面』应改写为『**协议面已对齐，玩法深度 / 真渲染 / 宠物装备三页 / 场标精灵 / AskPet 位图条 / 部分运行时 dispatch 未逐条验收**』。真缺收敛为 E19–E22 四项 + §9.5.3 的场地伤害族。」[推断]
- `:49` — A 线/B 线口径张力（推断）：`r14:247,274` 判「ijl15 与自定义实现全部扩展=本线明确不做」，`parity-gap:113` 又纳入 §3 [推断]
- `parity-gap-2026-11/README.md:79` — 「结论：**宠物系统 = 能看到、不能用**。这是与原生差距最大的一整块玩法面。」——**旧账，已被 feature-gap:195/388 §9.5 逐字覆写作废** [推断]
- `s9-protocol-align/notes/gap-inventory.md:3,5,25,125` — 基于**旧 head 44d0d5d9ca / client_out_sent=103**「**70 个** s9 已注册 handler 客户端从不发送」「**需先补 UI 集**…宠物/好友/召唤三族优先」——协议层已过时，`feature-gap:63` 明确其优先级最低 [推断]
- `evidence/angel-beidou-petsummon-ea-crosscheck.json:2-10` + `feature-gap:612` — 968/968 同 EA ⇒「符号详细 ≠ 功能更多」（Angel 3122 修饰名/361 类 vs BeiDou 50/10/0）[推断]
- `opcode-matrix.json:2400-2668` vs `feature-gap:207-214` — matrix 判 E7/E8/E9/E13/E14 `client_sent:true`，§7 判「🟡 运行时 dispatch 未逐条证」；口径差＝「有构造类」vs「有运行时发送点」→ **r04 源码盘点已补出 .dispatch() 点销掉此口径差**（`Stage.cpp:491` / `UIMiscShells.cpp:6857,6870,6897,6908` / `UIAdmin.cpp:150,164` 仅 GM）[推断]
- `beidou-v17-parity/notes/r11-baseline-replication.md:131` — 「④charLook/itemInfo 跳过按 s9 addCharLook/addItemInfo(true) 字段序实现，装备/宠物块长度按 v83 固定假设」[推断]

## §5 「待确认/待实测/需实机/无证据/证伪/未照搬/结构不同/显式预留/不做」逐字关键句（带行号）

- `feature-gap-inventory-2026-11/README.md:207-209,213-214` — 「🟡 有构造类、运行时 dispatch 未逐条证」[观察]
- `:221` — 「❌ 真缺·显式预留」[观察]
- `:224` — 「**类 E 收敛结论（推断）**…真缺收敛为 E19–E22 四项」[推断]
- `:301` — 「**唯一真剩**…五条为「🟡 有构造类、运行时 dispatch 未逐条证」」[观察]
- `:388` — 「…判定**作废**」[观察]
- `:518` — 「Mob 情报浮窗 / Boss 分阶段 / activates_by_touch·reactInBox | 资源缺源 / 服务端裁决」[观察]
- `:523` — 「场景变更 / 任务窗「寻找 NPC 位置」 | 无任何皮肤 UI/协议线、无封包/IDA 证据，不发明客户端能力」[观察]
- `:582` — 「原 TOP20 里 #1/#3/#4/#5/#6/#7/#10…已被协议层权威重算 + 源码逐字 .dispatch() 推翻…**原 ❌ 一律降级为「玩法深度 / 真机验收」，勿再按补发包立项**」[观察]
- `:58` — ⊘「**明确不做**（既有裁决 / 证据不足显式预留 / 需双端立项 / 高版本属性）」[观察]
- `parity-gap-2026-11/README.md:64-67` — 「**2026-11-04 Wave 4 复核覆写**…**判词全表见 wave4-battle-packets/README.md——未来会话接线一律以该文档为准，勿按本节原表动工**」[观察]
- `:89` — 「MTS/拍卖窗口为壳（原生窗口在、s9 无服务端）——显式预留」[观察]
- `:100` — 「**证伪（2026-11-04）**…裁决改"保持壳/显式预留"」[观察]
- `:224` — 「不可做：接协议。s9 无协议 ⇒ 按门禁 7「不发明客户端能力」，**维持假领取并显式标注**」[观察]
- `map-object-audit/README.md:48` — 逐字「> 显式预留（不做）：Mob 情报浮窗（G8 资源缺源部分）、Boss 分阶段专属逻辑…`activates_by_touch/reactInBox`（s9 服务端在 `stats.getType(state)` 裁决，客户端无需还原该数据层）。」[观察]
- `:63,65` — 「## 未做（显式预留，不藏半成品）」；「G8 出生特效、G9 怪物状态图标、G4 雾区贴图：**参照端资源同缺**（探针实证部署树，非本端遗漏；协议/语义侧已全部消费对齐）」[观察]
- `:73` — 「G8 出生特效**显式预留**…特效来源需用户提供或按 s9 注释枚举逐项映射；G9 怪物 debuff 图标：部署 Mob.img 无 status 节点——原生图标源待决（交用户抉择）」[观察]
- `map-object-audit/evidence/codebuddy-baseline.txt:173` — 逐字「**核心缺口**是 Summon/Dragon/Door/Portal/MonsterStatus/Catch/ItemFX/CharBox 仅以 `FieldMarker` 文本呈现，无真实渲染/交互。」[观察]
- `:194-198` — 「### ⚠️ 部分实现（含 TODO / 仅场标，无真实渲染或交互）」「Dragon…仅 FieldMarker 文本 "Dragon"，**无龙精灵渲染**」「Door…无真实门对象/碰撞」「Portal…**无真实传送门对象**」「MonsterStatus…**状态未可视化到怪物**」「Catch / ItemFX / CharBox…仅场标文本，无对象化」[观察]
- `:203-204` — 「真正「缺失」的是**渲染层**，而非协议层…仅以 `FieldMarker` 文本占位（这是该审计客户端的既定设计上限，若要补全需新建对应 MapObject 子类并接入容器）」[观察]
- `wave4-battle-packets/README.md:17,18,20,22` — 「不做（无脚本触发面）」「依赖未建」「联机事件图批再说」[观察]
- `pending-live-tests/README.md:264` — 逐字「- **仍预留**：KeyDown 持续技松开取消（无 cancel 上行证据）；AskPet 亲密/饱食原生位图条（无 gage 资源，ColorBox）」[观察]
- `:326,408` — 「**仍预留**：Pet Say 正文 TEXT_X（无独立 inset 证据）」「**仍预留**：AskPet 亲密/饱食原生位图条（无 gage 资源，ColorBox）」[观察]
- `npc-talk-utildlgex-align/evidence/a11m-askpet-dlgtype6-full-ui.md:36-42` — 「亲密/饱食原生位图条 | 无 UtilDlgEx 专用 canvas；ColorBox 几何条（a11n 再扫仍无）」[观察]
- `a11l-askpet-mirror-ida.md:41-49` — 「**AskPet 原生亲密位图条 | ⏸ UtilDlgEx_Pet 仅 backgrnd，无 gage；维持 ColorBox**」[观察]
- `a11g-tip-817-821-pet-inset-ida.md:61` — 「Pet Say 正文 `TEXT_X` 未改（无独立 inset 证据；仅输入槽 9ACC20）」[观察]
- `a11j-askpet-mirror-pending.md:12,21` — 「仍预留 | dlgType=6 完整宠立绘/亲密条；现金栏 64 位 uniqueId 入库」「仍预留 | `IsAniBackgrd` 动画底；BtMain 全态打磨」[观察]
- `map-mob-drops/README.md:15,17` — 「**无真实掉落表**（显式空态，不编造数据）」「明确不做：服务端封包拉表…」（属世界地图掉落 UI，非本面）[观察]

## §6 ★ 重点核对：codebuddy-baseline.txt:173,193,199-203 的 8 类是否「仍仅 FieldMarker」

结论：**不是全部仍仅场标；codebuddy-baseline 是较旧审计基线读数，证据优先取较新现场（feature-gap:63/65-74）** [观察]
- **Summon**：`codebuddy-baseline.txt:188` 已把「Summon（真实精灵）：SPAWN_SPECIAL_MAPOBJECT / REMOVE_SPECIAL_MAPOBJECT / MOVE_SUMMON / SUMMON_ATTACK / DAMAGE_SUMMON / SUMMON_SKILL」列入**已实现**；`beidou-v17-parity/notes/r400-mts-summon.md:4` 有 `MapSummon::spawn_summon/set_summon_stance` → 真精灵已接 [观察]
- **Dragon**：`codebuddy-baseline.txt:194` 判「仅 FieldMarker 文本 "Dragon"」→ **已被 `map-object-audit/README.md:40` G5 ✅已修覆写**：MapDragon 真对象（Mob/8840000.img 帧集）[观察]
- **Door / Portal（动态）**：`codebuddy-baseline.txt:195-196` 判仅场标 → **已被 `README.md:41` G6 / `:42` G7 ✅已修覆写** [观察]
- **Catch / ItemFX / CharBox**：`codebuddy-baseline.txt:198` 判仅场标 → **已被 `README.md:46` G11 ✅已修覆写**（BasicEff/Catch、Cash/0537、Cash/0503、ItemEff）[观察]
- **MonsterStatus**：`codebuddy-baseline.txt:197,205` 判「仅生成调试场标 "Debuff#..."，状态未可视化」+ `:167`「s9 applyMonsterStatus 每状态写 short(value)+int(skill)+short(-1)，客户端只按 bits 读 short(value)+int(skill)，**未消费每状态尾随的 short(-1)**。多 buff 时可能错位」→ **仍仅文本场标**，且因 `README.md:44` G9「Mob.img 2057 文件无 status/level 节点」+ `:65/:73`「参照端资源同缺」而**显式预留** [观察]
- **残留仅场标/预留**：MonsterStatus（G9，资源缺源）、G8 出生特效（无 Mob/SpecialEff.img）、G4 雾区贴图（资源同缺）；`README.md:48/:65/:73` 均为「显式预留（不做）」[观察]
- **口径差提醒**：`feature-gap-inventory-2026-11/README.md:220` E20 仍写「❌ 真缺·部分已修（G11/G6/G7），余者仅 FieldMarker 文本」，未把 G5 Dragon 计入已修；而 `map-object-audit/README.md:40` 已把 G5 标 ✅已修 → 两档对「已修集合」不一致，新校准 README 需统一 [推断]

## §7 已知「4 个 s9 适配 vs 083 原生冲突点」当前状态

**先澄清：仓库内不存在名为「恰好 4 个 s9 适配冲突」的单一清单** [观察]。存在的是：①四面共同红线；②各面 §6 D 决策点；③`docs/features/corewindow-angel-calibration-2026-11/notes/r05-prior-corewindow-doc-audit.md:55-62`「§2 四个已知冲突的当前状态」——但那是核心窗口**文档审计深度**（①Skill/SkillEx/Macro/Enchant 🟡 ②QuestAlarm 🟡 ③ToolTip 🟡 ④CashShop 礼物/礼包深度 🟡），**不是 s9 适配冲突** [观察]。与宠物面相关、当前仍生效的 s9 适配裁决：
- `login-flow-angel-calibration-2026-11/README.md:21` — 「(a) 083 原生与 s9 适配的字节/编号不一致（level 1B↔2B、exp 4B↔8B、CHARLIST 外层结构、VAC opcode、SERVERLIST 拆包）」；`:336` D1 裁决 (a) 保持 s9 联机优先；`:337` D2 CHARLIST exp 宽度 (a) 先抓帧再定 [观察]
- `corewindow-angel-calibration-2026-11/README.md:286` — D2「083 原生语义 vs s9 联机，冲突处以谁为准？→ **保持 s9 联机优先**，Angel 仅作语义校准源，差异逐条标注「改回原生会断联机」」；`:288` D4 扩展背包 0x7D5 / 国服 0x7D3=BtGather 保留 s9/国服适配 [观察]
- `statusbar-hud-angel-calibration-2026-11/README.md:69,287` — 「编号对齐但字节/字段布局不同处，一律不视为「当前端错」」；D6 QS 几何「**保持当前端**（已用正确端截图逐像素对齐，改回破坏实机排版）」[观察]
- `social-economy-angel-calibration-2026-11/README.md:309-310` — D3 联盟双入口「保持现状（083 原生无独立窗，但 s9 已用双入口）；`校准 ≠ 改回原生`」；D4「**保持 s9 联机优先**：220 用 222/223 代理、MTS/ITC 维持「仅壳」、每日签到维持 NO-NATIVE 假态」；`:130` B1 PARTY_SEARCH_REGISTER ❌ 真「完全无」[观察]
- 宠物面本轮直接相关冲突：**PARTY_SEARCH_REGISTER(220)**（s9 空 stub、客户端用 222/223 代理，`feature-gap:343`）与 opcode 编号 c2s/s2c 撞值（见 §9.2）——两者均按「保持 s9 联机优先」处理，未改回原生 [推断]

## §8 §7 类 E 22 项在 §9.5 覆写后状态（feature-gap-inventory-2026-11/README.md:201-222, :388）

变为 ✅（旧「❌ 发不出」作废，有 .dispatch()）：**E1**(0x62 SPAWN_PET 98)、**E2**(0x4C PET_FOOD 76)、**E4**(0xA7 MOVE_PET 167)、**E5**(0xA8 PET_CHAT 168)、**E6**(0xA9 PET_COMMAND 169)、**E10**(0x4B USE_SUMMON_BAG 75)、**E11**(0xAF MOVE_SUMMON 175)、**E12**(0xB0 SUMMON_ATTACK 176)、**E15**(0xB5 MOVE_DRAGON 181)、**E16**(0x51 USE_CATCH_ITEM 81)、**E17**(0x35 USE_DEATHITEM 53)、**E18**(0x74 USE_REMOTE 116)；另 E3 USE_MOUNT_FOOD(77) 表内亦 ✅ [观察]
仍 ❌ 真缺：**E19**(AskPet 位图条)、**E20**(场标→真精灵，部分已修)、**E21**(宠物装备三页，显式预留)、**E22**(PARTY_SEARCH_REGISTER 220 完全无) [观察]
仍 🟡 未逐条证：**E7/E8/E9**(0xAA/AB/AC)、**E13/E14**(0xB1/B2)——**r04 本轮补出 dispatch 点**（E7 `Stage.cpp:491`、E8/E9 `UIMiscShells.cpp:6897,6908`、E13/E14 `UIAdmin.cpp:150,164` 仅 GM 路径）→ 判「已发但 E13/E14 仅 GM 命令，非正常玩法」。[观察+推断]
- `:582` 逐字「原 TOP20 里 #1/#3/#4/#5/#6/#7/#10…**原 ❌ 一律降级为「玩法深度 / 真机验收」，勿再按补发包立项**」；`:586` #1「宠物/召唤/龙整族 c2s 发送 + 命令 UI」✅ c2s 已 148/148 全覆盖；`:595` #10 USE_CATCH_ITEM/USE_DEATHITEM/USE_SUMMON_BAG ✅ 三条均有 .dispatch()/动态分派 [观察]

## §9 宠物召唤龙常量/协议表

### §9.1 c2s（客户端出站，权威：opcode-matrix.json + feature-gap §7）
| opcode | 值(hex/dec) | 名称 | 来源行号 |
|---|---|---|---|
| 0x35 | 53 | USE_DEATHITEM | feature-gap:217 / gap-inventory.md:57 |
| 0x4B | 75 | USE_SUMMON_BAG | opcode-matrix.json:1234 / feature-gap:210 |
| 0x4C | 76 | PET_FOOD | opcode-matrix.json:1263 / feature-gap:202 |
| 0x4D | 77 | USE_MOUNT_FOOD | opcode-matrix.json:1296 / feature-gap:203 |
| 0x51 | 81 | USE_CATCH_ITEM | opcode-matrix.json:1362 / feature-gap:216 |
| 0x62 | 98 | SPAWN_PET | opcode-matrix.json:1522 / feature-gap:201 |
| 0x74 | 116 | USE_REMOTE | opcode-matrix.json:1415 / feature-gap:218 |
| 0xA7 | 167 | MOVE_PET | opcode-matrix.json:2409 / feature-gap:204 / **Angel facts.md:673 一手坐实** |
| 0xA8 | 168 | PET_CHAT | opcode-matrix.json:2433 / feature-gap:205 |
| 0xA9 | 169 | PET_COMMAND | opcode-matrix.json:2457 / feature-gap:206 |
| 0xAA | 170 | PET_LOOT | opcode-matrix.json:2481 / feature-gap:207 |
| 0xAB | 171 | PET_AUTO_POT | opcode-matrix.json:2505 / feature-gap:208 |
| 0xAC | 172 | PET_EXCLUDE_ITEMS | opcode-matrix.json:2529 / feature-gap:209 |
| 0xAF | 175 | MOVE_SUMMON | opcode-matrix.json:2553 / feature-gap:211 |
| 0xB0 | 176 | SUMMON_ATTACK | opcode-matrix.json:2577 / feature-gap:212 |
| 0xB1 | 177 | DAMAGE_SUMMON | opcode-matrix.json:2601 / feature-gap:213 |
| 0xB2 | 178 | BEHOLDER | opcode-matrix.json:2625 / feature-gap:214 |
| 0xB5 | 181 | MOVE_DRAGON | opcode-matrix.json:2649 / feature-gap:215 |
| 0xDC | 220 | PARTY_SEARCH_REGISTER | feature-gap:222,343（**客户端无枚举/完全无**；代理 PARTY_SEARCH_START=222@OutPacket.h:222 / UPDATE=223@:223）|

### §9.2 s2c（服务端下发，权威：opcode-matrix.json:6577-6792；全 s9_emitted:true/HANDLED）
| 值 | 名称 | 行号 |
|---|---|---|
| 168 | SPAWN_PET | :6577 |
| 170 | MOVE_PET | :6592 |
| 171 | PET_CHAT | :6607 |
| 172 | PET_NAMECHANGE | :6622 |
| 173 | PET_EXCEPTION_LIST | :6637 |
| 174 | PET_COMMAND | :6652 |
| 175 | SPAWN_SPECIAL_MAPOBJECT(SpawnSummonHandler) | :6667 |
| 176 | REMOVE_SPECIAL_MAPOBJECT | :6682 |
| 177 | MOVE_SUMMON | :6697 |
| 178 | SUMMON_ATTACK | :6712 |
| 179 | DAMAGE_SUMMON | :6727 |
| 180 | SUMMON_SKILL | :6742 |
| 181 | SPAWN_DRAGON | :6757 |
| 182 | MOVE_DRAGON | :6772 |
| 183 | REMOVE_DRAGON | :6787 |
★ c2s/s2c **撞值必须分列**：MOVE_PET 167(c2s)↔170(s2c)；MOVE_SUMMON 175(c2s)↔SPAWN_SPECIAL_MAPOBJECT 175(s2c)；MOVE_DRAGON 181(c2s)↔SPAWN_DRAGON 181(s2c)。`beidou-v17-parity/notes/r302/r303/r305` 用的是 s2c 编号（`r302:7` PET_CHAT(171)/PET_NAMECHANGE(172)/PET_COMMAND(174)；`r303:7` summon 177–180、dragon 181–183；`r305:6` MOVE_PET(170)/PET_EXCEPTION_LIST(173)），与 c2s 不同口径 [观察]
其它相关：SET_TAMING_MOB_INFO opcode-matrix.json:4867 / r305:4 (48)；FAMILY_SUMMON_RESPONSE 0x99/153 `r434-family-summon-response.md:3`；CASH_PET_FOOD_RESULT :5227；BRIDLE_MOB_CATCH_FAIL :5272；SUMMON_ITEM_INAVAILABLE :6127；CATCH_MONSTER :7522 / CATCH_MONSTER_WITH_ITEM :7537；MONSTER_CARNIVAL_SUMMON :8032；AskPet s2c 304 msgtype 9/10/14 / c2s 60（pending-live-tests/README.md:406）[观察]

### §9.3 EA → 类名 → 来源（Angel 083，port 13339 / Angel.i64）
`feature-gap-inventory-2026-11/README.md:627` 逐字「| 宠物/召唤 | `CPet` / `CVecCtrlPet` / `CUIPetEquip` / `CSummoned` / `CDragon` | `0x703314..` / `0x9c406b..` / `0x800f7b..` / `0x7a3366..` / `0x4feb23..` | 28/6/2/17/10 |」；`:633` 使用约束「083 基准不变、禁止引 095 线符号；控件 id/坐标/包序先在 Angel 定位再回 13337 交叉验证，冲突以 13337 + 实机 WZ/IMG 为准；Angel 字符串缓存仅 1281（BeiDou 3889）；端口会变，每次先跑 server_health 读 idb_path」
| EA | 类/函数 | 来源 |
|---|---|---|
| 0x40e1a5 | CActionMan::LoadPetAction | angel-petsummon-ea.json:135 |
| 0x40fe2b | CActionMan::LoadSummonedAction | :141 |
| 0x4106cc | CActionMan::GetSummonedProp | :147 |
| 0x41258d | CActionMan::MergeTamingMobSprite | :165 |
| 0x4138be | CActionMan::LoadTamingMobAction | :177 |
| 0x456e35 | CAvatar::SetRidingVehicle | :489 |
| 0x456f18 | CAvatar::SetVisibleTamingMob | :501 |
| 0x4feb23 | CDragon::GetCurrentAction（10 函数组起点）| ea.json:1042 / facts.md:1184 |
| 0x5009fa | CDragon::CreateEffect | crosscheck |
| 0x4fefb1 | CDragon::Update | crosscheck |
| 0x4ff794 | CDragon::UpdateQuestInfo | facts.md:1303 |
| 0x703314 | CPet::Init `?Init@CPet@@QAEXPAVCUser@@AAVCInPacket@@@Z` | ea.json:2722 |
| 0x7076d8 | CPetTemplate::GetAutoSpeakingByEvent | ea.json:2892 |
| 0x7a3366 | CSummoned::Init | ea.json:3030 / facts.md:814 |
| 0x7acb5a | CSummonedBase::LoadAttackInfo | ea.json:3134 / facts.md:1036 |
| 0x800f7b / 0x8019f1 | CUIPetEquip::OnMouseMove / OnButtonClicked | ea.json:3312 / facts.md:739-744 |
| 0x9b7684 | CVecCtrlDragon::WorkUpdateActive | ea.json:4304 / facts.md:1324 |
| 0x9c406b | CVecCtrlPet::CreateInstance | ea.json:4386 / facts.md:673 |
| 0x9c7e30 | CVecCtrlSummoned::CreateInstance | ea.json:4424 |
其它一手常量（facts.md）：宠物 CASH 槽 type=5 / 装备槽 -100 起（:298,:6615,:6724-6726）；CPet::ChatCommand Lv≥15（:222）；CPet 亲密/等级/饱食三项 _ZtlSecureFuse 偏移（:333-335,:466-468）；CPet 三页装备槽区间 21–29 / 31–37 / 39–45（:625-628）；CSummoned::Init 字段序 Decode2/2/1/2/1/1 + Skill==0 多 1B（:814-825）；Beholder 技能 id 1320006/1320008/1320009（:961,:1001,:1008）；CDragon 帧时长 `*(v3+64)*(*(this+43)+10)/16`（:1258-1259）；CVecCtrlSummoned 移速常量 42/70（facts :1120-1125）；SVecCtrlPet 700ms 门闩（facts.md:6785、:486 500ms）；SP_ 资源节点 185 个（facts.md:6），宠物相关 `SP_4818_EFFECT_SETEFFIMG_D_PETEFFECT`、`SP_4819_...PETBACKGRND_D`(:130-131)、`SP_2870_EFFECT_BASICEFFIMG_SUMMONED`(:95)、`SP_5508_EFFECT_BASICEFFIMG_DRAGONEYES`(:141)、`SP_1590_EFFECT_SUMMONIMG_D`(:60)、`SP_380/381` 亲密↑↓(:108-110)、`SP_873..880` 骑宠/宠物装备校验文案(:164-171) [观察]

## §10 资源检索

- **正确端截图**：宠物/召唤龙**精灵正确端截图未找到**。仅有装备窗/UtilDlgEx 底板 PNG：`docs/features/beidou-v17-parity/notes/ui_layout_dump/equip_pet.png`；`docs/features/ui-v83-window-restore/evidence/w7-equip/pngs/{BtPet1_normal.png,pet.png,BtPet1_disabled.png,BtPet1_states4_vs_ref.png}`；`r15-newrefs/_nx_all/{UIWindow.img_UtilDlgEx_MultiPetEquip_backgrnd.png,UIWindow.img_UtilDlgEx_Pet_backgrnd.png}`；`r27-oracle/tmpl/{Equip_pet.png,Equip_BtPet1_normal.png}`；`w5-tooltip/pngs/{cbpet0_c.png,cbpet0_nw.png}` [观察]
- **真机抓屏（非宠物专用）**：`docs/features/pending-live-tests/evidence/{live-20261105-softkey-screenshot.png,live-20261105-r3-printscreen.png}`；且 `feature-gap:567`「本环境历史抓屏全黑，不假装已验证」[观察]
- **layout JSON**：`docs/features/**/layout*.json` 仅 4 个且**均非本面**（char-stats-ui-ps/ui/detail/v4/layout_cn.json、map-search-ui/resources/layout.json、map-search-ui/ui/search_panel_backgrnd/v3_pipeline/layout.json、map-info-panel/resources/layout.json）→ 本面 layout JSON **未找到**；`**/evidence/screens/**` → **No files found** [观察]
- **IMG 节点 dump**：**存在** `_out/summon_probe_result.txt`(452 行) / `_out/summon_probe_full.txt`(486 行)：Skill 命名空间 summon 精灵帧集 dump，`:1-4` scanning E:/MXD/BeiDou-Client-xy/Data/Skill found 76 .img；`:5-36` 3111002(311.img) summon size=4 (die/hit/stand/summoned 81x96…)；`:37-68` 3211002 同形；`:69-124` 2321003(232.img) size=5 (attack1 18 帧/die/fly/stand/summoned 171x142)；`:125-168` 3221005 size=5；`:169-234` 2121005 size=5 (attack1 19 帧/move/stand/summoned)；`:235-398` 1321007(132.img) size=10 (die/move/skill1..skill6/stand/summoned，凝视体族)；`:399-445` 5211001 size=4；`:446` `=== 2311009 NOT FOUND in 231.img`；`:448-452` Sound/Reactor.img [观察]
- **UIWindow.img/Pet\* 节点 dump**：`docs/features/beidou-v17-parity/notes/evidence/ida_uiwindow_nodes_in_exe.txt` 为全量非宠物专用；`r24/*_wz_nodes.txt` 均为 messenger/partysearch/remotegachapon/cashgachapon/family/memo/delivery/duey/storebank/vicioushammer/vegaspell → **宠物/召唤/龙专用 wz_nodes dump 未找到**；仓库内 `**/*img.xml` 仅 `docs/features/v83-hud-quest/evidence/StatusBar.img.xml` [观察]
- **Photoshop 产物**：`docs/features/pet-summon-angel-calibration-2026-11/` 下无 ui/、无 .psd → 本面 Photoshop 产物 **未找到** [观察]
- **095 线排除**：`docs/features/port-095/evidence/opcode-v95/dec/_OnPacket_CSummonedPool__QAEXJAAVCInPacket___Z.c` 存在，但按门禁**禁引作 083 依据**，仅登记排除 [观察]
- 其它 *mob* README：`docs/features/map-mob-drops/README.md:3,15,17`（世界地图掉落 UI，非宠物面）；`docs/features/field-marker-g11/README.md` grep Pet/Summon/Dragon/Mount/宠物/召唤 → **No matches found** [观察]
- 源码侧存在：`Gameplay/MapleMap/{MapSummon.h,MapSummon.cpp,MapDragon.h,MapDragon.cpp}`、`Character/Look/PetLook.{h,cpp}`、`Net/Packets/{PetPackets.h,SummonDragonPackets.h}` [观察]

## §11 Angel 新校准应优先复核的 15 个点

1. **Angel 侧 c2s 构造点全集**：facts.md grep COutPacket 仅 25 处、宠物/召唤面一手坐实仅 3 个 COutPacket 构造（:673 MOVE_PET=167、:2222 164=CModCrcKey、:4399 183=Quickslot），其余 E1/E2/E5/E6/E10/E11/E12/E15/E16/E17/E18 的 opcode 在 Angel 符号里逐个定位 `COutPacket::COutPacket(a1: N)`，与 opcode-matrix c2s 值逐一交叉 [待实机/观察]
2. **E7/E8/E9/E13/E14 五条张力**：opcode-matrix 判 client_sent:true、feature-gap:207-214 判「🟡 运行时 dispatch 未逐条证」——本轮 r04 已用源码销掉口径差（见 §8），Angel 侧保留为「出站构造稀疏、当前端由 MovementPacket/道具动态分派覆盖」[推断]
3. **CUIPetEquip 三页 vs E21**：facts.md:739-744 id 2001/2002/2003（0x7D1–0x7D3）与 feature-gap:221 `UIEquipInventory.cpp:622 BtPet1/2/3 显式预留`、a11m 底图 UIWindow.img/UtilDlgEx_MultiPetEquip 对齐；复核「2000=TogglePetEquip」是否即装备窗总开关 → r01/r03 已坐实 [观察]
4. **宠物装备槽位区间**：facts.md:625-628 `21–29 / 31–37 / 39–45` 与 :6724-6726 `GetItem(..., a4:-100-v6)` 偏移、-100 起点、type=5 CASH 槽（:298,:6615）三者一致性 [观察]
5. **CPet::Init 字段序**：facts.md:321-335 `Decode4(templateid)/DecodeStr(name)/DecodeBuffer(160,8)/_ZtlSecureFuse<short/uchar/short>` 与 s9 SpawnPetHandler(98) / a11l `46D301(invType=5, uniqueId)` 逐字段对账 → r02 已销案（`MapObjectHandlers.cpp:487-536` long uniqueId + short fh）[观察]
6. **CPet::ChatCommand Lv≥15 门槛**（facts.md:222-226）与 s9 PetChatHandler(167) / PetCommandHandler 是否有等级校验，避免把纯客户端门禁误判为协议字段 [推断]
7. **CSummoned::Init 字段序**：facts.md:814-825 `Decode2/2/1/2/1/1 + Skill==0 时多 Decode1` 与 s9 SpawnSummonHandler(=SPAWN_SPECIAL_MAPOBJECT 175) 逐字段；注意 Skill 分支 → r02 已销案（`MapObjectHandlers.cpp:1370-1419` skip_byte 0x0A filler）[观察]
8. **Beholder 三技能 id**：facts.md:961/1001/1008 `1320009/1320008/1320006` 与 wave4:24「s9 BEHOLDER(178) 只对 AURA_OF_BEHOLDER 结算；本端暗骑 aura 充能面未接」——判定 Angel 是否有对应技能常量 [观察]
9. **CDragon 帧与余晖**：facts.md:1258-1259 帧时长公式、:1283-1301 Update `updated & 0x10`、:1303-1322 UpdateQuestInfo 与 map-object-audit:40「MOVE_DRAGON 走 MovementParser 尾态 + 400ms 余晖回 stand、die1/filpL/R 有意不接」对齐 [观察]
10. **Angel=BeiDou 同 EA**（crosscheck:2-10 968/968）→ 按 feature-gap:633 约束「先在 Angel 定位再回 13337 交叉验证，冲突以 13337 + 实机 WZ/IMG 为准；字符串锚点以 13337 为准」；每轮先 server_health 读 idb_path [观察]
11. **c2s/s2c 撞值分列**（见 §9.2）：新 README 常量表必须两栏分开，切勿把 beidou notes 的 s2c 编号当成 c2s；r302/r303/r305 全部为 s2c 口径 [观察]
12. **AskPet gage 缺失**：a11m:36-42 / a11l:41-49 / pending-live-tests:264,408 一致判「UtilDlgEx_Pet 仅 backgrnd，无 gage；维持 ColorBox」——用 Angel UIWindow.img/UtilDlgEx_Pet 节点复核是否真无 gage canvas（若 Angel 有则需重判 E19）[待实机]
13. **PARTY_SEARCH_REGISTER(220)**：feature-gap:343 + social-economy:310 D4 当前裁决「保持 s9 联机优先、220 用 222/223 代理」；新校准若发现 Angel 有 220 真发包，须标「s9 适配，改回原生会断联机，交用户裁决」，不得盲改 [观察]
14. **G5 Dragon 修复态统一**：codebuddy-baseline:194「仅 FieldMarker "Dragon"」vs map-object-audit:40「MapDragon 真对象 ✅」vs feature-gap:220 E20「部分已修(G11/G6/G7)」三档口径不一致；本轮 r04/r05 用 Angel CDragon/CVecCtrlDragon EA + 源码 MapDragon.cpp:25 复核当前端确已真对象化落地 [推断]
15. **资源缺源显式预留族**：MonsterStatus(G9)/G8 出生特效/G4 雾区——map-object-audit:44,65,73 判「参照端资源同缺（Mob.img 2057 文件无 status/level、无 Mob/SpecialEff.img），非本端遗漏」；用 Angel `EFFECT/BasicEff.img`、`Summoned.img`、`Mob/SpecialEff.img` 节点复核是否真无源，避免误判为本端遗漏或误按「补发包」立项 [待实机]

## §12 交付纪律提醒

- 旧账三处已作废，勿再立项：`parity-gap:79`「宠物系统=能看到、不能用」、`gap-inventory.md:57-96`「c2s 整片不发/需先补 UI」、`feature-gap:582` 原 TOP20 #1/#3/#4/#5/#6/#7/#10 补发包判定 [观察]
- `feature-gap:13` 逐字「⚠️ 协议层状态一律以 §9.5 三态为准，§9.1–§9.4 旧账已作废」；`:63` 证据优先级见 §0.3 [观察]
- 本面 README 已在本轮补成（`docs/features/pet-summon-angel-calibration-2026-11/README.md`），§x 明确「校准 ≠ 改回原生」红线与 15 点复核结论 [推断]
- 全程只读，未修改任何产品源码 [观察]
