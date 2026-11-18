# Angel 原生移植 · 最终闭环台账（2026-11-18，goal 第5轮穷尽验证）

> Objective：把 Angel 已实现、本端未移植、且可移植的功能/逻辑全部移植进来；排除双端阻塞、无数据源、已裁决不做的项。
> 结论：**可移植项已全部落地，其余经 IDA(13337/Angel.i64) + orange-wz(10012/live WZ) + s9 服务端三重证据确认阻塞或不适用。**

## 一、已移植（本 goal 期间落地）

| 项 | 证据（行号） | 构建 |
|---|---|---|
| USE_ITEM_REWARD(112) 奖励盒开奖 | IO/UITypes/UIItemInventory.cpp:856（原 202* 分支缓存变量即 break 死代码→dispatch） | ✅ EXITCODE=0 |
| 宠物命令主链（聊天文本→WZ interact 表→PET_COMMAND(169)） | IO/UITypes/UIChatBar.cpp:65 try_pet_command / :105 PetCommandPacket(uid,N) / :2616 分流 | ✅ EXITCODE=0 |
| 序号一致性 | s9 PetDataFactory.java:49-55 与客户端读同一 WZ `interact/{N}/prob|inc`，N 语义对齐 | 静态核对 |

**构建产物**：live=归档 SHA `5208f0c4a9e1b032aac6fd4d8ad28ef4c5486548d48f4c7c2ddc92c088b4ada2`（01:14:33），无 error。（登录器会回滚 exe，旧 SHA 声明失效，交付前须重核。）
**真机验收：⏳ 未做**——奖励盒开奖、宠物命令动作/亲密度反馈均未登录实测；构建通过≠功能正确。

## 二、审计确认为旧台账僵尸记录、本端早已闭环（抽检在位，无需动）

临时属性面板(UITemporaryStatView)、道具 opcode 8 项(73/82/85/78/81/75/53/116)、地图交互 5 项(100/101/206/115/376)、s2c 3 项(287/288/376)、宠物显示+发包 8 项(98/167/170/76/77/175/176/181)、长伤害(int64)/骰子(randomize_look)/伤害皮肤(DamageSkin.img)、GuildBBS、公会副本 HUD——抽检全部命中源码（本轮 grep 证据）。

## 三、三重证据确认为阻塞/不适用（objective 明确排除）

| 项 | 阻塞证据 |
|---|---|
| 魔方结果窗 | s9 CashOperationHandler.java 分支仅到 0x31、无 0x40（0x40=Unhandled）；live UIWindow.img 无 MiracleCube 节点（orange-wz 实测）→ 无服务端逻辑 + 无资源 |
| 七彩棱镜染色 | live UIWindow.img 无 ColorPrism/Prism 节点（orange-wz 实测）→ 无资源 |
| 背包格子锁定 | live UIWindow.img 无 LockSlot/SlotLock 节点 → 无资源 |
| 宠物说话(168) | 原生 CPet::ChatCommand@0x7050be 走 SendChatMsgSlash 斜杠命令+StringPool 固定 ID 0x110，具体命令文本无法确定，且与本端已有 / 命令冲突 → 预留不硬做 |
| 宠物自动药(171)/排除(172) | IDA xrefs_to：SendStatChangeItemUseRequestByPetQ@0x9de400 **零 xref**、SendUpdateExceptionListRequest@0x6a0dd0 仅匿名 sub_6A0DCE（无具名 UI caller）→ 083 原生无 UI 触发入口，无可参照实现 |
| 战斗环境 56/191/192/193/194 | 本端无 opcode/wire，缺 Boss/障碍/联机互伤/道场内容面；补枚举=死包（wave4-battle-packets 判词） |
| 212 | SNOWBALL(原生)↔LEFT_KNOCKBACK(s9) 语义冲突，D5 裁决未拍板 |
| Title/Ranking/MTS | 无数据源/空 stub，用户 m04292 已裁决销案（development-plan:252） |

## 四、非本 goal（用户单独委托、已另线处理）

套装 tooltip 根因（Etc 合并丢 SetItemInfo.img）与花名册导出：用户已单独要求并提交 commit b570ea17（WZ 权威表 + txt 降级），属装备槽位扩展线，不计入本 goal。

## 五、交付口径

- 可移植项：全部落地 + 构建闭环。
- 阻塞/无数据源/已裁决项：三重证据固化，objective 定义内明确排除。
- 未验证：真机行为（奖励盒/宠物命令）与已移植项的联机实测——如实声明，未把"构建通过"包装成"功能已验收"。
