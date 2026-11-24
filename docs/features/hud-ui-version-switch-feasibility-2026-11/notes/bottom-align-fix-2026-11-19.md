# 底栏贴底/名字居中/图标避让（2026-11-19 21:xx，用户图2正确端对照）

## 用户四条反馈 → 逐条处理（行号级）
1. **底栏悬空（图2 应是贴经验条/贴视口底）**
   - 根因：`UIStatusBar.cpp` init_modern 各分辨率分支 `position.y = VHEIGHT - 120`，经验条高 24、exp_pos.y=87 → 经验条底 = VHEIGHT-120+87+24 = **VHEIGHT-9**，离视口底 9px = 悬空。
   - 修：5 处 `position.y` → `VHEIGHT - 111`（:580/587/594/601/608），经验条底正好落 VHEIGHT。血条(70)/菜单(37)/QS(73) 相对 EXP_BOTTOM=87 的底对齐关系不变。
2. **快捷键键位数/映射/展开UI/图标偏左上**
   - 键位表：上轮已改 `MODERN_QS_KEYS[20]` = 083 `CLASSIC_QS_KEYS[26]` 前20（:152-155），并去掉前8槽玩家绑定优先（:2385/:2460）。
   - 图标偏左上根因：`draw_modern_quickslot` 原 `draw(center+org)` 让 32×32 图标占满整格、与格顶键帽(CAP_TOP)重叠。
   - 修：图标在「键帽下方剩余区」居中 —— `CAP_H=9`，`lt = cell + ((32-dim)/2, CAP_H + (32-9-dim)/2)`，`draw(lt+org)`（:2444-2460 附近）。
   - **展开UI（图4 62×100 竖条）未实现**：实测 192 分卷 `UI_005.wz/UIWindow5.img/premiumQuickSlotMini`(121×45)、`UI_004.wz/UIWindow2.img/SlideMenu`(284×217)、`UI_002.wz/StatusBar3.img/mainBar/quickSlot/button:Fold|Extend`(12×71)。图4 62×100 与上述均不完全对应，需用户指认或再取证；当前仍用 quickSlot Fold/Extend 竖钮贴 QS 左缘。
3. **角色名应在等级后灰色区垂直居中**
   - 根因：`namelabel.draw(position + namelabel_pos)`（:708）直接用 y=hpmp_pos.y()+6（贴顶），未居中。
   - 修：用 `y_for_ink_top` 把 A13M 墨迹顶锚到 `hpmp_pos.y() + (37-13)/2`（背景 mainBar/status/backgrnd 高37），即背景垂直中心。
4. **键位数与图2不一样**
   - 当前 20 槽（2行10列）；图2 正确端观感同为 2×10，但键位字符序列已按 083 前20。若仍有差异需用户标注具体格位。

## 验证状态（如实）
- BUILD_EXITCODE=0（三处改动编译通过）。
- **真机截图未完成**：部署后客户端启动进入 `loop:enter`（_init_debug.log 203701572），但进程随即消失；4 个僵尸 MapleStory 进程（PID 26292/36840/42860/10856，各仅 28-36K 内存，Access denied 杀不掉）阻塞新实例存活。**需用户手动结束这些僵尸进程**（任务管理器以管理员身份）后再由我启动截图验收。
- live 当前 SHA `8cc51e6b4ee7cc662e811f582c36227b00b6a361150d586d7d76b66dada263b2`（本轮部署，实时核对）。

## 未完成/待决
- 图4 展开UI 的 62×100 资源归属未定（premiumQuickSlotMini/SlideMenu/FoldExtend 均不匹配），需用户指认或提供 083 参考。
- 真机验收阻塞于僵尸进程。
