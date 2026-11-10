# 第十二面之后 · 跨面决策矩阵（D1–D14 × 12 面 = 168 条）

> 性质：**纯文档汇总**。不新增任何代码、不裁决任何业务规则；只把十二份面 README 的 §6 决策点横向拉平，
> 供一次性批量裁决。各面 README 仍是唯一权威，本文件不改写任何面结论。

---

## 0. 六条结论

1. **十二面各 14 条、合计 168 条**决策点已全部提取并机器校验（每面 rows=14、无重号、无缺号）。
2. 按「推荐默认」的裁定性质分四类：**K1 可直接做 10 条 / K2 保持现状 94 条 / K3 待裁决·待证据 39 条 / K4 需人工判读 25 条**。
   → **104 条（62%）已可一次性结案**（K1+K2）；余下 K3+K4 共 64 条中，**36 条可由 §5 的四条批量裁决一并消解**。
3. 168 条里有 **32 条被多个主题同时命中**（如 `skill/D1` 既属撞号分列又属总体取向），
   说明跨面存在**结构性重复**：**12 组主题**覆盖 116 条，且 **12 组全部跨 ≥3 面**。
4. 合并机会最大的是 **T5 IMG/orange-wz 阻塞（25 条 / 12 面全覆盖）** 与 **T6 真机抓帧补证（28 条 / 11 面）**：
   这两组本质上**同一根因、同一处置**，裁决一次即可同时关闭 40+ 条，不需要逐面重复问。
5. `T1 总体取向` 只有 14 条但横跨 **9 面**——它是唯一一条「一次裁决决定全局基调」的元决策，
   建议作为**第 0 号裁决**先定，其余 167 条的推荐默认都以它为背景。
6. 收敛后真正需要逐条读原文的只有 **28 条（16.7%）**，清单见 §5.1。

---

## 1. 取证基线

### 1.1 数据源与提取口径

| 项 | 值 |
| --- | --- |
| 数据源 | `docs/features/{face}-angel-calibration-2026-11/README.md` 的 `## 6. 决策点 D1–D14` |
| 面数 × 条数 | 12 × 14 = **168** |
| 切段正则 | `\n##\s*§?\s*6\.?[^\n]*\n(.*?)(?=\n##\s*§?\s*7\.?\s)` |
| 行识别 | `^\|\s*\**\s*(D\d+)\s*\**\s*\|` |
| 产物 | `evidence/d-matrix.tsv`（27359 B，168 行 + 表头，8 列 TSV） |

### 1.2 表头异构（必须按列名映射，不能按列序号）

| 面 | §6 表头 | 特殊情况 |
| --- | --- | --- |
| login-flow | `ID / 决策点 / 选项 / 推荐默认 / 影响面` | 推荐默认写 `(a)/(b)/(c)` 字母，真值在「选项」列 |
| statusbar-hud | `# / 决策点 / 推荐默认值 / 依据` | — |
| corewindow | `决策点 / 问题 / 推荐默认 / 影响面` | **无 ID 列**；首列「决策点」只写 `D1..D14`，真语义在「问题」列 |
| social-economy | `决策点 / 问题 / 推荐默认 / 依据/风险` | 同 corewindow，需优先取「问题」列 |
| cashshop | `# / 决策点 / 推荐默认 / 依据 / 级别` | 多一列「级别」 |
| 其余 7 面 | `# / 决策点 / 推荐默认 / 依据` | — |

### 1.3 两处必须记录的提取坑

- **坑① 单元格内裸竖线**：`login-flow` 的 D3 行含 `JOB/100==22||2001`（两枚 `|`），`split("|")` 得 7 列。
  处置：列数 ≠ 表头列数时改「端锚定」（`id=cells[0]`、`decision=cells[1]`、`recommend=cells[-2]`、`impact=cells[-1]`、其余并入选项）。
- **坑② 选项字母**：`login-flow` 10 条推荐默认只写 `(a)` / `(b)`。处置：正则回查「选项」列 `(a) … ；(b) …` 取对应文案，
  已全部补全（见 TSV `resolved_option` 列；本文件 §3.1 展示的就是解析后的真值）。

### 1.4 校验结果

| 校验项 | 结果 |
| --- | --- |
| 每面行数 | 12/12 面均为 14 |
| 总行数 | 168 = 12 × 14 |
| 重号 / 缺号 | 无（每面 D1..D14 连续） |
| 空推荐默认 | 0 |
| 决策点误取为 ID | 0（`BAD_DECISION=0`） |

---

## 2. 口径

### 2.1 四类裁定（按「推荐默认」的语义强度分）

| 类 | 含义 | 条数 | 处置 |
| --- | --- | --- | --- |
| K1 | 可直接做（零风险 / 只改注释 / 只加日志 / 只回写文档） | 10 | 无需裁决，可纳入下一轮 P0 |
| K2 | 保持现状（不新增 / 不拆 / 不分列 / 维持 / 已坐实非缺陷） | 94 | 认可即结案，只需确认「不反对」 |
| K3 | 待裁决 / 待证据（待真机 / 待 IMG / 待 s9 确认 / 挂 P2-P3） | 39 | 需用户逐条或按主题批量裁 |
| K4 | 需人工判读（措辞跨类、或结论混装） | 25 | 需逐条读原文 |

### 2.2 三类证据优先级（各面一致，此处只摘要）

```text
opcode-matrix.json  >  live 源码构造点/调用点  >  Angel 083 反编译  >  s9 服务端源码  >  旧台账/旧笔记
```

### 2.3 与本仓库既定红线的关系

- **083/v83 only**，禁引 095（`Maplestory95.i64` / port-095 / ExpeditionIntermediary / v95 Academy）。
- **校准 ≠ 改回原生**：改回 083 原生会断 s9 联机，所有「保持 s9」类结论都基于此。
- **数值重合 ≠ 语义重合**：撞号一律分列，禁止仅凭 opcode 数值判等（T4）。
- **不发明客户端能力**：无资源/无服务端数据的项一律显式预留或降级壳（T11）。

### 2.4 怎么用这份文件

```text
第 0 步：读 §5 的 5 个批量裁决 → 一次性覆盖 104 条（K1+K2）
第 1 步：按 §4 的 12 组主题裁 K3 → 一次裁决关闭同组所有面
第 2 步：只剩 §3 里标 K4 的 27 条需要逐条读原文
```

