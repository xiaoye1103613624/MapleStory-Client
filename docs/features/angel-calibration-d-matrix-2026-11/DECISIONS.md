# 跨面决策矩阵 · 批量裁决书（round86 / m09211）

> **性质**：本文件是**裁决态**，不是判据、不改源码 / 协议 / WZ / IMG。
> **权威链**：各面 `*-angel-calibration-2026-11/README.md` §6（判据） → 本矩阵 README §3（只读索引） → 本裁决书（状态）。
> **触发**：用户 m09211「全部继续，如果能自测的可以自己启动测试」；m09210「记录一次有可见 PUA 输出的交付，不记录评分」。
> **口径**：采纳 `docs/features/angel-calibration-d-matrix-2026-11/README.md` §5 五条批量裁决 + §5.1 残差 28 条逐条裁定；全案以第 0 号（s9 联机优先）为背景。
> **生成**：`tools/_batch_adjudicate.py` 从矩阵 README §3 的 12 张表逐字解析 168 行，非人工誊写。

## 0. 第 0 号裁决（元决策 · T1）

**s9 联机优先；Angel.exe(13339) 083/v83 带符号 IDB 仅作语义校准源；校准 ≠ 改回原生；数值重合 ≠ 语义重合；083/v83 only 禁引 095。**
改回 083 原生包形会断 s9 联机；14 条 T1 明细（`login-flow/D1`、`corewindow/D2`、`skill/D13`、`map/D3` …）本身均属 K2「保持」，无需再逐条裁。

## 1. 五条批量裁决（按矩阵 §5 实算，裁定 = 采纳）

| # | 裁决 | 覆盖组 | 条数 | 其中 K3/K4 | m09211 裁定 |
| --- | --- | --- | --- | --- | --- |
| 0 | 认可「s9 联机优先 / Angel 仅语义校准源」全局基调 | T1 | 14 | 0 | ✅ 采纳（第 0 号，见 §0） |
| 1 | 一句「同意保持现状」认可 94 条 K2 | — | 94 | 0 | ✅ 采纳，全部销案为「无变更」 |
| 2 | 认可「IMG 阻塞期走 `.nx` + 真机兜底」并登记补证清单 | T5 ∪ T6 | 39 | 23 | ✅ 采纳但**前提翻案**，见 §2 |
| 3 | 授权 Q2 文档债批量回写 | T3 | 7 | 7 | ✅ 采纳，回写 = 裁决态销案，不重写历史证据 README |
| 4 | 认可「不发明能力 / 撞号分列 / 跨面归属」三条口径 | T11 ∪ T4 ∪ T10 | 38 | 9 | ✅ 采纳 |

```text
需裁决总量（K3+K4）        = 64
第 2/3/4 号批量裁决直接覆盖 = 36
第 0/1 号覆盖（K2+K1 全量） = 104
残差（§5.1 逐条读原文）     = 28   → 本裁决书 §5 逐条裁定完毕
──────────────────────────────────────────
未决 = 0   /   168 条全部有裁定
```

## 2. T5 前提翻案（round85 实证 · 本轮必须纠正的旧口径）

- **旧口径**（矩阵 §4 T5 / 各面 D10/D13/D14 / `AGENTS.md` 旧文）：「orange-wz 对私服 img 有解析缺陷 → IMG 内容层阻塞期一律 `.nx` + 真机截图兜底、F 维保持 ⏳」。
- **实证链**（round85 `4b2452c4` / `evidence/guild-window-083-round85.json` 三实验）：①65536B 全零 key → `IMG 解析失败`；②真 65536B `key_gms`（`tools/_gms_key.b64`）→ 仍 `IMG 解析失败`；③**128B `WzAESConstant.DEFAULT_KEY`**（`tools/_gms_key_128.b64`，172 字符 base64，iv `TSPHKw==`）→ `query_nodes` 返回 `Equip` / `BtPetEquipShow` / `BtPetEquipHide` / `ToolTip/Equip`，`isError:false`。
- **真因**：`WzMutableKey.getTrimmedUserKey()`（`E:/project/orange-wz/.../WzMutableKey.java:84-90`）走「128B → 每 16B 取 1B → 32B AES key」派生；`tools/_wz.py` / NoLife `.nx` 路径用的 65536B `key_gms` 是全量 XOR 表，两条规格互不通用。**非会话过期 / 非重名 / 非服务端故障 / 非零值**。`AGENTS.md:23` 已按路径分两套规格补正。
- **本轮裁定**：T5 ∪ T6 共 39 条状态改为「**MCP 已可用；F 维 12 项与抓帧仍挂 P2，仅因真机观感未采，不再以 MCP 阻塞为理由**」。恢复动作从「修 MCP」改为「启 s9 + 五屏截图」，归 `pending-live-tests` §44 用户认领。

## 3. L-B 与 `InPacket::available()` 追认（dev-plan §3 / §11.3 口径差异销案）

`docs/features/development-and-fix-plan-2026-11/README.md` §11.1 把「L-B 全部 12 项保持 s9 优先、无变更」记为 m04292 裁决；§11.3 自述「m04292 原文只授权开发和修复、自测、提交、不能自测的记录下来一起测，并未逐条裁决跨面 D 矩阵，需用户追认」。

**m09211「全部继续」即该追认**：L-B 12 项（Quest 107 x/y、场地伤害族、220、MTS 0xFD、Title 称号窗、扩展装备栏、每日签到双端、trade-house-083、0x3730→0x3740 等）与 `Net/InPacket.h:37 bool available()` 全仓 393 处当字节数比较——**一律维持现状，零协议改动**。理由：一行 `size_t available()` 会让 393 处恒假守卫同时变活路径，等于一次性改变几乎所有 handler 的短包行为，须真机逐面回归后才可动（`pending-live-tests` §27-A）。§11.3「需用户追认」结案。

## 4. 168 条逐条裁定（12 面 × 14）

> `推荐默认` 列截自矩阵 README §3 原文（全角 `｜` 转 ` / `），完整判据仍以各面 README §6 为准。

### 4.1 登录链路 (`login-flow`，14 条）

