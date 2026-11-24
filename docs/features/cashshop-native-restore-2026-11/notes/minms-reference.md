# MinMS 原生商城实现对照参考（给「按 MinMS/Angel 源码原生还原」用）

> 本文为纯只读研究报告，未修改任何源码。所有结论均带源文件行号。
> 阅读对象：`E:/project/MinMS/src/client/IO/UITypes/UICashShop.cpp`（3440 行）+ `.h`（567 行）+ `E:/project/MinMS/src/client/Data/CommodityData.{h,cpp}`（175 / 471 行）。
> 对照对象：`E:/project/MapleStory-Client/IO/UITypes/UICashShop.{h,cpp}`（365 / 1705 行）。
> 引用路径约定：`CD.h`=CommodityData.h，`CD.cpp`=CommodityData.cpp，`CS.h`=MinMS UICashShop.h，`CS.cpp`=MinMS UICashShop.cpp，`ET.cpp`=MinMS EquipTooltip.cpp，`本地 .h/.cpp`=本端 MapleStory-Client 版本。

---

## 0. 阅读前必须知道的三条"底层逻辑"（否则全盘误读）

1. **MinMS 的 UICashShop 是"手机优先（Phone-first）"改写版，不是 800×600 原版的逐行拷贝。** 类注释 `CS.h:29` 自称 "Phone-first Cash Shop"。所有布局常量分两层：
   - **美术坐标（art / WZ 坐标）**：以 800×600 原生图为基准的量（如 `GRID_X=278`、`NAME_X=76`），见 `CS.h:129-131` 注释 "Classic CS art is 800×600; scale to the phone viewport"。
   - **屏幕坐标补丁（screen px，sc() 之后叠加）**：`LIST_SCREEN_DX/DY`、`NAME_DY_PX`、`COL1_SCREEN_DX`、`ROW_GAP_PX` 等（`CS.h:144-158`）。换算函数 `sc()` = `lround(wz*scale)`（`CS.cpp:295-298`），`list_sx/list_sy` = `sc(wz)+LIST_SCREEN_D*`（`CS.cpp:300-308`）。**还原原生 800×600 时，取 art 坐标层，忽略 screen 补丁层。**
2. **原生 UI 资源来自 `UI/CashShop.img`，监护类只加载节点、绝大多数字段是贴图烘焙。** 构造期贴图清单：`CS.cpp:151-176`（`Base/backgrnd`、`Base/Preview/0`、`CSList/Base|ItemIcon|BtBuy|BtGift|BtReserve|BtRemove`、`CSTab/Tab/1..9`）。
3. **关键版本差异必须先对齐，否则会把"版本不同"误判成"移植残缺"**：用户截图的 9 个一级 tab 是「首页 装备 消耗 设置 其他 宠物 礼包 帮助 购物车」，而 MinMS 的 9 个是「首页 **热卖** 装备 消耗 设置 其他 宠物 礼包 购物车」（`CD.h:72-86`、`CD.cpp:300-315`）。截图 13 个装备二级类含「上衣/标/骑宠」，MinMS 是 12 个含「热销/勋章」。这是**版本口径差异**（见 §1.3 / §8），还原时要明确以哪一套为准。

---

## 1. 分类映射

### 1.1 一级 tab：Category 枚举（9 个）
`CD.h:72-86` 定义，`CD.cpp:300-315` 给出中文 label：

| 索引 | 枚举 | label | 依据 |
|---|---|---|---|
| 0 | `CAT_HOME` | 首页 | `CD.h:76`、`CD.cpp:304` |
| 1 | `CAT_HOT` | 热卖 | `CD.h:77`、`CD.cpp:305` |
| 2 | `CAT_EQUIP` | 装备 | `CD.h:78`、`CD.cpp:306` |
| 3 | `CAT_USE` | 消耗 | `CD.h:79`、`CD.cpp:307` |
| 4 | `CAT_SETUP` | 设置 | `CD.h:80`、`CD.cpp:308` |
| 5 | `CAT_ETC` | 其他 | `CD.h:81`、`CD.cpp:309` |
| 6 | `CAT_PET` | 宠物 | `CD.h:82`、`CD.cpp:310` |
| 7 | `CAT_PACKAGE` | 礼包 | `CD.h:83`、`CD.cpp:311` |
| 8 | `CAT_CART` | 购物车 | `CD.h:84`、`CD.cpp:312` |

枚举注释明确 "Order: 首页 热卖 装备 消耗 设置 其他 宠物 礼包 购物车"（`CD.h:73`），并注明 "Main tabs match UI/CashShop.img CSTab/Tab/1..9"（`CD.h:72`）——即贴图 key 是 **1-based**（`CS.cpp:172-176`：`tabs[std::to_string(i+1)]`）。

