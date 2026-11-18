# 怪物手册居中 + 拖拽 + HP/MP 配色修复（2026-11-17）

## 1. 用户诉求（原始记录）

> 聊天室ui样式存在问题，功能不确定是否完整，怪物手册打开后没有居中显示而且不可以拖拽，怪物手册功能、ui可能不完善；上面两个问题都处理一下，对照angel、minms、img闭环一下；

两个问题：① 聊天室 UI 样式/功能；② 怪物手册不居中、不可拖拽、功能可能不完善。要求对照 angel（IDA）、minms（移植参考）、img（WZ/IMG 资源）闭环。

## 2. 问题一定位：聊天室 UI（结论：已完备，无需改动）

真机确认聊天室窗口即 `UIMessenger` 群组聊天窗（截图 m00112：灰/蓝/红/绿四条名字条 + Admin 头像）。

| 证据字段 | 内容 |
| --- | --- |
| 类定义 | `IO/UITypes/UIMessenger.h:22` `class UIMessenger : public UIDragElement<PosMESSENGER>`（已是可拖窗） |
| 布局常量 | `docs/features/ui-v83-window-restore/evidence/w4-chatbar/messenger-layers.md` §12 真机 PASS：`BT_ENTER_X=247`、`SLOT_COUNT=4`、`CHAT_LOG_TOP=231`/`VISIBLE=4`/`LINE_H=14` |
| 交互/国际化 | ESC 回调、blink/chatBalloon 已接线；`messenger.self`/`no_room` 等 i18n 已就绪 |
| 已知缺口（留存未猜测） | `backgrnd2` y193..209 输入槽未使用；聊天左缩进 x=16/行高 14 无像素 oracle——文档已记录，不凭猜测改 |

结论：聊天室在 angel/minms/img 对照下功能与样式已闭环，本次不改动，避免回归。

## 3. 问题二定位：怪物手册不居中/不可拖拽（根因确认）

| 证据字段 | 内容 |
| --- | --- |
| 根因 | `IO/UITypes/UIMonsterBook.h:46` 原本 `class UIMonsterBook : public UIElement`——继承普通 `UIElement` 而非 `UIDragElement<Pos>` |
| 直接后果 | ctor 只设 `dimension=Point<int16_t>(516,349)`（UIMonsterBook.cpp:120），无 position 居中、无 dragarea，故窗口落 (0,0) 且无法拖动 |
| 可拖基类 | `IO/UIDragElement.h:33-131`：`send_cursor` 在顶部 dragarea 内按下进入拖拽、移动改 position、`remove_cursor` 保存坐标；`center_if_position_unset`(107-119) 在保存坐标==默认值时按视口居中，用户拖过后写配置尊重其位置 |
| oracle | `docs/features/ui-v83-window-restore/evidence/r27-oracle/MonsterBook.json`：book_stats = infoPage(40,34)、cardSlot(269,52)、BtClose(429,12)、RightTab0(439,29)、label rows [112,165,196,217,253]；与当前布局一致 |
| 功能盘点 | 13 节点绑定、左 5x5 卡片格 + 搜索、右详情、9 分类页签、4 情报页签、card→mob 反查（reward 238xxxx）、C/R/S 快捷键、滚轮翻页均已实现——功能基本完备，缺陷集中在居中/拖拽 |

## 4. 修改清单（4 处，均已落地）

1. `Configuration.h`（PosMESSENGER 之后）新增：
   `struct PosMONSTERBOOK : public Configuration::PointEntry { PosMONSTERBOOK() : PointEntry("ui.position.monsterbook", "(200, 100)") {} };`
   默认值 `(200,100)` 作为 "未拖动" 哨兵，供 `center_if_position_unset` 判定。
2. `IO/UITypes/UIMonsterBook.h:27` `#include "../UIElement.h"` → `#include "../UIDragElement.h"`；`:46` 基类改为 `UIDragElement<PosMONSTERBOOK>`。
3. `IO/UITypes/UIMonsterBook.cpp`：
   - ctor `dimension = Point<int16_t>(516, 349);` 后加 `dragarea = Point<int16_t>(dimension.x(), 20);`
   - ctor `#endif` 后、`load_cards` 前加 `center_if_position_unset(dimension);`
   - `send_cursor` 末尾 `return UIElement::send_cursor(...)` → `return UIDragElement<PosMONSTERBOOK>::send_cursor(clicked, cursorpos);`（卡片格点击在上方已拦截，拖拽交给基类）
