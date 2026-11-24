# 聊天分组页签缺失 + 中文乱码根因分析（chat-group-charset-rca-2026-11-19）

- **取证日期**：2026-11-19（ Asia/Shanghai，11:51–12:02 采样）
- **任务来源**：用户真机反馈「本端聊天框没有『全体/战斗/组队/好友/公会/联合』分组页签（正确端图2有），且聊天中文会乱码」，要求功能完整还原。
- **取证性质**：**只读取证，未改任何代码/资源/库/IDB**。所有结论带可复核的文件:行号或地址。
- **技能门禁遵循**：SKILL.md 门禁1/5/7/8/18（先证据后分析、禁想象、编码分层、WZ 走 orange-wz、原生还原走 REA→IDA→IMG）；AGENTS.md §1/§2/§3（orange-wz MCP 128B key + `TSPHKw==` IV，`load→query→unload`）；门禁17（DB 只查不写，本轮**未连库**，charset 结论引用已有只读快照文档）。
- **三层链路均读穿**：① IMG 资源（orange-wz）→ ② 原生二进制（IDA 13339）→ ③ 本端源码（UIChatBar / MessagingHandlers / PacketCharset / InPacket / Locale / GraphicsGL）；不跳层、不猜。

> **参考图处置**：用户指定 `2f49588688d27813019d002e33e65754.jpg`（图2，1920×1080 整图）在 `E:/project/MapleStory-Client`、`E:/project`、`C:/Users/11036` 全盘按文件名均未找到（仅 `E:/project/OmniParser/imgs/` 下三张无关 demo jpg）。按用户授权「如找不到文件就基于描述+代码/资源取证」，本文**不依赖截图像素**，页签语义由 IMG 节点名 + IDA 原生函数 + 用户描述三方交叉确认。

---

## 0. 结论速览（TL;DR）

| 问题 | 一句话根因 | 定性 |
| --- | --- | --- |
| 缺 6 个分组页签 | 资源**有**（StatusBar3 `chat/common/chatTarget/{all,friend,party,guild,association,afctv}` + `Tap_outside`+`BtAddTap_outside`），但 `UIChatBar.cpp` **modern 分支（332–451）只用 `chatTarget/base` 建了一个下拉式『聊天目标』框**，从未创建/绘制/绑定那 6 个 filter 按钮；`MessageGroup` 枚举（头:67–78）虽定义了 ALL/BATTLE/PARTY/FRIEND/GUILD/ALLIANCE，但**从不参与分组过滤**——三条 `show_message` 全把消息硬塞 `MessageGroup::ALL`（:2254/:2302/:2316），`draw()` 无条件画整个 `message_history`。 | 纯**客户端代码缺失**（资源齐、枚举齐、只差 UI/交互/过滤三段） |
| 中文乱码 | **双层**：① 收包层 `packet_to_internal` 的「严格 UTF-8 快路径」（`PacketCharset.cpp:33-101`、调用点 :116-117）在 GBK 双字节上会**误判为 UTF-8 而跳过 GBK→UTF8 转码**——暴力统计 GB2312 汉字区误判率 **13.74%**（一级常用字区 **18.14%**），误判即原字节进 UI → 乱码；② 若服务端按账号 `language=2`（US-ASCII）发包，中文在**服务端已被写成 `?`**（`CharsetConstants.getCharset(2)`），客户端收到的是 ASCII `?`，任何客户端解码都救不回——这是**配置/服务端编码约定**层。渲染层（`GraphicsGL` Unicode charmap + 思源宋体 + `.notdef` 保护）已核对，**不是**字库缺字的锅。 | ① 客户端**概率性解码 bug**（主）；② 部署账号/服务端编码约定（需实测确认） |

**区分两套东西（用户容易混，本文严格分开）**：
- **聊天目标（对所有人/密语/组队/好友/公会/联盟）** = 发消息发给谁，源码 `ChatTargetIndex` + `chat_target_name()`（`UIChatBar.cpp:1887-1903`），modern 下拉在 :430-451 —— 本端**已有**。
- **聊天分组频道（全体/战斗/组队/好友/公会/联合 +）** = 收消息看哪一路、过滤页签，原生 `CUIStatusBar::m_apBtChatFilter[0..6]` + `m_dwChatFilterFlag`（IDA `0x1697020`）—— 本端**完全缺失**。用户要的是后者。

---

## 1. 分组页签缺口（逐条行号 + 资源/原生佐证）

