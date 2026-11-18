# 192 客户端素材 / 万能登录器 IDA / 095 端 — 综合再分析

> 日期：2026-11-18　输入：用户附图（orzRepacker v1.16.20 解析 192 `UINew7.wz\UIWindowCN.img\CS_S0100` 魔女森林活动 UI，803×713 ARGB4444）+ 万能登录器 IDA(13339) + 前面官方公式/095 核实
> 本轮新证据均已实测，不是推测。

---

## 1. 关键结论速览

| 新证据 | 结论 |
|---|---|
| 192 UI 能用 orz 新版本密钥解析 | ✅ **实测确认可用**，且已打通 orange-wz MCP 自动化路径。**UI 素材可导出移植**，但"能导出"≠"该移植"，要看消费点（见 §3） |
| 万能登录器 IDA(13339) | ❌ **VMProtect 加壳**（.vmps0/.vmps1 共 14MB），IDA 182 函数里 149 个 nullsub、字符串仅 583 条无业务逻辑。**对当前伤害/属性/套装系统零参考价值** |
| 095 端（上轮核实） | 潜能有底层，星之力/强化是脚本私服，附加潜能/火花没有，高版本伤害属性未接入 |
| 对 Phase1/2（已写的代码） | **无影响**：192 是活动 UI 素材、万能登录器是登录外壳，都不碰伤害公式。官方公式的 2 个校正点（无视防御连乘、暴击区间）仍需按 `OFFICIAL-FORMULAS.md §5` 补 |

---

## 2. 192 资源解析 — 实测打通（含可用 key）

### 2.1 资源盘点
- 目录：`E:\MXD\【2023.8.8江浩生日福利】怀旧岛192整合版\怀旧岛192客户端\冒险岛online192\Data`
- 命名空间：Base/Character/Effect/Etc/Item/Map/Mob/Morph/Npc/Quest/Reactor/Skill/Sound/String/TamingMob/UI（16 个，459 文件）
- UI：`UI_000.wz` ~ `UI_008.wz`（共约 630MB），GUI 里显示为逻辑名（UINew7 等），磁盘上是编号 wz。

### 2.2 "新版本客户端" key（重要，可直接复用）

来源：`E:\project\orange-wz\src\main\java\orange\wz\provider\WzKeyStorage.java:51`
```java
keys.add(new WzKey(3, "新版本客户端", WZ_LATEST_IV, DEFAULT_KEY));
```
- `WZ_LATEST_IV = {0,0,0,0}`（`WzAESConstant.java:10`）→ base64 `AAAAAA==`
- `DEFAULT_KEY` = 128B GMS key → base64（同 `tools/_gms_key_128.b64`）：
  `EwAAAFIAAAAqAAAAWwAAAAgAAAACAAAAEAAAAGAAAAAGAAAAAgAAAEMAAAAPAAAAtAAAAEsAAAA1AAAABQAAABsAAAAKAAAAXwAAAAkAAAAPAAAAUAAAAAwAAAAbAAAAMwAAAFUAAAABAAAACQAAAFIAAADeAAAAxwAAAB4AAAA=`

**踩坑**：一开始我用 GMS IV（`TSPHKw==`）加载 192 wz，能 load 但节点名全是乱码（`list_children` 返回乱码 IMAGE）——**真因是 IV 不对，不是 key 错**。换成零 IV 后：
- `UI_007.wz` → 正确列出 UIWindowEvent/UIWindowGL/UIWindowJP/UIWindowPL/UIWindowString/UIWindowTW/UIWorldGauge/Urus 等 img
- `UI_006.wz` → UIWindow7/8/BT/CN/CT img；`UIWindowCN.img` 下有 Jiaozi_minigame/Incubator_CN/CN_Web/CN_PicturePlate/CN_Limitbreakthrough/CN_50100 等节点

这推翻了旧记忆里"192 用私服自定义 key 全解不开"的结论——**用对 IV 就能解**。

### 2.3 orange-wz MCP 调用模板（后续移植直接套）
```
load_files(
  paths=[wz绝对路径],
  key={name:"新版本客户端", ivBase64:"AAAAAA==", userKeyBase64:<上面128B>},
  exclusive:false)
→ query_nodes(op:"tree"/"detail", autoParse:true, includePng:true) 导出
→ unload_all(hintGc:true)
```
注意：`list_children` 对某些 wz 返回空，但 `query_nodes(op:"tree", autoParse:true)` 能正确列节点——**用 query_nodes 而非 list_children**。

