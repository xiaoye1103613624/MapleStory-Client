# r02 — 主 agent 当前端源码逐字销案复核（第五功能面「宠物·召唤兽·龙」）

> 主 agent 独立逐字复核（2026-11-08 22:07–22:22），不依赖三路 subagent 结论，用于把 r01（Angel 083 一手语义）与当前端 checked-in source 逐条销案 / 推翻 / 新增。
> 标注：[观察]=逐字可读；[推断]=由观察推出；[待实机]=需真机/IMG/抓包。全部路径为仓库相对路径+行号。
> 基准 083/v83；只读，未改任何产品源码。

## §0 六条结论

1. **协议面全闭环**：`Net/Packets/PetPackets.h`（9 包）+ `Net/Packets/SummonDragonPackets.h`（5 包）全部 c2s 构造点齐；`Net/PacketSwitch.cpp:565-668` 宠物/召唤/龙 s2c opcode 枚举 + emplace 全齐；Angel `CVecCtrlPet::EndUpdateActive` 一手坐实 `COutPacket(167)` + 8B uid，与 `PetPackets.h:62` MovePetPacket 对齐 → **不缺封包**。[观察]
2. **宠物本体真渲染已实现**：`Character/Look/PetLook.cpp:38-50` 帧集 + `:61-67` draw + `:69-153` 物理 + `Character/Char.cpp:68` 接线；与 Angel `CPet` 28 方法（LoadLayer/PrepareActionLayer/SetSetItemEffect/SetSetItemBackground）语义层对应但结构不同（当前端拆 `Pet.h` 数据 + `PetLook.h` 外观，挂 `Char.h:162 PetLook pets[3]`）→ **「校准 ≠ 改回原生」**。[观察+推断]
3. **CUIPetEquip 三页 = 装备窗嵌入子面板，非独立浮窗**：Angel `if(a2==2000) TogglePetEquip; else if(a2>0x7D0 && a2<=0x7D3) *(this+360)=a2-2001` ↔ 当前端 `UIEquipInventory.cpp:181 pet_tabs[3]` / `:609-623 change_pet_tab`——**壳在、槽位数据/穿戴逻辑缺**（`:622` 显式预留）。全库无 `UIPet.h` 属结构对应，非缺失。[观察]
4. **召唤/龙是「真对象状态机 + 缺帧回退占位」并存**：`MapSummon.cpp:34-65` / `MapDragon.cpp:25`（`Mob["8840000.img"]`）已真对象化；但帧集缺失时一律 `FieldMarker::draw` 黄字兜底（`MapSummon.cpp:162`、`MapDragon.cpp:47,82`），`MapSummon.cpp:145 (void)s;` 丢弃 SUMMON_SKILL newStance、`MapDragon.h:16-19` die1/filpL/R 不接 → 属显式预留/降级，非「整族缺」。[观察]
5. **新发现 095 注释污染 1 处**：`Gameplay/MapleMap/MapPortals.h:40`「传送门触发范围（对齐 v95：CPortal::CheckKey 的按键通道 vs ProcessPortal 的碰触通道）」→ 处置=只换注释依据引 Angel 083，不改行为。`PetPackets.h`/`SummonDragonPackets.h`/`Gameplay/MapleMap/*.h`/`UIEquipInventory.cpp` 均无 095/v95/Maplestory95/port-095 命中。[观察]
6. **两处待裁决的数据丢弃（s9 适配，不盲改）**：`MapObjectHandlers.cpp:526 static_cast<int32_t>(uniqueid)` 把 Angel 8B uid 丢高 32 位；`:520-523` nameTag/balloon 读了就丢。改回原生语义会断 s9 联机 → 标「须用户裁决」。[观察+推断]

## §1 出站封包逐字销案（Angel ↔ 当前端）

