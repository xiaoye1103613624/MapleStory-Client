# R10 — 个人情报全功能实现（用户 2026-11-18 七图反馈："所有功能必须全部实现"）

## 缘起（用户原话 + 拍板）
> 图1：名称偏上、个人信息字体不对、位置偏上、人气ui没有、底部按键整体偏上；
> 道具：没显示身上装备，显示装备悬浮也要有 tooltip；想买的：没显示商城想买列表；排除道具：缺ui内容；
> 收藏/骑宠/宠物：ui缺内容样式，是没对接服务端数据还是功能没实现？所有功能必须全部实现；

用户拍板（ask_user_question 确认）：
1. **测的是 Admin 自己**（无坐骑/宠物/心愿单数据）→ 空态也必须有正确渲染，不能整片空白。
2. **授权改服务端**补他人装备字段 → 道具子窗 + tooltip 完整实现。

## 截图来源端裁决（USER.md 教训）
七图截于 00:09~00:15（本端真机现状），非参考端。部署 exe 当时为 `90b5633b`/`369ca922`（被并发构建/登录器反复覆盖），但图1人气灰块出现在我 R09 改的 x≈239 位置 → 确认 R09 代码进了被测版本，排除证据错配。

## 三层链路根因（回答"没对接数据 vs 没实现"）
| 层 | 结论 | 证据 |
|---|---|---|
| 服务端下发 | **下发** 宠物/坐骑/心愿/图鉴/勋章/结婚；**不下发**他人装备明细（s9 原生缺） | `BeiDou-Server_s9/.../PacketCreator.java:2863-2945 charInfo()` |
| 客户端解析 | **完整解析**并调 `update_full(data)` | `PlayerInteractionHandlers.cpp:30-120` |
| 渲染层 | **真凶**：hardcode 英文占位串（"Mount:"/"Lv."/"#id"/"Normal:"）+ 坐标错位 + 子窗背景板画错位置 + 只画背景板无内容 | `UICharInfo.cpp` draw_* 系列 |

**定性：数据链路通（除装备/排除道具），问题在"功能没实现到位"。**

## 逐条行号级修复

### 图1 基础 UI
| 反馈 | 根因 | 修复 |
|---|---|---|
| 字体不对/位置偏上 | `UICharInfo.cpp:270` 值起点 `(153,65)` → 实测值字 y≈60，标签 y≈75，偏上19px；颜色 `EMPEROR` 灰 | `:270` 起点改 `(155,75)`；构造函数 `:107-111` 颜色改 `BLACK` 深黑；去掉 guild 的 +1 偏移 |
| 名称偏上 | `:276` `name.draw(+(59,131))`，名字条 y=147..162 | 改 `+(59,150)` |
| 人气 ui 没有 | 上轮纯灰块 | `:1051-1069` 重画：r04 权威 x=232/249,y=113，深灰边框+浅灰底+黑 ^/v 字形 |
| 底部按键偏上 | 资源坐标已按 r04 对齐（`:95-98`），实为整窗字段区视觉连带 | 字段区下移后整体观感修正 |

### 图2 道具（装备 + tooltip）
- **服务端** `PacketCreator.charInfo()` 末尾追加 `addCharEquips(p, chr)`（复用 spawn 的 `(byte pos, int itemid)* + 0xFF` 编码，向后兼容旧客户端尾部 skip）。
- **客户端解析** `PlayerInteractionHandlers.cpp` 勋章段后读装备列表到 `CharInfoData::equips`，guard <128。
- **渲染** `UICharInfo.h` 新增 `ItemSlotView`（图标+名称+命中矩形）；`update_full` 用 `ItemData::get(itemid).get_name()/get_icon()` 建 3 槽视图；`draw_item_panel` 在 `position+panel_offset` 画图标(x≈8)+名称(x≈52)（**修了旧代码背景画 position、内容画 position+panel_offset 的错位 bug**）；`send_cursor` 命中槽弹 `UI::show_item(ITEMINVENTORY, itemid)` 装备 tooltip。

### 图3 想买的
- 同样 3 空槽视图（wish_views），显示商城心愿道具名+图标+tooltip；空槽无内容但结构正确。

### 图4 排除道具
- **handler 接线**：`MapObjectHandlers.cpp:1802` 旧只读 cid+idx 后丢弃；新按 s9 `loadExceptionList` 格式（int cid/byte idx/long petId/byte count/int itemid*）完整解析，`UICharInfo::store_excluded_items(cid, items)` 静态暂存。
- **渲染** `draw_exclude_panel`：在 backgrnd6 上画动态行（道具名），无数据显示"无排除道具"空态。
- 边界：看自己时服务端 `CharInfoRequestHandler:41` 不下发排除列表（仅他人），故 Admin 自己看为空属正确行为，已给空态。

### 图5 收藏（勋章图鉴）
- `update_full` 的 `book_info_text` 从 hardcode `"Lv.x Normal:x Special:x..."` 改为纯数值 `level/normal/special/total`。
- `draw_medal_panel` 旧 `book_info_text@(150,205)` 压在中文标签上（图5"一般 .I Nor 特殊 Special"重叠即证据）→ 改 `(150,210)`；佩戴中/总勋章数 `(150,330)`；勋章任务列表 `(110,350+i*15)`；空数据给"-"。

### 图6 骑宠
- `update_full` 从 `"Mount:id Lv.x/EXP:x Tired:x"` 英文串改为纯值/空态，用 `ItemData::get(id).get_name()` 显示坐骑名。
- `draw_mount_panel` 旧 `(95,232)/(95,250)` 偏上且压背景 → 改 `(110,288)/(270,288)` 对齐 backgrnd5 底部"种类/等级/经验值/疲劳度"标签右侧空栏。

### 图7 宠物
- `update_full` 宠物文本保持中文可读（名字 + Lv/亲密/饱清）。
- `draw_pet_panel` 三栏坐标从 `(95,232+i*54)` 改 `(110,205+i*35)` 对齐 backgrnd2 右侧三行；空态给"-"。

## 本轮改动文件
客户端：`IO/UITypes/UICharInfo.h/.cpp`、`IO/UITypes/UIMiscShells.cpp`（R09 已在，本轮未再动）、`Net/Handlers/PlayerInteractionHandlers.cpp`、`Net/Handlers/MapObjectHandlers.cpp`
服务端：`BeiDou-Server_s9/gms-server/src/main/java/org/gms/util/PacketCreator.java`

## 验证与诚实声明
- 客户端 `tools/build_smoke.bat` EXITCODE=0 / GATE OK / error C=0；4 个改动 obj 均 00:39 freshly built 入链。
- 服务端 `mvn -o compile` RC=0，PacketCreator.class 00:39 编译。
- 部署 exe 00:39:33 刷新 SHA `0066cd2692377c969c370fe8416f0e5f7f9f077313def831d68795cc07555366`，10 秒复查稳定未回滚。
- **真机未验收**：本轮无实机截图。客户端需重新登录加载新 exe；服务端装备字段需**用户重启 s9** 才生效（oh my，服务端 java 改动必须重启）。用户须按七图同样路径复核：看他人装备是否显示+悬浮 tooltip、看自己各子窗空态是否正确、图1字体/位置/人气箭头是否对齐。
- **服务端未擅自重启**（影响其他在线测试），交用户处理。
- 补丁：`patches/r10-charinfo-subpanels-fullchain.diff`（客户端）、`patches/r10-server-charinfo-equips.diff`（服务端）。
