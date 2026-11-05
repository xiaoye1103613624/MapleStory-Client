# 地图对象逻辑全量审计报告（map-object-audit）

> 承接 CodeBuddy 中断会话 `947f8ba0412e4e539eed10d8ecd7916b`（plan 已 building，因频率限制 error 6004 停在 audit-baseline）。
> 转录全文与子代理基线已归档：[evidence/codebuddy-transcript-947f8ba0.md](evidence/codebuddy-transcript-947f8ba0.md)、[evidence/codebuddy-baseline.txt](evidence/codebuddy-baseline.txt)。
> 本轮由 DSH 接续：基线断言逐条**亲自核验**（客户端源码 + s9 源码 + 运行时 img catalog + IDA），修正子代理两处失真，产出缺口清单与修复方案，并按用户指令直接进入 P0 开发。

## 裁决口径（三方交叉核验）

| 方 | 来源 | 用途 |
| --- | --- | --- |
| ① 原生逻辑 | BeiDou.exe IDA @13337（launcher 层明文字符串有限；游戏层证据用已落盘反编译语料 `E:\MXD\BeiDou-Client-xy\REA反编译`） | 行为基准 |
| ② s9 协议 | `E:\project\BeiDou-Server_s9` PacketCreator.java / handlers | 包格式与 opcode 权威 |
| ③ 运行时 img | `E:\MXD\BeiDou-Client-xy\Data`（catalog_*.json 为 --resource-probe 产物） | 节点存在性 ground truth（R1/R2 红线） |
| ④ 当前实现 | 本仓库 Gameplay/MapleMap + Net/Handlers | 被审对象 |

## 对子代理基线的两处修正（已亲自核验）

1. **“场标审计客户端”说法过头**：Summon 实际是**真状态机对象**（`MapSummon`：stand/hit/die/summoned，取证注释 `_tools/summon_probe`，`MapFieldMarkers::spawn_summon` 走 `MapSummon` 实例）；Mist 也是真对象（`MapMist`），但**区域尺寸/类型被丢弃**（见 G4）。纯文本场标的是 Dragon/Door/动态Portal/MonsterStatus/Catch/ItemFX/CharBox。
2. **“opcode 与 s9 完全一致”成立**，且 `consume_mob_stati`（MapObjectHandlers.cpp:67-105）对 spawn 内嵌状态的解析**逐字节正确**（含 Java 掩码 mod32 折叠、反射计数器）——子代理未看到此函数，其对 SPAWN_MOB 路径的担忧不成立。缺陷只在 **APPLY_MONSTER_STATUS handler 自身**（只取第一条状态，见 G1）。

## 逐对象结论

### 完整（协议解析与消费均对齐，无需改动）

- **Mob**：SPAWN/KILL/CONTROL/MOVE/HP/DAMAGE 全链对齐 s9；分裂怪 `effect==-3` 雪藏+父死唤醒已接（Mob.cpp:157 + MapMobs.cpp:72）；hide/unhide 已修（:674-676,721-726）。
- **Npc**：spawn/move/remove + 商店/对话（NpcInteractionHandlers）完整。
- **Reactor**：hit/destroy 解析对齐；`Sound/Reactor.img/{rid}/{state}/Hit` 与 IDA `sub_989ADD`（"Sound/Reactor.img/%s/%s"@0x3073+"Hit"@0x5543，本轮重新反编译坐实）一致；攻击命中触发 `DamageReactorPacket`（Combat.cpp:191，s9 ReactorHitHandler 字段 int+int(pos)+short+skip4+int 逐一对齐）。
- **Drop**：`playerdrop = !read_bool()` 与 s9 `writeByte(isPlayerDrop()?0:1)` **恰好一致（非 bug）**；拾取归属由服务端 `ItemPickupHandler`/`MapItem` 裁决，客户端 bounds 判定+服务端校验的双层结构与原生同构。
- **Char/Pet/Summon（对象本体）**：真实渲染，宠物挂角色（Char.cpp:66-119）。
- **BossHP**：FIELD_EFFECT mode5 字段与 s9 showBossHP 一致；UIBossHP 用 DualMobGauge 三分件（backgrd/gauge/text，原生 `sub_56C9B7` 同序，r17 证据在案）。

### 部分实现 / 缺口清单（按优先级）

