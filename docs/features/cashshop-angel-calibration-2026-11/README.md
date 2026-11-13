# 商城（CashShop）· Angel 083 ↔ 当前端 六/七维校准

> 第七面 · 2026-11-09 · 基准 = GMS v83 / 083（Angel.exe pid 29620 port 13339）
> 性质：校准 / 对照 / 决策点 / Plan 文档，**非实现提交**。全程只读取证，未改任何源码，未创建提交。

## §0 五条结论

- **0.1 同二进制、只补语义**：七面 EA 字节级一致（登录 327/328、HUD 547/548、core 439/441、social 752/752、pet 968/968、activity 844/844、**cashshop 938/938**）。`0x4b7404 = ?OnKey@CCSWnd_List@@UAEXII@Z` 在 Angel 头 8 字节 `[255,116,36,8,131,193,252,255]`，BeiDou 侧 `get_func_attr = 0xffffffffffffffff` 但 8 字节相同 → **IDA 覆盖差异，非布局差异**。Angel 3122 修饰名 / 361 类 / 2612 方法 vs BeiDou 50 / 10 / 0，差异全在语义层。
- **0.2 商城是单体大窗 + 7 子窗复合**：Angel `CCashShop` 66 方法，7 子窗 `CCSWnd_Char/List/Inventory/Locker/Tab/Best/Status`；当前端 `UICashShop` 单类 + 20 `Buttons` + 独立 `UIStateCashShop` state —— **独立真窗，非第五/六面 DIY 合并壳**。
- **0.3 两级包分发**：外层 `CCashShop::OnPacket` 11 case（323..333 **无 330**）；内层 `OnCashItemResult` 48 case（正值 31 + 负值 17，负值 = `Decode1` unsigned byte 溢出成 signed char）。48 case 中 **24 落未命名 `sub_47xxxx`**，本轮记 `⏳`。
- **0.4 opcode 双端**：Angel c2s 7 = `21/38/68/123/229/230/256`；当前端 c2s 5(+1) = `40/79/228/229/230(+256)`。`229/230` 重合 ✅，`21/38/68/123` 子功能并入 UI 逻辑 ⚠️ **s9 优先**。
- **0.5 校准 ≠ 改回原生**：改回 083 opcode 会断 s9 联机；所有冲突交用户裁决（§6 D1–D14）。`⏳` 集中在 E/F 两维（orange-wz IMG 阻塞，见 `notes/r03-img-blocked-fallback.md`）。

## §1 取证基线

### 1.1 IDA 端点表
| 端点 | pid | port | imagebase | strings | 用途 |
| --- | --- | --- | --- | --- | --- |
| Angel.exe | 29620 | 13339 | 0x400000 | 1281 | 修饰名 / 反编译语义（一手） |
| BeiDou.exe | 31332 | 13337 | 0x400000 | 3889 | 字符串 / EA 字节级交叉（一手） |

### 1.2 三联门禁
| 门禁 | 状态 | 说明 |
| --- | --- | --- |
| REA shipped-artifact | ✅ | 既有解析链 |
| IDA Angel + BeiDou | ✅ | 938/938 EA 一致，6 件 evidence |
| orange-wz(IMG) | ⚠️ 阻塞 | 见 r03；F 样式维整体降级 `⏳` |

### 1.3 采集脚本 / evidence / notes
| 类别 | 文件 |
| --- | --- |
| 脚本 | `tools/ida_angel_cashshop_{ea,decompile,normalize,crosscheck,facts}.py`、`tools/_gen_cashshop.py` |
| evidence | `evidence/angel-cashshop-ea.json`(217284B)、`angel-cashshop-decompile.json`(1182583B)、`angel-cashshop-decompile-full.json`(1050955B)、`angel-cashshop-truncated.json`、`angel-cashshop-facts.md`(203501B / 3939 行 / 266 `SP_` / 44 类)、`angel-beidou-cashshop-ea-crosscheck.json`(937/938)、`angel-beidou-cashshop-ea-bytelevel-audit.json`(1886B) |
| notes | `r01-angel-native-cashshop-facts.md`(136 行 / Angel 原生基线)、`r03-img-blocked-fallback.md`、`r05-cashshop-four-state-audit.md` |

