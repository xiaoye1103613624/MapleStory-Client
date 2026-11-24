# 扩展改动 16 项需求研判与开发计划（2026-11-18）

## 0. 决策摘要（用户已拍板）

| 决策点 | 结论 |
| --- | --- |
| 客户端架构 | **全部移植为原生实现**（REA→IDA→IMG 还原后写进现有 UI/Net 体系，不做 DLL 注入） |
| 服务端 + opcode | **统一 BeiDou-Server_s9**，**授权重排 opcode**（冲突项分配空闲值，双端严格对齐） |
| 装备染色 | **用 ColorPrism（5782000，全装备）**，满足"普通+现金全支持" |
| 数据库 | **暂不碰库**，只落 SQL 脚本归档，DDL 由用户手工执行 |
| 范围分期 | **按依赖分三阶段**推进 |

## 1. 三条底层逻辑分水岭

### ① 客户端架构错配（决定所有 C++ 需求打法）
包内 C++ 全是 Kaentake 风格 Detours 注入，硬编码 v83 exe 固定地址（image base 0x400000）：
- `cashshop-window/README.md:96-98`：地址钉死标准 v83 exe，依赖宿主框架 `wvs/wnd.h` 等 9 个头文件
- `个人伤害统计/…/damagerank_client_readme.md:124-138`：hook `CClientSocket::ProcessPacket @0x004965F1`
- 表情包/染色/美容院/签到同套路

当前 `MapleStory-Client` 是**原生源码客户端**（`IO/UITypes/UISideIconBar.h:13`、`Util/SideIconConfig.h:14-20`），无 Detours 宿主。
**结论：DLL 逻辑不能 drop-in，必须 REA→IDA→IMG 证据翻译为原生实现（Skill 门禁 5/18 红线）。**

### ② 服务端错配 + opcode 冲突（已实核 s9）
包面向 Cosmic/HeavenMS（org.gms），当前主用 BeiDou-Server_s9（协议与主线不同，s9 用 4B exp）。冲突 opcode 实核：

| 需求包 opcode | s9 现状 | 证据 |
| --- | --- | --- |
| 伤害统计 0x3713/0x3714 | CUSTOM_PACKET / DPT_TRACKER 已占用 | RecvOpcode.java:25 / SendOpcode.java:358 |
| 表情包 0x11C/0x17F | 0x11C=LEFT_KNOCK_BACK 占用 | SendOpcode.java:291 |
| 签到 0x11A/0x17C | 0x11A=HIT_SNOWBALL 占用 | SendOpcode.java:289 |
| 染色 0x11D/0x184 | 0x11D=COCONUT_HIT 占用 | SendOpcode.java:292 |
| 商城 0x3730/0x3731 | 0x3730=LIMIT_BREAK_SYNC 占用 | SendOpcode.java:359 |
| 融合 0x4F | 0x4F=USE_CASH_ITEM（本身是该 opcode 扩展） | RecvOpcode.java:94 |

**⇒ 全部自定义 opcode 重分配、双端严格对齐。**

### ③ 资源密钥错配（用户点名的密钥问题）
资源包为 GMS083 加密体系：img 头 `73 f8…`（GMS AES），wz 头 `50 4b 47 31`（PKG1）。
当前客户端走 nx 体系、服务端走 XML WZ。按 AGENTS.md 硬门禁：
**禁止裸拷；必须 orange-wz MCP `load_files`→`batch_convert_key`→`save_as` 异路径→短启 probe。**
两套 key 规格严禁混用：MCP 路径 128B key，nx 回退路径 65536B 全量表。

## 2. 16 项研判矩阵

| # | 需求 | 类别 | 包内容 | 客户端 | 服务端 | 入口 | 风险 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | cashshop-window | B功能 | 1cpp+h+4java+img | Detours CWnd 3500行→原生重写 | 4java+TSV商品表 | 侧边栏 | 高 |
| 2 | 聊天表情包 | B功能 | 5cpp+5java+3img | hook+头顶动画 | 校验+广播 | 聊天栏 | 中高 |
| 3 | 掉落特效 | A/B混合 | 12webp+cpp+wz+java | 资源+渲染 | java | 自动触发 | 中 |
| 4 | 个人伤害统计 | B疑似已落地 | 35h+23cpp+2wz | s9已有opcode占位 | s9已有DPT_TRACKER | F12 | 先核查 |
| 5 | 动漫皮肤 | A资源 | 96img+1xml | 密钥转换+nlnx | 同步 | 无 | 低中 |
| 6 | 111款阿尔泰皮肤 | A资源 | 112img+113xml | GMS083适配+密钥 | 同步 | 无 | 中 |
| 7 | 卡片名称 | A资源 | 1img+1xml | 密钥转换 | 同步 | 无 | 低 |
| 8 | 每日签到 | B功能 | 3java+cpp+wz+sql | 28天网格窗 | handler+3列DDL | @daily/侧边栏 | 中高 |
| 9 | 美容院 | B功能 | 12java+24h+6sql | 3tab窗口+预览 | handler+2表DDL | @beauty/侧边栏 | 高 |
| 10 | 融合外观 | B功能 | 113cpp+64h+sql | anvil协议重 | Equip加列+发包 | NPC | 高 |
| 11 | 伤害皮肤 | B功能 | 110cpp+62h+6img | 数字渲染重 | 3表DDL+发包 | 侧边栏/NPC | 高 |
| 12 | 适用于083时装 | A资源 | 15wz | PKG1导入 | 服务端XML | 发道具 | 中 |
| 13 | 修复NameTag错位 | A修复 | 1img(5.6MB) | 资源替换+短启 | 视情况 | 无 | 低 |
| 14 | 装备染色ColorPrism | B+C评估 | ColorPrism包 | HSV改色引擎 | 18列4 changeset | NPC/道具 | 高 |
| 15 | 装备悬停对比 | B功能 | txt 311行残缺 | tooltip hook | 无 | 悬停 | 中(源不全) |
| 16 | 装备增加技能 | B功能 | 4h+2cpp+zip | 资料极少 | 待评估 | 装备属性 | 信息不足 |

