# 技能（Skill）· Angel 083 ↔ 当前端 七维校准

> 第八面 · 2026-11-09 · 基准 = GMS v83 / 083（Angel.exe pid 29620 port 13339）
> 性质：校准 / 对照 / 决策点 / Plan 文档，**非实现提交**。全程只读取证，未改任何源码，未创建提交。

## §0 五条结论

- **0.1 同二进制、只补语义（第八面闭环）**：Angel 技能面 106 函数 / 22 类全部具名反编译成功，EA 交叉 105/106；唯一缺失 `0x8aa4e4 = CWndSkillGuide::OnKey` 经 `get_bytes` 双端 32 字节逐字相同（`f6 44 24 0b 80 75 16 83 7c 24 04 1b 75 0f … c2 08 00`）洗清 → **106/106 by bytes**。八面 EA 字节级一致（登录 327/328、HUD 547/548、core 439/441、social 752/752、pet 968/968、activity 844/844、cashshop 938/938、**skill 106/106**）。Angel 3122 修饰名 / 361 类 / 2612 方法 vs BeiDou 50 / 10 / 0，差异全在语义层。
- **0.2 三族并立**：Angel 侧「窗口族」`CUISkill/CUISkillEx/CUISkillChangeConfirm/CWndSkillGuide/CDraggableSkill/CUIToolTip` +「数据族」`CSkillInfo/SKILLENTRY/SkillInfo/MOBSKILLLEVELDATA` +「战斗族」`CUserLocal::DoActiveSkill_*` 12 子型 / `CalcDamage` / `CMob`；当前端对应 `UISkillBook`(单类) + `SkillBook/SkillData/SkillMacros` + `Skill/AttackAndSkillPackets`。技能升级 `COutPacket(90)` 与当前端 `SpendSpPacket(SPEND_SP=90)` **全链一致（含 body：time+skillid）**，是本面唯一一条端到端正面闭环。
- **0.3 opcode 数值重合 ≠ 语义重合（本面最关键纪律）**：Angel 083 侧 7 个 c2s `90/91/92/93/101/104/165`；当前端同数值分别指向 `SPEND_SP/USE_SKILL/CANCEL_BUFF/SKILL_EFFECT/USE_INNER_PORTAL` 与 s2c `FAMILY_SUMMON_REQUEST(104)/UPDATE_CHAR_BOX(165)`。`92/93/101` **同号异义**、`104/165` **c2s↔s2c 撞号** → 083 与 s9 是两套命名空间，禁止仅凭数值判等。
- **0.4 当前端「资源缺失降级」是增量而非缺陷**：`UISkillBook.cpp:1098-1152 send_spup` 在 `UIWindow.img/Skill/skillPoint` 本包 WZ NOT FOUND、BT_OKAY 从未创建时直接 `SpendSpPacket(id).dispatch()`；`set_macro` 收起须显式 `mex->deactivate()`。二者是适配残缺 WZ / 修复用户报障（「升级技能按键没反应」「展开后收缩不了」）的增量，Angel 原生走完整面板、无此降级 → **校准 ≠ 改回原生**。
- **0.5 遗留收敛**：98 条判定 `✅61 / ⚠️22 / ⏳15 / ❌0 / ⊘0`（`61+22+15=98`，已由脚本逐条重数核验）。全部 `⏳15` 收敛为可命名一类：IMG 门禁阻塞（F 样式 12 + E 排版 2 + G 交互 1，见 `notes/r03-img-blocked-fallback.md`）；⚠️ 集中在 C 协议维（083/s9 命名空间分歧，s9 优先，交用户裁决 §6）。改回 083 opcode 会断 s9 联机。

## §1 取证基线

### 1.1 IDA 端点表
| 端点 | pid | port | imagebase | strings | 用途 |
| --- | --- | --- | --- | --- | --- |
| Angel.exe | 29620 | 13339 | 0x400000 | 1281 | 修饰名 / 反编译语义（一手） |
| BeiDou.exe | 31332 | 13337 | 0x400000 | 3889 | 字符串 / EA 字节级交叉（一手） |

### 1.2 三联门禁
| 门禁 | 状态 | 说明 |
| --- | --- | --- |
| REA shipped-artifact | ✅ | 既有 r17 链（BeiDou V17.7 线，单列；反证 stripped 端符号层空白，见 r05 §2） |
| IDA Angel + BeiDou | ✅ | 106/106 反编译 + 字节级 106/106，5 件 evidence |
| orange-wz(IMG) | ⚠️ 阻塞 | 见 r03；F 样式维整体降级 `⏳` |

### 1.3 采集脚本 / evidence / notes
| 类别 | 文件 |
| --- | --- |
| 脚本 | `tools/ida_angel_skill_{ea,decompile,crosscheck,byteaudit,facts}.py` |
| evidence | `angel-skill-symbol-inventory.json`(28543B / 106 funcs / 22 cls)、`angel-skill-decompile.json`(510807B / 106 全 OK)、`angel-beidou-skill-ea-crosscheck.json`(19699B / 105/106)、`angel-beidou-skill-ea-bytelevel-audit.json`(2484B / 106/106 by bytes)、`angel-skill-facts.md`(65734B / 1514 行 / 7 opcode literals) |
| notes | `r01-angel-native-skill-facts.md`、`r03-img-blocked-fallback.md`、`r05-skill-four-state-audit.md` |

