# R27 轮：12 组实测缺陷的全量修复计划

日期：2026-11-01　输入：用户实拍 11 张（15:08 / 15:15 两批）+ 桌面原生参考帧 ~40 张
构建基线：md5 `9636f77b4030ab4d9a85379cb0694911`（4,419,584 B，2026-11-01 14:16:01）——**用户截图用的就是这一份**，故以下均为真缺陷，非旧包残留。

---

## 0. 已确认决策（不再询问）

| # | 决策 | 来源 |
| --- | --- | --- |
| D1 | 非位图烘焙的文字**一律走 i18n**；默认中文、由客户端配置驱动；运行时切换的**刷新通道本轮打通，切换 UI 留下一期** | 用户 2026-11-01 明示 |
| D2 | 三家族（骑士团/冒险家/战神）全开、技能层全修、战神含姿态 | 先前答复 |
| D3 | 一次性全量规划 + 分批交付，每批自带验收 | 用户偏好 |

## 1. 立项根因（10 路取证的收敛结论）

**不是缺素材。** 部署包 `Data/UI/` 与参照端 `BeiDou-ClientV17.7` 的 UI 包**字节相同**（`UIWindow.img` 11,806,696 B），且 `StatusBar.img` 有、`StatusBar3.img` 无 ⇒ `is_classic_hud()` 为真，**classic 分支确证生效**。`UIWindow.img` 的 112 个根里 `Stat/Equip/Item/Skill/SkillMacroEx/Messenger/GameOpt/SysOpt/GameMenu/ShortCut/MonsterBook/Family/UserList` **全部存在**；`Basic.img/CheckBox` 存在（4 态 12x12）。

真缺的只有：`UIWindow2/4/6.img`、`StatusBar3.img`、`Combo`、`Number`、`socialChatEnter`、`socialRank`、`Item/pos`、`Equip/tabbar`、`Skill/main`、`UserList/Main` 子树。

错位来自**三个公共件级根因族**：

