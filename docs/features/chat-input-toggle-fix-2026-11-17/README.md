# 聊天栏 +/− 按钮与输入框激活修复（2026-11-17）

## 需求与症状

用户在问题端反馈（截图仅作问题描述输入，**来源端未确认，不作为验收凭据**）：

1. 聊天未激活、历史未展开时右侧 `+` 号点击没有效果；
2. 历史展开后右侧还是 `+` 号；
3. 点击输入框内容区域后右侧切出 `-` 号，但输入框无法激活、不能输入。

三张截图红字均为 `You do not have enough hp to use this skill.`（战斗 HP 不足刷屏），与聊天栏逻辑无关。

## RCA 根因（逐行，均在 IO/UITypes/UIChatBar.cpp）

`+/−` 符号唯一由 `sync_classic_buttons()`（:1785-1802）决定：`BtMax(+)` active = `!classic_log_open`、`BtMin(-)` active = `classic_log_open`。三个症状指向三处状态/命中缺口：

| # | 根因位置 | 缺口 |
|---|----------|------|
| A | `set_chat_type` 同态早退（原 :1822-1832） | 同 nType 时只同步焦点、不重算 `classic_log_open`，造成 `chat_type=3 && log_open=false` 脱钩 → 点 `+` 无反应、历史开却仍显示 `+` |
| B | 收起态命中分界（原 :888） | 用局部裸宽 `dimension.x()-80`(=486) 比屏幕光标 x；classic HUD 居中 `position.x()>0` 时按钮屏 x≈cx+536 恒 >486，分界错位 → `+` 号点不到 |
| C | 收起态点击处理 + 输入转发（原 :884-904 / :1018） | 点灰带误调 `set_classic_log(true)`（展开历史）而非 `toggle_input(true)`；且只有 `view_input` 才转发 `input_text.send_cursor` → 点输入区切出 `-` 却打不了字 |

## 修复（一次性三处，同一 TU）

1. **三态强不变量**：删除 `set_chat_type` 同态早退，`classic_log_open`/`view_input`/`input_text` state 一律由 `nType` 唯一推导。type3 同态不会误清输入文本（只有 type1 清），正常流程零回归。
2. **坐标系修正**：分界基准改为 `position.x() + LOG_BT_X`（本栏屏幕左沿 + 按钮行内偏移），不再用裸宽度比屏幕 x。
3. **激活入口**：收起态点灰带左段 → `toggle_input(true)`（原生 083 StartChat，nType1→2）；另在展开态历史框内补输入正文区命中（与 `update()` 的 input rect 同源），点击 → `toggle_input(true)` 再聚焦。

## 验证

- 构建：MSBuild Release/x64/v145，`/t:Rebuild` 到隔离目录 `chatlog_fix_out/`（脚本 `build_chatlog_fix.bat`，日志 `chatlog_fix_build.log`）。
- 结果：`BUILD_EXITCODE=0`，无 `error C`、无链接 error；仅既有 warning（C4456/C4286/LNK4098/LNK4099 等，与本次改动无关）。
- 产物：`chatlog_fix_out/MapleStory.exe`，sha256 `3bbacfc0f34194c8a5af06a52553db3084c64f7395c755723e5de5de6dfe8fb9`（2026-11-17 23:35）。
- live `MapleStory.exe` 当前 sha256 `17c03fa3fc304d559093d05cd37a98eaeab3b9c6ba07464325f82dc4ec859793`（仅对比，**未部署、未覆盖、未动进程**）。

## 部署记录（2026-11-17 23:50）

- 部署脚本：`tools/deploy_chatlog_fix.ps1`（杀进程 → 备份 → 覆盖 live+pua24 → 立即核对 → 延迟 5s 复查）。
- 备份：`E:\MXD\BeiDou-Client-xy\MapleStory.exe.bak-pre-chatlogfix-`（= 部署前 live，回滚点有效）。
- 三处 SHA 实时 + 延迟 5s + 事后复查均一致 = `3bbacfc0f34194c8a5af06a52553db3084c64f7395c755723e5de5de6dfe8fb9`：
  - `chatlog_fix_out/MapleStory.exe`（构建产物）
  - `BeiDou-Client-xy/MapleStory.exe`（live）
  - `BeiDou-Client-xy/MapleStory_pua24.exe`（标记副本）
- 残留一个被登录器守护的客户端进程 PID 26292（23:29 创建），`taskkill` 后被自动拉起；不影响 exe 文件部署，但**用户重启客户端时需重新登录器拉起，确认加载的是新 exe**。

## 真机验收状态（如实声明）

**已部署、SHA 稳定一致，但真机交互未由我确认。** 截图来源端未确认，只作问题描述；三处症状是否消失需用户在问题端实测：
1. 收起态点 `+` 应展开历史、符号切 `-`；
2. 展开后符号稳定显示 `-`；
3. 点输入框内容区应激活输入（白光标出现、可打字），不再只是切符号。

登录器会回滚 exe（本次部署前 live 已从 `17c03fa3` 被改写为 `73f1c492`），旧部署声明会失效——**用户每次验收前须实时核对三处 SHA 仍为 `3bbacfc0…`**。
