# 软键盘热键收口（回归原生 PIC 唯一用途）

> 日期：2026-11-17 ｜ 状态：代码完成、构建通过（EXITCODE=0）、实机已验证通过（物理删除版 build `28e09e60`，服务端 8484 + MySQL 在线；见 §6.6）

## 1. 问题与用户诉求

用户报告"很多按键都会打开软键盘"，并指出软键盘本应只在输入密码、点击软键盘时才打开。

## 2. 原生证据（REA / IDA / IMG）

结论：**原生 v83 北斗客户端不启用软键盘，软键盘的原生唯一场景是选角 PIC/二次密码（SPW）**；"普通按键弹软键盘"是本端历史 SoftKey 波次的过度实现，非原生。

- 原生 v83 BeiDou.exe / suipre.dll 对 `SoftKey`/`SoftKeyboard`/`BtNum`/`KeyWindow` 三类编码均 **0 命中**（`evidence/ida_softkey_dump.txt`）。
- 原生唯一软键盘场景：`CLogin::SendSelectCharPacket @ 0x5da2a0`，由 `m_bLoginOpt`（服务端状态）驱动，PIC/SPW 才弹 `CSoftKeyboardDlg(nMin:6,nMax:16)`；普通选角（case 2/3）直接发包不弹（`REA反编译/095REA/.../5da2a0.json`）。
- v95 参照端才加载软键盘资产（`n/SoftKey/BtNum/%d` @ 0xB5563C 等），v83 不加载。
- 本端约 465–500 处 `emplace<UISoftKey>` 热键/按钮接线来自历史波次（r161/r254/r285 等），非原生还原。

完整证据见 `evidence/native-softkey-rea.md`。

## 3. 实现逻辑

### 3.1 总闸（默认关闭）

- 新增配置 `ui.softkey.hotkeys`（`Configuration.h/.cpp` 的 `SoftkeyHotkeys`，默认 `false`），已入 SAVE_ORDER、中英文言词条、`xy-config.example.yaml`。
- `IO/UI.cpp` 的 `UI::send_key`：关闭时，未绑定键的**可打印字母**（GLFW_SPACE..Z）不再把 keycode 当 action 下传给窗口 handler——从源头掐断所有"裸字母 → emplace<UISoftKey>"捷径。
- **保留**：方向键/回车/小键盘回头的 ASCII 兜底（焦点气球目录/列表依赖），以及 ESC/Tab/X 的层级派发。
- **不受影响**：选角 PIC/二次密码由服务端状态驱动，不走 ASCII 兜底。

### 3.2 默认模式改 Text + PIC 显式化

- `UISoftKey::Mode` 默认由 `Pic` 改为 `Text`（明文/最短 1/允许键盘输入），避免任何软键盘被误设成"密码框"观感。
- 所有 PIC 合法路径改为**显式** `Mode::Pic`（密文掩码/最短 6/check_pic/仅鼠标）：
  - `UICharSelect.cpp`：`require_pic==1` 选角、`DeleteCharPicPacket` 删除校验、`request_pic()` 注册/修改的两次输入（共 4 处）。
  - `UIViewAllChar.cpp`：`sel_use_pic` 查看全部角色 PIC（1 处）。

### 3.3 为何不逐模块改造 465 处

subagent 全量核对 packet 调用点后确认：总闸掐断的裸键捷径，绝大多数功能都有**原生替代途径**：

| 功能 | 原生替代 |
|---|---|
| 聊天 / 密语 | Enter 或点击输入框 → UIChatBar 物理键盘直接输入（`GeneralChatPacket @UIChatBar.cpp:2718` / `WhisperPacket :2878`） |
| 换频道 | UIChannel 窗口 + BtChange 按钮 |
| 丢金币 | 背包金币按钮 → UIEnterNumber |
| 拾取 / NPC / reactor | 走过去自动拾取 / 鼠标点 NPC / 物理攻击 |
| UserList 好友组队、KeyConfig 保存、WorldSelect 选世界、ViewAllChar 选角 | 均有按钮/菜单点击途径 |

因此逐模块把裸键改成文本输入直连**不必要且爆炸半径大**，违反最小改动原则，故不实施。

## 4. 取舍与影响（总闸关闭后失去快捷键的功能）

以下为**调试/边缘型 ID 直接输入**，关闭后裸键不再弹软键盘，也没有 100% 等价替代（均为私服调试入口，原生 v83 不依赖软键盘）：

1. 用椅子（指定 ownerid）：`UseChairPacket`
2. 物品特效：`UseItemEffectPacket`
3. 门 ownerid：`UseDoorPacket`
4. 未选中目标时用技能 / 换地图（裸 F/M）：`UseSkillPacket` / `ChangeMapPacket`
5. 表情 ID（裸 E/X）：`FaceExpressionPacket`（聊天行的表情按钮鼠标点击仍可用）