**首页 / 热卖 / 购物车 的处理（不是普通商品列表）**：
- 首页：`rebuild_shop_rows()` 早退，`list_offset=0; selected_row=-1; sync_page_chrome(); rebuild_row_buttons(); return;`（`CS.cpp:1202-1210`）——注释 "首页: banners later"，即首页只显示背景烘焙 banner，无商品数据。
- 购物车：`rebuild_shop_rows()` 走 `rebuild_cart_rows()`（`CS.cpp:1211-1215`），本端购物车是**客户端本地 wishlist**（`CS.h:558` 注释 "Local wishlist (CSTab 购物车); not sent to server yet"）。
- 热卖：`rebuild_shop_rows()` 从 `on_sale()` 过滤 `sn_category(c.sn)==cat`（`CS.cpp:1217-1223`），并按 Priority 降序稳定排序（`CS.cpp:1244-1263`，阈值 `HOT_PRIORITY=8`，`CD.h:161`）。

### 1.2 二级分类数量：`sub_count()`（`CD.cpp:335-346`）
| 一级 | 二级数量 | 依据 |
|---|---|---|
| 首页 CAT_HOME | 0 | `CD.cpp:344` default |
| 热卖 CAT_HOT | `HOTSUB_COUNT`=2 | `CD.cpp:339`、`CD.h:111-116` |
| 装备 CAT_EQUIP | `EQSUB_COUNT`=**12** | `CD.cpp:340`、`CD.h:93-108` |
| 消耗 CAT_USE | `USESUB_COUNT`=3 | `CD.cpp:341`、`CD.h:119-125` |
| 设置 CAT_SETUP | 0 | `CD.cpp:344` |
| 其他 CAT_ETC | `ETCSUB_COUNT`=7 | `CD.cpp:342`、`CD.h:128-138` |
| 宠物 CAT_PET | `PETSUB_COUNT`=3 | `CD.cpp:343`、`CD.h:141-147` |
| 礼包 / 购物车 | 0 | `CD.cpp:344` |

### 1.3 装备二级类：12 类精确顺序与标签（`CD.h:93-108` 枚举 + `CD.cpp:420-438` label）

| 序 | 枚举 | label | 匹配 item type（`CD.cpp:440-469`） |
|---|---|---|---|
| 0 | `EQSUB_CAP` | 帽子 | type==100（`CD.cpp:452`） |
| 1 | `EQSUB_FACE` | 脸饰 | type==101\|\|103（103 耳环并入脸饰，`CD.cpp:453-454`） |
| 2 | `EQSUB_EYE` | 眼饰 | type==102（`CD.cpp:455`） |
| 3 | `EQSUB_OVERALL` | 长袍 | type==104\|\|105（无独立上衣 tab，104 并入长袍，`CD.cpp:456-457`） |
| 4 | `EQSUB_HOT` | **热销** | priority>=8（`CD.cpp:458`） |
| 5 | `EQSUB_PANTS` | 裤裙 | type==106（`CD.cpp:459`） |
| 6 | `EQSUB_SHOES` | 鞋子 | type==107（`CD.cpp:460`） |
| 7 | `EQSUB_GLOVES` | 手套 | type==108（`CD.cpp:461`） |
| 8 | `EQSUB_WEAPON` | 武器 | type==109 或 130..170（`CD.cpp:462-464`） |
| 9 | `EQSUB_RING` | 戒指 | type==111（`CD.cpp:465`） |
| 10 | `EQSUB_MEDAL` | **勋章** | type==114（`CD.cpp:466`） |
| 11 | `EQSUB_CAPE` | 披风 | type==110（`CD.cpp:467`） |

> ⚠️ **与截图的硬差异**：用户截图 13 类为「帽子 脸饰 眼饰 长袍 **上衣** 裤裙 鞋子 手套 武器 戒指 **标** 披风 **骑宠**」（戒指选中）。MinMS 这里**没有**「上衣 / 标 / 骑宠」，多了「热销 / 勋章」。MinMS 源码自己注释承认取舍："No separate 上衣 tab — coats (104) sit with overalls (105)"（`CD.cpp:456`）、"Category.img has no 耳环 — fold 103 into 脸饰"（`CD.cpp:453`）、"shields 109 (no dedicated tab)"（`CD.cpp:463`）。**还原 13 类截图态需要补 3 个类并重排，MinMS 12 类只能作为 SN/type 匹配逻辑的参考，不能直接当标签表。**

### 1.4 其他一级的二级标签（`CD.cpp:348-392`）
- 热卖：新品 / 活动（`CD.cpp:355-356`）。
- 消耗：卷轴 / 通讯物品 / 天气（`CD.cpp:364-366`）。
- 其他：美容美发 / 个人商店 / 游戏 / 表情 / 婚礼 / 效果 / 人物（`CD.cpp:372-378`）。
- 宠物：宠物 / 宠物装备 / 宠物消耗（`CD.cpp:384-386`）。
- 设置 / 礼包 / 购物车 / 首页：无二级（`CD.cpp:389-391` default 返回 ""）。

