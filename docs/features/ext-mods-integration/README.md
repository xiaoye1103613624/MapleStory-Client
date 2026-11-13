# 扩展改动（E:\MXD\扩展改动）接入可行性分析与实施计划

> 目标栈：客户端 `E:\project\MapleStory-Client`（源码级 C++，SDL3 + nlnx，MSBuild 构建）＋ 服务端 `E:\project\BeiDou-Server_s9`（OdinMS 派生 v83 Java，登录端口 8484，Java21 + MySQL beidou）。
> 生成日期：2026-11-06。状态：分析/计划（未实施代码改动）。

## 0. 核心结论

`E:\MXD\扩展改动` 这批约 **70 个**扩展功能，原本针对**另一套代码栈**（原版 v83 零售编译后**二进制客户端** + OdinMS 系**服务端**）编写。与当前运行栈存在**代码栈错配**：

- 当前客户端是**源码级重建**（无原版固定地址，UI 全部在 `IO\UITypes` 下以 `ms::UI*` 组织），原版 `CUI*/CWnd*` 类与 `0x004E80EB` 这类硬编码地址**在本工程无意义**。
- 当前服务端 `s9` 已是 **OdinMS 派生 v83**，协议/Opcode 已与客户端逐字节对齐，Java 层 diff 成本最低。
- 资源（WZ/img）以运行时部署 `Data/<ns>/*.img` 为准（工程红线 R1/R2/R5/R6）。

因此**不能直接套用二进制补丁**，必须按可移植性重新分级。分级为 **A 纯资源 / B 服务端 Java / C 客户端源码 / D 二进制补丁（需重写为源码级）/ E 数据库迁移** 五类。

## 1. 分类判定规则

| 特征 | 归类 |
| --- | --- |
| 目录仅含 `.img/.png/.webp/.xml/.wz`（可能附带 `.js` 地图/NPC 脚本） | **A 纯资源**（落地 `Data/` + 运行时节点校验） |
| 含 `PacketCreator`/`AbstractCharacterObject` 风格 `.java`、`.patch`、`.sql` | **B 服务端 Java** +（有 `.sql`）**E** |
| 含 `.cpp/.h` 且引用原版 `CUI*/CWnd*` 类 | **C 客户端源码**（映射到本工程 `IO\UITypes`） |
| 含 `.md` 描述硬编码地址（`0x00…`）、`Detours`/`PatchCall`/`FakeTear` 字样 | **D 二进制补丁**（必须源码重写） |
| 含 `.sql` / 涉及库表 | **E 数据库迁移**（对齐 s9 Flyway `Vx.x` 迁移） |

多栈功能取**主类 + 次类**标注（如 `C+B+E`）。

## 2. 全量功能清单与分级矩阵

难度 1（拖放即用的资源）~ 5（需重写整块逻辑/跨栈强耦合）。风险：低/中/高。

### A 类 — 纯资源（主类 A，最易移植）

