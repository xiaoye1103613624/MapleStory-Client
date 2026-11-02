# R27-B5-1a 展开态逐格读数（历史框 / 右端竖滚动条 / 顶边拖拽）

> 原生 oracle：`C:\Users\11036\Desktop\正确客户端截图\聊天框展开时候.png`（**569×103 局部截帧**）
> 同批对照：`聊天框收起的时候.png`（566×40，见 `coords-fold.md`）
> 原始读数：`coords-expand-raw.txt`（逐行取色）、`_expand_align.txt`（左右沿）、
> `_expand_sb_map.txt`（滚动条列 0/1 化点阵，本页表格就是它的人工判读）

## 1. 对齐基准

两张截帧不同源（展开 569×103 / 收起 566×40），对齐锚点仍用**白输入带顶缘那条
(255,255,255) 高光行**：

| | 原生展开截帧 | 映射到本端 |
| --- | --- | --- |
| 白带顶缘高光 | y=75 | `position.y()`（= 视图 y 653） |
| 灰带（白带上方 1px 内沿） | y=71..74 | `FOLD_EDGE_TOP` 那条线，收起态已定版 |
| x 轴 | 截帧 x0 == 视图 x0 | 见下方反证 |

x 轴对齐的反证（同一行 y=88，即白带内）：不透明亮段是 `(80, 535)` 与 `(548, 568)`，
中间 **536..547 空掉 12px** —— 那正是 `Basic.img/BtMin` 的 12×12（本端
`LOG_BT_X = 536`），说明截帧 x0 与视图 x0 重合，可以直接跨图比 x。

## 2. 定版读数（全部相对白带顶缘，负值=在框那一侧）

| 项 | 原生截帧 | rel 白带顶 | 本端落点 |
| --- | --- | --- | --- |
| 历史框（半透明底板）顶边 | y=3（x=400 列：y0..2 是亮场景 (102,102,51)，y3 起 (48,40,16) 被压暗） | **−72** | `position.y() - user_view_y` |
| 底板覆盖宽度 | x=45 行：x≤549 已被压暗，x≥565 是彩色场景 | 全宽 0..565 | `dimension.x() = 566` |
| 滚动条灰轨左沿 | x=551 | −（框右沿内缩 15） | `classic_scroll_x() = dimension.x() - SCROLL_W = 551` |
| 滚动条灰轨右沿 | x=564（VScr5 滑块实测宽 15） | | `SCROLL_W = 15` |
| 滚动条轨道上下沿 | y=4 .. y=70（框顶 +1、白带顶 −5） | −71 .. −5 | 轨道取 `Range(SCROLL_INSET, H - SCROLL_INSET)`，`SCROLL_INSET = 11` |
| 上箭头高光 | y=15（白，x 555..561） | −60（= 框顶 +12） | Slider 的 `prev` 画在 `vertical.first()` |
| 下箭头高光 | y=60（白，x 556..562） | −15（= 框底 +11，即距白带顶 15） | Slider 的 `next` 画在 `vertical.second()` |
| 底板内正文 | 文字直接压在压暗场景上，无独立背景 | | 沿用 Q1 定案的 `TooltipBackdrop` |

本次截帧的历史框高 **72px**（y3..y74）。**这是用户当次的拖拽结果，不是默认值**：
原生框高可由用户上下拖，落在配置里，所以本端不能把 72 抄成常量，
默认沿用 80（`CLASSIC_LOG_H_DEFAULT`），并把拖拽结果持久化到 `chat.y`
（modern 分支早就这么存，classic 之前硬编码 80 且从不读回）。

## 3. 判读过程（点阵图，避免把箭头读成轨道）

`_expand_sb_map.txt` 把 x 546..568 × y 0..75 分类成 `W`(>200 亮) / `G`(100..200 中性灰) /
`D`(<100 暗) / `M`(彩色=未压暗的场景)，滚动条那一段读出来是：

```
  3 DDDDDMMMMMMDDDDDDDDMMMM     ← 框顶：x551 起才有灰，左侧仍是场景
  4 DDDDDMGGGGGGGGGGGGGMMMM     ← 轨道顶
 15 DDDDDGGGGWWWWWWWDGGGMMM     ← 上箭头（白）
 60 DDDDDGGGDWWWWWWWGGGGMDD     ← 下箭头（白）
 70 DMMMMGGGDDDGGGGGGGGGMMM     ← 轨道底
 71 GGGGGGGGGGGGGGGGGGGWWWW     ← 灰带/白带内沿（收起态已定版那条）
 75 WWWWWWWWWWWWWWWWWWWWWWW     ← 白输入带顶缘（锚点）
```

