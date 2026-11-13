# 第十面 地图(Map) — Angel 083/v83 ↔ 当前端 逐功能校准

> 目标 (m00690 / m03482)：「以 Angel(13339) 的 083/v83 带符号 IDB 为权威校准源，从登录链路开始，逐功能完整校验并校准当前 MapleStory-Client 的功能、业务、逻辑、UI、排版、样式、交互的正确性 / 缺失 / 一致性；每轮先做 REA→IDA→IMG 只读取证与对照清单，交决策点 + 一次性 Plan 后再进入实现。」
> 本面 = 第 ⑩ 面（登录 / HUD / core / social / pet / activity / cashshop / skill / npc 之后），round32/90，`goal-11388c42` active/armed。
> **性质：校准 / 对照 / 决策点 / 一次性 Plan 文档，非实现提交。全程只读未改游戏源码 / 协议 / WZ / IMG / XML，未建提交。**

## 0. 五条结论

1. **十面 EA 字节级一致** — 本面 `133/133 by bytes`（2 个 missing EA 经 32B 双端逐字比对洗掉：`0x9e98c2 ::_GCWorldMapDlg`、`0x58e12b CFuncKeyMappedMan::FuncKeyMapped`），钉死 Angel(13339) 与 BeiDou(13337) 是**同一份 v83 二进制的不同符号状态**；Angel 只补语义层（`3122` 修饰名 / `361` 类 / `2612` 方法 vs BeiDou `50` / `10` / `0`）。十面台账：登录 `327/328`、HUD `547/548`、core `439/441`、social `752/752`、pet `968/968`、activity `844/844 (by bytes)`、cashshop `938/938 (by bytes)`、skill `106/106 (by bytes)`、npc `71/71 (by bytes)`、**map `133/133 (by bytes)`**。
2. **九面里第一次「c2s opcode 数值 + 语义 + body 三对齐」** — Angel 侧本面仅 2 个 c2s 字面量：`COutPacket(135)` = `CFuncKeyMappedMan::SaveFuncKeyMap` ↔ 当前端 `CHANGE_KEYMAP=135` + `ChangeKeyMapPacket`；`COutPacket(183)` = `CQuickslotKeyMappedMan::SaveQuickslotKeyMap` ↔ `CHANGE_QUICKSLOT=183` + `ChangeQuickslotPacket`（`EncodeBuffer(0x20)` ↔ 8×`write_int`）。不靠 `数值≠语义` 兜底，是正面全闭环。s2c `OnPacket case 128/129/130` 与 `PacketSwitch.cpp` 三 handler 注册也逐一对上。
3. **数值重合 ≠ 语义重合（承第八/九面纪律，本面补反例）** — `135` 在 c2s = `CHANGE_KEYMAP`、在 `PacketSwitch.cpp:218` s2c = `WHISPER`；`183` 在 c2s = `CHANGE_QUICKSLOT`、在 `:262` s2c = `REMOVE_DRAGON`。083 / s9 两套命名空间，`CHANGEMAP=38 / CHANGE_MAP_SPECIAL=100 / USE_INNER_PORTAL=101 / PLAYER_MAP_TRANSFER=207 / WORLD_MAP_PLAYERS=376 / WORLD_MAP_DROP_RESULT=377` 分列不混，禁止仅凭数值判等。
4. **IMG 门禁本面第一次真闭合（推翻历史口径）** — `custom-orange_wz` 已连接（global / 29 工具 / health 2/2），**目录结构层 WORKS**（`Data/Map children=8` 真实节点留证），但任何 internal `.img` 的 tree/search **内容层 FAILS**（`IMG 解析失败`，65536B GMS↔BMS 全表 / `autoParse:true` / 下钻三形态都救不回）→ 命中 AGENTS.md（YG 2026-11-09 ACTIVE）写死的合规回退条件，原始报错文本 + key 规格逐条留证后回退 `.nx`。**推翻的是前九面「未连接/绕过」口径（O1），不动已实现功能**；F 维 12 项诚实记 ⏳，绝不假绿。
5. **判定汇总（脚本逐条重数，非手写）** — 七维 98 条：`A14 + B14 + C14 + D14 + E16 + F12 + G14 = 98`；判定 `✅71 / ⚠️13 / ⏳14`（无 ❌ / ⊘）。校准 ≠ 改回原生：083 `CWorldMapDlg`/`CUIMiniMap` 是 MFC `CDialog` 系，当前端 `UIWorldMap`/`UIMiniMap` 是 `UIDragElement` + 多分辨率重锚 + 侧栏状态机，功能覆盖但实现不同源，冲突一律 s9 优先。

## 1. 取证基线

### 1.1 IDA 端点表

| 端点 | pid | port | 角色 | 状态 |
| --- | --- | --- | --- | --- |
| `Angel.exe` | 29620 | 13339 | **083/v83 带符号 IDB 权威源**（imagebase `0x400000`，`3122` 修饰名） | ✅ REACHABLE |
| `BeiDou.exe` | 31332 | 13337 | 同一份二进制 stripped 状态（符号层对照 / 字节级审计） | ✅ REACHABLE |
| `custom-orange_wz` | — | 10012 | WZ/IMG/XML 结构层（ACTIVE 规则第一顺位） | ✅ 已连接 29 工具 |

### 1.2 三联门禁（REA → IDA → IMG）

