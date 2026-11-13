# 宠物·召唤兽·龙 — Angel 083 反编译全维校准（2026-11）

> 第五功能面（goal round 3）。以 Angel(13339) 083/v83 带符号 IDB 为权威校准源，对当前端 `MapleStory-Client` 的宠物 / 召唤兽 / 龙 / 骑宠 / 动作帧系统逐条校验「功能 / 业务 / 逻辑 / UI / 排版 / 样式 / 交互」六维的正确性、缺失、一致性。
> 前四面（登录 → 状态栏 HUD → 核心窗口 → 社交经济）已按同一方法闭环，本文沿用其口径与证据优先级。
> 基准固定 **083/v83**；全程只读取证，未改任何产品源码、未构建、未实机、未提交。

## §0 五条结论

1. **Angel 与 BeiDou 是同一份 v83 二进制的不同符号状态**：本面 `968/968` 具名函数 EA 全同、0 缺失、BeiDou 侧 966 个 `sub_*` + 2 具名 → 与前四面（登录 327/328、HUD 547/548、核心窗口 439/441、社交 752/752）合计**五面一致**。Angel 只补符号语义层（CPet 28 / CSummoned 16 / CDragon 10 / CMob 51 / CActionMan 17 方法逐字），**不补功能、不是版本更新**。[观察]
2. **不缺窗口 / 封包 / handler**：`Net/Packets/PetPackets.h` 9 个 c2s + `SummonDragonPackets.h` 5 个 c2s 全有构造点；`Net/PacketSwitch.cpp` 宠物/召唤/龙 s2c opcode + emplace 全齐；`opcode-matrix.json` 148/148 client_out 全 MATCHED。Angel `CVecCtrlPet::EndUpdateActive` 一手坐实 `COutPacket(167)` + 8B uid，与 `PetPackets.h:56` 逐字对齐 → **协议面全闭环**。[观察]
3. **结构性差异 = 原生单体对象 → 当前端拆分重写，「校准 ≠ 改回原生」**：Angel `CPet`/`CSummoned`/`CDragon` 是 `CUser`/`CField` 下独立对象 + `CVecCtrlPet/CVecCtrlSummoned/CVecCtrlDragon` 物理控制器 + `CActionMan/CActionFrame/CAnimationDisplayer` 动作帧系统；当前端拆成 `Character/Inventory/Pet.h`(数据) + `Character/Look/PetLook.h`(外观，挂 `Char.h:162 pets[3]`) + `Gameplay/MapleMap/MapSummon.h:20`/`MapDragon.h:32`(继承 `FieldMarker`)。全库**无 `UIPet.h`**、无 `CActionMan/TamingMob` 客户端源码 → **不是一一对应，但也不是功能缺失**，是架构重写。[观察+推断]
4. **真缺口三层**：① 原生独有整族缺（CActionMan 动作帧系统 / TamingMob 骑宠实体与预览 / 三页宠物装备负槽位穿戴 / 自动说话 &0x100 / 场地伤害族 5 条 c2s / PARTY_SEARCH_REGISTER(220) 跨面 / MonsterStatus(G9)+G8 出生特效资源缺源）；② 显式预留 DIY 壳（`UIEquipInventory.cpp:622` 三页穿戴、ColorBox 亲密条、`FieldMarker` 黄字兜底、`PetExceptionListHandler` 空实现、"HitN" 文本场标）；③ 文档一致性（`MapPortals.h:40` 新 095 注释污染 1 处 + Q2 `feature-gap:197/220/627`、`parity-gap:79` 旧账漂移）。[观察]
5. **判定分布（§3 共 98 条）**：✅55 / ⚠️20 / ❌16 / ⊘2 / ⏳5。s9 适配冲突（`MapObjectHandlers.cpp:526` 8B uid→int32 丢高 32 位、`:520-523`/`:1414-1417` 多字段读了就丢、c2s/s2c 撞值）一律标注「**改回原生会断联机，须交用户裁决，不得盲改**」，与四面红线一致。[观察+推断]

## §1 取证基线与三联门禁

### §1.1 IDA 端点（每轮先 server_health）

