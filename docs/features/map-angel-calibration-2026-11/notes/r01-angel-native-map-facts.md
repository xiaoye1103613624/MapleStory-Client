# 第十面 地图(Map) — Angel 083 原生一手事实基线

> 取证源：`Angel.exe (pid=29620, port=13339)` 的 083/v83 带符号 IDB（imagebase `0x400000`）。
> 对照源：`BeiDou.exe (pid=31332, port=13337)` 同一份二进制的 stripped 状态。
> 性质：只读取证；非实现提交。校准 ≠ 改回原生；s9 协议优先；083/v83 唯一基准，禁引 095。

## 0. 七条硬结论

1. **十面 EA 字节级一致** — 本面 `133/133 by bytes`（2 个 missing EA 经 32 字节双端比对洗掉），与登录 327/328、HUD 547/548、core 439/441、social 752/752、pet 968/968、activity 844/844、cashshop 938/938、skill 106/106、npc 71/71 同构，钉死 Angel 与 BeiDou 是**同一份 v83 二进制的不同符号状态**，Angel 只补语义层（`3122` 修饰名 / `361` 类 / `2612` 方法 vs BeiDou `50` / `10` / `0`）。
2. **本面 Angel 侧仅 2 个 c2s opcode 字面量** — `COutPacket(135)`（`CFuncKeyMappedMan::SaveFuncKeyMap` @ `0x58df2f`）与 `COutPacket(183)`（`CQuickslotKeyMappedMan::SaveQuickslotKeyMap` @ `0x72b87e`）。地图主体是**资源装载 + 渲染管线**（`CMapLoadable` 36 方法），不是发包密集面。
3. **数值 + 语义双对齐** — `135` = 当前端 `OutPacket::CHANGE_KEYMAP`（`Net/OutPacket.h:169`）+ `ChangeKeyMapPacket`（`Net/Packets/PlayerPackets.h:102-122`）；`183` = `OutPacket::CHANGE_QUICKSLOT`（`Net/OutPacket.h:204`）+ `ChangeQuickslotPacket`（`Net/Packets/SkillMacroPackets.h:34-49`）。这是九面里第一次「c2s opcode 不靠 `数值≠语义` 兜底，而是正面全闭环」。
4. **s2c 三 case 全对齐** — `CMapLoadable::OnPacket @ 0x644446` 的 `switch(a2) case 128/129/130` 与 `Net/PacketSwitch.cpp:284/:344/:345` 的 `SET_BACK_EFFECT=128 / SET_MAP_OBJECT_VISIBLE=129 / CLEAR_BACK_EFFECT=130` + `:632/:694/:695` handler 注册逐一对上。
5. **083 内部与 s9 结构微差（记 D1）** — `SaveFuncKeyMap` 用 `Encode4(mode=0) + Encode4(count) + N×(Encode4(key) + sub_4E43FA(mapping))` 且**只在 89 键有变更时收集**；当前端 `ChangeKeyMapPacket` 是 `write_int(0) + write_int(size) + N×(write_int(key) + write_byte(type) + write_int(action))`。同源同义，键位条目编码拆分别不同 → 保持 s9 拆法，不回改。
6. **IMAGE 门禁本面第一次真闭合** — orange-wz MCP（`custom-orange_wz`，`http://127.0.0.1:10012/mcp`，29 工具）已连接且**目录结构层 WORKS**（`Data/Map` children=8 真实节点），但任何 internal `.img` 的 tree/search **内容层 FAILS**（`IMG 解析失败`，GMS↔BMS 65536B 全表 / `autoParse:true` / 下钻三形态均救不回）→ 命中 AGENTS.md 写死的合规回退条件，原始报错文本 + key 规格已逐条留证，见 `r03`。
7. **校准 ≠ 改回原生** — 083 的 `CWorldMapDlg`(`0x9eb366` 起 18 方法) / `CUIMiniMap`(`0x854b8c` 起 10 方法) 是原生 MFC 风格大窗；当前端 `UIWorldMap` / `UIMiniMap` 是 `UIDragElement` 独立真窗 + 多分辨率重锚 + 侧栏状态机。功能覆盖但实现不同源，冲突一律 s9 优先。

## 1. 取证基线与三联门禁

