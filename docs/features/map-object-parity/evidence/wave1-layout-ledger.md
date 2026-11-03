# 波1 · 地图对象封包布局对账单

日期：2026-11-03 · 权威来源：BeiDou-Server_s9 gms-server `PacketCreator.java`（PC）
/ `AbstractMovementPacketHandler.java`（AMPH）；客户端逐字节仿真 11 形态全过。

## 修正子代理账本（重要）
子代理审计报告中 #4 MoveMobPacket、#10 MobMoved、#3 SpawnPet 三处「字节数错位」
经逐字节仿真（tools 下模拟服务端写序 × 客户端读序）**证伪：实际对齐**（其算术漏算了
bool/short 宽度）。真实缺陷以下表为准，全部有 PC 行号 + 仿真双重证据。

## 修复清单
| 包 | 客户端位置 | 缺陷 | 服务端依据 | 表现（修前） |
| --- | --- | --- | --- | --- |
| SPAWN_SPECIAL_MAPOBJECT | MapObjectHandlers SpawnSummonHandler | skill 与 level 间的 `byte 0x0A` 填充被当 level 读，pos 起错位 | PC:1167-1181 | 召唤兽等级恒 10、落位错 1 字节 |
| SPAWN_MIST | SpawnMistHandler | 框为 4×int（16B），客户端按 4×short（8B）；skillDelay short 未消费 | PC:4074-4088 | 毒雾位置读到 skillDelay 区 |
| SPAWN_MOB / _C | SpawnMobHandler / SpawnMobControllerHandler | 尾段 4 形态（parentless/fake/makeReal/parent）宽度不同：marker byte vs short、有无 team；写死 marker 当 team | PC:1449-1459/1544-1564/1578-1598/1606-1619 | team 恒 -1/-2、淡入失效、扎昆之手类错位 |
| SPAWN_MOB_C stati | 同上 | 控制器 encodeTemporary 变长（16B 掩码 + 8B/状态 + 反射计数器），写死 skip(16) | PC:1461-1502/3078-3091 | 接管带 debuff 怪后 pos/stance/fh 全错 |
| 移动流解析 | MovementParser | cmd 3/4/7/8/9（9B teleport）、14（9B）、18/19/20/22（7B relative）、10（1B equip）、21（3B）零消费整包漂移 | AMPH:45-157 | 弹簧/传送/换装后同包后续片段全错位 |
| SPAWN_PET | SpawnPetHandler | uniqueId 是 long、fh 是 short；客户端 int+skip(4)+int 吞了 nameTag | PC:4455-4468 | pid 高 32 位丢失、tag/balloon 并进 fhid |

## 数据接线
MobSpawn/Mob 新增 spawn_effect/parent/parent_oid 存储（波2 播特效/父怪雪藏接线）。

## 验证
- MSBuild Release x64（VS18 v145）：`已完成代码的生成` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
- 11 形态字节仿真：plain/fade/makeReal/fake/eff6/eff15/parent8/parent-3/fake+eff 全 PASS
  （仿真脚本与输出见 git 历史备注；负例 parentoid 高位非零也校验）