### §1.1 `Net/Packets/PetPackets.h`（149 行，W1 宠物族 c2s，9 个 OutPacket 全有构造点）
注释头逐字「W1 宠物族 c2s — 对账 s9 RecvOpcode + *Pet*Handler / SpawnPetHandler」。
- `:13 SPAWN_PET(98/0x62)` SpawnPetPacket `write_time(); write_byte(cash_slot); write_byte(0); write_byte(lead?1:0);`
- `:28 PET_FOOD(76/0x4C)`；`:42 USE_MOUNT_FOOD(77/0x4D)`
- `:56 MOVE_PET(167/0xA7)` MovementPacket `write_int(pet_unique_id); write_long(0); write_byte(1); writemovement(movement);` → **与 Angel `facts.md:673-678` `COutPacket(167)` + `EncodeBuffer(v5+160, 8u)` + `CMovePath::Flush` 逐字对应（8B uid 前缀）** ✅ 销案
- `:71 PET_CHAT(168/0xA8)` `write_int(pet_unique_id); write_int(0); write_byte(0); write_byte(act); write_string(text);`
- `:87 PET_COMMAND(169/0xA9)` `write_int(pet_unique_id); write_int(0); write_byte(0); write_byte(command);`
- `:102 PET_LOOT(170/0xAA)` `write_int(pet_unique_id); for(i<13) write_byte(0); write_int(oid);`
- `:117 PET_AUTO_POT(171/0xAB)`；`:133 PET_EXCLUDE_ITEMS(172/0xAC)` `write_int(pet_unique_id); write_int(0); int8_t n=...; write_byte(n); for write_int(item_ids[i])`
→ **9 包全有构造点，`feature-gap:340-341` #7/#8 与 opcode-matrix:2409-2529 client_sent:true 三方一致**。[观察]

### §1.2 `Net/Packets/SummonDragonPackets.h`（103 行，W2 5 个 OutPacket）
- `:13 MOVE_SUMMON(175/0xAF)` `write_int(oid); write_point(start); write_byte(1); writemovement(movement);`
- `:28 SUMMON_ATTACK(176/0xB0)` `struct SummonAttackHit { int32_t monster_oid; int32_t damage; };` 构造 `write_int(summon_oid); write_int(0); write_byte(direction); write_byte(min(hits.size(),15)); write_long(0); ×{write_int(monster_oid); 18×write_byte(0); write_int(damage)}` → 与 Angel `CSummoned::TryDoingAttackManual(22699B)`/`ProcessAttack(19570B)` 的攻击表语义对应
- `:60 DAMAGE_SUMMON(177/0xB1)` `write_int(oid); write_byte(-1); write_int(damage); write_int(monster_id_from);`
- `:75 BEHOLDER(178/0xB2)` `write_int(oid); write_int(skill_id); write_short(0)`（注释「AURA_OF_BEHOLDER 读 short；其它读 byte——写 short 0 兼容两侧」）→ 与 Angel Beholder 技能 id `1320006/1320008/1320009`（facts.md:961,1001,1008）对应
- `:90 MOVE_DRAGON(181/0xB5)` `write_point(start); write_byte(1); writemovement(movement);`
→ **5 包全有构造点**；运行时 `.dispatch()` 点：`MapFieldMarkers.cpp:366`(MOVE_SUMMON)/`:391`(SUMMON_ATTACK)/`:417`(MOVE_DRAGON)、`UIAdmin.cpp:150`(BEHOLDER)/`:164`(DAMAGE_SUMMON，**后两条仅 GM 命令路径**)。[观察]

