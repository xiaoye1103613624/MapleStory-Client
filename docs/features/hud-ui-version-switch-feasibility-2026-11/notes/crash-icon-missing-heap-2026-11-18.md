# 启动闪退：缺 Icon.png → MISSING_ICON → 退出析构 0xC0000374

日期：2026-11-18

## 现象

Locale 退出 AV（0xC0000005）修好后，正常启动约数秒仍报 `0xC0000374`（ntdll 堆损坏）。`--resource-probe` 可过；隔离 StatusBar3 / 关 auto / 关 nxcache 仍崩。

## 证据

`_init_debug.log`（本轮加的启动面包屑）：

```text
window:FAIL Could not load icon.
start:init FAILED Could not load icon.can't fopen
start:return
WinMain exit
```

随后 Application Error `0xc0000374`。`boot_perf` 仅有 `BOOT window=…` 是因为 `BootTimer` 在 `return error` 时仍会析构打点，**不能**当成 window 成功。

根因：`IO/Window.cpp` 用 `SDL_GetBasePath()+Icon.png`；live `E:\MXD\BeiDou-Client-xy` **没有**该文件。Windows 路径把缺图标当成致命错误返回，此时 SDL 窗口/GL 已半初始化；`WinMain` 退出后静态析构触发堆损坏。

## 修复

1. 缺 `Icon.png` 改为与 Android 一致：打 WARN 并跳过，不中止启动。
2. 将仓库 `Icon.png` 同步到 live / OutDir / min_probe。
3. 顺带修 `WeatherFx.cpp` 残留 `PaletteColors`→`TintRamp`（否则无法链出新 exe）。

## 验收

- live 短启 ≥20s：`init:done` → `loop:enter`，无 Application Error。
- `login.auto=true`、`nxcache=true`、StatusBar3 在盘均可。
