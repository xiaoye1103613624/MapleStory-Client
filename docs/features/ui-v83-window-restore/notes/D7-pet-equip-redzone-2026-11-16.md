# D7/D7b 不可用槽红色区域 —— 完整性核查、补齐与真机验收（2026-11-16）

## 结论速览（最终态）

| 部分 | 状态 | 证据 |
| --- | --- | --- |
| 角色主窗 2 格（骑兽 5,230 / 鞍子 38,230） | ✅ 完整（既有） | D7b 原生帧自证 99/100%，真机 Δ=1/0；`slot_available()` 运行期谓词；真机粉 397/395 每格 |
| 宠物面板 5 格 | ✅ 本批补齐 + 真机验收 | 离线逐像素 5120/5120=100%；真机 5 格全粉 3821/5120（格内文字/图标覆盖所致）|
| 主窗第 7 行第 3 格 `SHOULDER`（护肩） | ✅ 按用户裁决 m00717 定为**可用槽，不 tint**，并**改位到盾牌上方 (137,98)** | 真机 (137,98) pink=0、正常显示；旧位 (71,230) 不再泛红 |
| 盾牌 137,131 | ✅ 完整（既有） | `slot_available()` 看 `is_twohanded()`；本角色非双手武器故真机未触发粉色（符合预期）|

> 用户裁决（m00717，2026-11-16）：「护肩位置可用，同时把护肩槽位改到盾牌上方。」——护肩是**可用槽**，不是不可用槽，绝不能染红；图标从 (71,230) 移到盾牌正上方 (137,98)。本批据此定案，取代下文 §2 早期的「归属未定/待判据」推演（保留作为取证链）。

## 1. 宠物面板 5 格补齐（本批实装）

### 1.1 取证链（Angel IDA + orange-wz MCP 双源）
- **orange-wz MCP**（`Equip/pet` 128B DEFAULT_KEY，origin=(0,0)，PNG 177×181 ARGB8888）：底图所有槽**全蓝** `RGB(187,204,221)`，**没有任何粉色** ⇒ 粉色是运行期逐像素改色，与主窗 `Equip/backgrnd` 同构。历史 D7「命中率 34.7~77.2%、偏移没尺、等展开帧」的前提（以为要新美术/新判据）不成立。
- **Angel IDA**（行为真源）：`CUIPetEquip::OnMouseMove @0x800f7b` 用 `GetItem(..., -100-v6)` 取宠物装备、`dword_BED5A0+960/+964` 判宠物数；`OnButtonClicked @0x8019f1` 仅切页（`+360=页号`+`InvalidateRect`）⇒ 判据在绘制态，点击不改可用性。
- **截图**（`角色装备.png`，未召唤宠物态）精确网格：列 x=185/218/251/284 step33，行 y=134/167/200/233；5 个粉格 canvas 局部 `(46,11)(112,11)(112,77)(79,110)(112,110)`，32×32，主色精确 `RGB(204,153,170)`；`tint((187,204,221))=(204,153,170)` 逐位相等。

### 1.2 实装（分支 feature/dev_0.0.1）
- `IO/UITypes/UIEquipInventory.h:120-126`：`PET_TINT_CELLS[5]`、`pet_disabled_cells`、`build_pet_disabled_cells()`、`pet_slot_available() const`。
- `IO/UITypes/UIEquipInventory.cpp:231` 构造建格；`:238-256` draw 中 `pet_panel_open` 时按 `!pet_slot_available()` 画 5 格；`:611-624` 复用 `build_tinted_regions()` 裁格；`:627+` 谓词：当前宠物页无召唤宠物 → 5 格转粉，召唤后恢复蓝（与 Angel 宠物数判据同构）。
- 复用 `Graphics/MemoryTexture.cpp:121-146/174-234` 的整数核，保证与主窗改色逐像素同机制。

### 1.3 逐像素验证（离线）
- `tools/_pet_tint_verify_exact.py`：5 格各 **1024/1024 = 100%**，GRAND **5120/5120 = 100.000%**。
- `tools/_pet_fullpanel_verify.py` / `_pet_fullpanel_alpha.py`：整张 177×181 canvas 重建，`tint-cell mismatches = 0`（零误染、零漏染）；其余差异集中在 y≥144 的页签按钮/边框（独立 sprite）与半透明像素合成，与本次改动无关。
- `tools/_pet_mismatch_map.py`：空间分布证实差异不在格区。

## 2. 护肩 (SHOULDER) —— 用户裁决「可用槽、不 tint、改位到盾牌上方」

