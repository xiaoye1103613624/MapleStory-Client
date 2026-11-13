# UIUserList 家族页（共用窗第 3 页签）实现逻辑

> 续作来源：dsh 会话 `session-b251b7ee-938b-45e2-9e83-fb427cb1782b`（2026-11-07，turn 15
> 因模型服务错误中断，用户要求读取会话继续执行）。
> 基准：083/v83 原版；协议事实源 `E:\project\BeiDou-Server_s9`；
> 资源事实源：运行时部署 `E:\MXD\BeiDou-Client-xy\Data\.nxcache\UI.nx`
> （按 R1 以运行时数据为准，不采信打包 `UI.wz`）。

## 1. 定位与数据通路

083 中文客户端「家族」= `GUILD`（G 键），不是独立浮窗，而是
「好友 / 组员 / 家族 / 家族联盟 / 拒绝列表」共用窗（`UIWindow.img/UserList`）第 3 页签。
`r27-oracle` 逐页签 SAD 实证已判定第 3 格不是 BossParty；本切片即按此校准。

会话前半段已把数据从 `UIGuild` 浮窗实例上移到常驻模型：

```
s9 GUILD_OPERATION(0x3A) 回包
  0x1A getGuildInfo 全量 → GuildModel::set()
  0x27 新成员 / 0x2C 退出 / 0x2F 逐出 / 0x3D 在线态 / 0x40 职位 / 0x44 公告
                         → GuildModel::add_member / remove_member / set_member_*
  0x03 解散 → GuildModel::clear()
        ↓
UIUserList(GUILD 页签) 与 UIGuild 浮窗 均只读 GuildModel::get().data()
```

必要性：R28-B2 之后 handler 不再为回包自动开窗；若数据只活在某个 UI 实例里，
登录时的 0x1A 到达时无人开窗 → 数据丢失 → 按 G 打开页面全空。

## 2. 资源节点（运行时 IMG 核验）

`tools/nx_inspect.py list <UI.nx> 2 UIWindow.img/UserList/Guild/GuildInfo` 实测：

| 节点 | 类型 / 尺寸 | 用途 / 落位 |
| --- | --- | --- |
| `guildinfo0` | bitmap 281x27 | 在线家族横幅 @(12,111) |
| `guildinfo1` | bitmap 281x27 | 离线家族横幅 @(12,189) |
| `guildinfo6` | bitmap 282x20 | 公告白条 @(12,78) |
| `guildinfo7` | bitmap 281x18 | 列头「名字/职业/等级/地位」@(12,139)/(12,217) |
| `guildinfo2/3/4/5` | bitmap | 备用横幅/分隔线，本切片未使用 |
| `guildmark` | bitmap 20x20 | 家族徽章，本切片未使用 |
| `Btnotice` `BtInvite` `BtWithdraw` `BtKick` `BtWhere` `BtWhisper` `BtChat` `BtUp` `BtDown` `BtGuildBBS` `BtPartyInvite` `BtInfo` | 四态按钮容器 | 12 个页内按钮 |

导出位图与逐像素测量脚本/结果在 `evidence/guildpage/`（`tools/_measure_academy.py`，
`native_top_3x.png` / `native_bottom_3x.png` / `banners_sheet.png` / `btn_sheet.png`）。

底图（2026-11-08 更正）：三张 312x389 互斥底经 5 张 oracle 帧 meanSAD 量出
好友=`backgrnd3`、**家族/家族联盟=`backgrnd2`**、组员/拒绝列表=`backgrnd`；
`change_tab()` 按页签只选一张，禁止叠底。此前「家族页用 backgrnd / 联盟页只有美术」
的结论是错的，见 `userlist-five-tabs/notes/diagnosis-and-plan.md` §3。

## 3. 绘制与行命中

`UIUserList.cpp` 文件级常量（绘制与命中必须共用，否则点行选中会错位）：

| 常量 | 值 | 依据 |
| --- | --- | --- |
| `GUILD_ROW_H` | 15 | 原生行带实测 |
| `GUILD_ONLINE_Y` / `GUILD_ONLINE_ROWS` | 157 / 2 | 在线横幅 y111、列头 y139 之后 |
| `GUILD_OFFLINE_Y` / `GUILD_OFFLINE_ROWS` | 217 / 5 | 离线横幅 y189、列头 y217 之后 |

- `draw_guild_tab()`：未入会时在米色标题条（烘焙在底图）写「没有参加的家族」；
  入会时画公告白条 + `guild_notice_text`；随后按在线/离线两组画横幅、列头与成员行。
- 列文字 x = 27 / 114 / 179 / 222（由 `guildinfo7` 列头墨迹逐像素量得，已含 x12 落位偏移）。
- 职业名取 `Job(static_cast<uint16_t>(m.job)).get_name()`（构造临时 Job，`get_name()` 公开）；
  地位取 `g.rankTitles[m.rank-1]`（rank 越界显示空串）；选中行文字用 `TORCHRED`。
- `update()` 每帧仅当公告文本变化时才 `change_text`，避免逐帧重排字形。
- `guild_row_at()` 把屏幕坐标换算成窗内相对坐标，按与绘制完全相同的可见行数命中，
  返回成员 cid；`send_cursor()` override 在家族页点击时写入 `guild_selected_cid`，
  事件仍交回 `UIDragElement` 处理按钮/拖拽（不吞事件）。

## 4. 按钮 → 协议路由（含 s9 字段序）

