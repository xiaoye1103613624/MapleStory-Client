# r01 · Angel 083 技能（Skill）面原生事实基线

> 第八面 · 2026-11-09 · 一手源 = Angel.exe pid 29620 port 13339 的 083/v83 带符号 IDB
> 性质：观察层事实记录，不含推断、不含裁决、不含实现。逐字伪代码见 `evidence/angel-skill-decompile.json`，语义抽取见 `evidence/angel-skill-facts.md`（1514 行 / 65734 B）。

## §0 七条硬结论

- **0.1 同二进制、只补语义（第八面闭环）**：Angel 技能面 106 函数 / 22 类，EA 交叉 105/106；唯一缺失 `0x8aa4e4 = CWndSkillGuide::OnKey` 经 `get_bytes` 双端 32 字节逐字比对后判为 **106/106 by bytes**（`evidence/angel-beidou-skill-ea-bytelevel-audit.json`）。八面 EA 字节级一致（登录 327/328、HUD 547/548、core 439/441、social 752/752、pet 968/968、activity 844/844、cashshop 938/938、**skill 106/106**）。Angel 3122 修饰名 / 361 类 / 2612 方法 vs BeiDou 50 / 10 / 0，差异全在语义层。
- **0.2 技能面是「窗口族 + 数据族 + 战斗族」三族并立**：Angel 侧 `CUISkill`/`CUISkillEx`/`CUISkillChangeConfirm`/`CWndSkillGuide`/`CDraggableSkill`/`CUIToolTip` 窗口族，`CSkillInfo`/`SKILLENTRY`/`SkillInfo`/`MOBSKILLLEVELDATA` 数据族，`CUserLocal::DoActiveSkill_*` 12 子型 + `CalcDamage`/`CMob` 战斗族。当前端对应 `UISkillBook`(单类) + `SkillBook`/`SkillData`/`SkillMacros` + `Skill`/`AttackAndSkillPackets`。
- **0.3 Angel 侧 c2s opcode 7 个**：`90 / 91 / 92 / 93 / 101 / 104 / 165`（`angel-skill-facts.md:1504-1514`）。**与当前端数值重合但语义不同**（详见 §4），083 与 s9 是两套 opcode 命名空间，校准时须 c2s↔s2c 分列。
- **0.4 技能升级走 `CanSendExclRequest(500)` 互斥门禁 + `COutPacket(90)`**：`CWvsContext::SendSkillUpRequest @ 0xa23cf3` 逐字 `Encode4(get_update_time())` + `Encode4(skillid)` + `SendPacket`；当前端 `SpendSpPacket(int32_t skill_id)` = `SPEND_SP(90)` + `write_time()` + `write_int(skill_id)` → **✅ 全链一致（非冲突）**。
- **0.5 当前端存在 083 无对应的「资源缺失降级」路径**：`UISkillBook.cpp:1098-1152 send_spup` 在 `UIWindow.img/Skill/skillPoint` 缺失、BT_OKAY 未创建时直接 `SpendSpPacket(id).dispatch()` 保证技能仍能升级（用户报「升级技能按键没反应」的修复）。Angel 侧 `CUISkill::OnSkillLevelUpButton` 走完整面板，无此降级——属当前端为适配残缺 WZ 的**增量**，非「不如原生」。
- **0.6 现有技能笔记是独立的 BeiDou.exe(V17.7) REA 线**：`docs/features/beidou-v17-parity/notes/evidence/r17/rea_search_Skill*.json` 的 subject 为 `BeiDou-ClientV17.7/BeiDou.exe`(sha256 `1198fa57...`)，`rea_search_SkillEx/SkillMacro/SkillUp.json` 全 n:0 → stripped 端符号层几乎全空。**不得冒充 Angel 13339 一手事实，须单列**（见 r05）。
- **0.7 校准 ≠ 改回原生**：改回 083 opcode 会断 s9 联机；所有数值重合但语义分歧项交用户裁决（README §6）。

## §1 三联门禁

| 门禁 | 状态 | 说明 |
| --- | --- | --- |
| REA shipped-artifact | ✅ | 既有 r17 REA 链（BeiDou V17.7 线，单列；仅作反证 stripped 端符号层空白） |
| IDA Angel 13339 + BeiDou 13337 | ✅ | 106/106 反编译 + 字节级 106/106；6 件 evidence |
| orange-wz(IMG) | ⚠️ 阻塞 | `mcp_connector_status` 仅 `json-photoshop`（状态未知），orange-wz 未连接 → F 样式维整体降级 `⏳`，见 r03 |

