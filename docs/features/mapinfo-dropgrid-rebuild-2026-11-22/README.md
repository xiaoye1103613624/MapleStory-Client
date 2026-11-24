# 地图信息面板：移除烤死 5×5 格 + 爆率/金币/滑块重排（2026-11-22 21:37 反馈）

> 状态：本端截图确认 + 纠正此前理解错误 + 全量构建 + 部署 live + 短启无闪退；真机观感待用户截图验收。
> 部署 exe：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` SHA256 `7c7368dda9415876230f171a681e39834814d2a09c93ff82ae03a3f90cc33f97`（5367808 B，23:09:00）
> 部署 PNG：`E:\MXD\BeiDou-Client-xy\UI\MapInfo\info_panel_backgrnd.png`（已移除烤格）

## 1. 用户纠正（关键：此前理解错了）
用户 21:37 明确："我说的是自带的五行五列的格子移除，不是物品之前的4行4列后面的格子移除；爆率没在灰色背景中；金币爆率应该在'mesos'前面灰色框内；右侧滑动块偏右下、没在灰色竖条纹内。"
截图来源确认：21:37 晚于当时 live（含我 ratefix 版）→ 本端证据。

## 2. 根因（逐条行号级）
1. **自带 5×5 烤格**：`docs/features/map-info-panel/resources/_gen_map_info_ui.py:367-373` 把 5×5 深蓝灰格线烤进 `info_panel_backgrnd.png`（probe 实测竖线 x=30/61/64/95/98/129/132/163/166/197、横线 y=312/343/346/377/380/411/414/445/448/479）。此前我误把运行时 4×4 叠在烤格上 → 两套格子错位。用户要的是**移除烤格**。
2. **爆率没在灰背景**：运行时黑条 `ColorBox` 有画，但字用基线 `bar_pos.y()+11`，墨迹顶跑到条上方；应让墨迹顶对齐条内。
3. **金币不在 mesos 前灰框**：`MESO_TEXT_Y=MESO_Y+12` 基线导致字跑到框下；灰框实测 y≈483..505。
4. **滑块偏右下**：灰竖槽实测 x≈210..220 / y≈309..483，滑块常量 `LOWER_SCROLL_X=210/Y=313/H=166` 未对齐槽中心。

## 3. 修复（一次成型）
- **生成脚本**：`:366-373` 不再画 5×5 格子 rect/高光，只保留 slot 位置元数据；重跑脚本 → 新 `info_panel_backgrnd.png` 仅剩 well 外框（x=18/210），已同步 repo `UI/MapInfo/` 与 live `UI/MapInfo/`。
- **运行时画唯一一套 4×4**：`UIMapInfoPanel.cpp` 加回 `slot.draw(cell_pos, SLOT, SLOT)`（此前误移除），格子现在由运行时按 4×4 绘制，与图标/爆率同一坐标系。
- **爆率**：墨迹用 `rate.y_for_ink_top(bar_pos.y()+1)` 精确落进深色条内。
- **金币**：`meso_amount.y_for_ink_top(MESO_Y+(MESO_H-字高)/2)` 让数值在 mesos 前白框垂直居中，x 右对齐到框右缘。
- **滑块**：`LOWER_SCROLL_X=215 / Y=310 / H=172`，对齐灰竖槽中心；`set_content_height` 的 vertical range 同步。

## 4. 构建/部署
- 并行会话持续构建锁 PDB，用 `/p:IntDir=<独立>\obj\ + /p:OutDir=<独立>\` 隔离 → `MSBUILD_RC=0`。
- 部署时 exe 被真实客户端 PID 57904 占用，用户明确授权"结束该客户端再覆盖" → taskkill 后覆盖，实时核对 SHA == 产物；PNG 早已同步 live。
- 短启 `_ab_launch.ps1 -Wait 12` → `ALIVE`，无堆损坏。
- 真机观感（格子/爆率/金币/滑块）待用户截图，须先核对 SHA `7c7368dd…`。

## 5. 关联
- 上轮对齐：`docs/features/worldmap-tip-icon-align-2026-11-22/README.md`。
- 左侧信息面板：`docs/features/map-info-panel/`。
- 回退点：`build-dropgrid-verify/MapleStory.exe`。
