# #15 `corewindow/D2` 五窗逐窗反编译比对（round100）

> **本轮口径变更（用户 2026-11-12 指令）**：「所有逻辑以 Angel 端为准，放弃 BeiDou 端的逻辑」。
> 即 `corewindow/D2` 判定从 round99 的 `KEEP`（s9 协议优先 / `BATCH_0_BASELINE`）**翻案**：Angel 083/v83 原生
> （13339 `` `Angel.i64 ``）成为逻辑真值源；BeiDou(13337) 无符号 IDB 逻辑不再采信。
> **本轮只做只读取证与逐窗比对，零源码改动**（门禁 1/5/18：先证据、再分析、后抉择、再计划、后实现）。

## 0. 结论速览

| 项 | 结果 |
| --- | --- |
| 取证路径 | `REA/IDA(13339 现场 decompile) → 当前端源码 → orange-wz(IMG)` 三层只读取证 |
| IDB 校验 | `server_health`：`` `Angel.i64 `` / `` `Angel.exe `` / v83 / imagebase `0x400000` / hexrays_ready ✅ |
| 现场反编译 | 五窗方法族 40 个 + 12 个业务/发包锚点伪代码全文（落 `evidence/p3-item15-corewindow-decompile-round100.json`） |
| IMG 核验 | orange-wz MCP（128B `WzAESConstant.DEFAULT_KEY` + GMS IV）`loadedCount=1`，**本次解析成功无异常**，未回退 `.nx` |
| 逐窗判定 | 5 窗 × 4 维（按钮 id / 发包 / 状态机 / 资产）全部列出差异，见 §2 |
| ~~不可直接改（协议层）~~ **round101 已更正** | 083 发包 opcode/载荷与 s9/当前端**完全同构**（`COutPacket(87)`/`(88)` = `DISTRIBUTE_AP`/`AUTO_DISTRIBUTE_AP`）；「断联机」只针对 W 桶 6 个 **s2c** 改造字段，与 c2s 加点包无关（见 §7） |
| **可按 Angel 校准**（L 桶，不改 wire） | 500ms 独占门闩、4 个 `SP_` tooltip 分支、三颗 `BtAuto` 资产、ToggleDragonEquip、`InvalidateRect`/`ReDrawQuickslot` 跨窗重绘 |
| 待决项 | **Q1 已被 round101 证据消解**（保持 s9 字段类型 + L 桶按 Angel 逻辑即可，无需二选一）；剩 Q2/Q3/Q4（§4） |

---

## 1. 证据与门禁

| 门禁 | 落地 |
| --- | --- |
| 门禁 5 禁想象修改 | 12 个锚点全部 **13339 现场 `decompile`**，非引用既有文档；EA 与既有 `angel-corewindow-decompile-full.json` 逐一对上 |
| 门禁 8 WZ/IMG 走 MCP | orange-wz MCP `load_files`（`exclusive:false`）+ `query_nodes`，`unload_all` 收口 |
| 门禁 18 原生还原主路径 | REA/IDA 逻辑 + IMG 资源双解析；**未启用截图兜底**（主路径证据充分） |
| AGENTS.md §2 | 主路径 orange-wz 本次 `loadedCount=1` 成功，无 `Index N out of bounds`，**不需要** `.nx` 回退 |

**既有产物复析**（skill「先读产物再复析」）：`docs/features/corewindow-angel-calibration-2026-11/`
的 `README.md`（六维 98 条 / D1–D14）、`notes/r01~r05`、`evidence/angel-corewindow-decompile-full.json`（296 函数）。
本轮现场复核发现既有文档 2 处不完整，已在 §2 修正（这正是「angel 端是否不完整」问题的答案：**既有取证基本完整，但有细节缺口**）。

### 1.1 五窗方法族 EA（现场 `func_query`，2026-11-12）

```
CUIStat      (8)  0x8c4c7f OnCreate / 0x8c522f OnButtonClicked / 0x8c545d ToggleDetail
                  0x8c5845 EnableApUpButton / 0x8c59d4 ResetInfo / 0x8c59ff Draw
                  0x8c79f5 RestoreButtons / 0x8c8692 AutoApUp
CUIEquip     (4)  0x7fe9bb OnMouseMove / 0x7feb72 OnButtonClicked
                  0x7ff8dd ToggleDragonEquip / 0x7ffa84 TogglePetEquip
CUIItem     (12)  0x81c6b7 ChangeTab / 0x81c6c9 OnCreate / 0x81d01f OnChildNotify
                  0x81d230 OnTabChanged / 0x81d42e OnMouseButton / 0x81d8ad OnMouseMove
                  0x81d996 OnDropMoney / 0x81db7e GetSlotPositionFromPoint / 0x81dc20 Draw
                  0x81e2c8 GetItemSlotRect / 0x81e3a6 SetArrangeButton / 0x81e5da SetLastestGetItemEffect