| ID | 类 | 推荐默认（截） | 裁定状态 | 依据 |
| --- | --- | --- | --- | --- |
| **D1** | K2 | 总体取向：083 原生 vs s9 适配冲突怎么处置 | 保持 s9 联机优先，仅在文档标注差异 | `BATCH_0_BASELINE` | s9 联机优先 / Angel 083 仅语义校准源 |
| **D2** | K3 | CHARLIST exp 宽度：`read_long()` 8 B vs 注释自述「exp4+tail4」 vs 083 原生 wire 4 B | 先抓一帧真机 CHARLIST + ... | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D3** | K3 | 骑士团条件解码分支 | 先确认 s9 是否发追加段再补 | `KEEP_S9` | 骑士团条件解码分支：先确认 s9 是否发追加段；不发则不补，保持现状 |
| **D4** | K2 | level 1 B ↔ 2 B | 保留 2 B（999 级改造，代码自证） | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D5** | K4 | 滚动补间 S 曲线 → 线性 | 改为 Angel 常量速度线性 | `P2_LIVE` | 滚动补间 S 曲线→线性：属观感，真机五屏配准前不动当前实现 |
| **D6** | K3 | `GetStepHeight` step4 偏移分支 | (a)（需先确认本端 step4 是否可达） | `KEEP_S9` | GetStepHeight step4 偏移：保持当前端，不据 083 改路径 |
| **D7** | K4 | 原生独有交互/UI 分支：case 26 上次登录世界高亮、case 27 推荐世界弹窗、case 23 SPW 结果、case 1/2 GUEST/AccountInfo、125–13... | `BATCH_3_DOC_DEBT` | 授权回写=裁决态销案；历史证据 README 不重写（证据链留存） |
| **D8** | K4 | View Choice / `CUILoginStart` 5 按钮 | 按 `CUIRecommendWorld` 四节点补真弹窗 + 复核 2 个缺失按钮 | `P0_SHIPPED` | View Choice / CUILoginStart 5 按钮：round66 cfd3c10b 已按 083 重修推荐世界弹窗；余 2 按钮属增强 P3 |
| **D9** | K4 | 60 频道翻页 | 补翻页（15 世界 / 60 频道 + 翻页控件） | `KEEP_CURRENT` | 60 频道翻页：当前端按截图对齐，补翻页控件属增强 P3，不改 |
| **D10** | K4 | IMG 层 orange-wz MCP | (b)（既有文档记录该 MCP 对私服 img 有解析缺陷） | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D11** | K4 | stub 文案与实现不一致 | 删除 `locale/zh-CN.ini:5-6` stub 文案、改为真行为提示 | `NO_DEFECT` | locale stub 文案：现场 worldselect.stub_view_map/stub_view_all 对应功能确未开放，是真提示非不一致，销案不删 |
| **D12** | K4 | 登录发包字段布局 / VAC 编号 | 只做逐字节核对文档、不动代码 | `DOC_ONLY` | 登录发包字段布局 / VAC 编号：只做逐字节核对文档，不动代码 |
| **D13** | K2 | CHARLIST 外层固定 15 槽 vs 按 charcount | 保持 s9 | `BATCH_0_BASELINE` | s9 联机优先 / Angel 083 仅语义校准源 |
| **D14** | K3 | 上一轮遗留 6 项（r01 §9） | (a)（需实机/IMG） | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |

### 4.2 状态栏 HUD (`statusbar-hud`，14 条）