| 按钮（IMG 节点） | 落位 | 动作 | s9 依据 |
| --- | --- | --- | --- |
| 邀请 `BtInvite` | (14,306) | `GuildInvitePacket(name)`；优先取 `UICharInfo` 目标名，否则软键盘输入 | case 0x05：`string name` |
| 出去 `BtWithdraw` | (84,306) | `GuildLeavePacket(self_oid, self_name)` | case 0x07：`int cid + string name`，服务端校验必须本人 |
| 位置 `BtWhere` | (156,306) | `FindPlayerPacket(选中成员名)`（检索代理） | — |
| 悄悄话 `BtWhisper` | (232,306) | 软键盘输入正文 → `WhisperPacket` | — |
| 地位↑ `BtUp` / ↓ `BtDown` | (14,328)/(84,328) | `GuildChangeRankPacket(cid, 当前 rank ∓1 夹到 [2,5])` | case 0x0E：`int cid + byte newRank`，**绝对职位**且 `(2,5]` |
| 删除 `BtKick` | (156,328) | `GuildExpelPacket(cid, name)` | case 0x08：`int cid + string name` |
| 对话 `BtChat` | (232,328) | 展开聊天输入行并预置 `/g ` | `UIChatBar::input_text_enter_callback` 的 `/g` → `MultiChatPacket(GUILD)` |
| 通知 `Btnotice` | (287,78) | 软键盘输入 → `GuildNoticePacket(notice)` | case 0x10：`string notice`（≤100 字） |
| 留言板 `BtGuildBBS` | (120,356) | `emplace<UIGuildBBS>()` + `request_list(0)` | BBS_OPERATION(155) |
| 邀请组队 `BtPartyInvite` | (196,356) | `PartyInvitePacket(选中成员名)` | PARTY_OPERATION(124) |
| 资料 `BtInfo` | (268,356) | `emplace<UICharInfo>(选中成员 cid)` | — |

所有「行操作」按钮统一要求先点选成员行；未选中时通过 `guild_hint` 在聊天栏提示，
不发包。`BT_TAB_UNION`（第 4 页签）显式不切页。

## 5. 键位与文案

- `IO/UIStateGame.cpp`：`KeyAction::Id::GUILD`（v83 G 键）并入共用窗分支，
  映射到 `UIUserList::Tab::GUILD`；原来打开 `UIGuild` 浮窗的 case 删除。
  浮窗仍由底栏「社区→公会」钮保留入口，未删除窗体本身。
- `KeyAction::Id::BOSSPARTY` 仍指向同一家族页签（既有版式校准，保留）。
- i18n：`locale/{zh-CN,en-US}.ini` 各新增 6 键
  `guild.page_none` / `guild.notice_none` / `guild.need_select` /
  `guild.invite_prompt` / `guild.notice_prompt` / `guild.whisper_prompt`；
  构建脚本已把两份 ini 复制到部署目录。

## 6. 验证门禁（2026-11-07 23:23）

| 门禁 | 结果 |
| --- | --- |
| `python tools/gen_opcode_matrix.py` | 重算成功：client out 148/148 全有构造点，never-constructed=0，stale 注释=0 |
| `tools/build_smoke.bat` 协议门禁 | `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)` |
| MSBuild Release\|x64 | `EXITCODE=0`，0 error；仅既存 LNK4098/LNK4099 警告 |
| 部署 | `E:\MXD\BeiDou-Client-xy\MapleStory.exe` 23:23:05，4,896,768 B；locale 2 文件已同步 |

## 7. 显式预留（不假装闭环）

1. **真机冒烟未跑**：需用户在屏前按 G → 检查第 3 页签渲染、行点选、12 按钮发包与 s9 回包；
   本环境抓屏历史全黑，无法自截实机画面。
2. 第 4 页签「家族联盟」已于 2026-11-08 按运行时 `GuildUnion` 子节点（12 按钮 +
   7 位图 + 独立 `backgrnd`）还原，不再是预留；按钮语义映射待实机确认，
   见 `userlist-five-tabs/notes/diagnosis-and-plan.md` §7。
3. `UIChatBar::change_target(int32_t)` 仍是全客户端共用的空实现（GUILDCHAT 等键也走它）；
   本切片不改公共聊天行为，家族对话改走已接好的 `/g` 别名。
4. 在线 2 行 / 离线 5 行无滚动条；成员数超出可见行时暂不分页，后续需补 Slider。
5. 未新增家族页专用快捷键，避免与既有 `A/G/C/I/L/E/M/P/W` 键位冲突；操作走鼠标按钮。
6. `guildmark`（徽章）与 `guildinfo2..5` 备用件未绘制：原生家族页这些件不在当前截图可见区，
   待有带徽章家族的实机截图再校准。

## 8. 本切片文件清单

```
新增  Character/GuildModel.{h,cpp}                    （会话前半段，数据层）
      docs/features/social-loop-audit-2026-11/notes/guild-tab-implementation.md
修改  Net/Handlers/GuildHandlers.cpp                  （会话前半段，模型驱动）
      IO/UITypes/UIGuild.{h,cpp}                     （会话前半段，改读模型）
      IO/UITypes/UIUserList.h                        （家族页声明 + send_cursor）
      IO/UITypes/UIUserList.cpp                      （本轮：资源/绘制/命中/12 按钮路由）
      IO/UIStateGame.cpp                             （本轮：G 键接共用窗家族页）
      locale/{zh-CN,en-US}.ini                       （本轮：6 键 × 2）
      docs/features/s9-protocol-align/protocol/opcode-matrix.json（重算）
      docs/features/social-loop-audit-2026-11/README.md（§五-B 收口 + §六）
```
