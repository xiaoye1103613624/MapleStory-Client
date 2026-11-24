# 美容院 / 伤害皮肤 / 融合外观 — 实现与侧栏挂载状态核查（2026-11-23 17:45）

## 来源端确认（重要）
- 用户截图 `XiaoYe_2026-11-23_17-44-58.png`（sha256 4fa5774f…），视觉解析见本会话（115x387 左侧栏：蓝色书+DAILY、黄闪电+200%、蓝星、下方三个红方块）。
- live exe SHA 实时核对：`2D980CB20E02D8C29D16C0AD785D48CD90EF55CCE43BB8EC2DE7CBBE69B234D2`，mtime 2026-11-23 17:15:31 —— **不是**我 15:32 部署的 `F8D3ED…`（那版已被替换）。截图 17:47 属于这版之后，故以当前 live 配置/代码为准，不拿旧部署声明当证据。
- 视觉工具本轮限流后无法再看三张 PNG，红灯身份改由配置行号+资源文件名判定（不靠看图）。

## 一句话结论
三个扩展**目录里的东西存在，但本端客户端功能未实现、未挂载侧栏**。图里三个红块**不是**这三个扩展，而是旧 `side_icons.ini` 后三个槽：每日签到 / 伤害统计 / 装备染色（这三个功能本端已实现）。

## ① 三个扩展目录状态
- `E:\MXD\扩展改动\美容院\`：73 文件。含 `BeautySalonShare/`、`BeautySalonv83/`（Item/Sound/String/UI.wz + README + FILE_LIST）、`BeautySalon-BlackUI.7z`、`UIBlackV2.wz`。README 原文：*source code only, no .wz; v83 kaentake-style DLL injector + Cosmic server; @beauty* —— 第三方 DLL 注入方案，非本工程源码。
- `E:\MXD\扩展改动\伤害皮肤\`：322 文件。含 `damageskin.cpp/.h`、`Client-v83/`（CMake 工程）、`DAMAGE_SKIN_SERVER.md`（Cosmic 服务端建表教程）、`WZ needed/`（0590/0591.img.xml 等）。第三方源码 + 教程。
- `E:\MXD\扩展改动\融合外观\`：320 文件。含 `Client-v83/`、`FUSION_ANVIL_SERVER-SIDE.md`（Cosmic）、`tooltip.cpp`、`UIWindow.img`、`Custom.wz`、`Anvil WZ/`。第三方源码 + 教程。
- 本端 `MapleStory-Client` 全局搜 `BeautySalon/DamageSkin/FusionAnvil/美容院/伤害皮肤/融合外观`：**0 命中**。本端只有 `UIColorPrism`（七彩棱镜染色），与"美容院/伤害皮肤/融合外观"是不同东西。

## ② 是否挂载左侧竖栏
- 本端侧栏权威配置 `Util/FeatureRegistry.cpp:69-97` 的 builtin 表：只有 questlog/event/dailygift/damagerank/dailycheckin/colorprism/witchforest/breakthroughstone/enhance/alliance/... 等；**无 beauty/damageskin/fusion**。
- `Util/SideMenuConfig.cpp:398-475` `builtin_seed()` 及 live `side_menu.json` 里也无这些条目。
- live `E:\MXD\BeiDou-Client-xy\side_icons.ini:5-34` 是 6 槽老配置（book/bulb/star/checkin/damagerank/colorprism），同样无这三项。
- 结论：**未挂载**。

## ③ 图里三个红灯是什么（行号级）
live `E:\MXD\BeiDou-Client-xy\side_icons.ini` 槽位顺序：
- `slot.0 :5-9` book → QuestLog（蓝色书+DAILY）
- `slot.1 :10-14` bulb → Event（黄灯泡+200%）
- `slot.2 :15-19` star → DailyGift（蓝星）
- **`slot.3 :20-24` checkin → DailyCheckin（每日签到）** 红块①
- **`slot.4 :25-29` damagerank → DamageRank（伤害统计）** 红块②
- **`slot.5 :30-34` colorprism → ColorPrism（装备染色）** 红块③
图标文件 `UI/SideIcons/{checkin,damagerank,colorprism}{,_over}.png` 存在（红底占位/导出图）；视觉后端限流未能逐张复核颜色，故以槽位顺序与文件名判定为上图后三个红块，且与用户"三个红灯"一一对应。
三个功能本身均已实现：`Util/FeatureRegistry.cpp:75-77` 分别 emplace `UIDamageRank`/`UIDailyCheckin`/`UIColorPrism`；UI 类文件 `IO/UITypes/UIDailyCheckin.{h,cpp}`、`UIDamageRank.{h,cpp}`、`UIColorPrism.{h,cpp}` 均存在。

## 待办（若用户要真正接入这三个扩展）
这是新功能开发（DLL 注入方案 vs 本端原生 C++ 移植两条路），需先证据解析（REA/IDA/orange-wz）→ 抉择 → Plan，不在本轮"查询状态"范围内。未获授权不动代码。