### 1.1 IMG 资源侧：6 页签 + 加号资源**全部存在**（orange-wz 实证）

- 工具/条件：orange-wz MCP `127.0.0.1:10012`，key `tools/_gms_key_128.b64`（128B `WzAESConstant.DEFAULT_KEY`，B64_LEN=172，HEAD=`EwAAAFIAAAAqAAAAWwAAAAgA`），IV `TSPHKw==`，`exclusive:false`，`load_files → get_node_tree_json/search_node → unload_all` 单会话收口。文件：live `E:/MXD/BeiDou-Client-xy/Data/UI/StatusBar3.img`（GMS 密文）。取证脚本 `tools/_hud_probe/wz_chatgroup_fresh.py` / `wz_chatgroup_origins.py`，原始输出 `tools/_hud_probe/wz_chatgroup_*.out`；前序 `wz_chatgroup_probe.py/.out` 同结论。**无解析异常**（本次 load 返回 `{"rootCount":1,"loadedCount":1}`，unload 正常）。

`chat/common/chatTarget` 子节点（`get_node_tree_json`，9 个）：

| 节点 | normal/0 尺寸 | 态 | 用户页签语义 | 存在 |
| --- | --- | --- | --- | --- |
| `base` | 48×18 | normal/pressed/disabled/mouseOver | 底框 | ✅ |
| `all` | 48×18 | 4 态（pressed 48×18） | **全体** | ✅ |
| `friend` | 48×18 | 4 态 | **好友** | ✅ |
| `party` | 48×18 | 4 态 | **组队** | ✅ |
| `guild` | 48×18 | 4 态 | **公会** | ✅ |
| `association` | 48×18 | 4 态 | **联合**（公会联盟） | ✅ |
| `afctv` | 48×18 | 4 态 | **战斗/活动**（AfreecaTV/活动频道） | ✅ |
| `backgrnd` | 46×2/46×1/46×2（origin (0,2)） | — | 页签条底纹 | ✅ |
| `mouseOver` | 44×14 | — | 悬浮高亮 | ✅ |

全部 canvas `origin=(0,0)`（除 `backgrnd/0` 为 (0,2)）。`search_node` 复核：`chatTarget` 唯一命中 `chat/common/chatTarget`；`Tap` 命中 `chat/common/Tap_outside`、`chat/common/BtAddTap_outside`（各 **50×22**，四态齐全、pressed/disabled/mouseOver 为真实 50×22 而非 1×1）——**这就是图2那排页签 + 加号**的原生底图。`chat/ingame/view/tab` 也存在（normal/disabled/mouseOver 52×18，checked 1×1 UOL），但它是**历史框上方的单标签底**，不是那排分组页签（见 §1.3 区分）。`chat/ingame/filter`（backgrnd 68×168 + button:edit/delete/subMenu）是过滤器子窗，同样不是页签本体。

> 命名映射结论：**正确端 6 页签在本端 WZ 里的名字是 `all/friend/party/guild/association/afctv`（不是 `battle/union`）；加号是 `BtAddTap_outside`。** 资源零缺口。

### 1.2 原生二进制侧：7 个 filter 按钮 + 位掩码（IDA 13339 实证）

- IDB：`E:/MXD/反编译IDA/MapleStoryT.i64`，module `MapleStoryT.exe`（「192客户端」），imagebase `0x400000`，`server_health` = auto_analysis_ready/hexrays_ready/strings_cache_ready 全 true。
- `find_regex("chat/Tap|chatTarget|_SetFilterButton|GetChatTarget")` → 16 命中，两组关键字符串：
  - `UI/StatusBar2.img/mainBar/chatTarget/{all@0x2429cf0, party@0x2429c98, friend@0x2429c40, guild@0x2429be8, association@0x2429b80, afctv@0x2429b28, base@0x25260f8}`
  - `UI/StatusBar2.img/chat/Tap/{all@0x2527204, friend@0x25271c0, party@0x2527178, guild@0x2527130, association@0x25270e0, expedition@0x2527090, afreecaTV@0x2527040}` + `chat/tapBar@0x2526d84` / `chat/tapBarOver@0x2526e20`
- `CUIStatusBar::OnCreate@0x16BE4E0` 用 `CLayoutMan::AddButton` 创建 **7 个** `m_apBtChatFilter[0..6]`，UOL 与 ID（原始反编译 `_raw_deComp_oncreate.c:1475-1632`）：

