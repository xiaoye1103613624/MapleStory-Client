# #4 个人伤害统计原生移植 · 接入点全清单（2026-11-18，Phase 0 补全）

> 目的：USER.md 硬性要求"要改一次改全、禁止挤牙膏"。本清单把原生移植触及的**每一个点**逐行号锁定，实现时零遗漏。
> 全程只读取得，未改任何代码、未碰 live。

## 1. 必须新增/修改的文件与位置

| # | 位置 | 动作 | 依据 |
|---|---|---|---|
| 1 | `Character/DamageRank.h` | **新建** | 数据结构 `PlayerEntry/SkillEntry`，移植参考包 `uiDamageRank.h:11-50` |
| 2 | `IO/UITypes/UIDamageRank.h/.cpp` | **新建** | UI 类，继承 `UIDragElement<PosDAMAGERANK>` |
| 3 | `Configuration.h` 约 :957 | **加** `struct PosDAMAGERANK : PointEntry`（仿 `PosMONSTERBOOK` :957-960，key `ui.position.damagerank`） | `Configuration.h:957` |
| 4 | `IO/UIElement.h` `enum Type` 约 :243-244 | **加** `DAMAGERANK`（放在 `SIDEICONBAR` 后、`NUM_TYPES` 前） | `UIElement.h:243-244`；注意 :226-228/:238-242 警告：新增 Type 增大 EnumMap→**必须整工程 Rebuild** |
| 5 | `IO/UIStateGame.cpp` | **加** UIDamageRank 的 emplace/绘制/更新接入（仿 UIMonsterBook 注册） | 待实现时定位（UI.cpp:103 建 UIStateGame） |
| 6 | `IO/UITypes/UIMiscShells.cpp` 约 :6314 | **改**：见下方"F12 冲突" | `UIMiscShells.cpp:6314-6319` |
| 7 | `Net/Handlers/WaveAShellHandlers.cpp:3340-3346` | **改**：`UIAdmin::set_message(tip)` → 喂 UIDamageRank（upsert+reload），保留 DEBUG 到收口清理 | 已证 |
| 8 | `MapleStory.vcxproj` + `.filters` | **登记** `UIDamageRank.cpp`/`.h`（仿 UIMonsterBook 登记行） | vcxproj :399/:753 附近 |

## 2. 硬冲突：F12 已被占用（必须拍板）
- `IO/UITypes/UIMiscShells.cpp:6314-6319`：`keycode == 123`（F12）→ 打开 `UIReset`（AP/SP 洗点 SoftKey）。
- `IO/KeyCodes.h:95` GLFW_KEY_F12=301；`IO/KeyConfig.h:37` F12 枚举；参考包热键是 F12（`damagerank_client_readme.md:11`）。
- **不能抢 F12**。三个方案：
  - **A（推荐）**：改用侧边栏图标入口（`UISideIconBar` 配置驱动，`Util/SideIconConfig.h:14-20` Action::OpenUI，无需动快捷键、与用户"部分功能依赖左侧竖栏图标菜单"要求一致）。
  - **B**：改用未占用键（需扫全快捷键表确认空闲，如 F9~F12 中未被占用者——F10=Book :6303、F11=AranSkillGuide :6309、F12=UIReset :6314 已占，需另找）。
  - **C**：F12 长按/组合键区分（实现复杂、易冲突，不推荐）。

## 3. 原生框架契约（实现必须遵守）
- UI 基类：`IO/UIElement.h:59-130`，`Type` 枚举 :65-245（末项 `SIDEICONBAR` :243、`NUM_TYPES` :244）。
- 拖拽基类：`IO/UIDragElement.h`，模板参数=位置配置结构；`send_cursor` 已实现拖拽+`Setting<T>::save(position)`。
- 位置配置：`Configuration.h:957-960` `PosMONSTERBOOK : PointEntry("ui.position.monsterbook","(200,100)")`，仿此加 `PosDAMAGERANK`。
- 窗口范式：`IO/UITypes/UIMonsterBook.h:46-61`（继承声明 + `draw/update/button_pressed/send_cursor/send_key/send_scroll/get_type` 全套覆写）。
- 按钮 enum 段：`UIMonsterBook.h:92-102` 示范；沿用参考包 id 1000/1002/1003/1004/1005（`uiDamageRank.h:59-65`）。
- 按键分发：`IO/UI.cpp:207` `UI::send_key` → `UIStateGame::send_key`；功能键走 `UIMiscShells.cpp` 软键链（`keycode==123` 等）。

## 4. 数据结构与协议（无协议风险，照搬）
- `PlayerEntry{int charId;string name;int job;uint64 totalDamage}`、`SkillEntry{skillId,name,delta/total/max/minDamage,count}`（`uiDamageRank.h:13-28`）。
- 服务端发包 s9 完整：`PacketCreator.java:341-368`（mode0 reset / mode1 player: byte1,int charId,int jobId,string name,long totalDamage / mode2 skill: byte2,int skillId,string,4×long,int count）。
- 客户端解析已通：`WaveAShellHandlers.cpp:3304-3347`（字段与服务端对齐），只差把结果从 UIAdmin 提示改为喂 UI。

## 5. 实现顺序（一次性成型）
1. `Character/DamageRank.h` 数据结构。
2. `Configuration.h` 加 `PosDAMAGERANK`；`UIElement.h` 加 `DAMAGERANK` Type。
3. `UIDamageRank.h/.cpp`（含双页渲染、滚动、拖拽、按钮），对齐 UIMonsterBook 范式。
4. `UIStateGame` 接入 + `UIMiscShells.cpp` 入口（按用户拍板的 A/B/C）。
5. `WaveAShellHandlers.cpp:3340-3346` 改喂数据。
6. `vcxproj`/`.filters` 登记 → **整工程 Rebuild**（因 Type 尺寸变）。
7. 验证：VS 构建 error=0/lnk=0 + 协议 mode0/1/2 造包字段落地；真机未验如实声明。

## 6. 待用户拍板
- **入口方案 A/B/C**（F12 冲突）：推荐 **A**（侧边栏图标，零快捷键冲突 + 符合需求原文）。
