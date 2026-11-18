# 商城无商品：根因与修复（2026-11-16 实机坐实）

> 用户现象：进现金商城后商品列表空白（原先被「正在准备商品」大弹层糊弄）。
> 结论：**不是客户端解析错位，也不是服务端少发包**。v83 的在售目录基表本来就在
> 客户端本地 `Etc/Commodity.img`；s9 `openCashShop` 下发的那个 short 是
> `modified_cash_item`「改价/上下架覆盖表」的条数，运营没改价时为 0 —— 合法语义。
> 旧 handler 把它当成「主商品列表条数」，于是目录恒空。

## 1. 服务端发包真值（逐字核对）

`E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/util/PacketCreator.java:7399-7445`
`openCashShop(Client c, boolean mts)`，非 MTS 分支：

| 顺序 | 服务端 | 说明 |
| --- | --- | --- |
| 1 | `addCharacterInfo(p, player)` | 角色全量 |
| 2 | `writeByte(1)` | Not MTS |
| 3 | `writeString(accountName)` | 账号名 |
| 4 | `writeInt(0)` | 固定 0 |
| 5 | `writeShort(items.size())` | **modified_cash_item 覆盖表条数**（源码注释 `//Guess what`） |
| 6 | N × `writeModifiedCashItem` | 变长位图编码，见 §3 |
| 7 | `skip(121)` | 占位 |
| 8 | 热卖网格 | `writeInt(cat) writeInt(gender) writeInt(sn)`，cat 1..8 × gender 0..1 × 该榜条目 |
| 9 | `writeInt(0) writeShort(0) writeByte(0) writeInt(75)` | 尾部 11B |

表注释来源：`resources/db/migration/V1.9.4__add_all_table_comments.sql:584`
`ALTER TABLE modified_cash_item COMMENT = '商城物品改价/属性覆盖表'`。
同构取数逻辑见 `service/CashShopService.java:59-73`（WZ 基表 + DB 覆盖，
「以数据库为准更新可能更新的字段」）——服务端自己也把 WZ 当目录基表。

## 2. 抓包字节表（report-20261030-2232-paired.md:854，包长 1695B）

| 偏移 | 内容 |
| --- | --- |
| @589 | `01` = writeByte(1) NotMTS |
| @590-591 | `05 00` = account 串长 |
| @592-596 | `admin` |
| @597-600 | `00 00 00 00` = writeInt(0) |
| @601-602 | `00 00` = **writeShort(0) 覆盖表条数** |
| @603-723 | `skip(121)` 全 0 |
| @724-1683 | 热卖网格 80 组 × 12B，字段序 **(cat, gender, sn)** |
| @1684-1694 | `00 00 00 00 00 00 00 4b 00 00 00` = 尾部 int0/short0/byte0/int75 |

网格实测：5 个 SN（`2fdfdc4/2fdfe05/2fdfe35/2fc7728/2faf0af`）× 8 类 × 2 性别 = 80 组。
游标模拟 `tools/_cashshop_cursor_sim.py` 输出 `VERDICT: EXACT 全消费`（终点 = 1695，残余 0）。

**踩坑留证**：曾按 12B/组把网格起点误判为 @732、字段序误判为 (sn,cat,gender)，
因而以为 @1680「尾巴越界」、尾部 11B「重叠 -8」。正确起点是 @724、序是 (cat,gender,sn)，
两者严丝合缝。别再肉眼横跳，用锚点（writeInt(75) 的 `4b 00 00 00`）反推。

## 3. CommodityFlag 位图编码（覆盖表单项）

`constants/game/CommodityFlag.java`：写序 = `sort` 升序，只写非空字段，
`FLAG` = 所写字段的 bit 之和。客户端解码须按同一张表：

