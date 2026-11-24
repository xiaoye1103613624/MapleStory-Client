# 窗口内商城（cashshop-window）详细可行性与实施方案（2026-11-21）

> 三路逐行号深挖：参考客户端 3468 行、服务端 4 Java 779 行、本端 Journey-native 全链路。证据落盘于本目录及子代理报告。本文给"详细且深入"的结论，不改代码。

## 0. 一句话结论
**可做，可行性中高。** 服务端几乎可 1:1 移植（约 4 人日，95% API 现成）；客户端不是 drop-in（CWnd/CAvatar 架构异构，需按 Journey-native 重写 UI 层与预览层），但**本端已有一套原生商城的活体预览沙盒**（爬梯/跳跃/攻击/表情/试穿/现金特效），把最难的预览部分从"从零重写"降级为"复用+切换"。真正需要决策的是**服务端走哪条路**（新目录/购物车事务 vs 复用现有 Commodity.img）。

---

## 1. 参考包设计全貌（要还原什么）

### 1.1 核心差异（README:1-30）
- **不切 stage**：CWnd 覆盖在地图上，保留 buff/交互，开窗关窗零成本（对比原生商城切到独立 CASHSHOP stage）
- **服务端目录，无行数上限**：`catalog.tsv` 1843 商品，itemId 为主键（无 serial 编码分类），一 tab 一 tab 按需下发，突破原生 Commodity.img 的 serial→行上限
- **活体预览（playground）**：真实动量走/跳/攻/表情/现金特效/**爬梯**（唯一能看披风帽子背面的途径），物理常量来自 Map.wz/Physics.img
- **购物车**：双击 pin，一次事务批量买（校验→余额→累计预留空间→发货→扣款）
- **搜索**：客户端本地 ASCII 子串过滤；**焦点灯**：绿=窗口拥有键盘，红=没有

### 1.2 协议（双向独立 opcode，参考 0x3730/0x3731）
C→S action（cpp:73-76）：0 REQUEST_CATALOG / 1 BUY(+i32 itemId) / 2 BUY_CART(+u8 n,+n×i32) / 3 REQUEST_CATEGORY(+u8 tab,+u8 cat)
S→C resp（cpp:78-83）：0 OPEN(+3×i32 现金) / 1 CATALOG(+flag,tab,cat,u16 n,+n×记录) / 2 BUY(+code,i32) / 3 CASH(+3×i32) / 4 BUY_CART(+code,i32,u8 delivered,i32 spent) / 5 INDEX(+u16 n,+n×{u8 tab,u8 cat,i32 cnt})
关键机制：**开局只发 INDEX+CASH，商品按分类点击时按需拉取**（这是无上限的来源）；CHUNK=400 分包，空分类也发包（否则客户端 loading 不结束）；边界常量 chunk≤2048/category≤65536/name≤64，任一段 bad 整块丢弃。

### 1.3 客户端状态机（cpp:1459-1664, 2558-2641, 3323-3456）
- 状态字段：tab/cat/选中/滚动/拖拽/购物车(kCartMax=20)/焦点/playground 物理态/搜索串
- 双线程架构（**必须保留**）：接收线程 `HandleSync` 只解析+举原子旗（g_bWantOpen/g_bCatalogDirty/g_bCartBought），主线程 `Tick`+`Update` 才建窗/应用/驱动预览
- 11 个 CWnd override（源码实为 11+1 析构，README 说 10 不准）：Draw/OnMouseButton/Move/Wheel/Enter/Destroy/Update/RTTI/IsKindOf/SetFocus/OnKey
- 焦点灯机制：OnSetFocus return 1 抢占键盘，真实角色冻结；关闭时必须 RestoreFocusToWndMan，否则玩家冻结

### 1.4 物理/爬梯数值（cpp:415-439, 723-745）
walk 125px/s、jump -555、gravity 2000、fallmax 670、mass=100（walkForce140000/100=1400 accel、drag800）；爬梯 62px/s、梯子 X 区间按三背景分别测量。**这些是数据，不是地址，可原样保留。**

---

## 2. 本端能力映射矩阵（决定可行性）

| 参考能力 | 本端现成接口（行号） | 可行性 |
|---|---|---|
| 窗口基类/拖拽 | UIDragElement\<T\>(IO/UIDragElement.h:28-133) + UIElement 虚体系 | ✅ 直接替代 CWnd |
| 商品目录 | load_local_commodities(CashShopHandlers.cpp:139-176) + 服务端覆盖(:763-792)，live 2010 在售 | ✅ 现成（但受 Commodity.img 上限约束） |
| 活体预览 draw | Char::draw_preview(Char.cpp:110)、CharLook::draw/set_hair/set_face/add_equip/attack/set_stance/set_expression/set_action(CharLook.h:39-55) | ✅ 现成 |
| 走/跳/攻/表情 | Player::send_action/attack/set_expression/set_stance、Char::State 枚举 | ✅ 现成 |
| **爬梯预览** | Stance::Id::LADDER(Stance.h:36) + Ladder + Player::set_ladder(Player.cpp:815)；**本端原生商城沙盒已完整实现**：init_preview_sandbox(UICashShop.cpp:1208-1246)、send_action(:989-1013)、apply_preview_controls(:1330-1371) | ✅✅ 本端已有等价物，最难项降级 |
| 试穿/现金特效/宠物 | apply_try_on_item(:115)、cash_item_effect_*(:106-110)、preview_pet(:1312-1328) | ✅ 现成 |
| 网格+滚动+图标+tooltip | UIShop.cpp:94-147/714-741（item_y124,row42,Slider）、ItemData::get_icon、UI::show_item | ✅ 骨架现成 |
| 按钮四态 | MapleButton(4态)/TwoSpriteButton(2态)/AreaButton；try_maple_btn(UICashShop.cpp:109) | ✅ 现成 |
| 发包/收包 | CashShopPackets.h 全类 + CASHSHOP_OPERATION(229)；PacketSwitch emplace 模式(:796-798) | ✅ 现成（新私有 opcode 需加） |
| i18n | Locale::text + 54 条 cashshop.* 词条 | ✅ 现成，加词条即可 |
| 资源 | BtBuy/backgrnd/Preview0..2/Tab1..9/BtReserve(=Cart) live 已有；**WndBg 独立节点/BtCart 字面节点缺** | ⚠️ 缺项有语义替代；要视觉一致需导 WndBg（转 key 硬门禁） |
| CUtilDlg Notice 弹窗 | 无 | ⚠️ 替代 UINotice/UIYesNo/UICashBuyConfirm(CSNotice/4) |
| CAvatar 爬梯物理 | 无该类 | ✅ 用本端 Physics+Ladder 替代（沙盒已验证） |
| 服务端事务/目录 | 无（要新写或复用） | ⚠️ **核心决策点，见 §3** |

---

## 3. 服务端两条路（核心决策点）

### 路线 A：完整移植参考包服务端（目录 TSV + 购物车事务 + 新 opcode）
- 4 Java 779 行 1:1 移植（包名改写），s9 全部关键 API 现成：CashShop.NX_CREDIT/getCash/gainCash、Inventory.checkSpots(chr,probes)(:458)、Character.canHoldUniques(:2251)、Inventory.isFull(margin)(:374)、IIProvider 四方法、InventoryManipulator.addFromDrop(:178)、Pet.createPet、PacketCreator.enableActions(:1108)
- 3 个小缺口：① `ItemConstants.EXPIRING_ITEMS` 常量不存在（加开关或改逻辑）② `CashShopCatalog.load()` 无启动接线（补一次调用）③ opcode 改选
- 工作量 ≈ 4 人日；风险：无整单事务回滚（阶段4异常会"免费送出已投递件"，参考实现刻意如此）
- 换来：**服务端目录无行数上限、购物车批量事务防双花、anti-cheat 校验完整**

### 路线 B：复用本端现有商城协议（Commodity.img + CASHSHOP_OPERATION，服务端零/极小改动）
- catalog 用现有 2010 在售（受 Etc/Commodity.img serial 上限约束，非无限）
- 购买复用 CashShopPackets.h 现有包类 + 服务端既有 action 语义，服务端**零改动**
- 客户端只做"stage 切换 → GAME 态弹窗"改造 + 新 UI + 复用预览沙盒
- 换来：工作量小很多（省 4 人日服务端）、零服务端回归风险；**失去"无行数上限/购物车批量事务"**

推荐判断：若用户要"和参考包一样完整"→ A；若"先要一个能开窗试穿购买的窗口商城、不追求无限目录"→ B。**这是必须用户拍板的方向。**

---

## 4. 客户端实施方案（两路线通用部分）

### 4.1 共存 + 开关（推荐，零兼容风险）
- 新增配置开关 `cashshop.window_mode`（Configuration.h 增项）
- 关：原 stage 商城路径完全不变
- 开：`SetCashShopHandler::handle` 保留 catalog/榜单解析(:700-803)，但跳过 :805-807 transition+change_state，改为 GAME 态 `UI::get().emplace<UIWindowCashShop>()` 弹窗
- **关键不可动**：Stage.cpp:82 `if(state==INACTIVE||mapid变化) load_map` 是全局黑屏修复，撤销会重新引入 exit black screen
- 退出只 deactivate 弹窗，不调 exit_cashshop(:1528) 的 PlayerLoginPacket+load+transfer

### 4.2 新窗 UI 类
`class UIWindowCashShop : public UIDragElement<PosCashShopWindow>`（Configuration.h 增 Pos 类型或复用 PosSHOP），继承 UIElement 虚体系，注册 Type 枚举 + UI.h/UI.cpp。
- 保留参考包的双线程旗机制（接收线程解析入队，Update 消费）
- 网格/滚动照 UIShop.cpp 骨架，图标用 ItemData::get_icon，tooltip 用 UI::show_item
- 四态按钮用 MapleButton + try_maple_btn 模式
- 文案走 Locale，加 cashshop_window.* 词条

### 4.3 预览层（复用本端沙盒，最大省力点）
把 UICashShop.cpp:1208-1371 的 preview_sandbox/init_preview_sandbox/send_action/apply_preview_controls 抽成可复用组件（或直接在新窗内重建同样逻辑），玩法数值照参考包 Physics.img 常量。爬梯几何按三背景实测（参考 kArtLadderX0/1）。现金特效复用 cash_item_effect_* 链。

### 4.4 资源门禁
WndBg 若从参考 wz 导入：走 orange-wz batch_convert_key(GMS,verify:true) + copy_paste 子树进 live CashShop.img（参考与 live 同 GMS 头，已在研判阶段 probe 确认），本端 --resource-probe 短启；禁止裸拷。BtCart 用现有 BtReserve 替代可不导。

---

## 5. 分阶段实施计划（一次成型，按依赖分组）

| 阶段 | 内容 | 路线A | 路线B |
|---|---|---|---|
| P0 决策 | 用户拍板路线 A/B + 范围（完整/核心闭环） | 必须 | 必须 |
| P1 服务端 | 4 Java 移植+接线+opcode / 或零改动 | 4 人日 | 0 |
| P2 客户端骨架 | 开关+共存+GAME 态弹窗+Type 注册+网格+按钮+tooltip+i18n | 共用，约 2-3 人日 | 同 |
| P3 预览层 | 复用沙盒+爬梯+试穿+现金特效+焦点灯 | 共用，约 1-2 人日 | 同 |
| P4 事务/购物车 | 购物车 UI+批量发包+结果处理 / 或复用现有单件购买 | A 另 1-2 人日 | B 复用 |
| P5 资源 | WndBg 转 key 注入（可选，视觉一致才要） | 0.5 人日 | 同 |
| P6 联调验收 | 全量构建+部署 live 正式 exe+真机（开窗/关窗/预览/购买/搜索/跨图自动关） | 1-2 人日 | 同 |
| 合计 | | ≈10-13 人日 | ≈5-7 人日 |

---

## 6. 风险与诚实边界
1. **真机未验收**：所有可行性基于源码逐行取证，不等于运行时正确；必须真机测开窗/关窗/焦点/购买/跨图自动关。
2. **stage 切换是雷区**：Stage.cpp:82 黑屏修复不可动；进店 load(-1) 与 800×600 视口必须条件化，否则地图被卸载。
3. **服务端 opcode 双向独立**：s9 惯例 C→S 奇/S→C 偶；参考 0x3730/3731 中 0x3730 已被 LIMIT_BREAK_SYNC 占用，建议改用 0x3721/0x3722 或 0x3731/0x3732，两侧 + 客户端 opcode 矩阵同步。
4. **路线 A 无事务回滚**：免费送出已投递件是参考实现刻意取舍，须接受。
5. **编码**：writeString 随客户端语言取字符集（CN=GBK），参考搜索是 ASCII-only，中文商品名会搜索不匹配。
6. **资源转 key**：跨 IV 禁裸拷，必须 batch_convert_key(verify:true)，否则 orange-wz 可读但本端堆损坏 0xc0000374（已有历史教训）。

## 7. 用户拍板（2026-11-21）：路线 C（商品房 + Web 管理）
用户自定义回复：**商品用脚本入库到服务端 DB，客户端读服务端商品，增加 Web 管理功能，不以本地 Commodity.img 为主**。范围=完整还原，WndBg=导入求视觉一致。这既不是 A（TSV 文件目录）也不是 B（本地 Commodity.img），是新路线 C（DB 权威 + Web 管理）。

### 7.1 决定性发现：S10 兄弟仓已把路线 C 整套做完（本人已亲自核实，非子代理转述）
`E:/project/BeiDou-Server_S10/` 已存在完整实现，s9 没有：
- 服务端：`CashShopCatalog.java`(125行)、`WindowCashShopService.java`(1769行)、`WindowCashShopController.java`(244行)、DTO×4、`db/migration/V1.11.7__xy_cashshop_window.sql`（`xy_cashshop_item`/`xy_cashshop_category`/`xy_cashshop_category_item` 三表）、`V1.11.9__xy_cashshop_seed.sql`（1843 条种子）、`cashshop/catalog.tsv`(1846行)。target/classes 有编译后 .class = 已验证可编译。
- Web：`gms-ui/src/views/game/windowCashShop/`（商品管理页）+ `views/client/windowCashShopSync/`（客户端同步页）+ `src/api/windowCashShop.ts`（getItems/getItemsGrouped/saveCategory/saveItem/link/unlink/reload/importTsv/refreshNamesFromWz 全套 API）。
- **但 S10 对应客户端（XyMapleStory-cms085）没有窗口商城客户端源码**（只有原生 UIStateCashShop）——客户端仍需自写。
- 结论：路线 C 从 S10 **移植**到 s9，不是从零开发；服务端/Web 工作量从"4+人日"降为"移植+适配"，风险大幅下降。

### 7.2 路线 C 落地方案（按层）
**DB 层**：从 S10 移植 `V1.11.7__xy_cashshop_window.sql` 三表（s9 当前最大迁移 V1.11.15，改编为 V1.11.16+）到 `db/migration/`；数据用 S10 `V1.11.9` 种子或 `scripts/import_cashshop_catalog.py` 读 catalog.tsv 入库（ON DUPLICATE KEY UPDATE 幂等）。**DDL 只落脚本，不由 AI 执行**（用户规矩）。
**后端层**：移植 `CashShopCatalog`（DB 快照：all/byItemId/byBucket/index 四个 volatile 不可变视图 + load() 可热重载）、`WindowCashShopService`（1769行，含 importFromTsv/CRUD/分类关联/图标同步）、`WindowCashShopController`（`/windowCashShop/v1/*` REST）、4 个 DTO、对应 DO/Mapper（MyBatis-Flex，s9 已有模式）。包名 org.gms.* 一致，MyBatis-Flex 1.8.9 一致，基本机械移植。
**Web 层**：移植 S10 gms-ui 的 `windowCashShop/` 页 + `windowCashShop.ts` API + 路由/菜单/i18n；s9 gms-ui 是同款 arco-design-pro-vue（Vue3+Vite3），组件/拦截器/`ResultBody{code:20000}` 全套现成。**注意**：前端加页必须 `yarn build` 重建 dist 并部署（当前 dist 构建于 11-18、未内嵌进 jar；dev 为 Vite8787 + API8686 双进程）——部署形态要先定（同源 static vs 独立静态服务）。
**客户端层**：不变——照 §4 做"共存+开关"GAME 态弹窗 + 复用预览沙盒 + 网格/购物车/搜索/焦点灯；商品源改为服务端下发（新增私有 opcode 对，与 S10 客户端协议约定；S10 服务端有 client sync DTO 可参考其数据结构，但具体 opcode/gms-server 给药客户端的同步格式需从 S10 DTO 字段反推——此为本路线唯一需新设计处，因为 S10 没有对应客户端实现可对表）。

### 7.3 路线 C 修订工作量
| 层 | 来源 | 估计 |
|---|---|---|
| DB | S10 三表+种子移植 | 0.5 人日 |
| 后端 | S10 Service/Controller/Catalog/DTO/DO/Mapper 机械移植+适配 | 1.5-2 人日 |
| Web | S10 页面/API/路由/i18n 移植 + build/部署 | 1 人日 |
| 客户端 | 按参考包+本端能力重写（含 DB 商品同步协议） | 4-5 人日 |
| 联调验收 | 构建+部署+真机 | 1-2 人日 |
| 合计 | | **≈8-10.5 人日**（比从零路线C 大幅下降） |

### 7.4 路线 C 新增风险
1. **客户端↔服务端商品同步协议无现成对表**：S10 服务端有 `WindowCashShopClientSyncReq/RtnDTO`，但没有 S10 客户端源码可对，需从 DTO 字段定义协议并让客户端按此实现——这是唯一"新设计"环节，必须在实现前先把 DTO 字段逐条读出定死（避免猜）。
2. **DB 语义冲突**：s9 现有 cashShop 管理页管按 sn 的 WZ 稀疏覆盖表 `modified_cash_item`，路线 C 商品表按 item_id；两套并存要清晰隔离（新页独立，不改旧页）。
3. **前端部署形态未定**：prod dist 是否嵌入 jar / 独立静态服务，上线前必须明确并重建。
4. 其余同 §6（真机未验收、stage 雷区、DDL 只落脚本、转 key 硬门禁、编码）。

## 8. 共用商城入口的配置开关（用户 2026-11-21 新增需求）
用户要求：**窗口商城与原生商城共用主功能菜单的"商城"入口**；增加一个配置位：配置存在且为 true → 打开窗口商城；配置不存在或为 false → 仍走原生商城。

### 8.1 入口链路（逐行号，三处共用同一发包）
| 入口 | 位置 | 现状 |
|---|---|---|
| 主菜单"商城"按钮（现代底栏） | `IO/UITypes/UIStatusBar.cpp:956-959 case BT_CASHSHOP` → `EnterCashShopPacket().dispatch()` | 共用点1 |
| 经典底栏商城按钮 | `UIStatusBar.cpp:949-953 case BT_CLASSIC_SHOP` → 同发包 | 共用点2 |
| 聊天命令 `/shop` `/cashshop` | `IO/UITypes/UIChatBar.cpp:2943-2951` → 同发包 | 共用点3 |
三处最终都走 `EnterCashShopPacket()`（opcode ENTER_CASHSHOP=40/0x28 → s9 EnterCashShopHandler）。**不改动服务端入口即可实现分流**：客户端发包前判配置即可。

### 8.2 配置机制（三态语义精确匹配现有 Setting<>）
- `Configuration.h:1170-1191 Setting<T>::get()`：配置存在→返回该 entry（:1181-1184）；**配置缺失→返回 `static T defaultentry`（:1187-1188）**。
- `Configuration.h:224-229 BoolEntry::save/load()`：bool 配置读写。现有范例 `Fullscreen() : BoolEntry("window.fullscreen","false")`（:482）、`SideIconsEnabled() : BoolEntry("ui.side_icons","true")`（:588）。读值统一 `Setting<X>::get().load()`（IO/Window.cpp:450、UI.cpp:216）。
- **三态实现**：新增 `struct WindowCashShopEnabled : Configuration::BoolEntry { WindowCashShopEnabled() : BoolEntry("ui.cashshop.window_mode","false") {} }`（默认值 **false** = 原生商城）。入口处：
  ```cpp
  if (Setting<WindowCashShopEnabled>::get().load()) {
      // 窗口商城：GAME 态 UI::get().emplace<UIWindowCashShop>()（不切 stage）
  } else {
      EnterCashShopPacket().dispatch();  // 原生商城，保持原样
  }
  ```
  - 配置存在且 true → 窗口商城 ✓
  - 配置存在且 false → `load()` 返回 false → 原生商城 ✓
  - 配置不存在 → `defaultentry` 默认 "false" → `load()` false → 原生商城 ✓
  **完全符合用户三态要求，无需新机制。** 关键：默认值必须是 "false"，绝不能 "true"。
- 分散在三处入口（UIStatusBar 两处 + UIChatBar 一处）的建议：抽一个统一 helper（如 `open_cashshop_entry()`）封装分流，避免三处重复判断导致不一致。

### 8.3 与路线 C 的关系
- 此开关与 §4.1"共存+开关"是同一设计的具体化：原生商城路径完全保留（关该开关即零回归），窗口商城是 GAME 态弹窗、商品读服务端 DB（路线 C）。
- 服务端 EnterCashShopHandler 无需改动（窗口模式根本不发 ENTER_CASHSHOP 包，走私有商品同步包）。

## 9. 最终待确认（实现前）
- **确认从 S10 移植路线 C**（本报告已按用户拍板默认如此，若否请纠正）。
- **确认前端部署形态**：dev 双进程（Vite8787+API8686）还是 prod 重建 dist 嵌入/独立托管。
- **客户端商品同步协议**：是否允许我先读 S10 的 4 个 ClientSync DTO 字段，据此定死协议再实现（不猜）。
- **配置键名确认**：默认提议 `ui.cashshop.window_mode`（默认 false）；若你有既定键名请告知。

以上确认后按 §7.2 + §8 分层一次成型实施。

