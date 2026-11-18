# 死亡链路 + HP=0 全链路补齐（2026-11-17 23:49）

> 授权：用户「以服务端为主，客户端没有实现和对接的全部补充」；USER.md「一次性改完、禁止挤牙膏」
> 真机证据：23:26 视频 5 问（见 `notes/realdevice-20261117_2326-five-questions.md`）

## 一、取证结论：死亡对接不需要新协议

| 层 | 现状 | 证据 |
| --- | --- | --- |
| s9 服务端 opcode | 无普通死亡/复活 opcode（仅斗技场 `MONSTER_CARNIVAL_DIED=0x126`） | `SendOpcode.java:301`、`RecvOpcode.java` 无死亡项 |
| 服务端死亡业务 | 完整：`playerDead():7081-7146` 掉经验/清buff/下椅；`hpChangeAction:8397-8415` 判死；复活走 `ChangeMapHandler:98-115`（`isAlive()==false` → wheel 或 respawn 回村） | 源码 |
| 服务端 HP 同步 | `STAT_CHANGED` 发 HP=0（`PacketCreator.updatePlayerStats`，HP=0x400 走 writeInt，客户端 `PlayerHandlers.cpp:176` read_int 对齐） | 源码 |
| 客户端缺口 | `ChangeStatsHandler::handle_stat` 只 `set_stat(HP)`，**无 HP==0→DIED 转换**；opcode 仅斗技场 294；`Char` 有 `DIED=18` 枚举但无设置点；死亡动画 `"dead"` 已接线（`Stance.cpp:104`）但无触发 | 源码 |

**结论**：原生 v83 死亡 = 客户端检测 HP stat 更新里的 0 → 自己切 DIED + 播 dead stance；复活走既有 `ChangeMapPacket(true,...)`（`GameplayPackets.h:29`）。**不需要新增 opcode**，纯客户端补齐即可，服务端业务已完整。

## 二、本轮改动（客户端全补，4 处）

| # | 文件:行 | 改动 |
| --- | --- | --- |
| 1 | `Character/Player.h:56` + `Player.cpp:440-452` | 新增 `Player::die()`：清 `attacking` → `Char::set_state(DIED)`（绕过 `set_state` 的 attacking 门禁，避免血0还站着放完攻击）；dead stance 由 `Char::set_state` 自动设置 |
| 2 | `Net/Handlers/PlayerHandlers.cpp:114-118` | `ChangeStatsHandler::handle` 末尾：HP≤0 → `player.die()`（核心状态转换） |
| 3 | `Character/Player.cpp:449-452` | `can_use` 开头：`state==DIED` → `FBR_OTHER`，死亡态禁止一切技能/攻击 |
| 4 | `Gameplay/Stage.cpp:396-400` | `try_heal_over_time`：`get_char_state()==DIED` → return，死亡态不回血（消除 110↔1110 竞态） |

**未动**：服务端（业务已完整，无需改）；autopot（F，服务端配置行为，原生设计）；死亡特效独立节点（原生死亡表现 = dead stance 动画，非独立 Effect 节点，已接线）。

## 三、验证（逐项贴输出）

| 项 | 结果 |
| --- | --- |
| 编译 | 首次 EXITCODE=1 = LNK1104 exe 被占用（PID 45484），非编译错误；按 m04292 授权 taskkill 后重建 → **EXITCODE=0 / GATE OK**，`error C/fatal error` 计数=0 |
| 源码特征 | die()×1、ChangeStats die调用×1、can_use DIED×4、Stage DIED禁回血×1、闪烁修复×3、龙咆哮x表×2，全部在 |
| 部署 | live `MapleStory.exe` 23:49:08，SHA256=`73f1c4924a40c75e617d34068ea75d7712885bc81755c82ed4a746c3cab9b333`（实时核对，防回滚） |
| PID 26292 | 32K 空壳无 Path，Access denied 但不占目标文件（与既有 memory 一致），不影响构建 |

## 四、复活链路补齐（第13轮，用户选②「复活走原生逻辑，链路完整」）

