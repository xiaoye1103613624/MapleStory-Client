# 登录弹窗视口居中与分辨率自适应

> 状态：已闭环  
> 适用客户端/协议版本：BeiDou gms083（默认 1280×720，适配多分辨率切换）  
> 项目名称：MapleStory-Client  
> 作者/日期：2026-09-24  
> 产物目录：docs/features/login-modal-viewport-center/

## 1. 背景与目标

- **问题**：输入账号密码后确认协议弹窗（`UITermsOfService`）偏左上；同族登录 Notice / Wait 也写死约 800×600 绝对坐标。
- **根因**：面板与子控件使用屏幕绝对坐标；分辨率改为 1280×720 后不再居中；登录态无 `update_screen` 广播，切分辨率后无法重算。
- **目标**：登录模态统一「面板局部坐标 + 视口居中 + 分辨率变更重算」，后续切分辨率不丢位置。
- **不做**：不改地图锚点登录页（`UILogin` / `UIWorldSelect` / `UICharSelect`）；不动协议/封包。

## 2. 影响范围

| 层级 | 路径 | 变更 |
| --- | --- | --- |
| 客户端 | `IO/UIElement.h/.cpp` | 新增 `center_on_screen()` |
| 客户端 | `IO/UIStateLogin.h/.cpp` | 缓存 VWIDTH/VHEIGHT，变化时广播 `update_screen` |
| 客户端 | `IO/UITypes/UITermsOfService.*` | 局部坐标 + 居中 + `update_screen` |
| 客户端 | `IO/UITypes/UILoginNotice.*` | Notice / QuitConfirm / ClassConfirm / KeySelect / KeyConfirm |
| 客户端 | `IO/UITypes/UILoginWait.*` | 面板归一化到 (0,0) 后居中 |

## 3. 布局规则

1. 子控件只用**相对面板**的局部坐标；面板 sprite 在 `(0,0)`。
2. `dimension = 面板位图尺寸`，`position = center_on_screen()`（`(vieww-dim.x)/2, (viewh-dim.y)/2`）。
3. 视口变化 → `UIStateLogin::update` → 各弹窗 `update_screen` → 再 `center_on_screen()`。

### TOS 局部槽位（证据：`evidence/tos-0.png`）

| 项 | 局部坐标 | 依据 |
| --- | --- | --- |
| 面板 | 431×426，origin (0,0) | `Login.img/TOS/0` |
| OK / Cancel | (268, 388) / (342, 388) | 烘焙按钮槽约 y=400，按钮 75×32 |
| 文案 | (36, 50)，宽 340 | 纸面区约 x∈[18,421]、y∈[40,389] |
| 滑条 | x=384，y∈[55,360] | 贴文案右侧 |

## 4. 验证

- [x] `MSBuild … /t:Rebuild` Release/x64（2026-09-24）成功，产物 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
- [ ] 人工：登录弹协议 → 视口居中；错误 Notice / 等待框居中
- [ ] 人工：若可切分辨率，弹窗仍居中

## 5. 回滚

还原上述 UI / UIElement / UIStateLogin 文件即可。