| ID | 类 | 推荐默认（截） | 裁定状态 | 依据 |
| --- | --- | --- | --- | --- |
| **D1** | K2 | 单体 `CUIStatusBar` → 当前端 10+2 类拆分，是否合并回单体？ | 保持拆分（功能性等价优先，合并会重写全部 HUD 类、风险极高） | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D2** | K3 | 脏词 / 刷屏 / 重复句 / 禁言业务规则是否补？ | 先确认 s9 服务端是否已实现（避免双罚 / 与服务端判定冲突），确认后再定客户端补不补 | `KEEP_S9` | 脏词/刷屏规则：先确认 s9 服务端是否已实现，避免双罚，客户端不补 |
| **D3** | K1 | IME 候选窗：重建 `CCtrlMLEdit` 状态机，还是只开聊天框 unicode？ | 先零风险开 `UIChatBar.cpp` 的 `set_accept_unicode(t... | `P0_SHIPPED` | d6029c63 / 2-19 已修中文 IME |
| **D4** | K1 | `change_target(int32_t action) {}` 空 stub 是否接线？ | 接线到 chatTarget 下拉选择（零风险，6 个频道快捷键复活） | `P0_SHIPPED` | 6ca463e1 / 2-18 已接六频道键 |
| **D5** | K1 | 按钮 id 注释误标（`2005/2006` vs `1006/1009`）是否改？ | 直接改注释（零风险，纯文档） | `P0_SHIPPED` | round52 / 2-17 已换 1005/1006/1009 注释 |
| **D6** | K2 | QS 几何常量：改回原生 32×32/647/427/8 格，还是保持当前端 28/35/34/9/9？ | 保持当前端（已用正确端截图逐像素对齐，改回破坏实机排版） | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D7** | K3 | 聊天窗三态 + 原生尺寸体系（26..507/509-h/id1010）是否对齐？ | 先五屏实机截图核对三态可见差异，有差异再定 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D8** | K3 | `CUIPartyHP` 6 人组队血条是否实现？ | 先确认 s9 是否下发组队血条数据，无数据则不做（避免假窗） | `P0_SHIPPED` | CUIPartyHP 6 人血条：round68 1c69da3f 已在共用窗用 GaugeBar 真位图重修；独立悬浮窗属架构级 P3 不做 |
| **D9** | K3 | `SetButtonBlink` + `OnChildNotify` 通知链是否补？ | 低优先，先记录（视觉提示，不影响功能闭环） | `P0_SHIPPED` | SetButtonBlink 通知链：round67 b13f8e94 已按 083 SetButtonBlink @0x8dfe21 重修状态机；任务指引 blink 属 P3 |
| **D10** | K2 | orange-wz IMG MCP 是否接入？ | 不接（已知解析缺陷，证据以 `.nx` dump + 截图 + `StatusBar.img.xml` 为准） | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D11** | K4 | 是否回写 `feature-gap-inventory-2026-11/README.md` 的 4 条旧账（B1/B2/#2/#9）销案？ | 回写（属文档一致性，不改产品源码；本轮不... | `BATCH_3_DOC_DEBT` | 授权回写=裁决态销案；历史证据 README 不重写（证据链留存） |
| **D12** | K2 | `UIMobGage.*` 空 Type（MOBGAGE=102）是否补实体类？ | 保持（`UIBossHP` 已覆盖 Boss 血条，空 Type 无害） | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D13** | K2 | 小地图 SMALL/BIG 缩放钮 / `TODO: Toggle scrolling map` 是否补？ | 保持显式预留（本包无件，「不发明客户端能力」门禁） | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D14** | K1 | 是否进入实施阶段？ | 先按 §7 P0 做零风险纯客户端校准，P1 待上述裁决；全程单测 + `tools/build_smoke.bat` + 五屏实机比对；原子提交须单独授权 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |

### 4.3 核心窗口 (`corewindow`，14 条）

| ID | 类 | 推荐默认（截） | 裁定状态 | 依据 |
| --- | --- | --- | --- | --- |
| **D1** | K2 | 是否把 12 独立窗 + 20+ 壳 + 5 组件合并回 Angel 62 类一一对应？ | 保持现有重构（合并会重写全部核心窗、风险极高且破坏已实机对齐的排版） | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D2** | K2 | 083 原生语义 vs s9 联机，冲突处以谁为准？ | 保持 s9 联机优先，Angel 仅作语义校准源，差异逐条标注「改回原生会断联机」 | `BATCH_0_BASELINE` | s9 联机优先 / Angel 083 仅语义校准源 |
| **D3** | K3 | 脏词过滤整族（HUD 聊天 + 本面宏名）是否补？ | 先确认 s9 服务端是否已实现，避免双罚；确认后再定客户端是否补 | `KEEP_S9` | 脏词过滤整族：同 statusbar-hud/D2，s9 服务端未确认前不补 |
| **D4** | K2 | 扩展背包 `0x7D5` / 国服 `0x7D3=BtGather` 如何处理？ | 保留 s9/国服适配，仅记录差异，不补原生分支 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D5** | K1 | 095 线地址污染 4 处是否换依据？ | 零风险换注释：改引 Angel `CUIStat::OnCreate 0x8c4c7f` / `CUIItem::OnChildNotify ... | `P0_SHIPPED` | round52 / 2-17 已换 083 EA |
| **D6** | K2 | `CLASSIC_SKILL_OFFSET` 文档 (7,99) vs 源码 (10,87)？ | 不改源码，先实机比对技能窗文字/贴图 12px 错位再定 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D7** | K4 | TooltipBackdrop 配色两版以谁为准？ | 以源码 `TextTooltip.h:28-47` 为真值，只回写文档中间态说明 | `BATCH_3_DOC_DEBT` | 授权回写=裁决态销案；历史证据 README 不重写（证据链留存） |
| **D8** | K3 | `CUIStat` 6 掩码 / `CUISkill` 500ms 门闩 / `CUIKeyConfig` 5 分支覆盖度？ | 先出逐条对照清单，不改代码；确认 s9 是否需要 | `DOC_ONLY` | CUIStat 6 掩码 / CUISkill 500ms / CUIKeyConfig 5 分支：round75 c07b1a7b 已复核，只出对照清单不改代码 |
| **D9** | K2 | 宏系统 32 格 → 当前端 5 槽是否补？ | 保持 5 槽（`SkillMacros::MAX_SLOTS=5` 与 s9 `SKILL_MACRO(110)` 对齐） | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D10** | K2 | `CWorldMapDlg 666×524` 是否改当前端世界地图？ | 保持 s9 布局，原生「无在线玩家列表」结论已坐实，右上列表为 s9 挂接，不判缺失 | `BATCH_0_BASELINE` | s9 联机优先 / Angel 083 仅语义校准源 |
| **D11** | K4 | Q2 清单 C14/C34/C35/C13 及 §16 #8/#9 等旧账是否回写？ | 交用户裁决后统一回写（本面只出销案证据，不擅改 Q2 文档） | `BATCH_3_DOC_DEBT` | 授权回写=裁决态销案；历史证据 README 不重写（证据链留存） |
| **D12** | K4 | 是否补采 `ViciousHammer`/`VegaSpell`/`CTemporaryStatView` EA + 映射 `sub_8E49B5`/`sub_83EA05`？ | 下一... | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D13** | K2 | orange-wz IMG MCP 阻塞如何处理？ | 不接（已知解析缺陷），保持仓库内 `nx_inspect.py` dump + 正确端截图链 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D14** | K1 | stub / 英文占位文案是否本地化？ | P0 零风险先做：`cashshop.stub_job/stub_name`、`UIKeyConfig.cpp:731` 英文、locale ... | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |

### 4.4 社交经济 (`social-economy`，14 条）

