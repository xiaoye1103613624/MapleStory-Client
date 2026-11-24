# 世界地图六项修复（滚轮 / 爆率 / tooltip / 图标 / 搜索 / 箭头按钮）

> 状态：全链路取证 + 方案A/A/B 用户拍板 + 全量构建 + 部署 live + 短启无闪退；**真机交互待用户截图验收**。
> 日期：2026-11-22 · 分支 `feature/dev_0.0.1`
> 部署：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` SHA256 `b395e917044496847796a35ac44bc6b85a7c7c64b0128ab7551ee1d410fd25a2`（5337088 B，00:11:32）

## 1. 用户反馈（6 项，逐条行号级根因见交付消息）

1. 左侧怪物/爆物列表滚轮无效；爆率数值不明显，问如何布局。
2. 爆物 tooltip 时有时无（前几个显示、后面不显示）；右侧滑块也不能滚轮。
3. 世界地图 tooltip 有怪物/NPC 信息但没有对应图标。
4. 搜索：竖杠偏高、回车无效、结果重叠不在灰框内、结果正确性存疑（点了不显点位/不显左侧怪物/提示无此怪）。要求对照高版本检查。
5. 删除大地图右侧箭头。
6. 顶栏「可以自动飞行 / 登记指路」点击没效果，问作用。

用户拍板：**1A**（4×4 放大格 + 爆率黄字底衬）、**2A**（tooltip 按每行 life id 取真实站姿图标）、**3B**（搜索索引只收录有地图引用的怪/NPC）。

## 2. 改动明细（点到面）

| # | 层 | 文件 | 改动 |
|---|---|---|---|
| 1/2滚轮 | 派发链 | `UI/UITypes/UIWorldMap.{h,cpp}` + `UIMapInfoPanel.{h,cpp}` | 新增 `UIWorldMap::send_scroll` override（原默认空），光标落在 info_panel 范围内时按上/下区门控转发给 `mob_slider`/`drop_slider` 的 `send_scroll`；panel 非 UIElement 收不到 `UIStateGame::send_scroll` 广播，故由宿主转发。 |
| 1爆率 | 布局 | `UIMapInfoPanel.cpp` | 方案A：GRID 5×5→4×4，SLOT 30→38、GRID_X 31→22 居中；爆率由 11px 黑字压图标改为格子底部深色半透明底衬(0.62) + ItemNo 金色字居中横条，一眼可读。 |
| 2tooltip | 命中区 | `UIMapInfoPanel.cpp` | `drop_grid_area()` 是面板局部坐标，旧版直接拿它比全局 cursorpos → 命中区错位到屏幕左上；改为 +position 得 g_abs 再判定，行列计算仍用局部坐标（两处坐标系统一）。 |
| 3图标 | 数据源 | `IO/Components/MapTooltip.{h,cpp}` | 方案A：`String/MonsterBook.img/icon` 在本端 .nx 实测不存在（空纹理根因）；改为每行按 life_id 取 `Mob/<id7>.img/stand/0`（NPC 走 Npc stand）真实站姿图，逐行绘制 `mob_icon_stand[i]`/`npc_icon_stand[i]`，reset 清空。 |
| 4搜索竖杠 | 对齐 | `UIWorldMap.cpp` | `search_text.draw` 去掉硬编码 `-5` 偏移，改 `(0,0)`，与 update 锚点一致。 |
| 4回车 | 回调 | `UIWorldMap.cpp` | 注册 `search_text.set_enter_callback(submit_native_search)`；send_key 判断由字面 `13`（永不命中）改为 `KeyAction::Id::RETURN`（值 62）。输入框聚焦时 RETURN 由 Textfield 回调处理，非聚焦由 send_key 兜底。 |
| 4结果点位 | 粒度 | `UIWorldMap.{h,cpp}` | 新增 `resolve_spot_mapid()`：搜索索引给细分 mapid，可能不在任何 spot 的 mapNo 里 → 精确命中用原 id，否则取同 prefix 的 spot 代表 id 做选中/高亮，解决"点了地图不显示点位高亮/左侧不出怪"。 |
| 4结果正确性 | 索引 | `Util/Misc.cpp` | 方案B：`get_map_search_index` 追加怪物/NPC 名前，先建 `life_map_index()`（单次扫 Map002 建 (kind,id)->map，带 built 标志），只收录至少出现在 1 张地图 life 里的实体，杜绝"搜到却提示未在任何地图出现"。 |
| 5右箭头 | 删除 | `UIWorldMap.cpp` | 移除右箭头 draw_tab 调用、is_in_range 命中、send_cursor 悬停/点击分支（右侧搜索面板早已 set_visible(false)，右箭头是无功能孤儿）；保留左箭头。 |
| 6按钮 | 反馈 | `UIWorldMap.cpp` + locale | 两按钮加 BUTTONCLICK 音效；未选中提示改为操作化文案（"请先在大地图点击选择地图点位…"），中英双语。 |

## 3. 对照高版本的结论（问题4）

`docs/features/beidou-v17-parity/notes/r207/r208/r766` 一手记录：高版本搜索是 SoftKey（数字 mapid → select_map_spot + ChangeMap；T/Enter 对已选图传送），原生 `WorldMapSearch` 轨只有 resultField/Mob/Npc 行 + BtAllsearch，无独立"回车搜名字"语义——本端回车搜索本就是自造扩展，本轮使其真正可用（回调 + 正确键码），并让结果点击走高版本同款 select_map_spot 语义。

## 4. 验证

- **隔离构建**（并行会话多次正在写同工程：UIEnhance 新文件未入 vcxproj / `Locale::has` 非法静态调用，均与本 6 项无关，待其收口后）：`MSBUILD_RC=0`，error=0，产物 SHA `b395e917…fd25a2`。
- **部署**：live 正式 exe 覆盖，实时核对 SHA == 产物；locale 新文案就位（zh 命中 2）。
- **短启**：`tools/_ab_launch.ps1 -Wait 12` → `RESULT=ALIVE`，未触发堆损坏；测试进程已关闭。
- **未做**：真机交互（滚轮、爆率观感、tooltip、图标、搜索回车/结果、右箭头消失、按钮提示）待用户截图验收，截图须本端。

## 5. 回滚与风险

- 回滚：用上一版 bak（`MapleStory.exe.bak_before_diffweapon_20261121` 等）会丢失本轮及前序多轮修复，不推荐；本轮产物 `build-wm6fix-verify/` 可留作回退点。
- 风险：`life_map_index()` 首次扫 Map002（8M 节点）一次性成本，建表后 O(1)；搜索索引构建时会触一次，约 1 秒内。
- 并行会话：本轮构建被并行「装备强化」功能中间态打断 3 次，最终以其收口后的干净工作树构建；本修复未触碰并行文件。

## 6. 关联

- 前序四合一：`docs/features/worldmap-4fix-2026-11/README.md`。
- 192 搜索/三分区：`docs/features/worldmap-192-search/README.md`。
- 左侧信息面板：`docs/features/map-info-panel/`。
- 高版本搜索对照：`docs/features/beidou-v17-parity/notes/r207|r208|r766-*.md`。