原生取证：UIWindow.img 无 Revive/Dead 窗口节点（orange-wz）；s9 服务端无普通死亡 opcode、复活走 `ChangeMapHandler:98-115`（`died=1 && !isAlive()` → wheel/respawn）。原生 v83 死亡复活 = **按 JUMP 键**发 `ChangeMapPacket(died=true)`，服务端 respawn 回图。不改协议、纯客户端对接。

| # | 文件:行 | 改动 |
| --- | --- | --- |
| 5 | `Gameplay/Stage.cpp:562-573` | `Stage::send_key` 开头：DIED 态下按 JUMP（down 沿）→ `ChangeMapPacket(true,-1,"",false).dispatch()`；同时吞掉死亡态其它按键（倒地后不能放技能/开窗） |

**完整链路闭合（逐环有据）**：
1. HP=0 → `ChangeStatsHandler` → `player.die()` → DIED + dead stance（第12轮）
2. 按 JUMP → `Stage::send_key` → `ChangeMapPacket(true,-1,"",false)`（第13轮）
3. 服务端 `ChangeMapHandler:98-115` → `Character.respawn():7197-7213`（`changeMap(returnMap)` + `updateHp(50)` + `setStance(0)`）
4. 客户端收 SetField → `Stage::respawn():166` → `Player::respawn():222` → `nullstate.update_state()`（`PlayerStates.cpp:45`）按当前输入重算为 STAND/FALL/WALK → 角色站起

**第13轮验证**：构建 EXITCODE=0 / GATE OK、编译错误=0；live `MapleStory.exe` 23:54:38 SHA256=`90b5633b5888b75e6c967f46a3c96dbc19534adf9bb852e65c92610c55de3a44`（实时核对）；复活包特征×1、die()×1、HP0→die×1、DIED禁技×4、DIED禁回血×2 全在。

**第14轮：复活包 body 逐字节对拍（防 exp 8B/4B 类错位重演）**

| 字段 | 客户端写 `GameplayPackets.h:31-35` | 服务端读 `ChangeMapHandler:84-90` | 字节 |
| --- | --- | --- | --- |
| died | `write_byte(died)` | `readByte()` = "1=from dying" | 1B ✅ |
| targetid | `write_int(-1)` | `readInt()` | 4B ✅ |
| targetp | `write_string("")` = `write_short(0)` + 0 数据 | `readString()` 2B 长度前缀 | 2B ✅ |
| 保留位 | `skip(1)` | `readByte()` | 1B ✅ |
| wheel/chasing | `write_short(0)` | `readByte()` wheel + `readByte()` chasing | 2B ✅ |

客户端总长 = 1+4+2+1+2 = **10B**；服务端读 = 1+4+2+1+1+1 = **10B**，逐字段对齐、无错位。复活包这一环确认真闭合，非"看起来闭合"。写_string 前缀=2B（长度按转码后字节数，`OutPacket.cpp:105-113`）已核对。

**第15轮：复活回图→角色站起 逐行实证（最后一环）**

此前"复活后 DIED→STAND"是推断，本轮逐行坐实：
- 服务端 `ChangeMapHandler:98-115` 复活 → `Character.respawn():7197`（`changeMap`+`updateHp(50)`）
- 客户端收 SET_FIELD：`SetFieldHandler::change_map:274-283` → `transition:100` → `Stage::load:62` **末尾调 `respawn(portalid)`**（:87 区间内）
- `Stage::respawn:166-173` → `player.respawn:startpos`
- `Player::respawn:222-228` → `nullstate.update_state(*this)`
- `PlayerNullState::update_state:45-73`（`PlayerStates.cpp`）按当前按键/地面重算为 STAND/FALL/WALK，**不看旧 DIED** → 角色自然站起

复活全链 4 环（发包/解析/服务端复活/客户端站起）全部逐行验证，无推断残留。

## 五、诚实声明（USER.md 红线）

- **真机未验收**：未实机确认"HP=0→倒地→按JUMP→复活回村站起"全流程。不称"完全修复"。
- **JUMP 键为依据原生 v83 的复活触发选择**；若真机原生实为别的键/屏幕按钮，需据实机调整（当前服务端 died=true 协议已对接，换触发点不影响服务端链路）。
- autopot 被自损触发（F）：服务端配置原生设计，未动。
- 未提交（用户未要求自动提交）。

## 六、下一步

用户真机确认死亡→复活全流程后 mark complete + 提交。