### 1.5 商品归类与 SN 规则（决定"哪个商品进哪个 tab"）
- `category_for_item()`（`CD.cpp:257-298`）：礼包(is_package 或 type 910/911)→CAT_PACKAGE（`:264-267`）；宠物(type 500/524/180–183)→CAT_PET（`:269-274`）；装备(prefix==1，即 1xxxxxx)→CAT_EQUIP（`:276-280`）；消耗(type 508–514)→CAT_USE（`:282-287`）；设置(type 301–399)→CAT_SETUP（`:289-293`）；其余 5xxxxxx→CAT_ETC（`:295-297`）。
- `sn_category(sn)=sn/10000000`（`CD.cpp:317-324`）；`sn_subcategory(sn)=(sn/100000)%100`（`CD.cpp:326-333`）。
- `matches_sub()`（`CD.cpp:394-418`）：先比 `sn_category==cat`，装备类特殊——`EQSUB_HOT` 走 priority（`CD.cpp:404-407`），否则先比 SN 子类、不符再用 item type 回退（`CD.cpp:408-415`）。
- 数据源：`Ensures loaded` 从 `Etc/Commodity.img` 读 SN/ItemId/Count/Price/Period/Gender/Priority/OnSale（`CD.cpp:18-42`），`on_sale_list` 只保留 `on_sale`（`CD.cpp:166-182`）；礼包标记来自 `CashPackage.img`（`CD.cpp:117-121`）。

---

## 2. 商品卡片布局（2 列 × 5 行 = 10/页）

### 2.1 网格常量（`CS.h:132-146`）——art 坐标层
- `GRID_X=278`（`:135`，注释 "was 255"）、`GRID_Y=88`（`:136`，"was 138"）：注释 "CSTab strip + item list: art nudge (+23,-50) vs classic anchors"（`CS.h:134`）。**所以要还原经典 800×600 原版，基准应是 255/138，MinMS 把它挪到了 278/88。**
- `CARD_W=200`、`CARD_H=80`（`:137-138`）；`COL_GAP=0`（`:139`）；`GRID_COLS=2`、`GRID_ROWS=5`、`VISIBLE_CARDS=10`（`:140-142`）。
- `card_pos()`（`CS.cpp:678-692`）：`x = list_sx(GRID_X + col*(CARD_W+COL_GAP))`，第 2 列额外 `+COL1_SCREEN_DX=10`（`:148`、`CS.cpp:683-686`）；`y = list_sy(GRID_Y) + row*(sc(CARD_H)+ROW_GAP_PX)`，`ROW_GAP_PX=2`（`:150`、`CS.cpp:690`）。
- screen 补丁：`LIST_SCREEN_DX=-5`、`LIST_SCREEN_DY=8`（`CS.h:145-146`）。

### 2.2 卡内元素
- **卡片底图**：`list_base = Texture(CSList/Base)`（`CS.cpp:162-163`），每张卡 draw（`CS.cpp:2976-2979`）。
- **icon 槽**：`ICON_INSET_X=5`、`ICON_INSET_Y=5`、`ICON_SLOT=64`（`CS.h:159-162`，注释 "CSList/Base white icon well is (5,5)..(68,68); ItemIcon frame is 72×72"）；`ICON_SCALE=2.0f`（`:163`）。`draw_card_icon()` 居中缩放（`CS.cpp:694-718`）。选中卡另画 `item_icon_frame` at `card_pos + (sc(4),sc(4))`（`CS.cpp:2981-2985`）。
- **名称/价格**：`NAME_X=76`、`NAME_Y=10`、`PRICE_Y=32`（`CS.h:164-166`）；screen 补丁 `NAME_DY_PX=-14`、`PRICE_DY_PX=-10`（`:167-169`）。draw 时 `row.name` at `card_pos+(sc(NAME_X), sc(NAME_Y)+NAME_DY_PX)`，`row.price_text` at `card_pos+(sc(NAME_X), sc(PRICE_Y)+PRICE_DY_PX)`（`CS.cpp:2990-2999`）。
- **字体/颜色/对齐**：`row.name`/`row.price_text` = `{ Text::A13B, Text::LEFT, Text::BLACK, Text::FIELD }`（`CS.cpp:1279-1280`）；draw 时再传 `Color::BLACK`（`CS.cpp:2989-2999`）。即 **A13B 字体、左对齐、黑色、FIELD 无描边**。
- **三个按钮**：`ACTION_Y=54`、`ACTION_W=37`、`ACTION_H=19`、`ACTION_STEP_PX=48`、`ACTION0_X=70`（`CS.h:171-177`，注释 "购买 / 赠送 / 加入购物车 (same Y)"）、`ACTION_DX_PX=11`（`:178`）。X 计算 `action_btn_screen_x(i)=sc(ACTION0_X)+ACTION_DX_PX+i*48`（`CS.cpp:465-472`）。Y=`sc(ACTION_Y)`（`CS.cpp:3010/3018/3033`）。贴图来源：`BtBuy/normal|disabled/0`、`BtGift/normal/0`、`BtReserve/normal|disabled/0`（购物车）、`BtRemove/normal/0`（购物车态第三个按钮变成删除，`CS.cpp:165-170`、`CS.cpp:3022`）。
- **徽章（EUEW/NEW/SALE）**：**MinMS 根本没有实现**。`CS.cpp` 全文无 badge/label 绘制（grep 确认所有 `label` 命中都是二级分类文字/背包 tab/名牌，无商品徽章），也没有 `CSList` 下的 `BtNew/BtSale` 之类贴图加载（构造期只加载 `:162-170` 列出的贴图）。用户截图的左上角徽章在 MinMS 里缺失——还原须另找原生资源/逻辑。