| idx | UOL (`UI/StatusBar2.img/chat/Tap/…`) | nID | 对应页签 | 对应 bit |
| --- | --- | --- | --- | --- |
| 0 | `all` | 0x3F9 | 全体 | 特殊（flag==0） |
| 1 | `friend` | 0x3FA | 好友 | 0x8 (bit3) |
| 2 | `party` | 0x3FB | 组队 | 0x4 (bit2) |
| 3 | `guild` | 0x3FC | 公会 | 0x10 (bit4) |
| 4 | `association` | 0x3FD | 联合 | 0x20 (bit5) |
| 5 | `expedition` | 0x3FE | 远征队 | 0x100000 (bit20) |
| 6 | `afreecaTV` | 0x3FF | 直播/战斗 | 0x40000000 (bit30) |

- `CUIStatusBar::_SetFilterButton@0x1697020`（decompile 全文）：
  ```c
  CCtrlButton::SetCheck(&m_apBtChatFilter[0], m_dwChatFilterFlag == 0);       // 0x1697035 全体=所有位清零
  CCtrlButton::SetCheck(&m_apBtChatFilter[1], m_dwChatFilterFlag & 8);        // 0x169704a 好友
  CCtrlButton::SetCheck(&m_apBtChatFilter[2], m_dwChatFilterFlag & 4);        // 0x169705f 组队
  CCtrlButton::SetCheck(&m_apBtChatFilter[3], m_dwChatFilterFlag & 0x10);     // 0x1697074 公会
  CCtrlButton::SetCheck(&m_apBtChatFilter[4], m_dwChatFilterFlag & 0x20);     // 0x1697089 联合
  CCtrlButton::SetCheck(&m_apBtChatFilter[5], m_dwChatFilterFlag & 0x100000); // 0x16970a1 远征队
  CCtrlButton::SetCheck(&m_apBtChatFilter[6], m_dwChatFilterFlag & 0x40000000);// 0x16970b8 直播
  ```
- `CUIStatusBar::GetChatTarget@0x761956` 仅 `return m_nChatTarget;`（`0x7` 字节）——**证明「聊天目标」是另一套单值状态**，与 filter 页签的 `m_dwChatFilterFlag` 位图**不是一回事**。`SetChatTarget@0x16B7CE0` 的 switch(nTarget=0..7) 也只做 StartChat/密语目标输入，不碰 filter 位。
- 支撑函数：`SetChatFilterType@0x16B1AA0`（`m_dwChatFilterFlag ^= 1<<n` 或清 0，随后 `_RefreshChatLog@0x16B17D0` + `_SetFilterButton`）、`AddChatFilterFlag@0x16B1B30` / `RemoveChatFilterFlag@0x16B1B70`、`_ResetChatBarPos@0x169FC70:0x169FF35`、`OnButtonClicked@0x16BACB0`（0x16BAF57–0x16BAFF9 七个分支）都调用 `_SetFilterButton` → **原生是「按钮→改位图→刷新聊天日志→回写按钮 check 态」的完整闭环**。
- 原生 7 页签 vs 本端 WZ 6 页签（`all/friend/party/guild/association/afctv`）：本端把 `expedition` 省了，`afctv` 顶替活动/直播位；**用户图2所报 6 页签与本端 WZ 的 6 个 `chatTarget/*` 一一对应**，可以 1:1 还原，不需要新增资源。

### 1.3 本端源码侧：缺口逐条（modern 分支 = 用户当前主题）