| 环 | 端点 / 工具 | 状态 | 产物 |
| --- | --- | --- | --- |
| REA | 既有 `beidou-v17-parity/notes/evidence/r17/rea_search_*.json` | ✅ 单列（subject = `BeiDou-ClientV17.7/BeiDou.exe`，sha256 `1198fa57...`，**不冒充 13339 一手**） | r05 §5 |
| IDA 符号 | 13339 `idautils.Functions` + `idc.demangle_name` | ✅ `133 funcs / 27 classes` | `evidence/angel-map-symbol-inventory.json` |
| IDA 反编译 | 13339 `decompile`（`include_addresses:true`） | ✅ `133/133 ok` | `evidence/angel-map-decompile.json` |
| IDA 交叉核验 | 13337 `lookup_funcs`（60/chunk） | ✅ `131/133` EA 命中 | `evidence/angel-beidou-map-ea-crosscheck.json` |
| IDA 字节审计 | `get_bytes` 32B 双端（通用版，无硬编码 EA） | ✅ `133/133 by bytes` | `evidence/angel-beidou-map-ea-bytelevel-audit.json` |
| facts 抽取 | `normalize_code` 三形态归一（纯文本 / `{addr,code,refs}` JSON 串 / **output-ref 溢出** URL 取回）+ `label@EA` 键防重名覆盖 | ✅ `1936 行 / 76629 B / 2 opcode` / `physical_keys=133` / 无残留 | `evidence/angel-map-facts.md` |
| IMG 结构层 | orange-wz `load_files(Data/Map)` + `list_children` | ✅ `children=8` 真实节点 | `evidence/orange-wz-map-structure.json` |
| IMG 内容层 | orange-wz `get_node_tree_json` / `search_node` on internal `.img` | ⚠️ `IMG 解析失败` → 合规回退 | `evidence/orange-wz-gate-verdict.json` |
| .nx 回退 | `tools/nx_inspect.py`（PKG4） | ✅ 内容层节点可读 | `evidence/nx-content-probe.json` |

5 个采集脚本：`tools/ida_angel_map_{ea,decompile,crosscheck,byteaudit,facts}.py`（`python -m py_compile` → `PYCOMPILE_OK`）。门禁 probe：`tools/orange_wz_{gate_smoke,node,autoparse,dir_nx,drilldown}_probe.py` + `tools/face10_gate_and_map_probe.py`。

**字节审计逐字**（`angel-beidou-map-ea-bytelevel-audit.json`）：
- `missing_eas = ["0x9e98c2", "0x58e12b"]`
- `0x9e98c2 ::_GCWorldMapDlg` 32 字节双端 `identical=True`
- `0x58e12b CFuncKeyMappedMan::FuncKeyMapped` 32 字节双端 `identical=True`
- `effective_conclusion = "133/133 by bytes"`
- 十面一致性表：login `327/328` / statusbar_hud `547/548` / corewindow `439/441` / social_economy `752/752` / pet_summon_dragon `968/968` / activity_minigame `844/844 (by bytes)` / cashshop `938/938 (by bytes)` / skill `106/106 (by bytes)` / npc `71/71 (by bytes)` / **map `133/133 (by bytes, after 2-EA audit)`**
- `verdict = "Angel(13339) and BeiDou(13337) are the same v83 binary in different symbol states across all ten faces. Angel only adds the symbol/semantic layer."`

## 2. 133 funcs / 27 classes 清单（类分布 Top）

`CMapLoadable 36` / `?$ZMap 20` / `CWorldMapDlg 18` / `CUIMiniMap 10` / `<free> 8` / `CFuncKeyMappedMan 5` / `CActionFrame 4` / `CWvsContext 4` / `CItemInfo 3` / `CMapleTVMan 3` / `CQuickslotKeyMappedMan 3` / `CDraggableItem 2` / `CDraggableSkill 2` / `CNpc 2` / `?$ZRef 1` / `CConfig 1` / `CDraggableMenu 1` / `CField 1` / `CMacroSysMan 1` / `CUIKeyConfig 1` / `CUIMapTransfer 1` / `CUIMapleTV 1` / `CUIToolTip 1` / `CUser 1` / `CUserLocal 1`。