---

## 3. 十二面 168 条矩阵

> 列：「类」= §2.1 的 K1–K4。文本中 `|` 已转全角 `｜` 以免破坏表格。

### 3.1 登录链路（`login-flow`，14 条）

| ID | 决策点 | 推荐默认 | 类 |
| --- | --- | --- | --- |
| **D1** | 总体取向：083 原生 vs s9 适配冲突怎么处置 | 保持 s9 联机优先，仅在文档标注差异 | K2 |
| **D2** | CHARLIST exp 宽度：`read_long()` 8 B vs 注释自述「exp4+tail4」 vs 083 原生 wire 4 B | 先抓一帧真机 CHARLIST + 读 s9 `PacketCreator.addCharStats` 源码再定 | K3 |
| **D3** | 骑士团条件解码分支 | 先确认 s9 是否发追加段再补 | K3 |
| **D4** | level 1 B ↔ 2 B | 保留 2 B（999 级改造，代码自证） | K2 |
| **D5** | 滚动补间 S 曲线 → 线性 | 改为 Angel 常量速度线性 | K4 |
| **D6** | `GetStepHeight` step4 偏移分支 | (a)（需先确认本端 step4 是否可达） | K3 |
| **D7** | 原生独有交互/UI 分支：case 26 上次登录世界高亮、case 27 推荐世界弹窗、case 23 SPW 结果、case 1/2 GUEST/AccountInfo、125–130 兜底 | (a)（GUEST 按 Q2 裁决不做、Academy/BossParty 按 MEMORY 不做） | K4 |
| **D8** | View Choice / `CUILoginStart` 5 按钮 | 按 `CUIRecommendWorld` 四节点补真弹窗 + 复核 2 个缺失按钮 | K4 |
| **D9** | 60 频道翻页 | 补翻页（15 世界 / 60 频道 + 翻页控件） | K4 |
| **D10** | IMG 层 orange-wz MCP | (b)（既有文档记录该 MCP 对私服 img 有解析缺陷） | K4 |
| **D11** | stub 文案与实现不一致 | 删除 `locale/zh-CN.ini:5-6` stub 文案、改为真行为提示 | K4 |
| **D12** | 登录发包字段布局 / VAC 编号 | 只做逐字节核对文档、不动代码 | K4 |
| **D13** | CHARLIST 外层固定 15 槽 vs 按 charcount | 保持 s9 | K2 |
| **D14** | 上一轮遗留 6 项（r01 §9） | (a)（需实机/IMG） | K3 |

### 3.2 状态栏 HUD（`statusbar-hud`，14 条）

| ID | 决策点 | 推荐默认 | 类 |
| --- | --- | --- | --- |
| **D1** | 单体 `CUIStatusBar` → 当前端 10+2 类拆分，是否合并回单体？ | 保持拆分（功能性等价优先，合并会重写全部 HUD 类、风险极高） | K2 |
| **D2** | 脏词 / 刷屏 / 重复句 / 禁言业务规则是否补？ | 先确认 s9 服务端是否已实现（避免双罚 / 与服务端判定冲突），确认后再定客户端补不补 | K3 |
| **D3** | IME 候选窗：重建 `CCtrlMLEdit` 状态机，还是只开聊天框 unicode？ | 先零风险开 `UIChatBar.cpp` 的 `set_accept_unicode(true)` + 复用系统 IME，不重建候选窗状态机 | K1 |
| **D4** | `change_target(int32_t action) {}` 空 stub 是否接线？ | 接线到 chatTarget 下拉选择（零风险，6 个频道快捷键复活） | K1 |
| **D5** | 按钮 id 注释误标（`2005/2006` vs `1006/1009`）是否改？ | 直接改注释（零风险，纯文档） | K1 |
| **D6** | QS 几何常量：改回原生 32×32/647/427/8 格，还是保持当前端 28/35/34/9/9？ | 保持当前端（已用正确端截图逐像素对齐，改回破坏实机排版） | K2 |
| **D7** | 聊天窗三态 + 原生尺寸体系（26..507/509-h/id1010）是否对齐？ | 先五屏实机截图核对三态可见差异，有差异再定 | K3 |
| **D8** | `CUIPartyHP` 6 人组队血条是否实现？ | 先确认 s9 是否下发组队血条数据，无数据则不做（避免假窗） | K3 |
| **D9** | `SetButtonBlink` + `OnChildNotify` 通知链是否补？ | 低优先，先记录（视觉提示，不影响功能闭环） | K3 |
| **D10** | orange-wz IMG MCP 是否接入？ | 不接（已知解析缺陷，证据以 `.nx` dump + 截图 + `StatusBar.img.xml` 为准） | K2 |
| **D11** | 是否回写 `feature-gap-inventory-2026-11/README.md` 的 4 条旧账（B1/B2/#2/#9）销案？ | 回写（属文档一致性，不改产品源码；本轮不擅自改，交用户裁决） | K4 |
| **D12** | `UIMobGage.*` 空 Type（MOBGAGE=102）是否补实体类？ | 保持（`UIBossHP` 已覆盖 Boss 血条，空 Type 无害） | K2 |
| **D13** | 小地图 SMALL/BIG 缩放钮 / `TODO: Toggle scrolling map` 是否补？ | 保持显式预留（本包无件，「不发明客户端能力」门禁） | K2 |
| **D14** | 是否进入实施阶段？ | 先按 §7 P0 做零风险纯客户端校准，P1 待上述裁决；全程单测 + `tools/build_smoke.bat` + 五屏实机比对；原子提交须单独授权 | K1 |

### 3.3 核心窗口（`corewindow`，14 条）