| ID | 类 | 推荐默认（截） | 裁定状态 | 依据 |
| --- | --- | --- | --- | --- |
| **D1** | K1 | `change_target()` 空 stub + 10+4 处 v95 注释污染是否本轮清理？ | 做：零风险先接线 `UIChatBar::change_target`（本地回显 ... | `P0_SHIPPED` | 6ca463e1 + fef8a56d / 2-18 2-20 已闭环 |
| **D2** | K3 | `PARTY_SEARCH_START(222)` / `UPDATE(223)` 字段以哪套为准？ | 先抓帧 + 读 s9 服务端 `addCharStats` 再定，不盲改 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D3** | K2 | 联盟双入口（页签 + `UIAlliance` 浮窗）/ `UIGuild` 浮窗是否合并回原生？ | 保持现状（083 原生无独立窗，但 s9 已用双入口）；合并风险极高 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D4** | K2 | `PARTY_SEARCH_REGISTER(220)` / MTS 0xFD / 每日签到是否立项补？ | 保持 s9 联机优先：220 用 222/223 代理、MTS/ITC 维持... | `BATCH_0_BASELINE` | s9 联机优先 / Angel 083 仅语义校准源 |
| **D5** | K2 | `UNION` 排枚举末尾 / 动态 id 是否收敛为 Angel 连续 0..4？ | 保持 `tab_button()` 显式映射（非缺陷） | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D6** | K3 | 五页签落位/容量/权限显隐是否按 Angel 逐格回改？ | 先跑 5 张 oracle 帧 `layout.py` 量测，只在 Δ>2px 处微调；`TAB_POS` 是截图实测，比 ... | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D7** | K3 | `UIStorage ["Storage.img"]` × 083 `:29 Trunk` / 25 个节点名失配是否改节点路径？ | 先出「代码字面量 ↔ 运行时节点」对照清单，只改有... | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D8** | K3 | UIParty 4 钮 / UIBuddyList / UIGuild 4 钮 / UIStorage 3 钮 / 六 DIY 壳 / pulley 上行 / 列表加长是否补深度？ | ... | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D9** | K2 | `UIFriendGroup` / 公会徽章编辑 / MapleTV 完整 UI 三个「整族缺失」是否立项？ | 保持明确不做/搁置：083 顶层清单无独立窗、s9 无后端；MapleT... | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D10** | K2 | MTS / ITC 拍卖、UIWedding 玩法判定、s2c 空实现是否补？ | 保持：s9 无拍卖后端；UIWedding 判定留服务端；空实现属显式预留 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D11** | K4 | Q2 清单 6 处自相矛盾 + 095 文档污染 + `TemporaryStatView 全缺` 旧账是否回写？ | 交用户裁决后统一回写（本面只出销案证据，不擅改 Q2 / 登录面 ... | `BATCH_3_DOC_DEBT` | 授权回写=裁决态销案；历史证据 README 不重写（证据链留存） |
| **D12** | K2 | `wedding.*` 26 键全英文 / en-US 中文硬编码 / stub 文案是否译？ | 保持决策 B「保持英文原值、只建键」，等 083 String.img / IDA 依据再换 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D13** | K3 | 8 个社交窗缺 `ui.position.*` 项 / 五页签实机验收是否补？ | 补键为低优先（不动硬编码坐标）；实机验收需用户屏前复拍，本环境历史抓屏全黑不假装已验证 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D14** | K1 | 是否接 orange-wz(IMG) MCP / 是否进入实现？ | 不接（Host 未注册 + 已知解析缺陷，替代链已够）；先完成 P0 零风险 + 全量 Rebuild + 五屏实机... | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |

### 4.5 宠物召唤 (`pet-summon`，14 条）

| ID | 类 | 推荐默认（截） | 裁定状态 | 依据 |
| --- | --- | --- | --- | --- |
| **D1** | K4 | `MapPortals.h:40` 095 注释污染 | 做：只换注释依据引 Angel 083 `CPortal`，不碰通道逻辑 | `KEEP_WITH_NOTE` | MapPortals.h 095 注释：同 activity-minigame/D1，本端扩展身份标注，保留并补口径注记 |
| **D2** | K2 | 两处 s9 字节丢弃（`:526` 8B uid→int32、`:520-523`/`:1414-1417` 字段读了就丢） | 保持 s9 联机优先，改回原生会断联机，交裁决 | `BATCH_0_BASELINE` | s9 联机优先 / Angel 083 仅语义校准源 |
| **D3** | K3 | 三页宠物装备负槽位穿戴（区间 21–29/31–37/39–45 + -100 偏移） | 先补「槽位数据 ↔ 协议批次」对照，接线待裁决 | `P0_SHIPPED` | 三页宠物装备负槽位：round78 活 IDA CUIEquip/CUIPetEquip + .nx 真节点已判合法混合面；本轮收口归档，不重写 |
| **D4** | K2 | CActionMan 动作帧 / TamingMob 骑宠实体 / 自动说话 / Lv≥15 / 500 冷却整族 | 维持不做（或单列补齐计划），待裁决 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D5** | K2 | c2s/s2c 撞值分列口径 | 保持分列（167↔170、175↔175、181↔181） | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D6** | K3 | BtPet1/2/3 origin (15,13) vs (15,0)、y 高 13px | 只记不改，等一张原生展开宠物面板帧再定 | `P2_LIVE` | BtPet1/2/3 origin (15,13) vs (15,0)：等一张原生展开宠物面板帧再定 |
| **D7** | K3 | 4 格改色命中率 34.67% 偏低格 | 先出「像素改色 ↔ 原生 disabled」对照，待裁决 | `P2_LIVE` | 4 格改色命中率 34.67%：先出像素改色↔原生 disabled 对照，真机补证 |
| **D8** | K3 | 召唤/龙/骑宠 UI 资源层空白（无截图/layout/PS/顶层节点） | 补 orange-wz 实解析或原生展开帧，待资源 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D9** | K2 | AskPet gage ColorBox / locale 4 stub 键 / Configuration 覆盖稀薄 | 维持降级占位（E19 无资源），低优先补文案 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D10** | K2 | 场地伤害族 5 条 / 220 / G8 / G9 / `change_target` 空 stub 整族 | 维持不做，跨面统一登记 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D11** | K4 | Q2 文档债统一回写（`feature-gap:197/220/627`、`parity-gap:79`、`gap-inventory.md` 旧 head） | 交裁决后统一回写，本面... | `BATCH_3_DOC_DEBT` | 授权回写=裁决态销案；历史证据 README 不重写（证据链留存） |
| **D12** | K3 | Beholder/DamageSummon 仅 GM 路径、PetExceptionListHandler 空实现、PetChat/"HitN"/MoveSummon 壳 | 是否补玩家... | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D13** | K3 | 龙 die1/filpL/R/skill 帧 + 召唤浮动帧 + CQuestMan 联动 + move_ttl 余晖 | 资源在/不在分列，接不接待裁决 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D14** | K2 | 门禁与提交 / 续作 | 不接 orange-wz；先 P0 + 全量 Rebuild + 1280×720 实机；原子提交须授权；P3 续作第六面 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |

### 4.6 活动小游戏 (`activity-minigame`，14 条）

