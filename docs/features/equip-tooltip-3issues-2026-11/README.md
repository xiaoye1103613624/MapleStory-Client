# 装备 tooltip 三问题：取证结论与开发计划（2026-11-17）

> 个人学习/技术研究用途，单机离线环境。证据驱动，未真机验收项一律标注。

## 0. 需求与已确认抉择

- 原始需求（m00002，逐字）：装备并排展示时相同属性是否有差值展示、差值是否只显示白色；装备 "owner" 字段、"固有"、"不可交换" 等特性属性展示排版/颜色是否正确；095 新增特性能否按 095 逻辑展示。参照 angel / 095 IDA、MinMS 源码与 img 资源；测试可直接改数据；DBX MCP 全局注册。
- 用户抉择（m00389）：
  - `scope` = 全选（①先给真实结论不改码 + ②修差值 + ③补 owner/固有/不可交换 + ④补 095 特性对比窗）
  - `version` = 两端都要对齐（v83 Angel + 095）
  - `evidence` = 接受源码层结论（不真机验收）
- lead 解读（已告知用户）：① = 改前给足结论（本节 1，已交付）；②③④ = 待落地功能。若非此意，用户可喊停。
- 二次抉择修订（m00417/m00421，逐字）：
  - `A_style` = "保持现状不画" → **功能 A 取消**（diff==0 不画差值是期望行为，不动代码）。
  - `B_input` = "095ida mcp是开启的（开启游戏.vmp.exe (pid=15148, port=13338)）" → 用户纠正：不得拿 VMP 壳当借口，必须实际用 13338 挖。
  - `C_scope` = "接受镜像（推荐）" → 功能 C 按本端构型镜像，不保证与 095 逐像素一致。
  - `commit` = "按功能分两次原子提交"。
- 当前状态（终态）：`功能A 已取消 + 功能C 已原子提交(64c7500a) + 功能B 已原子提交(f2037ca3)并构建通过（详见 §7）`。

## 1. 取证结论（行号级，已验证事实）

### 1.1 差值颜色：源码层彩色，"白色"是 diff==0 不画差值的既定行为

| 环节 | 文件:行号 | 事实 |
| --- | --- | --- |
| 差值计算+上色 | `IO/Components/EquipTooltip.cpp:589-607` | `diff = hovered_value - equipped_value`；`>0` → `Color::Name::GREEN` "(+N)"，`<0` → `Color::Name::RED` "(−N)" |
| 门槛 | `:592`、`:1365` | `if (diff != 0)` 才构造；draw 时 `if (!diff.empty())` 才画 → diff==0 时那行只剩白色属性本体 |
| 属性本体色 | `:587` | `statlabels_preview[es].build(..., Color::Name::WHITE, true)` |
| 绘制位置 | `:1360-1366` | `right = line.draw(pos)`；`diff.draw(Point(right, pos.y()))` 紧跟属性右缘 |
| 颜色落地 | `Graphics/Text.cpp:24,37` → `Graphics/GraphicsGL.cpp:1474-1476,1837-1842` | `word.color` 存色，drawtext 取 `Color::colors[word.color]`，参数 color 仅作 alpha |
| 色值 | `Graphics/Color.h:53-102,106-155` | GREEN=27→(0,0.75,0)、RED=4→(1,0,0)，索引对齐无错位成白 |
| 溯源 | git blame `:589-607` | commit `73783131`（2026-10-21，feat 装备对比差值 正绿负红） |
| live exe | `E:/MXD/BeiDou-Client-xy/MapleStory.exe` | mtime 2026-11-17 16:22，SHA256 `0c3bf5db75ac9661cf5ff58b226b01e4913e5784cc8b8dab88441805cb886649`；时间上应已入链，未反汇编目标码逐字节确认 |

结论：若"白色"指相同属性那一行 = diff==0 不画差值（设计行为）；若指有差异行也是白色 = 与源码矛盾，需本端真机截图或 live 反汇编定性（用户已选接受源码层结论，本版不追真机）。

### 1.2 owner / 固有 / 不可交换：本端与 v83 Angel 均无此字段，095 有但取证受阻

