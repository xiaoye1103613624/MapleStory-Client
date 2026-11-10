# Wave2 · round52 K1 批量零风险项销案（drop/D7 + drop/D4 + statusbar-hud/D5 + corewindow/D5）

## 0. 性质与授权

- 授权来源：用户 m04292「可以自测的自己测试……功能测试完成就提交代码」，本轮沿用既有一次性 Plan
  （`docs/features/development-and-fix-plan-2026-11/README.md`），不重开 Plan。
- 决策来源：`docs/features/angel-calibration-d-matrix-2026-11/README.md` §5 与 §6 P0 ——
  跨面 168 条决策点里 **K1 = 10 条可直接做**（纯文档 / 纯日志 / 未初始化变量护栏，零协议零行为风险）。
  本轮取 P0 代表项里的 4 条：`drop/D7`、`drop/D4`、`statusbar-hud/D5`、`corewindow/D5`。
- 只读校验：**未解析任何 WZ/IMG/XML 内容**。`corewindow/D5` 的坐标结论沿用各面 README 既有证据
  （节点 origin + 截图实测），本轮只换注释里的**依据出处**，不动任何坐标数值。

---

## 1. 四条决策点逐条销案

### 1.1 `drop/D7` —— `Drop::Drop` switch 缺 `default`（`state` 未初始化）

- live 修复前：`Gameplay/MapleMap/Drop.cpp:22-54` 的 `switch (mode)` 只有 `case 0/1`、`case 2`、`case 3`，
  **无 `default`**；`Gameplay/MapleMap/Drop.h:55 State state;` 无初值（`Drop.h:43-48 enum State { DROPPED, FLOATING, PICKEDUP };`）。
- 由此：`mode ∉ {0,1,2,3}` 时 `state` 保持未初始化，而 `Drop::update()`（`Drop.cpp:56-106`）第一件事就是比较 `state`，
  属未定义行为。
- s9 侧 `DROP_ITEM` 的 `mod` 实测只有 `{1,2,3}`（`MapleMap.java:1143-1161 spawnDrop` / `:1163-1178 spawnMesoDrop` 恒 `(byte)1`，
  `:1180-1190` / `:1192-1202 disappe*` 为 `(byte)3`，`MapItem.java:253` 为 `(byte)2`），
  所以**当前 s9 协议下不可达**——本项属防御性护栏，不虚报当前必崩。
- 修复：`switch` 末尾补 `default:`，兜底落 `DROPPED`（最保守「落地态」，与 `case 0/1` 的落地分支同构），
  三个合法 case 的赋值逐字不变。

### 1.2 `drop/D4` —— `ItemData` 缺失时掉落被静默丢弃

- live 修复前：`Gameplay/MapleMap/MapDrops.cpp:80-84`
  `else if (const ItemData& itemdata = ItemData::get(itemid)) { ... drops.add(...); }`——
  `ItemData` 无效（`operator bool` 走 `valid`）时 `else if` 为假，**整条掉落被丢弃且无任何日志**。
  该文件原本不含任何 `LOG(` 调用，也没有 `#include "../../MapleStory.h"`。
- 修复：补 `else { LOG(LOG_DEBUG, "MapDrops: no ItemData for itemid=" << itemid << " oid=" << oid); }`，
  并补 `#include "../../MapleStory.h"`（LOG 宏）与 `#include <iostream>`（宏体内用 `std::cout`，`MapleStory.h` 自身不含）。
  **不改变「不实例化」的行为**，只让现象可观测。
- 诚实边界：真实掉落表缺失属既有台账（`docs/features/map-mob-drops/README.md`「无真实掉落表，显式空态」），本轮不补数据。

### 1.3 `statusbar-hud/D5` —— 状态栏按钮 id 注释误标（2004/2005/2006 → 1005/1006/1009）

- `IO/UITypes/UIStatusBar.cpp:1905` 原注释写 `KeySet=2004`、`QuickSlot=2005/(768,515)`、`QuickSlotD=2006/(573,515)`，
  并把来源记成「IDA BeiDou v17.7 脚本化 CreateCtrl（`sub_8D01B2`）」。
- Angel 083 实证：`?OnCreate@CUIStatusBar@@UAEXPAX@Z @ 0x8d01b2`（0x392d）反编译逐字，11 条 `CreateCtrl(caller, this, id, x, y, type)`：
  `1000/(573,543,t6)` `1001/(629,543,t6)` `1002/(685,543,t2)` `1007/(741,543,t2)`
  `2000/(618,515,t2)` `2001/(648,515,t2)` `2002/(678,515,t2)` `2003/(708,515,t2)`
  **`1005/(738,515,t2)` KeySet**、**`1006/(768,515,t2)` QuickSlot（`SP_1984_UI_STATUSBARIMG_QUICKSLOT`）**、
  **`1009/(573,515,t2)` BtClaim（`SP_1976_UI_STATUSBARIMG_BTCLAIM`）**。