### 1.4 本面独有技术点
1. `get_bytes` 洗 1-EA 缺失：与第七面 `0x4b7404` 同构，措辞更正为 **106/106 一致**（非「105/106 阈值口径」）。
2. `code_of` 双形态归一：decompile MCP 返回「纯文本」或 `{"addr","code","refs"}` JSON 串，facts 脚本兼容解析内层 `code`，修复后 234 → 1514 行、7 个 opcode 全抽出。
3. 符号层完整性显著优于商城面：106 函数**全部具名、无 `sub_47xxxx` 遗留**（第七面 24 个）。
4. c2s↔s2c 数值撞号识别：`104/165` 在 083 是 c2s、在 s9 是 s2c，方向分列。

## §2 口径

### 2.1 七维 × 五判定
A 功能 14 / B 结构 14 / C 协议 14 / D 数据 14 / E 排版 16 / F 样式 12 / G 交互 14 = **98 条**。
判定：`✅ 一致` · `⚠️ 差异-s9优先` · `❌ 缺失` · `⊘ 不适用` · `⏳ 待证据(IMG/符号)`。

### 2.2 证据优先级
实时运行行为 > 抓包 > 当前服务资源 > 进程配置 > 持久化状态 > 生成产物 > 源码 > 注释与死代码。本面以 IDB 反编译 + 当前端源码逐字为一手；既有 r17 REA 属 BeiDou V17.7 线，单列不混入一手列。

### 2.3 083 vs s9 红线
- 基准唯一 = 083/v83，**禁引 095 线**（`docs/features/port-095/**`）。
- 协议值 **s9 优先**：`90 = SPEND_SP`、`91 = USE_SKILL`、`92 = CANCEL_BUFF`、`93 = SKILL_EFFECT`、`101 = USE_INNER_PORTAL`、`110 = SKILL_MACRO`、`183 = CHANGE_QUICKSLOT`；s2c `36 = UPDATE_SKILL`、`124 = SKILL_MACROS`、`190 = SKILL_EFFECT`、`191 = CANCEL_SKILL_EFFECT`。
- 校准只做对照与决策点，**不改源码、不改协议、不提交**。

### 2.4 冲突取值表
| 冲突 | Angel 083 | 当前端 s9 | 取值 | 依据 |
| --- | --- | --- | --- | --- |
| 技能升级 | `COutPacket(90)` time+skillid | `SPEND_SP(90)` time+skillid | ✅ 一致 | 双端全链重合 |
| 技能使用 | `COutPacket(91)` | `USE_SKILL(91)` | ✅ 一致 | 双端重合 |
| 取消/增益掩码 | `92` SendSkillCancelRequest | `CANCEL_BUFF(92)` | ⚠️ s9 | 同号异义 |
| 起手/特效 | `93` DoActiveSkill_Prepare | `SKILL_EFFECT(93)` | ⚠️ s9 | 同号异义 |
| 传送登记 | `101` TryRegisterTeleport | `USE_INNER_PORTAL(101)` | ⚠️ s9 | 同号异义 |
| 反挂机/家族 | `104` c2s DoAntiMacroSkill | `FAMILY_SUMMON_REQUEST(104)` s2c | ⚠️ 分列 | c2s↔s2c 撞号 |
| 组合栏请求 | `165` c2s CUISkillEx | `UPDATE_CHAR_BOX(165)` s2c | ⚠️ 分列 | c2s↔s2c 撞号 |
| 升级门禁 | `CanSendExclRequest(500)` | 无 | ⚠️ 记录遗留 | 不新增，见 §6 |

## §3 七维 98 条

> 每行格式：`# | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策`。