### 1.1 端点表
| 端点 | pid | port | imagebase | strings | 用途 |
| --- | --- | --- | --- | --- | --- |
| Angel.exe | 29620 | 13339 | 0x400000 | 1281 | 修饰名 / 反编译语义（一手） |
| BeiDou.exe | 31332 | 13337 | 0x400000 | 3889 | 字符串 / EA 字节级交叉（一手） |

### 1.2 采集脚本 / evidence
| 类别 | 文件 |
| --- | --- |
| 脚本 | `tools/ida_angel_skill_{ea,decompile,crosscheck,byteaudit,facts}.py` |
| evidence | `angel-skill-symbol-inventory.json`(28543B / 106 funcs / 22 cls)、`angel-skill-decompile.json`(510807B / 106 全 OK)、`angel-beidou-skill-ea-crosscheck.json`(19699B / 105/106)、`angel-beidou-skill-ea-bytelevel-audit.json`(2484B / 106/106 by bytes)、`angel-skill-facts.md`(65734B / 1514 行 / 7 opcode literals) |
| notes | `r01-angel-native-skill-facts.md`(本件)、`r03-img-blocked-fallback.md`、`r05-skill-four-state-audit.md` |

### 1.3 本面独有技术点
1. `get_bytes` 洗 1-EA 缺失：`0x8aa4e4` 双端 32 字节 `f6 44 24 0b 80 75 16 83 7c 24 04 1b 75 0f 8b 0d 80 10 bf 00 85 c9 74 05 e8 c4 ff ff ff c2 08 00` 完全相同 → IDA 覆盖差异非布局差异（与第七面 `0x4b7404` 同构）。
2. `code_of` 双形态归一：decompile MCP 返回「纯伪代码文本」或 `{"addr","code","refs"}` JSON 串两种形态，facts 脚本已兼容解析内层 `code`，修复后 234 行 → 1514 行（65734 B），7 个 opcode 字面量全抽出。
3. c2s↔s2c 数值撞号识别：083 的 `104/165` 与 s9 的 s2c `FAMILY_SUMMON_REQUEST/UPDATE_CHAR_BOX` 同号不同向。

## §2 符号清单（106 funcs / 22 classes）

| 类族 | 数量 | 代表 |
| --- | --- | --- |
| `<free>` | 19 | `get_skill_root_from_job`(0x4a8c4f)、`is_skill_need_master_level`、`is_keydown_skill`、`is_able_to_map_macrosys_skill`、`is_heros_will_skill`、`get_novice_skill_point`、3×`get_random_*_attack_action`、4×`is_correct_*_attack`、`is_correct_job_for_skill_root`、`is_antirepeat_buff_skill`、`play/stop_skill_sound`、2 构造器(`0CUISkill`/`0CWndSkillGuide`)、`ZList::RemoveAll` |
| `CUserLocal` | 30 | `DoActiveSkill` + 12 子型(Melee/Shoot/VehicleShoot/MagicAttack/TownPortal/StatChange/Heal/Prepare/BoundJump/SmokeShell/RecoveryAura)、`SendSkillUseRequest`/`SendSkillCancelRequest`/`DoActiveSkill_CombatStep`/`DoAntiMacroSkill`、`TryDoing*`/`TryRegisterTeleport`、`OnSkillCooltimeSet`/`OnKeyDownSkillEnd`/`OnOpenSkillGuide`、`ValidateItemSkill`/`ValidateSkillBonus`/`GetProperBulletPosition`/`IsTeleportSkillAvailable`/`TryDoingPreparedSkill`/`FindParty` |
| `CWvsContext` | 7 | `SendSkillUpRequest`(0xa23cf3)、`OnChangeSkillRecordResult`(0xa1e48c)、`GetSkillLevelUpState`/`SetSkillCooltimeOver`/`RemoveSkillCooltimeOver`/`OnSkillUseResult`/`OnSkillExpireMessage` |
| `SKILLENTRY` | 8 | `GetLevelData`(0x760f23)/`GetSpecialUOL`/`GetEffectUOL`/`GetScreenEffectUOL`/`GetRandomAppointedAction`/`IsActionAppointed`/`IsCorrectWeaponType`/`IsCorrectAppointedAction` |
| `CUISkill` | 5 | `OnCreate`(0x8aaa88)/`OnSkillLevelUpButton`/`CloseSkillGuide`/`GetSkillRootVisible`/`OpenCurSkillGuide` |
| `CSkillInfo` | 4 | `GetSkill`(0x75c755)/`GetSkillRootVisible`/`GetSkillLevel`/`CheckConsumeForActiveSkill`(0x764256) |
| `CDraggableSkill` | 4 | `OnDropped`/`OnDoubleClicked`/`MapMacro`(0x4fb112)/`MapFuncKey` |
| `CUser` | 5 | `SetAttackAction`/`ShowSkillEffect`/`ShowSkillPrepare`/`ShowSkillSpecialEffect`/`GetDashingSkill` |
| `CUserRemote` | 4 | `OnMelee/Shoot/Magic/BodyAttack` |
| `CUISkillEx` | 3 | `ShiftMacroUIState`(0x8bad5a)/`OnButtonClicked`(0x8bb2bf)/`OnSkillLevelUpButton` |
| 其余 | 17 | `CField`(2)、`CMob`(2)、`CUIToolTip`(2)、`CalcDamage`(2)、`SkillInfo`(2)、`CAnimationDisplayer`、`CPet`、`CUISkillChangeConfirm`、`CWndSkillGuide`、`MOBSKILLLEVELDATA`、`SecondaryStat` |