如需恢复这些，正确做法是复用 `UIEnterNumber` 通用数字输入框挂到对应按钮，而不是恢复裸键 PIC 软键盘；本版不实施，显式记录为预留。

> **注意**：本节（§3-§5 早期版本）的"总闸/配置回滚"方案已被 §6 的**物理删除**方案取代。下方 §6 为最终落地状态。

## 5. 验证（早期总闸版，已被 §6 取代）

- 构建：`tools/build_smoke.bat` → EXITCODE=0、GATE OK；部署 exe 时间戳刷新 2026-11-17 13:36:23。
- 静态：所有改动文件 lint 0 错误；PIC 路径显式化经逐处核对（UICharSelect 4 处 + UIViewAllChar 1 处）。
- **未跑项**：实机按键/截图验证需服务端 8484 + MySQL + Java21，本会话服务端未启动，按计划列为未跑。

## 6. 最终方案：物理删除非 PIC 软键盘（用户决策覆盖）

用户后续明确要求"除了二级密码调用保留，其它地方移除"，否决了 §3-§5 的运行时总闸方案。最终落地为**物理删除全部非 PIC 软键盘调用**。

### 6.1 参考端依据（Angel / MinMS）

- **MinMS**（`E:\project\MinMS\src\client\`）：有 UISoftKey 类但全仓唯一实例化点就是选角 PIC（`UISoftKey.h:28-29` 注释 "used for pic/pin input"；`UICharSelect.cpp:396-401`）。其它输入全部走正途：聊天=Textfield（`UIChatBar.cpp:238,984`）、密语=`/whisper`（:1028）、换频道=UIChannel 按钮（`UIChannel.cpp:377`）、数量输入=UIEnterNumber 弹窗（`UINotice.cpp:301,361,368,428`）、椅子=背包点击 `UseChairPacket`（`Character/Player.cpp:2481`）。
- **Angel**（v83 原生行为真源，`E:\MXD\反编译IDA\v83\Angel.i64`，IDB 无 exe 本体）：v83 exe/suipre.dll 对 SoftKey/BtNum/KeyWindow 三编码 0 命中；有 CSoftKeyboardDlg 类但仅 PIC/SPW 发包用（`CLogin::SendSelectCharPacket @0x5da2a0` case 0/1）。聊天=`CCtrlMLEdit::OnIMEComp @0x4d33c2`、丢金币=`CUIItem::OnDropMoney @0x81d996→CUtilDlgEx→MESO_DROP(94)`，均无软键盘。
- 结论：软键盘 = 仅 PIC；本端约 420 处普通输入软键盘是历史波次错误接线，删除符合原生。

### 6.2 删除范围与保留清单

- **删除**：全工程 `emplace<UISoftKey>` 共 426 处盘点（IO 目录）+ 1 处（WaveAShell 心愿单），非 PIC 的全部删除。实际物理删除/替换 **410 条语句、68 个文件**（UIMiscShells 158、UIItemInventory 21、UIUserList 18、UICashShop 18、UIStatusBar 11、UIChatBar 11、UICharInfo 8、UIFamily 9、UIDelivery 9 等；含 WaveAShellHandlers.cpp:3809 心愿单）。
- **保留（5 处 PIC，唯一合法场景）**：`UICharSelect.cpp:1265`（选角 PIC）、`:1338`（删除角色校验）、`:1729`/`:1732`（注册/修改 PIC 两次输入）、`UIViewAllChar.cpp:189`（查看全部角色 PIC）。与 MinMS/Angel 唯一用途一致。

### 6.3 删除方法与安全措施

- 采用**括号匹配的语句级解析删除**（脚本 `C:\Users\11036\.rea\softkey_remove.py`，非盲正则）：精确定位 `UI::get().emplace<UISoftKey>(...);` 完整语句（跳过字符串/注释、匹配嵌套括号），倒序删除避免行号错乱。
- 三道保护：① 参数含 `Mode::Pic` 跳过（PIC 例外）；② 嵌套调用只删最外层、含 PIC 的外层跳过（12 处嵌套）；③ 无花括号的 `if/else` 主体删除后改 `;` 占位防悬空（9 处）。
- 删除前备份 `IO/UITypes` 到 `C:\Users\11036\.rea\backup_UITypes_20261117`（184 文件）以便回滚。
- 删除后 `emplace<UISoftKey>` 全局仅剩 5 处 PIC（UICharSelect 4 + UIViewAllChar 1）；`.agents/worktrees/` 下的 worktree 副本未触碰。

### 6.4 业务逻辑取舍

- 参考端正途在本端均已存在（UIChatBar 文本聊天、UIChannel 换频道按钮、UIEnterNumber 数字输入、背包点击椅子），删除软键盘后这些功能仍有入口，业务不丢。
- 以下**调试/边缘型 ID 输入**删除后失去快捷键途径（原生 v83 本就不依赖软键盘，属预期内）：表情 ID、指定 ownerid 椅子/物品特效/门、未选中技能/地图 ID、结婚心愿单文本输入。
- 配置 `ui.softkey.hotkeys` 骨架与 `IO/UI.cpp:317-336` 裸键总闸保留不改（现已成为无实际软键盘调用的冗余防线，但不影响功能）；**旧行为的软键盘代码已被物理删除，该配置无法恢复已删调用**。

### 6.5 验证（最终）

- 构建：`tools/build_smoke.bat` → EXITCODE=0、GATE OK；部署 exe `E:\MXD\BeiDou-Client-xy\MapleStory.exe` 刷新至 **2026-11-17 16:57:39**（4.58MB）。
- 静态：lint 0 错误；grep 复核 `emplace<UISoftKey>` 仅 5 处 PIC、`Mode::Pic` 调用点仅那 5 处；抽查悬空保护点（UIChat.cpp:148、UIStatusBar.cpp:1379、UICharSelect.cpp:1124）语法完整。
- 实机：见 §6.6（物理删除版 build `28e09e60` 真机验收，已补跑通过）。

### 6.6 实机验收（物理删除版 build `28e09e60`，2026-11-17 补跑）

服务端 8484 + MySQL 在线，客户端 `xy-config.yaml` 为 `login.auto: true`，自动登录进图（角色「暗之黑骑士」）。验收用部署端 `MapleStory.exe` 真机客户区截图（1280×720），脚本 `_verify_live_final.py` / `_verify_clean_d.py`；截图存档于 `evidence/screens/`。

| 项 | 操作 | 证据图 | 结论 |
| --- | --- | --- | --- |
| 怪物手册仍居中 | B 键开手册 | `vlive_01_book.png` | ✅ 手册居中显示，无软键盘 |
| 点「开关按钮」不弹软键盘 | 点击 BtSearch（手册左页顶 x≈573,y≈222，视觉像 toggle 的搜索键） | `vlive_02_click_search.png` | ✅ **不弹软键盘**（用户核心诉求），仅出现 hover 手形/卷轴 tooltip |
| 按 S 不弹软键盘 | S 键 | `vlive_03_press_s.png` | ✅ 不弹软键盘；弹的是「角色能力」窗（STATSINFO 原生快捷键，非软键盘） |
| 临时属性面板不弹（手册态） | ESC 后 D×4 | `vlive_04_after_d.png` | ✅ 无临时属性面板、无软键盘 |
| 临时属性面板不弹（纯游戏态） | 关面板后 D×4 | `vclean_01_after_d.png` / `vclean_02_final.png` | ✅ 无临时属性面板（右下「主菜单」为 HUD 常驻快捷盘，非临时弹窗，不影响判定） |

**验收判读依据**：

- 用户原话「点击怪物手册上的开关按钮会打开软键盘」——该「开关按钮」经复核即 `UIMonsterBook.cpp:156` 的 `BtSearch_pos(175,29)`（手册原点≈(395,188)，屏幕坐标≈(573,220)，贴图是视觉上像 toggle 的搜索键）。物理删除后 `case BtSearch` 为空实现，`vlive_02` 证实点击后无软键盘——核心诉求闭环。
- 「软键盘被很多按钮触发」——全工程 `emplace<UISoftKey>` 已物理删至仅 5 处 PIC（见 §6.2），`vlive_02`/`vlive_03` 双重印证普通点击/裸键均不再弹软键盘。
- 「buffer 后显示临时属性 UI，帮我隐藏」——`UI.cpp` 已移除全局 D 兜底、`UIStateGame.cpp` 建窗即 `deactivate()`；`vlive_04`/`vclean_01`/`vclean_02` 证实手册态与纯游戏态按 D 均不再弹面板。

**边界与保留（如实声明）**：

- 手册搜索（输入怪物 ID 跳转）物理删除后成为空实现，属 §6.4 已记录的「调试/边缘型 ID 输入删除后失去途径」预留；`vlive_02` 的 hover tooltip 是残留 UI 元素，无功能。
- 按 S 弹「角色能力」窗是 STATSINFO 原生快捷键被触发，非软键盘问题，不在本次任务范围（本次仅消除软键盘误弹），未改。
- 本轮验收为客户端 UI 行为，不涉及服务端发包；PIC 合法路径（5 处）未在本轮触发（s9 `enable_pic=false`，符合 §2 原生结论）。
