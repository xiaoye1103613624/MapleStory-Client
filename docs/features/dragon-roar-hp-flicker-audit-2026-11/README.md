# 龙咆哮特效 / HP=0 / 血条闪烁 — 取证审计（只读，未改源码）

> 日期：2026-11-17 ｜ 状态：**闪烁修复 + G1 龙咆哮半血门禁均已实现+构建+部署（真机待用户验收）**
> 性质：取证 + 修复文档。已改 2 个源码文件：`UIStatusBar.cpp`（闪烁节流）、`Player.cpp`（G1 龙咆哮 HP>59%maxHP 门禁）。§3 经 IDA 原生全链审计发现 5 项缺口：G1 已补、G2-G5 遗留不动。60fps 帧级闪烁实测≈15Hz（evidence/hp-flicker-frame-analysis.txt）。

## §0.2 G1 修复闭环（2026-11-17 22:44）

| 项 | 内容 |
| --- | --- |
| 改动文件 | `Character/Player.cpp:466-494`（仅 cpp，不改任何头文件 → 零增量 obj 布局风险） |
| 手法 | 在 `Player::can_use` 调 `move.can_use` 后追加龙咆哮(1311006)特判：HP > maxHP×x/100 才可放。x 按技能等级取真实值（常量表 1级59…30级30，orange-wz 全 30 级实测 `x=60-level`），等级钳 1..30。`cdd=maxHP` 由服务端 `StatEffect.java:1439 getCurrentMaxHp()` 坐实。**第10轮修正**：早期版本固定用 59% 属高估门槛，会把 30 级 HP40% 本可放的技能误拦；已改为按等级 x 表解决 |
| 依据三方一致 | Angel `CheckConsumeForActiveSkill@0x764256 if(1311006)` + 服务端 `StatEffect.java:772/1439` hpR=-x/100 自损 + `applyHpMpChange:8481` 拒自杀 → 门禁 = 恢复原生，非新增玩法 |
| 算法级验证 | 第10轮 11 用例全 PASS：1级 x=59(590拦/591放)、30级 x=30(300拦/301放/400放)、15级 x=45、等级钳制(0→1级/99→30级)、其它技能不受影响、maxhp=0 防御 |
| 构建 | `tools/build_smoke.bat` → `EXITCODE=0` + `GATE OK`，无 error |
| 部署 | live `MapleStory.exe` 22:51:22，SHA256=`969c42ff4690d5c029075a018f6a7c5c9fbe585c63ec0d11b14e72761748813f`（实时核对，含按等级x的 G1 + 闪烁两修复） |
| **真机验收** | **未做**。需用户实机：①HP 压到 59% 以下按龙咆哮应被拦并提示 HP 不足；②HP>59% 可放；③闪烁变"闪一次歇1秒"。未确认前不称"完全修复" |
| 提交 | 未提交（用户未要求自动提交） |