### 2.3 价格文本格式（`CS.cpp:1281`）
`row.price_text.change_text(std::to_string(row.price) + " 点券");`
- **有 " 点券" 后缀**（半角空格 + 中文"点券"）。
- **没有千分位分隔符**（无 `2,480`，而是 `2480 点券`）——`CS.cpp` 全文无 `split_number`/逗号格式化（grep 确认）。这与截图「2,480 点券」不符。
- 余额数字同样是裸 `std::to_string`（`CS.cpp:1505-1507`），无分隔符。

---

## 3. 一级 / 二级 tab

### 3.1 一级 tab 贴图与选中态
- 贴图：`category_tabs[i] = Texture(tabs[std::to_string(i+1)])`，`tabs = UI/CashShop.img/CSTab/Tab`（`CS.cpp:172-176`）。**是整条贴图**（每个 tab 一张 508×78 条带里的一帧），不是逐字绘制。
- draw：只画当前 `category` 那一张：`tab.draw(position + (list_sx(TAB_STRIP_X), list_sy(TAB_STRIP_Y)), scale)`（`CS.cpp:2947-2954`）。即**选中态 = 选择不同贴图**，没有额外的 highlight 叠加。
- 坐标常量：`TAB_STRIP_X=278`（"was 255"）、`TAB_STRIP_Y=10`（"was 60"）、`TAB_STRIP_H=78`（`CS.h:212-215`，注释 "+23 x, -50 y vs classic backgrnd anchors"）。经典原版锚点同样是 255/60。
- 命中区：`TAB_SLOTS[9]`（`CS.h:194-211`，`TabSlot{x,w}`）：`{0,68},{68,52},{120,52},{172,52},{224,52},{276,52},{328,52},{380,60},{440,68}`；`TAB_HIT_PAD_X=2`（`:216`）。按钮维度 `slot.w+2*PAD` × (`sub_panel?MAIN_TAB_ROW_H:TAB_STRIP_H`)（`CS.cpp:802-822`），`MAIN_TAB_ROW_H=52`（`CS.h:218`）。
- 说明：一级 tab 的文字是**烘焙在贴图里的**（CSTab/Tab/N 整条贴图），C++ 不绘制一级 tab 文字。

### 3.2 二级分类
- **背景带**：就是一级 tab 贴图 `CSTab/Tab/N` 的下半部分——"Equip / 热卖 / 消耗 / 其他 / 宠物 sub-tabs share the pink band on CSTab"（`CS.h:217`）。**没有单独的二级背景贴图**，只有选中态叠一层半透明白 `ColorBox select_bg(1,1,Geometry::WHITE,0.18f)`（`CS.cpp:640`、`:646-651`）。
- 行常量：`SUB_ROW_Y=53`、`SUB_ROW_H=24`、`SUB_STRIP_W=508`、`SUB_HIT_PAD_Y=4`（`CS.h:218-222`）；二级槽 `category_sub_slot()` 把 508 宽均分成 count 份（`CS.cpp:594-616`）：`slot.x = strip_x + i*strip_w/count`，`slot.w` 到下一段边界；行 y `list_sy(TAB_STRIP_Y+SUB_ROW_Y)`，命中区额外上下扩 `SUB_HIT_PAD_Y`（`CS.cpp:610-614`）。
- 文字是**运行时绘制**：`Text(Text::A13B, Text::CENTER, Text::BLACK, Text::FIELD, sub_label(cat,i))`（`CS.cpp:653-658`），垂直居中 `label_y = row_y + row_h/2 - (linespace+5)/2`（`CS.cpp:659-664`）。
- 何时显示：仅当 `active_sub_count()>0`（`CS.cpp:583-592`，委托 `sub_count(category)`），即首页/设置/礼包/购物车无二级。

---

## 4. 左侧面板