### §1.3 s2c 注册（`Net/PacketSwitch.cpp`）
opcode 枚举 `:119 SPAWN_PET=168`、`:235 PET_CHAT=171`、`:236 PET_NAMECHANGE=172`、`:237 PET_COMMAND=174`、`:259 SUMMON_SKILL=180`、`:260 SPAWN_DRAGON=181`、`:262 REMOVE_DRAGON=183`、`:285 MOVE_PET=170`；emplace `:565 <SPAWN_PET,SpawnPetHandler>`、`:589 <PET_CHAT,PetChatHandler>`、`:590 <PET_NAMECHANGE,PetNameChangeHandler>`、`:591 <PET_COMMAND,PetCommandHandler>`、`:597 <SPAWN_SPECIAL_MAPOBJECT,SpawnSummonHandler>`、`:607 <SUMMON_SKILL,SummonSkillHandler>`、`:608 <SPAWN_DRAGON,SpawnDragonHandler>`、`:610 <REMOVE_DRAGON,RemoveDragonHandler>`、`:633 <MOVE_PET,MovePetHandler>`；`:1013-1027` opcode_msg 字符串表。→ **任务点名 6 条 handler 全注册，无缺**。[观察]

## §2 收包字段序逐字销案（Angel ↔ s9 addPetInfo / spawnSummon）

### §2.1 `Net/Handlers/MapObjectHandlers.cpp:487-536 SpawnPetHandler::handle` ✅ 销案
`int32_t cid=recv.read_int(); Optional<Char> character=Stage::get().get_character(cid); uint8_t petindex=recv.read_byte(); int8_t mode=recv.read_byte();`
`mode==1` → 注释「byte(1) byte(0) int itemId str name **long uniqueId** pos byte stance **short fh** bool nameTag bool balloon。修前 uniqueId 用 int+skip(4) 凑宽度、fh 用 int 吞掉 nameTag…按原类型读」→ `recv.skip(1); int32_t itemid=recv.read_int(); std::string name=recv.read_string(); int64_t uniqueid=recv.read_long(); Point<int16_t> pos=recv.read_point(); uint8_t stance=recv.read_byte(); int32_t fhid=recv.read_short(); bool nametag=recv.available()?recv.read_bool():false; bool balloon=...; character->add_pet(petindex, itemid, name, static_cast<int32_t>(uniqueid), pos, stance, fhid);`
`mode==0` → `bool hunger=recv.read_bool(); character->remove_pet(petindex, hunger);`
→ **与 Angel `CPet::Init`(facts.md:321-335) `Decode4(templateid)/DecodeStr(name)/DecodeBuffer(160,8)/_ZtlSecureFuse<short/uchar/short>` 字段序一致：8B uid 已按 long 读** ✅；`fh` 已按 short 读 ✅。
⚠️ **残留 2 处降级（列 D 决策点）**：`:526 static_cast<int32_t>(uniqueid)` 丢高 32 位（Angel uid 8B、`a11l` 反查 invType=5 用 long）；`:520-523 (void)nametag; (void)balloon;` 读了就丢。[观察+推断]

### §2.2 `Net/Handlers/MapObjectHandlers.cpp:1370-1419 SpawnSummonHandler::handle` ✅ 销案
注释「s9 PacketCreator.spawnSummon（PacketCreator.java:1167-1181）：int cid, int oid, int skill, byte 0x0A(v83 填充), byte level, pos, byte stance, short 0, byte moveType, bool !puppet, bool !animated。修前把 0x0A 当 level 读、pos 从错位字节起读 ⇒ 每只召唤兽等级恒 10、落位错 1 字节」→ `cid/oid/skill` 各 read_int、`recv.skip_byte()` 0x0A、`skill_level=read_byte`、`pos=read_point`、`stance=read_byte`、`skip_short`、`movetype=read_byte`、`attacking=read_bool`、`animated=read_bool`、`Stage::get().get_field_markers().spawn_summon(oid, cid, skill, pos, skill_level);`、`if(animated) ...summon_spawn(oid);`
注释「moveType: 0 不动，1 跟随，2/4 仅传送跟随，3 鸟跟随（PC:1177）」「客户端召唤兽位置由 MOVE_SUMMON 包驱动（原生同样是包驱动而非本地 AI ⇒ 这里无需消费）」
→ **与 Angel `CSummoned::Init`(facts.md:814-825) `Decode2/2/1/2/1/1 + Skill==0 时多 Decode1` 语义对应（字段宽度依 s9，编号一致但字节布局不同，按红线不判当前端错）** ✅；0x0A filler 已修 ✅。⚠️ `:1414-1417 (void)stance; (void)movetype; (void)attacking;` 读了就丢 → 列 D 决策点。[观察]

