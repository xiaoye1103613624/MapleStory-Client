# round78 · UIEquipInventory 装备窗宠物区按 Angel 083 只读取证复核（2026-11-11）

## 1. 目标与触发
- 用户 m09211「全部继续，如果能自测的可以自己启动测试」+ m04292/m09859/m13177 硬指令链；round86 `bdfd8f40` 批量裁决销案后，收口历史挂账的 round78 装备窗。
- 本轮**不重写架构、不改任何行为字面量**：只把 083 依据链补进代码注释并交付四文档 + harness，真机观感项交用户认领。

## 2. 三维取证（门禁 18：REA→IDA→orange-wz→合规截图备选）
- **orange-wz 128B 门禁**：`tools/_equip_retest_128.py` 直连 `http://127.0.0.1:10012/mcp`，`userKeyBase64`=128B `WzAESConstant.DEFAULT_KEY`（`tools/_gms_key_128.b64` 172 字符）、iv `TSPHKw==`，`UIWindow.img/Equip` 返回 10 个真节点、9 个关键节点全 true、`isError:false`。旧 `tools/_equip_probe_stdout.txt` 的 4 处 `IMG 解析失败` 是 65536B NoLife 全量表错规格假阻塞（round85 `4b2452c4` 已翻案），真因 `WzMutableKey.java:84-90` 走「128B→32B AES 派生」，与 `.nx` 全量 XOR 表互不通用。
- **083 活 IDA**（`tools/_ida_equip/`）：`CUIEquip` 8 符号 + `CUIPetEquip` 2 符号；`CUIEquip::OnButtonClicked @0x7feb72` case 3000 商城 / 3001 `ToggleDragonEquip @0x7ff8dd` / 3002 `TogglePetEquip @0x7ffa84`；`TogglePetEquip` 用 `ZAlloc 1456B` 建独立 `CUIPetEquip` 子窗（父窗偏移 +123）；`CUIPetEquip::OnButtonClicked @0x8019f1` case 2000 收回 / 2001-2003 三页签 `InvalidateRect`；`sym2_*OnCreate.json` 均 `total=0` ⇒ **无布局立即数**。
- **当前端逐行**（`IO/UITypes/UIEquipInventory.cpp` 672 行）：真资源根 `UiAssetResolve::window4("Equip")`；`backgrnd 175x304` + 5 列 33 步进 iconpositions 表；宠物区 `Texture(Equip["pet"]) 177x181` 内嵌、`pet_panel_pos=(right-3,bottom-h)`、BT_PET_SHOW/HIDE 开合、BT_PET_TAB0/1/2 三页签；截图 SAD 坐标补全 083 无立即数处；`BtPet1/2/3 origin (15,13)` 只记不改待原生帧。

## 3. 定性：合法混合面反例（排除虚构根同族病）
- **真资源根**：10 个 `Equip` 节点经 128B 门禁全部存在 → 不是「虚构根 / 全文字自绘」的第 N 例同族病（对照 round76 UIGuild `GuildUI.img` 五证为 0）。
- **截图 SAD 合法**：083 无 `OnCreate` 符号 ⇒ 无布局立即数，按门禁 18 截图仅作备选、补立即数缺口，非「自造版式」。
- **架构差异非缺陷**：083 是独立 `CUIPetEquip` 浮窗，本端是内嵌宠物面板；两者交互语义（开合 + 2001-2003 三页签 → BT_PET_SHOW/HIDE + BT_PET_TAB0/1/2）对齐，属 Journey 架构适配。按「校准≠改回原生 / s9 优先」不重写。

## 4. 实施（0 行为字面量）
- `UIEquipInventory.cpp` 宠物区注释前插入 17 行 083 依据纯注释（资源根 / IDA 链 / 无 OnCreate / 定性）；EOL 保持 BOM+LF 无尾换行。
- 四文档：`evidence/equip-window-083-round78.json`、`notes` 本文件、dev-plan §4 `| **2-34** |` 行、PLT `## 45.` 五屏归口（CRLF 无尾空行）。
- harness `tools/test_round78_equip_window_083.py`：静态守卫（083 EA / 128B 十节点 / OnCreate=0 / 0 行为行 / EOL）+ 场景（三页签真值表 / 独立子窗 / 合法混合面判定）。

## 5. 验收与遗留
- 自测：`gen_opcode_matrix.py --check`（GATE OK `s9 a78ba681a4`）+ `build_smoke.bat`（EXITCODE=0，touch cpp 真重编覆盖）+ harness 全绿 + `git diff --check` 干净。
- 提交：精确 add、零署名、hunk 级隔离外部 11 个源码 WIP。
- 遗留（用户认领）：§45 四项真机观感（展开帧定位 / 三页签 origin / BtDetail·BtCashshop 默认隐藏 / 独立子窗 vs 内嵌面板交互）；§44 的 60 节 / 142 处真机标记与五屏仍全 open。