| 环 | 工具 / 端点 | 结果 | evidence |
| --- | --- | --- | --- |
| REA | 既有 `beidou-v17-parity/notes/evidence/r17/rea_search_*.json`（subject = `BeiDou-ClientV17.7/BeiDou.exe`，**单列不冒充 13339**） | ✅ 四态素材 | r05 §4 |
| IDA 符号 | 13339 `idautils.Functions` + `idc.demangle_name`，kws `("Map","MAP","map")` | ✅ `133 funcs / 27 classes` | `angel-map-symbol-inventory.json` |
| IDA 反编译 | 13339 `decompile(include_addresses:true)` | ✅ `133/133 ok` | `angel-map-decompile.json` |
| IDA 交叉 | 13337 `lookup_funcs`（60/chunk） | ✅ `131/133`（2 missing） | `angel-beidou-map-ea-crosscheck.json` |
| IDA 字节审计 | `get_bytes` 32B 双端（通用版，无硬编码 EA） | ✅ `133/133 by bytes` | `angel-beidou-map-ea-bytelevel-audit.json` |
| facts | `code_of` 多态归一 | ✅ `1936 行 / 76629 B / 2 opcode` | `angel-map-facts.md` |
| **IMG 结构层** | orange-wz `load_files(Data/Map)` + `list_children` | ✅ `children=8` 真实节点 | `orange-wz-map-structure.json` |
| **IMG 内容层** | orange-wz `get_node_tree_json` / `search_node` on internal `.img` | ⚠️ `IMG 解析失败` → 合规回退 | `orange-wz-gate-verdict.json` |
| **.nx 回退** | `tools/nx_inspect.py`（PKG4） | ✅ 内容层节点可读 | `nx-content-probe.json` |

### 1.3 采集脚本 + evidence + notes

5 个 IDA 脚本（`python -m py_compile` → `PYCOMPILE_OK`）：`tools/ida_angel_map_{ea,decompile,crosscheck,byteaudit,facts}.py`（从第九面 NPC 已验证 SOP 派生，不重手写）。
6 个门禁 probe：`tools/orange_wz_{gate_smoke,node_probe,schema_probe,autoparse_probe,dir_nx_probe,drilldown_probe}.py` + `tools/face10_gate_and_map_probe.py`。
evidence 8 件 + notes 3 件（`r01` 一手事实 / `r03` IMG 门禁回退 / `r05` 四态审计）+ 本 README。

### 1.4 本面独有技术点

1. **byteaudit 通用化** — 读 `crosscheck.json` 的 `missing_eas` 列表逐个洗，不再硬编码 EA（第九面 `0x6d065b` 是写死的，第十面 2 个 EA 自动处理）。
2. **decompile 三形态归一 + `label@EA` 防覆盖** — decompile MCP 返回纯伪代码文本 / `{addr,code,refs}` JSON 串 / dict，另有**大函数 output-ref 溢出**（inline JSON 被截断成坏 JSON + 尾部 `http://127.0.0.1:13339/output/<uuid>.json`），`normalize_code` 对第 3 形态按 URL 取回真 `code`；result key 用 `"%s@%s" % (label, addr)` 防同源重名覆盖。修复前 `physical_keys=118`（丢 15 个逻辑函数、facts `:972` 残留坏 JSON 串），修复后 `physical_keys=133` / `EMPTY 0 / OUTPUT_REF 0 / BROKEN_JSON 0 / residual_JSON_string_lines 0`（承第八面 facts 假闭环教训的第二次升级）。
3. **orange-wz 假绿自我批判** — v2 smoke 曾把 `IMG 解析失败` 文本当 OK 打出 `steps 6 ok 6`，改严格 classify 后写进 verdict 的 `no_fake_green: True`；标尺不严就是自嗨。
4. **结构层 / 内容层分离** — 目录枚举出真实节点 ≠ internal `.img` 可解析，两条证据链分开留证，不互相冒充。

## 2. 口径

### 2.1 七维 × 五判定

| 维 | 含义 | 条数 |
| --- | --- | --- |
| A 功能 | 地图能力是否具备 | 14 |
| B 业务 | 换图 / 键位 / 世界地图业务流程 | 14 |
| C 逻辑 | 分发 / 差分 / 装载顺序 / 边界 | 14 |
| D 数据 | MapInfo / Portal / WorldMap 结构体 | 14 |
| E 排版 | 坐标 / 尺寸 / 布局常量 | 16 |
| F 样式 | 贴图 / 颜色 / 像素（依赖 IMG 内容层） | 12 |
| G 交互 | 按钮 / 键盘 / 鼠标 / 拖拽 | 14 |

五判定：`✅` 双端一致且已实现 / `⚠️` 有差异但 s9 优先或需裁决 / `⏳` 真机或资源缺口遗留 / `❌` 缺失 / `⊘` 不适用。

### 2.2 证据优先级

live runtime behavior → 抓包 → 当前端源码（本面主体）→ Angel 083 反编译一手 → 字节审计 → REA 既有素材（单列）→ 既有笔记（四态归组）→ 注释 / 死代码。用源码解释运行时，不用注释覆盖源码。

### 2.3 083 vs s9 红线

- **083/v83 唯一基准**：Angel(13339) IDB 是本面权威源；`docs/features/port-095/**`、`Maplestory95.i64` 相关 EA/字段序 = 095 线，**禁引**为 083 一手，仅作历史实现注记。
- **校准 ≠ 改回原生**：改回 083 发包会断 s9 联机；冲突交用户裁决 D1–D14，不得盲改。
- **s9 协议优先**：`CHANGEMAP / CHANGE_MAP_SPECIAL / USE_INNER_PORTAL / PLAYER_MAP_TRANSFER / WORLD_MAP_*` 走 s9 命名空间，与 083 内部 opcode 分列。
- **数值重合 ≠ 语义重合**：`135/183` c2s↔s2c 撞号不同义（见 §0.3）。
- **WZ/IMG/XML 一律先 orange-wz**：仅解析异常才回退 `.nx`，回退须记录原始报错 + key 规格（AGENTS.md ACTIVE，覆盖历史 D10/D13/D14）。

### 2.4 冲突取值表

