# 帮助入口取证 —— 「帮助」= ENTER_MTS(0x9C)，落点由服务端 use_mts 决定

结论先行：原生状态栏「帮助」不是本地窗，而是发 **ENTER_MTS(0x9C)**；服务端在
`use_mts=false` 时把这条包改派成 `npc/9900001.js`（BeiDou 脚本中心）的 NPC 对话。
本仓早先在 `help-window.md` §2 记的两条阻塞（「须新增 GAME_OPTION 协议线」「须 IDA
定位 fallback 串」）**均被本轮取证推翻**：不需要新协议，fallback 就是复用 0x9C。

## 1. 服务端链路（逐跳可复核）

| 跳 | 位置 | 事实 |
| --- | --- | --- |
| 收包 | `gms-server/src/main/java/org/gms/net/PacketProcessor.java:216` | `registerHandler(RecvOpcode.ENTER_MTS, new EnterMTSHandler())` |
| opcode | `gms-server/src/main/java/org/gms/net/opcodes/RecvOpcode.java:159` | `ENTER_MTS(0x9C)` |
| 开关分叉 | `.../handlers/EnterMTSHandler.java:54` | `if (!GameConfig.getServerBoolean("use_mts")) { openCenterScript(c); return; }` |
| 改派 | `.../handlers/EnterMTSHandler.java:283` | `NPCScriptManager.getInstance().start(c, NpcId.BEI_DOU_NPC_BASE, null)` |
| npc id | `gms-server/src/main/java/org/gms/constants/id/NpcId.java:40` | `BEI_DOU_NPC_BASE = 9900001` |
| 脚本落点 | `.../scripting/npc/NPCScriptManager.java:138` | fileName 为空时回落 `getInvocableScriptEngine("npc/" + npc + ".js")` → `scripts-zh-CN/npc/9900001.js` |
| 开关默认 | `.../resources/db/migration/V1.7.0__create_game_config.sql:51` | `('server','Game Mechanics','java.lang.Boolean','use_mts','false','是否开启拍卖行(enable mts)')` |
| 脚本内容 | `scripts-zh-CN/npc/9900001.js:44-62` | `cm.sendSimple(OldTitle + 余额四行 + #L 菜单)`；GM 追加 61–68 那排（`:55-61`） |

全服务端检索 `BEI_DOU_NPC_BASE` / `9900001`（排除 scripts 与 sql 迁移）只有
`EnterMTSHandler` 一处起该脚本；另一条理论上能到达 `npc/9900001.js` 的路径是
`NPCTalkHandler` 点**地图实体 NPC 的 oid**（`NPCTalkHandler.java:59-82`），而 9900001
不作为实体摆图。故「点按钮出脚本中心对话」只有 0x9C 一条解释。

## 2. 客户端侧（参考端 `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe`，IDA gui session `ida_beidou`）

| 证据 | 地址 | 说明 |
| --- | --- | --- |
| `UI/StatusBar.img/BtNPT` | `.rdata` `0xb3bab4`，xref `0x8d3005` in `sub_8D01B2` | 状态栏「帮助」钮的位图根；BtNPT = NPC Talk 家族 |
| 控件注册 | `0x8d3033-0x8d3068` | `push esi(this); push 3E9h(=1001 控件 id); push 275h(=629 x); push 21Fh(=543 y); push 6; push &cb` → 与本仓 `UIStatusBar.cpp` 既有注释「BtNPT=1001/(629,543)」逐字节吻合 |
| 真拍卖行窗体在原生存在 | `0xaf5fb0 / 0xaf605c / 0xaf6074 / 0xaf6098 / 0xaf61d0` | `UI/ITC.img/{Auction/backgrnd, BtAuction, BtSell, Auction/BtBid, Auction/backgrnd1}`，宿主 `sub_5B2CA2`（ITC 窗）——即 `use_mts=true` 那一侧的 UI 素材确实存在 |
| **无**本地说明书窗 | 全文件扫描 | `Npt_GuideLine` / `NptGuide` 在 `BeiDou.exe`、随带 DLL 与 `Data/` 的明文与 4 字节序列中**零命中**；`Npt_GuideLine.img/NptGuide` 的 Text00..48 只是素材残留，原生没有代码去读它 |
| **无**硬编码 9900001 | 全文件扫描 | `0x970FE1` 的 LE 字节序列与 ASCII `9900001` 在 exe/dll/Data 均零命中（仅 `Data/Item/Consume/0243.img`、`Data/Quest/Check.img` 命中，属无关字节巧合）→ 客户端不持有该 id，进一步证明「脚本选择」完全在服务端 |

未取证项（诚实标注）：`BtNPT` 点击到发 0x9C 的那条 exe 内分发链**没有挖到发送点**——
状态栏模块（`sub_8CFAD8`~`sub_8D3ADF`）内没有对控件 id 1001 的 `cmp` 分发，推测走 SUI
回调表，继续挖成本高。故 0x9C 的归属由「服务端唯一可达路径 + 参考端实测现象」推定，
并以真机点一次作为最终判定（见 CHECKLIST）。

## 3. 本轮改动

- 帮助两入口（`UIStatusBar.cpp` BtNPT / 菜单 Help）与菜单「拍卖行」→ `EnterMtsPacket().dispatch()`；
  拍卖行原先用 `FindPlayerPacket` 冒充检索代理（自创），一并撤掉。
- 商城 `BtHelp` / CharInfo `BtFAQ` / ChatBar `BtHelp` 三颗问号统一走同一通道；
  **注意**：这三颗的原生行为未取证（历史上是 r209/r210/r228 自创接到 UIHelp），
  统一口径见 `notes/open-issues.md`。
