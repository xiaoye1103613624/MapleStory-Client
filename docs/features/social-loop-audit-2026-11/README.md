# 社交系统闭环审计 · 家族 / 组队 / 远征 / 结婚 / 学院（2026-11-06）

> 触发：用户问「家族、组队、远征、结婚、学院等社交类的功能是否闭环了？」，随后授权按
> 结婚 → 冒烟归档 → 联盟补完 → 学院/远征调研 的顺序推进。
> 方法：全部以现场源码为证（客户端 `E:\project\MapleStory-Client` × s9 `E:\project\BeiDou-Server_s9`），
> 旧台账结论逐条复核，能证伪的标证伪。
> **对照基准（用户 2026-11-06 明确裁决）：功能与资源一律参照 GMS 083 原版**
> （`BeiDou-ClientV17.7\BeiDou.exe` + 其 Data 资源；协议侧 s9 即 v83 派生）。
> `port-095`（浩浩岛095/PLUS2）只是独立在研的端口移植线：v95 的 opcode 表 / UI / 资源
> **不得**用作本线功能参照；找资源（时装/图标/道具）也须选 083 适配版本。


## 一、五系统终判

| 系统 | 判定 | 依据 |
| --- | --- | --- |
| **组队 Party** | ✅ 闭环 | `UIParty` 实体窗 + c2s `PARTY_OPERATION=124`/`DENY_PARTY_REQUEST=125` + s2c PARTYLIST 全链；组队搜索以 `PARTY_SEARCH_START/UPDATE(222/223)` 代理（s9 0xDC 空 stub，留档视同消化）；GameOpt `PARTY_APPLY/PARTY_SEARCH` 拦截已接。冒烟=#34 |
| **家族 Family（中文客户端旧称「学院」）** | ✅ 闭环 | c2s 145–153 全 9 包 + s2c 94–104 全 handler + `UIFamily`/`UIFamilyTree` 实体 + CharInfo/状态栏/右键菜单入口 + GameOpt `FAMILY_APPLY`；R28-B2 后回包不再无端开窗。冒烟=#34 |
| **结婚 / 婚礼** | ✅ 双端闭环（2026-11-06 本轮完成） | 见 [wedding-flow](../wedding-flow/)。此前判「壳/不做」**前提已证伪**：s9 婚礼后端完整且注册（RingAction 9 mode / Wedding 3 mode / WeddingTalk / Amoria NPC 9201008-14 / Chapel+Cathedral 事件脚本），缺口纯在客户端不发；本轮补齐 c2s 137/138/139 + s2c 字段修正 + MarriageInfo 数据层。合影仍预留（s9 需外部 WvsMapGen）。冒烟=#32 |
| **学院 Academy（韩系独立导师系统）** | ❌ 不可端内闭环 | **两义分判（2026-11-07 用户指认 083 键位表后校准）**：① 083 键位 F = v83 action **26 FAMILY**，中文客户端称「学院」——这就是家族窗 `UIFamily`，功能早已闭环，唯 **F 键入口是死键**（枚举编号错位，MEDALS 槽无 case），已随本轮接线（见 f-keymap-action-translate 补记 + pending #35）；② 韩系导师 Academy（师生/学分）只见于 095 线符号，083 清单 `UIWindow.img` 无此窗，s9 零命中 ⇒ 非 083 原版功能，不做 |

| **远征 Expedition（BossParty 招募面板）** | 🟡 玩法可参与，面板非 083 原版缺口 | s9 **有**完整 Expedition 服务端（`server/expeditions/Expedition(Type|BossLog)`，扎昆/章鱼/阿里安特竞技场等事件驱动，经 NPC 脚本 + serverNotice 6 号文本 + 普通组队入队运作）——083 原版玩家即以此流程参加远征，客户端现有队伍+NPC 链已覆盖。独立招募面板（ExpeditionIntermediary/v95 UserList Boss 页）是 **095 线证据**：GMS113 `UIWindow.img` 的 `UserList` 页签实为「好友/组员/家族/家族联盟/拒绝列表」五页（r27-oracle 逐页 SAD 实证），**不存在 BossParty 第 6 页** ⇒ 本端按 083 基准**不算缺口**；`UIUserList` 第三格曾被误当 BossParty 用的注释应改按「家族」页校准（见 §五-B 后续小项） |