| 冲突 | Angel 083 | 当前端 / s9 | 取值 |
| --- | --- | --- | --- |
| c2s 135 | `COutPacket(135)` 89 键快照差分 + `sub_4E43FA` 条目编码 | `ChangeKeyMapPacket` 全量 `updated_actions` + `write_int(key)/write_byte(type)/write_int(action)` | 保持 s9 拆法，不回改（D1） |
| c2s 183 | `EncodeBuffer(0x20)` 原始缓冲 | 8×`write_int` = 32B | ✅ 已一致，无需裁决 |
| 换图入口 | `CField::LoadMap` 无独立 portal 发包字面量 | `CHANGEMAP=38` + `CHANGE_MAP_SPECIAL=100` + `USE_INNER_PORTAL=101` | s9 通道分列（D3） |
| 世界地图 | `CWorldMapDlg` MFC `CDialog` 18 方法 | `UIWorldMap` `UIDragElement` + 侧栏状态机 + 付费传送 | 保持当前端（D4） |
| `OnMapTransferResult` | `Decode1(mode)+Decode1(flag)+case 2..11 Decode4 列表` | 当前端 `mode`+`vip` 两字段 `LOG` | 不强行对齐（D6） |
| IMG 内容层 | — | orange-wz `IMG 解析失败` → `.nx` 合规回退 | 结构层留证，内容层 ⏳（D10/D13/D14 已被 ACTIVE 覆盖） |

## 3. 七维 98 条逐条对照

> 每行格式 `| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |`；判定列由 `tools/face10_readme_audit.py` 逐条重数（见 §9）。

### 3.1 A 功能（14 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| A1 | 十面同二进制 | `133/133 by bytes`，2 missing EA 32B 双端一致 | byteaudit verdict | ✅ | 销案，Angel 仅补语义层 |
| A2 | c2s 135 键位存档 | `SaveFuncKeyMap @ 0x58df2f COutPacket(135)` | `OutPacket.h:169 CHANGE_KEYMAP=135` + `PlayerPackets.h:103` | ✅ | 数值语义双对齐 |
| A3 | c2s 183 快捷栏触发键 | `SaveQuickslotKeyMap @ 0x72b87e COutPacket(183)` | `OutPacket.h:204 CHANGE_QUICKSLOT=183` + `SkillMacroPackets.h:43` | ✅ | 数值+body+语义三对齐 |
| A4 | s2c 128 背景特效 | `OnPacket case 128 → OnSetBackEffect @ 0x6445c5` | `PacketSwitch.cpp:284 SET_BACK_EFFECT=128` + `:632` | ✅ | 销案 |
| A5 | s2c 129 物件可见 | `OnPacket case 129 → OnSetMapObjectVisible @ 0x6449d2` | `PacketSwitch.cpp:344` + `:694` | ✅ | 销案 |
| A6 | s2c 130 清背景特效 | `OnPacket case 130 → OnClearBackEffect @ 0x6449ca` | `PacketSwitch.cpp:345` + `:695` | ✅ | 销案 |
| A7 | 小地图开关 | `OnMiniMapOnOff @ 0xa129ed Decode1 → ReloadMiniMap` | `UIMiniMap.h:34 TOGGLED=true` + r315 | ✅ | 销案 |
| A8 | 小地图三形态 | `CUIMiniMap::SetCtrl/SetLayer/ToggleMap` | `UIMiniMap.h:88-93 Type{MIN,NORMAL,MAX}` | ✅ | 销案 |
| A9 | 地图装载管线 | `CField::LoadMap @ 0x529bb4` + `CMapLoadable` 36 方法 | `MapInfo.h:57` + `Stage` 装载时序 | ✅ | 同源，实现分列 |
| A10 | 世界地图分区上下钻 | `SetWorldMapDeeper @ 0x9ee30e` / `Shallower @ 0x9ee507` | `UIWorldMap.h:126 jump_to_map_region` | ✅ | 功能覆盖 |
| A11 | MapleTV 双屏（第九面交叉） | `SetMapleTVMessage @ 0x6d417e` / `DrawMapleTVMessage @ 0x6d4ad4` | `Npc.cpp` 上屏 240×90 / 下屏 240×180 | ✅ | 销案 |
| A12 | QUICKSLOT_INIT 下行 | `CQuickslotKeyMappedMan::OnInit @ 0x72b82a` | `PacketSwitch.cpp:306 QUICKSLOT_INIT=159` + `:654` | ✅ | 销案 |
| A13 | 135 键位收集策略 | 89 键快照差分，仅变更键才发包 | `ChangeKeyMapPacket` 全量 `updated_actions` | ⚠️ | D1 保持 s9，不回改 |
| A14 | 地图传送结果多分支 | `OnMapTransferResult @ 0xa25268 case 2..11` | `WaveAShellHandlers.cpp:2255` 2 字段 LOG | ⚠️ | D6 不强行对齐 |

