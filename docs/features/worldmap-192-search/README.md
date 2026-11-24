# 世界地图：192 原生搜索轨 + 悬浮 tip 三分区（2026-11-18）

> 状态：**真机验证通过**（2026-11-19，s9 服务端在线）。构建 BUILD_EXIT=0，exe 已刷新。
> 用户决策：只并搜索面板子树 / 移除自定义右侧面板 / 服务端全闭环（经取证缩减，见 §4）/ probe+短启+真机。

## ⚠️ 关键坑：改 .img 必须删 .nxcache/UI.nx（否则不生效）

运行时 `resource.nxcache: true`（`xy-config.yaml`）→ `ImgFiles::init` 把 Data/**/*.img 派生为 `.nxcache/UI.nx` mmap 加载。
`ImgFiles.cpp:255 nx_is_complete(nxpath)` **只校验缓存完整性，不比对 .img 时间戳** → 改了 UIWindow.img 后旧 UI.nx 仍被复用，新节点永不生效。

改完任何 .img 后必须：
1. 关闭客户端；
2. 移走 `E:\MXD\BeiDou-Client-xy\Data\.nxcache\UI.nx`（保留备份）；
3. 重启客户端自动重建（实测 92MB 仅 2-3 秒）。

**资源并入的正确位置**：192 `WorldMapSearch/Bt*` 必须并入 **`UIWindow.img/WorldMap`**（原有 title+Border 的完整节点），
不要并入 `UIWindow2.img` —— `UiAssetResolve::window("WorldMap")` 优先 UIWindow2，若它只有搜索子树无 Border，
会导致地图边框/标题栏丢失、搜索面板定位错乱（真机踩坑已修）。

## 真机验证（2026-11-19）
- `W` 键开世界地图 → 边框/标题栏正常 + 右侧搜索面板默认显示（搜索框+绿色搜索按钮+提示文字）+ 左上"登记指路"按钮。
- 点地图 marker（林中之城附近）→ 左侧"地图信息"面板展开（怪 HP/EXP/属性抗性/掉落）+ 悬浮 tooltip 三分区（地图名+怪物 Lv+NPC）。
- elemAttr 显示正常（无元素属性怪显示"属性:无 抗性:无"）。
- 登录收尾时 admin 账号在线状态未释放（非本功能问题，服务端账号状态）。

## 1. 需求与结论

推翻此前"保持不动"决策，按多方面举证完善世界地图。结合 `E:\MXD\扩展改动\世界地图tip`（Kaentake 083 DLL）文档，最终实现：

1. 192 原生右侧搜索轨（含结果列表）入库并启用，移除自定义 `UIMapSearchPanel` 右侧面板。
2. 悬浮 tooltip 三分区：怪物 / NPC（已接通）+ 在线玩家（本次新增），样式对齐文档 DLL。
3. 缺陷补齐：侧栏 tip 展开/收起文案、AutoFly/NaviRegister 按钮反馈。
4. "服务端闭环"经取证确认大部分已闭环，数据缺失项不臆造（§4）。

## 2. 素材迁移（G0，AGENTS.md §3.1 门禁）

### 192 源路径（实测确认）
- 包：`E:\MXD\【2023.8.8江浩生日福利】怀旧岛192整合版\怀旧岛192客户端\冒险岛online192\Data\UI\UI_004.wz`
- 节点：`UIWindow2.img/WorldMap/{WorldMapSearch, BtSearch, BtNaviRegister, BtAutoFly_1, BtAutoFly_2}`
- `WorldMapSearch` 子节点：`backgrnd`(147×537) / `notice`(88×59) / `BtAllsearch` / `BtLevelMob` / `resultField|resultMob|resultNpc`(各 mouseOverBase 115×13 + icon 11×11)

