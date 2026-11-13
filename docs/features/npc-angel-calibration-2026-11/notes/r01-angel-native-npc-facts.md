# 第九面 NPC — Angel 083 IDB 一手事实基线

> 取证日期 2026-11-09 · 权威源 `Angel.exe (pid=29620, port=13339)` 083/v83 带符号 IDB
> 对照源 `BeiDou.exe (pid=31332, port=13337)` 同二进制 stripped
> 性质：只读取证，不含推断、不含实现修改。

## §0 七条硬结论

1. **九面 EA 字节级一致钉死同二进制**：本面 71 funcs / 15 classes，crosscheck 70/71，唯一 missing `0x6d065b`（`::_GCNpc` scalar deleting destructor）经两端各读 32 字节逐字相同洗白 → `71/71 by bytes`。`evidence/angel-beidou-npc-ea-bytelevel-audit.json` 的 `nine_face_consistency` 表：login `327/328`、statusbar_hud `547/548`、corewindow `439/441`、social_economy `752/752`、pet_summon_dragon `968/968`、activity_minigame `844/844 (by bytes)`、cashshop `938/938 (by bytes)`、skill `106/106 (by bytes)`、npc `71/71 (by bytes, after 1-EA audit)`。
2. **Angel 只补语义层**：BeiDou 13337 侧 70 个 EA 全部 `sub_XXXXXX`、`beidou_real_named=0`；Angel 侧 71 个具名（CNpc 32 / CNpcPool 14 / CVecCtrlNpc 5 / free 4 / CNpcTemplate 4 / ?$ZList 2 / CAdminShopDlg 2 / CActionMan 1 / CQuestMan 1 / CTrunkDlg 1 / CUserLocal 1 / CUtilDlgEx 1 / CWvsContext 1 / PAVCVecCtrlNpc 1 / VCNpc 1）。字节一致 ⇒ 差异是 IDA 分析覆盖，不是代码布局。
3. **本面 c2s 只有 2 个 opcode 字面量**：`COutPacket(58)`（`CUserLocal::TalkToNpc` / `CNpc::ShowQuestList`）、`COutPacket(197)`（`CNpc::GenerateMovePath`）。见 `evidence/angel-npc-facts.md §2` 汇总表。
4. **结构性差异（非冲突）**：083 `TalkToNpc @ 0x95fe9e` = `COutPacket(58)` + `Encode4(oid)` + `Encode2(x)` + `Encode2(y)`（两次 Encode2 取玩家坐标）；当前端 `TalkToNPCPacket(int32_t oid)` = `NpcActionTalkPacket(oid).dispatch()` + `write_int(oid)`。两条链不同源，按 s9 优先保持当前端，记入决策点 D1，禁止凭「都是 58」判等。
5. **083 一手证据替换 095 历史注记**：Angel `CNpc::GenerateMovePath @ 0x6d3273` 字段序逐字为 `COutPacket(197)` → `Encode4(oid)` → `Encode1(a2 /*action*/)` → `Encode1(a3 /*chatIdx*/)` → `CMovePath::Flush(v4+428, &v11, 0, 0)` → `SendPacket`，与当前端 `NpcActionMovePacket(oid, action, chat_idx, x,y,vx,vy, elems) : MovePathPacket(NPC_ACTION)` 完全对齐。既有 `docs/features/change-quickslot-persist/notes/npc-action-move-pending.md` 引用的 `Maplestory95.i64`（`CNpc::GenerateMovePath 0x671590` / `CMovePath::Encode 0x666e20` / `Flush 0x668160` / `v95 send op 241`）属 095 线，**降级为历史实现注记，不作 083 一手事实，不作校准依据**。
6. **s2c 分发八条双端全对齐**：`CNpcPool::OnPacket @ 0x6d9734` switch case `81 → OnNpcImitateData` / `82 → OnUpdateLimitedDisableInfo` / `257 → OnNpcEnterField` / `258 → OnNpcLeaveField` / `259 → OnNpcChangeController` / `260..262 → OnNpcPacket` / `263 → OnNpcTemplatePacket`；当前端 `Net/PacketSwitch.cpp` 注册 `IMITATED_NPC_DATA=81`(:295) / `LIMITED_NPC_DISABLE_INFO=82`(:339) / `SPAWN_NPC=257`(:140) / `REMOVE_NPC=258`(:246) / `SPAWN_NPC_C=259`(:141) / `NPC_ACTION=260`(:319) / `SET_NPC_SCRIPTABLE=263`(:310)。数值与语义双向一致。
7. **校准 ≠ 改回原生**：083 是校准标尺，s9 是联机协议基准；同号异义、c2s↔s2c 撞号、083 独有字段一律分列，冲突交用户裁决，不盲改源码。

