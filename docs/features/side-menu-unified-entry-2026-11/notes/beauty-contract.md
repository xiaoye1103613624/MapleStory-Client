# 美容院（Beauty Salon）v83 DLL 注入版 → 本端原生移植契约（只读提取）

> 本文只做**证据提取与移植建议**，未改任何代码。只读研究日期：2026-11-23。
> 源：`E:\MXD\扩展改动\美容院\BeautySalonv83\`（下称 **v83 包**）。
> 目标：`E:\project\MapleStory-Client`（x64 C++ 客户端）、`E:\project\BeiDou-Server_s9\gms-server`（s9 Java Spring admin，AGPL）。
>
> 引用格式：`文件:行号`。所有行号来自本次实际读取（ops_inspect_local_file 输出 revision 见各文件读取证据）。

---

## 0. 一句话结论

业务语义完全可移植（2 个游戏内 opcode、7 种 C→S 动作、2 种 S→C 响应、2 张表、3 tab×6 slot 状态机、avatar 预览逻辑、解锁道具 5920000、`@beauty` 命令）；**v83 32 位绝对地址、CInPacket 原始内存布局、AvatarLook 字节偏移、Detours hook 链、WZ 路径 `UI/v83.img/...` 不可直接移植**，必须按本端既有封装重写。

---

## 1. 协议总览

- 双向单 opcode `0x174`（十进制 372），v83 包源码：`client/src/beautyshop.cpp:117`（`kOpcode_SaveBeauty`）、`java/README.md:24/29`、`README.md:28`。
- **⚠ 本端占用冲突**：s9 `SendOpcode.SET_ITEM_FINAL_DAMAGE(0x175)`（`SendOpcode.java:396`）已占 0x175，0x174 在本次 grep 的两张 opcode 表里**均未出现**（`RecvOpcode.java`/`SendOpcode.java` grep `0x174|0x173` 为 0 命中）；客户端 `OutPacket::Opcode` 中 `SET_ITEM_BONUS_REQUEST = 0x177`、`EQUIP_GROWTH_TIP_REQUEST = 0x17D`（`Net/OutPacket.h:304-307`），`0x174` 亦未见。但 v83 包 README 明确要求「0x174 必须两端都空闲，否则两端同改」（`README.md:83-85`）。**移植建议沿用 0x174（两端均空闲，已逐表 grep 确认），但仍须在落地前由主端再做一次 opcode 门禁复核；或按本端私有扩展惯例改用 0x3746 附近的双端成对空闲号（0x3743 SIDE_MENU_SYNC / 0x3745 SKILL_STAT_OVERRIDE 已占，见 `PacketSwitch.cpp:441-444`、`SendOpcode.java:401-403`）**。
- 多字节字节序：**小端（LE）**，逐字节由低到高写出。证据：v83 `COutPacket::Encode2/Encode4` 用 `EncodeBuffer(&n,2/4)` 内存直拷（`client/reference/wvs/packet.h:33-37`，x86 小端）；s9 `ByteBufOutPacket.writeShort()/writeInt()` 为 `writeShortLE/writeIntLE`（`ByteBufOutPacket.java:58-65`）；本端 `OutPacket::write_short()/write_int()` 循环 `write_byte(sh); sh >>= 8`（`Net/OutPacket.cpp:60-76`）。三端一致。

### 1.1 客户端 → 服务端（`0x174`，包体首字节 action）

包结构：`int16 opcode(0x174)` + `int8 action` + 按 action 附加字段。
服务端读取：`BeautyHandler.java:38-62`；客户端写出：`beautyshop.cpp:639-641 / 916-938 / 1210-1215`。

| action | byte | 附加字段（顺序/类型） | 客户端写出点 | 服务端读取/处理点 |
|---|---|---|---|---|
| `0` REQUEST | `0` | 无 | `beautyshop.cpp:639-641`（构造窗口即发） | `BeautyHandler.java:40` → `sendData()` |
| `1` SAVE | `1` | `int8 slot` + `int8 type` | `:916-922` | `:41-45` → `handleSave()` `:83-98` |
| `2` APPLY | `2` | `int8 slot` + `int8 type` | `:924-930`（type = `m_nCurrentTab`） | `:46-50` → `handleApply()` `:100-129` |
| `3` DELETE | `3` | `int8 slot` + `int8 type` | `:932-938` | `:51-55` → `handleDelete()` `:131-137` |
| `4` UNLOCK | `4` | `int16 pos`（CASH 栏物品栏位；`Encode2` LE） | `:1210-1215` | `:56-59` → `handleUnlock()` `:71-81` |

- `type`：`0=Hair, 1=Face, 2=Skin`（`beautyshop.cpp:119-121`、`BeautyHandler.java:26-28`）。
- `slot`：`0..5`（`kMaxSlots=6`，`beautyshop.cpp:130`、`BeautyHandler.java:29`）。
- 服务端对 SAVE/APPLY/DELETE 统一校验 `slot∈[0,6) && type∈{0,1,2}`（`BeautyHandler.java:139-142`）；SAVE 额外要求 `slot < unlockedSlots`（`:87-89`）。APPLY/DELETE **不检查 unlocked**（仅靠能否找到已存行兜底）。UNLOCK 上限 6（`:72-79`）。

### 1.2 服务端 → 客户端（`0x174`，包体首字节 respType）

包结构：`int16 opcode(0x174)` + `int8 respType` + 载荷。
服务端写出：`BeautyPackets.java:17-91`；客户端解码：`beautyshop.cpp:940-1034`。

| respType | byte | 载荷 | 服务端 | 客户端 |
|---|---|---|---|---|
| `0` OPEN | `0` | 无 | `beautyOpen()` `BeautyPackets.java:17-21` | `beautyshop.cpp:1266-1267` → `OpenBeautyShop()` |
| `1` DATA | `1` | 见下方完整字段表 | `beautyData()` `BeautyPackets.java:23-91` | `HandleServerResponse()` `beautyshop.cpp:940-1034` |

**DATA 载荷逐字段（权威顺序，两端必须逐字节一致）：**

| 序 | 字段 | 类型/字节数 | 写出 | 读取 |
|---|---|---|---|---|
| 1 | `respType` | int8 (1) | `BeautyPackets.java:48` | `beautyshop.cpp:1264`（hook 先跳 opcode 2 字节） |
| 2 | `unlockedSlots` | int8 (1) | `:49` | `beautyshop.cpp:944`（>6 截断 `:945`） |
| 3 | `hairCount` | int8 (1) | `:51` | `:947` |
| 4 | `hairId[6]` | int32×6 (24) | `:52-54` | `:948-950`（仅 i<hairCount 读，其余置 0） |
| 5 | `faceCount` | int8 (1) | `:55` | `:952` |
| 6 | `faceId[6]` | int32×6 (24) | `:56-58` | `:953-955` |
| 7 | `skinCount` | int8 (1) | `:59` | `:957` |
| 8 | `skinId[6]` | int32×6 (24) | `:60-62` | `:958-960` |
| 9 | `hairSavedCount` | int8 (1) | `:63` | `:962` |
| 10 | `hairSaved[6]` | int8×6 (6) | `:64-66`（1/0） | `:963-968` |
| 11 | `faceSavedCount` | int8 (1) | `:67` | `:970` |
| 12 | `faceSaved[6]` | int8×6 (6) | `:68-70` | `:971-976` |
| 13 | `skinSavedCount` | int8 (1) | `:71` | `:978` |
| 14 | `skinSaved[6]` | int8×6 (6) | `:72-74` | `:979-983` |
| 15 | `fullDataCount` | int8 (1) = `rows.size()` | `:76` | `:985` |
| 16 | `rows[]` × N | 见下 | `:77-90` | `:986-1029` |

**row（每条约 41 字节）字段序：**

| 字段 | 类型 | 写出 | 读取 |
|---|---|---|---|
| `type` | int8 | `BeautyPackets.java:78` | `beautyshop.cpp:987` |
| `slot` | int8 | `:79` | `:988` |
| `gender` | int8 | `:80` | `:989` |
| `skin` | int32 | `:81` | `:990` |
| `hair` | int32 | `:82` | `:991` |
| `face` | int32 | `:83` | `:992` |
| `hat` | int32 | `:84` | `:993` |
| `top` | int32 | `:85` | `:994` |
| `bottom` | int32 | `:86` | `:995` |
| `shoes` | int32 | `:87` | `:996` |
| `weapon` | int32 | `:88` | `:997` |
| `cashWeapon` | int32 | `:89` | `:998` |

> count 字段恒为 `SLOT_COUNT=6`（`BeautyPackets.java:10,51,55,59,63,67,71`），客户端却把它们当「可读上限」用（`beautyshop.cpp:947-983`）；改动任一侧前必须同步。`rows` 无序（SQL `ORDER BY slottype, slot`，`BeautyStorage.java:21`），但客户端按 `type+slot` 索引（`beautyshop.cpp:1001-1006`），顺序不敏感。
> `unlocked`/count/row 数均为 `writeByte`，>255 会溢出；SLOT_COUNT=6 固定，row 最多 6×3=18，安全。

### 1.3 权威语义注意点（易踩坑）

- **`hat/top/bottom/shoes/weapon/cashWeapon` 6 个装备 id 随行下发，但客户端预览只用到 `nSkin/nHair/nFace`（`beautyshop.cpp:832-843`），其余字段本端可保留兼容但不强制渲染。**
- `gender` 也只存不用（客户端 `s.nGender=gender` `:1007`，无后续消费）；`AvatarLook` 的性别取自当前角色 `charData`（`:830`），不是存档性别。
- **APPLY 是解耦的**：Hair 只 setHair、Face 只 setFace、Skin 只 setSkinColor（`BeautyHandler.java:114-126`），每次都 `chr.equipChanged()`（`:127`）广播；不是整套装一键换装。
- 客户端消费道具时**弹窗确认只用于 DELETE**（`beautyshop.cpp:1078-1091`）；UNLOCK 消费无二次确认。

---

## 2. 数据库契约

### 2.1 `beautystorage`（v83 `025-beauty.sql:1-22`）

| 列 | SQL 类型 | 约束 | 业务含义 |
|---|---|---|---|
| `id` | INT UNSIGNED AUTO_INCREMENT | PRIMARY KEY | 代理主键 |
| `characterid` | INT NOT NULL | UNIQUE(characterid,slot,slottype) + KEY + FK characters.id ON DELETE CASCADE | 角色 id |
| `slot` | TINYINT NOT NULL | 同上 UNIQUE | 0..5 |
| `slottype` | TINYINT NOT NULL | 同上 UNIQUE | 0 Hair / 1 Face / 2 Skin |
| `gender` | TINYINT NOT NULL DEFAULT 0 | — | 存档性别（仅存） |
| `skin` | INT NOT NULL DEFAULT 0 | — | 肤色 id |
| `hair` | INT NOT NULL DEFAULT 0 | — | 发型 id |
| `face` | INT NOT NULL DEFAULT 0 | — | 脸型 id |
| `hat` / `top` / `bottom` / `shoes` / `weapon` / `cashweapon` | INT NOT NULL DEFAULT 0 | — | 装备快照 id |
| `savedat` | TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP | — | 存档时间 |

SQL 访问 (`BeautyStorage.java`)：`loadAll` SELECT 按 `slottype,slot` 排序（`:19-32`）；`save` 13 列 upsert `ON DUPLICATE KEY UPDATE`（`:34-58`）；`delete` 按三元组（`:60-71`）。

### 2.2 `beautyunlock`（v83 `026-beauty-unlock.sql:1-7`）

| 列 | 类型 | 约束 | 含义 |
|---|---|---|---|
| `characterid` | INT NOT NULL | PRIMARY KEY + FK characters.id ON DELETE CASCADE | 角色 id |
| `slots` | TINYINT NOT NULL DEFAULT 0 | — | 已解锁槽数 0..6（三 tab 共享一份） |

`BeautyStorage.getUnlockedSlots()` `:73-86`（缺行返回 0）；`setUnlockedSlots()` `:88-99`（upsert）。

> 本端落地注意：s9 扩展表惯例是 `resources/db/extensions/*.sql` + 运维手工执行 + Storage 内存降级（见 `db/extensions/daily_gift_state.sql:1-15`、`server/coloring/ColoringPrismStorage.java:1-13`），**不使用 Liquibase changelog-tables.xml**（v83 包 `java/README.md:56-70` 的 changeSet 25/26 是 Cosmic 的做法）。本端建议跟随 s9 惯例放 `db/extensions/beauty.sql`（含两表），Storage 首 SQL 失败后降级为内存；DDL 不由 Agent 执行。

---

## 3. UI 状态机（3 tab × 6 slot）与按钮动作

### 3.1 视觉布局（v83  baked-in 坐标，非契约但移植需对齐本端 WZ）

- 窗口 397×370，居中创建（`beautyshop.cpp:479-482`）；tab：Face 44? → 实际 `m_rcTabFace={9,25,43,43}`、`m_rcTabHair={44,25,78,43}`、`m_rcTabSkin={79,25,113,43}`（`:506-508`），左→右 Face / Hair / Skin，但 `m_nCurrentTab` 初始 0=Hair（`:469`）。
- slot 列 x={9,138,268}，行 y={47,204}，slotW=121/slotH=153（`:510-513,569`）；占位图中心 x={39,167,297}、y={76,233}（`:574-577`）；按钮 x={32,164,293}、y={172,330}，70×19（`:614-624`）。
- 按钮四态数组索引：0=disabled、1=mouseOver、2=normal、3=pressed（`:519-559`，`README.md:104-107`）。状态默认值 `applyBtnState=deleteBtnState=2`（`:625-626`）。

### 3.2 状态字段与判定（`beautyshop.cpp:362-458`）

| 字段 | 含义 |
|---|---|
| `m_nCurrentTab` | 0 Hair / 1 Face / 2 Skin |
| `m_nSelectedSlot` | 当前选中槽，-1 无 |
| `m_nUnlockedSlots` | 已解锁数；`IsSlotLocked(i) = i >= m_nUnlockedSlots`（`:421`） |
| `m_bDataReady` | 收到首包 DATA 后才绘制 slot（`:737`） |
| `m_nPressedApplySlot` | 记录按下态槽，LBUTTONUP 才触发（`:1073,1104`） |
| `SlotData.bIsSaved` / `hasHairData` / `hasFaceData` / `hasSkinData` | 各 tab 的存档态；`SlotHasData(s,tab)` `:428-432` |
| `SlotData.applyBtnState` / `deleteBtnState` | 四态机 |

### 3.3 每个 slot 的渲染决策（`Draw()` `:710-785`）

1. 先画 `Slot/layer:slot` 帧（`:742-744`）。
2. `IsSlotLocked(i)` → 叠加 `disableSlot` 灰罩，`continue`（`:746-751`）。**锁定槽既不可点也不可保存。**
3. `!bIsSaved` → 画当前 tab 的 `enableHM/FM/SM` 空位图（`:753-755`）。
4. 主按钮：`hasDataForCurrentTab ? Apply{tab} : Save{tab}`——**同一矩形、互斥显示**（`:757-767`）。即空槽显示 Save，已存槽显示 Apply。
5. `hasDataForCurrentTab` 才画 Delete（`:769-774`）。
6. `g_enableNameLabels && hasData && szName[0]` 才画名字（Hair/Face；`:776-780`，Skin 无名字 `:1028`）。
7. 关闭按钮：`{376,5,389,18}`（`:504`）、三态（`:783`）。

### 3.4 鼠标/按钮动作（`OnMouseButton` `:1036-1137`、`OnMouseMove` `:1139-1199`）

| 交互 | 条件 | 动作 | 声音 |
|---|---|---|---|
| 点 tab 矩形 | 任意 | `SelectTab(n)` → 重播 `Tab`，`RebuildAvatars()`（`:787-793`） | Tab |
| 点 slot 内主按钮（LBUTTONDOWN） | 未锁定 | `applyBtnState=3`（pressed），记 `m_nPressedApplySlot=i`（`:1071-1076`） | Click（`:1066`） |
| LBUTTONUP 仍在主按钮 | 未锁定 + `hasData` | `SendApplyPacket(i)`（`:1112-1114`） | Apply |
| LBUTTONUP 仍在主按钮 | 未锁定 + `!hasData` | `m_nSelectedSlot=i` → `SaveBeauty()` → `SendSavePacket(i,tab)`（`:1115-1118,909-914`） | Lock |
| 点 Delete（LBUTTONDOWN） | 未锁定 + `hasData` | 原生 `CUtilDlg_YesNo("Are you sure want to delete this?")`，返回 6=Yes 才 `SendDeletePacket`（`:1078-1091`） | BtMouseClick / Unlock |
| 点关闭按钮（LBUTTONDOWN/UP） | — | `Destroy()`（`:1042-1047,1125-1131`） | BtMouseClick / MenuDown |
| 空白处拖拽 | `m_pLayer` 存在 | `RelOffset` + 更新 m_nWndX/Y（`:1141-1150`） | — |
| 悬停主按钮/Delete/Close | 非锁定态 | state 2→1，声音 BtMouseOver（`:1155-1181`） | BtMouseOver |
| 悬停名字带 (`slot.top+104..+120`) | 有名字 | 原生 tooltip 显示名字（`:1183-1196`） | — |
| `Update()` | 当前 stage 变化 | `Destroy()` 自动关窗（`:689-694`） | — |
| `Update()` | `ComputeLookHash(charData)` 变化 | `RebuildAvatars()`（`:696-706`） | — |

> 本端没有 `CUtilDlg_YesNo`/`CWnd` 这套引擎对象；DELETE 二次确认要用本端 UI 的确认弹窗/`MessageDialog` 等价物，或先只留客户端提示文本。

---

## 4. avatar 预览数据来源（可移植的业务语义 + 不可移植的 32 位取址）

### 4.1 数据来源链

1. `CWvsContext` 单例 → +`kOff_CharacterDataInCtx = 2094*4 = 0x20B8` 取 `CharacterData* charData`（`beautyshop.cpp:140-142,696-699,863-867`）。
2. 以 `charData` 构造 `AvatarLook`（`:828-830`，直接调 `AvatarLook::AvatarLook(CharacterData&)`）。
3. 按当前 tab 覆写 packed 字段（`AvatarLook` 字节偏移，v83 专用，`:149-157,832-843`）：
   - Hair tab：`buf+0x19 = s.nHair`；并清 `buf+0x1D`(hat)、`buf+0x21`(faceAcc)（`:832,840-843`）
   - Face tab：`buf+0x11 = s.nFace`；同样清 hat/faceAcc（`:834,840-843`）
   - Skin tab：`buf+0x0D = s.nSkin`；**不清** hat/faceAcc（`:836-838`）
4. `CAvatar::Init(pAvatar, buf, action=5(stand), layer, layer, z=1, x=slot.left+63, y=slot.top+90, scale=100, emotion=0)`（`:147,159-161,847-850`）。
5. Avatar 对象由 `ZRef<CAvatar>::_Alloc` 分配并挂在 `m_avatarRef[i]`，`ReleaseAvatars()` 释放（`:801-857,860-874`）。

### 4.2 名字来源

`String/Eqp.img/Eqp/{Hair|Face}/<id>/name`（`beautyshop.cpp:197-214`），经 `get_basic_font` + `DrawFittedName` 自适应字号/省略号画在 slot 内（`:216-268`）；tooltip 也显示同一名字（`:886-900`）。**Skin 无名字**（`:1028`）。

### 4.3 不可移植点

- `kAddr_CWvsContext_Instance=0x00BE7918`、`kOff_CharacterDataInCtx=2094*4`、`ZRefCAvatar_Alloc=0x428967`、`ZRef_Release=0x428C15`、`AvatarLook_ctor=0x4283FE`、`CAvatar_Init=0x45149F`、`get_basic_font=0x98A707`、`AvatarLook` 字段偏移 0x0C/0x0D/0x11/0x15/0x19/0x1D/0x21/0x45/0x115（`:140-157,191-192`）——全部 v83 image base 0x400000 硬编码（`client/README.md:49-76`）。本端 x64 原生客户端应使用 `Char`/`Avatar`/`AvatarLook` 现有 C++ 类，不需要也不允许按绝对地址调用。
- `ComputeLookHash()` 直接把 `AvatarLook` 当 0x210 字节 `unsigned char[]` FNV-1a 哈希（`beautyshop.cpp:174-189`），是 v83 内存布局依赖；本端应改为「比较当前角色 hair/face/skin/equip id 元组」或订阅角色外观变更事件。
- WZ 路径前缀 `UI/v83.img/beautyRoom/...`（`:489-559`）在本端应为 `UI/<当前版本>.img/...` 或已 unpack 的 `UIBitmap` 资源；**迁移前须走 orange-wz MCP + 显式 key 转换门禁**（见仓库 `AGENTS.md` §3.1），不得裸拷 `.img`。

---

## 5. item 5920000 解锁逻辑

1. 常量 `kBeautyShopItemID = 5920000`（`beautyshop.cpp:116`；`BeautyHandler.java:30`）；`README.md:16-17` 明确：**不打开 UI**，每消费 1 个解锁 1 个槽（0→6，按角色）。
2. 客户端 hook 三个函数（`AttachBeautyShopMod` `:1279-1287`）：
   - `CWvsContext::SendConsumeCashItemUseRequest`（`:1218-1228`）
   - `CWvsContext::SendEtcCashItemUseRequest`（`:1230-1239`）
   - `get_consume_cash_item_type`（`:1241-1249`，对 5920000 强制返回 1）
   命中 itemID 时**吞掉原调用**（`return`），只发 `[0x174][4][pos LE int16]`（`SendUnlockSlot` `:1210-1215`），避免原生 ConsumeCash 协议与私有解锁协议双发。非目标道具原样透传（`:1227,1238,1248`）。
3. 服务端 `handleUnlock`（`BeautyHandler.java:71-81`）：`unlocked<6` 时校验 CASH 栏 `itemPos` 实际是 5920000，调用 `InventoryManipulator.removeFromSlot(c, CASH, pos, 1, false)` 扣 1 个，再 `setUnlockedSlots(unlocked+1)`；无论成功失败都 `sendData()` 回快照（`:80`）。
4. **本端适配点**：s9 的 CASH 物品使用走 `UseCashItemHandler`（`org/gms/net/server/channel/handlers/UseCashItemHandler.java:82`），客户端 `USE_CASH_ITEM(79)` 包见 `Net/Packets/CashItemPackets.h:13-41`。私有 `0x174/action4` 由客户端显式发即可，**不需要也不应当 hook 原生函数地址**；但要注意若道具仍走原生 USE_CASH_ITEM，服务端 `UseCashItemHandler` 也需豁免 5920000 或客户端拦截不发原生包，否则双扣。
5. 未解锁槽表现：灰罩 + 不可点 + 保存被 `SaveBeauty()` 早退拦截（`beautyshop.cpp:909-911`）。服务端 SAVE 也再校验一次（`BeautyHandler.java:87-89`）。

---

## 6. `@beauty` 命令

- v83 做法：在 `GeneralChatHandler.handlePacket` **读取文本后、`char heading = s.charAt(0)` 之前**插入：
  `if (s.equalsIgnoreCase("@beauty")) { c.sendPacket(BeautyPackets.beautyOpen()); return; }`（`java/README.md:45-54`）。
- 响应 `[0x174][0]` 使客户端 `OpenBeautyShop()`（`beautyshop.cpp:1266-1267`），窗口构造时自动发 `[0x174][0]` REQUEST 拉数据（`:639-641`）。
- 本端对应文件：`org/gms/net/server/channel/handlers/GeneralChatHandler.java:40-53`（`handlePacket`，`heading` 在 `:53`），命令总线 `CommandsExecutor` 在 `:54-55`。可插在 `:53` 前，或注册为正式 `@beauty` command（更符合本端 `client/command` 体系）。
- 本端侧栏入口：与 `UIDailyCheckin`（侧栏/@daily）、`UIWitchForest`（侧栏/@witchforest）同范式（`IO/UIElement.h:261-269`），建议同时提供侧栏种子 `beauty` 与 `@beauty` 命令。

---

## 7. v83 32 位硬编码（不可移植） vs 纯业务语义（可移植）

### 7.1 不可直接移植（地址/布局/框架绑定）

| 项 | v83 值 | 出处 |
|---|---|---|
| 客户端 socket 单例 / SendPacket / ProcessPacket | `0x00BE7914 / 0x0049637B / 0x004965F1` | `beautyshop.cpp:132-134` |
| `play_ui_sound` | `0x00989588` | `:135` |
| `SendConsumeCash / SendEtcCash / get_consume_cash_item_type` | `0x00A0A63F / 0x00A1DC5B / 0x004863D5` | `:136-138` |
| `CWvsContext` 单例 / `CurrentStage` / CharacterData 偏移 | `0x00BE7918 / 0x00BEDED4 / 2094*4` | `:140-142` |
| `ZRef<CAvatar>::_Alloc / ZRef_Release / AvatarLook_ctor / CAvatar::Init` | `0x428967 / 0x428C15 / 0x4283FE / 0x45149F` | `:143-146` |
| `get_basic_font` | `0x0098A707` | `:191-192` |
| `CUtilDlg_YesNo` / `kYesNo_Yes=6` | `0x00992BFD` | `:273-275` |
| tooltip ctor/dtor/SetString/Clear | `0x008E49B5 / 0x008E6BA3 / 0x008E6E7D / 0x008E6E23` | `:281-284` |
| `CInPacket` 原始内存布局 | buffer `+0x8`、length `+0xC`、offset `+0x14` | `:293-314`；`reference/wvs/packet.h:5-16` |
| `COutPacket` 内存布局 | 0x10 字节、`Init(nType,0,0)` 写 2 字节 opcode | `reference/wvs/packet.h:19-60` |
| `AvatarLook` packed 字段偏移 | 0x0C/0x0D/0x11/0x15/0x19/0x1D/0x21/0x45/0x115 | `beautyshop.cpp:149-157` |
| `ComputeLookHash` FNV over 0x210 字节 | 依赖上述布局 | `:174-189` |
| Detours `ATTACH_HOOK` 链 / VEH 崩溃日志 | `:17-114,1279-1287` | 本端无 Detours 框架 |
| `CWnd`/`CRTTI`/`ZALLOC_GLOBAL`/`IWzCanvasPtr` COM 资源 | 整个 `CBeautyShop` 基类 | `:362-458` |
| WZ 资源路径 `UI/v83.img/beautyRoom/**` | 全部 LoadSprite | `:489-559` |
| Win32 32 位栈扫描（MapleStory.exe+0x400000） | `:97-110` | x64 无效 |

### 7.2 可移植（纯业务语义 / 协议 / 数据）

- 双向 opcode 语义与 7 个 action / 2 个 respType、所有字段顺序与类型（§1）。
- 3 tab × 6 slot、共享解锁计数、空/存/锁三态与主按钮 Save↔Apply 互斥、Delete 二次确认语义（§3）。
- 存档快照字段集（gender/skin/hair/face + 6 装备 id）、按角色三元组唯一键与 upsert/delete（§2）。
- 解锁道具 id 5920000、每消费 +1 槽、服务端权威校验道具与扣减（§5）。
- `@beauty` 发包开 UI、UI 自动 REQUEST 拉快照、所有写操作后服务端回推 DATA 的闭环（§6、README `:155-170`）。
- avatar 预览「以当前角色外观为底，只覆写当前 tab 对应字段，Hair/Face tab 隐藏帽子与脸饰，skin tab 不改」的业务规则（§4.1）。
- 名字来自 `String/Eqp.img/Eqp/{Hair,Face}/<id>/name`、Skin 无名字、声音事件集合（MenuUp/MenuDown/Tab/Lock/Apply/Unlock/BtMouseClick/BtMouseOver/Click）。

---

## 8. 本端应新增/修改文件建议（仅建议，未写）

### 8.1 客户端 `E:\project\MapleStory-Client`

| 动作 | 文件 | 依据/内容 |
|---|---|---|
| 改 | `Net/OutPacket.h` | `enum Opcode` 增 `BEAUTY_ACTION`（选 0x174 或双端复核后的空闲号），注释仿 `DAILY_GIFT_ACTION`（`:315-318`）写清 body。 |
| 新增 | `Net/Packets/BeautyPackets.h` | 7 个 `OutPacket` 子类（Request/Save/Apply/Delete/Unlock + 可选 Open），`write_byte/write_short`；风格对齐 `DailyGiftPackets.h:12-40`、`CashItemPackets.h:13-41`。 |
| 改 | `Net/PacketSwitch.cpp` | `enum` 增 `BEAUTY_RESULT`（与 C→S 配对的 S→C 号）、`emplace<BEAUTY_RESULT, BeautyHandler>()`、`opcode_msg` case；仿 `SIDE_MENU_SYNC`（`:441-444,836,1232`）。 |
| 新增 | `Net/Handlers/BeautyHandlers.h` | `BeautyHandler : PacketHandler`，按 §1.2 用 `InPacket::read_byte()/read_int()` 解析（`:940-1034` 语义），分发到 UI `apply_open()/apply_data()`；风格对齐 `CommonHandlers.h:32-60`。 |
| 新增 | `IO/UITypes/UIBeautySalon.h/.cpp` | `UIDragElement<PosBEAUTY>`，3 tab×6 slot 状态机、`Button::State` 四态、Save/Apply/Delete/Close、确认弹窗；风格对齐 `UIWitchForest.h:23-55`、`UIColorPrism.h:26-31`。 |
| 改 | `IO/UIElement.h` | enum `Type` 增 `BEAUTYSALON`（在 `:266-272` 附近；**改枚举须整工程 Rebuild**，见 `:264-266` 注释）。 |
| 改 | `Configuration.h` | 增 `PosBEAUTY : PointEntry("ui.position.beauty", "...")`，仿 `PosWITCHFOREST`（`:1003-1006`）。 |
| 改 | `IO/UI.cpp` | 增加窗口创建分支，仿 `:477` COLORPRISM。 |
| 改 | `resources/side_menu.json` / 侧栏种子 | 挂 `beauty` 种子（侧栏入口），与既有 builtin 同范式。 |
| 改 | avatar/角色外观事件 | 用本端 `CharLook`/`AvatarLook` 等价类按 §4.1 规则派生预览，不引入任何绝对地址；预览 hash 改为比较外观 id 元组。 |
| 资源 | 新 WZ/IMG | `UI.wz/<ver>.img/beautyRoom/**`（§4.3，走 orange-wz + key 转换门禁）、`Item.wz` 加 5920000（Cash）、`Sound.wz/UI.img` 缺项补齐。 |

### 8.2 服务端 `E:\project\BeiDou-Server_s9\gms-server`

| 动作 | 文件 | 依据/内容 |
|---|---|---|
| 改 | `org/gms/net/opcodes/RecvOpcode.java` | 增 `BEAUTY_ACTION(...)`，与客户端 Out 同号（`:210-230` 私有扩展段）。 |
| 改 | `org/gms/net/opcodes/SendOpcode.java` | 增 `BEAUTY_RESULT(...)`（`:389-403` 段，注意 0x175/0x176/0x17A/0x184 已占）。 |
| 改 | `org/gms/net/PacketProcessor.java` | `registerHandler(RecvOpcode.BEAUTY_ACTION, new BeautyHandler())`。 |
| 新增 | `org/gms/net/server/channel/handlers/BeautyHandler.java` | action 分发、校验、DB 调用、apply 三分支 + `equipChanged()`；包名/import 改为 `org.gms.*`，风格对齐 `GeneralChatHandler.java:36-55`。 |
| 新增 | `org/gms/server/beauty/BeautyData.java` | record，13 字段同 v83。 |
| 新增 | `org/gms/server/beauty/BeautyPackets.java` | `beautyOpen()/beautyData()`，字段顺序严格照 §1.2；可放 `util/PacketCreator.java` 静态方法（该文件已有 `dailyGiftSnapshot/witchForestState` `:364-408`，两种风格均可，建议独立类保持契约集中）。 |
| 新增 | `org/gms/server/beauty/BeautyStorage.java` | JDBC load/save/delete/unlock；改 `org.gms.util.DatabaseConnection`；加内存降级与 slf4j，仿 `ColoringPrismStorage.java:1-40`。 |
| 改 | `org/gms/net/server/channel/handlers/GeneralChatHandler.java` | 在 `:53` `heading` 前插 `@beauty`（或走 command 注册）。 |
| 改 | `UseCashItemHandler.java` 或客户端拦截 | 防止 5920000 走原生 USE_CASH_ITEM 双扣（§5.5）。 |
| 新增 | `resources/db/extensions/beauty.sql` | §2 两表，幂等 `IF NOT EXISTS`，仿 `daily_gift_state.sql`；DDL 运维手工执行。 |

### 8.3 落地前必须确认（不是本次只读研究能定的）

1. **opcode 最终取值**：0x174 两端当前 grep 空闲，但须走主端 opcode 门禁复核；本端习惯是成对私有号（C 奇 S 偶，见 `RecvOpcode.java:213-230`），若改用 0x3746/0x3747 需两端同改。
2. **WZ 资源是否具备**：`beautyRoom/**` 节点、5920000 Cash 道具在目标客户端版本 WZ 中是否存在；缺失时按 AGENTS.md §3.1 走 orange-wz + key 转换，不可裸拷。
3. **真机发包未验收**：本文是静态源码分析，未做任何运行时抓包/联调；所有包结构以源码逐字段比对为准，落地后必须用真机验证 OPEN→REQUEST→DATA→SAVE/APPLY/DELETE/UNLOCK 全链路。
4. **avatar 预览替代实现**：本端 avatar 类 API 未在本次范围内精读，仅给出业务规则，具体调用点需实现阶段再核。