| ID | 决策点 | 推荐默认 | 类 |
| --- | --- | --- | --- |
| **D1** | 是否把 12 独立窗 + 20+ 壳 + 5 组件合并回 Angel 62 类一一对应？ | 保持现有重构（合并会重写全部核心窗、风险极高且破坏已实机对齐的排版） | K2 |
| **D2** | 083 原生语义 vs s9 联机，冲突处以谁为准？ | 保持 s9 联机优先，Angel 仅作语义校准源，差异逐条标注「改回原生会断联机」 | K2 |
| **D3** | 脏词过滤整族（HUD 聊天 + 本面宏名）是否补？ | 先确认 s9 服务端是否已实现，避免双罚；确认后再定客户端是否补 | K3 |
| **D4** | 扩展背包 `0x7D5` / 国服 `0x7D3=BtGather` 如何处理？ | 保留 s9/国服适配，仅记录差异，不补原生分支 | K2 |
| **D5** | 095 线地址污染 4 处是否换依据？ | 零风险换注释：改引 Angel `CUIStat::OnCreate 0x8c4c7f` / `CUIItem::OnChildNotify 0x81d01f`，坐标结论保留 | K1 |
| **D6** | `CLASSIC_SKILL_OFFSET` 文档 (7,99) vs 源码 (10,87)？ | 不改源码，先实机比对技能窗文字/贴图 12px 错位再定 | K2 |
| **D7** | TooltipBackdrop 配色两版以谁为准？ | 以源码 `TextTooltip.h:28-47` 为真值，只回写文档中间态说明 | K4 |
| **D8** | `CUIStat` 6 掩码 / `CUISkill` 500ms 门闩 / `CUIKeyConfig` 5 分支覆盖度？ | 先出逐条对照清单，不改代码；确认 s9 是否需要 | K3 |
| **D9** | 宏系统 32 格 → 当前端 5 槽是否补？ | 保持 5 槽（`SkillMacros::MAX_SLOTS=5` 与 s9 `SKILL_MACRO(110)` 对齐） | K2 |
| **D10** | `CWorldMapDlg 666×524` 是否改当前端世界地图？ | 保持 s9 布局，原生「无在线玩家列表」结论已坐实，右上列表为 s9 挂接，不判缺失 | K2 |
| **D11** | Q2 清单 C14/C34/C35/C13 及 §16 #8/#9 等旧账是否回写？ | 交用户裁决后统一回写（本面只出销案证据，不擅改 Q2 文档） | K4 |
| **D12** | 是否补采 `ViciousHammer`/`VegaSpell`/`CTemporaryStatView` EA + 映射 `sub_8E49B5`/`sub_83EA05`？ | 下一轮补采（本面 441 精选未覆盖，用 `r24/*_wz_nodes.txt` 把 r16「仅 B 锚点」推到 C 前段） | K4 |
| **D13** | orange-wz IMG MCP 阻塞如何处理？ | 不接（已知解析缺陷），保持仓库内 `nx_inspect.py` dump + 正确端截图链 | K2 |
| **D14** | stub / 英文占位文案是否本地化？ | P0 零风险先做：`cashshop.stub_job/stub_name`、`UIKeyConfig.cpp:731` 英文、locale 英文原值批次（用户 2026-11-02 决策 B 保持英文原值者不动） | K1 |

### 3.4 社交经济（`social-economy`，14 条）

| ID | 决策点 | 推荐默认 | 类 |
| --- | --- | --- | --- |
| **D1** | `change_target()` 空 stub + 10+4 处 v95 注释污染是否本轮清理？ | 做：零风险先接线 `UIChatBar::change_target`（本地回显 6 频道）+ 只换注释依据引 Angel `CUIMessenger`/`CUIFamily`，不改行为/协议/坐标 | K1 |
| **D2** | `PARTY_SEARCH_START(222)` / `UPDATE(223)` 字段以哪套为准？ | 先抓帧 + 读 s9 服务端 `addCharStats` 再定，不盲改 | K3 |
| **D3** | 联盟双入口（页签 + `UIAlliance` 浮窗）/ `UIGuild` 浮窗是否合并回原生？ | 保持现状（083 原生无独立窗，但 s9 已用双入口）；合并风险极高 | K2 |
| **D4** | `PARTY_SEARCH_REGISTER(220)` / MTS 0xFD / 每日签到是否立项补？ | 保持 s9 联机优先：220 用 222/223 代理、MTS/ITC 维持「仅壳」、每日签到维持 NO-NATIVE 假态 | K2 |
| **D5** | `UNION` 排枚举末尾 / 动态 id 是否收敛为 Angel 连续 0..4？ | 保持 `tab_button()` 显式映射（非缺陷） | K2 |
| **D6** | 五页签落位/容量/权限显隐是否按 Angel 逐格回改？ | 先跑 5 张 oracle 帧 `layout.py` 量测，只在 Δ>2px 处微调；`TAB_POS` 是截图实测，比 IDA 立即数更贴实机 | K3 |
| **D7** | `UIStorage ["Storage.img"]` × 083 `:29 Trunk` / 25 个节点名失配是否改节点路径？ | 先出「代码字面量 ↔ 运行时节点」对照清单，只改有运行时 dump 佐证的；无证据的保持显式预留 | K3 |
| **D8** | UIParty 4 钮 / UIBuddyList / UIGuild 4 钮 / UIStorage 3 钮 / 六 DIY 壳 / pulley 上行 / 列表加长是否补深度？ | 先补零资源依赖的行为（SoftKey 已有）；需要 IMG 件的列 P2，不凭空造贴图 | K3 |
| **D9** | `UIFriendGroup` / 公会徽章编辑 / MapleTV 完整 UI 三个「整族缺失」是否立项？ | 保持明确不做/搁置：083 顶层清单无独立窗、s9 无后端；MapleTV 待 REA/IDA 取证后另立 Plan | K2 |
| **D10** | MTS / ITC 拍卖、UIWedding 玩法判定、s2c 空实现是否补？ | 保持：s9 无拍卖后端；UIWedding 判定留服务端；空实现属显式预留 | K2 |
| **D11** | Q2 清单 6 处自相矛盾 + 095 文档污染 + `TemporaryStatView 全缺` 旧账是否回写？ | 交用户裁决后统一回写（本面只出销案证据，不擅改 Q2 / 登录面 / 核心窗口面文档） | K4 |
| **D12** | `wedding.*` 26 键全英文 / en-US 中文硬编码 / stub 文案是否译？ | 保持决策 B「保持英文原值、只建键」，等 083 String.img / IDA 依据再换 | K2 |
| **D13** | 8 个社交窗缺 `ui.position.*` 项 / 五页签实机验收是否补？ | 补键为低优先（不动硬编码坐标）；实机验收需用户屏前复拍，本环境历史抓屏全黑不假装已验证 | K3 |
| **D14** | 是否接 orange-wz(IMG) MCP / 是否进入实现？ | 不接（Host 未注册 + 已知解析缺陷，替代链已够）；先完成 P0 零风险 + 全量 Rebuild + 五屏实机比对，原子提交须用户授权 | K1 |

