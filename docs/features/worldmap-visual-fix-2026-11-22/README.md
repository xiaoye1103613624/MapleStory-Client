# 世界地图视觉细化（tooltip 图标尺寸 / 爆物格 / 搜索框与结果）

> 状态：全链路取证 + 一次性实现 + 全量构建 + 部署 live + 短启无闪退；**真机交互待用户截图验收**。
> 日期：2026-11-22 · 分支 `feature/dev_0.0.1`
> 部署：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` SHA256 `1d0f01557531edd8769be86e1a08e4030806af04b6fd007246efa64858fa6c15`（5357568 B，18:03:17）

## 1. 用户反馈（截图来源已确认本端：含 6fix 的逐行图标与「(怪物)」标注）

1. 地图点位 tooltip 怪物/NPC 图标太大（比名称大太多），要求约名称 2 倍。
2. 爆物列表：物品自带边框与底层格子不对应（双边框），要求移除底格；爆率数字（如 100）溢出格子；金币不在 mesos 框垂直居中。
3. 搜索：输入内容没在输入框内（跑到标题栏）；结果列表应从截图红框位置（灰框内）开始。

## 2. 逐条根因与修复（行号级）

### ① tooltip 图标过大
- 根因：`MapTooltip.cpp:259/286` 直接按 stand 帧原始尺寸画（怪物站姿可达 80px），而名称是 A12（约 12px）。
- 修复：新增 `draw_life_icon()`（`:44+`），按 `LIFE_ICON_TARGET_H=24`（名称 2 倍）等比缩放，超过才缩、小图原样画；mob/npc 两处改用该 helper。

### ② 爆物格 / 爆率 / 金币
- 双边框根因：`UIMapInfoPanel.cpp:438-439` 在物品图标下先画运行时 `slot.png`（32→38 缩放）底格，而物品图标本身带原生边框 → 两层边框尺寸不符。
- 修复：移除 `slot.draw()` 底格，只画图标（按 32px 原生尺寸，不再强放大到 38）。
- 爆率溢出根因：原用 `Charset(ItemNo)` 画 `"10.0%"`，但 ItemNo 字库只有 0-9、无 `.` 和 `%`，宽度爆格。
- 修复：改用 `Text A11B 黄字`（自带小数点与百分号、宽度自适应），底部 12px 深色半透明条居中；数量字移到右上角避开爆率条。
- 金币不居中根因：`MESO_TEXT_Y=MESO_Y+2` 是 Text 基线，A11B 字形顶≈基线-9 → 字形压在框顶。
- 修复：`MESO_TEXT_Y=MESO_Y+12`，字形落框中心。

### ③ 搜索输入位置 / 结果起点
- 输入内容跑到标题栏根因：`Textfield` 是 FreeType 基线绘制，bounds y=25 是基线，字形顶上移约 10px 压进背板深色标题栏（背板 y=0..18 深色、y=20 起白槽）。
- 修复：`search_text.set_text_shift(Point(0,10))`，字形下移回白槽。
- 结果列表偏上根因：`RESULT_AREA_TOP=44` 让结果行画在输入行附近，用户红框标注期望在灰框 y≈86。
- 修复：`RESULT_AREA_TOP=86`（截图红框实测位置），`native_row_max()` 由 y=86..520 算行数，结果行整体落入灰框内、不与输入/标题重叠。

## 3. 构建与部署（并行会话冲突记录）

本轮并行会话持续构建同一工程并锁住 `x64\obj-Release\vc142.pdb`，多次全量构建撞 `C1041/C1090 PDB API 调用失败`。最终解法：MSBuild 命令行覆盖 `/p:IntDir=<独立目录>\obj\`（命令行属性优先级高于 vcxproj 硬编码的 `$(PlatformTarget)\obj-$(Configuration)\`），配合 `/p:OutDir=<独立目录>`，PDB/obj 全隔离 → `MSBUILD_RC=0`。此为本仓库应对并行构建冲突的可复用手法。

- 产物：`build-wm8fix-verify/MapleStory.exe` SHA `1d0f0155…c15`。
- 部署：live 正式 exe 覆盖，实时核对 SHA == 产物；locale 就位。
- 短启：`_ab_launch.ps1 -Wait 12` → `ALIVE`，无堆损坏。
- **未做**：真机交互（图标尺寸、爆物格/爆率观感、金币居中、搜索输入位置与结果位置）待用户截图验收。

## 4. 回滚与风险

- 回退点：`build-wm8fix-verify/MapleStory.exe`；历史 bak 会丢失多轮修复，不推荐。
- 风险：图标缩放到 24px 后极小站姿（如 1px 宽帧）仍按原尺寸画（不放大），属预期；爆率 Text 用黄字在深色条上，对比度高。
- 并行会话：本轮全程遭遇其 PDB 锁竞争，未触碰其文件，最终以 IntDir 隔离构建收口。

## 5. 关联

- 6fix：`docs/features/worldmap-6fix-2026-11-22/README.md`。
- 四合一：`docs/features/worldmap-4fix-2026-11/README.md`。
- 左侧信息面板：`docs/features/map-info-panel/`。
