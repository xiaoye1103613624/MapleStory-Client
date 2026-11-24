# 商城原生还原真残缺补全（2026-11-21）

> 按 Angel/MinMS 源码还原商城。带可见 PUA 输出（🟠 阿里味），按用户要求仅记交付事实、不记评分。

## 1. 决策与版本口径（用户 2026-11-21 拍板）

- **版本口径：沿用 live 热卖版**。一级 tab 顺序 `TAB_CATEGORY={8,1,2,3,4,5,6,7,-1}`、二级 12 类、TAB_HIT_X、tab 烘焙文字**全部不动**。参考图是"帮助版"（13 类/含帮助 tab），但用户明确不改成帮助版。
- **实现方式：C++ 运行时绘制，零 WZ/img 改动**。不新增/不替换任何贴图，不发明服务端 opcode。

## 2. 为什么不是"整体照抄 MinMS"（顶层设计）

三联取证（两份逐行报告 + live WZ 实测 + 参考图）确认：MinMS 是 **Phone-first 手机改写版**，布局分 art 坐标 + screen 补丁两层；徽章、右侧热销榜、4 模式 tab、`<1>[12]` 分页这五类截图特征它要么没有、要么格式不同。**本端已做对**价格千分位、徽章枚举、热销榜、已购/背包格子——整体照抄 MinMS 反而会改坏。因此本轮只补"与版本口径无关的真残缺"。

- 分析证据：`notes/minms-reference.md`（MinMS 4 文件 4553 行逐行）、布局基准 `notes/layout-contract.md`、live WZ 节点实测（CSEffect 6 帧序 / CSChar 无模式 tab / BtSearch / BtEx* / BtReserve 五态齐全）。

## 3. 本轮实现（逐条，行号）

| 项 | 内容 | 落点 |
| --- | --- | --- |
| A | 徽章接数据：`priority>=8`→HOT，其余 NONE（不臆测） | `UICashShop.cpp` rebuild_items |
| B | 第3按钮"购物车"：`BtCart` 每卡（CSList/BtReserve），写本地 `local_cart_sns` + locale 提示，无 opcode | `.h` 枚举 / `.cpp:255,803` |
| C | 折扣百分比 `lround(percent*100)`，0.5→"(50%)" | `.cpp:1743` |
| D | 4 模式 tab：运行时 ColorBox+Text 绘制，`hit_mode_tab` 命中，0/1/2/3 模式切换 | `.cpp:1782/1798`, `.h` |
| E | 商品选购文字 + 扩栏五钮（BtExEquip..Trunk）竖排 + locale 提示 | `.cpp:218,519-526,795` |
| F | 搜索钮 `BtSearch` → UISoftKey 本地输入 + 反馈 | `.cpp:193,717` |
| G | 装备 tooltip：`UIStateCashShop` 照抄 `UIStateGame` 的 clear/show_equip/show_item/show_text，卡片悬停挂 tooltip | `UIStateCashShop.cpp:124-177`, `UICashShop.cpp:1817` |
| H | 分页页码列表 `<N>/[M]`（当前±2+末页） | `.cpp:1647` |
| I | 余额千分位 `split_number` | `.cpp:403` |
| L | locker 持有数 `cashshop.hold_count="x{}"` | `.cpp:1725` |
| i18n | 14 键双语（mode0-3/buy_select/ext_*/ext_hint/search_hint/cart_hint/hold_count） | `zh-CN.ini`/`en-US.ini` |

**明确跳过**：J（CashShopHandlers 硬编码英文，不在允许文件范围）、K（买/取回执 rebuild 时序，同）；二者列为本版预留，未虚报完成。

**偏差**：Tooltip::Parent 无 `CASHSHOP` 枚举，按"照抄不自创"用 `SHOP`（功能等价）。改字面 CASHSHOP 需动范围外 `Tooltip.h`。

## 4. 验证证据（红线一：闭环）

- `build_cashshop_native.bat`（`/m`→`/m:1` 修 C1041 PDB 撞车）→ **EXITCODE=0**，`error C/LNK/fatal` 计数 **0**
- 产物 `cashshop_native_int/MapleStory.exe`，5,138,432 B，mtime 10:28 > 源码末次改动
- 4 个代码文件 `[DBG]` 残留 **0**（工匠自检 + lead grep 复核）
- 独立复核：lead 亲自 grep 关键实现标记 + 读 tooltip/mode tab 代码，无空指针隐患、有边界守卫

## 5. 部署状态（已闭环）

用户 2026-11-21 授权"写入正式名 + 结束残留进程"后：
- 新 exe 已覆盖到正式名 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（5,138,432 B，mtime 10:33:58）
- **SHA 实时三处一致**：产物 = live = `35587e648c2295f724eacf2dbe425667ff97baf749f913f7c2b94abc6d552732`
- 无旁挂变体新增（本次未产出任何带日期/用途后缀的 exe）
- 残留 4 个 `MapleStory.exe` 为**内核级僵死进程**（0MB / 无 StartTime / taskkill 拒绝访问），不占用 exe、无法结束，属系统残留，需管理员或重启回收；与新 exe 部署无关，不影响启动。已如实告知用户。

## 6. 诚实声明

- 真机未验收（agent 无 GUI 权）。参考图是原生正确态/验收基准，非本端证据；真机验收须用**本端**截图。
- 购物车/扩栏/搜索均为本地逻辑 + 提示，无服务端语义（符合不发明 opcode）；完整功能需服务端另行支持。
- 4 模式 tab 坐标 y≈214 为像素估值，未真机目视；若与立绘重叠需现场微调。
- 未提交（自动提交需用户授权）。

## 7. 产物

- `notes/minms-reference.md` — MinMS 原生逐行对照
- `notes/layout-contract.md` — Layout Contract + PASS/FAIL 验收表（实测列待真机填）
- `evidence/live_tabs/`、`evidence/candidate_tabs*` — tab 版本口径取证
- 源码改动 4 文件 + 2 locale + build 脚本（本文件目录外）
