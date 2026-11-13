# 第十二面 · 掉落(Drop) Angel 083/v83 ↔ 当前端 全维校准 (P0 纯文档)

> 目标：`goal-11388c42`「以 Angel(13339) 的 083/v83 带符号 IDB 为权威校准源，逐功能完整校验并校准当前 MapleStory-Client 的功能、业务、逻辑、UI、排版、样式、交互」。本面 = ⑫掉落(Drop)，round50/90。
> 方法：REA → IDA(Angel 13339 / BeiDou 13337) → IMG(orange-wz MCP，ACTIVE) 只读取证 → 七维对照清单 → 决策点 + 一次性 Plan → 交用户裁决后再进实现。
> 性质：校准/对照/决策点/Plan 文档，**非实现提交**；全程只读，未改游戏源码/协议/WZ/IMG/XML。

## 0. 五条结论

1. **十二面 EA 字节级同二进制闭环再 +1**：Drop 面 `18/18 EAs identical`（`angel-beidou-drop-ea-crosscheck.json`），BeiDou 侧 `beidou_sub_named=18 / real_named=0 / missing=0` → 第三次独立复证 Angel(13339) 与 BeiDou(13337) 是同一份 v83 GMS 二进制，Angel 只多一层符号语义（18 funcs / 9 classes 全具名）。
2. **本面唯一 c2s 出站只有 1 个 opcode = 94 (0x5E, MESO_DROP)**：`CWvsContext::SendDropMoneyRequest @0xa23de5` 内 `COutPacket(94)` + `Encode4(update_time)` + `Encode4(amount)`；当前端 `Net/Packets/PlayerPackets.h:135-144 MesoDropPacket` 为 `write_time() + write_int(amount)` → **字段序逐字对齐**。捡物走 `PICKUP_ITEM(202)` / `PET_LOOT(170)`（s9 `ItemPickupHandler` 的 `skip(4)+byte+pos+oid` 与 `PetLootPacket` 的 `petId+13×byte+oid` 对齐）。
3. **收包 `DROP_LOOT(268)` / `REMOVE_LOOT(269)` 主链已对齐，且 083 与 s9 在 animation=4 上语义分叉**：s9 `PacketCreator.java:2631-2642 removeItemFromMap` 在 `animation>=2` 恒写 4 字节 `chrId`、仅 `pet=true` 再写 1 字节 slot；而 083 `CDropPool::OnDropLeaveField @0x506590` 对 `v2==4` 读的是 `Decode2` 并写 `*(v4+68)=update_time+delay`（原端「爆炸延时」记账）。当前端按 s9 读 `int cid` 是对的，**数值同为 4、语义不同 → 不得按 083 改成读 short**。
4. **金币图标四档阈值 083 与当前端完全一致**：Angel `sub_506BFE @0x506bfe` = `a1<50 → 0`；`a1>=100 → (a1>=1000)+2`；否则 `1`，即 `0/1/2/3` 对应 `<50 / 50..99 / 100..999 / >=1000`；当前端 `Gameplay/MapleMap/MapDrops.cpp:78` 的 `itemid>999 BAG : >99 BUNDLE : >49 GOLD : BRONZE` 逐档等价。图标路径 `Item/Special/0900.img/09000000..03/iconRaw` 亦由 `.nx` 复核存在。
5. **orange-wz IMG 门禁按 `AGENTS.md` ACTIVE 真闭合**：结构层 `Data/Item/Special` 枚举 4 个真 `.img` 子节点 WORKS；内容层 `get_node_tree_json`/`search_node` 8 次全 `IMG 解析失败`（GBK 原文）；命中回退条件后合规走 `.nxcache/Item.nx`（`Special` 4 子节点、`0900.img` 4 档 `iconRaw`）。原始报错文本 + 65536B key 规格逐条留证，`no_fake_green`。

## 1. 取证基线

### 1.1 IDA 端点表

| 环 | 端点 | 结果 |
| --- | --- | --- |
| REA | 既有 `evidence/r17/rea_search_*` 与 `docs/features/beidou-v17-parity/notes/r98/r107/r134/r180/r183/r471/r649/r788/r789` | ⚠️ subject 为 `BeiDou-ClientV17.7/BeiDou.exe`，非 13339，仅作 stripped 端字符串锚点，单列不冒充一手 |
| IDA Angel | `127.0.0.1:13339`（`Angel.i64`, imagebase 0x400000） | ✅ 18 funcs / 9 classes 全具名，反编译 `18/18 ok` |
| IDA BeiDou | `127.0.0.1:13337` | ✅ crosscheck `18/18 EAs identical`，`real_named=0` 全退化为 `sub_XXXXXX` |
| IMG (orange-wz MCP) | `127.0.0.1:10012` | ✅ 结构层 WORKS / ⚠️ 内容层 `IMG 解析失败` → 合规 `.nx` 回退（见 r03） |

### 1.2 三联门禁（AGENTS.md ACTIVE 顺序）

`orange-wz MCP → 仅当服务端返回解析异常 → .nx dump + 运行时 probe`。本面 8 次内容层查询全部返回 `IMG 解析失败: <name>`（GBK），故走回退并留原文与 key 规格；未以 `.nx` 替代结构层结论。

### 1.3 采集脚本 + evidence + notes

| 类别 | 文件 |
| --- | --- |
| 符号清点 | `tools/ida_angel_drop_ea.py` → `evidence/angel-drop-symbol-inventory.json` |
| 双端交叉 | `tools/ida_angel_drop_crosscheck.py` → `evidence/angel-beidou-drop-ea-crosscheck.json` |
| 字节审计 | `tools/ida_angel_drop_byteaudit.py`（0 missing → not required） |
| 全量反编译 | `tools/ida_angel_drop_decompile.py` → `evidence/angel-drop-decompile.json`（18/18） |
| 语义抽取 | `tools/ida_angel_drop_facts.py` → `evidence/angel-drop-facts.md`（310 行） |
| IMG 门禁 | `tools/face12_gate_and_drop_probe.py` → `evidence/orange-wz-drop-structure.json` / `evidence/nx-drop-content-probe.json` |
| 重数审计 | `tools/face12_readme_audit.py` |
| 笔记 | `notes/r01-angel-native-drop-facts.md` / `notes/r03-img-gate-fallback.md` / `notes/r05-drop-four-state-audit.md` |