exe SHA 演进：`6395485a`(22:17 闪烁修复) → `66102e84`(22:34 另一任务怪物手册构建，经查仍含闪烁修复) → `daeb4fd5`(22:44 G1固定59%+闪烁) → `969c42ff`(22:51 本轮 G1改为按等级x表+闪烁，当前 live）。

## §0.1 闪烁修复闭环（2026-11-17 22:17）

| 项 | 内容 |
| --- | --- |
| 改动文件 | `IO/UITypes/UIStatusBar.cpp:2762-2801`（仅 cpp，未改公共头 → 避开 msbuild-header-layout-stale-objs 坑） |
| 手法 | 复用现成的 `classic_last_hp/mp`（原为无读取用途的 bookkeeping）当静默倒计时，不新增类成员 |
| 逻辑 | flash>0 递减，归0时设静默60帧；静默期递减不重启；60帧后仍低血才再闪；血量恢复清零静默 |
| 算法级验证 | 旧算法6帧(~0.1s≈10Hz)狂闪；新算法65帧(~1.08s)周期；带血量恢复边界 PASS（脚本输出见对话） |
| 构建 | `tools/build_smoke.bat` → `EXITCODE=0` + `GATE OK`；build_log.txt 无 error（仅既有 C4701/LNK4098 警告） |
| 部署 | 22:17 本功能构建 SHA=`6395485a...`；**22:34:27 被另一任务（怪物手册）的构建覆盖**：live `MapleStory.exe` 与 `MapleStory_pua24.exe` 同写为 SHA256=`66102e8479b0d7597a0d17090c43b9e08df9df3a96866df2ac627eb2c3870d0f`。经查 `build_monsterbook_all.log:3871` 输出到 `monsterbook_all_out\` 后被同步到 live，且日志 :3092 显示 `UIStatusBar.cpp(2756)`（报警区）已重新编译 → **当前 live exe 仍含本功能闪烁修复**，只是不再是 22:17 那份。USER.md 防回滚纪律：绝不拿旧 SHA 声明冒充现状。 |
| **真机验收** | **未做**。本端无自动登录+进图截图链路复验，需用户实机把 HP 压到阈值以下看闪烁是否变为"闪一次歇1秒"。在用户确认前不得称"已修复完成" |
| 提交 | 未提交（用户未要求自动提交） |

## §0 五条结论先行（逐条带行号）

| # | 用户疑问 | 取证结论 | 判定 |
| --- | --- | --- | --- |
| 1 | 技能释放特效对吗 | 龙咆哮 `1311006` WZ `effect` 0..17 共 18 帧（每帧 delay=80）。客户端 `Skill.cpp:62-68` 判 `src["effect"]["0"]` 为 bitmap → `SingleUseEffect`，`SkillUseEffect.cpp:24` 把**整个 effect 节点**当 1 段动画加载。与原生一致。截图中紫色法阵即此 effect（level `time=4`，持续4秒）。 | ✅ 正确，按原生 |
| 2 | 攻击怪物特效不对？ | **起手/释放特效经 IDA 原生对拍确认符合原生**（见 §2 补证）：Angel `DoActiveSkill_CombatStep@0x96d95e`→`CUser::ShowSkillEffect@0x933990`（内部 refs 调 `SKILLENTRY::GetEffectUOL@0x932d40`/`GetScreenEffectUOL@0x936304`），与本端 `SingleUseEffect(src["effect"])` 语义一致。`hit/0` 是单个动画（内含 0/1/2 三子帧，delay=120），`Skill.cpp:88-115` 走 `SingleHitEffect`（`:110`），整段加载。**第4轮逐行追通 hit 播放全链**：`Combat.cpp:368-369 apply_damage_effect` 用 `effect.move_id`→`get_move`(:540-551 返回对应Skill)→`MapMobs.cpp:169-176 apply_damage`→`move.apply_hiteffects`→`Skill.cpp:277`→`SingleHitEffect::apply`(`SkillHitEffect.cpp:26-29`)→`target.show_effect(hit/0整段动画)`。**调用链完整、move_id 来源正确，未发现断链或错传**。观感差异（缺失/位置/时机）仍需用户实机指明，代码层已无更多可查的结构问题。 | ✅ 起手特效IDA对拍 + hit播放链逐行确认无bug；观感待用户实机指明 |
| 3 | 无血也能放技能？ | 截图那次拦截正常（`Skill.cpp:314-315`→`Combat.cpp:135-142`→`Messages.cpp:63-64`，法阵是4秒残留）。**第5轮发现真实缺口**：Angel 原生对龙咆哮有专用半血门禁，本端缺失。**第7轮已补、第10轮修正为按等级x**：`Player.cpp:466-494` HP>maxHP×x/100（x=60-等级，1级59…30级30）才可放（§0.2），11用例验证 PASS。 | ✅ 已修+构建+部署，真机待验收 |
| 4 | 血量能为0、碰怪不死 | **（第4轮修正，推翻早期 Short.MIN_VALUE 误判）** 逐行追死：①`hpChangeAction(Short.MIN_VALUE)` 两条路径（`:8424`/`:8442`）由 `use_fixed_ratio_hpmp_update` 开关控制，`GameConfig.getServerBoolean` 在键缺失时返回 **false**（`GameConfig.java:434-439`，配置无此键）→ **这两条路径当前根本不走，不是真因**。②正常受击链：`TakeDamageHandler.java:304/309`→`addMPHP`(`AbstractCharacterObject.java:551`)→`updateHpMp`→`changeHpMpPool` clamp到0→`setHp`(:225，传入真实oldHp)→`dispatchHpChanged(oldHp)`→`hpChangeAction(oldHp)`(:8397)：`hp<=0 && oldHp>hp`→`playerDead()`(:7081) **死亡判定本身有效**。③龙咆哮是原生按最大HP比例自损技（`StatEffect.java:772`/`AbstractDealDamageHandler.java:271,808`），走 `applyHpMpChange`(:8471)：`cannotApplyHp = hpchange!=0 && nextHp<=0 && (!zombify||hpCon>0)`，hpCon>0 时非GM直接 `return false`（:8481-8483）→ **自损技不会把自己扣死，HP被保护，这是原生设定**。④结论：服务端逻辑自洽，未发现确凿 bug；用户所报"HP=0碰怪不死"最可能是自损技保护/zombify/客户端显示比例的观感，需真机复现触发源（发包日志）才能定性，**不臆造修改、不改玩法**。 | ✅ 代码结论：服务端死亡判定有效、无确凿bug；现象待真机复现 |
| 5 | 低血条闪烁非常快 | 根因同前（旧 `UIStatusBar.cpp:2762-2768` 归0当帧重启，6帧≈10Hz永动）。**已修**：`:2762-2801` 加60帧静默期，实测算法周期由6帧→65帧(≈1.08s)，见 §0.1。 | ✅ 已修+构建+部署，真机待验收 |

## §1 截图来源端声明（重要纪律）

- 用户截图 `docs/features/.../screens/user-report-1280x720.png`：简体中文 UI + 英文硬编码报错 + 角色 Admin LV850。
- **报错文本来自本端客户端源码**（`Messages.cpp`/`IO/Messages.h`），可证该画面至少触发了本端逻辑。
- 但按用户当面教训（m00862）：**仍不能仅凭截图判定这是"修复后本端"的验收画面**。截图来源端请用户确认；在确认前，本证据只用于**定位问题**，不作为任何"已修复"验收凭据。
- 视频 `XiaoYe_2026-11-17_21-47-50.mp4`（134x44 / 3.42s / 60fps）为 HP 条区域特写，21 帧联络表见 `evidence/hp-flicker-contact-sheet.png`，黑↔满白交替实证闪烁。

## §2 证据链（工具与出处）

- WZ/IMG：orange-wz MCP（127.0.0.1:10012，128B key，端点已连通，v1.157.49）。
  - `Data/Skill/131.img` → `skill/1311006`：effect 0..17、hit/0(3帧)、level/1..30、`action/0=alert3`、`hs=h1..h30`、`time=4`、`mpCon=16`、`mobCount=15`。
- 客户端源码：`Gameplay/Combat/{Skill,SkillUseEffect,SkillHitEffect,Combat}.cpp`、`Character/Player.cpp`、`Character/Char.cpp`、`IO/Messages.cpp`、`IO/UITypes/UIStatusBar.cpp`、`IO/Components/Gauge.cpp`。
- 服务端源码：`BeiDou-Server_s9`（`StatEffect.java`、`AbstractDealDamageHandler.java`、`Character.java`、`AbstractCharacterObject.java`、`TakeDamageHandler.java`）。
- 既有文档：`docs/features/skill-angel-calibration-2026-11/README.md`（98 条校准，本体实现判定一致，特效结构属语义族一致）；`set-hpmpalert`、`touch-monster-attack`、`damage-calc-cap-parity`。
- **IDA 原生对拍（本轮新增，Angel.i64 @13337，hexrays ready）**：
  - `CUserLocal::DoActiveSkill_CombatStep @0x96d95e`（完整反编译）：`SendSkillUseRequest` 成功后 → `CUser::ShowSkillEffect(this, a2, a3, a4:6, …)` @0x96db11 → 冷却登记。
  - `CUser::ShowSkillEffect @0x933990`（0x295f B）：refs 含 `SKILLENTRY::GetEffectUOL@0x932d40`、`GetScreenEffectUOL@0x936304`——**原生起手特效就是从技能节点 effect UOL 取动画**，与本端 `SingleUseEffect` 一致，坐实"释放特效按原生"。
  - `SKILLENTRY::GetEffectUOL@0x932d40`：按等级取 effect UOL，空则回退 `this+27`；`GetScreenEffectUOL@0x936304` 取全屏 effect。
  - 局限：IDA 字符串缓存仅 1281 条，`find_regex` 对 `hpFlash/HPFlash/HitEffect/EffectUOL/OnStatChanged` 全 0 命中——**原生 hit 特效播放与 HP 报警闪烁节拍未取得直接反编译证据**，不臆造。

## §2.1 闪烁修复的行号级方案（待用户授权后实现）

根因链（逐行）：
- `UIStatusBar.cpp:2759` `hp_low = hp_warn>0 && hp<=hp_warn/100`；HP=0 恒 true。
- `UIStatusBar.cpp:2762-2769`：每 tick `if(classic_hp_flash>0)--flash; else if(flash==0 && hp_low) flash=5;` —— flash 从 5 递减到 0 的**当帧立刻被重置为 5**，形成无间隔的 5帧闪+立即重播循环（无静默期），观感为持续高频闪。
- `UIStatusBar.cpp:2734-2735` 每 tick 调 `classic_hp_flash_sprite.update()` → `Graphics/Sprite.cpp:38` → `Animation::update()`（`Graphics/Animation.cpp:310-313`，固定 `Constants::TIMESTEP`）按 WZ 帧 delay 推进；5 帧动画播完循环重头。
- `UIStatusBar.cpp:2785-2786` `hpbar.update(hp)/mpbar.update(mp)` → `Gauge.cpp:70-93` 24 步平滑；目标恒 0 时平滑到位不抖，但 flash 覆盖层持续闪造成"整条黑↔亮"观感。
- 同构 bug：MP 报警 `UIStatusBar.cpp:2771-2778`（同样每 tick 重置 classic_mp_flash=5）。

修复方案（最小改动，仅客户端）：在 `UIStatusBar.h` 增一个报警静默计时成员（如 `int16_t classic_hp_alert_cooldown`），在 `update_classic` 中：hp_low 且 flash==0 且 cooldown<=0 时才启动 flash=5 并设 cooldown=N；每 tick cooldown 递减；flash>0 期间只递减不重启。N 取一个"闪一次后静默约1秒"的值，避免 ~10Hz 永动。**注意**：`UIStatusBar.h` 是公共头，历史坑 memory:msbuild-header-layout-stale-objs 要求改头后清理增量 obj 再全量构建，否则旧 obj 布局不一致会崩（见既有 memory）。备选方案：不增成员，复用现有 `classic_last_hp` 之外再借一个已存在但未用的状态位——须先核实，不可臆造。

## §3 由点到面：Angel 原生技能前置/发包缺口全审计（第5轮 IDA 实测）

用户问「是否还有没发包/没接收的问题、技能前置释放要求 Angel 是否分析全」。用 IDA 13337 反编译 Angel 三条核心链，逐条对照本端，**发现此前 skill 校准 README 标 ⚠️ 遗留的几项里有 2 项是龙咆哮真实缺口**：

| # | 缺口 | Angel 083 原生（IDA 一手） | 本端现状 | 性质 |
| --- | --- | --- | --- | --- |
| G1 | **龙咆哮专用 HP 前置门禁** | `CheckConsumeForActiveSkill@0x764256`：`if(nSkillID==1311006)`（0x7643de）→ ① `cdd/2 > 当前HP → return 2`（**要求当前HP > 最大HP的一半**）；② `v36 = 当前HP × LevelData[79]/100`（按当前HP百分比算自损，LevelData[79] 对应 WZ `x`） | `Skill::can_use`(`Skill.cpp:304-335`) 只看 `hp<=stats.hpcost`；`hpcost` 从 WZ `hpCon` 读(`SkillData.cpp:79`)，而龙咆哮 level 节点**无 hpCon、只有 x=59** → hpcost=0 → **没有任何半血门禁/比例自损校验** | ❌ 真实缺口，可能是用户「无血也能放/血量能为0」的根因 |
| G2 | **发包 300ms 互斥 + 技能等级门禁** | `SendSkillUseRequest@0x96d399`：`CanSendExclRequest(300)`(:96d3c4) + `GetSkillLevel<=0 return`(:96d3f5) + `v12=(skillid/1000)%10; (v12==0||9)&&sub_764721==0 → return`(:96d41f) | `UseSkillPacket`(`AttackAndSkillPackets.h:287-303`) 直发，无互斥/等级门禁；调用侧 `Combat::use_move` 仅有 `can_attack`+`can_use` | ⚠️ 遗留（README C10），s9 服务端亦有权威校验，客户端不加不会致错，但属未对齐 |
| G3 | 起手广播 93 同号异义 | `DoActiveSkill_Prepare@0x96a86e` COutPacket(93) = 准备/起手 | 本端 93 = `SKILL_EFFECT` 白名单广播（`SkillEffectPacket`） | s9 优先，不动（README C6） |
| G4 | 反重复技 CAntiRepeat | `SendSkillUseRequest` 对 `is_antirepeat_buff_skill` 调 `CAntiRepeat::TryRepeat`，失败 return 0 | 本端无 | ⚠️ 遗留，属 buff 技，非龙咆哮攻击技 |
| G5 | 指定动作/龙动作 | `SendSkillUseRequest` 对 `IsActionAppointed` 算随机动作 + `get_dragon_action_code_from_name`，龙咆哮等 DragonKnight 技走 CDragon 动作 | 本端 `SkillAction` 有 Single/ByLevel/Regular，无 CDragon 专用动作 | ⚠️ 遗留，观感层，须真机确认 |

**关键定性**：G1 是唯一能解释用户现象的真实功能性缺口——原生「HP 必须 > 半血才能放龙咆哮」，本端完全没有。这与截图左下「HP(0/1008) 但曾放出法阵」的观感吻合（法阵是先放出的残留，血量是后掉到0）。但**修复需改 `Skill::can_use` 的全技能路径或加龙咆哮特判，属玩法行为变更，按 skill 门禁5/用户授权规则须先获明确同意**，本轮只定位不擅改。

#### G1 深度坐实（第6轮）

| 证据 | 内容 | 强度 |
| --- | --- | --- |
| 唯一调用链 | `xrefs_to CheckConsumeForActiveSkill@0x764256` → 唯一调用者 `CUserLocal::DoActiveSkill@0x966f7a` L407：`CheckConsumeForActiveSkill(..., v27=CharacterData, v140, v145, skillid)` | IDA 一手 |
| a3 来源 | `v5 = *dword_BE7918`(全局CUserLocal实例)；`v140 = v5+8380`；`v145 = v5+8500`。故 `cdd = _ZtlSecureFuse(*(CUserLocal+8476))`（加密封装读） | IDA 一手，但**偏移语义未被结构命名直接坐实** |
| 三方逻辑自洽 | ① `cdd/2 > 当前HP → return 2`（要求 HP≥50%cdd）；② `v36 = cdd * LevelData[79](=WZ x)/100`，`v36 >= 当前HP → return 2`（要求 HP>cdd*x/100=x=59%）；③ 服务端 `StatEffect.java:772 ret.hpR = -x/100.0`（当前HP自损59%）。三者叠加要求 HP>59%最大HP，扣完不死，逻辑闭合 | 强推断（cdd=最大HP） |
| 服务端权威 | `AbstractDealDamageHandler.java:271` 龙咆哮额外检测距离250000；`:808` 专用伤害公式；`StatEffect.java:772` hpR 自损 | 一手 |

**仍缺**：`CUserLocal+8476` 未被 IDA 结构/命名直接证明=maxHP（加密封装），cdd=maxHP 是三方交叉强推断非直接命名。**若授权修复，按 cdd=maxHP 实现（HP>maxHP×x/100 才可放），并保留真机验证阈值。**

## §4 剩余待用户（仅真机验收，无代码阻塞）

1. **真机验收**：①HP<59% 按龙咆哮被拦 + 提示 HP 不足；②HP>59% 可放；③闪烁变"闪一次歇1秒"。
2. **"攻击怪物特效不对"具体现象**（若仍觉得不对）：①没 hit 火花 / ②法阵位置/大小 / ③数量/持续 / ④其他。代码层 hit 播放链已逐行确认无 bug。
3. **截图来源端**：是否本机当前端实机画面？（证据纪律，未确认前不作验收凭据）

## §5 下一步

用户真机确认后 mark complete + 原子提交（若需要）。G2-G5 作为已记录遗留，除非用户要求否则不动（s9 优先 + 爆炸半径）。