| 层 | 证据 | 事实 |
| --- | --- | --- |
| 数据层 | `Data/ItemData.cpp:28-29,76-77` | 仅 `untradable=src["tradeBlock"]`、`unique=src["only"]` 两个 bool，无 owner/固有/不可交换 |
| 展示层 | `EquipTooltip.cpp:274-285` `equip_trade_label()` | 仅 unique/untradable → `equip.trade.{both,unique,untradable}` |
| 排版/色 | `:523-528,790-795,1069-1070,1279-1280` | trade 行 ORANGE，画在名称区 sub_y=24 下方（非分隔线以下属性块） |
| 文案 | `locale/zh-CN.ini:18-20` | "不可交易 / 唯一道具 / 唯一道具，不可交易" 三条 |
| IMG | orange-wz（128B key，exclusive=false）读 live `UIWindow.img` | `ToolTip/Equip/Property/0-18` 共 19 张属性名 canvas（宽31-82，高9，ARGB8888）；全库搜 owner/unique/potential 零命中 |
| v83 Angel | IDB(13337) 全串扫 | 仅 `equipTradeBlock`(`0xaf63e8`) + ITC 串，无 owner/固有/不可交换；`sub_5D57BC@0x5d57bc` 用该键读 WZ `get_int32` |
| MinMS | `src/client/IO/Components/EquipTooltip.{h,cpp}` | 老版 Journey，同样无这些字段 |

结论：v83 原生（Angel）与当前实现一致，"是否正确"在 v83 基准下 = 正确（trade 行橙色、名称下方，与 `:155-156` 注释自述的 079/083 对齐一致）；owner/固有/不可交换属 095 越版特性，本端无数据无展示。

### 1.3 095 特性：preview 对比窗缺失（客观缺陷），095 原生逻辑 VMP 壳挡住

| 项 | 证据 | 事实 |
| --- | --- | --- |
| header 声明 | `EquipTooltip.h:184-188` | `haspotlines[5]/potlines[5]/haspotlines_preview[5]/potlines_preview[5]` 均在 |
| header 缺声明 | `EquipTooltip.h:190-197` | 只有 `spark_line/soul_line/star_line`，**无** `*_preview` 版本 |
| 主窗构建 | `EquipTooltip.cpp:852-941` | `:862-884` potlines（get_potential 0-2 + get_bonus_potential 0-1，值=条目 id）；`:894-941` spark/soul/star；文案走 locale `equip.potline.*`/`equip.spark.*`/`equip.soul.*`，缺键回退带编号占位 |
| 主窗绘制 | `:1163-1208` | 潜能行 + 火花/宝珠/星之力，位置在"可升级次数/金锤子"之后、"描述"之前 |
| preview 缺失 | 实证 grep | `potlines_preview` / `haspotlines_preview` 在 cpp 中 **0 引用**；preview 侧（`:340-613` 构建、`:1240-1386` 绘制）既未构建也未绘制这些行 |
| preview 已有 | `:426-453` potflag_preview（潜能等级旗标）、`:523-528` trade_line_preview | 名称区隐藏词条已叠高，唯独潜能内容/强化三件套缺 |
| 095 原生 | IDA(13338) = `开启游戏.vmp.exe` | VMP 壳，静态字符串只有 Windows API，游戏逻辑运行时解壳，IDA 静态不可反编译（硬阻塞） |
| 095 WZ | orange-wz 加载 095 `Base.wz` | 128B key 可 load，节点结构与 v83 不同，`search ToolTip` 0 命中 |

结论：功能C 可按本端既有构型镜像补齐（不保证与 095 逐像素一致，因 095 原生不可取证）；功能B 若坚持"按 095 原生"则硬阻塞。

## 2. 数据归属（Plan 关键）

- preview 对比窗主体 = **已装备物品**：`EquipTooltip.cpp:348 auto eequip = inventory.get_equip(EQUIPPED, eqslot)` → `:352 const Equip& equip = *eequip`。
- 差值 = 悬停物品 oequip 相对已装备 equip 的增量：`:566-567 equipped_value = equip.get_stat(es)` / `hovered_value = oequip.get()->get_stat(es)`。
- 主窗 set_equip 的 `equip` = 悬停物品；功能C 镜像时 preview 侧数据源须用已装备 `equip`（`:352` 作用域内的那个），两者不可混用。

## 3. 开发计划（一次性完整，待用户确认后实现）

### 功能A — 修差值：diff==0 也显示中性标记

- 改动文件：`IO/Components/EquipTooltip.cpp`
- 改动点：`:592 if (diff != 0)` 门槛放开；diff==0 时构造 `Color::Name::LIGHTGREY` 的中性文本（建议 `" (0)"`，样式待确认）；`:1365 if(!diff.empty())` 相应保证能画出。
- 影响面：仅对比预览窗属性行；不动主窗、不动 statlabels。
- 待确认：diff==0 标记的文本与颜色（建议灰色 "(0)"，用户可改为 "(±0)" / 不显示）。
- 验证：单测/构建 + 造一件与已装备同属性的悬停装备，确认该行出现中性标记；回归有差异行仍绿/红。