CUISkill     (5)  0x8aa4c5 CloseSkillGuide / 0x8aaa88 OnCreate / 0x8acf1a OnSkillLevelUpButton
                  0x8bcdfa GetSkillRootVisible / 0x961063 OpenCurSkillGuide
CUIKeyConfig (9)  0x8335d0 OnButtonClicked / 0x83372c OnMouseButton / 0x833749 OnDestroy
                  0x8338f9 OnMouseMove / 0x833a65 GetShortCutIndexByPos
                  0x83550d TryBeginDragFuncKeyMappedIcon / 0x83665b CNoticeDlg::ctor / 0x836dc0+0x836de7 ReleaseRaw
```

---

## 2. 逐窗反编译比对（083 Angel 为准 vs 当前端）

### 2.1 Stat 能力值窗

**083 `CUIStat::OnButtonClicked @0x8c522f`（现场反编译全文见 evidence）**

```c
switch (a2) {
  case 2000: v2=2048;   goto LABEL_3;
  case 2001: v2=0x2000; goto LABEL_3;   // 8192
  case 2002: v2=64;     goto LABEL_3;
  case 2003: v2=128;    goto LABEL_3;
  case 2004: v2=256;    goto LABEL_3;
  case 2005: v2=512;    goto LABEL_3;
LABEL_3: CWvsContext::SendAbilityUpRequest(*dword_BE7918, v2); break;
  case 2006: CUIStat::ToggleDetail(this); break;
  case 2007: case 2008: CUIStat::AutoApUp(a1:1); break;
  case 2009: CUIStat::AutoApUp(a1:0); break;
  default:   CUIWnd::OnButtonClicked(a2); break;
}
```

`CUIStat::AutoApUp @0x8c8692` 现场反编译：走 `CWvsContext::GetCharacterData` → `sub_7939BA`
（自动分配方案表）→ `_ZtlSecureFuse<short>` 读剩余 AP → 按 `64/128/256/512` 逐项 clamp 累加。
**手动加点**是客户端本地校验 + `SendAbilityUpRequest(87)` 掩码发包（其内部 `@0xa239b7` 现场反编译见 §7）；
**自动加点** `AutoApUp` 先本地算方案 + 弹确认，确认后调 `sub_A23B3D @0xa23b3d` 发 `COutPacket(88)`（`AUTO_DISTRIBUTE_AP`）单包。

| 维度 | 083 Angel | 当前端 | 判定（Angel 为准） |
| --- | --- | --- | --- |
| 按钮 id | 2000–2009 十分支 | `BT_CLOSE..BT_DETAIL_DETAILCLOSE` 0–13 连续枚举 | **形态差**（枚举是本地重构，非 wire）→ 不动 |
| 手动加点发包 | `COutPacket(87)` + `Encode4(time)` + `Encode4(掩码)` | `SpendApPacket(87)`: `write_time(); write_int(codes[stat])` | ✅ **wire 同构**（更正）；差异在掩码语义（下条），属 L 桶 |
| 掩码语义 | 2000→2048=MAXHP, 2001→0x2000=MAXMP；2002–2005=64/128/256/512 | `HP=0x400(1024)/MP=0x1000(4096)`；STR..LUK=64/128/256/512 | **数值部分重合 ≠ 语义重合**：HP/MP 编码不同，STR..LUK 恰巧相同 |
| 自动加点 | `COutPacket(88)` + `time` + `count` + `count×{int type,int amount}`，按钮 2007/2008/2009 两态 | 单 `BT_AUTO` → `UIYesNo` → `AutoAssignApPacket(88)`（`time + int(0) + 2×{int,int}`） | ✅ **wire 同构**（更正）；差异=本地算方案 vs 服务端算方案，属 L 桶状态机 |
| 详情切换 | `2006 ToggleDetail`（单 toggle） | `BT_DETAILOPEN/CLOSE/DETAIL_DETAILCLOSE` 三分支 → `set_detail(bool)` | 语义一致，本地实现差异 → 不动 |
| 独占门闩 | **`CanSendExclRequest(500,0)` 500ms 时间门闩**（`SendAbilityUpRequest` 与 `sub_A23B3D` 开头都调；`@0x485bf7` = `get_update_time()-last>=500`） | `UI::get().disable()` 等服务端回包 enable | **L 桶缺口**（更正 round100「083 无门闩」误判）：可补本地节流，不动 wire |

**s9 对端铁证**（`E:\project\BeiDou-Server_s9`）：
- `DistributeAPHandler.handlePacket:33-36` = `p.readInt(); int num = p.readInt(); AssignAPProcessor.APAssignAction(c, num);`
- `AssignAPProcessor.addStat:711` = `switch(apTo)` 用 `case 64 -> STR` 且 `APReset :564 case 64 -> STR`。
- `AutoAssignHandler:36` = `AssignAPProcessor.APAutoAssignAction(p, c)`（服务端按职业智能分配，非 083 客户端算方案）。

→ **~~结论：两套协议，改则断联机~~ round101 更正（见 §7）**：现场反编译 `SendAbilityUpRequest @0xa239b7`
证明 083 发的就是 `COutPacket(87)`（`DISTRIBUTE_AP`）、`sub_A23B3D @0xa23b3d` 发的就是 `COutPacket(88)`
（`AUTO_DISTRIBUTE_AP`），**opcode 与载荷布局与 s9 / 当前端三者一致**。原判定把「`CWvsContext` 内部方法名不同」
误当成「协议不同」，属误判。真正会让 s9 断联机的只有 W 桶 6 个 **s2c** 改造字段（§7）。

**★ 本轮新发现（既有 README §4 未记）**：orange-wz 实测 `Stat` 下有 **`BtAuto` / `BtAuto1` / `BtAuto2` 三颗独立资产**，
正好对应 083 的 `2007/2008 → AutoApUp(1)` 与 `2009 → AutoApUp(0)` 两态三钮。当前端只有 1 颗 `BT_AUTO`
（`UIStatsInfo.cpp:146-158` 的落点是靠 `UIWindow2.img/Stat/main/BtAuto` 的 origin 推断，注释自承「漂移风险记 backlog」）。
**这是 L 桶资产层可校准点**：三颗节点的真实坐标/状态可用 orange-wz 取，不需改 wire。

### 2.2 Item 背包窗

**083 `CUIItem::OnChildNotify @0x81d01f`（现场反编译）**——既有 README §4 只记了 2 个分支，现场全文是 **4 个**：

```c
if (a2==2000 && a3==500) { OnTabChanged(this, a4); return; }   // tab 500
else if (a2==2001 && a3>=0x12C && a3<=0x130) { ... SetLastestGetItemEffect ... }
if (a3==101) {  // tooltip 文案分发
  if (a2 != 2003) {
    switch (a2) {
      case 0x7D4: SP_698_ITEM_SORT              // 整理
      case 0x7D5: SP_4501_EXPAND_INVENTORY      // 扩展
      case 0x7D6: SP_4502_COLLAPSE_INVENTORY    // 收起  ← 既有文档未记
      case 0x7D7: SP_4504_...CASH_ITEMS         // 现金扩槽说明 ← 既有文档未记
      default: return;
    }
  } else { SP_697_SLOT_MERGER ... }             // 2003 槽位合并
}
if (a3==100) (*(*this+32))(this, a2);          // 按钮派发
```

| 维度 | 083 Angel | 当前端 | 判定 |
| --- | --- | --- | --- |
| 整理/排序 | `0x7D4` 为 tooltip 文案；实际动作在 `a3==100` 派发 | `BT_GATHER/SORT` → `GatherItemsPacket(69)` / `SortItemsPacket(70)`（`write_time+write_byte(tab)`） | ✅ opcode 一致（`§7`）→ 保持 s9 现状；形态/文案差异记 L 桶 |
| 扩槽 | `0x7D5` / `0x7D7` 两个提示文案 | `BT_FULL/SMALL` 纯本地展开；`BT_CASHSHOP`→`EnterCashShopPacket(40)`；扩槽走 `UseCashItemBasicPacket(79)` / `CashShopInvSlotPacket` | 语义部分对齐，形态差 |
| 收起 | `0x7D6 SP_4502_COLLAPSE_INVENTORY` | `BT_SMALL` 有动作但**无该 tooltip** | **L 桶缺口**（可补文案/态） |
| 槽位合并 | `a2==2003 → SP_697_SLOT_MERGER` | `merge\|combine\|合并` 在 `UIItemInventory.cpp` **0 命中** | **L 桶缺口**（083 有、当前端无） |
| tab 500 / a3=101 | 原生分发协议 | 6 个连续 tab 枚举，`0x7D4..7D7` 四分支仅以**注释**存在于 `:546-551` | **L 桶缺口**：注释已在、实现未落 |
| SetArrangeButton | `@0x81e3a6` | `UIItemInventory.cpp:1459-1499` 已对齐（`nState 0→BtGather(0x7D3) / 1→SP_1852 BtSort(0x7D4)`） | ✅ 已对齐 |

### 2.3 Skill 技能窗

**083 `CUISkill::OnSkillLevelUpButton @0x8acf1a`（现场反编译）**：

```c
if (CWvsContext::CanSendExclRequest(a1:500, a2:0) != 0) {   // 500ms 独占请求门闩
  ... GetCharacterData ... _ZtlSecureFuse<short>(v4+137, *(v4+141)) ... get_novice_skill_point(cd) ...
  if (GetSkillLevelUpState(v38, v8)==1 && *(v8+11)==0) { ... SP_3509/3510/3511 三档前置拦截 ... }
}
```

| 维度 | 083 Angel | 当前端 | 判定 |
| --- | --- | --- | --- |
| 500ms 独占门闩 | `CanSendExclRequest(500,0)` 在函数最外层 | **无**（全仓库 `CanSendExclRequest` 源码 0 命中，只在 `tools/_r91_deliver.py` 等反编译交付文档里）；只有 `UI::get().disable()` | **L 桶实质缺口**（可补本地节流，不动 wire） |
| SP 前置校验 | 门闩内 `_ZtlSecureFuse<short>` 剩余 SP + `get_novice_skill_point` + `GetSkillLevelUpState` + `SP_3509/3510/3511` 三档提示 | `can_raise()` `:1073-1099`（beginner_sp/sp/masterlevel/ANGEL_BLESSING/check_required）**只用于把 + 钮置 DISABLED**；`send_spup:1101-1116` 与 `BT_OKAY` 发包路径**都不调用它** | **L 桶实质缺口**（校验未覆盖所有发包入口） |
| 加点发包 | `CUISkillEx::OnButtonClicked COutPacket(165)` 等 | `SpendSpPacket(90)` = `write_time(); write_int(skill_id)` | ✅ opcode 一致（`§7`）→ 保持 s9 现状；形态/文案差异记 L 桶 |

### 2.4 KeyConfig 键盘设置窗

**083 `CUIKeyConfig::OnButtonClicked @0x8335d0`（现场反编译，比既有 README §4 更精确）**：

```c
CUIWnd::OnButtonClicked(a2);
if (a2==1) { SaveFuncKeyMap(dword_BED5A0); SaveQuickslotKeyMap(dword_BED658); goto LABEL_20; }
if (a2==2) { InvalidateRect(TSingleton<CUIStatusBar>::ms_pInstance, 0);  goto LABEL_20; }
LABEL_20: CWvsContext::UI_Close(*dword_BE7918, 5); return;    // 1/2 都关窗（UI_Close id=5）
if (a2==2000) { CNoticeDlg(0).DoModal()==1 → DefaultFuncKeyMap + DefaultQuickslotKeyMap + ReDrawQuickslot; }
else if (a2==2001) { CNoticeDlg(1).DoModal()==1 → memset(dword_BED5A0+4, 0, 0x1BD=445) + sub_836619 + InvalidateRect(StatusBar); }
else if (a2==2002) { sub_836C9B(this); return; }
```

`GetShortCutIndexByPos @0x833a65`：`dword_BE27E4..BE2C6C` 命中表 + `32×32` 格 + 特殊映射 `'6'→42 / 'Y'→29 / 'Z'→56`。

| 维度 | 083 Angel | 当前端 | 判定 |
| --- | --- | --- | --- |
| 按钮 id | 1/2/2000/2001/2002 原生 id | `CLOSE=0,CANCEL=1,DEFAULT=2,DELETE=3,KEYSETTING=4,OK=5` | 形态差（本地枚举） |
| 保存+关窗 | `1=Save` / `2=InvalidateRect(StatusBar)` → 两者都 `UI_Close(5)` | `OK=5 → save_staged_mappings()+close()`；`CLOSE/CANCEL=0/1 → close()` | 语义近似对齐 |
| 恢复默认 | `2000` DoModal → `Default*` + **`ReDrawQuickslot`** | `DEFAULT=2` → UIOk → `clear()` + `staged_mappings=alternate/basic_keys` | **L 桶缺口**：无跨窗重绘状态栏 |
| 清空 | `2001` DoModal → `memset(0x1BD)` + `InvalidateRect(StatusBar)` | `DELETE=3` → UIOk → `clear()` | **L 桶缺口**：无 `InvalidateRect` 等价通知 |
| 2002 | `sub_836C9B` | 无对应分支（`KEYSETTING=4` 是开 `UIQuickslotConfig` 子窗） | 语义不同，待裁决 |
| 状态栏重绘 | `InvalidateRect(TSingleton<CUIStatusBar>)` 两处 | `Invalidate` 在 `UIKeyConfig.cpp` **0 命中** | **L 桶实质缺口** |
| 保存发包 | `SaveFuncKeyMap` / `SaveQuickslotKeyMap`（本地存储 + 后续同步） | `ChangeKeyMapPacket(135)` + `ChangeQuickslotPacket(183)` | ✅ opcode 一致（`§7`）→ 保持 s9 现状；形态/文案差异记 L 桶 |

### 2.5 Equip 装备窗

**083 `CUIEquip::OnButtonClicked @0x7feb72`（现场反编译）**：

```c
switch (a3) {
  case 3000: *(*dword_BE7918+14332)=50200052; CWvsContext::SendMigrateToShopRequest(...); break;
  case 3001: CUIEquip::ToggleDragonEquip(this, a2); break;
  case 3002: CUIEquip::TogglePetEquip(this); break;
  default:   CUIWnd::OnButtonClicked(a3); break;
}
```

`ToggleDragonEquip @0x7ff8dd` / `TogglePetEquip @0x7ffa84` 现场反编译：都是 `ZAllocEx::Alloc(1408/1456)`
+ 相对父窗 `+117 / +123` 的 y 偏移 + 创建/销毁独立子窗（`CWnd::Destroy`）。

| 维度 | 083 Angel | 当前端 | 判定 |
| --- | --- | --- | --- |
| 商城入口 | `3000` → 写全局 `50200052` + `SendMigrateToShopRequest` | `BT_SHOP=2` → `EnterCashShopPacket(40)`（空 body） | ✅ opcode 一致（`§7`）→ 保持 s9 现状；形态/文案差异记 L 桶 |
| 龙装备切换 | `3001 ToggleDragonEquip` 独立子窗 | **未实现**（`Dragon` 仅 `:167` 一行 083 注释，无按钮/分支） | **L 桶实质缺口**（但需 IMG 资产核对是否有 dragon 节点） |
| 宠物装备 | `3002 TogglePetEquip` 独立子窗 | 内嵌 panel `BT_PET_SHOW/HIDE` + 3 tab（`:318-328`），非独立窗 | 形态差，083 为准则应改结构（影响面大，需裁决） |
| IMG 资产 | — | orange-wz 实测 `Equip` 有 `BtPetEquipShow` / `BtPetEquipHide` / `BtPet1..3` / `pet` / `FullBackgrnd`；**无 dragon 相关节点** | 龙装备切换**缺资产支撑** → 不宜凭 083 代码硬补 |

---

## 3. 判定汇总（按「Angel 为准」新口径）

| 类别 | 条目 | 数量 | 是否可改 |
| --- | --- | --- | --- |
| ~~W 桶（协议/wire）~~ **round101 更正** | 上述 5 类 c2s 包 opcode 全部与 s9 一致 | 5 类 | ✅ 不是断联机原因；保持 s9 现状即可 |
| **W 桶真风险**（s2c） | 6 个改造字段宽度/类型：level 2B / HP-MP int / exp 8B / GachaExp 4B / spawn tail int0 / removeItemFromMap 4B cid | 6 | ❌ 改回 083 宽度会平移包中间字段 → `LoginParser.cpp:150-174` 白屏（`CONSISTENCY-VERDICT.md:136`） |
| **L 桶（逻辑归属）** | 083 客户端本地算 AP 方案 / s9 服务端按职业算方案；500ms 门闩；SP 前置校验；tooltip；跨窗重绘 | 5 组 | ✅ **与 wire 正交，可按 Angel 还原**（`CONSISTENCY-VERDICT.md:142-144`） |
| **L 桶（本地逻辑，可校准，不动 wire）** | ① Skill 500ms 门闩 ② Skill SP 前置校验覆盖所有发包入口 ③ Item `0x7D4/7D5/7D6/7D7` 四 tooltip + 槽位合并 `SP_697` ④ KeyConfig `InvalidateRect(StatusBar)` / `ReDrawQuickslot` ⑤ Stat 三颗 `BtAuto` 资产落点 | 5 组 | ✅ 可按 Angel 补齐 |
| **形态差（本地重构，非缺陷）** | 按钮 id 枚举（2000/3000 系 vs 0 起连续）、detail toggle 三分支 vs 单 toggle、宠物装备内嵌 panel vs 独立子窗 | 3 组 | ⚠️ 语义等价或需大改结构 → 交裁决 |
| **缺资产支撑** | Equip `ToggleDragonEquip`（IMG 无 dragon 节点） | 1 | ⛔ 不硬补（门禁 9/不发明客户端能力） |
| **已对齐** | Item `SetArrangeButton @0x81e3a6`、Stat STR/DEX/INT/LUK 掩码数值、Stat detail toggle 语义 | 3 | ✅ 不动 |

**回答上一轮的问题「算术没改，还是 angel 端不完整？」**：
- 算术（168 条分类账 / reopen 27 / P3 16 条）**本轮仍不动**——#15 是单条决策点的证据深化，不新增/注销决策点。
- Angel 端**既有取证基本完整**（296 函数全文 + 六维 98 条），但**有 2 处细节缺口**已被本轮现场复核补上：
  ① `CUIItem::OnChildNotify` 是 **4 个** `SP_` 分支（既有 README §4 只记 2 个，漏 `0x7D6` 收起 / `0x7D7` 现金扩槽）；
  ② `CUIKeyConfig::OnButtonClicked` 的 `a2==2` 也走 `UI_Close(5)`、且 `2001` 有 `InvalidateRect(StatusBar)`（既有文档把这几点混在 2000/2001 描述里）。
  另有 1 处**资产层新发现**：`Stat/BtAuto/BtAuto1/BtAuto2` 三颗，印证 083 自动加点两态三钮。

---

## 4. 待决项（阶段 1：需用户拍板，本轮不擅自实现）

| # | 决策点 | 选项 | 影响面 |
| --- | --- | --- | --- |
| ~~**Q1**~~ | ~~W 桶是否真的按 083 全改？~~ | **已由 round101 证据消解**：字段类型保持用户改造后的 s9 值（两端本就一致），逻辑按 Angel 083 还原，两者正交、无冲突 | 不再是决策点 |
| **Q2** | 是否回写 `DECISIONS.md` §0 第 0 号元裁决与 `P3-C-BATCH-MAPS.md` #15 的 `KEEP` 判定？ | **A 回写**（新口径「Angel 为准」正式入账，注明 W 桶例外）；**B 只在本文件登记、不动历史裁定** | 全案 14 条 T1 基线的表述一致性 |
| **Q3** | L 桶补齐范围 | **A 全补 5 组**（含 Skill 门闩/校验、Item 4 tooltip+合并、KeyConfig 重绘、Stat 三 BtAuto）；**B 只补零风险项**（tooltip 文案 + 资产落点）；**C 本版只出清单不实现** | 实现工作量 / 回归风险 |
| **Q4** | 形态差 3 组（尤其 Equip 宠物装备改独立子窗）是否纳入？ | **A 不纳入**（记为「本地重构，语义等价」）；**B 纳入**（大改 UI 结构，需 Photoshop/IMG 出图链路） | 排版回归风险 |

**推荐组合**：Q2=A + Q3=C（先出清单）→ 再按 Plan 模式给一次性完整实施计划（门禁 1 阶段 2）。
理由：Q1 已由证据消解（用户 2026-11-12 的判断成立：字段类型是他有意调整且两端一致，不构成阻碍）；
Q3 的 L 桶补齐涉及 5 个 UI 文件的行为改动，应一次性 Plan 后再实现（避免零碎改）。

---

## 5. 复现命令

```bash
# ① IDB 校验 + 五窗方法族 + 12 锚点现场反编译（13339 = Angel v83）
python C:/Users/11036/AppData/Local/Temp/ida13339.py server_health '{}'
python C:/Users/11036/AppData/Local/Temp/ida13339_batch.py <jobs.json> <out.json>
# ② IMG 核验：orange-wz MCP load_files(128B key, GMS IV, exclusive:false) → query_nodes → unload_all
#    key = tools/_gms_key_128.b64 ; root = E:\MXD\BeiDou-Client-xy\Data\UI\UIWindow.img
# ③ s9 协议对拍
#    BeiDou-Server_s9/.../DistributeAPHandler.java:33-36
#    BeiDou-Server_s9/.../AssignAPProcessor.java:700-711
```

## 6. 约束

- 083/v83 only，禁引 095（本轮全为 `` `Angel.i64 `` v83 现场证据）
- 本轮**零源码 / 零 wire / 零 WZ 改动**，纯只读取证
- 门禁 5 禁想象修改 / 门禁 8 WZ-IMG 走 MCP / 门禁 18 REA→IDA→orange-wz 只读
- IMG 主路径 orange-wz 本次 `loadedCount=1` 成功，符合 AGENTS.md §2（未回退 `.nx`）
- 待 Q1–Q4 抉择并确认 Plan 后，方可进入实现阶段

