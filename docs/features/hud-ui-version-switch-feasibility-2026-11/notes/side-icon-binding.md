# 左侧竖栏绑定表（UISideIconBar）

## 模型

贴图只负责外观；点击由 C++ `dispatch` 映射。**不是** WZ/服务端脚本画出来就有功能。

| 字段 | 含义 |
|---|---|
| `slot.N.id` | 槽位名（日志用） |
| `slot.N.enabled` | `0/1` |
| `slot.N.icon` | UI 命名空间路径，如 `UIWindow4.img/ShortCutUI/contentsIcon/hekaton` |
| `slot.N.action` | `OpenUI` \| `ChatCmd` \| `Packet` \| `NpcScript` |
| `slot.N.arg` | 窗名 / 聊天串 / opcode 名 / NPC 脚本名 |
| `collapsed` | 竖栏折叠（只留展开钮） |

落盘：客户端工作目录 `side_icons.ini`（默认从本目录 `resources/side_icons.ini` 部署）。

## 布局（2026-11-18）

- **x**：视口左缘 + `MARGIN_X=4`
- **y**：`(view_h - bar_h) / 2` 垂直居中
- **禁止**跟到小地图右侧；与通高小地图重叠时靠小地图 192 尺寸钳制 + 竖栏后绘可点
- IDA：`CUIContentsShortcut` / `ShortCutUI`；本栏左缘居中，不用 Open 贴图旁逻辑
- **图标（本版）**：本地 MainNotice 导出 PNG → `UI/SideIcons/{book,bulb,star}.png`（`local:` 前缀）
- **IDA 真路径**：`Etc/ContentsShortcut.img` + `UIWindow4.img/ShortCutUI/contentsIcon/%s`（弹层）；折叠钮 `ShortCutUI/btMain`
- **禁止**再用 `StatusBar3.img/mainBar/menu|submenu` 冒充左侧栏（主菜单留底栏）
- 详见 [side-icon-local-mainnotice-2026-11-18.md](side-icon-local-mainnotice-2026-11-18.md)

## 默认槽（本版可点）

| id | icon | action | arg | 打开 |
|---|---|---|---|---|
| book | local:book（EventList） | OpenUI | QuestLog | `UIQuestLog` |
| bulb | local:bulb（Daily 灯泡） | OpenUI | Event | `UIEvent` |
| star | local:star（GalaxyStar） | OpenUI | DailyGift | `UIDailyGift` |

折叠钮：`UIWindow4.img/ShortCutUI/btMain`（四态）。

另支持 OpenUI：`EventWindow` / `GameOpt` / `MonsterBook` / `OptionMenu` / `DamageRank` / `SysOpt`（可经 ini 加槽，勿默认塞底栏主菜单图标）。

`ChatCmd` → `GeneralChatPacket`；`Packet` / `NpcScript` 弹 `UIOk` 明示预留（禁止假按钮静默无反馈）。

## 动态增减

改 `side_icons.ini` 后热重载（约 5s 轮询，或折叠钮会写回 collapsed）。无需改 C++ 枚举。总开关：`Settings` / `ui.side_icons`。

## 与 ThemePack

竖栏独立 `UIElement::Type::SIDEICONBAR`，classic/modern 均显示。

## 资源流水线（证据）

```text
192 UI_005.wz UIWindow4.img/ShortCutUI
 → create_img(新版本零 IV) + copy_paste
 → save_as staging UIWindow4.zero.img
 → batch_convert_key(GMS, verify:true) → out_gms/UIWindow4.zero.img
 → 原子替换 live Data/UI/UIWindow4.img
```
