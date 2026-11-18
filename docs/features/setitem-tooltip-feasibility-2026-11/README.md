# 套装系统 / 套装 tooltip 依附装备 tooltip — 可行性与链路闭环分析（2026-11-17）

> 个人学习与技术研究用途，单机离线环境。证据驱动；凡未真机验收项一律标注，不拿参考端截图当本端证据。
> 本文最初是**可行性研究 + 逻辑链路分析**；2026-11-17 已按两轮决策完成实现，见文末 §9 实现记录。§1–§8 保留为历史取证与分析。

## 0. 需求与边界

- 原始需求（m00002）：研究 `E:\MXD\自定义功能实现\套装系统`、`E:\MXD\扩展改动\套装系统` 的实现可行性及开发；"装备套装 tooltip 依附查看的装备 tooltip"的具体逻辑，看文档 + 095 + 公开资料全面分析；要求逻辑、功能链路闭环。
- 本次交付：①资料溯源 ②三套方案对比 ③本端现状行号级链路 ④完整功能链路 ⑤可行性结论 ⑥决策点 ⑦一次性开发 Plan。
- 未做：不修改任何业务代码；不碰工作区已有的"装备 tooltip 三问题"未提交改动。

## 1. 资料溯源（已读，可复核）

| 来源 | 路径 | 关键内容 |
| --- | --- | --- |
| 主资料目录 | `E:\MXD\自定义功能实现\套装系统`（19 文件） | 服务端 Java（SetItemManager/SetBonus/SetTiersV2Parser/…）、插件 C++（setitem.cpp/SetItemBridge.cpp）、SQL 迁移、参考 README、最终实现方案.md |
| 精简副本 | `E:\MXD\扩展改动\套装系统`（5 文件） | README + 3 个桥接文件；README 与主资料 md5 相同 `5c9725ff…`，同源 |
| 设计文档 | `…\参考资料\README.md`（316 行） | 架构/协议/布局/时序/坑，权威说明 |
| 落地方案 | `…\最终实现方案.md`（48 行） | 来源 `BeiDou-Server_xy` + `BeiDou-ijl15`，"照搬开发"步骤 |
| 095 参照端 | `E:\MXD\【2021最新】095PLUS2_10.1国庆版` | 服务端仅 3 个套装礼包脚本（三国/漫新/考古），非系统核心；客户端 `开启游戏.vmp.exe` VMP 壳，静态不可反编译 |
| 公开资料 | GMS083 社区 | 存在公开的 GMSv83/KMS IDB、GMSv95 PDB，`CUIToolTip`/`ShowItemToolTip` 地址可查（见末尾链接） |
| 本端现状 | `E:\project\MapleStory-Client` | commit `bfd00b00`（2026-11-13）已落地套装数据+tooltip 组件；工作区另有进行中的定位修复 diff |

**资料自述的关键裁决**（README 第 6/17/19/39 行）：装备 tooltip 保持原版，不做自定义正文改写（自定义 tip 字段不全、曾致悬停闪退，实验钩子已关闭）；套装 tip 是**另建的 companion 面板**，只做展示，战斗加成权威在服务端。

## 2. 三套方案对比（核心差异）

| 维度 | 资料方案（ijl15.dll 插件） | 本端方案（当前 HEAD） | 095 原生 |
| --- | --- | --- | --- |
| 装备 tip | 原版不动，Detour hook 注入 | 原生 C++ 源码内建改造 | VMP 壳，静态不可取证 |
| 套装 tip 载体 | 另建 `g_setTooltipBuf` + `SetToolTip_String2` 撑底板 + 自绘 | `SetTooltip` 类作为 `EquipTooltip` 成员，`draw()` 末尾叠加 | 未知（WZ 结构不同，orange-wz 搜 ToolTip 0 命中） |
| 依附机制 | hook `ShowItemToolTip`(0x8F5B20)/`DrawToolTip_Equip`/`ClearToolTip` | C++ 组合：`set_item()` 喂数据 + `draw()` 定位绘制 | 未知 |
| 数据源 | WZ `Etc/SetItemInfo.img` + DB `set_item` 表 | 本地 `Data/SetItem.txt` 花名册 | 未知 |
| 加成权威 | 服务端 `SetItemManager` + 协议 0x175/176/177/179 | 客户端 `Player::apply_set_item_bonuses` 本地结算 | 未知 |
| 面板顺序 | `[悬停][套装][对比]`，套装在 `mainX+mainW+4` | `[悬停][对比][套装]`，套装在 `pos+FRAME_WIDTH+18`，右溢出翻左 | 未知 |

