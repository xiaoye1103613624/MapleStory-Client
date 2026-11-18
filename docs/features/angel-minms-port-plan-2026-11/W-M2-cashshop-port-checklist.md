# W-M2 商城完善 · 可执行移植清单（MinMS → 本端）

> 触发：用户 m02051「拆，现在商品展示和角色逻辑都有问题，购买还没试」。
> 定位：把 angel-minms-port-plan-2026-11/README.md §4.1 W-M2 拆成逐项可执行的「算法/坐标/状态机」对照表。
> 纪律：抄算法 + 布局常量 + 状态机，在本端既有类重写；禁 jrc 命名空间、禁 MinMS .img 资源假设（本端 NX + UiAssetResolve）、禁改 s9 包体/opcode（L-B 裁决）；每项落地前走 REA→IDA→orange-wz 锚点。
> 状态：全部为待执行；本清单是盘点，不是实施记录。

---

## 0. 三痛点现状诊断（源码实证，非体感）

| 痛点 | 本端现状（证据） | MinMS 更深实现（证据） | 结论 |
| --- | --- | --- | --- |
| 商品展示 | UICashShop.cpp:1094 rebuild_items 仅 9 主类页签（UICashShop.h:88 TAB_CATEGORY[9]）、单页 2×5=10 槽、list_offset+slider；无装备子分类、无独立翻页页条 | UICashShop.h:75-100 含 BT_SUB0..72 装备子页签、BT_PAGE_PREV/NEXT；CommodityData.h:74-86 九主类 + category_for_item()；UICashShop.cpp:636 draw_category_sub_tabs | 能显示（已真机验收），但分类深度/翻页交互浅 |
| 角色逻辑 | UICashShop.cpp:249-251 构造时拷贝一次 avatar_look；update():369 无 avatar_look.update 动画驱动（静态单帧）；无试穿/还原/脱下 | UICashShop.cpp:1704 init_preview_looks 重应用 EQUIPPED 全槽(1714-1741)；1748 init_preview_sandbox 物理地板+梯子+respawn；3343-3358 试穿三态 | 立绘能显示但不会动、不能试穿——这是角色逻辑有问题的实质 |
| 购买 | UICashShop.cpp:752-780 UIYesNo 二选一 + 写死 CashShopBuyPacket(1, sn)（useNX=1）；无支付类型选择、无 pending、无成功回执清购物车；未真机验证 | UICashShop.cpp:2630 request_buy → UICashBuyConfirm（三支付选项，UICashBuyConfirm.cpp:318 BT_OK 回调 selected_cash_type()）→ 2663 dispatch_buy 设 pending_buy_sn → 1632 confirm_purchase_success 成功后清购物车 | 购买链缺支付选择+回执状态机，且尚未点过 |

---

## 1. 购买链（P0，用户明确「购买还没试」）

| # | MinMS 锚点 | 本端落点 | 移植内容（算法/状态机） | s9 约束 | 验证法 |
| --- | --- | --- | --- | --- | --- |
| B0 | 真机基线（前置动作） | 当前已部署 exe | 先点一次购买，抓 packet_dump.log 记 action/useNX/sn 与回执，作 B1-B5 前后对照 | xy-config.yaml:63 debug.packet_dump=false 需临时开，验证后还原 | 抓到 action 0x03 + useNX=1 + sn 的完整收发 |
| B1 | UICashBuyConfirm.h:22-30 构造(item_id,price,nx,maple,prepaid,preferred,ok,cancel) | 新增 IO/UITypes/UICashBuyConfirm.h/.cpp（glob 确认本端 0 命中） | 弹确认窗 + 三支付选项（点券/信用卡/抵用券），UICashBuyConfirm.cpp:318-347 BT_OK 回调 cash_type | 支付类型值取 CashShopOperationPacket 常量；本端 buy 包 CashShopPackets.h:33-39 第 3 字段为 useNX(int) | 真机点购买→确认窗→选支付→抓包核 action 与 useNX |
| B2 | UICashShop.cpp:2649/1655-1666 preferred_cash_type | UICashShop.cpp 增同名方法 | 按余额优先选默认支付：nx>0→NX，maple>0→MAPLE，否则 PREPAID | 余额来自 s9 showCash 三币种 int（本端 handler:179-191 已接） | 改余额再购，确认默认选中项随余额变 |
| B3 | UICashShop.cpp:2663-2679 dispatch_buy + 1632-1647 confirm_purchase_success/clear_pending_buy | UICashShop.cpp:775 当前直接 dispatch | 引入 pending_buy_sn：发购买包后置位；CashShopOperationHandler 收成功回执调 confirm_purchase_success 清本地购物车，失败调 clear_pending_buy | 不新增 opcode，复用 CASHSHOP_OPERATION(229)；成功/失败码见 handler:76-88 | 买成功→购物车项消失；失败→pending 清空且提示正确文案 |
| B4 | UICashShop.cpp:2640 row_purchase_block_message + 1415 update_action_button_states | 本端 update_items():1149-1154 已有 buyable 粗判 | 补全阻断原因（余额/年龄/性别/已购/库存），按钮态与提示文案对齐 | 失败码表已在 handler:76-88，复用 Locale key | 逐码构造数据核按钮禁用+提示 |
| B5 | UICashShop.cpp:2671-2678 礼包走 CashShopBuyPackagePacket(action 0x1E) | 本端已有 CashShopPackagePacket（CashShopPackets.h:97-107） | 只把 B1 确认窗接到礼包分支，验证 is_package 分流 | 本端 action 0x1E 字节序已按 s9 写好，不改 | 真机点礼包核 action=0x1E |

