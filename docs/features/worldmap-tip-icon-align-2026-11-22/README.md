# 世界地图 tooltip：怪物/NPC 图标与名称水平中心对齐（2026-11-22 21:36 反馈）

> 状态：本端截图确认 + 行号级根因 + 全量构建 + 部署 live + 短启无闪退；真机观感待用户截图验收。
> 部署：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` SHA256 `2ee4d69f85c7b6b01ff55fac681ae9cb9949ccfc5c32238261c7f136e2a737df`（5367808 B，22:26:05）

## 1. 反馈与证据
用户 21:36 截图 `XiaoYe_2026-11-22_21-36-13.png`：怪物/NPC 图标与名称不在同一水平中心轴（图标压在文字上方/偏左）。
来源确认：截图 21:36 晚于当时 live（20:49 `f9ec71fe…`）→ 本端证据。

## 2. 根因（MapTooltip.cpp / .h）
- `MapTooltip.h:50-51` 旧 `LIFE_ICON_ADJ=(1,9)`、`LIFE_LABEL_ADJ=(16,3)`：24px 图标从 y+9 画到 y+33（中心≈y+21），A12 文字基线在 y+3（墨迹中心≈y+0）→ 相差约 21px，图标压在文字上。
- `draw_life_icon` 用 `DrawArgument(pos,scale,scale)`（center=pos），可见框左上 = pos - origin*scale；站姿帧 origin 非0 时图标额外偏移。
- 行高累计 `set_worldmap_mapid` 用 `label.height()+1`，与新的图标行带不一致。

## 3. 修复（一次成型）
- 统一"行带"：`LIFE_ROW_H=24`（=图标目标高），`LIFE_ICON_X=2`、`LIFE_TEXT_X=28`。
- `draw_life_icon(icon, slot_tl)`：缩放 `max_dim≤24`，并补偿 origin（`pos = slot_tl + origin*scale`），让可见框左上角精确落在槽位。
- 每行（mob/npc）：图标可见框垂直居中于行带 `(LIFE_ROW_H - vis_h)/2`；文字用 `y_for_ink_top(row_y + (LIFE_ROW_H - 字高)/2)` 让墨迹也在同一行带垂直居中 → 图标与名称共享水平中心线。行前进 `LIFE_ROW_H+1`。
- `set_worldmap_mapid` 的 mob/npc 行高累计同步改为 `LIFE_ROW_H+1`，底板高度与绘制一致。
- 数量头/玩家分区仍用 `LIFE_LABEL_ADJ`（不受影响）。

## 4. 构建/部署
- 并行会话持续构建锁 PDB，用 `/p:IntDir=<独立>\obj\ + /p:OutDir=<独立>\` 隔离后 `MSBUILD_RC=0`。
- 部署时仅僵尸进程占用，覆盖后实时核对 SHA == 产物。
- 短启 `_ab_launch.ps1 -Wait 12` → `ALIVE`，无堆损坏。
- 真机观感（图标是否与名称同一水平中心线）待用户截图。

## 5. 关联
- 爆率修复：`docs/features/worldmap-ratefix-2026-11-22/README.md`。
- 视觉细化：`docs/features/worldmap-visual-fix-2026-11-22/README.md`。
- 回退点：`build-aligntip-verify/MapleStory.exe`。
