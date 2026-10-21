# 登录书框素材对比（证据）

日期：2026-09-22
数据源：`E:\MXD\BeiDou-Client-xy\Data\.nxcache\UI.nx`（PKG4，NoLifeNx 输出）
复现工具：`tools/nx_inspect.py`（只读，列出节点 / 解码位图）

## 结论

图1（客户端实际画面）用的是**第三方素材包**，图2（参考客户端）用的是**原版素材**，
两者不是同一套 UI：

| 项 | `MapleEzorsiaV2wzfiles.img/Common/frame1024` | `Login.img/Common/frame`（原版） |
| --- | --- | --- |
| 尺寸 | 1024×768 | 1280×720 |
| 书页开口（透明区） | 约 611×461（占窗口 60%×60%） | 约 1091×581（占窗口 85%×81%） |
| 开口左/右边距 | 约 215 / 202 px | 约 108 / 93 px |
| 开口上/下边距 | 约 115 / 193 px | 约 31 / 109 px |
| 与截图逐像素匹配 | 图1：1:1 @(0,0) 命中 **79%** | 图2：1:1 @(0,0) 命中 **98.8%** |

两张书框的整体美术（纸边、书脊、右下 MapleStory 标志、页角花纹）是同一风格，
差别只在**开口大小**：Ezorsia 那张边框很宽，登录地图只能从中间 6 成见方的小窗透出，
看起来就是"没铺满窗口"。

## 资源清单（UI.nx）

- `Login.img/Common/frame` — 1280×720，原版唯一一版
- `MapleEzorsiaV2wzfiles.img/Common/frame{1024,1280,1366,1600,1920}` — 1024×768 / 1280×720 / 1366×768 / 1600×900 / 1920×1080
- **整个 UI.nx 中 1024×768 的位图只有 `frame1024` 一张**（`nx_inspect.py find 1024x768` 核实），
  即原版数据没有 1024×768 书框；原版登录 UI 是按 1280×720 做的。

## 复现命令

```text
python tools/nx_inspect.py list  <UI.nx> 2 Login.img/Common/frame
python tools/nx_inspect.py size  <UI.nx> 1024x768
python tools/nx_inspect.py image <UI.nx> Login.img/Common/frame out.png
```

截图素材（本机）：

- 客户端实际画面：`C:\Users\11036\AppData\Local\PixPin\Temp\XiaoYe_2026-09-22_23-12-34.png`（1024×768）
- 参考客户端画面：`C:\Users\11036\AppData\Local\PixPin\Temp\XiaoYe_2026-09-22_23-14-01.png`（1277×694 裁剪）

对比图：

- `login-frame-ezorsia1024.png`（第三方，宽边框）
- `login-frame-official-1280x720.png`（原版，开口铺满）
