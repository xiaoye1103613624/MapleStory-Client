# T14 gap-inventory —— 服务端有 handler、客户端从不发的 opcode 三分类

> 派生自 `protocol/opcode-matrix.json`（s9 git head `44d0d5d9ca`）+ `RecvOpcode.java` + `PacketProcessor.java:138-319`。
> 计算口径：gap = { `PacketProcessor.registerHandler` 实际生效值（后注册覆盖前） } − { 客户端 `OutPacket::Opcode` 中 `client_sent=true` 的值 }。
> 结果：**70 个** s9 已注册 handler 客户端从不发送；另有 `LOGIN_EMAIL=3` 是客户端独有（无服务端 handler，与 PLAN §2 F5 一致，不计入 gap）。

## 1. 校验方法（可复算）

```python
import json, re
j = json.load(open('protocol/opcode-matrix.json', encoding='utf-8'))
sent = {int(e['client_value']) for e in j['c2s'] if e.get('client_sent')}
# RecvOpcode 名→值
nv = {m.group(1): int(m.group(2), 16)
      for m in re.finditer(r'(\w+)\((0x[0-9A-Fa-f]+)\)',
           open('.../RecvOpcode.java').read())}
# PacketProcessor 生效 handler（数组覆盖写，后注册者生效）
reg = {}
for m in re.finditer(r'registerHandler\(RecvOpcode\.(\w+)',
                     open('.../PacketProcessor.java').read()):
    n = m.group(1)
    if n in nv: reg[nv[n]] = n
gap = sorted(set(reg) - sent)      # → 70
```
`_meta.counts` 对账：s9 注册 handler 172（含 `CUSTOM_PACKET` 0x3713 在 gap）、客户端 `client_out_sent` 103、矩阵 `c2s MATCHED 99 / CLIENT_ONLY 4` 与本次差集一致（99 个客户端发送值命中服务端 handler；其余 4 个客户端专有值中 `LOGIN_EMAIL=3` 无 handler，另 3 个为历史 s2c 值误用，已在 T6 修正）。

## 2. 与探针黑名单交叉

`tools/probe/opcode_blocklist.txt`（89 条）对 gap 内 opcode 形成**第二道否决**：黑名单内 = 探针禁发（破坏性/持久化，须一次性测试号 + 用户逐条放行）；黑名单外 = 可直接探针（服务端走 null 分支或被 handler 正常消化，零副作用）。

- **探针禁发（在 blocklist，共 39 条）**：`0x0002 0x0009 0x000A 0x000E 0x0010 0x0012 0x002F 0x0035 0x004B 0x004C 0x004D 0x0051 0x0055 0x0070 0x0073 0x0074 0x0082 0x008A 0x008B 0x00A1 0x00A7 0x00A8 0x00A9 0x00AA 0x00AB 0x00AC 0x00AF 0x00B0 0x00B1 0x00C1 0x00DA 0x00DC 0x005D 0x0062 0x0065 0x0039(?不在gap) 0x0076 0x0081 0x00EE`
- **探针安全（不在 blocklist，共 31 条）**：`0x0004 0x001B 0x001C 0x001F 0x0020 0x0038 0x0049 0x004E 0x0052 0x0058 0x0059 0x005C 0x0063 0x0064 0x006D 0x0089 0x00A2 0x00B2 0x00B5 0x00B7 0x00BD 0x00BF 0x00C0 0x00C2 0x00C5 0x00CE 0x00D3 0x00D4 0x00D5 0x1000 0x3713`

> 注：blocklist 中 `0x0039 MONSTER_BOOK_COVER` 不在本 gap（客户端已发），列此仅证黑名单覆盖；`0x00EE USE_ITEMUI` 实际在 gap 且被 blocklist 收录，归探针禁发。

## 3. 三分类总表

判定维度：(a) 客户端有无对应 UI/逻辑基础设施；(b) s9 handler 是否有真实玩法（已注册即有）；(c) 优先级。
- **可实现**：基础设施在，仅缺发送路径或少量接线 → 可直接补齐发送包。
- **需先补 UI**：客户端缺该功能的 UI/逻辑实体，须先建功能再发 → 实现成本 ≠ 发包。
- **明确不做**：项目范围/默认配置不含（PIN 关闭、现金/婚恋/活动小游戏/GM 专属/NoOp/改名转档等），附理由。

