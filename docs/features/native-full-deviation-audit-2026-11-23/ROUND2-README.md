# 第二轮查漏补缺 · 残留偏差排查（2026-11-24）

> 触发：用户「是否还存在别的功能偏差，挨个排查一遍」。第一轮 8 域审计 + 修复已 commit `9ef96287`。本轮针对第一轮**未深挖的层面**（协议/渲染/音频/数据加载器/输入平台）和**标为预留/NO-FIX 的项**做第二轮排查。
> 性质：**只读排查**，未改源码、未编译、未发包。真机未验，逐条声明。
> 方法：5 个只读子代理分域 + lead 对每条高严重度结论源码复核 + orange-wz D3 二次实证（纠正首轮误判）。

## 0. 一句话结论
还有偏差，且发现**一处 lead 首轮的判断错误**：首轮 D3 只探 `Skill/210.img` 就断言"技能 6 类特效族资源缺源、不造"，本轮跨 5 个 img 复查证伪——prepare/keydown/keydownend/screen/special/tile **6 类实际有源**，只有 receiver/finish 真缺。此外确认 2 个 P1（输入按键 repeat 连发、渲染 z 序分桶潜在）、多个 P2（右 Ctrl bug、EquipGrowthData 逻辑恒假、技改 5 字段未消费、音频 HSTREAM 双缩放等），以及一批"前轮判阻塞其实已可修"的翻案项。

## 1. ★ 首轮误判纠正（最重要，lead 已实证）
| 项 | 首轮结论 | 本轮实证 | 影响 |
|---|---|---|---|
| 技能特效族 6 类（prepare/keydown/keydownend/screen/special/tile） | "资源缺源，只预留不造" | orange-wz 跨 5 img 复查（`tools/_d3_skill_effect_verify2.py`，128B key，isError:false）：`1412.img` 单文件即含 prepare/keydown/keydownend/screen/tile；`1510/000/2211.img` 含 special。**只有 receiver/finish 全库 0 命中**。首轮 `_d3_skill_effect_deep.py` 只 load 210.img → 以偏概全 | 可修：客户端补节点解析+通用 ScreenTileEffect 播放（不动服务端/不造资源）。这是当前最高价值的真实可修偏差 |
| 伤害皮肤 | 列为预留 | 全链路已闭环（DamageSkin.img 624 节点 + 收发包 + handler + 服务端齐全） | 销案，真机点验即可 |
| 地图搜索 | 列为预留 | 已完整实现（UIWorldMap.cpp:59/133-204 + UIMapSearchPanel + WorldMapSearch 节点） | 销案 |
| 怪物手册 LeftTabInfo | 缺失 | 已实现（UIMonsterBook.cpp:179-216），仅 ContextMenu 仍缺（资源在位可修） | 部分销案 |
| 雇佣商人进店 | "服务端无 handler" | 服务端 HiredMerchantRequest 完整（PacketProcessor.java:270 注册） | 翻案：非服务端阻塞 |
| 公会徽章编辑 | "服务端无后端" | s9 GuildOperationHandler.java:222-244 case 0x0f 完整（校验+扣费+setGuildEmblem+广播） | 翻案：补客户端发包即可 |
| 小游戏 Omok/MatchCard/RPS | "空键" | 服务端完整 + 客户端主链路已通（建房/准备/落子/翻牌），剩棋盘/动画打磨 | 翻案：主链路销案 |
| 好友分组窗 | "整族缺失" | 本地分组筛选已闭环（UIUserList.cpp:227/345-360/899-902），独立浮窗需双端 | 翻案 |
| 地图搜索 / MapleTV / 交易所 | — | 搜索已实现销案；MapleTV 服务端有主体但客户端 BtOk 空（需 REA）；交易所资源在位但服务端无后端（双端立项） | 分项 |

## 2. 本轮新发现的真实偏差（按严重度）

