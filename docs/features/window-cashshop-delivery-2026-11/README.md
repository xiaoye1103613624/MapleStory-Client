# 窗口内商城（路线 C）· 完整交付记录（2026-11-21）

## 用户拍板与需求
- 路线 C：商品脚本入库服务端 DB + 客户端读服务端商品 + Web 管理后台，不以本地 Commodity.img 为主
- 完整还原（网格/购物车/搜索/焦点灯/活体预览）
- 导入 WndBg 求视觉一致
- 主菜单"商城"入口共用配置开关 `ui.cashshop.window_mode`（**默认 false**）：true→窗口商城，false/缺配置→原生商城

## 四层交付

### 资源（已入 live，probe 通过）
- 参考 `cashshop-window/wz/UI/CashShop.img` 经 orange-wz batch_convert_key(GMS 同 key, verify) → copy WndBg(760×495)+BtBuy/BtCart 四态(76×18) 进 live `Data/UI/CashShop.img`
- probe：根节点完整，Preview/0..2(212×165) 原样保留；原子替换后 live SHA `5D2D1334…`（与 staging 产物一致）
- 备份：`_ext_import_staging/2026-11-21/cashshop/CashShop.live.bak`

### 服务端（从 S10 移植，方案 B 隔离降级桩，mvn compile BUILD SUCCESS）
- 23 个新文件：CashShopCatalog/CashShopWindowPackets/CashShopWindowPurchase(去伤害皮肤分支)/CashShopClickType/CashShopItemNames/CashShopTaxonomy/CashShopAssetCheck/ClientDataPath/DamageSkinCashItems/InventorySlotCashItems + CashShopWindowHandler(6 action) + WindowCashShopService(810行) + WindowCashShopController(21端点) + DO×3/Mapper×3/DTO×4
- 4 处接线：RecvOpcode `CASHSHOP_WINDOW_ACTION(0x3730)` / SendOpcode `CASHSHOP_WINDOW_SYNC(0x3731)` / PacketProcessor 注册 / ServerManager 启动 `CashShopCatalog.load()`
- DB 脚本（只落，**未执行 DDL**）：`V1.11.16__xy_cashshop_window.sql` 三表 + `V1.11.17__xy_cashshop_seed.sql` 3711 条种子 + `cashshop/catalog.tsv` 1846 行
- 降级桩（不影响商城主链路）：clientPath/asset/icon 端点缺失（Web 对应工具页会 404）；syncIcons/syncFromClientData 返回占位；seedMount/seedDefaults 跳过皮肤/XY 种子；S10 伤害皮肤/图标基础设施未移植（不改 Character/登录）

### Web（从 S10 移植，vite build ✓ 2118 modules）
- 页面：game/windowCashShop + client/windowCashShopSync（+ 传递依赖 clientPath/assetHub 等）
- API：windowCashShop.ts（21 端点）+ clientPath/icon/asset（对应服务端 404 的工具页）
- 路由/i18n 全补齐；`vue-tsc` 3 个错误在未改动的 file/index.vue（既有问题，与移植无关）
- **只构建，未部署 dist**（部署形态用户未定：dev Vite8787+API8686 双进程 vs prod 重建 dist）