---

## 7. round101 更正：「断联机」三层辨析（用户 2026-11-12 追问）

> 追问：「断 s9 联机 是因为我改了一些字段类型 还是说逻辑上不一致？如果是类型修改问题那是我调整的，
> 但是不影响，只要按我指定客户端的逻辑来加上我改的类型是不是就可以了？」
> **答：用户的判断成立。** 下面用现场反编译 + s9 源码坐实三层拆解。

### 7.1 三层拆解

| 层 | 内容 | 083 vs 当前端 vs s9 | 是否断联机 |
| --- | --- | --- | --- |
| ① **opcode（包种类）** | 5 类 c2s 发包 | **全部一致**（`RecvOpcode.java`） | ❌ 否 |
| ② **wire 布局（字段宽度/类型）** | `time + 2×{int,int}` 等 | **全部一致**（见 7.2） | ❌ 否 |
| ③ **逻辑归属（谁算方案）** | 083 客户端算 / s9 服务端算 | 不同 | ❌ 否（属 L 桶，纯本地行为） |
| ④ **W 桶 6 个 s2c 改造字段** | level 2B / HP-MP int / exp 8B / GachaExp 4B / spawn tail int0 / removeItemFromMap 4B cid | 用户有意改造，两端已一致 | ✅ **只有改回 083 宽度才断**（包中间字段平移 → 白屏） |