`CMapLoadable` 36 方法（资源装载管线）：`Init / LoadMap / Close / Update / MakeBack / MakeCloud / MakeObj / MakeObjLayer / MakeTile / MakeObstacles / MakeVectorAnimate / RestoreBack / RestoreObj / RestoreTile / RestoreViewRange / RestoreWeather / RestoreBGM / PlayBGMFromMapInfo / PlayNextMusic / ProcessReflection / ReloadBack / SetFieldMagLevel / SetObjectState / GetObjectRect / UpdateObstacleInfo / SetLayerListVisible / SetMapObjectVisible / TransientLayer_{FireCracker,NewYear,Weather} / WeatherLayer_RemoveAll / OnPacket / OnSetBackEffect / OnClearBackEffect / OnSetMapObjectVisible / OnLeaveDirectionMode`。

`CWorldMapDlg` 18 方法：`CreateWorldMapDlg / OnCreate / OnDestroy / Draw / HitTest / OnKey / OnMouseButton / OnMouseEnter / OnMouseMove / OnButtonClicked / LoadMapList / MakeBorder / SetWorldMap / SetWorldMapDeeper / SetWorldMapShallower / CheckLinkInfo / CheckSpotInfo / ClearToolTip`。

完整 133 函数标题 + 逐函数关键信号见 `evidence/angel-map-facts.md` §1（1936 行）。

## 3. 核心伪代码逐字（从 `angel-map-decompile.json` 内层 `code` 抽出）

### 3.1 `CFuncKeyMappedMan::SaveFuncKeyMap @ 0x58df2f`（c2s 135）

```c
COutPacket::COutPacket(a1: 135); /*0x58df49*/
v2 = 0; /*0x58df4e*/
v10 = 0; /*0x58df54*/
COutPacket::Encode4(this: &v7, a2: 0); /*0x58df57*/          // mode = 0
v9 = 0; /*0x58df5c*/
LOBYTE(v10) = 1; /*0x58df62*/
v3 = this + 4; /*0x58df66*/
do /*0x58df90*/
{
  if ( *v3 != v3[445] || *(v3 + 1) != *(v3 + 446) ) /*0x58df7b*/  // 当前键位 vs 上次保存快照
    *ZArray<long>::InsertBefore(a1: -1) = v2; /*0x58df87*/         // 收集变更键索引
  ++v2; /*0x58df89*/
  v3 += 5; /*0x58df8a*/
}
while ( v2 < 89 ); /*0x58df90*/                                     // 89 键扫描
if ( dword_BEC1F8 != 0 ) /*0x58df9a*/
  CSequencedKeyMan::Restore(this: dword_BEC1F8); /*0x58df9c*/
if ( v9 != 0 ) /*0x58dfa6*/
{
  v4 = *(v9 - 4); /*0x58dfa8*/
  if ( v4 != 0 ) /*0x58dfad*/
  {
    COutPacket::Encode4(this: &v7, a2: *(v9 - 4)); /*0x58dfb3*/    // count
    for ( i = 0; i < v4; ++i ) /*0x58dfbc*/
    {
      COutPacket::Encode4(this: &v7, a2: *(v9 + 4 * i)); /*0x58dfc7*/  // key index
      sub_4E43FA(a1: this + 4 * *(v9 + 4 * i) + *(v9 + 4 * i) + 4, a2: &v7); /*0x58dfdd*/  // mapping 编码
    }
    CClientSocket::SendPacket(this: dword_BE7914, a2: &v7); /*0x58dff1*/
  }
}
```

**083 语义**：仅当 89 个键位中至少一个与快照不同才发包；`count + N×(key, mapping)` 编码。当前端 `ChangeKeyMapPacket` 是「全量 `updated_actions` 上层传什么发什么」，差异在收集策略与条目编码，不在 opcode 语义 → 记 D1，保持 s9。

### 3.2 `CQuickslotKeyMappedMan::SaveQuickslotKeyMap @ 0x72b87e`（c2s 183）

```c
int __thiscall CQuickslotKeyMappedMan::SaveQuickslotKeyMap(CQuickslotKeyMappedMan *this)
{
  _BYTE v3[4]; // [esp+8h] [ebp-1Ch] BYREF
  _BYTE v4[12]; // [esp+Ch] [ebp-18h] BYREF
  int v5; // [esp+20h] [ebp-4h]

  COutPacket::COutPacket(a1: 183); /*0x72b897*/
  v5 = 0; /*0x72b89c*/
  COutPacket::EncodeBuffer(a1: this + 4, a2: 0x20u); /*0x72b8a9*/   // 32 字节原始缓冲
  CClientSocket::SendPacket(this: dword_BE7914, a2: v3); /*0x72b8b8*/
  memcpy(a1: this + 36, a2: this + 4, a3: 0x20u); /*0x72b8c4*/      // 保存快照
  v5 = -1; /*0x72b8c9*/
  return ZArray<unsigned char>::RemoveAll(a1: v4); /*0x72b8d8*/
}
```