## §1 三联门禁（REA → IDA → IMG）

| 门禁 | 端点 | 结果 | 证据 |
| --- | --- | --- | --- |
| IDA Angel 083 | 13339 | ✅ REACHABLE，71/71 反编译 OK | `evidence/angel-npc-symbol-inventory.json`、`angel-npc-decompile.json`(314388B) |
| IDA BeiDou stripped | 13337 | ✅ REACHABLE，70/71 EA + 1-EA 32B 字节审计 | `evidence/angel-beidou-npc-ea-crosscheck.json`、`angel-beidou-npc-ea-bytelevel-audit.json` |
| REA 既有 | `BeiDou-ClientV17.7/BeiDou.exe` | ✅ 单列（subject 非 13339） | `beidou-v17-parity/notes/evidence/r17/rea_search_Skill*.json` 等，stripped 符号层空 |
| IMG orange-wz | — | ⚠️ 未连接（仅 `json-photoshop`） | `mcp_connector_status`；降级链见 `notes/r03-img-blocked-fallback.md` |

采集脚本（均由第八面已验证 SOP 派生 + 通用 byteaudit）：

```
tools/ida_angel_npc_ea.py           符号发现 → angel-npc-symbol-inventory.json
tools/ida_angel_npc_decompile.py    71 函数全量 Hex-Rays → angel-npc-decompile.json
tools/ida_angel_npc_crosscheck.py   EA 一致性 → angel-beidou-npc-ea-crosscheck.json
tools/ida_angel_npc_byteaudit.py    missing EA 通用 32B 比对 → angel-beidou-npc-ea-bytelevel-audit.json
tools/ida_angel_npc_facts.py        opcode/Encode/Decode/case/SP_ 抽取 → angel-npc-facts.md (922 行)
```

## §2 符号清单（71 funcs / 15 classes）

类分布：`CNpc 32`、`CNpcPool 14`、`CVecCtrlNpc 5`、`<free> 4`（`CreateNpc` / `0CNpc` 构造 / `_GCNpc` scalar deleting destructor / `1CNpc` 析构）、`CNpcTemplate 4`、`?$ZList 2`（AddTail / RemoveAt）、`CAdminShopDlg 2`（GetCurNPCItem / SetNPC）、`CActionMan 1`（LoadNpcAction @ 0x40db31, size 0x572）、`CQuestMan 1`（GetQuestByNpc）、`CTrunkDlg 1`（SetNPC）、`CUserLocal 1`（TalkToNpc @ 0x95fe9e）、`CUtilDlgEx 1`（SetNPC @ 0x9a64fc）、`CWvsContext 1`（GuildNPCSay）、`PAVCVecCtrlNpc 1`、`VCNpc 1`。

CNpc 32 方法（按 `angel-npc-facts.md §1`）：`GetType/GetPos/GetPosPrev/GetZMass/GetRTTI/IsKindOf/Init/SetQuestList/SetClientActionByQuest/SetActive/SetShoeAttr/SetMoveAction/SetImitatedLook/PrepareActionLayer/GetCurrentAction/SetLayerZ/Update/OnResolveMoveAction/OnLayerZChanged/GetShoeAttr/OnMove/GenerateMovePath/ShowQuestList/GetDCRange/GetQuestDCRange/UpdateScript/SetMapleTVMessage/DrawMapleTVMessage/OnSetSpecialAction/OnUpdateLimitedInfo/RestoreLayers/GetActionFrameList`。

CNpcPool 14 方法：`FindNpc`(×2 重名，JSON 合并为 1 key → 物理 71 键 70)、`SetLocalNpc/SetRemoteNpc/Update/OnPacket/OnNpcImitateData/OnUpdateLimitedDisableInfo/OnNpcPacket/GetNpc/OnNpcTemplatePacket/OnNpcEnterField/OnNpcLeaveField/OnNpcChangeController`。

## §3 关键伪代码逐字（一手，来自 `angel-npc-decompile.json`）

### `CNpc::GenerateMovePath @ 0x6d3273 (1649 B)` — c2s 197 权威