### P1（2 项）
| 项 | 证据 | 对照 | 说明 |
|---|---|---|---|
| 按键 repeat 连发抑制缺失 | `IO/Window.cpp:824-836` SDL_KEY_DOWN/UP 直接 `UI::send_key`，未读 `event.key.repeat`；`IO/UI.cpp:218-554` 无连发抑制 | MinMS `IO/UI.cpp:401-408`：pressed && is_key_down && (ITEM/SKILL/MACRO) 直接 return | 长按技能/道具/宏连续触发，可能清空整叠道具。修复：UI::send_key 加 MinMS 同款 one-shot 守卫（复用 event.key.repeat 或 is_key_down） |
| 批渲染 z 序被 blend 分桶破坏 | `Graphics/GraphicsGL.cpp:2053-2218 draw_batches()` 按 main→mul(2094)→add(2131)→glow(2173) 分四次绘制，quads 按提交序排 z | MinMS `GraphicsGL.cpp:1742-1870` 单一队列一次 glDrawArrays | 若普通 sprite 与 glow/tint quad 交错提交（z 相邻）叠放次序会错。当前调用面（WeatherLamps/Fx）在场景末尾，其后仅 UI，故**当前不触发**；属潜在结构缺陷，真机交错场景需确认 |

### P2（重点项）
| 项 | 证据 | 说明 |
|---|---|---|
| 右 Ctrl 键码映射错误 | `IO/Keyboard.cpp:123-126 rightctrlcode()` 返回 `GLFW_KEY_LEFT_CONTROL`（应为 RIGHT_CONTROL=345）；`UI.cpp:285` 左/右 ctrl 两项同值 | 右 Ctrl 无法参与 Ctrl+C/V；修复 1 行 |
| EquipGrowthData 文案保留分支恒假 | `Data/EquipGrowthData.cpp:210` 先 `e.local_only=false`，`:215` 才读它算 keep_local ⇒ 恒 false，`:217-225` 无条件清空本地成长文案 | 服务端 has_data=false 时保留本地文案的逻辑永不生效。真机未验 |
| 技改覆盖 14 字段中 5 个被忽略 | `Data/SkillData.cpp:277-288`（case 5 time/6 cooltime/7 x/8 y/10 watk 忽略）；handler `CommonHandlers.cpp:246-254` 逐字段收 | 服务端技能平衡改这 5 项客户端静默无效；根因 `SkillData.h:32-57` Stats 缺字段（MinMS 有 hp/moneycon/itemcon/speed/jump/x/y/z/time） |
| 音频环境音 HSTREAM 双缩放 | `Audio/Audio.cpp:114 create_bgm_stream` HSTREAM + `:499 BASS_ATTRIB_VOL`，但 `:461 BASS_CONFIG_GVOL_STREAM` 是 BGM 全局音量 | 注释称环境音跟随 SFX、不受 BGM GVOL_STREAM，实际 HSTREAM 受其二次缩放 → 调 BGM 音量会连带改环境音 |
| LEVELUP/TOMBSTONE 已注册无播放点 | `Audio/Audio.h:66-67` 枚举 + `Audio.cpp:205-206` 加载，全仓无 `Sound::Name::LEVELUP/TOMBSTONE` 播放调用（仅定义处） | 升级/墓碑音效不播（MinMS 有播放点） |
| 动画帧控制 API 缺失 | `Animation.h:66-101` 无 update(timestep,loop)/total_delay/set_zigzag/freeze_at 等；`Animation.cpp:186 std::set` 排序去重帧 | MinMS `Animation.h:67-153` 全套。导致 pet stand `0,1,2,1,0,5` 回摆帧被 set 去重吞掉、Papulatus 冻结/展开、mob 攻击整段 hold 等精细动画缺失。当前端 vs MinMS 最大功能落差 |
| EffectLayer 缺 delay/下落/粒子/偏移 | `EffectLayer.h:42-68` 仅 Effect(Sprite+speed)，无 add_fall/add_particle/add_offset；全仓零命中 | MinMS `EffectLayer.h:43-79` + FallEffect/OffsetEffect 类。技能 special 雨滴/大爆炸/名牌环形粒子等无延时位移特效 |
| 文本不响应 xscale/yscale | `GraphicsGL.cpp:1806-1960 drawtext` 不读 DrawArgument 缩放；`Text.h:47-49` 注释自承 | MinMS `GraphicsGL.cpp:1523-1529` 缩放字形。当前靠新增 A22M 字号绕开，功能等价但契约不一致 |
| 键盘 assign 越界防护缺失 | `Keyboard.cpp:148-157` 无 key 边界检查；MinMS `Keyboard.cpp:119-122` 有 `key>=KEYTABLE_SIZE return` | 异常 KEYMAP 包 key>89 越界写，健壮性缺陷 |
| 技改 x/y 与元素 | `Mob.cpp:1002-1004` 已用 attack.ignoredef；`SkillData.cpp:99` element 写入但无 getter | 首轮"Attack.h 无 element"说法不准——ignoredef 已在位；元素克制维持"不改以避免与 s9 权威结算不一致"裁决 |

