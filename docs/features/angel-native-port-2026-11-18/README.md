# Angel 原生可移植项审计与接线（2026-11-18）

> 目标（用户）：把 Angel 已实现、本端未移植但可移植的逻辑全部移植进来，向完美还原原生对齐。
> 方法：8 路并行只读审计（REA/IDA/parity 台账 + 当前源码现场逐条复核，不信旧台账），再按证据接线。
> 构建 `tools/build_smoke.bat` EXITCODE=0、GATE OK、无 error。live=归档 SHA256 `b76e50829bbe3234e21659004cba82df1a90c4562a7f623260fe8549cd6e99fa`（00:42:06）。

## 一、审计结论总览（关键：旧台账大面积过期）

旧 `parity-gap-2026-11` / `feature-gap-inventory-2026-11` 的大量 ❌ 是**未回写的僵尸记录**。逐条复核当前源码后：
- **已闭环（无需动）**：临时属性面板、道具 opcode 8 项(73/82/85/78/81/75/53/116)、地图交互 5 项(100/101/206/115/376)、s2c 3 项(287/288/376)、宠物显示+发包 8 项(98/167/170/76/77/175/176/181)、长伤害/骰子/伤害皮肤、GuildBBS、公会副本 HUD。
- **真缺且无依赖（本次已接）**：USE_ITEM_REWARD(112)。
- **真缺但需前置（分期）**：宠物 C/T/E/P 四项、新窗三项。
- **阻塞/已裁决不做**：战斗环境 5 项(56/191/192/193/194 缺玩法面)、212 语义冲突待裁决、Title/Ranking/MTS 无数据源已裁决。

## 二、本次已接线

### USE_ITEM_REWARD(112) — 物品奖励盒开奖
- 根因：`UIItemInventory.cpp:853-857` 202* 分支只缓存 `use_slot/use_id` 就 break，死代码。
- 依据：包类 `Net/Packets/MapInteractPackets.h:67-76`（wire=short slot+int itemid）就绪；s9 `ItemRewardHandler.java` 完整开奖逻辑；同函数 227*/500* 双击即发为同构范式。
- 修复：`UIItemInventory.cpp:855-858` 双击 202* → `UseItemRewardPacket(slot,item_id).dispatch()`。纯客户端接线，无服务端改动。
- 状态：✅ 构建通过。**真机未验收**（需游戏内实际双击奖励盒物品验证开奖）。

## 三、分层清单（下一步）

| 层 | 项 | 前置依赖 | 可移植性 |
|---|---|---|---|
| A 可接线但需 SoftKey 回传基建 | PET_CHAT(168)/COMMAND(169)/AUTO_POT(171)/EXCLUDE(172)，包类齐、发送分支空 | 全端 SoftKey 文本回传链路缺（`UICoupon` S 分支也空）；D12 归 P3 backlog 需拍板；EXCLUDE 的 s2c 回包亦空壳 | 基建做好后可批量接 |
| B 新窗（需工具） | 七彩棱镜染色(21)、魔方结果窗(23)、背包格锁(24) | REA 测绘 + Photoshop 出图 + 资源入库；当前仅 orange-wz(10012) 在线，IDA/REA/Photoshop MCP 未暴露 | 工具补齐后可建 |
| C 阻塞 | 56/191/192/193/194 战斗环境报文 | 缺 Boss/障碍/联机互伤/道场内容面，补枚举=死包 | 随内容批 |
| D 待裁决 | 212(SNOWBALL↔LEFT_KNOCKBACK 语义冲突，D5 至今未拍板) | 人工裁决 | 不得盲改 |
| E 已裁决不做 | Title/Ranking/MTS；PARTY_SEARCH_REGISTER 等 | 无数据源/空 stub，用户已销案 | 维持 |

## 四、续作轮新增（2026-11-18 第二轮）

