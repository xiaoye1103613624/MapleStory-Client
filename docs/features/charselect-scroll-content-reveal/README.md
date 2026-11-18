# 选角详情卷轴 · 内容随纸面渐显（2026-11-16）

## 需求

用户视频反馈：选角时卷轴展开的约 0.5 秒内，属性名+数值格子裸露在森林/蓝天背景上（"没有羊皮纸背景、像多出一格一格"）。用户确认非加载问题后，选择"内容随纸面渐显"方案（最贴近原生卷轴观感）。

## 根因与取证

详见 `evidence/reveal-analysis.md`。要点：

- 卷轴展开首帧 frame0（高 30，delay 250ms）只是卷起木杆、无纸面，但内容板从第一帧就完整绘制 → 格子裸露。
- orange-wz 实时取证的帧数据 + 视频抽帧双重确认，纸面底 ≈ 帧高−18，推导出可见高度 `vis = clamp(帧高−43, 0, 115)`。

## 实现（零资源改写，仅 `IO/UITypes/UICharSelect.cpp`）

### 1. 卷轴帧与可见高度

- 把当前卷轴帧指针提到块外（`const Frame* scroll_frame`），供纸面绘制与 vis 计算共用。
- 有效内容高度 `content_h = 贴图高 − 36`（原 `Range(0,36)` 裁掉自带"排行榜"行后的高度）。
- 仅展开播放中（`charinfo_scroll_playing`）才按当前帧高算 `vis = clamp(帧高−43, 0, content_h)`；定格态强制 `vis=content_h`。

### 2. 内容板动态裁切

`charinfo.draw(args, Range<int16_t>(0, board_h − vis))`：

- 定格态 vis=content_h → `Range(0,36)`，与原实现逐字节一致。
- frame0 vis=0 → 整块跳过绘制（显式 `if(vis>0)`，避免 UV 反转）。
- 展开期间 vis 随纸面高度增长 → 底板自上而下显现。

### 3. 文字渐显

各值行按 `row_visible(y) = (vis>0 && y+4<=vis)` 判定才绘制：

- 等级、`infolabels[6]`（职业/人气/四维）、排行值统一判定。
- 定格态全部绘制（零回归）；卷起态 vis=0 全部隐藏。

### 4. 零回归与兜底

- 定格态（静止展示）：vis=content_h、Range(0,36)、全部文字绘制，与改动前完全一致。
- 节点缺失（`charinfo_has_scroll==false`）：scroll_frame=nullptr，vis 保持 content_h，走原兜底分支。

## 验证

- `cl.exe /Zs` 单文件语法检查（Release x64 宏/include 集）：见执行记录。
- 完整构建 + 实机抓屏：**未执行**。工作区既有无关错误阻塞 `tools/build_smoke.bat`：`IO/Components/MapTooltip.cpp:350` 的 `ColorLine(size, color, opacity)` 少传第 4 个 `bool vertical` 参数，473~481 行有级联语法错误。该文件非本次改动，用户已选"只单文件验证、不碰 MapTooltip.cpp"。

## 遗留 / 后续

- 用户修复 MapTooltip.cpp 后，重跑 `tools/build_smoke.bat` + 抓屏（选角页切换角色瞬间）确认格子裸露消失、定格态外观不变。
- 用户选择"内容随纸面渐显"；当前帧数据（frame0→frame1 直接 30→161）实际只有"全隐藏→全显示"两态，公式对将来出现中间高度的帧也成立。

## 产物

- `evidence/reveal-analysis.md`：视频抽帧 + IMG 帧数据 + 几何推导。
- `tools/_charselect_out/`：视频联系表、导出贴图 PNG、语法检查输出（临时排障产物，可不入库）。