| 功能 | 关键文件 | 难度 | 风险 | 备注 |
| --- | --- | --- | --- | --- |
| 111款阿尔泰皮肤分享（GMS083适配） | img=112 xml=113 js=3 | 1 | 低 | GMS083 皮肤；js 可能为 NPC 脚本（同属 A/轻 B） |
| 200级武器 | img=28 | 1 | 低 | 武器贴图；itemid 范围需服务端配表 |
| 4傻BOSS+地图配套 | img=26 js=4 wz=6 | 2 | 中 | BOSS+地图资源；js 为地图脚本，需服务端加载 |
| 动漫皮肤 | img=96 xml=1 | 1 | 低 | 皮肤贴图 |
| 对话框 | png=132 | 1 | 低 | UI 对话框图集 |
| 按键 | png=376 | 1 | 低 | 按键图标 |
| 花语自由市场 | img=24 xml=24 | 2 | 中 | 自由市场场景资源 |
| 适用于083时装 | wz=15 txt=1 | 2 | 中 | 时装 WZ（cash/equip 兼容） |
| 无尽辉耀装备 | img=15 xml=15 | 2 | 中 | 装备贴图+描述 |
| UIMesoMarket_金币市场.img | png=58 | 2 | 中 | 金币市场 UI |
| Classic | ini=85 wz=171 | 2 | 中 | Classic UI 数据 |
| 妖精学院+列娜海峡 | img=222 js=89 xml=228 | 3 | 中 | 地图资源+脚本；体量大 |
| 遗忘山谷 | img=119 xml=111 | 3 | 中 | 地图资源 |
| 遗忘山谷+未来之门 | img=267 js=50 xml=278 | 3 | 中 | 地图资源+脚本 |
| 左侧小图标 | png=8842 | 1 | 低 | 小图标集 |
| 图片素材 | png=2 webp=5 | 1 | 低 | 杂项素材 |
| 卡片名称 | img=1 xml=1 | 1 | 低 | 怪物卡名称资源 |
| 高版本怪物卡 | img=2 txt=1 | 1 | 低 | 仅资源；卡牌**系统**逻辑需 C 类单独实现 |
| 修复名片错位NameTag | img=1 | 1 | 低 | 资源；**实际坐标修复在 C 类客户端源码** |
| 装备染色功能 | img=1 | 1 | 低 | 资源；逻辑在「装备染色」 |
| 装备悬停已装备对比功能 | png=1 txt=1 | 3 | 中 | 功能描述+素材；逻辑为 C 类 |
| 显示装备具体信息 | txt=1 | 3 | 中 | 功能描述；逻辑为 C 类 |
| 玩家商店加长 | png=1 webp=1 | 2 | 中 | 资源；商店 UI 拉长 |
| 商店列表加长 | webp=1 | 1 | 低 | 资源 |
| 导入高版本现金物品 | xml=1 | 2 | 中 | 现金物品定义；需服务端 item 表 |
| 一些脚本 | js=201 jse=1510 | 3 | 中 | NPC/服务端脚本；须对照 s9 脚本引擎适配 |

### B 类 — 服务端 Java（主类 B）

| 功能 | 关键文件 | 难度 | 风险 | 备注 |
| --- | --- | --- | --- | --- |
| 美容院 | java=12 sql=6 wz=13 cpp=3 h=24 | 5 | 高 | 跨 A+B+E+C；样例见 classification.md |
| 角色槽位 | java=1 sql=2 cpp=2 | 4 | 高 | 跨 C+B+E；样例见 classification.md |
| 每日签到 | java=3 sql=1 cpp=1 | 3 | 中 | 跨 C+B+E |
| 天气系统 | java=16 patch=11 cpp=14 | 4 | 高 | 跨 C+B+A |
| 世界地图tip | java=1 cpp=8 h=5 | 3 | 中 | 跨 C+B |
| 怪物卡片收集张数搜索 | java=3 cpp=4 | 4 | 中 | 跨 C+B |
| 个人伤害统计 | patch=5 cpp=23 h=35 | 4 | 高 | 跨 C+B；服务端伤害聚合广播 |
| 装备染色 | java=2 cpp=2 h=2 | 4 | 中 | 跨 C+B |
| GMS083 Kaentake 聊天表情 | java=5 cpp=5 h=3 | 3 | 中 | 跨 C+B |
| cashshop-window | java=4 cpp=1 h=1 | 4 | 高 | 跨 C+B+A |
| 收纳背包 | java=16 sql=2 cpp=3 | 3 | 中 | 跨 C+B+E |
| 套装系统 | java=1 cpp=1 h=2 | 4 | 中 | 跨 C+B |
| 商店回购 | java=2 cpp=1 h=1 | 3 | 中 | 跨 C+B |
| 团队buffer显示 | java=3 cpp=1 | 3 | 中 | 跨 C+B |
| 背包格子锁定 | java=2 cpp=1 | 3 | 中 | 跨 C+B |
| 掉落特效 | java=1 cpp=2 | 3 | 中 | 跨 C+B |
| 死亡次数 | img=2 md=1 | 3 | 中 | 功能；资源+描述 |