### 1.4 本面独有技术点

- `angel-drop-decompile.json` 的 `functions` 值可能是**双编码 JSON 字符串**（`json.loads(v)['code']` 才能取到伪代码），脚本与人工读都必须走这层解包，否则会把整段 JSON 当一行。
- 阈值函数 `sub_506BFE` 未具名，必须按 `GetMoneyIcon` 内 `sub_506BFE(a1: a3)` 的调用点反查地址再单独反编译，否则四档阈值会被漏掉（facts 只抽字符串，抽不到数值分支）。
- 本面 18 个函数里 **9 个与拖拽（CDraggableItem/Menu/Skill、CWnd::OnDragDrop、CWndMan::Begin/EndDragDrop）相关**，属 `kws` 宽命中，不计入掉落主链。

## 2. 口径

### 2.1 七维 × 五判定

| 维 | 含义 | 条数 |
| --- | --- | --- |
| A | 功能 | 14 |
| B | 业务 | 14 |
| C | 逻辑 | 14 |
| D | 交互 | 14 |
| E | 排版 | 16 |
| F | 样式 | 12 |
| G | 协议/纪律 | 14 |
| 合计 | | **98** |

判定：`✅` 一致/销案 · `⚠️` 结构性差异或分列 · `⏳` 遗留(IMG/真机/未实证) · `❌` 缺失 · `⊘` 不适用。本面 ❌ 3 条（booby trap 族、拖拽丢弃族），无 ⊘。

### 2.2 证据优先级

实时运行行为 > 抓包 > active served assets > 当前进程配置 > 持久挑战状态 > 生成 artifacts > 检出源码 > 注释/死代码。本面以 `angel-drop-decompile.json` 内层伪代码 > 当前端源码 > s9 `PacketCreator` 权威写法 > 既有笔记。

### 2.3 083 vs s9 红线

- **083/v83 唯一基准**：`Angel.i64`；`Maplestory95.i64` / `docs/features/port-095/**` / `ExpeditionIntermediary` 全程禁引为 083 一手依据。
- **校准 ≠ 改回原生**：083 的 `animation=4` 读 `Decode2`、`v2==0` 走 `IWzGr2DLayer` 消失动画、独立 booby trap 流程，都是「对照事实」，不是「当前端必须补齐」；补齐会断 s9 联机。
- **数值重合 ≠ 语义重合**：`269` 双端同号但 animation=4 的包体宽度不同；`GetMoneyIcon` 的四档编号一致但资源路径在 083 是 `SP_1589_ICONRAW + Format` 动态拼、在当前端是 `.nx` 直取。
- **子串假阳性禁令**：`kws=(Drop/PickUp/Loot)` 命中 `CDraggable*` 9 个拖拽函数，属宽口径，不进掉落主链结论。

### 2.4 冲突取值表

| 冲突 | 083 一手 | 当前端 | 取值 |
| --- | --- | --- | --- |
| animation=4 包体 | `OnDropLeaveField` 读 `Decode2`（爆炸延时，写 `+68` 字段） | 读 `int cid`（s9 写 4 字节） | 取 s9 当前端，D1 记录 |
| animation=0 表现 | `IWzGr2DLayer::Animate/RemoveCanvas` 消亡动画 | `state=PICKEDUP` 原地淡出 | 取当前端近似，记 ⏳（F 维） |
| booby trap | `BoobyTrapCheckPickupItem` + `SendBoobyTrapAlert` 全套 | 无实现 | 记 ❌，D5 待裁决 |
| 拖拽丢弃 | `CDraggableItem::PutItem` / `CWndMan::BeginDragDrop` | 无实现（背包拖出用 SoftKey） | 记 ❌，D6 待裁决 |
| 金币图标阈值 | `sub_506BFE`：`<50/50..99/100..999/>=1000` | `>49/>99/>999` 四档 | 一致，进正面清单 |


## 3. 七维 98 条逐条对照

> 每行 `| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |`；判定列由 `tools/face12_readme_audit.py` 逐条重数。

