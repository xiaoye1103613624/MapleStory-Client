# 192 底部状态栏节点归属判定（2026-11-19）

## 用户问题
「192 底部状态栏路径是不是 UI_002.wz/StatusBar3.img？」

## 取证结论：是

用 orange-wz MCP 加载 `E:\MXD\【2023.8.8江浩生日福利】怀旧岛192整合版\怀旧岛192客户端\冒险岛online192\Data\UI\UI_002.wz`（零 IV `AAAAAA==` + 128B DEFAULT_KEY，exclusive:false，load→query→unload_all 收口），`search_node(keyword=StatusBar)` 命中三张 IMG：

| IMG | 顶层子节点 | mainBar 子节点 | 判定 |
|---|---|---|---|
| `StatusBar.img` | — | — | 083 老包 |
| `StatusBar2.img` | starPlanet / mainBar / chat / mapleTalk | 55 个老式节点（backgrnd/lvBacktrnd/gaugeBackgrd/chatSpace/BtCharacter/…/Menu/System/quickSlot） | TMS 风格另一套 UI，**不是**当前底部栏 |
| **`StatusBar3.img`** | mainBar / chat / OptionMenu / FadeYesNo / KeyConfig / Message / ChatEmoticon / ChatFontColor / Chat_Command | **status / status800 / EXPBar / menu / submenu / quickSlot** | ✅ **192 当前底部栏**——正是 live 使用、我们 modern/chat 链路消费的那套 |

`StatusBar3.img` 的 `mainBar = {status, status800, EXPBar, menu, submenu, quickSlot}` 与 `UIStatusBar::init_modern()`（`IO/UITypes/UIStatusBar.cpp:283` 起：`status`/`EXPBar`/`menu`/`quickSlot`/`submenu`）逐节点对应。

## 交叉印证
- `resources/STATUSBAR3-INDEX.md:7-10`：StatusBar3 源包正是 `UI_002.wz`，源 key 零 IV + 128B key，目标 GMS `TSPHKw==`。
- live `Data/UI/StatusBar3.img` SHA256 `93d4af56239109e0582e21f523f68023d38ed1be91e51de51ccd6de9da9c3d49`（1,866,425B），本端 `--resource-probe` 报 `[StatusBar3.img] children=10`，**资源已迁移完成且本端可加载**。
- 因此「192 资源同步过来」这一环对底部栏而言**早已闭环**，无需迁移 StatusBar2。

## 重要口径修正
用户口头称的「192 客户端 = MapleStoryT.exe（IDA 13339）」，其二进制内 **StatusBar3 明文路径 0 个、StatusBar2.img/mainBar/* 大量**（`find_regex` 实测）。即：这份 IDA 的布局逻辑照 StatusBar2 写，而 live 底部栏是 StatusBar3。两套契约不同——见下一节「IDA 还原覆盖度」。
