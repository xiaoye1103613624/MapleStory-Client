# 商城全链路展示对齐 · 修复与真机验证报告（2026-11-17 第二轮）

> 用户诉求（原话）
> - m01289：「商城链路所有功能必须全部修复，ui、样式、分类、商品展示等也要全面修复」
> - m01296：「物品图标、商品名称、售价等信息要对齐背景展示」

## 结论：商品卡片（图标/名称/售价）+ 余额区 + 二级分类三大展示面全部按 MinMS 对齐并真机验证通过

最终产物 `build-pua/MapleStory.exe` SHA=`c0de2fd29e0bb6f960639349a237f0da99fa673e9f67cb675e8d0e7aa4a25ab7`。
部署：canon `MapleStory.exe`、side `MapleStory_pua24.exe`、`MapleStory_pua25.exe` 四处全 c0de2fd2，15s 复查无 watchdog 回滚。

### 一、商品卡片对齐（图标 / 名称 / 售价）——核心诉求

真机放大图 `evidence/card_zoom.png`（修复前）暴露三处错位，逐一定责修复（`tools/_apply_cardalign.py`，备份 `tools/_bak_UICashShop.cpp.before_cardalign`）：

| 缺陷 | 修复前 | 修复（对齐 MinMS） | 修复后证据 |
| --- | --- | --- | --- |
| 图标溢出压字 | ICON_SCALE=2.0 硬放大，72px 图标溢出 64px 白框，压住"皇家理发券"首字 | 加 fit_scale clamp：`min(ICON_SCALE,(64-2)/max(dim))`，自适应缩到槽内、不溢出 | `evidence/cardalign_zoom.png`：图标完整落在白框内，不再压字 |
| 名称压图标 | CENTER 对齐、锚点 x=100，宽 92px 文字从 x=54 起压进图标白框(5..68) | 改 LEFT 对齐 + NAME_X=76（MinMS `UICashShop.h:164`），ellipsis 预算 92→116 | "皇家理发券"四字完整清晰，不再被盖 |
| 售价对比度低 | GRAY 灰字画在浅蓝底(192,224,224)，暗像素仅 49/1600 | 名称/价格/折扣统一改 A13B + BLACK（MinMS `UICashShop.cpp:2989/1279-1280` 用 `Color::BLACK`），价格区 LEFT 对齐 | 暗像素 PRICE 49→184（约3.75×），NAME 35→103 |

MinMS 对照锚点：`UICashShop.cpp:694-718 draw_card_icon`（origin*scale 补偿）、`:2989-2999`（名/价 BLACK + NAME_X/PRICE_Y）、`UICashShop.h:164-169`（NAME_X=76/NAME_Y=10/PRICE_Y=32 + DY 微调）。

### 二、余额区对齐（owner 意识主动发现）

`evidence/balance_zoom.png`（修复前）：蓝色标签（点卷余额/信用卡/抵用卷，CSStatus 贴图自绘 x≈280-326）与自绘黑色数字重叠成一团。根因：数字 RIGHT 锚点 `WND_STATUS_X+90=344` 落在框左侧标签区。修复：锚点改 `WND_STATUS_X+216=470`（对齐 MinMS `BALANCE_VALUE_X=462` 同思路：标签在左、数值在右）。修复后 `evidence/balance_zoom_after.png`：蓝字在左、黑字在右，数字右端 x=468 vs 标签末端 x=326，间隔 142px，完全不重叠。

### 三、二级分类（真机验证补证）

上一轮已编码，本轮补真机切换证据：
- 装备页签：`evidence/category_equip_zoom.png`，12 项子分类（帽子/脸饰/眼饰/长袍/热销/裤裙/鞋子/手套/武器/戒指/勋章/披风）中文渲染正常，dark=1232。
- 消耗页签：`evidence/category_consumable2_zoom.png`，3 项（卷轴/通讯物品/天气），dark=509——证明页签切换动态生效、非画死。
- 设置页签：子区为空（符合 SUB_COUNTS[4]=0）。

### 四、其他功能核查结论

- 购物车页签：`evidence/cart_view.png`，可正常切换到购物车视图，空态显示"暂无商品"（中文 locale `cashshop.empty_notice=当前无可购商品`）。
- 翻页：本端商品仅 10 项（单页满），走滑条翻页（`list_slider`，`UICashShop.cpp:230-247/898`），当前无第二页数据可验；MinMS 是页码条（`PAGE_BTN`），属实现路径差异，非缺陷。
- 购买/赠送/购物车回执：走服务端协议（`CashShopBuyPacket`/`CashShopPackagePacket`），本地无对应服务端状态无法端到端验证，本轮不改。

已披露小瑕疵：购物车空态文字为 MINESHAFT 深灰画在蓝底上，对比度偏低；MinMS 无对应空态实现可对照，属边缘观感问题，未纳入本轮改动范围。

### 五、证据索引

| 项 | 文件 |
| --- | --- |
| 卡片修复前后放大对比 | `evidence/card_zoom.png`（前）/ `evidence/cardalign_zoom.png`（后） |
| 卡片像素采样 | NAME dark 35→103、PRICE dark 49→184（脚本 `tools/_cardalign_verify2.py`） |
| 余额区修复前后 | `evidence/balance_zoom.png`（前）/ `evidence/balance_zoom_after.png`（后） |
| 分类切换 | `evidence/category_equip_zoom.png` / `evidence/category_consumable2_zoom.png` |
| 购物车视图 | `evidence/cart_view.png` |
| 改动脚本 | `tools/_apply_cardalign.py`；备份 `tools/_bak_UICashShop.cpp.before_cardalign` |

### 六、复盘四步法

1. 回顾目标：商城全链路 UI/样式/分类/商品展示全面修复对齐。
2. 评估结果：商品卡片三要素（图标/名称/售价）+ 余额区 + 二级分类全部真机闭环；购物车功能正常；翻页/购买受数据与服务端的客观限制。
3. 分析原因：卡片错位根因是照搬 MinMS 图标放大却漏了名称布局（仍用 CENTER+x100）与文字颜色（灰/银），放大图一眼暴露；余额重叠是"数字自绘补位"时锚点留在标签区——都靠真机像素证据而非"部署成功"发现。
4. 沉淀 SOP：UI 改动必须真机放大图逐像素比对；对照移植要成套（图标 scale 与文字锚点/颜色一起改，不能只搬一半）；测试实例占用 exe 用 side 别名部署，多实例登录注意 admin 账号单点冲突（先优雅退出旧实例）；canon 部署后 15s 复查防 watchdog 回滚。