| # | 缺口 | 精确位置 | 证据/说明 | 影响 |
| --- | --- | --- | --- | --- |
| G1 | **构造：没有创建 6 个 filter 页签按钮对象** | `UIChatBar.cpp:332-451`（modern 构造） | 只建 BtMax/BtMin(:397-398)、5 个输入行按钮(:418-422)、1 个 BtWhisper 目标箭头(:448-450)。`buttons[]` 枚举 `UIChatBar.h:182-192` 只有 `BtMax/BtMin/BtChat/BtHelp/BtItemLink/BtChatEmoticon/BtOutChat/BtWhisper`，**无任何 `BtFilterAll/...`**。全文件 `grep chat/common/chatTarget` 只在 :434 出现一次且只取 `base`。 | 用户看不到那排页签——这是直接现象 |
| G2 | **draw：没有绘制页签条与选中态** | `UIChatBar.cpp:513-836`（draw） | classic 分支(:515-632)与 modern 分支(:633-819)都只画历史框、目标下拉列表、`classic_chat_target` 底框(:807-811)、目标文字(:813-817)。`chat/common/Tap_outside`、`BtAddTap_outside`、`chatTarget/{all,friend,party,guild,association,afctv}` 从未被 `Texture(...).draw()`。 | 即使补按钮对象也不显示 |
| G3 | **button_pressed：没有页签点击处理** | `UIChatBar.cpp:854-934` | switch 只有 :858-928 八个 case（BtMax/Min/ChatEmoticon/Chat/Help/Whisper/OutChat/ItemLink），default 返回 DISABLED(:929-932)。原生 `OnButtonClicked@0x16BAF57-0x16BAFF9` 的 7 个 filter case 在本端**一个都没接**。 | 点了没有/无法切换分组 |
| G4 | **send_key：没有页签切换键** | `UIChatBar.cpp:1744-1836` | W/S/P/Y/G/L/B/F 分支里 S/P/Y/G/L(:1761-1785) 全是**空体**（只有 `return`）；原生 `SetChatFilterType` 由这些快捷键/按钮驱动。 | 键盘切不了页签 |
| G5 | **消息分组状态从来不落地**：三条 show_message 全写死 `MessageGroup::ALL` | `UIChatBar.cpp:2254`（MessageType 重载）、`:2302`（ChatChannel 重载）、`:2316`（带 speaker 重载） | `Message` 结构有 `group` 字段（`UIChatBar.h:235`），构造函数 `:247/:248` 也接受 group，但三个调用点一律 `MessageGroup::ALL`。`ChatChannel`（头:92-104）虽与入站 handler 一一对应，却只用于配色（`channel_color:2274-2296`），不参与分组。 | 即使画出页签也无数据可过滤 |
| G6 | **draw 不过滤**：无条件画整个 `message_history` | `UIChatBar.cpp:561-569`（classic 展开）、`:588-596`（classic 输入态）、`:769-773`（modern 展开/收起） | 循环条件是 `message_history.size()`，无 `if (msg.group == active_group)`。全文件 `grep MessageGroup` 除枚举/构造默认值外**无任何比较**，`active_group` 状态成员根本不存在（`UIChatBar.h` 无此字段）。 | 页签点了也不会改变显示内容 |
| G7 | **有「目标」UI 但语义错位**：modern 建的是下拉目标框，不是页签 | `UIChatBar.cpp:430-451`、`:775-818`、`change_target:1915-1949`、`chat_target_name:1887-1903` | 代码注释自称「全部/好友/组队…切换入口」，实际 `chat_target_name()` 返回的是**「对所有人/悄悄话/对组队/对家族/对联盟/对夫妻」**（`locale/zh-CN.ini:107-114`，已核对为 UTF-8）——这是**发消息对象**，不是**看消息分组**。IDA `GetChatTarget`（§1.2）证明原生把两者分开。 | 用户把下拉误认/被告知是页签，但图2那排仍没有 |
| G8 | **native 7→本端 6 的语义对齐未文档化/未实现** | 对照 IDA §1.2 表 与 IMG §1.1 表 | `association` 在图2中文 UI 通常译「联合」，`afctv` 对应战斗/活动；`expedition` 本端 WZ 无节点。实现时 6 页签应绑 `all/friend/party/guild/association/afctv`，不能凭空造 expedition。 | 直接决定修复时按钮数据源选哪 6 个节点 |

> 结论：分组页签**不是资源缺失、不是二进制不支持、不是「应该没问题」**，而是 `UIChatBar` 一条完整的 UI→交互→数据过滤链在 modern 分支从未实现；`MessageGroup` 枚举留下来的「半成品」正说明当年只做到枚举就被搁置。

---

## 2. 中文乱码根因（双层：收包 + 编码约定；渲染已排除）

### 2.1 收包链路全貌（三层读穿）