| 端口 | IDB | module | strings | 角色 |
|---|---|---|---|---|
| 13339 | `E:\MXD\反编译IDA\v83\`Angel.i64` | `` `Angel.exe `` | 1281 | 083/v83 带符号校准源（本面权威） |
| 13337 | `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64` | `BeiDou.exe` | 3889 | 083/v83 无符号排版基准 / EA 交叉验证 |
| 13338 | headless 095 | — | — | **不用**（禁引 095 线作 083 依据） |

端点 uptime 复核：13339 = 76189s、13337 = 725060s，均 healthy。`Angel.exe` 是 IDB root filename（`ida_mcp.py:317-327` `binary=ida_nalt.get_root_filename()`），磁盘无本体。

### §1.2 REA → IDA → orange-wz(IMG) 三联门禁

| 层 | 状态 | 说明 |
|---|---|---|
| REA（当前端源码） | ✅ | `xiaoye-maple-story-dev` + `reverse-engineer-anything` 已加载；checked-in source 逐字盘点（r04） |
| IDA（Angel 083 + BeiDou 交叉） | ✅ | 5 脚本串跑全 exit 0；392 函数逐字 Hex-Rays；968/968 EA 交叉 |
| orange-wz(IMG) | ⚠️ **阻塞（六面一致）** | 本 DSH Host `mcp_connector_status` 仅 `json-photoshop`；`mcp_connector_catalog(keyword=orange)`=0；`~/.cursor/mcp.json:3-6` 仅声明未注册；`img-evidence-audit.md:7` 已知 `Index 16 out of bounds` 解析缺陷 |

IMG 阻塞替代链 = 仓库 `.nx` 节点字面量 + `tools/nx_inspect.py` dump + `docs/features/ui-v83-window-restore/evidence/` PNG + 正确端截图库 `C:\Users\11036\Desktop\正确客户端截图\`（`角色装备.png` 348×305）。**本面召唤/龙/骑宠三块 UI 资源层几乎空白**（无独立截图 / layout JSON / PS 产物 / UIWindow 顶层节点），详见 r03 §7。

### §1.3 5 个采集脚本 + 6 evidence + 5 notes

脚本由 `tools/_gen_petsummon.py` 从社交面模板生成（sed 修 `ida_angel_pet-summon_`→`ida_angel_petsummon_`），`py_compile` 全 SYNTAX OK，TARGETS/FOCUS 各 53 类：

| 脚本 | 产物 | 结果 |
|---|---|---|
| `tools/ida_angel_petsummon_ea.py` | `evidence/angel-petsummon-ea.json` | 968 具名函数 / 64 类 |
| `tools/ida_angel_petsummon_decompile.py` | `evidence/angel-petsummon-decompile.json` | 392/392 OK |
| `tools/ida_angel_petsummon_normalize.py` | `angel-petsummon-decompile-full.json` / `-truncated.json` | total 392 / trunc_fixed 4 / empty 1（后补全） |
| `tools/ida_angel_petsummon_facts.py` | `evidence/angel-petsummon-facts.md` | 5819 行 / 270482 B / SP_ 185 个 |
| `tools/ida_angel_petsummon_crosscheck.py` | `evidence/angel-beidou-petsummon-ea-crosscheck.json` | 968/968 同 EA / 0 missing / 966 sub_* + 2 具名 |

逐字结论：`"same binary layout (968/968 EAs identical; differences are IDA analysis coverage, not code)"`。样本 `ABSORBITEM::Update 0x4416db→sub_4416DB(0x3c6)`、`CActionFrame::Draw 0x401de9→sub_401DE9(0x659)`。

**单独补全 `CPet::OnMove @ 0x70474d`**（decompile 首次返回空，直连 13339 重取写回，重跑 facts）：
```
void __thiscall CPet::OnMove(CPet *this, struct CInPacket *a2)
{
  CMovePath::OnMovePacket(this: (*(this + 36) != 0 ? *(this + 36) - 12 + 428 : 428), a2);
}
```
refs `?OnMovePacket@CMovePath@@QAEXAAVCInPacket@@H@Z @ 0x68b371`。

notes：`notes/r01-angel-native-petsummon-facts.md`（Angel 一手语义）、`r02-main-agent-crosscheck-cancel.md`（主 agent 逐字销案）、`r03-petsummon-img-style-evidence.md`（IMG/样式）、`r04-current-petsummon-source-inventory.md`（源码盘点）、`r05-prior-petsummon-doc-audit.md`（既有文档四态）。

## §2 口径

### §2.1 六维 × 五判定

- 六维：**A 功能 / B 业务 / C 逻辑 / D UI / E 排版 / F 样式 / G 交互**（§3 共 98 条：14/14/14/14/16/12/14）。
- 判定：**✅ 一致且正确**（Angel 语义 ↔ 当前端 source 销案）；**⚠️ 部分 / 降级 / 待裁决**（壳在、语义简化、s9 适配冲突）；**❌ 缺失 / 错误**（整族缺或显式预留）；**⊘ 结构性不做**（095 专有 / 结构性不接）；**⏳ 待实机 / 待资源 / 待展开帧**。

### §2.2 证据优先级（沿用四面）

`运行时行为 > 抓包 > 活跃资产 > 当前进程配置 > checked-in source > 生成证据 > 注释/死码`。协议层以 `s9-protocol-align/README.md:45`「opcode/协议以 s9 为准」+ `feature-gap:63` 优先级为准：`opcode-matrix.json`（`generated_at=2026-11-08T00:56:27` / `s9_git_head=3d5ccc6f8e`）> 源码 `.dispatch()` > 生成 evidence > `gap-inventory.md` 旧 head `44d0d5d9ca`/`client_out_sent=103`（作废）。

### §2.3 083 vs s9 红线 + MEMORY.md 术语

- 基准固定 **083/v83**；禁引 095 线符号/IDB（`Maplestory95.i64`、port-095 映射、`_OnPacket_CSummonedPool...` 095 反编译、ExpeditionIntermediary）作 083 依据。
- 083 术语：F 键 action26 = FAMILY =「学院」窗（UIFamily）；G 键 = GUILD =「家族」= 公会（UIUserList 第 3 页签）。判「不做」的只有 095 专有的 v95 导师 Academy 与远征 BossParty 独立面板。
- **改回原生会断 s9 联机**：`login-flow:26-27` / `corewindow:286` / `social-economy:309-310` 四面红线，本面续用 → 冲突项一律「交用户裁决，不得盲改」。

### §2.4 本面 7 处冲突取值表

| 冲突 | 旧账 | 本面取值依据 | 结论 |
|---|---|---|---|
| 宠物系统可用性 | `parity-gap:79`「能看到、不能用」 | `feature-gap:195/388` §9.5 + r04 `.dispatch()` 点 | ✅ 协议闭环，旧账作废 |
| E7/E8/E9/E13/E14 dispatch | `feature-gap:207-214` 🟡 未逐条证 | `Stage.cpp:491`/`UIMiscShells.cpp:6897,6908`/`UIAdmin.cpp:150,164` + opcode-matrix client_sent:true | ✅ 已发（E13/E14 仅 GM 路径） |
| 宠物真渲染 | `feature-gap:197`「无真渲染」 | `PetLook.cpp:38-50,61-67` + `Char.cpp:68` | ✅ 已实现（旧账实指召唤/龙场标） |
| G5 Dragon 修复态 | `codebuddy-baseline:194` 仅场标 / `feature-gap:220` 漏记 | `map-object-audit/README.md:40` + `MapDragon.cpp:25` | ✅ 真对象 + 缺帧回退占位 |
| 8 类场标是否全仅文本 | `codebuddy-baseline:173,193,199-203` | r05 §6 较新现场（Summon/Dragon/Door/Portal/Catch/ItemFX/CharBox 全已真对象化） | 7 类已修，仅 MonsterStatus(G9) 仍文本 |
| c2s/s2c 撞值 | beidou notes r302/r303/r305 用 s2c 编号 | `opcode-matrix.json` 两栏 + `Net/OutPacket.h` vs `Net/PacketSwitch.cpp` | 必须分列（MOVE_PET 167↔170、MOVE_SUMMON 175↔175、MOVE_DRAGON 181↔181） |
| 083 原生 vs s9 字节布局 | 改回原生会断联机 | 四面红线 | 保持 s9 联机优先，交用户裁决 |

## §3 六维对照总表（98 条）

> 每行：`# | 校准点 | Angel 083 一手语义（r01） | 当前端 checked-in source | 判定 | 处置 / 决策`。行号均为仓库相对路径。

