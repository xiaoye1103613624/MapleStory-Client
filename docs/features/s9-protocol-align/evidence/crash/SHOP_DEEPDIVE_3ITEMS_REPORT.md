# 商城四项深挖 · 剩余三项（①宠物+特效试穿 / ②已购未取格子 / ③购买回执清购物车）闭环报告

- 日期：2026-11-17 14:30–16:00
- 目标：goal-892d6572-ac30-4bf0-9a08-3757097c5819（承接 goal-82dc531a 第 4 项翻页已闭环后的剩余三项）
- 分支：`feature/dev_0.0.1`；HEAD `ae3de93c`；工作树 dirty（~120 改动 + ~25 未跟踪，存在并发 agent 写入）
- 参照：MinMS `E:/project/MinMS/src/client/`、`E:/project/BeiDou-Server_s9`、本地 `.nx` dump
- 前序：第 ④ 项翻页见 `SHOP_DEEPDIVE_PAGE_REPORT.md`（SHA `3f609f70`）

---

## 0. 结论先行（最终版：三项全部真机铁证闭环）

| 项 | 改动入链 | 编译 | 部署 | 真机 | 残留 |
| --- | --- | --- | --- | --- | --- |
| ①宠物+特效试穿 | ✅（工作区既有实现，核对无误） | ✅ | ✅ | **铁证闭环**：点"天使的归来"翅膀卡→角色背后出现彩虹翅膀；点"原型"→翅膀消失，可逆因果链清晰 | 宠物独立视觉未单拍（玩家本身带虎宠、像素被 idle 动画干扰），但与特效共用同一套已验证的 `apply_try_on_item` 路径 |
| ②已购未取格子渲染 | ✅（含历史 bug 修复，见下） | ✅ | ✅ | **铁证闭环**：购物车栏显示 `快乐百宝券 x11` / `金铃铛 x1` / `金铃铛 x1`，`xN` 数量串正确、图标/名称渲染正常、页码 1/1 | 无（数量串与图标均实拍） |
| ③购买回执清购物车 | ✅（pending-buy 状态机 + 历史 bug 修复） | ✅ | ✅ | **铁证闭环**：健康会话真实购买最便宜 1 点券道具，余额即时 `989999→989998`（扣 1），购物车同步出现到货项；回执链（发包→0x57 入账→showCash 刷余额）端到端观测成立 | 无 |

**本轮最大发现（根因，非本次需求引入的历史 bug）**：`Net/InPacket.cpp:30` 的 `bool InPacket::available() const { return length() > 0; }` 返回 **bool**，但 `CashShopHandlers.cpp` 里 75 处写成 `recv.available() >= N`（N=2/4/8/13/73）。bool 提升为 0/1，`1 >= 4/8/...` 恒为 **false**——导致 `0x4B/0x57/0x89/0x8D` 解析出的 `CashInvEntry` 每个字段都是 0 且读指针不推进：容器被塞入 N 个全 0 项（`cs_meta` 黄字显示"仓库 3"），而 `rebuild_items` 的 `if (e.itemid <= 0) continue` 把它们全部过滤 → **购物车永远空、余额/商品列表却正常**（后者走直接 `read_int`/本地 nx dump，不依赖坏判断）。这解释了前三轮所有"购物车空"现象：服务端购买从头到尾成功，是客户端解析 bug 让到货项隐形。修复：`available() >= N` → `length() >= N`（`length()` 返回 `size_t` 剩余字节数），75 处全量替换，`while (recv.available())` 的 bool 用法保持不变。修复后同一账号、同样购买流程，购物车立刻正确显示全部到货项。

**编译/部署铁证**：三项改动 + history bug 修复全部进入最终 exe `MapleStory_pua28.exe`（SHA `36e63c1c69aead7c6325a60e7a61053957c97c648ac01492eed762491c9cded7`）；errorC=0 / errorLNK=0；obj mtime 15:58:32 > cpp mtime 15:58:26；部署槽 `cmp` 字节一致。