**结论**：「断联机」既不是「字段类型不一致」，也不是「逻辑不一致」——前三者都不导致断联机。
真正会断的只有 ④，而 ④ 是你自己调整、且 `PacketCreator.java` 与 `LoginParser.cpp` 两端已配套一致的字段，
**保持现状即可，不需要动**。所以「按我指定客户端（Angel 083）的逻辑 + 加上我改的类型」= **完全可行，且正是正解**。

### 7.2 五类 opcode / wire 一手对拍（现场 13339 反编译 + s9 RecvOpcode）

| 包 | 083 Angel（现场反编译） | s9 `RecvOpcode.java` | 当前端 `OutPacket.h` | 一致 |
| --- | --- | --- | --- | --- |
| 手动加点 | `COutPacket(87)` `@0xa239b7` | `:101 DISTRIBUTE_AP(0x57)` | `SPEND_AP=87` | ✅ |
| 自动加点 | `COutPacket(88)` `@0xa23b3d` | `:102 AUTO_DISTRIBUTE_AP(0x58)` | `AUTO_DISTRIBUTE_AP=88` | ✅ |
| 整理/排序 | `a3==100` 派发 | `:85/86 0x45/0x46` | `69 / 70` | ✅ |
| 改键 | `SaveFuncKeyMap` | `:141 CHANGE_KEYMAP(0x87)` | `135` | ✅ |
| 商城 | `SendMigrateToShopRequest @0xa04dca` | `:58 ENTER_CASHSHOP(0x28)` | `40` | ✅ |
| 技能加点 | `CUISkillEx COutPacket(165)`* | `:104 DISTRIBUTE_SP(0x5A)=90` | `SPEND_SP=90` | ⚠️ 165 是引导/宏子包，非 SP 加点主包，单独复核 |