### 3.5 宠物召唤（`pet-summon`，14 条）

| ID | 决策点 | 推荐默认 | 类 |
| --- | --- | --- | --- |
| **D1** | `MapPortals.h:40` 095 注释污染 | 做：只换注释依据引 Angel 083 `CPortal`，不碰通道逻辑 | K4 |
| **D2** | 两处 s9 字节丢弃（`:526` 8B uid→int32、`:520-523`/`:1414-1417` 字段读了就丢） | 保持 s9 联机优先，改回原生会断联机，交裁决 | K2 |
| **D3** | 三页宠物装备负槽位穿戴（区间 21–29/31–37/39–45 + -100 偏移） | 先补「槽位数据 ↔ 协议批次」对照，接线待裁决 | K3 |
| **D4** | CActionMan 动作帧 / TamingMob 骑宠实体 / 自动说话 / Lv≥15 / 500 冷却整族 | 维持不做（或单列补齐计划），待裁决 | K2 |
| **D5** | c2s/s2c 撞值分列口径 | 保持分列（167↔170、175↔175、181↔181） | K2 |
| **D6** | BtPet1/2/3 origin (15,13) vs (15,0)、y 高 13px | 只记不改，等一张原生展开宠物面板帧再定 | K3 |
| **D7** | 4 格改色命中率 34.67% 偏低格 | 先出「像素改色 ↔ 原生 disabled」对照，待裁决 | K3 |
| **D8** | 召唤/龙/骑宠 UI 资源层空白（无截图/layout/PS/顶层节点） | 补 orange-wz 实解析或原生展开帧，待资源 | K3 |
| **D9** | AskPet gage ColorBox / locale 4 stub 键 / Configuration 覆盖稀薄 | 维持降级占位（E19 无资源），低优先补文案 | K2 |
| **D10** | 场地伤害族 5 条 / 220 / G8 / G9 / `change_target` 空 stub 整族 | 维持不做，跨面统一登记 | K2 |
| **D11** | Q2 文档债统一回写（`feature-gap:197/220/627`、`parity-gap:79`、`gap-inventory.md` 旧 head） | 交裁决后统一回写，本面不擅改 Q2 | K4 |
| **D12** | Beholder/DamageSummon 仅 GM 路径、PetExceptionListHandler 空实现、PetChat/"HitN"/MoveSummon 壳 | 是否补玩家路径 / 攻击帧，待裁决 | K3 |
| **D13** | 龙 die1/filpL/R/skill 帧 + 召唤浮动帧 + CQuestMan 联动 + move_ttl 余晖 | 资源在/不在分列，接不接待裁决 | K3 |
| **D14** | 门禁与提交 / 续作 | 不接 orange-wz；先 P0 + 全量 Rebuild + 1280×720 实机；原子提交须授权；P3 续作第六面 | K2 |

### 3.6 活动小游戏（`activity-minigame`，14 条）

| ID | 决策点 | 推荐默认 | 类 |
| --- | --- | --- | --- |
| **D1** | `IO/UIStateGame.cpp:532` + `IO/UITypes/UISpecial.h:9` 095 注释污染 | 做：只换注释依据引 Angel 083，不碰福利中心行为 | K4 |
| **D2** | crosscheck「layout differs」阈值误判 | 更正为 844/844 字节级一致（字节审计文件为准），修正 `ida_angel_activity_crosscheck.py:107` 阈值逻辑 | K4 |
| **D3** | 小游戏 DIY 合并壳（Omok 胜负/禁手、MemoryGame 牌面、RPS 状态机、Factory 6 类） | 维持「wire later」壳，单列补齐计划待裁决 | K2 |
| **D4** | 活动场地对象族 / 活动 UI 完整控件表 / Tutor·EventAlarm 壳 | 维持 `FieldMarker` 黄字 + HUD 壳降级，待裁决 | K2 |
| **D5** | 三处硬冲突 + 4 个 s9 零枚举值（212 命名、case 296、299/300、212/213/215/218） | 保持 s9 联机优先，改回原生会断联机，交裁决、不得盲改 | K2 |
| **D6** | Maker 113 字段序（Angel 11 字段 vs `MakerPackets.h` 2 int 简化） | 保持 s9 简化，出「字段序 ↔ s9 MakerProcessor」对照后待裁决 | K2 |
| **D7** | c2s/s2c 撞值分列口径 | 保持分列（123↔314、189↔189、117↔117、130↔63） | K2 |
| **D8** | IMG 资源层空白（活动截图/layout/PS/顶层场地节点全 0、Slidemenu 空壳、Quiz 304） | 补 orange-wz 实解析或原生展开帧，待资源/待实机 | K3 |
| **D9** | 真缺整族（入场错误码 22、295 LEAVE、332 Stamp、locale stub、Configuration 7 键、`change_target` 空 stub、签到/XyLottery/MTS） | 维持不做 / 低优先补文案，跨面统一登记 | K2 |
| **D10** | 295 LEAVE 是否补 `OnShowMemberOutMsg` 语义（SP_4096/4097） | 先补零资源依赖行为，待裁决 | K3 |
| **D11** | Q2 文档债统一回写（`feature-gap` F1/§14.1/§9.5.3 #9/§17、`:292` 238、`parity-gap:12,56`、`s9-protocol-align` 旧读数、`gap-inventory.md` 旧 head） | 交裁决后统一回写，本面不擅改 Q2 | K4 |
| **D12** | CGrenade::JustBomb 是否补完整 decompile（当前 disasm 兜底已足语义） | 维持 disasm 兜底，低优先 | K2 |
| **D13** | AskPet gage ColorBox / 通用组件常量已齐 | 维持降级占位，低优先 | K2 |
| **D14** | 门禁与提交 / 续作 | 不接 orange-wz；先 P0 + 全量 Rebuild + 1280×720 活动屏实机；原子提交须授权；P3 后续功能面续作 | K2 |

### 3.7 商城（`cashshop`，14 条）

