# 验收清单 — StatusBar3 + ThemePack + 左侧竖栏

## 前置

- [x] `Data/UI/StatusBar3.img` 在 `BeiDou-Client-xy`（~1.8MB，含 mainBar）
- [x] `MapleStory.exe` 为含 ThemePack + UISideIconBar 的构建（`hud_sidedock_out` 同步）
- [x] 工作目录有 `side_icons.ini`（默认可点 4 槽）

## ThemePack

1. 进图，确认 classic 居中底栏（若当前 `ui.theme=classic|auto` 且无 modern 偏好）。
2. 点小地图标题栏左侧 **BtHide 样式 MapleButton**（不再是黄字「UI」）≥3 次。
3. 期望：classic 居中底栏 ↔ modern StatusBar3 底栏/聊天即时切换，不崩。
4. 系统设置「主题」项切换同样生效；重启后 `ui.theme` 保持。

## 左侧竖栏

1. 视口左侧偏上有竖排图标 + 底部折叠钮。
2. 点折叠：槽位隐藏，只留展开钮；再点恢复。
3. 至少点开 **任务**、**活动/每日/系统设置** 中任意 2 个 → 对应已有窗出现。
4. 编辑 `side_icons.ini` 增删槽 / 改 `arg`，等待热重载或重进图后生效。

## 明确不做（本版）

- 不整包搬 192 玩法/CEF/协议
- 不把 `功能_*.js` 当 UI 绑定源
- 不改 opcode 宽度
