# 多分辨率居中修复（2026-11-21）

## 0. 背景
用户本端截图 `XiaoYe_2026-11-21_10-46-10.png`（1922×83，已确认本端）反馈两点：
1. `StatusBar.img/base/backgrnd` 在大分辨率没跟底部信息栏一起居中；
2. 大分辨率进入/切换时窗口没在屏幕中间，偏右下。

合入上一批（背景 A+C、怪物跨图残留、名牌水平居中、i18n 收口）一并部署。

## 1. 逐条根因与修复（行号为改后）

| # | 问题 | 根因（行号级） | 修复 |
|---|------|--------------|------|
| 1 | backgrnd 不随信息栏居中 | `Util/HudLayout.h:78-83 hud_center_x()` 返回的是**内容组左缘** `(VW-content_w)/2` 而非视口中心；`UIStatusBar.cpp:3014` 旧式 `bg_x = classic_content_cx - bg_w/2`（classic_content_cx 同为左缘）把背景推到左外侧（1920 档算出 -306），右侧露空 | `UIStatusBar.cpp:3014 bg_x = position.x()`，与血量/名牌/QS/数值同源左缘，一起居中；背景宽仍 = content_w |
| 2 | 切档窗口偏右下 | `Window.cpp initwindow()` 只 `SDL_SetWindowSize`，左上角固定 → 尺寸变大后整体向右下扩张 | `Window.cpp initwindow()` 加 `if(!fullscreen) SDL_SetWindowPosition(glwnd, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED)`，每次建窗/切档重新居中（SDL3 `SDL_video.h:308/1788` 已确认 API） |

## 2. 附带修正
- 上一批 i18n 键（message.skill.* / damagerank.* / uinpctalk.tip* 等 40 键）此前只写进了 build-final-assets/locale，未进 live。本轮确认源码 `locale/{zh-CN,en-US}.ini` 含全部键，并部署到 live，避免技能不足提示显示成 locale 键名。
- 坐标系证据：`UIStatusBar.cpp:308/:2366 position.x = hud_cx`（内容组左缘），`UIElement.cpp:226-227` sprites 以 position 为父坐标；`:714 hpmp_sprites[0].draw(position)`、`:3121-3168` 名牌/数值/QS 全部 `position + 相对偏移`。

## 3. 构建与部署
- 脚本 `tools/_adaptive_build.bat`（Release x64，v142，隔离 IntDir `build-adaptive-int/`）。
- 构建 `adaptive_build.log` → `MSBUILD_EXIT=0`，`error` = 0。
- 产物 `build-adaptive/MapleStory.exe`，5,163,520 B，**SHA256 `6E52732E633BDD05E10A8205DBACA78917A1590D4604AF6B82C979A7A8857EC0`**。
- 按部署铁律覆盖 live 正式 exe `E:\MXD\BeiDou-Client-xy\MapleStory.exe`，**live 实测 SHA = 产物 SHA（实时核对，含 8s 后防回滚复核一致）**。
- 覆盖 live `locale/zh-CN.ini`（SHA `4f19e76d...9015c`）/ `en-US.ini`（SHA `74e3d6c8...48b`），与源码一致。
- 备份：`MapleStory.exe.bak-before-centerfix-20261121`（原 fidelity 构建 SHA `05af98bc...582`）。
- 全程未用带日期/用途后缀的变体名做交付，未触碰其它 variant exe。

## 4. 真机验收清单（待用户，截图须本端）
- [ ] 1920×1080 / 2560×1440 档：`base/backgrnd` 与血条/QS/菜单整组水平居中，右缘不再露空；
- [ ] 大分辨率进入、游戏内系统设置切档：窗口居中，不再偏右下；
- [ ] 野外门→城镇：怪物不残留到新地图；
- [ ] 怪物等级+名称名牌整组相对怪物水平居中；
- [ ] 技能 HP/MP 不足提示显示中文（非 `message.skill.hp` 键名）；
- [ ] 背景四边无黑边/接缝。

## 5. 如实声明
- 已构建、已部署 live 正式 exe + locale 并实时核对 SHA（含防回滚复核）。
- 未做真机截图验收；截图须本端，用户未提供本轮新截图前不宣称视觉通过。
- 期间检测到并发会话改过 `UIDamageRank.cpp`/`UIColorPrism.cpp` 等同批文件，本机构建已包含当前工作树全部改动；部署产物 SHA 与本机构建一致。