```c
int __thiscall CNpc::GenerateMovePath(int *this, int a2, char a3)
{
  v4 = *(this + 37) != 0 ? *(this + 37) - 12 : 0;
  if ( *(*(this + 42) + 12) != 0 ) {
    if ( a2 != -1 ) {
      v6 = *(CNpc::GetActionFrameList(this, a2) + 12);
      while ( v6 != 0 ) { v5 += *(v7 + 16); }
      CVecCtrlNpc::UpdateActiveInterrupted(this: v4, a2: v5, a3: 0);
    }
    for ( i = 0; i < 55; ++i ) {
      if ( CMovePath::IsTimeForFlush(this: (v4 + 428), a2: 0) != 0 ) break;
      CVecCtrl::UpdateActive(this: v4);
    }
  }
  COutPacket::COutPacket(a1: 197);                       /*0x6d3312*/
  v10 = *(this + 41);                                    /*0x6d3317*/
  COutPacket::Encode4(this: &v11, a2: v10);              /*0x6d3325  oid*/
  COutPacket::Encode1(this: &v11, a2);                   /*0x6d3330  action*/
  COutPacket::Encode1(this: &v11, a2: a3);               /*0x6d333b  chatIdx*/
  if ( *(*(this + 42) + 12) != 0 )
    CMovePath::Flush(this: (v4 + 428), a2: &v11, a3: 0, a4: 0);
  CClientSocket::SendPacket(this: dword_BE7914, a2: &v11);
  *(this + 38) = 1;
  return ZArray<unsigned char>::RemoveAll(a1: v12);
}
```

对照当前端 `Net/Packets/NpcInteractionPackets.h:27-205` `NpcActionMovePacket`：`write_int(oid) + write_byte(action) + write_byte(chat_idx) + encode_movepath(...)` —— **字段序逐字对齐**。Flush 条件 `IsTimeForFlush` + `for(i<55)` 与 `Gameplay/MapleMap/Npc.cpp:35-38` `kNpcPathFlushMs=1000` + `:220` `path_accum_ms >= 1000 && phobj.fhid > 0` 对齐。

### `CUserLocal::TalkToNpc @ 0x95fe9e (1297 B)` — c2s 58 结构差异

```c
int __thiscall CUserLocal::TalkToNpc(CUserLocal *this, char a2, CNpc *a3)
{
  if ( *(this + 3149) != 0 ) CUserLocal::OnKeyDownSkillEnd(this);
  v4 = *(a3 + 49);
  if ( v4 != 0 && *(v4 - 4) != 0 ) {
    CNpc::ShowQuestList(this: a3);            /* 有任务列表 → 本地开窗，不发 58 */
  } else {
    COutPacket::COutPacket(a1: 58);           /*0x95fedf*/
    v10 = *(a3 + 41);
    COutPacket::Encode4(this: &v11, a2: v10); /*0x95fef4  oid*/
    v7 = (*(v5 + 16))(a1: v6);
    COutPacket::Encode2(this: &v11, a2: *v7); /*0x95ff0b  玩家 x*/
    v8 = (*(*v6 + 16))(a1: v6);
    COutPacket::Encode2(this: &v11, a2: *(v8 + 4)); /*0x95ff1f  玩家 y*/
    CClientSocket::SendPacket(this: dword_BE7914, a2: &v11);
  }
  return sub_6D9B97(a1: &a2);
}
```

083 带 `Encode2(x)+Encode2(y)`；当前端 `TalkToNPCPacket` 只 `write_int(oid)` 并顺带派 `NpcActionTalkPacket(oid,-1,0)`（197 对话姿态）。**s9 `NPCAnimationHandler` length==6 契约要求短包**，保持当前端；记 D1。

### `CNpcPool::OnPacket @ 0x6d9734 (893 B)` — s2c 八条分发

```c
switch ( a1 ) {
  case 81:  return CNpcPool::OnNpcImitateData(a1: a2);
  case 82:  return CNpcPool::OnUpdateLimitedDisableInfo(a1: a2);
  case 257: return CNpcPool::OnNpcEnterField(a1: a2);
  case 258: return CNpcPool::OnNpcLeaveField(a1: a2);
  case 259: return CNpcPool::OnNpcChangeController(a1: a2);
  default:  break;
}
if ( a1 >= 260 && a1 <= 262 ) CNpcPool::OnNpcPacket(a1, a2);
if ( a1 == 263 ) return CNpcPool::OnNpcTemplatePacket(a1: 263, a2);
return 263;
```

