# r3 真机验证记录（2026-11-16）

## 环境与方法
- 构建：`tools/build_smoke.bat` EXITCODE=0（含 GATE OK，opcode matrix 与 s9 3e36ada0c7 一致），产物直写 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`。
- 启动：`xy-config.yaml` `login.auto=true`（admin/admin）+ `debug.packet_dump=true`，从部署目录启动自动登录进图（132-暗之黑骑士）。
- 开窗：`R` 键（FRIENDS=R）打开 `UIUserList` 好友页；五个页签按客户区坐标 (108,127)/(139,126)/(170,126)/(200,127)/(253,126) 逐页点击截图。
- 基准：`evidence/frames/native_{friend,party,guild,union,blacklist}_tab.png`（Angel 083 oracle 帧）；所有坐标以像素扫描/局部放大对拍。

## 真机截图（新构建，均存 `evidence/live-r3-*.png`）
| 页签 | 证据 | 对拍结论 |
|---|---|---|
| 好友 | `live-r3-friend.png` | 顶部横幅/0-25/增加朋友/增加群/在线可见、蓝字 `Default Group(0/0)` + 点线、none、目前位置、底部两排 8 钮全灰，均与 oracle 同构；无跨页按钮叠绘 |
| 组员 | `live-r3-party.png` | 在线(0/0)/离线(0/0) 双段 + 双列头 + none + 5+4 底钮（创建/搜索/退出/邀请/悄悄话；悄悄话/邀请组队/踢人/对话），与 oracle 一致 |
| 家族 | `live-r3-guild.png` | 没有参加的家族 + 在线/离线家族 (0/0) 双列头/none；底栏 4+4+3 全灰，首槽空，与 oracle 一致 |
| 家族联盟 | `live-r3-union.png` | 还未加入联盟提示 + 笔形钮；底栏 4+4+2 全灰（row3 仅邀请组队/资料），与 oracle 一致 |
| 拒绝列表 | `live-r3-blacklist.png` | 拒绝列表 (0/26) + none；增加（黄绿可点）/删除（灰禁用）两钮，与 oracle 一致 |

## 真机暴露并已修的问题（本轮修复）
1. **五页底栏同屏叠绘**：r3 新建的底栏按钮只设置 state、从未切 active，五页按钮全部绘制到同一区域（首轮截图确认）。`UIUserList::set_page_buttons()` 改为「先五组全 `set_active(false)`，再按当前页显示+置态」；跨页切换与 `refresh_from_models()` 幂等。
2. **`BT_FRIEND_SHOW_ONLINE` 只引用未绑定**：好友页缺「在线可见」筛选钮。`bind_runtime_assets()` 补绑 `Friend/BtShowOnline @(228,77)`。
3. **好友分组行叠图**：`friend4` 实为「离线好友」筛选态横幅（orange-wz 128B key 导出 PNG 确认），旧实现当分组条画在 (12,100)，与蓝字 `Default Group` 叠死。改为不绘制 `friend4`，改画蓝字 (16,112) + `friend6` 线 (102,112) 宽 123；列头/分隔条 oracle 空态不绘制；none 首行 150→130，目前位置 292→313。
4. **其余四页坐标按 oracle 像素扫描校准**：组员横幅 50/131、计数 261、none 106/187；家族在线横幅 111→103、未入会文案 96→115、第三行 4 钮右移一列（BtBoard 空槽不绑定）；联盟底栏重排（BtChange 到 row1col3、Btnotice 独立 @287,78、BtWhere 空槽不绑定）；拒绝列表横幅 55、计数 233,65、none 86、增加 (61,356)/删除 (180,356)。
5. 守卫 `tools/test_r3_userlist_083.py` 新增 H1-H3 锁定 1/2/3 项结构修复，F4 期望值随 none 首行 86 更新；33/33 PASS。

## 发包证据
- 点击组员页「创建组队」：`[SEND] PacketDump : 7c 00 01` → `OutPacket::Opcode::PARTY_OPERATION=124` + mode 1（`PartyCreatePacket::write_byte(1)`，s9 `PartyOperationHandler` 对齐）。
- 服务端回包：`[RECV] PacketDump : 3e 00 07 01 ...` → `SendOpcode PARTY_OPERATION=0x3E` + mode 7（建队成功）。原始行见 `evidence/live-r3-packetdump.txt`。
- 空态 disabled 钮（如好友页修改/删除、家族/联盟整组、拒绝列表删除）点击不派发（`UIElement::send_cursor` 对 DISABLED 早退），与 oracle 灰态不可操作语义一致。

## 未完成/限制
- 好友增删、组队邀请/驱逐、家族/联盟真实数据流依赖其他在线角色与文字输入；本端 SDL Textfield 无法由注入脚本键入（历史结论），未做双开全流程；相关 handler→模型→UIUserList 刷新链路已由静态守卫与模型单测覆盖。
- 窗口打开偏左上属既有多分辨率策略问题（可拖窗 `SavedPosClamped` 只夹回视口、无居中；`center_on_screen()` 存在但不用于可拖窗），与本次五页签修复无关，另案处理。
- 客户端在测试末尾两次静默退出（无 Application Error 事件），发生在打开/关闭误触的「搜索」浮窗后，未复现稳定路径，暂不归因于本次改动；实机证据均在退出前取得。