要点：
- 灰轨只占 **x 551..564**，两侧都是 `M`（未压暗场景）⇒ 聊天栏右沿就是 565/566，
  与 `base/chat` 位图 **566×5** 相符。旧常量 `MAX_ROW_W = 560` 比原生窄 6px，
  滚动条放进去会溢出，本次一并改成 **566**。
- 上下箭头分别在 rel −60 / −15，而框是 −72..−1 ⇒ 轨道两端各内缩约 11~12px，
  不是贴到框顶框底。本端用 `SCROLL_INSET = 11`；不内缩的话 `next` 正好压在白带顶缘上。
- 框底到白带顶之间还有 y72..74 三条 `D/G`，是灰带自己的内沿（收起态那条
  `FOLD_EDGE_TOP` 暗线），不属于历史框。

## 4. 收起态那一列的 ▲▼（本次不做，附证据留档）

`聊天框收起的时候.png` 里 x 552..564、y 10..34 有一对蓝色 ▲▼（同一列，贴在滚动条
该在的位置）。它是**状态栏**的按钮排，不是聊天历史框的控件；本端该处目前被
`UIStatusBar` 的大喇叭占位（本端按钮排整体比原生偏左约 25px）。
⇒ 这对 ▲▼ 随「底栏按钮排整体落位」一起修，混进 B5-1 会互相盖。

## 5. 本端实现映射

| 需求（用户原话） | 实现 |
| --- | --- |
| 「白色区域上方是会有对话框背景的」 | 保留 `TooltipBackdrop`（Q1 判据），底板宽 = 566、高 = `user_view_y` |
| 「右侧可以滑动」 | `Slider(Slider::Type::CHATBAR, …)`（v83 无 `chat/common/scroll`，组件内已兜底 `Basic.img/VScr5`），画在 `classic_log_position()`，正文起点按 `classic_scroll_offset` 行偏移 |
| 「背景可以上下拖拽大小」 | 顶边 `GRIP_H = 6` 命中带 → `set_classic_log_height()`，范围 `[40, 420]`，松手 `Setting<ChatViewY>::save` |
| 拖拽中不脱手 | `is_in_range()` 在 `classic_resizing` 时整体归聊天栏（否则光标一跑出框就改派给场景） |

未做（显式预留）：滚轮翻历史。`UIStateGame::send_scroll` 是**广播给所有 active 元素**，
而聊天栏常驻 active，`UIElement` 又拿不到当前光标位置 ⇒ 直接接 `send_scroll`
会把全屏滚轮都吃掉。要做得先把光标位置透传给 `send_scroll`，属于框架面改动，
本轮不动。

## 6. 真机复验（2026-11-02，出包 16:37 与 17:10 两轮，读数一致）

> 判据脚本：`expand-verify.py`（本目录），读数落 `expand-verify.txt`；
> 驱动：`E:\project\_r27_b51.py`（进图 → 点 `+` → 灌消息 → 点滚动条 → 关框再开 → 拖顶边 → 退出 → 重启）。
> 全部坐标是**视图坐标**（截帧已减掉窗口装饰 `(8, 31)`）。

| 锚点 | 实测 |
| --- | --- |
| 白输入带顶缘（锚点行） | 视图 y=653 |
| 收起态灰带 | y 656..678，高 23 |
| 灰带内墨迹 | 顶 y=662（锚点 +9）、左沿 x=6 |
| 灰带右沿 | x=550（551 起是滚动条列，与 §3 一致） |
| 蓝色 `+` | 左沿 x=537 |
| 灰带上沿暗线 | 实测 `(87, 87, 95)` == 原生 |
| 展开态底板顶边 | y=573 == 653 − 80（偏差 0） |
| 底板压暗 | 正文列平均亮度 收起 29.7 → 展开 4.3 |
| 滚动条列 553..564 | 亮度 127.4，同帧正文（400..540）2.7 ⇒ 轨道只铺在那一列 |
| 灌消息后滑块 | 轨道列逐行差 >8 的行数 = 37（127.4 → 145.0） |
| 点滚动条 | 框顶仍 y=573（几何不动）、正文签名 42/63 行改变 |
| 关框再开 | 63/63 行回到滚动前（回到最新一条） |
| 顶边拖 60px | 框顶 573 → 513（期望 513，偏差 0），滚动条列跟着拉长 |

