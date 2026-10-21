# r05 · 四窗布局对照与修复（BeiDou V17.7 → 当前客户端）

> 日期：2026-10-22 | 参考端：`E:\MXD\BeiDou-ClientV17.7` | 当前端：`E:/project/MapleStory-Client`（部署 `E:\MXD\BeiDou-Client-xy`）
> 方法：**数据驱动取证** —— 从部署数据 `Data/UI/UIWindow.img` 导出窗口背景精灵 PNG，再用脚本对其做逐像素行程/格线测量（`notes/ui_layout_dump/`），以此替代目视比对（本会话不支持读图）。exe 侧逆向结论见 §6。

## 0. 结论速览

| 窗口 | 数据节点 | backgrnd | 根因 | 修复文件 |
| --- | --- | --- | --- | --- |
| 角色能力 | `UIWindow.img/Stat` | 208x347 | classic 分支把 backgrnd/backgrnd2/basicStat 三精灵叠绘在 (0,0)；标签用 GMS 现代偏移且缺「等级/经验值」 | `IO/UITypes/UIStatsInfo.{h,cpp}` |
| 角色装备 | `UIWindow.img/Equip` | 175x304 | 帽子等槽位落在错误列/行（标签与槽位格错位） | `IO/UITypes/UIEquipInventory.cpp` |
| 技能 | `UIWindow.img/Skill` | 175x289 | 现代 12 行双列布局越出 175 宽窗口；行绘制未应用滚动 offset；`[DBG-SKB]` 调试代码 | `IO/UITypes/UISkillBook.{h,cpp}` |
| 现有道具 | `UIWindow.img/Item` | 175x289 | classic 回退常量行步进 36（应为 34），网格与背景格线累计错位 | `IO/UITypes/UIItemInventory.cpp` |
| 职业名 | — | — | `Job::get_name()` 为英文硬编码；参考端中文表在 `ijl15.dll` | `Character/Job.cpp` |

四窗标签/格子均**烘焙在 backgrnd 精灵**里，客户端只按坐标摆图标与文本；因此坐标必须以背景实测格线为准。

## 1. Stat（角色能力）— `UIWindow.img/Stat/backgrnd` 208x347

背景左侧彩色标签条（粉色=信息行、绿色=四属性）实测 chip 顶边：

| 行 | 标签 | chip 顶边 y | 代码取值 |
| --- | --- | --- | --- |
| 0 | 名字 | 31 | (60,31) |
| 1 | 职业 | 49 | (61,49) |
| 2 | 等级 | 77 | (61,77) ← 新增 |
| 3 | 家族 | 95 | (61,95) |
| 4 | HP | 113 | (61,113) |
| 5 | MP | 131 | (61,131) |
| 6 | 经验值 | 149 | (61,149) ← 新增 |
| 7 | 人气度 | 167 | (61,167) |
| - | 能力值输入框 | 62..86 × 215..227 | AP 右对齐 (84,216) |
| 8 | 力量 | 243 | (61,243) |
| 9 | 敏捷 | 271 | (61,271) |
| 10 | 智力 | 297 | (61,297) |
| 11 | 运气 | 325 | (61,325) |

其他精灵：`backgrnd2` 218x203 = 详细属性板（仅详细模式，绘于 `position+(212,0)`）；`basicStat` 159x121 = 「基本属性点」帮助弹框（绘于窗口上方 `y = position.y - 121 - 2`，上方不足时改放下方）。

修复要点：
1. classic 只把 `backgrnd` 加入 `sprites`；`backgrnd2`/`basicStat` 改为独立 `Texture` 成员按条件绘制（消除重影）。
2. `StatLabel` 新增 `LEVEL`、`EXP`；`update_all_stats()` 填等级/经验/家族；`update_stat()` 与 `PlayerHandlers::need_statsinfo_update` 增加 `LEVEL`、`EXP` 分支使等级/经验变化即时刷新；经验显示为 `exp(pct%)`（pct 取自 `ExpTable::values[level]`）。
3. classic `draw()` 只绘 8 信息行 + AP + STR/DEX/INT/LUK；现代分支（双列/详细面板）保持不动。

## 2. Equip（角色装备）— `UIWindow.img/Equip/backgrnd` 175x304

槽位格（脚本扫描行内亮度跳变得到的一致边框位置）：