### 4.1 预览窗
- 贴图：仅 `preview_well = Texture(base["Preview"]["0"])`（`CS.cpp:160`）；draw at `position + (preview_sx(PREVIEW_X), preview_sy(PREVIEW_Y))`（`CS.cpp:2887-2893`）。
- 锚点 art 常量：`PREVIEW_X=20`、`PREVIEW_Y=50`、`PREVIEW_W=212`、`PREVIEW_H=165`（`CS.h:292-296`）；screen 补丁 `PREVIEW_SCREEN_DX=5`、`PREVIEW_SCREEN_DY=-12`（`:297-299`）。
- 角色脚底 `CHAR_FEET_X=126`、`CHAR_FEET_Y=195`（`CS.h:300-301`）；井内地板 `PREVIEW_FLOOR_Y=134`、梯子 `PREVIEW_LADDER_X=150, Y1=10, Y2=134`、`PREVIEW_SPAWN_X=86`（`:302-307`）。角色用自绘 `CharLook preview`（`CS.cpp:2907`），脚底 `preview_feet_art()`（`CS.cpp:2896-2900`）。
- **ON 按钮 / 红 X / 保存图标**：三个 CSChar 按钮来自 `UI/CashShop.img/CSChar`：`BtBuyAvatar`/`BtDefaultAvatar`/`BtTakeoffAvatar`（`CS.cpp:243-249`）。art X=`BT_BUY_AVATAR_X=27`/`BT_DEFAULT_AVATAR_X=89`/`BT_TAKEOFF_AVATAR_X=151`，Y=`BT_AVATAR_Y=232`（`CS.h:319-322`）；screen 补丁 `CSCHAR_BTN_SCREEN_DX[3]={-17,10,40}`、`CSCHAR_BTN_SCREEN_DY=20`（`:325-326`），命中 padding `AVATAR_HIT_PAD=10`（`:327`）。按钮文字是**贴图自带**，C++ 不绘制。截图里的「ON/OFF 聊天条」是 `preview_chat_field`（`CS.cpp:2942`）＋ `ChatBalloon`（`CS.cpp:2937-2940`），聊天条 art `PREVIEW_CHAT_Y=212`、`PREVIEW_CHAT_H=20`（`CS.h:308-315`）。

### 4.2 4 个模式 tab（热点推荐/购买试穿/我的现状/原型）
**MinMS 没有这组 4-tab 控件。** 对应功能被拆成 CSChar 的 3 个按钮 + 内部状态：`reset_preview_to_baseline()`（"我的现状"/还原）、`reset_preview_to_prototype()`（"原型"）、`try_on_selected_row()`（购买试穿）（`CS.h:449-453`、`CS.cpp:1779-1814`、`2088`）。`button_pressed()` 里 `BT_DEFAULT_AVATAR`→baseline、`BT_TAKEOFF_AVATAR`→prototype（`CS.cpp:3349-3358`）。**截图 4-tab 的贴图/坐标/label 在 MinMS 中不存在**，需回到原生 Asset 或本端资源另找。

### 4.3 已购道具面板（CSInventory / Locker）
- 网格：`PURCHASED_COLS=6`、`PURCHASED_ROWS=2`、`PURCHASED_SLOT_COUNT=12`（`CS.h:245-248`）；格子中心 art 坐标 `PURCHASED_COL_CX[6]={36,71,106,141,176,211}`、`PURCHASED_ROW_CY[2]={364,398}`（`:249-251`）。
- 槽中心 `purchased_slot_center()`（`CS.cpp:2131-2139`）= `side_sx/side_sy`（= `sc(wz)+SIDE_SLOT_SCREEN_DX/DY`，`CS.h:289-291` 的 `-18/+58`，`CS.cpp:402-410`）。
- 数据：`locker_rows`（`CS.h:560`），服务端 locker 项；draw at `CS.cpp:2483-2510`，选中项画 `item_icon_frame`（缩放 0.5）。图标 cell `SIDE_ICON_CELL=32`、`SIDE_ICON_SCALE=1.0`（`CS.h:287-288`），cell 锚见 `side_icon_cell_tl()`（`CS.cpp:53-78`）。**格子里的文字（如数量）MinMS 没画**，只画 icon。

