# Fusion Anvil（融合外观）移植契约 — v83 DLL 注入版 → 本端 C++ x64 + BeiDou-Server_s9

> 只读研究产出（2026-11-23）。目标是把 `E:\MXD\扩展改动\融合外观\Client-v83` 的
> `Kaentake.dll`（v83 32 位注入版）的**业务语义**移植到本端 C++ x64 客户端
> (`E:\project\MapleStory-Client`) + s9 服务端 (`E:\project\BeiDou-Server_s9`)。
> **本文只给契约与建议，不改任何代码。**

取证来源（均为逐行读过的原文，行号见文末「证据索引」）：

| 来源 | 路径 |
|---|---|
| v83 客户端主逻辑 | `E:\MXD\扩展改动\融合外观\Client-v83\src\fusionanvil.cpp`（677 行，全读） |
| 他人染色刷新/延迟 Apply | `src\avatarmod.cpp` / `avatarmod.h`（75 / 8 行，全读） |
| HSL 染色引擎 | `src\equipdye.cpp` / `equipdye.h`（360 / 91 行，全读） |
| 七彩棱镜窗口 | `src\coloringprism.cpp` / `coloringprism.h`（303 / 78 行，全读） |
| 常量 | `src\constants.h`（14 行，全读） |
| 服务端对接文档 | `E:\MXD\扩展改动\融合外观\FUSION_ANVIL_SERVER-SIDE.md`（253 行，全读） |
| UI 素材 | `E:\MXD\扩展改动\融合外观\UIWindow.img`（203321 B，加密 IMG 二进制） |
| WZ 包 | `src\Custom.wz`（45859 B，`PKG1` 头，Package file v1.0 2002） |
| 本端客户端 | `Character\Inventory\{Equip,Inventory}.{h,cpp}`、`Character\Look\{CharLook,CharEquips}.{h,cpp}`、`Character\Player.cpp`、`Net\Handlers\Helpers\ItemParser.cpp`、`Net\Packets\{CashItemPackets,ColorPrismPackets}.h`、`Net\OutPacket.h` |
| s9 服务端 | `gms-server\src\main\java\org\gms\{client\inventory\Equip.java, client\inventory\ItemFactory.java, util\PacketCreator.java, net\server\channel\handlers\UseCashItemHandler.java, server\coloring\ColoringPrism*.java}` |

> 说明：按仓库 `AGENTS.md`，WZ/IMG 节点查询应走 orange-wz MCP；**本子代理会话未注册该 MCP 工具**
> （工具发现返回无匹配）。本文对 IMG/WZ 只做二进制封装确认 + 依据源码中硬编码的 WZ 路径列出
> 素材契约，**未解析节点内容**；节点级核对留给主代理在挂有 orange-wz 的会话中完成。

---

## 1. Opcode 与包结构

### 1.1 C→S `UserConsumeCashItemUseRequest`（opcode `0x4F` = 79）

**关键更正（先记结论）**：`0x4F` 是 **v83 的 opcode 值**。本端 `Net\OutPacket.h:131`
登记的 `USE_CASH_ITEM = 79` 也是同一个十进制值 —— 79 十进制 == `0x4F`，两者一致，
不存在版本差异。s9 服务端 `UseCashItemHandler.handlePacket`
（`UseCashItemHandler.java:104-106`）读前两个字段：

```
short position      // 现金道具在 CASH 背包的槽位
int   itemId        // 5900000
```

**v83 注入版新增的 layout**（`fusionanvil.cpp:478-485` 的 `SendRequestPacket`）：

```
short position      = m_nItemPOS（打开窗口时双击的现金道具槽）
int   itemId        = 5900000
short basePos       = pos0  // 右槽（功能/保留件）的槽位，wire 值
short skinPos       = pos1  // 左槽（外观/被消耗件）的槽位，wire 值
```

