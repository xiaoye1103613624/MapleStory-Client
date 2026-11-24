# 192 ShortCutUI 横向弹层 + 签到/染色对齐修正 · 交付（2026-11-22）

## 两项交付（合并为一个产物）

### A. 192 ShortCutUI 横向弹层（本轮主任务）
用户拍板：按 192 原生还原侧栏折叠；IDA 一手纠正"向右弹出"实为屏幕居中 6 列面板；触发=点击 toggle + 悬浮展开（用户增强）。
- IDA 一手：`CUIContentsShortcut`(size3200) / `CUIContentsShortcutOpen`(size444) / `ContentsInfo`(32B)，布局 ctor@0x117bdb0（屏幕居中 l=(screenW-w)/2,t=(screenH-h)/2，nLine=ceil(count/6)）、OnCreate@0x117aed0（图标 x=((56-iconW)>>1)+56*(i%6)+10, y=92*(i/6)+33）、OnButtonClicked@0x117cfd0（btMain ID2000 toggle）、LoadContentsInfo@0x117b3a0（Etc/ContentsShortcut.img）、SelectIcon@0x117a460。源库：MapleStoryT.i64(13339)。
- 实现：`UIContentsShortcut.h/.cpp`（新，Type=CONTENTSSHORTCUT）：353×(33+92*nLine+33) 三段背景屏幕居中、6列图标（IDA 公式）、btClose 四态、hover tooltip；数据源本端静态表（192 XML 5条：BossArena/CrossHunter/ItemPot/Bits/maplechat）。
- 入口：`UISideIconBar` 加 btMain 钮（UIWindow4.img/ShortCutUI/btMain 四态+ani），点击 toggle + 悬浮展开（`hover_main_opened` 去重，mouse leave 不关）。
- dispatch：idx34→UIMessenger（本端有 Type）；CrossHunter73/ItemPot51/Bits125→UIOk「暂未开放」；BossArena 任务型→UIOk「任务快捷入口暂未开放」（无 CUtilDlgEx，未臆造）。
- i18n：zh-CN/en-US 各 7 条 windowcashshop→contentsshortcut 词条。

### B. 签到/染色对齐修正（上一轮产物 2432B410 的合并）
- 签到：CELL_H=71 + CELL_HEADER_H=19，icon 画粉头下内容区（不再压"第N天"）；删重复 hint（素材已烧引导字）。
- 染色：删 label_title/drop_hint/row_labels/success_hint 四处重复 Text（素材已烧中文）；row_top 改查表 {263,299,331}(实测板顶) + ROW_H=30，轨/thumb 对齐标签板。

### 顺带修复（并行会话遗留，非本轮功能）
1. `Util/Misc.cpp:218-219/238-239` if 续行 `&&` 另起行语法错（合并条件）
2. `UIEnhance.cpp` 未加入 vcxproj/filters（补入）
3. `UIEnhance.cpp:90` `Locale::has` 非静态当静态调用（改 `Locale::get().has`）

## 构建与部署（三证 + 铁律）
- 全量 Rebuild：`BUILD_EXITCODE=0`、链接行、error=0、产物 5337088B
- 按铁律部署 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`，实时核对 SHA = 产物 = **`688CA526467F65CF393B01899735CB0AFD6609F5876D39E4E7DBC8D56D88FDCC`**（用户已关客户端，占用解除）
- 回滚点：`MapleStory.exe.bak-before-shortcut-20261122`

## 真机验收清单（待用户，截图须本端）
1. 竖栏顶部/底部出现 btMain 入口图标（四态+ani）
2. 点击 btMain → 弹出**屏幕居中** 353×158 面板（top/middle/bottom 三段），5 个图标 6 列排布（实际1行），hover 出中文 tooltip
3. 悬浮 btMain 也能展开（去重：不会每帧重复弹）；mouse leave 不自动关闭
4. 点图标：Chat→UIMessenger 打开；其它→UIOk「暂未开放」提示；BossArena 任务型→对应提示
5. btClose / ESC / 再点 btMain 均可关闭
6. 签到：物品 icon 在"第N天"粉头下方内容区、不再压字；底部无裁切 hint
7. 染色：标题/投放提示/滑条标签用素材烧字、无重叠；三条轨/thumb 对齐粉色标签板；数值 +0 显示在右侧

## 诚实边界
1. **真机未验收**：构建过≠显示对，7 项待真机逐项复核。
2. 弹层面板是**屏幕居中**（IDA 原生），不是贴侧栏向右滑出——用户已确认按 IDA。
3. 悬浮展开是**用户增强**（原生仅点击），mouse leave 不自动关闭（按拍板）。
4. dispatch 仅 Messenger 真接上，其余入口降级 UIOk 提示（本端无对应 UIElement::Type / CUtilDlgEx 任务选择，未臆造）。
5. 图标只取 contentsIcon/*/normal/0 单帧（IDA 证实原生如此），不走参考包序列帧。
6. live `Etc/ContentsShortcut.img` 缺失，本端用 C++ 静态表替代（避开跨 key WZ 风险）。

## 证据
- 方案：`docs/features/sideicon-collapse-192-panel/PLAN.md`
- 研判：`docs/features/sideicon-collapse-assessment-2026-11/README.md`
- IDA 一手：子代理 13339 MapleStoryT.i64 反编译（本会话记录）