### 3.2 B 业务（14 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| B1 | 换图上行 | `CField::LoadMap` 装载链（无独立 c2s 字面量） | `GameplayPackets.h:25-40 ChangeMapPacket` CHANGEMAP=38 | ✅ | s9 通道销案 |
| B2 | 脚本门换图 | 083 走 `CNpc`/script 链 | `MapInteractPackets.h:14 ChangeMapSpecialPacket` =100 | ✅ | s9 通道 |
| B3 | 内部传送门 | 083 `Portal::WarpInfo` 结构 | `MapInteractPackets.h:28 UseInnerPortalPacket` =101 | ✅ | s9 通道 |
| B4 | 换图完成确认 | `CWvsContext::OnMapTransferResult` 下行 | `GameplayPackets.h:289 PlayerMapTransferPacket` =207 | ✅ | s9 通道 |
| B5 | 世界地图玩家列表 | `CWorldMapDlg::LoadMapList @ 0x9ed38b` | `WorldMapHandlers.cpp:14` WORLD_MAP_PLAYERS=376 | ✅ | 销案 |
| B6 | 世界地图掉落查询 | `CUIToolTip::SetToolTip_WorldMap @ 0x8e7434` | `WorldMapHandlers.h:12` WORLD_MAP_DROP_RESULT=377 | ✅ | 销案 |
| B7 | 键位存档业务 | `CFuncKeyMappedMan::SaveFuncKeyMap` | `PlayerPackets.h:103` + `KeyAction::to_server` 回译 | ✅ | 销案 |
| B8 | 快捷栏触发键业务 | `CQuickslotKeyMappedMan::SaveQuickslotKeyMap` | `SkillMacroPackets.h:40` + `Keyboard::assign_quickslot` | ✅ | 销案 |
| B9 | 键位 / 触发键分列 | 083 `CFuncKeyMappedMan` vs `CQuickslotKeyMappedMan` 两类 | `SkillMacroPackets.h:35-39` 注释「槽内绑定走 KEYMAP(135)」 | ✅ | 销案 |
| B10 | portal 无独立 c2s | 083 `CField::LoadMap` 无 portal 发包 | `s9-protocol-align/notes/s8-npc.md` 走 CHANGEMAP | ✅ | 交叉面销案 |
| B11 | QUEST_ACTION 107 | 083 未命中 | `s8-npc.md` restoreLostItem 低严重度不改 | ✅ | 销案 |
| B12 | MapleTV 广播 | `CMapleTVMan::GetSenderName/GetReceivername` | `MapNpcs.cpp:74 broadcast_tv_message` 按当前图 TV NPC | ✅ | 销案 |
| B13 | 世界地图付费传送 | 083 `CWorldMapDlg` 无扣费字面量 | `UIWorldMap.h:123 request_teleport` 扣 10 万金币 | ⚠️ | s9 扩展，D4 保持 |
| B14 | 任务列表本地开窗 | `CNpc::ShowQuestList`（第九面交叉） | 当前端任务面 P3 ⑪ | ⚠️ | 交叉面遗留，P3 |

### 3.3 C 逻辑（14 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| C1 | s2c 三 case 分发 | `OnPacket @ 0x644446 switch(a2) case 128/129/130` | `PacketSwitch.cpp:975-977` opcode 名映射 | ✅ | 逐一对上 |
| C2 | 89 键差分循环 | `do{if(*v3!=v3[445]...) InsertBefore(v2); v3+=5;}while(v2<89)` | `ChangeKeyMapPacket` 上层传 `updated_actions` | ✅ | 收集策略差异记 D1 |
| C3 | 32B 快照保存 | `EncodeBuffer(this+4,0x20u)` + `memcpy(this+36,this+4,0x20u)` | `ChangeQuickslotPacket` 8×`write_int` | ✅ | 三对齐 |
| C4 | 装载顺序 | `LoadMap → 8×Restore* → PlayBGMFromMapInfo` | `MapInfo` 构造读 `nl::node src` | ✅ | 销案 |
| C5 | BGM 播放条件 | `if(*(*dword_BE7918+12464)==0) PlayBGMFromMapInfo` | `MapInfo.h:64 get_bgm()` | ✅ | 销案 |
| C6 | ESC 返回上一级 | `OnKey @ 0x9ee717 if(a2==27) SetWorldMapShallower` | `UIWorldMap.h:63 send_key(..., bool escape)` | ✅ | 语义承接 |
| C7 | 小键盘 * 映射 | `if(BYTE2(a3)==54) v3=42` | 当前端无对应映射 | ⚠️ | D13 + G13，真机补 |
| C8 | 障碍区更新 | `CMapLoadable::UpdateObstacleInfo @ 0x642264` | `Gameplay/Physics/Physics.cpp` + r388 | ✅ | 销案 |
| C9 | ZMap 哈希表 | `?$ZMap Insert/GetAt/RemoveKey/ResizeHashTable` | 当前端 `std::unordered_map`（`MapPortals.h:66`） | ✅ | 容器等价 |
| C10 | FUNCKEY_MAPPED::Decode | `DecodeBuffer(a2, this, 5u) @ 0x4e4409` | `KeyMapTranslate.h` + `WaveAShellHandlers.cpp:3145` | ✅ | 销案 |
| C11 | 地图边界钳制 | `CVecCtrl::BoundPosMapRange @ 0x9b45c1` | `Physics.h` walls/borders | ✅ | 销案 |
| C12 | 135 条目编码 | `sub_4E43FA(this+..., &v7)` 编码 mapping | `write_byte(type)+write_int(action)` | ⚠️ | D1 保持 s9 拆法 |
| C13 | 183 服务端截断 | 083 `EncodeBuffer` 原始 32B | 注释「服务端逐槽 `(byte)readInt` 截断」 | ⚠️ | 既有注记，不回改 |
| C14 | 传送结果 Decode4 列表 | `case 2/3 *v7++=Decode4(a2)` | 当前端 2 字段 LOG | ⚠️ | D6 不强行对齐 |

