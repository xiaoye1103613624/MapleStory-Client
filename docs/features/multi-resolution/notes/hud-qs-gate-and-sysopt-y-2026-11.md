# HUD QS 宽格门槛 + SysOpt Y 校准（2026-11）

## SysOpt 错乱根因

- 运行时 `UIWindow.img/SysOpt/backgrnd` = **299×396**（orange-wz `get_detail`）。
- 像素量取洋红行带中心 ≈ `45/75/105/135/165/195/225/255/285/315` + 新增「窗口大小」`≈346`。
- 与原生 365 底板前 10 行 **同 Y**，并非整板 +28；先前 `SLIDER_ROW/CHECK/COMBO +28` 把控件压到下一行 → 叠字、勾选偏左、下拉错行、「MapleStory文件/夹」折行飘中。
- SCROLL LOCK 为底板烘焙截图热键示意，非错贴图。
- 修复：控件 Y 回退 `_settings_map2.txt` / 083 原生值；语言项改落底部按钮条左侧；下拉文案 `formatted=false` 禁折行。保留 SysOpt 模态延迟 `queue_viewsize` 崩溃补丁。

## HUD 公式变更

- 旧：`is_qs_wide = VWIDTH >= 1024`；装不下时 `qs_x = vw - qs_w` 仍用 QS26 → 1024 压住功能钮。
- 新：`is_qs_wide(view_w, slots32) = view_w >= HUD_DESIGN_W + qs_panel_w`（26→1266，32→1371）；否则强制 QS08，并以所选宽重新 `hud_content_w` / 居中。
- classic `base/backgrnd` 绘制已注释（易恢复）；`backgrnd2`/gauge/chrome 保留。
