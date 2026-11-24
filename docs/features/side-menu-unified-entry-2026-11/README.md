# 左侧统一功能入口（SideMenu + FeatureRegistry）

按 Plan `side_menu_registry_eb885fe2`（1A flyout / 2A 后台权威）落地。

## 本版交付

| 层 | 内容 |
|---|---|
| 客户端 | `FeatureRegistry` 收敛 OpenUI；`SideMenuConfig` JSON（含 `order`）；`UISideIconBar` 树 + 右侧 flyout；`LocalFrameButton`（iconKey→Default/Notify）；ini 迁移；`SIDE_MENU_SYNC` 0x3743 |
| 资源 | `UI/SideIcons/frames/{iconKey}/` LocalPng 序列帧（自 `E:\MXD\扩展改动\左侧小图标\ICON`） |
| 种子 | 仓库根与 `resources/side_menu.json` |
| 服务端/Web | 本工作区无 admin 工程 → schema/SQL/API/协议草案见 `notes/`；客户端可用本地 JSON 热重载验证 |

## 种子菜单树（order）

```
root:
  [5]  任务 (leaf → questlog)
  [10] 日常 (folder)
        [10] 每日签到  [20] 每日礼物  [30] 活动  [40] 勋章
  [20] 战斗 (folder)
        [10] 伤害统计  [20] 魔女森林  [30] 突破石  [40] 强化
  [30] 活动 (folder)
        [10] 现金商店窗  [20] 聊天表情  [30] 神秘徽章
  [40] 工具 (folder)
        [10] 装备染色  [20] 世界地图  [30] 地图信息
        [40] 怪物卡片  [50] 专业技术  [60] 家族
        [70] 联盟  [80] 选项
```

`btMain` + `UIContentsShortcut` 仍独立，不参与本树。

## 本地验证

1. 确认 `Settings` 中 `side_icons=true`
2. 将 `side_menu.json` 与 `UI/SideIcons/frames/` 放到客户端工作目录（与 exe 同级或旁路）
3. 进游戏：左侧应见「任务 + 四个目录」；点「战斗」右侧横排展开子项；再点收起
4. 热重载：改 `side_menu.json` 的 `order`/`enabled`/`revision++`，约 5s 内竖栏重建
5. 服务端就绪后：登录推送 `0x3743`（int revision + maple-string JSON）可覆盖本地缓存

## 非目标（仍有效）

- 不做多级目录；不把目录展开成居中 ShortCutUI
- 不把 icon2 全量进 WZ
- NpcScript 仅当前地图按模板 id 找 oid → `TalkToNPC`（无跨图远程脚本包）