## §3 关键伪代码逐字（Angel 一手）

- `CWndSkillGuide::OnKey @ 0x8aa4e4`(430B)：`if ( a3 >= 0 && a2 == 27 && dword_BF1080 != 0 ) return CUISkill::CloseSkillGuide(this: dword_BF1080);` —— ESC(a2=27) 关技能指引。
- `CWvsContext::SendSkillUpRequest @ 0xa23cf3`(1401B)：`result = CanSendExclRequest(a1:500,a2:0); if(result!=0){ COutPacket::COutPacket(a1: 90); Encode4(get_update_time()); Encode4(a2 /*skillid*/); CClientSocket::SendPacket(dword_BE7914,&v6); *(this+2089)=1; *(this+2090)=get_update_time(); }`
- `CUserLocal::SendSkillUseRequest @ 0x96d399`(8263B)：`CanSendExclRequest(a1:300,a2:0)`；`if (CSkillInfo::GetSkillLevel(dword_BE78DC,v10,*pSkill,0) <= 0) return 0;`；`v12 = *pSkill/1000%10; if((v12==0||v12==9) ...)`。
- `CSkillInfo::GetSkill @ 0x75c755`：`if ( a2 / 10000 / 100 == 22 || a2 / 10000 == 2001 ) return 0;` —— 22xx/2001(Evan 系) 特判。
- `CWvsContext::OnChangeSkillRecordResult @ 0xa1e48c`：`Decode1` → `Decode2(count)` → 循环 `Decode4(skillid)`+`Decode4(level)`（level<0 → RemoveKey，否则 Insert）+ `Decode4` + `if(is_skill_need_master_level(skillid)) Insert` + `DecodeBuffer(v8,8)`。
- `CUISkillEx::OnButtonClicked @ 0x8bb2bf`：`if (a2>=0x7DA && a2<0x7DE) OnSkillLevelUpButton(a2-2010)`；`a2==2023` → `ShiftMacroUIState(0)`；`a2==2024` → `COutPacket::COutPacket(a1: 165)` + `SendPacket`（**c2s 165 无 body**）；其余 → `CUIWnd::OnButtonClicked`。
- `CUISkill::OnSkillLevelUpButton @ 0x8acf1a`：`CanSendExclRequest(a1:500,a2:0)` + `get_novice_skill_point(cd)`。
- `CUserLocal::DoActiveSkill_Prepare @ 0x96a86e`(14079B)：**`COutPacket(93)`** + `Encode4/Encode1/Encode2` + case 3/4/5 + `SP_2843/2844/2849/2850/4374`（子弹/武器/箭/标枪不足提示）。
- `CDraggableSkill::MapMacro @ 0x4fb112`：`is_able_to_map_macrosys_skill(*(this+6))` → `CSkillInfo::GetSkill` → `SKILLENTRY::GetLevelData` → `CMacroSysMan::FlushToSvr()`。
- `SKILLENTRY::GetLevelData @ 0x760f23`(7130B)、`CUISkill::OnCreate @ 0x8aaa88`(15602B)、`CUISkillEx::ShiftMacroUIState @ 0x8bad5a`(9527B, `SP_4346_UI_UIWINDOWIMG_SKILLMACROEX_BTMACROOUT`)。

## §4 c2s opcode 对照表（核心：数值重合 ≠ 语义重合）