### A 功能（14 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
|---|---|---|---|---|---|
| A01 | 宠物召唤/落地 SPAWN_PET | `CPet::Init 0x703314` Decode4/DecodeStr/DecodeBuffer(160,8) | `PetPackets.h:13`(c2s 98) + `MapObjectHandlers.cpp:487-536` + `PacketSwitch.cpp:565`(s2c 168) | ✅ | 销案 |
| A02 | 宠物移除 mode==0 | `CPet` 销毁路径 / `remove_pet` | `MapObjectHandlers.cpp:534 remove_pet(petindex, hunger)` | ✅ | 销案 |
| A03 | 宠物移动 MOVE_PET | `CVecCtrlPet::EndUpdateActive COutPacket(167)+EncodeBuffer(v5+160,8u)+CMovePath::Flush` | `PetPackets.h:56`(c2s 167, long 0 前缀) + `PacketSwitch.cpp:633`(s2c 170) | ✅ | 一手坐实，销案 |
| A04 | 宠物话/命令 | `CPet::OnActionCommand`/`ParseCommand`/`DoAction` | `PetPackets.h:71`(168)/`:87`(169) + `PacketSwitch.cpp:589-591` | ✅ | 销案 |
| A05 | 宠物拾取 PET_LOOT | 宠物拾取动作层 | `PetPackets.h:102`(170) + `Stage.cpp:491` dispatch | ✅ | 销案（E8 口径差销掉） |
| A06 | 自动喝药/排除物品 | 宠物技能位掩码 `GetUpgradePetSkill` | `PetPackets.h:117`(171)/`:133`(172) + `UIMiscShells.cpp:6897,6908` | ✅ | 销案（E7/E9 口径差销掉） |
| A07 | 宠物/骑宠食物 | CASH 物品使用通道 | `PetPackets.h:28`(76)/`:42`(77) | ✅ | 销案 |
| A08 | 召唤兽移动/攻击/受伤/Beholder | `CSummoned::TryDoingAttackManual(22699B)`/`ProcessAttack(19570B)`/`SetDamaged` | `SummonDragonPackets.h:13,28,60,75`(175/176/177/178) + `MapFieldMarkers.cpp:366,391` / `UIAdmin.cpp:150,164` | ⚠️ | 构造点齐，但 177/178 的 dispatch **仅 GM 路径** → D12 |
| A09 | 龙移动 + spawn/remove | `CDragon::SetMoveAction` 写回 owner+0x148 | `SummonDragonPackets.h:90`(c2s 181) + `PacketSwitch.cpp:608,610`(s2c 181/183) + `MapFieldMarkers.cpp:417` | ✅ | 销案（注意 c2s/s2c 撞值 181） |
| A10 | 召唤兽 spawn/技能 s2c handler | `CSummoned::Init 0x7a3366` Decode2/2/1/2/1/1 | `MapObjectHandlers.cpp:1370-1419` + `PacketSwitch.cpp:597,607` | ✅ | 销案（0x0A filler 已修） |
| A11 | 骑宠/驯兽/孵化/Evan 龙骑 | `SP_873/874/877/879/880/3818/4378/5454` 原生文案族 + `CAvatar::SetRidingVehicle 0x456e35`/`MergeTamingMobSprite 0x41258d` | `UICharInfo.cpp:104-108`「坐骑外观未暴露：Ride 保持禁用」+ `UISkillBook.cpp:172` | ❌ | 整族缺 TamingMob 实体 → D4 |
| A12 | PARTY_SEARCH_REGISTER(220) | 083 组队搜索注册 | `feature-gap:220`/社交面 E22「完全无」 | ❌ | 跨面真缺，维持不做 → D10 |
| A13 | 场地伤害族 5 条 c2s | 083 场地持续伤害上行 | `feature-gap §7 E 类`/wave4 场地伤害族 | ❌ | 整族缺 → D10 |
| A14 | Beholder 暗骑 aura 充能 | `SP_2870_EFFECT_BASICEFFIMG_SUMMONED` + 1321007 skill1..6 凝视体帧集 | `SummonDragonPackets.h:75` 仅发包、无 aura 充能渲染 | ❌ | 整族缺 → D10 |

### B 业务（14 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
|---|---|---|---|---|---|
| B01 | 宠物挂 CASH 物品槽 type=5 | `CPet::GetItemSlot FindCashItemSlotPosition(a2:5,...)` → `GetItem(a3:5,...)+4` | `UIItemInventory.cpp:827-832 if(prefix==500)` 双击召唤 + `Pet.h:25` | ✅ | 语义销案 |
| B02 | 宠物等级 1B | `CPet::GetLevel _ZtlSecureFuse<uchar>(ItemSlot+53,...)` | `Pet.h:41 petlevel` | ✅ | 字段对应（存储形态不同：偏移混淆 vs POD） |
| B03 | 亲密度/饱食度 | `CPet::OnValidateStat *(this+42)=_ZtlSecureFuse<short>(ItemSlot+59)` 等 | `Pet.h:42 closeness/:43 fullness` + `UIMiscShells.cpp:2019 ColorBox` | ✅ | 字段对应，条样式见 D08/F08 |
| B04 | 8B uid（long） | `CPet::Init DecodeBuffer(160,8)` | `MapObjectHandlers.cpp:521 read_long()` ✅ 但 `:526 static_cast<int32_t>(uniqueid)` 丢高 32 位 | ⚠️ | s9 适配，禁盲改 → D2 |
| B05 | 宠物名/改名 PET_NAMECHANGE | `CPet::OnNameChanged` / 名字匹配 `ZXString::operator==(a1:*(this+136)+4)` | `PacketSwitch.cpp:236,590` + `UIMiscShells` 软键 `"Pet name"` | ✅ | 销案 |
| B06 | 动作表 28B/条 + 等级门控 | `CPet::OnActionCommand result = 28*Decode1; if(Level<*v6 \|\| v6[1]<Level) return;` | `PetLook.h:44-49 stancebyvalue(value/2)` + 8 stance | ✅ | 语义层对应（简化映射，非一一） |
| B07 | 说话 500 冷却 + 名字匹配 | `CPet::ParseCommand *(this+184)<500` + TrimRight/Left | `MapObjectHandlers.cpp:1206 PetChatHandler` `(void)act;` `:1222 character->speak(text)` | ⚠️ | 话当角色气泡，冷却/名字门控未接 → D12 |
| B08 | 自动说话 &0x100 + 气泡 5000ms/样式1002 | `CPet::AutoSpeakingByEvent if((GetUpgradePetSkill(this)&0x100)!=0){...MakeBalloon(...5000,1002...)}` | 当前端无 `AutoSpeakingByEvent` 对应 | ❌ | 整族缺 → D4 |
| B09 | ChatCommand Lv≥15 门禁 | `CPet::ChatCommand` / `CursedChatCommand` | 当前端未见 Lv 门禁 | ❌ | 待复核是否纯客户端门禁 → D4 |
| B10 | 三页装备槽区间 21–29/31–37/39–45 | `CUIPetEquip::OnMouseMove GetItem(a3:1,a4:-v5)` / `a4:-100-v6` | `UIEquipInventory.cpp:622` 逐字「预留：三页宠物装备槽的穿戴/卸下…槽位数据待协议批次」 | ❌ | 真缺 E21 → D3 |
| B11 | 召唤兽随技能帧集 | `CSummoned::Init CSkillInfo::GetSkill(dword_BE78DC,...)` | `MapSummon.cpp:34-39 nl::nx::Skill[skill_namespace(skill_id)+".img"]["skill"][strid]["summon"]` | ✅ | 语义销案 |
| B12 | 龙与任务联动 | `CDragon::UpdateQuestInfo` 与 `CQuestMan`/`CharacterData` 联动 | 当前端 `MapDragon` 未接 CQuestMan | ⏳ | 待查任务数据源 → D13 |
| B13 | 龙 oid=owner cid / 键 cid^0x40000000 | `CDragon` owner 语义 | `MapDragon.h:16` 逐字「龙 oid 语义 = owner cid…原生键 cid^0x40000000」 | ✅ | 语义层销案 |
| B14 | MonsterStatus(G9)/G8 出生特效 | 083 Mob.img status/level 节点 | `map-object-audit/README.md:44/:65/:73` 2057 文件无 status/level 节点、参照端资源同缺、显式预留 | ❌ | 资源缺源 → D10 |

