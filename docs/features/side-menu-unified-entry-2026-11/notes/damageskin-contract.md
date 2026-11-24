# 伤害皮肤——v83 扩展包契约提取与本端（C++ x64 + BeiDou-Server_s9）补齐方案

> 只读研究产物。来源：`E:\MXD\扩展改动\伤害皮肤`（v83 Kaentake.dll 注入版 + DAMAGE_SKIN_SERVER.md + WZ needed）、本端 `E:\project\MapleStory-Client`、服务端 `E:\project\BeiDou-Server_s9`（并对照 S10 完整实现）。
> 视角：**本端 s9 已实现大半，不是从零移植**。本文给出 ①现有链路缺口 ②扩展包可补充点 ③挂侧栏的最小增量方案。**仅建议，不改代码。**

---

## 0. TL;DR（给决策者）

| 项 | 结论 |
|---|---|
| 本端已通 | DB 列 `characters.damage_skin`（s9 V1.9.6）、登录包 `DAMAGE_SKIN=0xF7/247`、`DAMAGE_MONSTER=0xF6/246` 尾部带 skinId、`DamageNumber::apply_skin` 按 id 切字形、`Configuration` 本地缓存、`DamageSkin.img` 624 节点已入库（资源侧已验证可达） |
| **真正缺的三件** | ①**C2S 选择/购买路径**：s9 无 RecvOpcode、无 handler、无 `setDamageSkin` 调用；②**权威目录/拥有数据**：s9 无 `damageskin_catalog` / `damageskin_inventory` 表，无 catalog/inventory 下发包；③**选择 UI**：`UICharInfo.cpp:538-539` 只有 debug 入口，`UIDamageSkinPicker` 不存在 |
| 扩展包能补的 | picker UI 语义（MY/SHOP 双栏 + 预览 + 单位字形筛选）、catalog/inventory 服务端范式（S10/DAMAGE_SKIN_SERVER.md）、单位伤害 `1.8K` 渲染细节（v83 NoCustom/glUnit） |
| 不可移植 | v83 所有 32 位硬编码地址 + Detours/inline hook + `IWzProperty`/`CWnd` 引擎内部结构（见 §5） |
| 侧栏挂载 | `side_menu.json` 已有 `damageskin` 叶子（位于"美容院/伤害皮肤/融合外观"批次），只需接 `FeatureRegistry` open 回调 → 打开新建的 `UIDamageSkinPicker` |

---

## 1. v83 扩展包提取的准确契约（语义层，可移植）

### 1.1 Opcode 与包结构

来源：`Client-v83/src/damageskin.cpp:504-509,534-546,552-615`；`DAMAGE_SKIN_SERVER.md:567-600,629-670`。

**C2S（客户端→服务端）**
| opcode | 名称 | 载荷 |
|---|---|---|
| `0x110` | DAMAGE_SKIN_APPLY | `int32 skinId` |
| `0x111` | DAMAGE_SKIN_PURCHASE | `int32 skinId` |

**S2C（服务端→客户端）**
| opcode | 名称 | 载荷 |
|---|---|---|
| `0x170` | DAMAGE_SKIN_CATALOG | `int16 count` + count×(`int32 skinId`, `int64 priceMesos`) |
| `0x171` | DAMAGE_SKIN_INVENTORY | `int32 activeSkinId` + `int16 count` + count×`int32 skinId`（不含 id=0） |
| `0x172` | DAMAGE_SKIN_RESULT | `byte op`(1=apply,2=purchase) + `byte ok` + `int32 skinId` + `int32 newMesos` |
| `0x173` | DAMAGE_SKIN_BROADCAST | `int32 charId` + `int32 skinId` |

> ⚠️ 这些是 **v83/Cosmic 的 0x110-0x173 段**，与本端 s9 的 `0xF6/0xF7` 段**不是一套**。本端不得照抄号段；若新增 C2S，须在 s9 `RecvOpcode`（当前无 skin 项）避空申请新号并 C/S 成对。