服务端文档 `FUSION_ANVIL_SERVER-SIDE.md:248-253` 的 opcode 表把它写成
`short pos, int itemId, short basePos, short skinPos`，与其 handler 代码
（`:147-148`）变量命名正好相反 —— handler 里先 `readShort()` 的是 **skinPos**，
后一个是 **basePos**（`:147 skinPos = p.readShort(); :148 basePos = p.readShort();`）。

> ⚠️ **移植要点 / 需复核的矛盾点**：客户端 `SendRequestPacket` 先编码 `pos0`（数组 index 0
> = 左槽 = skin），再编码 `pos1`（index 1 = 右槽 = base）；而服务端把第一个 short 解释为
> skin、第二个解释为 base。两者在**槽序上是一致的**（skin 在前，base 在后），
> 但字段名 `basePos/skinPos` 在 opcode 表与客户端注释里标反了。**按「wire 序 = skin 先、base 后」
> 实现即可，不要按字段名脑补。**

本端风格建议（`Net\Packets\CashItemPackets.h` 已有同 opcode 的一堆类，如
`UseViciousHammerPacket:13-25`、`UseItemProtectorPacket:44-54`）：新增
`UseFusionAnvilPacket : OutPacket(OutPacket::Opcode::USE_CASH_ITEM)`，
`write_short(cashSlot); write_int(itemId=5900000); write_short(skinSlot); write_short(baseSlot);`。
本端 `write_short` 对已穿戴槽直接写 wire 负数（`ColorPrismPackets.h:12,60` 同款）。

### 1.2 S→C 外观广播：在 equip 块尾部追加 `anvilItemId`

服务端 `PacketCreator.java:592-643` 的单个 equip 编码顺序（s9 现行）：

```
byte   upgradeSlots
byte   level
short  str,dex,int,luk,hp,mp,watk,matk,wdef,mdef,acc,avoid,hands,speed,jump (15×short)
string owner
short  flag
[ cash: 10×byte 0x40 | 非cash: byte0, byte itemLevel, int expNibble, int vicious, long0 ]
long   getTime(-2)
int    -1
short  potential1, potential2, potential3, bonusPotential1, bonusPotential2  (5×short)
short  spark, soul
byte   starForce
```

**融合外观新增**：在上面**最末尾**再写 `int anvilItemId`（0 = 无）—
`FUSION_ANVIL_SERVER-SIDE.md:100-112`。

客户端 `ItemParser.cpp:68-157` 的 `add_equip` 与之逐字段对齐，最后三段是：
- `:133-140` 版本门控 `recv.length() >= 10` → 读 5×short 潜能；
- `:150-155` 门控 `recv.length() >= 5` → 读 spark/soul/short、starForce/byte；
- **新增**：门控 `recv.length() >= 4` → 读 `anvilItemId`（int），传给
  `inventory.add_equip(...)`；`Equip` 类（`Equip.h:42-43, 70-92`）加成员
  `int32_t anvil_item_id` 并带默认值参数，保持既有调用点不变（与 spark/soul/star_force 同款）。

> 门控必须用 `recv.length()`（剩余字节数），**不能**用 `available()` —— 后者返回 bool，
> `available() >= N` 恒 false，历史上已导致整包错位（`ItemParser.cpp:129-132, 148-149` 有血泪注释）。

### 1.3 其它 opcode（仅记录，不混用）

- `0x11D` = 七彩棱镜 `COLORING_PRISM_ACTION`（本端 `OutPacket.h:327-331`，
  `ColorPrismPackets.h:1-14`）；与融合外观**不是同一个功能**，见 §6。
- 角色外观广播 `addCharEquips`（`PacketCreator.java:295-333`）：`(byte pos, int itemid)* + 0xFF`
  两段（mask/masked），末尾 `cWeapon int + 3×pet int`。融合外观时把写入的 `itemid`
  从 `getItemId()` 换成 `getAnvilItemId()`（非零时），**仅视觉 int 替换，属性不动**。

---

## 2. 数据库字段与服务端模型

按 `FUSION_ANVIL_SERVER-SIDE.md:19-94`：