### 功能B — 补 owner / 固有 / 不可交换（硬阻塞，需用户拍板）

- 阻塞原因：① 数据层无字段，WZ 键名未知（不能凭记忆编造，门禁5）；② 095 展示逻辑 VMP 壳不可静态取证；③ v83 原生本就没有这些字段，"两端对齐"在 v83 侧无依据。
- 需用户提供其一：(a) 095 解壳后 IDB / 动态 dump，给出字段名与排版颜色；(b) 095 端本机 tooltip 截图（须确认来自 095 端，USER.md 教训）；(c) 数据包字段顺序抓包证据。
- 用户拍板选项：提供输入后实现 / 本版显式预留（数据层骨架 + 配置读键 + 占位不显示）/ 本版取消。
- 在输入到位前**不写业务实现**，避免想象修改。

### 功能C — 补 095 特性的对比窗行（可行）

- 改动文件：`IO/Components/EquipTooltip.h`、`IO/Components/EquipTooltip.cpp`
- header：新增 `bool has_spark_line_preview`、`Text spark_line_preview/soul_line_preview/star_line_preview`（及 `has_*_preview`），与已有 `potlines_preview` 配套。
- cpp 构建（preview 数据块 `:340-613` 内）：镜像 `:852-941` 的口径，数据源用已装备 `equip`（`:352`），产出 `haspotlines_preview[5]/potlines_preview[5]`（get_potential/get_bonus_potential）+ spark/soul/star preview；文案仍走 locale `equip.potline.*`/`equip.spark.*`/`equip.soul.*`，缺键回退占位；同步累加 `height_preview`。
- cpp 绘制（preview 绘制块 `:1240-1386` 内，属性行之后、描述之前）：镜像 `:1163-1208` 的顺序与配色（潜能 WHITE、星之力 YELLOW），用 preview 成员 + BODY_LEFT_X 偏移。
- 影响面：仅 preview 对比窗；不动主窗、不动 statdeltas。
- 验证：构建 + 造已装备装备带潜能/火花/宝珠/星之力，确认对比窗对应行出现且配色/顺序与主窗一致；回归无特性装备不留空行、高度正确。

## 4. 通用验证门禁

- 改动后按仓库约定 touch 源文件 + 删除对应 `x64/obj-Release/<文件>.obj` 强制失效增量缓存（本仓库 MSBuild 假通过陷阱）。
- 构建日志 error C 计数 = 0；obj mtime > cpp mtime；部署 exe SHA 相对基线变化（当前基线 `0c3bf5db…`）。
- 严格 UTF-8 编码（用户 2026-11-17 m00328 全局约定）。
- 临时调试日志功能闭环后删除，仅保留上线级节点日志。

## 5. 风险与回滚

- 栈/代码布局敏感：本仓库对栈布局敏感（历史 FIN 补丁 heisenbug），功能A/C 为纯 UI 逻辑改动，风险相对低，但仍需最小 diff 构建后实测启动。
- 回滚：git 单功能原子提交前可 `git checkout -- <file>`；预览窗改动不触及服务端/协议。
- live exe 被登录器反复回滚覆盖，任何"已部署"声明前必须实时核对 SHA（USER.md 教训）。

## 6. 待用户确认项（一次性拍板）

1. 功能A：diff==0 的标记文本/颜色（建议灰色 "(0)"）。
2. 功能B：提供 095 输入 / 本版预留 / 本版取消（三选一）。
3. 功能C：确认按"本端既有构型镜像"（因 095 原生不可取证，无法逐像素对齐）是否接受。
4. 是否授权进入实现（确认后按 A→C 顺序落地，B 按拍板结果处理）。


## 7. 实现终态（2026-11-17 收口）

### 7.1 功能A — 取消
- 用户二次抉择 `A_style="保持现状不画"`：diff==0 不画差值是既有设计行为，**不动代码**。

### 7.2 功能C — 已原子提交
- 提交 `64c7500a`：`✨ feat(equip-tooltip): 补齐095特性对比预览窗展示 (#devfix-equip-compare-095)`，2 文件 130 行。
- 实现：preview 对比窗补齐潜能行（get_potential 0-2 + get_bonus_potential 0-1，带 (B) 前缀）+ 火花/宝珠/星之力三件套，配色/顺序镜像主窗；数据源用已装备 equip（`:352`），早退分支不残值。
- 验证门禁：`build_verify.bat` EXITCODE=0、error C=0、`verify_int/EquipTooltip.obj` mtime > cpp、产物 `verify_out/MapleStory.exe` SHA256 `e6443c8d3a8bde3eda0552c863aaa1abfa69d0e48f838ffd18883ffa84f175b8`。

