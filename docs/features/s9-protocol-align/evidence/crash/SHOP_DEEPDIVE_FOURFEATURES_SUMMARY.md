# 商城四项扩展深挖 · 汇总报告

日期：2026-11-17 · 分支 `feature/dev_0.0.1` · 目标 `goal-82dc531a`
方法：🟠阿里（定目标→追过程→拿结果闭环 + 复盘四步法）

## 一、目标与结论

用户追问「还有没完成的？或者深入处理的嘛？」，四项全选深入处理。逐项结果：

| 项 | 内容 | 结论 | 验收 |
| --- | --- | --- | --- |
| ① | 试穿沙盒补宠物+特效 | ✅ 完成（本轮编码+构建+部署+真机） | 宠物 orange 120→491→120；特效翅膀真机可见（修 stance 错位 bug 后） |
| ② | CSInventory/CSLocker 格子UI | ✅ 完成（服务端权威 cash_inventory 复用 CSList 格） | 购物车页签渲染空态格子 |
| ③ | 购买回执清购物车 | ✅ 适配完成（本端无 cart_items，改为 pending_buy_sn 状态机对账） | handler 4 处调用 confirm/clear；真机端到端购买受服务端名额限制无法稳定复现，代码路径已核对 |
| ④ | 翻页条改造 | ✅ 完成（前期轮次） | 真机 1/201→2/201→1/201，商品 hash 随之变化 |

## 二、①宠物+特效试穿（本轮主攻）

### 根因/缺口
本端 `UICashShop` 试穿沙盒 `apply_try_on_item` 只有脸(type2)/发(type3)/装备三分支，缺宠物(type500)与现金特效路径；MinMS 有完整的 `cash_item_effect_root/clear_preview_pet/set_preview_effect` 体系。

### 资源可行性（先取证，不空转）
- `E:/MXD/BeiDou-Client-xy/Data/Item/Pet/` 有 1247 个 `500xxxx.img`；`Etc.nx/Commodity.img` 上架 OnSale==1 中 prefix=500 有 34 只宠物。
- `nl::nx::Item["Cash"]` 49 个 img，带 effect 节点道具：5010000 / 5030008 / 5280000，其中 5010000(SN 50500000)、5030008(SN 50100031) 在售。
- `Item.nx` effect 子节点：05010000 有 `default`；05030008 有 `0..8`；05280000 有 `0..35` + `z`。

### 改动（tools/_apply_petfx.py，11 处 assert 唯一，备份 tools/_bak_UICashShop.{h,cpp}.before_petfx）
- `.h`：include PetLook.h；新增 6 方法声明（`cash_item_effect_root/stance`、`has_cash_item_effect`、`clear/set_preview_effect`、`clear/set_preview_pet`）；成员 `PetLook preview_pet` / `Animation preview_effect` / `preview_effect_item_id` / `preview_effect_below`。
- `.cpp`：`cash_item_effect_root` 从 `nl::nx::Item["Cash"]["0"+id/10000]["0"+id]["effect"]` 取节点，`cash_item_effect_stance` 按 `Stance::names` 取帧回退 `default`；`set_preview_effect` 读 z 定前后景；`set_preview_pet` 用本端 6 参 `PetLook(iid,name,0,Point(feet.x-40,feet.y),2,0)`（MinMS 9 参，去掉本端无的 balloon/name flag）。
- 接入 6 处：`can_try_on` 放行 type500/has_effect；`reset_preview_to_baseline` 清宠物/特效；`clear_conflicting_try_ons` 加宠物/特效同槽互斥；`apply/toggle` 重放集合加宠物/特效分支；`draw` 按 z 前后景绘制；`update` 驱动 effect.update + pet 跟随/爬梯。

### 适配要点（冰山下面）
- 本端 `PetLook::draw(viewx,viewy,alpha)` 三参、`phobj.get_absolute` 内部加 viewx/viewy，与 MinMS 传 position+art 不同 → 传 `position + WND_CHAR/PREVIEW_WELL` 的 local 偏移。
- 本端 `PetLook` 6 参构造，MinMS 9 参 → 掉 name/balloon flags。
- `Stance::names[STAND1]` 取 effect 帧，缺失回退 `default`。

### 构建/部署门禁
- BUILD_EXITCODE=0、error C=0、obj mtime 14:29:18 > cpp 14:28:05、SHA 3f609f70 → 2e75e6bd（宠物版）。
- 并发会话期间源码又加入③回执/②已购栏（14:41:45 稳定），重编整合：obj 14:42:54 > cpp 14:41:45、error C=0、SHA 2e75e6bd → 79df8d99（整合版）。部署 `MapleStory_pua26.exe`。

