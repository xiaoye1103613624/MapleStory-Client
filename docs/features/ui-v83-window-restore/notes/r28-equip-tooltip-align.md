# R28 追加 · 装备 Tooltip 原生逐像素复校（2026-11-08 落地）

> 来源：dsh 会话 `session-b8e5680e-6d15-4acc-b734-8cba88ddfbfb`（原任务「物品 Tip 与装备属性显示效果对齐问题」）。
> 该会话已完成全部取证/分析但代码 0%，step 55 因 `unreachable` 中断；本轮按已确认结论与两项用户抉择续作落地。
> 边界：纯客户端显示层。未改服务端、未改 opcode、未写库、未改 `.img`/`.nxcache`（IMG 全程只读 dump）。

## 1. 用户诉求与两项抉择

原诉求 6 条：背景色/透明度、属性排版与间距、装备属性是否完整、图标下灰色底板、职业行下方属性对齐、补装备 ID 行。

会话 `ask_user_question`（seq 348/349）已由用户拍板：

| 决策点 | 用户选择 | 落地口径 |
| --- | --- | --- |
| 正文行对齐 | **按你的要求：整行居中（冒号近似居中）** | 每行「名：值」整体在框内水平居中；**明确偏离原生左对齐**（原生各行冒号 x=98/86/110/185） |
| 道具等级/道具经验两行 | **仅 `itemlevel`/`itemexp` 非零时显示** | 两行成组判断 `itemlevel > 0 \|\| itemexp > 0`；无成长数据的装备不画这两行，需求块推进量同步缩短 |

## 2. 证据（可复核，非目测）

| 证据 | 结论 |
| --- | --- |
| `evidence/r28-tooltip-native/full_4x.png`、`req_6x.png`、`body_6x.png` | 原生参考帧 `正确客户端截图/背包中鼠标悬浮物品上显示的物品信息内容.png`（白色背心 268×310）放大裁切 |
| `req_6x.png` 逐行读字 | 需求块**单列 8 行、12px 行距**：要求等级/力量/敏捷/智力/运气（白字 `0`）、要求人气（白字 `-`）、道具等级（橙字 `1`）、道具经验（橙字 `0 %`） |
| `body_6x.png` | 正文行色 = **(255,238,204) 奶油色**，行距 15px，每行前有橙色菱形项目符；原生为左对齐 |
| 背板像素统计（`evidence/native-verdicts-2026-11-03/native-tooltip-alpha-raw.txt`） | 背板**实心** `(79,79,119)` + 1px 亮边 `(85,85,130)`，无灰外框；压米色窗/深色画面 ΔR=0.6 ⇒ 透出 ≈0.3% |
| `E:\MXD\BeiDou-Client-xy\REA反编译\wz_dump\UIWindow.img_runtime.json` | `ToolTip/Equip/Can` 与 `Cannot`：`reqLEV/reqSTR/reqDEX/reqINT/reqLUK/reqPOP` 均 47×11、数字 0–9 5×7、`none` 5×1、`percent` 5×7；`GrowthEnabled`/`GrowthDisabled`：`itemLEV`/`itemEXP` 47×11 + 数字 + `percent` + `none` + `max`；`IconBase/0` 32×32 |

> R1/R5 核验：位图节点路径、类型与尺寸全部取自**运行时部署数据**的 probe dump，未使用打包 `UI.wz` 陈旧副本。
> R6 守卫：`IconBase/0`、`Growth*` 取用时均允许节点缺失（BeiDou 的 img 为定制精简版），缺失即跳过而非崩溃。

## 3. 改动清单

