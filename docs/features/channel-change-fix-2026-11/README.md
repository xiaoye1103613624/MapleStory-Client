# 换频道（CHANGE_CHANNEL）全流程修复与真机验证

> 状态：已闭环（真机 trace 铁证验证通过）
> 适用客户端/协议版本：v83 客户端 / BeiDou-Server_s9（收包 0x27 / 发包 0x10）
> 项目名称：MapleStory-Client（分支 feature/dev_0.0.1）
> 作者/日期：2026-11-16 ~ 2026-11-17
> 相关提交：未提交（改动留在工作树，见 patches/）
> 产物目录：docs/features/channel-change-fix-2026-11/

## 1. 背景与目标

用户报告"换线没落地"：点击换频道后场景与怪物点位完全不变、频道号看似没切，怀疑是假换线。要求检查换频道全流程（多人游戏、怪物移动、任务/掉落、地图加载、服务端通信），对比 Angel / MinMS 客户端与 s9 服务端逻辑，并修复；另需解释"停服务端后客户端仍在线"。

**目标**：
- 定位并修复换线链路的客户端侧断点，使换线真实连接到新频道、清旧场、按新频道 spawn 重建地图实体。
- 明确换线后任务/掉落/地图标记如何出现（服务端 spawn 驱动，非本地生成）。
- 修复"停服仍在线"（断线 FIN 检测）。

**明确不做**：不改 s9 服务端协议（已验证本端收发方向正确）；不做 GUI 视觉还原；不外抛账号凭据到文档。

## 2. 影响范围

| 层级 | 路径/模块 | 变更类型 |
| --- | --- | --- |
| 客户端 | `Gameplay/Stage.h` / `Stage.cpp` | 新增 CharEntry 登录态快照（store/get/has） |
| 客户端 | `Net/Handlers/SetFieldHandlers.cpp` | SET_FIELD set_field 双分支（UICharSelect / Stage 快照）+ 回游戏态 |
| 客户端 | `Net/Handlers/PlayerHandlers.cpp` | CHANGE_CHANNEL 回包成功码校验 + 用当前角色 oid 补发 cid |
| 客户端 | `Net/SocketWinsock.cpp` | receive() 最小 diff FIN 检测（recv()==0 → success=false） |
| 服务端 | `org.gms.net.server.channel.handlers.ChangeChannelHandler` 等 | **只读核对，未改动** |
| DB/配置 | — | 未改 |
| WZ/XML/脚本 | — | 未改 |

## 3. 最终实现逻辑

### 3.1 主流程（换线五段）

1. **发包**：`IO/UITypes/UIChannel.cpp::change_channel()` → `LoginParser::expect_channel_reentry()` + `ChangeChannelPacket(selected).dispatch()` → `OutPacket::CHANGE_CHANNEL=0x27`，body = `write_byte(channel) + write_int(0)`。s9 `ChangeChannelHandler.java` 读 `readByte()+1`、`readInt()` 丢弃（早期只写 1 字节导致服务端 readInt 异常=永不回包，已修）。
2. **服务端**：`Client.changeChannel()` 同频道拒绝/autoban、商城/小游戏/商店/交易中 enableActions 拒绝；否则存档、清旧频道、setSessionTransitionState（按 IP 登记 cid）、向**目标频道**发 `getChannelChange` 回包（0x10，body = 1B 成功码 + 4B IP + 2B port，**无 cid**）。
3. **收回包**：`Net/PacketSwitch.cpp` 注册 `CHANGE_CHANNEL=0x10 → ChangeChannelHandler`；`handle()` 先 `inspect_bool()` 校验成功码（失败清重入标记 + 恢复 UIChannel + 红字提示并 return），再 `note_channel_entry_cid(get_oid())` + `parse_login()`。
4. **重连 + 登录**：`LoginParser::parse_login()` 读 IP/port → `Session::reconnect()`（本端阻塞式 Winsock 同步 init，无 MinMS 那种异步 onmessage 重入，故不移植延迟重连）→ 因 `channel_reentry_pending` 用补发的 cid 发 PlayerLoginPacket。新频道服 `PlayerLoggedinHandler` 校验 transition-state 的 cid → `getCharInfo` 下发 SET_FIELD。
5. **落地**：`SetFieldHandler::handle()` 从包内读 channel 并 `Configuration::set_channelid()`；`set_field()` 用 **UICharSelect（选角时）或 Stage::has_char_entry 快照（游戏内换线时）** 双分支取 CharEntry，`was_ingame` 时 `change_state(GAME)`；`transition()` 清旧场（Stage::clear 清 chars/npcs/mobs/drops/reactors/markers）→ Stage::load → transfer_player。地图实体由后续独立 spawn 包（SpawnMob/Npc/Char/Drop/Reactor）按新频道重建。

### 3.2 数据 / 协议 / 语义

- opcode 方向：收包（C→S）CHANGE_CHANNEL=**0x27**；发包（S→C）CHANGE_CHANNEL=**0x10**、SET_FIELD=**0x10=125**。本端收发与 s9 一致。
- 回包字节：`writeByte(1)` 成功码 + 4B IP（read_byte×4）+ 2B port（`read_short()` 原生小端，与服务端 writeShortLE 吻合）。
- cid 来源：回包不带 cid；游戏内换线用 `Stage.get_player().get_oid()`（其本质 == 角色 cid，见 `Player::Player(const CharEntry&)`）。