**083 语义**：`EncodeBuffer(32B)` 原始 8×int 触发键表。当前端 `ChangeQuickslotPacket`（`SkillMacroPackets.h:43-48`）正是 `for (uint32_t key : trigger_keys) write_int(...)` 8×int = 32B → **数值 + body 结构 + 语义三对齐 ✅**。

### 3.3 `CMapLoadable::OnPacket @ 0x644446`（s2c 三 case）

```c
void __thiscall CMapLoadable::OnPacket(char *this, int a2, struct CInPacket *a3)
{
  switch ( a2 ) /*0x64444f*/
  {
    case 128: /*0x64444f*/
      CMapLoadable::OnSetBackEffect(a1: a3); /*0x64447a*/
      break;
    case 129: /*0x64444f*/
      CMapLoadable::OnSetMapObjectVisible(a1: a3); /*0x64446c*/
      break;
    case 130: /*0x64444f*/
      CMapLoadable::OnClearBackEffect(this: (this - 8), a2: a3); /*0x64445e*/
      break;
    default:
      break; /*0x64444f*/
  }
}
```

当前端 `Net/PacketSwitch.cpp:284 SET_BACK_EFFECT = 128` / `:344 SET_MAP_OBJECT_VISIBLE = 129` / `:345 CLEAR_BACK_EFFECT = 130` + `:632/:694/:695` `emplace<..., SetBackEffectHandler/SetMapObjectVisibleHandler/ClearBackEffectHandler>()` + `:975-977` opcode 名映射 → **三 case 全对齐 ✅**。

### 3.4 `CField::LoadMap @ 0x529bb4`（地图加载入口）

```c
CMapLoadable::LoadMap(this); /*0x529bc6*/
CField::RestoreOption(a1: this); /*0x529bcd*/
CField::RestoreHelpMsg(a1: i); /*0x529bd7*/
CField::RestorePeculiarInfo(a1: this); /*0x529bde*/
CField::RestoreSeat(a1: this); /*0x529be5*/
CField::RestoreClock(a1: this); /*0x529bec*/
CField::RestoreToolTip(this); /*0x529bf3*/
CField::RestoreWeatherMsg(a1: this); /*0x529bfa*/
if ( *(*dword_BE7918 + 12464) == 0 ) /*0x529c0e*/
  CMapLoadable::PlayBGMFromMapInfo(a1: this); /*0x529c12*/
```

当前端 `Gameplay/MapleMap/MapInfo.h:57-93` `class MapInfo` 承接 `fieldlimit/cloud/bgm/mapdesc/mapname/streetname/mapmark/swim/town/hideminimap/recovery/walls/borders/seats/ladders`；`Stage` 负责装载时序 → 同源，实现分列。

### 3.5 `CWorldMapDlg::OnKey @ 0x9ee717`（ESC 返回上一级）

```c
if ( a3 >= 0 ) /*0x9ee71e*/
{
  v3 = BYTE2(a3); /*0x9ee72c*/
  if ( BYTE2(a3) == 54 ) /*0x9ee734*/
    v3 = 42; /*0x9ee738*/                       // 小键盘 * → 42 映射
  if ( *(v3 + dword_BED5A0 + 4 * v3 + 4 + 1) == 5 ) /*0x9ee742*/
  {
    (*(*(this - 1) + 52))(a1: this - 4, a2: 1); /*0x9ee769*/
    return; /*0x9ee76c*/
  }
  if ( a2 == 27 ) /*0x9ee748*/                   // ESC
  {
    v4 = *(this + 30); /*0x9ee74a*/
    if ( v4 != 0 && *v4 != 0 ) /*0x9ee755*/
    {
      CWorldMapDlg::SetWorldMapShallower(this: (this - 4)); /*0x9ee75a*/
      return; /*0x9ee75f*/
    }
  }
}
CDialog::OnKey(this, a2, a3); /*0x9ee774*/
```