**四点关键差异**（可行性判断的抓手）：
1. **实现形态**：插件黑盒 hook vs 源码内建。本端是源码可改，不需要 DLL hook，稳定性更好。
2. **数据源**：资料走 WZ+DB；本端因 live `Etc/` 无 `SetItemInfo.img` 节点（MCP 取证）+ s9 服务端无 xy 的 combat.stat 框架，退化为本地 txt 花名册。
3. **加成权威**：资料服务端权威；本端纯客户端本地结算，**无协议包、无服务端校验**。
4. **布局顺序**：资料套装在中间，本端套装在最右——观感与资料不一致，需用户裁决。

## 3. 本端现状：行号级链路（已接线，非半成品）

数据层 → 计算层 → 展示层 → 依附层 → 启动加载，五段全通：

| 环节 | 文件:行号 | 事实 |
| --- | --- | --- |
| 花名册解析 | `Data/SetItemData.cpp:33-107` | `load()` 读 `Data/SetItem.txt`，解析 `SET/ITEM/TIER` 三类行；缺文件优雅降级为"无套装" |
| item→set | `Data/SetItemData.cpp:109-113` | `set_of(item_id)`，0 = 非套装件 |
| 去重计数 | `Data/SetItemData.cpp:121-137` | `count_equipped()`，同 itemid 多件（双戒指）只计一次 |
| 达标档位 | `Data/SetItemData.cpp:139-152` | `active_tiers(set_id, count)` 返回所有 `count>=tier.count` 的档位 |
| 属性注入 | `Character/Player.cpp:115-162` | `apply_set_item_bonuses()`：ALLSTAT→四维；DAMAGE/BOSS/IGNOREDEF→百分比；其余走 `set_stat_id`→EquipStat |
| 面板数据构建 | `IO/Components/SetTooltip.cpp:58-128` | `set_item()`：标题 `套装名(n/N)` + 每档一行 `N件套：属性 +值`，档位达成高亮 |
| 面板绘制 | `IO/Components/SetTooltip.cpp:130-150` | `draw()`：TooltipBackdrop + 标题 + 分隔线 + 档位行，`FRAME_WIDTH=200` |
| 成员挂载 | `IO/Components/EquipTooltip.h:21,176` | `#include "SetTooltip.h"` + `SetTooltip settip;` |
| 喂数据（依附点1） | `IO/Components/EquipTooltip.cpp:620-626` | `set_equip()` 内 `settip.set_item(item_id, equipped_snapshot)`；`equipped_snapshot` 来自 `:293` `Stage…get_inventory().equipped_item_ids()` |
| 变更重算 | `IO/Components/EquipTooltip.cpp:290-306` | `snapshot_hash`（FNV 式）纳入早退判定，换装后同槽位也强制重算套装件数 |
| 宽高夹取（依附点2） | `IO/Components/EquipTooltip.cpp:1034-1043,1249-1250` | 主窗与 preview 都把 `SetTooltip::FRAME_WIDTH` 计入右边界 |
| 绘制定位（依附点3） | `IO/Components/EquipTooltip.cpp:1218-1235` | `draw()` 末尾：顶端 `tip_top=pos.y()-height`，X 在对比窗/主窗右侧 +18；右溢出翻到左侧 |
| 启动加载 | `MapleStory.cpp:19,158` | `SetItemData::load("")` |
| git 落地 | `bfd00b00`（2026-11-13） | `feat(item): 套装属性数据与套装 tooltip 对齐天使客户端（SetItem/SetTooltip）`，6 文件 +646 行 |

**结论**：本端套装系统不是"没做"，而是**源码内建、链路完整**。所谓"又失败"更可能指向：①当前工作区正在修的定位问题（375 行未提交 diff，把面板从 `[悬停][套装]` 改成 `[悬停][对比][套装]` 并加 6px 缝隙/右翻）；②真机未验收（无截图证明面板实际出现/不重叠）；③与资料方案的观感/数据源/权威性不一致。

## 4. 完整功能链路（闭环视图）