| 项 | 值 |
|---|---|
| 新列 | `inventoryequipment.anvilItemId INT NOT NULL DEFAULT '0'`，迁移脚本 `025-fusion-anvil.sql`，位置 `AFTER ringid` |
| 建表 | `003-inventory.sql` 的 `CREATE TABLE inventoryequipment` 内 `ringid ... , anvilItemId INT NOT NULL DEFAULT '0',` |
| Java 模型 | `Equip.java` 加 `private int anvilItemId = 0;`，`copy()` 里 `ret.anvilItemId = anvilItemId;`，getter/setter |
| 持久化 | `ItemFactory.loadEquipFromResultSet` 加 `equip.setAnvilItemId(rs.getInt("anvilItemId"));`；两处 `INSERT` 占位符 +1 并绑定 `setInt(24, ...)` |

**s9 现状差异（重要，别照抄文档的占位符数字）**：s9 已按显式列名 INSERT
（`ItemFactory.java:50-62`），当前 31 个占位符，列清单到 `starForce` 结束，
`ringid` 在第 23 位、`potential1..bonusPotential2` 在 24-28、`spark/soul/starForce` 在 29-31
（`ItemFactory.java:178-188`）。所以 s9 的移植应当是：列清单加 `` `anvilItemId` ``、
占位符 **31→32**、`setEquipInsertParams` 末尾加 `ps.setInt(32, equip.getAnvilItemId());`，
load 侧在 `setRingId(rs.getInt("ringid"))`（`:151`）附近加
`equip.setAnvilItemId(rs.getInt("anvilItemId"))`。文档里「24 占位符」是 Cosmic 旧树口径，
**不适用于 s9**（s9 已经历潜能 + spark/soul/starForce 两次扩列）。

---

## 3. base / skin 双选 UI 流程（纯业务语义，可移植）

窗口类 `CUIFusionAnvil : CWnd`（`fusionanvil.cpp:157-211`），素材全部来自
`UI/UIWindow.img/Synthesizing/…`（`:249-269`）：

- 背景 `backgrnd` / 内框 `backgrnd1` / 标题 `SubTitle`
- 按钮 `BtOk/{normal,pressed,disabled,mouseOver}/0`、`BtCancel/...`、`BtExit/...`
- 装饰动画 `Effect/0..11`（每 100ms 一帧循环，`:331`）
- 音效（leaf 名，前置 `Sound.wz/UI.img/`）：`MenuUp`（打开）、`BtMouseClick`、`BtMouseOver`、
  `DragEnd`、`anvil`（确认发包）、`MenuDown`（退出）

窗口 186×195，居中；两个槽命中矩形 `{37,98,80,141}`（左）与 `{106,98,149,141}`（右），
图标 draw 基线 y=135、x 中心 42/111（`:239-243, 287-293, 354-367`）。

**打开触发链（三处 hook 全部为 v83 地址，语义可移植）**：
1. `CWvsContext::SendConsumeCashItemUseRequest` hook（`:564-578`）：itemId==5900000 时
   不发包，改为 `new CUIFusionAnvil(nPOS, nItemID)`；已有实例则先销毁再（下次点击）重开，防堆叠。
2. `CWvsContext::SendEtcCashItemUseRequest` hook（`:585-593`）：部分 v83 现金道具走此路径，
   同样拦截 5900000（**本端无此分支，可忽略**）。
3. `get_consume_cash_item_type` hook（`:602-608`）：v83 内建表里没有 5900000 → 双击被吞；
   hook 返回非零 token 让客户端路由到 SendConsume 路径（**本端无此查表机制，可忽略**）。

**拖放与校验**（`CDraggableItem::OnDropped` hook `:626-657` → `PutItem :438-469`）：
- `pTo->IsKindOf(&CUIFusionAnvil::ms_RTTI)` 判定落到本窗口（自带 RTTI，因为 v83 的
  `CWnd::ms_RTTI_CWnd` 指向字符串常量，走链会崩 —— 注入版特有的坑，本端不要复刻）。