**手动加点载荷**（`SendAbilityUpRequest @0xa239b7` 逐字）：
```c
COutPacket::COutPacket(87);  COutPacket::Encode4(get_update_time());
COutPacket::Encode4(a2);     CClientSocket::SendPacket(...);
```
= 当前端 `SpendApPacket(87)` 的 `write_time(); write_int(codes[stat]);`，**字节布局相同**。

**自动加点载荷**（`sub_A23B3D @0xa23b3d` 逐字）：
```c
COutPacket::COutPacket(88); Encode4(time); Encode4(count);
for (i=0; i<count; ++i) { Encode4(type); Encode4(amount); }
```
- s9 标准分支 `AssignAPProcessor.java:122 skip(8)` + `:442-449 for(i<2){ type=readInt(); val=readInt(); gainStatByType(...) }`
- 当前端 `PlayerPackets.h:65-70 write_time(); write_int(0); write_int(first); write_int(first_val); write_int(second); write_int(second_val);`
→ **三者同为 `time + 2×{int,int}`**；083 的 `count` 恰好落在 s9 `skip(8)` 的第二字，当前端写 0 也能被跳过。

### 7.3 真正的差异在逻辑归属（L 桶，与 wire 正交）

- **083**：`CUIStat::AutoApUp @0x8c8692` 本地调 `sub_7939BA`（自动分配方案表）→ 按 `64/128/256/512`
  clamp 累加 → 弹 `SP_1951` 确认框展示方案 → 用户确认才发 88 包。**客户端算方案**。
