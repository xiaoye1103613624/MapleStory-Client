# 玩家列表 & 家族五页签共用窗（UIUserList）— 最终实现文档

## 1. 范围与结论

以 Angel 083 原生 `CUIUserList` 为基准，把好友 / 组员 / 家族 / 家族联盟 / 拒绝列表五页签统一到 `IO/UITypes/UIUserList.{h,cpp}` 一个可拖拽窗；旧的独立浮窗 `UIBuddyList` / `UIParty` 已删除，其 handler 同步与血条能力并入共用窗。

- 布局：五页互斥底图（好友=backgrnd3、家族/联盟=backgrnd2、组员/拒绝=backgrnd），全部按运行时 `UIWindow.img/UserList` 节点与 Angel OnCreate 校准；**正确端截图仅终验/微调**。
- 页签：`Tab` 枚举对齐 Angel `FRIEND/PARTY/GUILD/UNION/BLACKLIST`；切页禁止 `change_tab(buttonid)`，一律显式 `Tab::*`。
- 交互：五页签 + 行选中 + 滑块 + 四态按钮；空列表/无选中/未入会/未入盟时按钮走 img `disabled` 四态贴图（`UIElement::send_cursor` 对 DISABLED 不派发点击 = 不可操作）。
- 输名口径：**SoftKey = 物理键盘 / ChatBar**，禁止 `UISoftKey`（SoftKeyboard.img）弹窗；悄悄话 = `UIChatBar::set_whisper_target`。
- 协议：只使用 s9 既有 opcode（`PARTY_OPERATION=124`、`BUDDYLIST=63`、`GUILD_OPERATION=65`、`FindPlayer` / 悄悄话包），不臆造拉黑/联盟协议；拒绝列表为客户端本地闭环（s9 无黑名单 opcode）。
- 血条：常驻 `PartyModel::member_hps`（修复「先收血包、后开窗丢失」），组员页 `BtHP` 开关切换显示。
- **W1（2026-11-20）**：`Tab` 枚举对齐 Angel `FRIEND/PARTY/GUILD/UNION/BLACKLIST`；切页与 `send_cursor` 行命中一律显式 `Tab::*`；证据主路径 = Angel IDA + orange-wz IMG。悄悄话 = `ChatBar::set_whisper_target`；输名/公告 = 物理键盘 / ChatBar（**禁止** SoftKeyboard.img 弹窗）。
- **W1.5（2026-11-20）**：`AllianceModel` + handler 写模型；UNION 入盟行渲染与按钮灰亮。见 `notes/w15-alliance-membership-2026-11-20.md`。
- **W1.5 收口（2026-11-20）**：Angel+s9 补齐 Kick(`0x06`)/ChangeMaster(`0x07`)/Grade(`0x09`) 字段并接线；见 `notes/w15-alliance-outbound-clear-2026-11-20.md` + `evidence/alliance-kick-grade-changemaster-2026-11-20.md`。
- **W2（2026-11-20）**：`UIMedal` 接 `Title` 真材 + Detail；`UIFamily` 特权翻页/FamilyUse 复核、去 SoftKey。见 `notes/w2-medal-family-2026-11-20.md`。
- **输名对话框收口（2026-11-20）**：`UIEnterString`（Notice4+物理键 Textfield）接好友加/组队邀/黑名单/公会/联盟/学院箴言；禁 SoftKeyboard。见 `notes/enterstring-physical-kbd-2026-11-20.md`。
- **联盟职衔名窗（2026-11-20）**：`UIAllianceGrade`（Angel `CWndAllianceGrade`）+ 出站 `0x08`；UNION `BtInfo` Toggle。见 `notes/alliance-grade-name-2026-11-20.md`。**≠** 底栏 ↑↓ 成员职级（`0x09`）。

## 2. 关键实现

