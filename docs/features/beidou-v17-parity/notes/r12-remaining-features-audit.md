# r12 · 参考端剩余功能审计（rea 反编译 BeiDou.exe，2026-10-22）

> 目的：用 rea 枚举参考端（BeiDou v83）已实现、当前 MapleStory-Client 尚未复刻的功能面，与已落地 7 项基线比对，分类【核心功能缺口】与【扩展功能】。
> 约束：只改客户端、不改服务端；能复刻的功能必须 s9 服务端已有对应 handler。
> 参考端二进制：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64`（PE x86，base 0x400000，rea IDA 提供方已加载）。
> 完整 UI 窗口清单权威源：`docs/features/v83-hud-quest/evidence/UIWindow.img.top-level.txt`（v83 GMS 113 个顶层节点）。

## 一、rea 枚举实证（BeiDou.exe 字符串命中）

| 关键字 | 命中（证据） | 说明 |
|---|---|---|
| `MonsterBook` | `UIWindow.img/MonsterBook/{ContextMenu/BtRelease,BtRegister,fullMark,select,cover}` | 怪物图鉴 UI 存在 |
| `Alliance` | `goAllianceTALK`/`goAllianceINVITE`/"Guild Alliance Invitation."/"You joined [%s] guild alliance." 等 | 联盟 UI/流程存在 |
| `CashShop` | `/CashShop.img/CSItemSearch/Price`、`/CashShop.img/CS3thEvent/0`、`CashShop` | 现金商城（已落地购买） |
| `CashGachapon` | `UIWindow.img/CashGachapon/{EffectNormal,EffectJackpot}`、`EXP/ GachaponEXP` | 现金扭蛋 |
| `Gachapon` | `UI/ITC.img/...` 无；仅 `CashGachapon` + `RemoteGachapon` 字段 | 见下 |
| `Auction` | `UI/ITC.img/Auction/{backgrnd,BtAuction,BtBid,backgrnd1}` | 拍卖（ITC） |
| `MapleTV` | `Nexon\MapleStory\MapleTV`、`MapleTVDownloader` | 启动器级电视（非核心玩法） |
| `Wedding` | `wedding%d` | 婚礼（弱证据） |
| `UIWindow.img/Maker` | `Maker/{GaugeBar,gauge,bar,notUse,randomMake,randomRecipe}` | 制造/工匠 |
| `UIWindow.img/ViciousHammer` | `ViciousHammer/GaugeBar/{gauge,bar}` | 神秘锤强化 |
| `UIWindow.img/VegaSpell` | `VegaSpell/{EffectArrow/0,GaugeBar/gauge}` | 维加卷轴 |
| `UIWindow.img/Quest` | `Quest/Gauge/{spot,gauge,frame}` | 任务卷轴（QUESTLOG 已有） |
| `UIWindow.img/PartyRace` | `PartyRace/{Stage,Result}/{...}` | 组队赛跑活动 |
| `UIWindow.img/DualMobGauge` | `DualMobGauge/{Mob/9700036,gauge,text,backgrd}` | 双 mob 血量（活动） |
| `UIWindow.img/UserList/GuildUnion` | `UserList/GuildUnion/backgrnd` | 联盟成员列表 |

## 二、diff：参考端 113 节点 vs 当前客户端能力

当前客户端 `IO/UIElement.h::Type` 已实现（含本轮 7 项）：LOGIN/TOS/GENDER/WORLDSELECT/REGION/CHARSELECT/VIEWALLCHAR/LOGINWAIT/RACESELECT/CLASSCREATION/SOFTKEYBOARD/LOGINNOTICE/STATUSMESSENGER/STATUSBAR/CHATBAR/BUFFLIST/NOTICE/NPCTALK/SHOP/STATSINFO/ITEMINVENTORY/EQUIPINVENTORY/SKILLBOOK/QUESTLOG/WORLDMAP/USERLIST/MINIMAP/CHANNEL/CHAT/CHATRANK/JOYPAD/EVENT/DAILYGIFT/KEYCONFIG/OPTIONMENU/QUIT/CHARINFO/CASHSHOP/STORAGE/PARTY/BUDDYLIST/TRADE/GUILD/MERCHANT。

**当前端确实缺失 UI/handler 的参考端节点（候选缺口）**：
`MonsterBook`(数据层 Character/MonsterBook 已有，UI 缺)、`Maker`、`CashGachapon`/`RemoteGachapon`、`Alliance(GuildUnion)`(66 延后)、`Auction`、`Family`/`FamilyTree`、`Ranking`、`EntrustedShop`/`MemberShop`(玩家店变体，PersonalShop=MERCHANT 已做)、`MonsterCarnival`、`AriantMatch`/`classMatch`、`Wedding`、`MapleTV`、`PartyRace`、`ViciousHammer`、`VegaSpell`、`Minigame`/`RpsGame`/`SpeedQuiz`/`InitialQuiz`(小游戏/答题)、`MobGage`/`DualMobGauge`(活动)、`Incubator`、`StoreBank`、`Delivery`、`Plant`、`TreasureBox`、`NewYearsCard`、`Transform`、`Coupon`、`ItemProtector`、`KarmaScissors` 等。

## 三、s9 服务端 handler 可达性核验（决定能否复刻）

| 功能 | s9 handler | 结论 |
|---|---|---|
| MonsterBook | `MonsterBookCoverHandler.java`（另有 MONSTERBOOK 入站，实现期核对） | ✅ 可复刻（客户端已有数据层） |
| Maker（制造） | `MakerSkillHandler.java` | ✅ 可复刻 |
| Alliance（联盟） | `AllianceOperationHandler.java` + `DenyAllianceRequestHandler.java`（ALLIANCE_OPERATION=66） | ✅ 可复刻（r11 已标延后） |
| RemoteGachapon | `RemoteGachaponHandler.java`（地图扭蛋 NPC） | ⚠️ 仅地图扭蛋，非 CashGachapon |
| Auction | 0 文件 | ❌ 服务端无，需改服务端→排除 |
| Rank/Ranking | 0 文件 | ❌ 服务端无→排除 |
| CashGachapon | 无独立 handler（或走 CashOperationHandler，待核） | ⚠️ 待核，疑似扩展 |
| Family/Wedding/MonsterCarnival/Minigame 等 | 多数无独立 handler 或属活动/扩展 | ⏸ 归扩展功能，本阶段不碰 |

## 四、分类结论

### ✅ 核心功能缺口（参考端有 + s9 有 handler + 当前端缺）→ 本阶段复刻
1. **MonsterBook 怪物图鉴**（UI 窗口 + MONSTERBOOK 入站；客户端 Character/MonsterBook 数据层已具备）
2. **Maker 制造/工匠**（UI + MakerSkillHandler 出站/入站）
3. **Alliance 联盟**（UIGuildUnion + ALLIANCE_OPERATION=66 handler；r11 已标延后，现补完）

### ⏸ 扩展功能（ijl15.dll 33 项，本阶段不碰）
CashGachapon / RemoteGachapon / ViciousHammer / VegaSpell / Auction(无服务端) / Ranking(无服务端) / Family / Wedding / MonsterCarnival / PartyRace / Minigame(Rps/Speed/Initial) / EntrustedShop / MapleTV / MobGage / Incubator / StoreBank / Delivery / Plant / TreasureBox 等 —— 其中 Auction、Ranking 因 s9 无 handler 即便想做也需改服务端，明确排除。

## 五、下一步
- 完成 todo `diff-baseline`：本笔记即 diff 产物。
- 进入 todo `implement-core-gaps`：按 MonsterBook → Maker → Alliance 顺序逐项七层闭环复刻（资源节点确认→入站 handler→出站包→UI→联调→单提交）。
- 协议事实源优先 s9 源码；rea 仅补 UI 节点路径/字段序印证。
- 每项原子提交，r11/r12 笔记持续归档。

## 六、实现记录

### MonsterBook 怪物图鉴（已完成，2026-10-22）
- 协议事实源：s9 `MonsterBookCoverHandler`(收 `MONSTER_BOOK_COVER=0x39`) / `PacketCreator.changeCover`(发 `MONSTER_BOOK_SET_COVER=0x54`) / `addCard`(发 `MONSTER_BOOK_SET_CARD=0x53`)。卡片数据已在登录 `addMonsterBookInfo` 随角色包下发，客户端 `CharacterParser::parse_monsterbook` 已解析（本地完备，无需再拉取）。
- `OutPacket.h`(M)：`MONSTER_BOOK_COVER = 57`。
- `MonsterBookPackets.h`(NEW)：`MonsterBookSetCoverPacket(int32_t cardid)`（write_int）。
- `MonsterBookHandlers.{h,cpp}`(NEW)：`MonsterBookCoverHandler`(SET_COVER=84 → `set_cover`+刷新UI) / `MonsterBookCardHandler`(SET_CARD=83 → `read_byte` full + `int cardid` + `int level`，`cardid%10000` 入库 + 刷新UI)。
- `UIMonsterBook.{h,cpp}`(NEW)：`Type::MONSTERBOOK`。展示封面名 + 卡片总数 + 卡片列表（怪物名经 `nl::nx::String["Mob.img"][id]["name"]` 解析）；点击卡片行发 `MonsterBookSetCoverPacket` 设封面。
- 触发（双保险）：`UIStateGame` `MONSTERCOLLECTION` 键 → `emplace<UIMonsterBook>`；`UICharInfo` 新增 `BtBook` 按钮（节点缺失安全跳过）→ 打开图鉴（稳定可测触发点）。
- 注册：`UIElement.h`(Type::MONSTERBOOK)、`PacketSwitch.cpp`(recv 83/84 + include)、`MapleStory.vcxproj`(UIMonsterBook.cpp / MonsterBookHandlers.cpp)。
- 构建：EXITCODE=0（仅 LNK4098/4099 既有警告）。
- 已知简化：①仅展示登录包已下发的卡片，服务端仅回 changeCover/addCard 增量，无独立"拉取全量"请求（数据本地完备，无需）；②Special 卡(id 238xxxx)沿用服务端 `id%10000` 方案落低位 id，与服务端一致；③点卡片设封面走 MONSTER_BOOK_COVER 出站，服务端回 changeCover 后 UI 刷新。

## 七、Maker 实现记录（已完成）
- 协议事实源：rea 逆向 BeiDou.exe（参考端 v83）确认 `sub_823A13` 按配方计数循环建 UI、本地 `Maker.img` 为数据源；s9 `MakerProcessor.makerAction`（MAKER_SKILL=0x71）读 `type=int + toCreate=int`，`toCreate` = **结果道具 itemId**（`getItemCreateEntry(toCreate)`；仅装备类读 stimulant/reagent）。s9 仅 `MAKER_RESULT=0xD9` 下发结果，从不下发配方目录。
- `OutPacket.h`(M)：`MAKER_SKILL = 113`。
- `MakerPackets.h`：`MakerCreatePacket(int32_t toCreate)`（type=1 + 结果 itemId；装备类补 `write_byte(0)+write_int(0)` 占位）。
- **nlnx 扩展**：`nx.hpp` extern 列表 + `nx.cpp` 节点定义列表加 `Maker`；`nx.cpp::load_all` 三分支挂载 `Maker` 根（Base/退化分支 `add_file("Maker.nx")`，Data.nx 聚合分支 `Base["Maker"]`）；`NxFiles.h` `NUM_FILES` 28→29 + `filenames` 加 `"Maker.nx"`（缺失不影响启动，空节点安全降级）。与参考端一致：配方来自本地 `Maker.img`。
- `UIMaker.{h,cpp}`：`Type::MAKER`。`load_recipes` 优先解析 `nl::nx::Maker["recipe"]`（字段 targetItem/cost/reqLevel，待 Maker.nx 提供时生效）；**兜底**：无 `Maker.img` 时加载内嵌 s9 `makercreatedata` 静态表（`Data/MakerRecipes.h`，由 `tools/gen_maker_recipes.ps1` 从 `V1.0.53__maker_insert_data.sql` 生成，834 条）。图标/名/消耗渲染 + 点击行 `MakerCreatePacket(itemId)`。
- 触发：`UIStateGame` `PROFESSION` 键 → `emplace<UIMaker>`（节点缺失安全跳过）。
- 注册：`UIElement.h`(Type::MAKER)、`vcxproj`。
- 构建：EXITCODE=0（仅 LNK4098/4099 既有警告）。
- 已知简化：①nx 解析字段名（targetItem/cost/reqLevel）按 v83 标准，待 Maker.nx 提供后建议用 rea 核对参考端实际字段；②s9 兜底表仅含 makercreatedata 主行（itemId/meso/reqLevel/reqItem/reqEquip/catalyst/quantity），未展开 makerrecipedata 附加材料；③制造结果依赖 s9 `MAKER_RESULT` 回显（既有流程）。

## 八、Alliance 实现记录（已完成）
- 协议事实源：s9 `AllianceOperationHandler`（recv `ALLIANCE_OPERATION=143`，action 字节：0x01 刷新/0x02 离盟/0x03 邀请/0x0A 公告）+ `GuildPackets`（send `ALLIANCE_OPERATION=66`：`getAllianceInfo=0x0C` / `updateAllianceInfo=0x0F` / `allianceNotice=0x1C`）。
- `OutPacket.h`(M)：`ALLIANCE_OPERATION = 143`（客户端**发**，=s9 RecvOpcode）。
- `AlliancePackets.h`(NEW)：`AllianceActionPacket(action)` / `AllianceNoticePacket`(0x0A+string) / `AllianceInvitePacket`(0x03+string)。
- `AllianceHandlers.{h,cpp}`(NEW)：`AllianceInfoHandler`(recv 66) 解析 0x0C(getAllianceInfo: id/name/5 ranks/guildIds/notice)、0x1C(notice)、0x0F(updateAllianceInfo，跳过 guildInfo 块)，写入 `UIAlliance`。
- `UIAlliance.{h,cpp}`(NEW)：`Type::ALLIANCE`。打开即发 0x01 请求；展示联盟名/公告/成员公会 id 列表；`BtRefresh`(0x01)/`BtLeave`(0x02)。背景 `UIWindow.img/UserList/GuildUnion/backgrnd`。
- 触发：`UIGuild` 新增 `BtAlliance` 按钮（节点缺失安全跳过）→ `emplace<UIAlliance>`。
- 注册：`UIElement.h`(Type::ALLIANCE)、`PacketSwitch.cpp`(recv 66 + include)、`vcxproj`。
- 构建：EXITCODE=0（仅 LNK4098/4099 既有警告）。
- 已知简化：①仅展示联盟名/公告/成员公会 id（公会名需 guildInfo 块，0x0F 已跳过）；②创建联盟/邀请/改公告需文本输入（UISoftKey），当前未接按钮（仅刷新/离盟可测）；③0x0F 的 guildInfo 块整体跳过，仅取 guild id 列表。
- **2026-11-06 补完（随 wedding-flow 同轮）**：上述简化②③已消解 —— 0x0F 的 guildInfo 块已按 s9 `getGuildInfo` 写序解析出公会名（`parse_guild_info_name`）；邀请/公告按钮 + SoftKey（`BtInvite`/`BtNotice`/`I`/`N` 键）已接；0x0D(getGuildAlliances)/0x03(allianceInvite→UIYesNo+`AllianceAcceptPacket` 0x04，消费 GameOpt `UNION_APPLY`)/0x02/0x0E/0x18/0x1A/0x1D(disband) 分支补全；`ensure_ui` 收口为「仅活动窗更新」（R28-B2）。创建联盟仍走 s9 NPC 脚本（非 c2s，端内无需包）。详见 [social-loop-audit-2026-11](../../social-loop-audit-2026-11/)。


