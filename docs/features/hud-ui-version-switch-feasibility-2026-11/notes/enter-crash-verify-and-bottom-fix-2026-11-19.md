# 进图闪退复验 + 底部UI三修（2026-11-19 23:xx）

## 一、进图闪退：已消失（另一会话修复生效，我复验）
- 现状：`UIStateGame` 构造加临时面包屑 `_enter_trace.log`（现已删），完整跑到 `ctor:done`；
  窗口标题变 `【1-Admin 132-静之骑士】`，进程稳定 496MB、FPS:60，**进图成功无闪退**。
- WER 铁证（此前 14:46/14:49/14:52 三份构建）：异常码 `c0000005`、模块 MapleStory.exe、
  偏移 `0x261695/0x261697`（VA `0x140261697`，imagebase 0x140000000）。
  反汇编定位到 `UI::open` 大类分发里 `mov rcx,[140491188h]`（Singleton<UI>，全文引用 1415 次）
  后接 `mov rax,[rcx]` / `call [rax+0B0h]` / `jne is_active·makeactive` / `emplace<T>`。
- 另一会话已修的真崩溃点：`UIStatusBar.h:265-271` 注释坐实——经典快捷栏数组原按
  `MODERN_QS_SLOTS(20)` 分配，而 `refresh_classic_qs_icons()` 遍历 `0..CLASSIC_QS_SLOTS-1`(26)
  → i≥20 越界写 0xC0000005。现数组已按 `CLASSIC_QS_SLOTS` 分配
  （`classic_qs_names/counts[CLASSIC_QS_SLOTS]`、`classic_qs_icons[CLASSIC_QS_SLOTS]`），崩溃消除。

## 二、用户四条修复（本轮）
1. **底栏贴经验条**：`QS_H` 由 `quickSlot/layer:cover`(562×73，含外扩投影) 改为
   `quickSlot/backgrnd`(557×67) 实测高 —— 用 73 会把底板连带键帽整体上移 6px 顶出经验条
   （用户报「键帽飘在上方」真因）；`position.y = VHEIGHT-111` 使经验条底落视口底。
2. **角色名垂直居中**：`Text::draw(pos)` 的 `pos.y` 是**行顶**（含 ink_offset 上空白），
   必须经 `y_for_ink_top`。改为 `namelabel.y_for_ink_top(levelset_pos.y())` —— 与「等级 N」
   同一墨迹顶（图1「等级200 yyy天堂2」同排）。此前直接用 y 或双重偏移都会下沉压血条。
3. **快捷键拖拽（道具/按键/技能）**：根因 `send_icon` 原守卫 `!classic_mode` 直接 return false
   → modern(192) 下快捷格永远拒绝拖放。已改：
   - 新增 `modern_qs_slot_at()`（格 32×32 命中，与 draw 同几何）
   - `send_icon` 双路：classic/`modern_qs_slot_at` + `MODERN_QS_KEYS` 取键位
   - `send_cursor` 双路：classic/modern 共用「单击使用 + 拖出换位」状态机
   - 新增 `activate_modern_qs_slot` / `release_modern_qs_slot`（keydown→keyup 配对）
   - `setup_qs_drag_icon` 双路：格原点 `classic_qs_cell_at` / `modern_qs_cell_at`
4. **图标居中**：整格居中 `cell + (32-dim)/2`（origin 抵消），键帽压图标左上角与图1一致；
   回退此前 CAP_H 下移方案（会让图标出格）。

## 三、验证（实测）
- BUILD_EXITCODE=0；live SHA `56d95d8d7a205f5f3be2e374214e9126…` 与产物一致（实时核对）。
- 截图 tools/_v16_game.png / _v16_status.png / _v16_bottom.png：进图正常、名字与等级同排居中、
  快捷键 2×10 键帽在格顶。
- 拖拽代码已在位（modern_qs_slot_at :2349、MODERN_QS_KEYS 取键 :2830、状态机 :2866-2871），
  但**真机拖放动作未逐次点验**（需用户在格子上拖一次确认）。

## 四、仍未完成（如实）
- **展开UI（用户图4 的 62×100 竖条）**：实测 192 分卷 `UIWindow5.img/premiumQuickSlotMini`(121×45)、
  `UIWindow2.img/SlideMenu`(284×217)、`StatusBar3.img/mainBar/quickSlot/button:Fold|Extend`(12/13×71)
  均不匹配 62×100。未实现，需用户指认资源或给 083 参考。
- **聊天框 UI 与图1 差异**：未逐像素对齐（本轮未改动 UIChatBar）。
- **底栏仍未 100% 贴视口底**：截图经验条下仍有细微留白，需再校 EXPBar backgrnd 实际绘制高。