- 列 x：**5, 38, 71, 104, 137**（步进 33）
- 行 y：**32, 65, 98, 131, 164, 197, 230**（步进 33）
- 证据：`ui_layout_dump/_equipcols2.txt`（多行扫描均在 x=37/70/103/136 出现边框跳变）

`classic_layout` 槽位表（括号内为标签）：

| 行 | 列1 (5) | 列2 (38) | 列3 (71) | 列4 (104) | 列5 (137) |
| --- | --- | --- | --- | --- | --- |
| 32 | — | — | 帽子 | — | — |
| 65 | 勋章 | 脸饰 | — | 指环R3 | 指环R4 |
| 98 | — | — | 眼饰 | 耳饰 | — |
| 131 | 披风 | 上衣 | 项链 | 武器 | 盾牌/副手 |
| 164 | 手套 | 裤·裙 | 腰带 | 指环R1 | 指环R2 |
| 197 | — | — | 鞋子 | — | — |
| 230 | 骑兽 | 鞍子 | — | — | — |

旧表把 HAT 放在 (38,35)（第 2 列，额饰格），导致帽子图标压到额饰、帽子格空置——即用户截图「标签与槽位不对齐」。

## 3. Skill（技能）— `UIWindow.img/Skill/backgrnd` 175x289

分区实测（`_skillbands.txt`）：

- 标题 0–22；tab 条 23–54；书封条 55–86；**列表区 87–257（170px）**；技能点行 258–281（输入框 266–280）。
- 行精灵 `skill0/skill1` 141x35；`line` 141x1；`BtSpUp` 12x12；`CoolTime` 32x32；`tip0-2` 193x102。

修复要点：
1. 新增 `CLASSIC_ROWS = 4`：列表区 170px ÷ 行高 40 = 4 行（参考端截图亦为 4 行新手技能）；旧代码固定 12 行会把 8 行画到窗口外（引擎无裁剪）。
2. 单列起点 `CLASSIC_SKILL_OFFSET = (10,87)`；右列越界问题（旧 `ROW_WIDTH=143` → 右列 x=153，伸到 294）在 classic 分支不再使用。
3. classic 控件：书封图标 (11,55)、书封名 (28,68)、SP 文本 (109,267)、slider `Range(87,258)` x=158；`BtSpUp` 实际存在（12x12），故按可见行数只创建 4 个，位于行右端 `x=+130`。
4. 新增 `visible_rows()` / `skill_index(row)`：现代双列 offset 以「行对」为单位（index = `2*offset + row`），经典单列 index = `offset + row`；绘制/命中/按钮状态三处统一，顺带修复了**滚动 offset 从未参与绘制**的既有缺陷。
5. 删除 `[DBG-SKB]` 临时日志与 `#include <fstream>`。

## 4. Item（现有道具）— `UIWindow.img/Item/backgrnd` 175x289

槽位格实测（`_itemcols2.txt` / `_edges.txt`，格内部亮度 232、格间 255）：

- 列：**x = 8, 44, 80, 116**（步进 36 = 32 + space_x 4）
- 行：**y = 51, 85, 119, 153, 187, 221**（步进 34 = 32 + space_y 2），6 行；末行底 251，下方 252-261 为金币行
- 证据：行内扫描在 x=40/76/112/148 出现格间条带；列扫描在 y=82/116/150/184/218 出现格间条带

修复：classic 回退常量 `slot_space_y` 由 `4` 改为 `2`（步进 36 → 34）。旧值使 6 行累计下移 10px，表现为「网格与窗口底部大片空隙/越往下越偏离格线」。其余（slot_col=4、slot_row=6、slot_pos=(8,51)、tab (10+i*31,26)、BtCoin (7,266)）与数据相符，未改。

## 5. 职业名中文化（`Character/Job.cpp`）