| 位置 | 内容 |
|---|---|
| `UIUserList::set_page_buttons(uint8_t)` | 五组底栏按钮先全 `set_active(false)`，再按当前页显示+置态（消除跨页叠绘）；`PARTY` 页调 `update_party_button_states()`；行级状态交给 `update_*_button_states()` |
| `UIUserList::refresh_from_models()` | handler 收包后的统一刷新入口 |
| `update_friend_button_states()` | Angel `CTabFriend::OnClick` 规则：无选中全灰 / 在线全亮 / 离线仅 {Chat,Whisper,Party} 亮 |
| `set_guild_buttons()` | 未入会整组 DISABLED，Btnotice 常亮 |
| `update_union_button_states()` | 未入盟整组 DISABLED，笔形钮常亮；入盟按选中点亮行操作 |
| `Character/AllianceModel` | 联盟头信息 + 成员行；handler 主写 |
| `UIMedal` Title 真材 | `UIWindow.img/Title` backgrnd/Tab/Detail |
| `UIFamily` | Family 特权翻页 + FamilyUse；禁 SoftKey |
| `UIAllianceGrade` | 五段职衔名编辑；`AllianceGradeNamePacket(0x08)`；入口 UNION `BtInfo` |
| `update_blacklist_button_states()` | 增加常亮、删除无选中灰 |
| `draw_party_tab()` | 在线/离线双段、双列头、`GaugeBar` 血条裁右填充 |
| `Character/PartyModel` | `set_member_hp` / `member_hp` / `drop_member_hp`，`clear()` 同步清血量 |
| `BuddyHandlers` / `PartyHandlers` | 写模型后 `get_element<UIUserList>()->refresh_from_models()`，不再找旧浮窗 |
| `UIChat` / `UIChatBar` / `MessagingHandlers` | 悄悄话收件人/Find 结果统一取 `UIUserList::get_friend_selected_cid()` |

## 3. 证据与校准

- IDA/反编译：`notes/r1-angel-ida-forensics.md`、`evidence/ida-r1-decompile-index.md`（`CTabFriend::OnButtonClicked @0x91cf4f`、`SetButton @0x91e073`、`CTabFriend::OnClick @0x91dba9`、`CUIPartyHP::Create @0x91f934`）。
- W1 刷新：`notes/w1-angel-img-crosscheck-2026-11-20.md` + `evidence/angel-w1-*.dec.json` + `evidence/img-w1-userlist-children.md`。
- 退役影响面：`notes/r2-retire-legacy-impact.md`。
- 运行时节点树：`evidence/userlist_runtime_nodes.txt`（802 行）。
- oracle 帧与真机对拍：`evidence/frames/native_*_tab.png` vs `evidence/live-r3-*.png`；坐标扫描与结论见 `notes/r3-live-verification.md`（历史对拍；W1 终验须用当前构建重截）。
- 发包证据：`evidence/live-r3-packetdump.txt`（创建组队 `7c 00 01` → 服务端 `3e 00 07`）。

## 4. 验证门禁

- `tools/build_smoke.bat` EXITCODE=0（含 s9 opcode matrix GATE OK；2026-11-20 收口波已重生成矩阵）。
- `tools/test_r3_userlist_083.py` PASS（含 I3 Kick/Grade、I7 UIEnterString、I8 出站、I9 AllianceGrade 0x08）。
- 真机对拍：oracle 帧见 `evidence/frames/native_*_tab.png`；本波可见窗阻塞见 `evidence/live-oracle-compare-block-2026-11-20.md`（勿假装已对拍）。

## 5. 显式预留 / 已知限制

- 好友增删、组队邀请/驱逐、家族与联盟真实数据流需要其他在线角色与文字输入；本端注入脚本无法向 SDL Textfield 键入，未做双开全流程（handler→模型→UIUserList 刷新链路已有守卫覆盖）。
- 被屏蔽好友分支：`s9` 无 buddy block 位，代码保留 `BtUnBlock` 切换但当前不触发。
- 联盟职衔名窗：Angel 删职衔时的「下级占用 / 在职成员」Notice 文案链未全量复刻（服务端仍校验）；公会侧 `CWndGuildGrade` 另案。
- UNION `BtInfo` 改挂职衔名窗后，联盟成员「资料/CharInfo」入口未另开钮（家族页 `BtInfo` 仍开 CharInfo）。
- 窗口打开位置偏左上属既有多分辨率/拖拽存档策略问题，与本窗功能无关，另案处理。

## 6. W2 状态（2026-11-20 已收口）

| 模块 | 入口 | 状态 |
|------|------|------|
| 勋章 | `IO/UITypes/UIMedal.{h,cpp}` | Title 真材 + ToggleDetail；114xxxx 文本态 |
| 学院 | `IO/UITypes/UIFamily.{h,cpp}` | Family 节点 + 翻页/Use；禁 SoftKey |