- 落点 `rx >= 93`（窗口半宽）→ index 1（右槽/base），否则 index 0（左槽/skin）。
- 接受**已穿戴（slot<0）与背包（slot>0）**两种装备；服务端按符号解析库存类型。
- 校验：`get_bodypart_from_item(itemId, gender=2, &bodyPart, 0)` 必须成功；
  两件 bodyPart 必须相同；若两者都是武器，`get_weapon_type` 必须相同；两件 itemId 不得相同。
  全通过才点亮 OK（`m_bBtOkEnable=1`）。
- 按钮态：disabled > pressed > hover > normal；Cancel/Exit 只关窗，OK 才发包。

**发包**（`:471-487`）：`play_ui_sound(L"anvil")` → `COutPacket(0x4F)` 写
`short cashSlot, int 5900000, short skinPos, short basePos` → 直接
`CClientSocket::SendPacket` → `Destroy()`。

**Server 端消费**（`FUSION_ANVIL_SERVER-SIDE.md:131-198`）：
`itemType = 5900000/10000 = 590` 分支；读 skinPos、basePos；按符号选
`EQUIPPED`/`EQUIP` 库存取件；两者必须是 `Equip`、itemId 不同、`itemId/10000` 相同
（同部位 + 同武器子型，如 1302 单手剑 ≠ 1402 双手剑）；**skin 必须在 EQUIP 背包**
（已穿戴需先卸下，`:177-183`）；`baseEquip.setAnvilItemId(skinEquip.getItemId())`；
`ModifyInventory(3, base)` + `ModifyInventory(0, base)` 刷新 + `equipChanged()`；
`InventoryManipulator.removeFromSlot(EQUIP, skinPos, 1, false)` 消耗 skin；
`remove(c, position, itemId)` 消耗现金道具；`enableActions()`。

---

## 4. 外观广播：服务端把 base 当 skin 发

**核心语义**（`FUSION_ANVIL_SERVER-SIDE.md:114-129`）：角色选择画面、野外其他玩家、
自己换装后的 avatar —— 所有 `addCharEquips` 输出里，若该装备 `anvilItemId != 0`，
把渲染用的 itemId 替换为 `anvilItemId`；**stats / 强化 / ringId / 真实 itemId 全保留**
（真 itemId 仍在 equip 块和 inventory 里，只是「外观 int」被调包）。

客户端侧对应两条路径（v83 注入版是两个 hook）：
1. **解码**：`GW_ItemSlotBase::Decode` post-hook（`:499-516`）——原生解码后，
   若 `GetType()==1`（Equip），多读一个 `int` 写进 `GW_ItemSlotEquip.nAnvilItemID`（结构偏移 `0xF9`）。
2. **渲染**：`AvatarLook::Load` post-hook（`:527-552`）——遍历 51 个 equipped 槽
   （`apEquipped + 8*i + 4`），对每件 Equip 读 `nAnvilItemID`，
   非零则覆盖 `anHairEquip[i-1]`（数组在 `AvatarLook+0x1D`，51 项）。
   即「把 base 的外观槽写成 skin 的 id」，本机与他人一视同仁（服务端只发替换后的外观 int
   是给 addCharEquips 路径；equip 块里的 anvilItemId 是给本机精确渲染用）。

> **本端落点**：`Player::change_equip`（`Player.cpp:322-328`）从
> `inventory.get_item_id(EQUIPPED, slot)` 取 id → `look.add_equip(itemid)`；
> `CharLook::add_equip`（`CharLook.cpp:403-405`）→ `CharEquips::add_equip`
> （`CharEquips.cpp:34-54`，按 itemid 建 `Clothing`，由 `cloth.get_eqslot()` 决定部位）。
> 本端只要让 `change_equip` 取「外观 id = anvil_item_id ? anvil_item_id : item_id」即可；
> 他人 avatar 由 s9 `addCharEquips` 替换后天然正确，无需本机 hook。
> 本端 equip 解析在 `ItemParser.cpp:68-157`，anvilItemId 存进 `Equip` 后由
> `Inventory::get_equip`（`Inventory.h:117`）供 `change_equip` 读取。

---