| 文件 | 改动 |
| --- | --- |
| `Graphics/Color.h` | 新增 `Color::Name::TOOLTIPBODY` `(255,238,204)`，并同步补 `colors[NUM_COLORS][3]` 数组项（枚举/数组必须同增） |
| `IO/Components/TextTooltip.h` | `TooltipBackdrop` 填充 `(85,85,130)`→`(79,79,119)`、灰边 `(128,128,128)`→亮边 `(85,85,130)`，均用 float 分式构造避免 `Color` 双重载歧义 |
| `Character/Inventory/Equip.h/.cpp` | 新增 `int16_t get_itemexp() const`，供成长两行的非零判断与百分比数值 |
| `IO/Components/EquipTooltip.h` | 新增 `iconbase`、`growthset`、`growth_lev_label/growth_exp_label/growth_percent/fame_none`、`show_growth*`、`growth_*_str*`、`req_shift*`、`itemid*` |
| `IO/Components/EquipTooltip.cpp` | 见 §4 |
| `locale/zh-CN.ini`、`locale/en-US.ini` | 文案对齐原生：`类型`→`装备分类`、`剩余强化次数`→`可升级次数`、`已用锤子`→`金锤子已提高的强化次数`；新增 `equip.label_itemid`（`道具ID：{}` / `Item ID: {}`） |
| 部署 `E:\MXD\BeiDou-Client-xy\locale\{zh-CN,en-US}.ini` | 同步上述文案（构建后复核 md5 一致，未被构建覆盖） |

### `EquipTooltip.cpp` 细节

1. **分隔线**：`ColorLine(..., WHITE, 0.4f, ...)` → `1.0f`（原生 y237 全宽纯白 1px）。
2. **图标衬底**：`UIWindow.img/IconBase/0` 2x 画在 `(16,80)`，道具图标 2x 画在 `(18,82)`——与 `SkillTooltip.cpp:168-173` 同一接入方式与 (2,2) 偏移；`iconbase.is_valid()` 守卫。
3. **需求块**：`requirements` 补 `FAME`；`reqstatpositions` 由两列 `(97/177, 47/62/77)` 改单列 `x=97`、`y=47+12·i`（6 行）；数字偏移 `+54`→`+50`（47px 标签 + 3px）。`FAME` 值画 `Can/none` 短横而非 `000`。
4. **成长两行**：`GrowthEnabled/itemLEV|itemEXP` 作标签（y=119/131），`GrowthEnabled` 数字字模画数值，`percent` 位图接在经验数字后 2px；`show_growth` 为假时两行与对应推进量一并不画。
5. **正文**：`category/wepspeed/statlabels/slots/hammers/desc` 由 `WHITE` 改 `TOOLTIPBODY`；属性行 `Alignment::LEFT`→`CENTER`，绘制锚点统一 `BODY_CENTER_X = FRAME_WIDTH/2 = 132`；`desc` 保持左对齐（可能多行）。对比窗差值文本改挂在 `center + label.width()/2`（居中后的右缘）。
6. **道具 ID 行**：`itemid`/`itemid_preview` 构造在 `category` 之前、绘制在其上一行（15px 行距），走新增 i18n 键。
7. `draw` 与 `draw_preview` 同步改造（两者几何、颜色、行序完全一致）。

## 4. height / draw 同步推导

旧布局 `pos` 累计：`44 + 12 + 88 + 49 = 193`（需求块推进 88）；`height` 基准 540。

新布局需求块由「两列 3 行」变「单列 6/8 行」：

- 6 行：末行 y=107，高 11 ⇒ 结束 118 ⇒ 推进 `REQ_SHIFT_BASE = 124`
- 8 行：末行 y=131，高 11 ⇒ 结束 142 ⇒ 推进 `REQ_SHIFT_GROWTH = 148`
- 新增道具 ID 行占 15px

于是 `height = HEIGHT_BASE + (req_shift - 88)`，其中 `HEIGHT_BASE = 540 + 15`：

| 状态 | req_shift | height 基准 | 相对旧增量 | 绘制内容下移 |
| --- | --- | --- | --- | --- |
| 无成长数据 | 124 | 591 | +51 | `(124-88) + 15 = 51` ✓ |
| 有成长数据 | 148 | 615 | +75 | `(148-88) + 15 = 75` ✓ |

