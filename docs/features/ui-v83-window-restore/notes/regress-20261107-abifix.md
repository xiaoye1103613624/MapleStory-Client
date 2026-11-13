# 2026-11-07 回归修复：14 项「之前好的功能又坏了」

## 主因：UIElement::Type::NUM_TYPES 扩容后增量编译 → ABI 混尺寸

- `IO/UIElement.h` 12:21 加 `SPECIALUI`，`NUM_TYPES` 变大；
  `EnumMap<Type, UPtr, NUM_TYPES> elements` 是 UIStateGame/UIStateLogin/UIStateCashShop
  **成员数组**，实例尺寸随 NUM_TYPES 变（头文件注释自己写明须整工程 Rebuild）。
- 实测 `x64/obj-Release` 286 个 obj 里 146 个仍是 12:21 前的旧尺寸
  （UI.obj / Charset.obj / Icon.obj = 13:01），23:20 部署的 exe 是新旧混链。
- 症状签名：所有窗口的数字/文字/点击「随机消失」——属性数值(#1/#2)、金币(#4)、
  底部名称职业(#3)、快捷键提示(#7)、NPC 名称(#8)、点击事件(#11)、技能等级标题(#12)、
  聊天类型文字(#14) 全部同源。
- 处置：`MSBuild /t:Rebuild`（OutDir=build-rebuild，不碰部署目录），0 error，
  产物已部署 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（00:30）。

## 独立真缺陷（非 ABI）

1. **item6 双击装备**：`UIItemInventory::doubleclick` EQUIP 分支被 095 魔方劫持
   （`CubeRerollPacket(slot)`），既穿不上也无提示。已恢复原生「双击=穿戴
   （can_wear_equip 内建 UIOk 提示）」；魔方入口收回到 CASH 页签 243* 双击 →
   SoftKey 输入装备槽位（`cube.prompt_slot` i18n 已补 zh/en）。
2. **item9 传送门**：`MapPortals::find_warp_at` 把 REGULAR/SCRIPTED 也放进自动触发，
   与原生相反。改为 `PortalScope::TOUCH_ONLY`（仅 TOUCH/SCRIPTED_TOUCH 碰触即发）+
   按上键 `PortalScope::ALL`（对齐 v95 CPortal::CheckKey / ProcessPortal）。
   光圈位置本身走 Texture origin，视觉锚点正确（bounds ±25×-100..25 为原生矩形）。
3. **item13 M 键循环**：原生 M = MIN→NORMAL→MAX→关→MIN。UIMiniMap::send_key 新增
   `KeyAction::Id::MINIMAP` 分支（持久化 minimap.type + deactivate 表达「关」）；
   UIStateGame 兜底 `emplace<UIMiniMap>(stats)` 重建。
4. **item12 组合栏关不掉**：`UISkillBook::close()` 未收独立窗 UISkillMacroEx，
   已补 deactivate（X/ESC/K 三条关闭路径都过 close）。
5. **服务端存档**：`ItemFactory.java` INSERT 占位符 27→28 已由并行会话修复
   （23:40 源码、23:50 编译重启，logs/out.log 无新报错）。

## 真机回归取证（pass1~pass6，见 evidence/regress-20261107/）

- 第二根因（字体）：R28-B5 把默认字体改成 `fonts/SourceHanSerifSC/...`，
  部署目录缺该字体 → `addfont` 全批失败，所有 FreeType 文字消失；烘焙位图文字
  正常。补齐字体 + 整工程 Rebuild 后，`r6_*.png` / `r2_*.png` 里属性数值、
  金币 9,260,000、LV/新手/xiaoye、HP/MP/EXP、快捷键键位、NPC「罗杰」、
  技能等级/技能点、聊天带文字全部恢复（#1/#2/#3/#4/#7/#8/#12/#14）。
- #13 M 键 MIN→NORMAL→MAX→关→MIN：`r2_01..r2_05` 五帧逐态取证，通过。
- #12 组合栏三行 + 关窗收组合栏：`r6_01` 三行、`r6_02` 双窗同关，通过。
- #14 白底：`r2_06`（输入激活）出现白色输入行，与原生「聊天框展开时候.png」
  一致；`r6_02`（收起）是灰带。白色输入带是原生展开态，不是回归；另补
  `chatTarget` 缺失时的深色兜底框防白字压白底。
- #10 MapleTV：已定位为「场域大屏贴图未接入」并按运行时 WZ 偏移补齐默认
  双屏画面，详见 [regress-20261107-mapletv.md](regress-20261107-mapletv.md)。
- #5/#11 未能自动化闭环：账号无已装备道具可对比（#5）；自动点击坐标始终未
  命中 NPC（#11），不能据此判功能坏。留用户手点。
- 取证脚本：`docs/features/ui-v83-window-restore/tools/regress-20261107*.py`
  （自动登录 + 逐窗截图；跑前须确保无同账号实例）。
