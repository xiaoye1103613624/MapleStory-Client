# Wave2 2-13 · UIItemInventory 资源驱动 slot_col 除零旁路守卫销案（round45 / 2026-11-10）

## 0. 性质与授权

- 用户 m04292 授权：当前客户端「现有功能补充、错误逻辑修复、各 ui 拼接、动画、交互一并修复」；可自测的自己测、测完提交、不能自测的记 backlog。本轮沿用该一次性 Plan（`README.md` §4 Wave2），不重开 Plan、不进入 Wave3 L-B（§3 门控）。
- 目标源仍是 Angel(13339) 083/v83 带符号 IDB 逐功能校准；本轮取证链为 **REA → live 源码构造点 → orange-wz(IMG) 门禁 → `.nx` 运行时 probe**，全程只读取证（校准确认）+ 单文件 L-A 最小修复（m04292 授权），不触碰协议/包体/WZ/IMG/XML 内容。
- 第 12 例同族病（round34 UIWorldSelect → 35 UICharSelect → 36 Textfield → 37 UINpcTalk → 38 UIRaceSelect → 39 UIGuildBBS → 40 MOVE_SUMMON → 41 入口遮蔽 → 42 无符号下溢 → 43 空串 UB → 44 裸 `std::stoi` → 45 资源驱动整数除零）：**主体路径有兜底、旁路裸信资源**。

## 1. 缺陷逐字与事实链

| 环节 | 逐字证据 | 来源 |
| --- | --- | --- |
| 多资源入口（非死代码） | `nl::node Item = UiAssetResolve::window("Item");` | `IO/UITypes/UIItemInventory.cpp:131` |
| window() 明确双路径 | `nl::node modern = nl::nx::UI["UIWindow2.img"][name]; if (modern) return modern; nl::node legacy = nl::nx::UI["UIWindow.img"][name]; ... return legacy;` | `Util/UiAssetResolve.cpp:42-55`；`Util/UiAssetResolve.h:20-24` 注释 `Prefer UIWindow2.img[name], else UIWindow.img[name]` |
| 旁路裸赋值 | `nl::node pos = Item["pos"];` → `if (pos) { slot_col = pos["slot_col"]; slot_pos = pos["slot_pos"]; slot_row = pos["slot_row"]; slot_space_x = pos["slot_space_x"]; slot_space_y = pos["slot_space_y"]; }` | `IO/UITypes/UIItemInventory.cpp:140-149` |
| 主体兜底 | `else { slot_col = 4; slot_row = 6; slot_space_x = 4; slot_space_y = 2; slot_pos = Point<int16_t>(8, 50); }`（`:152-158` 853d4568 位图实测长注释） | `IO/UITypes/UIItemInventory.cpp:150-164` |
| 衍生量 | `max_slots = slot_row * slot_col;` / `max_full_slots = slot_col * max_slots;` | `IO/UITypes/UIItemInventory.cpp:166-167` |
| 三处整数除法 | `int16_t totalrows = inventory.get_slotmax(tab) / slot_col;` / `uint16_t row = slotrange.at(tab).first / slot_col;` / `slider.setrows(row, slot_row, inventory.get_slotmax(tab) / slot_col);` | `IO/UITypes/UIItemInventory.cpp:310`, `:689`, `:690`（全仓 ops_grep 确认仅此 3 处除法 + 2 处赋值 `:144`/`:159`） |
| 未初始化成员 | `int16_t slot_col;` / `int16_t slot_row;`（无类内初值，运行时赋值） | `IO/UITypes/UIItemInventory.h:157-158` |
| nlnx 库语义 | `int64_t get_integer(int64_t = 0) const;` → `node::get_integer(int64_t def) const { if (!m_data) return def; switch ... case type::none/vector/bitmap/audio: return def; ... }`；`operator signed short()` 走 `get_integer()` | `includes/NoLifeNx/nlnx/node.hpp:104`, `:116-118`；`includes/NoLifeNx/nlnx/node.cpp:161-179` |
| 结论 | `pos` 节点存在但 `slot_col`/`slot_row` 子节点缺失或非整数 → `get_integer()` 返回默认 **0** → `slot_col == 0` → `:310`/`:689`/`:690` 整数除零、`max_slots` 归零，构造或切页签直接崩 | 同上合成 |
| git 历史 | `:144` = `16d01198 (xiaoye 2026-10-21 19:35:10 +0800)`；`:310` 属 `df31fb1c` | `git blame -L 144,144` / `-L 306,312` |

证据优先级：`opcode-matrix.json` > live 源码构造点/调用点 > nlnx 库语义 > git blame > 历史台账。

