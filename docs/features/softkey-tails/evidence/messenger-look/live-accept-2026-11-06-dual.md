# Messenger 双开真机（2026-11-06）

## 环境

| 项 | 值 |
| --- | --- |
| exe | `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（SoftKey Text + KEY-TEXT 修复后，约 10:06） |
| Admin | `BeiDou-Client-xy` + `login.auto` 临时 true（已回 false，未入库口令） |
| xiaoye | `E:\MXD\ms-alt-b53` + `XY_DATA_DIR`→主 Data；账号仅落仓库外 config |
| 开窗键 | 实测 **`C`**（white>50k 才算真 Messenger；误开其它窗 white≈5k） |

## 已闭合

| # | 项 | 结论 | 证据 |
| --- | --- | --- | --- |
| 5 | 本机立绘 | ✅ | `20261106_p2_admin_local.png`、`20261106_p9_admin_local.png` |
| 5 | **对方立绘** | ✅ p9 | SoftKey Text KEY-TEXT 修复后邀请成功；`20261106_p9_admin_both.png` / `20261106_p9_xy_both.png`（双方 STAND1） |
| 5c | 关闭钮 | ✅ | `(282,12)` 可关；p6/p7 77k→~6k；p9 77k→13k（残余白块见下 SoftKey 散落） |

## 部分闭合 / 待拍

| # | 项 | 结论 | 备注 |
| --- | --- | --- | --- |
| 5b | blink / chatBalloon | 🟡 同房已具备 | p9 已同房并打字；单帧未稳定抓到泡/blink 动画 → pending 手点补拍 |

## SoftKey：影响联调的 UI

| 问题 | 影响 | 状态 |
| --- | --- | --- |
| Text 聚焦后 KEY TEXT 被 `text_input_active` 跳过 | 邀请名进不了框 → 误判协议 | ✅ 已修（`UISoftKey::update` Text→`end_ascii_text_input`） |
| Text tip `formatted=true` | `#` 可能吞提示 | ✅ Text tip 改 `formatted=false` |
| SoftKey **按键散落**（键帽漂出底板盖住 Messenger） | 鼠标点键不可信；关窗白底计数偏高 | ⏳ pending：需 SoftKey.img origin/`keypos` 对账（orange-wz 对本批 img 内部节点有已知解析限制）证据 `_p9_softkey_region.png` |

## 手点路径（邀请）

1. 双开进图；Admin 按 **`C`** 开 Messenger（white≈77k）。
2. 点空 NameBar → SoftKey Text；**键盘**输入角色名 → Enter。
3. 对方 YesNo → Enter；双方应见对方立绘。
4. 打字不回车看 chatBalloon；回车看 blink。
5. 点关闭 `(282,12)`。

## 配置收尾

`xy-config.yaml`（主目录 + `ms-alt-b53`）`login.auto` 已回 **false**；口令未进仓库；不 push。

## DML

授权 `UPDATE accounts SET loggedin=0 WHERE name IN ('admin','xiaoye')`：先 SELECT 均为 0；UPDATE 行数 0（本已清）。
