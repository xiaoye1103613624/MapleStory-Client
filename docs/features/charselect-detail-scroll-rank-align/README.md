# 选择角色页 · 详情卷轴与排行值对齐（2026-11-16）

## 需求

用户反馈选角页详情卷轴（红框区域）：
1. "排行榜"下方空白纸面过长，排行榜既然是最后一行，高度应短一截；
2. "服务器排行榜"的属性值应往右居中显示。

## 取证结论（证据链）

见 `evidence/img-nodes-2026-11-16.md`。要点：

- orange-wz 实时读取运行时 `E:\MXD\BeiDou-Client-xy\Data\UI\Login.img`：`CharSelect/scroll` 有 4 组卷轴帧：
  - `scroll/0` = 展开 30→161→160→**158**（末帧定格）
  - `scroll/1` = 收起 158→…→30
  - `scroll/2` = 展开 30→196→195→**193**（原代码误用）
  - `scroll/3` = 收起 193→…→30
  - 两组展开帧结构完全相同（木杆 y1、底卷边 6px 渐暗），仅纸面长度差 35px。
- 正确端《选择角色页.png》逐像素量取：卷轴顶杆 y≈195、底卷边 y347~352，总高≈158；按代码定位 `scroll_pos.y≈195`，用 193 帧底到 y≈388（多 35~43px = 红框空白），用 158 帧底 y≈353 与正确端吻合。
- 排行值：正确端"排行不可用"文字中心 x≈481，内容板米黄区 x391~577 中心 x≈484 ⇒ 值居中于板；原代码无 rank 左对齐(36)、有 rank 右对齐(115)均偏一侧。

## 实现（零资源改写，仅代码）

文件：`IO/UITypes/UICharSelect.cpp`、`.h`（注释）。

1. **卷轴帧源**：`CharSelect/scroll/2`（193 高）→ `CharSelect/scroll/0`（158 高），缺失时回退 `scroll/2`。动画播放用 `get_delay()`/`frames.size()` 动态驱动，换组零改动。`scroll_pos` 锚点与所有值行 y 不动。
2. **排行值居中**：删去左/右对齐双分支，统一按内容板中线 `CS_DETAIL_RANK_X - ranklabel.width()/2` 绘制（x=91，charInfo1 宽 183/2；y=93 baseline 不变）。有 rank 名次与"排行不可用"同位置，避免切换跳变。

## 验证

- UICharSelect.cpp 单文件语法检查（cl.exe /Zs，Release x64 宏/include 集）：**RC=0**，仅既有 `USE_NX` 宏重定义警告。
- 完整构建 + 实机抓屏对比：**未执行**。原因：工作区既有的无关错误阻塞构建——`IO/Components/MapTooltip.cpp:350` 调用 `ColorLine(size, color, opacity)` 少传第 4 个 `bool vertical` 参数（构造函数为 `ColorLine(int16_t, Color::Name, float, bool)`），其后 473~481 行有级联语法错误。该文件非本次改动（用户选择"只单文件验证，不碰 MapTooltip.cpp"）。

## 待办 / 遗留

- MapTooltip.cpp 既有编译错误修复后，重新 `tools/build_smoke.bat` 并抓屏（选角页）与正确端逐像素对比卷轴底边与排行值中心。
- 有 rank 场景无正确端截图证据，本次按"居中"统一处理（与原 Angel 右对齐口径不同）；若有确凿证据需回退该分支，恢复 `x=115-width` 即可。

## 产物

- `evidence/img-nodes-2026-11-16.md`：orange-wz 节点取证 + 贴图/截图像素量取。
- `tools/_charselect_out/`：导出的 charInfo1/scroll 各帧 PNG、正确端卷轴放大图、cl 语法检查输出（临时排障产物，可不入库）。