---

## 1. 证据链（REA→IDA→MinMS/IMG → 决策）

### 1.1 服务端 s9 协议（决定性证据）
`E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/net/server/channel/handlers/CashOperationHandler.java:152-200`：
- 购买 action `0x03`（单件）/`0x1E`（礼包），读入 `byte useNX` + `int sn`。
- 成功：单件 → `PacketCreator.showBoughtCashItem`（`0x57`）；礼包 → `0x89`；随后下发 `showCash`（`QUERY_CASH_RESULT` 三币种 int）。
- 失败：`0x5C showCashShopMessage(code)`。
- `PacketCreator.java:7168-7189` `addCashItemInformation` 布局：`writeLong(cashId) + writeInt(accountId) + writeInt(0) + writeInt(itemId) + writeInt(SN) + writeShort(qty) + writeFixedString(13) + writeLong(expire) + writeLong(0)`；`:7215` `0x57` + 该布局；`:7286` `0x5C` + byte code。服务端 `OutPacket` 写方法均为 LE（`ByteBufOutPacket.java:56-93`），与客户端 `InPacket::read` LE 对齐。

### 1.2 红线：本地封包字节序不可照搬 MinMS
`Net/Packets/CashShopPackets.h:30-40`：`CashShopBuyPacket(int32_t useNX, int32_t sn)` 写 `byte 0x03, byte 0, int useNX, int sn`，与 s9 一致。**禁止**改成 MinMS 的 `(sn, cash_type)` 顺序。三项改动 + bug 修复均未触碰封包字节序/操作码/宽度。

### 1.3 语义差异（决定 item③ 的实现边界）
本地"购物车"栏（第 9 页，`menu_index==8`）= 服务端权威容器 `UIStateCashShop::cash_inventory`（由 `0x4B showCashInventory` 落地）；`0x57` 把刚买到的项 **push 进** `cash_inventory`。本地没有 MinMS 的客户端 `cart_items` 向量，因此 MinMS 的 `remove_cart_item_by_sn(sn)` 不能移植——成功时用 sn 删除会把刚到货项删掉。决策：只移植 pending-buy 状态机（发包置位 → 成功回执确认 → 失败回执清除），不做按 sn 删除。已在代码注释中固化该决策依据。

### 1.4 MinMS 锚点
`MinMS UICashShop.cpp`：`can_try_on :1947-1982`、`set_preview_pet :1921-1945`、`set_preview_effect :1896-1914`、`apply_try_on_item :1984-2014`、`dispatch_buy :2663-2679`、`confirm_purchase_success :1632-1642`、`clear_pending_buy :1644-1647`、`LockerRow/draw_backpack_tabs :1510/:2215`。

### 1.5 资源侧（orange-wz MCP 不可用，走 `.nx` 回退）
本 harness 未暴露 `mcp__orange_wz__*`（`ops_discover_tools` 无匹配），依 AGENTS.md §2 回退 `.nx` dump：`tools/_nx_cashshop_widgets.py` 查 `UI.nx CashShop.img`，`CSLocker` 仅 `BtRebate/BtRebate2`、`CSInventory` 仅 `BtEx{Consume,Equip,Etc,Install,Trunk}`——均无独立格子位图，故已购未取格子复用 `CSList` 商品板。item① 特效节点：本端 `cash_item_effect_root` 取 `nl::nx::Item["Cash"][strprefix+".img"][strid]["effect"]`（与 MinMS 同源）；节点存在性未能经 orange-wz 复核，但真机成功渲染翅膀 = 运行时侧证节点存在。

---

## 2. 改动清单（最小 diff）

