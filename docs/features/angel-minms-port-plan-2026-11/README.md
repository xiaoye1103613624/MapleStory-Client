# Angel + MinMS 对照：完善 / 移植 / 修复 计划（2026-11-16）

> 触发：用户 m00120「多端支持还是使用 SDL，不推荐移植的就先搁置一下，帮我出个对照 Angel+minms 完善、移植、修复的计划文档」。
> 本次交付 = 本计划文档本身（只读产出，源码零改动、不建提交）。确认后按 §6 Wave 顺序开工。

---

## 0. 一句话结论

| 端 | 定位 | 结论 |
| --- | --- | --- |
| **Angel** | GMS 083/v83 原生客户端（**只有二进制 + IDB**：`Angel.i64` / `Angel.exe`，非源码） | 行为/布局/状态机的**唯一真源**，不是可粘贴工程 |
| **本端** MapleStory-Client | Journey/Heaven 系 + s9 深度集成 | 功能面**最宽**：~93 UITypes + ~60 壳、148/148 c2s、305 s2c，整体**比 MinMS 完善** |
| **MinMS** | `maplestory-wasm` 汉化优化叉（`src/client` = Journey），Linux/Android + BeiDou + `.img` 资源 | 总面更薄，但**若干单窗更深**（Enchant/CashShop/NpcTalk/PartyHP/勋章任务/CD 条）；**只按窗移植算法与坐标，禁止整仓粘贴** |

**三大铁律（贯穿全文）**：
1. **多端继续走 SDL**（本端已 SDL3 化，见 [cross-platform-build](../cross-platform-build/README.md)）；MinMS 的 GLFW / Android JNI 引擎与 `maple_engine` ABI **不移植、搁置**。
2. **083/v83 是语义基准，s9 是联机基准**；数值重合 ≠ 语义重合，禁止盲改包体（既有 L-B 裁决不重开）。
3. **REA → IDA → orange-wz MCP →（仅内容层解析异常）→ `.nx` 回退**（见 `AGENTS.md`）；移植前每个锚点都要有证据，不凭体感。

---

## 1. 证据基线（权威台账，避免过时口径）