### §2.3 其它 s2c handler 降级点
`:1206` PetChatHandler `:1222 character->speak(text);`（宠物话当角色气泡，`(void)act;`）；`:1616` SummonAttackHandler `:1636 "Hit"+std::to_string(total)` / `:1643` 文本场标显示伤害（无攻击帧）；`:1772` PetExceptionListHandler `:1776-1780` 空实现；`:1601` MoveSummonHandler `:1609` 只读位置丢弃 movement 段。→ 均「壳/占位」，非协议缺失。[观察]

## §3 渲染与数据模型逐字销案（Angel ↔ 当前端）

### §3.1 宠物本体 ✅ 真渲染已实现（推翻旧账）
- `Character/Look/PetLook.h:31-42` `enum Stance : uint8_t { MOVE, STAND, JUMP, ALERT, PRONE, FLY, HANG, WARP, LENGTH };`；`:44-49 stancebyvalue(uint8_t value) { uint8_t valueh = value / 2; return valueh >= LENGTH ? STAND : static_cast<Stance>(valueh); }` → **与 Angel `CPet::DoAction` 的 stance/2、动作表映射语义对应**
- `Character/Look/PetLook.cpp:35` `namelabel = Text(Text::Font::A13M, Text::Alignment::CENTER, Color::Name::WHITE, Text::Background::NAMETAG, name);`
- `:38` `nl::node src = nl::nx::Item["Pet"][strid + ".img"];`；`:40-46` `animations[Stance::MOVE]=src["move"]; [STAND]=src["stand0"]; [JUMP]=src["jump"]; [ALERT]=src["alert"]; [PRONE]=src["prone"]; [FLY]=src["fly"]; [HANG]=src["hang"];`；`:48-50` `nl::node effsrc = nl::nx::Effect["PetEff.img"][strid]; animations[Stance::WARP] = effsrc["warp"];` → **与 Angel `SP_4818_EFFECT_SETEFFIMG_D_PETEFFECT` / `CPet::LoadLayer(8082B)` / `PrepareActionLayer(4731B)` 资源族对应**
- `:69-153 update(physics, charpos)` 本地 AI：`PETWALKFORCE=0.35` / `PETFLYFORCE=0.2`、`distance>150` 直接 set_position、x 差 >50 施加 hforce + flip、`type=PhysicsObject::Type::NORMAL/FLYING` + `NOGRAVITY` → 与 Angel `CVecCtrlPet::WorkUpdateActive(14018B)` 语义层对应（当前端简化物理）
- `:170-174 set_stance(uint8_t stancebyte) { flip = stancebyte % 2 == 1; stance = stancebyvalue(stancebyte); }`
- `Character/Char.cpp:68 pet.draw(...)`、`:99-121` 按角色 state 切 HANG/FLY/STAND、`:316-322 add_pet` → 接线闭环
→ **`feature-gap-inventory-2026-11/README.md:197`「无真渲染」判词对宠物本体已过时**，本面 README 按 checked-in source 取值。[观察+推断]