### P3（精选）
IME 预编辑态（Window.cpp:892-899 只处理 TEXT_INPUT，无 TEXT_EDITING；ime_context/ime_suppressed 死代码）；滚轮/右键/双击无 enabled 门禁（UI.cpp:168-216）；失焦不清按键态（UI.cpp:145-166，与 MinMS 同源缺失）；ItemData 消耗音效缺（MinMS play_use_sound）；EquipData FAME(reqPOP) 未接；MapInfo 死字段/swimArea；WeaponData::get_attackdelay 除零（死代码，MinMS 同源）；夜色 tint alpha 潜在误乘（GraphicsGL.cpp:917，当前 alpha 恒 1 不触发）；MobHpBar 无 clamp（服务端保证 0-100）。

## 3. 明确无偏差 / 已销案（防重复立项）
协议层：161 个 c2s opcode 全部有构造点、无空 handler；WEAPON_TINT_SYNC(0x372F) 是 s9 旧巴西服双实现噪音（客户端走当前 0x184/0x11D ColorPrism 通路，已对齐）；ColorPrism 当前通路逐字段对齐 s9。渲染：分辨率适配、UTF-8/中文、描边、origin 锚点、GL 状态还原均有/等价。音频：环境音循环已实现。数据：MobDropData 非 stub（handler 填充）、WANTS_COVER 机制在本基线不适用、0x175/0x176/0x177 handler 对齐。输入：拖拽模型、Quickslot 8 键、剪贴板、码点级中文删改均已闭环。

## 4. 仍真阻塞（非"现在能修"）
- Mob 出生特效：三方确认真缺 `Mob/SpecialEff.img`（逻辑播放点已预留 Mob.cpp:224-230，补资源即可，需跨端转 key）。
- receiver/finish 两类技能特效：全库 0 命中，维持预留。
- 活动 HUD 空键（S/T）：服务端活动协议缺失。
- PARTY_SEARCH_REGISTER(220)：s9 PartySearchRegisterHandler 确为空体（222/223 已等价代理）。
- 交易所完整交易、MapleTV 完整播放：前者服务端无后端、后者需 REA/IDA 动作帧取证。
- 元素克制/mastery、冲锋地形步进+magnet_pull、武装 mount action：风险裁决 / 需真机手感 / 需逆向取证，非资源或协议硬阻塞。

## 5. 建议优先修复（纯客户端、零服务端、零造资源）
1. 技能特效族 6 类（1412.img 14121004 四合一验证成本最低）——补 `Skill.cpp` 节点解析 + `SkillUseEffect.h` 通用播放类（receiver/finish 仍预留）。
2. 输入按键 repeat one-shot 守卫（UI::send_key，MinMS UI.cpp:401-408）——P1，防长按连发。
3. 右 Ctrl 键码 1 行修复（Keyboard.cpp:125）。
4. EquipGrowthData keep_local 分支修正（把 local_only 读取提到赋值前）。
5. 公会徽章发包（GuildPackets.h 增 GuildChangeEmblemPacket，按 s9 0x0f 读序）+ RECOIL_SHOT 枚举（SkillId.h 加 5201006）。

## 6. 声明
本轮全部为静态只读分析 + orange-wz D3 二次实证；未改源码、未编译、未真机发包。首轮"技能 6 类特效族资源缺源"判断错误已在 §1 如实纠正。截图未采信（来源端未确认）。用户拍板后再决定是否进入修复 Plan。