当前端 `IO/UITypes/UIWorldMap.h:63 send_key(int32_t keycode, bool pressed, bool escape)` + `select_map_spot` / `jump_to_map_region`；ESC 语义经 `escape` 参数承接 → 同源，「返回上一级分区」需 P2 真机比对。

### 3.6 `CWvsContext::OnMiniMapOnOff @ 0xa129ed`

```c
int __thiscall CWvsContext::OnMiniMapOnOff(CWvsContext *this, struct CInPacket *a2)
{
  *(this + 3582) = CInPacket::Decode1(this: a2); /*0xa129fc*/
  return CUIMiniMap::ReloadMiniMap(this: dword_BED788); /*0xa12a0d*/
}
```

当前端既有笔记 `r315-minimap-onoff-ui.md`：「on：emplace/activate UIMiniMap（CharStats）；off：deactivate」→ **销案 ✅**（Decode1 开关 → ReloadMiniMap）。

### 3.7 `CWvsContext::OnMapTransferResult @ 0xa25268`（s2c 多分支）

```c
v3 = CInPacket::Decode1(this: a2); /*0xa25288*/
v4 = CInPacket::Decode1(this: a2); /*0xa2528b*/
switch ( v3 )
{
  case 2: ... case 3: ...
    *v7++ = CInPacket::Decode4(this: a2); /*0xa252db*/
  case 5: case 6: case 7: case 8: case 9: case 10: case 11: ...
}
```

当前端 `Net/Handlers/WaveAShellHandlers.cpp:2255-2273 MapTransferResultHandler` 读 `mode` + `vip` 并 `LOG(LOG_DEBUG, ...)` → 083 是 `Decode1(mode)+Decode1(flag)+按 mode 分流的 Decode4 列表`，当前端简化为 2 字段日志 → 记 D6「不强行对齐 083 多分支」。

## 4. c2s / s2c opcode 对照

| 方向 | dec | hex | Angel 083 一手 | 当前端 | 判定 |
| --- | --- | --- | --- | --- | --- |
| c2s | 135 | 0x87 | `CFuncKeyMappedMan::SaveFuncKeyMap` `COutPacket(135)` | `CHANGE_KEYMAP=135`(`OutPacket.h:169`) + `ChangeKeyMapPacket`(`PlayerPackets.h:103`) | ✅ 数值语义对齐 / ⚠️ body 收集策略微差 |
| c2s | 183 | 0xB7 | `CQuickslotKeyMappedMan::SaveQuickslotKeyMap` `COutPacket(183)` `EncodeBuffer(0x20)` | `CHANGE_QUICKSLOT=183`(`OutPacket.h:204`) + `ChangeQuickslotPacket` 8×`write_int`(`SkillMacroPackets.h:43`) | ✅ 三对齐 |
| s2c | 128 | 0x80 | `OnPacket case 128` → `OnSetBackEffect` | `SET_BACK_EFFECT=128` + `SetBackEffectHandler` | ✅ |
| s2c | 129 | 0x81 | `OnPacket case 129` → `OnSetMapObjectVisible` | `SET_MAP_OBJECT_VISIBLE=129` + handler | ✅ |
| s2c | 130 | 0x82 | `OnPacket case 130` → `OnClearBackEffect` | `CLEAR_BACK_EFFECT=130` + handler | ✅ |
| c2s | 38 | 0x26 | （083 无独立 `COutPacket(38)` 字面量，走 `CNpcPool`/`CField` 换图链路） | `CHANGEMAP=38` `ChangeMapPacket`(`GameplayPackets.h:25-40`) | ⚠️ 交叉面/命名空间分列 |
| c2s | 100 | 0x64 | — | `CHANGE_MAP_SPECIAL=100` `ChangeMapSpecialPacket`(`MapInteractPackets.h:14`) | s9 通道 |
| c2s | 101 | 0x65 | — | `USE_INNER_PORTAL=101` `UseInnerPortalPacket`(`MapInteractPackets.h:28`) | s9 通道 |
| c2s | 207 | 0xCF | — | `PLAYER_MAP_TRANSFER=207` `PlayerMapTransferPacket`(`GameplayPackets.h:289`) | s9 通道 |
| s2c | 376 | 0x178 | （`CWorldMapDlg` 无直接回包字面量） | `WORLD_MAP_PLAYERS=376` `WorldMapPlayersHandler`(`WorldMapHandlers.cpp:14`) | s9 通道 |
| s2c | 377 | 0x179 | — | `WORLD_MAP_DROP_RESULT=377` `WorldMapDropResultHandler`(`WorldMapHandlers.h:12`) | s9 通道 |
| s2c | 159 | 0x9F | `CQuickslotKeyMappedMan::OnInit` 下行 | `QUICKSLOT_INIT=159` `QuickslotInitHandler`(`PacketSwitch.cpp:306/:654`) | ✅ |