### §3.2 宠物数据模型（Angel type=5 CASH 槽 ↔ 当前端）
- Angel `CPet::GetItemSlot` 逐字 `CharacterData::FindCashItemSlotPosition(this: v3, a2: 5, a3: *(this + 20))` → `CharacterData::GetItem(this: v3, a2: &v6, a3: 5, a4: CashItemSlotPosition) + 4`（**宠物挂 a2=5 Cash 栏**）；`CPet::GetLevel` `return _ZtlSecureFuse<unsigned char>(a1: ItemSlot + 53, a2: *(ItemSlot + 55));`（等级 itemslot+53 1B）；`CPet::OnValidateStat` `*(this+42)=_ZtlSecureFuse<short>(ItemSlot+59,...)`（亲密度）等
- 当前端 `Character/Inventory/Pet.h:25 class Pet`（纯背包数据）字段 `:38-45` item_id,expiration,petname,petlevel,closeness,fullness,unique_id；`:44` 逐字「现金宠服务端 64 位 uniqueId（AskPet 包内 long）；未解析时为 0。」
→ **字段语义一一对应（等级/亲密/饱食/uid），但存储形态不同**：Angel 从 ItemSlot 偏移读（`_ZtlSecureFuse` 混淆），当前端拆成独立 POD；`UIItemInventory.cpp:827-832 if (prefix == 500)` 双击 CASH 500* 槽召唤 ↔ Angel `FindCashItemSlotPosition(a2:5)` ✅ 语义销案。[观察]

### §3.3 CUIPetEquip 三页 ✅ 壳在 / ❌ 穿戴逻辑缺
- Angel `CUIPetEquip::OnButtonClicked` 逐字 `if(a2==2000){ CUIEquip::TogglePetEquip(this: dword_BED64C); } else if(a2>0x7D0 && a2<=0x7D3){ *(this+360)=a2-2001; CWnd::InvalidateRect(this, a2:0); }`（**2000=总开关、2001/2002/2003=三页**）；`CUIPetEquip::OnMouseMove` `CharacterData::GetItem(..., a3:1, a4:-v5)` / `a4:-100-v6`（**宠物装备槽 = 装备栏 a3=1 负槽位**）
- 当前端 `UIEquipInventory.cpp:181 pet_tabs[3]={"BtPet1","BtPet2","BtPet3"}`、`:183-185` `(right+7+34*i, bottom-30)`、`:609-623 change_pet_tab` 只切 TwoSpriteButton 美术态、`:622` 逐字「预留：BtPet1/2/3 三页宠物装备槽的穿戴/卸下（Equip/pet 下无 Slots 子节点，槽位数据待协议批次）」
→ **三页签美术壳/显隐已对齐 Angel ctrl 2001–2003；负槽位取物 + 穿戴/卸下逻辑未接线**（Angel 三页区间 `facts.md:625-628 21–29 / 31–37 / 39–45`）→ E21 真缺仍成立，列 D 决策点。[观察]
- 旁证 `corewindow-angel-calibration-2026-11/evidence/angel-corewindow-decompile-full.json:139` `CUIEquip::OnButtonClicked case 3001u: ToggleDragonEquip(...) / case 3002u: TogglePetEquip(this)`、`:141 TogglePetEquip(Alloc(1456), v5=...+123, sub_7FE299, *(this+361)=1)` → 原生确为装备窗 Toggle 拉起子面板，非独立窗。[观察]