- **s9**：`APAutoAssignAction` 有 `use_server_auto_assigner` 开关；开启时 `:178 getJobStyle(opt)` 按职业
  智能算方案（考虑装备需求），**服务端算方案**。
- 这是 `CONSISTENCY-VERDICT.md:142-144` 已裁定的「L 桶同字段原始逻辑按 Angel 083 还原照常触发，与 wire 层正交」。

### 7.4 500ms 门闩更正

`CanSendExclRequest @0x485bf7` 现场反编译：
```c
return *(this+2089)==0 && (a3!=0 || _ZtlSecureFuse<short>(...) > 0)
       && get_update_time() - *(this+2090) >= a2;   // a2 = 500
```
`SendAbilityUpRequest` 与 `sub_A23B3D` **开头都调 `CanSendExclRequest(500,0)`**。
→ round100 §2.1 写「083 无时间门闩」是错的；083 Stat/Skill 两侧都有 500ms 门闩，
当前端只有 `UI::get().disable()` → **这是确凿的 L 桶缺口，可按 Angel 补齐且不动 wire**。

### 7.5 round100 误判原因与影响

误判根因：把「`CWvsContext::SendAbilityUpRequest` / `SaveFuncKeyMap` / `SendMigrateToShopRequest`」
这些 **083 客户端内部方法名** 当成了「另一套协议」，未反编译其函数体就判定为 W 桶冲突。
本轮补反编译 5 个函数体后全部证伪。**影响**：§0、§2.1、§3、§4-Q1 的「W 桶冲突/断联机」表述已就地更正；
§2.2–2.5 中「W 桶不同 → 不可直接改」的措辞同样应理解为「opcode 一致、保持 s9 现状、差异记 L 桶」。
证据落盘 `evidence/p3-item15-disconnect-triage-round101.json`。