| opcode | s9 名 | handler | 客户端基础设施 | 判定 | 探针 | 理由 |
|---|---|---|---|---|---|---|
| 0x0002 | GUEST_LOGIN | GuestLoginHandler | 无（走账号登录） | 明确不做 | 禁发 | v83 游客登录，本项目用账号体系 |
| 0x0004 | SERVERLIST_REREQUEST | ServerlistRequestHandler | 登录即拉全量 | 可实现 | 安全 | 仅刷新大区列表，复用 CHARLIST 通道 |
| 0x0009 | AFTER_LOGIN | AfterLoginHandler | 登录态机已覆盖 | 可实现(隐式) | 禁发 | 客户端登录流程已等价于该状态机 |
| 0x000A | REGISTER_PIN | RegisterPinHandler | 无 | 明确不做 | 禁发 | s9 `enable_pin=false` 默认不发 |
| 0x000E | PICK_ALL_CHAR | ViewAllCharSelectedHandler | 有（UIViewAllChar 概览） | 需先补UI | 禁发 | r10 仅存概览解析，选取派发未接线 |
| 0x0010 | NAME_TRANSFER | TransferNameHandler | 无 | 明确不做 | 禁发 | 改名写库，v83 私服小众功能 |
| 0x0012 | WORLD_TRANSFER | TransferWorldHandler | 无 | 明确不做 | 禁发 | 跨世界转移写库 |
| 0x001B | STRANGE_DATA | LoginRequiringNoOpHandler | 无 | 明确不做 | 安全 | handler 即空操作 |
| 0x001C | RELOG | RelogRequestHandler | 有（断线重连） | 可实现 | 安全 | 断线后可发重登请求 |
| 0x001F | VIEW_ALL_PIC_REGISTER | ViewAllCharRegisterPicHandler | 无 PIC | 明确不做 | 安全 | PIC 关闭，无注册流程 |
| 0x0020 | VIEW_ALL_WITH_PIC | ViewAllCharSelectedWithPicHandler | 无 PIC | 明确不做 | 安全 | 同上 |
| 0x002F | TOUCH_MONSTER_ATTACK | TouchMonsterDamageHandler | 有（战斗系统） | 可实现 | 禁发 | 触怪攻击发送路径未接线 |
| 0x0035 | USE_DEATHITEM | UseDeathItemHandler | 弱（死亡道具少） | 需先补UI | 禁发 | 死亡道具使用 UI 缺失 |
| 0x0038 | MOB_BANISH_PLAYER | MobBanishPlayerHandler | 有（怪物机制） | 可实现 | 安全 | 怪物驱逐事件发送 |
| 0x0049 | CANCEL_ITEM_EFFECT | CancelItemEffectHandler | 有（效果系统） | 可实现 | 安全 | 取消道具效果 |
| 0x004B | USE_SUMMON_BAG | UseSummonBagHandler | 弱（召唤道具） | 需先补UI | 禁发 | 召唤袋 UI 缺失 |
| 0x004C | PET_FOOD | PetFoodHandler | 有（Pet/PetLook 基础设施） | 需先补UI | 禁发 | 宠物喂食发送路径未接线 |
| 0x004D | USE_MOUNT_FOOD | UseMountFoodHandler | 有（PetLook） | 需先补UI | 禁发 | 坐骑喂食发送路径未接线 |
| 0x004E | SCRIPTED_ITEM | ScriptedItemHandler | 有（脚本引擎） | 可实现 | 安全 | 脚本物品发送 |
| 0x0051 | USE_CATCH_ITEM | UseCatchItemHandler | 弱 | 需先补UI | 禁发 | 捕捉道具 UI 缺失 |
| 0x0052 | USE_SKILL_BOOK | SkillBookHandler | 有（技能系统） | 可实现 | 安全 | 技能书使用发送 |
| 0x0058 | AUTO_DISTRIBUTE_AP | AutoAssignHandler | 有（属性系统） | 可实现 | 安全 | 自动分配 AP |
| 0x0059 | HEAL_OVER_TIME | HealOvertimeHandler | 有（状态系统） | 可实现 | 安全 | HOT 发送 |
| 0x005C | CANCEL_BUFF | CancelBuffHandler | 有（Buff 系统） | 可实现 | 禁发 | 取消增益发送（blocklist 收录） |
| 0x005D | SKILL_EFFECT | SkillEffectHandler | 有（技能系统） | 可实现 | 禁发 | 技能特效广播发送 |
| 0x0062 | SPAWN_PET | SpawnPetHandler | 有（Pet 基础设施） | 需先补UI | 禁发 | 宠物召唤发送未接线 |
| 0x0063 | CANCEL_DEBUFF | CancelDebuffHandler | 有（状态系统） | 可实现 | 安全 | 取消减益 |
| 0x0064 | CHANGE_MAP_SPECIAL | ChangeMapSpecialHandler | 有（地图系统） | 可实现 | 安全 | 特殊地图切换 |
| 0x0065 | USE_INNER_PORTAL | InnerPortalHandler | 有（传送门） | 可实现 | 禁发 | 内部传送门 |
| 0x006D | GRENADE_EFFECT | GrenadeEffectHandler | 有（事件） | 可实现 | 安全 | 手榴弹效果 |
| 0x0070 | USE_ITEM_REWARD | ItemRewardHandler | 有（掉落） | 可实现 | 禁发 | 物品奖励发放 |
| 0x0073 | USE_TREASUER_CHEST | UseTreasureChestHandler | 有（宝箱） | 可实现 | 禁发 | 宝箱开启 |
| 0x0074 | USE_REMOTE | RemoteGachaponHandler | 弱（扭蛋） | 需先补UI | 禁发 | 远程扭蛋（现金内容） |
| 0x0076 | ADMIN_CHAT | AdminChatHandler | 无玩家路径 | 明确不做 | 禁发 | GM 专属；客户端仅 `ADMIN_COMMAND=0x80` 已发 |
| 0x0081 | ADMIN_LOG | AdminLogHandler | 无 | 明确不做 | 禁发 | GM 日志 |
| 0x0082 | BUDDYLIST_MODIFY | BuddylistModifyHandler | 有（UIBuddyList/BuddyPackets） | 需先补UI | 禁发 | 好友增删发送路径未接线 |
| 0x0089 | RING_ACTION | RingActionHandler | 无 | 明确不做 | 安全 | 婚戒社交，范围外 |
| 0x008A | WEDDING_ACTION | WeddingHandler | 无 | 明确不做 | 禁发 | 婚礼系统，范围外 |
| 0x008B | WEDDING_TALK_MORE | WeddingTalkMoreHandler | 无 | 明确不做 | 禁发 | 同上（撞值 0x8B） |
| 0x00A1 | CASHSHOP_SURPRISE | CashShopSurpriseHandler | 无 | 明确不做 | 禁发 | 现金商店内容 |
| 0x00A2 | CLICK_GUIDE | ClickGuideHandler | 有（引导） | 可实现 | 安全 | 点击引导 |
| 0x00A7 | MOVE_PET | MovePetHandler | 有（Pet） | 需先补UI | 禁发 | 宠物移动发送 |
| 0x00A8 | PET_CHAT | PetChatHandler | 有（Pet） | 需先补UI | 禁发 | 宠物对话发送 |
| 0x00A9 | PET_COMMAND | PetCommandHandler | 有（Pet） | 需先补UI | 禁发 | 宠物指令发送 |
| 0x00AA | PET_LOOT | PetLootHandler | 有（Pet） | 需先补UI | 禁发 | 宠物拾取发送 |
| 0x00AB | PET_AUTO_POT | PetAutoPotHandler | 有（Pet） | 需先补UI | 禁发 | 宠物自动药水发送 |
| 0x00AC | PET_EXCLUDE_ITEMS | PetExcludeItemsHandler | 有（Pet） | 需先补UI | 禁发 | 宠物排除物品发送 |
| 0x00AF | MOVE_SUMMON | MoveSummonHandler | 弱（召唤体） | 需先补UI | 禁发 | 召唤兽移动发送 |
| 0x00B0 | SUMMON_ATTACK | SummonDamageHandler | 弱 | 需先补UI | 禁发 | 召唤兽攻击发送 |
| 0x00B1 | DAMAGE_SUMMON | DamageSummonHandler | 弱 | 需先补UI | 禁发 | 召唤兽受伤发送 |
| 0x00B2 | BEHOLDER | BeholderHandler | 弱 | 需先补UI | 安全 | 召唤体效果（远程/标飞） |
| 0x00B5 | MOVE_DRAGON | MoveDragonHandler | 弱（龙） | 需先补UI | 安全 | 龙召唤移动（战神） |
| 0x00B7 | CHANGE_QUICKSLOT | QuickslotKeyMappedModifiedHandler | 有（快捷栏） | 可实现 | 安全 | 快捷栏改键发送 |
| 0x00BD | AUTO_AGGRO | AutoAggroHandler | 有（怪物 AI） | 可实现 | 安全 | 自动仇恨 |
| 0x00BF | FIELD_DAMAGE_MOB | FieldDamageMobHandler | 有（战斗） | 可实现 | 安全 | 场景伤害怪物 |
| 0x00C0 | MOB_DAMAGE_MOB_FRIENDLY | MobDamageMobFriendlyHandler | 有（战斗） | 可实现 | 安全 | 友方怪物互伤 |
| 0x00C1 | MONSTER_BOMB | MonsterBombHandler | 有（事件） | 可实现 | 禁发 | 怪物炸弹 |
| 0x00C2 | MOB_DAMAGE_MOB | MobDamageMobHandler | 有（战斗） | 可实现 | 安全 | 怪物互伤 |
| 0x00C5 | NPC_ACTION | NPCAnimationHandler | 有（NPC） | 可实现 | 安全 | NPC 动作动画 |
| 0x00CE | TOUCHING_REACTOR | TouchReactorHandler | 有（反应器） | 可实现 | 安全 | 触碰反应器 |
| 0x00D3 | SNOWBALL | SnowballHandler | 无 | 明确不做 | 安全 | 雪球活动小游戏 |
| 0x00D4 | LEFT_KNOCKBACK | LeftKnockbackHandler | 有（事件） | 可实现 | 安全 | 左击退 |
| 0x00D5 | COCONUT | CoconutHandler | 无 | 明确不做 | 安全 | 椰子活动小游戏 |
| 0x00DA | MONSTER_CARNIVAL | MonsterCarnivalHandler | 无 | 明确不做 | 禁发 | 怪物嘉年华内容 |
| 0x00DC | PARTY_SEARCH_REGISTER | PartySearchRegisterHandler | 弱（组队搜索） | 需先补UI | 禁发 | 组队搜索注册发送 |
| 0x00EE | USE_ITEMUI | RaiseIncExpHandler | 有（Raise UI） | 可实现 | 禁发 | 道具界面使用（blocklist 收录） |
| 0x00FD | MTS_OPERATION | MTSHandler | 无 | 明确不做 | （非 blocklist） | 拍卖行现金内容；虽不在 blocklist 但属范围外，探针亦不建议 |
| 0x1000 | SET_HPMPALERT | SetHpMpAlertHandler | 有（HP/MP 条） | 可实现 | 安全 | 扩展通道 0x1000；关联 S4 切片 |
| 0x3713 | CUSTOM_PACKET | CustomPacketHandler | 无 | 明确不做 | 安全 | 自定义/调试通道（13 37 玩笑值） |