## 2. 除零 / 模零族 153 命中全量排除（换族扫描记录）

本轮从「入口遮蔽 / 下溢 / 空串 UB / 裸转换」换到正交算术 UB 族。python 正则 `[/\%]\s*(标识符或 .size())` 扫 `IO Gameplay Character Data Graphics Net Util/**/*.cpp`（去字符串/行注释/include）→ `TOTAL_HITS 153`。只认「运行时可变 + 整数除法 + 无守卫」三条全中；浮点 `/0` 走 IEEE inf 不崩先剔。

| 分组 | 逐字排除 | 结论 |
| --- | --- | --- |
| 编译期常量 | `IO/Cursor.cpp:107 constexpr HIDE_AFTER = HIDE_TIME / Constants::TIMESTEP`；`IO/UITypes/UIChannel.cpp:120,125,131,136` + `IO/UITypes/UICharSelect.cpp:1396` 用 `SLOTS_PER_ROW`；`IO/UITypes/UICharSelect.cpp:144,158,951,984` 用 `PAGESIZE`、`:851` 用 `COLUMNS`；`IO/UITypes/UIItemInventory.cpp:1783-1786` 用 `constexpr PAGE_COLS=4 / PAGE_ROWS=6`；`IO/UITypes/UIMaker.h:78 int16_t row_h = 56;` → `:151/:152/:182` 全安全 | 分母恒正且编译期可见 |
| 上游守卫（整数） | `IO/UITypes/UIBossHP.cpp:71 max_hp = max > 0 ? max : 1;` → `:76 (int64_t(cur_hp)*100)/max_hp`；`IO/Components/MapleFrame.cpp:38-39 xtile/ytile = std::max<int16_t>(..., 1)` → `:44-45`；`IO/Components/NameTag.cpp:104 if (body_width <= 0 || width <= 0) return;` + `:107 std::max<int16_t>(1, ...)` → `:109`；`IO/UITypes/UIFamily.cpp:166 if (defs.empty()) return;` → `:168 n>=1` → `:169 % n`；`IO/UITypes/UIMiniMap.cpp:262-265 mag clamp [0,8]` → `:267 scale=pow(2,mag)∈[1,256]`、`:272 scale=16` 兜底 → `:356/:826` | 分母恒正 |
| 浮点 / 守卫 | `IO/Components/Slider.cpp:123-136 if (rowmax > 0) ... else rowheight = 0;` + `:134 if (rowmax > 0 && rowheight < 1) rowheight = 1;`（`:340 double yoffset / rowheight` 是 double）；`IO/LoginBackground.cpp:69 if (!tween_active || tween_duration == 0) return ...` + `:77 double t`；`IO/Components/IconCover.cpp:26 if (duration <= TIMESTEP) scalestep=1.0f;` + `:29 float`；`IO/UITypes/UICashShop.cpp:928 if (item.discount_price > 0 && price > 0)` + `:935 float_t percent`；`IO/UITypes/UIMapInfoPanel.cpp:622` / `UIMapMobPanel.cpp:273` `static_cast<float>` | 非整数除零崩路径 |
| Point 分量 | `Template/Point.h:208-214 constexpr Point<T> operator / (Point<T> v) const { return { a / (v.a == 0 ? 1 : v.a), b / (v.b == 0 ? 1 : v.b) }; }` → `UIMiniMap.cpp:821/832 / Point<int16_t>(2, 0)` | 分量零兜底，假阳性 |
| **唯一剩余** | `slot_col = pos["slot_col"]`（`if(pos)` 旁路）→ `:310/:689/:690` | **本轮立项** |

### 2.1 orange-wz 门禁与合规 `.nx` 回退（AGENTS.md ACTIVE）