## 5. cash item id 5900000 逻辑（v83 特定 vs 可移植）

| 面 | v83 实现 | 本端/s9 对应 |
|---|---|---|
| 识别 | `kFusionAnvilItemID = 5900000`（`:45`），三处 hook 拦截 | 本端在 cash item 双击/使用处按 itemId 判断，弹自绘 UI；无 Detours |
| WZ 条目 | `Item.wz/Cash/0590.img/05900000.img`，`info.{icon,iconRaw,cash=1,consumeOnPickup=1}`（`FUSION…md:217-230`） | 需把该 cash item 加进本端 Item.wz 与 s9 `wz_itemdata`/string 表（`:234-236`） |
| 分类 token | `get_consume_cash_item_type` 返回 1（`:602-608`） | 本端无内建表，直接分支 |
| 消费 | 服务端 `remove(c, position, itemId)` | s9 同款 |
| itemType 路由 | `5900000/10000 = 590` 分支（`:131`） | s9 `UseCashItemHandler` 现有分支到 561（`:650-712`），新增 590 分支即可 |

**注意 itemId 5900000 的 category `590` 与装备 category（104/105/106/110/170 等）不同域**：
590 是现金道具，不参与 `get_bodypart_from_item` 校验；被融合的是两件 equip。

---

## 6. 与染色 / 棱彩（ColoringPrism）的关系

三套机制**互相独立、可叠加**，不要混为一谈：

| 机制 | 数据 | 落点 | opcode |
|---|---|---|---|
| Fusion Anvil 融合外观 | `inventoryequipment.anvilItemId`（模板 id，0=无） | 换「用哪个 itemId 画」（整件外观替换） | C→S `0x4F` |
| 七彩棱镜染色 | s9 `coloring_prism_dye` 表（`inventoryitemid PK, characterid, itemid, hue/sat/light FLOAT`；`ColoringPrismStorage.java:5,31,64,124`） | 在 WZ 画布上逐像素 HSL 偏移（`equipdye.cpp:109-149`） | C→S `0x11D`，S→C `0x184` resp=1/2 |
| 他人染色刷新 | 服务端下发 merge → 客户端 `RequestForeignAvatarRefresh` → `CUser::Update` 轮询消费（`avatarmod.cpp:33-59`，`coloringprism.cpp:87-112`） | `ClearActionLayer + PrepareActionLayer` 重建动作层 | 同上 |

关系细节：
- 二者**作用阶段不同**：anvil 在「选 itemId → 加载 Clothing/动作帧」之前替换 id；
  dye 在「画布已加载」之后改像素。所以一件 transmog 后的 base 仍可被染色 —— 客户端
  `ApplyForeignVisibleDye`（`equipdye.cpp:284-294`）按 itemId 索引 dye；若服务端按 skin 的
  itemId 发 dye（因为它广播的视觉 id 已是 skin），二者天然对上。**移植时服务端应保证
  anvil 替换后的视觉 id 与 dye 记录的模板 itemId 是同一个（优先用 skin id 作为 dye 的 key）。**
- 本机若对同一 itemId 有自己的 dye，则跳过他人 dye（`equipdye.cpp:290-291`）。
- 棱镜窗口的滑条映射（`coloringprism.h:20-22`）：hue `v*1.8` → [-180,180]，
  sat `v*0.007` → [-0.7,0.7]，light `v*0.0075` → [-0.75,0.75]；
  发包用 `memcpy(float→int32_t)` 传位（`ColorPrismPackets.h:33-38,54-65`）。
  本端已实现（`ColorPrismPackets.h`、`coloringprism` 对应 handler），**融合外观不要复用
  这套 opcode 或数据包类**。
- HSL 可染品类白名单：`104/105/106/110/170`（`coloringprism.h:14-17`、`equipdye.cpp:226-233`）；
  融合外观的 `PutItem` 用的是 `get_bodypart_from_item`（覆盖所有部位）+ `get_weapon_type`
  校验武器子型 —— **比染色白名单宽**，移植时不要把染色的品类白名单套到 anvil 上。