### §3.4 召唤兽 / 龙真对象 ✅ 落地 + ⚠️ 缺帧回退
- `Gameplay/MapleMap/MapSummon.h:20 class MapSummon : public FieldMarker`；头注释逐字「v83 玩家召唤兽的动画不在 Mob.nx，而是随技能打包在 Skill/<ns>.img/skill/<id>/summon 下，子节点即状态机帧集：stand(8帧循环, origin 35,96) hit(4帧受击) die(1帧死亡) summoned(12帧入场闪光)；修前读 summon["0"]（该节点不存在）⇒ 落到 summoned["0"]（7×7 占位小帧），这就是召唤兽看不见 / 显示成小碎块的根因」
- `MapSummon.cpp:34-39` `nl::nx::Skill[skill_namespace(skill_id)+".img"]["skill"][strid]["summon"]` 回退 `["summoned"]`；`:41-52` 只收容器段跳过 info；`:56-65` 无 stand 退化基类占位 / 有 summoned 则 `set_state("summoned")`；`:104-114 apply_damage` 播 hit；`:116-130 play_die` 播 die 否则 deactivate；`:138-146 set_stance(int8_t s)` 逐字「SUMMON_SKILL 的 newStance（s9 Character.java:4217/4232，beholder 系召唤物）：固定 5（驻留）与随机 6..8（浮动系）。原生据此切换上下浮动帧；但本数据召唤兽帧集只有 stand/hit/die/summoned（探针实测，无游走/浮动帧），位置移动由 MOVE_SUMMON 包驱动 ⇒ 无帧可播，保持当前状态」`(void)s;`；`:183-198` 一次性段播完 die→deactivate / 其它→stand
- `Gameplay/MapleMap/MapDragon.h:32 class MapDragon : public FieldMarker`；头注释逐字「Mob/8840000.img: stand(6) move(18) hit(1) die1(90) attack1-5 skill1-7 filpL(6)/filpR(6) summon(41)。部署仅 8840000（8840001..14 全部缺失——与原生同树，V17.7 Data/Mob 也只有这一只）」「SPAWN_DRAGON = int(owner cid) + short x,short 0,short y,short 0 + byte stance + byte 0 + short job（PacketCreator:7405-7415）」「龙 oid 语义 = owner cid（Dragon.java:34 getObjectId()），原生键 cid^0x40000000」「stance 奇偶 = 朝向」「死亡段 die1(90帧) 不接：s9 removeDragon 仅在登出/换图/删号调用…无伤害上行 ⇒ REMOVE ≠ 死亡」；`:50 int32_t move_ttl = 0;`
- `MapDragon.cpp:25 nl::node src = nl::nx::Mob["8840000.img"];`、`:52 flip=(stance%2)==0`、`:47`「无 stand ⇒ 基类占位文本兜底（不空画）」
→ **与 Angel `CSummoned::ProcessAction`（30ms tick + 每动作 20 字节表）、`CDragon::SetMoveAction`（动作写回 owner+0x148）、`CDragon::UpdateQuestInfo`（CQuestMan 联动）语义层对应；结构不同（当前端继承 FieldMarker，Angel 独立对象 + CVecCtrlSummoned/CVecCtrlDragon）** → 「校准 ≠ 改回原生」。[观察+推断]

### §3.5 整族缺（Angel 有 / 当前端源码 0 命中）
`CActionMan`(17 方法：`LoadPetAction 0x40e1a5`/`LoadSummonedAction 0x40fe2b`/`GetSummonedProp 0x4106cc`/`MergeTamingMobSprite 0x41258d`/`LoadTamingMobAction 0x4138be`)、`CActionFrame`(9)、`CAnimationDisplayer`(19)、`CVecCtrlPet/CVecCtrlSummoned/CVecCtrlDragon`、`TamingMob` 骑宠实体、`CAvatar::SetRidingVehicle 0x456e35`/`SetVisibleTamingMob 0x456f18` → 当前端 Character/、Gameplay/、IO/ 三目录 grep 全 0 命中（命中全在 tools/ 与 docs evidence 的 IDA 证据里）。→ **动作帧系统 + 骑宠实体整族缺**（列 D 决策点，含 083 原生 `SP_873..880/SP_3818/SP_4378/SP_5454` 骑宠/驯兽/孵化/Evan 龙骑文案族）。[观察]

## §4 IMG / 排版 / 样式 逐字（与 r03 互证）

