# 多分辨率自适应重构 + IDA 逻辑复用（2026-11-20 01:xx）

## 一、用户原话需求（五条）
1. 快捷键是否用 `StatusBar3.img/mainBar/quickSlot/backgrnd`，靠截取控制显示长度？
2. 血量条位于底部**中心**（血条 UI 中轴居中，不是左缘居中）
3. 主功能菜单移到**等级上方**
4. 多分辨率切换时快捷键**右侧格子截断完整**（不能半格）；宽度不够可超出、够则全显示
5. 按键映射参照 **083 的 13 格**，多余格子按规律往后延续
6. 快捷键支持键位/技能/药水图标**拖入、抓出**的原生逻辑
7. 底部 UI（聊天组合框/信息框/主功能菜单/快捷键）**自适应宽度**，过宽时**居中**

## 二、IDA 复用取证（MapleStoryT.exe，13339）
| 证据 | 地址 | 结论 |
|---|---|---|
| `CQuickSlot::GetPosByIndex` | `0x16968f0` | `if (nIdx < m_nQuickSlotDisplayNo) { *x=s_ptShortKeyPos[nIdx].x; *y=...y }` |
| `CQuickSlot::ChangeScreenResolution` | `0x169b710` | 800×600/1024×768 → `m_nQuickSlotDisplayNo=8`；**其余→28** |
| `s_ptShortKeyPos` | `0x279c390`（28 项） | 列 x=7,40,73,106│150,183,216,249│282,315,348,381│414,447；行 y=15,48。**列距33、行距33、4列一组、组间gap11** |
| `_ResetChatBarPos` | `0x169fc70` | `RelMove(nX:0, nY:m_ptChatWnd.y-2)` → 聊天贴左 |
| `CQuickSlot::Toggle` | `0x16972a0` | `RelMove(m_nInitY±67)` 整层滑动（已复用） |
| `CUIStatusBar::ChangeScreenResolution` | `0x16a1fe0` | 仅转调 QS + 镜像层，**无**居中/菜单坐标 |
| `CUIStatusBar::OnCreate` | `0x16be4e0` | 86KB，反编译输出被截断，未取全 |

**有可复用的**：28 格坐标表、格数 8/28 规则、聊天贴左、折叠滑动。
**没有的**（我自研设计）：血条中轴居中、菜单在等级上方、整体宽度自适应居中、按格对齐截断。

反编译留证：`tools/_ida_layout/{qs_changeres,qs_getposbyindex,qs_getbyinitx,sb_changeres,sb_resetchatbar,qs_init}.c`

## 三、本轮代码改动
### A. 28 格坐标表（IDA 直接复用）
`UIStatusBar.cpp:145-180`：`QS_COL_X[14]`/`QS_ROW_Y[2]` + `make_qs_pos()` constexpr 生成
`MODERN_QS_POS`（28 项，逐组「上行4格→下行4格」）。
键序 `MODERN_QS_KEYS[28]` = 083 的 26 个 + 按同规律续 NUM6/NUM7。

### B. 格数与整格截断（IDA 规则 + 自研）
`UIStatusBar.cpp:364-398`：
- `qs_max_cols = (VWIDTH<=1024 && VHEIGHT<=768) ? 4 : 14`（IDA 8/28 格）
- `qs_span_w(cols) = 7 + (cols-1)*33 + 32 + 4`（末格右缘 + 右留白）
- 若 QS 会压到血条则减列，**始终按整列** → 不出现半格
- `modern_qs_count = min(28, qs_cols*2)`

### C. 血条中轴居中
`status_x = (VWIDTH - 198) / 2`（让 198 宽素材中心落在视口中心）

### D. 菜单移到等级上方
`menu_x = (VWIDTH - 210)/2`、`menu_y = EXP_BOTTOM - STATUS_H - MENU_H - 6`；
`menu_strip = (menu_x, menu_y)`

### E. 底板裁剪到末格边界
`avail_w = 7 + (cols-1)*33 + 32 + 4`（由实际格数反算，不按像素裁）

## 四、本轮修复的两个真 Bug

### ① 【严重·自引入】MODERN_QS_SLOTS=28 越界写 → 快捷键画到 (0,0)
- 现象：QS 底板画在屏幕左侧覆盖聊天框，右侧空白。
- 定位：临时垫片 `_layout_trace.log` 打出
  `INIT-END qs_pos=(806,20)` 但 `DRAW qs_pos=(0,0)`（同一 this）。
- 根因：`refresh_modern_qs_icons` 循环 `i<MODERN_QS_SLOTS(28)`，
  而 `classic_qs_icons/names/counts` 数组按 `CLASSIC_QS_SLOTS(26)` 分配 →
  **i=26,27 越界写破坏相邻成员 `quickslot_pos`**。
  （与历史 0xC0000005 同源，方向相反：那次是 26>20）
- 修复：`UIStatusBar.h` 新增 `QS_SLOTS_MAX = max(CLASSIC_QS_SLOTS, MODERN_QS_SLOTS) = 28`，
  三个数组统一按 `QS_SLOTS_MAX` 分配，并写明硬约束注释。

### ② 【既有·他会话引入】WeatherFx.cpp 编译破损
- `Weather/WeatherFx.h:24` 报 `C2653 "nl" 不是类或命名空间`，
  并出现 `USE_NX` 宏重定义。
- 修复：`WeatherFx.cpp` 最前 `#include "../MapleStory.h"`（先定义 USE_NX + 带入 nlnx）。

## 五、验证
- BUILD_EXITCODE=0；live SHA `e775f9fdae1596de8f02…` 与 `hud_theme_out` 产物一致（实时核对）。
- 截图 `tools/_v31_game.png` / `_v31_bottom.png`：
  聊天贴左 · **血条中轴居中** · **菜单六钮在等级上方** · 快捷键贴右 · 经验条贴底。
- 客户端 PID 33924 进图正常，无闪退。

## 六、仍未完成（如实）
1. **QS 底板 layer:cover 半透明**会透出下方血条颜色（观感杂），未处理。
2. **快捷格内图标是玩家实际绑定**（社区/技能图标），与键帽混排观感乱——功能正确但非"纯键位图"。
3. **拖入/抓出**：`send_icon`/`send_cursor` 的 modern 双路已在上一轮实现，本轮未再改，
   真机拖放动作未逐次点验。
4. **底部整体居中**：目前是「血条中轴居中 + 菜单居中 + QS 贴右」的分布，
   未做"四件套作为整体且过宽时整体居中"的统一容器计算（当前视口下无重叠，故未触发）。
5. `CUIStatusBar::OnCreate`(86KB) 未取全，其中可能还有聊天框/信息框精确坐标可挖。