### 1.4 本面独有技术点
1. 1-EA 缺失的字节级归因（非阈值口径问题）：`present >= len(items)-2` 判定正确，但措辞须更正为 **938/938 一致**。
2. 内层 48 case 的 signed/unsigned 溢出还原（负值 → 133..162 区间）。
3. 独立真窗形态识别：区别于 pet/activity 面的合并壳。
4. `commodity_count > 0 && < 4000` + `skip(count*44)` 的容错读序与 44 字节/件结构对齐。

## §2 口径

### 2.1 七维 × 五判定
A 功能 14 / B 结构 14 / C 协议 14 / D 数据 14 / E 排版 16 / F 样式 12 / G 交互 14 = **98 条**。
判定：`✅ 一致` · `⚠️ 差异-s9优先` · `❌ 缺失` · `⊘ 不适用` · `⏳ 待证据(IMG/符号)`。

### 2.2 证据优先级
实时运行行为 > 抓包 > 当前服务资源 > 进程配置 > 持久化状态 > 生成产物 > 源码 > 注释与死代码。本面以 IDB 反编译 + 当前端源码逐字为一手，注释（含 095 污染注释）仅作留痕。

### 2.3 083 vs s9 红线
- 基准唯一 = 083/v83，**禁引 095 线**（含 `docs/features/port-095/**`、`Net/Packets/CashShopPackets.h:182` CubeRerollPacket 注释）。
- 协议值 **s9 优先**：`229 = RecvOpcode.CASHSHOP_OPERATION = 0xE5`（`RecvOpcode.java:199`）、`228 = CHECK_CASH = 0xE4`、`230 = COUPON_CODE = 0xE6`、`256 = USE_MAPLELIFE = 0x100`。
- 校准只做对照与决策点，**不改源码、不改协议、不提交**。

### 2.4 冲突取值表
| 冲突 | Angel 083 | 当前端 s9 | 取值 | 依据 |
| --- | --- | --- | --- | --- |
| 商城主 opcode | 229 | 229 | ✅ 一致 | 双端重合 |
| 优惠券 | 230 | 230 | ✅ 一致 | 双端重合 |
| 现金道具 | 79 | 79 | ✅ 一致 | `USE_CASH_ITEM` |
| 入口/查询 | 21/38/68/123 | 40/79/228 | ⚠️ s9 | 子功能并入 UI 逻辑 |
| 现金交易房 | 123 | `PLAYER_INTERACTION` | ⚠️ 分列 | 撞名，c2s↔s2c 分列纪律 |
| 内层 case | 48 | 19 mode | ⚠️ 非一一对应 | 24 未名 `sub_47xxxx` |

## §3 七维 98 条

> 每行格式：`# | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策`。

### A · 功能与窗口结构（14）
| # | 校准点 | Angel 083 一手 | 当前端 source | 判定 | 处置 |
| --- | --- | --- | --- | --- | --- |
| A1 | 商城入口 opcode | c2s 21/38/68/123 族 | `OutPacket.h:70` `ENTER_CASHSHOP=40` | ⚠️ | s9 优先，D1 |
| A2 | 窗口类型 | `CCashShop` 单体大窗 | `UICashShop : UIElement` `TYPE=CASHSHOP` | ✅ | 销案 |
| A3 | 独立 state | `CCashShop::OnCreate` | `UIStateCashShop` `emplace<UICashShop>()` | ✅ | 销案 |
| A4 | 模态/开关 | 模态窗 | `FOCUSED=true` / `TOGGLED=false` | ✅ | 销案 |
| A5 | 子窗数量 | 7 子窗复合 | 单 `UICashShop` + 20 `Buttons`，未拆 7 子窗 | ⚠️ | D2 |
| A6 | 生命周期方法 | 66 方法 | `draw/update/send_cursor/pre_add/remove/get/get_front/remove_cursor/emplace` | ⚠️ | D3 |
| A7 | 进商城 transition | `OnCreate` / `OnSetCashShop` | `SetCashShopHandler::transition()` → `Stage::load(-1,0)` + `UI::change_state(CASHSHOP)` | ✅ | 销案 |
| A8 | 退出按钮 | `BtExit` 类 | `Buttons::BtExit` | ✅ | 销案 |
| A9 | 充值 | `ChargeNX` 族 | `BtChargeNX` / `BtChargeRefresh` + `charge_charset` | ✅ | 销案 |
| A10 | 心愿 | 心愿数据族 | `BtWish` + `cash_wishlist[10]` + `CashShopWishPacket(0x05)` | ✅ | 销案 |
| A11 | 背包 / 仓库 | `Inventory`(9) / `Locker`(8) 子窗 | `BtInventory` + `cash_inventory` / `cash_gifts` | ⚠️ | D2 |
| A12 | 试衣 | `CCSWnd_Char`(18 控件) | `BtBuyAvatar/BtSaveAvatar/BtTakeoffAvatar/BtDefaultAvatar` + `preview_sprites[3]` | ✅ | 销案 |
| A13 | 列表容量 | `CCSWnd_List`(18) | `MAX_ITEMS = 7u*2u+1u = 15` | ⚠️ | D4 |
| A14 | 分页 | `BtNext/BtPrev` 类 | `BtNext` / `BtPrev` + `list_slider` | ✅ | 销案 |