**纪律**：083 与 s9 是两套命名空间；`135` 在 `PacketSwitch.cpp:218` 另有 `WHISPER=135`（s2c）、`183` 在 `:262` 有 `REMOVE_DRAGON=183`（s2c）——c2s↔s2c 撞号不同义，禁止仅凭数值判等（承第八/九面 `数值重合 ≠ 语义重合`）。

## 5. 数据族 / SP_ 节点（IMG 结构层留证）

- `?$ZMap` 20 方法：`Insert / GetAt / RemoveAll / RemoveKey / ResizeHashTable`（`@ 0x72631a/0x72636d/0x726440/0x672948/0x675a77`）。
- `CItemInfo::FindMapByName @ 0x5cfaf3` / `GetMapString @ 0x5cf792` / `IterateMapString @ 0x5d0467` — 地图名 `String.wz` 查表链。
- `SP_267_YOU_ARE_CURRENTLY_AT_A_PLACE_WHERE_R_NTHE_WORLD_MAP_IS_NOT_AVAILABLE`（`CUIMiniMap`/`UseFuncKeyMapped` 命中）— 世界地图不可用提示。
- `CNpc::SetMapleTVMessage @ 0x6d417e` / `DrawMapleTVMessage @ 0x6d4ad4` — 自由市场 MapleTV 双屏（第九面交叉，本面命中 2 函数）。
- orange-wz 目录结构（`evidence/orange-wz-map-structure.json`）：`Data/Map` children=8 = `Back / Map / Obj / Tile / WorldMap / Effect.img` 等；子层 `Back`(Amoria.img…)、`Map`(Map0…)、`Obj`(acc1.img…)、`Tile`(allblackTile.img…)、`WorldMap`(WorldMap.img…)。
- .nx 回退内容层（`evidence/nx-content-probe.json`）：`smap.nx 147` / `zmap.nx 151` / `Map.nx 7` / `UI.nx 20` / `String.nx 20` 个 root children。

## 6. 限制与未知

- **IMG 内容层**：orange-wz 对 internal `.img` 一律 `IMG 解析失败`，逐像素贴图对齐（F 维 12 项）走 .nx 回退，真机逐像素仍 ⏳ → r03 / P2。
- **`functions` 结构**：`angel-map-decompile.json` 的 `functions` 是 dict 非 list（`CNpcPool::FindNpc` 式重名合并风险本面未触发，但读取器仍按多态归一，承第八面教训）。
- **真机缺口**：`CWorldMapDlg::SetWorldMapDeeper/Shallower` 上下钻真机观感、`TransientLayer_*` 特效层实机、`CMovePath`/`VecCtrl` 地图边界（第九面 L3 遗留）仍待 P2。
- **095 禁引**：`docs/features/port-095/**`、`Maplestory95.i64` 相关 EA/字段序仅作历史注记，不作 083 一手依据。
- **REA subject 单列**：既有 `rea_search_*.json` 属 `BeiDou-ClientV17.7/BeiDou.exe`，不冒充 13339。

## 7. 复现命令

```bash
PYTHONIOENCODING=utf-8 python tools/ida_angel_map_ea.py
PYTHONIOENCODING=utf-8 python tools/ida_angel_map_decompile.py        # 133/133 ok
PYTHONIOENCODING=utf-8 python tools/ida_angel_map_crosscheck.py       # 131/133
PYTHONIOENCODING=utf-8 python tools/ida_angel_map_byteaudit.py        # 133/133 by bytes
PYTHONIOENCODING=utf-8 python tools/ida_angel_map_facts.py            # 1936 lines / 2 opcode
PYTHONIOENCODING=utf-8 python tools/face10_gate_and_map_probe.py      # orange-wz 门禁 verdict + Map 结构 + .nx 回退
python tools/gen_opcode_matrix.py --check                              # GATE OK
```
