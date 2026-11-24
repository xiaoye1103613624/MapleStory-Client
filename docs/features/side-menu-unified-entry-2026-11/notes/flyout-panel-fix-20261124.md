# Flyout 底板 + 成员窗口点位修正（2026-11-24）

用户拍板 **A**：底板效果参照 ShortCutUI + 修错位点位 + 同步 live frames。

## 代码

| 文件 | 改动 |
|---|---|
| `IO/UITypes/UISideIconBar.h/.cpp` | 加载 `ShortCutUI` top/middle/bottom；展开时贴目录右侧画三段底板；格距改 IDA 公式 X=56/Y=92、TOP=33、PAD=10；越界夹视口 |
| `IO/UITypes/UIMaker.cpp` | `LayoutPolicy::Center` + `center_on_screen()` |
| `IO/UITypes/UIAlliance.cpp` | 同上 |
| `Util/FeatureRegistry.cpp` | `mysticbadge` 改为 `UIEquipInventory::makeactive()` → `sync_side_panels` 锚到装备窗左侧（不再单独 `(0,0)`） |

## 资源

- 已将仓库 `UI/SideIcons/frames/`（16 组 + Anvil 占位克隆自 mapleStyle）同步到 live `E:\MXD\BeiDou-Client-xy\UI\SideIcons\frames\`。

## 构建

- `build_sidemenu_flyout_panel_20261124.bat` → `EXITCODE=0`
- 产物：`build-sidemenu-flyout-panel-20261124\MapleStory.exe`

## 验收点

1. 点侧栏目录：右侧出现 ShortCutUI 风格底板，成员图标在框内横排。
2. 点「神秘徽章」：装备窗打开，神秘徽章贴左侧，不在屏幕左上角。
3. 点「专业技术 / 联盟」：窗口屏幕居中。
4. live 有 `UI/SideIcons/frames/<iconKey>/*.png` 时走序列帧动效。
