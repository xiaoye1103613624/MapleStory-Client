# 左侧切换尺寸按钮 + 多分辨自适应（2026-11-20 00:xx）

## 用户三条反馈与逐条结论

### 1.「底部经验条也不见了」→ 已修（真因：origin 双重偏移）
- `EXPBar/1920/layer:back` = 1920×10、**origin=(0,-23)**（orange-wz 实测）。
- `Texture::draw(pos)` 视觉左上 = `pos - origin`。原代码把 `exp_pos.y = 87` 当作"视觉顶"，
  实际视觉顶 = 87 - (-23) = **110**，1280×720 下 position.y=609 → 屏幕 y=719，
  正好被推出视口底边 → 经验条整条不可见。
- 修复：`exp_pos.y = EXP_BOTTOM + EXP_ORIGIN_Y = 87 + (-23) = 64`（:349-352），
  经验条回到 609+87=696..720 可见区。截图 tools/_v25_bottom.png 可见 `616169972 [12.85%]`。

### 2.「不改分辨率，要做多分辨适配，往左或者把快捷键窗口进行切换」→ 已改
- Settings 已回退 **1280×720**（window.width/height + game_width/game_height 四项）。
- 弃用「按 1920 绝对坐标 + 等比缩放」方案（素材尺寸固定，缩放会把血条压扁）。
- 改为**从右往左链式排布**（:337-370）：
  - `qs_cols` 按视口切换：≥1600→10 列 / ≥1280→6 列 / 更窄→4 列
    （对应 192 `OptionMenu/game/combo:quickSlotNum` 可配格数、083 `quickSlot08/26/32` 三档宽度 UI）
  - `qs_x = VWIDTH - qs_visible_w - 2`（贴右缘）
  - `menu_x = qs_x - 4 - 210`（六钮 6×35 在 QS 左）
  - `status_x = menu_x - 8 - 198`（血条再左）
  - 任意视口宽都不重叠；底板按 `avail_w` 动态裁剪（:721）。
- 删除 1280/1366/1920 的 `pos_adj` 与 ±7/-5 微调块（链式已自适应）。

### 3.「左侧有切换快捷键显示尺寸的 UI 按钮」→ 已让它可见（真因：被底板覆盖）
- 图4（62×100）实测 = **放大 ~1.45 倍的局部特写**：`button:Extend/Fold` 竖条（12/13×71）
  + 相邻第 1 列（32×66 两格）≈ 44×71 → 放大后 62×100。
- orange-wz 扫 StatusBar3.img/mainBar **无** 45×47 或 62×100 节点，只有
  `button:Extend(13×71)/Fold(12×71)/Extend800/Fold800`，佐证上述判定。
- 旧代码两处错：①位置 `quickslot_pos-(2,0)` 与底板重叠；②**绘制在底板之前 → 被底板整块盖住**。
- 修复：
  - 位置改到底板左缘外侧：`(quickslot_pos.x - 13, EXP_BOTTOM - 71)`（:491-500）
  - 绘制移到 `draw_modern_quickslot()` **之后**（:735-742），不再被背板覆盖
- 截图 tools/_v23_qs.png 可见 QS 最左白色竖条。

## 附带修正
- 经验条文字位置：图1 实测在**血条左下方**（1920 下 x≈400..520 = 视口 21%~27%），
  非居中。`statset_pos.x = VWIDTH*21/100 + 120`（:403，statset 为 RIGHT 对齐）。
- 快捷格键帽 `CAP_TOP` 由 (1,0) 改 (2,1)，避免压住格子上边框（:2508 附近）。

## 验证
- BUILD_EXITCODE=0；live SHA `28a8954874eb21f331658ba1e09f89db…` 与 hud_theme_out 产物一致（实时核对）。
- 进图截图 tools/_v25_game.png / _v25_bottom.png：经验条可见、血条/菜单/快捷键链式排开不重叠、
  左侧切换竖条可见。
- 客户端 PID 45932 进图成功、无闪退（`_init_debug.log` 到 loop:enter；WER 无新崩溃记录）。

## 仍未完成（如实）
- **聊天框 UI** 未逐像素对齐图1（本轮未动 UIChatBar；图1 页签深棕底金边+深色历史框+输入行三段式）。
- **切换竖条点击行为**未真机点验（`toggle_qs` 的 `quickslot_adj=(0, ±67)` 已按 IDA RelMove 实现）。
- 血条右缘有少量素材烤字（status/backgrnd 自带）在菜单左侧露出，属素材内容非布局错误。