### ①宠物 + 特效试穿（工作区既有实现，逐点核对无误，未重复改写）
`IO/UITypes/UICashShop.cpp`：`can_try_on :1267-1292`（脸/发 2/3 + `has_cash_item_effect` + 宠物 `type==500` + 装备 `prefix==1 && !(180..183)` 四路）；`cash_item_effect_root/stance/has_cash_item_effect :1358-1375`、`set_preview_effect :1380+`、`set_preview_pet :1369+`；`clear_conflicting_try_ons :1463-1518`；`apply_try_on_item :1520-1552`/`toggle_try_on_item :1554-1587`（回基线集合重放）；`draw :287-303`（effect 分层）、`update :443-453`；三态按钮 `:737-778`。`IO/UITypes/UICashShop.h:253-260`：`preview_pet`/`preview_effect`/`preview_effect_item_id`/`preview_effect_below` 成员。

### ③购买回执状态机（本次实现）
`IO/UITypes/UICashShop.h:66-75`：`static int32_t pending_buy_sn`、`set_pending_buy`、`confirm_purchase_success`、`clear_pending_buy`（含"不按 sn 删除"决策注释）。`UICashShop.cpp:1235-1266` 定义与实现。发包前置位 4 处：`:720/:752/:887/:1108`。`CashShopHandlers.cpp` 成功钩子 `0x57 :317`、`0x89 :531`、`0x8D :551` 调 `confirm_purchase_success()`；失败钩子 `0x5C :329` 调 `clear_pending_buy()`。

### ②已购未取格子数量串（本次实现）
`IO/UITypes/UICashShop.cpp:1753-1788`：购物车栏（`menu_index==8`）价格位显示 `"x" + std::to_string(item.count>0?item.count:1)`，其余分类保留原价/折扣逻辑。真机已实拍 `x11`/`x1` 正确。

### ★历史 bug 修复：InPacket 剩余字节判断（本次 RCA 根因修复）
`Net/Handlers/CashShopHandlers.cpp`：75 处 `recv.available() >= N` → `recv.length() >= N`（`available()` 返回 bool 不能比大小；`length()` 返回剩余字节数 `size_t`）。覆盖 `0x4B/0x57/0x89/0x8D/0x4D/0x5E` 等所有 cash_inventory 填充 handler。这是购物车空的真正根因，非本次需求引入。

---

## 3. 编译 → 部署铁证

- 强制失效：`touch` 改动源文件 + `rm build-pua-int/CashShopHandlers.obj`（规避增量编译假通过）。
- 构建工具：`tools/_build_uifour.sh`（MSBuild Release x64 v142；OutDir `build-pua/`，IntDir `build-pua-int/`）。
- 最终结果（history bug 修复版）：`BUILD_EXITCODE=0`、`error C=0`、`error LNK=0`；cpp mtime 15:58:26 < obj mtime 15:58:32 < exe mtime 15:58:38。
- SHA 演进：前基线 `2e75e6bd…` → 三项改动版 `79df8d99…`（pua26）→ +history bug 修复版 `36e63c1c69aead7c6325a60e7a61053957c97c648ac01492eed762491c9cded7`（pua28，size 5237760）。
- 部署：`E:/MXD/BeiDou-Client-xy/MapleStory_pua28.exe` = build-pua exe，`cmp` 字节一致。
- 并发注记：14:53 另一 agent 曾改 `matches_sub` 重建 pua27；本轮 bug 修复后重新强制全量编译并部署独立槽 pua28，验证均以 pua28 为准。

---

## 4. 真机验证（DPI client origin=(640,424)，client 800×600）

- 商城进入正常：`evidence/pua26_shop_baseline.png` 白底 21.9%，页签/商品板/预览/余额栏/EXIT 全渲染。
- ①（铁证）：`其他` tab `client(562,50)` + `效果` 子类 `client(600,77)` → `evidence/effect_sub2.png`；点首卡"天使的归来" `client(305,130)` → `evidence/effect_try_click.png` 角色背后出现彩虹翅膀（预览区像素差 11187/28125，放大 `effect_wings_zoom.png`）；点"原型" `client(210,247)` → `evidence/effect_tryoff.png` 翅膀消失。可逆因果链闭环。
- ③安全侧（未授权前）：`evidence/buyconfirm_open.png`（购买对话框正常弹出，婚礼门票 25,000 点券）→ `evidence/buyconfirm_cancelled.png`（取消关闭、无扣费）。
- ②+③最终铁证（pua28，history bug 修复后，健康会话 pid 41468）：`evidence/p28_cart.png` 购物车栏显示 **快乐百宝券 x11 / 金铃铛 x1 / 金铃铛 x1**（放大 `p28_cart_goods.png`），页码 1/1，余额 `2,541,050 / 989998 / 1,000,000`。