- 服务端约定：`docs/features/s9-protocol-align/notes/charset.md:60-95`（dbx **SELECT 只读快照**，本轮未连库）确认 `ByteBufOutPacket.java:79` 的 `writeString` 按 `CharsetConstants.getCharset(getClientLang())` 编码；本部署 `accounts.language=3` ⇒ `LANGUAGE_CN(3,"GBK")`（`CharsetConstants.java:55`），`application.yml:52 language: zh-CN` ⇒ 即便兜底也是 GBK。⇒ **封包边界应一律 GBK**。
- 本端边界转换确实接上了：`InPacket::read_padded_string@Net/InPacket.cpp:101-119`，`:118 return packet_to_internal(raw)`；`OutPacket.cpp:110` 对称 `packet_from_internal`。各 chat handler（`MessagingHandlers.cpp:583 ChatReceived`、`:754-755 MultiChat`、`:824-827 WhisperResult`、`:894-897 SpouseChat`、`:407/:433/:437/:471 ServerMessage`）**统一走 `recv.read_string()`**，没有哪条聊天路径绕过 `InPacket`。
- 内部表示是 UTF-8：`CMakeLists.txt:115-117` / `MapleStory.vcxproj` 六处 `/utf-8`（源与执行字符集均 UTF-8），见 `charset.md:129-133`。⇒ `GBK → UTF-8` 边界转换方向正确；`locale/zh-CN.ini` 实测 UTF-8 无 BOM（`Locale::load_file` `Util/Locale.cpp:66-98` 只 strip BOM、不转码，正合 UTF-8 内部表示）。

### 2.2 根因 A（客户端，主因）：UTF-8 快路径误判 → 跳过 GBK→UTF8 转码

`Net/PacketCharset.cpp:107-147` `packet_to_internal`：
- `:112-113` 纯 ASCII 原样返回（正确）；
- `:116-117` **`is_strict_utf8_with_multibyte(bytes)` 为真就原样返回**（注释意图：防「UTF-8 被当 GBK 解成『娆㈣繋』」）；
- `:119-143` 否则才 `MultiByteToWideChar(CP936) → WideCharToMultiByte(CP_UTF8)`。

问题在判定函数 `:33-101`：GBK 双字节首字节常落在 `0xC0-0xEF`、尾字节常落在 `0x80-0xBF`，**与 UTF-8 两/三字节序列在字节层面重叠**。例：` GBK "¡"(c2a1)`、`"姝"(c2a3)`、` GBK "禄"(c2ac)` 等会被判为合法 UTF-8（`c2 80..BF` 是合法 2 字节 UTF-8）→ 直接跳过转码，原始 GBK 字节被当 UTF-8 喂给 Text → **乱码**。

硬数据（复刻 C++ 判定逻辑，脚本 `tools/_hud_probe/utf8_misfire_rate.py:30-51`、`utf8_fastpath_check.py`，本轮实跑）：
```
GB2312 双字节汉字区 0xB0A1-0xF7FE: total=6768 misfire=930  rate=13.74%
一级常用字区 0xB0A1-0xD7FE:        total=3760 misfire=682  rate=18.14%
```
常见词抽样 25 个（你好/冒险岛/聊天/全体/战斗/组队/好友/公会/联合/已被监控/装备/…）恰好 **0/25** 误判——说明它是**概率性、遇字才炸**的乱码（同一句话换个字就可能正常），与用户「聊天中文会乱码」的偶发观感一致。另测 51 个日常词命中 3 个（谢谢/要/们所在的 GBK 双字节组合），复现门槛低。

> 这是**唯一一条「聊天路径没走转换」的确切位置**：不是 handler 没调 `read_string`（全都调了），而是 `packet_to_internal :116-117` 的快路径把本该转换的 GBK 字节原样放行了。

### 2.3 根因 B（部署/服务端约定，需真机确认）：`language=2` → 服务端把中文写成 `?`

`docs/features/scrolling-notice-charset-2026-11/README.md:9-23` 已实证同类症状：账号 `admin`、`language=2` ⇒ 服务端按 US-ASCII 发包，中文变 `????`（口语也叫「乱码」）；库内原文是正确 UTF-8 `欢迎来到 XiaoYe冒险岛！！`。该文档 `:14-15` 记的已修是**服务端** `PacketCreator.serverMessage` 强制 GBK + 客户端快路径；但**普通聊天 `GeneralChat`/`MultiChat` 是否也强制 GBK 未在本文范围证实**（`charset.md:122` 只登记 `PacketCreator:7471` 一处 `isZhCN()` 硬编码 GBK，聊天发包未列）。若当前登录号仍是 `language=2`，则 `ChatReceivedHandler`/`MultiChatHandler` 收到的就是 ASCII `?`，客户端无从还原。

> 判别实验（不改库）：GRC 号 `Admin`（gm=6）用 `!showpackets` 抓 `ChatReceived` 入站 hex——中文字段若已全是 `0x3F`（`?`）则是根因 B；若是 `0xB0A1..0xF7FE` GBK 双字节且显示乱码则是根因 A。**本轮只读取证未做该实验**，不宣称已区分，但两条路径都给出可落地修法（§3）。

