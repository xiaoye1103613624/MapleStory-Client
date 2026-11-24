# 整窗 UI Theme 映射表格式（预留）

本期只接线 HUD；整窗切换时按下列表为每个逻辑窗填一行，**禁止**新代码直写资源根。

## 字段

| 字段 | 含义 |
|---|---|
| `alias` | 逻辑名（Item / Equip / Stat / Skill / KeyConfig / OptionMenu …） |
| `classic_root` | ClassicV83 节点，如 `UIWindow.img/Item` |
| `modern_root` | ModernMainBar 偏好链，如 `UIWindow2.img/Item` → `UIWindow.img/Item` |
| `resolver` | 现有 API：`window` / `window4` / `status_bar_root` / 专用 |
| `layout_notes` | 坐标表是否随主题变 |

## 初稿（当前行为 = modern 优先回退 classic）

| alias | classic_root | modern_root | resolver |
|---|---|---|---|
| Item | UIWindow.img/Item | UIWindow2.img/Item | window |
| Equip | UIWindow.img/Equip | UIWindow4.img/Equip → UIWindow2 → UIWindow | window4 |
| Stat | UIWindow.img/Stat | UIWindow4.img/Stat → … | window4 |
| Skill | UIWindow.img/Skill | UIWindow2.img/Skill | window |
| Quest | UIWindow.img/Quest | UIWindow2.img/Quest | window |
| KeyConfig | UIWindow.img/KeyConfig | StatusBar3.img/KeyConfig → UIWindow | status_bar_root / window |
| SysOpt | UIWindow.img/SysOpt | （同） | window |
| GameOpt | UIWindow.img/GameOpt | （同） | window |
| Shop | UIWindow.img/Shop | UIWindow2.img/Shop2 | shop_root |
| MiniMap | UIWindow.img/MiniMap | UIWindow2.img/MiniMap | window |

## 未来 pack

- `StatusBar2`（MapleStoryT）/ `192skin`：新增 `ThemeId` + 上表加列；填包前必须 orange-wz 节点对照，禁止裸拷 WZ。

## 架构升级：策略模式（2026-11-19 落地）

多版本主题切换已从 enum+散落 if/else 重构为**策略（Strategy）+ 工厂注册表**，满足用户「083/095/192 多版本共存、新增版本不改消费端」的要求：

| 组件 | 文件 | 职责 |
|---|---|---|
| 策略接口 | `Util/UiThemeLayout.h` `IThemeLayout` | `id/family/available/status_bar_root/window_root/window4_root/shop_root/metrics` |
| 布局常量 | `UiThemeLayout::LayoutMetrics` | HUD 基准原点、QuickSlot 原点/pitch/排数，避免魔数散落 |
| 083 策略 | `Classic083Layout` | StatusBar.img + UIWindow.img |
| modern 策略 | `ModernStatusBar3Layout` | StatusBar3.img + UIWindow2/4 |
| 192 策略 | `StatusBar2Layout` | StatusBar2.img + UIWindow2/4；metrics 取 IDA `0x16BE531`/`s_ptShortKeyPos@0x279C390` 实测 (512,577)/(7,15)/pitch33 |
| 注册表 | `UiThemeLayout::get/active` | 按 ThemeId 取策略，Auto 按 192→modern3→classic 探测可用包 |

- `UiTheme` 保留为**门面**（API 签名不变）：`clamp_to_available/can_use/layout_family/toggle/to_config/from_config` 全部委托策略；`toggle` 在所有可用策略间循环。
- `UiAssetResolve::status_bar_root/has_game_hud` 委托 `active()` 策略，消费端（UIStatusBar/UIMiniMap/UIOptionMenu）零改动。
- **新增 095**：只需 ① `ThemeId` 加枚举 ② 写一个 `IxXxLayout` 策略并在 `UiThemeLayout.cpp` 的 `g_registry` 注册 ③ `to_config/from_config/display_name` 加字符串映射。不再动 UiTheme/UiAssetResolve 的控制流。

> 192 当前 `available()=false`（live 无 StatusBar2.img），自动回退 modern；跨端导入 StatusBar2 属高风险迁移（见 AGENTS.md §3.1 + crash-icon-missing-heap note），未在本轮执行。192 的**布局/交互逻辑**已用 IDA+IMG 证据对齐到 modern 策略。