### 3.1 A 功能 14 条（✅8 / ⚠️3 / ⏳1 / ❌2）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| A1 | s2c 进入 `268` | `CDropPool::OnPacket @0x5058db` `if (a1==268) OnDropEnterField` | `Net/PacketSwitch.cpp:142` `DROP_LOOT=268`；`:573` `emplace<DROP_LOOT, DropLootHandler>()` | ✅ | 数值一致，进正面清单 |
| A2 | s2c 离开 `269` | 同函数 `a1-269` 分支 → `OnDropLeaveField` | `Net/PacketSwitch.cpp:143` `REMOVE_LOOT=269`；`:574` 注册 `RemoveLootHandler` | ✅ | 数值一致 |
| A3 | c2s 丢金币 `94` | `CWvsContext::SendDropMoneyRequest @0xa23de5` `COutPacket(94)` | `Net/OutPacket.h:165-166` `MESO_DROP=94`；`PlayerPackets.h:135-144 MesoDropPacket` | ✅ | 本面唯一 c2s 出站 |
| A4 | 丢金币包体 | `Encode4(update_time)` + `Encode4(amount)` | `MesoDropPacket(int32_t amount)`：`write_time(); write_int(amount);` | ✅ | 字段序逐字对齐 |
| A5 | 捡物 `202` | 083 走 `CDropPool::TryPickUpDrop @0x50463c` + `CanSendExclRequest(30)` 节流 | `GameplayPackets.h:259-271 PickupItemPacket` = `write_int(0)+write_byte(0)+write_point+write_int(oid)` | ✅ | 与 s9 `ItemPickupHandler` `skip(4)+byte+pos+int oid` 对齐 |
| A6 | 宠物捡物 `170` | 083 宠物拾取走 drop-pool 内部 looter 绑定 | `PetPackets.h:102-115 PetLootPacket` = `write_int(petId)+13×write_byte(0)+write_int(oid)` | ✅ | 与 s9 `PetLootHandler` 注释同序 |
| A7 | 掉落实体族 | `CDropPool` 单类 7 方法（Pool 级） | `Gameplay/MapleMap/Drop.cpp` 基类 + `ItemDrop` / `MesoDrop` 两派生 | ✅ | 083 单类池化 vs 当前端多态实体，语义等价 |
| A8 | 金币四档图标 | `CDropPool::GetMoneyIcon @0x506c26` 内 `sub_506BFE @0x506bfe`：`<50→0`；`>=100→(>=1000)+2`；否则 `1` | `Gameplay/MapleMap/MapDrops.cpp:78` `itemid>999 BAG : >99 BUNDLE : >49 GOLD : BRONZE` | ✅ | 四档阈值逐档等价（`<50 / 50..99 / 100..999 / >=1000`） |
| A9 | 拾取优先级 | 083 `TryPickUpDrop` 遍历池内 drop 后按归属挑一个 | `Gameplay/Stage.cpp:479-493` 先扫 3 宠物槽取 `pet_uid`，有则 `PetLootPacket` 否则 `PickupItemPacket` | ⚠️ | 宠物优先是 s9 口径，083 无此分层；分列不合并 |
| A10 | booby trap 检测 | `CDropPool::BoobyTrapCheckPickupItem @0x5081b0` + `CWvsContext::SendBoobyTrapAlert` + `CUserLocal::CheckBoobyTrapPickUpRequest @0x95d7b7`（含 `BYTE2(a2)==82` / `==54` 双分支与 `IsDarkSight`/`IsSneak`/`IsStun` 前置） | 全仓 `booby`/`trap` 仅命中 includes/SDL3/stb 第三方，零实现 | ❌ | 083 独有交互链；D5 交用户裁决是否立项 |
| A11 | 拖拽丢弃 | `CDraggableItem::OnDropped @0x4ef140` → `PutItem`；`CDraggableMenu @0x4f91e4` / `CDraggableSkill @0x4faa22`；`CWndMan::BeginDragDrop @0x9e353d` / `EndDragDrop @0x9e37c2` | `OnDragDrop` / `BeginDragDrop` / `EndDragDrop` 全仓零命中；当前端走 SoftKey（`r180` `D` 键 `MoveItemPacket` dst=0） | ❌ | 083 独有交互族；D6 待裁决 |
| A12 | `animation=0` 消亡动画 | `OnDropLeaveField` `v2==0` 段整块 `IWzGr2DLayer::Animate/RemoveCanvas/GetCanvas` COM 调用 | `Drop::expire case 0` 仅 `state=PICKEDUP`，靠 48 帧淡出 | ⏳ | 原端特效无法用当前端渲染栈复刻；F 维统一 ⏳ |
| A13 | 落地/漂浮两态 | 083 池对象 `+68`/`+72` 记账 + COM 画布，无独立状态机 | `Drop.cpp:56-106` `DROPPED→FLOATING` 状态机 + `phobj.y = basey+5+(cos(moved)-1)*2.5` | ⚠️ | 两套实现范式不同；当前端自洽，不改 |
| A14 | 归属与 playerdrop | `OnDropEnterField` 里 `v108` 为 1 或 2、且 `v115[29]==0` 时的特判分支 | `DropLootHandler` 读 `owner` + `playerdrop = !read_bool`，`DropSpawn` 存字段但无归属判定 | ⚠️ | 字段已备、判定未用；D3 待裁决 |

### 3.2 B 业务 14 条（✅9 / ⚠️3 / ⏳2）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| B1 | 丢金币额度 | 083 仅 `CanSendExclRequest(200)` 节流 + 地图禁丢位检查（`SP_4558`） | 客户端 `MesoDropPacket` 无本地额度校验 | ⚠️ | s9 `MesoDropHandler` 权威：`meso>9 && meso<50001`；客户端不重复校验属正确分层 |
| B2 | 金币扣除 | 服务端扣 | `MesoDropPacket(amt).dispatch()` 三处调用（`UIItemInventory.cpp:579/:1314`、`UIMiscShells.cpp:5934`） | ✅ | 客户端只发包，扣费权威在 s9 |
| B3 | 掉落归属期 | 083 `v115[29]` 参与进入分支判定 | `DropSpawn` 有 `pickuptype` 字段但 `Drop` 未消费 | ⏳ | 归属超时表现需真机双端观感 |
| B4 | FFA droptype=2 | 083 同池无 droptype 概念 | `DropLootHandler` 读 `pickuptype` 传入 `spawn`，`Drop` 侧未分流 | ✅ | 字段透传消费在 UI 层，不属本面 |
| B5 | 爆炸掉落 | 083 `v2==4` 记账延时 + 后续爆炸 | `Drop::expire case 4`：`angle=0` + `PICKEDUP` + `vspeed=-4.5` + `type=NORMAL` | ✅ | s9 `AbstractDealDamageHandler:234` 走 4，语义与客户端一致 |
| B6 | 宠物拾取 | 083 池内 looter 绑定 + `Decode1(slot)` | `RemoveLootHandler` `if (recv.available()>0) recv.read_byte()` 吞 slot | ✅ | 与 s9 `pet=true` 才写 slot 对齐 |
| B7 | 无动画清除 | 083 `v2==1` 直删 | `Drop::expire case 1/default` → `deactivate()` | ✅ | s9 `silentRemoveItemFromMap` = `removeItemFromMap(objId,1,0)` |
| B8 | 超时消失 | 083 `v2==0` 走消亡特效 | `Drop::expire case 0` → `state=PICKEDUP` 原地淡出 | ✅ | 表现差异归 A12 ⏳，业务结果（消失）一致 |
| B9 | 换图清空 | 083 池 Dispose | `Gameplay/Stage.cpp:102 drops.clear()`；`Stage::clear()` 内同 | ✅ | 无残留 |
| B10 | 真实掉落表 | 083 `MobDropData` 数据驱动 | 台账 `docs/features/map-mob-drops/README.md` 记「接口已就绪、无真实掉落表、显式空态」 | ⏳ | 数据缺口非协议缺口，不编造数据 |
| B11 | 图标缺失时的掉落 | 083 `ICONRAW` 取不到则该 drop 不实体化 | `MapDrops::update` `else if (const ItemData&)` 为假时**静默跳过**（无 `drops.add`、无日志） | ⚠️ | 结果与 083 同为「不显示」，但当前端缺日志难排查；D9 待裁决 |
| B12 | 金币资源缺失降级 | 083 `_QueryInterface` 失败逐层早退 | `MapDrops::init()` `if (!src) return;` 优雅降级 | ✅ | `.nx` 复核 `Special/0900.img` 四档 `iconRaw` 齐备 |
| B13 | 任务道具掉落 | s9 `spawnDrop` 有 `needQuestItem` + `lockItem` 分支 | `DropLootHandler` 无任务道具判定 | ⚠️ | 锁在服务端，客户端无需同步；分列记录 |
| B14 | 拾取距离校验 | 083 `CanSendExclRequest` 节流 | 客户端不校验；s9 `ItemPickupHandler` `800×600` + `AutobanFactory.ITEM_VAC` | ✅ | 反外挂权威在服务端 |

