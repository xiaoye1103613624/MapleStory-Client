# 右侧地图搜索 UI（出图）

> 状态：资源壳 + `UIMapSearchPanel` 已对接；搜索过滤 / 传送数据 / 结果列表内容后期对接  
> 决策：1B（保留原 `WorldMapSearch`，新面板再外挂）+ 数据区留空  
> 产物：`docs/features/map-search-ui/` · 运行时 `UI/MapSearch/`

## 1. 吸附关系

```
[怪物栏] | [世界地图 Border] | [原 WorldMapSearch] | [新地图搜索 260×537] | [掉落栏]
```

- 高度：与世界地图 Border 一致（537，或运行时 `bg_dimensions.y()`）
- 宽度：推荐 **260**（可调；需容纳五 Tab）
- 掉落栏起点：`Border宽 + 原搜索宽 + 260`

## 2. 分区与留空

| 区 | 资源 | 运行时填写（本版不画死） |
|---|---|---|
| 标题「地图搜索」 | 烤在 `search_panel_backgrnd` | 右侧状态文案 |
| 搜索输入槽 | 空槽 | `A11M` 输入文字 |
| Tab 五类 | `tabs/tab_*_{disabled,enabled}` | — |
| 返回 / 副标题槽 | `BtBack_*` + 空槽 | 副标题文字 |
| 详情卡 | 空图标框 + 空行 | 立绘、名称、等级、HP、EXP |
| 列表区 | 空列表底 + 可选选中底 | 地图名、数量等 |
| 底栏 | 空费用槽 + `BtTeleport_*` | 金币消耗文案 |

## 3. 资源清单

### chrome/

| 文件 | 尺寸 |
|---|---|
| `search_panel_backgrnd.png` | 260×537 |
| `detail_icon_frame.png` | 56×56 |
| `list_row_selected.png` | 220×30 |

### tabs/（仿背包，Border 银蓝配色）

`tab_{all,map,mob,drop,npc}_{disabled,enabled}.png` — 槽宽约 44/47×18，烤字：全部/地图/怪物/掉落/NPC

### buttons/（四态）

- `BtBack_{normal,mouseOver,pressed,disabled}.png` — 44×18  
- `BtTeleport_{normal,mouseOver,pressed,disabled}.png` — 64×22  

命中区以 [`resources/layout.json`](resources/layout.json) 为准（与 `UIMapSearchPanel` 常量同步）。

### preview/

- `search_panel_alone.png` — 仅新面板（叠放 Tab/按钮示意）  
- `dock_context.png` — 与「原搜索栏」占位并排示意  

### source/

对照导出：`ItemTab/`、`Basic/`（背包 Tab / 关闭钮体量参考）

## 4. 生成

```bat
python docs\features\map-search-ui\resources\_gen_map_search_ui.py
```

依赖：`docs/features/map-mob-drops/resources/source/UIWindow_WorldMap/WorldMap_Border_*.png`

## 5. 客户端对接（本版已落地壳）

1. `UIMapSearchPanel` + `LocalPngTexture` 加载 `UI/MapSearch/`
2. 位置：`position + (bg_x + search_rail_width(), 0)`
3. Tab / 返回 / 传送：PNG 多态（仿背包交互）；传送本版 `disabled`
4. `get_drop_panel_pos` / `ensure_panels_on_screen` 已计入 260 宽

## 6. 明确未实现

搜索逻辑、传送封包、结果数据绑定、加载状态文案。
