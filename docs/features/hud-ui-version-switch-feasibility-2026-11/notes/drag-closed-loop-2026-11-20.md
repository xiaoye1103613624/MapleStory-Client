# 拖拽闭环攻关 + 部署时序（2026-11-20 01:2x-01:37）

## 一、本次会话完成度（对照用户五条 + 拖拽）

| 需求 | 状态 | 证据 |
|---|---|---|
| ① 快捷键用 `quickSlot/backgrnd` + 截取控制长度 | ✅ | 底板 557×67，`Range` 按整列裁 |
| ② 血量条**中轴**居中 | ✅ | `status_x = (VWIDTH-198)/2`，截图 `_v33_game.png` 血条居中 |
| ③ 主功能菜单移到**等级上方** | ✅ | `menu_y = EXP_BOTTOM-STATUS_H-MENU_H-6`，六钮在血条上方 |
| ④ 右侧格子**截断完整**（不半格） | ✅ | `qs_cols` 按整列算，底板裁到末格右缘 |
| ⑤ 键位映射参照 083 13 格 + 延续 | ✅ | IDA `s_ptShortKeyPos` 28 格实表 + 083 26 键序续 NUM6/NUM7 |
| ⑥ 底部自适应 / 过宽居中 | ⚠️ 分布式 | 聊天贴左 + 血条居中 + QS 贴右；未做统一容器 |
| ⑦ 图标**拖入/抓出** | ⚠️ 半通 | 起拖已修好（trace 见 `SETUP slot=0`），**drop 环节未闭环** |

## 二、拖拽链路三个真实 bug（逐层挖出）

### Bug A：`clicked` 参数语义误判（起拖永不触发）
- 旧代码把每次 `send_cursor(clicked=true)` 当"新按下"，拖动经过每格都重设 `qs_drag_slot`：
  trace 铁证 `at=(821)slot=0 drag_slot=0` → `at=(854)slot=1 **drag_slot=1**`（起点被覆盖）→ `dragging` 恒 0。
- 修：加 `qs_pressed` 边沿标记 + 起拖检测前置。

### Bug B：`clicked` 在按住期间会变 0
- trace 铁证：`clicked=1(821) → clicked=1(825) → clicked=0(830)`，鼠标未松开。
- 说明 `clicked` 语义是"本次是否 down 帧"，不是"左键是否按住"。把 false 当松开会在移动帧清状态。
- 修：**起拖检测放在最前，不依赖 clicked 真假**。

### Bug C：`qs_press_pos` 每帧被刷新 → delta 永远 < 阈值
- trace 铁证：`drag_slot=0` 稳定，但 `dragging` 恒 0；因为每帧 `qs_press_pos = cursor`，
  delta 只剩相邻两帧间距（~5px）< 阈值 8px。
- 修：**同格内绝不刷新起点**，只在新格（真正 new down）时设置。
- 修后 trace 出现 `SETUP slot=0 at=(830,660)` —— **起拖成功触发**。

### 剩余缺口：drop 未闭环
- `UIStateGame::send_cursor` 只在 `cursor_state == IDLE` 时 `drop_icon`；
  而 `UIStatusBar::send_cursor` 拖拽中返回 `CANCLICK`，且松手帧同样返回 `CANCLICK`。
- 需让拖拽中返回 `GRABBING`、松手帧返回 `IDLE`，才能触发 `drop_icon → front->send_icon`。
- 另：自动化鼠标模拟在部分启动批次里 down 帧丢失（trace 全 `clicked=0`），
  导致无法稳定复现，需人工点一次确认。

## 三、部署时序（重要环境约束）

**现象**：部署后 8 秒 SHA 从 `e775f9fd` 变 `01a9dcf5`（回滚）。
**结论**：启动器在**启动时**检查并替换 exe；只要 exe 未被运行锁定就会换。
**正确流程**：`cp → 立即 start → 8s/53s 两次复查 SHA`。
本轮按此流程执行，`d050d419` 三次核对（即时/8s/53s）均未变，客户端 PID 22848 正常。

## 四、验证
- BUILD_EXITCODE=0（清理全部 `[DBG-*]` 垫片后）。
- live SHA `d050d41980c63848b59371a7…`，即时/8s/53s 三次一致。
- 截图 `tools/_v33_game.png`：聊天左 · 血条居中 · 菜单在等级上方 · 快捷键贴右 · 经验条贴底。

## 五、教训（防复发）
1. `send_cursor(bool clicked)` 的 `clicked` 不是"按住状态"，是 down 帧标记，且按住期间会回 0。
2. 拖拽状态机必须：**先判起拖 → 再处理 down → 最后处理 up**，且起点只在 new down 时记录。
3. QS 槽位数组必须按 `max(CLASSIC_QS_SLOTS, MODERN_QS_SLOTS)` 分配（本轮因 28>26 越界把
   `quickslot_pos` 写成 0，快捷键画到屏幕左上）。
4. 部署后必须"立即启动 + 8s 复查"，否则会被启动器回滚。
