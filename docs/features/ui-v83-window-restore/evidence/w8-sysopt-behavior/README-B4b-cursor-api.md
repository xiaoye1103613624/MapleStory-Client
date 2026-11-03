# R28-B4b · 原生光标 API 取证（推翻 B4-1 的 warp 回写路径）

日期：2026-11-03 深夜。工具：IDA MCP（13337，IDB = BeiDou.exe.i64）+ 二进制串扫描。
方法：导入表枚举 + 六模块原始字节 needle 计数（needle 为 API 名 ASCII 串）。

## 1. 结论

**原生从不把游戏内光标回写成系统光标。** 「鼠标速度」只缩放客户区内相对位移、
虚拟光标钳制在客户区、自绘光标显示 —— 没有第二条路。

| needle | BeiDou.exe (8,503,296 B) | Gr2D_DX8.dll | suipre.dll | ijl15.dll | Canvas.dll | Shape2D.dll |
| --- | --- | --- | --- | --- | --- | --- |
| SetCursorPos | 0 | 0 | 0 | 0 | 0 | 0 |
| GetCursorPos | 0 | 0 | 0 | 0 | 0 | 0 |
| SendInput | 0 | 0 | 0 | 0 | 0 | 0 |
| mouse_event | 0 | 0 | 0 | 0 | 0 | 0 |
| ClipCursor | 0 | 0 | 0 | 0 | 0 | 0 |
| ShowCursor | **1**（导入表 user32!ShowCursor @0xaf02ac） | 0 | 0 | 0 | 0 | 0 |

IDA 侧交叉验证：
- `entity_query imports regex=*Cursor*` → 只有 `ShowCursor`（user32）与 `SetWindowPos`。
- `xrefs_to ShowCursor(0xaf02ac)` → 唯一代码引用 `sub_42C6C4`@0x42c780（显示/隐藏系统光标，
  与 SDL_HideCursor 同义；不是位置回写）。
- `find_regex "Mouse|MouseSpeed|WM_MOUSE"` → 0 命中（主 exe 文案本就加密，与 §2.1 既有结论一致；
  行为语义以注册表 soMouseSpeed + 截帧滑钮读数 + 「无回写 API」三条互证，足够定性）。

## 2. 对本包的意义

B4-1（aa0d4e2）实现里「窗口化时 SDL_WarpMouseInWindow 回写系统光标」这一段没有原生依据，
且实测构成反馈环：warp 目标是小数坐标、系统光标只能落整数像素，回抛 motion 事件与目标差
±0.5px，该噪声又被倍速放大再回写 ⇒ 光标逐帧抖动（用户报「移动鼠标乱闪」）。
R28-B4b 改为：虚拟光标只吃 `event.motion.xrel/yrel`（不回写、无环路），与原生口径一致。

副作用声明（有意为之）：虚拟光标跑到窗边时系统光标可能已停在客户区边缘，
移出窗口再回来会触发一次重锚定（SDL_GetMouseState 直接取真实位置，误差 ≤ 一次增益的位移）。

## 3. 探针脚本

`cursor_api_scan.py`（同目录，python3 直跑，只读）。IDB 侧查询记录在会话导出
`E:\MXD\BeiDou-Client-xy\确认B0执行状态与字体_2026-11-03_22-35.md` 之后的本会话。