### C 逻辑（14 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
|---|---|---|---|---|---|
| C01 | SpawnPet 字段序 | `CPet::Init` templateid/name/8B uid/pos/stance/fh | `MapObjectHandlers.cpp:505-526` 逐字注释「修前 uniqueId 用 int+skip(4) 凑宽度、fh 用 int 吞掉 nameTag…按原类型读」 | ✅ | 已修，销案 |
| C02 | SpawnSummon 0x0A filler | `CSummoned::Init` Skill==0 多 Decode1 | `MapObjectHandlers.cpp:1390-1410`「修前把 0x0A 当 level 读、pos 从错位字节起读 ⇒ 等级恒 10、落位错 1 字节」 | ✅ | 已修，销案 |
| C03 | 6 条 s2c handler 注册 | — | `PacketSwitch.cpp:565,589,590,591,597,607,608,610,633` 全 emplace | ✅ | 销案 |
| C04 | nameTag/balloon 丢弃 | Angel 读 bool nameTag/balloon 用于气泡 | `MapObjectHandlers.cpp:520-523 (void)nametag;(void)balloon;` | ⚠️ | s9 适配，禁盲改 → D2 |
| C05 | stance/movetype/attacking 丢弃 | `CSummoned::Init` 消费 stance/moveType | `MapObjectHandlers.cpp:1414-1417 (void)stance;(void)movetype;(void)attacking;` | ⚠️ | 注释「包驱动而非本地 AI ⇒ 无需消费」→ D2 |
| C06 | c2s/s2c 撞值分列 | 083 原生两栏独立 | `opcode-matrix.json` 两栏 + `Net/OutPacket.h` vs `Net/PacketSwitch.cpp` | ✅ | 口径已建立，保持分列 → D5 |
| C07 | PetExceptionListHandler 空实现 | `CPet::OnLoadExceptionList` | `MapObjectHandlers.cpp:1772-1780` 空实现 | ⚠️ | 壳 → D12 |
| C08 | PetChat act 丢弃 | `CPet` action 分类 | `MapObjectHandlers.cpp:1206/:1222 (void)act;` + `character->speak` | ⚠️ | 壳 → D12 |
| C09 | 召唤攻击 "HitN" 文本场标 | `CSummoned::ProcessAttack` 伤害帧 | `MapObjectHandlers.cpp:1616/:1636-1643 "Hit"+std::to_string(total)` | ⚠️ | 壳 → D12 |
| C10 | MoveSummon 丢 movement 段 | `CMovePath::OnMovePacket` 完整移动 | `MapObjectHandlers.cpp:1601-1609` 只读位置 | ⚠️ | 壳 → D2 |
| C11 | 缺帧回退 FieldMarker 黄字 | `CAnimationDisplayer` 帧集 | `MapSummon.cpp:162`/`MapDragon.cpp:47,82` 全退 `FieldMarker::draw` | ⚠️ | 降级占位，非整族缺 → D13 |
| C12 | SUMMON_SKILL newStance 丢弃 | `CSummoned::ProcessAction` 30ms tick + 20B/动作 | `MapSummon.cpp:138-146 (void)s;`「帧集只有 stand/hit/die/summoned…无帧可播，保持当前状态」 | ⚠️ | 显式说明的降级 → D13 |
| C13 | 龙 die1(90帧)/filpL/filpR 不接 | `CDragon` 死亡/翻转段 | `MapDragon.h:16-19`「s9 removeDragon 仅在登出/换图/删号…无伤害上行 ⇒ REMOVE ≠ 死亡」 | ⊘ | 结构性不做（s9 语义），列说明 |
| C14 | 8 类场标 7 已修仅 G9 文本 | 083 场标对象族 | `map-object-audit/README.md:40-46` G5/G6/G7/G11 ✅ + r05 §6 | ✅ | 口径统一，Q2 E20 待回写 → D11 |

### D UI（14 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
|---|---|---|---|---|---|
| D01 | CUIPetEquip 三页 ctrl | `OnButtonClicked` `a2==2000→TogglePetEquip; 2001/2002/2003→*(this+360)=a2-2001` | `UIEquipInventory.cpp:181 pet_tabs[3]`/`BtPet1/2/3` + `:609-623 change_pet_tab` | ✅ | 壳对齐（2000=Toggle / 三页索引） |
| D02 | 穿戴=装备窗嵌入子面板 | `CUIEquip::OnButtonClicked case 3002u: TogglePetEquip(this)`（`corewindow…decompile-full.json:139,141`） | 全库无 `UIPet.h`；`UIEquipInventory.cpp` 内嵌 pet 面板 | ✅ | 结构对应，非缺失 |
| D03 | 三页切 TwoSpriteButton 美术态 | `CWnd::InvalidateRect(this,0)` | `UIEquipInventory.cpp:60-86 try_pet_tab_btn`/`:610-623` PRESSED/NORMAL | ✅ | 销案 |
| D04 | 穿戴/卸下逻辑 | `GetItem(a3:1,a4:-100-v6)` 负槽位取物 | `UIEquipInventory.cpp:622` 显式预留 | ❌ | 真缺 E21 → D3 |
| D05 | Equip/pet 177×181 底图 + 13 槽名烤进贴图 | `SP_4819_EFFECT_SETEFFIMG_D_PETBACKGRND_D` | `UIEquipInventory.cpp:162 pet_backgrnd=Texture(Equip["pet"])` + `:213-215` | ✅ | 销案 |
| D06 | 不可用槽逐像素改色 | 原生无第二张 disabled 美术 | `UIEquipInventory.cpp:219-221` + `asset-node-inventory.md` 调色公式 `R'=snap17(0.75R+68.25)` | ✅ | 销案 |
| D07 | AskPet 419×297 布局 | 083 UtilDlgEx_Pet | `UIMiscShells.h:271-288` + `a11m-askpet-dlgtype6-full-ui.md:10-34` | ✅ | 布局闭环 |
| D08 | 亲密/饱食 gage 位图 | 083 原生条资源 | `UIMiscShells.cpp:2019/:2029 ColorBox(BAR_W,BAR_H,DARKGREY,0.85f)` 降级 | ❌ | E19 无资源 → D9 |
| D09 | UtilDlgEx_MultiPetEquip 419×219 | 083 多宠改名窗 | `UIMiscShells.cpp:6922 load_window_bg("UtilDlgEx_MultiPetEquip")` | ✅ | 销案 |
| D10 | 骑宠按钮 BtRide | `CAvatar::SetVisibleTamingMob`/TamingMob 预览 | `UICharInfo.cpp:90/93` + `:104-108`「Ride 保持禁用」+ `UISkillBook.cpp:172` | ⚠️ | 壳在渲染缺 → D4 |
| D11 | 宠物名 Tag | `CPet::OnNameChanged` | `PetLook.cpp:35 Text(...Background::NAMETAG,name)` + `NameTag.cpp:133-134 Group::PET` | ✅ | 销案 |
| D12 | UIMapMobPanel 怪物面板 | `CUIMapMobPanel` | `IO/UITypes/UIMapMobPanel.{h,cpp}` 存在 | ✅ | 存在性销案 |
| D13 | UIAskPet/UtilDlgExPet/MultiPet 软键壳 | 083 UtilDlgEx 软键 | `UIMiscShells.cpp:6826-6915` 硬编码 `"Pet name"/"cmd 0-9"/"pet chat"/"exclude ids"/"slot,itemId"` | ✅ | 销案 |
| D14 | 095 专有 v95 Academy/BossParty 面板 | 083 无 | MEMORY.md 硬约束判「不做」 | ⊘ | 结构性不做 |