> A 维小计：✅9 / ⚠️5。

### B · 类 / 对象结构（14）
| # | 校准点 | Angel 083 一手 | 当前端 source | 判定 | 处置 |
| --- | --- | --- | --- | --- | --- |
| B1 | 主类方法面 | `CCashShop` 66 方法 | 单 `UICashShop` | ⚠️ | D3 |
| B2 | 商品结构 | Commodity 族 | `struct CashCommodity{int32 sn,itemid,price; int16 count; string name;}` | ✅ | 销案 |
| B3 | 现金道具条目 | Locker 族 | `struct CashInvEntry{int64 cash_id; int32 itemid,sn; int16 qty;}` | ✅ | 销案 |
| B4 | 礼物条目 | Gift 族 | `struct CashGiftEntry{int64 cash_id; int32 itemid; string from;}` | ✅ | 销案 |
| B5 | 三币种 | 现金查询族 | `cash_nx_credit` / `cash_maple_point` / `cash_nx_prepaid` | ✅ | 销案 |
| B6 | 余额就绪标志 | — | `cash_balances_ready` | ✅ | 新增语义 |
| B7 | 心愿定长 | 10 心愿 | `std::array<int32_t,10> cash_wishlist` | ✅ | 销案 |
| B8 | 仓库槽 | Locker | `cash_storage_slots` | ✅ | 销案 |
| B9 | 角色槽 | Char | `cash_char_slots` | ✅ | 销案 |
| B10 | 商品容器 | List | `static cashshop_commodities`（`SetCashShopHandler` 填充） | ✅ | 销案 |
| B11 | SP 表特判 | 角色信息族 | `CashShopParser::hasSPTable` `EVAN=2001, 2200..2218` | ✅ | 销案 |
| B12 | 角色信息解析 | `OnSetCashShop` 角色数据 | `parseCharacterInfo` 复用 `CharacterParser` 14 项 | ✅ | 销案 |
| B13 | Evan 剩余 SP | SP 表 | `parseRemainingSkillInfo`（count × 2 byte） | ✅ | 销案 |
| B14 | 杂项 skip | 时间戳族 | `recv.skip(4)` gachaexp / timestamp | ✅ | 销案 |

> B 维小计：✅13 / ⚠️1。

### C · 协议 opcode（14）
| # | 校准点 | Angel 083 一手 | 当前端 source | 判定 | 处置 |
| --- | --- | --- | --- | --- | --- |
| C1 | 商城主 c2s | 229 | `OutPacket.h:270` `CASHSHOP_OPERATION=229` | ✅ | 销案 |
| C2 | 赠礼 `Encode1(4)` | 229 子功能 | `CashShopGiftPacket(0x04 + birthday + sn + name + msg)` | ✅ | 销案 |
| C3 | 心愿 `Encode1(5)` | 229 子功能 | `CashShopWishPacket(0x05 + 10×int SN)` | ✅ | 销案 |
| C4 | 购买 | 229 子功能 | `CashShopBuyPacket(0x03 + byte0 + int useNX + int sn)` | ✅ | 销案 |
| C5 | 取出 0x0D | 229 子功能 | `CashShopTakePacket(0x0D + int cashId)` | ✅ | 销案 |
| C6 | 存入 0x0E | 229 子功能 | `CashShopPutPacket(0x0E + cashId + skip(4) + invType)` | ✅ | 销案 |
| C7 | 礼包 0x1E | 229 子功能 | `CashShopPackagePacket(0x1E + byte0 + useNX + sn)` | ✅ | 销案 |
| C8 | 背包扩容 0x06 | 229 子功能 | `CashShopInvSlotPacket(mode0)` / `CashShopInvSlotSnPacket(mode1)` | ✅ | 销案 |
| C9 | 仓库扩容 0x07 | 229 子功能 | `CashShopStorageSlotPacket` | ✅ | 销案 |
| C10 | 角色栏 0x08 | 229 子功能 | `CashShopCharSlotPacket` | ✅ | 销案 |
| C11 | 查询现金 228 | c2s 族 | `CheckCashPacket(CHECK_CASH=228)` | ✅ | 销案 |
| C12 | 优惠券 230 | c2s 230 | `CouponCodePacket(COUPON_CODE=230)` | ✅ | 销案 |
| C13 | 现金道具 79 | c2s 族 | `CashItemPackets.h` 18 packet（`USE_CASH_ITEM=79`） | ✅ | 销案 |
| C14 | 24 未名 case | 48 case 内 24 `sub_47xxxx` | `CashShopOperationHandler` 约 19 mode | ⏳ | 见 r05 §4，P3 |

