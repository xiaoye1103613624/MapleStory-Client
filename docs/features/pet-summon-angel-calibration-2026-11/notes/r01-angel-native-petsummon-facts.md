# r01 — Angel 083「宠物·召唤兽·龙」一手反编译语义（逐字 Hex-Rays）

> 校准源：Angel(13339) = `E:\MXD\反编译IDA\v83\`Angel.i64`，module `` `Angel.exe ``，083/v83 GMS 系带符号 IDB。
> 本文件只记录 Angel 侧「观察到的事实」与「有依据的推断」，不含当前端源码判断（见 r02/r04）。
> 所有坐标/立即数/字节布局均来自 `evidence/angel-petsummon-decompile-full.json`（392 函数纯伪代码），可 grep 复核。

## §0 七条硬结论

1. **同一份 v83 二进制不同符号状态**：`angel-beidou-petsummon-ea-crosscheck.json` 逐字 `"same binary layout (968/968 EAs identical; differences are IDA analysis coverage, not code)"`。Angel 968 具名函数 / 64 类 → BeiDou 968/968 全部同 EA，0 缺失，其中 966 个在 BeiDou 侧为 `sub_*`、仅 2 个自带名字。五面一致（登录 327/328、HUD 547/548、核心窗口 439/441、社交 752/752、宠物 968/968）。
2. **Angel 补的是符号语义层，不是功能/版本**：CPet 28 / CSummoned 16(+Base 1) / CDragon 10 / CMob 51 / CVecCtrlPet 6 / CVecCtrlSummoned 6 / CVecCtrlDragon 2 / CUIPetEquip 2 / CPetTemplate 2 全部有修饰名，BeiDou 侧同名函数退化为 `sub_*`；二进制字节布局逐字相同。
3. **宠物本体 = Cash 物品槽（type=5）**：`CPet::GetItemSlot` 明确走 `CharacterData::FindCashItemSlotPosition(this: v3, a2: 5, a3: *(this + 20))` → `CharacterData::GetItem(this: v3, a2: &v6, a3: 5, a4: CashItemSlotPosition) + 4`。等级/亲密度/技能都从这块 itemslot 的固定偏移读 `_ZtlSecureFuse`。
4. **`CUIPetEquip` 是「三只宠物页签」控件，不是独立浮窗**：`OnButtonClicked` 逐字 `a2 == 2000 → CUIEquip::TogglePetEquip(dword_BED64C)`、`a2 > 0x7D0 && a2 <= 0x7D3 → *(this + 360) = a2 - 2001; InvalidateRect`。即 ctrl id **2000=关闭/切换，2001/2002/2003 = 三宠物页（选中索引 0/1/2 存 this+360）**，且被 `CUIEquip`（装备窗）Toggle 拉起 → 宠物穿戴是**装备窗的嵌入子面板**，严谨对应当前端 `UIEquipInventory.cpp:622 BtPet1/2/3`。
5. **骑宠/驯兽/孵化在 083 原生已存在且成族**：`CDraggableItem` 里 13 条 SP_ 骑宠校验文案（`SP_3818` 现金鞍具骑乘、`SP_873` 需驯服怪、`SP_874` 需装鞍、`SP_386` 请选择宠物、`SP_877` 与当前宠物不兼容、`SP_879` 无存活宠物、`SP_880` 宠物无法穿戴、`SP_4378` 迷你龙蛋孵化需现金商店孵化器）。`SP_5454_THIS_ITEM_CAN_ONLY_BE_EQUIPPED_ON_EVAN`（Evan 龙骑）也在 083。
6. **`CPet::OnMove` 委托 `CMovePath::OnMovePacket`（本次单独补全）**：`0x70474d` 逐字 `CMovePath::OnMovePacket(this: (*(this + 36) != 0 ? *(this + 36) - 12 + 428 : 428), a2);`，引用 `?OnMovePacket@CMovePath@@QAEXAAVCInPacket@@H@Z @ 0x68b371`。宠物/召唤体移动走统一 `CMovePath` 解析链。
7. **「校准 ≠ 改回原生」延续**：原生以 `CPet`/`CSummoned`/`CDragon` 三个实体类 + `CVecCtrl*` 物理控制器 + `CActionMan/CActionFrame/CAnimationDisplayer` 动作帧驱动真实渲染；当前端以 FieldMarker/数据包壳重构，非一一对应（详见 r02/r04）。s9 联机适配冲突一律交用户裁决，不盲改。

## §1 取证基线与门禁

| 项 | 值 | 来源 |
|---|---|---|
| 端点 | 13339 Angel / 13337 BeiDou healthy，13338(095 headless) 不用 | win_bash health 探针 |
| 类索引 | `_angel_class_index_full.json` 361 类；窄筛 33 类/235 方法，宽扫 116 类/1189 方法 | tools/_gen_petsummon.py |
| 采集脚本 | `tools/ida_angel_petsummon_{ea,decompile,normalize,crosscheck,facts}.py`（5 个，py_compile 全 OK） | 社交面模板派生 |
| EA 索引 | `angel-petsummon-ea.json` = **968 具名函数 / 64 类** | EA 脚本 |
| 反编译 | `angel-petsummon-decompile.json` **392/392 OK**；normalize `total 392 trunc_fixed 4 empty 1` | decompile+normalize |
| 空响应补全 | `CPet::OnMove @ 0x70474d`（normalize 阶段 EMPTY，直连 13339 decompile 写回 full.json 后重跑 facts） | 本面单独动作 |
| 语义抽取 | `angel-petsummon-facts.md` **5819 行 / 270482 B**，SP_ 全集 **185 个**（`UI_UIWINDOWIMG` 仅 6），53 类标题 + §3 坐标段 :6358 起 | facts 脚本 |
| EA 一致性 | `angel-beidou-petsummon-ea-crosscheck.json` **968/968 同 EA / 0 missing / 966 sub_* + 2 具名** | crosscheck 脚本 |
| 协议/Decode | 60 个函数含 `COutPacket::COutPacket` / `CInPacket::Decode1/2/4/8` / `Encode*` / `SendPacket` | full.json 正则扫描 |

**三联门禁**：REA ✅（attached IDA profile，SHA-256 绑定，结论区分观察/推断/未知）· IDA ✅（Angel+BeiDou 双端反编译+EA 交叉）· IMG ⚠️ **阻塞**（orange-wz MCP 未接入本 DSH Host，`mcp_connector_status` 仅 json-photoshop、`~/.cursor/mcp.json:3-6` 仅声明、`img-evidence-audit.md:7` 已知 `Index 16 out of bounds`）→ 替代链 = 仓库 `.nx` 节点字面量 + `nx_inspect.py` dump + 正确端截图（五面一致）。

## §2 CPet（28 方法，逐字）

`CPet` 方法名全集（EA 侧）：`Init / SetShoeAttr / GetItemSlot / GetLevel / SetMoveAction / GetBodyRect / PrepareActionLayer / Update / OnResolveMoveAction / OnMove / OnAction / OnNameChanged / OnActionCommand / SetPositionContext / OnAvatarModified / OnValidateStat / ParseCommand / ChatCommand / CursedChatCommand / DoAction / GetUpgradePetSkill / UpdatePetAbility / OnLoadExceptionList / GetAdditionalLayer / SetSetItemEffect / SetSetItemBackground / LoadLayer / AutoSpeakingByEvent`。

### §2.1 数据模型：物品槽固定偏移（★ 校准锚点）

- **`CPet::GetItemSlot`**（750B）：
  ```
  result = (*(**(this + 37) + 12))(a1: *(this + 37));
  v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v6) + 4);
  CashItemSlotPosition = CharacterData::FindCashItemSlotPosition(this: v3, a2: 5, a3: *(this + 20));
  v5 = *(CharacterData::GetItem(this: v3, a2: &v6, a3: 5, a4: CashItemSlotPosition) + 4);
  return v5;
  ```
  → 宠物本体挂在 `CharacterData` 的 **a2=5（Cash/宠物装备栏）**，`a3 = *(this+20)`（宠物 SN/槽位），返回 `GW_ItemSlotBase+4` 指针。
- **`CPet::GetLevel`**（246B）：`ItemSlot = CPet::GetItemSlot(this); if ( ItemSlot != 0 ) return _ZtlSecureFuse<unsigned char>(a1: ItemSlot + 53, a2: *(ItemSlot + 55)); else return -1;` → **等级 = itemslot+53 处 1 字节**（双份校验 fuse，+55 为校验副本）。
- **`CPet::GetUpgradePetSkill`**（414B）：`v2 = sub_4E81DB(a1: v4 + 81, a2: *(v4 + 85))` → **__int16 位掩码技能字**，itemslot+81/+85。
- **`CPet::OnValidateStat`**（1906B）：
  ```
  *(this + 42) = _ZtlSecureFuse<short>(a1: ItemSlot + 59, a2: *(ItemSlot + 63));   // 亲密度 closeness（short）
  *(this + 43) = _ZtlSecureFuse<unsigned char>(a1: v4 + 67, a2: *(v4 + 69));      // 1B 状态
  *(this + 44) = _ZtlSecureFuse<short>(a1: v4 + 73, a2: *(v4 + 77));              // short 状态
  CPet::SetShoeAttr(this);  CPet::UpdatePetAbility(a1: this);
  ```
  随后比较 `v3=*(this+42)` 与新值：下降走 `SP_381_THE_CLOSENESS_DECREASED_D`（`StringPool::GetString(... nIdx: SP_381...)` + `ZXString<char>::Format`），上升走另一条文案 → **亲密度增减有系统气泡**。

### §2.2 命令解析：28 字节/条的动作表 + 等级门槛 + 500ms 冷却

- **`CPet::OnActionCommand(CPet *this, int *a2)`**（3061B）：
  ```
  if ( *(this + 37) != 0 && *(result + 1384) != 0 ) return result;   // owner 忙则忽略
  Level = CPet::GetLevel(this);
  v5 = CInPacket::Decode1(this: a2);   // 命令类型 0/1/...
  if ( v5 == 0 ) {
      *(this + 45) = 0;
      result = 28 * CInPacket::Decode1(this: a2);       // ★ 每条动作记录 28 字节
      v6 = (result + *(*(this + 34) + 68));             // 表基址 = petTemplate+68
      if ( Level < *v6 || v6[1] < Level ) return result; // ★ 等级下限/上限校验
      if ( CInPacket::Decode1(this: a2) != 0 ) v7 = v6 + 5; else v7 = v6 + 6;  // success/fail 分支
      goto LABEL_26;
  }
  ```
  → 宠物指令表以 **28 字节为步长**，每条含 `[level_min, level_max, ..., 成功动作, 失败动作]`，由 `Decode1` 的命令索引 ×28 定位，且用 `CPet::GetLevel` 做等级门控。
- **`CPet::ParseCommand(int this, volatile LONG *a2)`**（4071B）：
  ```
  if ( v3 > -1 || *(this + 180) != 0 || *(this + 184) < 500
       || (*(**(this + 148) + 12))(a1: *(this + 148)) == 0 || *(*v5 + 1388) > 0 )
      { v34 = -1; if (a2!=0) ZXString<char>::_Release(lpAddend: a2-3); return 0; }
  ZXString<char>::TrimRight(a1: 0); v6 = ZXString<char>::TrimLeft(a1: 0);
  ... if ( ZXString<char>::operator==(a1: *(this + 136) + 4) == 0 ) ...   // 与宠物名前缀比对
  ```
  → **500（毫秒/ tick）冷却** + 空闲状态门槛 + 前后空白 Trim + 宠物名匹配，是「宠物学说话/喊名字触发指令」的解析链。
- **`CPet::ChatCommand`**（482B）/ **`CPet::CursedChatCommand`**（3325B）：正常指令与「诅咒/反噬」特殊指令分链。
- **`CPet::AutoSpeakingByEvent`**（1424B）：
  ```
  if ( (CPet::GetUpgradePetSkill(this) & 0x100) != 0 ) {          // ★ 技能位 0x100 = 事件自动说话
      CPetTemplate::GetAutoSpeakingByEvent(a1: &v18, a2);
      ... v11 = 5000; v12 = 1002; ... CAnimationDisplayer::LoadLayer(a1: this + 144);
      CChatBalloon::MakeBalloon(this: (this + 20), a2: v8, a3: v9, a4: v10,
                                a5: v11 /*5000*/, a6: v12 /*1002*/, a7: v13, int32: v14, a9: v15);
  }
  ```
  → 自动说话 = 技能掩码 `&0x100` 门控 + `CPetTemplate::GetAutoSpeakingByEvent` 取文案 + `CChatBalloon::MakeBalloon` 出 **5000ms / 样式 1002** 的气泡。

### §2.3 动作/图层/套装效果（大函数族）

- **`CPet::DoAction`**（3747B）：`if ( a3 >= 0 )` 先查 `CVecCtrl::IsOnLadder` / `IsOnRope`，命中则 `a3 = 8`（爬梯/绳动作）；`if ( v8 >= 9 )` → `*(this+248) = v8; CPet::PrepareActionLayer(this); v13 = *sub_70D6E8(a1: &v34, a2: v8 - 9);` → **动作号 ≥9 走扩展动作表**（`v8-9` 索引）。
- **`CPet::GetAdditionalLayer(CPet *this, int a2)`**（162B）：`a2==0 → this+192; a2==1 → this+216; else 0` → **恰 2 个附加图层槽**。
- **`CPet::SetPositionContext`**（93B）：`return sub_9C50C1(a1: a2);` 单行转发。
- **`CPet::OnAvatarModified`**（172B）：`if ( *(this+62) > -1 ) *(this+62) = -1; sub_703CF4(a1: this); return CPet::PrepareActionLayer(this);` → 主人外观变更后重建宠物动作层。
- **`CPet::SetShoeAttr`**（2333B）：`CPet::GetItemSlot` → 12 次 `TSecType<double>::GetData(a1: v17 + 12/24/36/48/...)` + `sub_500D61` → **鞋子/装备属性 12 组 double 安全类型**。
- **`CPet::UpdatePetAbility`**（4242B）：遍历 `*dword_BE7918 + 12912` 的装备槽表（`v4 = *(v32 + 8 * *v33 + 4)` → `CItemInfo::GetEquipItem`），槽位判定 `if ( *v33 == 24 || v8 == 25 || (v9 = *(this+39)) == 0 && (v8 >= 21 && v8 <= 29 || v8 == 46) || v9 == 1 && (v8 ...)` → **宠物装备槽 21–29 / 46 / 24 / 25** 决定捡物/自动喝药/扩范围等能力。
- **`CPet::LoadLayer`**(8082B) / **`PrepareActionLayer`**(4731B) / **`SetSetItemEffect`**(8075B) / **`SetSetItemBackground`**(11255B) / **`OnLoadExceptionList`**(3109B) / **`Init`**(5048B) / **`Update`**(5441B) / **`OnResolveMoveAction`**(1390B)：图层加载与套装效果大函数，引用 `SP_4818_EFFECT_SETEFFIMG_D_PETEFFECT`、`SP_4819_EFFECT_SETEFFIMG_D_PETBACKGRND_D`（★ 宠物套装特效/底板节点）。

### §2.4 移动

- **`CPet::OnMove @ 0x70474d`**（156B，本面补全）：
  ```
  void __thiscall CPet::OnMove(CPet *this, struct CInPacket *a2)
  {
    CMovePath::OnMovePacket(this: (*(this + 36) != 0 ? *(this + 36) - 12 + 428 : 428), a2);
  }
  ```
  refs：`?OnMovePacket@CMovePath@@QAEXAAVCInPacket@@H@Z @ 0x68b371` → 宠物移动包转交 `CMovePath`，目标对象为 owner 的 `+428`（或 owner-12+428）。
- **`CVecCtrlPet`**（6）：`WorkUpdateActive`(14018B)、`CollisionDetectWalk`(2509B)、`EndUpdateActive`、`SetActive`、`Init`、`CreateInstance` → **宠物走地物理**：主动跟走 + 碰撞检测。

## §3 CSummoned（16 + CSummonedBase 1 + CVecCtrlSummoned 6）

- **`CSummoned::Init(CSummoned *this, CInPacket *a2, CInPacket *a3)`**（960B，★ spawn 包字节布局）：
  ```
  v5  = CInPacket::Decode2(this: a3);      // 2B
  v9  = CInPacket::Decode2(this: a3);      // 2B
  v10 = CInPacket::Decode1(this: a3);      // 1B
  LOWORD(v6) = CInPacket::Decode2(this: a3); v14 = v6;   // 2B → Foothold SN
  v11 = CInPacket::Decode1(this: a3);      // 1B
  v12 = CInPacket::Decode1(this: a3);      // 1B
  Skill = CSkillInfo::GetSkill(this: dword_BE78DC, a2: *(this + 45));
  CWvsPhysicalSpace2D::GetFoothold(dwSN: v14);
  if ( Skill == 0 ) return 0;
  v8 = CInPacket::Decode1(this: a3);       // 1B
  return CSummoned::Init(this, a2, a3: v5, a4: v9, a5: v10, Foothold: v14, a7: v11, a8: v12, a9: v8);
  ```
  → 召唤体入场包 = `Decode2/2/1/2/1/1` + 技能校验 + foothold + 尾部 `Decode1`；技能 ID 来自 `*(this+45)`。
- **`CSummoned::ProcessAction`**（2801B）：`CurrentAction = CSummoned::GetCurrentAction(this, a2: 0); *(this + 63) -= 30;` → **30ms 动作 tick**；`v4 = *(this + 61) + 20 * CurrentAction` → **每动作记录 20 字节**的动作表。
- **攻击 AI 大函数族**：`TryDoingAttackManual`(22699B)、`ProcessAttack`(19570B)、`TryDoingAttack`(5973B)、`TryDoingHeal`(2613B)、`SetDamaged`(4700B, 9 处协议点)、`ShowEffect`(1314B)、`MakeIncDecHPEffect`(715B)、`PrepareActionLayer`(4161B)、`GetBodyRect/GetHeight/GetCurrentAction/OnResolveMoveAction/SetMoveAction/Update`；`CSummonedBase::LoadAttackInfo`(18194B) 加载攻击信息表，引 `SP_1637_MOBCOUNT`。
- **`SP_2870_EFFECT_BASICEFFIMG_SUMMONED`** ← CDragon, CSummoned 共用召唤特效节点。
- **`CVecCtrlSummoned`**（6）：`WorkUpdateActive`(8587B)、`CtrlUpdateActiveFly`(4801B)、`CollisionDetectFloat`(2375B)、`Init/SetActive/CreateInstance` → **召唤体浮空物理**（与宠物「走地」CVecCtrlPet 的 `CollisionDetectWalk` 明确区分）。

## §4 CDragon（10 + CVecCtrlDragon 2）

- **`CDragon::SetMoveAction(CDragon *this, int a2, int a3)`**（616B）：
  ```
  if ( a3 != 0 || (result = _ZtlSecureFuse<long>(a1: this + 140, a2: *(this + 37)), a2 != result) ) {
      v5 = _ZtlSecureTear<long>(a1: a2, a2: this + 140);
      *(this + 37) = v5;
      if ( _ZtlSecureFuse<long>(a1: this + 152, a2: v6) <= -1 ) CDragon::PrepareActionLayer(this);
      result = _ZtlSecureFuse<long>(a1: this + 140, a2: *(this + 37));
      *(*(this + 55) != 0 ? *(this + 55) - 12 + 0x148 : 328) = result;   // ★ 动作同步回 owner+0x148
  }
  ```
  → 龙动作经 `_ZtlSecureTear/Fuse` 安全存取，并写回主人对象 `+0x148` 字段。
- **`CDragon::UpdateQuestInfo`**(6514B) / `CreateEffect`(3979B) / `PrepareActionLayer`(4169B) / `ProcessAction`(3517B) / `Update`(4727B) / `NeedToTalk`(1920B) / `GetCurrentAction` / `SetFrameInfo` / `SetOneTimeAction` → 龙与**任务状态联动**（`CQuestMan` + `CharacterData`），`SP_5508_EFFECT_BASICEFFIMG_DRAGONEYES` 龙眼特效节点。
- **`CVecCtrlDragon`**（2）：`WorkUpdateActive`(7311B) / `WorkUpdatePassive`(1212B) → 龙跟随物理含主动/被动两态。
- **083 已含 Evan 龙骑**：`SP_5454_THIS_ITEM_CAN_ONLY_BE_EQUIPPED_ON_EVAN`（← CDraggableItem），与 `CDragon` 类树同批存在，非 095 新增。

## §5 CUIPetEquip（2，★ 三宠物页签）+ CPetTemplate（2）

- **`CUIPetEquip::OnButtonClicked(CUIPetEquip *this, unsigned int a2)`**（285B，逐字）：
  ```
  if ( a2 == 2000 ) {
      CUIEquip::TogglePetEquip(this: dword_BED64C);
  }
  else if ( a2 > 0x7D0 && a2 <= 0x7D3 ) {       // 0x7D0=2000, 0x7D3=2003
      *(this + 360) = a2 - 2001;                 // 选中宠物索引 0/1/2
      CWnd::InvalidateRect(this, a2: 0);
  }
  ```
  → **ctrl id 2000 = 关闭/切换（由 `CUIEquip` 全局单例 `dword_BED64C` Toggle），2001/2002/2003 = 三只宠物页签**；`this+360` 存当前选中槽。宠物穿戴 UI **不是独立窗口**，而是装备窗 `CUIEquip` 的子面板（Toggle 拉起），严谨对应当前端 `IO/UITypes/UIEquipInventory.cpp:622 BtPet1/2/3` 三页。
- **`CUIPetEquip::OnMouseMove(char **this, int a2, int a3)`**（3974B）：`v33 = *(CWvsContext::GetCharacterData(...) + 4)` → `CharacterData::GetItem(this: v33, a2: v29, a3: 1, a4: -v5)` 与 `a3: 1, a4: -100 - v6` → **宠物装备格 = 负槽位（装备栏 a3=1，槽 -100-...）**，悬停取物品出 Tooltip。
- **`CPetTemplate::GetPetTemplate(unsigned int a1)`**（323B）：`sub_70DC0C(&a1, a2: v3)` 按模板 ID 查表。
- **`CPetTemplate::GetAutoSpeakingByEvent`**（448B）：`v3 = *(this+21); if ( v3 != 0 && *(v3-4) > a3 ) ... ZXString<char>::operator=(a1: v5 + 4 * a3); else ... sStrDefault` → 事件索引 → 文案（数组，越界回默认串），供 `CPet::AutoSpeakingByEvent` 调用。

## §6 CMob / 动作帧 / 池族（Angel 补语义）

- **`CMob`**(51) / **`CMobPool`**(13) / **`CMobTemplate`**(2) / **`MobStat`**(6) / **`MOBSKILLLEVELDATA`**(1) / **`MOBINFO`**(1) / **`CVecCtrlMob`**(11)：怪物生成/移动/受击/控控应答全链。关键协议点：`MobStat::DecodeTemporary` **99 处**（临时状态字节巨表）、`CMob::GenerateMovePath` 11、`CMob::Init/Update` 各 9、`CMob::OnCtrlAck` 5、`CMob::OnMove/OnDamaged` 各 4、`CMobPool::OnMobEnterField/OnMobChangeController` 各 3、`CMob::ProcessStatSet` 3。`SP_1590_EFFECT_SUMMONIMG_D` ← CMob、`SP_954_MOB_07DIMG` ← CMobTemplate、`SP_3306_SOUND_CASHEFFECTIMG_08D_HIT` ← CMob。
- **动作帧三件套**：`CActionMan`(17，`Init` / `LoadFaceLook` / `MergeCharacterSprite` / `GetWeaponAfterImage` 等)、`CActionFrame`(9：`LoadMappers` / `ExtractMap` / `MergeGroup` / `UpdateMBR` / `UpdateVisiblity` / `Draw` / `Merge` / `GetZMapper`)、`CAnimationDisplayer`(19，`LoadLayer`)、`CAvatar`(28) / `CAvatarMegaphone`(8) / `CUIAvatarVAC`(13) → 宠物/召唤体/龙/怪的最终渲染都落在这条 `CActionMan → CActionFrame → CAnimationDisplayer::LoadLayer` 链（与 `CPet::AutoSpeakingByEvent` 里 `CAnimationDisplayer::LoadLayer(a1: this+144)` 互证）。
- **池/掉落/场标**：`CDropPool`(7，`OnDropEnterField` 13 协议点 / `OnDropLeaveField` 5)、`CReactorPool`(4)、`CAffectedAreaPool`(4，`OnAffectedAreaCreated` 7)、`CTownPortalPool`(3)、`CPortalList`(3)、`CField`(65)、`CStage`(5，`OnSetField` 18 协议点)、`CUserPool`(21) → 场地对象入场/离场/状态广播协议完整。
- **气泡类型**：`CChatBalloon` 引 `SP_5643_PET` / `SP_1593_MOB` / `SP_3452_ITEM_CASH_0503IMG` → 083 气泡枚举里宠物/怪/现金物品气泡是独立类型常量。

## §7 SP_ 全集 185（关键子集）

| SP_ 节点 | 归属类 | 语义 |
|---|---|---|
| `SP_4818_EFFECT_SETEFFIMG_D_PETEFFECT` | CPet | 宠物套装特效 |
| `SP_4819_EFFECT_SETEFFIMG_D_PETBACKGRND_D` | CPet | 宠物套装底板 |
| `SP_381_THE_CLOSENESS_DECREASED_D` | CPet | 亲密度下降系统气泡 |
| `SP_2870_EFFECT_BASICEFFIMG_SUMMONED` | CDragon,CSummoned | 召唤特效 |
| `SP_5508_EFFECT_BASICEFFIMG_DRAGONEYES` | CDragon | 龙眼特效 |
| `SP_1590_EFFECT_SUMMONIMG_D` | CMob | 召唤怪图 |
| `SP_1637_MOBCOUNT` | CSummonedBase | 攻击信息表计数 |
| `SP_954_MOB_07DIMG` | CMobTemplate | 怪模板图 |
| `SP_5643_PET` / `SP_1593_MOB` | CChatBalloon | 气泡类型枚举 |
| `SP_3818_YOU_CAN_RIDE_THE_TAMED_CASH_MONSTER_WHEN_IT_IS_EQUIPPED_WITH_A_CASH_SADDLE` | CDraggableItem | 骑宠·现金鞍具 |
| `SP_873_YOU_MUST_HAVE_A_TAMED_MONSTER_R_NIN_ORDER_TO_EQUIP_THIS` | CDraggableItem | 骑宠·需驯服怪 |
| `SP_874_YOU_MUST_BE_EQUIPPED_WITH_A_SADDLE_R_NTO_EQUIP_THIS` | CDraggableItem | 骑宠·需装鞍 |
| `SP_386_PLEASE_SELECT_THE_PET_YOU_WOULD_LIKE_TO_PUT_THE_PET_EQUIPMENT_ON` | CDraggableItem | 宠物穿戴·选宠物 |
| `SP_877_THIS_ITEM_CANNOT_BE_EQUIPPED_WITH_YOUR_CURRENT_PET` | CDraggableItem | 宠物穿戴·不兼容 |
| `SP_879_YOU_DO_NOT_CURRENTLY_HAVE_A_PET_THAT_IS_ALIVE` | CDraggableItem | 宠物穿戴·无存活 |
| `SP_880_THE_PET_IS_UNABLE_TO_WEAR_THE_EQUIPMENT` | CDraggableItem | 宠物穿戴·不可穿 |
| `SP_4378_TO_HATCH_PIGMY_EGG_YOU_NEED_TO_R_NPURCHASE_THE_INCUBATOR_FROM_THE_CASH_SHOP` | CDraggableItem | 迷你龙蛋孵化 |
| `SP_5454_THIS_ITEM_CAN_ONLY_BE_EQUIPPED_ON_EVAN` | CDraggableItem | Evan 龙骑专属 |
| `SP_4266_YOU_MAY_ONLY_EQUIP_ONE_WEDDING_RING` | CDraggableItem | 婚戒（同族校验） |

`grep -c '^- `SP_' angel-petsummon-facts.md` = 185，`UI_UIWINDOWIMG` 仅 6 → **宠物/召唤/龙面的资源引用主体是 `Effect/Item/Sound/String` 而非 `UIWindow.img`**（与社交面 84 个 `UI_UIWINDOWIMG` 形成结构差异：本面几乎不占 UI 窗口件表，主要落在动作帧/特效/文案）。

## §8 协议/Decode 点（60 函数，前 30）

`MobStat::DecodeTemporary`(99) · `CSummoned::TryDoingAttackManual`(23) · `CStage::OnSetField`(18) · `CDropPool::OnDropEnterField`(13) · `CMob::GenerateMovePath`(11) · `AvatarLook::Decode`(10) · `CSummoned::SetDamaged`(9) · `CMob::Init`(9) · `CMob::Update`(9) · `CPet::Init`(7) · `CSummoned::Init`(7) · `CReactorPool::OnReactorChangeState`(7) · `CReactorPool::FindHitReactor`(7) · `CAffectedAreaPool::OnAffectedAreaCreated`(7) · `CSummoned::TryDoingAttack`(6) · `CReactorPool::OnReactorEnterField`(6) · `CTownPortalPool::OnTownPortalCreated`(6) · `CPet::OnActionCommand`(5) · `CSummoned::TryDoingHeal`(5) · `CMob::OnCtrlAck`(5) · `CDropPool::OnDropLeaveField`(5) · `CPet::ParseCommand`(4) · `CPet::DoAction`(4) · `CMob::OnMove`(4) · `CMob::OnDamaged`(4) · `CUserPool::OnUserRemotePacket`(4) · `CPet::OnAction`(3) · `CMob::ProcessStatSet`(3) · `CMobPool::OnMobEnterField`(3) · `CMobPool::OnMobChangeController`(3)。

→ 本面的「收包解码」集中在**场地对象入场/状态/移动**（`C*Pool::On*EnterField` 族），而「出站发包」构造点在本面 392 函数里稀薄（宠物/召唤/龙实体类主要被服务端驱动）→ 出站协议需以当前端 `Net/OutPacket.h` + `Net/Packets/PetPackets.h` 为准（r02/r04）。

## §9 五面一致性表

| 功能面 | Angel 函数 | BeiDou 同 EA | 结论 |
|---|---|---|---|
| 登录 | 327 | 328 | 同二进制不同符号状态 |
| 状态栏 HUD | 547 | 548 | 同上 |
| 核心窗口 | 439 | 441 | 同上 |
| 社交经济 | 752 | 752 | 同上 |
| **宠物·召唤兽·龙** | **968** | **968** | **同上，0 缺失** |

## §10 限制与未知

- `CMovePath` 在 `angel-petsummon-ea.json` 有 10 个具名方法，但不在本面 `TARGETS/FOCUS` 53 类反编译精选集（392 函数）内 → `CMovePath::OnMovePacket` 只有从 `CPet::OnMove` 的引用（`0x68b371`）与逐字调用签名，未取到其自身伪代码；如需字节级移动路径布局，须单独 decompile `0x68b371`。
- `CSummoned::Init` 的重载 `CSummoned::Init(this, a2, a3: v5, a4: v9, a5: v10, Foothold: v14, a7: v11, a8: v12, a9: v8)` 目标体在 392 精选集内可能未单独收录（同名多态），字节语义以入口 `Decode` 序列为准。
- normalize 阶段 `trunc_fixed 4`（4 个大函数经 `/output/<uuid>.json` 补全）；`empty 1`（`CPet::OnMove`）已单独补全并重跑 facts。
- IMG 层（orange-wz）门禁阻塞 → `Effect/Item/Sound` 节点仅从 SP_ 字符串/引用侧观察，未做像素级视觉复核（`read_image` 对 PNG 返回 `model "hy4free" does not declare image input`）；替代链见 §1。
- 本文件全程只读取证：未改任何产品源码、未构建、未实机、未提交。