| ID | 类 | 推荐默认（截） | 裁定状态 | 依据 |
| --- | --- | --- | --- | --- |
| **D1** | K4 | `IO/UIStateGame.cpp:532` + `IO/UITypes/UISpecial.h:9` 095 注释污染 | 做：只换注释依据引 Angel 083，不碰福利中心行为 | `KEEP_WITH_NOTE` | 095 注释：UIStateGame.cpp:532 / UISpecial.h:9 逐字是「095 自定义中文 UI 入口」本端扩展身份标注，非 083 地址污染；按 round72 教训机械换地址=二次造假，保留并补口径注记 |
| **D2** | K4 | crosscheck「layout differs」阈值误判 | 更正为 844/844 字节级一致（字节审计文件为准），修正 `ida_angel_activity_crosschec... | `DOC_ONLY` | crosscheck 阈值误判：844/844 字节级一致为准，工具层阈值逻辑登记 P2，不本轮改 |
| **D3** | K2 | 小游戏 DIY 合并壳（Omok 胜负/禁手、MemoryGame 牌面、RPS 状态机、Factory 6 类） | 维持「wire later」壳，单列补齐计划待裁决 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D4** | K2 | 活动场地对象族 / 活动 UI 完整控件表 / Tutor·EventAlarm 壳 | 维持 `FieldMarker` 黄字 + HUD 壳降级，待裁决 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D5** | K2 | 三处硬冲突 + 4 个 s9 零枚举值（212 命名、case 296、299/300、212/213/215/218） | 保持 s9 联机优先，改回原生会断联机，交裁决、不得盲改 | `BATCH_0_BASELINE` | s9 联机优先 / Angel 083 仅语义校准源 |
| **D6** | K2 | Maker 113 字段序（Angel 11 字段 vs `MakerPackets.h` 2 int 简化） | 保持 s9 简化，出「字段序 ↔ s9 MakerProcessor」... | `BATCH_0_BASELINE` | s9 联机优先 / Angel 083 仅语义校准源 |
| **D7** | K2 | c2s/s2c 撞值分列口径 | 保持分列（123↔314、189↔189、117↔117、130↔63） | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D8** | K3 | IMG 资源层空白（活动截图/layout/PS/顶层场地节点全 0、Slidemenu 空壳、Quiz 304） | 补 orange-wz 实解析或原生展开帧，待资源/待实机 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D9** | K2 | 真缺整族（入场错误码 22、295 LEAVE、332 Stamp、locale stub、Configuration 7 键、`change_target` 空 stub、签到/XyL... | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D10** | K3 | 295 LEAVE 是否补 `OnShowMemberOutMsg` 语义（SP_4096/4097） | 先补零资源依赖行为，待裁决 | `KEEP_S9` | 295 LEAVE / OnShowMemberOutMsg：s9 无该 opcode，不新增发包 |
| **D11** | K4 | Q2 文档债统一回写（`feature-gap` F1/§14.1/§9.5.3 #9/§17、`:292` 238、`parity-gap:12,56`、`s9-protocol-al... | `BATCH_3_DOC_DEBT` | 授权回写=裁决态销案；历史证据 README 不重写（证据链留存） |
| **D12** | K2 | CGrenade::JustBomb 是否补完整 decompile（当前 disasm 兜底已足语义） | 维持 disasm 兜底，低优先 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D13** | K2 | AskPet gage ColorBox / 通用组件常量已齐 | 维持降级占位，低优先 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D14** | K2 | 门禁与提交 / 续作 | 不接 orange-wz；先 P0 + 全量 Rebuild + 1280×720 活动屏实机；原子提交须授权；P3 后续功能面续作 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |

### 4.7 商城 (`cashshop`，14 条）

| ID | 类 | 推荐默认（截） | 裁定状态 | 依据 |
| --- | --- | --- | --- | --- |
| **D1** | K2 | 入口 opcode 21/38/68/123 vs 40 | 保持 s9 | `BATCH_0_BASELINE` | s9 联机优先 / Angel 083 仅语义校准源 |
| **D2** | K2 | 是否拆 7 子窗 | 不拆，保持单窗 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D3** | K2 | 是否补齐 66 方法面 | 仅文档标注，不补 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D4** | K2 | `MAX_ITEMS=15` vs `CCSWnd_List` 18 控件 | 保持 15 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D5** | K2 | 外层缺 323/326/327/332 handler | 记录遗留，不新增 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D6** | K2 | 48 case ↔ 19 mode 映射 | 不强行对齐 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D7** | K2 | 123 现金交易房撞名 | 分列，不合并 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D8** | K4 | `CubeRerollPacket:182` 095 注释 | 仅清理注释措辞（可选） | `DOC_ONLY` | CubeRerollPacket 095 注释：纯措辞，登记注释清理 P2，零行为差异 |
| **D9** | K3 | 24 未名 `sub_47xxxx` | 待 IDA 补符号 / 动态跟包 | `P2_IDA` | 24 个未名 sub_47xxxx：待 IDA 补符号 / 动态跟包，P2 |
| **D10** | K3 | F 维 12 项 IMG 资源 | 待 orange-wz 恢复 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D11** | K2 | 7 vs 9 tab（E5/E11） | 保持 9，标注差异 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D12** | K2 | `SPEICAL` 拼写 | 保持原样 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D13** | K2 | `256 USE_MAPLELIFE` 跨面归属 | 归跨面，本面只引用 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D14** | K3 | 是否触发 build_smoke.bat | 等 IMG 恢复后一并 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |

### 4.8 技能 (`skill`，14 条）

| ID | 类 | 推荐默认（截） | 裁定状态 | 依据 |
| --- | --- | --- | --- | --- |
| **D1** | K2 | `92/93/101` 同号异义是否对齐 083 | 保持 s9 | `BATCH_0_BASELINE` | s9 联机优先 / Angel 083 仅语义校准源 |
| **D2** | K2 | `104/165` c2s↔s2c 撞号是否新增 083 发包 | 不新增，分列记录 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D3** | K2 | 是否补 `CanSendExclRequest(500/300)` 互斥门禁 | 不新增 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D4** | K2 | 是否新增独立 `CWndSkillGuide` 子窗 | 不新增 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D5** | K2 | `CUISkillChangeConfirm` 确认窗是否补齐 | 不补齐，保留降级 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D6** | K2 | `is_keydown_skill` / KeyDown 状态机是否补 | 不新增（P2 复评） | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D7** | K2 | `get_novice_skill_point` 新手 SP 特判是否补 | 记录遗留 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D8** | K2 | `is_able_to_map_macrosys_skill` 宏准入白名单是否补 | 不新增 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D9** | K2 | 双布局分支 `classic` 是否收敛为原生单布局 | 保持双布局 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D10** | K2 | `BT_SP*` 动态 x=`bg_dimensions.x()` 是否改静态 | 保持动态 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D11** | K2 | `BT_HYPER/GUILDSKILL/RIDE` 三空按钮是否接管 | 保持 DISABLED | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D12** | K2 | 组合栏 `deactivate()` 与 `ShiftMacroUIState` 形态差异 | 保持当前端显式修复 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D13** | K2 | 技能记录回包 `Decode1/2/4` vs `UPDATE_SKILL(36)` | 保持 s9 | `BATCH_0_BASELINE` | s9 联机优先 / Angel 083 仅语义校准源 |
| **D14** | K3 | F 样式 12 项 IMG 实机复核 | 挂起 P2 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |

### 4.9 NPC (`npc`，14 条）

| ID | 类 | 推荐默认（截） | 裁定状态 | 依据 |
| --- | --- | --- | --- | --- |
| **D1** | K2 | c2s 58 是否补 `Encode2(x)+Encode2(y)` | 不补，保持 s9 `write_int(oid)` + 197 对话姿态 | `BATCH_0_BASELINE` | s9 联机优先 / Angel 083 仅语义校准源 |
| **D2** | K2 | 60/61/162 本面未抽到 `COutPacket` 是否判缺 | 不判缺，记 Angel 覆盖差异 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D3** | K2 | 是否补 083 `ShowQuestList` 本地任务列表窗 | 不补，P3 ⑪ 任务面统一裁决 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D4** | K2 | s2c 304/305/306 与 c2s/s2c NPC_ACTION 撞名 | 分列不判等 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D5** | K2 | 095 依据是否沿用 | 不沿用为 083 一手，仅历史注记 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D6** | K2 | 是否补完整 `CVecCtrlNpc` 攒多节 | 保留单节 attr=0 简化 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D7** | K4 | 是否接 `SP_4419/4420` NPC 召唤/回归特效 | 暂不接 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D8** | K2 | AskAvatar / Quiz / Pet 缺窗体资源 | 保留 `end_talk` 降级 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D9** | K2 | 是否新增当前端缺的 083 发包 | 不新增 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D10** | K2 | 是否补互斥门禁 / 改 `OutPacket.h` | 不改 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D11** | K2 | IMG 恢复前 F 维 12 项 | 保持 ⏳ | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D12** | K2 | 71/71 by bytes 是否推翻既有「同二进制」结论 | 不推翻，正向闭合 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D13** | K2 | REA r17 subject 是否并入 13339 一手 | 不并入，单列 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D14** | K2 | 是否在本面建提交 / 改源码 | 不建、不改 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |

### 4.10 地图 (`map`，14 条）

| ID | 类 | 推荐默认（截） | 裁定状态 | 依据 |
| --- | --- | --- | --- | --- |
| **D1** | K2 | 083 `SaveFuncKeyMap` 89 键差分 + `sub_4E43FA` 条目编码 vs s9 全量 `ChangeKeyMapPacket` | 保持 s9 拆法，不回改 | `BATCH_0_BASELINE` | s9 联机优先 / Angel 083 仅语义校准源 |
| **D2** | K2 | 083 `CMapLoadable` 36 方法资源管线是否照搬 | 不照搬，保持当前端 `MapInfo`/`Stage` 装载 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D3** | K2 | 083 无独立 portal c2s vs s9 `CHANGEMAP=38 / CHANGE_MAP_SPECIAL=100 / USE_INNER_PORTAL=101` | 分列命... | `BATCH_0_BASELINE` | s9 联机优先 / Angel 083 仅语义校准源 |
| **D4** | K2 | 083 `CWorldMapDlg` MFC 大窗 vs 当前端 `UIWorldMap` 侧栏状态机 + 付费传送 | 保持当前端 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D5** | K2 | 是否补齐 083 有而当前端未见的发包 | 不新增 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D6** | K2 | `OnMapTransferResult` 083 case 2..11 + Decode4 列表 vs 当前端 2 字段 LOG | 不强行对齐 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D7** | K2 | `135/183` c2s↔s9 s2c 撞号（WHISPER=135 / REMOVE_DRAGON=183） | 分列不混，禁止仅凭数值判等 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D8** | K2 | `CHANGE_QUICKSLOT` 服务端 `(byte)readInt` 截断 vs 客户端 8×`write_int` | 保持既有注记，不回改 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D9** | K2 | 083 地图名查表 `CItemInfo` vs 当前端 `String.nx` | 保持当前端，P3 ⑪ 联动逐字对账 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D10** | K4 | IMG 内容层 `IMG 解析失败` 是否继续用 `.nx` 兜底 | 按 ACTIVE 规则合规回退，结构层留证，内容层 ⏳ | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D11** | K2 | F 维 12 项是否现在标 ✅ | 保持 ⏳，P2 真机再翻 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D12** | K4 | `SP_267` / `String.nx` 全量对账 | P3 ⑪ 任务面联动 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D13** | K4 | `CWorldMapDlg::OnKey` 小键盘 54→42 触发 5 号动作 | 真机补映射，不盲加 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D14** | K2 | 本面是否建原子提交 / 改协议 | 不建提交、不改源码/协议 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |

### 4.11 任务 (`quest`，14 条）

| ID | 类 | 推荐默认（截） | 裁定状态 | 依据 |
| --- | --- | --- | --- | --- |
| **D1** | K2 | 是否给 `QuestActionPacket` 补 `Encode2(x)+Encode2(y)` | 不补 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D2** | K2 | 是否补 action0 `OnCompleteQuestFailed` buffer 监听 | 不补 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D3** | K2 | 是否补 action2 尾 `Encode4(v30)` | 不补 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D4** | K2 | 是否把 083 双 record 通道并进 `UPDATE_QUEST_INFO(211)` | 不并 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D5** | K3 | 是否实现 `UIQuestLog::send_key` | 待裁决 | `KEEP_CURRENT` | UIQuestLog::send_key：s9 优先，不补当前端未接线键位，登记 P3 |
| **D6** | K2 | 是否拆 `CUIQuestInfo`/`CUIMedalQuestInfo` 独立窗 | 不拆 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D7** | K2 | 是否补 `IsAutoStartQuest` 等自动态机 | 不补 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D8** | K2 | 是否把 `sub_721008` 条件分支搬进当前端 | 不搬 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D9** | K3 | 是否新增 c2s 107 真机抓包工具 | 挂 P2 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D10** | K3 | 是否修 orange-wz 内容层 `IMG 解析失败` | 挂 P2 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D11** | K3 | 是否补 F 样式维 12 项 | 挂 P2 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D12** | K3 | 是否补 `CUIMedalQuestInfo` 独立窗 | 挂 P3 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D13** | K4 | 是否把 36 opcode 常量写回 facts 作专属结论 | 否 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D14** | K4 | 是否建 git commit / 改源码 | 否 | `NO_OP` | 本面是否建提交/改源码：不建独立提交，统一并入本轮批量裁决提交 |