### 4.4 背包面板
- 网格：`BACKPACK_COLS=4`、`BACKPACK_ROWS=3`、`BACKPACK_SLOT_COUNT=12`（`CS.h:252-255`）；中心 art `BACKPACK_COL_CX[4]={38,73,108,143}`、`BACKPACK_ROW_CY[3]={495,530,562}`（`:256-257`）。
- 5 个背包 tab：`BACKPACK_TAB_COUNT=5`、`BACKPACK_TAB_Y=470`（`CS.h:258-259`），art 宽 27×16（`Base/Tab2/Enable|Disable` 标签，`CS.h:260-262`），底托盘 `UIWindow.img/Item/New` 的 `Tab0/Tab1`（34×19，`CS.h:263-267`、`CS.cpp:197-209`）。tab 类型顺序 `BACKPACK_TAB_TYPES[5]={EQUIP,USE,SETUP,ETC,CASH}`（`CS.cpp:131-138`）。
  - tab 贴图：`backpack_tab_bg_idle/selected[i]`（托盘）、`backpack_tab_enable/disable[i]`（文字），draw at `draw_backpack_tabs()`（`CS.cpp:2215-2269`）。**tab 文字是贴图（Enable/Disable），不是运行时 Text**。位置 `backpack_tab_plate_origin()`（`CS.cpp:412-441`），screen 补丁 `BACKPACK_TAB_SCREEN_DX=30, DY=-73`（`CS.h:268-270`）。
  - "扩栏按钮"：`CSInventory` 的 `BtExEquip/BtExConsume/BtExInstall/BtExEtc/BtExTrunk`（`CS.cpp:212-224`），art `BACKPACK_EXT_X=168`、`Y0=468`、step 22（`CS.h:273-276`），screen 补丁 `+27/-76`（`:277-279`），目标显示宽 `BACKPACK_EXT_SCREEN_W=78`（`:284-285`）。按钮文字同样贴图自带（截图「装备窗/消耗窗/设置窗/其他窗/特殊窗/仓库」）。
- 背包格 draw（`CS.cpp:2514-2563`）：从 `Stage::get().get_player().get_inventory()` 取 item icon，可转现金装备加 `draw_side_cash_badge()`（`CS.cpp:2397-2423`，`CashShop.img/CashItem/0` 角标）。

---

## 5. 右侧热销榜 + tooltip

### 5.1 热销榜：**MinMS 未实现**
MinMS UICashShop 中没有任何"右侧 5 行榜单"的绘制或数据结构（grep `Best/Rank/排行/热销榜` 无命中；唯一 `EQSUB_HOT` 是装备二级的排序列表，在中间商品区 `CS.cpp:1244-1263`）。用户截图右侧「热销榜 1-5 条目 + 搜索道具按钮」在 MinMS 里没有对应代码。本端反而有（`本地 .h:154-162` `BEST_ITEM_X=742/Y0=157/DY=69/SLOT=32/BEST_ROWS=5`、`WND_SEARCH_X=690/Y=97`、`best_icons[]`）——这块以本端为准。

### 5.2 tooltip 触发
- 命中：鼠标在卡片上（`hit_card()`→`row_at_local()`，`CS.cpp:2126-2129`、`2566`）且**点击卡片**时 `button_pressed(BT_CARD*)` 调 `show_row_tooltip(row)`（`CS.cpp:3430-3437`）；悬停本身不弹（MinMS 是点击触发）。
- `show_row_tooltip()`（`CS.cpp:2593-2628`）：取 `shop_row.item_id` 与 SN 对应的 `commodity->period`，调 `UI::get().show_item(Tooltip::CASHSHOP, item_id, period, tooltip_anchor_for_row(row))`（`CS.cpp:2623-2627`）。锚点 = 卡片位置（`CS.cpp:2115-2124`）。
- 分发（`UIStateCashShop.cpp:210-249`）：礼包(type 910 且有 package contents)→`set_package`（`:213-225`）；装备类 `InventoryType::by_item_id==EQUIP`→`eqtooltip.set_item(parent,itemid)` + `set_period(period)`（`:227-243`）；其他→`ittooltip.set_item`（`:245-249`）。

### 5.3 tooltip 内容格式（来自共享 `EquipTooltip`，不在 UICashShop 内）
- 需求属性行：`LEVEL/STR/DEX/INT/LUK/FAME` 依次 push（`ET.cpp:239-244`），draw 顺序 `LEVEL→STR→DEX→INT→LUK→FAME`（`ET.cpp:772-777`）＋ 成长 itemLEV/itemEXP（`:779-801`），共 8 行（`ET.cpp:652`）。贴图 key `Cannot/Can` 的 `reqLEV/reqSTR/reqDEX/reqINT/reqLUK/reqPOP`（`ET.cpp:246-260`）。
- 6 个职业 chip：`beginner, warrior, magician, bowman, thief, pirate`（`ET.cpp:273-275`），按 item 的 `reqJob` 决定 enabled（`ET.cpp:460-496`）。对应截图「新手/战士/魔法师/弓箭手/飞侠/海盗」。
- 装备分类行：`push_line("装备分类 : " + equipdata.get_type())`（`ET.cpp:598`）——注意冒号是**半角空格 + 半角冒号**。
- 描述：`itemdata.get_desc()`，`Text{A11M, LEFT, WHITE, desc, tip_width-sc(24)}`（`ET.cpp:569-577`）。对应截图底部描述文本。
- 面板标题/名字/图标等见 `ET.cpp:187-203`、`:444-457`、`:637-661`。

---

## 6. 底部分页