### 关键坑（务必记住）
- **192 是零 IV 分卷**：orange-wz `load_files` 用 key `{ivBase64:"AAAAAA==", userKeyBase64:128B DEFAULT_KEY, name:LATEST}`。
- **`list_children(nodePath:"")` 对分卷根返回空**（曾误判"解不开"）；必须直接指定 `nodePath:"UIWindow2.img/..."` 才能解析，画布 ARGB4444、像素可读。
- **跨端入库**：`create_img_file(GMS)` + `copy_paste_nodes` 落盘即 GMS 密文（与 StatusBar3 教训一致）——`batch_convert_key` 实测报 "source parse failed"（源已是目标 key），属冗余步骤，跳过。
- 合并进 live `Data/UI/UIWindow2.img` 的 `WorldMap` 节点（保留原 4 个 Item 节点）；`verify_canvas_formats` 32 画布全 ARGB4444 / flag=0。

### 留档
- `resources/staging/UIWindow2_merged.img`：合并产物（GMS key）。
- `resources/staging/UIWindow2Search_srckey.img`：192 子树中转（GMS key 落盘）。

## 3. 代码改动

| 文件 | 改动 |
|---|---|
| `IO/Components/Tooltip.h` | 新增共享结构 `MapTooltipPlayer{name,level,channel}` |
| `IO/Components/MapTooltip.{h,cpp}` | 新增 `set_players` + `player_header/player_labels[10]`；`draw_worldmap` 增玩家分区（金标题 `[Lv.x] Name (Ch.y)`）；reset 清空 |
| `IO/UIState.h` / `UIStateGame.{h,cpp}` / `UI.{h,cpp}` | `show_map` 加可选末参 `players={}`，全链路转发到 `matooltip.set_players` |
| `IO/UITypes/UIWorldMap.{h,cpp}` | ① tip 三元按展开/收起态取 `.557/.558`/`.560/.561`；② `BT_AUTOFLY/BT_NAVIREG` 接明确提示、`BT_ALLSEARCH` 接搜索提交；③ hover spot 按需请求 0x178（`hover_req_mapid` 去重）并把玩家传入 tooltip；④ 移除地图右上固定玩家层；⑤ 原生搜索轨：`submit_native_search` 用 `get_map_search_index` 匹配、`resultField/Mob/Npc` 结果行绘制 + 点击命中、回车/BtAllsearch 触发；⑥ `map_search_panel.set_visible(false)` 移除自定义右面板 |
| `IO/UItypes/UIMapSearchPanel.h` | 加 `set_visible` |
| `locale/{zh-CN,en-US}.ini` | 新增 `.558/.561` 收起文案 + `autofly_na/navireg_na` |

## 4. 服务端"全闭环"取证结论（重要）

用户选"全部补齐闭环"，但取证发现目标项大多已闭环，数据缺失项不应臆造，**故未新增 opcode**（新增会引入 gate 失败 + 双端同步风险，无真实数据支撑）：

| 项 | 状态 | 依据 |
|---|---|---|
| 在线玩家列表 | ✅ 已闭环 | s9 `WorldMapPlayersHandler` + Recv `0x115`/Send `0x178`，客户端已用 |
| 地点掉落查询 | ✅ 已闭环 | s9 `WorldMapDropRequestHandler`（个体掉率）+ `WORLD_MAP_DROP_RESULT(0x179)` |
| 传送费用 | ✅ 已闭环 | `MapWarpCommand.COST=100000` == 客户端 `MAPWARP_COST=100000`；无需回包 |
| 怪物属性/抗性 | ⛔ 数据缺失 | 实测部署 `Mob/0100100.img/info` **无 `elemAttr` 节点**（北斗精简版，R6）；s9 `MonsterInformationProvider` 也无 → 保持 `-`（非缺陷） |
| 任务窗 MARKNPC 定位 | ⛔ 证据缺失 | `QuestData::get().get_npc()` 有 npc id，但 npc→map 反查无服务端协议证据（原注释明确"不发明协议"），保持预留 |

## 5. 验证