### A · 功能与窗口结构（14）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
| --- | --- | --- | --- | --- | --- |
| A1 | 技能主窗构造 | `CUISkill::OnCreate @ 0x8aaa88`(15602B) 单窗 | `UISkillBook : UIDragElement<PosSKILL>` `UISkillBook.cpp:142` | ✅ | 单窗形态一致 |
| A2 | 窗口属性 | `CUIWnd(a1,a2:3,a3:3,a4:155,a5:6,a6:1,…)` | `UISkillBook.h` TYPE=SKILLBOOK/FOCUSED=false/TOGGLED=true | ✅ | 拖拽窗一致 |
| A3 | 技能指引子窗 | `0CWndSkillGuide @ 0x8aa243` + `CUISkill::OpenCurSkillGuide @ 0x961063` | 当前端无独立 SkillGuide 窗 | ⚠️ | 记遗留，不新增（s9 优先） |
| A4 | ESC 关指引 | `CWndSkillGuide::OnKey @ 0x8aa4e4` `a2==27`→`CloseSkillGuide` | `UISkillBook.cpp:797 send_key` escape→关 sp/close | ✅ | 交互一致 |
| A5 | 扩展窗组合栏 | `CUISkillEx::ShiftMacroUIState @ 0x8bad5a`(9527B) | `UISkillMacroEx` `anchor_right_of(*this,-3)` | ⚠️ | 形态差异，保持当前端叠加 |
| A6 | 升级确认窗 | `CUISkillChangeConfirm::GetResult @ 0xa0f8c8` | 当前端 skillPoint 面板资源缺失 | ⚠️ | 保持降级路径（§0.4） |
| A7 | 技能拖拽 | `CDraggableSkill::OnDropped @ 0x4faa22`(8169B) | `SkillIcon::drop_on_bindings`→`Keyboard::Mapping(SKILL,id)` `UISkillBook.cpp:1-130` | ✅ | 一致 |
| A8 | 双击使用 | `CDraggableSkill::OnDoubleClicked @ 0x4fb001` | `UISkillBook` 双击 → use skill | ✅ | 一致 |
| A9 | 技能 tooltip | `CUIToolTip::SetToolTip_Skill @ 0x8f25d0`(12475B) | `SkillDisplayMeta` `UISkillBook.cpp:1-130` | ✅ | 语义族一致 |
| A10 | 宏系统 tooltip | `CUIToolTip::SetToolTip_MacroSysSkill @ 0x8f2276`(7388B) | 当前端宏提示简化 | ⚠️ | 记遗留 |
| A11 | 宏按键映射 | `CDraggableSkill::MapMacro @ 0x4fb112`→`CMacroSysMan::FlushToSvr()` | `send_icon`→`SkillMacros::assign_skill` `UISkillBook.cpp:1282` | ✅ | 一致 |
| A12 | 展示容量 | `CUISkill` 4 页签 × 网格 | `ROWS=12` / `CLASSIC_ROWS=4` `UISkillBook.h:121` | ✅ | 一致 |
| A13 | 页签分职业 | `get_skill_root_from_job @ 0x4a8c4f` | `BT_TAB0..4` + `joblevel_by_tab` `UISkillBook.cpp:1161` | ✅ | 一致 |
| A14 | 承载 state | `CUISkill` 挂 `CWvsContext` 全局 | `UISkillBook` 挂 UIStateGame（非独立 state） | ⚠️ | 区别于商城独立真窗，保持 |

### B · 结构与数据族（14）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
| --- | --- | --- | --- | --- | --- |
| B1 | 技能查询 | `CSkillInfo::GetSkill @ 0x75c755` `22xx/2001 特判 return 0` | `SkillData::get(int32_t id)` `SkillData.h` | ✅ | 一致（Evan 特判见 D8） |
| B2 | 等级查询 | `CSkillInfo::GetSkillLevel @ 0x7616f6` | `SkillBook::get_level` `SkillBook.h:1-53` | ✅ | 一致 |
| B3 | 根/master 可见 | `CSkillInfo::GetSkillRootVisible @ 0x75c7c5` + `CUISkill::GetSkillRootVisible` | `SkillData::get_masterlevel` / `collect_passives` | ✅ | 一致 |
| B4 | 消耗校验 | `CSkillInfo::CheckConsumeForActiveSkill @ 0x764256`(7443B) | `Skill::can_use(level,weapon,job,hp,mp,bullets)` `Skill.h:1-60` | ✅ | 一致 |
| B5 | 等级数据表 | `SKILLENTRY::GetLevelData @ 0x760f23`(7130B) | `SkillData::Stats` `SkillData.h:1-132` | ✅ | 一致 |
| B6 | 武器适配 | `SKILLENTRY::IsCorrectWeaponType @ 0x766157` | `SkillData::get_required_weapon` | ✅ | 一致 |
| B7 | 特效 UOL | `SKILLENTRY::GetEffectUOL/GetScreenEffectUOL` | `SkillUseEffect/SkillHitEffect` `Skill.h` | ✅ | 语义族一致 |
| B8 | 指定动作 | `SKILLENTRY::GetRandomAppointedAction/IsActionAppointed` | `SkillAction` `Skill.h` | ✅ | 一致 |
| B9 | 动作校验 | `SKILLENTRY::IsCorrectAppointedAction @ 0x…` | `Skill::apply_actions` | ✅ | 一致 |
| B10 | job→root 映射 | `get_skill_root_from_job` + `is_correct_job_for_skill_root @ 0x7669dd` | `joblevel_by_tab`(枚举到职阶) | ⚠️ | 映射粒度差异，保持当前端 |
| B11 | master level 判定 | `is_skill_need_master_level @ 0x4e8f04`(383B) | `SkillData::get_masterlevel` | ✅ | 一致 |
| B12 | 蓄力/持续技 | `is_keydown_skill @ 0x4fb08f`(823B) | `SkillEffectPacket` 注释「本端无 KeyDown 条状态机」`AttackAndSkillPackets.h:305` | ⚠️ | 记遗留（P2，不新增状态机） |
| B13 | 字段技能解析 | `SkillInfo::Parse @ 0x5880bf` / `SetFieldType @ 0x587f55` | `SkillData : Cache<SkillData>` `rooted` 标记 | ✅ | 一致 |
| B14 | 怪物技 | `MOBSKILLLEVELDATA::GetEffectUOL` / `CMob::TryDoingSkill @ 0x66e7af` / `UpdateAffectedSkillList` | 当前端 Mob skill 处理 | ✅ | 语义族一致 |