| ID | 决策点 | 推荐默认 | 类 |
| --- | --- | --- | --- |
| **D1** | 入口 opcode 21/38/68/123 vs 40 | 保持 s9 | K2 |
| **D2** | 是否拆 7 子窗 | 不拆，保持单窗 | K2 |
| **D3** | 是否补齐 66 方法面 | 仅文档标注，不补 | K2 |
| **D4** | `MAX_ITEMS=15` vs `CCSWnd_List` 18 控件 | 保持 15 | K2 |
| **D5** | 外层缺 323/326/327/332 handler | 记录遗留，不新增 | K2 |
| **D6** | 48 case ↔ 19 mode 映射 | 不强行对齐 | K2 |
| **D7** | 123 现金交易房撞名 | 分列，不合并 | K2 |
| **D8** | `CubeRerollPacket:182` 095 注释 | 仅清理注释措辞（可选） | K4 |
| **D9** | 24 未名 `sub_47xxxx` | 待 IDA 补符号 / 动态跟包 | K3 |
| **D10** | F 维 12 项 IMG 资源 | 待 orange-wz 恢复 | K3 |
| **D11** | 7 vs 9 tab（E5/E11） | 保持 9，标注差异 | K2 |
| **D12** | `SPEICAL` 拼写 | 保持原样 | K2 |
| **D13** | `256 USE_MAPLELIFE` 跨面归属 | 归跨面，本面只引用 | K2 |
| **D14** | 是否触发 build_smoke.bat | 等 IMG 恢复后一并 | K3 |

### 3.8 技能（`skill`，14 条）

| ID | 决策点 | 推荐默认 | 类 |
| --- | --- | --- | --- |
| **D1** | `92/93/101` 同号异义是否对齐 083 | 保持 s9 | K2 |
| **D2** | `104/165` c2s↔s2c 撞号是否新增 083 发包 | 不新增，分列记录 | K2 |
| **D3** | 是否补 `CanSendExclRequest(500/300)` 互斥门禁 | 不新增 | K2 |
| **D4** | 是否新增独立 `CWndSkillGuide` 子窗 | 不新增 | K2 |
| **D5** | `CUISkillChangeConfirm` 确认窗是否补齐 | 不补齐，保留降级 | K2 |
| **D6** | `is_keydown_skill` / KeyDown 状态机是否补 | 不新增（P2 复评） | K2 |
| **D7** | `get_novice_skill_point` 新手 SP 特判是否补 | 记录遗留 | K2 |
| **D8** | `is_able_to_map_macrosys_skill` 宏准入白名单是否补 | 不新增 | K2 |
| **D9** | 双布局分支 `classic` 是否收敛为原生单布局 | 保持双布局 | K2 |
| **D10** | `BT_SP*` 动态 x=`bg_dimensions.x()` 是否改静态 | 保持动态 | K2 |
| **D11** | `BT_HYPER/GUILDSKILL/RIDE` 三空按钮是否接管 | 保持 DISABLED | K2 |
| **D12** | 组合栏 `deactivate()` 与 `ShiftMacroUIState` 形态差异 | 保持当前端显式修复 | K2 |
| **D13** | 技能记录回包 `Decode1/2/4` vs `UPDATE_SKILL(36)` | 保持 s9 | K2 |
| **D14** | F 样式 12 项 IMG 实机复核 | 挂起 P2 | K3 |

### 3.9 NPC（`npc`，14 条）

| ID | 决策点 | 推荐默认 | 类 |
| --- | --- | --- | --- |
| **D1** | c2s 58 是否补 `Encode2(x)+Encode2(y)` | 不补，保持 s9 `write_int(oid)` + 197 对话姿态 | K2 |
| **D2** | 60/61/162 本面未抽到 `COutPacket` 是否判缺 | 不判缺，记 Angel 覆盖差异 | K2 |
| **D3** | 是否补 083 `ShowQuestList` 本地任务列表窗 | 不补，P3 ⑪ 任务面统一裁决 | K2 |
| **D4** | s2c 304/305/306 与 c2s/s2c NPC_ACTION 撞名 | 分列不判等 | K2 |
| **D5** | 095 依据是否沿用 | 不沿用为 083 一手，仅历史注记 | K2 |
| **D6** | 是否补完整 `CVecCtrlNpc` 攒多节 | 保留单节 attr=0 简化 | K2 |
| **D7** | 是否接 `SP_4419/4420` NPC 召唤/回归特效 | 暂不接 | K4 |
| **D8** | AskAvatar / Quiz / Pet 缺窗体资源 | 保留 `end_talk` 降级 | K2 |
| **D9** | 是否新增当前端缺的 083 发包 | 不新增 | K2 |
| **D10** | 是否补互斥门禁 / 改 `OutPacket.h` | 不改 | K2 |
| **D11** | IMG 恢复前 F 维 12 项 | 保持 ⏳ | K2 |
| **D12** | 71/71 by bytes 是否推翻既有「同二进制」结论 | 不推翻，正向闭合 | K2 |
| **D13** | REA r17 subject 是否并入 13339 一手 | 不并入，单列 | K2 |
| **D14** | 是否在本面建提交 / 改源码 | 不建、不改 | K2 |

### 3.10 地图（`map`，14 条）

| ID | 决策点 | 推荐默认 | 类 |
| --- | --- | --- | --- |
| **D1** | 083 `SaveFuncKeyMap` 89 键差分 + `sub_4E43FA` 条目编码 vs s9 全量 `ChangeKeyMapPacket` | 保持 s9 拆法，不回改 | K2 |
| **D2** | 083 `CMapLoadable` 36 方法资源管线是否照搬 | 不照搬，保持当前端 `MapInfo`/`Stage` 装载 | K2 |
| **D3** | 083 无独立 portal c2s vs s9 `CHANGEMAP=38 / CHANGE_MAP_SPECIAL=100 / USE_INNER_PORTAL=101` | 分列命名空间，保持 s9 | K2 |
| **D4** | 083 `CWorldMapDlg` MFC 大窗 vs 当前端 `UIWorldMap` 侧栏状态机 + 付费传送 | 保持当前端 | K2 |
| **D5** | 是否补齐 083 有而当前端未见的发包 | 不新增 | K2 |
| **D6** | `OnMapTransferResult` 083 case 2..11 + Decode4 列表 vs 当前端 2 字段 LOG | 不强行对齐 | K2 |
| **D7** | `135/183` c2s↔s9 s2c 撞号（WHISPER=135 / REMOVE_DRAGON=183） | 分列不混，禁止仅凭数值判等 | K2 |
| **D8** | `CHANGE_QUICKSLOT` 服务端 `(byte)readInt` 截断 vs 客户端 8×`write_int` | 保持既有注记，不回改 | K2 |
| **D9** | 083 地图名查表 `CItemInfo` vs 当前端 `String.nx` | 保持当前端，P3 ⑪ 联动逐字对账 | K2 |
| **D10** | IMG 内容层 `IMG 解析失败` 是否继续用 `.nx` 兜底 | 按 ACTIVE 规则合规回退，结构层留证，内容层 ⏳ | K4 |
| **D11** | F 维 12 项是否现在标 ✅ | 保持 ⏳，P2 真机再翻 | K2 |
| **D12** | `SP_267` / `String.nx` 全量对账 | P3 ⑪ 任务面联动 | K4 |
| **D13** | `CWorldMapDlg::OnKey` 小键盘 54→42 触发 5 号动作 | 真机补映射，不盲加 | K4 |
| **D14** | 本面是否建原子提交 / 改协议 | 不建提交、不改源码/协议 | K2 |

