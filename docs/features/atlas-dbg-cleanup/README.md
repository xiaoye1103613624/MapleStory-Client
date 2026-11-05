# atlas_dbg Release 落盘清理

> 状态：已闭环（工程债）  
> 日期：2026-11-06  
> 产物：`docs/features/atlas-dbg-cleanup/`

## 1. 背景

`Graphics/GraphicsGL.cpp` 的 `dbgatlas()` 在 Windows Release 链路仍向 exe 同目录追加
`atlas_dbg.log`（限 200 条），源自登录页竖条纹/错贴联调。功能已闭环，属临时调试残留
（parity-gap §4 / r810 §5 / CHECKLIST 收尾欠账）。

**非 B11**：B11 是 `_ui_asset_diag.log`（需用户点头）；本项是无开关的 `atlas_dbg.log`，可直接删。

## 2. 改动

- 删除 `dbgatlas()` 及 `stale-offset` / `null-offset-draw` 两处调用
- 去掉仅为其引入的 `<fstream>`
- 陈旧图集自愈逻辑保留（尺寸不符仍 erase 重传），仅不再落文件

## 3. 验证

- Release|x64：`tools/build_smoke.bat` GATE
- 运行后 exe 旁不应再生成 / 追加 `atlas_dbg.log`
