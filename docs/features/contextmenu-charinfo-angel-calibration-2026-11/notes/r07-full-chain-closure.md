# R07 — 右键链路全子 UI 闭环与全问题修复（2026-11-16）

## 缘起
用户连续三问："右键限制和功能都实现了嘛？功能列表所有功能都闭环了嘛？点击后打开/展开的 UI 上功能和样式都闭环了嘛？"，并要求"全部都补齐，做功能的同时涉及到有问题的都罗列一下，可能都需要解决"。本轮对右键菜单 8 项功能逐条审计其发包→服务端 opcode→handler→UI 全链路，并修复所有发现的问题。

## 一、右键菜单 8 项功能闭环审计（全部已实现）
| 菜单项 | 客户端出口 | s9 opcode/handler | UI 结果 | 结论 |
|---|---|---|---|---|
| Info 查看情報 | CharInfoRequestPacket(cid) | CHAR_INFO → CharInfoHandler | UICharInfo 全字段/子窗 | ✅ 闭环（第二/三轮已补 UI） |
| Save 保存名字 | 剪贴板 + 拉起聊天输入 | 本地行为 | 聊天输入态 | ✅ 闭环（Angel 语义） |
| Trade 申请交易 | TradeInvitePacket(cid) | 交易邀请 → TradeHandler | UITrade 交易窗 | ✅ 闭环（本轮补拖拽发包+校验） |
| Whisper 悄悄话 | set_whisper_target(name) | 私聊包 | 聊天密语态 + 回显 | ✅ 闭环（本轮统一回显出口） |
| Party 组队邀请 | PartyInvitePacket(name) | 组队邀请 | 被邀请方 UIYesNo | ✅ 闭环（本轮补邀请门禁） |
| Guild 公会邀请 | GuildInvitePacket(name) | 公会邀请 | 被邀请方 UIYesNo | ✅ 闭环（本轮补 GameOpt 门禁） |
| Claim 举报 | ReportPlayerPacket(name,desc,type,reason) | SUE → ReportHandler → reportResponse | UIReport 理由/描述/回执 | ✅ 闭环（本轮大改，见二） |
| Family 家族登记 | FamilyAddPacket(name) | 家族邀请 | 被邀请方确认 | ✅ 闭环 |

弹出条件（UIStateGame::rightclick）：仅命中玩家身体矩形（脚底锚 72×110）才弹，空地/怪物/NPC 不弹（第一轮已实机验证）。