## 8. round101 实施落点（2026-11-12 按计划落地）

> 用户「那就按计划实施」后，将 §3 / §7 的 L 桶项落到当前端源码；W 桶（6 字段宽度等）全程零改动。

### 8.1 已落地（源码 + `tools/build_smoke.bat` EXITCODE=0 通过）

| # | L 桶项 | 083 依据 | 当前端改动 | 文件 |
| --- | --- | --- | --- | --- |
| L2 | Stat/Skill 500ms 独占门闩 | `CanSendExclRequest(500,0)` @0x485bf7（§7.4） | `send_apup` / 自动加点 `AutoAssignApPacket` 派发前、`UISkillBook::send_spup` / `spend_sp` 前加 `steady_clock` 500ms 本地节流（与 `UI::get().disable()` 并存，纯本地、不动 wire） | `UIStatsInfo.cpp/.h`、`UISkillBook.cpp/.h` |
| L3 | Skill SP 前置校验覆盖所有发包入口 | 083 三档 `SP_3509/3510/3511` 本地门控 | `send_spup`（资源缺失直发路径）与 `spend_sp` 发包前调 `can_raise(skill_id)` | `UISkillBook.cpp` |
| L5 | KeyConfig 改键后状态栏跨窗重绘 | 083 `CUIStatusBar::ReDrawQuickslot` / `InvalidateRect` | `clear()`（DEFAULT/DELETE/reset 经它）与 `save_staged_mappings()`（OK/safe_close 经它）后调 `UIStatusBar::redraw_quickslots()` | `UIKeyConfig.cpp`、`UIStatusBar.h` |
| L6 | Stat 三颗 `BtAuto` 资产 | 083 `OnCreate` 不创建 `BtAuto`/`2007/2008/2009`，由 `RestoreButtons`/状态机创建；`OnButtonClicked` 仍处理 `2007/2008→AutoApUp(1)`、`2009→AutoApUp(0)` | 当前端 `UIStatsInfo.cpp` 已实现单 `BT_AUTO` 增强（注释明「原生基准面无此件、不存在与原生不一致」），保留为本地增强，**零新增** | `UIStatsInfo.cpp` |
| L4 | Item 整理/聚集/展开/收起/槽位合并悬停提示 | 083 `CUIItem::OnChildNotify` a3==101（0x7D4/7D5/7D6/7D7/2003） | **文案已加**（`locale/zh-CN.ini` + `en-US.ini` 的 `inventory.tip_*`）；逐帧悬停 wiring 因现有 `send_cursor` 的 item tooltip 分支未联机验证，**标注待联机核验**，不盲改以免破坏现有道具 tooltip | `locale/zh-CN.ini`、`locale/en-US.ini` |

