# Messenger 立绘闭环（2026-11-05）

## 已证实

1. **资源**：`UIWindow.img/Messenger/NameBar/0..3` = 89×19，origin (0,0)；槽位硬编码
   `(14,108)/(103,108)/(192,108)/(103,128)`（`messenger-layers.md`）。
2. **协议**：s9 `addMessengerPlayer`/`updateMessengerPlayer` 带 `addCharLook`；
   `joinMessenger` **只回 position**。`createMessenger` 历史上**零回包**。
3. **本机缺口**：建房/进房后本机座位永远收不到 look → 立绘空白（SoftKey #5 真因）。

## 本轮改动

| 端 | 改动 |
| --- | --- |
| 客户端 | `fill_local_look`：用 `Player::get_look()` 填本机座；`try_create_room` 乐观 `set_local_pos(0)` |
| 客户端 | 脚点常量 `LOOK_SCALE=0.70` / `LOOK_FOOT_DX=44` / `LOOK_FOOT_DY=-10`（NameBar 中线 + 上沿抬 10） |
| s9 | create 后补发 `joinMessenger(0)`，与 join 路径对齐 |

## 局限

- 缩放无原生 oracle（r474/r495 经验值）；真机截图可再微调。
- `blink` / `chatBalloon` 仍预留（见 messenger-layers §5）。
- 换装后本机立绘不自动刷（无对本机的 updateMessenger）。
