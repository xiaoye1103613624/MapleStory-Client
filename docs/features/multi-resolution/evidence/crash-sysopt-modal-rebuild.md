# Crash fix: SysOpt resolution freeze → 0xC0000374

- Dump: MapleStory.exe.23560.dmp @ 2026-11-12 11:24 — exception `0xC0000374` (heap corruption), same family as prior SDL resize feedback crash.
- Root cause: SysOpt OK applied `Constants` while the modal was still in the UI lifetime; next frame `initwindow` + StatusBar rebuild raced under residual SysOpt state (plugin known hang: switch-under-modal + StatusBar recreate). HudLayout broadcast-to-all + full StatusBar recreate amplified allocation pressure.
- Fix: queue viewsize → deactivate SysOpt → `Window::check_events` applies then `initwindow` → `UIStateGame` rebuilds/reanchors; classic same-QS-tier only `update_screen`; icon load once.
- Build: Release x64 → build-rebuild\MapleStory.exe (2026-11-12 12:05:13)