### 2.4 渲染层（已排除，不是字库缺字）

- `Graphics/GraphicsGL.cpp:277-287` `addfont` 显式 `FT_Select_Charmap(face, FT_ENCODING_UNICODE)`，注释 `:284-286` 记载「不选 charmap → CJK 渲染成同一个 .notdef 方块（『新手』→『ÆÆ』）」——该坑已填。
- `GraphicsGL.cpp:449-467` `get_wide_glyph`：`:461-462` 对 `FT_Get_Char_Index==0`（.notdef）直接 `return nullptr` 跳过，不画方块；`:466-467` FT_Load_Char 失败也跳过。
- `GraphicsGL.cpp:1416` / `:1799` / `:1849` 布局与绘制共用 `Utf8.h:35-62 utf8_next` 解码（注释 `:28-29` 记 T12b「两边各写一遍解码导致宽度/推进不一致」已修）。
- 字体：`GraphicsGL.cpp:225-236` 所有 A11M..A18B 走 `FONT_NORMAL_STR/FONT_BOLD_STR`；`Configuration.h:597` / `Configuration.cpp:338` 记录 R28-B5 起从 SimHei 换**思源宋体 SourceHanSerifSC**（仓库 `fonts/SourceHanSerifSC/` 实存，CJK 覆盖完整），SimHei 已删。⇒ **不是字库缺中文**。
- `Util/Locale.cpp:119-133` 的 `tr` 找不到 key 会返回 key 原文（如 `chat.target.all` 未翻译会显示英文 key，属另一类「文案不对」，非中文乱码）；`locale/zh-CN.ini:107-114` 实测这些 key 都在且为 UTF-8。

---

## 3. 修复建议（精确到文件行号；本轮**未实施**，供 lead 决策/Plan）

> 顺序建议：先 A（客户端解码，改动小、影响全端），再 B（页签，UI 链较长），最后真机复核；每条都按 SKILL 门禁 3/15 跑构建与节点日志，临时 DEBUG 日志验收后删。

### 3.1 中文乱码

| 优先 | 动作 | 精确位置 | 说明 |
| --- | --- | --- | --- |
| P0 | **移除/收紧 UTF-8 快路径**：`packet_to_internal` 对「可能是 GBK 双字节」的字符串**无条件走 CP936→UTF8**。最稳妥：直接删 `PacketCharset.cpp:116-117` 的 `is_strict_utf8_with_multibyte` 早退（保留 :112-113 ASCII 原样；保留 `packet_from_internal:155-161` 的 `MB_ERR_INVALID_CHARS` 兜底）；若担心历史 UTF-8 线上包，改为**仅在完整合法 UTF-8 且不含任何 GB2 双字节模式时**早退，或加显式「本部署封包恒 GBK」编译/配置开关（默认关快路径）。 | `Net/PacketCharset.cpp:33-101`（判定函数）、`:116-117`（早退点） | 根因 A 直接修复；影响面是所有 `read_string` 调用点（ASCII 不受影响，:112-113 保留）。改后跑 `tools/probe/charset_check` 全部向量 + `utf8_misfire_rate.py` 应降为 0 跳过。 |
| P0 | **真机判别根因 B**：用 gm 号抓 `ChatReceived` 入站 hex，看中文字段字节。若全 `0x3F` ⇒ 服务端按 `language=2` 写死 `?`，须在**服务端** `GeneralChatHandler`/`MultiChatHandler` 发包处按 `isZhCN()` 强制 `getCharset(3)`（仿 `PacketCreator.serverMessage` 已修法，`scrolling-notice-charset-2026-11/README.md:14`），或按部署约定把登录号 `language` 置 3（**写库须另行授权，禁止本次直接改**）。 | 服务端发包点（不在本端仓库；参照 `docs/features/s9-protocol-align/notes/charset.md:60-95` 的 `ByteBufOutPacket.java:79` 与 `CharsetConstants.java:55`）；本端只做抓包取证 | 区分 A/B，避免误改客户端；DB 写入需用户明示同意（门禁17）。 |
| P1 | 回归：复测 6 个常见词 + 930 个误判字节对，确认入站聊天全是正确 UTF-8；同步客户端 exe 到 `E:/MXD/BeiDou-Client-xy`（占用冲突按 SKILL 门禁14 首遇询问策略）。 | `PacketCharset.cpp` / 部署目录 | 兜住「修了但部署没更新」的假绿。 |