> C 维小计：✅13 / ⏳1。

### D · 数据与 handler（14）
| # | 校准点 | Angel 083 一手 | 当前端 source | 判定 | 处置 |
| --- | --- | --- | --- | --- | --- |
| D1 | 外层 11 case | `323..333` 无 330 | `CashShopHandlers.h` 7 handler 覆盖 324/325/328/329/331/333 | ⚠️ | 缺 323/326/327/332，D5 |
| D2 | 内层 48 case | 31 正 + 17 负 | `CashShopOperationHandler` 19 mode | ⚠️ | 非一一对应，D6 |
| D3 | 负值溢出 | `Decode1` signed char | 当前端 `write_byte` 正值枚举 | ✅ | 已规避 |
| D4 | 324 三 int | 现金查询 | `QueryCashResultHandler` 3×`read_int` + `cash_balances_ready=true` | ✅ | 销案 |
| D5 | 325 回包 | `OnCashItemResult` | `CashShopOperationHandler` | ✅ | 销案 |
| D6 | mode 0x4B/0x4D/0x4F | 内层 case 族 | 同文件分发 | ✅ | 销案 |
| D7 | mode 0x55..0xA0 | 内层 case 族 | `0x55/0x57/0x5C/0x5E/0x60/0x62/0x64/0x68/0x6A/0x6C/0x85/0x87/0x89/0x8D/0x9E/0xA0` | ⚠️ | 与 48 case 非一一对应，D6 |
| D8 | 现金道具复用 | 内层族 | `static skip_cash_item_info(InPacket&)` | ✅ | 销案 |
| D9 | Gachapon 333 | 0xE4 fail / 0xE5 success | `CashItemGachaponResultHandler` | ✅ | 销案 |
| D10 | 商品数容错 | `OnSetCashShop` | `commodity_count>0 && <4000` | ✅ | 销案 |
| D11 | 44 字节/件 | Commodity 结构 | `skip(count*44)` | ✅ | 销案 |
| D12 | 名字变更 328/329 | 外层 case | `CashShopCheckNameChangeHandler` / `CashShopNameChangePossibleHandler` | ✅ | 销案 |
| D13 | 转服 331 | 外层 case | `CashShopTransferWorldPossibleHandler` | ✅ | 销案 |
| D14 | 123 撞名 | 现金交易房 c2s | `PLAYER_INTERACTION` 同值异义 | ⚠️ | c2s↔s2c 分列，D7 |

> D 维小计：✅10 / ⚠️4。

### E · 排版与控件坐标（16）
> Angel 控件 id / 坐标逐字见 `notes/r01` §3 与 `evidence/angel-cashshop-facts.md`；当前端 UI 布局在 `UICashShop.cpp` / `.nx`，未经 orange-wz 一手解析，本维以**结构级对照**为主，坐标精度整体标 `⏳`。

