# 左侧小图标 / 底部菜单 / 右上时间天气 修复（2026-11-23）

## 背景
用户反馈四项：①上面三个图标点击闪退；②小灯泡图标含义；③底部两排功能按钮离经验条太近；④右上时间过大、天气想显示图标。

## 截图来源端认定（重要）
- 崩溃证据：Windows WER `AppCrash_MapleStory.exe_39c9cc…`（2026-11-23 15:06:25，`c0000005`，故障模块 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`）；截图 `XiaoYe_2026-11-23_15-07-48.png` 生成于 15:07:48，紧随崩溃。
- 右上有 44px 黄色时间，与 `UIBuffList.cpp:163-169`（2026-11-23 改动）吻合；小地图时间 16:01 与天气 HUD 同源。
- 结论：**截图即本端（修改后）**，可作为问题描述依据；但注意崩溃发生在 05:27 构建的旧 exe，而 live exe 时间戳 13:28（`build_dailygift_final`），旧崩溃栈已由本次构建替换。

## ① 点击侧栏图标闪退 — 已修复
**根因（行号级）**：
- `IO/UIElement.cpp:386` `btit.second->bounds(position)` 是崩溃点（WER 偏移 `0xe084b`，PDB 符号化 = `UIElement::send_cursor`）。
- `send_cursor` 遍历 `buttons`（`:381`）；`UIElement.cpp:404/414` 调 `button_pressed`。
- `UISideIconBar::button_pressed`（`UISideIconBar.cpp:478/483/497`）内 `toggle_folder/close_flyout` → `rebuild_buttons()` → `buttons.clear()`（`:134`）——**遍历中清空容器 → 迭代器失效 → 下一轮 `:386` 访问野指针**。
- 上面前三个是 root 项（folder 或 leaf，均触发 rebuild），故「点上面三个闪退」。

**修复**：点击回调只改数据 + `defer_rebuild()` 置脏；`rebuild_buttons/layout_buttons` 延迟到 `update()` 下一帧（send_cursor 遍历之外）执行。flyout 分支先 `const auto item = fly_items[idx]` 拷值再 close，避免读已清空容器。

## ② 小灯泡图标含义
`Util/SideIconConfig.cpp:66`：`{ "bulb", true, "local:bulb", Action::OpenUI, "Event" }` —— 灯泡（bulb）= **活动** 入口（点开活动窗 `UIEvent`）。同类 legacy：`book`=任务日志、`star`=每日礼物。`SideMenuConfig.cpp:503` 迁移时把 `bulb` 映射为图标键 `Daily`。

## ③ 底部功能按钮右移 — 已修复
`IO/UITypes/UIStatusBar.cpp` 新增 `MENU_X_SHIFT=48`（`:371` 附近），`menu_x`（`:407`）由纯居中改为居中 + 48。主功能菜单条（`menu_pos`/`menu_strip` 均基于 `menu_x`）整体右移，拉开与经验条距离；血条/经验条/快捷栏锚点不变。

## ④ 时间缩小一半 + 天气图标 — 已修复
- `Graphics/Text.h:47-52`：新增 `A22M`（22px），时间由 `A44M`(44px) 改 `A22M`，缩一半；A44M 保留加载以兼容 NUM_FONTS 循环。
- `Graphics/GraphicsGL.cpp:274-277`：addfont A22M。
- `IO/UITypes/UIBuffList.h/.cpp`：时间 `timeclock_*` 字体 A22M；新增 `weatherglyph_*`（A22M，贴右缘，时间右对齐点左移 label宽度+8 让位），天气按 `weather::State::sky()` 映射 CJK 单字：晴/雨/雪/阴/雷/叶/花/雾。
- **不用 emoji**：默认字体 `SourceHanSerifSC` 无 emoji 码点，`GraphicsGL.cpp:500` 缺字保护会返回 nullptr → 变方块/不显示；CJK 单字可靠。

## 验证与交付（2026-11-23 15:32）
- 构建：`build_sideicon_hud_20261123.bat` → `build-sideicon-hud-20261123/MapleStory.exe`，**EXITCODE=0**（仅既有 warning，无 error）。
- 同步：占用 live 的 PID 53856（命令行确认 = live 客户端）已终止；覆盖前备份 `MapleStory.exe.bak-sideicon-hud-20261123`；用 `tools/_sync_sideicon_hud_20261123.ps1` 覆盖。
- SHA256 复核（覆盖后 + 延迟 6s 复核均一致，登录器未回滚）：`F8D3EDAF9CA32DA1F2F67448C621719CAE76B62860E6C481E8DA08988017DB10`，live_size=5513216，mtime 15:32:26。
- 另外 5 个 MapleStory 进程（命令行空、跨天启动）非 live 目标，未触碰。
- **真机未验收**：本机 WER 崩溃栈来自 05:27 旧 exe，非本次产物；需用户启动新 exe 复测四点。live 当前未运行。