### E 排版（16 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
|---|---|---|---|---|---|
| E01 | pet 面板定位 | 原生 pet 子面板压边 | `UIEquipInventory.cpp:169-171`「pet[177x181] 压住窗口右边 3px，底边与窗口底边齐平」`pet_panel_pos=(right-3,bottom-height)` | ✅ | 销案 |
| E02 | BtPetEquipShow 坐标 | — | `UIEquipInventory.cpp:173-174 (right-73,bottom-24)` | ✅ | 销案 |
| E03 | BtPetEquipHide 坐标 | — | `UIEquipInventory.cpp:187-188 (right+152,bottom-22)` | ✅ | 销案 |
| E04 | 三页按钮 (right+7+34i,bottom-30) | 083 三页间距 | `UIEquipInventory.cpp:183-185` 间距 34（`w7-equip/coords.md` 模板匹配） | ✅ | 销案 |
| E05 | BtPet1/2/3 四态 origin (15,13) | — | `UIEquipInventory.cpp:176-180` 更正旧注释 (15,0)，见 `r27-b3-equip/README.md §1` | ⏳ | **只记不改**，待原生展开帧 → D6 |
| E06 | 三页 y 可能整体高 13px | — | `UIEquipInventory.cpp:176-180`「按 (15,0) 推的现有 y 可能整体高 13px…列入用户取证」 | ⏳ | 待展开帧，禁盲调 → D6 |
| E07 | 4 格改色命中率 | — | `r03 §0` 71.27/77.19/34.67/71.08% | ⚠️ | 34.67% 格偏低待复核 → D7 |
| E08 | 槽位 x∈{5,38,71,104,137} y=32+33k | 083 槽位栅格 | `asset-node-inventory.md:43-52` | ✅ | 销案 |
| E09 | AskPet WND_W=419/WND_H=297 | 083 UtilDlgEx_Pet 419×296 | `UIMiscShells.h:271-288` | ✅ | 销案 |
| E10 | PET_LOOK(209,218)/BTN 组坐标 | — | `UIMiscShells.h:271-288 PET_LOOK_X/Y=209/218; BTN_PREV=15/191; BTN_NEXT=341/191; BTN_OK=314; BTN_CANCEL=363; BTN_EXIT=9; BTN_BOTTOM_Y=272` | ✅ | 销案 |
| E11 | iconpositions TAMEDMOB(5,230)/SADDLE(38,230) | 083 骑宠图标位 | `UIEquipInventory.cpp:131-132` | ✅ | 销案 |
| E12 | dimension = pet_panel_open ? expanded : base | — | `UIEquipInventory.cpp:606` | ✅ | 销案 |
| E13 | pet_panel_open = open && pet_backgrnd.is_valid() | — | `UIEquipInventory.cpp:592` | ✅ | 销案 |
| E14 | 14 张 pet PNG 无配套 layout JSON | — | `r03 §3`（`equip_pet.png 177x181`/`BtPet1_*`/`UIWindow.img_UtilDlgEx_*` 全无 layout） | ⏳ | 无尺，待实机/IMG → D8 |
| E15 | 召唤/龙/骑宠 UI 无独立截图/layout/PS/顶层节点 | — | `r03 §7` `**/*[Ss]ummon*.png`/`*Dragon*.png`/`*Mount*.png` 全 No files found | ❌ | 资源层空白 → D8 |
| E16 | `角色装备.png` 348×305 宠物面板未展开 3 FAIL | — | `UIEquipInventory.cpp:160` 基准帧 + `r27-b3 README:598-601` 3 条 FAIL 全在宠物面板 | ⏳ | 待展开帧 → D6 |

