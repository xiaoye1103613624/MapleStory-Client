# Wave3 · 快捷窗 + S 自定义 V4

## S（角色属性）自定义规则

live `UIWindow.img/Stat/backgrnd` = **300×392**，`backgrnd2` = **352×392**（2026-11-13 orange-wz 复测确认）。

权威截图 `角色能力.png` 为 428×349 主+详合图（classic 宽痕迹），作文案/相对布局对照；运行几何以 live 画布 + V4 表为准。

`UIStatsInfo` 检测宽高 ≥290×380 → `custom_v4=true`：

- 数值坐标走 `UIStatsLayoutV4.h`（值进框：ink_top = box_y + 3）
- 详情原点 `(DETAIL_OX, DETAIL_OY)=(306,0)` → 与主栏同高 **底对齐**
- 命中框展开含详情宽；不叠原生 Disabled/basicStat

## 其它快捷窗

`UIDragElement` 统一 `SavedPosClamped` + 拖曳/开窗 `clamp_to_view`：E/I/K/P/G/F/KeyConfig/UserList 等可拖窗不画出客户区。

居中类：`UIQuestLog` / `UIWorldMap` / `UINotice` / `UINpcTalk` 已 `LayoutPolicy::Center` + clamp。

## E/I/K 逐像素（2026-11-13）

详见 `notes/w03-eik-pixel-2026-11-13.md`。

- E/I：对照正确截图模板匹配通过，本轮无位移改动。
- K：修正 `CLASSIC_SKILL_OFFSET`/`CLASSIC_SPUP_OFFSET`、去掉 Skill backgrnd (1,0) 假偏移、宏窗取消构造期居中。
- 残差：UserList / Quest / KeyConfig；S 的 EP/抗性行未绑 StatLabel（本轮未做）。
