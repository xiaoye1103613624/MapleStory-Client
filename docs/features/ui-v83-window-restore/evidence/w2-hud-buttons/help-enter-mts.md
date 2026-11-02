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
| 本地说明书**文本素材确实存在** | `E:\MXD\BeiDou-Client-xy\Data\UI\Npt_GuideLine.img` → `NptGuide/Text00..48`（zh-CN） | 运行时 dump `E:\MXD\BeiDou-Client-xy\Npt_GuideLine.img_runtime.json` 里 `Text00=" ■ 简介\r\n"`、`Text03=" ■「贩卖」说明\r\n"`、`Text04="1, 物品的贩卖：\r\n在画面最下方的身上道具内…"`，与 `evidence/native-refs-r14/ec520a9f-….png` 那张原生窗体截图**逐行吻合** ⇒ 原生有一个渲染这份说明书的本地窗（带竖滚动条 + 右上 close 页签）。**本行是 2026-11-02 对同节初稿的更正**，初稿见下 |
| **无**硬编码 9900001 | 全文件扫描 | `0x970FE1` 的 LE 字节序列与 ASCII `9900001` 在 exe/dll/Data 均零命中（仅 `Data/Item/Consume/0243.img`、`Data/Quest/Check.img` 命中，属无关字节巧合）→ 客户端不持有该 id，进一步证明「脚本选择」完全在服务端 |

### 2.1 初稿强结论作废（自查纠错）

初稿在 §2 记过一条「**无**本地说明书窗：`Npt_GuideLine`/`NptGuide` 在 exe、随带 DLL 与
`Data/` 的明文与 4 字节序列中零命中 ⇒ 原生没有代码去读它，Text00..48 只是素材残留」。**该结论错误**，错在取证方法：

1. `.img` 是加密容器，对明文做字节扫描天然零命中——「扫不到」不等于「不存在」；
2. 扫描按文件**内容**做，没有覆盖 `Data/UI/Npt_GuideLine.img` 这个**文件名**本身，而它就在客户端目录里躺着。

同时 `ec520a9f` 截图与运行时 dump 两条独立证据都指向「窗体存在、文本读得出」，所以这不是推测，是初稿把结论写过头。

**修正后的口径**：原生本地说明书窗 + `Npt_GuideLine` 文本 = **真实存在**，但它属于 **`use_mts=true` 那一侧的拍卖行流程**（从 ITC/Auction 窗内进入），不是「帮助」按钮的落点。「帮助 → 0x9C → `use_mts=false` → `npc/9900001.js`」这条链路不受本次更正影响——它由服务端唯一可达路径（§1）+ 真机出站 `9c 00` 与回程 npcid `9900001`（§6.1）两头夹住。

> 顺带一条资源不同步事实（仅登记，未处理）：客户端 `Data/UI/Npt_GuideLine.img` 是 **zh-CN** 文案，
> 服务端 `gms-server/wz/UI.wz/Npt_GuideLine.img.xml` 是 **EN** 文案，两份语言版本不一致；
> 将来真做拍卖行 UI 时要按当前客户端语言取哪一份需先定。

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
见本文 §2 的 `UI/ITC.img/Auction` 五件；其**说明书窗**的文本素材见 §2 更正行
（`Data/UI/Npt_GuideLine.img/NptGuide/TextNN`，zh-CN）与截图 `native-refs-r14/ec520a9f-….png`
——原生该窗带竖滚动条，正文按 `TextNN` 逐段拼接，可作为立项时的版式基线。

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

## 7. §6.2 缺陷的取证与处置（2026-11-02 21:0x–22:4x，**代码完成、真机未复验**）

用户口径：「改布局器；颜色按原生的来，或者按冒险岛不同业务不同颜色的逻辑来；先验证」。
下面每条都先有取证，再动代码。

### 7.1 颜色：`#L` 不带颜色，选项继承当前笔色

`scripts-zh-CN/npc/9900001.js:44-62` 是决定性证据——色标只出现在标题串里
（`#e欢迎来到#rBeiDou#k脚本中心#n`），**10 个 `#L<n>#…#l` 令牌内部一个色标都没有**：

```js
text += "#L3#传送自由#l \t #L69#快速转职#l \t #L70#学习技能#l\r\n";
```

