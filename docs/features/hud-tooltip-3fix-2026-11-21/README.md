# 多分辨率状态栏 + 套装面板 + 装备词条间距 三问题修复（2026-11-21）

> 个人学习/技术研究，单机离线。逐条行号级结论；真机未验收如实声明。

## 0. 用户裁决

- 问题1 目标宽度 = **含快捷格的整组内容宽**（`HudLayout::hud_content_w`）。
- 问题2 Lv 前缀 = **纯装备等级**（`Equip::get_level()`）。

## 1. 逐条根因与修复

| # | 问题 | 根因（证据） | 修复位置 |
| --- | --- | --- | --- |
| 1 | 多分辨率 StatusBar.img/base/backgrnd 不要全宽拉伸 | `UIStatusBar.cpp:3013-3015` 旧 `Point<int16_t>(VWIDTH,0)` 把 817x71 素材拉满视口（1920 全宽） | 新增成员 `classic_content_w/cx`（`UIStatusBar.h`），init_classic/reanchor 两处赋值 `HudLayout::hud_content_w/center_x`；draw_classic 改用 `bg_x=cx-w/2`、`bg_w=content_w`（不再通栏） |
| 2a | 套装图标偏上 | `SetTooltip.cpp` 图标中心 `cur.y+LINE_H/2` 相对 A11M 文字墨偏高约半字高 | draw 加 `ICON_DY=6`（半字高）下移；X 标记同口径下移 |
| 2b | 套装名称前加 Lv:等级 | SetTooltip 无等级数据 | `set_item()` 加 `hover_level` 参数（`SetTooltip.h:24`），EquipTooltip 调用处传 `equip.get_level()`（`EquipTooltip.cpp:946`）；悬停件等级>0 时前缀 `Locale::format("setitem.level",...)`；locale 键 zh/en `setitem.level=Lv:{}` |
| 2c | 套装各阶段标题加粗+纯色 | 标题 A11M 细体 + GAUGEGREEN(153,204,51) 发灰 | 字体改 `A11B`；`tier_header_color()` 改 `LIGHTGREEN`(0,1,0) 纯绿 |
| 2d | 套装属性后退一个中文字符 | body 前缀 2 空格（约 1 字符） | body 前缀改 4 空格（约 2 字符，即再退 1 汉字） |
| 3 | 装备名称/不可交易/所有者上下间距不一致、贴一起 | 实测 draw-y：name 墨底 y22、trade y24（仅 2px）、owner y42、req y60；`SUBLINE_FIRST=墨底+2` 使名称到首词条只 2px，而其余为 18px | `SUBLINE_FIRST` 改 `NAME_Y+NAME_INK_H+(SUBLINE_STEP-11)=29`（名称墨底+7），名称→首词条视觉空隙与词条间一致 |

## 2. 构建

- 脚本 `tools/_hud_tooltip_3fix_build.bat`（Release x64，v142，隔离 IntDir）。
- `hudtip3fix_build.log` → `MSBUILD_EXIT=0`，`error C`=0。
- 产物 `build-hudtip3fix-out/MapleStory.exe`，5,125,120 B，SHA256 `5D1E88658FE48C2DC047567DB8339CAC2FC380DF37BE47E4A2EEE8A5AA001707`。

## 3. 部署（按用户铁律：只落 live 正式 exe）

- 部署前 live exe SHA = `a43b0a16…`（并行会话刚更新过）。
- 部署时正式 exe 被运行中的客户端 PID 29056 锁定 → 用户明示「直接结束进程再覆盖」→ 结束 PID 29056 后覆盖。
- 备份：`MapleStory.exe.bak_before_hudtip3fix_20261121`、`locale/*.bak_before_hudtip3fix_20261121`。
- 覆盖正式名 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` + `locale/zh-CN.ini`/`en-US.ini`。
- **部署后 live 实测 SHA = `5D1E8865…A001707`**（与 build 一致，实时核对）。
- 无旁挂变体名。

## 4. 真机验收清单（待用户，截图须本端）

1. 1920 宽屏：底栏 backgrnd 收窄到含快捷格的整组内容宽，不再通栏拉满。
2. 套装成员图标下移半字高、与名称水平对齐；悬停件名前有 `Lv:N`。
3. 套装档位标题加粗纯绿；属性行整体后退一个中文字。
4. 装备名称/不可交易/所有者三段间距一致。

## 5. 如实声明

- 已部署 live 正式 exe 并核对 SHA，但未做真机截图验收（需重启客户端后用户确认）。
- 因结束进程导致当前游戏会话被关闭（用户授权）。