- 页码文本：`page_label.change_text(std::to_string(cur+1) + "/" + std::to_string(pages))`（`CS.cpp:741-758`，具体 `:745-746`），格式 **`当前/总页数`**，如 `1/8`。**不是**截图的 `<1>` / `[12]` 那种"当前页带尖括号、其他页方括号多页并列"格式——MinMS 只显示一个页码框。本地同理（`本地 .cpp:1499`：同样是 `cur+1 + "/" + pages`）。这是与截图的另一处硬差异。
- 字体：`page_label = Text(Text::A13B, Text::CENTER, Text::BLACK, Text::FIELD, "1/1", 0, false)`（`CS.cpp:265`）。
- 箭头：`UIWindow.img/MonsterBook` 的 `arrowLeft/arrowRight` MapleButton（`CS.cpp:183-185`），`PAGE_BTN_VISUAL_SCALE=0.8f`（`CS.h:154`）。
- 坐标（`CS.cpp:826-854`）：`page_y = list_bottom_y() + PAGE_BAR_GAP_PX(=8)`；`list_bottom_y() = list_sy(GRID_Y)+GRID_ROWS*sc(CARD_H)+(GRID_ROWS-1)*ROW_GAP_PX`（`CS.cpp:733-739`）；`page_cx = (grid_left+grid_right)/2`（网格水平中心）；`PAGE_ARROW_FROM_CENTER=40`（`CS.h:156`）→ `prev_x=page_cx-40-arrow_w`（arrow_w=`lround(24*scale*0.8)`，`CS.cpp:837-842`）、`next_x=page_cx+40`。页码画在 `page_y+4+PAGE_LABEL_DY_PX(=-16)`（`CS.cpp:3049-3051`，`CS.h:158`）。
- 翻页逻辑 `change_list_page()`（`CS.cpp:760-780`）：clamp `[0,pages-1]`，改 `list_offset=next*10`，越界重置 `selected_row`，`rebuild_row_buttons()` + `sync_page_chrome()`。

---

## 7. 余额栏

- 三币种标签与数值：**标签是烘焙在 `backgrnd` 贴图里的**（`CS.h:183` 注释 "Bottom balance rows (labels baked into backgrnd); values right-aligned in slots"）；C++ 只画三个右对齐数值 Text：`nx_label` / `maple_label` / `prepaid_label`（`CS.h:529-532`）。
- 数值坐标：`BALANCE_VALUE_X=462`（`CS.h:184`）；`NX_BALANCE_Y=533`、`PREPAID_BALANCE_Y=546`、`MAPLE_BALANCE_Y=559`（`:185-188`，注释 "点券 / 信用卡 / 抵用券 — prepaid sits on the middle row"）。draw at `CS.cpp:3059-3068`。
- 字体/颜色：`Text(Text::A13B, Text::RIGHT, Text::BLACK, Text::FIELD, "", 0, false)`（`CS.cpp:261-263`）——A13B、右对齐、黑色、无描边。
- 数值格式：`std::to_string(nx|maple|prepaid)`（`CS.cpp:1505-1507`），**无千分位**。
- 充值和退出按钮：退出是 `backgrnd` 自带贴图 + 透明命中区 `EXIT_X=620/EXIT_Y=532/EXIT_W=158/EXIT_H=46`（`CS.h:189-193`、`CS.cpp:178-181`）。**MinMS 没有独立"充值"按钮的实现**（构造函数未加载 BtCharge* 之类节点）。

---

## 8. 本端对比：缺失 / 错位清单

### 8.1 本端已有的、且比 MinMS 更贴近截图的（保留）
- 右侧热销榜 5 行 icon（`本地 .h:154-162`、`.cpp:refresh_best_icons()`）、搜索按钮常量（`本地 .h:140-141`）——MinMS 无，见 §5.1。
- 商品徽章 `Item::Label` 枚举（含 `NEW=14`、`SALE=17`、`EVENT=4`、`HOT` 等，`本地 .h:191-218`），且 `.cpp:407-412` 有 `item_labels[li].draw(plate+(4,2))`——MinMS 无徽章，见 §2.2。这是截图「左上角徽章」的正确落点，但要核对 Label→贴图帧映射与 SN 元数据是否真的填了 label（当前 `rebuild_items` 里 `items.emplace_back(..., Item::Label::NONE, ...)` 全填 NONE：`.cpp:1450/1467/1484`，即**徽章枚举有、数据没接**）。
- 价格千分位：`string_format::split_number(price_text)` + `" " + Locale::text("cashshop.currency")`（`.cpp:1573-1580`），产出 `2,480 点券`，**已符合截图**；MinMS 反而没有（§2.3）。