⇒ 「选项该是什么色」由脚本自己铺色标决定，`#L` 本身不决定颜色。这同时意味着
「按原生来」和「不同业务不同颜色」是**同一条实现**：布局器只负责继承当前笔色，
谁想要蓝色谁在 `#L…#` 里写 `#r` 之类。因此本仓原先硬编码的
`Color::Name::LIGHTBLUE`（把选项一律涂蓝）属自创，已删除。

### 7.2 排版：原生是**行内**选项，不是每选项一行

同一份脚本用 ` \t ` 把三项排成一行、用 `\r\n` 分行（三列 × 三行 + 一行单项）。
所以选项不是独立控件，它是正文里的一段行内可点文本。旧实现
`c.pos = Point<int16_t>(TEXT_X, 70 + row * 18)` 逐行另起、再用独立 `Text` 覆盖绘制，
与原生版式不符，废弃。

### 7.3 换行根因在共享布局器，不在窗口

`Graphics/GraphicsGL.cpp` `createlayout()` 的分词集合原为 `" \#\t"`，**不含真 0x0D/0x0A**：
WZ 串里的断行是字面量 `\r\n`（反斜杠+字母，由 `LayoutBuilder::add` 的 `case '\\'` 处理），
而网络送来的脚本串是真 CR/LF 字节。真字节进不了分词集合 ⇒ 整段正文被当成**一个词**连着画，
而控制字符本身零宽又不产出字形（`addfont` 只烘焙 ASCII 32..127）⇒ 什么都不画，多行挤成一行。

改动（按用户要求落在布局器，不做局部归一）：

- 分词集合加 `\r\n`；
- `add()` 新增 `case '\r'` / `case '\n'`，且**同一处只断一次**：`\r` 见后继 `\n` 时 `skip=2`
  一并吃掉；`\n` 若词起点已越过本字节（`prev <= first`）则不再重复断，并保持 `skip=0`
  ——控制字符没生成字形，推进量为 0 不会顶开笔位，但 `advances` 仍按字节索引补齐。

回归面核对：全仓检索确认没有任何 C++ 字面量把真 CR/LF 喂给 `Text`，故本次只影响网络串。

### 7.4 命中框从布局反取

`Text` 侧补一条通用能力而非在窗口里手算：`Text::Layout::position_of(size_t pos)`
（`Graphics/Text.cpp`）。`advances` 本来就按字节索引，缺的只是「这个字节落在哪一行」，
所以按行的词跨度 `[words.front().first, words.back().last)` 定位；边界用**严格小于**，
因为上一行的末边界与下一行的首边界是同一个字节，先撞上上一行会拿到行末的 x。

`UINpcTalk` 侧：`strip_simple_choices()` 把 `#L<n>#label#l` 就地拆成正文里的 `label` 加
字节区间 `[first, last)`；`change_text()` 在 `text.change_text("")` **之前**调
`layout_simple_choices(text)` 反取落点（布局一旦清空就再也取不到）；宽度另用一个
`maxwidth=0` 的 `Text` 量自然宽，色标由同一段 `Text` 自己解释，`#e` 之类加粗的宽度也对得上。
命中框跟随正文滚动（`offset * (height - TEXT_SLACK)`），滚出可视带的选项不可点——绘制已被裁掉，
留着命中就会「点屏幕上的空位、选中另一行」。

### 7.5 离线自证抓出**两个**我自己写的真 bug

**① off-by-one**：首版 `strip_simple_choices` 写的是 `out.append(tx, pos, label_begin - pos)`，
把 `#L3#` 令牌**本身**拼进了正文（残留半截令牌，后续会被布局器当色标解析）。
改为 `out.append(tx, pos, l - pos)`，并把 `c.last` 挪到 append 之后再取 `out.size()`。

**② 只认闭合 `#l`**：拿全服脚本语料跑同构算法才发现，`9900001.js` 那种
`#L<n>#label#l` 只是**规范写法**；大量移植过来的原生脚本根本不写 `#l`：

```js
// npc/1012100.js —— 逐行一项，全靠下一个 #L 或断行收尾
text += "#L0#请解释一下成为猎人的意义。\r\n#L1#请解释一下成为弩弓手的意义。\r\n#L3#我会选择我的职业！";
```