用户纠正"软键盘只用于二级密码、其他走物理键盘"——核实属实（UICharSelect Mode::Pic、UI.cpp:317 已移除裸字母 SoftKey），子 agent"SoftKey 基建缺失"结论被推翻并记录。利用现成 REA 反编译文本 + orange-wz(已在线) 深挖宠物 A 层，取得完整原生契约（见 [notes/pet-native-contract.md](notes/pet-native-contract.md)）：
- 宠物说话/指令输入来自聊天栏文本（`CPet::ParseCommand(sChat)`@0x6a3cc0 / `DoAction(...,sChat,...)`@0x6a2340），不是工具窗/软键盘 → `UIUtilDlgExPet` C/T/E/P 空分支是错误设计残留，不往里填。
- 4 包 s9 handler 齐全、wire 逐包核对与本端 packet 类一致（勿按 095 原生 wire 改）。
- orange-wz 实测宠物 interact 命令表就在 `Data/Item/Pet/<petid>.img/interact`（command/inc/prob/l0/l1/success/act/fail/act），= 原生 INTERACTION 数据源。取证脚本 notes/_owz_pet_query.py、_owz_pet_tree.py 可复用。
- 结论：A 层证据链齐，但"宠物命令"是完整 feature（PetData 加载+聊天分流+亲密度判定+动作播放+4 包），非单点接线，留作 goal 下轮实现。

## 五、第三轮：宠物命令主链已实现（2026-11-18 01:14）

全链核查后的关键收敛：s9 `PetChatHandler` 直接广播客户端的 act+text；`PetCommandHandler` 用 `PetDataFactory.getPetCommand(petid, command)` 做权威概率判定并广播；**本端 s2c `PetCommandHandler`(174) 已用 mode 驱动 `move_pet` 动作、s2c `PetChatHandler` 已 `speak()` 画气泡**——本地表现层全已有。客户端唯一缺口 = c2s 命令翻译发包。

实现：
- `IO/UITypes/UIChatBar.cpp:65` `try_pet_command()`：纯聊天文本（非 /@! 命令）遍历当前召唤宠物（Player.get_pet(i)，itemid/uid>0），读 `nl::nx::Item["Pet"][iid+".img"]["interact"][N]["command"]`，匹配则发 `PetCommandPacket(uid, N)`（:105，int8 溢出保护），支持「宠物名+空格+命令」前缀（原生 IsNamedPet 分支）。
- `:2616` 在 `send_message` 的 try_chat 后、斜杠命令前插入分流，命中即消费不走普通聊天。
- **序号一致性验证**：s9 `PetDataFactory.java:49-55` 也读 `Pet/{petid}.img/interact/{skillId}/prob|inc` —— 与客户端同一个 WZ 节点、同一个 N，序号语义完全对齐（关键 P1 已排除）。
- 构建 EXITCODE=0、GATE OK、无 error；live=归档 SHA `5208f0c4a9e1b032aac6fd4d8ad28ef4c5486548d48f4c7c2ddc92c088b4ada2`（01:14:33）。构建前一次 LNK1104（exe 被客户端 PID 17016 占用），taskkill /F /PID 后重建通过。
- **真机未验收**：需游戏内召唤宠物后在聊天栏打 interact 命令（如 c1）验证宠物动作/亲密度反馈。

下轮剩余（宠物 4 包另外三项）：PET_CHAT 需确认"玩家主动让宠物说话"的客户端入口语义；PET_AUTO_POT 需自动药配置 UI；PET_EXCLUDE 需排除列表管理 UI——三者各自需要独立 UI 入口数据源，非简单接线，按依赖排期。新窗 B 层（棱镜/魔方/格锁）Photoshop 已启动但 REA MCP 仍未暴露，待工具齐全建窗。

## 六、验收状态

- 编译/链接：✅ EXITCODE=0、GATE OK、无 error C/LNK/fatal。
- SHA：接 USE_ITEM_REWARD 时 live=归档 `b76e5082…`（00:42）；后复查 live 已被登录器并发回滚为 `eeaf53ac…`，源码改动坐实（UIItemInventory.cpp:856），下次构建重生成。旧 SHA 声明失效。
- **真机验收：⏳ 未做**（奖励盒开奖、后续各项均未登录实测；构建通过≠功能正确）。

## 五、复测步骤（本端真机）

1. 背包双击 202* 物品（物品奖励盒/幸运箱）→ 服务端开奖、获得奖励物品。
