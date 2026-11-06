# Messenger 双开真机（2026-11-06）

## 环境

| 项 | 值 |
| --- | --- |
| exe | `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（含 SoftKey Text 模式后重建） |
| Admin | `BeiDou-Client-xy` + `login.auto` 临时 true（已回 false，未入库口令） |
| xiaoye | `E:\MXD\ms-alt-b53` + `XY_DATA_DIR`→主 Data；账号仅落仓库外 config |
| 开窗键 | Admin Settings 历史为 `V`（本轮实测 **`C` 才开 Messenger**）；alt 无 Settings → 默认 `C` |

## 已闭合

| # | 项 | 结论 | 证据 |
| --- | --- | --- | --- |
| 5 | 本机立绘 | ✅ Admin / xiaoye 各自开房均见 STAND1 立绘 + NameBar 四色条 | `20261106_p2_admin_local.png`、`20261106_xiaoye_room.png` |
| 5c | 关闭钮 | ✅ 标题栏右上 X 可见；点 `(282,12)` 后白底计数 77k→~5k | `messenger-btclose-coords/evidence/20261106_p2_admin_open.png`、`…_closed.png` |

## 未闭合（显式预留）

| # | 项 | 阻塞 | 补验 |
| --- | --- | --- | --- |
| 5 | 对方立绘 | 邀请未进房 | SoftKey **Pic** 门禁曾拦短名/键盘；已改为 **Text** 模式（见下）。本轮收尾 Admin 强杀后占线（登录弹「正在连接的账户」），未再双开复验邀请 |
| 5b | blink / chatBalloon | 同上，需同房双方 | 清 `loggedin`/踢幽灵会话后：Admin 开房→空名牌 SoftKey 键入 `xiaoye`→对方 Enter 同意→打字不回车看泡→回车看 blink |

## SoftKey 根因（本轮代码已修）

`UISoftKey` 默认按 **PIC 二级密码**实现：

1. `set_text_callback` 吞掉物理键盘字母数字（提示只能用鼠标）
2. OK 要求长度 ≥ 6
3. `check_pic` 重复字符校验

Messenger 邀请/聊天/拒绝与 UIAdmin 命令误用默认 Pic → 自动化 `type_text` 无效、短名无法 OK。

**修复**：`UISoftKey::Mode::{Pic,Text}`；Messenger / UIAdmin 走 `Mode::Text`（明文、最短 1、可键盘）；登录二级密码仍默认 Pic。

## 手点路径（邀请）

1. 双开进图；Admin 按 **`C`** 开 Messenger（见本机立绘）。
2. 点空 NameBar（或 `I` 且输入框未聚焦）→ SoftKey。
3. **键盘**输入对方角色名 → Enter/OK → 出站 `7a 00 03` + mapleStr。
4. 对方 YesNo → Enter 同意；双方应见对方立绘。
5. 一方输入框打字不回车 → 对方立绘上方 chatBalloon；回车 → 标题 blink。
6. 点关闭钮 `(276,6)` 关窗。

## 配置收尾

`xy-config.yaml`（主目录 + `ms-alt-b53`）`login.auto` / `packet_dump` 已回 **false**；口令未进仓库。