---

## 7. 不可移植（v83 32 位硬编码地址 / hook）vs 可移植（纯业务语义）

### 7.1 不可移植 —— 注入版专属，本端 x64 不要复刻

`fusionanvil.cpp:18-42` 的地址常量（image base `0x400000` 的 32 位 v83 镜像）：

| 用途 | 地址 | 说明 |
|---|---|---|
| `AvatarLook::Load` | `0x004E72AD` | 渲染 hook 点 |
| `GW_ItemSlotBase::Decode` | `0x004E33F9` | 解码 hook 点 |
| `CInPacket::Decode4` | `0x00406629` | 多读 int |
| `SendConsumeCash` / `SendEtcCash` | `0x00A0A63F` / `0x00A1DC5B` | 发包拦截 |
| `CDraggableItem::OnDropped` / `OnDoubleClick` | `0x004EF140` / `0x004EFD25` | 拖放/双击 |
| `get_consume_cash_item_type` | `0x004863D5` | 分类表 |
| `play_ui_sound` / `ClientSocket::SendPacket` | `0x00989588` / `0x0049637B` | 音效/发包 |
| `ClientSocket::Instance` / `CWvsContext::Instance` | `0x00BE7914` / `0x00BE7918` | 单例 |
| `CharacterData::GetItem` / `TSecType<long>::GetData` | `0x004282F7` / `0x0042873D` | 取装/解密 itemId |
| `get_weapon_type` / `get_bodypart_from_item` | `0x00460AA0` / `0x004606A0` | 校验 |
| 结构大小 patch | `0x005D6053`, `0x004E3580` | `push 0F9h` 改 `0xFD`（扩结构体） |
| `GW_ItemSlotEquip.nAnvilItemID` | 偏移 `0xF9` | 32 位结构布局 |
| `AvatarLook.anHairEquip` | 偏移 `0x1D`，51 项 | 32 位结构布局 |
| `CUser::Update` / `GetCUserLocal` / `PrepareActionLayer` / `ClearActionLayer` | `0x00930B27` / `0x00BEBF98` / `0x00453AD1` / `0x00453A29` | `avatarmod.cpp:15-24,54-56` |
| `LoadCharAction` / `LoadItemAction` / `CUtilDlg::Notice` | `0x0040B9E7` / `0x00410848` / `0x009929DD` | `equipdye.cpp:168-171,190-192`，`coloringprism.cpp:118-120` |

还有注入版特有的「防御性」代码：`__try/__except`（`:506-514, 534-546, 650-652`）、
自造 `ms_RTTI`（`:160-165, 630-648`）、`Patch4` 改立即数（`:665-666`）——
都是 DLL 寄生在不属于自己的进程里才需要的，**本端原生代码一律不需要**。

### 7.2 可移植 —— 业务语义（本文建议保留）

1. 双槽（左 skin / 右 base）、拖放落点分左右、四类校验（bodyPart 相同 / 武器子型相同 /
   itemId 不同 / 都是 Equip）、OK 才发包。
2. 现金道具 5900000 双击拦截 → 弹窗，不直接走默认消费。
3. 包体 `short cashSlot, int itemId, short skinPos, short basePos`。
4. 服务端 stamp `base.anvilItemId = skin.itemId`，消耗 skin + 现金道具，保留 base 本体。
5. equip 序列化尾部追加 `int anvilItemId`；客户端解析时按剩余字节门控读取。
6. 外观广播替换视觉 id（base → skin），不动属性。
7. 落 DB 用 `inventoryequipment.anvilItemId`，随角色持久化（「登出重进外观仍在」）。
8. 与染色正交：视觉 id 统一用 skin id，避免 dye key 错位。

---

## 8. 本端新增类 / 文件建议（**仅建议，不写**）

本端是 Journey-like C++ x64 分层：`Character/Inventory`（数据）、`Character/Look`（渲染）、
`Net/Packets/<Feature>Packets.h`（C→S 包类）、`Net/Handlers`（S→C 处理）、
`UI/<Feature>`（窗口）。建议：

