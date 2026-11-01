# r15 · 基础 UI 对照正确客户端截图：差距诊断与 REA 分段计划

> 日期：2026-10-22  
> 目标原生：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe`（**非** ijl15 / `自定义功能实现`）  
> 当前端：`E:\project\MapleStory-Client`（部署常在 `E:\MXD\BeiDou-Client-xy`）  
> 正确截图目录：`C:\Users\11036\Desktop\正确客户端截图`  
> 证据目录：[`evidence/r15/`](evidence/r15/)  
> 本阶段：**调研/文档**，不落业务大改；建议修复项待用户确认 Plan 后再实现。

---

## 0. 必须回答的三个结论（先读）

### 0.1 BeiDou.exe 是否已「全部反编译分析完」？

**没有。**

| 维度 | 现状 | 证据 |
| --- | --- | --- |
| 函数总量 | IDA survey：**55654** 函数（命名仅 ~510，未命名 ~54615） | `evidence/ida_survey_binary_parsed.json` |
| 字符串 | ~**3889** 条 | 同上 |
| r14 catalog | **功能面清单**（`UIWindow.img` 112 顶层节点 + 字符串锚点 + s9 Handler 三角） | [`r14-native-feature-catalog.md`](r14-native-feature-catalog.md) |
| 已做 REA | 定点 `search` / 少量 `decompile`（Maker、StatusBar CreateCtrl 等）；**禁止**整包 `rea analyze`（idalib 超时） | r14 §0；本目录 `evidence/r15/` |
| 缺什么 | 登录主链各 `CUI*`/`sub_*` 构造函数、WorldSelect/CharSelect/StatusBar 的完整伪代码与控件表、进图后 HUD 点击/快捷键分发、背包/装备/… 后续批次 | 见 §6 |

**覆盖了什么：** 资源能力面（112 节点）、社交经济 P0.5 接线、若干原生窗锚点（Maker/Quest/UserList/…）、StatusBar CreateCtrl 坐标表（本轮补证）。  
**没覆盖什么：** 55654 函数全量反编译、登录→选频→选角→底栏主循环的逐函数对照闭环。

### 0.2 用户报告的基础问题根因是「REA 没反编译全」还是「对照/实现有问题」？

**主要是「对照/实现未闭环 / 接线与条件过滤有问题」，不是「没把 exe 全部反编译完」。**

| 证据类型 | 说明 |
| --- | --- |
| 资源 | r00：两端 `Data/UI/*.img` SHA256 **全同** → 登录/选频/底栏「不像」**不是缺资源** |
| 代码已有对照痕迹 | `UILogin`/`LoginBackground`/`UIWorldSelect`/`UICharSelect`/`UIStatusBar::init_classic` 均含 BeiDou/IDA 注释与硬编码偏移 → 问题在**对照未验收闭环**，而非「零证据」 |
| 可定位实现缺陷 | 见 §3：`channel_count < 2` 跳过世界、选角排行未绘、底栏快捷格**无点击分发**、仍留 `[DBG-CLK]` 等 |
| REA 角色 | 全量反编译**既不必要也不现实**；完美复原只需**按主链分段定点 decompile**（§6）。仅当某控件表/偏移对不齐时，才对该 `sub_*` 补证据 |

例外：个别坐标/控件 id 仍需 REA 精校（例如 StatusBar `sub_8D01B2` 控件 id 与注释中的 2004/2005 编号不完全一致——坐标已对齐，见 §3.5）。

### 0.3 Wave 建议（请用户拍板）

**建议先修登录 / 频道 / 选角 / 底栏基础 UI（本文件 P0–P1），再继续 Wave A 社交内容（Messenger/PartySearch/…）。**  
理由：主循环进不去或进图后 HUD 不可用，Wave A 无法实机验收。

---

## 1. 正确客户端截图清单（vision 语义）

| # | 文件 | 画面语义（正确端应有） |
| --- | --- | --- |
| 1 | `登录界面.png` | 书本外框 + 森林背景；木牌登录板；账号/密码；「连接」；保存账号/密码修改/找回密码；新的帐户/网站/结束；Ver. 0.83；BeiDouMS/枫叶 branding |
| 2 | `选择频道页.png` | 同书本框+森林；左栏「选择频道」+ 查看世界地图/所有角色/回到首页；中央绳挂木牌，**20 块竖牌**（首牌「1 蓝蜗牛」） |
| 3 | `选中频道页.png` | 点世界后展开羊皮卷：公告、蓝蜗牛标题、**CH.1–CH.20** 网格（CH.1 选中圈）、负载条、「到选择的世界去」 |
| 4 | `选择角色页.png` | 书本+桥梁槽位；选中角色头顶 **charInfo 羊皮属性板**（职业/等级/四维/人气 + 排行区「排行不可用」）；右木牌 开始/创建/删除；Tab 提示泡 |
| 5 | `进入游戏页.png` | 新手村；操作说明对话；底栏 Lv/HP/MP/EXP；**商城/帮助/目录/界面**；右侧快捷键格；聊天/小地图/任务告知 |
| 6 | `打开了S-E-I-K页面.png` | 同时打开 角色能力 / 装备 / 技能 / 道具（S-E-I-K）；底栏与快捷格仍在 |
| 7 | `键盘设置.png` | 经典键盘设定窗；E=装备、I=道具、K=技能、Q=任务、W=大地图…（与 v83/BeiDou 默认表一致） |

---

## 2. 与已有文档的关系

| 文档 | 作用 | 与本轮关系 |
| --- | --- | --- |
| [`r14-native-feature-catalog.md`](r14-native-feature-catalog.md) | 原生功能面 catalog | **不是**全量反编译；本轮确认其边界 |
| [`r00-baseline-parity.md`](r00-baseline-parity.md) | 资源哈希一致、早期能力缺口 | 资源无缺口 → UI bug 归实现 |
| [`r05-ui-layout.md`](r05-ui-layout.md) | 进图后 Stat/Equip/Skill/Item **四窗格线** | 已修进图四窗坐标；**不等价于**登录链/底栏点击 |
| [`r11-baseline-replication.md`](r11-baseline-replication.md) | 仓库/组队/好友等 P0.5 | Wave A 前置；本轮基础 UI 应优先 |
| 本文件 r15 | 截图对照 + 根因分类 + 分段 REA | **交付物** |

---

## 3. 逐项 Diff（正确截图 × 当前实现）

根因分类标签：`资源` / `坐标` / `协议` / `未接线` / `REA证据不足` / `对照未闭环`

### 3.1 登录界面背景 UI

| 项 | 正确端 | 当前端 | 根因分类 | 说明 |
| --- | --- | --- | --- | --- |
| 书本外框+森林 | 有 | `LoginBackground` + `Login.img/Common/frame` + MapLogin | 对照未闭环 / 坐标 | 代码路径已存在（`IO/LoginBackground.cpp`，step0 偏移 `{137,-20}`）；需实机与截图叠图验收相机/开孔 |
| 木牌登录板 | Obj `login.img/Title/signboard` | `UILogin` bedou 分支按告示牌推面板原点 | 坐标 | 注释称对齐 CUITitle；若牌位偏，重测 `signboard` origin + `button_pos` |
| BeiDouMS Logo / Ver.0.83 | 有 | `version` 节点 + Title 资源 | 对照未闭环 | 资源应在；若缺标多为绘制层/缩放 |
| 加载前黑底 | — | `ColorBox` 黑底兜底 | 对照未闭环 | `LoginBackground::available()` 为 false 时会退化 |

**建议修复项：** 实机叠图验收 step0；确认部署端 `MapLogin`/`Login.img` 与 V17.7 一致；偏差则只调 `LoginBackground::target_offset(0)` 与面板相对告示牌偏移。

### 3.2 登录后的加载弹窗 UI

| 项 | 正确端 | 当前端 | 根因分类 | 说明 |
| --- | --- | --- | --- | --- |
| Loading 卷轴 | `Login.img/Notice/Loading` | `UILoginWait`：bedou 只用 `Loading/backgrnd`，避免叠 `Notice/backgrnd` | 对照未闭环 / 坐标 | 代码已防双弹窗；`kBeiDouBarOff(46,98)` 需与正确端截图对 bar 槽 |
| 取消键 | 有 | 放在卷轴下方 | 坐标 | 若压住 LOADING 字需微调 |

**建议修复项：** 抓一张当前端 Loading 与正确端并排；只改 bar/取消偏移，不扩协议。

### 3.3 选择频道页：背景、频道缺失、选不了

| 项 | 正确端 | 当前端 | 根因分类 | 说明 |
| --- | --- | --- | --- | --- |
| 背景书本+森林 | step1 视角 | `LoginBackground::set_step(1)` | 对照未闭环 / 坐标 | 与登录同套背景；偏移 `{28, step_height}` |
| 20 世界竖牌 + 展开 CH.1–20 | 有 | `UIWorldSelect`：`CHANNEL_BUTTON_COUNT=30`，网格布局 | 协议 / 未接线 / 对照 | **硬缺陷：** `draw_world()` 中 `if (world.channel_count < 2) continue;` —— 单频道世界整块被跳过，表现为「世界/频道缺失」 |
| 频道可选 | 两步：点世界→点频道→再点或「到选择的世界去」 | `channelid` 默认 `0xFF`，二次点击同频道 `enter_world` | 对照未闭环 | 逻辑合理；若按钮未 `set_active` 或命中区错则「选不了」 |
| 负载条 | 部分 CH 有蓝块 | `channel_gauge` + `channel_capacities` | 协议 | `change_world` 用 `min(count, capacities.size(), 30)`；解析见 `LoginParser::parse_world` |

**代码锚点：**

```697:698:IO/UITypes/UIWorldSelect.cpp
			if (world.channel_count < 2)
				continue;
```

**建议修复项（P0）：** 将过滤改为 `channel_count < 1`（或 `== 0`）；联调确认 s9 下发的 `channelload_size`；实机验证 20 格与「到选择的世界去」。背景偏差单独叠图修 step1 偏移。

### 3.4 选择角色页：选中后任务/属性 UI

| 项 | 正确端 | 当前端 | 根因分类 | 说明 |
| --- | --- | --- | --- | --- |
| 属性羊皮板 | `charInfo1`：职业/等级/四维/人气 | `UICharSelect` 绘 `charInfo` + `CS_DETAIL_*` 硬编码 | 坐标 / 对照未闭环 | 无 NX `detailPos` 时走常量；需与截图像素对齐 |
| 排行区 | 「服务器排行榜 / 排行不可用」 | **未绘制** `stats.rank` / `jobrank` 文案 | 未接线 | `StatsEntry` 已解析 rank（`LoginParser::parse_charentry`），`UICharSelect` 无输出 |
| 「任务」观感 | 截图实为属性板+Tab 提示，非 QuestLog | 若错窗/错贴图会被误认为任务 UI | 对照未闭环 | 先确认是否画了错误节点或偏移把板子画成对话框 |

**建议修复项（P1）：** 补排行文案（无排行时固定「排行不可用」）；用正确端截图校准 `CS_CHARINFO_OFF` 与各 `CS_DETAIL_*`；REA 定点 CharSelect 构造（见 §6 Batch C）。

### 3.5 进游戏后底部信息栏按钮、快捷键点击

| 项 | 正确端 | 当前端 | 根因分类 | 说明 |
| --- | --- | --- | --- | --- |
| 商城/帮助/目录/界面 | BtShop/BtNPT/BtMenu/BtShort | `init_classic` 已按 IDA CreateCtrl 坐标建钮并在 `button_pressed` 接线 | 对照未闭环 / 未接线 | 仍有 `[DBG-CLK]` 写 `_clk.log` → 点击链路曾不稳定，需实机确认是否进回调 |
| Equip/Inven/Stat/Skill 小键 | 有 | `BT_CLASSIC_EQUIP/INV/STAT/SKILL` → emplace 对应 UI | 对照未闭环 | 与截图「打开了 S-E-I-K」一致的目标路径 |
| 右侧快捷格点击 | 格可点开绑定功能 | **仅绘制** `classic_qs_icons`（且只认 SKILL/ITEM）；**无槽位 hit→send_key** | 未接线 | 用户「快捷键点击功能缺失」的最直接代码证据 |
| 键盘 S/E/I/K | 正确端键盘设定图：E 装备 / I 道具 / K 技能等 | `UIStateGame` MENU 分支已实现；默认表 `basic_keys` | 对照未闭环 | 若存档 keymap 为空/旧 alternate，会表现为「按键无效」；应强制 classic 默认或首次导入 `basic_keys` |
| CreateCtrl 证据 | — | `sub_8D01B2`（size `0x392d`） | REA 部分补齐 | 坐标：`evidence/r15/statusbar_createctrl_coords.txt`（1000@573,543 … 2003@708,515；KeySet/QS 为 id 1005/1006/1009，与旧注释 2004–2006 **编号不同、坐标一致**） |

**建议修复项（P0–P1）：**  
1) 快捷格：为 8 槽实现点击 → `keyboard.get_maple_mapping` → 复用 `UIStateGame` 同套 MENU/SKILL/ITEM 分发；MENU 型也要画图标。  
2) 确认 `is_in_range` 与 `position_x/y` 在非 800 宽分辨率下仍盖住按钮。  
3) 删掉临时 `[DBG-CLK]`（功能闭环后）。  
4) 默认键位与正确端键盘截图对齐（`basic_keys`）。

---

## 4. 根因总表（按优先级）

| 优先级 | 问题 | 分类 | 主要文件 | 是否必须先全量 REA？ |
| --- | --- | --- | --- | --- |
| P0 | 世界因 `channel_count < 2` 被跳过 | 协议过滤/实现 | `UIWorldSelect.cpp` | 否 |
| P0 | 底栏快捷格不可点 | 未接线 | `UIStatusBar.cpp` | 否（已有 CreateCtrl 坐标） |
| P0 | 底栏四按钮点击是否进回调 | 对照未闭环 | `UIStatusBar.cpp` + 实机 `_clk.log` | 否 |
| P1 | 选角属性板坐标/排行文案 | 坐标 + 未接线 | `UICharSelect.cpp` | 定点 CharSelect 即可 |
| P1 | 登录/选频背景与 Loading bar | 坐标/对照 | `LoginBackground` / `UILogin` / `UILoginWait` | 定点 CUITitle/Wait |
| P2 | keymap 默认与正确端键盘图 | 对照未闭环 | `UIKeyConfig` / Keyboard 存档 | 否 |
| P2 | 进图四窗细调 | 坐标 | 已有 r05 | 否 |
| 后置 | Wave A 原生内容 | 功能面 | r14 | 分段 REA |

---

## 5. 本轮 REA/IDA 定点证据（`evidence/r15/`）

| 产物 | 内容 |
| --- | --- |
| `rea_search_StatusBar.txt` / `rea_search_UIStatusBar_path.txt` | 命中 `UI/StatusBar.img/BtNPT` @ `0xb3bab4`，`…/chatTarget` @ `0xaf72e0` |
| `ida_decompile_8D01B2.json` | StatusBar 构造 `sub_8D01B2` 伪代码（MCP 截断后 curl 取回）；抽出 CreateCtrl 坐标 |
| `statusbar_createctrl_coords.txt` | id/x/y 表（与当前 `init_classic` 网格一致） |
| `rea_decompile_DrawWorldItems_624e13.txt` | `sub_624E13` size `0x535` 存在；REA schema 报 `unreadable_output`，需再 curl MCP output |
| `r15_rea_coverage_snapshot.json` | 覆盖面快照 |
| 其它 `rea_search_*.txt` | Login/WorldSelect/CharSelect/CUITitle 等字符串检索信封（多数路径运行时拼接，字符串库不全属预期） |

**工具约束复核：** `rea doctor --provider ida` 注册健康；attached MCP `13338`；**勿** `rea analyze` 整包。

---

## 6. 分段完整分析路线图（完美复原证据面）

原则：每批 = **字符串/xref → 构造函数 decompile → 控件表/坐标落盘 → 当前端 diff → 实机截图验收**；通过再进下一批。

| Batch | 范围 | 关键锚点（已知/待补） | 验收截图 |
| --- | --- | --- | --- |
| **A 登录** | Title / Notice / Loading / TOS | CUITitle OnCreate；`Login.img/Title*`、`Notice/Loading`；`LoginBackground` step0 | `登录界面.png` + Loading 实机 |
| **B 世界/频道** | WorldSelect 竖牌、展开卷、CH 网格、GoWorld | `sub_624E13`（DrawWorldItems）；WorldSelect signboard Obj；去掉错误 `channel_count` 过滤 | `选择频道页.png` / `选中频道页.png` |
| **C 选角** | 槽位、charInfo1、排行、开始/创建/删除 | CharSelect 构造；`charInfo1` 原点；rank 字段 UI | `选择角色页.png` |
| **D 进图 HUD** | StatusBar classic、聊天条、小地图、任务告知 | `sub_8D01B2`（已部分）；快捷格点击；BtShop/NPT/Menu/Short | `进入游戏页.png` |
| **E 快捷键/四窗** | KeyConfig 默认表；S-E-I-K 开窗；四窗格线回归 | `basic_keys` vs 键盘截图；r05 四窗 | `打开了S-E-I-K页面.png` / `键盘设置.png` |
| **F 背包后链路** | Equip/Item/Skill/Stat 深度、NPC/商店 | 按 r14 节点表 | 实机清单 |
| **G Wave A 内容** | Messenger/PartySearch/Duey/Hammer/… | r14 §6 | 功能用例 |

每批证据落：`notes/evidence/r15/batch-<X>/`（decompile json、控件表、当前端 vs 正确端截图对）。

---

## 7. 建议修复优先级（实现留给 Plan）

1. **P0 频道过滤** — `channel_count < 2` → `< 1`  
2. **P0 底栏快捷格点击 + 确认四按钮回调**  
3. **P1 选角 charInfo 坐标 + 排行文案**  
4. **P1 登录/选频背景与 Loading 叠图微调**  
5. **P2 classic 默认键位与键盘设定窗对齐**  
6. **再开 Wave A**（r14）

---

## 8. 给用户的拍板项

| # | 决策 | 推荐 |
| --- | --- | --- |
| W1 | 先基础 UI（登录→频道→选角→底栏）还是先 Wave A？ | **先基础 UI** |
| W2 | P0 是否允许直接改 `channel_count` 过滤与快捷格点击（小范围）？ | 允许则出短 Plan 后实现 |
| W3 | 是否需要你再补「当前端」同场景截图以便叠图像素 diff？ | 建议补，加速坐标验收 |

---

## 9. 相关路径速查

- 参考 exe：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe`  
- 正确截图：`C:\Users\11036\Desktop\正确客户端截图\`  
- 源码：`IO/UITypes/UILogin*.cpp`、`UIWorldSelect.*`、`UICharSelect.*`、`UIStatusBar.*`、`IO/LoginBackground.*`、`IO/UIStateGame.cpp`  
- REA：`D:\software\nodejs-24\rea.cmd` + `$env:REA_ANALYSIS_PROVIDER='ida'` + `$env:REA_IDA_MCP_CONFIG='C:\Users\11036\.rea\ida-mcp.json'`
