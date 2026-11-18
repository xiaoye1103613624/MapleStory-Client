# 聊天区 UI + 协议还原 · 2026-11-14

对照：视频 `聊天区域功能展示.mp4`（抽帧 `screens/chat-vid-*.png`）+ 截图「聊天选择对话的类型.png」+ Angel IDA `MakeCtrlEdit` / `OnKey` / `SendGroupMessage`。

## 根因

| # | 现象 | 根因 | 处置 |
| --- | --- | --- | --- |
| 1 | 频道下拉顺序/文案不对 | `ChatTargetIndex` 为 ALL/WHISPER/…/MEGAPHONE；Angel 为 对好友→对群→…→对所有人（0..7） | 重排枚举 + locale |
| 2 | 「喇叭」项 | 非原生下拉项；原生第 2 项是 **对群**（`GetOnlineFriendIDByGroup`） | `TARGET_GROUP` + MultiChat type0 分组过滤 |
| 3 | 夫妻发包错 | `SpouseChatPacket` 只写正文；s9 先 `readString` 收件人 | 补配偶名+正文 |
| 4 | 默认频道 | `classic_chat_target_id=0` 变成「对好友」 | 默认 `7`（对所有人） |

## Angel 发包矩阵（证据）

- `MakeCtrlEdit` @ `0x8dfb36`：AddItem 下标 0..7
- `OnKey` Enter：`0..4` → `SendGroupMessage`；`5` → Spouse(121)；`6` Whisper；`7` GeneralChat
- MultiChat opcode **119**：type 0 buddy / 1 party / 2 guild / 3 alliance（与 s9 `MultiChatHandler` 一致）
- Spouse opcode **121**：`EncodeStr(partner)+EncodeStr(msg)`

## 改动文件（客户）

- `IO/UITypes/UIChatBar.h` / `.cpp` — 枚举、路由、对群、send_spouse_chat、斜杠 `/group`
- `Net/Packets/MessagingPackets.h` — SpouseChatPacket 双字符串
- `Character/BuddyListModel.h` — `active_group`
- `IO/UITypes/UIUserList.cpp` — 选中好友时写入 active_group
- `IO/UITypes/UIMiscShells.cpp` — MateMessage 走 send_spouse_chat
- `locale/zh-CN.ini` / `en-US.ini` — 八键文案 + buddy/spouse 提示

服务端：**未改**（协议已对齐；夫妻仅客户补配偶名）。

## 像素

列表配色/行高沿用 `coords-targetlist.md`（沿 153,153,153 / 内区 51,85,119 / 高亮 85,153,187 / 行高 16）；顺序与视频 `chat-vid-11/12` 一致。

## 真机热修 · 2026-11-14 下午（+号 / 白幕 / 层级）

| # | 现象 | 根因 | 处置 |
| --- | --- | --- | --- |
| A | 点蓝底 `+` 无反应 | 收起态 `send_cursor` 对右段 `x>=dim-80` 直接 `return IDLE`，BtMax 永远收不到点击 | 右段落到 `UIElement::send_cursor` |
| B | 下拉被白色大块盖住 | 列表 ColorBox 写成 `Color(153.0f,…)`，浮点 ctor 要 `[0,1]`，>1 在 GL 饱和成白 | 改 `Color(0x999999FFu)` / `0x335577FF` / `0x5599BBFF` |
| C | 列表在历史下面 | `draw` 先画列表再画历史消息 | 历史底板+消息先画，列表后画 |

## 真机热修 · 列表与触发钮重叠

| # | 现象 | 根因 | 处置 |
| --- | --- | --- | --- |
| D | 末行「对所有人」与 chatTarget 钮叠字 | 列表画在触发钮之后且底边=钮顶，末行盖住钮顶；行文未走 `y_for_ink_top` | 锚点 `TARGET_BOX_Y-1-list_h`；draw 序：历史→列表→触发钮；行文 `y_for_ink_top` |

改动：`IO/UITypes/UIChatBar.cpp`。

## 构建 / 部署

- Staging：`build-chat-ui\MapleStory.exe`（5115392）
- Live：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` — **MATCH**
- SHA256：`413121B33BCE4410A03AD9852AB4306664F0859816F8A61C60BB3B86A3B23120`
- MSBuild Release|x64，exit 0

## 自测清单

1. 点「对所有人」→ 8 项序：对好友/对群/对组队/对家族/对联盟/对夫妻/悄悄话/对所有人；高亮底项
2. 切频道后面钮文案同步；Enter 对应发包
3. 对好友 / 对群（好友列表点选分组成员后）有在线收件人
4. 对夫妻：已婚通、未婚提示「你还没有结婚…」
5. 悄悄话 → 弹「请输入悄悄话的对象」→ 回填发送
6. 历史展开/滚动、收起灰带、多分辨率不与 HUD 重叠

## 残差

- REA MCP 本轮不可用；以 IDA + 视频为准
- 「对群」依赖好友 `group` 字段；未选好友时回落到首个在线好友组名
- 喇叭不进下拉（Angel 亦无）；道具喇叭路径未本轮点测
- 真机夫妻若仍异常，再抓包核对 Spouse 字段
