# Angel 本地行为对齐（1B + Q3）

对照源：当前 IDA MCP 上的 `Angel.i64`（v83，imagebase `0x400000`）。本轮只改本地行为。六个 s2c 改造字段宽度、加点掩码、opcode 88 的整数载荷都没动。

## 已补

| 项 | 依据 | 改动 |
| --- | --- | --- |
| 怪物离场 | `CMobPool::OnMobLeaveField @0x67961d`：动画字节 0 只停用，非 0 才死亡 | `Mob::kill` 保持该分支；死亡动画卡住时本地 180 帧后移除 |
| 技能加点 | `CUISkill::OnSkillLevelUpButton @0x8acf1a` 的 500ms 门闩和前置校验 | `send_spup` 在直发和打开确认板之前都走 `can_raise`；确认钮仍走 `spend_sp` |
| 背包悬停文案 | `CUIItem::OnChildNotify @0x81d01f` 的 `0x7D4/0x7D5/0x7D6` | 整理 / 聚集 / 展开 / 收起悬停显示已有 locale 键 |
| 自己拾取 | 083 拾取写入聊天记录 | 仅当 `Stage::is_player(cid)` 时写一行聊天，不改离场包 |

## 核对后已对齐，未再改

- 能力窗手动/自动加点已有 500ms 节流，88 包字节保持现状。
- 键位窗默认与清空都会 `redraw_quickslots()`，135/183 包未改。
- 聊天三态 `set_chat_type(1/2/3)` 已在 `UIChatBar`。
- 世界地图 `parentMap` 已能向上返回。
- 任务窗 `send_key` 保持空：Angel 全库没有任务窗 OnKey。

## 不做

| 项 | 原因 |
| --- | --- |
| 勋章详情窗 `CUIMedalQuestInfoDetail @0x83d255` | 内容未取证，第二次点击只保持选中 |
| 图鉴卡片名与勾选筛选 | 选中卡的名字已经走 `String/Mob.img`；`MonsterBook.img/<mob>/map` 是地图号列表，不是左页 9 个便签的套别。勾选翻转在，按套筛选仍缺归属表 |
| 家族聊天 `change_target` | 不发明新的聊天协议 |
| 龙装备独立窗 | `UIWindow.img/Equip` 只有宠物三槽和 `BtPetEquipShow/Hide`，没有 dragon 节点 |
| 宠物装备独立窗、组队血条独立窗、商城七子窗 | 形态重构，本轮不拆窗 |
| `BtAuto1` / `BtAuto2` 额外落点 | 两颗图在 `Stat` 下，origin 都是 (0,0)，尺寸 73×17。`CUIStat::OnCreate @0x8c4c7f` 不创建它们；和已接上的 `BtAuto`（100×35，落在 (106,198)）不是并排的第二、三颗钮 |
| 槽位合并提示 `inventory.tip_slotmerge` | 没有对应按钮，不发明控件 |
| 现金扩槽提示 `0x7D7` | 没有独立按钮；商城入口不是扩槽 |
| 60 频道翻页 | `Login.img/WorldSelect/channel` 只有 0–19 和 `chSelect`/`chgauge`/`chEvent`，没有翻页钮。超过 30 继续截断并告警 |
| 上次登录世界高亮 | 已有：`LastConnectedWorldHandler` 读 4 字节后调用 `UIWorldSelect::set_focus_world`。列表本身不带这个字段，没有包时不另用未初始化的 `Configuration::worldid` 去猜 |
| 宠物亲密/饱食真条 | 对话壳注释写明没有 gage 节点，继续用色块 |
| 嘉年华 case 296、299/300 撞号 | 会碰到包号归属 |
| 好友分组窗、公会徽章编辑、MapleTV 播放、MTS、称号、场地伤害族 | 缺资源或缺服务端，或会改包 |
| `InPacket::available()` 改成字节数 | 会一次改活大量短包判断 |

工作区里勋章、图鉴、玩家列表、属性名返回值的未提交差异保留：它们是本地显示或悬空引用修复，没有改包。

## 095 龙神资源（只查，未导入）

当前 v83 `Data/Skill` 没有 `2001.img`、`2200.img`、`2210`–`2218.img`。`Data/Character/Dragon` 有 12 张龙装备外观（`01942000`–`01972002`），不是职业技能。

095 PLUS2 客户端的 `Skill.wz` / `String.wz` / `Character.wz` / `UI.wz` 是标准 `PKG1`（`Package file v1.0 Copyright 2002 Wizet, ZMS`）。orange-wz 用同一把 128 字节 `DEFAULT_KEY`，分别试了 GMS IV `TSPHKw==`、CMS IV `uX1j6Q==`、空 IV `AAAAAA==`，目录子节点都是空的，点名 `2001.img` / `000.img` / `Skill.img` 返回「节点路径不存在」。整文件明文扫描也没有 `2001.img` 或「龙神」（名字在目录里是加密的）。

同一套 PLUS2 的服务端 `wz` 已经解成 xml，龙神技能在里面：

| 文件 | 内容 |
| --- | --- |
| `Skill.wz/2001.img.xml` | 约 433KB；`String/Skill.img` 书名「龙初心」 |
| `Skill.wz/2200.img.xml` | 书名「龙神第一次」；`22000000` 名「龙魂」，`22001001` 为魔法弹 |
| `Skill.wz/2210.img.xml`–`2218.img.xml` | 后续转职技能都在 |
| `Character.wz/Dragon` | 16 张 `01942000`–`01972003` |
| `String/Eqp.img` | 有「龙神英雄羽毛」「龙神金色之翼」等装备名 |

这些 xml 没有拷进当前客户端。要在 v83 里玩龙神，还得先把技能图、职业分支和角色龙外观接上，这一步等明确要导入再做。