### 客户端

| 新增/改动 | 路径建议 | 职责 |
|---|---|---|
| 新增 | `Character/Inventory/Equip.{h,cpp}`（**改**） | 加 `int32_t anvil_item_id` 成员、构造末位默认参数、`get_anvil_item_id()`；与 spark/soul/star_force 同款（`Equip.h:42-43, 62-69`） |
| 改 | `Inventory.{h,cpp}` 的 `add_equip` | 末位加 `int32_t anvil_item_id = 0`，存入 `Equip` |
| 改 | `Net/Handlers/Helpers/ItemParser.cpp::add_equip` | 在 star_force 门控（`:150-155`）后加 `if (recv.length() >= 4) anvil = recv.read_int();`，传入 `add_equip` |
| 新增 | `Net/Packets/FusionAnvilPackets.h` | `UseFusionAnvilPacket : OutPacket(USE_CASH_ITEM)`，写 `short,int,short,short`；风格抄 `CashItemPackets.h:13-25`、`ColorPrismPackets.h:41-66` |
| 改 | `Character/Player.cpp::change_equip`（:322-328） | 取 `Optional<const Equip>`，用 `get_anvil_item_id() ? : get_item_id()` 作为 `look.add_equip(id)` 的参数；无 anvil 时行为不变 |
| 新增 | `UI/FusionAnvil/UIAnvilDialog.{h,cpp}` | 自绘窗口：186×195、两槽、OK/Cancel/Exit、Effect 动画；素材从 `UI/UIWindow.img/Synthesizing/...` 走本端 WZ 资源管理器（不要硬编码 v83 地址） |
| 改 | cash item 双击分发处（找 `USE_CASH_ITEM` 调用点） | itemId==5900000 → 打开 `UIAnvilDialog`，其余透传 |
| 新增（可选） | `UI/FusionAnvil/AnvilSlotValidator.{h,cpp}` | 封装 bodyPart / 武器子型 / itemId 校验；本端可用 `ItemInfo`/`EquipSlot` 现有 API 替代 `get_bodypart_from_item` |

### 服务端（s9）

| 新增/改动 | 路径建议 | 职责 |
|---|---|---|
| 新增 | `src/main/resources/db/tables/026-fusion-anvil.sql`（编号接现有最大） | `ALTER TABLE inventoryequipment ADD COLUMN anvilItemId INT NOT NULL DEFAULT 0 AFTER ringid;`，并更新 canonical 建表脚本 |
| 改 | `org.gms.client.inventory.Equip.java` | 加 `anvilItemId`（`@Getter/@Setter` 风格对齐现有 `spark/soul/starForce` 的 `:83-86`），`copy()` 复制 |
| 改 | `ItemFactory.java` | `INSERT_INVENTORY_EQUIPMENT`（`:55-62`）列+占位符 31→32，`setEquipInsertParams` 末尾 `setInt(32,...)`（`:178-188`），load 侧 `setAnvilItemId(rs.getInt("anvilItemId"))`（`:151` 附近） |
| 改 | `PacketCreator.java::addEquip`（:592-643） | 在 `writeByte(getStarForce())` 之后 `p.writeInt(equip.getAnvilItemId())` |
| 改 | `PacketCreator.java::addCharEquips`（:295-333） | 写 `myEquip/maskedEquip` 的 value 时按 `getAnvilItemId()!=0 ? : getItemId()`；cWeapon 与 pet int 不变 |
| 改 | `UseCashItemHandler.java` | 在 561 分支（`:650-712`）之后加 `else if (itemType == 590)`，按 §3 校验 + stamp + 消耗 + `ModifyInventory`/`equipChanged`/`enableActions` |
| 新增 | `org.gms.server.anvil.FusionAnvilService.java`（可选） | 把校验/消耗/广播抽成服务类，避免 handler 膨胀；风格参考 `server/coloring/ColoringPrismStorage.java` |

### 验收要点（本端真机未验收，务必逐条实测）

