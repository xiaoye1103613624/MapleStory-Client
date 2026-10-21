# 仓库 UI 绘制产物

依据用户截图绘制的 MapleStory 风格「仓库」整体 UI 预览与按钮双态参考。

## 文件

| 文件 | 说明 |
| --- | --- |
| `warehouse_ui.psd` | 分层 PSD（Photoshop 工作稿） |
| `warehouse_ui_full.png` | 全画布预览（窗口 + 按钮双态条） |
| `warehouse_window.png` | 仅仓库窗口区域 |
| `warehouse_button_states.png` | 按钮 Normal / Pressed 对照条 |
| `_draw_warehouse.jsx` | 可复现绘制脚本 |

## 含有

- 上下双面板布局（存入 / 取出）
- 左侧空头像框（无角色/NPC）+ 功能按钮 + 提示文案
- 右侧 10×4 道具格、粉色斜线锁定区、底部货币条
- 按钮未点击 / 点击两态（桃色 / 绿色退出仓库 / 蓝色道具排列）

## 刻意省略

- 角色与 NPC 立绘
- 顶部背包分类页签（装备/消耗等）
- 游戏场景、HUD、小地图
