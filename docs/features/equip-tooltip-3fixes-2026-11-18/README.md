# 装备 tooltip 三问题修复（2026-11-18）

> 个人学习/技术研究用途，单机离线环境。证据驱动，未真机验收项一律标注。

## 0. 需求（用户逐条，2026-11-17）

1. 名称下方两个属性显示重叠，类似词条要保持相同上下间距（图1，扎昆头盔：所有者/要求等级叠字）。
2. 图2 红色框内属性值默认颜色偏灰，要显示成白色，和职业列表白色一样明显。
3. 图3 搜不到高版本装备，`!item 1000040/1003364/1003242` 报「物品id xxx 不存在」。

用户抉择：问题1/2 直接修+构建+部署具名 exe；问题3 名称来源由用户提供高版本客户端路径解决；明确要求服务端 XML 同步。

## 1. RCA（行号级，已验证）

### 问题1 — 词条与需求首行叠字
- 词条叠行 `IO/Components/EquipTooltip.cpp:1178-1197`（主窗）/ `:1397-1419`（对比窗）：`sub_y` 从 24 起，每行 `+=14`。
- 本例 trade(唯一道具/不可交易合并1行) + owner(所有者) 两行：`sub_y=24+14+14=52`，恰好等于 `HEADER_SHIFT=52`（`:178`），`pos.shift_y((sub_y>52)?sub_y:52)` 不下压。
- owner 画在 y38，A11M 中文字模下沿≈y50；需求首行"要求等级"画在 shift 后 y52（`:253` + `:1212`，`REQ_VALUE_DY=2`）——留白仅 2px，抗锯齿下沿一溢即叠字。图1 放大帧肉眼确认重叠。
- 根因：14px 步长对 A11M 中文字模下沿不足，且 `sub_y==HEADER_SHIFT` 卡边界不下压。

### 问题2 — 属性值偏灰
- 职业行：位图字模，实测纯白 `(255,255,255)`。
- 属性标签：`EquipTooltip.cpp:100` 写死 `Color::Name::TOOLTIPBODY` = `Color.h:154` `(255,238,204)` 奶油色，实测最亮 `(250,233,200)`。
- 属性数值：`EquipTooltip.cpp:962`/`:693` 已是 `Color::Name::WHITE`，但走 `Text::A11M` 抗锯齿后最亮 `(233,233,233)`。
- 两条渲染路径（位图字模 vs Text 字体）色差，整行观感比职业行暗一档。
- 注：图2 红框是用户标注框（RGB 216,74,47），框内 "(+4)" 本身是白字，不是红字。

### 问题3 — 搜不到装备
- 报错源：`BeiDou-Server_s9/.../i18n/message_zh_CN.properties:296` `ItemCommand.message3=物品id {0} 不存在`；触发点 `ItemCommand.java:56-57` `if (ii.getName(itemId)==null)`。
- 判空链：`ItemInformationProvider.java:1397-1418` `getName→getNameDesc→getStringData:208-290` 从 `String.wz/Eqp.img.xml` 取 `Eqp/{cat}/{id}` 节点。
- 实测：三 ID 装备模型 `Data/Character/Cap/01000040.img / 01003242.img / 01003364.img` 在客户端全有（与 BeiDou-Client_S9 同 hash）；但客户端 `String/Eqp.img` 与服务端两个 `Eqp.img.xml` 三 ID 名称节点 **0 命中**（orange-wz find_node「节点路径不存在」+ Python 精确计数）；`1322007` 两处都有（图2 能显示名字）。
- 根因：高版本装备只移植了 Character 模型 img，未移植 String 名称表条目 → `getName()==null` → GM 发装备报「不存在」。道具资源移植不完整（skill 门禁6/道具完整性）。

## 2. 修复

### 问题1/2（客户端 C++，`IO/Components/EquipTooltip.cpp`）
- 新增 `SUBLINE_STEP=18`（`:179` 附近），主窗/对比窗所有词条行 `+=14` 改 `+=SUBLINE_STEP`（6 处）；注释同步。
  - 2 词条时 `sub_y=60>52` 下压到 60：owner(y42，墨底≈54) 到需求首行(y60) 留白 6px，叠字消除；无/单词条 `sub_y≤42` 走 HEADER_SHIFT，视觉零变化。
- `BodyLine::build:100` 标签色 `TOOLTIPBODY` → `WHITE`，属性整行与职业行同为纯白观感。

