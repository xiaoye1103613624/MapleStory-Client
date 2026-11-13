# 「玩家列表 & 家族」五页签共用窗：根因诊断与还原 Plan

> 2026-11-08，用户给出 5 组「当前 / 正确」截图，问：缺失是 REA 没拿到正确客户端代码，
> 还是 IMG 节点数据没校验？§1–§6 是证据解析、诊断与规划；用户回复「开始」后，
> 已按 D1–D3 推荐值实施，实施结果见 §8。

## 1. 结论（直接回答）

**主因是 IMG 节点数据整套没有按运行时校验，不是 REA 反编译失败。**

`IO/UITypes/UIUserList.cpp` 沿用的是开源 Journey v83 客户端的节点命名，
与北斗运行时部署数据 `UIWindow.img/UserList` 的节点树**不同名**：

- 运行时 IMG 有 **158** 个不同节点名；代码里 **48** 个 `["..."]` 字面节点引用，
  其中 **25 个在运行时根本不存在**（52% 失配）：
  `Sheet1 / Sheet2 / Sheet6 / Main / title / base / CbCondition / TapShow* /
  BtPartyMake / BtPartyOut / BtPartySettings / BtPartyLevel / BtPlusFriend /
  BtMessenger / BtSheetIClose / BtEnter / Boss / PartySearch / partyName / request /
  table / text`。
- 反过来，运行时资源大量**从未被代码引用**：
  `Friend` 38 个直接子节点、`Party` 18 个、`BlackList` 5 个、
  `Guild/GuildInfo` 23 个、`GuildUnion` 20 个 —— 其中 `Party`、`GuildUnion`
  两套几乎完全没接（`GuildUnion` 代码里 0 个实际引用）。

所以五个页签里只有**家族页**（2026-11-07 按运行时节点重做过一次）接近原生，
其余四页只剩「底图烘焙件 + 硬编码 `Text("none")`」——与用户截图完全一致。
这不是数据没填，是资源件压根没取到 / 没绘制。

**REA 侧是次要因素，且不构成"缺失原因"**：本功能线确实没有做原生逻辑反编译取证，
但即使拿到原生代码也拿不到每页签的贴图落位与按钮表 —— 布局是 WZ/脚本驱动的：

| 核查 | 结果 |
| --- | --- |
| `rea` provider=ida | `list_documents` / `binary_overview` / `search_strings` 均返回 `unreadable_output`（提示 `rea doctor`）；`current_document` = `unsupported_provider`。**当前不可用，记阻塞，不假装已解析** |
| 本地已有反编译产物 | `REA反编译/ui/*.c`、`party/*.c` 无 UserList 五页签窗口逻辑；`095REA` 线虽有 UserList 记录，但按 083/v83 基准**禁用**，不作依据 |
| exe 静态取证 | `BeiDou.exe` 内 `UIWindow.img/UserList` 1 处（file off `0x73C2DF`）、`UserList` 4 处；`.text` 内**无**该字符串 VA 的 4 字节立即数引用 ⇒ 按名查表 / 脚本拼路径，布局常量不在立即数里 |

即：**布局的 ground truth 只能是运行时 IMG 节点树，落位校准只能靠原生截图**；
REA 恢复后能补的是窗口创建 / 页签切换回调的语义，不是 UI 版式。

## 2. 证据清单（本轮落盘）

| 文件 | 内容 |
| --- | --- |
| `evidence/userlist_runtime_nodes.txt` | `tools/nx_inspect.py list <UI.nx> 4 UIWindow.img/UserList` 完整 802 行树（含节点名/类型/尺寸/origin） |
| `evidence/node_ref_audit.txt` | 代码字面引用 × 运行时节点名逐项对照 + 各页签直接子节点清单（含审计口径说明） |
| `evidence/r27-oracle/UserList.json`（既有） | 五格页签 oracle、三张互斥底图判定、`layout.py` 逐像素量具 |
| 用户提供的 5 组截图 | 当前/正确 各 5 张，作为落位、计数、禁用态与中英文文案的视觉校准 |

运行时顶层：`BlackList / Friend / Guild / GuildUnion / Party / Tab /
backgrnd(312x389) / backgrnd2(312x389) / backgrnd3(312x389)` —— **没有 `Main`、
没有 `Sheet*`**（`Main` 在 ctor 有 `if (!Main) Main = UserList;` 回退，救回了页签条与底图；
但 `UserList["Sheet1"/"Sheet2"/"Sheet6"]` 无回退，恒空）。