| 类别 | 路径 | 用途 |
| --- | --- | --- |
| 缺口权威 | `docs/features/feature-gap-inventory-2026-11/README.md` | §9.5 协议重算 / §14 明确不做 / §15 全局阻塞 / §16 TOP20 / §17 Angel EA |
| Angel 流程审计 | `docs/features/angel-flow-audit-2026-11/README.md` | P0–P3 可做项收口状态 |
| 开发修复计划 | `docs/features/development-and-fix-plan-2026-11/README.md` | L-A / L-B 分层、执行纪律、Wave 先例 |
| 双端协议矩阵 | `docs/features/s9-protocol-align/protocol/opcode-matrix.json` | `s9_git_head=3d5ccc6f8e`，唯一权威读数 |
| 跨平台 | `docs/features/cross-platform-build/README.md`、`docs/features/client-platform-masterplan/README.md` | SDL3 多端现状与平台化总纲 |
| 真机待测 | `docs/features/pending-live-tests/README.md` | 已接线待点的 ⏳ 项 |
| MinMS 源码 | `E:\project\MinMS\src\client\`（IO/UITypes、Net、Character、Map） | 单窗深度移植源 |
| 构建门禁 | `tools/build_smoke.bat`、`tools/gen_opcode_matrix.py --check` | 每批必跑 |
| 提交规范 | skill `git-commit-standard` | Gitmoji + 云效分类 + `(#IssueID)` + 零署名 |

**过时口径纠偏**（勿重开已销案项）：
- Q2 表里的「右侧快捷 8 格点击」「单频道 world 跳过」在 live 源码已销案（`UIStatusBar` classic_qs → `send_key`；`UIWorldSelect` 无 `channel_count<2`），60 频道翻页已做。
- HeavenClient 与 Angel / 本端 / MinMS **非同树**（HeavenMS 独立重写），仅作旁证，不作移植源。

---

## 2. 三方面总览

```
                    完善（对齐 Angel 原生深度）
                             │
   Angel(行为真源) ──REA/IDA──┤
                             ├──► 移植（按窗：算法 + 坐标常量 → 本端重写）
   MinMS(单窗更深) ───────────┤
                             │
                          修复（P0 / 收包空实现 / 真渲染 / pending-live-tests）
```

- **完善**：本端相对 Angel 真正还差的「深度」与「真实渲染」，主线在本地可做（L-A）。
- **移植**：MinMS 更深窗的**算法/布局常量**，落到本端既有类，不整文件替换。
- **修复**：硬缺陷 + 收包空实现 + 真渲染动画 + 真机 backlog；具体 bug 立案见 §5。

---

## 3. 完善（对齐 Angel 原生深度）

### 3.1 已闭环，勿再列为缺口
- 十二面 Angel 校准主链（login/HUD/corewindow/skill/quest/npc/map/drop/pet/cashshop/activity）全闭环。
- 协议：`client_out_sent=148/148`、s2c 注册 ~305、dispatch ~917；旧「宠物不能发包」判定已覆写。
- angel-flow P0–P3 可做项（UserList 灌模、勋章键、图鉴 String、NpcTalk `#E/#I/#S/#K`）已收口。
- L-B 协议级项已裁决**保持 s9、不补 083 包体**（Quest 107 x/y、场地伤害族、MTS、220 stub、每日签到双端等）——本波不重开。

### 3.2 真实剩余（按层，不是「缺文件」）

| 层 | 现状 | 代表项 | 本波动作 |
| --- | --- | --- | --- |
| 玩法深度 / 壳未到 C | 窗在、发包在，交互/数据绑定浅 | NPC Quiz/AskPet gage/Mirror；CashShop 礼物/礼包；PersonalShop 变体；宠物装备三页 | §6 Wave 完善 |
| 真渲染 / 动画 | 占位或降级 | DamageSkin 运行时；宠物亲密/饱食条；MapleTV 双屏；隐藏门三态观感；BossHP 多 oid | §5 修复 + ⏳ 真机 |
| 样式 / IMG 拼接 | orange-wz 内容层常 FAIL，靠 `.nx` 回退 | 各面 F 维样式；StatusBar/OptionMenu 控件叠图 | §5 修复（证据先行） |
| 双端阻塞 / 明确不做 | ⊘ | Title 无服务端；MTS；PARTY_SEARCH_REGISTER；场地伤害触发面；095 远征面板；ijl15 扩展矩阵（300级/长伤害/签到协议） | **搁置，不立项** |
| 真机挂起 | ⏳ | [pending-live-tests](../pending-live-tests/README.md) 大批「已接线待点」 | §5 修复 backlog |

### 3.3 相对 Angel 仍值得追的原生深度（本地可做、不碰红线）
1. **匠人之魂 `CUIEnchantDlg`**：本端仅 `UIEnchantSkill` 薄壳（`IO/UITypes/UIMiscShells.cpp`）；Angel/MinMS 有完整拖放卷轴窗。→ W-M1
2. **独立 `CUIPartyHP` 悬浮血条**：本端已把 GaugeBar 并入 UserList（round68），Angel/MinMS 是独立窗——属架构选择，非完全缺失。→ W-M4（默认保持内嵌）
3. **`CUIMedalQuestInfo` / 勋章任务族**：本端 `UIMedal` 有、图标列预留；MinMS 有完整 `UIMedalQuest`。→ W-M6
4. **商城购买/礼物/确认链**：本端 `UICashShop` ~1.1k 行；MinMS ~3.4k 行含 `UICashBuyConfirm`。→ W-M2
5. **NpcTalk 深分支**：本端已主链齐；MinMS `UINpcTalk.cpp` ~4442 行（约本端 2×），作分支/排版参照（继续服从 s9 MoreTalk 契约）。→ W-M5

---

## 4. 移植（MinMS → 本端，按窗）

> **原则**：移植 = 抄算法 + 布局常量 + 状态机，在本端既有类里**重写**；禁止复制 `jrc` 命名空间、MinMS `.img` 资源假设、整文件粘贴。每个移植项落前先走 REA→IDA→orange-wz 校准锚点。

### 4.1 高价值可移植

| 源（MinMS） | 本端现状 | 移植内容 | 风险 | Wave |
| --- | --- | --- | --- | --- |
| `UIEnchant.*` | `UIEnchantSkill` 壳 | 拖装备/卷轴、白卷勾选、Tooltip、Angel 注释坐标语义 | 中（对齐 s9 强化发包） | W-M1 |
| `UICashShop` + `UICashBuyConfirm` | 薄商城 | 购买确认、礼物钮态、avatar 购买阻拦、SN 挂起流 | 中（逐项对 s9 CashShop opcode） | W-M2 |
| `UICooldownList.*` | 无独立类 | 技能 CD 图标条布局 | 低 | W-M3 |
| `UIPartyHP.*` | UserList 内嵌 GaugeBar | 默认**不拆窗**，只回填 Gauge 坐标常量；裁决要独立窗才迁 | 中（与 UserList 冲突二选一） | W-M4 |
| `UIMedalQuest.*` | `UIMedal` + Title ⊘ | 勋章任务分类/进度 UI（有数据才接；无 Title 协议只做已有 medal 面） | 中高 | W-M6 |
| `UIShortCut.*` | 已有较完整壳 | 对照按钮序/锚点，勿整换 | 低 | W-M5 附带 |

路径锚点：MinMS `E:\project\MinMS\src\client\IO\UITypes\` ↔ 本端 `E:\project\MapleStory-Client\IO\UITypes\`。

### 4.2 只参照，不整迁
- MinMS `UINpcTalk`（~4k 行）→ 按分支对照补本端，保持 s9 `NpcTalkMorePacket`。
- MinMS `UIUserList` 的 `stub_feature(...)` 表 → 反证哪些钮本端已接、MinMS 还没有（社交面**以本端为准**）。
- Angel IDA EA 表（feature-gap §17）→ 任何移植前仍走 REA→IDA→orange-wz；MinMS 注释里的 Angel 地址可当索引。

### 4.3 不移植 / 搁置（用户拍板：先搁置）
- **多端引擎**：MinMS 的 Linux GLFW / Android JNI / `maple_engine` ABI / VirtualPad / `UINotificationInbox` 移动通知 —— 本端多端继续走 **SDL3**，不换引擎。
- **命名空间与资源**：`jrc` 命名空间、MinMS `.img` 资源假设（本端 NX + `UiAssetResolve` 为准）。
- **协议红线**：任何改 s9 包体宽度/opcode 的「083 原样」逻辑（L-B 裁决）。
- **社交/公会**：MinMS 公会是 `NullHandler` + `guild_id` 永不写入的 stub，弱于本端，**禁止回迁**。

---

## 5. 修复（缺陷 / 收包空实现 / 真渲染 / 真机 backlog）

| 类别 | 代表项 | 处理方式 |
| --- | --- | --- |
| P0 / 硬缺陷 | 进图白屏类（已定案 s9 exp 4B+skip4，见 [enterfield-exp-misalign](../s9-protocol-align/notes/enterfield-exp-misalign.md)）、装备槽位不变量（见 `equip-slot-expansion-2026-11`） | 已闭环项做回归门禁；新 bug 单独立案 |
| 收包空实现 | s2c 已注册但 handler 空/浅的项 | L-A 本地灌模，原子提交 |
| 真渲染 / 动画 | DamageSkin 运行时、宠物亲密/饱食条、MapleTV 双屏、隐藏门三态、BossHP 多 oid | 证据（IDA/截图）→ 实现 → 真机 ⏳ |
| 样式 / IMG 拼接 | 各面 F 维样式、StatusBar/OptionMenu 叠图 | orange-wz MCP 优先，异常才 `.nx` 回退并留原文 |
| 真机 backlog | [pending-live-tests](../pending-live-tests/README.md) 已接线待点项 | 用户方便时连 s9 一起点；DB 造数据默认只读、需明示同意 |
| 双端阻塞 / ⊘ | Title、MTS、场地伤害触发面、扩展矩阵 | **搁置，不重开** |

> 构建纪律 reminder：二进制脚本改源码不改 mtime → MSBuild 假通过；验证前必须 `touch` 源文件 + `rm` 对应 `.obj`，核对 obj mtime > cpp mtime 且 exe SHA 变化。

---

## 6. 执行波次（确认后按序开工；单功能原子提交）

> 每批固定闭环：`证据(REA→IDA→orange-wz) → 实现 → gen_opcode_matrix.py --check → build_smoke.bat → 自测证据 → 原子提交`。改过协议的必跑握手烟测；不能自测的进 §5 ⏳ backlog。

| Wave | 内容 | 层 | 依赖 / 风险 |
| --- | --- | --- | --- |
| W-M1 | 用 MinMS+Angel 证据加深 `UIEnchant`（充实 `UIEnchantSkill`） | L-A | 中；对齐 s9 强化发包 |
| W-M2 | CashShop 购买确认 + 礼物阻拦态（参照 MinMS，对接 `CashShopPackets`） | L-A | 中；逐项对 opcode |
| W-M3 | 落地 `UICooldownList` 轻量版 | L-A | 低 |
| W-M4 | PartyHP：默认保持 UserList 内嵌，回填 Gauge 常量；独立窗需裁决 | L-A | 中；与 UserList 二选一 |
| W-M5 | NpcTalk 深分支差分补齐（对照 MinMS，不整文件替换） | L-A | 中；保持 s9 MoreTalk |
| W-M6 | 勋章任务族 UI（`UIMedalQuest` 参照，无 Title 协议只做已有 medal 面） | L-A | 中高；数据依赖 |
| W-FIX | 修复 backlog：P0 回归 + 收包空实现 + 样式拼接 + 真渲染（逐项立案） | L-A | 视项 |
| W-LIVE | 消 `pending-live-tests` 真机项（用户连 s9 配合） | ⏳ | 需真机/DB |

**并行**：W-M* 与 W-LIVE 可并行；双端 / ⊘ 项不重开。

---

## 7. 待裁决（实现前只需拍板）

1. **下一波范围**：W-M1→W-M6 默认序，还是指定单窗（如只做商城 / 只做 Enchant）？
2. **PartyHP 形态**：继续 UserList 内嵌（推荐，已对齐 round68），还是拆独立 `UIPartyHP` 悬浮窗？
3. **扩展矩阵**：300 级 / 长伤害 / 签到双端等 ijl15 项继续 ⊘ 不立项？（默认：是）
4. **搁置确认**：多端 SDL 保留、MinMS 引擎/公会/资源不移植、双端阻塞不重开 —— 按默认执行？

> 无待决回复「按推荐默认开 W-M1」即可进入实现。

---

## 附录 A：三端量级对照（现场统计）

| 维度 | MapleStory-Client | MinMS | HeavenClient |
| --- | ---: | ---: | ---: |
| UITypes `.h` | ~93 + MiscShells~60 | ~43 | ~39 |
| PacketSwitch `emplace` | ~305 | ~119 | 更少 |
| c2s 出站 | 148/148 | ~83 | — |
| 典型深窗 | CashShop ~1165 行 | CashShop ~3440 行 / NpcTalk ~4442 行 | 薄 |
| 工程形态 | Win MSVC + SDL3 + s9 | C++17 + GLFW/JNI + `.img` | 经典 Win Journey |