### 3.11 任务（`quest`，14 条）

| ID | 决策点 | 推荐默认 | 类 |
| --- | --- | --- | --- |
| **D1** | 是否给 `QuestActionPacket` 补 `Encode2(x)+Encode2(y)` | 不补 | K2 |
| **D2** | 是否补 action0 `OnCompleteQuestFailed` buffer 监听 | 不补 | K2 |
| **D3** | 是否补 action2 尾 `Encode4(v30)` | 不补 | K2 |
| **D4** | 是否把 083 双 record 通道并进 `UPDATE_QUEST_INFO(211)` | 不并 | K2 |
| **D5** | 是否实现 `UIQuestLog::send_key` | 待裁决 | K3 |
| **D6** | 是否拆 `CUIQuestInfo`/`CUIMedalQuestInfo` 独立窗 | 不拆 | K2 |
| **D7** | 是否补 `IsAutoStartQuest` 等自动态机 | 不补 | K2 |
| **D8** | 是否把 `sub_721008` 条件分支搬进当前端 | 不搬 | K2 |
| **D9** | 是否新增 c2s 107 真机抓包工具 | 挂 P2 | K3 |
| **D10** | 是否修 orange-wz 内容层 `IMG 解析失败` | 挂 P2 | K3 |
| **D11** | 是否补 F 样式维 12 项 | 挂 P2 | K3 |
| **D12** | 是否补 `CUIMedalQuestInfo` 独立窗 | 挂 P3 | K3 |
| **D13** | 是否把 36 opcode 常量写回 facts 作专属结论 | 否 | K4 |
| **D14** | 是否建 git commit / 改源码 | 否 | K4 |

### 3.12 掉落（`drop`，14 条）

| ID | 决策点 | 推荐默认 | 类 |
| --- | --- | --- | --- |
| **D1** | 是否按 083 把 `269` 的 `animation=4` 改成读 `Decode2` | 不改 | K2 |
| **D2** | 是否补 `animation=0` 的 `IWzGr2DLayer` 消亡动画 | 不补 | K2 |
| **D3** | 是否实现掉落归属超时（`pickuptype`/`playerdrop` 判定） | 待裁决 | K3 |
| **D4** | 是否把 `MapDrops.cpp:80` 的静默丢弃加日志 | 建议加 DEBUG 日志 | K1 |
| **D5** | 是否立项 booby trap 全链（3 函数 + 告警包） | 挂 P2 | K3 |
| **D6** | 是否立项拖拽丢弃族（9 函数 + `SetCursorState(11)`） | 挂 P3 | K3 |
| **D7** | 是否给 `Drop::Drop` switch 补 `default` 护栏 | 建议补（`state=DROPPED`） | K1 |
| **D8** | 是否给 `Optional<Drop>` 向下转型加校验 | 不改 | K2 |
| **D9** | 是否补丢金币长确认文案（`SP_5280`） | 待裁决 | K3 |
| **D10** | 是否处理 `DROP_ITEM mod=3`（消失掉落）的 `dropfrom` 语义 | 待裁决 | K3 |
| **D11** | 是否补拾取 `map_tip`（`r471`） | 待真机确认 | K3 |
| **D12** | 是否补 `animation=4` 的延时爆炸帧 | 不改 | K2 |
| **D13** | 是否把 F 维 12 项按 `.nx` 像素补齐样式结论 | 挂 P2 | K3 |
| **D14** | 是否建 git commit / 改源码 | 否 | K4 |

---

## 4. 跨面主题：12 组（覆盖 116 / 168 条，全部跨 ≥3 面）

> 排序按「涉及面数」。同组内一次裁决即可同时关闭该组所有条目。
> 覆盖 116 条 < 168 条，因为 52 条是**单面独有**决策（如 `cashshop/D2` 是否拆 7 子窗），无法跨面合并。