### 3.4 D 数据（14 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| D1 | 地图元信息 | `CMapLoadable::Init/LoadMap` 读 info 节点 | `MapInfo.h:78-85 fieldlimit/cloud/bgm/mapdesc/mapname/streetname/mapmark` | ✅ | 销案 |
| D2 | 边界 Range | `CVecCtrl::BoundPosMapRange` | `MapInfo.h:89-90 Range<int16_t> mapwalls/mapborders` | ✅ | 销案 |
| D3 | 座位 / 梯子 | `CField::RestoreSeat` | `MapInfo.h:91-92 vector<Seat>/vector<Ladder>` | ✅ | 销案 |
| D4 | recovery 恢复率 | `RestoreSeat` / 桑拿 | `MapInfo.h:67-69 get_recovery()` 对齐 `s9 77*recovery*1.5` | ✅ | 销案 |
| D5 | 传送门 14 型 | `Portal::Type` 枚举（083 `CField` 读 `portal/*`） | `Portal.h:27-44 SPAWN..TYPE14` | ✅ | 销案 |
| D6 | WarpInfo 结构 | `CNpc`/`CField` 换图目标 | `Portal.h:60-76 mapid/toname/name/intramap/valid/scripted` | ✅ | 销案 |
| D7 | 隐藏门三态 | `HidePhase NONE/START/CONTINUE/EXIT` | `Portal.h:52-58` + `ph_start/ph_continue/ph_exit` | ✅ | 销案 |
| D8 | 门触发范围 | `CUIMiniMap`/`CField` 碰触通道 | `MapPortals.h:43-47 PortalScope{TOUCH_ONLY,ALL}` | ✅ | 销案 |
| D9 | 世界地图在线玩家 | `CWorldMapDlg::LoadMapList` | `UIWorldMap.h:37-43 WorldMapPlayer{name,level,channel}` | ✅ | 销案 |
| D10 | 掉落查询回包 | `CUIToolTip::SetToolTip_WorldMap` | `WorldMapHandlers.h:16-42 version/mob_id/count/entries` | ✅ | 销案 |
| D11 | 地图名查表 | `CItemInfo FindMapByName/GetMapString/IterateMapString` | `String.nx` + `Net/KeyMapTranslate.h` | ✅ | 同源 |
| D12 | IMG 结构层 | — | orange-wz `Data/Map children=8`（`orange-wz-map-structure.json`） | ✅ | 真实节点留证 |
| D13 | SP_267 提示串 | `SP_267_YOU_ARE_CURRENTLY_AT_A_PLACE...WORLD_MAP_IS_NOT_AVAILABLE` | 当前端 StringPool 未逐字对账 | ⚠️ | P3 ⑪ 联动 |
| D14 | String.nx 全量 | 083 `SP_` 节点族 | `String.nx 20 children` 未逐条比对 | ⚠️ | P3 ⑪ 联动 |

### 3.5 E 排版（16 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| E1 | 小地图顶带锚点 | `CUIMiniMap::SetCtrl @ 0x858344` | `UIMiniMap.h:52-55 CENTER_START_X=64/BTN_MIN_Y=4/ML_MR_Y=17/MAX_ADJ=43` | ✅ | 销案 |
| E2 | 小地图标记位 | `CUIMiniMap::LoadMiniMap @ 0x858603` | `UIMiniMap.h:58-62 MARK_X=8/MARK_Y=24/M_START=36` | ✅ | 销案 |
| E3 | MAX 顶带 72px | 083 `MakeBorder` 类族 | `UIMiniMap.h:56-59` 白标题 y5..18 / 浅蓝信息带 y20..65 / 38×38 标记居中 | ✅ | 逐行取色留证 |
| E4 | NPC 列表尺寸 | `CUIMiniMap::OnMouseMove` 列表 | `UIMiniMap.h:61-63 LISTNPC_ITEM_HEIGHT=17/WIDTH=140/TEXT_WIDTH=114` | ✅ | 销案 |
| E5 | 世界地图把手 | `CWorldMapDlg::MakeBorder @ 0x9ebfc3` | `UIWorldMap.h:95-96 TAB_W=26/TAB_H=92`（框内不外凸） | ✅ | 销案 |
| E6 | 侧栏缓动时长 | `CWorldMapDlg::Draw @ 0x9eb9c6` | `UIWorldMap.h:99-103 SIDE_EXPAND_MS=220.0f`（float，整型除法恒 0 坑） | ✅ | 销案 |
| E7 | 多分辨率重锚 | 083 `CDialog` 固定坐标 | `UIWorldMap.h:57 update_screen` 居中重锚 | ✅ | 当前端增量 |
| E8 | MAPSPOT 类型上限 | `CWorldMapDlg::CheckSpotInfo @ 0x9edd67` | `UIWorldMap.h:82 MAPSPOT_TYPE_MAX=4u` | ✅ | 销案 |
| E9 | 对话框重锚（第九面交叉） | `CUtilDlgEx::SetNPC @ 0x9a64fc` | `UINpcTalk.h reanchor_talk()` 1280×720 保留 `(400,240)` | ✅ | 交叉面销案 |
| E10 | 世界地图分区命名 | `CWorldMapDlg::LoadMapList` | `UIWorldMap.h:124-126 "WorldMap0"+mapid/10000000` | ✅ | 口径一致 |
| E11 | 世界地图边框绘制 | `MakeBorder @ 0x9ebfc3 (26557B)` | `UIWorldMap::draw` + `draw_tab` | ✅ | 同源 |
| E12 | 小地图图层调整 | `CUIMiniMap::SetLayer @ 0x855d02` | `UIMiniMap.h:69 update_canvas()` | ✅ | 销案 |
| E13 | 逐控件坐标 | 083 `Draw @ 0x9eb9c6` MFC 控件表 | 当前端 `UIWorldMap` 侧栏 `layout_panels()` | ⚠️ | D4 保持当前端，不回改 |
| E14 | 大函数逐行排版 | `LoadMiniMap @ 0x858603 (21963B)` / `OnMouseMove @ 0x85ade2 (22396B)` | 当前端 `UIMiniMap.cpp` 未逐行对齐 | ⚠️ | P2 真机补 |
| E15 | 像素级贴图对齐 | — | F 维依赖 orange-wz 内容层（当前 `IMG 解析失败`） | ⏳ | P2 / r03 §6 |
| E16 | 白字黑描边 | 083 MAX 顶带地图名 | `UIMiniMap.h:123-126 OutlinedText region_text/town_text` | ✅ | `minimap.png` 实测留证 |