- 交叉校验：`grep -rn "2005|2006|1006|1009" IO/UITypes/UIStatusBar.*` 命中的**只有这一行注释**，
  没有任何代码使用这些数字 → 纯注释改写，零行为风险。
- 修复：注释改成 083 地址 + 正确 id，并显式记下「旧注释把 KeySet/QuickSlot/BtClaim 记成 2004/2005/2006 与 `0x8d01b2` 逐字不符」。

### 1.4 `corewindow/D5` —— 4 处 095 线地址污染换 083 依据

按 `corewindow` 面 §7 Plan 第 1 条列出的 4 处（实际落到 6 个文件位置），逐处换依据：

| # | 位置 | 修复前（095 线） | 修复后（Angel 083 实证） |
| --- | --- | --- | --- |
| a | `IO/UITypes/UIStatsInfo.cpp:146-153` | `Maplestory95 CUIStat::OnCreate(0x867b90)` 称其对 `Stat/main/BtAuto` 走 `CLayoutMan::AddButton` | `Angel 083 CUIStat::OnCreate @ 0x8c4c7f` 逐字**只建** `2006/(124,324)` 的 BtDetail，**没有任何 BtAuto 的 CreateCtrl/AddButton**（同函数 SP_ 节点仅 `SP_1955_..JOB` / `SP_2064_..BTDETAIL` / `SP_1947_..STAT_DISABLED_`；strings 缓存 `BTAUTO`/`BT_AUTO`/`STAT_BTAUTO` 全 0 命中）。坐标改口径为「节点 origin=(-94,-198) + 截图实测」推断，数值不动 |
| b | `IO/UITypes/UIItemInventory.cpp:124` | `Maplestory95 CUIItem::OnCreate @ 0x7CE520` | `Angel 083 CUIItem::OnCreate @ 0x81c6c9` 逐字 `memset(this + 378, 0, 0x18u);` + `SetArrangeButton(this + 377, this + *(this + 377) + 378);` |
| c | `IO/UITypes/UIItemInventory.cpp:544-548` | `CUIItem::OnButtonClicked @ 0x7CC4F0` 称 0x7D3/0x7D4 点击不切钮 | `CUIItem::OnChildNotify @ 0x81d01f` 的 switch 只覆盖 `0x7D4/0x7D5/0x7D6/0x7D7` 的**悬停提示文案**；切钮只由 `CWvsContext::OnGatherItemResult @ 0xa1e943` / `OnSortItemResult @ 0xa1e96d` 内部 `SetArrangeButton(ui, ti, 1/0)` 触发 |
| d | `IO/UITypes/UIItemInventory.cpp:1457` | `SetArrangeButton @ 0x7CCC90` | `Angel 083 CUIItem::SetArrangeButton(nTI, nState) @ 0x81e3a6` 逐字 `*(this + nTI + 378) = nState;`；`nState==0 → BtGather`、`nState==1 → SP_1852 = BtSort` |
| e | `IO/UITypes/UIItemInventory.h:50` | `（Maplestory95 @ 0x7CCC90）` | `Angel 083 CUIItem::SetArrangeButton(nTI, nState) @ 0x81e3a6` |
| f/g | `Net/Handlers/InventoryHandlers.cpp:56,71` | `OnGatherItemResult @ 0x9F1280` / `OnSortItemResult @ 0x9F12B0` | `@ 0xa1e943` / `@ 0xa1e96d`，并注明旧地址在 083 IDB 里是 `ZAllocEx` 构造器 |

**逐地址反查（Angel 13339 `py_eval`）**：`0x9f1280 → sub_9F1257`、`0x9f12b0 → sub_9F1296`、
`0x7ccc90 → ?OnMouseMove@CUIAdminShopWishListSearchResult@@UAEHJJ@Z`（完全不相关）、
`0x7ce520 → sub_7CD840`、`0x7cc4f0 → sub_7CC3D4`、`0x867b90 → sub_867B61`
→ **全部证实旧注释的 095 地址在 083 里指向无关函数**。
BeiDou 13337 复核 `0x9F1280`/`0x9F12B0` 同为 `sub_9F1257`/`sub_9F1296`（同二进制第 4 次独立复证）。