---

## 2. 角色逻辑（P1，「角色逻辑有问题」的实质）

| # | MinMS 锚点 | 本端落点 | 移植内容 | s9 约束 | 验证法 |
| --- | --- | --- | --- | --- | --- |
| C1 | UICashShop.cpp:1707-1741 init_preview_looks 重应用 EQUIPPED 全槽（含 cash 层） | UICashShop.cpp:246-256 仅一次拷贝 | 构造后按 Inventory.get_item_id(EQUIPPED, slot/cash_key) 重应用装备，避开 live look 动画中态 | 本端 EquipSlot 枚举==wire 绝对值不变量；to_cash_equipped 换算照搬 MinMS | 换装后进商城，立绘装备与角色一致 |
| C2 | UICashShop.cpp:1748-1769 init_preview_sandbox 地板/梯子/respawn | 本端无物理沙盒 | 可选架构升级；P1 先做 C3 动画驱动，物理沙盒后置 | 不引入 MinMS Physics 依赖进本端主循环 | 立绘有呼吸/待机帧变化（由 C3 验收） |
| C3 | MinMS preview_actor 在 update():3123-3169 持续更新 | UICashShop.cpp:369 update() 缺 avatar_look.update（本次 grep 复核：250/251/271 之外零命中） | update() 内调 avatar_look.update()（签名 CharLook.cpp:65/195），推进 STAND1 动画帧 | 纯客户端渲染，不碰包体 | 真机连续截图两帧立绘像素有差异，非静态 |
| C4 | UICashShop.cpp:3343-3358 BT_BUY_AVATAR/DEFAULT/TAKEOFF 试穿三态 | UICashShop.h:174-178 有枚举但 button_pressed 无分支 | 实现试穿：点卡片临时 add_equip(itemid)→立绘变装；DEFAULT 还原 baseline；TAKEOFF 脱下 | 试穿只改本地 look，不发购买包；真购买走 B 链 | 点装备商品→立绘变装→还原/脱下复原 |
| C5 | LabelNameTag（UICashShop.h:16） | 本端 name_label/job_label（UICashShop.cpp:279-283） | 官方预览区实时名牌；本端顶栏 label 已有，如需贴脚名牌再参照 | 纯 UI，无包体 | 截图对照官方 商城.png 预览区 |

---

## 3. 商品展示（P2，已显示，补深度）

| # | MinMS 锚点 | 本端落点 | 移植内容 | s9 约束 | 验证法 |
| --- | --- | --- | --- | --- | --- |
| S1 | CommodityData.h:74-89 category_for_item | UICashShop.cpp:1136 当前以 c.sn/10000000 判类 | 用 itemid+is_package 分类替代 SN 首位假设，覆盖非标准 SN | 分类 id 与 s9 CategoryType 一致（首页=8 特例保留） | 造非标准 SN 数据核归类 |
| S2 | UICashShop.h:80-81 BT_SUB0..72 + UICashShop.cpp:636 draw_category_sub_tabs | UICashShop.h:88 仅主类 9 页签 | 加装备子页签（EQSUB），状态机 category_sub + rebuild_shop_rows 过滤 | 子页签纯本地过滤，不发 opcode | 真机切子页核商品过滤 |
| S3 | UICashShop.h:151-160 PAGE_BAR/PAGE_BTN + change_list_page(3359-3369) | 本端 slider+list_offset 已能翻页，无页条 UI | 可选：加页码条（BT_PAGE_PREV/NEXT），与 slider 同步 | 不改包体 | 真机点页条核翻页 |
| S4 | CashShopParser.cpp:52-64 PRIORITY 先于 PRICE 读（Cosmic 排序坑） | 本端 CashShopParser 已同源实现（handler:92 注释） | 已闭环，仅回归：确认 modified 列表解析顺序一致 | s9 CommodityFlag.sort 契约 | 造打折商品核价格不回退 WZ 零售价 |

---