## 二、相邻社交面状态（顺带复核）

- **好友**：BUDDYLIST c2s 错配 63→130 已修（2026-11-02），链路对 s9 生效。
- **公会 / 联盟**：`UIGuild` + `Alliance` 复刻；本轮联盟补完 —— s2c 0x03 邀请 YesNo（消费 GameOpt `UNION_APPLY`，
  gameopt-invite-mask 的显式预留随之消解）、0x0D 公会名全块解析（`set_guild_names`）、0x02/0x0E/0x18/0x1A/0x1D
  静默消费、`AllianceAcceptPacket(0x04)`；`ensure_ui` 收口为仅活动窗更新（R28-B2 同类开窗问题清零）。
  仍未接：创建联盟（s9 走 NPC 脚本 `createAlliance`，非 c2s，端内无需新包）、改职衔 0x08 出站（服务端有 handler，客户端缺钮，低优先）。
- **信使 Messenger / BBS / 备忘录 / 密语 / 婚礼聊天 SPOUSE_CHAT**：有窗有包；BBS 完整度、#5b 气泡单帧待真机。

## 三、本轮改动文件清单（结婚 + 联盟）

```
新增    Character/MarriageInfo.h
        Net/Packets/WeddingPackets.h
        docs/features/wedding-flow/README.md
修改    Net/OutPacket.h                          RING_ACTION/WEDDING_ACTION/WEDDING_TALK
        Net/Packets/AlliancePackets.h            AllianceAcceptPacket
        Character/Player.{h,cpp}                 get_marriage()
        Net/Handlers/Helpers/CharacterParser.{h,cpp}  parse_ring3(带存) + parse_marriage_block
        Net/Handlers/SetFieldHandlers.cpp        （登录挂 MarriageInfo）
        Net/Handlers/Helpers/CashShopParser.cpp  （商城登录同链）
        Net/Handlers/WaveAShellHandlers.cpp      MarriageRequest/Result、WeddingGiftResult 重写、
                                                 PartnerMapTransfer 不再开窗
        Net/Handlers/AllianceHandlers.cpp        七分支补全 + ensure_ui 收口
        IO/UITypes/UIWedding.{h,cpp}             键位全接（I/T/S/X/C/B/R），去自动弹软键盘
        IO/UITypes/UIAlliance.{h,cpp}            set_guild_names / set_ranks
        IO/UITypes/UIItemInventory.cpp           求婚盒/请柬/凭据 双击路由
        locale/{zh-CN,en-US}.ini                 wedding.* 24 键（值暂英文，遵 R27-B1c 决策 B）
        docs/features/gameopt-invite-mask/README.md     UNION_APPLY 预留消解
        docs/features/parity-gap-2026-11/README.md      婚礼行改判
        docs/features/s9-protocol-align/notes/gap-inventory.md  137/138 改判
        docs/features/pending-live-tests/README.md      新增 #32/#33/#34
        docs/features/s9-protocol-align/protocol/opcode-matrix.json（重算）
```

## 四、验证

| 门禁 | 结果 |
| --- | --- |
| `gen_opcode_matrix.py --check` | GATE OK（s9 3d5ccc6f8e）；never-constructed=0；collision 139:{WEDDING_TALK,WEDDING_TALK_MORE} 如实入册 |
| MSBuild Release\|x64 | **EXITCODE=0**，0 error |
| 部署 | 旧占用镜像已改名（`MapleStory.exe.bak_20261106` 保留 22:19 原物可回滚）；新 exe 已链接到 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（23:16，4,871,680 B） |
| 真机冒烟 | ⏳ wedding-flow §4 七步 + pending-live-tests #32/#33/#34 |

## 五、决策项收口（按 083 原版基准）

- **A. 学院**：~~B/C 双端立项~~ → **判定为「不做」**。Academy 不是 083/GMS113 原版功能
  （`UIWindow.img` 顶层清单 0 命中，s9 后端 0 命中），做成即偏离基准的加戏。v83 中文所称
  「学院」= 家族，已闭环。本文件即显式预留。
- **B. 远征**：**玩法维持现状**（083 原版就是 队伍+NPC+事件 流程，本端已可参与）；
  独立招募面板非 083 原版 UI，不做。原「遗留小项」`UIUserList` 第 3 页签按 r27-oracle
  实证应是「家族」而非 BossParty —— 已于 2026-11-07 续作完成，见 §六。