### `CNpcPool::OnNpcChangeController @ 0x6d9a83 (400 B)` — 授控

```c
char __thiscall CNpcPool::OnNpcChangeController(CNpcPool *this, struct CInPacket *a2)
{
  v3 = CInPacket::Decode1(this: a2);
  v4 = CInPacket::Decode4(this: a2);
  if ( v3 != 0 ) return CNpcPool::SetLocalNpc(this, a2: v4, a3: a2);
  return CNpcPool::SetRemoteNpc(this, a2: v4);
}
```

对照 `MapNpcs.cpp:31-48` 同 oid 重新下发先 `remove(oid)` 再 add；`Npc::set_control(bool)` @ `Npc.cpp:343-351` 重置 `path_accum_ms/path_start_x/path_start_y`。

### `angel-npc-facts.md` 其他一手信号

| 行 | 信号 |
| --- | --- |
| :92 | `SP_1147_ZIGZAG`, `SP_5624_DELAY` |
| :134 | `SP_3955_UI_MAPLETVIMG_TVCHAT1`, `SP_3956_UI_MAPLETVIMG_TVCHAT2` |
| :361 | `SP_1665_UI_BASICIMG_DCMARK` |
| :441 | `CNpc::SetMapleTVMessage @ 0x6d417e (17313 B)` + `SP_3942_UI_MAPLETVIMG_TVON` / `SP_3943_UI_MAPLETVIMG_TVOFF` |
| :471 | `SP_3190_UI_UIWINDOWIMG_QUESTICON_D` |
| :504 | `CNpc::ShowQuestList @ 0x6d3392 (27614 B)` + 12 个 SP_（QuestInfo/ScriptInfo/UtilDlgEx List0-3） |
| :573 | `OnNpcChangeController` s2c `Decode1 + Decode4` |
| :583-604 | `OnNpcEnterField` / `OnNpcLeaveField` `Decode4` + `GetNpcTemplate` |
| :647-684 | `OnNpcPacket` / `OnPacket` / `OnNpcTemplatePacket` `Decode4/Decode1/DecodeStr` |
| :726 | `CNpcTemplate::GetNpcTemplate @ 0x6dcb16 (6414 B)` + `SP_1666_STRING_NPCIMG` / `SP_955_NPC_07DIMG` |
| :758 | `CNpcTemplate::RegisterNpc @ 0x6dce02 (2000 B)` |
| :819 | `CUtilDlgEx::SetNPC @ 0x9a64fc (1915 B)`（当前端 `UINpcTalk.h` 注释引用的 `sub_9A64FC` 在 083 侧具名闭合） |
| :822-828 | `CVecCtrlNpc::CreateInstance @ 0x9c1ce6` `ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 600)` |

## §4 c2s / s2c opcode 对照（数值 ≠ 语义，分列）

| 方向 | opcode | 083 一手来源 | 当前端 | 判定 |
| --- | --- | --- | --- | --- |
| c2s | 58 `0x3A` | `CUserLocal::TalkToNpc` / `CNpc::ShowQuestList` `COutPacket(58)` | `TALK_TO_NPC=58`(`Net/OutPacket.h:99`) / `TalkToNPCPacket` | ✅ 数值一致，⚠️ 包体差异（D1） |
| c2s | 197 `0xC5` | `CNpc::GenerateMovePath @ 0x6d3273` `COutPacket(197)` | `NPC_ACTION=197`(`Net/OutPacket.h:190`) / `NpcActionMovePacket` | ✅ 字段序逐字对齐 |
| c2s | 60 / 61 / 162 | Angel 本面未抽到 `COutPacket` 字面量（走 `CUtilDlgEx`/`CUIShop` 另一条链） | `NPC_TALK_MORE=60` / `NPC_SHOP_ACTION=61` / `CLICK_GUIDE=162` | ⚠️ Angel 覆盖差异，非缺失（D2/D4） |
| s2c | 81 / 82 | `OnPacket` case | `IMITATED_NPC_DATA=81` / `LIMITED_NPC_DISABLE_INFO=82` | ✅ |
| s2c | 257 / 258 / 259 | `OnPacket` case | `SPAWN_NPC=257` / `REMOVE_NPC=258` / `SPAWN_NPC_C=259` | ✅ |
| s2c | 260..262 | `OnPacket` `a1>=260 && a1<=262 → OnNpcPacket` | `NPC_ACTION=260` | ✅（与 c2s 197 同名不同号，禁止判等） |
| s2c | 263 | `OnPacket` case | `SET_NPC_SCRIPTABLE=263` | ✅ |
| s2c | 304 / 305 / 306 | `CNpcPool::OnPacket` 不覆盖（对话/商店独立链） | `NPC_DIALOGUE=304` / `OPEN_NPC_SHOP=305` / `CONFIRM_SHOP_TRANSACTION=306` | ⚠️ 分列（D4） |