```
[启动] MapleStory.cpp:158 SetItemData::load("")
   └─ Data/SetItem.txt → definitions / item_to_set

[悬停套装装备] UI 调用 EquipTooltip::set_equip(parent, ivp)   EquipTooltip.cpp:288
   ├─ :293 取已穿戴快照 equipped_item_ids
   ├─ :303 snapshot_hash 早退判定（换装强制重算）
   ├─ :620 item_id = equip.get_item_id()
   └─ :626 settip.set_item(item_id, equipped_snapshot)
        └─ SetTooltip.cpp:64 set_of → :69 get → :74 count_equipped → :92 active_tiers
             → 生成 title + tier_lines，is_valid=true

[绘制] EquipTooltip::draw(pos)
   ├─ :1034 套装面板宽度计入视口夹取
   ├─ :1218 if(settip.valid())
   │    ├─ 顶端 tip_top = pos.y() - height
   │    ├─ X = 对比窗右侧(有preview) / 主窗右侧 + 18
   │    ├─ :1231 右溢出 → 翻到主窗左侧
   │    └─ :1234 settip.draw(setpos)  ← 套装面板落在装备 tip 右侧
   └─ draw_preview :1249 同步计入宽度（不重复绘制套装面板）

[穿戴变化] Player::recalc_stats → apply_set_item_bonuses  Player.cpp:115
   └─ set_of → count_equipped → active_tiers → stats.add_* （加成与面板同源）
```

**"依附"的本质**：不是独立窗体，而是 `EquipTooltip` 的一个成员组件，生命周期与装备 tip 绑定（`set_equip` 喂数据、`draw` 尾部按装备 tip 的 `pos/height/FRAME_WIDTH` 计算锚点），换装/换悬停由 `snapshot_hash` 驱动重算，右边界与主窗共用夹取。这与资料里 DLL hook 方案语义一致（都是"挂在装备 tip 右侧的 companion"），只是实现形态不同。

## 5. 可行性结论

| 目标 | 可行性 | 依据 |
| --- | --- | --- |
| 本端源码内建套装 tooltip 依附装备 tooltip | **已实现、链路闭环** | 第 3 节 14 处行号证据，HEAD 已接线 |
| 做到"观感 + 数据源 + 权威性"完全对齐资料方案 | **可行但需改造** | 数据源 WZ+DB、服务端权威协议在本端缺失，需服务端 s9 配套 |
| 对齐 095 原生 | **硬阻塞** | 客户端 VMP 壳静态不可反编译；WZ 结构与 v83 不同；仅公开 PDB/IDB 可查地址，无法取证原生 tooltip 逻辑 |
| 真机验收当前定位修复 | **待办** | 工作区 375 行 diff 未提交、未构建、无本端截图；登录器回滚 exe 需实时核对 SHA |

**一句话**：功能链路本身可行且已接通；真正的不确定在"对齐到哪个基准"（本端构型 / 资料 xy 方案 / 095 原生）和"数据源与权威性要不要升级到服务端"，这两点必须用户拍板，不能靠猜。

## 6. 待用户拍板（决策点，阻塞实现）

1. **范围**：本次仅交付"可行性 + 链路分析"（当前默认），还是要我继续把套装 tooltip 做到真机闭环（构建 + 本端截图验收 + 修定位）？
2. **数据源路线**：保持本地 `Data/SetItem.txt` 花名册，还是对齐资料的 WZ `SetItemInfo.img` + DB `set_item` 权威表？
3. **加成权威**：保持客户端本地结算，还是接服务端 `SetItemManager` + 协议 0x175/176/177/179（需改 BeiDou-Server_s9）？
4. **依附顺序**：保持本端 `[悬停][对比][套装]`，还是改回资料的 `[悬停][套装][对比]`？
5. **095 对齐**：095 原生 VMP 壳不可取证，是否接受"按本端构型做，不追求 095 逐像素对齐"？

## 7. 一次性开发 Plan（确认后按序落地，plan 外改动先问）

> 前置：先按第 6 节拍板；涉及 WZ/IMG 走 orange-wz MCP（10012 在线），不裸拷；涉及 DB 先问 dbx 通道且默认只读。