- `String.nx` **无** `Job.img` 节点（实测顶层仅 Cash/Consume/EULA/Eqp/Etc/Ins/Map/Mob/MonsterBook/Npc/Pet/Skill/ToolTipHelp…），故职业名只能硬编码。
- 参考端中文表位于 `ijl15.dll` 文件偏移 **0x2c868 起**，定长 8 字节/项、GBK 编码：`0x2c868 新手 → 0x2c870 战士 → 0x2c878 弓箭手 → 0x2c880 飞侠 → 0x2c888 海盗 → 0x2c890 管理员 → 0x2c898 骑士团 → 0x2c8a8 剑客 → 0x2c8b0 勇士 → 0x2c8b8 英雄 → 0x2c8c0 准骑士 → 0x2c8c8 骑士 → 0x2c8d0 圣骑士 → 0x2c8d8 枪战士 → 0x2c8e0 龙骑士 → 0x2c8e8 黑骑士 → 0x2c8f0 法师 → 0x2c908 法师(火,毒) → 0x2c914 巫师(火,毒) → 0x2c920 魔导师(火,毒) → 0x2c930 法师(冰,雷) → 0x2c93c 巫师(冰,雷) → 0x2c948 魔导师(冰,雷) → 0x2c958 牧师 → 0x2c960 祭司 → 0x2c968 主教 → 0x2c970 猎人 → 0x2c978 射手 → 0x2c980 神射手 → 0x2c988 弩弓手 → 0x2c990 游侠 → 0x2c998 箭神 → 0x2c9a0 刺客 → 0x2c9a8 无影人 → 0x2c9b0 隐士 → 0x2c9b8 侠客 → 0x2c9c0 独行客 → 0x2c9c8 侠盗 → 0x2c9d0 拳手 → 0x2c9d8 斗士 → 0x2c9e0 冲锋队长 → 0x2c9ec 火枪手 → 0x2c9f4 大副 → 0x2c9fc 船长 → 0x2ca04 初心者`。
- 已按该表改写 `Job::get_name()`（0=新手 / 100=战士 / 200=法师 / 300=弓箭手 / 400=飞侠 / 500=海盗 / 900,910=管理员 / 1000=初心者 / 骑士团系=骑士团）。工程已启用 `/utf-8`，源文件按 UTF-8 保存即可正确渲染。

## 6. exe 逆向备忘

- 库：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64`（PE x86，base 0x400000；**无 CUI* 符号**，函数均 sub_*）。
- 字符串锚点定位到的 `sub_7D1FC5`（引用 "Equip"）、`sub_5D2D22`（引用 "Skill"）均为 COM/VARIANT 风格**脚本绑定函数**，并非窗口布局代码；`find_bytes` 搜 32px 槽位表模式 0 命中。
- 结论：**布局常量不在 exe 立即数里**，以数据侧（UIWindow.img，部署数据与之逐字节一致）+ 背景精灵实测为准；本文件测量即最终依据。

## 7. 构建与验证状态

- `tools/build_smoke.bat`（Release|x64，OutDir = `E:\MXD\BeiDou-Client-xy\`）EXITCODE=0，exe 时间戳已刷新。
- **实机截图验证本轮未能完成**：当前环境游戏窗口（含登录界面）截出来全黑。A/B 验证——把本次改动 `git stash` 后重建的基线 exe 同样全黑（`_cap_base.png` mean=10.1，与改动后完全一致），故黑屏为**环境/渲染层既有问题**，与本次四窗改动无关。待该问题解决后再补四窗截图。
- 坐标正确性以 §1–§4 的实测格线为准（每项均给出脚本产物名，可复现）。

## 8. 未做 / 遗留

- `Configuration.h` 的 `PosCHARINFO/PosEQINV/PosSKILL/PosINV` 默认打开位置**未校准**：参考端截图位置无法证明是硬编码默认位（exe 内无相关常量），且位置会被 `Settings`/`xy-config.yaml` 覆盖，缺证据不动。
- 第二张参考截图中的「键盘设定」窗口不在本次范围。
- 技能列表是否会因 `JobData::get_skills()`（读 `Skill/000.img/skill`）为空而空白：数据侧确认 000.img 下存在 0001000/0001001/0001002 且无 invisible，过滤逻辑与参考端一致；该结论待实机复核。

## 9. 证据产物（`notes/ui_layout_dump/`）

- 背景 PNG：`stat_backgrnd.png`、`equip_backgrnd.png`、`skill_backgrnd.png`、`item_backgrnd.png`（及 `*_grid/zoom` 变体）
- 测量脚本与输出：`_skillbands.{py,txt}`、`_itemcols2.{py,txt}`、`_itemprof.txt`、`_edges.{py,txt}`、`_equipcols2.{py,txt}`、`_itemgrid2.txt`、`_ijl_dump.{py,txt}`（ijl15.dll 职业表 GBK 解码）
- 通用工具：`tools/nx_inspect.py`（nx 只读检查）、`tools/capture/pwcap.py`（PrintWindow 抓像素）、`docs/.../ui_layout_dump/_ascii.py`（截图转 ASCII 亮度图，供不支持读图时判读）