## §5 数据族（083 ↔ 当前端）

| 族 | Angel 083 | 当前端 | 判定 |
| --- | --- | --- | --- |
| 模板加载 | `CNpcTemplate::GetNpcTemplate` `SP_1666 STRING_Npc.img` / `SP_955 NPC/%07d.img` | `Npc.cpp:41-149` `nl::nx::Npc[extend_id(id,7)+".img"]` + `String["Npc.img"][id]` + `info.link` | ✅ |
| MapleTV | `SetMapleTVMessage @ 0x6d417e` / `DrawMapleTVMessage` + `SP_3942/3943 TVON/TVOFF` + `SP_3955/3956 TVCHAT1/2` | `Npc.h` `maple_tv/tv_basic/tv_chat/tv_media/tv_lines` + `Npc.cpp:151-198` 双屏 + `:395-401 set_tv_message` | ✅ 机制，⏳ 贴图待 IMG |
| TV 坐标 | WZ `MapleTVmsgX/Y` `MapleTVadX/Y` short | `Npc.cpp` 9250025 `msgX=65304` 按 int16 解释 = -232 | ✅ |
| 任务交叉 | `CQuestMan::GetQuestByNpc` + `CNpc::ShowQuestList` `SP_3232 QUEST_QUESTINFOIMG` / `SP_3190 QUESTICON_D` | 当前端无独立 QuestList UI（`TalkToNPCPacket` 直发 58） | ⚠️ P3 ⑪ 任务面交叉（D3） |
| 物理 | `CVecCtrlNpc::CreateInstance` Alloc(600) / `WorkUpdateActive` / `UpdateActiveInterrupted` | `Npc.cpp` `kNpcWalkForce=0.08` + `physics.move_object` 简化 | ⚠️ 完整 VecCtrl 预留（D6） |
| NPC 效果 | `SP_4419/4420 EFFECT_MAPEFFIMG_NPCSUMMON/NPCRETURN` / `SP_1665 DCMARK` | 当前端未接 | ⚠️（D7） |

## §6 限制与未知

- **IMG 门禁阻塞**：orange-wz 未连接，贴图/布局/样式维只能降级（见 `r03`），F 维 12 项全部记 ⏳，不伪造 ✅。
- **095 禁引**：`docs/features/port-095/**`、`Maplestory95.i64`（`0x671590` / `0x666e20` / `0x668160` / `v95 op 241`）仅作历史实现注记；本面 083 一手地址为 `CNpc::GenerateMovePath 0x6d3273`。
- **REA subject 单列**：既有 `beidou-v17-parity/notes/evidence/r17/rea_search_*.json` 的 subject 是 `BeiDou-ClientV17.7/BeiDou.exe`（sha256 `1198fa57ca5a7c489bae43ec13c69681d9cabe0f96762f3dc0357facf2e7d4df`），非 13339，不冒充 Angel 一手。
- **重名合并**：`CNpcPool::FindNpc` 两个重载在 `angel-npc-decompile.json` 里合并为 1 个 key（物理 70 键 / 逻辑 71 函数），facts 已按 71 计数。
- **真机缺口**：c2s 197 `length>6` 十六进制样例 / 双开观感 / 完整 VecCtrl 多节 / 下行完整 elem 插值，均为既有笔记遗留，未在本面关闭。
- 全程只读，未改游戏源码/协议，未建提交。

## §7 复现命令

```bash
PYTHONIOENCODING=utf-8 python tools/ida_angel_npc_ea.py
PYTHONIOENCODING=utf-8 python tools/ida_angel_npc_decompile.py       # DONE 71/71 ok
PYTHONIOENCODING=utf-8 python tools/ida_angel_npc_crosscheck.py      # 70/71 EAs identical
PYTHONIOENCODING=utf-8 python tools/ida_angel_npc_byteaudit.py       # VERDICT: 71/71 by bytes
PYTHONIOENCODING=utf-8 python tools/ida_angel_npc_facts.py           # 922 lines, 2 opcode literals
python tools/gen_opcode_matrix.py --check                              # GATE OK
```