- **C. 资源基准**：婚礼请柬/门票/道具图标等资源如需补，一律取 083 适配来源
  （`E:\MXD\适用于083时装`、`动漫皮肤083.zip`、GMS113 原版 Data 等）；095 PLUS2 /
  怀旧岛192 / 浩浩岛 系资源不作本线参照与入库源。

## 六、家族页（共用窗第 3 页签）续作收尾（2026-11-07 晚）

> 触发：dsh 会话 `session-b251b7ee-938b-45e2-9e83-fb427cb1782b` 在 turn 15
> 因模型服务 `112/model_error` 连续重试中断，turn 16–19 全是失败重试/用户中止；
> 用户随后要求「读取 dsh 会话继续执行」。本轮按会话证据续作并收口。

**断点复核**：会话产物里 `Character/GuildModel.{h,cpp}`、`Net/Handlers/GuildHandlers.cpp`、
`IO/UITypes/UIGuild.cpp` 的模型驱动改造与 `UIUserList.h` 的家族页声明都还在，但
`UIUserList.cpp` 已回到 HEAD 原版 —— 家族页实现整体缺失（会话末次编辑是覆盖式重写，
引用了未定义成员且会编译失败），协议矩阵 `client_sent_count` 事实也未重算，构建门禁
停在 `GATE FAIL`。

**本轮补齐**（详见 [notes/guild-tab-implementation.md](notes/guild-tab-implementation.md)）：

| 层 | 内容 |
| --- | --- |
| 数据 | 复用 `GuildModel`：0x1A 全量 + 0x27/0x2C/0x2F/0x3D/0x40/0x44 增量；回包不再为数据开浮窗 |
| 资源 | 运行时 `UI.nx` 核验 `UIWindow.img/UserList/Guild/GuildInfo` 全节点（`guildinfo0..7` + 12 个 `Bt*`），坐标取 `evidence/guildpage/` 逐像素行带实测 |
| 绘制 | 公告白条/在线离线横幅/列头/成员行（名字·职业·等级·地位），未入会显示「没有参加的家族」 |
| 交互 | 成员行点选（override `send_cursor`）+ 12 按钮路由；页签切换联动按钮可用态 |
| 接线 | **G 键**（`KeyAction::Id::GUILD`）改开 `UIUserList(Tab::GUILD)`；`UIGuild` 浮窗保留底栏入口 |
| 文案 | `locale/{zh-CN,en-US}.ini` 各补 6 键（`guild.page_none` 等），构建时已同步到部署目录 |
| 门禁 | `gen_opcode_matrix.py` 重算 → `GATE OK`；MSBuild Release\|x64 `EXITCODE=0`，exe 23:23 刷新 |

**协议依据**（s9 `GuildOperationHandler`，逐字段核对）：0x05 邀请、0x07 离会
（`int cid + string name`，服务端校验必须是本人）、0x08 逐出、0x0E 改职位
（`int cid + byte newRank`，`newRank∈(2,5]`，非增减量）、0x10 公告（`string`，限 100 字）、
0x1A 全量家族信息。

**显式预留 / 未跑项**：

- ⏳ 真机冒烟未跑：需用户在屏前按 G → 看第 3 页签渲染、点成员行选中、逐个点按钮发包；
  本环境无法自截实机画面（历史抓屏全黑问题），不假装已验证。
- 第 4 页签「家族联盟」已于 2026-11-08 按运行时 `GuildUnion` 子节点还原
  （12 按钮 + 7 位图 + 独立底图），不再"只画不切页"；详见
  `../userlist-five-tabs/notes/diagnosis-and-plan.md` §8。
- 家族对话按钮走聊天栏 `/g` 斜杠别名（`UIChatBar::input_text_enter_callback` 已接）；
  `UIChatBar::change_target(int32_t)` 仍是全客户端共用的预留空实现，不在本切片动公共行为。
- 在线 2 行 / 离线 5 行无滚动条（083 页内可见行数如此），成员更多时需补 Slider 分页。
- 未新增家族页专用快捷键，避免与既有 `A/G/C/I/L/E/M/P/W` 全局键位冲突。