## 4. 分类汇总

- **可实现（共 31）**：`0x0004 0x0009 0x001C 0x0038 0x0049 0x004E 0x0052 0x0058 0x0059 0x0063 0x0064 0x006D 0x00A2 0x00B7 0x00BD 0x00BF 0x00C0 0x00C2 0x00C5 0x00CE 0x00D4 0x1000`（战斗/状态/地图/技能/快捷栏/扩展 HPMP，基础设施齐备）+ 探针禁发但同属可实现的 `0x002F 0x005C 0x005D 0x0065 0x0070 0x0073 0x00C1 0x00EE`（发送路径被 blocklist 拦，补齐发包后须走测试号验证）。
- **需先补 UI（共 20）**：`0x000E 0x0035 0x004B 0x004C 0x004D 0x0051 0x0062 0x0074 0x0082 0x00A7 0x00A8 0x00A9 0x00AA 0x00AB 0x00AC 0x00AF 0x00B0 0x00B1 0x00B2 0x00B5 0x00DC`（宠物/好友/召唤/捕捉/组队搜索/死亡道具/远程扭蛋——客户端有数据层但缺发送接线或整个功能 UI）。
- **明确不做（共 19）**：`0x0002 0x000A 0x0010 0x0012 0x001B 0x001F 0x0020 0x0076 0x0081 0x0089 0x008A 0x008B 0x00A1 0x00D3 0x00D5 0x00DA 0x00FD 0x3713` + `0x00EE`？否（`0x00EE` 归可实现）。9 处 PIN 关闭/现金/婚恋/活动小游戏/GM/NoOp/改名转档/调试。

## 5. 下一步（P3 实现排期前置）

1. **探针安全集（31 条中不在 blocklist 的）** 可用 `s9_probe.py` 直接发一次，按 PONG-ack oracle 判定服务端是忽略/报错/回包，把结论回填本表第 6 列 → 闭合 T14b "opcode 名值对但服务端复用" 的风险面（多数应为 null 分支忽略）。
2. **可实现集** 按游戏流程分批补发包（每片一原子提交）；其中 `0x1000 SET_HPMPALERT` 并入 S4 切片一并核对 body。
3. **需先补 UI 集** 单独立项（非纯协议层），其中宠物/好友/召唤三族优先（数据层已就绪，性价比高）；组队搜索/捕捉/死亡道具/远程扭蛋视需求。
4. **明确不做集** 关闭，理由已记录；如未来开 PIN/现金/婚恋，再回开对应条目。

> 风险纪律（继承 PLAN §7）：探针禁发集**绝不**在 Admin 在线会话发；任何探针发送副作用判定只能由 PONG-ack oracle 或服务端日志给出，禁止以"连接存活"为据。