### F 样式（12 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
|---|---|---|---|---|---|
| F01 | Pet 帧集 8 stance + warp | `SP_4818_EFFECT_SETEFFIMG_D_PETEFFECT` + `CPet::LoadLayer(8082B)`/`PrepareActionLayer(4731B)` | `PetLook.cpp:40-50` move/stand0/jump/alert/prone/fly/hang + `Effect["PetEff.img"]` warp | ✅ | 语义销案 |
| F02 | Skill summon 帧集 stand/hit/die/summoned | `SP_2870_EFFECT_BASICEFFIMG_SUMMONED` | `MapSummon.cpp:34-52` + `_out/summon_probe_result.txt`(452 行)/`_out/summon_probe_full.txt`(486 行) | ✅ | 销案 |
| F03 | 龙 Mob/8840000.img 帧集 | `SP_5508_EFFECT_BASICEFFIMG_DRAGONEYES` | `MapDragon.cpp:25 nl::nx::Mob["8840000.img"]` + `MapDragon.h:8` stand(6)/move(18)/hit(1)/die1(90)/attack1-5/skill1-7/filpL/R/summon(41) | ✅ | 资源在，帧集清单销案 |
| F04 | NameTag 背景 | `CChatBalloon`/NameTag | `PetLook.cpp:35 Background::NAMETAG` + `NameTag.h:29-50 Group{NONE,MEDAL,PET} MIN_ID=3 MAX_ID=254` | ✅ | 销案 |
| F05 | MapleButton 四态 | 083 按钮四态 | `MapleButton.cpp:55-71 src["normal"]/["pressed"]["0"]/["mouseOver"]["0"]/["disabled"]["0"]` + `Button.h:28-36` | ✅ | 销案 |
| F06 | Icon 32×32 | 083 图标格 | `Icon.h:105-106 cell_width=32;cell_height=32` | ✅ | 销案 |
| F07 | EquipTooltip 资源 | 083 ToolTip/Equip | `EquipTooltip.cpp:53 UIWindow.img/ToolTip/Equip` + `:62 IconBase/0` | ✅ | 销案 |
| F08 | locale 宠物文案 | 083 原生文案族 | `zh-CN.ini:349 charinfo.pet_wish` + `:610-612 utildlgexmultipet.*` = 4 stub 键 ×2 语言、zh-CN 全英文未译 | ❌ | stub → D9 |
| F09 | Configuration 落盘设置 | 083 自动喝药/宠物名/骑宠/排除 | `Configuration.h:1055-1064` 仅 `ui.autopot.hp_item/mp_item`；全文 1088 行 grep pet/summon/dragon/mount/taming 仅 `:1055` 一条注释（无 `pet_auto_pot` 字面量） | ⚠️ | 覆盖稀薄 → D9 |
| F10 | 龙 die1(90帧)/filpL/R/skill1-7 未接 | `CDragon` 动作段 | `MapDragon.h:8,16-19` 资源在、动画不接 | ⚠️ | 待裁决 → D13 |
| F11 | 召唤兽浮动/游走帧缺 | `CSummoned::ProcessAction` | `MapSummon.cpp:138-146 (void)s;`「本数据帧集无游走/浮动帧，位置由包驱动」 | ⚠️ | 数据缺源 → D13 |
| F12 | orange-wz(IMG) 门禁阻塞 | — | `r03 §6` Host 未注册 + `img-evidence-audit.md:7` `Index 16 out of bounds` / `codebuddy-transcript:37` `Index 16 out of bounds for length 4` | ⚠️ | 六面一致阻塞，替代链取证 → D14 |

### G 交互（14 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
|---|---|---|---|---|---|
| G01 | 双击 CASH 500* 槽召唤 | `FindCashItemSlotPosition(a2:5)` | `UIItemInventory.cpp:827-832 if(prefix==500)` | ✅ | 销案 |
| G02 | 宠物跟随本地 AI | `CVecCtrlPet::WorkUpdateActive(14018B)` | `PetLook.cpp:69-153 PETWALKFORCE=0.35/PETFLYFORCE=0.2/distance>150/x 差>50 hforce+flip/NOGRAVITY` | ✅ | 语义层对应（简化物理） |
| G03 | 宠物话/命令经聊天框 | `CPet::ParseCommand` TrimRight/Left + 名字匹配 | `PetPackets.h:71,87` + `UIMiscShells.cpp:6826-6915` 软键 | ✅ | 销案 |
| G04 | 宠物自动拾取 | 宠物拾取动作 | `Stage.cpp:491` + `PetPackets.h:102` | ✅ | 销案 |
| G05 | 三页切换交互 | `CWnd::InvalidateRect` | `UIEquipInventory.cpp:609-623` 只切美术态、`:622` 槽位数据缺 | ⚠️ | 壳 → D3 |
| G06 | 骑乘交互 | `CAvatar::SetRidingVehicle`/TamingMob | `UICharInfo.cpp:104-108` Ride 禁用 | ❌ | 渲染缺 → D4 |
| G07 | 召唤兽位置包驱动 | Angel 同（非本地 AI） | `MapObjectHandlers.cpp:1414` 注释「客户端召唤兽位置由 MOVE_SUMMON 包驱动（原生同样是包驱动而非本地 AI ⇒ 这里无需消费）」 | ✅ | 语义销案 |
| G08 | 龙 apply_move + move_ttl 回 stand | `CDragon::SetMoveAction`/动作帧时长 `*(v3+64)*(*(this+43)+10)/16` | `MapDragon.h:50 int32_t move_ttl = 0;` + `map-object-audit:40` 400ms 回 stand | ⚠️ | 余晖/帧时长待复核 → D13 |
| G09 | 宠物命令 500 冷却 + 名字门控 | `CPet::ParseCommand *(this+184)<500` | 当前端未接 | ❌ | 整族缺 → D4 |
| G10 | 动作表 28B/条 ≥9 扩展 | `CPet::OnActionCommand 28*Decode1` + `DoAction if(v8>=9){...sub_70D6E8(v8-9)}` | `PetLook.h:31-42` 8 stance + `stancebyvalue` | ✅ | 简化映射销案 |
| G11 | 自动说话 &0x100 交互 | `CPet::AutoSpeakingByEvent` | 当前端无 | ❌ | 整族缺 → D4 |
| G12 | 召唤攻击 s2c "HitN" 文本场标 | `CSummoned::ProcessAttack` | `MapObjectHandlers.cpp:1616-1643` | ⚠️ | 壳 → D12 |
| G13 | 宠物改名交互 | `CPet::OnNameChanged` | `PacketSwitch.cpp:590` + `UIMiscShells` 软键 `"Pet name"` | ✅ | 销案 |
| G14 | 6 频道快捷 change_target 空 stub | `CChatHelper`/频道切换 | `IO/UITypes/UIChatBar.h:123 change_target(int32_t) {}` | ❌ | 跨面共性真缺 → D10 |

**§3 判定汇总**：✅55 / ⚠️20 / ❌16 / ⊘2 / ⏳5 = 98 条（行级状态机复核见 §9.2）。

## §4 Angel 083 十条硬发现（r01 一手语义）