## 二、本轮修复的问题清单（用户要求"都罗列"）
### 举报链路（问题最集中）
1. **打开即发包**：旧 UIReport 构造期从 UICharInfo 兜底取名即刻 `ReportPlayerPacket` 发包 → 打开窗口=已举报。修复：目标名经构造/set_target 传入，构造期不发包，仅 `submit()`（用户 Enter 确认）发一次。证据：`UIMiscShells.cpp::UIReport` 构造/submit。
2. **重复提交**：加 `submitted` 布尔门闩，提交后 `send_key` 直接 return，等回执。
3. **理由硬编码/无选择**：补 `1-4` 键选理由，状态栏实时显示 `report.reason.0..3`（非法程序/骚扰/诈骗/其它）；描述走 `D` 键 SoftKey 输入。
4. **无回执 UI**：`SueCharacterHandler`（`WaveAShellHandlers.cpp:2601`）收到 mode 后调 `UIReport::on_report_result(mode)`，成功/4 类失败分别显示 `report.result.success` / `report.fail.1..4`。
5. **服务端成功不回包**：`BeiDou-Server_s9/.../ReportHandler.java` type==0 成功分支原本无响应 → 补 `sendPacket(PacketCreator.reportResponse((byte)0))`；type==1 次数用尽原穿透 → 补 else 回 mode=2 并 return；`chatlog==null` 改空串；新增 `sanitize()`（去控制字符/trim/限长 200）。**注意：服务端改动需用户重启 s9 才能实机验证回执。**
6. **locale 显示 raw key**：客户端从 CWD 的 `E:\MXD\BeiDou-Client-xy\locale\` 读文案，非源码 build-tooltip 目录 → 同步 16 条 `report.*` 到部署 zh-CN.ini/en-US.ini。
7. **文字重叠（R08/R09 实机发现并修复）**：`Report/backgrnd`(154×166) 烘焙了标题/粉色 "YOU ARE REPORTING" 条/FOR 标签/三行警告文字/INCLUDE CHAT LOG，但 relY 48-70 是米色空白输入框。R08 首版仍把 `target_label` 画在 relY=20（压粉色条）、`>` 标记画在 relY=60/76/92（压 FOR 与警告文字）。R09 修复：目标名移到米色框 `(10,51)`、删除 `>` 标记、理由改 1-4 键选（背景不烘焙理由行）。实机 `evidence/live_r09_report.png` 确认无重叠。

### 交易链路
8. **拖拽加物品无效**：UITrade 未 override `send_icon`，默认实现吞掉拖拽 → 新增 `send_icon`，把光标减 position 反解自己侧 3×3 槽，校验 iv 1..5、pos>0 后派发 `TradeAddItemPacket(iv,pos,1,slot)`（字段顺序对齐 s9 SET_ITEMS 0x0F）。
9. **非法槽位/金币**：`add_item()` 忽略 slot<1||>9；`prompt_meso()` 限 0<amt<=2100000000。

### 邀请/弹窗/聊天
10. **公会邀请无 GameOpt 门禁**：`GuildHandlers.cpp` 收到邀请不检查玩家"接受邀请"设置 → 加 `UIGameOpt::accepts(UIGameOpt::GAME_INVITE)`，拒绝则 `DenyGuildRequestPacket(gname)`。（曾误写 `UIOptionMenu::accepts` 编译失败，已改对类名。）
11. **弹窗空壳可点**：`UIMiscShells.cpp::load_window_bg()` 与 `UINotice.cpp` UIYesNo 按钮加载加 `UiAssetResolve::usable()` guard，防止 img 节点为空时画出空白但可点的窗。未拆 UIElement::Type（模态互斥，拆 EnumMap 风险大于收益）。
12. **私聊回显不统一**：`UIChatBar.h` 新增 public 出口 `send_whisper_external`，focus_whisper 的 SoftKey 回调与其它入口统一走 `send_whisper(target,text)` 暂存正文，`WhisperResult` 成功时统一回显。

### CharInfo 数据健壮性
13. **CharInfoHandler 边界防御**：包长 `<14` 直接 drain return；宠物最多 3 只且每次读前 `available()` 守卫；wishlist_size 改 `uint8_t`；book/medal 字段全部 `available()` 守卫；medal quest 最多 12 条。

## 三、验证状态
- 构建：`tools/build_smoke.bat` EXITCODE=0 / GATE OK，最终 exe 20:48:11（晚于 UIMiscShells.cpp 20:16:15）。
- 实机：auto login 进图 → 右键玩家 → 菜单含"举报" → 打开举报窗，`evidence/live_r09_report.png` + `_r09_report_crop.png` 确认标题/粉色条/米色框目标名/FOR/警告文字/底部蓝条状态均无重叠。
- **未能实机复现项（环境限制，非代码缺陷）**：按键送达——SendKeys 与 keybd_event 均无法把 `1`/Enter/Esc 送进该 SDL3 客户端（历史多次记录 raw 输入在 UIPI 下被拦），故 1-4 选理由、Enter 提交、D 描述、Esc 关闭的实机交互未截图确认；代码层 `UIReport::send_key` 四分支齐全。服务端 ReportHandler 回执需用户重启 s9 后实机确认。

## 四、改动文件
客户端（本仓库）：
- `IO/UITypes/UIMiscShells.cpp/.h`（举报窗重写 + ContextMenu + load_window_bg guard）
- `IO/UITypes/UITrade.cpp/.h`（send_icon + 校验）
- `IO/UITypes/UINotice.cpp`（UIYesNo guard）
- `IO/UITypes/UIChatBar.cpp/.h`（私聊回显出口）
- `Net/Handlers/GuildHandlers.cpp`（公会邀请门禁）
- `Net/Handlers/PlayerInteractionHandlers.cpp`（CharInfo 边界防御）
- `IO/UITypes/UICharInfo.cpp/.h`（人气快捷键禁用态门禁，见第三轮）
服务端（另一仓库 `E:\project\BeiDou-Server_s9`）：
- `gms-server/src/main/java/org/gms/net/server/channel/handlers/ReportHandler.java`
部署配置：
- `E:\MXD\BeiDou-Client-xy\locale\zh-CN.ini` / `en-US.ini`（16 条 report.*）
- `E:\MXD\BeiDou-Client-xy\xy-config.yaml`（验证期 auto=true，已恢复 false）