### 3.3 C 逻辑 14 条（✅11 / ⚠️3）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| C1 | 构造 mode 未覆盖 | 083 池对象字段全量初始化 | `Drop.cpp:22-54 switch(mode)` **无 default**，`Drop.h:55 State state;` 无初值 | ⚠️ | 当前 s9 只发 `mod∈{1,2,3}` → 不可达；D7 待裁决是否加护栏 |
| C2 | DROPPED 落地 | 083 无独立状态机 | `Drop.cpp:60-75` `if (phobj.onground)` → `FLOATING` + `angle.set(0)` + `set_position(dest.x(), dest.y()-4)` | ✅ | 当前端自洽 |
| C3 | FLOATING 浮动 | 083 `MakeMoneyAnimation @0x506e62` COM 帧驱动 | `phobj.y = basey+5+(cos(moved)-1)*2.5`，`moved` 0..360 步进 0.025 | ✅ | 两套实现范式不同但曲线自洽，不改 |
| C4 | PICKEDUP 淡出 | 083 由 `+68` 时间戳驱动 | `PICKUPTIME=48` / `OPCSTEP=1/48`，`opacity<=OPCSTEP` 则 `deactivate(); return -1` | ✅ | 48 帧确定性终止 |
| C5 | expire case 0 | 083 `v2==0` 消亡 | `state=PICKEDUP`（不设 looter/vspeed） | ✅ | 与 s9 `0=expire` 一致 |
| C6 | expire case 2/4/5 | 083 2/3/5 读 cid、4 读 delay | `angle.set(0)` + `PICKEDUP` + `looter=lt` + `vspeed=-4.5f` + `type=NORMAL` | ✅ | s9 口径下 `chrId=0` 时 `looter=nullptr` 亦安全 |
| C7 | expire case 1/default | 083 `v2==1` 直删 | `deactivate()` | ✅ | 未知值兜底同 083 的 direct remove |
| C8 | `mode>1` 等价性 | — | `RemoveLootHandler` `if (mode > 1)` ⟺ s9 `animation >= 2` | ✅ | 逻辑等价已核 |
| C9 | `mode==4` 包体分叉 | `OnDropLeaveField` `v2==4` 读 `Decode2`（delay，写 `+68`） | 客户端读 `int cid`（s9 `writeInt(chrId)` 4 字节） | ⚠️ | 数值同为 4、语义不同；取 s9，D1 记录不得按 083 改 short |
| C10 | 拾取去抖 | 083 节流 `CanSendExclRequest(30)` | `find_loot_at` 命中置 `lootenabled=false`，`MapDrops::update` 每帧末复位 `true` | ✅ | 每帧闸非持久锁，语义正确 |
| C11 | 迭代中删除 | 083 ZList `RemoveAll` | `MapObjects.cpp:59-96` `iter = objects.erase(iter)` 返回下一迭代器 | ✅ | 写法正确，无失效迭代器 |
| C12 | 容器向下转型 | 083 池内同类型元素 | `MapDrops.cpp:103` `Optional<Drop> drop = drops.get(oid)` 由 `Optional<MapObject>` 经 `Optional(Optional<R>)` 做 `static_cast` 无 RTTI 校验 | ⚠️ | 容器只装 `MesoDrop`/`ItemDrop` → 恒合法；D8 记录观察项 |
| C13 | 拾取判定区域 | 083 `tagPOINT` + `dword_BF0484` 命中测试 | `Drop::bounds()` 32×32；`Rectangle::contains` 对退化矩形返 false | ✅ | 非退化矩形，判定正确 |
| C14 | 无掉落早退 | 083 池空直接返 | `find_loot_at` `if (!lootenabled) return {0,{}}`；`check_drops` `if (!loot.first) return;` | ✅ | 双早退 |