## 3. 三阶段开发计划

### 阶段一：资源流水线 + 现状核查
- orange-wz MCP 转 key 流水线跑通（SOP 归档）：✅ SOP 已落盘 `notes/wz-key-conversion-sop.md`，基于 29 工具真实签名 + 128B/65536B 双 key 规格 + GMS IV 源码实核；HTTP 直连握手实测 load_files 成功。
- #5/6/7/12/13 资源导入（每项：load_files→batch_convert_key→save_as异路径→probe→原子替换）：⏳ 待授权——live 目标已锁定为 `E:\MXD\BeiDou-Client-xy\Data`（见 SOP §4.7）；实测资源文件未被独占锁定，前几轮"3进程占用=硬阻塞"判断已修正，真实阻塞=缺 live 改写授权。须用户明确授权改该目录后执行。
- #4 伤害统计现状核查：✅ 完成（见 §5，协议已通缺 UI，归阶段二）。
- **实测验证**：✅ 标准 GMS 包主干可行——动漫皮肤 01076000.img（头 `73f86c77…`）128B key 下 get_node_tree_json 返回真实 canvas 节点树、search_node 命中 info，isError:false，证明可导入。
- **实测风险**：卡片名称 Ins.img 非标准 GMS083 key，`IMG 解析失败`（头 `73f851bc…` vs GMS `73f86c77…`），数据实为明文 XML；须单独定规格，禁止 GMS key 硬套。
- **门禁**：禁裸拷、短启 probe、SHA 留证

### 阶段二：轻量功能（原生移植）
- #3 掉落特效、#2 表情包、#15 悬停对比
- 每项先 REA→IDA→IMG 证据还原，再原生实现
- opcode 重排（s9 空闲段），双端对齐

### 阶段三：重功能
- #8 签到、#9 美容院、#1 商城、#14 染色ColorPrism、#11 伤害皮肤、#10 融合外观
- 每项：证据还原→服务端java→客户端原生→SQL脚本落盘（DDL用户手工）→双端构建

## 4. 验证门禁
- 服务端：mvn compile/package 贴输出
- 客户端：VS 构建 error=0/lnk=0，产物 SHA 核对
- 资源：orange-wz verify + 短启 probe（禁拿参考端截图当本端验收，证据错配是造假）
- 功能：真机未验如实声明，待用户按原路径复核
- 每个功能独立原子 commit（用户授权时）

## 5. #4 伤害统计现状核查结论（阶段一预执行，只读）
双端协议骨架已通，但 UI 未闭环——不是"已完成"，是"半成品"：
- 服务端：`Character.java` 有 `damageRankOpen/Reset/Close`；`CustomPacketHandler.java:41-48` 三态分发；`SendOpcode.java:358` DPT_TRACKER(0x3714)；`RecvOpcode.java:25` CUSTOM_PACKET(0x3713)
- 客户端：`Net/PacketSwitch.cpp:328` 注册 DPT_TRACKER(0x3714)→`DptTrackerHandler`（:686）；`Net/Handlers/WaveAShellHandlers.cpp:3304-3347` 解析 mode 0/1/2（重置/玩家伤害/技能伤害）
- **缺口**：客户端仅把结果 `UIAdmin::set_message(tip)` 当调试提示（:3342-3345），无 F12 排行榜窗口、无玩家/技能双页、无拖拽滚动。要闭环需按包内 `damagerank_client_readme.md` 翻译出原生 `UIDamageRank` 窗口。
- 结论：#4 归入阶段二，工作量从"全新"降为"补 UI 渲染层"，opcode 无需重排（已占用且双端一致）。

## 6. 待办/风险
- #16 资料不足，阶段一后单独向用户索取更多信息
- 登录器会回滚 exe，部署声明需实时核对 live SHA
- 所有皮肤类外观资源须保证完整（角色模型部件，非仅图标），仅图标时须用户确认