---

## 3. UI 素材"能不能移植" — 分层判断

"能导出 PNG"只是技术前提。是否值得移植，按消费点分三类：

### 3.1 可直接移植（有消费点、风格兼容）
- **装备/技能/怪物图标**：当前客户端走 `nl::nx::Item/Skill/Mob`，192 的这些 ns 资源若风格与 v83 一致，可替换导出 PNG。但要注意 192 图标是高版本画风，混用可能违和。
- **通用 UI 控件**（按钮/边框/滚动条）：`Basic.img/BtXXX` 类，若当前端缺某按钮素材可从 192 借。

### 3.2 可作为参考但不能直接搬（无消费点 / 风格冲突）
- **截图里的 `CS_S0100`（魔女森林活动 UI）**：当前客户端源码里 `CS_S0100`/`魔女森林` **零引用**，也没有活动 UI 框架消费它。移植过来只是一张 PNG，没有逻辑承载 → **除非要新做活动系统，否则不移植**。
- **高版本专属面板**（星之力/潜能/火花/超级属性 UI）：192 有完整素材，但当前项目 UIStatsLayoutV4 是 v83 双栏布局，直接搬高版本素材会风格断裂。若 Phase3 决定做高版本强化 UI，这些是**素材来源**，需重新排版适配。

### 3.3 不能移植
- 涉及 192 服务端协议/加密的素材（万能登录器、登录器皮肤）——与当前 s9 协议无关。

**一句话**：192 素材库是"素材宝库"，但移植要按当前项目的消费点来，不是看到好看的就搬。当前 Phase1/2 是伤害公式，完全不需要 UI 素材。

---

## 4. 万能登录器 IDA(13339) — 逆向结论

通过 `C:\Users\11036\.rea\ida13339_call.ps1`（新建的直连脚本）连上你打开的 IDA：

| 项 | 值 |
|---|---|
| 模块 | 万能登录器.exe（VMProtect 加壳） |
| 基址/镜像 | 0x400000 / 0x11b8000 |
| 段 | `.text` 0x401000-0x4cf000（848KB）；`.vmps0` 5.8MB + `.vmps1` ~9.4MB（**加壳段占 90%**） |
| 函数 | 182 个，其中 **149 个 nullsub**、33 个真实函数；字符串 583 条全是 DLL 导入名 |
| 字符串搜索 | `登录|密码|账号|server|login|8484|maple` → 仅匹配 1 个 API 名 `CreateDialogIndirectParamA`，无任何业务字符串 |

**结论**：这是被 VMProtect 虚拟化的登录器外壳，真实逻辑在 VM 字节码里，IDA 静态几乎无法还原。它跟我们要做的伤害公式/属性系统/套装**没有任何关系**（登录器只负责启动/打补丁/传账号）。如果目标是分析 192 的登录协议，动态抓包（Wireshark/Fiddler）比逆向这个壳有效得多。**建议本轮不从它取证。**

---

## 5. 对开发计划的更新（叠加到 ROADMAP）

| Phase | 192 素材/IDA 证据的影响 |
|---|---|
| Phase 0 真机验收 | 无影响，照常做 |
| Phase 1 核心层 | 无影响 |
| Phase 2 伤害公式 | 无影响；官方公式 2 个校正点仍待补（`OFFICIAL-FORMULAS.md §5`） |
| Phase 3 强化 UI | **192 素材可作来源**：星之力/潜能/火花面板背景从 `UI_006/007.wz` 导出，但要按 v83 风格重新排版，且需新增消费点（UI 类） |
| Phase 4 强化系统 | 192 端只有潜能底层（无星之力官方/无火花），不能作为数值权威；数值以 BWIKI 官方公式为准 |

**新增工具能力**：`OFFICIAL-FORMULAS.md` 的所有官方数值 + 192 素材库（可导出）+ 095 潜能字段雏形，三者凑齐了"做高版本强化系统"的全部输入。但当前最优先仍是 Phase 0 真机验收 + Phase 2 的 2 个公式校正点。

---

## 6. 待用户确认