首版遇到这种串会直接 `break`，把整段原样吐回正文 ⇒ 满屏 `#L0#` 乱码、零个可点选项。
修法：找不到 `#l`（或 `#l` 出现在下一个 `#L` 之后）时，标签右界取
「下一个 `#L` / 第一个 CR 或 LF / 文本末尾」里最早的那个。

语料量化（`scripts-zh-CN` 全量 2052 个 `.js`）：

| 指标 | 修复前 | 修复后 |
| --- | --- | --- |
| 含 `#L` 的字符串字面量 | 779 | 779 |
| 解析出的可点选项 | **650** | **1012** |
| 标签里仍裹着真断行 | 2 | 0（断行改作右界） |

> 表里剩下的「正文仍含 `#L`」并非解析缺陷，而是扫描器按**单个字面量**切的：脚本写成
> `"\r\n#L" + i + "##m" + maps[i] + " (…)#l"`（`npc/1002007.js:46`），令牌被变量拼接切断，
> 静态扫描看到的本就是畸形输入。对畸形输入的行为是**正文原样保留、不吞、不崩**，属预期兜底。

再按**手工拼装的真实形态**逐个终判（10 例，全部通过）：行内三列、无闭合 `#l` 逐行、
变量拼装、标签内嵌 `#r…#k`、`#b` 铺在令牌前、标签内嵌 `#i/#t`、`#Lx#` 非法数字、
`#L` 悬在末尾、`#L0#` 空标签、单行多项。判据三条：令牌剥净、`[first,last)` 切片
与标签本体逐字节相等、标签不吃进断行。

> 语料里唯一「标签吃掉断行」的是 `npc/2040030.js` 的
> `#L0##b我的宠物变成了玩偶\r\n请帮我让它再次活动起来！#k#l`——它带**闭合 `#l`**，
> 跨行是脚本的本意，属合法形态。因此命中框高度改为取标签布局自身的
> `label.height()`（多行 ⇒ 多行高），而不是写死一行行距；否则下半截点不到。
> 这条是终判脚本顺手抓出来的第三个缺陷。

复核入口（可重跑）：`evidence/w2-hud-buttons/npctalk_strip_check.py`，
最近一次输出 `npctalk_strip_report.txt`（`FAILING CASES: 0`；
2052 个 `.js` / 779 段含 `#L` 字面量 / 1012 个可点选项）。

> 这同时解释了 §6.2 里「选项渲染成红色实心块」：除了 `#L` 无人处理，未剥净的令牌
> 会漏进布局器被按颜色令牌误解析。

### 7.6 构建与验收状态（诚实标注）

| 项 | 状态 |
| --- | --- |
| `-t:Rebuild`（含 §7.3/§7.4 全部改动） | 21:44:49 通过，exe md5 `68add478c181ecf92f7050781f23cfd5`、4424704 字节 |
| §7.5 两处 strip 修复后的增量构建 | **未产出**：`LNK1104` 写不进 exe——OutDir 里的 exe 被并行会话 22:21:28 的产物（md5 `f3d4289b1abcdd704ce7019c52ffec58`、4453376 字节）占用且该实例正在运行。按用户抉择「等对方空闲我再构建」，未结束对方进程 |
| 纯编译验证（**不改 OutDir**） | 2026-11-03 05:34 通过：`-p:OutDir=E:/project/_tmpbuild/ -p:IntDir=E:/project/_tmpint/` 覆盖输出到临时目录，0 error、链接成功产出 `MapleStory.exe`（md5 `1968d99cc2c441c68412b0db7446a025`、4378624 字节）。⇒ §7.3/§7.4/§7.5 全部改动**可编译可链接**；但该 exe 是「当前工作树全量」，含并行会话未提交的改动，**不作为部署产物** |
| 换行 / 行内选项 / 选项点击出站 `3c 00 <sel>` | **未经真机复验**，§6.2 两项缺陷状态为「代码已改、编译通过、待验」 |

复验判据（空闲后按此三条走）：① 余额四行分行；② 选项按三列行内渲染且颜色随正文（非蓝）；
③ 点一项出站 `3c 00 <sel>` 且服务端下一页出现。