### 3.4 D 交互 14 条（✅11 / ⏳1 / ❌2）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| D1 | PICKUP 键自动拾取 | 083 `CUserLocal` 键路 → `TryPickUpDrop` | `Gameplay/Stage.cpp:304-305` `if (player.is_key_down(KeyAction::Id::PICKUP)) check_drops();` | ✅ | 主路径一致 |
| D2 | 拾取键可用态门控 | 083 有 `IsStun`/`IsDarkSight` 前置 | `Stage.cpp:275-306` 仅当 `!is_climbing && !is_sitting && !is_attacking` 时处理 | ✅ | 门控等价（不含 stun/darksight，见 A10） |
| D3 | SoftKey 单引号拾取 | 083 无对应 | `UIMiscShells.cpp:4455-4474` `case '\''` → `find_loot_at` → `PickupItemPacket` | ✅ | 调试键，`r134` 已记 |
| D4 | SoftKey `K` 拾取 | 083 无对应 | `UIStatusBar.cpp:1369-1388` `keycode=='K'` 同构 | ✅ | `r183`/`r789` 已记 |
| D5 | GameMenu 脚下拾取 | 083 无对应 | `UIMiscShells.cpp:6272-6290` `UIGameMenu` 同键同构 | ✅ | `r788` 已记 |
| D6 | 背包丢金币 | 083 `CUIItem::OnDropMoney @0x81d996` → `CUtilDlgEx` 询问 → `SendDropMoneyRequest` | `UIItemInventory.cpp:579` `BT_COIN` SoftKey 金额 → `MesoDropPacket` | ✅ | `r107` 已记；询问框差异归 D11 |
| D7 | 背包 `Y` 丢金币 | 083 无对应 | `UIItemInventory.cpp:1314` SoftKey 金额 → `MesoDropPacket` | ✅ | `r180` 已记 |
| D8 | 背包 `D` 丢道具 | 083 `CDraggableItem::PutItem` | `MoveItemPacket` dst=0（`r180`） | ✅ | 结果同为落地，路径不同（归 A11 ❌） |
| D9 | 状态栏拾取最近 | 083 无对应 | `UIStatusBar.cpp` `K` 键（`r183`） | ✅ | 同上 |
| D10 | 拾取 tip | 083 `CHATLOG_ADD(SP_1245_PICKUPITEM)` 音效，无 tip | 客户端仅 `Sound(Sound::Name::PICKUP).play()`（`MapObjectHandlers.cpp:890`），无 `map_tip` | ⏳ | `r471` 记「mode>1 且本机 → map_tip Pickup」未实现；需真机确认观感 |
| D11 | 丢金币询问框 | `CUIItem::OnDropMoney` `CUtilDlgEx::SetUtilDlgEx(a2:2,...)` 带 `SP_5280` 长确认文案 | `UIEnterNumber`（`UINotice.cpp`，round44 已修空串/溢出）仅数字输入，无长文案 | ❌ | 文案层缺失；D11 待裁决 |
| D12 | 拖拽投掷 | `CWndMan::BeginDragDrop`/`EndDragDrop` + `CInputSystem::SetCursorState(11)` 光标态 | 无实现 | ❌ | 同 A11；D6 待裁决 |
| D13 | 拾取需面向/点击 | 083 纯键盘 | 纯键盘，无鼠标 | ✅ | 一致 |
| D14 | 同一帧不重复发包 | 083 节流 | `lootenabled` 每帧复位一次 | ✅ | 一帧最多一次 |

### 3.5 E 排版 16 条（✅12 / ⏳4）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| E1 | 掉落绘制层 | 083 `IWzGr2DLayer` 分层 | `MapDrops::draw(layer, ...)` → `MapObjects::draw` 按 `phobj.fhlayer` | ✅ | 层序一致 |
| E2 | 视口剔除 | 083 画布裁剪 | `MapObjects.cpp:30` `CULL_MARGIN=512`，锚点外 512px 内仍画 | ✅ | 32×32 掉落不会被误剔 |
| E3 | 四档金币图标尺寸 | 083 各档 `iconRaw` 独立位图 | `mesoicons[4]` 各取 `iconRaw`；`.nx` 仅能核 child 数 4 | ⏳ | 像素尺寸需 IMG 内容层恢复 |
| E4 | 绘制锚点 | 083 COM canvas 原点 | `phobj.get_absolute(viewx, viewy, alpha)` | ✅ | 含插值 |
| E5 | 金币帧延迟 | 083 `MakeMoneyAnimation` 内帧时间 | `Animation` 帧 `delay` 由资源定义 | ⏳ | 需资源层 |
| E6 | 物品图标原始尺寸 | 083 `ICONRAW` 原图 | `ItemData::get_icon(true)` 取 raw | ⏳ | 需资源层 |
| E7 | 落地前旋转 | 083 无（COM 帧） | `SPINSTEP=0.2f`，`angle += SPINSTEP` 每帧 | ✅ | 当前端自洽 |
| E8 | 淡出透明度 | 083 时间戳驱动 | `opacity -= 1/48`，`Linear<float>` 插值 | ✅ | 48 帧线性 |
| E9 | 与角色前后遮挡 | 083 层序 | `fhlayer` 参与 `MapObjects` 分层 | ✅ | 一致 |
| E10 | 拾取飞行水平速度 | 083 池内位移 | `phobj.hspeed = looter->hspeed/2.0 + (hdelta-16.0)/PICKUPTIME` | ✅ | 含主人速度分量 |
| E11 | 起点到终点水平速度 | 083 池内位移 | `phobj.hspeed = (dest.x()-start.x())/48` | ✅ | 48 帧对齐 |
| E12 | 落地落点 | 083 池内 dest | `set_position(dest.x(), dest.y()-4)`；构造亦 `start.y()-4` | ✅ | 统一 -4 偏移 |
| E13 | 档位选择顺序 | `sub_506BFE` 先 `<50` 再 `>=100` 再 `>=1000` | `itemid>999 ? BAG : >99 ? BUNDLE : >49 ? GOLD : BRONZE` | ✅ | 判定顺序等价 |
| E14 | 掉落与 UI 遮挡 | 083 分层 | `Layer::Id` 中 drop 在角色层，UI 独立绘制序 | ✅ | 一致 |
| E15 | 抬头低头不裁剪 | 083 镜头跟随 | `viewx/viewy` 由 camera 提供，cull 用同一对 | ✅ | 一致 |
| E16 | 相机缩放换算 | 083 固定比例 | 当前端无相机缩放，`get_absolute` 仅平移 | ⏳ | 若后续接缩放需复算，记录 |

