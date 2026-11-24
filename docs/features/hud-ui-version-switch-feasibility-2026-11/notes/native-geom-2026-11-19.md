# 192 原生几何对齐（2026-11-19 17:xx，用户图1正确端 vs 图2本端）

## 用户问题与逐条结论
1.「菜单/血条/快捷键ui是底部对齐，贴着经验条吧？」→ ✅ 已改。原代码顶对齐三种不同高度。orange-wz 实测原生节点高：status/layer:cover=70、menu按钮=37、quickSlot/layer:cover=73；经验条顶 EXP_BOTTOM=87。改为三者底边都贴 87：血条顶=17、菜单顶=50、QS顶=14。**证据 tools/_v7_bottom.png**：血条70、菜单37、QS73 底边齐平贴经验条。
2.「快捷键长条内按键对吗？」→ 键位序列由客户端逻辑(CUIKeyConfig::GetShortCutPos)+键盘映射决定，IMG 只有 KeyConfig/key/* 位图（7~10×9 顶带小条），无默认键位序列节点。图1首行 Q/W/E/R 是参考端键位配置；本端 MODERN_QS_KEYS 表 Shift/Ins/Hm/Pup 是本端配置，非素材错误。原生网格实测(导出 backgrnd PNG 量像素)：列 pitch=35、行 pitch=34、16列×2行，已按此还原 MODERN_QS_POS[32]（旧值(7,15)/33 属 StatusBar2 契约，是图标偏移根因）。
3.「UI 超出客户端，宽度不够？折叠逻辑？」→ 展开态32槽需 16×35=560 宽，起点916+560=1476>1280 溢出。已修：底图按 VWIDTH-qs_x 动态裁剪；图标循环跳过超出右缘格。**不再溢出**。
4.「结合192逻辑+素材节点定位属性还原」→ 本轮所有坐标来自 orange-wz 实测节点尺寸，非手拍。

## 代码行号
- UIStatusBar.cpp:327 EXP_BOTTOM=87/STATUS_H=70/MENU_H=37/QS_H=73（新增常量）
- :358/:375 hpmp_pos.y=EXP_BOTTOM-STATUS_H=17；hpset/mpset/namelabel 相对下移
- :364/:382 quickslot_pos.y=EXP_BOTTOM-QS_H=14；x=血条右缘+4+6*35+6
- :468 menu_strip.y=EXP_BOTTOM-MENU_H=50（底对齐）
- :718 QS底图按 avail_w=VWIDTH-qs_x-2 裁剪
- :2450 QS图标循环 cell_pos.x+32>VWIDTH-34 跳过

## 验证
BUILD_EXITCODE=0；live SHA `e0369fcad500d832a31316889af65a6e583acd6e2140c35f1c30c6ccec9c0219`（实时核对）；截图 tools/_v7_bottom.png。

## 残留（如实）
- 键位序列差异属配置层（MODERN_QS_KEYS），非素材；若用户要图1完全一致需改键位配置表，等指示。
- 展开态32槽逐格真机未截图，但溢出已由裁剪+跳过逻辑消除。