1. **宠物 = CASH 物品槽 a2=5**：`CPet::GetItemSlot` 逐字 `CharacterData::FindCashItemSlotPosition(this: v3, a2: 5, a3: *(this + 20))` → `GetItem(..., a3: 5, a4: CashItemSlotPosition) + 4`；等级 `itemslot+53` 1B、亲密度 `itemslot+59` short，全走 `_ZtlSecureFuse` 混淆读取。
2. **CUIPetEquip 是装备窗嵌入子面板，非独立浮窗**：`OnButtonClicked` `if(a2==2000){CUIEquip::TogglePetEquip(dword_BED64C);} else if(a2>0x7D0 && a2<=0x7D3){*(this+360)=a2-2001; InvalidateRect}` → ctrl 2000/2001/2002/2003；`OnMouseMove GetItem(a3:1,a4:-v5)` / `a4:-100-v6` → **宠物装备槽 = 装备栏 a3=1 的负槽位**。
3. **动作表 28B/条 + 等级上下限**：`CPet::OnActionCommand result = 28*Decode1(a2); v6=result+*(*(this+34)+68); if(Level<*v6 || v6[1]<Level) return result;`；`DoAction` `IsOnLadder/IsOnRope→a3=8`、`v8>=9` 走 `sub_70D6E8(v8-9)` 扩展表。
4. **命令 500 冷却 + 名字匹配**：`CPet::ParseCommand` 门槛 `v3>-1 || *(this+180)!=0 || *(this+184)<500 || (*(**(this+148)+12))==0 || *(*v5+1388)>0` + TrimRight/Left + `ZXString::operator==(a1:*(this+136)+4)`。
5. **自动说话技能掩码**：`CPet::AutoSpeakingByEvent if((GetUpgradePetSkill(this)&0x100)!=0){...CAnimationDisplayer::LoadLayer(this+144); CChatBalloon::MakeBalloon(this+20,...,5000,1002,...)}` → 气泡 5000ms / 样式 1002。
6. **骑宠/驯兽/孵化 083 已成族**：`CDraggableItem` 内 `SP_3818`(鞍具骑乘)、`SP_873/874/877/879/880`(驯兽/鞍具/存活/可穿校验)、`SP_4378`(皮格米蛋孵化器)、`SP_5454`(仅 Evan 可穿)、`SP_4266`(婚礼戒指单戴) 等 13 条成组。
7. **CSummoned::Init 字段序**：`Decode2/2/1/2/1/1` + `CSkillInfo::GetSkill(dword_BE78DC, *(this+45))` + `CWvsPhysicalSpace2D::GetFoothold` + `if(Skill==0) return 0;` + 尾部 `Decode1`；`ProcessAction` 30ms tick + 每动作 20 字节表 `*(this+61)+20*CurrentAction`。
8. **CDragon::SetMoveAction 写回 owner+0x148**：`v5=_ZtlSecureTear<long>(a2,this+140); *(*(this+55)!=0 ? *(this+55)-12+0x148 : 328)=result;`；`UpdateQuestInfo` 与 `CQuestMan`/`CharacterData` 任务联动。
9. **CPet::OnMove 委托 CMovePath**：逐字 `CMovePath::OnMovePacket(this: (*(this+36)!=0 ? *(this+36)-12+428 : 428), a2);`（`@ 0x70474d`，refs `0x68b371`）。移动端为包驱动。
10. **968/968 与五面 EA 一致性**：本面 `angel-beidou-petsummon-ea-crosscheck.json` + 前四面合计 → Angel 只补「符号语义层」，不补功能/版本。

## §5 四态（销案 / 推翻 / 新增 / 遗留）

- **✅ 销案（旧账作废）**：`parity-gap:79`「宠物系统 = 能看到、不能用」→ 9+5 个 c2s 构造点、6 条 s2c handler、148/148 client_out 全 MATCHED；`feature-gap:197`「无真渲染」→ 宠物本体 `PetLook.cpp` 真帧集 + 物理已实现（旧账实指召唤/龙场标）；E7/E8/E9/E13/E14 dispatch 口径差由 r04 补出 `.dispatch()` 点销掉；8 类场标 7 类已真对象化（仅 G9 仍文本）；`gap-inventory.md:3,5,25,125` 旧 head / 70 handler 过时。
- **❌ 推翻 / 真缺**：E19 AskPet gage 无资源（ColorBox）、E21 `UIEquipInventory.cpp:622` 三页穿戴、E22 PARTY_SEARCH_REGISTER(220) 完全无；CActionMan/TamingMob 客户端源码 0 命中；自动说话 &0x100 / Lv≥15 门禁 / 500 冷却；场地伤害族 5 条；MonsterStatus(G9)+G8 出生特效资源缺源。
- **➕ 新增**：`MapPortals.h:40` 095 注释污染 1 处（只换依据，不改行为）；两处 s9 适配字节丢弃（`:526` 8B uid→int32、`:520-523` nameTag/balloon、`:1414-1417` stance/movetype/attacking）；c2s/s2c 撞值分列口径；`feature-gap:220` E20 漏记 G5 的口径差。
- **⏳ 遗留 / 待实机**：`pending-live-tests:35/36/262/400/405` 等 22/23 号；「本环境历史抓屏全黑」`feature-gap:567`；`live-a53de1c8-equip-pet.png` 宠物面板未展开 3 FAIL；orange-wz 未接入导致 IMG 层阻塞（六面一致）。

## §6 十四个决策点（D1–D14，交用户裁决）

| # | 决策点 | 推荐默认 | 依据 |
|---|---|---|---|
| D1 | `MapPortals.h:40` 095 注释污染 | **做**：只换注释依据引 Angel 083 `CPortal`，不碰通道逻辑 | §4 新增 / r02 §5 |
| D2 | 两处 s9 字节丢弃（`:526` 8B uid→int32、`:520-523`/`:1414-1417` 字段读了就丢） | **保持 s9 联机优先**，改回原生会断联机，交裁决 | 四面红线 |
| D3 | 三页宠物装备负槽位穿戴（区间 21–29/31–37/39–45 + -100 偏移） | 先补「槽位数据 ↔ 协议批次」对照，接线**待裁决** | `UIEquipInventory.cpp:622` |
| D4 | CActionMan 动作帧 / TamingMob 骑宠实体 / 自动说话 / Lv≥15 / 500 冷却整族 | 维持不做（或单列补齐计划），**待裁决** | §4 硬发现 / A11/B08/B09/G06/G09/G11 |
| D5 | c2s/s2c 撞值分列口径 | **保持分列**（167↔170、175↔175、181↔181） | `opcode-matrix.json` 两栏 |
| D6 | BtPet1/2/3 origin (15,13) vs (15,0)、y 高 13px | **只记不改**，等一张原生展开宠物面板帧再定 | `UIEquipInventory.cpp:176-180` |
| D7 | 4 格改色命中率 34.67% 偏低格 | 先出「像素改色 ↔ 原生 disabled」对照，**待裁决** | r03 §0 |
| D8 | 召唤/龙/骑宠 UI 资源层空白（无截图/layout/PS/顶层节点） | 补 orange-wz 实解析或原生展开帧，**待资源** | r03 §7 |
| D9 | AskPet gage ColorBox / locale 4 stub 键 / Configuration 覆盖稀薄 | 维持降级占位（E19 无资源），低优先补文案 | `UIMiscShells.cpp:2019`/`zh-CN.ini` |
| D10 | 场地伤害族 5 条 / 220 / G8 / G9 / `change_target` 空 stub 整族 | 维持不做，跨面统一登记 | E19–E22 / G14 |
| D11 | Q2 文档债统一回写（`feature-gap:197/220/627`、`parity-gap:79`、`gap-inventory.md` 旧 head） | 交裁决后**统一回写**，本面不擅改 Q2 | §2.4 / r05 §12 |
| D12 | Beholder/DamageSummon 仅 GM 路径、PetExceptionListHandler 空实现、PetChat/"HitN"/MoveSummon 壳 | 是否补玩家路径 / 攻击帧，**待裁决** | A08/B07/C07–C10 |
| D13 | 龙 die1/filpL/R/skill 帧 + 召唤浮动帧 + CQuestMan 联动 + move_ttl 余晖 | 资源在/不在分列，接不接待裁决 | C11–C13/F10/F11/G08/B12 |
| D14 | 门禁与提交 / 续作 | 不接 orange-wz；先 P0 + 全量 Rebuild + 1280×720 实机；原子提交须授权；P3 续作第六面 | §7 Plan |