### 客户端（Journey-native 自写，全量 Rebuild 通过）
- 配置：`Configuration.h` `WindowCashShopEnabled("ui.cashshop.window_mode","false")` 默认 false + PosWINDOWCASHSHOP
- 入口 helper `UI::open_cashshop_entry()`：true→`emplace<UIWindowCashShop>`（GAME 态弹窗不切 stage），false→EnterCashShopPacket（原生）；三处入口（UIStatusBar BT_CASHSHOP/BT_CLASSIC_SHOP、UIChatBar /shop）统一调用
- 协议：OutPacket `CASHSHOP_WINDOW_ACTION=14128`(0x3730) 6 action 发包类；PacketSwitch `CASHSHOP_WINDOW_SYNC=14129`(0x3731) + handler；WaveAShellHandlers 解析 7 resp + 双线程队列（收包线程只入队，主线程 pump/apply）
- UI：`UIWindowCashShop`(760×495 UIDragElement)：6×4 网格+图标/名/价+tooltip、分类 tab(INDEX/TAXONOMY)、本地 ASCII 搜索、购物车(pin≤20/BUY_CART/retired)、焦点灯、活体预览(复用 UICashShop 沙盒：Player/Physics/Ladder/三背景/试穿/现金特效)、余额/购买码 i18n、ESC 关闭、拖拽记忆
- Type 枚举 `WINDOWCASHSHOP` + ESC 白名单 + vcxproj/filters + locale ~30 条 windowcashshop.*
- 修 4 个编译错误：send_cursor 返回 void 非 Cursor::State（关键）、UI.cpp 缺 GameplayPackets.h、Animation::draw alpha 参数位、Textfield::update 无参

## 构建与部署（三证 + 铁律）
- 全量 Rebuild：`BUILD_EXITCODE=0`、链接行、error=0、产物 5303296B
- 按铁律部署 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`，实时核对 live SHA = 产物 = **`058359544C1660AE916A669C437B394431D48771976D70011E3546B8000B2097`**（20:19:57）
- 服务端 `mvn -pl gms-server compile` = BUILD SUCCESS（未部署到运行服）

## 真机验收清单（待用户，逐项截图须本端）
**前置**：①服务端重启加载新 handler/Controller/Catalog ②手工执行 V1.11.16/17 SQL（AI 未执行 DDL）③运行期 `cashshop/catalog.tsv` 就位 ④Web 前端按选定形态 dev 启动或重建 dist。
1. 默认开关缺配置/false：主菜单"商城"→原生商城（stage 切换），行为与改动前一致（零回归）
2. 配置 `ui.cashshop.window_mode=true`：主菜单"商城"→窗口商城在地图上弹出（GAME 态，不切 stage，角色/buff 保留）
3. 开窗：WndBg 760×495 底板、Buy/BuyCart 四态按钮、6×4 网格商品图标/名/价、分类 tab、搜索框、预览窗
4. 交互：分类切换拉商品、搜索过滤、双击 pin 购物车、Buy/BuyCart 发包、余额刷新、购买码提示
5. 预览：试穿/走动/跳跃/爬梯/三背景切换/焦点灯绿红
6. 关闭：右上角关闭 + ESC 均关闭且焦点归还；拖拽位置记忆
7. 换图：窗口内切图自动关闭（参考包语义）
8. Web：/game/windowCashShop 商品 CRUD/分类/导入 TSV/热重载（clientPath/assetHub 工具页 404 属已知降级）

## 诚实边界（不藏）
1. **真机 8 项全部未验收**：构建过≠显示对/发包对，必须真机逐项复核。
2. **服务端未部署**：只编译通过；需你重启服务端 + 执行 SQL（DDL 只落脚本，按你规矩不执行）。
3. **Web 未部署 dist**：前端部署形态（dev 双进程 vs prod）未定，加页后需 `yarn build` 才生效。
4. 降级桩：伤害皮肤解锁/图标同步/客户端 Data 扫描在 s9 不可用（不属本需求，保留 TODO）；Web 的 clientPath/assetHub 工具页会 404。
5. 搜索为本地 ASCII 子串（协议无搜索 action）；滚动条用本端 Slider 替代 VScr4。
6. opcode 0x3730/0x3731：Recv 0x3730 空闲可用、Send 0x3730 被 LIMIT_BREAK_SYNC 占用故用 0x3731，与服务端一致。

## 证据路径
- 可行性/方案：`docs/features/cashshop-window-feasibility-2026-11/FEASIBILITY.md`
- 客户端实现：`docs/features/window-cashshop/README.md`
- DB：`BeiDou-Server_s9/gms-server/src/main/resources/db/migration/V1.11.16/17__*.sql`
- 回滚：`MapleStory.exe` 可由上一版备份恢复；`CashShop.live.bak`
