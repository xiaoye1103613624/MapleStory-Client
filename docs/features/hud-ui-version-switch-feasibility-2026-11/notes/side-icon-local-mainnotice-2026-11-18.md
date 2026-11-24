# 左侧竖栏：IDA 路径 + 本地 MainNotice 素材（2026-11-18）

## IDA（MapleStoryT.i64 / 13339）资源路径证据

| 用途 | 字符串 / 逻辑 | 结论 |
|---|---|---|
| 内容快捷配置表 | `Etc/ContentsShortcut.img` | 每项读 `resourcePath` / `openUI_idx` / `toolTip` / `questIDs` |
| 内容图标贴图 | `UI/UIWindow4.img/ShortCutUI/contentsIcon/%s` | `%s` = `resourcePath`（缺省 `default`） |
| 折叠主钮 | `UI/UIWindow4.img/ShortCutUI/btMain`（及 `/normal/0`） | `CUIContentsShortcutOpen` 贴小地图旁 |
| 弹层框 | `ShortCutUI/top|middle|bottom|textBox|btClose|newIcon` | 展开网格，非左侧常驻竖栏 |
| SideMenu | `UIWindow2.img/UserList/.../SideMenu/*` | **好友/组队右键菜单**，与左侧竖栏无关 |
| GrowHelp / LevelUpGuide / MirrorDungeon | 各自 `BtMain` / `BtGuide` / `btDungeonMove` | 小地图旁辅助钮，非书/灯/星竖栏 |

**本包 192 `UI_005` 的 `contentsIcon` 仅有** `default/hekaton/bossArena/crossHunter`，**没有**图1书/灯/星美术。  
图1竖栏视觉对应 **MainNotice** 导出（本地目录），不是 StatusBar3 submenu，也不是 ShortCutUI 活动圆标。

## 本地目录 `E:\MXD\扩展改动\左侧小图标`

| 子目录 | 规模 | 用途 |
|---|---|---|
| `ICON/1..21` | ~675 PNG | 按类型分夹的 MainNotice 帧（Appear/Default/Notify） |
| `icon2/` | ~8167 PNG | 扁平 `MainNotice.<Name>.<State>.N.png` 全量导出 |

本版选用（对齐图1 书/灯/星）：

| 语义 | 源文件 | 部署名 | 点击 |
|---|---|---|---|
| 书 | `ICON/2/EventList.Default.0.png` (+ Notify over) | `UI/SideIcons/book.png` | OpenUI QuestLog |
| 灯 | `ICON/4/Daily.Default.0.png` (+ Notify) | `UI/SideIcons/bulb.png` | OpenUI Event |
| 星 | `ICON/1/GalaxyStar.Default.0.png` (+ Notify) | `UI/SideIcons/star.png` | OpenUI DailyGift |

部署位置：`BeiDou-Client-xy/UI/SideIcons/` + 仓库 `UI/SideIcons/` + `docs/.../resources/SideIcons/`。  
**PNG 直读（LocalPngTexture），无需把 PNG 裸拷进 `.img`；** live `MainNotice.img` 本轮 orange-wz 子节点为空，未强行入库。

## 代码接线

- `side_icons.ini`：`local:book|bulb|star`
- `UISideIconBar`：优先 LocalTwoSpriteButton；折叠钮仍用 `ShortCutUI/btMain`
- `Button::set_position` 改为 virtual，保证本地钮布局生效

## 续作验收（2026-11-18 晚，非 resume 48df3f25）

1. **IDA 13339 复核**：`MapleStoryT.i64` 仍挂；字符串命中  
   `Etc/ContentsShortcut.img`、`UI/UIWindow4.img/ShortCutUI/contentsIcon/%s`、`.../btMain`、`resourcePath`、`openUI_idx`。  
   **无**书/灯/星专用 WZ 节点；竖栏美术继续走本地 MainNotice 导出。
2. **素材盘点**：`ICON/1..21`（675）+ `icon2`（8167）；本版三槽 SHA 对齐  
   EventList / Daily / GalaxyStar 的 Default.0 + Notify.0 → `UI/SideIcons/{book,bulb,star}[_over].png`。
3. **Rebuild→xy**：`hud_sideicon_out\MapleStory.exe`（4666880，2026-11-18 20:30）已覆盖 live；  
   `side_icons.ini` + `UI/SideIcons/*` 已同步；`Settings.side_icons=true`。  
   当时仍有旧 MapleStory 进程在跑——**需重启客户端**才加载新 exe。

## 续作收口 2026-11-18 20:32
- IDA 13339 复核：仍有 Etc/ContentsShortcut.img / UIWindow4.img/ShortCutUI/contentsIcon/%s / tMain / 
esourcePath / openUI_idx
- 素材：ICON 1/2/4 → star/book/bulb（Default+Notify）已刷到 repo/docs/xy/hud_sideicon_out
- Rebuild：hud_sideicon_out\MapleStory.exe (4666880, 2026-11-18 20:30:34) → 已覆盖 live xy；旧 exe 备份 MapleStory.exe.pre-sideicon-20261118-203050
- 运行中旧进程 PID 26292/36840/42860 仍占旧映像，需重启客户端验收