### 3.2 分组页签（功能完整还原，6 条对应 G1–G8）

| 优先 | 动作 | 精确位置 | 数据源/对齐 |
| --- | --- | --- | --- |
| P1 | **加按钮 ID 枚举**：`enum Buttons` 增 `BtFilterAll/BtFilterFriend/BtFilterParty/BtFilterGuild/BtFilterAssociation/BtFilterAfctv/BtFilterAdd`（7 个，对应原生 7 filter + 加号）；加成员 `MessageGroup active_group = MessageGroup::ALL` 与 `bool filter_flag[...]`（语义对齐原生 `m_dwChatFilterFlag` 位图：友=0x8/队=0x4/公=0x10/联=0x20/战斗=0x40000000，全体=flag==0）。 | `IO/UITypes/UIChatBar.h:182-192`（Buttons）、`:255-275`（成员区） | IDA §1.2 位掩码表；`MessageGroup` 枚举头:67-78 已存在，无需新增。 |
| P1 | **构造 modern 分支创建 6 页签 + 加号按钮**：按 IMG 实测尺寸，页签底 `chat/common/Tap_outside`（50×22）作普通态贴图，6 个页签用 `chat/common/chatTarget/{all,friend,party,guild,association,afctv}`（48×18，四态）叠字/选中；加号 `chat/common/BtAddTap_outside`（50×22）。**务必保留所有按钮对象**（`toggle_input/update_view` 对 `buttons[]` 无空指针检查，跳过创建会 `0xc0000005`，见 `wz-statusbar3-chat-qs-2026-11-19.md:111`）。 | `IO/UITypes/UIChatBar.cpp:430-451`（modern 构造目标 UI 段之后追加） | orange-wz §1.1 节点表；不要把 `BtWhisper` 目标箭头当页签（G7）。 |
| P1 | **draw 绘制页签行 + 选中态**：在 modern 绘制段（`:775-818` 之前/之后按层级）画 `Tap_outside` 底 + 6 个 `chatTarget/*` 的 normal/pressed + `BtAddTap_outside`；选中态用对应 `*/pressed` 或 `mouseOver`。位置以 IMG origin(0,0) 为锚，横向按 48/50px 步进（原生 `m_apBtChatFilter` 同排），**别用 `chat/ingame/view/tab`（52×18）**——那是历史框单标签底（§1.1）。 | `IO/UITypes/UIChatBar.cpp:513-836`（draw，modern 分支 `:633-819`） | 与 `classic_chat_target` 下拉分层，互不遮挡。 |
| P1 | **button_pressed 接 6 个 filter case**：仿原生 `OnButtonClicked@0x16BAF57-0x16BAFF9`，每个 case 调 `SetChatFilterType` 等价逻辑（位翻转 → 刷新 → 回写选中态）。新增 `set_chat_filter_type(uint8_t group)`：改 `filter_flag`，更新 `active_group`，触发重绘；**必须刷新聊天日志**（原生 `_RefreshChatLog@0x16B17D0`），否则点了没反应。 | `IO/UITypes/UIChatBar.cpp:854-934`（button_pressed switch），新函数放 `:2052 set_chat_type` 附近 | IDA `SetChatFilterType@0x16B1AA0` / `_SetFilterButton@0x1697020` 算法 1:1。 |
| P1 | **send_key 补齐快捷键**：把 S/P/Y/G/L/B 空体（`:1761-1795`）接成切页签（S=全体/战斗按原生位、P=组队、G=公会、L=联盟/联合、B=好友），调用同一 `set_chat_filter_type`；别与 `change_target:1915-1949` 的频道键混淆（那是**发送目标**）。 | `IO/UITypes/UIChatBar.cpp:1761-1795`（当前空分支），`:1915-1949`（对照勿混） | 原生快捷键由 `SetChatFilterType` 驱动（IDA §1.2）。 |
| P1 | **消息按分组落地 + draw 过滤**（闭环关键，缺了等于假页签）：① 三个 `show_message` 重载按 `ChatChannel` 映射 `MessageGroup`：ALL/GM/SYSTEM/MEGAPHONE→ALL，PARTY→PARTY，BUDDY→FRIEND，GUILD→GUILD，ALLIANCE→ALLIANCE（BATTLE 给 afctv 类入站保留位）——替换 `:2254/:2302/:2316` 的 `MessageGroup::ALL` 硬编码；② 四处 draw 循环加 `if (!passes_filter(msg)) continue;`；③ 加号按钮跳「自定义页签」入口（原生 `CLayoutMan::AddButton` + `chat/ingame/filter` 68×168 子窗，有资源可复用）。 | `UIChatBar.cpp:2254/:2302/:2316`（落地）、`:561-569/:588-596/:769-773`（过滤）、`:903-911`（加号复用 BtOutChat 式弹窗） | 三层闭环，按 SKILL「客户端 UI 子窗排查」§四步判定法：数据→测试对象→渲染→空态，别再出现「代码存在但真机没有」的假闭环。 |
| P2 | 真机验收：页签 6+1 显示、点击切换、分组过滤生效、中文显示正确；抓帧对齐 origin；同步 exe 到 live 目录。 | 全链 | 标注「真机待验」直到用户按原路径复核，不拿 build 过当功能对。 |