### 8.2 口径变更回写（L7）

- `DECISIONS.md` §0：追加 round101 W/L 桶三层辨析补正。
- `DECISIONS.md` §4.3 `corewindow/D8`：裁定状态 `DOC_ONLY` → `L_BUCKET_DONE`。
- `P3-C-BATCH-MAPS.md` #15（= `corewindow/D2`）：注明 W 桶 KEEP、L 桶已落地 D8。
- `P3-ANGEL-REBUILD-PLAN.md` §9 #15：见 §8.3。

### 8.3 与 P3-ANGEL-REBUILD-PLAN §9 的关系

§9 #15 维持 `KEEP`（W 桶 s9 协议优先，与 `corewindow/D2` 判定一致）；本轮 L 桶落地项归属 `corewindow/D8`，不推翻 #15 的 W 桶 KEEP。

### 8.4 构建与验收

- `tools/build_smoke.bat`：`EXITCODE=0`、`GATE OK`；`MapleStory.exe` 已刷新至 `E:\MXD\BeiDou-Client-xy\`。
- 注：3 个运行中 `MapleStory.exe`（含已知僵尸 34476）无法从本会话 `taskkill`（Access denied），磁盘 exe 已更新但运行实例仍为旧镜像，需用户重启客户端生效。
- 真机联测（500ms 门闩手感、SP 校验提示、改键后状态栏刷新、Item 悬停提示）待用户屏前复拍，本环境历史抓屏全黑不假装已验证。