### 问题3（资源，客户端 + 服务端双写）
- 名称来源：用户提供的高版本客户端路径中，192/095/079 三端明文 XML 对三 ID 名称**完全一致**（S9 同源端 orange-wz `get_node_detail` 交叉印证）：
  - `1000040` = 冰雪头盔
  - `1003242` = 冒险岛宝石贝雷帽
  - `1003364` = 传说冒险岛帽子
- 服务端（明文 XML，UTF-8，主用）：`BeiDou-Server_s9/gms-server/wz/String.wz/Eqp.img.xml` + `wz-zh-CN/String.wz/Eqp.img.xml` 的 `Cap` 段各插入 3 节点；备份 `.bak_eqp_names`；`minidom.parseString` 校验 XML 有效；读回名称正确。
- 客户端（加密 img，走 orange-wz MCP，128B key）：staging 副本 `Eqp.img` → `mutate_nodes` create_child 建 `Eqp/Cap/<id>` + `name`(STRING_PROPERTY) → `save_as` 到 `Eqp_patched.img` → get_node_detail 读回三名称正确。

## 3. 构建/部署证据（可复核）
- 构建：MSBuild `EXITCODE=0`，error C=0（仅 2 个无害 LNK warning）；假通过陷阱已堵：cpp mtime 00:03:13 < obj 00:09:29 < exe 00:10。
- 产物：`tooltipfix_int/MapleStory.exe` SHA256 `77e44a97b5870419b78f1ee3ef7a04d3ab777db65476f67a51e1b441f53844cd`。
- 客户端具名 exe：`E:\MXD\BeiDou-Client-xy\MapleStory_tooltipfix_20261118.exe`（SHA 与产物一致）；主 exe 未覆盖（运行中 PID 608/26292，避免占用/回滚）。
- 客户端具名资源：`E:\MXD\BeiDou-Client-xy\Data\String\Eqp.img.patched_20261118` SHA `28e92ef573dc9f1a7757e4e7a57904e3a4fc95e7c0e73543ec0cbefdaf28f1a7`；live `Eqp.img` 未动（SHA `b82daba9…`）。
- ACL 修复：x64/obj-Release 三目录写入权限经 `diagnose-windows-sandbox-acl` 授予并验证（报告 `.dsh-acl-out/acl-report-*.jsonl`）。

## 4. 部署闭环（2026-11-18 00:17 更新）
- **客户端 live 资源已替换**：结束客户端 PID 608（PID 26292 为无路径僵尸进程、不占用文件）后，`cp -f` 覆盖 `Data/String/Eqp.img`，实时 SHA = `28e92ef573dc9f1a7757e4e7a57904e3a4fc95e7c0e73543ec0cbefdaf28f1a7`（与补丁一致）。回滚备份 `backup/Eqp.img.original_b82daba9`（SHA `b82daba9…`）留存。
- **live 读回验证**：orange-wz 直接读 `E:/MXD/BeiDou-Client-xy/Data/String/Eqp.img`，三名称正确：冰雪头盔 / 冒险岛宝石贝雷帽 / 传说冒险岛帽子（非 staging 验证，是 live 当前文件）。

## 5. 未闭环 / 真机验收（如实声明）
1. **未真机截图验收**：三个问题均未在真机截图确认。问题1 行距以几何计算 + 图1 放大帧推导，问题2 色值以像素采样 + 色值常量推导；需用户切 `MapleStory_tooltipfix_20261118.exe`、重启服务端后截图确认：①词条与需求行不重叠且等距；②属性行与职业行同白；③`!item 1000040` 等三命令发出装备且 tooltip 显示正确中文名。
2. **服务端需重启/热更**：`ItemInformationProvider` 启动时加载 XML；运行中服务端可能缓存旧 nameDescCache，需重启服务端或确认热更机制后 `!item` 才生效。
3. 临时脚本 `tools/_probe_*.py`、`tools/_patch_*.py`、`tools/_eqp_staging/`、各 `build_*.log`、`.dsh-acl-out/` 为取证产物，可按需清理。

## 5. 相关
- 历史批次（不同问题）：`docs/features/equip-tooltip-3issues-2026-11/README.md`（差值/owner/095特性）、`docs/features/equip-tooltip-audit/`（装备分类/字段）。
- AGENTS.md：WZ/IMG/XML 走 orange-wz MCP；128B key + `exclusive=false`。