| # | 校准点 | Angel 083 一手 | 当前端 source | 判定 | 处置 |
| --- | --- | --- | --- | --- | --- |
| E1 | `CCSWnd_Char` 18 控件 | r01 §3 | 试衣按钮组 + `preview_sprites[3]` | ⚠️ | 结构对应、坐标待核 |
| E2 | `CCSWnd_List` 18 控件 | r01 §3 | `items` / `item_name,price,discount,percent[MAX_ITEMS]` / `list_slider` | ⚠️ | 同上 |
| E3 | `CCSWnd_Inventory` 9 | r01 §3 | `BtInventory` | ⚠️ | 同上 |
| E4 | `CCSWnd_Locker` 8 | r01 §3 | `cash_inventory` / `cash_gifts` | ⚠️ | 同上 |
| E5 | `CCSWnd_Tab` 7 | r01 §3 | `menu_tabs[9]` | ⚠️ | 7 vs 9 待核 |
| E6 | `CCSWnd_Best` 5 | r01 §3 | `Item::Label`（HOT/NEW/…） | ⚠️ | 同上 |
| E7 | `CCSWnd_Status` 4 | r01 §3 | `charge_charset` / 三币种显示 | ⚠️ | 同上 |
| E8 | 预览位 3 | r01 §3 | `BtPreview1/2/3` | ✅ | 销案 |
| E9 | 标签 22 种 | `Item::Label` 族 | 同名 22 枚举（含 `SPEICAL` 拼写） | ✅ | 枚举一致 |
| E10 | MVP 7 级 | `mvp` 族 | `mvp_sprites[7]` / `mvp_gauge` | ⚠️ | 帧内容待 IMG |
| E11 | 菜单 9 tab | `CCSWnd_Tab` | `menu_tabs[9]` | ⚠️ | 与 E5 同源 |
| E12 | 主窗 20 按钮 | 7 子窗按钮族 | `Buttons` 20 枚举 | ⚠️ | 未拆子窗 |
| E13 | 列表容量 15 | `CCSWnd_List` | `MAX_ITEMS=15` | ⚠️ | 与 A13 同源 |
| E14 | 折扣/百分比 | 商品族 | `discount` / `percent[MAX_ITEMS]` | ✅ | 销案 |
| E15 | slider | `CCSWnd_List` | `list_slider` | ✅ | 销案 |
| E16 | 窗口九宫格 / 绝对坐标 | r01 §3 坐标 | `.nx` + 截图库（体感输入） | ⏳ | P2，见 r03 |

> E 维小计：✅4 / ⚠️11 / ⏳1。

### F · 样式与资源（12）· 整维 ⏳（orange-wz 阻塞）
| # | 校准点 | 处置 |
| --- | --- | --- |
| F1 | 3 个 preview slot 外观帧 | ⏳ P2 |
| F2 | `ACTION/BOMB_SALE/BONUS/EVENT/HOT` 标签位图 | ⏳ P2 |
| F3 | `LIMITED/LIMITED_BRONZE/GOLD/SILVER` 位图 | ⏳ P2 |
| F4 | `LUNA_CRYSTAL/MASTER/MUST/NEW` 位图 | ⏳ P2 |
| F5 | `SALE/SPEICAL/SPECIAL_PRICE` 位图 | ⏳ P2 |
| F6 | `TIME/TODAY/WEEKLY/WONDER_BERRY/WORLD_SALE/NONE` 位图 | ⏳ P2 |
| F7 | `mvp_sprites[7]` 分级贴图 | ⏳ P2 |
| F8 | `mvp_gauge` 贴图 | ⏳ P2 |
| F9 | `menu_tabs[9]` 选中/未选中态 | ⏳ P2 |
| F10 | 20 按钮常态/hover/pressed | ⏳ P2 |
| F11 | `CashShop.img` / `Commodity.img` 节点树 | ⏳ P2 |
| F12 | BMS / Sound 资源 | ⏳ P2 |

> F 维小计：⏳12。降级清单与替代链见 `notes/r03-img-blocked-fallback.md`。

