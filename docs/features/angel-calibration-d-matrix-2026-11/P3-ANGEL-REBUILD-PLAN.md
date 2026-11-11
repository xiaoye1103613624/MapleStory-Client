# P3（16 条）按 Angel 083 全部重做 · 一次性 Plan

> 触发：用户 m12009「P3（16）按 Angel 全部重做」。
> 口径：round94 的 P3=16 是「按当时证据判定保持现状」，**本轮推翻该判定**——
> 全部 16 条重走 083 取证，凡 083 有据且零 wire 的一律改，凡证伪的改文档，
> 凡属架构差异/资源缺失的登记不发明。分流用 A/B/C 表示取证成熟度，**不是**结论。

## 0. 三条硬规则（沿用 round90/91/92 教训）

1. 登记前必须四证齐全：083 反编译（活 IDB 13339）+ s9 handler/源码 + 当前端字面量 + `DECISIONS.md` 原文。
   缺任何一项只登记不实施，绝不从矩阵文本推导。
2. 一片一原子 commit；W 桶 6 字段宽度/类型与 27 条 wire KEEP 全程零改动。
3. 每片验收：harness 静态守卫 + 场景、`gen_opcode_matrix.py --check` GATE、
   真重编三证（touch + build_log size/mtime + EXE mtime + 覆盖数）、零署名提交。

## 1. 分批

| 批 | 条数 | 定义 |
| --- | --- | --- |
| A | 9 | 083 证据已在库或 ICC 单点可补；零 wire；客户端本地可自测 |
| B | 5 | 需补 083 反编译或 IMG 节点后才能定性；可能有行为级改动 |
| C | 2 | 已有 083 证否/联机优先结论；重取证后大概率维持 |

合计 `9 + 5 + 2 = 16`。

## 2. 逐条（16）

| # | 批 | 决策点 | 议题 | 083 证据现状 | 本批动作 |
| --- | --- | --- | --- | --- | --- |
| 1 | A | `login-flow/D12` | 逐字节核对文档 | 已有：`login-flow-angel-calibration-2026-11/README.md` C 表逐字节对照 | 补 083 `CLogin`/`GW_CharacterStat` 逐字节表进产物 |
| 2 | A | `map/D1` | `SaveFuncKeyMap` 89 键差分 | 需补：083 `CFuncKeyMappedMan::SaveFuncKeyMap` 反编译 | 先出 89 键 vs s9 全量差分表，不改协议 |
| 3 | A | `map/D8` | `CHANGE_QUICKSLOT` 截断 | 需补：083 `CQuickslotKeyMappedMan` 与 s9 `QuickslotKeyMappedModifiedHandler` 对拍 | 出对照后只改客户端本地映射 |
| 4 | A | `skill/D11` | BT_HYPER / GUILDSKILL / RIDE 三空钮 | 已有：083 三钮 `return NORMAL` 立即数（corewindow README:95） | 补 DISABLED 态与提示，不新增窗 |
| 5 | A | `cashshop/D2` | 是否拆 7 子窗 | 已有：083 `CCCashShop` 66 方法 / 7 子窗 | 出「本端合并壳 ↔ 083 七子窗」逐项对照，不拆窗 |
| 6 | A | `corewindow/D10` | `CWorldMapDlg` 666×524 | 已有：083 立即数 | 按立即数校版本端尺寸/侧栏 |
| 7 | A | `map/D4` | `UIWorldMap` 侧栏 | 需补：IMG `UIWindow.img/WorldMap` 节点（orange-wz 128B） | 先枚举节点再定位侧栏 |
| 8 | A | `statusbar-hud/D1` | 单体 `CUIStatusBar` → 10+2 类拆分 | 已有：083 `CUIStatusBar` 类族符号 | 出类族↔本端单文件映射表，不拆文件 |
| 9 | A | `social-economy/D3` | 联盟双入口 | 已有：083 `CTabGuildAlliance` / `CUIAlliance` | 出双入口对照，保留本端入口 |
| 10 | B | `skill/D9` | classic 双布局 | 需补：IMG `Skill.img` 双布局节点 + 083 `CUISkill::OnCreate` | 出双布局判定依据后校版本端布局 |
| 11 | B | `social-economy/D12` | wedding 26 键英文 | 需补：IMG `UIWindow.img/Wedding*` + Locale 键 | 补 zh-CN/en-US 键值，不改逻辑 |
| 12 | B | `pet-summon/D2` | 两处字节丢弃 | 需补：083 `CDragon`/`CPet` Decode 反编译 | 证伪/证实后决定是否补读 |
| 13 | B | `login-flow/D13` | CHARLIST 固定 15 槽 | 需补：083 `CLogin::SendSelectCharacter` / s9 `getCharList` 对拍 | 出槽数对照，改则只改本地槽数组 |
| 14 | B | `social-economy/D9` | `UIFriendGroup`/公会徽章/MapleTV 三整族 | 需补：083 三族符号 + IMG 节点 | 出族级对照；缺资源则登记不发明 |
| 15 | C | `corewindow/D2` | 083 原生 vs s9 冲突 | 已有：`DECISIONS.md` s9 联机优先 | 重取证后确认维持 s9 |
| 16 | C | `quest/D5` | `UIQuestLog::send_key` | 已有：083 全库 `*Quest*OnKey*` / `*Quest*SendKey*` total 0 | 维持不补（无 083 依据即不发明） |