### C 类 — 客户端源码（主类 C，映射到 `IO\UITypes`）

| 功能 | 关键文件 | 难度 | 风险 | 备注 |
| --- | --- | --- | --- | --- |
| 融合外观 | cpp=113 h=64 idl=18 sln/vcxproj/rc | 5 | 高 | **完整客户端 mod 工程**，目标源码树不明，须先确认其基线 |
| 伤害皮肤 | cpp=110 h=62 idl=18 | 5 | 高 | 同上，疑似 DLL/插件工程 |
| 美容院 | cpp=3 h=24 | 5 | 高 | 跨 B+E |
| 角色槽位 | cpp=2 | 4 | 高 | 跨 B+E |
| 天气系统 | cpp=14 h=7 inc=8 | 4 | 高 | 跨 B+A |
| 世界地图tip | cpp=8 h=5 | 3 | 中 | 跨 B |
| 怪物卡片收集张数搜索 | cpp=4 | 4 | 中 | 跨 B |
| 个人伤害统计 | cpp=23 h=35 | 4 | 高 | 跨 B |
| 装备染色 | cpp=2 h=2 | 4 | 中 | 跨 B |
| cashshop-window | cpp=1 h=1 | 4 | 高 | 跨 B+A |
| 装备增加技能 | cpp=2 h=4 | 4 | 中 | — |
| 自定义角色属性 | cpp=1 | 4 | 中 | — |
| 套装系统 | cpp=1 h=2 | 4 | 中 | 跨 B |
| 扩展装备栏 | cpp=1 png=10 | 3 | 中 | 样例见 classification.md |
| 背包格子锁定 | cpp=1 | 3 | 中 | 跨 B |
| 每日签到 | cpp=1 | 3 | 中 | 跨 B+E |
| 商店回购 | cpp=1 h=1 | 3 | 中 | 跨 B |
| 团队buffer显示 | cpp=1 | 3 | 中 | 跨 B |
| 跳跃释放技能 | cpp=1 | 3 | 中 | — |
| 拖拽调整背包大小 | cpp=1 | 3 | 中 | — |
| 护肩槽 | cpp=1 | 4 | 中 | — |
| 左侧快捷栏 | md=2 | 3 | 中 | 功能描述 |
| quicklogin | cpp=1 h=1 | 2 | 低 | 快速登录客户端 mod |
| buffer计时 | cpp=1 | 3 | 中 | — |
| Bot UI | cpp=1 dll=2 exe=2 | 3 | 高 | **疑似外挂 UI，不建议移植**（见决策点） |
| 扩展背包 | png=2 txt=2 | 3 | 中 | 仅素材/描述，逻辑待补 |
| 更高的仓库列表 | wz=1 txt=2 | 3 | 中 | 描述 |
| 超技能 | zip=1 | 5 | 高 | v83 无原生超技能，需自建系统 |
| 修复名片错位NameTag | img=1 | 3 | 中 | 资源已给；坐标修复在客户端源码 |

### D 类 — 二进制补丁（必须源码重写，主类 D）

| 功能 | 关键文件 | 难度 | 风险 | 备注 |
| --- | --- | --- | --- | --- |
| 99999GW_CharacterStat（HP/MP 上限） | 根目录 `.md` + `src/maxHPmaxMP.cpp` | 4 | 高 | **确有业务价值**；客户端 `CharStats` uint16→int32 源码重写 + 收发包字节宽同步（详见 classification.md） |
| 血量突破上线 | `.md` | 4 | 高 | 与 99999GW **重叠**，合并处理 |
| 金币超上线 | `.md` `.txt` | 3 | 中 | meso 上限（int）；跨 B+E |
| 解锁300级 | `.md` `.txt` | 4 | 高 | 等级上限；跨 B+E（服务端等级常量+DB） |
| 伤害上限 | `.md` | 3 | 中 | 伤害值上限（通常已 int） |
| 083多分辨率 | `.md` `.png` `.txt` | 5 | 低 | **价值低**：本客户端 SDL3 可原生支持多分辨率，原版补丁无复用价值 |
| 伤害玩法 | `.md`×3 | 4 | 中 | 功能描述；逻辑待定 |
| 角色摇骰子 | dll/exe/ini | — | 高 | 二进制工具（非源码），**不移植** |