---

## 4. 未确证 / 局限（不糊弄）

1. **参考图2文件确实缺失**：全盘按文件名搜不到；页签外观/中文标签由「用户描述 + WZ 节点名 + IDA 原生 UOL」三方交叉推定，**未做像素级视觉校准**。若用户能补图，按 SKILL「截图兜底」落 `notes/layout-*.json` 再校相对位置。
2. **根因 A/B 未做实机 hex 判别**：本轮只读取证、未连库、未抓服务端包。两条都给出可落地修复与判别实验；不应宣称「已确定是哪一条」。
3. **本端 WZ 6 页签 vs 原生 7 个的语义差**：`afctv`（本端）对应原生 `afreecaTV`，`expedition` 本端无节点；图2中文显示「战斗」是否就是 `afctv` 需真机/补图复核，本文按节点名与 IDA UOL 对齐，不臆造第 7 个页签资源。
4. 页签 canvas 的 `origin` 在 `get_node_detail` 取值路径下返回结构嵌套（本轮以 `get_node_tree_json` 内联 `VECTOR_PROPERTY` 为准：页签帧均 (0,0)，`backgrnd/0` 为 (0,2)）；精确像素落点仍建议真机抓帧复核。
5. 本报告**只取证、零改动**：未修改任何 .cpp/.h/资源/exe/DB/IDB；取证脚本写在 `tools/_hud_probe/wz_chatgroup_fresh.py` / `wz_chatgroup_origins.py`（只读取证工具，非业务代码）。

---

## 5. 证据索引（便于复核）

| 证据 | 路径/锚点 |
| --- | --- |
| orange-wz 页签节点/尺寸 | `tools/_hud_probe/wz_chatgroup_fresh.out`、`wz_chatgroup_probe.out`（§1.1 表） |
| IDA 原生 filter 逻辑 | `docs/features/hud-ui-version-switch-feasibility-2026-11/evidence/_raw_deComp_oncreate.c:1475-1632` + 本文 §1.2 的 `0x1697020 / 0x761950 / 0x16B7CE0 / 0x16B1AA0 / 0x16B1B30 / 0x16B1B70` decompile |
| 本端源码缺口 | `IO/UITypes/UIChatBar.cpp:324-451, 513-836, 854-934, 1744-1836, 1887-1949, 2254, 2274-2316`；`IO/UITypes/UIChatBar.h:67-78, 92-104, 182-192, 235, 247-248` |
| 字符集/编码 | `Net/PacketCharset.cpp:15-18, 33-101, 107-147, 149-185`；`Net/InPacket.cpp:94-119`；`Net/Handlers/MessagingHandlers.cpp:583, 754-755, 824-827, 894-897`；`Util/Locale.cpp:66-133`；`Graphics/Utf8.h:24-62`；`Graphics/GraphicsGL.cpp:225-236, 277-287, 449-467, 1416, 1849`；`Configuration.h:597`、`Configuration.cpp:338`；`CMakeLists.txt:115-117` |
| 误判率硬数据 | `tools/_hud_probe/utf8_misfire_rate.py:30-51`、`utf8_fastpath_check.py`（本轮实跑：930/6768=13.74%，682/3760=18.14%，常见词 0/25） |
| charset 约定（只读快照） | `docs/features/s9-protocol-align/notes/charset.md:60-95, 124-137`；`docs/features/scrolling-notice-charset-2026-11/README.md:9-23` |
| 前序聊天栏节点证据 | `docs/features/hud-ui-version-switch-feasibility-2026-11/evidence/wz-statusbar3-chat-qs-2026-11-19.md`（§聊天目标/输入按钮） |