### 3.3 三个断点（修复前）+ 一个附加断点

| # | 断点 | 修复前 | 修复后 |
| --- | --- | --- | --- |
| A | cid 靠静态残留 `channel_entry_cid`（只 ServerIPHandler 赋值，换线不 note） | 换线时 cid=0 → skip 登录 | handle 内显式 note 当前角色 oid |
| B | 回包无成功码校验 | 任何 response 都当成功 | inspect_bool()==1 才继续 |
| C（致命）| set_field 硬依赖 UICharSelect | 换线后 UICharSelect 已销毁 → `return` 丢整个 SET_FIELD（假换线根因）| UICharSelect / Stage 快照双分支 + was_ingame 回 GAME |
| D | 停服后 recv()==0 不置 success=false | connected 恒 true → "停服仍在线" | 最小 diff FIN 检测 |

## 4. 资源与同步

- 三方对照逻辑/协议差异见 `notes/three-way-comparison.md`（服务端 s9 权威 + MinMS + Angel IDA）。
- 无 WZ/IMG/XML、无多语言目录改动、无外观道具、无密钥变更。
- 编码：源码改动沿用原文件 UTF-8，`SkillTooltip.cpp` 的历史 GBK lead byte 隐患已用等价 hex 转义规避（构建期修复，非本功能逻辑）。

## 5. 编码与 i18n

- 无新增面向玩家文案（失败提示沿用 csn 红字 chat 通道）；无 i18n 资源改动。

## 6. 产物清单

| 类型 | 路径 | 说明 |
| --- | --- | --- |
| 文档 | `README.md` | 本文件 |
| 证据 | `evidence/setfield_trace_AB.txt` | 真机 trace 新旧版 A/B 对照（110 行）|
| 证据 | `notes/three-way-comparison.md` | s9/MinMS/Angel 三方对照结论与地址 |
| 证据 | `notes/client-path.md` | 客户端路径记忆 |
| 补丁 | `patches/channel-fix-5files.diff` | 5 文件完整 git diff（283 行）|
| 构建索引 | `builds/INDEX.md` | 构建/部署/稳定性记录 |

## 7. 验证记录

- **真机 trace 铁证**（`E:/MXD/BeiDou-Client-xy/setfield_trace.log`）：
  - 旧版 line 2504–2555：`channel=1/3/4/5/11` → 全部 `set_field aborted: UICharSelect missing`（假换线）。
  - 修复版 line 2798–2902：`channel=1→2→1` → 每次 `set_field reusing Stage char snapshot (channel hop)` + 完整落图链（loadplayer → parse_stats/inventory/skillbook/cooldowns/questlog → Stage::load → UI State::GAME → transfer_player）。
  - 三项验收全中：频道号真实变化 + SET_FIELD 全部落图 + 仅修复版输出的 "reusing" 日志。
- **构建**：VS2019 BuildTools MSBuild，Release/x64/v142，`BUILD_EXITCODE=0`，error C/LNK = 0。
- **稳定性**：FIN 补丁迭代踩坑——分离变量/重构控制流会触发标题画面 RVA 0x61b6 布局敏感堆损坏（0xC0000374/0xC0000005，4/4 确定性崩）；改为最小 diff 后连续 4/4 SURVIVED。
- **live exe**：`E:/MXD/BeiDou-Client-xy/MapleStory.exe`，实测稳定存活、含全部修复标记、无临时钩子污染。
- 本功能不涉及 DB；库内数据未访问。
- 客户端验证步骤：登录进游戏 → 触发换频道 → 查 setfield_trace.log 出现 "reusing Stage char snapshot" + 新频道怪物/NPC spawn（频道号变化）。

## 8. 预留、风险与回滚

- **回滚**：`patches/channel-fix-5files.diff` 可逆；备份 `E:/MXD/BeiDou-Client-xy/MapleStory_final_fixed_3b178559.exe`（已验证稳定的最小 diff 版）。
- **风险**：本代码库对栈/代码布局敏感。后续若改 `SocketWinsock::receive()`，必须保持最小 diff（保留原 select+recv 控制流，仅在 `if(result>0)` 分支内加 FIN 判断），任何变量拆分/重构都可能复现启动堆损坏。
- **环境坑**：live exe 会被并行构建/登录器反复覆盖（本次核查期间 SHA 多次变化：3b178559 → 6178eaf4 → 8b03165b）；交付时以"含修复标记 + 稳定存活"为准，不追某个固定 SHA。
- **FIN 崩溃 heisenbug**：根因未做符号级收敛（无 cdb/windbg、dbghelp 符号解析失败），仅以"最小 diff 规避"闭环。若日后复现，需上真调试器抓 RVA 0x61b6 调用栈。

## 9. 对外交流摘要

换频道"假换线"根因是客户端 set_field 硬依赖选角 UI，换线后该 UI 已销毁导致整个 SET_FIELD 包被丢弃。已改为登录态 CharEntry 快照双分支，并补齐回包成功码校验、cid 补发、断线 FIN 检测。对照 s9/MinMS/Angel 三方逻辑确认协议与流程同构。真机 trace 显示修复后 channel=1→2→1 全部走通完整落图链，换线真落地。
