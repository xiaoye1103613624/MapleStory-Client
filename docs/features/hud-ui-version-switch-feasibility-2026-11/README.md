# HUD ThemePack 可切换（可扩展至整 UI）

## 一句话

运行时在 **083 Classic**（`StatusBar.img`）与 **Modern**（`StatusBar3.img`）底栏+聊天之间切换；配置键 `ui.theme`；协议仍锁 s9/083。另有配置表驱动的左侧功能竖栏（`UISideIconBar`）。

## 入口

| 入口 | 操作 |
|---|---|
| 小地图标题栏 Theme MapleButton（`BT_MIN` 左侧，`Basic.img/BtHide`） | 单击 Classic↔Modern，即时重建 |
| 系统设置底部「主题：…」 | 单击切换并落盘 |
| 左侧竖栏图标 / 折叠钮 | 开已有窗；`side_icons.ini` 可增减换绑 |

## 核心 API

- `Util/UiTheme.*` — `ThemeId` / `LayoutFamily` / `set_theme` / `toggle` / `resolve`
- `UiAssetResolve::is_classic_hud()` / `status_bar_root()` — 跟随 ThemePack
- `UIStateGame::apply_hud_theme()` / `UI::apply_hud_theme()` — `remove`+`emplace` STATUSBAR+CHATBAR
- `Util/SideIconConfig.*` + `UISideIconBar` — `side_icons.ini` 槽位表

## 配置

`ui.theme` = `auto` | `classic` | `modern`（缺包安全回退）  
`ui.side_icons` = `true` \| `false`（竖栏总开关）  
`side_icons.ini` — 槽位增减/换绑（见 [notes/side-icon-binding.md](notes/side-icon-binding.md)）

## 资源

- StatusBar3 入库索引：[resources/STATUSBAR3-INDEX.md](resources/STATUSBAR3-INDEX.md)
- 验收：[notes/acceptance-statusbar3-sidedock.md](notes/acceptance-statusbar3-sidedock.md)

## 整窗映射预留（下一阶段）

见 [notes/whole-ui-theme-map.md](notes/whole-ui-theme-map.md)。新窗口禁止直写 `nl::nx::UI["UIWindow*.img"]`，统一经 `UiAssetResolve::window` / `window4`（后续可挂 Theme 偏好链）。

## 回退矩阵

| 资源 | classic 请求 | modern 请求 |
|---|---|---|
| 仅 StatusBar.img | classic | → classic |
| 仅 StatusBar3 | → modern | modern |
| 两者皆有 | classic | modern |
| 皆无 | 不建 HUD | 不建 HUD |

## 多分辨率（modern）

非枚举视口宽对齐最近 `800/1024/1280/1366/1920` 布局常量；经验条仍拉满真实 `VWIDTH`。