- `IO/UITypes/UIEquipInventory.cpp:92 nl::node Equip = UiAssetResolve::window4("Equip");`、`:162 pet_backgrnd = Texture(Equip["pet"]);`（177×181）、`:169-171`「pet[177x181] 压住窗口右边 3px」`pet_panel_pos = (right-3, bottom-pet_backgrnd.height())`、`:173-174 BtPetEquipShow @ (right-73, bottom-24)`、`:187-188 BtPetEquipHide @ (right+152, bottom-22)`
- `:176-180` 逐字「实测 BtPet1/2/3 四态 origin 都是 (15,13)（不是旧注释写的 (15,0)，见 evidence/r27-b3-equip/README.md §1）。这三格在现有原生帧上 agree 仅 .322 —— 那张帧没画宠物页签，落位没有尺，故本轮**只记不改**：按 (15,0) 推出来的现有 y 可能整体高 13px，等一张展开宠物面板的原生帧再定（列入用户取证）。间距 34 沿用 evidence/w7-equip/coords.md 的模板匹配结果。」→ **排版待定项，禁止盲调**（列 D 决策点）
- `:213-215`「宠物装备面板底图（Equip/pet，槽名已烤进贴图，不再用 Text 重画）」；`:219-221`「不可用槽：原生没有第二张 disabled 美术，而是对同一张 backgrnd 的该格逐像素改色…改色只由 slot_available() 的运行期判据决定」
- `UIMiscShells.cpp:1762/:6805 load_window_bg("UtilDlgEx_Pet")`（419×296）、`:6922 "UtilDlgEx_MultiPetEquip"`（419×219）、`:6826-6915` 软键硬编码 `"Pet name"/"cmd 0-9"/"pet chat"/"exclude ids"/"slot,itemId"`；`UIMiscShells.h:271-288 WND_W=419;WND_H=297;PET_LOOK_X=209;PET_LOOK_Y=218;BTN_PREV_X/Y=15/191;BTN_NEXT_X/Y=341/191;BTN_OK_X=314;BTN_CANCEL_X=363;BTN_EXIT_X=9;BTN_BOTTOM_Y=272`
- `UIMiscShells.cpp:2010`「等级在 nameTag 左侧；亲密/饱食条在 nameTag 下方（UtilDlgEx 无专用 gage 节点，ColorBox 按条宽对齐 nameTag）」`:2019/:2029 ColorBox(BAR_W, BAR_H, DARKGREY, 0.85f)` / `:2025` 黄条 / `:2035` 绿条 → **E19 亲密/饱食原生位图条缺、ColorBox 降级占位**（`a11m:36-42`/`a11l:41-49`/`pending-live-tests:264,408` 三档一致）[观察]
- `UICharInfo.cpp:90/93 character["BtPet"]/["BtRide"]` + `:104-108`「坐骑外观未暴露：Ride 保持禁用」；`UISkillBook.cpp:172 main["BtRide"]` → 骑宠按钮壳在、渲染缺 [观察]
- `locale/zh-CN.ini:349 charinfo.pet_wish=Pet {} Wish {}`、`:610-612 utildlgexmultipet.*` 共 **4 stub 键 ×2 语言，zh-CN 全英文未译**；`Configuration.h:1055-1064` 仅 `ui.autopot.hp_item/mp_item`（无 `pet_auto_pot` 字面量、`Configuration.h` 全文 1088 行 grep pet/summon/dragon/mount/taming 仅 `:1055` 一条注释）→ 文案/落盘设置覆盖稀薄 [观察]

## §5 095 污染与文档一致性（处置 = 只换注释依据、不改行为）

| 位置 | 逐字 | 处置 |
|---|---|---|
| `Gameplay/MapleMap/MapPortals.h:40` | 「传送门触发范围（对齐 v95：CPortal::CheckKey 的按键通道 vs ProcessPortal 的碰触通道）」 | 换注释依据引 Angel 083 `CPortal`（`angel-petsummon-ea.json`），不碰通道逻辑 |
| `port-095/evidence/opcode-v95/dec/_OnPacket_CSummonedPool__QAEXJAAVCInPacket___Z.c` | 095 线 CSummonedPool 反编译 | 仅登记排除，禁引作 083 依据 |
| `beidou_v177_decrypted_strings.tsv:2341/4680/4681` | V177 明文串 `Character/PetEquip/%08d.img` 等 | 仅记存在性，不作 v83 基准 |
| `feature-gap-inventory-2026-11/README.md:197` | 「无真渲染/8 包/MapObjectHandlers.cpp:208,213,218」 | 旧读数，本面按 source 取值（9 包、真渲染已在、行号漂移），列 D 后统一回写 |
| `feature-gap-inventory-2026-11/README.md:220` E20 | 「部分已修(G11/G6/G7)」漏记 G5 | 三档不一致，按 `map-object-audit/README.md:40` + 源码 MapDragon 统一，列 D 后回写 |
| `feature-gap-inventory-2026-11/README.md:627` | 「28/6/2/17/10」 | Angel 实测 CPet28/CSummoned16/CDragon10/CMob51，属旧读数 |
| `parity-gap-2026-11/README.md:79` | 「宠物系统 = 能看到、不能用」 | 原文未删但已被 §9.5 覆写作废，属文档债 |