| 组 | 主题 | 条数 | 涉及面数 | 涉及面 | 合并建议 |
| --- | --- | --- | --- | --- | --- |
| **T1** | 总体取向：083 原生 vs s9 联机 | 14 | 9 | 活动小游戏、商城、核心窗口、登录链路、地图、NPC、宠物召唤、技能、社交经济 | **第 0 号裁决**：定「s9 联机优先、Angel 仅作语义校准源」为全局基调，其余 167 条以此为背景。 |
| **T2** | 095 注释污染清理 | 6 | 6 | 活动小游戏、商城、核心窗口、NPC、宠物召唤、社交经济 | 6 处注释文案清理，一次授权即可全改（纯注释，零行为差异）。 |
| **T3** | Q2 文档债统一回写 | 7 | 6 | 活动小游戏、核心窗口、登录链路、宠物召唤、社交经济、状态栏 HUD | **一次授权即可批量回写 Q2 文档债**（6 面 7 条），是目前唯一「用户一句话就能消除」的整组。 |
| **T4** | c2s/s2c 撞号分列口径 | 10 | 7 | 活动小游戏、商城、地图、NPC、宠物召唤、任务、技能 | 统一口径「撞号一律分列命名空间、禁止数值判等」，7 面 10 条一次认可。 |
| **T5** | IMG / orange-wz 内容层阻塞 | 25 | 12 | 活动小游戏、商城、核心窗口、掉落、登录链路、地图、NPC、宠物召唤、任务、技能、社交经济、状态栏 HUD | 裁决一次「IMG 内容层阻塞期一律 `.nx` + 真机截图兜底、F 维保持 ⏳」，12 面同时结案；恢复后单独立一轮 P2 补齐。 |
| **T6** | 真机抓帧/实机比对待补 | 28 | 11 | 活动小游戏、核心窗口、掉落、登录链路、地图、NPC、宠物召唤、任务、技能、社交经济、状态栏 HUD | 与 T5 合并为一条「补证清单」：只登记截图/抓帧需求，不逐面重复提问。 |
| **T7** | 不新增包 / 不改协议 | 11 | 5 | 商城、核心窗口、地图、NPC、技能 | 统一口径「不新增客户端发包、不改协议」，5 面 11 条一次认可。 |
| **T8** | 不建提交 / 不改源码 | 5 | 4 | 核心窗口、地图、NPC、宠物召唤 | 统一口径「本面无授权不建提交/不改源码」，4 面 5 条一次认可。 |
| **T9** | 零风险可直接实施 | 11 | 5 | 核心窗口、掉落、登录链路、社交经济、状态栏 HUD | 这 11 条是**唯一可直接开工的整组**，无需裁决，纳入下一轮 P0。 |
| **T10** | 跨面归属 / P3 联动 | 12 | 7 | 活动小游戏、商城、掉落、地图、NPC、宠物召唤、任务 | 统一「跨面归属项归到对应面 P3，不在本面重复立项」，7 面 12 条一次认可。 |
| **T11** | 降级壳 / 显式预留 | 18 | 7 | 活动小游戏、核心窗口、NPC、宠物召唤、技能、社交经济、状态栏 HUD | 统一口径「无资源/无后端 → 降级壳或显式预留，不发明能力」，7 面 18 条一次认可。 |
| **T12** | 已坐实非缺陷（销案） | 6 | 5 | 核心窗口、掉落、NPC、社交经济、状态栏 HUD | 6 条已坐实非缺陷，建议在对应面 README 里标「销案」以免后续轮次重复扫。 |

### 4.1 明细（按组）

- **T1 总体取向：083 原生 vs s9 联机**（14）：`login-flow/D1`, `login-flow/D13`, `corewindow/D2`, `corewindow/D10`, `social-economy/D4`, `pet-summon/D2`, `activity-minigame/D5`, `activity-minigame/D6`, `cashshop/D1`, `skill/D1`, `skill/D13`, `npc/D1`, `map/D1`, `map/D3`
- **T2 095 注释污染清理**（6）：`corewindow/D5`, `social-economy/D11`, `pet-summon/D1`, `activity-minigame/D1`, `cashshop/D8`, `npc/D5`
- **T3 Q2 文档债统一回写**（7）：`login-flow/D7`, `statusbar-hud/D11`, `corewindow/D7`, `corewindow/D11`, `social-economy/D11`, `pet-summon/D11`, `activity-minigame/D11`
- **T4 c2s/s2c 撞号分列口径**（10）：`pet-summon/D5`, `pet-summon/D13`, `activity-minigame/D7`, `cashshop/D7`, `skill/D1`, `skill/D2`, `npc/D4`, `map/D3`, `map/D7`, `quest/D4`
- **T5 IMG / orange-wz 内容层阻塞**（25）：`login-flow/D10`, `login-flow/D14`, `statusbar-hud/D10`, `corewindow/D13`, `social-economy/D8`, `social-economy/D14`, `pet-summon/D8`, `pet-summon/D14`, `activity-minigame/D8`, `activity-minigame/D14`, `cashshop/D10`, `cashshop/D14`, `skill/D7`, `skill/D10`, `skill/D14`, `npc/D7`, `npc/D11`, `map/D9`, `map/D10`, `map/D11`, `map/D12`, `quest/D6`, `quest/D10`, `quest/D11`, `drop/D13`
- **T6 真机抓帧/实机比对待补**（28）：`login-flow/D2`, `login-flow/D10`, `login-flow/D14`, `statusbar-hud/D6`, `statusbar-hud/D7`, `statusbar-hud/D10`, `statusbar-hud/D14`, `corewindow/D1`, `corewindow/D6`, `corewindow/D12`, `corewindow/D13`, `social-economy/D2`, `social-economy/D6`, `social-economy/D13`, `social-economy/D14`, `pet-summon/D8`, `pet-summon/D14`, `activity-minigame/D8`, `activity-minigame/D14`, `skill/D7`, `skill/D14`, `npc/D7`, `map/D6`, `map/D11`, `map/D13`, `quest/D9`, `quest/D11`, `drop/D11`
- **T7 不新增包 / 不改协议**（11）：`corewindow/D10`, `cashshop/D5`, `skill/D2`, `skill/D3`, `skill/D4`, `skill/D6`, `skill/D8`, `npc/D2`, `npc/D9`, `map/D5`, `map/D7`
- **T8 不建提交 / 不改源码**（5）：`corewindow/D6`, `corewindow/D7`, `pet-summon/D6`, `npc/D14`, `map/D14`
- **T9 零风险可直接实施**（11）：`login-flow/D11`, `statusbar-hud/D3`, `statusbar-hud/D4`, `statusbar-hud/D5`, `statusbar-hud/D14`, `corewindow/D5`, `corewindow/D14`, `social-economy/D1`, `social-economy/D14`, `drop/D4`, `drop/D7`
- **T10 跨面归属 / P3 联动**（12）：`pet-summon/D10`, `pet-summon/D14`, `activity-minigame/D9`, `activity-minigame/D14`, `cashshop/D13`, `npc/D3`, `npc/D6`, `map/D9`, `map/D12`, `quest/D12`, `quest/D13`, `drop/D6`
- **T11 降级壳 / 显式预留**（18）：`statusbar-hud/D13`, `corewindow/D1`, `corewindow/D14`, `social-economy/D4`, `social-economy/D7`, `social-economy/D8`, `social-economy/D10`, `pet-summon/D4`, `pet-summon/D9`, `pet-summon/D10`, `pet-summon/D12`, `activity-minigame/D3`, `activity-minigame/D4`, `activity-minigame/D8`, `activity-minigame/D9`, `activity-minigame/D13`, `skill/D5`, `npc/D8`
- **T12 已坐实非缺陷（销案）**（6）：`statusbar-hud/D1`, `statusbar-hud/D12`, `corewindow/D10`, `social-economy/D5`, `npc/D12`, `drop/D2`
---