### 3.6 F 样式 12 条（⏳12，IMG 内容层阻塞，r03 · 5 降级清单）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| F1 | 金币位图配色 | `09000000..03/iconRaw` 位图 | 同路径 `.nx` 可定位但无像素 | ⏳ | IMG 内容层 `IMG 解析失败` |
| F2 | 物品图标配色 | `Item/<id>/info/iconRaw` | `ItemData` 取 raw | ⏳ | 同上 |
| F3 | 掉落投影 | 083 逐 drop 画布无独立阴影层 | 无 | ⏳ | 需像素核对 |
| F4 | 金币高光 | 位图内置 | 位图直绘 | ⏳ | 需像素核对 |
| F5 | 图标描边 | 位图内置 | 位图直绘 | ⏳ | 需像素核对 |
| F6 | 抗锯齿 | 083 GDI/Gr2D 采样 | 当前端 GL 纹理采样 | ⏳ | 需交叉截图 |
| F7 | 旋转时模糊 | 083 COM 旋转 | `angle` + `Texture::draw` 无动态模糊 | ⏳ | 表现差异需真机 |
| F8 | 淡出曲线形状 | 083 时间戳线性 | `opacity` 线性 48 帧 | ⏳ | 需逐帧对比 |
| F9 | 四档图标相对大小 | 各档位图独立 | 各档独立位图 | ⏳ | 需像素核对 |
| F10 | 层内叠加顺序 | 083 Z 序 | 按 `fhlayer` 再按 oid | ⏳ | 需真机 |
| F11 | 拾取拖尾 | 083 无 | 无 | ⏳ | 一致但未实证 |
| F12 | 金币数值不显示 | 083 掉落物不显示数字 | 当前端亦不显示 | ⏳ | 一致但未实证 |

### 3.7 G 协议/纪律 14 条（✅12 / ⚠️2）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| G1 | `268` 读序 | `Decode1 mode` + `Decode4 oid` + ... | `DropLootHandler` `mode→oid→meso→itemid→owner→pickuptype→dropto` | ✅ | 与 s9 `dropItemFromMapObject` 同序 |
| G2 | `269` 读序 | `Decode1 mode` + `Decode4 oid` | `RemoveLootHandler` `mode→oid→(mode>1) cid→(avail) slot` | ✅ | 与 s9 `removeItemFromMap` 同序 |
| G3 | 过期时间 8 字节 | 083 无此字段（v83 服务端另包） | `if (!meso) recv.skip(8);` 对齐 s9 `addExpirationTime` `writeLong` | ✅ | 8 字节 |
| G4 | `meso` 标志跳读 | — | `bool meso = recv.read_bool()`；`!meso` 才 `skip(8)` | ✅ | 与 s9 `writeBool(meso>0)` 对齐 |
| G5 | `mode!=2` 追读起点 | 083 `v108` 分支 | `if (mode != 2) { dropfrom = read_point(); skip(2); }` | ✅ | 对齐 s9 `if (mod != 2) writePos + writeShort(0)` |
| G6 | playerdrop 反相 | — | `bool playerdrop = !recv.read_bool();` 对齐 s9 `writeByte(isPlayerDrop() ? 0 : 1)` | ✅ | 反相正确 |
| G7 | 尾吞余量 | 083 `DecodeBuffer` 一次取 | `while (recv.available()) recv.skip_byte();` 两 handler 均有 | ✅ | 防错位 |
| G8 | 越界保护 | 083 `CInPacket` 自带边界 | `InPacket::skip` 越界抛 `PacketError`；`PacketSwitch::forward` `catch (const PacketError&)` 只 warn | ✅ | 不崩不重连 |
| G9 | `available()` 类型 | — | `Net/InPacket.h:37 bool available() const;`，本面 3 处当计数用（`>0` / `while`）语义碰巧正确 | ⚠️ | 全仓 393 处待裁决（dev-plan §3.1 / PLT §27-A），本面不单独改 |
| G10 | `DROP_ITEM mod` 取值 | 083 `OnDropEnterField` 分支 `v108==1/2` 等 | s9 实际发 `{1,2,3}`（`spawnDrop`/`spawnMesoDrop` 恒 1、`MapItem.java:253` 2、`disappearing*` 3） | ⚠️ | 3 = 消失掉落（无 dropfrom），当前端 `mode!=2` 会多读 2 字节由尾吞兜住；D10 记录 |
| G11 | 083/095 禁引 | — | 本面全程只用 `Angel.i64`，未引 `Maplestory95.i64` / `port-095` | ✅ | 纪律合规 |
| G12 | 只读纪律 | — | 本面未改 `Gameplay/` `Net/` `IO/` 任何源码 | ✅ | 校准文档性质 |
| G13 | EA 字节级一致 | — | `18/18 EAs identical`，`missing=0` | ✅ | 第三次独立复证同二进制 |
| G14 | IMG 门禁留证 | — | 8 次 `IMG 解析失败` 原文 + `sha256_16 268a8407a47ee76c` / `iv TSPHKw==` / 65536B key 规格全留 | ✅ | `no_fake_green` |

### 3.8 判定汇总（脚本重数校对）

| 维 | ✅ | ⚠️ | ⏳ | ❌ | 合计 |
| --- | --- | --- | --- | --- | --- |
| A 功能 | 8 | 3 | 1 | 2 | 14 |
| B 业务 | 9 | 3 | 2 | 0 | 14 |
| C 逻辑 | 11 | 3 | 0 | 0 | 14 |
| D 交互 | 11 | 0 | 1 | 2 | 14 |
| E 排版 | 12 | 0 | 4 | 0 | 16 |
| F 样式 | 0 | 0 | 12 | 0 | 12 |
| G 协议/纪律 | 12 | 2 | 0 | 0 | 14 |
| **合计** | **63** | **11** | **20** | **4** | **98** |

> `63+11+20+4=98`；无 ⊘。以 `python tools/face12_readme_audit.py` 输出的 `OVERALL` 为准。

## 4. 十条硬发现

