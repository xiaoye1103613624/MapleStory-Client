# r05 · 第八面技能既有文档四态审计

> 2026-11-09 · 审计对象 = 本轮之前已落盘的技能相关笔记 / REA 证据 / 布局素材。
> 四态定义沿用活动面与商城面：**销案**（既有结论被一手证据确认并闭环）/ **推翻**（既有结论被一手证据否定）/ **新增**（Angel 083 一手新发现，既有文档未覆盖）/ **遗留**（待 IMG / 符号 / 用户裁决方动）。
> 性质：审计记录，**非实现提交**。

## §1 归组表

| 组 | 既有文件（`docs/features/beidou-v17-parity/notes/`） | 四态 | 依据 |
| --- | --- | --- | --- |
| 技能升级 / 加点 | `r122-admin-quiz-skillup-search.md`、`r446-skillup-bufflist-sm.md`、`r585-skillup-mount-cpq.md`、`r612-skillbook-learn-skillup.md`、`r703-skillup-softkey-length-carnival.md`、`r780-skillex-skillup-itemmove.md`、`r736-quest-skilluse-invgrow-pccafe.md` | **销案** | Angel `CWvsContext::SendSkillUpRequest @ 0xa23cf3` → `COutPacket(90)` + time + skillid，与当前端 `SpendSpPacket` 全链一致；既有笔记的升级链路无被 083 否定的地方 |
| UISoftKey 闪退链 | `r159-skill-stats-inv-softkey.md`、`r703-...softkey...md`、`r752-skillbook-hover-softkey.md`、`r251-skillbook-useskill-family.md` | **销案** | `PlayerHandlers.cpp:290-306` 注释实证：旧实现弹 UISkillUp 调试浮层 → `UISoftKey::draw` 空 `BtCaseKeys` 解引用 `0xC0000005`（RVA `0x1EC372`，2026-10-29 `softkey_trace.log` 调用栈）；现已改为 `UPDATE_SKILLS` 静默更新，闪退已闭环 |
| 技能宏 / 组合栏 | `r28-family-add-energy-skillmacro.md`、`r30-skillmacro-precept.md`、`r57-skillex-macro.md`、`r165-macro-pet-skill-softkey.md`、`r472-skillmacro-no-force-open.md`、`r506-skillmacros-silent.md`、`r780-skillex-skillup-itemmove.md` | **销案 + 新增** | 既有「不强制开窗」结论 ✅；Angel `CDraggableSkill::MapMacro @ 0x4fb112` → `is_able_to_map_macrosys_skill` → `CSkillInfo::GetSkill` → `SKILLENTRY::GetLevelData` → `CMacroSysMan::FlushToSvr()` 为**新增**一手链条；当前端 `set_macro` 收起须显式 `mex->deactivate()`（`UISkillBook.cpp:1249`，用户报「展开后收缩不了」的修复）亦为**新增**（Angel 侧无对应降级） |
| 技能升级面板资源缺失 | `UISkillBook.cpp:1098-1152 send_spup` 自身注释（+ `r612`/`r703` 相关背景） | **新增** | `UIWindow.img/Skill/skillPoint` 本包 WZ **NOT FOUND**（`nx_inspect` 实测）→ BT_OKAY 等钮从未创建 → `set_skillpoint(true)` 恒 false → 点 + 无反应（用户报「升级技能按键没反应」）；当前端直接 `SpendSpPacket(id).dispatch()` 保底。Angel `CUISkill::OnSkillLevelUpButton` 走完整面板、**无此降级** → 当前端为适配残缺 WZ 的增量，非「不如原生」 |
| 技能特效 / 施放 | `r330-skill-effect-tip.md`、`r336-owl-mts-skillfx.md`、`r342-fieldmarker-skill-anim.md`、`r358-foreignbuff-skill-try.md`、`r365-openui-skilleffect.md`、`r636-conti-skill-cs-rem-drain.md`、`r796-gamemenu-skill-effect-shop-charinfo.md` | **销案** | Angel `CUser::ShowSkillEffect/ShowSkillPrepare/ShowSkillSpecialEffect` + `CAnimationDisplayer::Effect_SkillUse` 语义族与当前端 `SkillEffectPacket`(93) / `MapObjectHandlers.h:168 s9 SKILL_EFFECT(190)` 方向一致；既有笔记无冲突结论 |
| 技能书 / 家族 / 宠物技 | `r117-buddy-group-useskill-book.md`、`r118-reset-skillex-deny-party.md`、`r301-chalkboard-facial-chair-skill.md`、`r306-invgrow-skillbook-report-session.md`、`r318-familytree-skilluse-namecancel.md`、`r377-lucksack-mobskill.md`、`r387-potion-skilluse.md`、`r496-skill-relog-chair-dedupe.md`、`r569-enter-map-skillbook.md`、`r627-skilluse-guild-family-return.md`、`r632-invgrow-skilllearn-storage-duey.md`、`r677-pet-skill-luck-shop-markers.md`、`r728-clock-shop-buddy-skilllearn.md`、`r785-itemsearch-aran-skillbook.md`、`r791-shortcut-skill-map-useitem.md` | **销案** | 交叉功能面既有记录，Angel 侧 `CPet::GetUpgradePetSkill` / `CMob::TryDoingSkill` / `CUserLocal::FindParty` 提供同族语义支撑，无推翻项 |
| 布局 / 贴图 | `r05-ui-layout.md §3`、`r51-enchantskill-drag.md`、`ui_layout_dump/skill_backgrnd.png`、`skill_backgrnd_grid.png`、`_skill_nodes.txt`、`_skillbands.py`、`_skillbands.txt` | **推翻 + 遗留** | **推翻**：`UISkillBook.cpp:142-250` 注释明确「r15 曾误判 borrow recommend/0 桃色横幅」，正确为 `tab_plate = classic ? UiAssetResolve::window("Item")["New"] : nl::node()`。**遗留**：逐像素 oracle「技能展开宏指令.png」需 IMG 恢复后复核（r03 §3 F 维 12 项） |
| 协议 opcode | `docs/features/s9-protocol-align/protocol/opcode-matrix.json`、`Net/OutPacket.h`、`Net/PacketSwitch.cpp` | **新增** | Angel 083 侧 7 个 c2s opcode `90/91/92/93/101/104/165` 为既有文档未覆盖的一手事实；其中 `92/93/101` 与当前端 `CANCEL_BUFF/SKILL_EFFECT/USE_INNER_PORTAL` **同号异义**，`104/165` 与 s2c `FAMILY_SUMMON_REQUEST/UPDATE_CHAR_BOX` **c2s↔s2c 撞号** → 新增「数值重合 ≠ 语义重合」分列纪律（README §2.4 / §6） |