### G · 交互与事件（14）
| # | 校准点 | Angel 083 一手 | 当前端 source | 判定 | 处置 |
| --- | --- | --- | --- | --- | --- |
| G1 | 单击购买 | `CCSWnd_List` 事件 | `BtBuy` + `CashShopBuyPacket` | ✅ | 销案 |
| G2 | 赠礼表单 | `Encode1(4)` | `CashShopGiftPacket(name/msg)` | ✅ | 销案 |
| G3 | 心愿添加 | `Encode1(5)` | `BtWish` + `CashShopWishPacket` | ✅ | 销案 |
| G4 | 优惠券输入 | c2s 230 | `BtCoupon` + `CouponCodePacket` | ✅ | 销案 |
| G5 | 取出/存入 | 0x0D/0x0E | `CashShopTakePacket` / `CashShopPutPacket` | ✅ | 销案 |
| G6 | 背包/仓库扩容 | 0x06/0x07/0x08 | 三个 `*SlotPacket` | ✅ | 销案 |
| G7 | 翻页 | `BtNext/BtPrev` | `BtNext` / `BtPrev` | ✅ | 销案 |
| G8 | slider 拖动 | `CCSWnd_List` | `list_slider` | ⚠️ | 事件体待 UI 实机 |
| G9 | 试衣保存/脱下/默认 | `CCSWnd_Char` | `BtSaveAvatar/BtTakeoffAvatar/BtDefaultAvatar` | ✅ | 销案 |
| G10 | 预览 1/2/3 切换 | `CCSWnd_Char` | `BtPreview1/2/3` | ✅ | 销案 |
| G11 | 详情包 | 商品族 | `BtDetailPackage` / `CashShopPackagePacket` | ✅ | 销案 |
| G12 | 非等级项 | 商品族 | `BtNonGrade` | ⚠️ | 语义待 IMG |
| G13 | 退出/充值跳转 | `BtExit` / `ChargeNX` | `BtExit` / `BtChargeNX` / `BtChargeRefresh` | ✅ | 销案 |
| G14 | 帮助/里程 | 主窗族 | `BtHelp` / `BtMileage` | ✅ | 销案 |

> G 维小计：✅11 / ⚠️3。

**98 条分布**：✅60 / ⚠️24 / ⏳14 / ❌0 / ⊘0。无「缺失」与「不适用」——本面双端功能面覆盖完整，未闭环项全部收敛到 IMG 阻塞与符号缺失两类可命名遗留。

## §4 十条硬发现
1. 938/938 EA 字节级一致 → Angel 与 BeiDou 同一份 v83 二进制，差异只在 IDA 符号覆盖。
2. `0x4b7404` 1-EA 缺失是 `get_func_attr` 未定义，8 字节仍相同 → 覆盖差异非布局差异。
3. `CCashShop` 66 方法 + 7 子窗复合，是当前端单 `UICashShop` 未拆分的历史全貌。
4. 当前端商城 = **独立真窗**（`UIStateCashShop` 独立 state），形态区别于 pet/activity 面合并壳。
5. 两级分发：外层 11 case（无 330）+ 内层 48 case，严禁混列。
6. 内层 17 个负 case = `Decode1` unsigned byte → signed char 溢出，实为 133..162 区间。
7. 48 case 中 24 落未命名 `sub_47xxxx`，本轮不臆测。
8. c2s `229/230` 双端重合；`21/38/68/123` 并入 UI 逻辑，s9 优先。
9. `commodity_count>0 && <4000` + `skip(count*44)` 与 44 字节/件结构对齐，是容错读序一手证据。
10. `CashShopParser::hasSPTable` 对 `EVAN=2001, 2200..2218` 特判，复用 `CharacterParser` 14 项解析。

## §5 四态
| 态 | 本面结论 | 详见 |
| --- | --- | --- |
| 销案 | 入口 / handler 读序 / mode 分发 / 229 包族 / 三币种 / 心愿 / SP 表等既有疑虑全部定论 | `notes/r05` §2 |
| 推翻 | **无**（独立真窗与合并壳分属不同面，标注适用面而非推翻） | `notes/r05` §0.3 |
| 新增 | 独立真窗形态、两级 48-case 分列、1-EA 字节级归因、44 字节/件 | §4 |
| 遗留 | IMG 资源层（F 维 12 条）+ 24 未名 `sub_47xxxx` | r03 / r05 §4 |

