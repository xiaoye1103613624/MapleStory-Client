# cashshop-window 扩展商店 · 现状研判（2026-11-21，点到面逐行号）

## 结论先行
**没做，功能/UI 全都没闭环。** 当前项目里这个扩展**一行实现都没有**：客户端走的是原生 stage 商城、opcode 0x3730 被 LIMIT_BREAK_SYNC 占用、服务端无 CashShopWindow 任何文件、live CashShop.img 未注入 plate/button。仅有的是 `w12-cashshop.md` 里"无新增差异"的历史研判记录（那是 Angel 原生商城校准，不是本扩展）。

## 参考包定义（E:\MXD\扩展改动\cashshop-window）
一套独立于原生 stage 商城的"覆盖地图窗口商城"（README:1-30）：
- 不切 stage（保持在地图、保留 buff/交互），CWnd 画在 field 上
- 服务端文件目录 catalogue（`data/catalog.tsv` 1845 行 / 1843 商品），一tab一tab按需发，无行数上限、serial 不编码分类
- **活体角色预览**：走路(真实动量 Map.wz/Physics.img)/跳跃/攻击/表情/现金特效/**爬梯子**（唯一能看披风/帽子背面）；3 张背景可切
- **购物车**：双击商品 pin 到预览+购物车，一次事务批量买（校验→支付能力→累计预留背包→发货）
- 搜索、焦点灯（PREVIEW 旁，绿=窗口拥有键盘/红=没有）

## 规模（真工程量，非 drop-in）
| 部分 | 文件 | 规模 |
|---|---|---|
| 客户端 | `client/cashshopwnd.cpp/.h` | **3468 行**，C++17，继承 `wvs/wnd.h::CWnd` override 10 虚方法 |
| 服务端 | `CashShopCatalog.java`/`CashShopWindowPackets.java`/`CashShopWindowPurchase.java`/`CashShopWindowHandler.java` | 221+170+266+122 = **779 行** |
| 数据 | `data/catalog.tsv` | 1846 行（1843 商品） |
| 美术 | `art/backgrnd.png` 760×495 + `wz/UI/CashShop.img`（WndBg + BtBuy/BtCart 四态） | plate+按钮 |
| 集成 | `INTEGRATION.md` | 6 项易静默失败的改动（收包路由/每帧tick/opcode/进商城改窗口/运行期目录/plate安装） |

## 当前项目状态（逐行号）
| 维度 | 现状 | 证据 |
|---|---|---|
| 客户端实现 | ❌ 无 | `IO/UITypes/UICashShop.cpp` 全是**原生 stage 商城**：进商城 `:1538-1547 stage.load/transfer`、离开 `:634 Stage::clear`、商品源 `:1560 UIStateCashShop::cashshop_commodities`（即 Etc/Commodity），无 WndBg/cart/search/爬梯预览 |
| opcode | ❌ 冲突 | 参考要 `CASHSHOP_WINDOW_ACTION/SYNC = 0x3730/0x3731`（INTEGRATION §2）；**当前 0x3730 = LIMIT_BREAK_SYNC**（`Net/PacketSwitch.cpp:420`），0x3731 空闲。需另选空闲对（如 0x3732/0x3733）并两侧同步 |
| 服务端 | ❌ 无 | `find BeiDou-Server_s9 -iname "*CashShopWindow*"` = 0；`grep EnterCashShop` in handlers = 0 |
| 运行期目录 | ❌ 无 | 无 `cashshop/catalog.tsv`（INTEGRATION §4 要 `<server working dir>/cashshop/catalog.tsv`） |
| live 资源 | ❌ 未注入 | `E:\MXD\BeiDou-Client-xy\Data\UI\CashShop.img` 1144984B / 8月4日，无 `Base/WndBg`/`BtBuy`/`BtCart`（plate 未装，INTEGRATION §5） |
| 文档研判 | ✅ 有但不同物 | `docs/features/angel-full-replicate-plan-2026-11/notes/w12-cashshop.md` 记"无新增差异（校准已闭环）"——指 Angel 原生商城协议校准，不是本扩展窗口商城；`dsh-badge` 无 |

## 移植性质与难点（诚实）
参考包明说（README:35-56）：客户端不是 drop-in——`cashshopwnd.cpp` include 9 个宿主 DLL 框架头（pch/hook/debug、wvs/wnd、wvs/wndman、wvs/packet、wvs/iteminfo、wvs/util、ztl），`CUICashShop` 继承宿主 `CWnd` 并 override 10 虚方法；**本客户端是 Journey-native 架构（UIDragElement/UIElement）不是那套 CWnd 框架**——必须重写 UI 层，不能直接编译。服务端 4 个 Java 文件也基于 Cosmic，需适配 s9（ByteBufOutPacket/InPacket/主包结构）。地址表针对标准 v83 exe（image base 0x400000），本端 exe 不同。
工作量估计：**4 千+行客户端重写 + 779 行服务端适配 + 资源转 key 注入 + opcode 重新分配 + catalog 部署**——是完整特性开发，不是"导入一下"。

## 建议（若用户要做）
按 USER.md 门禁 18（REA→IDA→orange-wz）：先把参考包 cashshopwnd.cpp 的窗口状态机/包体/资源节点逐条翻译成 Journey-native `UICashShopWindow`（或复用现有 UICashShop 骨架但去掉 stage 切换），服务端适配 s9 四文件，选空闲 opcode 对，catalog.tsv 部署到服务端运行目录，CashShop.img 的 WndBg/BtBuy/BtCart 走 orange-wz batch_convert_key 注入 live，最后真机验收（开窗/关窗/预览/购物车/购买/搜索）。

## 本轮动作
仅研判定性，未改任何代码/资源。待用户决定是否启动该特性开发。