### 4.12 掉落 (`drop`，14 条）

| ID | 类 | 推荐默认（截） | 裁定状态 | 依据 |
| --- | --- | --- | --- | --- |
| **D1** | K2 | 是否按 083 把 `269` 的 `animation=4` 改成读 `Decode2` | 不改 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D2** | K2 | 是否补 `animation=0` 的 `IWzGr2DLayer` 消亡动画 | 不补 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D3** | K3 | 是否实现掉落归属超时（`pickuptype`/`playerdrop` 判定） | 待裁决 | `KEEP_S9` | 掉落归属超时 pickuptype/playerdrop：s9 无该语义，不补判定 |
| **D4** | K1 | 是否把 `MapDrops.cpp:80` 的静默丢弃加日志 | 建议加 DEBUG 日志 | `P0_SHIPPED` | round52 / 2-17 已补 else+LOG |
| **D5** | K3 | 是否立项 booby trap 全链（3 函数 + 告警包） | 挂 P2 | `P3_NO` | booby trap 全链（3 函数+告警包）：长尾，明确不做 |
| **D6** | K3 | 是否立项拖拽丢弃族（9 函数 + `SetCursorState(11)`） | 挂 P3 | `BATCH_4_STANCE` | 不发明能力 / 撞号分列命名空间 / 跨面归属归对应面 P3 |
| **D7** | K1 | 是否给 `Drop::Drop` switch 补 `default` 护栏 | 建议补（`state=DROPPED`） | `P0_SHIPPED` | round52 / 2-17 已补 default DROPPED |
| **D8** | K2 | 是否给 `Optional<Drop>` 向下转型加校验 | 不改 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D9** | K3 | 是否补丢金币长确认文案（`SP_5280`） | 待裁决 | `KEEP_CURRENT` | 丢金币长确认 SP_5280：当前短提示可用，长文案属增强 P3 |
| **D10** | K3 | 是否处理 `DROP_ITEM mod=3`（消失掉落）的 `dropfrom` 语义 | 待裁决 | `KEEP_S9` | DROP_ITEM mod=3 消失掉落 dropfrom：按 s9 移除语义，不改 |
| **D11** | K3 | 是否补拾取 `map_tip`（`r471`） | 待真机确认 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D12** | K2 | 是否补 `animation=4` 的延时爆炸帧 | 不改 | `BATCH_1_KEEP` | 保持现状，认可即结案，无变更 |
| **D13** | K3 | 是否把 F 维 12 项按 `.nx` 像素补齐样式结论 | 挂 P2 | `BATCH_2_P2_BACKLOG` | MCP 已可用（round85 128B 实证）；F 维/抓帧仅因真机观感未采挂 P2，非解析阻塞 |
| **D14** | K4 | 是否建 git commit / 改源码 | 否 | `NO_OP` | 本面是否建提交/改源码：不建独立提交，统一并入本轮批量裁决提交 |

## 5. 残差 28 条逐条裁定（§5.1 原文逐条读后定）

| 面 / 条目 | 裁定 | 结论与是否动代码 |
| --- | --- | --- |
| `login-flow/D3` | `KEEP_S9` | 骑士团条件解码分支：先确认 s9 是否发追加段；不发则不补，保持现状 |
| `login-flow/D5` | `P2_LIVE` | 滚动补间 S 曲线→线性：属观感，真机五屏配准前不动当前实现 |
| `login-flow/D6` | `KEEP_S9` | GetStepHeight step4 偏移：保持当前端，不据 083 改路径 |
| `login-flow/D8` | `P0_SHIPPED` | View Choice / CUILoginStart 5 按钮：round66 cfd3c10b 已按 083 重修推荐世界弹窗；余 2 按钮属增强 P3 |
| `login-flow/D9` | `KEEP_CURRENT` | 60 频道翻页：当前端按截图对齐，补翻页控件属增强 P3，不改 |
| `login-flow/D11` | `NO_DEFECT` | locale stub 文案：现场 worldselect.stub_view_map/stub_view_all 对应功能确未开放，是真提示非不一致，销案不删 |
| `login-flow/D12` | `DOC_ONLY` | 登录发包字段布局 / VAC 编号：只做逐字节核对文档，不动代码 |
| `statusbar-hud/D2` | `KEEP_S9` | 脏词/刷屏规则：先确认 s9 服务端是否已实现，避免双罚，客户端不补 |
| `statusbar-hud/D8` | `P0_SHIPPED` | CUIPartyHP 6 人血条：round68 1c69da3f 已在共用窗用 GaugeBar 真位图重修；独立悬浮窗属架构级 P3 不做 |
| `statusbar-hud/D9` | `P0_SHIPPED` | SetButtonBlink 通知链：round67 b13f8e94 已按 083 SetButtonBlink @0x8dfe21 重修状态机；任务指引 blink 属 P3 |
| `corewindow/D3` | `KEEP_S9` | 脏词过滤整族：同 statusbar-hud/D2，s9 服务端未确认前不补 |
| `corewindow/D8` | `DOC_ONLY` | CUIStat 6 掩码 / CUISkill 500ms / CUIKeyConfig 5 分支：round75 c07b1a7b 已复核，只出对照清单不改代码 |
| `pet-summon/D1` | `KEEP_WITH_NOTE` | MapPortals.h 095 注释：同 activity-minigame/D1，本端扩展身份标注，保留并补口径注记 |
| `pet-summon/D3` | `P0_SHIPPED` | 三页宠物装备负槽位：round78 活 IDA CUIEquip/CUIPetEquip + .nx 真节点已判合法混合面；本轮收口归档，不重写 |
| `pet-summon/D6` | `P2_LIVE` | BtPet1/2/3 origin (15,13) vs (15,0)：等一张原生展开宠物面板帧再定 |
| `pet-summon/D7` | `P2_LIVE` | 4 格改色命中率 34.67%：先出像素改色↔原生 disabled 对照，真机补证 |
| `activity-minigame/D1` | `KEEP_WITH_NOTE` | 095 注释：UIStateGame.cpp:532 / UISpecial.h:9 逐字是「095 自定义中文 UI 入口」本端扩展身份标注，非 083 地址污染；按 round72 教训机械换地址=二次造假，保留并补口径注记 |
| `activity-minigame/D2` | `DOC_ONLY` | crosscheck 阈值误判：844/844 字节级一致为准，工具层阈值逻辑登记 P2，不本轮改 |
| `activity-minigame/D10` | `KEEP_S9` | 295 LEAVE / OnShowMemberOutMsg：s9 无该 opcode，不新增发包 |
| `cashshop/D8` | `DOC_ONLY` | CubeRerollPacket 095 注释：纯措辞，登记注释清理 P2，零行为差异 |
| `cashshop/D9` | `P2_IDA` | 24 个未名 sub_47xxxx：待 IDA 补符号 / 动态跟包，P2 |
| `quest/D5` | `KEEP_CURRENT` | UIQuestLog::send_key：s9 优先，不补当前端未接线键位，登记 P3 |
| `quest/D14` | `NO_OP` | 本面是否建提交/改源码：不建独立提交，统一并入本轮批量裁决提交 |
| `drop/D3` | `KEEP_S9` | 掉落归属超时 pickuptype/playerdrop：s9 无该语义，不补判定 |
| `drop/D5` | `P3_NO` | booby trap 全链（3 函数+告警包）：长尾，明确不做 |
| `drop/D9` | `KEEP_CURRENT` | 丢金币长确认 SP_5280：当前短提示可用，长文案属增强 P3 |
| `drop/D10` | `KEEP_S9` | DROP_ITEM mod=3 消失掉落 dropfrom：按 s9 移除语义，不改 |
| `drop/D14` | `NO_OP` | 本面是否建提交/改源码：不建独立提交，统一并入本轮批量裁决提交 |