## 3. 执行顺序

1. ~~**批 A 前 3 条**（login-flow/D12、skill/D11、corewindow/D10）—— 证据全在库，直接出对照 + 补丁。~~ **round96 已完成**（见 §6）。
2. ~~批 A 余 6 条 —— 每条补一个 IDA/IMG 探针后落。~~ **round97 已完成**（见 §7，六条全 DOC_ONLY）。
3. 批 B 5 条 —— 每条先出 083 反编译摘录，能证伪本端形态才动代码。
4. 批 C 2 条 —— 重取证，预期维持，仅补文档。

## 4. 算术不变

本条 Plan **不重分类**：168 = PURE 76 + WIRE 38 + MIXED 14 + OTHER 40 照旧；
reopen 27 = 已注销 10 + 残余 17 照旧；残余 17 = P1 0 + P2 1 + P3 16 照旧。
本轮只把 P3 的 16 条从「保持现状」改成「按 083 重做」，属登记层动作。

## 5. 与 login-flow/D3 的关系

`login-flow/D3` 原属残余 17 的 **P2（1 条）**，已在 round95 用 s9 源码直接对协议解掉，
故 P2 归零、P3=16 不变，合计 17 仍闭合。
## 6. round96 回执（A 批前 3 条）

| 条 | 结果 | 产出 |
| --- | --- | --- |
| `login-flow/D12` | `DOC_ONLY` 完成 | `docs/features/angel-calibration-d-matrix-2026-11/BYTE-LEVEL-STATS.md`（26 字段三维对照） |
| `skill/D11` | 定性：三钮**非 083** | `IO/UITypes/UISkillBook.cpp` 纯注释（083 `Skill` 无 `main`、`OnCreate` 按钮 id 实证） |
| `corewindow/D10` | 定性：083 硬编码 666×524，本端 654×521 属贴图推导 | `IO/UITypes/UIWorldMap.cpp` 纯注释 + PLT §55-3 真机对齐 |

三条均零 wire / 零行为改动；A 批余 6 条待续。

## 7. round97 回执（A 批余 6 条）

| 条 | 结果 | 产出 |
| --- | --- | --- |
| `map/D1` | `DOC_ONLY`：083 89 槽 × 5B 差分 vs s9 mode0 稀疏 map，本端已同形 | `P3-A-BATCH-REST-MAPS.md` §1 |
| `map/D8` | `DOC_ONLY`：083 `183 + 0x20` raw vs s9 严格 32B，本端 8×`write_int` 字节级等价 | 同上 §2 |
| `cashshop/D2` | 架构差异：083 `CCSWnd_*` 7 类 / 70 符号 vs 本端单类，不拆窗 | 同上 §3 |
| `map/D4` | 资源缺失 + 架构差异：`WorldMap` 仅 `title`+`Border`，`WorldMapSearch` 不存在 | 同上 §4 |
| `statusbar-hud/D1` | 架构差异：45 方法簇 `8/7/1/8/14/7` 全部有本端归属，不合并 | 同上 §5 |
| `social-economy/D3` | 保持现状：083 `CUIAlliance` 0 符号，入口是共用窗按钮 | 同上 §6 |

六条均**零 wire / 零行为改动**；A 批 9 条（前 3 + 余 6）完成，B/C 批 7 条待续。
