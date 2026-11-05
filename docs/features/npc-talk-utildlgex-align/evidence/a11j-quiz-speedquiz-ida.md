# A11j Quiz / SpeedQuiz（msgtype 5/6）— IDA 取证（本轮预留）

> IDB：`BeiDou.exe.i64` ｜ 日期：2026-11-05  
> 结论：**本轮不接完整 UI**；继续读尽包 + `NpcTalkMore(-1)`。

## 1. 已证实

| 项 | Quiz (5) | SpeedQuiz (6) |
| --- | --- | --- |
| 入口 | `747A22` → `A26B09` | `747A34` → `A26C66` |
| 开窗 | `A2B66D` → `7E99BC`（`dword_BF0E00`） | `A2B6B2` → SpeedQuiz 单例 |
| 入站 res==0 | 3×string + **3×int** | **5×int** |
| 出站 | `7EB276`：`byte 5` + **string**（**无** action） | `7EC934`：`byte 6` + string（无 action） |
| 校验 | `7EAE53`：len ∈ [min,max]；tip 3920/3921 | 独立逻辑 |
| 超时 | `7EB16E`：到期发空串并关窗 | 有剩余秒 |

Quiz 非 UtilDlgEx；本仓仅有 `UIInitialQuiz` / `UISpeedQuiz` 壳（SoftKey 误发 GetText `(2,1,string)`）。

## 2. 与 s9 冲突（阻塞完整闭环）

| | IDA | s9 `PacketCreator` |
| --- | --- | --- |
| Quiz msgtype | **5** | **`0x6`** |
| SpeedQuiz msgtype | **6** | **`0x7`**（与 AskAvatar=7 冲突） |
| Quiz 尾字段 | 3×**int** | short×2 + int |
| 出站 | `5`/`6` + string | `NPCMoreTalkHandler` 无对应文本支 |

按 IDA 解析 s9 包会错位；按 s9 发包客户端进 Avatar/错误 case。须**单独改服务端对齐**后再做 UI。

## 3. 预留原因

1. 服务端 msgtype/字段未对齐（用户此前定「服务端另开」）。  
2. InitialQuiz 完整布局（标题/题面/提示/倒计时/Edit）需 IMG 节点取证；IDB 无 `InitialQuiz` 明文字符串可锚。  
3. 出站无 action 字节，与现 `NpcTalkMorePacket(string)` 及 `NPCMoreTalkHandler` 契约均不符，改服务端前接 UI 会挂脚本或误解析。

## 4. 本轮动作

- Handler case 5/6：保持读尽 + `-1`。  
- 不臆造 SoftKey/壳窗出站为「完整 Quiz」。