### E 类 — 数据库迁移（次类，依附于 B）

- 美容院 `sql=6`、角色槽位 `sql=2`、每日签到 `sql=1`、收纳背包 `sql=2`。
- 须与 s9 的 Flyway 迁移（如 `V1.7.0__create_game_config.sql`）对齐，**禁止 DDL/DML 冲突**；涉及库内数据须用户授权（DB 红线）。

## 3. 须剔除的 agent 既往产物（非扩展核心）

根目录以下文件为 agent 既往 **Stat 窗背景扩展**工作脚本/产物，已从功能清单剔除：
`_b64_only.txt`、`_build_stat_backgrnd2.py`、`_chk_stat_backgrnd.py`、`_chk_wz_node.py`、`_deploy_stat_backgrnd2_mock.py`、`_extend_stat_backgrnd.py`、`_live_backgrnd2*.png`、`_parse_statdetail_draw.py`、`_preview_stat_bg.png`、`_set_png_payload.json`、`_set_stat_backgrnd2.py`、`_stat_backgrnd_live.png`、`_statdetail_draw_decomp.c`、`_UIWindow_statmerge.img`、`stat_backgrnd_ext.png`、`stat_detail_backgrnd2_*.png/*.txt`、`StatusBar.img`。

## 4. 跨栈强耦合功能（端到端）

1. **等级/金币/HP 上限**：D（客户端 `CharStats` 类型升级 + 编解码站点）+ B（s9 `PacketCreator` 改 `writeInt`/常量）+ E（DB 列宽）。客户端与服务端**必须同步改字节宽**，否则收发包错位。
2. **角色槽位**：C（`UICharSelect` 槽位布局）+ B（`CharlistHandler` 槽数）+ E（账号角色数上限表）。
3. **装备栏/背包扩展**：C（`UIEquipInventory`/`UIItemInventory` 槽矩阵与坐标）+ A（对应 img 背景）。
4. **个人伤害统计**：C（战斗伤害采集 UI）+ B（伤害广播/聚合包）。

## 5. 风险与决策点

- **D 类整体风险最高、收益偏低**：原版地址在本工程无意义。建议仅保留「99999 HP/MP」「金币/等级上限」等确有业务价值项；「083多分辨率」「伤害上限纯客户端 hack」评估优先级或暂缓。
- **融合外观 / 伤害皮肤** 为完整客户端 mod 工程（含 sln/vcxproj/idl/rc），**目标源码树不明**，须先确认其基线再评估，否则工作量不可估。
- **Bot UI** 疑似外挂 UI，**不建议移植**。
- **资源授权**：部分皮肤/BOSS/地图为私服改版资源，落盘前须用户确认授权与兼容性。
- **DB 红线**：涉及库内数据须先获授权，未同意禁止 DDL/DML（仅查询）。
- **兼容漂移**：打包 `UI.wz` 与运行时 `Data/` 已证实漂移（Stat backgrnd 175 vs 208），一切像素结论以运行时为准（R1/R2/R5/R6）。

## 6. 后续文档

- `notes/classification.md`：各类移植方法论 + 代表样例（改动文件、协议/Opcode、img 节点路径、SQL 迁移点）。
- `notes/roadmap.md`：P0~P5 分阶段路线图 + 验证矩阵 + 决策点清单。