### C · 协议（14）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
| --- | --- | --- | --- | --- | --- |
| C1 | 升级发包 | `SendSkillUpRequest @ 0xa23cf3` `COutPacket(90)`+`Encode4(time)`+`Encode4(skillid)` | `SpendSpPacket(SPEND_SP=90)` `write_time+write_int` `PlayerPackets.h:90-99` | ✅ | **全链一致**（非冲突） |
| C2 | 升级互斥门禁 | `CanSendExclRequest(a1:500,a2:0)` | 当前端无 `500ms` 互斥 | ⚠️ | 记遗留，不新增（s9 优先） |
| C3 | 使用发包 | `DoActiveSkill_Heal/RecoveryAura/SmokeShell/TownPortal` `COutPacket(91)` | `UseSkillPacket(USE_SKILL=91)` `AttackAndSkillPackets.h:287` | ✅ | 一致 |
| C4 | 怪物磁石尾字段 | `SendSkillUseRequest` 变长编码 | `if(skillid%10000000==1004) skip(2)` `AttackAndSkillPackets.h:298` | ⚠️ | s9 优先，保留 TODO |
| C5 | 取消技 | `SendSkillCancelRequest` `COutPacket(92)` | `CANCEL_BUFF=92` `OutPacket.h:146` | ⚠️ | **同号异义**，s9 优先 |
| C6 | 起手/准备 | `DoActiveSkill_Prepare` `COutPacket(93)` | `SKILL_EFFECT=93` `OutPacket.h:184` | ⚠️ | **同号异义**，s9 优先 |
| C7 | 传送登记 | `TryRegisterTeleport` `COutPacket(101)` | `USE_INNER_PORTAL=101` `OutPacket.h:174` | ⚠️ | **同号异义**，s9 优先 |
| C8 | 反挂机 | `DoAntiMacroSkill` `COutPacket(104)` c2s | s2c `FAMILY_SUMMON_REQUEST=104` `PacketSwitch.cpp:273` | ⚠️ | **c2s↔s2c 撞号**，分列 |
| C9 | 组合栏请求 | `CUISkillEx::OnButtonClicked a2==2024` `COutPacket(165)` 无 body c2s | s2c `UPDATE_CHAR_BOX=165` `PacketSwitch.cpp:254` | ⚠️ | **c2s↔s2c 撞号**，分列 |
| C10 | 使用互斥门禁 | `SendSkillUseRequest` `CanSendExclRequest(300)` + `GetSkillLevel<=0 return` | `UseSkillPacket` 直发 | ⚠️ | 记遗留，不新增 |
| C11 | 技能记录回包 | `OnChangeSkillRecordResult @ 0xa1e48c` `Decode1/2/4` 循环 + `is_skill_need_master_level` | `UpdateSkillHandler(UPDATE_SKILL=36)` `PlayerHandlers.cpp:290` | ⚠️ | s2c 编号不同，s9 优先 |
| C12 | 宏同步 | `CMacroSysMan::FlushToSvr()` | `ChangeSkillMacroPacket(SKILL_MACRO=110)` `SkillMacroPackets.h` | ⚠️ | s9 优先 |
| C13 | 特效白名单 | `DoActiveSkill_Prepare case 3/4/5` | `SkillEffectPacket` 18 技能白名单 `AttackAndSkillPackets.h:309` | ✅ | 语义族一致（起手广播） |
| C14 | 分列纪律 | Angel c2s 7 opcode 全从 `COutPacket` 构造参数抽出 | `opcode-matrix.json` + `gen_opcode_matrix.py --check` | ✅ | 门禁 GATE OK |