1. **192 素材用途**：你要移植的是哪类素材？①装备/图标散件 ②高版本强化系统面板背景（为 Phase3 做准备）③截图里的魔女森林活动 UI（需新做活动系统）。不同用途工作量和适配策略完全不同。
2. **是否现在校正公式**：官方资料已暴露无视防御（连乘 vs 我写的加法 cap）和暴击（区间 vs 固定 1.35）两处偏差，是否先补这两个校正点再真机验收？
3. **万能登录器**：你说打开它，是想分析 192 的登录协议吗？如果是，我建议改走动态抓包（这个 VMProtect 壳静态无解）。

---

## 7. 第二轮补查（用户追问：左侧菜单 / 192 服务端 / 三类素材 / 公式补完）

### 7.1 192 服务端 — ✅ 可获取、可反编译（比预想好很多）
- 路径：`E:\MXD\【2023.8.8江浩生日福利】怀旧岛192整合版\怀旧岛192服务端\MapleServer192`
- **`Server.jar`（5.6MB，config/lib/ 下）未混淆**，1491 个 class，包名可读（`client/`、`server/`、`configs/`），Kotlin(`*.kt`)+Java 混合。
- `MapleServer192.7z`（481MB）内含 `冒险岛代码文本版_192.1\`（WZ 数据的 .txt 文本导出，Equip/Skill/Mob 等，**不是 Java 源码**）。
- 可用 `C:\Users\11036\.jdks\ms-21.0.12.1\bin\javap.exe -classpath Server.jar -p <类>` 直接看方法签名/字段（无需反编译全文）。

关键类 javap 实测（高版本系统参照价值）：
| 类 | 关键字段（实测） | 对我们的价值 |
|---|---|---|
| `client.inventory.NirvanaFlame`（火花） | `nbossDamage/nignorePDR/ntotalDamage/nallStat/nwatk/nmatk` 等 19 项短字段 | 印证官方火花战斗属性字段命名（§3.4 BWIKI），可作 Provider 字段参照 |
| `server.StructSetItemStat`（套装） | `incMHPr/incMMPr/incPAD/incMAD/incAllStat/incPDD` + option1/2/3 + skillId | **与 s9 `SetBonus` 几乎同构**，印证我们套装方向正确 |
| `client.inventory.ModifyItemPotential`（潜能） | `newItem/csItemId/getPosition/getEquipOnlyId` | 潜能是"应用结果"封装，非属性定义 |
| `configs.StarForceConfig` | `START_DOWN/CURSE_REDUCE_COUNT/BONUS_TIME` | 星之力规则配置 |
| `server.StructSetItem` | `setItemStat/itemIDs/completeCount/setItemName` | 套装数据结构 |

**结论**：192 服务端是"高版本官方做法"的活证据，字段命名/结构可直接参照；但**不需要整体移植**——我们已经用 BWIKI 官方公式 + xy CombatStat 核心层覆盖了它的设计。`NirvanaFlame`/`StructSetItemStat` 可在 Phase4 做火花/强化 Provider 时对照。

### 7.2 192 左侧图标菜单（每日/任务/魔女抽奖）
- 客户端侧：192 有标准 `Shortcut`（在 UI_000.wz 等，如 `CashShop.img/PicturePlate/BtShortcut`）+ 大量高版本功能 img（`CNSupportSystem.img`、`EventShop.img`、`GuildUI2.img` 等）。左侧快捷栏是高版本标配。
- 服务端侧功能脚本（`MapleServer192\scripts\extend\`）：
  - `功能_日常任务.js`、`功能_任务奖励.js`、`功能_寻宝任务.js`、`功能_跑环任务.js` → 对应"每日/任务"类入口
  - `scripts\event\Other_OXEvent.js` 等 → 活动类
- **功能可移植性判断**：左侧菜单 = UI 图标 + 服务端脚本 + 客户端 UI 逻辑三件套。当前 s9 客户端（v83）没有这套框架，移植等于**新做一个功能系统**：
  - 简单入口（挂 URL/公告）→ 低成本
  - 真·日常任务/抽奖系统 → 需要完整任务系统 + 奖励发放 + 防重复，工作量大，且与当前 Phase1-4（伤害/强化）不同方向
  - **建议**：作为独立 feature 单独立项，不塞进当前伤害/属性项目。若只是要图标样式，导出 PNG 即可。

### 7.3 三类素材的最终决策
| 类别 | 决策 | 依据 |
|---|---|---|
| 散件图标（装备/技能/按钮） | ✅ 可导出借用，按需 | 有消费点，走 `nl::nx` 替换即可；注意画风与 v83 的一致性 |
| 强化面板背景（Phase3 备料） | ✅ 导出归档备用，本轮不做 UI | `UI_006/007.wz` 的星之力/潜能/火花面板，Phase3 做强化 UI 时按 v83 风格重排使用 |
| 魔女森林活动 UI（CS_S0100） | ❌ 暂不移植 | 当前端无活动 UI 框架、零引用，除非立"活动系统"专项 |
用户已确认这三类都评估，结论按上表执行（归档备料，不立即改 UI）。

### 7.4 公式补完 — ✅ 已完成（本轮实做）
按 `OFFICIAL-FORMULAS.md §5` 的两处校正，双端已改并验证：

**服务端 s9**：
- `CombatStatType`：新增 `StackRule.MULTIPLICATIVE_IGNORE`；`IGNORE_PDR/IGNORE_MDR` 从 `ADDITIVE_CAP` 改为连乘。
- `CombatStatResolver`：新增 `computeIgnore()` = `100-100×Π(1-vi/100)`，无视防御来源按官方连乘。
- `DamageCalculator`：暴击 `critMultiplier` 用区间中点 `1.35+cd`，`maxCritMultiplier`（反作弊上限）用区间上界 `1.5+cd`。
- 单测 `CombatStatResolverTest`：`ignoreDefenseMultipliesSources`（90+60→96、50×3→88）、`critMultiplierUsesOfficialRange`（中点1.65/上界1.80），全绿。
- 产物：`BeiDou.jar` 11:24:21（117MB）。

**客户端**：
- `Attack.h`：暴击区间默认 1.2~1.5（官方无暴伤装备基准）。
- `Player.cpp`：`critical_min/max = 1.0 + mincrit/maxcrit`（CharStats 的 mincrit/maxcrit 是暴伤%小数，修正了上一轮误写的 1.35+x）；无视防御以 0x175 服务端连乘值为权威（`synced` 时），否则回退本端累加。
- `next_damage` 已有区间随机逻辑（low~high），无需改。
- 产物：`MapleStory.exe` SHA `6d80e644` 11:30:03。

**验证**：服务端单测全绿、`mvn package` EXITCODE=0；客户端 build_smoke EXITCODE=0（opcode GATE OK）。**待真机验收**（用户 Phase 0）。

### 7.5 更新后的待办
1. **真机验收**（Phase 0）：新 jar(11:24) 重启服务端 + 新 exe(SHA 6d80e644)，验伤害数值 + 暴击区间 + 无视防御 + 无误封。
2. 传闻素材：散件/强化背景已归档可导出，魔女森林暂缓。
3. 左侧菜单/活动系统：建议独立立项，不并入当前项目。
4. 192 服务端 javap 分析：`NirvanaFlame`/`StructSetItemStat` 留作 Phase4 火花/强化 Provider 字段参照。

### 7.6 真机首报 NoSuchMethodError 的根因与修复（2026-11-18）
用户选角进游戏报错：`NoSuchMethodError: void org.gms.client.Character.sendCombatPanelStats()`（PlayerLoggedinHandler:246）。
- **根因**：IDEA 增量编译的中间态不一致——`PlayerLoggedinHandler.class`（11:24:04）先编译并调用了新方法，而 `Character.class` 当时还是旧版（无该方法）。用户 IDEA 用 `-classpath target/classes` 直接跑（非 launch.bat 的 fat jar），加载到不一致的 class → NoSuchMethodError。
- **排查坑**：`javap -classpath BeiDou.jar <类>` 读 Spring Boot fat jar 会**误报"方法不存在"**（类在 `BOOT-INF/classes/`，javap classpath 无法索引）。正确做法：用 python zipfile 读出 `BOOT-INF/classes/...class` 到临时文件再 `javap -p <file>`。
- **修复**：`mvn clean package -DskipTests` 全量重编，jar(12:19:05) 与 target/classes(12:18:56) 的 Character/Handler 均一致（实测 javap 确认）。
- **结论**：这是构建/运行态问题，非代码逻辑错误。**用户需重启 IDEA 运行（或 launch.bat）加载新编译类**即可进游戏。以后双端同步改类时，避免只增量编译部分文件——改完执行 clean 或确保全量重编后再跑。