⇒ F1–F7 / P1–P2 / S1–S4 / C1–C3 / R1–R2 共 **20 项 PASS**；
唯一未过的是 **R3「拖完的高度落盘进 `chat.y`」**：拖拽当时
`Setting<ChatViewY>::get().load()` 读回 140，`Settings` 里却仍是 `chat.y: 0`。

### 6.1 R3 定性：退出路径跑不到 `~Configuration`（durable 缺陷，本轮绕开）

先排除掉的可能：
- `ChatViewY` 没登记 —— `Configuration.cpp:203` 有 `settings.emplace<ChatViewY>()`，
  写盘模板 `SETTINGS_ORDER` 里 `聊天栏 / chat.y` 也在（`Configuration.cpp:104`）。
- `find_value` 撞名 —— 全仓只有一处 `"chat.y"` 字面量是 Entry 名（`Configuration.h:619`）。
- `xy-config.yaml` 覆盖 —— 该文件没有 `chat` 相关键。
- 写盘被截断 —— `Settings` 每次都是完整写出（结尾就是 `SETTINGS_ORDER` 的最后一项
  `ui.position.memo`）。

**定案用的取证**（临时追踪，闭环时已删除）：在 `Configuration::save()` 入口/结尾与
`~Configuration` 各加一条**即时 flush** 的行，聊天栏侧的 `resize end / saved=` 写进同一
个文件按 `t=` 排时间轴，跑两个进程各一次完整「拖拽 → 优雅退出」，得到：

```
save#1 t=… chat.y=0        ← 登录流程里显式的 Configuration::get().save()
save#1 done
save#2 t=… chat.y=0
save#2 done
chatbar t=… grip start y=576 h0=80
chatbar t=… resizing y=570 h=86 … resizing y=516 h=140
chatbar t=… resize end h=140
chatbar t=… saved=140
save#3 t=… chat.y=140      ← 本轮新增：松手即存
save#3 done
（此后进程消失，始终没有 dtor 行）
```

两个进程、四次退出（含最后单独复验的一次）都没有出现过 `dtor`，而退出后进程也确实没了
（`tasklist` 里没有残留）⇒ **优雅退出路径上 `~Configuration` 根本没被跑到**：
关窗 → 退出确认「确定」→ `UI::quit()` 置位 → `loop()` 退出 → 窗口销毁 → 进程结束，
静态析构没有轮到 `Configuration` 那一份（现场还留下过一个「窗口已消失、进程还在、
内存只剩 32K」的僵住实例，与「teardown 阶段卡/死在某个更早的析构里」一致）。

⇒ 影响面不止聊天栏：**所有「指望退出时自动落盘」的设置都是死的**
（`ui.position.*` 39 项窗口坐标、`chat.max`、音量等）。
本轮只按 B5-1 的验收线把 `chat.y` 在**松手那一刻显式 `Configuration::get().save()`**，
使「拖完记住」成立（R3/R4 已 PASS）；**通用修法（退出流程里显式 save，或查明卡住的那支
析构）属另一条缺陷**，未动的理由：一旦窗口坐标真的开始持久化，`Settings` 里历史上的
脏值（见 `notes/open-issues.md` 的同名提醒）会立刻接管布局，需要先做一次逐键比对，
须用户点头后再排期。

### 6.2 最终复验（**去掉全部临时追踪后**的出包，17:10 那一版）

驱动与判据各重跑一遍（跑前把 `Settings` 的 `chat.y` 复位成 0，确保是从默认 80 高拖起）：

| | 结果 |
| --- | --- |
| `chat.y before quit`（松手即存，未等退出） | **140** |
| `chat.y after quit` | **140** |
| 重启后展开态框顶 | y=513 == 653 − 140（偏差 0），帧 `live-b51-08-restored.png` |
| 21 项 oracle（F1–F7 / P1–P2 / S1–S4 / C1–C3 / R1–R4） | **TOTAL 0 FAIL**（读数见 `expand-verify.txt`） |


