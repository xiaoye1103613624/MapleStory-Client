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