| # | 级别 | 对象 | 缺口 | 证据 |
| --- | --- | --- | --- | --- |
| G1 | **P0 ✅已修** | MonsterStatus | 原 `ApplyMonsterStatusHandler` 只消费一个 int 当 mask（**s9 writeIntMask 实为两个 int**，PacketCreator.java:3995-4007）、只读第一条状态 6 字节且丢尾随 `short(-1)` ⇒ 单 buff 即错位、多 buff 全错。修复：双 int popcount 计条数、循环消费 8B/条、尾部按 `byte(size)+int(0)`（前可嵌 0..N 个 int reflection）精确收口，多状态聚合 `Debuff#id+n` | 修复见 MapObjectHandlers.cpp:1679+（本轮构建 0 error） |
| G2 | **P0 ✅已修** | Drop | `Drop::expire` switch 无 `case 4/5`：s9 实际会发 `4`（爆炸/吸卷，AbstractDealDamageHandler.java:229）与 `5`（宠物拾取，Character.java:2078）——default 不命中任何分支，**掉落物永不消失**。修复：2/4/5 同走飞取淡出，1/未知值 default 直接消失 | 修复见 Drop.cpp:108+ |
| G3 | P1 **✅已修（诊断已修正）** | Reactor | ~~播错帧~~（蓝军自检推翻：s9 顺序触发 new=old+1 时旧写法与 draw 的 `state-1` 恰好重合）。**真问题=乱序跳态崩溃**：timeout 换态（Reactor.java:347-349）时 `animations.at(state-1)` 查不到键直接抛 `std::out_of_range`（Reactor.cpp:62/:71）；且 `destroy()` 参数 state/pos 全弃用、写死 ++。修复：按 `state-1` 键存储、缺 hit 帧守卫不抛、destroy 消费服务端 state/pos | 修复见 Reactor.cpp:79-123 |
| G4 | P1 **✅已修（含区域渲染 2026-11-05）** | Mist | 永生+REMOVE_MIST；type/level/box 消费；**区域贴图**改接 `Skill/.../tile` 平铺（IDA `sub_432776`/`sub_432A1E`，见 [mist-area-render](../mist-area-render/)）。旧「Map/Effect 无 mist」预留口径作废 | MapMist + MapObjectHandlers；mist-area-render |
| G5 | P1 **✅已修** | Dragon | 文本场标 ⇒ `MapDragon` 真对象：`Mob/8840000.img` 帧集（stand6/move18/hit/die1 探针在案；8840001..14 与参照端同缺 ⇒ 全档同帧）；stance 奇偶=朝向（同 Mob.cpp:189）；MOVE_DRAGON 走 MovementParser 尾态驱动 move 段+400ms 余晖回 stand；die1/filpL/R 有意不接（理由在 MapDragon.h 头注） | 新文件 Gameplay/MapleMap/MapDragon.{h,cpp}（已登记 vcxproj） |
| G6 | P1 **✅已修** | Door | 文本场标 ⇒ `MapDoor` 真对象（`Map/Obj/door.img/darkWood` door/1+doorpull/0，缺失回退文本）；**连带修复上行包**：`UseDoorPacket` 原 byte+byte ⇒ s9 DoorHandler 读 int+byte 永远匹配不到门（只回 blocked6）——改 int(ownerid)+byte；Stage 触碰式 `check_doors`（1500ms 冷却防刷屏），归属裁决全在服务端（与原生同构）；调试键 'D'/'O' 改 SoftKey 输 ownerid | MapDoor.{h,cpp}、PlayerPackets.h:143+、Stage.cpp、两处 UI |
| G7 | P2 **✅已修** | 动态 Portal | 原常量 oid `0x50000001` ⇒ 第二扇门 spawn 覆盖第一扇（s9 每扇门一条 SPAWN_PORTAL，组队多门互踩）。改位置派生键 `0x55000000|(x<<16)|y`（避开全部已用异或段，蓝军自检抓出 0x50 段与 ShowItemEffect 撞段后改段）；NONE 清除改 `remove_by_kind(PORTAL)`（Kind 新增 PORTAL） | WaveAShellHandlers.cpp:2079+；FieldMarker.h Kind |
| G8 | P2 | Mob 出生特效 | `effect>0`（烟雾/石棺等，s9 注释枚举 1/3/4/6/7/8/13/19…）只存储不播放；`item_effect` 读出即弃。**注意**：部署 Mob 命名空间**无 Mob/SpecialEff.img**（catalog_Mob.json 2057 文件零命中），还原需另找特效来源或显式预留 | Mob.cpp:141 vs 无播放点；R1 资源核验 |
| G9 | P2 | 怪物状态图标 | debuff 只在怪旁生成文本场标；原生/参照按 Mob `info/state` 图标渲染。**部署资源核验**：Mob.img 2057 个文件**无 status/level 节点**（catalog 扫描零命中），BuffIcon.img 现仅供玩家 Buffs 列表 | G1 同点；catalog 扫描 2026-11-04 |
| G10 | P2 **✅已修（2026-11-06）** | HitReactor UI | 修前 `state>0` 超发 UITreasureBox；真路径=道具 `UseTreasureChest`→`UseTreasureBox(int)`→入站开箱窗。HitReactor 只 `trigger`；见 [g10-treasure-box-semantics](../g10-treasure-box-semantics/) | s9 UseTreasureChestHandler + PacketCreator.UseTreasureBox |
| G11 | P2 **✅已修（2026-11-05）** | Chalkboard/CharBox/ItemFX/Catch | 文本场标 ⇒ 真件：Catch=`BasicEff/Catch/{Success,Fail}`；黑板=`Cash/0537 sample`；店招=`Cash/0503 signboard`；ItemFX=`ItemEff`（去英文名牌）。见 [field-marker-g11](../field-marker-g11/) | FieldMarker + MapObjectHandlers |