**扩展包客户端处理语义**（`damageskin.cpp`）：
- `0x170` → 填 `g_vShopCatalog`，调 `RefreshDamageSkinPicker()`。
- `0x171` → 填 `g_vOwnedSkins`、`g_nDamageSkin = active`，刷 picker。
- `0x172` → `ok=0` 忽略；`ok=1 且 op=1` 设 `g_nDamageSkin=sid` + Notice "Damage skin has been applied."；`ok=1 且 op=2` 把 sid 加入 `g_vOwnedSkins` 并刷 picker。
- `0x173` → `SetSkinForChar(charId, skinId)`；若 `charId == g_nLocalCharId` 同步 `g_nDamageSkin`。
- 发包：`COutPacket(op); Encode4(skinId)`，fire-and-forget；服务端的 result 包为权威回执。

### 1.2 DB 表字段

来源：`DAMAGE_SKIN_SERVER.md:12-43`（Cosmic 版）；对照 `E:/project/BeiDou-Server_s9/.../V1.9.6__add_characters_damage_skin.sql:12-13`（s9 现有）。

```sql
-- v83 扩展包（Cosmic，三张）
ALTER TABLE characters ADD COLUMN activeDamageSkin INT NOT NULL DEFAULT '0';
CREATE TABLE damageskin_catalog (
  skinId INT NOT NULL, priceMesos BIGINT NOT NULL DEFAULT '10000000',
  PRIMARY KEY (skinId)) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
CREATE TABLE damageskin_inventory (
  id INT NOT NULL AUTO_INCREMENT, characterId INT NOT NULL, skinId INT NOT NULL,
  acquiredAt TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY(id), UNIQUE KEY uq_char_skin (characterId,skinId),
  KEY idx_char(characterId),
  CONSTRAINT fk_damageskin_char FOREIGN KEY(characterId) REFERENCES characters(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

```sql
-- s9 现状（已落库，仅一列）
ALTER TABLE `characters` ADD COLUMN `damage_skin` INT(11) NOT NULL DEFAULT 0
  COMMENT '玩家选择的伤害皮肤ID（0=默认BasicEff）';