### 真机证据（视觉后端不可用，全程 PIL 像素）
- 进商城白底 16.7%；宠物页签 1/4 页 34 只。
- 宠物试穿三向闭环：baseline orange 129 → 点击首卡 491（穿上，小老虎立绘清晰）→ 再点 120（脱下）→ reset 129。放大图 `petfx_preview_zoom.png` / `petfx_reset_zoom.png` / `petfx_toggle2_*`。

### 特效路径的真 bug 与修复（蓝军自检抓到）
- 现象：初版特效试穿真机像素差分恒 0。根因不是脚本，是代码——`cash_item_effect_stance` 用 `Stance::names[STAND1]`，但本端枚举与 names 数组错位：`Stance::names[20]="stabTF"`，而 MinMS 的 STAND1 应为 `"stand1"`（本端 "stand1" 在 idx21）；本端 v83 effect 实际节点为 `effect/default/帧(0..5)`（05010000）或 `effect/0..8`（05030008），既非 "stabTF" 也无 default 兜底 → 查空被清。
- 修复（tools/_fix_fx_stance.py，备份 `tools/_bak_UICashShop.cpp.before_fxfix`）：改用字面量优先级 `{"stand1","stand","default"}`，最后回退帧 `"0"`，不再依赖错位枚举名。重编门禁 error C=0、obj 14:53:12>cpp 14:53:04、SHA 79df8d99→de60985b，部署 pua27。
- 真机复验：切"其他→效果"子页签，reset 基线后点特效卡，角色背后出现清晰彩虹发光翅膀（放大图 `fx_final_on0_zoom.png`）。**特效试穿真机通过。**（之前验证脚本第二列卡片中心误用 x=430，正确为 x=520=PLATE_DX，已纠正。）

## 三、②③（并发会话改动，本轮核实纳入整合产物）
- ③：`pending_buy_sn` 静态状态机（set_pending_buy / confirm_purchase_success / clear_pending_buy），`Net/Handlers/CashShopHandlers.cpp` 4 处调用。本端无 MinMS `cart_items`，成功回执只清 pending + 刷新 UI，不删 cash_inventory（删 sn 会误删刚到货项）——适配而非照抄。
- ②：购物车/衣柜页签读服务端权威 `cash_inventory`，复用 CSList 商品格渲染（UI.nx 实测 CSLocker 只有 BtRebate、CSInventory 只有 BtEx* 按钮，无专用格子贴图）， locker 视图显示持有数 `xN` 而非误导性「0 点券」。

## 四、④翻页条（前期轮次，证据复用）
- 页码 "1/201"（201=ceil(2010/10)）；NEXT→2/201、PREV→1/201，商品 hash 随之变化。报告见 `SHOP_DEEPDIVE_PAGE_REPORT.md`。

## 五、复盘四步法
1. **回顾目标**：四项扩展全选深入 + 逐项真机验证。
2. **评估结果**：四项全部落地整合产物；①宠物本轮完整闭环，②③并发实现核实纳入，④前期已验。
3. **分析原因**：4 路研究子代理均半途挂掉但留下有效取证；主代理接手直接读源码，避免"等报告"空转。并发会话同改一个文件导致 exe 多次落后源码，靠 mtime+SHA 门禁识别并重编整合。
4. **沉淀规律**：WZ/IMG 走 orange-wz MCP（本会话 harness 未暴露，靠 .nx 直读 + 资源目录佐证）；编译纪律=删 obj + obj mtime>cpp + SHA 变 + error C=0；本端建钮统一 `try_maple_btn`；并发编辑场景必须核对产物 SHA 与源码一致性再验证。

## 六、证据索引
- `evidence/petfx_preview_zoom.png`（宠物穿上）、`petfx_reset_zoom.png`（reset 脱下）、`petfx_toggle2_*`（toggle 双向）、`fx_final_on0_zoom.png`（特效翅膀真机）、`integ_cart_tab.png`（购物车空态格子）、`integ_pet_zoom.png`（整合版宠物回归）、`integ_shop_full.png` / `fxfix_shop_full.png`（整合版商城+页码）。
- 脚本：`tools/_apply_petfx.py`、`_fix_fx_stance.py`、`_petfx_launch.py`、`_petfx_click.py`、`_petfx_reset.py`、`_fx_final.py`、`_integ_launch.py`、`_integ_verify.py`。
- 备份：`tools/_bak_UICashShop.{h,cpp}.before_petfx`、`_bak_UICashShop.cpp.before_fxfix`。
- 最终产物：exe SHA `de60985b65318559`（含①宠物+特效修复②格子③回执④翻页），部署 `MapleStory_pua27.exe`。