> 显式预留（不做）：Mob 情报浮窗（G8 资源缺源部分）、Boss 分阶段专属逻辑（s9 客户端侧仅 HP 条+tagColor，原生行为已由 mode5 覆盖）、`activates_by_touch/reactInBox`（s9 服务端在 `stats.getType(state)` 裁决，客户端无需还原该数据层）。

## 修复落地（本轮已完成 = G1/G2/G3）

- **G1**：mask 按 **两个 int** 消费（s9 writeIntMask 真值），循环 `bits` 条各 8B（value/skill/-1），尾部精确收口 `reflection* + byte(size) + int(0)`；多状态聚合 `Debuff#id+n`。单/多 buff 错位根除。
- **G2**：`expire` 补 `case 4/5`（同 2 飞取淡出；4 无主则原地淡出），`1/default` 直接消失——掉落残留路径清零。
- **G3**：hit 帧按 `newstate - 1` 键存储（与 draw/update 取帧下标同源）；`src[old]["hit"]` 空守卫，缺帧不再 `at()` 抛 `std::out_of_range`；`destroy` 消费服务端 state/pos，乱序下发用 `max(new, old+1)` 兜底。
- 验证：**`tools/build_smoke.bat` GATE OK + EXITCODE=0**（MSBuild Release x64，改动三 TU 全部重编，被改文件零新警告——C4458 已清），产物直落 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（11:39）。运行时冒烟（多 debuff 怪、宠物拾取、宝箱乱序触发）留给用户实机；Release LOG 为 no-op，观测走代码走查。

## 二期落地（用户裁决「2 可以先接着做」，2026-11-04）

G4/G5/G6/G7 已修（上表 ✅）；探针基建 `_tools/mapobj_probe`（build.bat+exe，只读取证，probe-out.txt/probe-out2.txt 归档）。构建闭环：`tools/build_smoke.bat` **GATE OK（opcode 矩阵含 USE_DOOR 修正后重生成）+ EXITCODE=0**，被改 8 个 TU 零新警告（PlayerPackets.h 62/69 两条 C4267/C4244 属既有 ChangeKeyMapPacket，非本次引入，最小改动不顺手修），exe 直落部署目录（13:02）。

**实机冒烟清单**（交付判据，留用户）：① Evan 上线带龙跟随、移动朝向正确；② 神秘门贴图显示、主人/队友踩门换图、非队友踩门服务端 blocked；③ 组队两人各放一门互不覆盖；④ 多 debuff 怪 Debuff#id+n 场标不错位；⑤ 宠物拾取/爆炸吸卷后掉落物不残留；⑥ 宝箱乱序触发不崩。

## 未做（显式预留，不藏半成品）

- G8 出生特效、G9 怪物状态图标、G4 雾区贴图：**参照端资源同缺**（探针实证部署树，非本端遗漏；协议/语义侧已全部消费对齐）。补资源即接线：G8=SpecialEff 挂载点（Mob.cpp 已存 effect）；G4=Map/Effect mist 键；G9=图标源待定后替换文本场标。
- G10 ✅ 已纠正（g10-treasure-box-semantics）；G11 ✅ 已贴图化（field-marker-g11）。
- UIBossHP 多 oid 分段（双段扎昆同时只显最后一条；s9 逐 oid 广播，客户端单条承接——需 UI 布局决策）。
- 改动未 git commit（等用户发话）。

## 构建/运行注意

- G4 Mist 区域渲染（MapHelper/mark，70 子节点已在案）；G5 Dragon 真精灵（spawn 尾段 stance/job 未消费）；G6 Door 真门对象+交互；G7 动态 Portal 多实例（现常量 oid 互斥覆盖）。
- G8 出生特效**显式预留**：部署 Mob 命名空间无 Mob/SpecialEff.img（catalog 2057 零命中，R1），特效来源需用户提供或按 s9 注释枚举逐项映射；G9 怪物 debuff 图标：部署 Mob.img 无 status 节点——原生图标源待决（交用户抉择）；G10 ✅ HitReactor 不再超发宝箱窗；道具开箱走 GachaponBox→UITreasureBox；G11 ✅ 已贴图化。

## 构建/运行注意

- Release `LOG()` 为 no-op（MapleStory.h）；观测判据用构建输出与代码走查。
- 构建前先 `taskkill /F /IM MapleStory.exe`（占用会 LNK1104 且 EXITCODE 仍 0）。
- OutDir 直落 `E:\MXD\BeiDou-Client-xy\`，勿再 robocopy。
