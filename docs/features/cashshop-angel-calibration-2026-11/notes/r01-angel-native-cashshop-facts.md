# r01 · Angel 083 商城现金交易面一手语义（观察层）

> 第七功能面（goal round 5，§7 P3 续作首面）。以 Angel(13339) 083/v83 带符号 IDB 为权威源，
> 对 `CCashShop`(66 方法全量) + 7 个 `CCSWnd_*` 子窗 + 试衣/心愿/赠送/优惠券/扭蛋/拍卖/角色售卖族
> 逐字抽取 Hex-Rays 语义。社交经济面仅精选过 `CCashShop` 10 方法，本面全量深化。
> 结论区分【观察 / 推断 / 未知】；全程只读，未改任何产品源码。

## §0 七条硬结论

1. **【观察】七面 EA 字节级一致钉死「同一份 v83 二进制、不同符号状态」**：登录 327/328、HUD 547/548、核心窗口 439/441、社交经济 752/752、宠物召唤龙 968/968、活动小游戏 844/844（3-EA 审计后）、本面 **938/938**（1-EA 审计后）。crosscheck 原始 `beidou_present=937 / beidou_missing=1 / missing_eas=["0x4b7404"]`，正解见 §1.1：`0x4b7404 = ?OnKey@CCSWnd_List@@UAEXII@Z`，Angel 是函数头 `(0x4b7404..0x4b7417)`，BeiDou `get_func_attr=0xffffffffffffffff`（未定义函数）但 **8 字节逐字相同** `[255,116,36,8,131,193,252,255]` → 纯 IDA 自动分析覆盖差异，非二进制布局差异。`tools/ida_angel_cashshop_crosscheck.py:107` 阈值 `same = present >= len(items) - 2` 在本面 1 Missing 时仍判「same binary layout」，正确；但 `937/938` 措辞须以 `evidence/angel-beidou-cashshop-ea-bytelevel-audit.json` 更正为 938/938 by bytes。
2. **【观察】Angel 只补符号语义层，不补功能/版本**：3.1 万个 Angel 具名函数与 BeiDou 无符号 `sub_*` 一一对应，符号 3122 修饰名/361 类/2612 方法 vs 50/10/0；本面 938 具名函数在 BeiDou 侧 930 `sub_*` + 7 具名 + 1 未定义函数。
3. **【观察】商城是「单体大窗 + 7 个子窗」的复合窗**：`CCashShop` 66 方法总控（`Init/LoadData/OnPacket/OnCashItemResult/ProcessBuy`），`CCSWnd_Char`(18) / `CCSWnd_List`(18) / `CCSWnd_Inventory`(9) / `CCSWnd_Locker`(8) / `CCSWnd_Tab`(7) / `CCSWnd_Best`(5) / `CCSWnd_Status`(4) 是 `OnCreate` 里逐字 `CCtrlTab/CCtrlSelector/CCtrlScrollBar/CCtrlButton/CCtrlEdit/CCtrlCheckBox` 建出来的子控件组（`SP_12xx_UI_CASHSHOPIMG_*` 资源名逐字见 §6）。
4. **【观察】两级包分发**：`CCashShop::OnPacket(int **this,int a2,...)` 外层 `switch(a2)` 11 个 case = **323/324/325/326/327/328/329/331/332/333**（无 330）；`OnCashItemResult`（case 325 命中）内层 `switch(CInPacket::Decode1(a2))` **48 个 case** = `71 75 76 77 78 79 80 85 86 87 88 89 91 92 94..110 -123..-114 -111 -110 -102 -101 -98 -96 -95`（含负值 case = byte 溢出成负数）。这是本面最硬的协议骨架。
5. **【观察】c2s 出站构造点全集仅 7 个 opcode**（259 函数内 `COutPacket::COutPacket(a1: N)` 全扫描，逐字见 §5）：`21` 重名检查 / `38` 转区 / `68` 管理商店心愿登记 / `123` 现金交易房 / `229` CASH_ITEM（购买/心愿/赠送共用）/ `230` 优惠券 / `256` 角色售卖重名检查。均为「单 opcode + 多子命令」结构（如 229 的 `Encode1(4/5/...)`）。
6. **【观察】083 原生商城功能成族且极深**：限量商品（`IsLimitGoodsRemain/ChangeLimitGoodsState/UpdateStock`）、试衣试穿（`CUserPreview/CVecCtrlUserPreview/CUIAvatarVAC/OnWear/OnTakeOff/SetPet/SetEmotion/SetActiveEffectItem`）、心愿单（`OnSetWish/OnRemoveWish/FindWishList/OnCashItemResLoadWishDone`，上限 10 `SP_528`）、赠送（`GiftWishItem/OnGiftPackage/SendGiftsPacket/OnGiftMateInfoResult`，`SP_3991..3998` 全套拒绝文案）、优惠券（`OnStatusCoupon/OnCashItemResUseCouponDone/Failed/SP_542..547`）、储物箱 Locker（`OnCashItemResLoadLockerDone/Failed`）、扭蛋（`OnCashItemGachaponResult`）、角色转区（`OnBuyTransferWorldItem/SendTransferFieldPacket/SP_4002..4008`）、角色售卖（`CUICharacterSaleDlg/CRegisterSaleEntryDlg/CRegisterAuctionEntryDlg`）。
7. **【推断】「校准 ≠ 改回原生」七面一致**：原生商城是 66 方法单窗 + 子控件组 + 现金交易闭环；当前端按 s9 联机协议重写为独立 UI/壳/DIY 合并（详见 r02/r04）。s9 适配冲突一律标注「改回原生会断联机，须交用户裁决，不得盲改」。IMG 层 orange-wz(IMG) MCP 未接入 Host → 七面一致记阻塞，替代链 = 仓库 `.nx` 节点引用 + `nx_inspect.py` dump + 正确端截图（详见 r03）。

