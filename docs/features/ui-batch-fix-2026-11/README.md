# UI 批量修复 · 2026-11-11（用户 11 条反馈）

参照基准：v083（正确客户端截图 `C:\Users\11036\Desktop\正确客户端截图\`）。
运行时资源：`E:\MXD\BeiDou-Client-xy\Data\UI\*.img`，经 orange-wz MCP（128B `tools/_gms_key_128.b64` + `ivBase64=TSPHKw==`）核验。
证据：录屏抽帧 `evidence/video/*.png`、总览 `evidence/video/*_sheet.png`、tip 参考帧 `evidence/ref_item_tooltip_zoom.png`、WZ 节点 `evidence/wz_IconBase_0.png`。

## 阶段 0 取证结论（逐条根因 → 修复）

| # | 现象 | 根因（文件:行号） | 修复 |
|---|------|------------------|------|
| 1 | 登录账号/密码白字模糊、不支持中文 | `IO/UITypes/UILogin.cpp:258-265/:304-311` bedou 分支 `Color::Name::WHITE` + `A11M` 压浅木槽；`IO/Components/Textfield.h:84` `accept_unicode=false`，`IO/UI.cpp:522-525` 因此 `SDL_StopTextInput` | 改 `Color::Name::JAMBALAYA`；`account/password.set_accept_unicode(true)`；删 `:615-629` A/P 软键盘捷径 |
| 2 | 频道选择双卷轴/偏移、羊皮纸抖动 | `IO/UITypes/UIWorldSelect.cpp:305-309` 非 bg_managed 再 emplace `WorldSelect/scroll/0/0`；`UIRegion.cpp:40` 又 emplace Gateway `backgrnd2`；`:425-429 channel_slide` + `:462-469` 每帧 layout + `draw(alpha)` 帧间插值 | 待角色/频道 subagent 最终结论 |
| 3 | 角色选择单击/双击失效、卷轴与属性板错位、删除弹窗错文案 | `IO/UITypes/UICharSelect.cpp:349-353` AreaButton 命中框在脚点下方 183×151，角色身体在上方 220px（仅 `:678-681` 双击有 body_rect 兜底）；`:1022-1023 request_pending` + `:1035-1073 require_pic` 拦双击；`:505 scroll_frame` 未补 origin 而 `:512-513 charinfo` 补了；`:1294 detail_key` 页偏移重复加；`:1140-1145` 直接 `DeleteCharPacket` 无确认窗 | 待角色/频道 subagent 最终结论 |
| 4 | 转职后仍显示「基本属性点」提示框 | `IO/UITypes/UIStatsInfo.cpp:447-448` `if (classic_help.is_valid())` 无条件绘制 | 加 `jobId == Job::Level::BEGINNER` 门控（与 `:614-629 show_ap` 同源） |
| 5 | 物品 tip：无透明度 / 图标底板在下方 / 要求 000 / 名称值不齐 / 职业间距 / 冒号居中 / 数值白色 / 右上「攻击力增加」 | `IO/Components/TextTooltip.h:42-58` 1.0α 实心；`EquipTooltip.cpp:696-701` + `ItemTooltip.cpp:113-116` 用 WZ `IconBase/0`（实测白底黑框、origin=0）而 `itemicon` 自带非 0 origin，2x 绘制错位；`:204-209/:453-458` 补零；`reqset` 5x7 与标签 47x11 同 y；`:750-758` 变宽 `+2`；`:341-400/:593-642` 整行 `CENTER` + `TOOLTIPBODY`；`:694/:842` 绘制 `equip.label_atkinc`（zh-CN=攻击力增加） | 半透明 fill α0.88/edge α0.92；程序 `ColorBox` 灰底垫在 itemicon 下并补偿 origin*2；去补零；值 y+2；`JOB_STEP=41` 固定步长；正文拆 lhs(RIGHT@132)/rhs(LEFT@132) 且 WHITE；删 atkinc 两行 |
| 6 | 技能栏标题偏右、列表偏上、「1/2/3/4/5」多余、不能拖拽、缺个性技能名输入/勾选/确认 | 正确帧 `技能框+技能组合框.png` 对照；`UIWindow.img/SkillMacro` 含 `macroname` 164x57 | 待技能栏 subagent 最终结论 |
| 7 | 好友页乱 | `IO/UITypes/UIUserList.cpp:1310-1313` 4 钮 x 重叠 9px 且 y 偏 3；`:1479-1496` 多画列头层；`UIUserList.h:161 friend_total=50` 与 0/25 不符 | E1/E3 + friend_total=25 |
| 8 | 组员页乱 | `UIUserList.cpp:1336-1347` party_btns 9 格；旧 `UIParty.cpp` 浮窗叠加 | 收敛旧浮窗绘制 |
| 9 | 家族页乱 | `UIUserList.cpp:49-56 GUILD_OFFLINE_Y=217` 与列头同 y；`:471-474` 多画 `guildinfo3/4`；`:1382-1396` grid 比 Angel 原生偏上 10px | E5/E6 |
| 10 | 家族联盟页乱 | `UIUserList.cpp:1529-1543 draw_union_tab` 空态无条件画家族页 4 件烘焙件；`:1071-1090` 7 钮弹 `UIAlliance` 360x320 浮窗 | E7/E8 + `UIMiscShells.cpp:5885-5889` A 键收敛 |
| 11 | 点很多按钮弹软键盘；金币图标应弹数量输入框 | 历史「SoftKey 波次」显式接线：`IO/UITypes/` 465 处 `emplace<UISoftKey>`，三参数重载默认 `Mode::Pic`；`UIItemInventory.cpp:576-589` BT_COIN 直接 SoftKey；`:1312-1323` 'Y' 调试残留；`UINotice.cpp:307-316/:426-440` S/N 与 S/T/Y/O 残枝 | BT_COIN → `UIEnterNumber`（083 `CUIItem::OnDropMoney @0x81d996 → CUtilDlgEx → MESO_DROP(94)`）；删 'Y'、S/N、S/T/Y/O、登录 A/P |

## 版本差告警（社交四窗）

orange-wz 实测部署 `UIWindow.img/UserList` 顶层仅 9 节点（无 `Main` 子树），`Main/CbCondition/Sheet/TapShow/BtSheetIClose/BtPlusFriend` 全部 `matches:[]`。用户错误截图中的「高级好友 Admin / 相同频道·不同频道」属旧 v095/Journey 残留绑定，HEAD 不会画出。修复以 HEAD 静态可证布局错误为准，运行时需确认 exe/IMG 版本。

## 决策（当前用户反馈覆盖历史结论）

- `docs/features/ui-v83-window-restore/evidence/native-verdicts-2026-11-03/native-tooltip-alpha-raw.txt` 历史结论「背板实心 1.0α」与当前用户「tip 框应该有透明度」冲突 → 按当前反馈改半透明，历史结论仅留证。
- 登录字色按正确截图 `登录界面.png` 的深棕字（JAMBALAYA），替换历史可见性权宜 WHITE。
- `UISoftKey` 默认 `Mode::Pic` 不动（保留 PIC 创建/验证链路），本轮只收口用户点名的登录链、金币链、UINotice 数字/OK 链；460+ 全量语义回滚留后续批次。

## 实现进度（2026-11-12 续作 session-9358bdad）

| # | 状态 | 落地摘要 |
|---|------|----------|
| 1 | ✅ | `UILogin`：字色 JAMBALAYA、`set_accept_unicode(true)`、床位 y 补偿 -1、删 A/P SoftKey |
| 2 | ✅ | 去掉 `UIWorldSelect` 静态双卷轴 + `UIRegion` Gateway backgrnd2；`channel_slide` lround + dy 去抖 |
| 3 | ✅ | 选角 AreaButton 覆盖身体；scroll origin 对齐；`detail_key` 去双重页偏移；无 PIC 删除先确认 |
| 4 | ✅ | `UIStatsInfo`：`classic_help` 仅 `Job::BEGINNER` 绘制 |
| 5 | ✅ | `TextTooltip` α0.88/0.92；`EquipTooltip`/`ItemTooltip` 程序灰底+origin×2、去补零、REQ_VALUE_DY=2、JOB_STEP=41、BodyLine 冒号中轴、删 atkinc |
| 6 | ✅ | 停画技能窗内嵌 `#1..#5` 假组合栏；`UISkillMacroEx` 加名输入/勾选/确定 + 拖技能 drop |
| 7 | ✅ | 好友钮 oracle (1/68/228,77)、`friend_total=25`、去掉第二列头叠绘 |
| 8 | 🟡 | 底栏 grid y+10（与其它页共用）；旧 `UIParty` 浮窗叠层未改（预留） |
| 9 | ✅ | `GUILD_OFFLINE_Y=235`；停画 guildinfo3/4 |
| 10 | ✅ | 联盟空态只留提示文案；7 钮不再弹 `UIAlliance`；A 键 → `UIUserList::UNION` |
| 11 | ✅ | `BT_COIN` → `UIEnterNumber`+`inventory.drop_meso`；删背包 Y SoftKey；`UIYesNo`/`UIEnterNumber`/`UIOk` 字母 SoftKey 残枝移除 |

## Plan / 验证

1. ~~落地 #1/#4/#5/#7-#11 的静态可证修复（本轮）。~~ **已完成代码**（#8 浮窗叠层预留）。
2. ~~回收/补 #2/#3/#6（角色·频道·技能栏）。~~ **已完成代码**。
3. ~~MSBuild Release\|x64 → 部署 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2026-11-12 01:11:21）。~~
4. ~~自测收尾~~：详见 [`notes/selftest-2026-11-12.md`](notes/selftest-2026-11-12.md)。
5. **2026-11-12 续作（Angel 全量登录链）**：权威切 Angel；详见
   [`../login-flow-angel-calibration-2026-11/notes/authority-switch-2026-11-12.md`](../login-flow-angel-calibration-2026-11/notes/authority-switch-2026-11-12.md)
   与 [`login-chain-matrix-2026-11-12.md`](../login-flow-angel-calibration-2026-11/notes/login-chain-matrix-2026-11-12.md)。
   - 登录字色改回 **WHITE**。
   - 频道卷轴/羊皮纸改 Angel `CUIChannelSelect` `(-176,-688)` + `(-38,-141)`。
   - 选角闪退真因：`Job::get_name` 按值返回导致 `get_jobname` 悬空 → 进图 `UIStatusBar::draw_classic` AV；已修。冒烟：`1-Admin` / `暗之黑骑士` 在线存活（09:55）。
6. 产物落本目录；**未** git commit。