## §6 决策点 D1–D14（**需用户裁决，不得盲改**）
| # | 决策点 | 推荐默认 | 依据 | 级别 |
| --- | --- | --- | --- | --- |
| D1 | 入口 opcode 21/38/68/123 vs 40 | 保持 s9 | 断联机风险 | P1 |
| D2 | 是否拆 7 子窗 | 不拆，保持单窗 | 当前端已可运行 | P1 |
| D3 | 是否补齐 66 方法面 | 仅文档标注，不补 | 最小改动 | P1 |
| D4 | `MAX_ITEMS=15` vs `CCSWnd_List` 18 控件 | 保持 15 | s9 一手 | P1 |
| D5 | 外层缺 323/326/327/332 handler | 记录遗留，不新增 | 无 s9 回包证据 | P1 |
| D6 | 48 case ↔ 19 mode 映射 | 不强行对齐 | 24 未名 | P1 |
| D7 | 123 现金交易房撞名 | 分列，不合并 | 协议纪律 | P1 |
| D8 | `CubeRerollPacket:182` 095 注释 | 仅清理注释措辞（可选） | 083 基准 | P0 |
| D9 | 24 未名 `sub_47xxxx` | 待 IDA 补符号 / 动态跟包 | 证据不足 | P3 |
| D10 | F 维 12 项 IMG 资源 | 待 orange-wz 恢复 | 门禁阻塞 | P2 |
| D11 | 7 vs 9 tab（E5/E11） | 保持 9，标注差异 | s9 一手 | P1 |
| D12 | `SPEICAL` 拼写 | 保持原样 | 与 Angel 枚举一致 | P0 |
| D13 | `256 USE_MAPLELIFE` 跨面归属 | 归跨面，本面只引用 | 避免重复计数 | P0 |
| D14 | 是否触发 build_smoke.bat | 等 IMG 恢复后一并 | 无源码改动 | P2 |

## §7 一次性 Plan
| 阶段 | 内容 | 状态 |
| --- | --- | --- |
| **P0** | 纯文档：本 README + `notes/r01` + `notes/r03` + `notes/r05` 归档；`python tools/gen_opcode_matrix.py --check` 门禁；六面既有 README 引用一致性核对 | ✅ 本轮交付 |
| **P1** | 需用户裁决 D1/D2/D3/D4/D5/D6/D7/D11 后方可动源码 | ⏸ 等裁决 |
| **P2** | orange-wz 恢复 → 回填 F 维 12 项 + E16 坐标 + `build_smoke.bat` | ⏸ 阻塞 |
| **P3** | 24 未名 `sub_47xxxx` 补符号；后续功能面（技能 / NPC / 地图 / 任务 / 掉落）续作 | ⏸ 续作 |

## §8 限制与未知
- orange-wz(IMG) 阻塞 → F 维整维 `⏳`，不臆测贴图 / 坐标精度。
- IDA 端点事实不可本地复跑，凭 `evidence/*.json` + 本地文档复核；opcode 门禁可本地跑。
- 24 未名 `sub_47xxxx` 无符号，语义待补；外层 323/326/327/332 无对应 s9 handler。
- `docs/features/port-095/**` 与 `CashShopPackets.h:182` 注释属 095 / 注释污染，本面禁引。
- 本面为**对照 / 决策文档**，不含实现、不含提交、不含源码改动。

## §9 证据索引与复现
| 类别 | 路径 |
| --- | --- |
| Angel 基线 | `notes/r01-angel-native-cashshop-facts.md` |
| IMG 降级 | `notes/r03-img-blocked-fallback.md` |
| 四态 / 48-case | `notes/r05-cashshop-four-state-audit.md` |
| evidence 6 件 | `evidence/angel-cashshop-*.json`、`evidence/angel-cashshop-facts.md`、`evidence/angel-beidou-cashshop-ea-*.json` |
| 当前端源码 | `IO/UITypes/UICashShop.h`、`IO/UIStateCashShop.h/.cpp`、`Net/Packets/CashShopPackets.h`、`Net/Packets/CashItemPackets.h`、`Net/Handlers/CashShopHandlers.h/.cpp`、`Net/Handlers/Helpers/CashShopParser.h/.cpp`、`Net/OutPacket.h` |
| 采集脚本 | `tools/ida_angel_cashshop_{ea,decompile,normalize,crosscheck,facts}.py` |

**复现命令**
```bash
python tools/gen_opcode_matrix.py --check     # 期望 GATE OK: opcode matrix matches both sources ...
# IDA 端点（服务在跑时）：
python tools/ida_angel_cashshop_ea.py
python tools/ida_angel_cashshop_decompile.py
python tools/ida_angel_cashshop_crosscheck.py
```

## §10 性质标注
校准 / 对照 / 决策点 / Plan 文档。**非实现提交**；原子提交须用户显式授权；全程只读，未改源码，未建提交。基准 = 083/v83 唯一，禁引 095。
