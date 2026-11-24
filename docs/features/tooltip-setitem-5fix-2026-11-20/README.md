# 装备/套装 tooltip 五问题一次性修复（2026-11-20）

> 个人学习/技术研究，单机离线。逐条行号级结论；截图来源端未确认前不作本端验收凭据（USER.md）；真机未验收如实声明。

## 0. 用户报的 5 项与裁决

1. 几个装备 tooltip 顶部不在一条水平线
2. 套装装备有的只显示装备 ID
3. 套装 UI 按图2/高版本逻辑：套装名字居中、颜色、各阶段颜色
4. 装备 tooltip 底部空白有时长度不一
5. 角色装备栏头盔跑到右边槽位上

用户裁决：套装配色「按图2近似色」；一次性全改（不分批挤牙膏）。

## 1. 证据链（先证据后实现）

- orange-wz MCP（10012，128B key `tools/_gms_key_128.b64`，iv `TSPHKw==`）一手实读 live：
  - `Data/UI/UIWindow.img/ToolTip/Equip` 仅 11 子节点（Can/Cannot/Dot/WeaponCategory/ItemCategory/Speed/Property/mesos/cash/GrowthEnabled/GrowthDisabled），**无套装面板节点** → 套装 UI 100% 本端 C++ 自绘（`SetTooltip.cpp`），无原生资源可套。
  - `Data/UI/UIWindow.img/Equip/backgrnd` = 175×304，origin(0,0)，ARGB8888；烘焙「帽子」格 x≈39..70（与 FACE 列 x=38 同列）。
  - `Data/String/Eqp.img` 实查：`1052357 / 1072521 / 1082314` **无 name 节点**；`1002357 / 1040002` 有。→ 套装成员裸显 ID 的真因是 String 缺 name，`ItemData::get_name()` 返回空，旧代码回退裸数字。
- 用户四张截图按「问题描述」用，不按验收证据用；真机验收须用户确认来源端（本端 vs 参考端）。

## 2. 逐条根因与修复（行号为改后）

| # | 根因 | 修复位置 |
| --- | --- | --- |
| ① 顶部不齐 | 主窗背板用未偏移 pos，套装顶用 `pos.y()-height`；pos 被 draw 链反复 shift_y，height 是 set_equip 另一套启发式累加，两套独立 → 顶随内容漂移；对比窗各自夹 y | `EquipTooltip.cpp:1458-1461`（设计）、`:1501` base_pos（夹取后锚点）、`:1505` 对比窗同锚点起画、`:1781` 套装/成长顶 = base_pos.y()；`draw_preview` 内部 y 夹取移除（`:1815-1821`） |
| ② 套装成员裸 ID | String 缺 name → `ItemData::get_name()` 空 → 旧 `to_string(display_id)` | `SetTooltip.cpp:334-348` 改为 `Locale::format("setitem.unidentified", 部位名)`；locale 键 `zh-CN.ini:1152` / `en-US.ini:1116` |
| ③ 套装 UI | 标题 LEFT/WHITE；档位只有 reached?YELLOW:SILVER 无阶段分色 | `SetTooltip.cpp:150-166` `tier_header_color()`（2套浅绿/3套绿/4套黄/5套橙/≥6套黄，未达银灰）；`:286`+`:451` 标题 CENTER + YELLOW（服务端文案标题同样）；`:418` 档位标题用阶段色 |
| ④ 底部空白不一 | 背板高度用启发式 `height+32`，与实际绘制 shift 总和不等；desc 高度未计入 | `EquipTooltip.cpp:1510-1559` 主窗按同一 shift 链预算 `content_extent`，背板高 = 内容跨度+12；`:1830-1876` 对比窗同口径 `preview_extent` |
| ⑤ 头盔错槽 | `iconpositions[HAT]=(71,32)`，x=71 落在第三列空槽；原生底图烘焙「帽子」格 x≈39 | `UIEquipInventory.cpp:114-119` 改 `(38,32)`，与同列 FACE 对齐；tint/disabled 格同源自动随动 |

## 3. 构建验证

- 脚本：`tools/_tooltip_5fix_build.bat`（Release x64，`/p:PlatformToolset=v142` 覆盖本机缺失的 v145）。
- 结果：`tooltip5fix_build.log` → `MSBUILD_EXIT=0`，`error C` 计数 0，产物 `build-tooltip5fix-out/MapleStory.exe`，SHA256 `707CE08AEB85911C60FE2363375E0B684080FF0FFF5EB0C99B4155F42115AE67`，5098496 B（2026-11-20 22:17）。
- 部署（2026-11-20 22:28，用户明确要求「部署live」）：
  - live 目录 `E:\MXD\BeiDou-Client-xy\` 无 plain `MapleStory.exe`（登录器按 variant 名管理），按既有约定用唯一新名：
    `MapleStory_tooltip_5fix_20261120.exe`，**live 实测 SHA = 707CE08AEB85911C60FE2363375E0B684080FF0FFF5EB0C99B4155F42115AE67**（与 build 完全一致，已实时核对，防登录器回滚）。5,098,496 B。
  - i18n 键需随行部署（CWD 相对 `locale/*.ini`）：`locale/zh-CN.ini` SHA `B7B0814F…F2ECF9F1`、`locale/en-US.ini` SHA `ED5FA6CF…D303F07B08`（与 build 一致）；覆盖前已备份为 `*.bak_before_tooltip5fix_20261120`。仅新增 `setitem.unidentified` 键，对其它 variant exe 无副作用。
  - 未触碰 `MapleStory_diag.exe`（并行会话的崩溃诊断产物，SHA `1A9FDC1C…`，非本构建）及其它 variant exe。
- **未做真机截图验收**（登录器会回滚 exe，部署声明以本次实时 SHA 为准）。

## 4. 真机验收清单（待用户，截图须确认本端）

1. 同时悬停「已穿帽 + 套装装备」：主窗/套装/成长/对比四片顶沿同一水平线。
2. 套装成员行：`1052357` 等由裸 ID 变为「部位（未鉴定）」。
3. 套装面板：标题居中暖金；2~7 套档位标题按阶段分色，未达档位银灰。
4. 不同道具（有/无 desc、潜能、星）：tooltip 底部留白恒定。
5. 角色装备栏：头盔图标落在「帽子」格内，与额饰同列。

## 5. 非目标 / 约束

- 不改 `EquipSlot` 枚举与协议（slot 不变量见 MEMORY.md；本次只改 UI 坐标）。
- 不新增 WZ/IMG 节点（套装面板无原生节点，纯代码绘制）。
- 未部署 live；登录器可能回滚 exe，部署前须实时核对 SHA。