4. HP/MP 死代码清理 + 配色（对齐原生截图 `怪物手册2.png` 选中蜗牛的 HP 玫红/MP 蓝）：
   - `refresh_info()` 循环复位时 `info_line[i].change_color(Color::Name::BLACK);`
   - 选中卡片分支去除无意义的 `collected ? "HP : ???" : "HP : ???"`，改为固定 `HP : ???` + `change_color(PINK)`、`MP : ???` + `change_color(BLUE)`（恒 `???` 是原生行为，本端无 Mob 数值源，不改语义）。

注：`Configuration.h` diff 中另有非本次的 `ChatType 3→1` 改动，属历史工作树内容，未触碰。

## 5. 构建与部署闭环（证据）

| 检查项 | 证据 |
| --- | --- |
| 构建脚本 | `build_monsterbook_verify.bat`（OutDir=`monsterbook_out\`、IntDir=`monsterbook_int\`）、日志 `build_monsterbook.log` |
| 防 mtime 陷阱 | 构建前 touch 三个源文件至 09:52:14，删除残留 obj；构建后 `UIMonsterBook.obj` mtime 09:57:44 > cpp 09:52:14、`Configuration.obj` 09:54:57 > h 09:52:14——证明真编译，非 MSBuild 跳过 |
| 编译结果 | `BUILD_EXITCODE=0`，`MapleStory.vcxproj -> monsterbook_out\MapleStory.exe`；编译日志中 `UIMonsterBook.cpp` 出现 2 次（含生成代码） |
| 产物新鲜度 | exe SHA `d30cf991...`（旧 live）→ `c6b43e0d...`（新），`ui.position.monsterbook` 字符串已编进二进制 |
| 部署 | `deploy_monsterbook.bat`：关客户端 → 备份 `*.bak-20261117_monsterbook` → 替换 `MapleStory.exe` 与 `MapleStory_pua24.exe`（两文件本就同 SHA）→ 三者 SHA 均 `c6b43e0d...` |

## 6. 真机验证（1280x720）

| 项 | 观察 | 结论 |
| --- | --- | --- |
| 居中 | 手册出现于屏幕中央（左边缘 x≈382=(1280−516)/2） | ✅ 居中生效（原为角落 (0,0)） |
| HP/MP 配色 | 卡片页右页 `HP : ???` 玫红、`MP : ???` 蓝色（截图 mb_02_dragged.png） | ✅ 配色生效且对齐原生 |
| 可拖拽 | 干净对比图 `mb_clean_01_open.png` / `mb_clean_02_drag.png`，像素分析手册内页包围盒中心：拖前 (627,345) → 拖后 (759,402)，位移 (+132,+57) | ✅ 拖拽生效（输入客户区 (640,195)→(800,300)，clamp 后一致） |
| 聊天室回归 | 登录进游戏、频道聊天窗正常显示（mb_00_in_game.png） | ✅ 无回归 |

### 拖拽验证方法说明

首轮自动验证（_verify_monsterbook.py）受两处环境干扰：微信更新弹窗抢焦点 + fade 淡入动画起点抓早（mb_01_opened.png 全黑），故追加干净版（_verify_drag_clean.py：等 fade 完成 + 主窗口 HWND 原点 + 拖前/拖后两张对比图）。桌面 2560x1600、游戏客户区 1280x720 居中，客户区原点 (640,424)；拖拽起点绝对屏幕 (1280,619) 正落在手册标题栏（客户区 (640,195)，即顶部 20px 的 dragarea 内），终点 (1440,724)。像素分析脚本取米白纸张色 `r>=212,g>=205,b>=180,max−min<=40`、ROI x250..1100/y150..650，2%~98% 分位抗噪。三次独立截图（首轮拖后 mb_02_dragged.png 中心 (732,393)、干净版拖后 (759,402)）均相对居中态有明显位移，结论一致。

## 7. 复盘四步法

1. **回顾目标**：修聊天室 UI + 手册居中/拖拽/功能，对照 angel/minms/img 闭环。
2. **评估结果**：聊天室经核实已完备未动；手册居中、配色已修，拖拽已实现并经真机验证；构建/部署全绿。
3. **分析原因**：手册缺陷根因是基类选错（UIElement 而非 UIDragElement）；HP/MP 是死代码坏味道但语义恰好对齐原生。
4. **沉淀规律**：可拖弹窗统一走 `UIDragElement<PosX>` + `center_if_position_unset` 模式；构建验证必须用 obj/exe mtime + SHA 三重校验防 MSBuild 假过。