### D · 数据与逻辑（14）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
| --- | --- | --- | --- | --- | --- |
| D1 | 数值字段族 | `SKILLENTRY::GetLevelData` 360+ 字节结构 | `SkillData::Stats`(damage/matk/mastery/attackcount/mpcost…) `SkillData.h` | ✅ | 一致 |
| D2 | 增删技能记录 | `Decode4(skillid)+Decode4(level)` level<0→RemoveKey | `SkillBook::set_skill(id,level,masterlevel,expiration)` | ✅ | 一致 |
| D3 | 被动技收集 | `is_antirepeat_buff_skill @ 0x96d6ca` | `SkillBook::collect_passives()` ordered map 防低级覆盖高级 | ✅ | 一致 |
| D4 | 前置技能 | `is_correct_job_for_skill_root` | `SkillBook::collect_required(id)` → `get_reqskills()` | ✅ | 一致 |
| D5 | 宏槽结构 | `CMacroSysMan` | `SkillMacroSlot{name,shout,skill1/2/3}` `MAX_SLOTS=5` `SkillMacros.h` | ✅ | 一致 |
| D6 | 宏包 body | `FlushToSvr` | `ChangeSkillMacroPacket` byte+string+byte+3×int `SkillMacroPackets.h` | ✅ | 一致 |
| D7 | 技能 id 枚举 | `get_skill_root_from_job` 数值族 | `SkillId::Id`(Beginner 8/12/1000.., Swordman 1000000.., F/PMage 2100000..) `SkillId.h` | ✅ | 一致 |
| D8 | Evan SP 表 | `CSkillInfo::GetSkill` `a2/10000/100==22 ‖ a2/10000==2001` | `hasSPTable(EVAN=2001,2200..2218)` `CashShopParser.h` | ✅ | 一致 |
| D9 | 使用条件 | `CheckConsumeForActiveSkill` + `ValidateItemSkill/ValidateSkillBonus` | `Skill::can_use(hp,mp,bullets,weapon,job)` | ✅ | 一致 |
| D10 | 伤害/蓄力字段 | `DoActiveSkill_CombatStep @ 0x96d95e` | `AttackPacket` has_charge_field 9 技 / sustained 4 技 `AttackAndSkillPackets.h` | ✅ | 语义族一致 |
| D11 | 新手 SP 特判 | `get_novice_skill_point @ 0x765e9e` / `is_heros_will_skill` | 当前端 SkillBook 未见新手 SP 特判 | ⚠️ | 记遗留（P2） |
| D12 | 宏准入白名单 | `is_able_to_map_macrosys_skill @ 0x4fb292` | `SkillMacros::assign_skill` 无准入判定 | ⚠️ | 记遗留，不新增 |
| D13 | 伤害计算 | `CalcDamage::PDamage/MDamage` | 当前端 Combat 伤害计算 | ✅ | 语义族一致 |
| D14 | 字段禁技 | `CField::IsSkillForbiden/RestoreForbiddenSkill` | 当前端 Field 禁技状态 | ✅ | 一致 |

### E · 排版（16）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
| --- | --- | --- | --- | --- | --- |
| E1 | 技能格偏移 | `CUISkill::OnCreate` 网格 | `SKILL_OFFSET=(11,93)` `UISkillBook.h` | ✅ | 一致 |
| E2 | 经典窗偏移 | v83 经典布局 | `CLASSIC_SKILL_OFFSET=(10,87)` | ✅ | 一致 |
| E3 | 行/格尺寸 | 网格常量 | `ROWS=12 / ROW_HEIGHT=40 / ROW_WIDTH=143` | ✅ | 一致 |
| E4 | 经典行/加号位 | 经典 4 行 | `CLASSIC_ROWS=4 / CLASSIC_SPUP_OFFSET=(130,20)` | ✅ | 一致 |
| E5 | 页签底板 | Tab 控件 | `TAB_PLATE_TOPLEFT=(4,23) / PITCH=34` | ✅ | 一致 |
| E6 | 页签中心 | Tab 槽 | `TAB_SLOT0_CENTER=21 / TAB_CENTER_Y=34` | ✅ | 一致 |
| E7 | 书名/文本位 | 文本 ink 常量 | `BOOK_TOPLEFT=(11,57) / CENTER_X=105 / INK_TOP=68` | ✅ | 一致 |
| E8 | 宏按钮位 | macro 面板 | `MACRO_BTN_TOPLEFT=(121,265)` | ✅ | 一致 |
| E9 | 经典滑条 | slider 常量 | `CLASSIC_SLIDER_X=158 / TOP=87 / BOTTOM=258` | ✅ | 一致 |
| E10 | 单元/meta 偏移 | 绘制偏移 | `SKILL_META_OFFSET=(2,2) / LINE_OFFSET=(0,37)` | ✅ | 一致 |
| E11 | 命中判定 | 逐格定位 | `skill_by_position` 逐行问 Icon 本格（修复旧 `(y-base)/ROW_HEIGHT` 除法 bug）`UISkillBook.cpp:1178` | ✅ | 一致 |
| E12 | 组合栏锚定 | 组合栏吸附 | `anchor_right_of(*this,-3)` gap=-3 重叠 3px 同高 289 `UISkillBook.cpp:515` | ✅ | 一致 |
| E13 | 双布局分支 | 原生单一经典布局 | `classic = !UiAssetResolve::usable(Skill["main"])` 双布局 `UISkillBook.cpp:142` | ⚠️ | 保持当前端环境适配 |
| E14 | SP 钮 x 位 | 动态定位 | `BT_SP*` x=`bg_dimensions.x()` `UISkillBook.cpp:142-250` | ⚠️ | 依赖 IMG 尺寸动态，保持 |
| E15 | 名称/等级 ink | 文本绘制位 | `name_text Point(38,-5)` / `level_text Point(38,13)` `UISkillBook.cpp:1-130` | ⏳ | IMG 阻塞（r03 F4/F5） |
| E16 | tab_plate 借用 | 经典底板来源 | `tab_plate = UiAssetResolve::window("Item")["New"]` `UISkillBook.cpp:142-250` | ⏳ | IMG 阻塞（r03 F8／r05 推翻 r15） |

