# 玩家列表 & 家族五页签共用窗（UIUserList）— 最终实现文档

## 1. 范围与结论

以 Angel 083 原生 `CUIUserList` 为基准，把好友 / 组员 / 家族 / 家族联盟 / 拒绝列表五页签统一到 `IO/UITypes/UIUserList.{h,cpp}` 一个可拖拽窗；旧的独立浮窗 `UIBuddyList` / `UIParty` 已删除，其 handler 同步与血条能力并入共用窗。

- 布局：五页互斥底图（好友=backgrnd3、家族/联盟=backgrnd2、组员/拒绝=backgrnd），全部按运行时 `UIWindow.img/UserList` 节点与 oracle 帧逐像素校准（2026-11-16 真机验证）。
- 交互：五页签 + 行选中 + 滑块 + 四态按钮；空列表/无选中/未入会/未入盟时按钮走 img `disabled` 四态贴图（`UIElement::send_cursor` 对 DISABLED 不派发点击 = 不可操作）。
- 协议：只使用 s9 既有 opcode（`PARTY_OPERATION=124`、`BUDDYLIST=63`、`GUILD_OPERATION=65`、`FindPlayer` / 悄悄话包），不臆造拉黑/联盟协议；拒绝列表为客户端本地闭环（s9 无黑名单 opcode）。
- 血条：常驻 `PartyModel::member_hps`（修复「先收血包、后开窗丢失」），组员页 `BtHP` 开关切换显示。

## 2. 关键实现

| 位置 | 内容 |
|---|---|
| `UIUserList::set_page_buttons(uint8_t)` | 五组底栏按钮先全 `set_active(false)`，再按当前页显示+置态（消除跨页叠绘）；行级状态交给 `update_*_button_states()` |
| `UIUserList::refresh_from_models()` | handler 收包后的统一刷新入口 |
| `update_friend_button_states()` | Angel `CTabFriend::OnClick` 规则：无选中全灰 / 在线全亮 / 离线仅 {Chat,Whisper,Party} 亮 |
| `set_guild_buttons()` | 未入会整组 DISABLED，Btnotice 常亮 |
| `update_union_button_states()` | 未入盟整组 DISABLED，笔形钮常亮 |
| `update_blacklist_button_states()` | 增加常亮、删除无选中灰 |
| `draw_party_tab()` | 在线/离线双段、双列头、`GaugeBar` 血条裁右填充 |
| `Character/PartyModel` | `set_member_hp` / `member_hp` / `drop_member_hp`，`clear()` 同步清血量 |
| `BuddyHandlers` / `PartyHandlers` | 写模型后 `get_element<UIUserList>()->refresh_from_models()`，不再找旧浮窗 |
| `UIChat` / `UIChatBar` / `MessagingHandlers` | 悄悄话收件人/Find 结果统一取 `UIUserList::get_friend_selected_cid()` |

## 3. 证据与校准

- IDA/反编译：`notes/r1-angel-ida-forensics.md`、`evidence/ida-r1-decompile-index.md`（`CTabFriend::OnButtonClicked @0x91cf4f`、`SetButton @0x91e073`、`CTabFriend::OnClick @0x91dba9`、`CUIPartyHP::Create @0x91f934`）。
- 退役影响面：`notes/r2-retire-legacy-impact.md`。
- 运行时节点树：`evidence/userlist_runtime_nodes.txt`（802 行）。
- oracle 帧与真机对拍：`evidence/frames/native_*_tab.png` vs `evidence/live-r3-*.png`；坐标扫描与结论见 `notes/r3-live-verification.md`。
- 发包证据：`evidence/live-r3-packetdump.txt`（创建组队 `7c 00 01` → 服务端 `3e 00 07`）。

## 4. 验证门禁

- `tools/build_smoke.bat` EXITCODE=0（含 s9 opcode matrix GATE OK）。
- `tools/test_r3_userlist_083.py` 33/33 PASS（旧浮窗退役、布局、disabled 三层、血条、接线、场景数值、跨页显隐与真机修复守卫）。
- 真机：五页签空态全部截图对拍 oracle，布局/灰态/按钮分组一致；创建组队按钮发包经 packet_dump 验证。

## 5. 显式预留 / 已知限制

- 好友增删、组队邀请/驱逐、家族与联盟真实数据流需要其他在线角色与文字输入；本端注入脚本无法向 SDL Textfield 键入，未做双开全流程（handler→模型→UIUserList 刷新链路已有守卫覆盖）。
- 被屏蔽好友分支：`s9` 无 buddy block 位，代码保留 `BtUnBlock` 切换但当前不触发。
- 联盟成员数据模型尚未建立（入盟态点亮与行渲染待联盟协议轮次）；当前未入盟态完整闭环。
- 窗口打开位置偏左上属既有多分辨率/拖拽存档策略问题，与本窗功能无关，另案处理。