**D2+D3+D4+D6 共同决定「校准 ≠ 改回原生」，与前四面一致。**

## §7 一次性 Plan（P0–P3）

- **P0（零风险 · 纯文档/客户端注释 · 可直接做）**：① D1 只换 `MapPortals.h:40` 注释依据引 Angel 083；② D5 固化 c2s/s2c 撞值分列口径到本面 README + r05；③ D11 前半：出「Q2 旧账 ↔ 本面取值」逐条对照表（不擅改 Q2 正文）；④ 门禁 `python tools/gen_opcode_matrix.py --check`；⑤ r01–r05 + 6 evidence 归档完整性校验。
- **P1（需用户裁决）**：D2 字节丢弃、D3 三页穿戴接线、D4 整族补齐、D10 维持不做、D12 玩家路径补齐、D13 帧集接线、D11 后半统一回写 Q2。逐点先交决策表，裁决后再动源码。
- **P2（需 IMG/实机）**：D6 原生展开帧定 origin、D7 改色对照、D8 补 orange-wz 实解析或资源、D9 文案补齐与 gage；随后单测 + `tools/build_smoke.bat` 全量 Rebuild（注意 `IO/UIElement.h:195-199` NUM_TYPES 警示）+ 1280×720 宠物面板/召唤/龙/骑宠屏比对 + 抓 `packet_dump.log` 验字段序。
- **P3（goal 续作）**：第六功能面「活动小游戏」复用本面 5 脚本生成器 + 三路只读取证模板 + 六维 98 条 / 14 决策点 / Plan 结构；goal 保持 active 直至逐面闭环，实施须等用户裁决各面 D1–D14。

## §8 限制与未知

- **IMG 层阻塞（六面一致）**：orange-wz(IMG) MCP 未接入本 DSH Host，且已知 `Index 16 out of bounds` 解析缺陷；本面召唤/龙/骑宠 UI 资源层几乎空白 → 排版/样式判定中 ⏳4 条、❌1 条属「待资源」而非「代码错误」。
- **`read_image` 限制**：模型 `hy4free` 未声明 image input，PNG 返回 `cannot read ... as an image: model "hy4free" does not declare image input` → 未做像素级视觉复核，截图尺寸/坐标取自 evidence 文本与源码常量。
- **CMovePath 未取自身伪代码**：`angel-petsummon-ea.json` 有 10 具名方法但不在 TARGETS/FOCUS 53 类精选集，`CMovePath::OnMovePacket` 仅 `CPet::OnMove` 的 `0x68b371` 引用；需字节级移动布局则单独 decompile。
- **未做**：产品源码改动、构建、实机、抓包、git 提交（原子提交须用户授权）。所有结论为只读取证 + 静态销案，标注 [观察]/[推断]/[待实机]。
- **跨 workspace**：`session_event_read({seq:885})` 报 `session target is outside the caller workspace`，历史取证走已落盘 evidence，不依赖跨 workspace 事件。

## §9 证据索引与复现

### §9.1 文件清单

```
docs/features/pet-summon-angel-calibration-2026-11/
├── README.md                                    ← 本文件（主交付物）
├── notes/
│   ├── r01-angel-native-petsummon-facts.md       Angel 083 逐字 Hex-Rays 语义
│   ├── r02-main-agent-crosscheck-cancel.md       主 agent 当前端源码逐字销案
│   ├── r03-petsummon-img-style-evidence.md       IMG 节点/组件/截图/locale/Configuration
│   ├── r04-current-petsummon-source-inventory.md 当前端源码逐类盘点 + 三结论
│   └── r05-prior-petsummon-doc-audit.md          既有文档四态 + 常量表 + 15 复核点
└── evidence/
    ├── angel-petsummon-ea.json                  968 函数 / 64 类 EA
    ├── angel-petsummon-decompile.json           392/392 原始
    ├── angel-petsummon-decompile-full.json      逐字伪代码（含 CPet::OnMove 补全）
    ├── angel-petsummon-truncated.json           /output/ 补全记录（trunc_fixed 4）
    ├── angel-petsummon-facts.md                 5819 行 / SP_ 185
    └── angel-beidou-petsummon-ea-crosscheck.json 968/968 同 EA
```

### §9.2 复现命令

```bash
# 门禁（协议矩阵对账）
python tools/gen_opcode_matrix.py --check     # → GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)

# 采集链（每台先 server_health 13339/13337）
python tools/ida_angel_petsummon_ea.py
python tools/ida_angel_petsummon_decompile.py
python tools/ida_angel_petsummon_normalize.py
python tools/ida_angel_petsummon_facts.py
python tools/ida_angel_petsummon_crosscheck.py

# §3 判定计数（python 行级状态机，禁止手算）
python - <<'PY'
import re, collections
t = open('docs/features/pet-summon-angel-calibration-2026-11/README.md', encoding='utf-8').read()
rows = re.findall(r'^\| ([A-G]\d{2}) \|', t, re.M)
sec = t.split('## §3')[1].split('## §4')[0]
verdicts = [m.group(1) for m in re.finditer(r'^\| [A-G]\d{2} \|[^\n]*?\| (✅|⚠️|❌|⊘|⏳) \|', sec, re.M)]
print('rows=', len(rows), 'TOTAL=', dict(collections.Counter(verdicts)))
PY
# → rows= 98 TOTAL= {'✅': 55, '⚠️': 20, '❌': 16, '⊘': 2, '⏳': 5}
```

## §10 性质标注

- 本文为**校准 / 对照 / 决策点 / 一次性 Plan**文档，不是实现提交；不修改任何产品源码，不触发构建与实机。
- 结论区分 [观察]（逐字可读）/ [推断]（由观察推出）/ [待实机]（需真机 / IMG / 抓包）。
- 基准 083/v83；095 线符号/IDB 仅登记排除，不作依据；「改回原生会断 s9 联机」项一律交用户裁决。
- 与前四份 README（`login-flow-angel-calibration-2026-11` / `statusbar-hud-angel-calibration-2026-11` / `corewindow-angel-calibration-2026-11` / `social-economy-angel-calibration-2026-11`）同方法、同口径、同证据优先级，五面 EA 交叉验证一致。