| Angel 083 (dec/hex) | 来源函数 | 当前端同数值 | 判定 |
| --- | --- | --- | --- |
| `90 / 0x5A` | `SendSkillUpRequest` | `SPEND_SP = 90` (`OutPacket.h:144`, `PlayerPackets.h:90-99`) | ✅ 全链一致（含 body：time + skillid） |
| `91 / 0x5B` | `DoActiveSkill_Heal/RecoveryAura/SmokeShell/TownPortal` | `USE_SKILL = 91` (`OutPacket.h:180`) | ✅ 一致 |
| `92 / 0x5C` | `SendSkillCancelRequest` | `CANCEL_BUFF = 92` (`OutPacket.h:146`, `CommonPackets.h:32`) | ⚠️ 同号异义，s9 优先 |
| `93 / 0x5D` | `DoActiveSkill_Prepare` | `SKILL_EFFECT = 93` (`OutPacket.h:184`) | ⚠️ 同号异义，s9 优先 |
| `101 / 0x65` | `TryRegisterTeleport` | `USE_INNER_PORTAL = 101` (`OutPacket.h:174`, `MapInteractPackets.h:26`) | ⚠️ 同号异义，s9 优先 |
| `104 / 0x68` | `DoAntiMacroSkill` | s2c `FAMILY_SUMMON_REQUEST = 104` (`PacketSwitch.cpp:273`) | ⚠️ c2s↔s2c 撞号，分列 |
| `165 / 0xA5` | `CUISkillEx::OnButtonClicked`(a2==2024) | s2c `UPDATE_CHAR_BOX = 165` (`PacketSwitch.cpp:254`) | ⚠️ c2s↔s2c 撞号，分列 |

> 另：当前端技能相关 opcode 还有 `USE_SKILL_BOOK=82` / `SKILL_MACRO=110` / `MAKER_SKILL=113` / `CHANGE_QUICKSLOT=183`；s2c `UPDATE_SKILL(36)` / `SKILL_MACROS(124)` / `SKILL_EFFECT(190)` / `CANCEL_SKILL_EFFECT(191)` / `SHOW_COMBO(225)` / `SKILL_LEARN_ITEM_RESULT(51)` / `SUMMON_SKILL(180)` 等。083 与 s9 命名空间不同，禁止仅凭数值判等。

## §5 数据族

- `SKILLENTRY::GetLevelData` 家族（8 方法）对应当前端 `SkillData::Stats{float damage; int32_t matk,fixdamage,mastery; uint8_t attackcount,mobcount,bulletcount; int16_t bulletcost; int32_t hpcost,mpcost; float chance,critical,ignoredef,criticaldamage,hrange; Rectangle<int16_t> range;}`。
- 当前端 `SkillBook`：`set_skill(id,level,masterlevel,expiration)` / `has_skill` / `get_level` / `get_masterlevel` / `get_expiration` / `collect_passives()`(ordered map 防低级覆盖高级) / `collect_required(id)`；private `struct SkillEntry{int32_t level; int32_t masterlevel; int64_t expiration;}` + `unordered_map<int32_t,SkillEntry>`。
- 当前端 `SkillMacros`：`struct SkillMacroSlot{string name; uint8_t shout=0; int32_t skill1/2/3=0;}` + 单例 `MAX_SLOTS=5` + `assign_skill(id)`。
- `SkillId::Id` 枚举逐职业技能 id（Beginner 8/12/1000..1005，Swordman 1000000..，F/PMage 2100000.. 等）。
- SP_ 节点样本：`SP_1410_CANVASFONT` / `SP_8_TAHOMA` / `SP_2843/2844/2849/2850/4374`（消耗不足）/ `SP_4346_..._BTMACROOUT`。

## §6 一手限制与未知

- 106 函数全部反编译成功，**无 `sub_47xxxx` 未命名分支遗留**（区别于第七面 24 个）；符号层完整性显著优于商城面。
- `0x8aa4e4` 是唯一 EA 缺失，已用字节级证据洗清；不得再写成「105/106 阈值口径」。
- IMG 门禁阻塞 → 窗口贴图 / 图标 / 字体 / 坐标像素级比对整体降级 `⏳`（r03）。
- 既有 r17 REA 属 BeiDou V17.7 线，非 13339 一手，单列不混用。
- 095 线（`docs/features/port-095/**`）禁引；全程未改任何游戏源码 / 协议，未建提交。

## §7 复现命令

```
python -m py_compile tools/ida_angel_skill_ea.py tools/ida_angel_skill_decompile.py tools/ida_angel_skill_crosscheck.py tools/ida_angel_skill_byteaudit.py tools/ida_angel_skill_facts.py
python tools/ida_angel_skill_ea.py           # → angel-skill-symbol-inventory.json
python tools/ida_angel_skill_decompile.py   # → angel-skill-decompile.json (106/106)
python tools/ida_angel_skill_crosscheck.py  # → angel-beidou-skill-ea-crosscheck.json (105/106)
python tools/ida_angel_skill_byteaudit.py   # → angel-beidou-skill-ea-bytelevel-audit.json (106/106 by bytes)
python tools/ida_angel_skill_facts.py       # → angel-skill-facts.md (1514 行 / 7 opcode)
python tools/gen_opcode_matrix.py --check   # → GATE OK
```