- **G1 位图 origin/锚点处理不统一。** `Texture` 落位是 `pos − origin`（[Graphics/DrawArgument.h:134](../../IO)）。`UIKeyConfig` 给 `backgrnd` 补了 origin（`:91-100`）却没给 `icon` 补（`:631-684`），而 `KeyConfig/icon/0` 实测 **32x31 / origin (0,32)** ⇒ 整块键帽上抬 32px。同类：装备图标绘制加 `(4,4)`、命中区不加（`UIEquipInventory.cpp:338-343` vs `:711-731`）；含标题的 `backgrnd` 与 `backgrnd2` 被同点各画一次 ⇒ 3px 错缝重影（`UIUserList.cpp:59` + `:269`）。
- **G2 文本基线/行距没有唯一真源。** `linespace() = height * 1.35 + 1`（[Graphics/GraphicsGL.h:245](file:///E:/project/MapleStory-Client/Graphics/GraphicsGL.h#L245)），而 `height` 取 ASCII 32..127 的 `bitmap.rows` **最大值**（含 `g/j/p/q/y` 降部，`GraphicsGL.cpp:309-320`）⇒ 行距虚高；各窗又用「墨迹顶 = draw_y + linespace − bitmap_top」反解 Y，且**混用 +6 与 +9** 两种假设。本仓 r15 定版文档 `evidence/w0-common/text-metrics.md` 已确立**纯数字 = +9**，而 R14-3 把金币从 275 改成 278 用的是 +6 ⇒ 上一轮把对的数改错了。
- **G3 两套互不相关的开关决定同一个窗。** 装备窗的坐标表用 `is_classic_hud()`，美术分支用 `classic_flat`（看 `Equip["Equip"]`）⇒ 一旦判据不同步，标签版式与坐标表整体错位，而命中区仍按表命中——正是用户看到的「图标画在别处、悬浮原位却出 tooltip」。

**中文矢量路径本身是对的**（`drawtext` / `measure` 都按解码码点分支，逐字用真实 `ch.ax`）⇒ 叠印不去动度量，只去动**位图叠放**。
遗留的固定步长位图字库：`IO/Components/Charset.cpp:85-174`（逐字节、只适合数字/拉丁）、`Gameplay/Combat/DamageNumber.cpp:133`（自带定宽表）——本轮只在需要处收口，不全局重写。

---

## 2. 批次分解

### B0 验收工具（阻塞所有后续批）
- 新增 `tools/ui_measure.py`：输入原生帧与客户帧 + 窗体锚点，输出**逐格墨迹行列剖面**与 Δx/Δy；替换 `tools/` 下 40 多个一次性量图脚本的用法。
- 为 12 组缺陷各建一行 **oracle 表**（原生量出的基准值），落在 `evidence/r27-oracle/`。
- 闭环判据：**每个后续修复项必须给出「oracle 值 ↔ 修复后实测值」两列**。
- ⚠ 脚本须先 `SetProcessDPIAware()`（本机 125% 缩放）。

### B1 公共件三族（改完再动各窗，否则各窗还会返工）
- **B1a origin 规则**：裸 `Texture::draw` 一律显式声明是否减 origin；修 `UIKeyConfig` icon、装备图标/命中同源、`UIUserList` 双 backgrnd 叠绘、`Item` 回退格位 `(8,51)`→按实测格线 `y=46` 重定。
- **B1b 基线唯一真源**：`linespace` 不再由降部最大行撑大；按原生帧量出 A12M/A11M 的真实行距与「数字/中文/带括号」三类墨迹顶，统一成一处常量，各窗反解公式改为引用它。
  **代价**：所有已按旧 linespace 手调过的窗口常量需同批重量（这是本批最重的部分，也是 D3 选宽档的必然结果）。
- **B1c i18n 接入**：`Util/Locale` + `locale/zh-CN.ini`/`en-US.ini` 已具备配置键 `ui.language`（默认 `zh-CN`）与 `reload()`（`Configuration.cpp:491` 已调），**但全仓只有 3 个文件在用**。本批：
  1. 把已点名站点搬进 ini：`EquipTooltip.cpp:268/279/286/297/304/438-455/511/522/529/540/547`、`Data/EquipData.cpp:68-136`（`TOP` 等）、`Character/EquipStat.cpp:34-52`（`Defense`）、`UIMonsterBook.cpp:46/84/85/89`、`UIUserList.cpp:86/157/243/799`、`UIFamily.cpp:53/54`、`UIFamilyTree.cpp:53/73`、`UIAlliance.cpp:35`、`UIDelivery.cpp:78`、`UIMapMobPanel.cpp:64`、`UIMapDropPanel.cpp:36`、`UIMiscShells.cpp:202/491/681/895/919/1068…`；
  2. 加**运行时刷新通道**：语言变更后广播，使 ctor 里初始化的 `Text` 成员重取文案（切换 UI 留下一期）；
  3. 边界写死：**烤进位图的中文不做 i18n**（能力窗标签行、装备槽名、UserList 标题、GameOpt 底板 12 行粉钮）。
  4. 编码：ini 走 UTF-8，渲染走已修的矢量路径；禁止把中文塞进 GBK 封包路径。

### B2 能力窗 / 背包 / 金币（用户报的 1、5）
`UIStatsInfo.cpp:225-237`（数值 Y 表）、`UIItemInventory.cpp:330`（金币）、格位回退表。依赖 B1a+B1b。

### B3 装备窗（用户报的 2）
- 槽位绘制/命中同源（含 `WEAPON`/`SUBWEAPON` 共用 `(137,131)` 串 tooltip）；
- **可用/不可状态**：现全未实现（`inoperativeSlot`/`slotBg` 零命中，`disabled`/`Slots` 读取在 flat 分支 `return` 之后不可达）；原生帧里 骑兽/鞋子 区确有灰/粉底 ⇒ 按 `Equip/Disabled/*` 实装；
- tooltip 职业名改**横排**（原生帧已裁决，非偏离原生）；尾行改中文（走 B1c）。

### B4 技能窗 + 技能组合栏（用户报的 3）
- **先取证再定方案**：用 `tools/nx_inspect.py` 实测那 4 个技能（精灵的祝福/蜗牛投掷术/团队治疗/疾风步）在 `.nxcache/Skill.nx` 里的真实路径，判定图标缺失是「命名空间换算错」还是「分卷未回退」还是「包里真没有」——`Data/Skill/` 是按 namespace 分文件的散 `.img`（000/100/111/1000…），而 `SkillData.cpp:32-38` 只查 `nl::nx::Skill` 单根，`Skill001/002/003` 已在 nlnx 挂载（`nx.cpp:71,103-105`）却无人回退。
- 组合栏 `UISkillMacroEx`（`UIMiscShells.cpp:3400-3674`）：吸附改**跟随技能窗移动**（现在只在点「宏指令」瞬间 `anchor_right_of` 一次，构造兜底 `center_on_screen`）；行数 `rows = total?:1` 导致只出一行 + `VISIBLE_ROWS=3` < 原生 5 行；`Macroicon` 循环按 5 颗但本包只有 0..2；**收起失效**：`set_macro(false)` 从不 `mex->deactivate()`。
- SP 数值硬编码 `(92,265)`/`(304,23)` 依 B1b 重量。

### B5 聊天栏 + 聊天窗（用户报的 6、7）
- `UIChatBar` classic 分支常量（`ctor:78` 收起位、`user_view_y=80` 展开高、消息循环 `:309-321` 按 13px 无裁剪堆叠、原生上限 `MAX_HEIGHT=467` 未用）；
- 「对所有人」现在是**循环换标签**（`cycle_chat_target` `:1266-1273`），原生是弹选择列表 ⇒ 缺列表窗（全仓无 ChatTargetList 类）⇒ 本轮按原生补一个目标选择弹层，发送通道路由仍走已证文本前缀；
- 「聊天窗」= **UIMessenger**（不是 UIChat）：`backgrnd`/`backgrnd2` 同点叠画、`backgrnd3` 偏移 `(55,22)`、切片 origin 全 0 ⇒ 彩色层堆叠。本包 `Messenger` 根**存在**（backgrnd 295x364 / backgrnd2 295x240 / backgrnd3 184x22 / NameBar / BtEnter / blink）⇒ 可按件重排。

### B6 手册双开 / 目录锚定 / 共用列表窗（用户报的 8、9、10）
- **怪物手册双开是我自己写的**：`UIMonsterBook.cpp:51-54` 主动 `emplace<UIBook>()` 当「并排参考壳」⇒ 删掉即止（`UIElement` 同 Type 不可能双开，无需加保护）。手册内部按 B1c 中文化 + 依 `MonsterBook/backgrnd`(475x349) 重排。
- **目录位置**：`UIGameMenu.cpp:4471` 用 `center_on_screen()`，原生是锚在底栏「目录」钮上方 ⇒ 改锚定（R14-12 当时自认「锚定未改」）。
- **好友/家族/组队共用 `UIWindow.img/UserList`**：删双 `backgrnd` 叠绘；页签真身 `UserList/Main/Tab/*` **本包缺失** ⇒ 无图无字的空槽只能改成本地件近似或**显式预留**（写清）。

### B7 功能级三件（用户报的 11、12）
- **ESC 打不开目录**：客户端默认键表不含 ESC，服务端 KEYMAP 从 DB 装载且该槽无记录 ⇒ 映射 `NONE`，零动作；即便绑上，`MAINMENU` 走 `UIStatusBar::toggle_setting()`（StatusBar3 子菜单），classic 下 `draw()` 提前 return 画不出。修法：客户端内置 ESC→`UIGameMenu`（与底栏「目录」同一条路），**不碰服务端 keymap**。
- **频道更换（需你点头，属协议面）**：出站包短 4 字节——`GameplayPackets.h:51-57` 只 `write_byte`，服务端 `ChangeChannelHandler.java:38` 是 `readByte(); readInt();` ⇒ 抛异常无回包；且回包 `PacketCreator.java:891-897` 的 IP/port 被客户端 `PlayerHandlers.cpp:45-63` 的 `while(available) skip_byte()` 丢弃 ⇒ 永不重连新频道。
- **游戏设置 / 系统设置同窗**：`UIMiscShells.cpp:4488-4495` 把 `BT_GAMEOPT`/`BT_SYSOPT` 合并到一个 case → `UIOptionMenu`；仓库无「系统设置」实现。`GameOpt`(158x284) 与 `SysOpt`(299x365) 两个根各自只有一张底板 ⇒ 控件需自建：**`Basic.img/CheckBox`（4 态 12x12）实测存在**，配合现成 `TwoSpriteButton`/`AreaButton+BoolPair` 范式实装。
- 通病登记：`UIStateGame.cpp:236-459` 一律以 `usable(...)` 静默 `break` 当失败处理 ⇒ 缺件即「点了没反应」且无提示；本批至少给**可诊断留痕**。

### B8 收尾
全量 `-t:Rebuild` 门禁（改公共类成员必须 Rebuild）→ `tools/build_smoke.bat` 出 `GATE OK` → 逐窗抓帧对照 oracle 表 → 更新 `CHECKLIST.md` 新 §L 段 + `exe-stamps.md` → 一功能一原子提交（**推送前对最终提交树重扫凭据**，含本轮新写文档）→ 删临时调试日志（含 `GraphicsGL.cpp:35 dbgatlas()`，随 #44 一并清）。

---

## 3. 验证门禁（本轮起的新标准）

上一轮返工的直接原因是我自证时只核「元素**在位**」，没核「**对齐**」。本轮每项必须给：

1. **oracle 值**（原生帧逐像素量出）与**修复后实测值**并列，差 >2px 记为未修；
2. 我驱动的截图项：我自己开客户端、开对应窗、抓帧、跑 `ui_measure.py`，把两列数贴进 CHECKLIST；
3. 只有你能验的（需特定号/队友/原生客户端并排）：单列一节，不混在「已修」里；
4. 构建门禁：`GATE OK: opcode matrix matches both sources` + `EXITCODE=0`；凡动 `GraphicsGL.h`/公共类成员 ⇒ 强制 `-t:Rebuild`。

## 4. 只能你取证 / 明确预留

| 项 | 状态 |
| --- | --- |
| 原生「游戏设置」「系统设置」两窗的完整版式帧 | 桌面 40 张里没有 ⇒ 需你截原生，否则只能按底板近似 |
| `UserList/Main/Tab`、`Item/pos`、`Equip/tabbar`、`Skill/main` | 本包真缺 ⇒ 本地件近似或预留 |
| `Combo`/`Number`（战神连击计数窗美术） | 真缺 ⇒ 显示层仍是空壳，需出图（ui-photoshop 流程） |
| `socialChatEnter`/`socialRank` | 真缺 ⇒ UIChat 只能留作 Messenger 的回退壳 |
| 键盘设定未绑定托盘 | `KeyConfig/icon` 只有 0..27/50..54/100..106 ⇒ 部分动作无图 |
| 骑士团四转 | 资产缺口（5 个 skill 子树为 278 字节空 imgdir + 服务端写死 120） |
| 暴怒炸金币堆 | 需「掉落物归属」独立功能线 |

## 5. 待你拍板

1. **B1b 的代价**：行距/基线改唯一真源后，**所有**已手调窗口常量要同批重量（本计划按此排期）。若你想省工，另一档是只改各窗常量、把 `linespace` 陷阱留着——不推荐，下一轮还会返工。
2. **B7 频道更换要改出站包结构**（协议面），按门禁先请你确认再动。
3. **#76 凭据处置**仍未答：095 参考库口令明文已随 `97954a2` 推到公开仓库（换本机口令 / 重写历史强推 / 都做）。
4. 顺带旧欠：`fonts/SimHei`（9.3 MB 微软随系统分发字体）是否继续留在公开仓库。

## 6. 风险与回滚

- B1 是公共件，爆炸半径最大 ⇒ 单独一批、单独一个提交，改完立刻全量 Rebuild + 冒烟进图，失败即回滚该提交而不影响 B0 工具。
- 各窗常量改动只动数值不动结构，可逐窗回滚。
- B7 的出站包改动有协议门禁对拍兜底；若真机不通，只回滚该提交，UI 与 ESC 改动不受影响。
