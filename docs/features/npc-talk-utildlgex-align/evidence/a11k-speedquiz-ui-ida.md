# A11k SpeedQuiz 完整 UI（msgtype 6）— 已定案已接线

> IDB：`BeiDou.exe.i64` ｜ IMG：`UIWindow.img/SpeedQuiz` ｜ 日期：2026-11-05  
> 结论：**入站开窗 + 题面渲染 + 出站 `6+string` 已接线**。

## 1. 已证实（IDA）

| 项 | 地址 / 证据 |
| --- | --- |
| 入站 | `747A34` → `A26C66`：`byte res`；`res==0` → **5×int**（nType, dwAnswer, nCorrect, nRemain, tRemainSec） |
| 开窗 | `A2B6B2` → `7EB303`（单例 `dword_BF0E04`）；`7EC595` 灌参 |
| 布局 ctor | `7EB516`：Edit `(100,339) 165×13`；OK `(233,380)`=`Basic/BtOK2`；Next `(190,380)`；Giveup `(126,380)` |
| 绘制 | `7EB7D5`：倒计时 num1 @ y=33（111/132/冒号155/168/189）；`%2d pts` @ `(237,45)`；`%2d` 余题 @ `(99,381)`；题面底右 `(156-w, 287-h)`，`nType==2` 时 x 再 `-16` |
| 题面 | `7EC595`：`nType<2` → StringPool **2706**=`Npc/%07d.img/stand/0` / **1594**=`Mob/%07d.img/stand/0`；`≥2` → 道具图标 |
| 出站 | `7EC934`：`byte 6` + maple string，**无** action |
| 按钮 | `7EC46B`：OK=答案（len≥2）；Next=`""`；Giveup=`"__GIVEUP__"`；超时=`""`（`7EC7CB`） |
| 壳路径 | StringPool **3924**=`UI/UIWindow.img/SpeedQuiz/backgrnd` |

## 2. IMG（orange-wz）

`UIWindow.img/SpeedQuiz`：

| 节点 | 尺寸 / 态 |
| --- | --- |
| `backgrnd` | **286×410** |
| `num1/0..9` + `comma` | 20×27 / 11×27 |
| `BtOK` / `BtNext` / `BtGiveup` | MapleButton 五态（本端 OK 跟 IDA 用 `Basic/BtOK2`） |

## 3. 本轮接线

| 层 | 状态 |
| --- | --- |
| 客户端入站 case 6 | → `UISpeedQuiz::open`（缺资源仍 `-1`） |
| 客户端 UI | 题面 Npc/Mob/Item + 倒计时 + pts/余题 + Edit + 三钮 |
| 客户端出站 | `NpcTalkMorePacket(AskSpeedQuiz{}, str)` = `6+string` |
| s9 | 协议已对齐（0251d394da）；CM 注释更新 |
| OxQuiz 场包 | 改走 `UIFloatNotice`，不再占用 SpeedQuiz 壳 |

## 4. 字体 / 颜色

正文与 Edit：宋体路径 **A12M** + `DARKGREY`/`BLACK`（对齐 InitialQuiz / 用户约定；原生 Edit 字库名 StringPool 5527=`Arial`，本仓不换字库文件）。

## 5. 显式预留

| 项 | 说明 |
| --- | --- |
| Timer SFX | IDA `7EC7CB` remain≤120 播 StringPool 1563=`Timer`；本轮未接音频 |
| `SpeedQuiz/BtOK` 节点 | IMG 有，但 IDA 用 `Basic/BtOK2`；跟 IDA |
| Pet / Mirror UI | 仍吞包 `-1` |
