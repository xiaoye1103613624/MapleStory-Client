# #8 每日签到 原生移植 · 交付记录（2026-11-18）

## 决策（用户已拍板）
- 下一目标 #8；opcode C 0x3717 / S 0x3718；mock 奖励可运行；入口侧栏图标 + @daily；DB 用**独立表 checkin_state，表缺失时降级内存态**（不碰 characters 存档结构，规避 critical memory「装备槽位不变量」同类风险）。

## 服务端（BeiDou-Server_s9，mvn -o compile=0）
| 文件 | 动作 |
|---|---|
| `org/gms/server/DailyCheckinStore.java` | 新建：per-char State(day/claimedMask/lastClaimMs)+claimableNow/applyClaim/cooldownSeconds；读写 `checkin_state` 表，SQL 异常置 tableUsable=false 降级内存 |
| `org/gms/server/DailyCheckinRewards.java` | 新建：mock 每日 1 meso+占位图标 2000000；iconItemId/grantDay/tooltip |
| `org/gms/net/server/handlers/DailyCheckinHandler.java` | 新建：PacketHandler(需 validateState)；等级门→claimableNow→grantDay+save→回 snapshot |
| `org/gms/client/command/commands/gm0/CheckinCommand.java` | 新建：@daily/@checkin 打开 |
| `org/gms/client/command/CommandsExecutor.java` | 注册 daily/checkin |
| `org/gms/net/opcodes/RecvOpcode.java` | `DAILY_CHECKIN(0x3717)` |
| `org/gms/net/opcodes/SendOpcode.java` | `DAILY_CHECKIN(0x3718)` |
| `org/gms/util/PacketCreator.java` | `dailyCheckinSnapshot(day,mask,justClaimed)` |
| `org/gms/net/PacketProcessor.java` | 注册 handler |
| `resources/db/extensions/daily_checkin_standalone.sql` | 落盘（CREATE IF NOT EXISTS，AI 未执行） |

## 客户端（VS 三证：链接行/error0/obj21:27；live exe 21:30 SHA=42BC6D90…）
| 文件 | 动作 |
|---|---|
| `Net/OutPacket.h` | `DAILY_CHECKIN=14103(0x3717)` |
| `Net/Packets/DailyCheckinPackets.h` | 新建 Open/Claim packet |
| `Net/PacketSwitch.cpp` | enum 0x3718 + emplace handler |
| `Net/Handlers/WaveAShellHandlers.h/.cpp` | `DailyCheckinHandler::handle`：解析 snapshot→`ui->apply_snapshot` |
| `IO/UITypes/UIDailyCheckin.h/.cpp` | 新建：28 格(7×4) 状态(locked/claimable/claimed)，打开即请求快照，点击可领格发 CLAIM |
| `IO/UIElement.h` | enum Type `DAILYCHECKIN` |
| `Configuration.h` | `PosDAILYCHECKIN` |
| `IO/UITypes/UISideIconBar.cpp` | include + open_ui `dailycheckin/checkin/sign` 分支 |
| `Util/SideIconConfig.cpp` | defaults 加 slot（注：live ini 已存在则不自动生效，见下） |
| `MapleStory.vcxproj/.filters` | 登记 cpp/h |

## 构建踩坑（沉淀）
- 服务端首编 EXIT=1：`if(!p.available())`——`available()` 返回 int 不能取反，改 `<=0`。
- 发现 live side_icons.ini 已被并发会话改为新格式(local:book 3slot)，defaults 里的 damagerank/dailycheckin 被覆盖丢失——但 UIElement Type/opcode/handler/文件全存活（并发未删核心功能），已重加 defaults 并验证。

## 真机验收（待用户，如实声明：未验收）
1. **@daily 命令**即可打开签到窗（不依赖 ini）。
2. 侧栏图标：live `side_icons.ini` 需手动追加 `slot.N.id=dailycheckin / action=OpenUI / arg=DailyCheckin`（defaults 只在文件不存在时生效，此为改 live 配置，AI 未擅动）。
3. 持久化：apply `daily_checkin_standalone.sql`（表缺失则每次登录内存态重置，功能仍跑不崩）。