- 先 MCP：`mcp__orange_wz__list_loaded_roots` → `Error POSTing to endpoint: {"jsonrpc":"2.0","error":{"code":-32001,"message":"会话不存在或已过期"}}`；`mcp_connector_status` 仍显示 `[custom-orange_wz] orange-wz (orange_wz) manual/none/streamable-http · 已连接 · 全局 → http://127.0.0.1:10012/mcp`。python 直连同端点 `initialize` → 200，serverInfo `orange-wz-http-mcp v1.157.49`，protocolVersion `2025-03-26`，`Mcp-Session-Id` 收口。
- key 规格（AGENTS.md 硬约束）：`tools/_gms_key.b64` = 87384 bytes base64 → raw 65536B 完整 `key_gms`，`raw_sha256_16 = 268a8407a47ee76c`；`ivBase64 = TSPHKw==`（GMS 4B `[0x4D,0x23,0xC7,0x2B]`）；`load_files exclusive:false`。
- `load_files(paths=[E:\MXD\BeiDou-Client-xy\Data\UI])` → `{"loadedCount":1,"rootCount":1,"exclusive":false,"generation":1}`；`list_children(nodePath="")` OK，含 `UIWindow.img`（**结构层 WORKS**）。
- `get_node_tree_json(rootPath=Data\UI, nodePath="UIWindow.img/Item/pos", maxDepth=3, autoParse:true)`、`search_node(keyword="slot_col", searchIn="name")`、单独 load `.img` 后下钻 → 全部 **isError true `IMG 解析失败: UIWindow.img`**（内容层 FAILS；GBK 原始错误文本按字节留存，UTF-8 误读呈 `IMG 解析失败` 乱码，hex `494d4720e79199efbd86...`）。`unload_all` → `{"rootCount":0,"ok":true}`。
- 命中 AGENTS.md 合规回退条件 → `.nx` dump + 运行时 probe：`tools/nx_inspect.py`（PKG4 只读 inspector）作用于 `E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx`（86919577 bytes）。
  - `UIWindow.img/Item/pos` → `NOT FOUND`；`UIWindow2.img/Item/pos` / `UIWindow2.img/Item` / `UIWindow4.img/Item` 均 `NOT FOUND`。
  - `UIWindow.img/Item` depth1 子节点逐字：`BtCashshop / BtCoin / BtFull / BtGather / BtSmall / BtSort / FullBackgrnd(bitmap 603x289) / New / Quality / Tab / activeIcon(bitmap 36x36) / backgrnd(bitmap 175x289) / bossPetIcon(bitmap 34x38) / disabled(bitmap 32x32) / shadow(bitmap 26x6)` —— **无 `pos` 子节点**。
- **诚实可达性**：当前 shipped v83 包走 `else` 分支 `slot_col = 4` 恒正，`if(pos)` 旁路连同除零**当前不可达**；但 `UiAssetResolve::window()` 明确支持 `UIWindow2.img`/`UIWindow.img` 多资源路径，换带 `pos` 且 `slot_col`/`slot_row` 缺失或为 0 的资源包即触发。按 round42/43/44 同族以「防御性旁路守卫 + 诚实标注」立项，**不把当前不可达虚报成当前必崩**。

## 3. 最小修复（`1 file changed, 14 insertions(+)`, `DIFFCHECK_CLEAN`）

- 位置：`IO/UITypes/UIItemInventory.cpp:142-149 if(pos)` 块 5 行裸赋值之后、`}` 之前。
- 新增 5 行依据注释（逐字）：「子节点缺失/非整数时 nlnx `get_integer()` 返回默认 0（`includes/NoLifeNx/nlnx/node.cpp:161-179`），`slot_col` / `slot_row` 为 0 会让 `:310` / `:689` / `:690` 三处整数除法除零、`max_slots` 归零，构造或切页签直接崩。整组回退到与下方 `else` 相同的 v83 BeiDou 位图实测常量；合法带 `pos` 的资源包（col>0 且 row>0）逐字保留、不触发本分支。」
- 守卫：`if (slot_col <= 0 || slot_row <= 0) { slot_col = 4; slot_row = 6; slot_space_x = 4; slot_space_y = 2; slot_pos = Point<int16_t>(8, 50); }`。
- 选择**整组回退**而非只 clamp `col`：row 为 0 时 `max_slots=0` 同样坏；`<= 0` 顺带挡负数资源值。合法包零改动 happy-path，不改协议/包体/WZ/IMG/XML。
- apply 实证：`OLD_FOUND 1` / `CRLF True` / `WROTE_OK bytes 68304 -> 68931` / `git diff --check HEAD` = `DIFFCHECK_CLEAN`。

## 4. 自测与门禁

| 项 | 命令 | 结果 |
| --- | --- | --- |
| 静态 + 场景 harness | `python tools/test_iteminventory_slotcol_guard.py` | `STATIC GUARDS: 17 checked (+3 count guards), 0 failed`；`PASS: 17 static guards + 13 scenarios (before DIV0 9 / BAD 6 -> after BAD 0; regression 7/7)`；`TEST_RC=0` |
| 场景覆盖 | S1 全空 / S2 缺 col / S3 缺 row / S4 col=0 / S5 col=-1 / S6 row=0；R1 col=4,row=6 / R2 12,6 / R3 5,8 / R4 6,4 / R5 1,1 / R6 8,12 / R7 当前 shipped 无 pos 走 else | before `div0=9 bad=6` → after `bad=0`，回归 7/7 |
| opcode 门禁 | `python tools/gen_opcode_matrix.py --check` | `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)` |
| 空白检查 | `git diff --check HEAD` | `DIFFCHECK_CLEAN` |
| 构建 | `tools\build_smoke.bat`（`MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c`） | `BEFORE 03:00:47 / 3521B` → `AFTER 03:29:29 / 9426B`；`BUILD_RC=0` + `GATE OK` x2 + `EXITCODE=0` + `ERROR_LINES 0`；tail `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe` + 3 行「已复制 1 个文件」+ `LNK4098 LIBCMT` / `LNK4099 vc120.pdb` / `/LTCG` warnings → 真重编译 |

