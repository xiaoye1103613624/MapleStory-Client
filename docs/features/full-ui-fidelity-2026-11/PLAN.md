# 四功能按参考包完整还原 · 证据清单与方案（2026-11-21）

用户当面确认：之前三个窗口是简化版（纯 Text+隐形热区），没按需求文档/参考插件完整实现；掉落特效效果不对。用户拍板：**全部按参考包完整还原** + **掉落特效按参考包帧序重做**。高版本客户端（XyMapleStory-cms085 / HeavenClient）经查 IO/UITypes 与 Data 均无这四类功能实现（grep 0 命中）——它们是低版本私服插件生态功能，无高版本可参照，必须照参考包插件实现。

## 1. 染色窗（coloring-prism/client/coloringprism.cpp，2575行）
### 参考契约（行号级）
- 窗 `kWndW=301,kWndH=406`（:260），背景 backgrnd 已烧标题/页签/横幅/滑条标签板
- 5 页签 `kTabCount=5,kTabX={6,50,102,146,190}`（:274-284）——**不是我的 6 页签**
- 投放槽 `kWellX=21,kWellY=59,kWellSize=48`、图标 32×32（:290-292）
- 预览窗 `kPreviewT=121,kPreviewB=271,kPreviewL=11,kPreviewR=289`（:311-315）：实时染色角色动画（CAvatar，走路/跳跃/朝向）
- 3 滑条 `kRowsT=277,kRowStep=32,kLabelX=9,kTrackX=68,kTrackW=176,kTrackH=8,kValueX=248`（:335-341）；轨 `trackTone/trackChroma/trackBright`（:369-371）；thumb=`Basic.img/Slider/thumb*` 22×14（:362）；数值可键盘输入（:831 m_editRow）
- 底部按钮 `kBtnT=380,kBtnW=47,kBtnH=18,kBtnX={12,301-47-12}`（:346-350）：Cancel（实际是 Reset）+ OK（确认），素材 `Basic.img/BtOK2`/`BtCancel2` 三态（:854-855,:342）
- 关闭钮 `kBtCloseX=281,Y=5,12×12`（:268）；字体 4 种（Dotum12 dark/light/title，:843-846）
### 我当前实现（差距）
`UIColorPrism.cpp:37-44` 全 Text 文字；`:69` 页签/滑条 AreaButton 隐形热区；`:62-63` backgrndLook 错误覆盖 backgrnd；无投放槽/预览窗/thumb/真按钮/字体体系；6 页签错（应 5）。
### 修复落点
重写 UIColorPrism：5 页签 + 投放槽（接拖放或选中）+ 预览层（复用 CharLook 渲染染色角色）+ 3 滑条（track canvas + thumb + 数值）+ OK/Reset 真按钮；坐标按上述常量。

## 2. 伤害统计（个人伤害统计/统计1/Tengutake/src/uiDamageRank.cpp）
### 参考契约
- `kLayerWidth=223`、展开高 223/最小高 20、30 玩家行/15 技能行（:25-33）
- `kHeaderHeight=26,kTitleY=26,kRowsStartY=52`、玩家行高 21+gap2（:35-44）、技能行高 42（:46）
- 背景 4 段 backgrndmax/min/center/bottom（:510-513）+ backgrndbottomover（:515）
- title1/title2（:530-531）、gauge 六种 commonl/c/r + selfl/c/r（:533-539）
- 5 按钮 kBtnReset/Switch/Auto/MinMax/Close，`kBtX` 位置 `:48-52`，四态 normal/mouseOver/pressed/disabled（:544-553）
- 滚动条 kScrollX=205,width=15,thumb≥12（:56-59）
### 我当前实现（差距）
`UIDamageRank.cpp:67-73` 只用 max/center/bottom 三张（min/bottomover/title1/2/gauge 六种全缺）；`:88-91` 页签 AreaButton 隐形；无四态按钮、无 gauge 条形图、无滚动条 thumb。
### 修复落点
重写 UIDamageRank：接全部 9 节点（probe 已确认 live 有 9 个 canvas）+ title1/2 + gauge 六段拼条 + 5 四态按钮 + 滚动条；按 223 宽布局。