## 4. 执行顺序与验收门

1. B0 真机购买基线（先点一次抓包） → 2. B1-B5 购买链（最高优先） → 3. C3/C1 立绘动画+装备重应用 → 4. C4 试穿三态 → 5. S1/S2 商品分类深度 → 6. S3/S4 可选与回归。

每批固定闭环（README:135）：证据(REA→IDA→orange-wz) → 实现 → gen_opcode_matrix.py --check → build_smoke.bat → 自测证据 → 原子提交。

构建纪律（README:129 / MEMORY 强约束）：二进制脚本改源码不改 mtime → MSBuild 假通过；验证前必须 touch 源文件 + rm 对应 x64/obj-Release/文件.obj，核对 obj mtime > cpp mtime 且 exe SHA 变化。MSBuild 路径：D:/software/Microsoft Visual Studio/18/Community/MSBuild/Current/Bin/MSBuild.exe。

真机进商城 SOP（血泪沉淀）：auto login 走 xy-config.yaml:55（login.auto=true/account=admin/pass=admin）；登录弹密码错误先查 accounts.loggedin（残留=2 会返回 loginok=7，置 0 后重启客户端）；进商城点底部商城按钮 client(584,707) 或 /shop；截图 tools/capture/capture-shot.ps1 -Target MapleStory -OutFile ...。

---

## 5. 红线与不移植

- 不改 s9 包体宽度/opcode（L-B 裁决）；购买 action 0x03、礼包 0x1E 字节序照旧。
- 不整文件粘贴 MinMS；jrc 命名空间 / MinMS .img 资源假设 / VirtualPad / maple_engine 不迁。
- 社交/公会不回迁（MinMS 是 stub，弱于本端）。
- 每个移植项落地前 REA→IDA→orange-wz 校准锚点；Angel IDA EA 表（feature-gap-inventory §17 CCashShop 0x468f3e..0x47c75e 共 66 方法）作索引。

---

## 附录 A：多分辨率现状（源码实证，2026-11-16）

结论先行：商城**没有多分辨率自适应**，采用「进店强制钉死逻辑视口 800×600」策略——靠全局 resize 物理窗口去适配，而非 UI 内部缩放映射。不是「不支持」，是「只支持一个被钉死的分辨率」。

证据链：
1. 进店锁定：`Net/Handlers/CashShopHandlers.cpp:802-803` `set_viewwidth(800); set_viewheight(600)`（:800 注释明说 Angel CCashShop 画布=800×600）；退出才在 `UICashShop.cpp:393-394` 恢复 `Setting<Width/Height>`。
2. UI 全绝对坐标、无缩放层：本端 `UICashShop.h:66-82` 全是固定常量（WND_LIST_X=275 / WND_LIST_Y=95 / PLATE_DX=206…），`UICashShop.cpp` 无 `update_screen` 重写、无 `sc()` 映射。对照 MinMS `UICashShop.cpp:1169 update_screen()` + `:295 sc(wz)=wz*scale` + `:300-409 list_sx/list_sy/side_sx/side_sy`——MinMS 做完整逻辑→屏幕缩放，本端一层没有。
3. 渲染链物理窗口=逻辑视口，无 letterbox：`Window.cpp:497 SDL_CreateWindow(width,height)`、`:527 SDL_SetWindowSize`、`:536 glViewport(0,0,width,height)`；`GraphicsGL.cpp:548 glUniform2f(uniform_screensize, VWIDTH, VHEIGHT)`。全局无等比缩放/黑边层——窗口多大逻辑画布就多大。
4. 窗口不可拖：`Window.cpp:833-840` 注释明确 `SDL_CreateWindow` 未加 `SDL_WINDOW_RESIZABLE`，分辨率档位只能走设置切换（UIOptionMenu/UIQuit）。

后果：
- 不论玩家原窗口 1280×720/1920×1080，进商城即跳 800×600 固定小窗、UI 铺满，退出跳回。
- 每次进出触发 `GraphicsGL::reinit` 的 `size_changed`→`clearinternal()`（:535/:562）重建纹理图集，有一次性性能抖动；历史上有 reinit 时机导致黑屏的 bug（:530-534 注释）。
- 退出路径若异常中断，视口可能残留 800×600。

要「真多分辨率」的正解（新增 M 组，待拍板）：
- M1 给 UICashShop 加 `update_screen` + `sc()/list_sx/side_sy` 映射层，把 800×600 WZ 美术等比缩放到任意视口。
- M2 选定 letterbox（保比例留黑边）或 stretch（铺满变形）。
- M3 去掉进店强制 resize，改由 UI 层适配，消除 atlas 重建抖动。
- 验收：1024×768 / 1280×720 / 1920×1080 三档进店，UI 不越界、坐标命中正确、退出视口正确恢复。