### 3.6 F 样式（12 条，全 ⏳ — orange-wz 内容层解析失败，合规 .nx 回退但逐像素未验）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| F1 | MapLogin 登录地图底图 | — | `UI/MapLogin.img` → orange-wz `IMG 解析失败` / `Map.nx` 7 children | ⏳ | P2 真机 |
| F2 | UIWindow 世界地图框 | `CWorldMapDlg::OnCreate` | `UI/UIWindow.img` → `IMG 解析失败` / `UI.nx` 20 children | ⏳ | P2 真机 |
| F3 | 世界地图分区贴图 | `CWorldMapDlg::SetWorldMap` | `Data/Map/WorldMap/WorldMap.img` → `IMG 解析失败` | ⏳ | P2 真机 |
| F4 | 背景层贴图 | `CMapLoadable::MakeBack/RestoreBack` | `Data/Map/Back/*.img` → `IMG 解析失败` | ⏳ | P2 真机 |
| F5 | 物件层贴图 | `CMapLoadable::MakeObj/MakeObjLayer` | `Data/Map/Obj/*.img` → `IMG 解析失败` | ⏳ | P2 真机 |
| F6 | 瓦片层贴图 | `CMapLoadable::MakeTile/RestoreTile` | `Data/Map/Tile/*.img` → `IMG 解析失败` | ⏳ | P2 真机 |
| F7 | smap 小地图裁剪 | `CUIMiniMap::LoadMiniMap` | `smap.img` → `IMG 解析失败` / `smap.nx 147 children` + depth2 | ⏳ | P2 真机 |
| F8 | zmap 小地图缩放 | `CUIMiniMap::SetLayer` | `zmap.img` → `IMG 解析失败` / `zmap.nx 151 children` + depth2 | ⏳ | P2 真机 |
| F9 | 地图名 / 提示串样式 | `CItemInfo::GetMapString` + `SP_267` | `String.nx 20 children` 未验字色 | ⏳ | P2 真机 |
| F10 | MapleTV 双屏贴图 | `CNpc::DrawMapleTVMessage` | `UI.nx` + 源码常量（第九面交叉） | ⏳ | P2 真机 |
| F11 | skillPoint 面板（第八/九面延续） | — | `UIWindow.img/Skill/skillPoint` NOT FOUND → `send_spup` 降级 | ⏳ | 保留资源降级 |
| F12 | 正确端截图逐像素 | — | `C:\Users\11036\Desktop\正确客户端截图\` 体感输入 | ⏳ | 不冒充节点证据 |

### 3.7 G 交互（14 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| G1 | 小地图按钮 | `CUIMiniMap::OnButtonClicked @ 0x85ace7` | `UIMiniMap.h:78-86 BT_MIN/MAX/SMALL/BIG/MAP/NPC` | ✅ | 销案 |
| G2 | 小地图滚轮/键盘/光标 | `CUIMiniMap::OnMouseMove @ 0x85ade2` | `UIMiniMap.h:42-44 send_scroll/send_key/send_cursor` | ✅ | 销案 |
| G3 | 世界地图键鼠 | `CWorldMapDlg::OnKey/OnMouseButton/OnMouseEnter/OnMouseMove` | `UIWorldMap.h:61-65 send_key/doubleclick/toggle_active` | ✅ | 销案 |
| G4 | 搜索结果双击跳分区 | `CWorldMapDlg::HitTest @ 0x9eb98e` | `UIWorldMap.h:65 doubleclick` → `jump_to_map_region` | ✅ | 销案 |
| G5 | 付费传送四入口 | 083 `CWorldMapDlg::OnButtonClicked` | `UIWorldMap.h:122-123 request_teleport`（按钮/S·F/T·回车/搜索框统一） | ✅ | s9 增量 |
| G6 | 侧栏展开状态机 | `SetWorldMapDeeper/Shallower` | `UIWorldMap.h:85-91 SideState{COLLAPSED,EXPANDING,EXPANDED,COLLAPSING}` | ✅ | 销案 |
| G7 | 世界地图鼠标事件族 | `OnMouseButton @ 0x9ee6e5 / OnMouseEnter @ 0x9ee2ec / OnMouseMove @ 0x9ee2b3` | `send_cursor` + `is_in_range` | ✅ | 销案 |
| G8 | 小地图 Stalkee | `CUIMiniMap::InsertStalkee @ 0x85bbe4` | `UIMiniMap.h:71 draw_movable_markers` | ✅ | 销案 |
| G9 | 换图参数交互 | 083 `CField` 换图 | `ChangeMapPacket(died,targetid,targetp,usewheel)` | ✅ | 销案 |
| G10 | 脚本门交互 | `Portal::WarpInfo.scripted` | `ChangeMapSpecialPacket` byte0+string+short0 | ✅ | 销案 |
| G11 | 内门交互 | `Portal::WarpInfo.intramap` | `UseInnerPortalPacket` byte0+string | ✅ | 销案 |
| G12 | 传送门冷却 | 083 `CField` 门触发 | `MapPortals.h:69 WARPCD=48` | ✅ | 销案 |
| G13 | 小键盘 * 触发 5 号动作 | `OnKey` `if(BYTE2(a3)==54) v3=42; if(...==5) (*(...))(a2:1)` | 当前端无对应 `KeyAction` 映射 | ⚠️ | D13，真机补 |
| G14 | 贴图交互态 hover/pressed | — | 依赖 orange-wz 内容层（当前 `IMG 解析失败`） | ⏳ | P2 / r03 §6 |

**判定汇总（脚本重数）**：`✅71 / ⚠️13 / ⏳14`，合计 `98`；无 `❌` / `⊘`。

## 4. 十条硬发现

1. **本面 Angel 侧只有 2 个 c2s opcode**（`135` / `183`），地图主体是资源装载 + 渲染管线（`CMapLoadable` 36 方法），不是发包密集面 —— 与 cashshop(7) / skill(7) / npc(2) 的量级对照成立。
2. **`COutPacket(135)` 89 键快照差分** — `do{...}while(v2<89)` + `Encode4(mode=0)+Encode4(count)+N×(Encode4(key)+sub_4E43FA)`；当前端 `ChangeKeyMapPacket` 全量传 `updated_actions`，收集策略差异记 D1。
3. **`COutPacket(183)` `EncodeBuffer(0x20)`** — 原始 32B 缓冲 ↔ 当前端 8×`write_int`，是九面里第一处 **数值 + body 结构 + 语义三对齐**。
4. **`CMapLoadable::OnPacket` 三 case 精确值** `128/129/130` ↔ `PacketSwitch.cpp:284/:344/:345` + `:632/:694/:695` 三 handler 注册，逐一对上无偏移。
5. **`CField::LoadMap` 8 个 `Restore*` 顺序** + `if(*(*dword_BE7918+12464)==0) PlayBGMFromMapInfo` 条件播放 BGM —— 装载时序一手证据。
6. **`CWorldMapDlg::OnKey` ESC(27)→`SetWorldMapShallower`、小键盘 54→42 映射** —— 083 世界地图键盘交互细节，当前端承接 `escape` 参数但小键盘映射缺（D13）。
7. **`133/133 by bytes`** 洗掉 2 个 missing EA（`0x9e98c2` / `0x58e12b`），十面同二进制 verdict 再加固。
8. **orange-wz 结构层 WORKS / 内容层 FAILS** —— 目录枚举 `Data/Map children=8` 是真节点；internal `.img` 一律 `IMG 解析失败`，4 条本质不同路径（standalone / autoParse+key.name / 目录+BMS / 下钻三形态）全部打穿，合规回退条件成立。
9. **`no_fake_green` 自我批判** —— smoke v2 曾把 `IMG 解析失败` 当 OK 打出 `6 ok 6`，改严格 classify 并写进 verdict；F 维 12 项诚实 ⏳。
10. **既有 53 项地图笔记四态归组**：销案 8 / 推翻 1（历史 orange-wz 口径）/ 新增 8 / 遗留 5；推翻的是依据来源与执行顺序口径，不动已实现功能。

## 5. 四态

| 态 | 组数 | 摘要 |
| --- | --- | --- |
| 销案 | 8 | S1 MINIMAP_ON_OFF / S2 MapObjectVisible / S3 WorldMap SoftKey / S4 ChangeMap / S5 Keymap / S6 Quickslot / S7 MapleTV / S8 s8-npc 切片 |
| 推翻 | 1 | O1 前九面「orange-wz 未连接/阻塞/以 .nx dump 为准」口径 → 本面实证已连接 + 结构层真实节点 + 内容层原文报错 + 合规回退（不动实现） |
| 新增 | 8 | N1 36 方法管线 / N2 三 case / N3 89 键差分 / N4 32B 快照 / N5 18+10 方法具名 / N6 地图名查表链 / N7 133/133 by bytes / N8 orange-wz 结构层+回退链 |
| 遗留 | 5 | L1 F 维 12 项逐像素 / L2 上下钻真机 / L3 特效层实机 / L4 OnMapTransferResult 多分支 / L5 String.nx + SP_267 逐字 |

详见 `notes/r05-map-four-state-audit.md`。**无「推翻已实现功能」** —— 校准 ≠ 改回原生。

## 6. 决策点 D1–D14（推荐默认，须用户裁决后 P1 方可动源码）

| # | 决策点 | 推荐默认 | 依据 |
| --- | --- | --- | --- |
| D1 | 083 `SaveFuncKeyMap` 89 键差分 + `sub_4E43FA` 条目编码 vs s9 全量 `ChangeKeyMapPacket` | **保持 s9 拆法，不回改** | 改回 083 发包断 s9 联机；`opcode 135` 数值语义已对齐，仅 body 微差 |
| D2 | 083 `CMapLoadable` 36 方法资源管线是否照搬 | **不照搬，保持当前端 `MapInfo`/`Stage` 装载** | 功能覆盖，实现不同源 |
| D3 | 083 无独立 portal c2s vs s9 `CHANGEMAP=38 / CHANGE_MAP_SPECIAL=100 / USE_INNER_PORTAL=101` | **分列命名空间，保持 s9** | 083↔s9 两套协议 |
| D4 | 083 `CWorldMapDlg` MFC 大窗 vs 当前端 `UIWorldMap` 侧栏状态机 + 付费传送 | **保持当前端** | 当前端为超集（多分辨率重锚/WORLD_MAP_PLAYERS/掉落查询） |
| D5 | 是否补齐 083 有而当前端未见的发包 | **不新增** | 本面 2 个 c2s 双端已对齐，无缺口 |
| D6 | `OnMapTransferResult` 083 case 2..11 + Decode4 列表 vs 当前端 2 字段 LOG | **不强行对齐** | 真机多分支回包 hex 未采，盲改会错位 |
| D7 | `135/183` c2s↔s9 s2c 撞号（WHISPER=135 / REMOVE_DRAGON=183） | **分列不混，禁止仅凭数值判等** | 承第八/九面纪律 |
| D8 | `CHANGE_QUICKSLOT` 服务端 `(byte)readInt` 截断 vs 客户端 8×`write_int` | **保持既有注记，不回改** | v83 触发键全 <0x100 无撞车 |
| D9 | 083 地图名查表 `CItemInfo` vs 当前端 `String.nx` | **保持当前端，P3 ⑪ 联动逐字对账** | 同源，非冲突 |
| D10 | IMG 内容层 `IMG 解析失败` 是否继续用 `.nx` 兜底 | **按 ACTIVE 规则合规回退，结构层留证，内容层 ⏳** | 原始报错 + 65536B key 规格已留证 |
| D11 | F 维 12 项是否现在标 ✅ | **保持 ⏳，P2 真机再翻** | 不假绿 |
| D12 | `SP_267` / `String.nx` 全量对账 | **P3 ⑪ 任务面联动** | 本面不扩范围 |
| D13 | `CWorldMapDlg::OnKey` 小键盘 54→42 触发 5 号动作 | **真机补映射，不盲加** | 当前端缺对应 `KeyAction` |
| D14 | 本面是否建原子提交 / 改协议 | **不建提交、不改源码/协议** | P0 纯文档闭环；原子提交须用户授权 |

## 7. 一次性 Plan（P0–P3）

| 阶段 | 内容 | 状态 |
| --- | --- | --- |
| **P0 纯文档（本轮）** | REA→IDA→orange-wz(IMG) 只读取证；5 IDA evidence + 3 orange-wz evidence + 4 件终稿（`r01`/`r03`/`r05`/README）；`gen_opcode_matrix.py --check`；98 条重数；present | ✅ 本面闭环 |
| **P1 需裁决** | D1–D14 由用户拍板后方可动源码（默认全「保持 s9 优先、不新增 083 发包、不改协议、不建提交」） | ⏸ 待用户 |
| **P2 等 IMG + 真机** | orange-wz 内容层解析恢复（或可解析 `.wz` 原包）→ F 维 12 项逐像素；上下钻 / 特效层 / OnMapTransferResult 多分支真机 hex；`build_smoke.bat` | ⏸ 挂起 |
| **P3 续作** | ⑪ 任务（Quest）/ ⑫ 掉落（Drop）按同一 REA→IDA→IMG 只读取证 + 决策点 + 一次性 Plan 方法开面 | ⏸ 下一面 |

## 8. 限制与未知

- **IMG 内容层**：orange-wz 对 internal `.img` 一律 `IMG 解析失败`；`Data/` 下 0 个 `.wz` / 0 个 `.xml` / 20 个 `.nx`（资源已转 PKG4 预烘焙），逐像素贴图对齐依赖 P2。
- **`functions` 结构 + 三形态归一**：`angel-map-decompile.json` 的 `functions` 是 dict 非 list；曾因**同源重名覆盖**丢 15 个逻辑函数（修复前 `physical_keys=118`）、且 5 个大函数为 output-ref 溢出坏 JSON。已用 `label@EA` 键 + `normalize_code`（纯文本 / `{addr,code,refs}` JSON 串 / output-ref URL 取回）全链重跑 → `physical_keys=133` / `EMPTY 0 / OUTPUT_REF 0 / BROKEN_JSON 0 / residual_JSON_string_lines 0`（承第八面假闭环教训的第二次升级）。
- **真机缺口**：`SetWorldMapDeeper/Shallower` 上下钻观感、`TransientLayer_*` / `ProcessReflection` 特效层、`OnMapTransferResult` 多分支回包 hex 未采。
- **REA subject 单列**：既有 `rea_search_*.json` 属 `BeiDou-ClientV17.7/BeiDou.exe`（sha256 `1198fa57...`），不是 Angel(13339) 一手。
- **095 禁引**：`docs/features/port-095/**` / `Maplestory95.i64` 仅作历史注记。
- **Hindsight**：本会话 `No knowledge pages yet`，远端 `api.hindsight.vectorize.io` 不可用，无知识页可引、不重试。

## 9. 证据索引与复现

| 文件 | 内容 |
| --- | --- |
| `evidence/angel-map-symbol-inventory.json` | 133 funcs / 27 classes |
| `evidence/angel-map-decompile.json` | 133/133 伪代码（`summary` + `functions` dict） |
| `evidence/angel-beidou-map-ea-crosscheck.json` | 131/133 EA（2 missing） |
| `evidence/angel-beidou-map-ea-bytelevel-audit.json` | 133/133 by bytes + 十面一致性表 |
| `evidence/angel-map-facts.md` | 1936 行逐函数信号 + 2 opcode 汇总 |
| `evidence/orange-wz-gate-verdict.json` | ACTIVE 规则 / 29 工具 / key_spec / P1–P4 / 原始报错逐字 / conclusion |
| `evidence/orange-wz-map-structure.json` | `Data/Map children=8` 结构层真实节点 |
| `evidence/nx-content-probe.json` | .nx 合规回退顶层节点 + depth2_sample |
| `evidence/orange-wz-{node,autoparse,dir-nx,drilldown}-probe.json` | P1–P4 四路径明细 |
| `notes/r01-angel-native-map-facts.md` | 083 一手事实 + 7 段核心伪代码逐字 + c2s/s2c 对照 |
| `notes/r03-img-gate-fallback.md` | 门禁实证 / key 规格 / 合规回退 / F 维 12 项降级 |
| `notes/r05-map-four-state-audit.md` | 53 项既有笔记四态归组 |
| `README.md`（本文件） | 七维 98 条 + 十条硬发现 + D1–D14 + P0–P3 |

```bash
# 采集（已跑，全部 exit 0）
PYTHONIOENCODING=utf-8 python tools/ida_angel_map_ea.py            # 133 funcs / 27 classes
PYTHONIOENCODING=utf-8 python tools/ida_angel_map_decompile.py     # DONE 133/133 ok (physical_keys=133, 三形态归一)
PYTHONIOENCODING=utf-8 python tools/ida_angel_map_crosscheck.py    # 131/133, missing 0x9e98c2 0x58e12b
PYTHONIOENCODING=utf-8 python tools/ida_angel_map_byteaudit.py     # VERDICT: 133/133 by bytes
PYTHONIOENCODING=utf-8 python tools/ida_angel_map_facts.py         # 1936 lines / 76629 B / 2 opcode
PYTHONIOENCODING=utf-8 python tools/face10_gate_and_map_probe.py   # orange-wz verdict + Map 结构 + .nx 回退

# 门禁 + 98 条逐条重数（本面验收）
python tools/gen_opcode_matrix.py --check                          # GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)
PYTHONIOENCODING=utf-8 python tools/face10_readme_audit.py          # TOTAL_ROWS 98 / OVERALL / 逐维 / DUP_IDS [] / MISSING []
```

## 10. 性质标注

本面 = **校准 / 对照 / 决策点 / 一次性 Plan 文档**，非实现提交。全程只读：未改游戏源码 / 协议 / WZ / IMG / XML，未建提交；orange-wz 会话已 `unload_all` → `roots=[]` 收口，无根锁残留。D1–D14 须用户裁决后 P1 方可动源码。
