# 爆物列表：金币框内 / 滑块入槽（2026-11-22 23:39 反馈）

> 状态：本端截图确认 + 行号级根因 + 全量构建 + 部署 live + 短启无闪退；真机观感待用户截图验收。
> 部署：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` SHA256 `396fc914218092e5388a1fbc90673bece9bf2ca4e9e22a3c555e6419b1630638`（5369344 B，00:00:59）

## 1. 反馈与证据
用户 23:39 截图 `XiaoYe_2026-11-22_23-39-22.png`（红框标注期望区）：①金币数值压在红横框上沿之外；②右侧滑块在红竖框右侧外。
来源：live SHA `855e630d…`（我 23:32 部署）早于截图 23:39 → 本端证据。

## 2. 根因（行号级 + probe 实测）
- **金币**：`UIMapInfoPanel.cpp:105` 旧 `MESO_Y=483`，但 probe 实测 y=483 是白框**上描边**(136,153,187)，真正纯白区是 **y=490..505 / x=19..176**。`meso_ink_top=483+(22-字高)/2≈488` 落在描边带 → 字压红框上沿。
- **滑块**：旧 `LOWER_SCROLL_X=215`，用户红竖框实测原图 x=195..211（面板坐标），215 在框外右侧；且滑块位图 VScr5 thumb 自身 origin 右偏≈8px，可见中心更靠右。

## 3. 修复（一次成型）
- `MESO_Y=490`（真白框上沿）、`MESO_H=16`、`MESO_X=19`、`MESO_FIELD_W=157`；金币墨迹用 `y_for_ink_top(490+(16-字高)/2≈492)`，落在白框内垂直居中；x 右对齐到框右缘（Alignment::RIGHT）。
- `LOWER_SCROLL_X=215→199`（补偿 origin 右偏后可见中心≈203 落红框）、`LOWER_SCROLL_H=170`；`set_content_height` 的 vertical range 同步。

## 4. 构建/部署
- 并行会话持续锁 PDB，`/p:IntDir=<独立>\obj\ + /p:OutDir=<独立>\` 隔离 → `MSBUILD_RC=0`。
- 覆盖 live 后实时核对 SHA == 产物；短启 `_ab_launch.ps1 -Wait 12` → `ALIVE`。
- 真机观感（金币在红横框内、滑块在红竖槽内）待用户截图，须先核对 SHA `396fc914…`。

## 5. 关联
- 上轮：`docs/features/worldmap-search-ui-fix-2026-11-22/README.md`。
- 底部金币框/滑条来源：`docs/features/map-info-panel/`。
- 回退点：`build-goldpos-verify/MapleStory.exe`。