各页签可用资源（直接子节点，节选）：

| 页签 | 横幅 / 列头 / 分隔 | 按钮 |
| --- | --- | --- |
| Friend | `friend0`(281x27) `friend4`(281x27) `friend1/2/3/5/6` `icon0..5` `line` `buttonbg` | `BtAdd` `BtAddFriend` `BtAddGroup` `BtBlock` `BtChat` `BtDelete` `BtGroupWhisper` `BtMate` `BtMessage` `BtMod` `BtParty` `BtShowAll` `BtShowOnline` `BtUnBlock` `BtWhere` `BtWhisper` `GroupMod` `Popup` |
| Party | `party0/4`(281x27) `party1/5`(279x18) `party2/3` `icon0/1` | `BtChangeBoss` `BtChat` `BtCreate` `BtHP` `BtInvite` `BtKick` `BtSearch` `BtWhisper` `BtWithdraw` `PartyHP` |
| BlackList | `blacklist0`(281x27) `blacklist1`(281x18) `blacklist2`(281x6) | `BtAdd` `BtDelete`(68x17) |
| Guild/GuildInfo | `guildinfo0/1/2/3/4/5/6/7` `guildmark`(20x20) | `BtBoard` `BtChange` `BtChat` `BtDown` `BtGuildBBS` `BtInfo` `BtInvite` `BtKick` `BtPartyInvite` `BtUp` `BtWhere` `BtWhisper` `BtWithdraw` `Btnotice` |
| GuildUnion | `backgrnd`(233x304) `guildinfo0/2/3/4/5/6/7` | `BtChange` `BtChat` `BtDown` `BtInfo` `BtInvite` `BtKick` `BtPartyInvite` `BtUp` `BtWhere` `BtWhisper` `BtWithdraw` `Btnotice` |

> 纠正上一轮文档的一处错误：`GuildUnion` 不是"只有美术、无内容"——它有 20 个
> 直接子节点（12 按钮 + 7 位图 + 独立 `backgrnd`），足以还原整个联盟页。

## 3. 五组截图逐页签差异（正确端 → 当前端）

| 页签 | 正确端有、当前端缺 |
| --- | --- |
| 家族 | 在线/离线横幅右上角 `[0/0]` `[1/1]` 计数、空列表 `none` 占位行、右侧滚动条、底部按钮 **4 列 x 3 行**、未入会时按钮全 **disabled 灰**、米色条「没有参加的家族」右侧笔钮（`Btnotice`）；当前端是 3 列彩色中文按钮 + 右上角多一个绿色勾选件 |
| 好友 | 计数 `0/25`（当前硬编码 `0/50`）、黄色「在线朋友」筛选（`BtShowAll/BtShowOnline`，当前误接成红色 `BtChat`）、列表内 `Default Group(0/0)` 分组行、中文「目前位置 - 蘑菇...」（当前英文 `My Location - Henesys Market`）、底部 10 个按钮、状态图标 `icon0..5`、右侧滚动条 |
| 拒绝列表 | 「拒绝列表 `0/26`」横幅(`blacklist0`)、列头格(`blacklist1/2`)、底部黄色增加 / 灰色删除(`BtAdd/BtDelete`)、滚动条 —— 当前端整页只有 `none` |
| 组员 | 在线/离线组员横幅 `0/0`(`party0/4`)、列头名字/职业/等级(`party1/5`)、`none` 行、底部 4x3 按钮、`PartyHP`/HP 显示、滚动条 —— 当前端整页只有 `none` |
| 家族联盟 | 米色条「还未加入联盟。」(`guildinfo6`) + 笔钮(`Btnotice`)、横幅/列头(`guildinfo0/2/3`)、底部 4x3 灰按钮、滚动条；页签本身在无联盟时应为 **disabled**（r27-oracle 已量出 `Tab3_disabled` agree 1.000）—— 当前端只有 `none` 且页签画 enabled |

三张互斥底图（同为 312x389、不透明占比 .988）按 r27-oracle meanSAD：
好友=`backgrnd3`(11.6)、家族联盟=`backgrnd2`(7.6)、组员/拒绝=`backgrnd`；
**家族页归属尚缺一次量测**，实现阶段补 `layout.py bias/check`。

## 4. 还原方法（证据优先级）