## §1 取证基线（三联门禁）

| 层 | 状态 | 证据 |
|---|---|---|
| REA / IDA（Angel 13339） | ✅ | `E:\MXD\反编译IDA\v83\`Angel.i64`，module `` `Angel.exe ``，imagebase 0x400000，strings_cache 1281，`auto_analysis_ready/hexrays_ready=true`，uptime 83045s |
| IDA（BeiDou 13337） | ✅ | `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64`，module `BeiDou.exe`，strings_cache 3889；用于 EA 交叉 |
| orange-wz(IMG) MCP | ⚠️ 阻塞 | 本 DSH Host `mcp_connector_status` 仅 `json-photoshop`；`~/.cursor/mcp.json:3-6` 声明但 Host 未注册；`img-evidence-audit.md:7` `Index 16 out of bounds` 解析缺陷 → 替代链取证 |

**5 脚本 / 6 evidence**（`tools/ida_angel_cashshop_{ea,decompile,normalize,crosscheck,facts}.py`，由 `tools/_gen_cashshop.py` 从第六面模板生成，py_compile 全 SYNTAX OK）：
- `evidence/angel-cashshop-ea.json` = **938 具名函数 / 59 类**（per_class 全表见下）
- `evidence/angel-cashshop-decompile.json` = **259/259 OK**；`angel-cashshop-decompile-full.json`（2 个 `/output/` 补全 `trunc_fixed 2 empty 0`）；`angel-cashshop-truncated.json`
- `evidence/angel-cashshop-facts.md` = **3939 行 / 203501 B**，SP_ 全集 **266 个**（`UI_UIWINDOWIMG` 5 个），类标题 44 个 + §3 坐标段 `:4260` 起
- `evidence/angel-beidou-cashshop-ea-crosscheck.json` = 937/938 + `missing_eas=["0x4b7404"]`
- `evidence/angel-beidou-cashshop-ea-bytelevel-audit.json` = 1-EA 字节级根因 + 七面一致性表

**EA per_class 全表（938/59）**：`CWvsContext 197, CUserLocal 120, CField...（本面未纳入）, CItemInfo 47, CMapLoadable 36, CWnd 32, CConfig 30, CAvatar 29, CWndMan 27, CUserRemote 21, CCSWnd_Char 18, CCSWnd_List 18, CScriptMan 17, CTradingRoomDlg 17, CClientSocket 15, CAdminShopDlg 14, CCashShop 66, CMiniRoomBaseDlg 15, CChatBalloon 11, CPersonalShopDlg 10, CCSWnd_Inventory 9, CShopDlg 9, CInPacket 9, COutPacket 9, CCSWnd_Locker 8, CAvatarMegaphone 8, CCSWnd_Tab 7, CUIRecommendWorld 7, CUIAdminShopWishList 7, CUserPreview 7, CVecCtrlUserPreview 7, CCSWnd_Best 5, CUIAvatar 5, CUIAdminShopWishListSearchResult 5, CUIAvatarMegaphone 6, CUIAvatarVAC 13, CUIAvatarMegaphone 6, CUICharacterSaleDlg 4, CConfirmPurchaseDlg 4, CCSWnd_Status 4, CCashTradingRoomDlg 5, CUIQuestDelivery 1, CUICashItemGachapon 3, CParcelDlg 1, CUIEventAlarm 1, UCS_COMMODITY 2, CWvsCommodityEx 1, AvatarLook 3, CTrunkDlg 22, CUIShopScanResult 2, CUIUserInfoWishList 2, CRegisterAuctionEntryDlg 2, CRegisterWishEntrySearchDlg 2, CCouponUseSelectDlg 2, CRegisterWishEntryDlg 1, CRegisterSaleEntryDlg 1, PAVCVecCtrlUserPreview 1, VCAvatar 1, VCSoldItemDlg 1, StringPool 6, CStage 5`（注：CConfig/CUserLocal/CWvsContext 等大类是 EA 宽集附带，decompile 只精选商城类）。

### §1.1 ★ 1 个 missing EA 字节级根因（本面独有技术点）

| EA | Angel 符号 | Angel 函数范围 | Angel bytes8 | BeiDou get_func_attr | BeiDou bytes8 | 判定 |
|---|---|---|---|---|---|---|
| `0x4b7404` | `?OnKey@CCSWnd_List@@UAEXII@Z` → `public: virtual void CCSWnd_List::OnKey(unsigned int,unsigned int)` | `0x4b7404..0x4b7417` | `[255,116,36,8,131,193,252,255]` | `0xffffffffffffffff`（未定义函数） | `[255,116,36,8,131,193,252,255]` | 字节逐字相同 → IDA 分析覆盖差异，非布局差异 |

双端 `py_eval` 全部逻辑包进 `def _scan()` + `new_locals=True` 规避 `api_python.py:113-204` exec/eval 双作用域陷阱（`NameError: name 'json' is not defined`）。与第六面 3-EA 案例同机制。

## §2 CCashShop 66 方法逐字（分组）

- **生命周期 / 数据**：`Init` / `LoadData`（11392 B，`CInPacket::Decode1` 标志 + `DecodeStr` 商品串 + `ZXString<char>::Mid/GetBuffer/_Cat` 拼 `FileName`，`SP_1385_ETC_CATEGORYIMG` 类别图）/ `Close` / `GoTo` / `RestoreFocus` / `RestoreFocus`（`SetUserPreviewControl` 逐字 `return CCashShop::RestoreFocus(this);`）/ `ShowMessageBox` / `ShowEntrustedShop` / `ShowPersonalShop` / `ShowBullet` / `BlowWeather` / `SetActiveEffectItem` / `SetEmotion` / `SetPet`。
- **收包分发**：`OnPacket`（11 case，§0.4）/ `OnCashItemResult`（48 case，§0.4）/ `OnQueryCashResult`（`3×Decode4` 三种现金余额 `v3/v4/v5<0` 校验 + `if(*(this+305)!=0 && FindWishList>=0) OnRemoveWish(*(this+305))` 自动清已购心愿）/ `OnCashItemGachaponResult` / `OnGiftMateInfoResult` / `OnCashItemResLoadLockerDone`（`Decode2` 循环 + `*(this+291)=Decode2` / `*(this+292)=Decode2`）/ `OnCashItemResLoadWishDone` / `OnCashItemResBuyFailed` / `OnCashItemResBuyPackageDone` / `OnCashItemResGiftCouponDone` / `OnCashItemResLoadGiftDone/Failed` / `OnCashItemResLoadWishFailed` / `OnCashItemResUseCouponDone/Failed` / `OnCashItemResLoadLockerFailed` / `OnCashItemResLimitGoodsCountChanged` / `OnStatusCharge` / `OnStatusCheck` / `OnStatusCoupon` / `OnStatusExit`。
- **购买 / 赠送 / 心愿**：`ProcessBuy`（1319 B，`a2/100` 商品 id 分派：`==11120`→`sub_46FFE7`、`/10000==910`→`OnBuyPackage`、`a3∈[80000000,90000000)`→`sub_46E5C5`、`v4==11128 && a2%10<=2`→`sub_470A5A`、`/1000==5430`→`sub_46C6F8`、`v5==555`→`sub_46C8E1`、`v5==911`→`sub_470653`、`a2==&off_5269A8`→`OnBuyTransferWorldItem`、else `sub_46DADD`）/ `OnBuyAvatar` / `OnBuyPackage` / `OnGiftPackage` / `GiftWishItem` / `OnSetWish`（`COutPacket(229)+Encode1(5)`，满 10 `SP_528_YOUR_WISH_LIST_IS_FULL`，重复 `SP_527_YOU_ALREADY_HAVE...`）/ `OnRemoveWish`（`COutPacket(229)+Encode1(5)+EncodeBuffer(v6,0x28)+SendPacket`）/ `FindWishList` / `SendGiftsPacket`（`COutPacket(229)+Encode1(4)+Encode4(*(this+326))`，`SP_561/562/563` 结果文案）/ `SendBuyAvatarPacket`（`SP_529/510/509` 购买结果）。
- **查询 / 校验 / 控件**：`GetCategorySubIndex` / `GetUsableCommodityIndexBySN` / `IsUsableItem` / `IsUsableItemGift` / `IsLimitGoodsRemain` / `ChangeLimitGoodsState` / `UpdateStock` / `IsControlingUserPreview` / `CheckTransferWorldPossible` / `NoticeFailReason`（16627 B，`SP_534..547` 全套失败原因：超时/现金不足/未满 14 岁赠送/赠送限额/同账号/名字错/性别限制/背包满/优惠券过期·已用·网吧专用）/ `OnChangedCategory/OnChangedCategorySub/OnChangedPage/OnChangedSortType` / `OnDefaultAvatar` / `OnKey` / `OnWear`（`CCSWnd_Char::OnWear(this:(this+1364),...)`）/ `OnTakeOff`（`sub_4AC6D6(a1:this+1364)`）/ `SendCheckDuplicateIDPacket`（`COutPacket(21)+EncodeStr+SendPacket`，`SP_516_YOU_CANNOT_USE_THIS_NAME`）/ `SendTransferFieldPacket`（`COutPacket(38)+SendPacket`）/ `SendBuyAvatarPacket` / `OnBuyTransferWorldItem`。

## §3 CCSWnd_* 7 子窗控件 id / 坐标（逐字 OnCreate）

- **`CCSWnd_Char`（18，试衣间）**：`CCtrlTab(a3:1004, a4:3, a5:141, a6:11, a7:115, a8:0)` + `SetTab(0)`；`SP_3129_BASE_TAB_ENABLE` / `SP_3130_BASE_TAB_DISABLE`；`CCtrlEdit(a3:1003, a4:50, a5:214, a6:182, a7:15)` + `SP_5527_ARIAL`；按钮 `SP_1206_CSCHAR_BTBUYAVATAR` / `SP_1207_BTDEFAULTAVATAR` / `SP_2902_BTTAKEOFFAVATAR` / `SP_3133/3134_BASE_PREVIEWONOFF_ON/OFF` / `SP_3131_UI_CASHSHOPPREVIEWIMG`；方法 `OnWear` / `CmpPet` / `EndChat` / `OnKeyRet` / `SetEmotion` / `ShowBullet` / `TransientLayer_Weather`。
- **`CCSWnd_Inventory`（9）**：`CCtrlTab(a3:1000, a4:4, a5:17, a6:28, a7:156, a8:0)` + `SP_3174/3175_BASE_TAB2_ENABLE/DISABLE`；`CCtrlScrollBar(a3:1001, a4:1, a5:0, a6:160, a7:54, a8:102)`；4 个 `CCtrlButton` `a3:1002/1003/1004` @ `(176,27)/(176,54)/(176,81)` + `SP_1210/1211/1212/1213/1215_CSINVENTORY_BTEX{EQUIP,CONSUME,INSTALL,ETC,TRUNK}`；`GetItemSlotRect` / `OnTabChanged`。
- **`CCSWnd_List`（18，商品列表）**：`CCtrlSelector(a3:1000, a4:4, a5:412, a6:406, a7:0)` + `SetSelectorAttr(-12949590,-1,-16777216)`；`SP_1217_CSLIST_BASE` / `SP_1218_CSLIST_ITEMICON` / `SP_1219/1220/1221_PICTUREPLATE_{HOWTO,HOWTOGIFT,NOITEM}` / `SP_1223/1224/1225/1226_CSLIST_BT{BUY,GIFT,RESERVE,REMOVE}` / `SP_2853_CSICON_D` / `SP_2879_CSDISCOUNT` / `SP_3119/3120_CSEFFECT_EFFECT_0/1` / `SP_3524_PICTUREPLATE_SHORTCUTHELP`。`OnKey @ 0x4b7404` 即本面 1-EA missing 根因。
- **`CCSWnd_Locker`（8）**：`CCtrlScrollBar(a3:1001, a4:1, a5:v11, a6:229, a7:29, a8:67)` + `SP_1253_UI_BASICIMG_ITEMNO`。
- **`CCSWnd_Tab`（7）**：`CCtrlSelector(a3:1000, a4:8, a5:59, a6:485, a7:0)` + `SP_1216_CSTAB_TAB_D`（8 大类页签）。
- **`CCSWnd_Status`（4，充值/查询/优惠券/退出）**：4 个 `CCtrlButton` `a3:1000/1001/1002` @ `(248,13)/(289,13)/(330,13)` + `SP_1227_BTCHARGE` / `SP_1228_BTCHECK` / `SP_1229_BTCOUPON` / `SP_1230_BTEXIT` + tooltip `SP_1280/1281/1282_STRING_TOOLTIPHELPIMG_SHOP_BUTTON_{CHARGE,CHECK,COUPON}`。
- **`CCSWnd_Best`（5）**：`ChangeCategory/Draw/GetBestRect/OnCreate/OnMouseButton`，`SP_3559/3560` 榜单占位串、`SP_3745/3746/3748` 等级门槛文案、`SP_3749` 账号注册 15 天限制。
- **辅助对话框**：`CConfirmPurchaseDlg::OnCreate` 多个 `CCtrlCheckBox` `a3:1000/1001` @ `(25,*(this+10)-95)/(25,*(this+10)-80)` + `SetChecked(*(this+33)==2/4)` + `SP_2697_CASHSHOPIMG_CSNOTICE_0`；`CUICashItemGachapon::OnCreate` `CCtrlButton(a3:2000,a4:68,a5:149)` + `SP_5066_CASHGACHAPON_BTOPEN` / `(a3:2001,a4:150,a5:8)` + `SP_5067_BTCLOSE`；`CUIItemMaker::OnCreate`（跨面复用）`30000/40000` ComboBox + `20000/20001` 按钮。

## §4 试衣 / 心愿 / 商品数据族

- **试衣**：`CUserPreview`（7：`Init/InitObjects/DoMeleeAttack/DoShootAttack/ShowADBoard/ShowFireCrack/Update` + `SP_2727_SAMPLE`）/ `CVecCtrlUserPreview`（7：`BeginUpdateActive/EndUpdateActive/WorkUpdateActive/WorkUpdateActiveLadderOrRope/IsAbleToClimbLadderOrRope/IsStun/CreateInstance`）/ `CUIAvatarVAC`（13：`MakeCharacter/MakeSelectedCharacterNameTag/MakeWorldIcon/DrawWorldIcon/GetScrollBarIdx/GetSelectedIdx/IsRequestValid/OnButtonClicked`）/ `CUIAvatar`(5) / `VCAvatar`(1) / `AvatarLook`(2：`Decode/Load`)。
- **心愿 / 管理商店**：`CUIAdminShopWishList`（7：`CanAddList/CloseAdminShopWishList/OnButtonClicked/OnKey/SearchItemName/SendRegisterPacket/ToggleAddOn`，`SendRegisterPacket → COutPacket(68)`）/ `CUIAdminShopWishListSearchResult`(5) / `CRegisterWishEntryDlg`(1) / `CRegisterWishEntrySearchDlg`(2) / `CUIUserInfoWishList`(2) / `CUIShopScanResult`(2) / `SP_3628__S__R_NWOULD_YOU_LIKE_TO_REGISTER`。
- **拍卖 / 角色售卖**：`CRegisterAuctionEntryDlg`(2) / `CRegisterSaleEntryDlg`(1) / `CUICharacterSaleDlg`(4：`ClearCurrentStep/OnButtonClicked/SendCheckDuplicateIDPacket/ShowWindow`，`SendCheckDuplicateIDPacket → COutPacket(256)`）/ `VCSoldItemDlg`(1)。
- **商品数据**：`UCS_COMMODITY`(2：`?1?$ZRef/ ?4?$ZRef`) / `CWvsCommodityEx::GetCommodityExByIndex`(1) / `CItemInfo`(47，宽集附带未全 decompile) / `SP_1233_BUYSHOPITEM`。
- **现金交易房**：`CCashTradingRoomDlg`（5：`OnButtonClicked/OnKey/OnLeave/PutMoney/Trade`，`PutMoney/Trade → COutPacket(123)`）/ `CParcelDlg::OnPacket` / `CUIQuestDelivery::OnChildNotify` / `CUIRecommendWorld`(7：`MoveWorld/SelectWorld/IsRequestValid`)。

## §5 ★ c2s 出站构造点全集（259 函数全扫描 `COutPacket::COutPacket(a1: N)`）

| opcode | dec | hex | 构造点（逐字） | 结构 |
|---|---|---|---|---|
| 21 | 21 | 0x15 | `CCashShop::SendCheckDuplicateIDPacket` | `EncodeStr(name) + SendPacket` |
| 38 | 38 | 0x26 | `CCashShop::SendTransferFieldPacket` | 空体 `+SendPacket` |
| 68 | 68 | 0x44 | `CUIAdminShopWishList::SendRegisterPacket` | 心愿登记 |
| 123 | 123 | 0x7B | `CCashTradingRoomDlg::PutMoney` / `::Trade` | 现金交易房（与 s9 PLAYER_INTERACTION 撞名，须分列） |
| **229** | 229 | 0xE5 | `CCashShop::OnSetWish` / `OnRemoveWish` / `GiftWishItem` / `OnBuyPackage` / `OnGiftMateInfoResult` / `OnGiftPackage` / `SendGiftsPacket` / `OnBuyAvatar` 等 | **主力 CASH_ITEM**：子命令 `Encode1(4)`=赠送 / `Encode1(5)`=心愿；`OnBuyPackage` 走 `/10000==910` 套餐 |
| 230 | 230 | 0xE6 | `CCashShop::OnStatusCoupon` | `COutPacket(230)+3×EncodeStr(券号)` |
| 256 | 256 | 0x100 | `CUICharacterSaleDlg::SendCheckDuplicateIDPacket` | 角色售卖重名 |

> 【未知】`OnSayImage/OnAskYesNo` 类脚本问答在本面未命中 opcode；`CCashShop::OnBuyAvatar/OnBuyPackage` 的最终发包是否复用 229 的其它子命令需结合 r02 当前端销案确认。

## §6 SP_ 266 全集关键子集（逐字，`angel-cashshop-facts.md` §1）

- **资源件表**：`SP_1301/1302_UI_CASHSHOPIMG_BASE_BACKGRND/1`、`SP_1385_ETC_CATEGORYIMG`、`SP_1249_SOUND_BGMUIIMG_SHOPBGM`、`SP_1232_DLGNOTICE`、`SP_1283_UI_UIWINDOWIMG_COUPON_BACKGRND`、`SP_1216_CSTAB_TAB_D`、`SP_2508_GUIDEWORDS_D`、`SP_3131_UI_CASHSHOPPREVIEWIMG`、`SP_2853_CSICON_D`、`SP_2879_CSDISCOUNT`、`SP_3119/3120_CSEFFECT_EFFECT_0/1`。
- **购买失败 / 限制**：`SP_509_THE_PURCHASE_HAS_FAILED`、`SP_510_YOU_HAVE_PURCHASED_THE_FOLLOWING_ITEMS...`、`SP_527/528` 心愿重复/已满、`SP_529_YOU_HAVE_SUCCESSFULLY_MADE_THE_PURCHASE`、`SP_534..547`（超时/现金不足/年龄/限额/同账号/名字/性别/背包满/券过期/券已用/网吧专用）、`SP_2505_ITEMS_ARE_NOT_AVAILABLE...AT_THIS_HOUR`、`SP_2506_THE_ITEM_IS_OUT_OF_STOCK`、`SP_2864_THE_OPTION_IS_NOT_AVAILABLE_FOR_THAT_PACKAGE`、`SP_2964_YOU_MAY_NOT_PURCHASE...ALREADY_HAVE_IT`、`SP_3441_PURCHASE_SUCCESSFUL...MAPLEPOINTS`。
- **赠送 / 转区 / 券**：`SP_3991_WOULD_YOU_LIKE_TO_GIFT...` + `SP_3992..3998`（赠送 7 类拒绝）、`SP_4002..4008`（转区 7 类拒绝：等级<20 / 同区 / 新区 / 不可转出 / 无空位）、`SP_3274/3275`（NX 券号说明 / 首购限制）、`SP_561/562/563`（赠送结果）、`SP_3745/3746/3748/3749`（等级/账号门槛）。
- **UIWindowIMG** 5 个：`SP_1283_COUPON_BACKGRND` 及 CashGachapon `SP_5066/5067` 等 → 本面 `UI_UIWINDOWIMG` 少（商城主资源走 `UI_CASHSHOPIMG_*` 独立 img，非 UIWindow.img）。

## §7 与 MEMORY.md 083 术语互证

- 基准固定 083/v83、禁引 095 线（`Maplestory95.i64` / port-095 / v95 Academy / BossParty）作依据——本面 259 函数逐字语义全部来自 13339 Angel v83，未混 095。
- 083 原生商城 `SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`（年龄验证 URL 串）在 CCashShop/CCSWnd_* 多处出现，属 GMS v83 商城残留文案，非功能缺失。
- `CCashShop::OnPacket` 外层 case 323–333 与 `Net/PacketSwitch.cpp` s2c 注册（r02 核对）方向须分列：Angel 这里是 **s2c 收包 opcode**（客户端收服务端），而 §5 的 `COutPacket(a1:N)` 是 **c2s 出站**，两者撞值须分表（沿用第五/六面 c2s↔s2c 分列纪律）。

## §8 七面一致性表

| 面 | Angel 具名 | BeiDou 同 EA | 备注 |
|---|---|---|---|
| 登录 | 327/328 | 全一致 | |
| 状态栏 HUD | 547/548 | 全一致 | |
| 核心窗口 | 439/441 | 全一致 | |
| 社交经济 | 752/752 | 全一致 | 含 CCashShop 精选 10 方法 |
| 宠物召唤龙 | 968/968 | 全一致 | |
| 活动小游戏 | 844/844 by bytes | 3-EA 审计后 | `layout differs` 阈值误判 |
| **商城现金交易** | **938/938 by bytes** | **1-EA 审计后** | `0x4b7404` IDA 覆盖差异 |

## §9 逐字伪代码索引

`evidence/angel-cashshop-decompile-full.json`（259 键，逐字 Hex-Rays）→ `angel-cashshop-facts.md`（§2 逐类 44 标题 / §3 坐标段 `:4260`）。关键函数逐字：
- `CCashShop::OnPacket @ case 323..333`（§0.4 全文）
- `CCashShop::OnCashItemResult`（48 case 全量表 §0.4）
- `CCashShop::LoadData`（`Decode1/DecodeStr/Mid/GetBuffer/_Cat` 商品目录解析）
- `CCashShop::ProcessBuy`（`a2/100/10000/1000` 商品 id 分派树全文）
- `CCashShop::NoticeFailReason`（`SP_534..547` 全套）
- `CCSWnd_Char/Inventory/List/Locker/Tab/Status::OnCreate`（§3 控件 id 表）
- `CUICashItemGachapon::OnCreate`（2000/2001 按钮）
- `CConfirmPurchaseDlg::OnCreate`（CheckBox 1000/1001）

复现命令（Windows bash）：
```
python tools/ida_angel_cashshop_ea.py
python tools/ida_angel_cashshop_decompile.py
python tools/ida_angel_cashshop_normalize.py
python tools/ida_angel_cashshop_facts.py
python tools/ida_angel_cashshop_crosscheck.py
```

## §10 限制与未知

- `CMovePath::OnMovePacket` 类底层移动布局本面未取（与第五面同限制）。
- `CItemInfo`(47) / `CWvsContext`(197) / `CUserLocal`(120) 等 EA 宽集大类只入 EA 索引、未全 decompile；商品数据结构 `UCS_COMMODITY` 只有 `ZRef` 构造/析构 2 方法，字段序需 r02 用当前端 `Character/Inventory` 模型补齐。
- `CCashShop::OnCashItemResult` 48 个 case 中 24 个落到 `sub_47xxxx`（无符号辅助函数），其语义本面记为【未知】，须 r02/r05 结合 s9 handler 补。
- 负值 case（`-123..-95`）是 `Decode1` 返回 unsigned byte 被 Hex-Rays 显示成 signed char 溢出，实为 133..161 段子命令，【推断】。
- IMG 层 orange-wz(IMG) 阻塞 → `UI_CASHSHOPIMG_*` 节点件表只能靠源码字面量 + 正确端截图，须 r03 覆盖度小结；`read_image` 对 PNG 报 `model "hy4free" does not declare image input` → 未做像素级视觉复核。
- 全程只读取证，未改任何产品源码、未构建、未实机、未提交。
