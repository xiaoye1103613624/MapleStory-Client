# R27-B7-4 最终实现逻辑：目录气球 → 游戏设置 / 系统设置 二分 + CheckBox

用户报的缺陷原话：

> 游戏设置和系统设置 怎么都是打开游戏设置窗口而且勾选框也没有？

两个症状分别是**路由**和**控件真身**，分开结案。

---

## 1. 「都打开同一个窗」的根因

本包（v83 资产）**没有 `StatusBar3.img/OptionMenu`**，原生那一张「游戏设置 / 系统设置 共用一个大窗」的
`backgrnd` 在 v83 已经拆成两张独立画布：

| 节点 | 实测尺寸 | 烘焙内容 |
| --- | --- | --- |
| `UIWindow.img/GameOpt/backgrnd` | **158 × 284** | 标题「游戏设置」+ 12 行粉色标签 + 12 个「同意」字样；**无勾选框、无按钮** |
| `UIWindow.img/SysOpt/backgrnd` | **299 × 365** | 标题「系统设置」+ 5 行复选 + 6 条滑条 + 2 个下拉；**无按钮** |

旧代码只有 `UIOptionMenu` 一个类型、一个窗体，两颗目录钮都 `emplace<UIOptionMenu>()`，
所以点哪颗都是同一张脸。

**处置**：`UIElement::Type` 新增 `GAMEOPT` / `SYSOPT` 两型，`UIGameOpt` / `UISysOpt` 各自
`UIDragElement<PosGAMEOPT>` / `<PosSYsopt>`，目录气球的两颗钮分别 emplace。

> ⚠ 这两项让 `UIElement::Type::NUM_TYPES` 变大，而 `EnumMap<Type, UPtr, NUM_TYPES>` 是
> `UIStateGame` 的成员 ⇒ **实例尺寸变了，必须整工程 `-t:Rebuild`**（增量会留下按旧尺寸编译的目标文件，
> 表现是登录阶段 100% 访问违例而构建 0 error）。

## 2. 「勾选框也没有」的根因

`GameOpt/backgrnd` 里烘焙的只有**文字**，勾选框本体是另一族资产：

```
UIWindow.img/Basic.img/CheckBox/0   12 × 12   空框
UIWindow.img/Basic.img/CheckBox/1   12 × 12   白框 + 红勾
```

旧实现把勾选状态画成 `Text`/`ColorBox` 的字符，v83 资产齐了以后应换成位图。
现在每行一个 `CheckBox` 纹理，`get_state()` 决定取 `/0` 还是 `/1`，命中区 = **12×12 AreaButton**
（不是整行；原生就是只有方框可点）。

`UIGameOpt` 12 行（寻找队伍 / 组队 / 家族 / 好友 … 邀请同意位）、`UISysOpt` 5 行
（静音 BGM / 静音 SFX / 技能特效 / 窗口模式 / Alt-Enter）同用这一族。

## 3. 「确定 / 取消」为什么曾经不出现（本轮踩的坑，值得记住）

`MapleButton` 吃的是**按钮根节点**，内部自己取 `normal` / `pressed` / `mouseOver` / `disabled`
（[IO/Components/MapleButton.cpp:25-71](../../../../../IO/Components/MapleButton.cpp)）：

```cpp
// 错：传叶子 —— 四态全空，画不出任何东西，而且 usable() 仍为真，连 diag 都不报
Basic["BtOK2"]["normal"][0]
// 对：传根
Basic["BtOK2"]
```

`UiAssetResolve::usable()` 判的是「这个节点像不像一张位图」，叶子恰好像，所以**静默失败**。
`try_maple_btn(...)` 的 `position` 语义是**贴图左上角**（它会把 normal 态 origin 加回去），
而 `MapleButton` 内部 `draw = pos - origin` —— 两个符号相反，实测格位必须按左上角给。

落位（`UIGameOpt`，窗体 origin `(170, 24)`）：
`CHECK_X=130 / CHECK_TOP=34 / CHECK_PITCH=18 / BTN_Y=262 / OK_X=34 / CANCEL_X=84`。

## 4. ESC 关不掉设置窗

`IO/UI.cpp::send_key` 的 ESC 分支是一份**显式 `types` 白名单**，不在名单里的窗体永远收不到 ESC。
新增的 `GAMEOPT` / `SYSOPT` 必须补进去，否则 ESC 直接落到 `MAINMENU`，把「目录」气球盖在还开着的设置窗上。

## 5. 滑条

`UISysOpt` 六条滑条（画质 / BGM / SFX / 鼠标速度 / HP 警戒 / MP 警戒）用
`SettingsSlider`：轨道取 `row_y + 1`、滑块取 `row_y`（1px 差是逐格量出来的）。
拖动由 `send_cursor` **自持**——命中即吞掉本次光标，不再走 `Button` 的「悬浮→点击」两态机，
否则拖动过程中会被 NORMAL/MOUSEOVER 来回打断。

## 6. 存档

`Setting<T>::get().save(v)` **只改内存**，必须紧跟 `Configuration::get().save()` 才落盘。
`commit()` 在「确定」里跑；音量两轨额外立刻调 `Music::set_bgmvolume` / `Sound::set_sfxvolume`，
与 `UI::send_focus` 走同一对入口。

## 7. 真机验收（判据 = 用户会看的那一档，不是「元素在位」）

对照原生 oracle（`_dump/native_gameopt.png` 156×283、`_dump/native_sysopt.png` 298×365；
原生帧与客户端帧都按 `view = 帧坐标 − (8, 31)` 对齐后逐格比）：

| 项 | 结果 |
| --- | --- |
| UIGameOpt 整窗 SAD | **1.45** |
| UISysOpt 整窗 SAD | **5.17** |
| 两颗目录钮分别开两窗 | PASS（不再同脸） |
| 勾选框位图 | PASS（第 6 行「寻找队伍」点掉后是空框，与 oracle 一致） |
| 确定 / 取消 | PASS（两颗都在，确定点击后关窗，窗区 SAD 回到 108） |
| ESC 关设置窗 | PASS |

## 8. 显式预留（不是静默半成品）

| # | 预留项 | 原因 / 接线点 |
| --- | --- | --- |
| R1 | `UIGameOpt` 12 个同意位**不拦截邀请** | 值已落盘、`accepts(Option)` 可查；回包收发在 [Net/Handlers/PlayerHandlers.cpp](../../../../../Net/Handlers/PlayerHandlers.cpp)，本轮由并行会话持有（禁改清单）。接线只需在对应 `*_REQ` handler 里查一次并回绝包，UI 侧不再动 |
| R2 | `UISysOpt` 两个下拉（截图路径 / 怪物信息）**只有收起态** | 整块 83×17 作命中区，点击只出 `LOG(LOG_INFO, ...)`；展开列表未实现。另：`MapleComboBox` 命中区本身有 bug，见 [notes/combo-box-hitbug.md](../../notes/combo-box-hitbug.md) |
| R3 | 画质 / 鼠标速度 / 窗口模式 / Alt-Enter / HP·MP 警戒 / 怪物信息 / 截图路径 **无运行时消费方** | 全仓检索 `SysOpt*` 只有 `Configuration.cpp:225-233` 注册 + `UIOptionMenu` 读写，没有任何渲染/输入/告警代码读它们。本版只保证「能改、能存、能查」，效果接线属后续批次 |
| R4 | 底栏那颗旧设置钮仍开空壳 `UIOptionMenu` | 落点 [IO/UITypes/UIStatusBar.cpp:969](../../../../../IO/UITypes/UIStatusBar.cpp)，该文件由并行会话持有，本轮不改 |
