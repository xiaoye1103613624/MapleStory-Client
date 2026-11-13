# W02 HUD/statusbar — 执行记录

## 目标

状态栏 / 聊天 / 快捷栏 / 组队血条等按 Angel 语义；保留 modern 皮肤与多分辨率（R1）。

## 已有证据

- [statusbar-hud-angel-calibration-2026-11](../../statusbar-hud-angel-calibration-2026-11/README.md) §7：P0/P1 已于 2026-11-10 实施并复验  
- IDA r02：[ida…/notes/r02-statusbar_hud-facts.md](../../ida-v83-083-v83-i64-inventory-2026-11/notes/r02-statusbar_hud-facts.md)

## 本波代码改动

**本波无新增代码差异。**

源码核对：`activate_classic_qs_slot` / `change_target` / `UITemporaryStatView` / 组队血条 / 按钮闪烁 / IME unicode 均已存在。旧 feature-gap「快捷栏无点击」「TemporaryStatView 全缺」已过期。

## 阻塞

- 原生 IME 候选窗状态机：保持系统 IME（R3），不重建 `CCtrlMLEdit`  
- 联机未在本轮复跑  

## 回滚

无本波新增 diff。