```

**关键差异**：s9 只有"当前选择"一个标量，**没有拥有清单和价格目录**。扩展包的 `damageskin_catalog`/`damageskin_inventory` 正是缺口②的现成范式（S10 已实现，见 §4.2）。

> 用户原话提到 `characters.activeDamageSkin` 与 `damageskin_catalog`——前者是 **v83 扩展包**的列名，本端 s9 对应列名为 **`damage_skin`**（务必区分，别在本端找 `activeDamageSkin`）。

### 1.3 伤害数字渲染：按皮肤切换的 WZ 节点路径/帧

来源：`damageskin.cpp:156-157,203-246,278-315,337-347`；`DamageNumber.cpp:219-253`。

**v83 扩展包（Effect/BasicEff.img 体系）**
- 加载根：`Effect/BasicEff.img/damageSkin/<id>`，枚举子节点，`id` 为十进制字符串。
- 一个完整皮肤需 4 个子属性：`NoRed0`（普通·小）、`NoRed1`（普通·大）、`NoCri0`（暴击·小）、`NoCri1`（暴击·大）；digit loop 从里取 key `"0".."9"` 的 canvas 逐位绘制。
- 单位皮肤：`damageSkin/<id>/NoCustom`，当 `NoCustom/customType == "glUnit"` 时启用；其 children `"0".."3"` 分别是 `.`/`K`/`M`/`B` 的字形（`damageskin.h:18-29`）。CExtension 用 `CUnitPropertyWrapper` 把 base 数字的 `"0".."9"` 与 NoCustom 的 `"0".."3"` 合并；`FormatUnitDamage` 把 ≥1000 格式化为 `1.8K`/`1M`/`1B`，再把 `.`/`K`/`M`/`B` 编码成 byte `58/59/60/61`（经引擎 `(byte-'0')` 后映射为 wrapper key `"10".."13"`）。
- MISS：`Effect_HP` 的默认 miss 取 `NoRed0` 下的 `Miss` 子 canvas，皮肤切换时改取皮肤的 `NoRed0`。

**本端（Journey + s9）**
- `DamageNumber::apply_skin(skin, out, type)`（`DamageNumber.cpp:219-253`）：`nl::nx::Effect["DamageSkin.img"][std::to_string(skin)]`，取 `NoRed1`/`NoRed0`（NORMAL）、`NoCri1`/`NoCri0`（CRITICAL），TOPLAYER 保持 BasicEff；缺 `NoRed1` 则回退默认。
- 默认 `BasicEff.img` 的 `NoRed0/1`、`NoCri0/1`、`NoViolet0/1`（`DamageNumber.cpp:255-265`）。
- **本端资源已就绪**：`DamageNumber.cpp:215-218` 注释 + 证据 `evidence/damagenumber-083-round70.json` 记 624 皮肤节点、`NoRed1/NoCri1` 齐全；并入计划见 `.codebuddy/plans/port-damageskin-resources_8cc191f8.md`（已全部 completed，目标 `Data\Effect\DamageSkin.img`）。
- **本端缺口（渲染细节）**：无 `NoCustom`/`glUnit` 单位字形处理（`DamageNumber.cpp:76-77` 注释明确 BasicEff 无 K/M/B，大数按最大宽 260px 等比缩放）；`D-A` 皮肤数字间距仍按 BasicEff 近似宽度表（`DamageNumber.cpp:149-151`），视觉级待裁决。

> 资源规格差异：扩展包走 `Effect/BasicEff.img/damageSkin/<id>`，本端走 `Effect/DamageSkin.img/<id>`。**皮肤 id 语义相同，WZ 容器不同**；移植语义时按本端容器取节点。

### 1.4 皮肤选择 UI 流程

来源：`damageskinpicker.cpp:19-38,71,520-568,1116-1360,1363-1488`。

- 打开方式：双击现金道具 **`5910000`**（picker opener），客户端拦截 `CWvsContext::SendConsumeCashItemUseRequest`，**不转发、不消耗道具**（再次点击关闭）。服务端 `UseCashItemHandler` 的 `itemType==591` 分支下发 catalog+inventory（`DAMAGE_SKIN_SERVER.md:688-693`）。
- 布局：430×360 窗，上方预览区（3 种 PreviewBackground 可切换），下方左右双栏 `MY DAMAGE SKINS` / `DAMAGE SKIN SHOP`，各 3×3 网格（每页 9 项，严格翻页），分页箭头 + 页码输入框（GO）、`check0/check1` 暴击切换、两侧"只看单位皮肤"筛选、底部 `APPLY`/`PURCHASE`/`BtClose`。
- 数据流：`MY` = `g_vOwnedSkins`（去 id=0）；`SHOP` = `g_vShopCatalog` 且 skinId 必须在本端 WZ 有 `DamageSkinProp`（离线测试时 catalog 为空才回退全部 WZ id）。预览跟随最后点击的栏（`m_nPreviewSide`），普通渲染 `12345`，单位筛选开则渲染 `1.8K`。
- 交互：点格子选中→预览刷新；APPLY→`Send_DamageSkinApply(id)`；PURCHASE→先查已拥有（有则 Notice 拦截），否则千分位格式化价格 + `CUtilDlg_YesNo` 确认→`Send_DamageSkinPurchase(id)`。服务端 result 包回来后 `g_nDamageSkin`/`g_vOwnedSkins` 更新并 `RefreshLists()`（新购项立即出现在 MY）。
- 商店价格：客户端从 catalog 取，缺失时兜底 `10000000`（v83 默认价，对应 `DamageSkinCatalog.DEFAULT_PRICE_MESOS=10_000_000`；S10 改为 `1_000_000_000`——见 §4.2 差异）。

### 1.5 item / 商店价格逻辑

来源：`damageskinpicker.cpp:71`（opener id）；`DAMAGE_SKIN_SERVER.md:237,261-296,400-434`；s9 `DamageSkinCashItems.java`。

| 项 | v83 扩展包 | s9 现状 |
|---|---|---|
| 打开入口道具 | `5910000`（不消耗） | `OPENER_ITEM_ID = 5_910_000`（已定义；窗口商城"XY玩法"页卖入口券） |
| 皮肤本体 SKU | catalog 直接按 skinId 定价（无独立 itemId） | `BASE=5_920_000`，`itemId = BASE + skinId`，`MAX_ITEM_ID=5_929_999`，NX 默认价 50,000（`DamageSkinCashItems.java:12-16`） |
| 价格表 | `damageskin_catalog.priceMesos`，WZ 启动播种，默认 10,000,000 | **未移植**：`CashShopWindowPurchase.java:30-33` 明确"去掉皮肤本体虚拟 SKU 分支（S9 未移植 xy_damageskin_* / DamageSkinInventory）" |
| 购买流程 | `PurchaseHandler`：校验 skin≠0、价格在表、未拥有、余额够→`gainMeso(-price)`→加 inventory→result | 无 C2S handler；商城购买分支被降级桩移除 |
| 拥有判定 | `DamageSkinInventory.ownsSkin(skinId)`（DB 表） | `Character` 只有一个 `damageSkin` 标量，无 inventory 概念 |

---

## 2. 现有 s9 链路盘点（lead 提供 + 本次复核，均带行号）

| 层 | 文件:行 | 现状 |
|---|---|---|
| DB | `BeiDou-Server_s9/.../V1.9.6__add_characters_damage_skin.sql:12-13` | `characters.damage_skin INT NOT NULL DEFAULT 0` ✅ |
| DB 映射 | `CharactersDO.java:82-83`、`Character.java:157,6742,6934` | 字段 + 加载 + 保存 ✅ |
| 服务端发包 | `PacketCreator.java:4389-4398` damageMonster 尾部 `writeInt(skinId)`；`:4411-4415` damageSkin(int) 写 `int32` | ✅ |
| opcode | `SendOpcode.java:262 DAMAGE_MONSTER(0xF6)`、`:405 DAMAGE_SKIN(0xF7)` | ✅ |
| 登录推送 | `PlayerLoggedinHandler.java:275` `sendPacket(PacketCreator.damageSkin(player.getDamageSkin()))` | ✅（但值恒为 0，见缺口①） |
| 客户端收包 | `Net/PacketSwitch.cpp:264 DAMAGE_SKIN=247`、`:647-648` 注册 `DamageSkinHandler` | ✅ |
| 客户端 handler | `Net/Handlers/MapObjectHandlers.cpp:1482-1495` `DamageSkinHandler::handle`：`skin!=0` 才写 Configuration，否则保留本地缓存 | ✅ |
| 配置缓存 | `Configuration.cpp:922-941`、`Configuration.h:121-125,1119-1124`（`ui.damage_skin`） | ✅ |
| 伤害数字渲染 | `Gameplay/Combat/DamageNumber.cpp:27-35,219-253` | ✅（按 id 切 `DamageSkin.img/<id>` 字形） |
| 战斗面板 | `CommonHandlers.cpp:83` `SetItemFinalDamageHandler` 读 `damage_skin_id`；`PacketCreator.java:7958-7964` 0x175 下发 | ✅ |
| 资源 | `DamageSkin.img` 624 节点已入库（port 计划 completed） | ✅ |
| UI 入口 | `IO/UITypes/UICharInfo.cpp:536-540` 注释明确"仅 debug 入口，正式选择 UI 按需求再接入" | ⚠️ 缺口③ |
| 侧栏种子 | `SideMenuService.java:83` `{"damageskin","伤害皮肤","0"}`；`resources/side_menu.json` features[] | ✅（种子已存在） |

---

## 3. 现有链路还缺什么（核心结论）

1. **C2S 选择/购买路径完全缺失（最大缺口）**
   - s9 `RecvOpcode.java` 无任何 damage skin 项（grep 0 命中）；`net/server/channel/handlers` 无 apply/purchase handler。
   - `Character` 有 `setDamageSkin`（`Character.java:6742,6934`）但**仅服务端登录/保存时被调用**，没有玩家主动设置的入口。
   - 后果：`DamageSkinHandler::handle` 注释（`MapObjectHandlers.cpp:1487-1491`）已写明——"s9 has no c2s path writing damage_skin, so the login packet always carries 0"，只能靠本地 `Configuration` 缓存，换机/清档即丢，且其他玩家看不到你的皮肤。
2. **皮肤目录/拥有数据缺失**
   - 无 `damageskin_catalog` / `damageskin_inventory` 表；无 catalog/inventory 下发包；客户端 picker 若建起来也拿不到 MY/SHOP 数据。
   - s9 `CashShopWindowPurchase.java:30-33` 明确这是"隔离降级桩"，皮肤本体 SKU 分支被移除。
3. **选择 UI 缺失**
   - 无 `UIDamageSkinPicker`；`UICharInfo.cpp` 只有 debug 按钮，点击发个 UseCashItem 包就没了。
4. **（渲染细节，非阻塞）** 单位皮肤 `1.8K`/K/M/B 未实现；皮肤数字间距仍用 BasicEff 近似表（D-A 决策未决）。

---

## 4. 扩展包里可补充的点（按优先级）

### 4.1 Picker UI 语义（直接从扩展包借鉴，重写为本端组件）
- 双栏 `MY` / `SHOP` + 顶部实时预览（选中即变，暴击切换、单位皮肤 `1.8K` 预览）。
- 3×3 分页（每页 9）、严格翻页、页码 GO、页码输入框键盘路由。
- 购买前置校验：已拥有则 Notice 拦截；否则千分位价格 + Yes/No 二次确认（防误触烧钱）。
- 服务端 result 包为权威：`ok=1` 才更新本地列表并刷 UI（客户端不乐观提交）。
- 打开入口：现金道具 `5910000`（s9 已定义 `OPENER_ITEM_ID`），点击打开 UI 而不消耗道具。

### 4.2 皮肤目录 / 商店（S10 有现成完整实现，可整段移植到 s9）
**S10 已实现、s9 缺失的 5 个类**：`org.gms.client.DamageSkinCatalog`、`DamageSkinInventory`、`net.server.channel.handlers.DamageSkinApplyHandler`、`DamageSkinPurchaseHandler`、`server.cashshop.DamageSkinCashItems`（s9 仅后者存在且被降级桩绕开）。
- `DamageSkinCatalog`（S10 `DamageSkinCatalog.java:19-55`）：启动扫 `Effect/BasicEff.img/damageSkin`，把有 `NoRed0/NoRed1/NoCri0/NoCri1` 四子节点的皮肤 `INSERT IGNORE` 进 catalog，默认价 S10 为 **`1_000_000_000`**（v83 扩展包是 `10_000_000`，**注意取哪个要拍板**）。
- `DamageSkinInventory`（类存在，含 `ownsSkin`/`addSkin`/`getOwnedIds`/`DEFAULT_SKIN_ID`）。
- 需补 DB：`damageskin_catalog`、`damageskin_inventory`（schema 见 §1.2，S10 应有对应 migration 可参照）。
- 需补 S2C 包（可复用 v83 语义、用 s9 空 opcode）：catalog/inventory/result/broadcast 四个 builder。
- 需补 C2S opcode + handler + `PacketProcessor` 注册（S10 `DamageSkinApplyHandler.java:18-33` 可直接抄逻辑：`ownsSkin` 校验→`setActiveDamageSkin`→result→地图广播）。
- 购买 handler 同样整段移植（`gainMeso(-price)`、`mesoToPacketInt` 做 32 位折算是 S10 已处理的坑）。

### 4.3 渲染细节（可选增强，非阻塞）
- `NoCustom`/`glUnit` 单位字形：本端 `DamageSkin.img` 若有该节点，可在 `DamageNumber` 里加 ≥1000 → `1.8K` 格式化 + `.`/`K`/`M`/`B` 字形读取（扩展包 `FormatUnitDamage`/`CUnitPropertyWrapper` 是语义参考，不是代码模板）。
- D-A 间距：按皮肤位图实际宽度算 advance（本端 `getadvance` 目前用 BasicEff 近似表，见 `DamageNumber.cpp:149-151`）。

---

## 5. 不可移植 vs 可移植（硬边界）

### 5.1 不可移植：32 位硬编码地址 / Detours hook（**禁止照搬**）
| 类别 | 证据（v83 源） |
|---|---|
| inline splice / 硬编码地址 | `damageskin.cpp:74-130`：`kSplice_Normal_Site=0x00437DA2`、`kSplice_Crit_Site=0x00437D8C`、`kSplice_MergeRet=0x00437DB6`、`kSplice_Miss_Site=0x00438A51`、三个 canvas imm8 patch 地址、`SmartPtrAssign=0x004027BE` |
| 网络/对象硬编码地址 | `damageskin.cpp:450-466,498,654-656,705-709`：`ClientSocket_ProcessPacket=0x004965F1`、`SendPacket=0x0049637B`、`Instance=0x00BE7914`、各 `Decode1/2/4`、Notice `0x009929DD`、`CMob::OnHit=0x00668B83`、`CUserLocal_Instance=0x00BEBF98` + 偏移 `1130*4` |
| Picker 硬编码地址 | `damageskinpicker.cpp:73-74,104-109,118,127,1441,1481-1485`：`SendConsumeCash=0x00A0A63F`、`get_consume_type=0x004863D5`、`get_basic_font=0x0098A707`、`play_ui_sound=0x00989588`、YesNo `0x00992BFD`、Notice `0x009929DD`、`CWndMan::ProcessKey=0x009E40F1` |
| hook 机制 | `hook.cpp:67-96,146-172`：`DetourTransactionBegin/Attach/Commit`、`VMTHook`、`PatchMemory`/`VirtualProtect` x86 `__declspec(naked)` 裸汇编 trampoline（`damageskin.cpp:357-390`）、`__thiscall`/`__stdcall` x86 调用约定、`IWzPropertyPtr`/`CWnd` 引擎 COM 对象布局、`CInPacket.m_uOffset@0x14` |

> 本端是 C++ x64 + Journey 架构（nlnx、`Net/Handlers`、`Stage`、`Charset`），**以上地址/hook/对象模型全部失效**。只取语义（§1/§4），不取实现。

### 5.2 可移植：纯业务 / 资源语义
- Opcode **语义**（四类包、字段序、回执模型）——号段重申请。
- DB 表语义（catalog/inventory/active 列，三表）。
- WZ 节点语义：`damageSkin/<id>/NoRed0|NoRed1|NoCri0|NoCri1`、`NoCustom/customType=glUnit` + 子 `"0".."3"`；本端容器 `Effect/DamageSkin.img/<id>`。
- 皮肤 id 约定：`0 = 默认 BasicEff`，非 0 为皮肤 id；MISS 取 `NoRed0`。
- picker 交互语义（双栏/预览/购买确认/result 权威）。
- item 语义：`5910000` opener 不消耗；皮肤 SKU `5920000 + skinId`（s9 已定义）。
- 攻击归因语义：每个攻击者按自己皮肤渲染（v83 用 `CMob::OnHit` 临时切全局；本端应由 `DAMAGE_MONSTER` 包带的 skinId 直接构造 `DamageNumber`（`show_mob_damage(oid, damage, skin)`，见 `MapObjectHandlers.cpp:1484`），无需全局切换）。

---

## 6. 本端新增类 / 文件建议（仅建议，不写）

> 对齐本端风格：`Net/Handlers/*Handler`、`IO/UITypes/*`、`Gameplay/Combat/DamageNumber`、`Configuration`、`Stage`。服务端对齐 `org.gms` 风格（复制 S10 同名类）。

### 6.1 服务端（BeiDou-Server_s9，`org.gms`）
1. **DB migration**：`V1.9.x__add_damageskin_catalog_inventory.sql`（参照 §1.2；只落脚本，DLL 执行需用户授权——PLAN-C 门禁 17）。
2. **`org.gms.client.DamageSkinCatalog`**：从 S10 整类移植（WZ 播种 + `getPrice`/`getAll`）。
3. **`org.gms.client.DamageSkinInventory`**：从 S10 整类移植（`ownsSkin`/`addSkin`/`getOwnedIds`）。
4. **`Character` / `CharactersDO`**：复用现有 `damageSkin` 标量；新增 `getDamageSkinInventory()` 惰性成员，登录 `loadCharFromDB` / `saveCharToDB` 同步 inventory（对齐 `Character.java:6742,6934`）。
5. **`PacketCreator`**：新增 `damageSkinCatalog()`/`damageSkinInventory(chr)`/`damageSkinResult(op,ok,skinId,newMesos)`/`damageSkinBroadcast(cid,skinId)`（字段序按 §1.1，opcode 用 s9 空号）。
6. **`RecvOpcode` + 两个 handler**：`DAMAGE_SKIN_APPLY`/`DAMAGE_SKIN_PURCHASE` + `DamageSkinApplyHandler`/`DamageSkinPurchaseHandler`（从 S10 同名类移植）+ `PacketProcessor` 注册。
7. **推送时机**：`PlayerLoggedinHandler.java:275` 附近补 catalog+inventory；`UseCashItemHandler` 的 `itemType==591` 分支下发 catalog+inventory（不消耗道具）；地图进图广播（参照 v83 STEP 16 / S10）。
8. **`CashShopWindowPurchase`**：解除皮肤本体虚拟 SKU 分支降级桩（对齐 S10），让商城能买皮肤、解锁 inventory。

### 6.2 客户端（MapleStory-Client，C++ x64）
9. **`Net/PacketSwitch.cpp`**：新增 `DAMAGE_SKIN_CATALOG/INVENTORY/RESULT/BROADCAST` 四个 opcode（s9 空号，与服务端成对）+ `emplace` handler 注册。
10. **`Net/Handlers/DamageSkinHandlers.{h,cpp}`**（新文件）：四个 handler，写入 `DamageSkinState`（见 11）；RESULT 成功时刷 UI + 更新 `Configuration`。
11. **`Gameplay/Combat/DamageSkinState.{h,cpp}`**（新文件，单例）：`vector<CatalogEntry{int id; long long price}>`、`vector<int> owned`、`int active`；提供 `apply_result`/`set_catalog`/`set_inventory`；风格仿 `CombatPanelStats`/`Configuration`。
12. **`IO/UITypes/UIDamageSkinPicker.{h,cpp}`**（新文件）：基于本端 UI 框架（参照 `UICharInfo`/`UIWindowCashShop`/`UIBeautySalon` 的现有类），双栏网格 + 预览（`Charset` 读 `DamageSkin.img/<id>` 帧）+ Yes/No 购买确认；打开时从 `DamageSkinState` 拉数据；从 `Stage`/`FeatureRegistry` 的 open 回调触发。
13. **`Configuration`**：`get/set_damage_skin` 已存在（`Configuration.cpp:922-941`），保留为本地缓存；服务端 result 为权威回写。
14. **`Gameplay/Combat/DamageNumber.cpp`**（小改）：（可选）单位字形 + 按皮肤位图宽算 advance（D-A 需先拍板）；现有 `apply_skin` 主体不动。
15. **`FeatureRegistry` / `side_menu.json`**：`damageskin` 叶子已存在（`SideMenuService.java:83`），接 open 回调 → `UIDamageSkinPicker::show()`。

### 6.3 C2S 发包（客户端→服务端）
16. 本端 `Net` 层新增两个 OutPacket（仿现有 `UseCashItemBasicPacket` 等）：`DamageSkinApplyPacket(skinId)`、`DamageSkinPurchasePacket(skinId)`，由 picker 的 APPLY/PURCHASE 按钮触发；opcode 与 §6.1 服务端成对。

---

## 7. 挂侧栏的最小增量方案（在现有基础上补齐）

> 目标：以最小改动打通"侧栏点伤害皮肤 → 打开 picker → 选/买 → 服务端权威落库 → 伤害数字按皮肤渲染"，不重写已通链路。

**阶段 A — 服务端先让 C2S 存在（否则 UI 是空壳）**
1. 落 `damageskin_catalog`/`damageskin_inventory` migration（脚本，DML/DDL 执行待授权）。
2. 从 S10 移植 `DamageSkinCatalog`/`DamageSkinInventory`/两 handler（§6.1 第 2/3/6 项）；`RecvOpcode` 申请两个新号。
3. `PacketCreator` 加 catalog/inventory/result 三包；登录 + 591 使用分支推送。
4. `CashShopWindowPurchase` 恢复皮肤 SKU 分支（§6.1 第 8 项）。

**阶段 B — 客户端接包 + 建状态 + 建 UI**
5. `PacketSwitch` 注册三个 handler；新建 `DamageSkinState` + `DamageSkinHandlers`。
6. 新建 `UIDamageSkinPicker`（双栏/预览/确认），数据全部来自 `DamageSkinState`，不发本地假数据。
7. `UICharInfo` 的 debug 入口保留或替换为转发（二选一，避免两个入口语义分叉）。

**阶段 C — 挂侧栏**
8. `side_menu.json` 的 `damageskin` 叶子 open 动作绑 `FeatureRegistry` 里的 `UIDamageSkinPicker::show`（参照 PLAN-C P0.4 / P3 第 14 项；"美容院/伤害皮肤/融合外观"同批）。
9. 验证：`mvn compile`（s9）+ 客户端 VS2019 build + 真机走一遍"侧栏打开→购买→应用→换图重登仍生效→他人可见"。

**关键约束（沿用 PLAN-C 门禁）**
- opcode 新号须 C/S 成对、避空，禁止抄扩展包 `0x110-0x173` 或 `0x174`。
- WZ 若需新增/改动，一律 orange-wz MCP + 显式转 key + `--resource-probe` 短启（AGENTS.md §3.1）；**本方案假设 `DamageSkin.img` 已就绪，不需再动资源**。
- DB 变更只出迁移脚本，执行需用户授权；不擅自对 live 库 DDL/DML。
- 伤害数字间距（D-A）与单位字形属视觉增强，单列决策点，不阻塞主链路。

---

## 8. 证据索引（行号级）

**v83 扩展包**
- 渲染：`Client-v83/src/damageskin.cpp:49-65`(总述)、`:74-130`(地址)、`:148-252`(加载)、`:278-347`(选 sprite)、`:712-959`(单位字形)
- 网络：`:504-509`(opcode)、`:534-546`(发包)、`:552-615`(收包)、`:627-642`(分发)、`:648-709`(攻击归因)
- UI：`damageskinpicker.cpp:19-38`(布局)、`:71`(item id)、`:520-568`(列表)、`:623-718`(预览)、`:1116-1360`(点击)、`:1363-1488`(打开/键盘)
- 服务端契约：`DAMAGE_SKIN_SERVER.md:12-43`(SQL)、`:210-330`(Catalog)、`:332-436`(Apply/Purchase)、`:567-600`(opcode)、`:629-670`(包)、`:688-693`(591分支)、`:749-752`(登录推送)、`:770-783`(地图广播)
- 资源：`WZ needed/0590.img.xml`、`0591.img.xml`（入口道具 cash=1，无价格字段→价格走服务端 catalog）

**本端 s9**
- `BeiDou-Server_s9/gms-server/src/main/resources/db/migration/V1.9.6__add_characters_damage_skin.sql:12-13`
- `.../org/gms/util/PacketCreator.java:4389-4415`、`.../net/opcodes/SendOpcode.java:262,405`
- `.../net/server/channel/handlers/PlayerLoggedinHandler.java:275`
- `.../org/gms/client/Character.java:157,2813,6742,6934`；`.../dao/entity/CharactersDO.java:82-83`
- `.../org/gms/server/cashshop/DamageSkinCashItems.java:11-16`；`CashShopWindowPurchase.java:30-33`（降级桩）；`SideMenuService.java:83`
- `MapleStory-Client/Net/PacketSwitch.cpp:264,647-648`；`Net/Handlers/MapObjectHandlers.cpp:1482-1495`；`Net/Handlers/CommonHandlers.cpp:83`
- `Gameplay/Combat/DamageNumber.cpp:27-35,76-82,149-151,215-265`
- `Configuration.cpp:922-941`、`Configuration.h:121-125,1119-1124`
- `IO/UITypes/UICharInfo.cpp:536-540`
- `docs/features/side-menu-unified-entry-2026-11/PLAN-C-execution.md:12-14,24-27`（不可移植声明 + P3 任务）、`notes/ext-mods-inventory.md`（伤害皮肤行）

**S10 完整参考（移植源）**
- `BeiDou-Server_S10/gms-server/src/main/java/org/gms/client/DamageSkinCatalog.java:19-55`、`DamageSkinInventory.java`
- `.../net/server/channel/handlers/DamageSkinApplyHandler.java:18-33`、`DamageSkinPurchaseHandler.java`
