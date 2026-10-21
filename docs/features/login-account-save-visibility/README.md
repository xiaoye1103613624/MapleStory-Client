# 登录账号保存与输入可见

> 状态：账号保存/可见已修（2026-09-28）；启动加速 C、进图仅 BGM **后续追踪**  
> 产物：`docs/features/login-account-save-visibility/`

## 1. 已修：保存账号 + 输入可见

### 现象

- 勾选「保存账号」后 `Settings` 只有 `login.save: true`，`login.account` 为空。
- BeiDou 木牌输入框打字后看不见字。

### 根因

- 账号只在登录成功包用服务端 `account.name` 写入；输入框内容从不落盘。
- 木牌槽用 `CREAM` + 无竖直微调，15px 槽内几乎看不见。

### 修复

| 文件 | 改动 |
| --- | --- |
| `IO/UITypes/UILogin.cpp` | 点连接时若勾选则保存输入框账号并 `Configuration::save()`；勾选切换立刻写盘；BeiDou 文字 `WHITE` + `+2px` Y |
| `Net/Handlers/LoginHandlers.cpp` | 成功包仅在 `account.name` 非空时覆盖；避免写空冲掉已存 ID |

### 验证

- [ ] 勾选 → 输入账号 → 连接 → 看 `Settings` 的 `login.account`
- [ ] 重启客户端账号自动填入
- [ ] 输入时账号/密码 `*` 在木牌槽内可见

## 2. 后续追踪：选角进游戏只有 BGM

### 现象（用户反馈 2026-09-28）

选角色进入游戏后仍只有背景音乐，画面无地图（历史：纯黑 + 未响应；曾修 atlas 2048 误杀）。

### 当前状态

- 曾部署去掉全局 atlas 2048 拒绝（见 `enter-game-blackscreen-fix`）。
- 用户确认：**进图仍只有 BGM** → 主因可能未完全消除，或另有绘制/相机/lock/fade 问题。

### 排查清单（下一轮）

1. 复现后读 `_enter_game.log`：是否仍完整 `fadeprocedure done`。
2. 查是否仍有 `_atlas_skip.log` / 大图被拒。
3. 进图后截图像素：纯黑 vs 有 UI 无地图。
4. `GraphicsGL::locked`、`Window::opacity`、Stage `state`、相机位置。
5. map=2 背景/tile 是否加载成功（`Map002` 节点）。

### 暂缓

- 启动加速方案 C（先出窗 + 懒加载）— 等进图问题闭环后再排。