- **P1 现状固化**：读透工作区 375 行 EquipTooltip diff，确认套装面板定位修复是否 intend；如与用户目标一致则纳入，否则先还原到 HEAD 干净态。
- **P2 数据源**（按决策 2）：保持 txt 则补完整套装条目 + 校验解析；升级 WZ+DB 则经 MCP 导出 SetItemInfo.img 节点 + 设计 DB 迁移 + 双端同步。
- **P3 加成权威**（按决策 3）：本地则核对 `apply_set_item_bonuses` 百分比口径与资料 damR/fdR；服务端则移植 SetItemManager + 4 个 opcode + Handler，并在 s9 侧找对应注入点（先 REA/IDA 取证，禁止想象改）。
- **P4 依附布局**（按决策 4）：调整 `EquipTooltip.cpp:1218-1235` 锚点顺序 + 宽高夹取，含右溢出翻左、多显示器边界。
- **P5 验证门禁**：touch 源文件 + 删 `x64/obj-Release/<文件>.obj` 强制重编（本仓库 MSBuild 假通过陷阱）；error C=0；obj mtime>cpp mtime；部署 exe SHA 相对基线 `0c3bf5db…` 变化；真机 hover 套装件（本端截图，非参考端）确认面板出现/不重叠/件数正确；不达标/非套装件无残留。
- **P6 产物**：更新本文为最终实现文档，归档 evidence（行号/节点/封包摘要），临时调试日志清理，仅保留上线级节点日志。
- **回滚**：单功能原子 commit 前可 `git checkout -- <file>`；不改服务端/协议时爆炸半径仅限客户端 UI。

## 8. 公开资料（外部数据，仅作线索）