---

## 2. 排除的假阳性（避免按 grep 立项）

本轮在锁定这 4 条之前整批扫过并逐字排除：

1. **反编译里找 `0x7D3`/`0x7D4` 字面量** → `CUIItem::OnCreate @ 0x81c6c9` / `OnChildNotify @ 0x81d01f` /
   `OnMouseButton @ 0x81d42e` 三处均 **0 命中**：按钮 id 常量不在二进制里硬编码，而是走头文件/枚举，
   不能因「反编译查不到」就判「原生没有该按钮」。
2. **`BtAuto` 是否在 083 存在**：`CUIItem`/`CUIStat` 两个 CreateCtrl 家族都没有 BtAuto；
   `UIWorldMap.cpp:133` 的 `BtAutoFly_1` 是**另一个窗口**的按钮，不构成反证。
   → BtAuto 属「客户端按 `UIWindow2.img/Stat/main/BtAuto` 节点补的原生没有的按钮」，本轮只在注释里诚实标注，不改行为。
3. **Gather/Sort 请求发包函数**：`entity_query *GatherItem*` / `*SortItem*` 只查出 `OnGatherItemResult` /
   `OnSortItemResult` 与 `CTrunkDlg::SendSortItemRequest`，**没找到 CUIItem 侧的 SendGatherItemRequest**，
   说明该 c2s 在 083 走别的封装；本轮不追（不属 K1 范围）。

---

## 3. 自测（可离线复刻，不需真机）

`tools/test_k1_batch_round52.py` —— 静态守卫 + 行为复刻两层：

- **静态守卫**：`d7_default_present`、`d7_default_assigns_state`、`d7_still_three_cases`、三条合法赋值逐字保留、
  `d4_has_else`、`d4_log_line`、`d4_include_maplemstory`、`d4_include_iostream`、`d4_no_behavior_change`
  （`drops.add(spawn.instantiate(icon));` 恰 2 次，证明没加第三条实例化路径）、
  `sb_d5_has_1006`、`sb_d5_has_1009`、`sb_d5_no_2005`、`sb_d5_no_2006`、`sb_d5_cites_083`、
  `cw_083_cuistat`、`cw_083_cuiitem_create`、`cw_083_garonotify_0x81d01f`、`cw_083_setarrange_0x81e3a6`、
  `cw_083_setarrange_header`、`cw_083_gather_result`、`cw_083_sort_result`，
  以及 `no095_<file>_<bad>` 对 `0x867b90 / 0x7CCC90 / 0x7CE520 / 0x7CC4F0 / 0x9F1280 / 0x9F12B0 / Maplestory95`
  在四个文件里清零。
- **行为复刻**：meso 四档 8 组，`ange_tier()`（083 `sub_506BFE @0x506bfe` 语义 `<50→0 / 50..99→1 / 100..999→2 / >=1000→3`）
  与 `client_tier()`（`itemid>999 BAG : >99 BUNDLE : >49 GOLD : BRONZE`）逐档相等；
  Drop 构造模型：合法 mode 0/1/2/3 修前修后**等价**，非法 mode `{4,5,7,127,-1,-128}` 修前 `None`（未初始化）→ 修后 `DROPPED`。

首跑 `TEST_RC=1`，3 个 FAIL（`no095_UIItemInventory.cpp_0x7CC4F0`、`no095_InventoryHandlers.cpp_0x9F1280`、
`no095_InventoryHandlers.cpp_0x9F12B0`）—— 原因是**我自己的新注释里仍写着被剔除的旧地址字面量**。
修法：注释改成「旧注释曾引 095 线地址 / 另一个地址」，不带具体数值。重跑：

```text
PASS: K1 batch round52 (drop/D7 + drop/D4 + statusbar-hud/D5 + corewindow/D5)
TEST_RC=0
```

> 教训（与本轮修复同源）：**「剔除污染」的注释里不能残留被剔除的字符串**，否则守卫自相矛盾、未来 grep 仍会命中。

门禁与构建：

```text
python tools/gen_opcode_matrix.py --check
  → GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)

MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c "tools\build_smoke.bat"
  → GATE OK ×2 + EXITCODE=0 + BUILD_RC=0
  → build_log.txt 2026-11-10 11:21:09 / 181614B（首行 Drop.cpp，六个改动文件全在编译列表）
  → ERROR_LINES 0；EXE 2026-11-10 11:21:08 / 4928512B（真重编译）
```

`taskkill` 对 34476/6108/56016 仍 `Access is denied`（32K 空壳，历史同款，不影响构建）。

