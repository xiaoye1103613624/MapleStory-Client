# A11j Quiz / SpeedQuiz（msgtype 5/6）— 已定案已接线

> IDB：`BeiDou.exe.i64` ｜ 日期：2026-11-05  
> 结论：**Quiz UI + 出站已接线**；SpeedQuiz 协议已对齐，UI 本轮仍吞包。

## 1. 已证实（IDA）

| 项 | Quiz (5) | SpeedQuiz (6) |
| --- | --- | --- |
| 入口 | `747A22` → `A26B09` | `747A34` → `A26C66` |
| 开窗 | `A2B66D` → `7E99BC`（`dword_BF0E00`） | `A2B6B2` → SpeedQuiz 单例 |
| 入站 res==0 | 3×string + **3×int** | **5×int** |
| 出站 | `7EB276`：`byte 5` + **string**（**无** action） | `7EC934`：`byte 6` + string（无 action） |
| 校验 | `7EAE53`：len ∈ [min,max]；tip 3920/3921 | 独立逻辑 |
| 超时 | `7EB16E`：到期发空串并关窗 | 有剩余秒 |
| 布局 | Edit `(109,157) 150×13`；OK `(241,199)`；倒计时 y=33 | — |

IMG：`UIWindow.img/InitialQuiz` → `backgrnd` **288×229**；`num1`/`num2` 数字；无 Bt*（用 `Basic.img/BtOK2`+`BtCancel2`）。

## 2. 本轮接线

| 层 | Quiz | SpeedQuiz |
| --- | --- | --- |
| s9 发包 | `OnAskQuiz` msgtype **5** + 3×int | `OnAskSpeedQuiz` msgtype **6** |
| s9 回程 | `NPCMoreTalkHandler` lastMsg 5：无 action，`readString` | 同形 lastMsg 6 |
| CM API | `sendAskQuiz` / `askQuiz` | `sendAskSpeedQuiz` / `askSpeedQuiz` |
| 客户端入站 | case 5 → `UIInitialQuiz::open` | case 6 仍读尽 + `-1` |
| 客户端出站 | `NpcTalkMorePacket(AskQuiz{}, str)` = `5+string` | 标签已备，UI 未接 |
| Avatar | **不动**（msgtype 7） | — |

## 3. SpeedQuiz pending

题面依赖 `nType/dwAnswer` 查表/图标，IDA/IMG 证据不足，不臆造完整 UI。协议字节已与 IDA 对齐，下一项接 UI 时只补渲染与出站 `AskSpeedQuiz`。
