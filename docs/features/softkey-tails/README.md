# SoftKey 旧波尾巴（计划穿插项）

## 状态（2026-11-05）

| 项 | 代码 | 实机 |
| --- | --- | --- |
| 登录遮罩叠图 | knobs 已锁；登录期 SCROLL_LOCK 全局截图已补 | 叠图对照 → pending §3 |
| PersonalShop CREATE FM | SoftKey/双击许可证 → CREATE；FM 前置；ROOM 0/4 分流 | 需 FM 图 + 514/503 点测 |
| Messenger 立绘 | ✅ 本机 look + 脚点常量 + s9 create→join | 需实机截图确认缩放/脚点 |
| Messenger blink / chatBalloon | ✅ IDA+IMG 取证后接入打字泡与标题 blink | 双人房真机；blink 无 Min 态属适配 |
| Guild BBS | 公会窗 `B` → `UIGuildBBS` 完整窗 + List；摘要栏仍同步 | 见 [guild-bbs-ui](../guild-bbs-ui/) / pending §12 |

## 本轮代码补强（Messenger 立绘）

- 根因：s9 `joinMessenger` 不带 look；create 曾静默 → 本机座位无立绘。
- 客户端：`UIMessenger::fill_local_look` + `LOOK_SCALE/FOOT_*` 常量；开窗乐观占座 0。
- s9：`MessengerHandler` create 后 `joinMessenger(0)`。
- 笔记：[notes/messenger-look-2026-11-05.md](notes/messenger-look-2026-11-05.md)

## 真机步骤

见 [pending-live-tests §3–6](../pending-live-tests/README.md)。