`taskkill /F /IM MapleStory.exe /T` 对 34476/6108/56016 三个 32K 空壳 `Access is denied`（历史同款，不占资源、不阻断构建）。

## 5. 范围边界与 [PUA生效 🔥] C4701 主动披露

- 本轮只改 `IO/UITypes/UIItemInventory.cpp` 的资源旁路守卫；不动 `tab` 状态机、不动 `button_pressed` 键位链、不动任何 WZ/IMG/XML 节点内容、不动 s9  opcode / 包体。校准十一面 README 只读，未改。
- **C4701 主动发现**：构建日志暴露 `IO/UITypes/UIItemInventory.cpp(1194): warning C4701: 使用了可能未初始化的局部变量"newtab"`（同文件同轮编译，非 error）。逐字读控制流：
  - `IO/UITypes/UIItemInventory.cpp:1170 InventoryType::Id newtab;`（局部变量，shadow 成员 `IO/UITypes/UIItemInventory.h:154`）；`:1172-1192 switch(tab)` 6 case（EQUIP→USE / USE→ETC / ETC→SETUP / SETUP→CASH / CASH→DEC / DEC→EQUIP）**无 default**；`:1194 button_pressed(button_by_tab(newtab));`。
  - `Character/Inventory/InventoryType.h:27-38 enum Id : int8_t { NONE, EQUIP, USE, SETUP, ETC, CASH, DEC, EQUIPPED, LENGTH };`；构造 `IO/UITypes/UIItemInventory.cpp:126 tab(InventoryType::Id::EQUIP)`；全仓 `tab =` 赋值点 `:516-541` 全是 6 合法页签值（`:1531 newtab = NONE` 是成员 newtab、非 tab）。
  - 判定：运行时 `tab` 恒为 6 合法枚举之一、switch 全覆盖 → 局部 `newtab` 恒被赋值；C4701 是 MSVC 对 `int8_t` 枚举的保守告警，**无任何代码路径可达未初始化 → 保守误报，不立项、不扩大本轮改动面、不虚报已修**。
  - 处置：本节披露 + 最小消警做法（`switch` 加 `default:` 或 `InventoryType::Id newtab = tab;`）交 `pending-live-tests` §24 backlog，后续轮次按数据决定。比 slot_col 更弱（连多资源触发路径都没有）。

## 6. 真机 backlog（进 `pending-live-tests` §24）

4 项：带 `pos` 资源包正常态 / `slot_col` 或 `slot_row` 缺失或为 0 的裁剪包 / 切页签与 slider 行数观感 / C4701 消警后编译器输出复核。自动 harness 只覆盖静态守卫 + 修复前/后算法复刻，不冒充真机 UI 事件。

## 7. 提交 scope

精确 `git add` 5 文件：`IO/UITypes/UIItemInventory.cpp`、`docs/features/development-and-fix-plan-2026-11/README.md`（§4 追加 2-13）、`docs/features/development-and-fix-plan-2026-11/notes/wave2-iteminventory-slotcol-recheck-20261110.md`、`docs/features/pending-live-tests/README.md`（追加 §24 round45）、`tools/test_iteminventory_slotcol_guard.py`。临时 `_build_run45.txt` 提交前删除、不入 add。`git-commit-standard`：`<Emoji> <Type>(<Scope>): <标题> (#<IssueID>)`，结构化 body，**零署名**。

## 8. 结论

除零 / 模零族 153 命中的全量逐字核准证明 live 源码主体路径守卫充分；唯一真实缺口是代码明确支持的多资源 `if(pos)` 旁路。整组回退守卫把它闭合，合法包逐字零改动，13 场景 + 20 静态守卫 + opcode GATE + 真重编译四项证据闭环。当前 shipped 包不可达这一事实如实写在本节 §2.1，不拿走不到的分支虚报当前必崩；第 12 例同族链 round34→45 收口。