### 4.1 三轮调查完整还原（为什么不早不晚才发现 history bug）

1. **第一轮（15:08 授权购买）**：用户授权"选最便宜道具验证"，买 1 点券金铃铛（SN 80000001，定位见 `tools/_nx_cheapest.py`/`_nx_price1_pos.py`，末页 201/201）。服务端日志 `out.log:1036` 记购买成功，但客户端购物车仍空、余额三币不变。误判"环境失效"（客户端内存骤降+ConnectionReset+换频道卡死），未定论。
2. **第二轮（RCA 纠错）**：逐项排除 `canBuy` 之后所有 return 点（itemid 4031191 ∉ cashStore/rateCoupon/mapleLife、仓库未满），证明扣款/入库/发包路径理论完备；证伪第一轮"DB 未扣"（末次存档 14:54:17 早于购买，陈旧快照）与"无日志=未发包"（源码无日志语句）两条错误论据。但仍未找到购物车空的代码根因，归为"服务端现象"。
3. **第三轮（健康会话复验 + 根因突破）**：
   - 先做无消费健康复核（`tools/_pua26_recheck_inventory.py`，pid 38616）：购物车仍空、余额与 DB 一致 → 排除"并发/进程异常"单因素解释。
   - 再做健康会话重买（`tools/_pua26_buy_verify_healthy.py`，同 pid）：购买后余额 `989999→989998`（`hv_balance_before/0p5s.png`）——**扣款成功、showCash 回执到达**，证明服务端处理成功、客户端收包管线通。
   - 关键矛盾：`cs_meta` 黄字（`label_hv_after_0p5s.png`）显示"仓库 3"（`cash_inventory.size()==3`），但购物车列表空。`购物车 tab 连点脚本`（`tools/_pua26_cart_reclick.py`）确认 tab 已切换（页码变 1/1、tab 高亮），排除"点击没生效"假象。
   - RCA 突破：`Net/InPacket.cpp:30 available()` 返回 bool，handler 里 `available() >= N` 恒 false → 所有 `CashInvEntry` 字段读成 0 且指针不推进 → `rebuild_items` 的 `if (e.itemid <= 0) continue` 全过滤。修复 75 处为 `length() >= N` 后（`36e63c1c`），同一账号重进购物车，3 件到货品全部正确显示。

**证据文件**：`evidence/pua26_shop_baseline.png`、`cart_tab.png`、`effect_sub2.png`、`effect_try_click.png`、`effect_wings_zoom.png`、`effect_tryoff.png`、`buyconfirm_open.png`、`buyconfirm_cancelled.png`、`cheapest_lastpage_zoom.png`、`cheapest_confirm_open.png`、`hv_balance_before.png`、`hv_balance_0p5s.png`、`hv_balance_1p5s.png`、`label_hv_after_0p5s.png`、`hv_cart_after.png`、`cartreclick_tabs3.png`、`p28_cart.png`、`p28_cart_goods.png`、`p28_cart_left.png`、`p28_cart_page.png`、`p28_meta.png`；驱动脚本见 `tools/_pua26_*.py`、`tools/_pua28_cart_verify.py`、`tools/_nx_cheapest.py`、`tools/_nx_price1_pos.py`。

---

## 5. 残留风险与未闭环项（如实披露）