`draw`/`draw_preview` 用同一个 `req_shift`/`req_shift_preview`，`set_equip` 每次悬停先重置状态，避免沿用上一件装备的数据；`backdrop.draw(..., height + 32)` 随 height 同步，内容不会越出背板。

## 5. 验证

| 项 | 结果 |
| --- | --- |
| `tools/build_smoke.bat` | `GATE OK`（opcode matrix 一致）→ `EXITCODE=0` |
| `build_log.txt` 精确筛查 | `error C` / `error LNK` / `fatal error` / `MSB` **0 条**；`Equip.cpp`、`EquipTooltip.cpp` 均参与编译；仅既有 C4456（`item_id`/`equipdata` 遮蔽）与工程既有 C4005/C4100/LNK4099 噪声 |
| 产物 | `E:\MXD\BeiDou-Client-xy\MapleStory.exe` = 4,924,928 B，时间戳 `2026-11-08 13:14:27`（与 `build_log.txt` 同秒刷新，确认非陈旧 exe） |
| IDE 诊断 | 6 个改动文件 `read_lints` = 0 error / 0 warning |
| locale | 源码与部署 `zh-CN.ini` md5 同为 `256059cf…`；`equip.label_itemid` 在部署侧存在；构建未覆盖回旧版 |
| 写入可靠性 | 本轮 4 个 C++ 文件用 Python 精确补丁 + 逐项命中校验（24/24 命中）+ 事后 grep 二次确认落盘；原文件已备份 |

## 6. 显式预留 / 未跑项

- **未做实机悬停截图验证**：需真实 s9 服务端登录进图并悬停装备；本轮只做到编译/链接 + 静态几何自洽 + 资源节点核验。上线前建议按 `toolchain` 的抓帧链补一张同场景对照。
- **正文行前的橙色菱形项目符**（`ToolTip/Equip/Dot/0` 5×5）原生存在，本轮按「最小改动」未复刻；整行居中后再补项目符需单独定义项目符与文本的居中基准，属后续增项。
- **`ItemTooltip`（消耗品 tip）未同步图标灰底板**：本轮范围按会话取证只限装备 tooltip；技能 tip 已自带 `IconBase/0`。
- **`build-rebuild/locale/zh-CN.ini` 是历史构建镜像**，不含新键且非部署源（部署 locale 由源码目录直接同步，已验证），故未改。
- **正文整行居中是用户指定偏离**，不是原生还原；若后续要求逐像素回归原生，只需把属性行 `Alignment` 改回 `LEFT`、锚点改回 `frame+16`，不影响其它改动。
- 成长两行按「成组显示」实现（`itemlevel>0 || itemexp>0` 时两行都画，值可为 0）。若要求「各行独立按自身数值非零显示」，改 `show_growth` 拆成两个布尔即可。

## 7. 产物与回滚

- 可复核 diff：[`patches/r28-equip-tooltip-align.diff`](../patches/r28-equip-tooltip-align.diff)（4 个 C++ 文件，改前=本轮备份，改后=工作树）。
- 原生证据：[`evidence/r28-tooltip-native/`](../evidence/r28-tooltip-native/)（`full_4x/req_6x/body_6x/job_6x/head_6x.png`）。
- 补丁脚本与改前备份：`%APPDATA%\CodeBuddy CN\User\globalStorage\tencent-cloud.coding-copilot\brain\bad8bebc8ec347a29a384cfc760a3991\` 下的 `patch_equip_tooltip.py`、`patch_locale.py`、`make_diff.py` 与 `*.bak`。
- 回滚：将上述 `.bak` 覆盖回对应源码路径，并按 §3 表回退 `Color.h`/`TextTooltip.h`/locale，再跑一次 `build_smoke.bat` 即可。