## 5. 批量裁决指引（5 次裁决：结案 104 条 + 消解 64 中的 36 条，只剩 28 条逐条读）

> 全部数字来自 `evidence/d-matrix.tsv` 实算，不是估计。

| # | 裁决 | 覆盖组 | 涉及条数 | 其中 K3/K4 | 说明 |
| --- | --- | --- | --- | --- | --- |
| 0 | 认可「s9 联机优先 / Angel 仅语义校准源」全局基调 | T1 | 14 | 0 | 元决策；14 条本身都属 K2「保持」，无需再裁 |
| 1 | 一句「同意保持现状」认可 94 条 K2 | — | 94 | 0 | 不新增/不拆/不分列/维持/已坐实非缺陷 |
| 2 | 认可「IMG 阻塞期走 `.nx` + 真机兜底」，并登记补证清单 | T5 ∪ T6 | 39 | **23** | 两组高度重叠，合并裁决；其余 16 条本身是 K2 |
| 3 | 授权 Q2 文档债批量回写 | T3 | 7 | **7** | 目前唯一「一句话可消除」的整组 |
| 4 | 认可「不发明能力 / 撞号分列 / 跨面归属」三条口径 | T11 ∪ T4 ∪ T10 | 38 | **9** | 三条口径一次确认；其余 29 条本身是 K1/K2 |

```text
需裁决总量（K3+K4）            = 64
第 2/3/4 号批量裁决直接覆盖      = 36
第 0/1 号覆盖（K2+K1 全量）     = 104
──────────────────────────────────────────
残差（必须逐条读 §3 原文）       = 28
```

### 5.1 残差 28 条（批量裁决覆盖不到，需逐条看）

| 面 | 条目 |
| --- | --- |
| 登录链路 | `login-flow/D3` `D5` `D6` `D8` `D9` `D11` `D12` |
| 状态栏 HUD | `statusbar-hud/D2` `D8` `D9` |
| 核心窗口 | `corewindow/D3` `D8` |
| 活动小游戏 | `activity-minigame/D1` `D2` `D10` |
| 商城 | `cashshop/D8` `D9` |
| 宠物召唤 | `pet-summon/D1` `D3` `D6` `D7` |
| 任务 | `quest/D5` `D14` |
| 掉落 | `drop/D3` `D5` `D9` `D10` `D14` |

> 说明：这 28 条是**单面独有**或**主题不重叠**的决策（如 `drop/D5` 是否立项 booby trap 全链、`corewindow/D3` 脏词过滤族是否补），
> 无法用跨面口径一次关闭。其中 `quest/D14`、`drop/D14` 只是「本面是否建提交/改源码」，属流程项，一句话即可。
---

## 6. 一次性 Plan（按裁决结果分档）

### P0 · 零风险 · 无需裁决即可开工（T9 组 11 条，其中 10 条已判 K1）

```text
范围：T9 组 11 条（只改注释 / 只加 DEBUG 日志 / 只回写文档 / 零风险接线）；其中 10 条已判 K1，
     `login-flow/D11` 因措辞含「删除 stub」未命中 K1 规则，实质同为纯文案改动，一并可做
代表项：statusbar-hud/D3 D4 D5、corewindow/D5、drop/D4 D7、login-flow/D11、social-economy/D1
门禁：python tools/gen_opcode_matrix.py --check → GATE OK
      MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c "tools\build_smoke.bat" → EXITCODE=0 + ERROR_LINES 0
提交：一特性一提交，零署名，精确 git add
```

### P1 · 裁决通过后实施（K3 中已具备证据的子集）

```text
前置：用户完成 §5 的第 0/2/3/4 号裁决
范围：T2 注释清理 4 条、T3 文档回写 7 条、T11 降级壳认可 15 条（若裁「维持」则零改动结案）
      以及各面 K3 里「补零资源依赖行为」的条目
约束：不改协议、不新增 c2s 包、不改 opcode-matrix
```

### P2 · 资源/真机恢复后

```text
前置：orange-wz 内容层恢复，或用户提供真机截图/抓帧
范围：T5 剩余（F 维 12 项 × 12 面）、T6 补证清单
```

### P3 · 长尾

```text
范围：T10 跨面归属项、各面「另立 Plan」的族（booby trap / 拖拽丢弃 / 小游戏壳 / 独立子窗）
```

---

## 7. 限制与未知

1. 本文件是**索引不是判据**：任一结论与对应面 README §6 冲突时，以面 README 为准。
2. K4 的 25 条是「措辞跨类」导致的机器未归类，**不代表结论模糊**——原文多数是明确的，只是混装了两种口径。
3. 主题聚类是关键词命中（`32 条多重命中`），存在少量误聚；明细见 §4.1，可逐条回查原 README。
4. 各面 §6 之外还有 D 以外的决策（§7 Plan 里的 P0–P3），本文件未收——它们不是「待裁决项」。
5. 未覆盖：`InPacket::available()` 393 处待裁决（`development-and-fix-plan-2026-11/README.md` §3.1、`pending-live-tests` §27-A），
   属跨面协议层问题，与本矩阵正交，不重复登记。

---

## 8. 证据索引与复现

| 文件 | 内容 |
| --- | --- |
| `evidence/d-matrix.tsv` | 168 行 × 8 列（`face / face_idx / id / decision / recommend / resolved_option / impact / options`） |
| 十二面 README | `docs/features/{face}-angel-calibration-2026-11/README.md`（各 14 条 D 的权威出处） |

```bash
# 复核每面 14 条 / 共 168 条
PYTHONIOENCODING=utf-8 python - <<'PY'
import io,csv,collections
r=list(csv.DictReader(io.open("docs/features/angel-calibration-d-matrix-2026-11/evidence/d-matrix.tsv",encoding="utf-8"),delimiter="\t"))
print(len(r), collections.Counter(x["face"] for x in r))
PY
```

---

## 9. 性质标注

- 本文件为**只读索引**，不改动任何源码、协议、WZ/IMG 资源或面 README。
- 不含任何业务裁决；所有「推荐默认」均转录自各面 README §6，未改写语义。
- 与 `docs/features/development-and-fix-plan-2026-11/README.md` §3 的 L-A / L-B 闸门一致：
  L-B 项（需用户裁决）的数量与本文件 K3+K4 = 64 条对应，K1+K2 = 104 条属 L-A。