## 3. 签到（docs/features/daily-checkin/ui/）
### 资源
- v4 整窗 `normal.png` 513×346，中文"每日签到/第1天…第28天"**已烧进栅格**
- cell v1：normal/mask/claimed 各 67×71、claimed-label 40×18（单格状态图）
### 我当前实现（差距）
`UIDailyCheckin.cpp:29-31` 标题"Daily Check-In"英文；`:78` 又叠 Text "Day N/claimed" 英文——**与烧进图的中文重复且语言错**；整窗背景有但 cell 状态图完全没用。
### 修复落点
去掉重复的 Text 标签（背景已含中文），改用 cell/v1 状态图叠在 28 个格子上（claimable 高亮/claimed 绿勾）；标题走背景自带，不另绘英文。

## 4. 掉落特效（扩展改动/掉落特效）
### 真因（probe 坐实）
live `DropAuraEff.img/dropItemAura/{Rare,Epic,Unique,Legendary}/{front,back}` **frames=0 空节点**——之前抽资源抽到空壳，所以没效果。需求包 12 webp 才是真素材：
- `0-5.webp` 61-73×33-38 RGBA = 6 个 grade 光环主体
- `0-5_光柱.webp` 53-62×46-48 RGBA = 对应光柱
参考包 `dropitemaura.cpp:131-134` 从 `Effect/BasicEff.img/dropItemAura/{grade}/front|back/{0..15}` 取 16 帧，`AURA_FRAME_MS=120`（:14），Putcolor 显隐切换（:33）。
### 修复落点
把 12 webp 转 canvas 写进 live `Effect/DropAuraEff.img/dropItemAura/{grade}/{front,back}/0`（单帧即可，帧循环由代码 120ms 计时驱动）；`DropAuras` 代码按 grade 取 front/back 叠加渲染 + 120ms 循环显隐，替代当前空节点。

## 资源导入完成情况（2026-11-21，全部 probe 验证 + 原子替换进 live）
| 资源 | 落位 | probe |
|---|---|---|
| ColorPrism.img（76KB GMS头转key） | live `Data/UI/ColorPrism.img` | backgrnd 301×406 + 3 track 176×8 + backgrndLook |
| 签到 v4 normal.png（513×346） | live `UIWindow.img/DailyCheckin/backgrnd` | 根节点 113→114 |
| 签到 cell 4 状态图 | live `UIWindow.img/DailyCheckin/cell/{normal,mask,claimed,label}` | 67×71×3 + label 40×18 |
| 伤害统计 9 canvas | live `UIWindow.img/DamageRank/*` | 223 宽分段 + title1/2 |
| 伤害统计 gauge 六种 + 5按钮四态 | live `UIWindow.img/DamageRank/gauge/*`, `Bt*/{normal,pressed,...}/0` | gauge 1×19 可拉伸、按钮 27×12 |
| 掉落特效 12 webp | live `Effect/DropAuraEff.img/dropItemAura/{0..5}/{front,back}/0` | 6 档数字节点，front 61-73×33-38 / back 53-62×46-48；旧 Rare/Epic 保留兼容 |

## 掉落特效的诚实的资源边界
参考包 `dropitemaura.cpp:131-134` 从 BasicEff.img 读 `{grade}/front|back/{0..15}` 共 **16 帧**、120ms 循环，形成流动动画。需求包 `掉落特效/` 每档只有 **1 张 webp**（0-5.webp 光环 + 0-5_光柱.webp 光柱），且 live 原 DropAuraEff 的 front/back 是空节点——所以本版能还原的是"**每档正确的光环+光柱静态分层图 + 服务端 6 档 grade 映射**"（之前 grade 映射 4 字符串全错、节点空，所以完全没效果），**16 帧逐帧流动动画无素材可依，无法还原**。若用户要逐帧动画，需提供 16 帧素材或 BasicEff.img 原包（属于新资源需求，不当场编造）。

## 实现顺序（一次成型，分步验证）
1. ✅ 资源全部导入 live 并 probe（见上表）
2. 代码：签到（已完成：去英文+cell 状态图）；染色/伤害统计由子代理按参考契约重写；掉落特效（已完成：6 档数字映射 + front/back 分层）
3. 全量构建三证 + 部署 live 正式 exe（铁律）+ SHA 核对
4. 真机待用户验收（截图须本端；16 帧动画限制如实声明）
