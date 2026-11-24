# Plan 落盘（已确认 1A / 2A）

详见 Cursor Plan `side_menu_registry_eb885fe2` 与本目录 README。

补充拍板：folder/leaf 均支持 `order`/`sort_order`；Web 拖拽与 JSON、竖栏 root、flyout 子项一律按 order 排序。

实现状态（2026-11-23）：

- [x] FeatureRegistry + SideMenuConfig JSON + order
- [x] UISideIconBar 树 + 右侧 flyout
- [x] LocalFrameButton iconKey 序列帧
- [x] ini 迁移 + 种子 side_menu.json + frames
- [x] SIDE_MENU_SYNC 0x3743 客户端收包
- [ ] 服务端广播 + Web 管理页（本仓库无 admin；草案在 notes/）