bit0 ITEM_ID(int) · bit1 COUNT(short) · bit2 PRICE(int) · bit3 BONUS(byte) ·
bit4 PRIORITY(byte) · bit5 PERIOD(short) · bit6 MAPLE_POINT(int) · bit7 MESO(int) ·
bit8 FOR_PREMIUM_USER(byte) · bit9 COMMODITY_GENDER(byte) · bit10 ON_SALE(byte) ·
bit11 CLASS(byte) · bit12 LIMIT(byte) · bit13 PB_CASH(short) · bit14 PB_POINT(short) ·
bit15 PB_GIFT(short) · bit16 PACKAGE_SN(byte n + n×int)；前两个固定 SN(int)+FLAG(int)。

旧 handler 这里的注释「11 字段 / 44B 每件」是凭猜测写的，与位图编码不符。
注意 bit2=PRICE、bit4=PRIORITY **不连续**，按 bit 序读会错位（本次已修）。

## 4. 本地目录数据（`tools/_nx_commodity_stats.py`）

`Data/Etc/Commodity.img`（= `.nxcache/Etc.nx`）实测：

- 总行数 8947；`OnSale==1` 共 **2010** 行（其余 6937 为下架）。
- 上架行按 SN 首位类目：1:53 2:1426 3:93 5:288 6:137 7:9 8:4。
- 上架行 Gender 取值：2:1947 1:33 0:26 -1:4。
- SN 全是 8 位；`Price` 单位是 NX 点券。

**图标/名称 vs 价格的坑**：`Item.nx/Cash/` 只有 `05xx.img` 号段，装备类现金
道具（10xxxxx 等）不在其中，故 `ItemData::get_price()` 对多数商城条目不可用；
标价必须取 `Commodity.img` 的 `Price`。已在 `UICashShop::Item` 增加 `nx_price`
字段承载（>0 优先，否则回落 ItemData）。

## 5. 修复落点（全部在客户端 repo）

| 文件 | 改动 |
| --- | --- |
| `IO/UIStateCashShop.h` | `CashCommodity` 补 period/priority/gender/onsale/has_onsale + 默认值 |
| `Net/Handlers/CashShopHandlers.cpp` | 新增 `decode_modified_cash_item`（按 §3 位图）与 `load_local_commodities`（本地基表 + s9 同序排序） |
| 同上 | `SetCashShopHandler::handle` 重写：覆盖表→overrides；网格值域守卫 + `seek(mark)` 回退；尾部 11B 显式读；本地目录 ⊕ 覆盖合并；仅裁 `!onsale` |
| `IO/UITypes/UICashShop.{h,cpp}` | `Item` 带 `nx_price`，`get_price()` 优先用它 |

刻意**不做**的三件事（做了就是自己造需求 / 少显示商品）：
1. 不按性别裁剪目录 —— s9 的过滤条件只有 onSale/itemId。
2. 不硬编码 80 组网格 —— 榜单条数随运营数据变，用值域守卫。
3. 覆盖表解码越界只丢覆盖项，不让整包解析炸掉（try/catch + LOG_WARN）。

## 6. 验收证据

- 协议门禁 `python tools/gen_opcode_matrix.py --check` → `GATE OK`。
- 游标模拟 → `EXACT 全消费`（残余 0）。
- `MSBuild /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v142` → `BUILD_EXITCODE=0`，`error C|error LNK|fatal` 计数 0。
- 增量缓存已强制失效：`CashShopHandlers.obj` 11:03:39 > cpp 11:03:28；exe SHA `95a73ea9…` → `b34ba348…`（`E:\MXD\BeiDou-Client-xy\MapleStory.exe`）。

## 7. 未做（明确边界，不虚报）

- 热卖网格只解析不渲染（`best_sellers` 已收集，UI 热卖位待接）。
- 分类/子分类翻页、礼包详情（PACKAGE_SN 子件）、按类搜索未做——需 `Category.img`
  与 s9 `getCommodityByCategory` 的 prefix 口径（`id + %02d(subId)` 前缀匹配）。
- 实机点击 Buy 走 `CashShopBuyPacket`（action 0x03 + useNX + sn）已具备，
  但尚未在真机上确认服务端扣费回执路径。
