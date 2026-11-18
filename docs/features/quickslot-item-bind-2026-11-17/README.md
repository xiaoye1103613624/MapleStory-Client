# 快捷栏道具绑定三修复（药水拖入 / 数量显示 / 图标数字对齐）

> 日期：2026-11-17 ｜ 分支：feature/dev_0.0.1
> 构建：`tools\build_smoke.bat` EXITCODE=0，GATE OK；UIStatusBar.cpp + UIKeyConfig.cpp 重新编译，
> 链接输出 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`。
> **live exe SHA256（本次最终）**：`dea78540d5dca10bb98c8cea67063b1bc1e16561654b09630362f6193e423193`
> （归档副本 `builds/MapleStory.exe` SHA 一致）。登录器可能回滚 exe，旧 SHA 声明会失效，交付前须重核。

## 用户原始反馈

1. 药水无法直接拖拽到右下角快捷键上。
2. 药水绑定键盘设定按键后，图标和数字错位；点确定后右下角对应按键显示的是名称，应该显示数量。

## 逐条根因（RCA，源码证据）

| # | 现象 | 病根 | 证据（修复前） |
|---|---|---|---|
| 1 | 药水拖不进快捷格 | `UIStatusBar::send_icon` 只接受 SKILL / MACRO，ITEM 落 `else → return false`，被状态栏无声拒绝 | UIStatusBar.cpp 原 :2351-2364 |
| 2a | 快捷格显示名称而非数量 | `refresh_classic_qs_icons` 把 `item.get_name()` 写入 `classic_qs_names`，draw 用中文 Text 画在格下半带 | UIStatusBar.cpp 原 :2250 / :2663-2667 |
| 2b | 图标与数字错位 | 键盘设定窗 KeyIcon::get_type() 恒为 KEY，不触发 Icon 构造对 ITEM 的 `shift(0,32)`；背包 ItemIcon 是 ITEM 会 shift → 同纹理在两窗相差 32px，图标与 Basic.img/ItemNo 数量字模分家 | Icon.cpp :39-40；UIItemInventory.cpp :1950；UIKeyConfig.cpp KeyIcon::get_type |

三症同根：**道具（ITEM）这条分支从未作为一等公民接进快捷栏** —— 技能/宏有全套拖拽 + 数量显示，道具被漏。

## 修复（最小改动，端到端）

### bug 1 — `IO/UITypes/UIStatusBar.cpp` `send_icon`
新增 ITEM 分支：`icon.get_type()==ITEM && get_item_id()>0` → `btype=ITEM, baction=itemid`，
随后复用既有 `keyboard.assign + ChangeKeyMapPacket + redraw_quickslots` 全链路。

### bug 2a — `IO/UITypes/UIStatusBar.cpp` 数量显示
- `UIStatusBar.h`：新增 `classic_qs_itemno`（Charset，Basic.img/ItemNo）、`classic_qs_itemno_ok`、`classic_qs_counts[26]`。
- 构造函数加载 ItemNo 字模（缺资源时回退 Text 画数字）。
- `refresh_classic_qs_icons`：ITEM 分支不再存 name，改存 `get_total_item_count(mapping.action)` 数量字符串（qty>1 才显示）；清空时同步清 counts。
- draw：道具数量用 ItemNo 字模画在格右下（`QS_SLOT-qw-1, QS_SLOT-12`），与背包 Icon 同源，数字位宽固定不再断行错位。

### bug 2b — `IO/UITypes/UIKeyConfig.cpp` 图标数字对齐
`get_item_texture()` 对道具纹理补做 `tex.shift(0,32)`，复刻背包 Icon 对 ITEM 的下移；
不动 KeyIcon::get_type()（拖拽路由仍需恒 KEY）、不动全局 Icon（爆炸半径大）。技能/宏纹理各自已抹平 origin，不受影响。

## 验证门禁

| 项 | 状态 | 证据 |
|---|---|---|
| 编译 | ✅ | build_log.txt：UIStatusBar.cpp / UIKeyConfig.cpp 均重新编译，无 error C/LNK/fatal |
| 协议 gate | ✅ | GATE OK |
| 链接输出 | ✅ | build_log.txt:75 `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe` |
| SHA 闭环 | ✅ | live 与归档副本 SHA 均 `dea78540…` |
| 静态核对 | ✅ | 三处改动逐行复核；`const Texture&` 拷贝后再 shift，不改 ItemData 原数据；Stage::get() 用法与同文件既有代码同质 |
| **真机验收** | ⏳ **未做** | 未登录实测三现象；用户提供的截图来源端存疑（USER.md 纪律：不得拿参考端截图当本端验收证据） |

## 已知点 / 非本次范围

- `update()` 每帧调用 `refresh_classic_qs_icons()`（原既有机制），本次在其中增加了 ITEM 的 `get_total_item_count` 查询（仅对已绑定的少量道具槽）；原机制本就每帧重建 26 张 Texture + 字符串，新增开销量级更小，未改刷新调度以避免计划外重构。
- 截图来源端未确认；若截图为参考端（正确客户端），其中显示不代表本端现状，需用户用本端真机复测三项。
- 技能/宏快捷格的名称显示未改（用户未报，最小改动原则）。

## 复测步骤（本端真机）

1. 背包把一瓶药水直接拖到右下角快捷格 → 应能绑定（bug1）。
2. 该格应显示药水图标 + 右下数量（如 `2`），不显示「红色药水」中文名（bug2a）。
3. 打开键盘设定（KeyConfig），把药水拖到某按键 → 图标与数量应上下对齐、不再相差一格（bug2b）；点确定后快捷格显示数量而非名称。