1. **`18/18 EAs identical`（`missing=0`）**：Drop 面第三次独立复证 Angel(13339) 与 BeiDou(13337) 同二进制；BeiDou 侧 18 个 EA 全部退化为 `sub_XXXXXX`（`real_named=0`），Angel 侧 18 funcs / 9 classes 全具名 → Angel 只补语义层，不带额外功能。
2. **本面唯一 c2s 出站 = 94 (0x5E)**：`CWvsContext::SendDropMoneyRequest @0xa23de5` 内 `COutPacket(94)` + `Encode4(update_time)` + `Encode4(amount)` + `CanSendExclRequest(200)`；捡物/宠物捡物是 s9 侧新增通道（`PICKUP_ITEM 202` / `PET_LOOT 170`），083 走 `CDropPool::TryPickUpDrop` 内联。
3. **`animation=4` 是 083 与 s9 的语义分叉点**：083 `OnDropLeaveField` `v2==4` 读 `Decode2` 并写 `*(v4+68)=update_time+delay`、`*(v79+72)=4`（原端爆炸延时记账）；s9 `removeItemFromMap` 在 `animation>=2` 恒写 4 字节 `chrId`（`AbstractDealDamageHandler:234` 传 0）。当前端读 `int cid` 是 s9 正确解，**不得按 083 改 short**。
4. **金币四档阈值逐档等价**：`sub_506BFE @0x506bfe` = `a1<50 → 0`；`a1>=100 → (a1>=1000)+2`；否则 `1`，即 `<50 / 50..99 / 100..999 / >=1000`；当前端 `MapDrops.cpp:78` `>999 BAG : >99 BUNDLE : >49 GOLD : BRONZE` 完全等价。
5. **`DROP_ITEM mod` s9 实际发 `{1,2,3}`**：`spawnDrop`(`MapleMap.java:1152`) 与 `spawnMesoDrop`(`:1171`) 恒 `(byte) 1`、`MapItem.java:253` 传 `2`、`disappearingItemDrop`(`:1186`)/`disappearingMesoDrop`(`:1198`) 传 `3`（消失掉落，无 dropfrom）。当前端 `if (mode != 2)` 对 mod=3 会多读 2 字节（`read_point`）——由尾 `while (available()) skip_byte()` 兜住不错位，但 `dropfrom` 值无意义；记 D10。
6. **`RemoveLootHandler` 的 `mode > 1` 与 s9 `animation >= 2` 完全等价**，`if (recv.available() > 0) read_byte()` 与「仅 pet=true 写 slot」在尾吞余量下行为一致；该处 round48 已修，本面复核为已闭。
7. **083 侧 9 个函数属拖拽族**（`CDraggableItem::OnDropped @0x4ef140`、`CDraggableMenu @0x4f91e4`、`CDraggableSkill @0x4faa22`、`CWnd::OnDragDrop @0x4243fe`、`CWndMan::BeginDragDrop @0x9e353d` / `EndDragDrop @0x9e37c2` 等）：是 `kws=(Drop/PickUp/Loot)` 宽口径命中，当前端零实现 → 两条 ❌ 的真正来源。
8. **booby trap 全链 083 独有**：`CUserLocal::CheckBoobyTrapPickUpRequest @0x95d7b7`（`BYTE2(a2)==82` / `==54` 双分支 + `IsDarkSight`/`IsSneak`/`IsStun` 前置）→ `CWvsContext::CheckBoobyTrapPickUpRequest @0xa32914` → `CDropPool::BoobyTrapCheckPickupItem @0x5081b0`（`SendBoobyTrapAlert(1)`），当前端零实现。
9. **`OnDropEnterField` 与当前端 `DropLootHandler` 字段序一致**（`Decode1 mode` → `Decode4 oid` → `Decode1 meso` → `Decode4 itemid` → `Decode4 owner` → `Decode1 pickuptype` → `Decode2×2 dropto` → `Decode4 dropper` → `Decode2×2 dropfrom`(mode≠2) → `Decode2 Fh` → `DecodeBuffer` 8B 过期时间(`meso==0`) → `Decode1 playerdrop` 反相），逐字段对齐 s9 `dropItemFromMapObject`。
10. **orange-wz 门禁本面按 `AGENTS.md` ACTIVE 真闭合**：结构层 `Data/Item/Special` 4 真 `.img` WORKS，内容层 8 次 `IMG 解析失败`（GBK 原文），命中回退后合规走 `.nxcache/Item.nx`（`Special` 4 子节点、`0900.img` 下 `09000000..03` 各有 `iconRaw`），`no_fake_green`。

## 5. 四态（详见 `notes/r05-drop-four-state-audit.md`）

| 态 | 组数 | 摘要 |
| --- | --- | --- |
| 销案 | 6 | S1 `268/269` 数值；S2 `94` 包体字段序；S3 `mode>1` ⟺ `animation>=2`；S4 金币四档阈值；S5 拾取优先级与 s9 对齐；S6 尾吞余量防错位 |
| 推翻 | 1 | O1「083 与 s9 animation=4 同语义」——083 读 `Decode2` delay、s9 写 4 字节 cid |
| 新增 | 7 | N1 `18/18 EA identical`；N2 唯一 c2s `94`；N3 `DROP_ITEM mod∈{1,2,3}`；N4 booby trap 全链 083 独有；N5 拖拽族 9 函数 083 独有；N6 `sub_506BFE` 四档阈值反编译；N7 orange-wz 结构/内容分层结论 |
| 遗留 | 5 | L1 真机双端 hex；L2 F 维 12 项像素（IMG 内容层）；L3 `animation=0` 消亡特效复刻；L4 拾取 tip（`r471`）未落地；L5 真实掉落表（`map-mob-drops` 空态） |

## 6. 决策点 D1–D14（推荐默认：保持 s9 优先、不补 083 发包、不改协议、保留资源降级、不建提交）