- `tools/build_smoke.bat` → `BUILD_EXIT=0`，exe 刷新（19:22）。
- `--resource-probe`：本环境被 3 个权限拒绝的僵尸 MapleStory 进程干扰，输出止于 "Parsing uol" 无法完成（环境问题，非素材）。
- **短启**：`ALIVE=True`（6 秒稳定），未触发 0xc0000374 堆损坏 → 素材兼容门禁过。
- **真机待办**（需用户启动 s9 服务端后）：世界地图开窗、右侧搜索输入→高亮/点击、悬浮 tip 三分区（怪/NPC/玩家）、传送扣费显示。

## 6. 与 Kaentake tip 文档的关系

文档 DLL 的 4 个零件当前端已有 3 个半（协议/life 索引/tooltip 骨架+已接通 mob/npc）。本次真正新增仅"玩家进 tooltip + 三分区样式"，其余复用现有组件，未引 DLL/hook/跨端运行时依赖。

---

## 7. 第二轮（2026-11-19）：素材补齐 + 顶栏三按钮完整实现

用户推翻"数据缺失就不做"：素材能补就补、需要接线；移除旧搜索/传送，192 功能与 UI 全移植。

### 7.1 怪物属性/抗性数据补齐（已实现）
- 从 192 解包 XML `MapleServer192/wz/Mob.wz/*.img.xml` 的 `elemAttr` 提取 **2797 条**（脚本 `extract_elem.py` → `_elem192.json`）。
- 解码规则由 `WzComparerR2/.../MobElemAttr.cs` 定案：I冰 L雷 F火 S毒 H圣 D暗 P物理；数字 0普通/1免疫/2抵抗/3弱。
- 生成静态 C++ 表 `Util/MobElemAttr.{h,cpp}`（72KB，二分查找），接入 `MapMobInfo.elem_attr`（`Misc.cpp:get_mobs_on_map`）→ `UIMapInfoPanel` 属性/抗性标签（攻击属性 I/L/F，抗性 S/H/D/P）。已加入 vcxproj。

### 7.2 顶栏三按钮（用户决策：全做）
- 素材 `BtSearch`/`BtNaviRegister`/`BtAutoFly_1`/`BtAutoFly_2`（99×16 四态）已在 `UIWindow2.img/WorldMap`，`UiAssetResolve::window("WorldMap")` 优先命中，构造函数已创建按钮。
- **搜索**（BT_SEARCH）：`set_search(!search)` 开合原生搜索轨（去掉 toggle_right）。
- **自动飞行**（BT_AUTOFLY）：用户决策"保留 !mapwarp 作底层" → `autofly_to_selected()` 对选中地图 `request_teleport()`（服务端 MapWarpCommand），带未选中/飞行提示。192 原生自动飞行是 64 位客户端本地寻路（s9 v83 无对应、scripts 搜不到 mapwarp/autofly），IDA 对该 exe 报 malformed，故以 !mapwarp 等价完成"到达"。
- **登记指路**（BT_NAVIREG）：`toggle_navi_register()` 本地收藏集合（会话内），再点取消，带提示。
- locale 新增 `.autofly_nosel/.autofly_go/.navireg_nosel/.navireg_add/.navireg_del`（zh/en）。

### 7.3 验证
- `BUILD_EXIT=0`（期间因其他并发会话改 opcode 触发 gate=2，重生 `gen_opcode_matrix.py` 后过）。
- 短启 `ALIVE=True` 无堆损坏。
- 真机待办（需 s9 服务端）：三按钮点击、搜索结果点击即走、属性/抗性非 `-`、自动飞行到达。

### 7.4 事实澄清
- 192 世界地图搜索/传送是**纯客户端功能**（192 scripts 无 WorldMap/mapwarp/autofly 引用）；elemAttr 数据在 192 WZ 有、当前端 v83 Mob.img 无（精简版）。
- 未新增任何协议/opcode：掉落/玩家/费用早已闭环，属性抗性改走本地数据表而非服务端。