### F · 样式（12）— 全维 IMG 阻塞，见 r03 §3

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
| --- | --- | --- | --- | --- | --- |
| F1 | 技能名字色 | `SP_1410_CANVASFONT / SP_8_TAHOMA` | `classic ? BLACK : EMPEROR` `UISkillBook.cpp:1-130` | ⏳ | P2 IMG 恢复后销案 |
| F2 | 图标三态 | `SKILLENTRY` icon 族 | `Icon{NORMAL,DISABLED,MOUSEOVER}` `SkillData.h` | ⏳ | r03 F2 |
| F3 | 名称截断 | 名称栏 | `MAX_NAME_WIDTH=97` | ⏳ | r03 F3 |
| F4 | 名称 ink 位 | 文本位 | `Point(38,-5)` | ⏳ | r03 F4 |
| F5 | 等级 ink 位 | 文本位 | `Point(38,13)` | ⏳ | r03 F5 |
| F6 | SP 三态贴图 | skillPoint 面板 | `sp_before/sp_after/used` Texture `UISkillBook.h` | ⏳ | r03 F6（本包 NOT FOUND） |
| F7 | 数字字符集 | Charset | `Charset sp_before/sp_after` | ⏳ | r03 F7 |
| F8 | 经典 tab 底板 | 底板贴图 | `Item/New` 借用 `UISkillBook.cpp:142-250` | ⏳ | r03 F8 |
| F9 | 关闭钮 | `BtClose2` | `UiAssetResolve::close_btn()` | ⏳ | r03 F9 |
| F10 | 宏 OK 钮 | `BtOK` | `Skill["macro"]["BtOK"]` | ⏳ | r03 F10 |
| F11 | 组合栏底板 | `SkillMacroEx/backgrnd` | `dimension += macro_backgrnd.x` `UISkillBook.cpp:1249` | ⏳ | r03 F11 |
| F12 | 组合栏收起钮 | `SP_4346_..._BTMACROOUT` `ShiftMacroUIState` | `mex->deactivate()` | ⏳ | r03 F12 |

### G · 交互（14）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
| --- | --- | --- | --- | --- | --- |
| G1 | 切页签 | `CUISkill` Tab | `BT_TAB0..4`→`change_tab` `UISkillBook.cpp:515` | ✅ | 一致 |
| G2 | 加号升级 | `CUISkill::OnSkillLevelUpButton @ 0x8acf1a` `a2-2010` | `BT_SPUP0..11`→`send_spup` | ✅ | 一致 |
| G3 | 确认升级 | `get_novice_skill_point(cd)` | `BT_OKAY`→`while(used>0) spend_sp` `UISkillBook.cpp:515` | ✅ | 一致 |
| G4 | SP 三态交互 | 面板态 | `BT_SPDOWN/SPMAX/SPUP` + 三态文本 `:515` | ✅ | 一致 |
| G5 | 展开组合栏 | `CUISkillEx::OnButtonClicked a2==2023`→`ShiftMacroUIState(0)` | `BT_MACRO`→`set_macro toggle`+`emplace UISkillMacroEx` | ✅ | 一致 |
| G6 | 提交宏 | `CMacroSysMan::FlushToSvr()` | `BT_MACRO_OK`→`ChangeSkillMacroPacket().dispatch()` | ✅ | 一致 |
| G7 | 关闭 | `CloseSkillGuide` / 窗关闭 | `BT_CLOSE`→`close_btn()` | ✅ | 一致 |
| G8 | 'S' 快捷键 | `SendSkillUpRequest` | `'S'`→`SpendSpPacket` 或 `UISoftKey` `:797` | ✅ | 一致 |
| G9 | 'U'/'F' 快捷键 | `SendSkillUseRequest` | `'U'/'F'`→`UseSkillPacket` 或 `UISoftKey` | ✅ | 一致 |
| G10 | 'T'/'P' 快捷键 | 变换/升级窗 | `'T'`→`UITransform` / `'P'`→`UISkillUp` | ✅ | 一致 |
| G11 | ESC/TAB | `CWndSkillGuide::OnKey a2==27` | `escape`→关 / `TAB`→翻页 `:797` | ✅ | 一致 |
| G12 | 资源缺失降级 | 原生走完整 skillPoint 面板 | `send_spup` `!find_btn(BT_OKAY)`→直接 `SpendSpPacket.dispatch()` `:1098-1152` | ✅ | **新增（增量）**，保留 |
| G13 | 组合栏收起 | `a2==2023 ShiftMacroUIState(0)` | `set_macro` 须显式 `mex->deactivate()` `:1249` | ⚠️ | 当前端显式修复，保持 |
| G14 | 未接管按钮 | `CUISkillEx` 全量按钮 | `BT_HYPER/BT_GUILDSKILL/BT_RIDE` 空且初始 DISABLED `:515` | ⏳ | IMG/符号待补（P2） |