## §2 既有 REA 证据单列（不得冒充 Angel 13339 一手）

`docs/features/beidou-v17-parity/notes/evidence/r17/`：

| 文件 | n | 处置 |
| --- | --- | --- |
| `rea_search_Skill.json` | 23 命中 | 单列：字符串层线索（`UIWindow.img/EnchantSkill/*`、`AranSkillGuide/%d/Skill/Tab`、`Skill/5201002`、`MobSkill.img` 等） |
| `rea_search_SkillEx.json` | 0 | 空 |
| `rea_search_SkillMacro.json` | 0 | 空 |
| `rea_search_SkillUp.json` | 0 | 空 |
| `rea_search_AranSkillGuide.json` / `EnchantSkill.json` / `MacroSkill.json` | — | 单列：字符串层线索 |

- subject = `BeiDou-ClientV17.7/BeiDou.exe`，sha256 `1198fa57ca5a7c489bae43ec13c69681d9cabe0f96762f3dc0357facf2e7d4df`，base `0x400000`，size `0xa94000`，md5 `44c1030ecd8484e849c83892eb8fa025`。
- `SkillEx/SkillMacro/SkillUp` 三个搜索 **n:0 全空** → stripped 端符号层几乎全空，**反证 Angel 带符号 IDB 只补语义层**（与 r01 §0.1 的 106 具名函数互为对照）。
- 这些文件属 BeiDou V17.7 线，**非 13339 IDB**，本面只作反证与字符串线索，不进 §3 判定表的一手列。

## §3 结论

- **销案**：绝大多数（升级链 / UISoftKey 闪退 / 宏 / 特效 / 交叉功能面）既有笔记被 083 一手语义确认，闭环。
- **推翻**：1 项（r15 tab_plate 借 `Item/New` 误判）。
- **新增**：3 组（技能升级资源缺失降级、`set_macro deactivate`、083 侧 7 opcode + 分列纪律；Angel `MapMacro` 一手链）。
- **遗留**：F 样式维 12 项 + E 排版 2 项 + G 交互 1 项（IMG 阻塞，r03）+ 决策点 D1–D14（待用户裁决，README §6）。
- 全程只读审计，未改既有笔记、未改源码、未建提交。
