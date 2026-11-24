# 7 项 UI 问题研判（2026-11-21，点到面逐行号证据）

> 两个取证子代理 + 主代理全链路读码结果。参考截图按 USER.md 红线仅作问题现象，不作本端验收凭据。

## 1. 染色窗 —— 用错了需求包（根因）
- 用户明确指向 `E:\MXD\扩展改动\装备染色功能\`（**中文包**）：指南 `:2-5` 基于 BeiDou/ijl15，NPC 1012117 打开，`UI/UIWindow.img/ColoringPrism/{backgrnd 300×398, BtOff, BtClose, BtCancel, BtOK, enabled}`（orange-wz probe 全部命中）。
- 我当前用的是 `装备染色/coloring-prism/`（**巴西服英文版**）：`ColorPrism.img`，协议 `0x372E/0x372F`。
- **两套协议完全不同**：中文包 C→S `0x11D` / S→C `0x184`（指南 `:78-85`），只染现金装备 104/105/106/110/170（`:18-28`），且要求从**已穿戴栏（slot<0）拖入**（`:28`、`:257`）。"功能用不了"的根因大概率是协议/资源套系错配，不是 UI 偏移。
- 指南 `:251-263` UI 契约：300×398、拖入校验、独立预览 Avatar、滑条 SoftRefresh、取消恢复、确定 ApplyOwned；`:249` 滑条映射 `H=v*1.8, S=v*0.007, L=v*0.0075`。
- 当前 `UIColorPrism.cpp` 是上一轮按巴西服包重写的（5页签+CharLook预览+thumb拖拽+中文文案），**没有接中文包的 BtOK/BtCancel 四态/BtOff**，也没接 0x11D/0x184。

## 2. 伤害统计（三处缺口，证据明确）
- **技能图标**：`UIDamageRank.cpp:736-740` 除 `skill_id==-1` 外一律画 `icon_common_atk`，**没消费 `e.skill_id`**（数据齐：`Character/DamageRank.h:33 SkillEntry.skill_id`）。参考包 `uiDamageRank.cpp:596-633 GetSkillIconCanvas` 用 `Skill/<job>.img/skill/<id>/icon` UOL 取图。项目现成接口 `Data/SkillData.h:95-97/104 get_icon()/get(id)` 可直接用。
- **文字偏下**：全窗 `grep y_for_ink_top=0`，没接项目唯一真源 `Graphics/Text.h:146-164`（`UIStatsInfo.cpp:336/468`、`UIStatusBar.cpp:758` 已按此整改）。玩家行 `cpp:695/699 y+1`、技能行 `cpp:744/748/752/757 y+5/y+22` 全是裸偏移。
- **自适应/下拉**：当前固定高 223（`h:77-78`）、技能最多 4 行可见（`h:95 kSkillFitRows=4`）、滚动条有但 `auto_mode=true` 默认隐藏（`cpp:378-381`）。参考包是**自适应高度 + 明确注释"Scrollbar not implemented"**（`uiDamageRank.cpp:818/946/1009`）——方向相反。此项属需求决策。

## 3. 签到（mock + 丢弃 + 关闭三因）
- **没物品**：服务端 `DailyCheckinRewards.java:2-5` 自述 mock、`grants[]` 全空、icon 全是药水 2000000；客户端 `WaveAShellHandlers.cpp:3387/3389` 原文注释 "iconItemId 预留，本窗不逐格绘制图标 / tooltip 预留" —— **服务端发的 icon+tooltip 全被丢弃**，窗内也不画（`UIDailyCheckin.cpp:103-123`）。
- **多件/默认几件/tooltip**：数据模型支持（`Reward.grants[]` 数组），但协议每格仅 1 个 `iconItemId`（`PacketCreator.java:352`）+ 28 条 tooltip 字符串；需求包 README `:20/72-75/293-310/335-336` 也是"一格一主 icon + hover tooltip 列全部物品"，一格 32px icon 居中，关闭钮 x490,y6,12×12。
- **关不掉**：`UIDailyCheckin.cpp:46-52` 关闭钮 (493,4) **完全落在 dragarea 高 20 内**（`cpp:25`），且 `send_cursor:150-153` 没像 `UIDamageRank.cpp:285-299` 那样把按钮区从拖拽区摘出；`button_pressed:136` 只 deactivate 无 remove_cursor；`UIDailyCheckin.h` **没有 send_key override**（连 Escape 兜底都没有）；TOGGLED=true 常驻，侧栏再点会 toggle_active 重开。
- 数据来自服务端表（可配），当前是 mock；"整体设计"见需求包 README `:16-23/317-337`。

## 4. 图4 "DAILY GIFT" —— 另一个功能，非高版本、非完整移植
- `UIDailyGift.h:31-35` 自述"每日礼物灰框 UI 骨架 / 纯客户端假数据 / 不渲染 ItemData 图标"；`docs/features/daily-gift-hd/README.md:1-4/17` 决策"仅 UI，无协议，无真实领取"，`:83` 关闭钮 Basic.img/BtClose3|2。
- `docs/features/activity-minigame-angel-calibration-2026-11/README.md:115` A14 明确判定"每日签到 → UIDailyGift 615 行仅 UI 假态 ❌ 维持不做"；`evidence/README.md:7` "无封包/IDA 证据"。
- 与 `UIDailyCheckin`（需求包原生移植+服务端闭环）是两套。Angel 原生还原证据：**无，且是反向证据**。

## 5. 任务列表（布局非IMG校正 + 右侧数字根因 + Angel证据边界）
- 布局常量全来自"参考截图实测"（`UIQuestLog.cpp:35/88-90/295`），非 IMG origin/IDA 校正：分类字 `cpp:135-140 tab_y=22+2`、行文字 `cpp:270/282 row_top+4/+3`（19px 行高未居中）、底带按钮 `cpp:863-876` 统一 `BAND_Y=370` 写死。
- **右侧数字根因**：`QuestData.cpp:140-142` 注释"本包不暴露 String/Quest.img"，name fallback `"Quest <id>"`（`:188-189`）、summary 空（`:230-233`）；detail 模式是本端自造字段 `quest.detail.id/npc/pre`（`UIQuestLog.cpp:661/671-683` + `zh-CN.ini:589-595`）把 id/npcid/前置id 用 `std::to_string` 打出来。
- **Angel 证据强但只覆盖协议层**：`docs/features/quest-angel-calibration-2026-11/notes/r01-angel-native-quest-facts.md` 带符号 IDB 153 funcs（CUIQuestInfo 15 方法等）、`r05` N6 证"083 有独立勋章/任务信息窗族，当前端合并"。但 UI 字段/坐标级伪代码未落盘，`w06-quest.md:6` 只还原了 change_tab。补字段级还原需新做 IDA 反编译。

## 6. 全局 ESC / 拖拽（明确 bug，可直接修）
- `IO/UI.cpp:429-462` 的 ESC 白名单**没有** `DAILYCHECKIN`/`DAMAGERANK`/`COLORPRISM` 三个 Type；派发用 `state->get_front(types)` → `send_key`（`:492` 附近）。
- 三个窗口**都没有 send_key override**（只有按钮 deactivate：`UIDailyCheckin.cpp:136`、`UIDamageRank.cpp:862`、`UIColorPrism.cpp:451`）。→ ESC 关不掉 + 需补全局白名单 + 各窗 send_key。
- 拖拽：三窗均 `UIDragElement<Pos...>`，但 `UIDailyCheckin.send_cursor:150-153`、ColorPrism 需核是否把按钮区从 dragarea 摘出（DamageRank 已有 `cpp:285-299` 正确范例）。染色"窗口无法拖拽"需核 dragarea 是否被 thumb/投放槽命中吞掉。

## 7. 小地图"非玩家"列表（四处）
- 面板在 `UIMiniMap`（`UIMiniMap.h:170-183 listNpc_*`，资源 `UIWindow.img/MiniMap/ListNpc`）。
- **绿点不居中**：`UIMiniMap.cpp:1285-1286` 绿点固定 `+2`、文字 `-2`，未按 17px 行高 `(h-元素高)/2` 居中。
- **双击慢**：`cpp:460-466/476-482` 单击即发 2 包（`NpcInteractionPackets.h:60-71` 先 NpcActionTalk 再 TALK_TO_NPC），且 `UIDragElement::send_cursor` 先执行可能吞点击；无一手证据，属运行时待抓包。
- **换图不刷新（时序 bug 坐实）**：`UIMiniMap.cpp:301-306/395` 靠每帧 mapid 差分刷新；但 `SetFieldHandlers.cpp:238-239` fadeout 时就 `Stage::clear()` 清了 npcs 容器而 **mapid 还是旧值** → 不刷新；load_map 设新 mapid 后 npcs 为空 → 刷出空列表，之后 NPC 陆续 spawn 但 mapid 不再变 → **永不刷新**。修：换图成功后显式触发重建 或 加 npc 数量/oid 变化检测。
- **换图掉落动画**：三容器 clear_spawns 已落地（`MapMobs.cpp:121-126`/`MapNpcs.cpp:64-71`/`MapDrops.cpp:124-131`），npc/mob 有 reanchor_spawns（`SetFieldHandlers.cpp:168/171`），**drops 无 reanchor**；旧包在新 foothold 找不到落脚点被重力拉下（Physics.cpp:106-109）。

## 决策点（必须用户拍板，猜错即返工）
D1 染色：切到中文包协议（0x11D/0x184 + 中文 WZ + 拖入已穿戴现金装）还是保留当前协议只换中文 UI 素材？
D2 签到物品：接 mock 表"单主 icon + hover tooltip 列多件"（需求包口径）还是扩协议支持每格多件 icon？
D3 伤害统计：改参考包"自适应高度+无滚动条"还是保留"固定高+滚动条"只修图标/居中？
D4 任务列表：本轮只做明显的 IMG origin 居中校正 + 去掉自造数字字段，还是新做 Angel IDA 字段级还原（重工程）？

---

## 用户已拍板（2026-11-21）与实现进度
D1=完整切中文染色包 / D2=用需求包示例奖励 / D3=伤害统计自适应高度 / D4=新做 Angel IDA 字段级还原。

| 项 | 状态 | 证据 |
|---|---|---|
| 2 伤害统计 | ✅ rank-fix：skill_icon()按skill_id取SkillData图标+缓存、10处文字走 y_for_ink_top、自适应高度无滚动条、+send_key | 子代理交付 |
| 3 签到 | ✅ 服务端 DailyCheckinRewards 28天真实表；客户端 icon_ids/tooltips 落地+每格画icon+hover tooltip；关闭钮(490,6)+命中区从dragarea摘出+send_key+close() | 主代理 |
| 5 任务 | ✅ quest-angel：真 IDA 一手（QuestInfo 16字段/CUIQuestInfo 42字段/CT_INFO 17字段）+ orange-wz 128B key autoParse 校正 IMG origin；行文字走 y_for_ink_top；去裸id改可读名；i18n；r07 文档 | 子代理交付 |
| 6 全局ESC/拖拽 | ✅ UI.cpp:429-462 白名单补 DAILYCHECKIN/DAMAGERANK/COLORPRISM；三窗 send_key | 主代理+子代理 |
| 7 小地图 | ✅ minimap-fix：绿点/文字居中；单击改mouse-up确认+命中区摘出；换图 refresh_npclist()显式钩子+签名兜底；drops reanchor_spawns | 子代理交付 |
| 1 染色 | ⏳ 服务端完整（0x11D/0x184+Handler+4类+SQL，dye-zh 完成）；**客户端半截**（dye-zh failed，仍是旧巴西服协议）→ dye-client 接手中 | 见服务端新类 |
| 4 图4DAILY GIFT | 结论：UIDailyGift 灰框假数据，非高版本非完整移植；angel 校准明确"仅UI假态❌"——不属本次改造，仅告知用户 | 子代理交付 |

**构建**：全量 Rebuild `BUILD_EXITCODE=0` error=0 产物 5192192B（含顺带修复并行会话遗留的 UIWorldMap.h:212 SearchRow 声明前置于定义编译错误——非本次6项）。此构建**不含染色客户端切换**（dye-client 在跑）。

**待办**：dye-client 回来 → 中文染色资源转key导入live → 整体再构建 → 部署 live 正式 exe + SHA 核对。