### 8.2 本端与 MinMS 对照的缺失 / 错位（按影响排序）
1. **一级 tab 顺序语义**：本端 `TAB_CATEGORY[9]={8,1,2,3,4,5,6,7,-1}`（`.h:185-189`）把索引 1 当"热卖"、索引 8 当"礼包"、第 9 个当购物车；截图要求「装备」在索引 1、且第 8 个是「帮助」。需按目标版本重排 `menu_index`↔category↔贴图帧映射（MinMS `TAB_SLOTS`/`CD.h:73` 只作参考）。
2. **装备二级 12 vs 截图 13**：本端 `SUB_COUNTS[9]={0,2,12,3,0,7,3,0,0}`、`SUB_LABELS` 装备 12 项（`.cpp:1593-1598`，含热销/勋章、无上衣/标/骑宠）——与 MinMS 完全一致，因此同样缺截图的 3 类。需补 13 类标签与 type 映射（§1.3）。
3. **分页格式**：本端 `page_label` 仍是 `cur+1/pages`（`.cpp:1499`），截图要求 `<1>` / `[12]` 多页样式（§6）。
4. **模式 4-tab**：两端都没有截图的「热点推荐/购买试穿/我的现状/原型」4-tab 贴图与文字（§4.2）。
5. **余额栏数值精度/坐标**：MinMS `BALANCE_VALUE_X=462, Y=533/559(点券/信用卡), 546(抵用券)`；本端 `x=484, y=546/562/578`（`.cpp:367-375`，`WND_STATUS_Y+16/32/48`）。币种行序也不同：MinMS 中间行是"抵用券"（`CS.h:186-188`），本端中间行是"信用卡(maple)"（`.cpp:355-357/373-375`）。截图是「点券余额 0 / 抵用券 5,240」——需确认目标版本里抵用券到底在第二行还是第三行后再定 y。
6. **价格无千分位只在 MinMS 侧**：本端已正确（§8.1）；还原时**不要**照搬 MinMS 的 `std::to_string(price)+" 点券"`。
7. **tooltip 装备分类冒号**：`EquipTooltip.cpp:598` 是 `"装备分类 : "`（半角），截图是「装备分类：戒指」（全角冒号）。若要求逐字一致需调整（是否改属文案资源，需另判）。

---

## 9. 原生还原优先级清单（P0 先做，按"截图可见 & 两端都缺/都错"排序）

| 优先级 | 项 | 证据 | 说明 |
|---|---|---|---|
| P0 | 装备二级改 13 类（补上衣/标/骑宠，去热销/勋章或按目标版本定） | `CD.h:93-108`、`CD.cpp:420-438`、`本地 .cpp:1593-1598`；截图 | 一眼可见，且直接决定"戒指"能否落在正确槽位 |
| P0 | 一级 tab 顺序（装备置索引 1、帮助）与 9 帧贴图映射 | `CD.h:72-86`、`本地 .h:189`；截图 | 全局导航错位，后续所有分类数据都跟着错 |
| P0 | 分页格式 `<当前> [其他…]` 多页并列 | `CS.cpp:745-746`、`本地 .cpp:1499`；截图 | 底部一眼可见，格式与截图不符 |
| P1 | 商品徽章接真实数据（Label NONE→按 SN/Priority/Period 映射 NEW/SALE/EUEW） | 本地 `.h:191-218`/`.cpp:1450,1467,1484,407-412`；MinMS 无 | 枚举与绘制在，仅缺数据源；改动小、可见度高 |
| P1 | 左侧 4 模式 tab 控件（贴图+文字+命中） | 两端均无；截图 | 需先定资源来源（原生 img 或本端补画），是独立新增件 |
| P1 | 余额栏币种行序与坐标（抵用券行对齐截图） | `CS.h:183-188`、`本地 .cpp:355-375`；截图 | 数字错位/币种错位会误导玩家 |
| P2 | 价格千分位保持本端 `split_number`（勿退回 MinMS 裸数字） | 本地 `.cpp:1573-1580` vs `CS.cpp:1281`；截图 `2,480 点券` | 防"还原"反而改坏 |
| P2 | tooltip 装备分类冒号全角/半角、需求属性行与 6 职业 chip 逐字对齐 | `ET.cpp:239-244,273-275,598`；截图 | 细节一致性，低风险 |
| P2 | 右侧热销榜沿用本端 `best_icons`（MinMS 无对应实现，别去找） | 本地 `.h:154-162`/`.cpp:refresh_best_icons()`；`CS.cpp` 无命中 | 明确口径，避免无效返工 |
| P3 | screen 补丁层不移植：还原只取 art 坐标（GRID 255/138、TAB 255/60） | `CS.h:134-136,144-158,212-215` | 手机适配层，原生 800×600 不需要 |

### 一句话结论
MinMS 是**手机改写版**：分类/SN/type 匹配规则、卡片几何、面板锚点、tooltip 分发逻辑可直接对照；但**徽章、千分位价格、右侧热销榜、4 模式 tab、`<1>[12]` 分页**这五类截图特征它要么没有、要么格式不同——其中本端已经做对了徽章与千分位，还原时按本文标注"取本端/补新增/改格式"分别处理，切忌整体照抄 MinMS。