**残差内已发货项**（不重复返工）：`login-flow/D8` → `cfd3c10b`；`statusbar-hud/D8` → `1c69da3f`；`statusbar-hud/D9` → `b13f8e94`；`corewindow/D8` → `c07b1a7b`；`pet-summon/D3` → round78 只读取证判合法混合面，本轮收口归档。

## 6. 覆盖统计与算术闭合

| 项 | 值 | 来源 |
| --- | --- | --- |
| 总条数 | **168** | 12 面 × 14，脚本解析 + 断言 |
| K1 / K2 / K3 / K4 | **10 / 94 / 39 / 25** | 矩阵 README §2.1，脚本断言一致 |
| 第 0/1 号覆盖 | **104**（62%） | K1+K2 |
| 第 2/3/4 号消解 K3/K4 | **36** | 矩阵 §5 实算 |
| 残差逐条裁定 | **28**（16.7%） | 本裁决书 §5 |
| **未决** | **0** | 104 + 36 + 28 = 168 |
| 本轮裁定状态分布 | `{"BATCH_0_BASELINE": 14, "BATCH_1_KEEP": 45, "BATCH_2_P2_BACKLOG": 39, "BATCH_3_DOC_DEBT": 7, "BATCH_4_STANCE": 28, "DOC_ONLY": 4, "KEEP_CURRENT": 3, "KEEP_S9": 7, "KEEP_WITH_NOTE": 2, "NO_DEFECT": 1, "NO_OP": 2, "P0_SHIPPED": 11, "P2_IDA": 1, "P2_LIVE": 3, "P3_NO": 1}` | `evidence/batch-adjudication-round86.json` |

> **双口径说明（防误读）**：上表「第 0/1 号覆盖 104」是矩阵 §5 的 **KIND 口径**（K1 10 + K2 94，按裁定性质分类）；而 `本轮裁定状态分布` 是 **主题批次口径**（按 §4.1 的 T1–T12 组优先分流，一条 K2 若同时命中 T1 / T5 / T11 便记入对应 `BATCH_*`，而非 `BATCH_1_KEEP`）。因此 `BATCH_1_KEEP` 只数「未被任何主题组命中的纯 K2」，不等于 94；两维度各自合计均为 168，不矛盾。逐条最终状态以本裁决书 §4 的 `裁定状态` 列为准。

## 7. 落地与未落地（诚实口径）

**已落地（本轮）**：本裁决书 + evidence JSON + notes + dev-plan §3 追认 / §4 2-33 / §5 hash 注记 / §11.3 销案 + PLT §44 + harness + `gen_opcode_matrix.py --check` + `build_smoke.bat` + 零署名原子提交。

**未落地（交用户认领，不假装完成）**：

- 真机五屏截图：`docs/features/pending-live-tests/README.md` 60 节 / 142 处真机标记，本轮**零真机执行**；归 §44 五屏归口清单，需用户启 s9 + 截屏联测。
- P2：F 维 12 项 × 12 面逐像素样式、`cashshop/D9` 24 个未名 `sub_47xxxx` 补符号、`activity-minigame/D2` crosscheck 阈值工具层修正。
- P3 长尾（明确不做）：booby trap 全链、拖拽丢弃族、独立 `CUIPartyHP` 悬浮窗、小游戏 DIY 壳、Quest 107 抓包工具。

**明确不改**：s9 协议 / 任何 opcode / `InPacket::available()` / `UIGuild` 4 处已收敛入口 / 历史证据 README（证据链留存，不重写）/ 外部程序在途 EXP WIP（hunk 级隔离，原样留工作树）。

## 8. 限制

1. 本裁决书是**状态不是判据**：任一结论与对应面 README §6 冲突时以面 README 为准；矩阵 README §7 的限制全部继承。
2. K4 的 25 条是「措辞跨类」导致的机器未归类，非结论模糊；本轮逐条裁定不改写面 README 原文。
3. 真机观感项**不得**据本文件标「已验证」；唯一验收信号仍是 `pending-live-tests` 的用户截屏记录。
4. 本轮零源码 / 零协议 / 零 WZ 改动，构建输出为门禁回归，不声称真重编覆盖（上一轮 `4b2452c4` 已对 3 个 cpp 真重编留证）。