### 2.1 归属取证链（保留，支撑裁决）
- **几何定位**：`tools/_ida_slot_array_decode.py` 现场 dump Angel `dword_BE2264`，小端 i32 解出第 7 行三格 `(5,233)(38,233)(71,233)` = 槽号 **18/19/20**，与本端 `TAMEDMOB=18/SADDLE=19/SHOULDER=20` 完全一致 ⇒ 第三格确实是 SHOULDER 正式槽（非幻影）。`sub_7FEC32 @0x7fec32` 槽号命中逻辑、`sub_7FE4C6 @0x7fe4c6` 拖放 handler 均无染色谓词；`CUIEquip` 类无 `CUIEquip::Draw`，染色由 WZ canvas filter + 角色数据生成。
- **早期推演**（已被裁决取代）：r27 文档 `evidence/r27-b3-equip/README.md:209/:211/:315` 禁止猜填、要求交互取证；三张静态截图该格恒粉无对照帧 ⇒ 早期判断「无运行期判据、不实装 tint」。

### 2.2 最终裁决与实装（m00717）
- 用户明确：护肩是**可用槽**（护肩位置可用），不得染红。⇒ 护肩**不进** `TINTABLE_SLOTS={TAMEDMOB,SADDLE,SHIELD}`，`slot_available(SHOULDER)` 恒 true（保持底图原样）。无需任何 tint 谓词——这从根上消除了早期「判据缺口」。
- 护肩图标改位：`IO/UITypes/UIEquipInventory.cpp:139` `iconpositions[SHOULDER] = Point<int16_t>(137, 98)`（原 (71,230)）；(137,98) 是 WZ `Equip/backgrnd` 合法空格（`tools/_check_cell_137_98.py`：978/1024 白底蓝边），位于 SHIELD(137,131) 正上方、与 RING4(137,65) 同列。

## 3. 构建与部署

- 最终构建：`verify_out/MapleStory.exe` SHA `4ed28a2cc19cde113e662d4cf507673dc28bd351b092c38b4966ddbe9ea73312`（含宠物 5 格 tint + 护肩改位），`verify_int_shoulder/UIEquipInventory.obj` mtime > cpp mtime，编译错误 0。MSBuild 全新 IntDir 规避增量陷阱；首轮 `fatal error C1090 PDB API 调用失败` 为并发/瞬时，重试成功。
- 部署：rename-swap 方案（目标 exe 被只读句柄占用，不可原地写但 `FileShare.Delete` 允许改名）→ 原子改名顶上。live 目录被并发会话反复重写（曾出现 F55F3667），验收时以 verify_out 构建重新部署并锁定 SHA=4ED28A2C 后启动。

## 4. 真机验收（2026-11-16，通过）

- 工具链：`tools/_equip_pet_final3.py`（一键 kill→部署→启动→反复 E 开装备窗至蓝格出现→点 BT_PET_SHOW→capture-shot.ps1 截图→逐格比对）。注意：`capture-shot.ps1` 用 `GetClientRect+ClientToScreen` 抓真实客户区，硬编码屏幕原点会抓错区域；客户端需多次 E 切换才展开装备窗（try0 blue=2538 关闭态，try1 blue=9855 展开态 winScore=8.7）。
- **宠物面板 5 格**（`f3-petpanel.png`，网格分析 `tools/_final_grid_analysis.py`）：宠物HP 767/1024、宠物MP 811、宠物装备 751、宠物戒指×2 746/746，总 **3821/5120** 精确 tint 色 `RGB(204,153,170)`；主人功能格（自动HP/MP/拣物等）保持蓝 `RGB(187,204,221)`，零误染。真机未达 100% 是格内槽名文字/图标覆盖所致（离线纯底图 100%）。
- **主窗 row7**：TAMEDMOB(5,230) 粉 397、SADDLE(38,230) 粉 395；旧护肩位 (71,230) pink=0（不再泛红）。
- **护肩新位 (137,98)**：client(387,258) pink=**0**、正常显示 ⇒ 可用槽不染红，裁决落实。
- 证据图：`docs/features/ui-v83-window-restore/evidence/f3-petpanel.png`、`f3-equip-try1.png`、`final2-*.png`、`redzone-live-*.png`、`petverify-*.png`。验收后已 `client_config.py --off` 恢复配置并关闭测试客户端。

## 5. 遗留（非阻塞）
- `tools/test_round78_equip_window_083.py` 的「0 行为改动」守卫 A4 断言在本次真实改动后会 FAIL，属预期；如需保留该门禁，应更新其 baseline 口径。
- 宠物槽穿戴逻辑仍未接（`Equip/pet` 下无 Slots 子节点，属协议批次预留），与本批「不可用槽红色区域」无关。
