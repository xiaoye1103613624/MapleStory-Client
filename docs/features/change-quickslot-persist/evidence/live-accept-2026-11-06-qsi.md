# live accept 2026-11-06 — CHANGE_QUICKSLOT / QSI

## Environment

- exe: `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（LastWrite 2026-11-06 06:37:10，4880896 bytes）
- 账号：`admin` / 角色 Admin（勿入库）
- 服务端：s9 登录 8484（java PID 6968）

## 幽灵 PID 34476

| 项 | 结果 |
| --- | --- |
| 特征 | WS=32KB、Handles=0、Path 空、CreationDate=2026-11-01；父进程 `360tray` |
| taskkill /F /IM | Access Denied |
| OpenProcess + TerminateProcess | err=5 |
| RunAs taskkill | 回报 DONE，幽灵仍在 |
| schtasks /RL HIGHEST | Access Denied |
| Windows sudo | 本机已禁用 |
| exe 文件锁 | **未锁**，可覆盖/启动 |

占用策略（用户确认）：此后一律强杀真实客户端；幽灵可残留，不挡部署。

## 封包证据（本会话 `packet_dump.log`）

| 项 | 结果 |
| --- | --- |
| RECV `QUICKSLOT_INIT(0x9F)` | ×2：`9f 00 00`（custom=0，无 8×int） |
| SEND `CHANGE_QUICKSLOT(183)` | **0**（QSI 后无回声） |
| PERF | `QSI custom=0`（两轮进图） |

→ **验收项 3（进图不刷 183）✅**

## UI 点测（本轮）

| 项 | 状态 |
| --- | --- |
| 自动登录 → Henesys Admin | ✅ |
| 键设置 → 变更快捷键 → 改槽 | ⏳ 自动化抢焦点/KeySet 命中未闭环；待手点或下轮驱动 |
| 重登持久化 / DB `quickslotkeymapped` | ⏳ 依赖上项 |
| TemporaryStatView（`D`） | SoftKey `D` 开板 ✅；明细行 ⏳ 待 `!buffme`（见 native-temporary-stat-view evidence） |

## 双开

本轮单开 Admin；Messenger / `xiaoye` 未测（有余力下轮）。

## SoftKey 续作（同日稍后）

- 已部署：ASCII 回退、`UIShortCut::FOCUSED`、Admin quick `buffme`、全局 SoftKey `D`
- 自动注入 `!buffme` 仍失败（SEND chat=0）；手点 Admin/`!buffme` 可一秒闭环