### 判定分布自检

| 维 | 条数 | ✅ | ⚠️ | ⏳ | ❌ | ⊘ |
| --- | --- | --- | --- | --- | --- | --- |
| A 功能 | 14 | 9 | 5 | 0 | 0 | 0 |
| B 结构 | 14 | 12 | 2 | 0 | 0 | 0 |
| C 协议 | 14 | 4 | 10 | 0 | 0 | 0 |
| D 数据 | 14 | 12 | 2 | 0 | 0 | 0 |
| E 排版 | 16 | 12 | 2 | 2 | 0 | 0 |
| F 样式 | 12 | 0 | 0 | 12 | 0 | 0 |
| G 交互 | 14 | 12 | 1 | 1 | 0 | 0 |
| **合计** | **98** | **61** | **22** | **15** | **0** | **0** |

`61+22+15 = 98` ✓；⏳15 = F12 + E2 + G1，与 r03 §1 / r05 §3 声明逐项吻合。

## §4 十条硬发现

1. **106/106 by bytes**：`0x8aa4e4` 32 字节双端逐字相同，八面同二进制结论闭环（区别于第七面 938/938 的 24 个 `sub_47xxxx` 遗留）。
2. **技能升级全链一致**：`COutPacket(90)+Encode4(time)+Encode4(skillid)` ↔ `SpendSpPacket(SPEND_SP=90)+write_time+write_int`，是八面中少见的端到端正面闭环。
3. **7 个 c2s opcode 一手抽出**：`90/91/92/93/101/104/165`，此前文档从未记录（r05 判「新增」）。
4. **数值重合 ≠ 语义重合**：`92/93/101` 同号异义、`104/165` c2s↔s2c 撞号，083 与 s9 两套命名空间。
5. **资源缺失降级是增量**：`UIWindow.img/Skill/skillPoint` 本包 NOT FOUND → `send_spup` 直接发包，修复用户报「升级技能按键没反应」。
6. **组合栏收起显式 `deactivate()`**：修复用户报「展开后收缩不了」，Angel 侧无对应降级。
7. **UISoftKey 闪退闭环**：`PlayerHandlers.cpp:290-306` 注释实证 `0xC0000005`（RVA `0x1EC372`）→ 改 `UPDATE_SKILLS` 静默更新（r05 销案）。
8. **r15 tab_plate 误判被推翻**：正确为 `Item/New` 借用，非 `recommend/0` 桃色横幅。
9. **既有 r17 REA 是独立 BeiDou V17.7 线**：`SkillEx/SkillMacro/SkillUp` 三搜索 n:0 全空 → 反证 Angel 只补语义层，不得冒充 13339 一手。
10. **符号层完整性优于商城面**：106 函数全具名、零 `sub_47xxxx`，`code_of` 双形态归一后 facts 234→1514 行。

## §5 四态

| 态 | 内容 |
| --- | --- |
| **销案** | 升级链 / UISoftKey 闪退 / 宏 / 特效 / 交叉功能面既有笔记被 083 一手语义确认（r05 §1 归组表） |
| **推翻** | 1 项：r15 `tab_plate` 借 `Item/New` 误判（r05 §1「布局/贴图」行） |
| **新增** | 技能升级资源缺失降级、`set_macro deactivate`、083 侧 7 opcode + 分列纪律、`CDraggableSkill::MapMacro` 一手链（r05 §1/§3） |
| **遗留** | F 样式 12 + E 排版 2 + G 交互 1（IMG 阻塞，r03）；C 协议 ⚠️ 待 D1–D14 裁决（§6） |

> **无「改回原生」态**：所有 ⚠️ 均按 s9 优先记录，禁止盲改协议。

## §6 决策点 D1–D14（交用户裁决，裁决后方动源码）