- 客户端解析 anvilItemId 后不出现整包错位（潜能/强化/戒指字段仍正确）。
- 换装 / 重登 / 角色选择画面 / 他人视角均为 skin 外观；属性 tooltip 仍是 base。
- skin 被消耗、base 保留；现金道具 5900000 被扣除；两人混合穿戴（worn + bagged）能成功。
- 同 itemId、跨平台部位、不同武器子型、skin 已穿戴 —— 四种失败分支有正确提示且不发包。
- transmog 后的装备仍可棱镜染色，且他人看到的染色不丢失（视觉 id 与 dye key 一致）。
- WZ/IMG 素材入本端时按 `AGENTS.md` §3.1 走 **orange-wz MCP + 显式 `batch_convert_key`**，
  不得裸拷（源 key/IV 与本端 GMS 不同）；本子代理未做节点级解析，节点清单需主代理复核。

---

## 9. 证据索引（逐条行号）

- `fusionanvil.cpp:18-47` — v83 地址常量、`kNewItemSlotEquipSize=0xFD`、`kOffset_nAnvilItemID=0xF9`、`5900000`、`0x4F`
- `fusionanvil.cpp:157-294` — `CUIFusionAnvil` 构造/资源路径/槽位矩形
- `fusionanvil.cpp:438-487` — `PutItem` 校验、`SendRequestPacket` 包体
- `fusionanvil.cpp:499-516` — Decode hook 追加读 int
- `fusionanvil.cpp:527-552` — AvatarLook::Load hook 覆盖 anHairEquip
- `fusionanvil.cpp:564-608` — 三处 cash item 拦截 / 分类 token
- `fusionanvil.cpp:626-677` — 拖放 hook + `AttachFusionAnvilMod` 安装
- `avatarmod.cpp:15-24,33-59,72-75` — CUser::Update hook、他人刷新队列、延迟 Apply
- `equipdye.cpp:18-66` RGB↔HSL；`:109-149` 像素染色；`:226-233` 品类；`:284-294` 他人 dye；`:307-348` 同步/延迟
- `coloringprism.h:14-22` 滑条映射；`.cpp:30-60` 发包 `0x11D`；`:65-112` 收包 `0x184`
- `FUSION_ANVIL_SERVER-SIDE.md:10-17` 概览；`:19-36` DB；`:38-94` Equip/ItemFactory；`:96-129` PacketCreator；`:131-198` handler；`:212-230` WZ；`:246-253` opcode 表
- 本端 `Equip.h:42-43,62-92`；`Inventory.h:69,77-78,116-117,139`；`ItemParser.cpp:68-157`（尤其 `:129-155` 门控）
- 本端 `CharLook.cpp:403-405`；`CharEquips.cpp:34-54`；`Player.cpp:322-328`
- 本端 `OutPacket.h:131`（`USE_CASH_ITEM=79=0x4F`）、`:327-331`（`COLORING_PRISM_ACTION=0x11D`）
- 本端 `CashItemPackets.h:13-25,44-54,190-199`；`ColorPrismPackets.h:1-14,24-38,41-80`
- s9 `Equip.java:73-91,106-120`；`ItemFactory.java:50-62,115-151,178-188`；
  `PacketCreator.java:295-333`（addCharEquips）、`:592-643`（equip 编码）；
  `UseCashItemHandler.java:92-119`（读 position/itemId）、`:650-712`（561 分支）、`:713`（NEW TYPE warn）；
  `ColoringPrismStorage.java:5,31,64,124`、`ColoringPrismDye.java:18-26`、`ColoringPrismPackets.java:67-68`
- IMG/WZ：`UIWindow.img` 203321 B，头字节 `73 f8 6c 77 …`（加密 IMG，未解析节点）；
  `Custom.wz` 45859 B，头 `50 4b 47 31`（`PKG1`）+ ASCII `Package file v1.0 Copyright 2002`。
  UI 资源路径以源码硬编码为准（`UI/UIWindow.img/Synthesizing/{backgrnd,backgrnd1,SubTitle,BtOk,BtCancel,BtExit,Effect/0..11}`）。