1. **运行时 IMG 节点树**（权威）：逐页签绑定真实节点名/四态贴图/尺寸，禁止再抄旧代码节点名（R1/R5）。
2. **原生截图 + r27-oracle `layout.py`**（落位校准）：oracle 只能量出来，Δ>tol(2px) 判未修复；页签选中/禁用态、计数、中英文文案以截图为准。
3. **s9 服务端源码**（协议）：按钮动作的 opcode 与字段序；好友/组队/公会/联盟/拒绝各有对应 handler。
4. **REA/IDA**（当前阻塞）：provider 恢复后补窗口创建 / 页签切换回调语义；`095REA` 线禁用。
5. 冲突时：**节点存在性与尺寸以运行时 IMG 为准**，落位/禁用态以原生截图校准。

## 5. 待用户抉择的决策点

| # | 决策 | 推荐 |
| --- | --- | --- |
| D1 | 还原范围 | **五页签全量**（UI + 业务接线一次做完），不拆成"先 UI 后功能"两轮 |
| D2 | 家族联盟页深度 | **UI 全量 + 按钮按 s9 `ALLIANCE_OPERATION` 接线**，未入联盟时按钮 disabled；运行时 `GuildUnion` 资源足够，不必留空 |
| D3 | 改造方式 | **按页签拆分成员函数增量改造**：保留现有好友/组队/拒绝的业务函数与 `GuildModel` 数据层，不整体推翻 1000+ 行文件；仅替换资源加载与绘制/按钮装配层 |

## 6. 一次性完整 Plan（确认后执行）

**阶段 A 证据固化**（已完成 3/3）
- [x] 运行时节点树 + 代码引用对照落盘
- [x] 5 张正确端截图按 ASCII 命名纳入 `evidence/frames/`，md5 校验（`userlist_oracle.py` 输出）
- [x] 三张互斥底图归属逐像素 meanSAD 量出：好友=bg3(11.59) / 联盟=bg2(7.61) /
      家族=bg2(25.06) / 拒绝=bg1(11.07) / 组员=bg1(23.50)。**家族与联盟共用 bg2**
- [ ] `layout.py mark/bias` 五页签逐格 oracle 探针：未跑（`check` 需要客户端复拍帧，
      当前环境抓屏全黑，须用户在屏前复拍后补跑）

**阶段 B 共用壳校准**
- 五格页签 `Tab/enabled|disabled/0..4` 正确态；无联盟时第 4 格 disabled（现有 `BT_TAB_UNION` 只画不切页的临时做法撤销）
- 三张互斥底图按当前页签**只画一张**；关闭钮 `Basic.img/BtClose`；窗口 dimension 按 oracle 定
- `dimension` / `is_in_range` / `send_cursor` 按五页签统一（含滑块、行命中）

**阶段 C 逐页签资源还原**（按 §2 节点表）
- 好友：`friend0/4` 横幅、`friend1/5` 列头、`friend2/3/6` 分隔、`(0/25)` 计数、`BtShowAll/BtShowOnline` 筛选、分组行、状态图标、底部按钮组、中文位置 i18n、滑块
- 组员：`party0/4` 横幅、`party1/5` 列头、计数、`BtCreate/BtInvite/BtWithdraw/BtKick/BtChangeBoss/BtSearch/BtHP/BtWhisper/BtChat`、`PartyHP`、滑块
- 家族：修正上一轮遗漏 —— `guildinfo2/3/4/5`、`guildmark`、`BtBoard/BtChange`、`BtGuildBBS`（节点确实存在）、`[0/0][1/1]` 计数、`none` 行、滑块、4x3 排布、未入会 disabled 态
- 家族联盟：`GuildUnion/guildinfo6` 米色条 + `Btnotice` 笔钮 + 横幅/列头 + 12 按钮 disabled 态 + 滑块 + 独立 `backgrnd`(233x304)
- 拒绝列表：`blacklist0/1/2`、`0/26` 计数、`BtAdd/BtDelete`、滑块

**阶段 D 业务接线**
- 按 s9 逐按钮发包（Buddy / Party / Guild / Alliance / Blacklist），空列表或无权限时按钮 disabled
- 行点选统一走 `send_cursor`；页签切换只切数据/按钮态，不重建窗口
- i18n 补齐中文位置等键（zh-CN / en-US 各一份）

**阶段 E 验收门禁**
- `gen_opcode_matrix.py` + `build_smoke.bat` → `GATE OK` / `EXITCODE=0`
- `layout.py check` 对 5 张 oracle 帧逐格 Δ≤2px；节点存在性 R5 复核（禁止把 `none` 容器当 bitmap）
- 真机冒烟（需用户在屏前）：5 页签逐页复拍 + 按钮发包抓 `packet_dump.log`

