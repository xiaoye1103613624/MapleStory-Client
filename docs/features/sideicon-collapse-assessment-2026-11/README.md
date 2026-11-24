# 侧栏折叠展开 —— 高版本证据与当前状态研判（2026-11-21）

## 用户问题
高版本（192/CMS）侧栏是不是有折叠展开：收缩时一个入口图标，悬浮/点击向右展开并排功能列表？

## 结论（逐行号证据）
**有，但和当前实现不是一回事。**

### 1. 高版本确有折叠展开（IDA 一手，项目已有证据）
`docs/features/hud-ui-version-switch-feasibility-2026-11/notes/side-icon-binding.md`：
- `:14` `collapsed` 字段 = 竖栏折叠（只留展开钮）
- `:23` IDA：`CUIContentsShortcut` / `ShortCutUI`
- `:25` **IDA 真路径**：`Etc/ContentsShortcut.img` + `UIWindow4.img/ShortCutUI/contentsIcon/%s`（**弹层**）；**折叠钮 `ShortCutUI/btMain`**
- `:37` 折叠钮 `UIWindow4.img/ShortCutUI/btMain`（四态）
- `:45` 折叠钮写回 collapsed
- `:54-59` 资源来自 192 `UI_005.wz` → `UIWindow4.img/ShortCutUI`（经 create_img→copy_paste→batch_convert_key 转 GMS 注入 live）

### 2. 折叠交互的历史验收（本端曾实现）
`notes/acceptance-statusbar3-sidedock.md:16-19`：
- `视口左侧偏上有竖排图标 + 底部折叠钮`
- `点折叠：槽位隐藏，只留展开钮；再点恢复`

### 3. 当前实现已移除折叠（2026-11-18 用户裁决）
`IO/UITypes/UISideIconBar.cpp`：
- `:179` 「原生竖栏无常驻折叠钮（用户裁决：去掉收起/展开图标与逻辑）」
- `:249` 「常驻显示全部槽位，无折叠态」
- `:272-273` 「原生 192：左侧功能竖栏只有裸图标，无灰色底板、无收起/展开钮。灰底 ColorBox 与折叠钮均为本项目历史自造，已移除（用户 2026-11-18 裁决）」

## 关于"向右展开并排功能列表"
用户描述的这个交互对应 192 的 **`ShortCutUI` 弹层**（`:25` `UIWindow4.img/ShortCutUI/contentsIcon/%s`）——点 `btMain` 弹出一个横向排列的快捷面板，与当前"竖排图标原地隐藏/显示"的折叠不是同一形态。当前 `UISideIconBar` 只有竖排常驻，没有弹层横向列表。

参考包 `E:\MXD\扩展改动\左侧小图标`：
- `ICON/{1..21}`：21 组图标，每组 Appear(~10帧)/Default(12帧)/Notify(12帧) 序列帧（GalaxyStar/EventList/Event/Daily/dailyGift/Content/... 全是活动入口）
- `icon2/MainNotice.*`：活动通知图标序列帧
- 这些是 192 高版本侧栏/通知图标**素材源**，无文档/代码；序列帧结构（每个图标有 Appear 出现动画）印证它们是**各自常驻显示**的图标，而非折叠后展开的子项。

## 判定
- 高版本**有折叠**（IDA 实锤 `ShortCutUI/btMain`），用户描述准确。
- 本端**曾实现竖向折叠又被裁决移除**；用户现在描述的"向右弹出的并排功能列表"是 192 弹层形态，**当前端完全没有**，若要还原属新功能开发（不是恢复旧折叠）。
- 需用户裁决：是要恢复旧竖向折叠，还是按 192 `ShortCutUI` 做横向弹层（真·高版本还原）？后者需 IDA 补 `CUIContentsShortcut` 的弹层布局/坐标一手，再按门禁18实施。

本轮只研判，未改代码/资源。