- 删除本地说明书窗 `UIHelp.h/.cpp` 及工程条目、清掉 UILogin / UIMiscShells 两处 `H`
  开发期入口与 `UIHelp::send_key` 的 S/T/G/C 热键。
- 出站/回程无需新增：`Net/OutPacket.h:198 ENTER_MTS=156`、
  `Net/Packets/MiscOutPackets.h:14 EnterMtsPacket`（此前唯一调用点是 UIMTS 的 `E` 热键）、
  回程 `Net/Handlers/NpcInteractionHandlers.cpp:43-67` 按 npcid 渲染不要求画面有实体 NPC、
  `#L` 回复 `IO/UITypes/UINpcTalk.cpp:373`。

## 4. 显式预留

`use_mts=true` 时服务端会走真 MTS 流程，本端只有文本空壳
（`IO/UITypes/UIMTS.cpp` 180 行 + `Net/Handlers/WaveAShellHandlers.cpp:3200-3363`
把 MTS_OPERATION 打成 "MTS sell ok" 之类 tip）。真拍卖行 UI 属独立立项，素材取证入口
见本文 §2 的 `UI/ITC.img/Auction` 五件。

## 5. 旧文档处置

- `help-window.md`：已在文件头加「⚠ 本文 §1、§2 的结论已被推翻」对照表，指向本文；
  §1 里「nx 字符串表是 UTF-8」的取证仍有效，其余按该表作废。
- **尚未改写、仅在此登记**（真机验收后随 T9 一并处理，避免与并行会话撞文件）：
  - `PLAN.md:40` RC9 行 —— 现写「接 `Npt_GuideLine` 文本 + `BtNPT`」，正确方案是发 0x9C；
  - `evidence/w2-hud-buttons/coords.md` §T-1 —— 整节描述 `UIHelp` 的取串/换行折叠，随窗口删除失效；
  - `beidou-v17-parity/notes/r112-gamemenu-help-joypad.md`、`r209-stub-to-packet-batch.md`、
    `r210-charinfo-cashshop-wire.md` —— 记录的是「自创接到 UIHelp」的历史，保留为过程档案，
    不再当作待办依据。

## 6. 真机验收（2026-11-02 19:0x–19:32，构建 18:46:17 / md5 `803d1e5d…`）

驱动方式：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`（PID 24148，18:49:15 起，Admin/LV1 彩虹岛·番加田），
`_drive.py` PrintWindow 旁路抓帧 + SendInput 合成点击；每次打键前 `foreground()` 断言命中
（本机 `focus ok=True`，本轮没遇到 360 抢前台）。

### 6.1 通过项

| 步 | 结果 |
| --- | --- |
| 点底栏「帮助」(BtNPT) | `[SEND] PacketDump : 9c 00` ⇒ **ENTER_MTS(0x9C) 出站成立** |
| 服务端回程 | `[RECV] 30 01 04 e1 0f 97 00 04 00 ef 01 09 09 09 09 09 …` ⇒ opcode `0x0130` NPC_TALK、**npcid `0x00970FE1 = 9900001`**、msgtype `04`=Simple、正文首字节 5×`09` = `OldTitle` 的 5 个制表符 |
| 窗口呈现 | `UINpcTalk` 弹出，左侧立绘 + 名牌「昨日小睦」，正文首行「欢迎来到BeiDou脚本中心」 |
| 关闭 | 点「结束对话」与 Esc 均正常收起，连续 5 次开/关不卡（服务端 CM 未悬挂） |
| 命中区 | 实测 x≈**640..688** 命中、x=635 落空；与位图可视框 637..688 一致 ⇒ 无命中区偏移 |

对照项（证明点击链路本身没坏）：同排「目录」(735,732) 开主菜单、「界面」(774,727) 开快捷面板、
「商城」(607,727) 发 `[SEND] 28 00` 且服务端回 `7f 00 …`。

### 6.2 未通过项（本轮新发现的**阻塞缺陷**，不在原计划范围）

对话正文渲染坏了，两处：

1. **`\r\n` 不换行**：`9900001.js` 的「当前点券 / 当前抵用券 / 当前信用券 / 当前金币」四行
   被挤成一行后按框宽软折行；
2. **`#L<n>#…#l` 选项不可用**：渲染成一串红色实心块（字被色块盖住），且**点击选项不发
   `NPC_TALK_MORE(60=0x3c)`** —— 实测在选项文字上点一次，出站日志零行。

根因定位到一处：`IO/UITypes/UINpcTalk.cpp:472` `format_text()` 只处理 `#p`/`#h`/`#t` 三个令牌，
**完全没有 `#L`/`#l` 分支**，也没有对 CR 做归一；未识别的 `#L3#` 于是原样漏进
`GraphicsGL::LayoutBuilder`，被按颜色/控制令牌误解析（截图 `live/npctalk-text-defect.png`）。

> 结论：**路由侧（本轮的改动目标）已真机成立**；但「帮助」对用户真正可用还差
> `UINpcTalk` 的 `#L/#l` + 换行解析，属另一模块，须单独拍范围（见 open-issues）。

### 6.3 本轮改动的可达性说明

`BT_MENU_AUCTION` / `BT_MENU_HELP`（`UIStatusBar.cpp:918/953`）在**当前 classic 皮肤下点不到**：
它们由 `toggle_menu()`（`:1624-1633`）激活，属 modern 顶栏 submenu；实测底栏「目录」只开
「主菜单」四项（频道变更 / 游戏设置 / 系统设置 / 游戏结束）。⇒ 这两处改动**仅编译级验证**，
真机不可达，不记作已验收。