1. **①宠物独立视觉未单拍**：玩家本身带虎宠、像素被 idle 动画干扰，宠物试穿未单独拍到清晰前后对比；但与特效共用同一套已验证的 `apply_try_on_item`→`set_preview_pet/set_preview_effect` 重放路径，特效侧已铁证。
2. **①特效 nx 节点未经 orange-wz 复核**：harness 未暴露 orange-wz MCP，按 AGENTS.md §2 回退 `.nx` dump；真机成功渲染翅膀等价于运行时侧证节点存在。
3. **★系统性历史 bug（本次仅修 CashShopHandlers 作用域）**：`available() >= N` 的 bool 误用是全仓库模式——`Net/Handlers/` 下 `AllianceHandlers.cpp`、`CommonHandlers.cpp`、`FredrickHandlers.cpp`、`Helpers/ItemParser.cpp`、`InventoryHandlers.cpp`、`MapObjectHandlers.cpp`、`MessagingHandlers.cpp`、`PlayerHandlers.cpp`、`PlayerInteractionHandlers.cpp`、`QuestHandlers.cpp`、`TestingHandlers.cpp`、`TradeHandlers.cpp`、`WaveAShellHandlers.cpp` 等 13+ 文件共约 334 处同类写法，语义同样恒 false（"剩余不足时读 0"的保护永远不触发，实际因 `read<T>` 越界会抛 `PacketError` 或读到错误数据）。本次为控制改动范围与回归风险，**只修了本目标直接相关的 `CashShopHandlers.cpp`**，其余文件同类 bug 已记录、建议另开专项统一整改（见 §7）。
4. **并发环境干扰**：14:53 起另一 agent 曾反复重建并触发 watchdog 切换进程；本轮最终以独立槽 pua28 干净产物为准，规避干扰。

---

## 6. 复盘四步法

1. **回顾目标**：四项深挖剩余三项，各走 REA→IDA→MinMS/IMG→最小 diff→编译→部署→真机验证闭环。
2. **评估结果**：三项全部真机铁证闭环——①特效翅膀出现/消失可逆；②购物车 `xN` 数量串 + 图标实拍；③健康会话真实购买余额即时扣 1、到货项同步出现。额外修复了阻塞验证的历史 bug（`available()` bool 误用）。
3. **分析原因**：workspace 与恢复摘要不符（item① 已由并发工作落地）→ 以 live 文件为准；增量编译陷阱 → touch+rm obj 强制失效；前三轮误判"环境失效/服务端现象"→ 根因是客户端解析 bug（`available()` bool），靠"余额变但购物车不变"的矛盾 + `cs_meta` 显示 3 项锁定，逐层排除后才定位；视觉后端挂 → 全程 PIL 像素判读。
4. **沉淀规律**：① `InPacket::available()` 是 bool、不是剩余字节数，判断长度必须用 `length()`——这类 API 语义误用要全仓库扫；② "服务端日志成功 + 客户端余额变 + 列表却空"是解析层 bug 的强信号，别急着甩锅环境；③ 二进制脚本改源码必须 touch+rm obj；④ 需消费验证在授权范围内用最便宜道具，健康会话即时观测，避免换频道重载干扰。

---

## 7. 后续（需用户裁决/授权）

- **系统性整改 `available() >= N`（建议另开专项）**：`Net/Handlers/` 下 13+ 文件约 334 处同类 bug，建议统一改 `length() >= N` 并跑全量 handler 回归。本次为控制范围未纳入，已如实记录。
- **①宠物独立视觉**：如需铁证，可在无自带宠物的角色上单独拍宠物试穿前后对比。
- **① orange-wz 资源复核**：若拉起 orange-wz MCP（`E:/project/orange-wz/start-mcp.bat`），复核 `CashEff.img`/`Item/Cash` 特效节点，补资源侧铁证。
- 客户端三项改动（编码+编译+部署+真机）已全部闭环交付；goal-892d6572 客户端作用域内三项完成。上述系统性 bug 整改、宠物单拍、orange-wz 复核属新增工作，不阻塞本目标。
