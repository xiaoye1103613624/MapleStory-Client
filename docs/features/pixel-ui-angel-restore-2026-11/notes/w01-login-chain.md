# Wave1 · 登录链像素（1280×720）

对照源截图：`C:\Users\11036\Desktop\正确客户端截图`（副本 `evidence/screens/`）。

## 证据

| 窗 | Angel | 当前端 | 结论 |
|---|---|---|---|
| Title | `CUITitle::OnCreate @0x62054a`：BtLogin(213,-5)… Edit(67,12)/(67,40) | `UILogin.cpp` bedou 分支同坐标 + TITLE_WINDOW_OFFSET(58,16) | 已对齐；字色 WHITE |
| World/Channel | 频道窗 (-176,-688)；卷轴 (-38,-141)；BtGoWorld(320,217) | `UIWorldSelect.cpp` WS_* 常量 | 已对齐 |
| CharSelect | 命中框/卷轴 origin / charInfo1 | `UICharSelect.cpp` AreaButton 盖身体 + scroll origin | 已按 2026-11 批次对齐 |
| RaceSelect | OnCreate 三卡 + map origin (-372,-2128) | `UIRaceSelect.cpp` kClassic* | 已对齐 |
| Creation | 板内实测 + IDA | `UICommonCreation.cpp` classic | 沿用 create-char-classic-dual-path |

## 本轮动作

- 规范化截图入库；layout 骨架 `notes/layout-login.json` + `evidence/layout-specs/*`
- 无新增登录链坐标改动（主路径已与 Angel 一致）；残差待 OmniParser 后端就绪后 `parse_screen` 覆写 elements