- [冒险岛怀旧服 GMS083 CUIToolTip 地址与公开 IDB/PDB 说明](https://moguwuyu.com/d/61-gms083-%E5%B1%8F%E8%94%BD%E8%A3%85%E5%A4%87%E7%AD%89%E7%BA%A7%E5%92%8C%E9%87%91%E9%94%A4%E5%AD%90%E6%98%BE%E7%A4%BA/2)
- [MapleStory N — Set Items 官方说明（套装效果触发/叠加口径）](https://docs.maplestoryn.io/msn-101/beginners-guide/item-and-equipment/set-items.md)

## 9. 实现记录（2026-11-17）

### 9.1 用户决策（已拍板，按此实现）

| 决策点 | 最终裁决 |
| --- | --- |
| 范围 | 做到真机闭环（构建 + 本端截图验收） |
| 数据源 | 改为读 WZ `Etc/SetItemInfo.img`（nlnx 运行时读），不再以 txt 为主源；txt 降级为兜底 |
| 加成权威 | 保持客户端 `Player::apply_set_item_bonuses` 本地结算，不动服务端/协议 |
| 依附布局 | 改回资料构型 `[悬停][套装][对比]`，套装在中间 |
| 095 对齐 | 接受本端构型，不追 095 逐像素 |

二次裁决：①属性键只映射本端支持子集，其余跳过并标注；②在现有工作区改动上叠加，只改套装相关行。

### 9.2 改动清单（5 个文件）

| 文件 | 改动 |
| --- | --- |
| `Data/SetItemData.h` | 头注释标数据源；新增 `static bool load_from_wz();` |
| `Data/SetItemData.cpp` | 新增 `load_from_wz()`：经资源门面取 Etc 根，读 `SetItemInfo.img`，白名单映射 inc* 键，构建 definitions/item_to_set；`load()`(txt) 原样保留兜底 |
| `MapleStory.cpp:157-163` | 启动改 `if (!load_from_wz()) load("")`，失败降级 txt |
| `IO/Components/EquipTooltip.h` | 新增 `set_aware_preview_offset()` 声明 |
| `IO/Components/EquipTooltip.cpp` | 新增 `set_aware_preview_offset()` 定义（:43）；宽高夹取（:1149/:1152）、套装定位（:1354）、preview 位移（:1374）改为套装在中间的布局 |

> **git 状态如实说明**：上表 5 文件是"本次套装实现涉及的全部改动"。但当前仓库为并行协作态——EquipTooltip 的布局改动已由并行流程收进 HEAD（commit `406abe0a` 17:28:17，`set_aware_preview_offset` 首次出现处），故工作区 `git status` 仅剩 3 个文件 modified：`Data/SetItemData.cpp`、`Data/SetItemData.h`、`MapleStory.cpp`（数据源升级部分）。**未擅自提交**（用户未要求 commit）；v3 构建（17:34–17:37）在两处改动均已入源码后进行，产物 `eeaf53a` 包含全部 5 文件改动。

### 9.3 数据源与属性映射

- WZ 源：`E:\MXD\BeiDou-Client-xy\Data\Etc\SetItemInfo.img`（233,149 B，625 个 set），全量节点树已导出 `raw-seteiteminfo-tree.json`（6.6 MB）留证。
- 映射白名单（inc* → 本端 stat_key）：incSTR/DEX/INT/LUK、incMHP→HP、incMMP→MP、incPAD→WATK、incMAD→MAGIC、incPDD→WDEF、incMDD→MDEF、incACC→ACC、incEVA→AVOID、incSpeed/incSPEED→SPEED、incJump/incJUMP→JUMP、incAllStat→ALLSTAT、damR→DAMAGE。
- 跳过（本端模型不支持，不导入）：`activeSkill`/`Option`/`OptionToMob`（LIST）、`incMHPr`/`incMMPr`（百分比 HP/MP）、`nbdR`、`incPQEXPr`、`jokerPossible`。跳过项计数记入 `load_from_wz()` 的 `skipped`。
- 名称编码：`setItemName` 与装备名（ItemData.cpp:87）共用 `get_string()` + `Text` 渲染管线，同构自洽；WZ 导出树中名称已验证为正确中文（如「强力漫步者套装」），最终以真机截图确认无乱码。

### 9.4 关键缺陷修复：lazy 根假闭环

- **问题**：初版 `load_from_wz()` 直接读 `nl::nx::Etc["SetItemInfo.img"]`。但本端生产是堆式 ImgBackend（`Data/*.img` 目录布局），Etc 是 lazy 根（`ImgSource.cpp:51`），boot 只装登录根、预热只装选角根，`nl::nx::Etc` 在 `load_from_wz()`（Stage::init 前）尚未绑定 → 空节点 → 静默降级 txt → WZ 改造形同虚设。
- **证据**：`Backends.cpp:189-195` `ImgBackend::root()` 内含 `ImgSource::ensure(id)` 负责按需装载 lazy 根；裸读全局变量绕过此门面。
- **修复**：`load_from_wz()` 改用 `res::ResourceService::get().root(res::RootId::Etc)` 取根（SetItemData.cpp:43 附近），兼容 img（按需 ensure）/ wz / nx 三后端。

### 9.5 构建与部署证据（可复核）

| 项 | 值 |
| --- | --- |
| 基线 exe SHA（= live 主 exe） | `ff156c9aea4789d6d2bf3d0a950a3e37c8790c7e22c4c0460f3b52f1cf692c63` |
| 构建命令 | `MSBuild.exe MapleStory.sln /t:Rebuild /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v145`，OutDir=`setitem_build_out\` IntDir=`setitem_build_int\` |
| v3 结果 | exit 0；`build_setitem_v3.log` error C=0 / error LNK=0 |
| 新 exe SHA | `eeaf53acb1b23148c193369d5e4f26fc54169672cf2a6d5abb612722d2481314`（≠ 基线） |
| 强制重编核验 | obj `setitem_build_int/SetItemData.obj` mtime 17:37:57 > cpp `Data/SetItemData.cpp` mtime 17:34:21（本仓库增量假通过陷阱已规避） |
| 部署副本 | `E:\MXD\BeiDou-Client-xy\MapleStory_setitem_v3.exe`（SHA 同上，与构建产物字节相等） |
| 主 exe | 保持基线未覆盖（客户端 PID 20076 运行中，避免占用/回滚；切换与验收交由用户） |

### 9.6 未闭环项（如实声明，不伪装完成）

1. **真机 hover 截图验收**：agent 无 GUI 操作权，无法本机采图；请用户用本端 exe 悬停套装件截图，确认：①套装面板出现在 `[悬停][套装][对比]` 中间位、不重叠；②套装名中文无乱码；③件数/属性档位正确（对照 WZ `SetItemInfo.img`）；④非套装件无残留面板。**须确认截图来源为本端，不是参考端/095**。
2. **运行期日志核验**：Release 下 `LOG_DEBUG` 为 no-op；若要确认 WZ 源真的命中（而非降级 txt），需临时开 debug 日志看 `SetItemData: WZ loaded ...`，或真机确认套装数据来自 WZ（如 txt 中不存在的套装能显示）。
3. **登录器回滚**：切换 exe 后实时核对 live `MapleStory.exe` SHA 是否为 `eeaf53a`；登录器可能自动回滚到基线。
4. **临时文件**：`build_setitem_v2.bat`/`build_setitem_v2.log`/`build_setitem_v3.log`、`raw-seteiteminfo-tree.json`（6.6 MB 取证留档，可按需清理）为本次构建/取证产物，非交付代码。