---

## 4. 范围边界

- 不改协议、不改 opcode、不改包体：`gen_opcode_matrix.py --check` 逐字通过。
- 不改任何坐标/尺寸数值：`corewindow/D5` 只换**依据出处**，`BtAuto(106,198)` 等落点原样保留。
- 不改运行时行为的三条：`drop/D7`（仅非法 mode，s9 不发的值）、`drop/D4`（仅新增日志）、
  `statusbar-hud/D5` + `corewindow/D5`（纯注释）。
- `drop/D4` 的「掉落表缺失」不属本轮：既有台账已记「无真实掉落表，显式空态」，本轮只补可观测性。

---

## 5. 真机待验（进 `pending-live-tests`）

| # | 操作 | 期望 |
| --- | --- | --- |
| 28-1 | 正常拾取/丢弃物品与金币 | 掉落动画、拾取提示、金币四档图标与修复前一致（本批零行为改动） |
| 28-2 | 角色信息窗 / 道具栏 / 属性窗 | 按钮位置与修复前逐像素一致（只换注释依据，不动坐标） |
| 28-3 | 打开道具栏切换 BtGather/BtSort | 与修复前一致（Gather/Sort 结果切钮路径未动） |
| 28-4 | 调 `LOG_LEVEL` 到 DEBUG 后进图刷怪 | 出现 `MapDrops: no ItemData for itemid=... oid=...` 只代表该 itemid 无 WZ 条目，不应伴随崩 |

---

## 6. 提交 scope

```text
Gameplay/MapleMap/Drop.cpp
Gameplay/MapleMap/MapDrops.cpp
IO/UITypes/UIStatusBar.cpp
IO/UITypes/UIStatsInfo.cpp
IO/UITypes/UIItemInventory.cpp
IO/UITypes/UIItemInventory.h
Net/Handlers/InventoryHandlers.cpp
tools/test_k1_batch_round52.py
docs/features/development-and-fix-plan-2026-11/notes/wave2-k1-batch-round52-20261110.md
docs/features/development-and-fix-plan-2026-11/README.md
docs/features/pending-live-tests/README.md
```

`git diff --stat HEAD`（源码部分）= `7 files changed, 60 insertions(+), 25 deletions(-)`；`git diff --check HEAD` 无输出。

---

## 7. 后续轮台账（本轮不做，已留证据）

全仓仍余 **5 处 095 线地址污染**（属 `corewindow/D5` 之外的 Wave3 范围），083 真地址已在本轮实测：

| 位置 | 现值（095） | Angel 083 真地址 |
| --- | --- | --- |
| `Net/Packets/MovePathEncode.h:2-3` | `Maplestory95.i64 @ 0x666e20` | `?Encode@CMovePath@@QAEXAAVCOutPacket@@PAH@Z @ 0x68a563`（0x269） |
| `Net/Packets/NpcInteractionPackets.h:42,44` | `CNpc::GenerateMovePath @ 0x671590`、`Encode 权威 0x666e20` | `?GenerateMovePath@CNpc@@IAEXJJ@Z @ 0x6d3273`（0x11f）、`CMovePath::Encode @ 0x68a563` |
| `Net/Handlers/WaveAShellHandlers.cpp:3585` | `IDA 0x666e20` | 同上 `0x68a563` |
| `Gameplay/MapleMap/Npc.cpp:253` | `CMovePath::Encode @ 0x666e20` | `0x68a563` |
| `Character/Player.cpp:211` | `对照 port-095 CWvsContext::CheckTemporaryStatDuration` | 需再取证（`*CheckTemporaryStat*` 在 083 IDB 0 命中，函数可能改名/内联） |

反证：`0x666e20` 在 083 落在 `sub_666362`、`0x671590` 落在 `?TrySpeaking@CMob@@IAEXJJ@Z` —— 都不是路径编码函数。

---

## 8. 证据优先级（本批实际遵守）

`opcode-matrix.json` > live 源码逐字 > Angel 083 IDA 反编译（13339）> BeiDou 13337 交叉（同二进制）> 各面 README 既有台账 > 095 线注释

- 反编译存盘：`tools/_ida_CUIStatusBar_OnCreate.txt`（126188B）、`tools/_ida_CUIItem_OnCreate.txt`（18077B）、
  `tools/_ida_CUIItem_OnChildNotify.txt`（3881B）、`tools/_ida_CUIItem_OnMouseButton.txt`（9269B）、
  `tools/_ida_CUIStat_OnCreate.txt`（8626B）。
