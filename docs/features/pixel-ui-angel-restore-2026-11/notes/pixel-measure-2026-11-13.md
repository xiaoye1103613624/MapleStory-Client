# 正确客户端截图像素量测（2026-11-13）

权威源：`C:\Users\11036\Desktop\正确客户端截图`（已覆盖同步至 `evidence/screens/`，见 `screenshot-map.md`）。

## 登录链（全帧 1280×720）

| 原名 | 尺寸 | 结论 |
|---|---|---|
| 登录界面 / 选择频道 / 选中频道 / 选择角色 / 进入游戏 / 创建* | 1280×720 | 与登录钉死决议一致 |
| 登录中心裁切 | 木牌三层（账号/密码/保存）+ 橙钮 | 与 bedou `UILogin` + Angel `CUITitle::OnCreate` 同构；`TITLE_WINDOW_OFFSET(58,16)` 沿用 |

## Wave2 底栏 / QS

| 原名 | 尺寸 | 对照 |
|---|---|---|
| 底部信息栏和功能按钮 | 816×142 | 功能钮条 |
| 底部快捷按钮窗 | 460×76 | QS 单独裁切 |
| 底部信息栏、按钮、快捷键窗 | 1282×142 | 合图；`1282−816=466` = `HudLayout::QS_PANEL_W_26` |
| 聊天框展开 / 收起 | 569×103 / 566×40 | 聊天条裁切 |

`tools/test_hudlayout.py` → `HUDLAYOUT_OK`。

## Wave3 快捷窗裁切

| 原名 | 尺寸 | 备注 |
|---|---|---|
| 角色能力 | 428×349 | 主+详合图；边缘能量峰 ≈x207 → classic 主栏宽痕迹；**live** `Stat/backgrnd`=300×392、`backgrnd2`=352×392 → 运行走 `UIStatsLayoutV4` |
| 角色装备 | 348×305 | E |
| 背包栏展开 / 折叠 | 602×291 / 179×289 | I |
| 技能框+技能组合框 | 380×286 | K+宏组合 |
| 键盘设置 | 629×369 | KeyConfig |
| 打开了S-E-I-K页面 | 1280×720 | 多窗同屏 |

## 构建

- Staging（`/FS` + 独立 IntDir）：`E:\MXD\BeiDou-Client-xy\_pixel_build_stage\MapleStory.exe` 成功（EXITCODE=0）。
- Live OutDir 仍被提权 `MapleStory.exe` 占用（Access denied），需用户关闭客户端后覆盖部署。