## §6 跨面共性与本面新增

- **五/六面一致性**：登录 327/328、HUD 547/548、核心窗口 439/441、社交 752/752、宠物 968/968 同 EA → Angel 只补符号语义层，不补功能/版本。[观察]
- **跨面共性真缺（沿用）**：`IO/UITypes/UIChatBar.h:123 change_target(int32_t) {}` 空 stub 死链；`CChatHelper::TryChat` 脏词/刷屏/重复句整族缺；`CCtrlMLEdit` IME 候选窗；095 注释污染族（社交面 `UIMessenger.cpp` 10 处 + `.h` 4 处 + 本面 `MapPortals.h:40`）。[观察]
- **本面新增真缺**：CActionMan 动作帧系统 / TamingMob 骑宠实体与预览 / 装备三页负槽位穿戴 / AskPet gage 位图条 / Beholder 暗骑 aura 充能 / 场地伤害族 5 条 c2s / PARTY_SEARCH_REGISTER(220)（社交面已列）/ MonsterStatus(G9)+G8 出生特效资源缺源显式预留。[观察]
- **本面新增降级（壳/占位）**：`MapObjectHandlers.cpp:526` 8B uid→int32 丢高 32 位、`:520-523`/`:1414-1417` 多字段读了就丢、`:1222` 宠物话当角色气泡、`:1636-1643` "HitN" 文本场标、`:1776-1780` PetExceptionListHandler 空实现、`UIAskPet` ColorBox 亲密条、`FieldMarker` 黄字兜底。[观察]

## §7 冲突取值表（升级证据优先级，供 README §2 引用）

| 冲突 | 旧账 | 本面取值依据 | 结论 |
|---|---|---|---|
| 宠物系统可用性 | `parity-gap:79`「能看到、不能用」 | `feature-gap:195/388` §9.5 + r04 `.dispatch()` 点 | ✅ 协议闭环，旧账作废 |
| E7/E8/E9/E13/E14 dispatch | `feature-gap:207-214` 🟡 未逐条证 | `Stage.cpp:491`/`UIMiscShells.cpp:6897,6908`/`UIAdmin.cpp:150,164` + opcode-matrix client_sent:true | ✅ 已发（E13/E14 仅 GM） |
| 宠物真渲染 | `feature-gap:197`「无真渲染」 | `PetLook.cpp:38-50,61-67` + `Char.cpp:68` | ✅ 已实现（旧账指召唤/龙场标） |
| G5 Dragon 修复态 | `codebuddy-baseline:194` 仅场标 / `feature-gap:220` 漏记 | `map-object-audit/README.md:40` + `MapDragon.cpp:25` | ✅ 真对象 + 缺帧回退占位 |
| 8 类场标是否全仅文本 | `codebuddy-baseline:173,193,199-203` | r05 §6 较新现场 | 7 类已修，仅 MonsterStatus(G9) 仍文本 |
| c2s/s2c 撞值 | beidou notes r302/r303/r305 用 s2c 编号 | `opcode-matrix.json` 两栏 + `Net/OutPacket.h` vs `PacketSwitch.cpp` | 必须分列（MOVE_PET 167↔170 等） |
| 083 原生 vs s9 字节布局 | 改回原生会断联机 | 四面红线 `login-flow:26-27`/`corewindow:286`/`social-economy:309-310` | 保持 s9 联机优先，交用户裁决 |

（全程只读，未修改任何产品源码、未构建、未实机、未提交。）