### 7.3 功能B — 取证翻案 + 实现（owner 展示行）
- 取证翻案（子 agent probe-095-owner 三路线穷尽）：owner 数据链路**全通**，本端缺的只是 UI 展示：
  - 服务端 `BeiDou-Server_s9/gms-server/.../PacketCreator.java:494 p.writeString(equip.getOwner())` 下发；`Item.java:44/:123`、`ItemFactory.java:103 equip.setOwner(rs.getString("owner"))` DB 持久化通。
  - 本端 `Character/Inventory/Equip.h:47 get_owner()` + `Equip.cpp:39-42` 已解析存储（lead 逐条亲验）。
  - 固有=unique=`ItemData.cpp:77 src["only"]`；不可交换=untradable=`:76 src["tradeBlock"]`；另有 `:78 unsellable=src["notSale"]`。
  - 095 WZ（orange-wz 128B）全库搜「固有/不可交换/所有者/绑定」=0 匹配——是概念名非 095 原生文案；tradeBlock/only 是 Item.wz 仅有相关 INT_PROPERTY。
- 用户拍板（m00601）：`b_owner`=加 owner 展示行（推荐）；`b_rename`=保持现状（交易词条维持「唯一道具/不可交易」，不改叫法）。
- 实现：`.h` 加 owner_line/owner_line_preview + has_owner/has_owner_preview；主窗与 preview 构建侧读 `equip.get_owner()`，非空才建 ORANGE 行并 `height += 14`；绘制侧在名称区词条区叠行（potflag→trade→owner），用 `pos.shift_y((sub_y > HEADER_SHIFT) ? sub_y : HEADER_SHIFT)` 动态下压——≤2 词条视觉零变化，3 词条全满才扩展防压图标；ctor 与 set_equip 重置 has_owner；locale 新增 `equip.owner`（zh「所有者：{}」/en「Owner: {}」）。
- 验证门禁：`build_verify.bat` EXITCODE=0、error C=0、`verify_int/EquipTooltip.obj` mtime 17:17:56 > cpp 17:17:25。
- **提交边界（已裁决并执行）**：功能 B 绘制依赖 `sub_y`/`HEADER_SHIFT`/`draw_equip` 等符号，这些在 HEAD 基线（功能C提交后）均不存在，属工作区未提交的「名称区词条区布局重构」。用户裁决 **合并为一个原子提交**（布局重构 + owner 相互依赖，无法干净剥离）。提交 `f2037ca3`：`✨ feat(equip-tooltip): 名称区词条布局重构并补齐 owner 展示行 (#devfix-equip-owner)`，4 文件（cpp/h 全量重构 + owner、locale 各 1 行 owner 文案）。提交前修复了自身引入的编码 bug（zh-CN.ini 为 GBK，edit 工具曾以 UTF-8 写入 owner 行致乱码，已按 GBK 字节重写并用 git update-index --cacheinfo 二进制暂存，逐字节校验通过）。工作区其余 220+ 历史 WIP（cashshop/技能等无关改动）全部保留未提交。

### 7.4 编码统一纠错（用户当面要求：GBK 全改 UTF-8，统一就全部统一）

- **问题**：§7.3 中我把 owner 行写成 GBK 字节是错误的。根因=用 `decode('gbk')` 去验证一个 UTF-8 文件，GBK 解码 UTF-8 中文必然失败，我反向误判成"文件是 GBK"，把对的改成错的。
- **正解标尺**：`decode('utf-8')` strict 通过 + 无 BOM + 无 fffd + 中文码点在 \u4e00-\u9fff。**绝不能用 `decode('gbk')` 验证 UTF-8 文件**。
- **执行**：全仓扫描 144 个 .ini，唯二污点是 `locale/zh-CN.ini` 与 `setitem_build_out/locale/zh-CN.ini` 的 owner 行（我引入），其余本就 UTF-8。两处统一改回 UTF-8；修复中曾误吃掉换行符致 owner 行与下一行粘连，已补回并字节级验证。
- **提交修正**：`git commit --amend --no-edit`，f2037ca3 → **406abe0a**（HEAD，无后继提交依赖，amend 安全）。HEAD blob 的 owner 行现为 UTF-8 字节 `e6 89 80 e6 9c 89 e8 80 85 ef bc 9a`，整个 zh-CN.ini strict UTF-8 通过。
- **终验**：全仓 144 个 .ini 问题数=0；工作区 owner 行与 HEAD 一致；其余 21 行 locale diff 是无关历史 WIP，保持未提交。
