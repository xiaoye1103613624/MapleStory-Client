# 登录链对照矩阵 · 2026-11-12

权威：Angel IDA。状态：`OK` / `FIX` / `CHECK` / `BLOCK`。

| Step | 页 | Angel EA / 常数 | 本端 | 定位 | 动画 | 按钮 | 功能 | 本轮动作 |
|------|----|-----------------|------|------|------|------|------|----------|
| 0 | 登录 | `CUITitle::OnCreate 0x62054A`；账号(67,12)/密码(67,40) | `UILogin.cpp` | OK | — | OK | OK | 字色→WHITE（2026-11-12） |
| 1a | 选世界 | `CUIWorldSelect 0x6246CE` `(-140,-804)`；槽步进 27 | `UIWorldSelect.cpp` | FIX→OK | — | OK | OK | `worldsrc_pos=viewpos+(-140,-804)` |
| 1b | 选频道 | `CUIChannelSelect 0x628501` `(-176,-688)`；卷轴 `0x629348` `(-38,-141)`；BtGoWorld(320,217) | 同上合并 | FIX→OK | FIX→OK | FIX→OK | OK | Angel 窗锚；弃截图Δ |
| 2 | 选角 | `CUICharSelect`；详情/三钮 | `UICharSelect.cpp` | OK | OK | OK | FIX→OK | 进图 AV：`Job::get_name` 悬空引用 |
| 3 | 职业团 | `CUINewCharRaceSelect 0x615596` `(-372,-2128)` 700×460 | `UIRaceSelect.cpp` | OK | OK | OK | OK | 与 IDA 一致，仅注释口径 |
| 3+ | 建角×3 | `CUINewCharNameSelect` 等 | `UICommonCreation` + 子类 | OK | OK | OK | OK | classic 命名态；结构与独立 NameSelect 类不同但功能闭环 |

## 频道锚点公式（落地）

```text
viewpos = LoginBackground::get_target_viewpos()   # step=1
channel_wnd_tl = viewpos + (-176, -688)
scroll_tl      = channel_wnd_tl + (-38, -141)     # = viewpos + (-214, -829)
channelsrc_pos = channel_wnd_tl + (CH_PANEL_W/2, CH_PANEL_H/2)
BtGoWorld_tl   = channel_wnd_tl + (320, 217)
worldsrc_pos   = viewpos + (-140, -804)
```

## IMG

`Login.img` / `Map/Obj/login.img` orange-wz 本环境曾 `IMG 解析失败` → 坐标以 IDA + 既有 nx/runtime 为准。