| # | 决策点 | 推荐默认 | 依据 |
| --- | --- | --- | --- |
| D1 | 是否按 083 把 `269` 的 `animation=4` 改成读 `Decode2` | 不改 | s9 `removeItemFromMap` 写 4 字节 cid；改 short 会立刻错位（C9/G1） |
| D2 | 是否补 `animation=0` 的 `IWzGr2DLayer` 消亡动画 | 不补 | 当前端无 083 COM 画布栈；现方案 48 帧淡出语义等价（A12/C5） |
| D3 | 是否实现掉落归属超时（`pickuptype`/`playerdrop` 判定） | 待裁决 | 字段已透传、判定未用；权威在服务端（A14/B3） |
| D4 | 是否把 `MapDrops.cpp:80` 的静默丢弃加日志 | 建议加 DEBUG 日志 | 缺 icon 时无痕难排查；加日志零行为变化（B11） |
| D5 | 是否立项 booby trap 全链（3 函数 + 告警包） | 挂 P2 | 083 独有、s9 有 `SendBoobyTrapAlert` 通道但当前端零实现（A10） |
| D6 | 是否立项拖拽丢弃族（9 函数 + `SetCursorState(11)`） | 挂 P3 | 交互面大，当前端 SoftKey 已能达成结果（A11/D12） |
| D7 | 是否给 `Drop::Drop` switch 补 `default` 护栏 | 建议补（`state=DROPPED`） | s9 当前只发 `{1,2,3}` 不可达，但未初值化是隐患（C1） |
| D8 | 是否给 `Optional<Drop>` 向下转型加校验 | 不改 | 容器只装 `MesoDrop`/`ItemDrop`，恒合法；改则动 `Template/Optional.h`（C12） |
| D9 | 是否补丢金币长确认文案（`SP_5280`） | 待裁决 | 当前 `UIEnterNumber` 已可用，仅文案差异（D11） |
| D10 | 是否处理 `DROP_ITEM mod=3`（消失掉落）的 `dropfrom` 语义 | 待裁决 | 目前靠尾吞兜住不错位，但 `dropfrom` 无意义（G10/#5） |
| D11 | 是否补拾取 `map_tip`（`r471`） | 待真机确认 | 083 只有 `SP_1245_PICKUPITEM` 音效、无 tip（D10） |
| D12 | 是否补 `animation=4` 的延时爆炸帧 | 不改 | s9 `chrId=0`，原端延时只服务原端自己的动画（C6/B5） |
| D13 | 是否把 F 维 12 项按 `.nx` 像素补齐样式结论 | 挂 P2 | IMG 内容层 `IMG 解析失败`，`.nx` 只能给结构不能给像素（F1–F12） |
| D14 | 是否建 git commit / 改源码 | 否 | 本面为 P0 纯文档，全程只读 |

## 7. 一次性 Plan

| 阶段 | 内容 | 状态 |
| --- | --- | --- |
| P0 | 纯文档闭环：本 README + r01/r03/r05 + 6 evidence + `AUDIT OK` + present + 答复 | ✅ 本轮完成 |
| P1 | D1–D14 裁决后才动源码（推荐默认全「保持 s9 优先」+ D4/D7 两条低风险护栏） | ⏸ 待用户拍板 |
| P2 | orange-wz 内容层恢复 + 真机 c2s `94`/`269` hex + booby trap 立项 + F 维 12 项像素 + `build_smoke.bat` | ⏸ 挂起 |
| P3 | 拖拽丢弃族 + 真实掉落表（`map-mob-drops`）接通 | ⏸ 后续面 |

## 8. 限制与未知

- **子串假阳性**：`kws=(Drop/DROP/drop/PickUp/Pickup/PICKUP/Loot/LOOT/loot)` 命中 9 个拖拽族函数，已单列，不进主链结论；后续面沿用须加 `Drag` 负向过滤。
- **IMG 内容层未恢复**：F 维 12 项诚实 ⏳，`no_fake_green`（`IMG 解析失败` 不计 OK）。
- **无具名阈值函数**：`sub_506BFE` 在 Angel 侧未具名，本次靠 `GetMoneyIcon` 调用点反查 + 单独反编译才取到四档；`facts` 自动抽取抽不到数值分支。
- **095 禁引**：`Maplestory95.i64` / `port-095/**` 不作 083 依据。
- **真机缺口**：无双端 `269` animation=4 抓包对照、无 `animation=0` 消亡特效观感、无拾取 tip 实证。
- **decompile 形态**：`angel-drop-decompile.json` 的 `functions` 值为双编码 JSON 字符串（`json.loads(v)['code']`），脚本已处理；`physical_keys=18`，无 output-ref 残留。

## 9. 证据索引与复现

### 9.1 文件清单

| 文件 | 说明 |
| --- | --- |
| `evidence/angel-drop-symbol-inventory.json` | 18 funcs / 9 classes 具名清点（13339） |
| `evidence/angel-beidou-drop-ea-crosscheck.json` | `18/18 EAs identical`，`missing=0` |
| `evidence/angel-drop-decompile.json` | 18 函数全量 Hex-Rays（双编码，需内层解析） |
| `evidence/angel-drop-facts.md` | 310 行语义抽取 + c2s opcode 汇总（仅 `94`） |
| `evidence/orange-wz-drop-structure.json` | 结构层 4 真 `.img` + 8 次 `IMG 解析失败` 原文 |
| `evidence/nx-drop-content-probe.json` | `.nx` 回退：`Special` 4 子节点、`0900.img` 四档 `iconRaw` |
| `notes/r01-angel-native-drop-facts.md` | 一手事实基线（七条硬结论 + 三联门禁） |
| `notes/r03-img-gate-fallback.md` | orange-wz 门禁实证与合规回退（key 规格 + 报错原文） |
| `notes/r05-drop-four-state-audit.md` | 既有笔记四态审计（销案/推翻/新增/遗留） |

### 9.2 复现命令

```bash
python tools/ida_angel_drop_ea.py
python tools/ida_angel_drop_crosscheck.py
python tools/ida_angel_drop_byteaudit.py
python tools/ida_angel_drop_decompile.py
python tools/ida_angel_drop_facts.py
python tools/face12_gate_and_drop_probe.py
python tools/face12_readme_audit.py
```

## 10. 性质标注

本文档是**校准 / 对照 / 决策点 / 一次性 Plan**，不是实现提交。全程只读：未改游戏源码 / 协议 / WZ / IMG / XML，未建 git commit，未触发 live 改写安全流。P1 实现须等用户对 D1–D14 拍板；P2 挂起等 orange-wz 内容层恢复与真机；P3 后续面。`goal-11388c42` 保持 active/armed。
