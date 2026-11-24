# 世界地图爆物列表：爆率可读性 + 图标格内居中（2026-11-22 18:04 反馈）

> 状态：本端截图确认 + 行号级根因 + 全量构建 + 部署 live + 短启无闪退；真机观感待用户截图验收。
> 部署：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` SHA256 `0d6613321292bb372cbdf80caffcd6462acd67c1f8f4556beb25f50d2d4b7efe`（5357568 B，18:27:44）

## 1. 反馈与证据
用户 18:04 截图 `XiaoYe_2026-11-22_18-04-25.png`：爆率黄字压在图标/蓝格线上、看不清。
来源确认：live SHA `1d0f0155…`（我 18:03 部署版）在截图时间之前 → 本端证据。

## 2. 根因（UIMapInfoPanel.cpp:441-496）
- 爆率看不清：`Text::draw` 传入点是**基线**，字形顶在基线上方约 9px；旧代码把基线放在深色条顶 `bar_pos.y+0`，字形大半落在条上方、压在图标下沿与蓝格线上；条透明度 0.62 太浅。
- 图标未居中：图标按整个 38px 格子居中（y≈3–35），与爆率条（y=26–38）重叠。

## 3. 修复（一次成型）
- 格内分区：上方 `icon_area_h = SLOT - 13 = 25px` 为图标区，图标按 `max_dim ≤ 25` 缩放后在**图标区内水平/垂直居中** → 每个图标落于格子中心；下方 13px 为爆率条，两者不重叠。
- 爆率条：深色 `ColorBox(SLOT, 13, BLACK, 0.78f)`；字用 `Text A12B 黄字`，基线 `bar_pos.y+11`（完整落在条内），`Alignment::LEFT` + 显式 `(SLOT - width)/2` 居中，不再 CENTER 与手算叠加偏左。
- 数量字仍在右上，避开爆率条。

## 4. 构建/部署
- 并行会话持续锁 `x64\obj-Release\vc142.pdb`，用 MSBuild 命令行 `/p:IntDir=<独立目录>\obj\` + `/p:OutDir=<独立目录>\` 隔离 PDB/obj（命令行属性覆盖 vcxproj 硬编码 IntDir）→ `MSBUILD_RC=0`。
- 部署时 live exe 被并行会话短启客户端（PID 51836）占用，等其自行退出后覆盖；实时核对 SHA == 产物。
- 短启 `_ab_launch.ps1 -Wait 12` → `ALIVE`，无堆损坏。
- 真机观感（爆率是否清晰、图标是否居中）待用户截图。

## 5. 关联
- 视觉细化：`docs/features/worldmap-visual-fix-2026-11-22/README.md`。
- 回退点：`build-ratefix-verify/MapleStory.exe`。