| # | 决策点 | 推荐默认 | 依据 |
| --- | --- | --- | --- |
| D1 | `92/93/101` 同号异义是否对齐 083 | 保持 s9 | 改回原生断联机（§2.3） |
| D2 | `104/165` c2s↔s2c 撞号是否新增 083 发包 | 不新增，分列记录 | 方向不同，s9 优先 |
| D3 | 是否补 `CanSendExclRequest(500/300)` 互斥门禁 | 不新增 | 行为差异无联机影响（C2/C10） |
| D4 | 是否新增独立 `CWndSkillGuide` 子窗 | 不新增 | 当前端 ESC 已闭环（A3） |
| D5 | `CUISkillChangeConfirm` 确认窗是否补齐 | 不补齐，保留降级 | 资源缺失，G12 已保底（A6） |
| D6 | `is_keydown_skill` / KeyDown 状态机是否补 | 不新增（P2 复评） | `SkillEffectPacket` 注释明示本端无状态机（B12） |
| D7 | `get_novice_skill_point` 新手 SP 特判是否补 | 记录遗留 | 影响面待 IMG/实机复评（D11） |
| D8 | `is_able_to_map_macrosys_skill` 宏准入白名单是否补 | 不新增 | 当前端 assign 无准入（D12） |
| D9 | 双布局分支 `classic` 是否收敛为原生单布局 | 保持双布局 | 环境适配增量（E13） |
| D10 | `BT_SP*` 动态 x=`bg_dimensions.x()` 是否改静态 | 保持动态 | 依赖 IMG 尺寸（E14） |
| D11 | `BT_HYPER/GUILDSKILL/RIDE` 三空按钮是否接管 | 保持 DISABLED | 无 083 语义落点（G14） |
| D12 | 组合栏 `deactivate()` 与 `ShiftMacroUIState` 形态差异 | 保持当前端显式修复 | 修用户报障（G13） |
| D13 | 技能记录回包 `Decode1/2/4` vs `UPDATE_SKILL(36)` | 保持 s9 | s2c 编号不同（C11） |
| D14 | F 样式 12 项 IMG 实机复核 | 挂起 P2 | orange-wz 阻塞（r03 §4） |

> **P1 触发条件**：以上任一点被用户裁决偏离「推荐默认」→ 进入一次性 Plan 后实施；未裁决前保持只读、不改源码。

## §7 一次性 Plan

| 阶段 | 内容 | 状态 |
| --- | --- | --- |
| **P0** | 纯文档：r01/r03/r05/README 归档校验 + 门禁 `gen_opcode_matrix.py --check` | ✅ 本轮闭环 |
| **P1** | 需裁决：D1–D13 任一偏离默认后的最小改动 + 回归 | ⏸ 待用户裁决 |
| **P2** | 需 IMG/实机：orange-wz 恢复 → F 12 项销案 + `build_smoke.bat` + D6/D7 复评 | ⏸ 挂起（r03 §4） |
| **P3** | 续作功能面：NPC / 地图 / 任务 / 掉落，沿用同一「REA→IDA→IMG 只读取证 + 决策点 + 一次性 Plan」方法 | ⏸ 挂起 |

## §8 限制与未知

- orange-wz(IMG) 阻塞 → F 样式维整体 `⏳`，E/G 各 2/1 项受影响；逐像素 oracle 未复核。
- 083 与 s9 命名空间不同，本协议对照仅作语义层标注，不作联机行为等价断言。
- 既有 r17 REA 属 BeiDou V17.7 线，单列不混用（r05 §2）。
- 106 函数无未命名分支，但 `get_bytes` 仅抽 `0x8aa4e4` 一处做 1-EA 归因（阈值判据 `present>=len-2` 已由字节级证据替代）。
- 095 线禁引；全程未改游戏源码 / 协议，未建提交。

## §9 证据索引与复现

### 9.1 文件清单

| 类别 | 文件 |
| --- | --- |
| 一手 evidence | `evidence/angel-skill-symbol-inventory.json`、`angel-skill-decompile.json`、`angel-beidou-skill-ea-crosscheck.json`、`angel-beidou-skill-ea-bytelevel-audit.json`、`angel-skill-facts.md` |
| 一手事实 | `notes/r01-angel-native-skill-facts.md` |
| 门禁/降级 | `notes/r03-img-blocked-fallback.md` |
| 四态审计 | `notes/r05-skill-four-state-audit.md` |
| 当前端源码 | `IO/UITypes/UISkillBook.h`、`.cpp`、`IO/UIStateCashShop.h`(对照)、`Character/SkillBook.h/.cpp`、`SkillMacros.h`、`SkillId.h`、`Gameplay/Combat/Skill.h`、`Data/SkillData.h`、`Net/Packets/AttackAndSkillPackets.h`、`PlayerPackets.h`、`SkillMacroPackets.h`、`Net/OutPacket.h`、`Net/PacketSwitch.cpp`、`Net/Handlers/PlayerHandlers.cpp`、`WaveAShellHandlers.cpp` |

### 9.2 复现命令

```
python -m py_compile tools/ida_angel_skill_{ea,decompile,crosscheck,byteaudit,facts}.py
python tools/ida_angel_skill_ea.py           # → 106 funcs / 22 cls
python tools/ida_angel_skill_decompile.py   # → 106/106 ok
python tools/ida_angel_skill_crosscheck.py  # → 105/106 EAs
python tools/ida_angel_skill_byteaudit.py   # → VERDICT 106/106 by bytes
python tools/ida_angel_skill_facts.py       # → 1514 行 / 7 opcode literals
python tools/gen_opcode_matrix.py --check   # → GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)
```

## §10 性质标注

本文件是**校准 / 对照 / 决策点 / Plan 文档**，不是实现提交。全程只读取证，未改任何游戏源码、未改协议、未创建提交。P0 纯文档阶段闭环；P1 需用户裁决 D1–D14 后方动；P2/P3 挂起。