**阶段 F 归档**
- 实现逻辑 notes + evidence（含复拍帧）、更新 `pending-live-tests/README.md` 的 #35 口径（G 键已改为共用窗家族页）并新增 #36（五页签冒烟）

## 7. 阻塞与显式预留

- `rea` provider 当前不可用（多次 `unreadable_output`），本轮未做原生反编译；恢复后补窗口语义，不阻塞 IMG 主线。
- 095 线产物按 083 基准禁用。
- 家族页底图归属、5 张 oracle 帧入库属阶段 A 待办，未伪造数值。
- 真机复拍需用户在屏前；本环境历史抓屏全黑，不假装已验证。
- `rea` provider 仍不可用；本次未做原生反编译，不假装已解析。

## 8. 实施记录（2026-11-08 01:41）

| 文件 | 改动 |
| --- | --- |
| `IO/UITypes/UIUserList.h` | `Tab::UNION`；40+ 运行时按钮枚举（追加在末尾，旧枚举值不变）；五页签 Texture/Text/Slider 成员；`bind_runtime_assets` / 4 个 `draw_*_tab` / `set_page_buttons` / `tab_button` 声明 |
| `IO/UITypes/UIUserList.cpp` | `bind_runtime_assets()` 按 158 节点树绑定五页资源 + 44 个按钮 + 5 个 Slider；draw 五分发；`change_tab` 三张互斥底 + 显式页签映射；`button_pressed` 新按钮路由；TAB 五页循环；家族页补计数 / none 行 / 滑块 / 4x3 排布 |
| `locale/{zh-CN,en-US}.ini` | 各补 5 键 |
| `s9-protocol-align/protocol/opcode-matrix.json` | 重算（client out 148/148） |
| `pending-live-tests/README.md` | #35 口径修订 + 新增 #36 |
| `social-loop-audit-2026-11/notes/guild-tab-implementation.md` | 纠正底图归属与联盟页状态 |

**门禁**：`gen_opcode_matrix.py` 重算 → `GATE OK: opcode matrix matches both sources`；
`build_smoke.bat` → `EXITCODE=0`、0 error / 0 fatal；
`E:\MXD\BeiDou-Client-xy\MapleStory.exe` 2026-11-08 01:41、4,920,832 B；
两份 locale 已同步部署目录（新键实测存在）。

**构建期非代码故障（已排除）**：连续构建触发 MSBuild `/m` 节点复用残留 →
39× `C1041 vc142.pdb` 争用，随后 `LNK1136 UICommonCreation.obj` 损坏。
处理：清空残留 `cl.exe`/`MSBuild.exe`、删坏 obj 与 pdb、
`MSBUILDDISABLENODEREUSE=1` 单次重建 → 0 error。与业务代码无关。

**显式预留（不假装闭环）**：

1. **落位是目视初值**：横幅 / 列头 / 计数 / 按钮网格坐标按 5 张 oracle 帧目视给出，
   尚未用 `layout.py` 逐格量测；Δ>2px 的格子需复拍后校准。
2. **oracle `check` 未跑**：需用户在屏前复拍 5 页客户端帧才能对账（本环境抓屏全黑）。
3. **列表数据仅空态**：五张 oracle 帧本身都是空态（0/25、0/0、0/26、
   「没有参加的家族」、「还未加入联盟。」），本次按空态还原；有真实数据时的
   行渲染 / 行命中 / 滑块联动需真机联调。
4. **按钮语义映射按节点名推断**：`BtMate` / `BtMod` / `BtMessage` /
   `BtGroupWhisper` / `GroupMod` 等与中文按钮文字的对应关系无原生证据，
   暂统一落到已核对的 `BuddyAdd` / `Whisper` / `FindPlayer` 包，需实机逐钮确认。
5. **联盟操作不臆造 action 字节**：`AllianceInvitePacket` / `AllianceNoticePacket`
   按已核对字段直发；其余联盟按钮统一打开 `UIAlliance` 窗处理。
6. `BT_GUILD_CHANGE`（家族页 `BtChange`）与 `PartyHP` 未落位 / 未接线，待原生证据。
7. 无家族 / 无联盟时的按钮 disabled 态、页签 disabled 态需数据驱动规则，
   本次按钮一律 active。
8. `friend_line`、`page_selected_row` 等成员为后续逐格校准预留，当前未参与绘制。
