# r03 · 第八面技能 IMG 门禁阻塞说明与替代取证链

> 2026-11-09 · 性质：门禁状态留痕 + 降级方案，**非实现提交**。
> 与第七面 `docs/features/cashshop-angel-calibration-2026-11/notes/r03-img-blocked-fallback.md` 同构，技能面沿用同一降级链。

## §0 门禁状态

| 门禁 | 状态 | 证据 |
| --- | --- | --- |
| REA shipped-artifact | ✅ | 既有 r17 链（BeiDou V17.7 线，单列，见 r05 §3） |
| IDA Angel 13339 + BeiDou 13337 | ✅ | 106/106 反编译 + 字节级 106/106（r01 §1） |
| **orange-wz(IMG)** | **⚠️ 阻塞** | `mcp_connector_status` 返回仅 1 条连接 `json-photoshop`（状态未知，「Host 尚未注册该 stdio Server 的工具」），**orange-wz 未连接** |

结论：三联门禁缺 IMG 一环。技能面 E 排版 / F 样式 / G 交互中依赖贴图与像素 oracle 的条目整体降级 `⏳`，不做臆测补全。

## §1 阻断的直接影响

- 无法用 orange-wz 拉取 `UIWindow.img/Skill/*`、`UIWindow.img/SkillMacroEx/*`、`Skill/<id>.img/icon*`、`Item/New`（经典 tab_plate 借用）、`BtClose2` 等节点做逐像素比对。
- 无法核对 22 类窗口族的真实贴图尺寸、锚点、切片、字体 ink 位。
- F 样式维 12 项全部 `⏳`（README §3 F 维）；E 排版 2 项、G 交互 1 项同受影响。

## §2 替代取证链（沿用第七面 r03）

1. **`.nx` 资源包 + `tools/nx_inspect.py`**：本地离线读取本包 WZ 编译产物，核对节点存在性 / 尺寸 / 子节点清单。已实证：`UIWindow.img/Skill/skillPoint` 在本包 **NOT FOUND**（`UISkillBook.cpp:1098-1152 send_spup` 注释），这正是当前端「资源缺失降级」路径的根因，属**真实环境事实**，非缺陷。
2. **正确客户端截图库** `C:\Users\11036\Desktop\正确客户端截图\` 作体感输入（老板体感是输入，不是验收证据）：仅作人工观感参考，不写入判定表。
3. **当前端源码常量** `IO/UITypes/UISkillBook.h` 的 `CLASSIC_SKILL_OFFSET/ROWS/SPUP_OFFSET` 与各 `*TOPLEFT/PITCH/CENTER` 坐标 + `docs/features/beidou-v17-parity/notes/r05-ui-layout.md §3` + oracle「技能展开宏指令.png」逐像素对齐记录。
4. **`ui_layout_dump/`** 既有落盘素材：`skill_backgrnd.png`、`skill_backgrnd_grid.png`、`_skill_nodes.txt`、`_skillbands.py/.txt`（r05 §2 归为「遗留」）。
5. **Angel 侧伪代码的 Ui 字符串常量**：`0CUISkill`/`0CWndSkillGuide` 构造器里的 `str_Ui_20` / `SP_1410_CANVASFONT` / `SP_8_TAHOMA` / `SP_4346_..._BTMACROOUT` 等，作节点名线索，不作像素证据。

## §3 F 样式维 12 项降级清单（P2 恢复后逐项销案）

| # | 校准点 | 当前端 source | 现状 | 恢复所需 IMG 证据 |
| --- | --- | --- | --- | --- |
| F1 | v83 经典窗纯黑技能名 vs 现代 EMPEROR 灰 | `UISkillBook.cpp:1-130 SkillDisplayMeta` | ⏳ | `UIWindow.img/Skill` 经典/现代两套字体色 |
| F2 | 技能图标三态 NORMAL/DISABLED/MOUSEOVER | `UISkillBook.cpp:1-130` / `SkillData.h Icon` | ⏳ | `Skill/<id>.img/icon*`/`iconDisabled`/`iconMouseOver` |
| F3 | 技能名 MAX_NAME_WIDTH=97 截断 | `UISkillBook.cpp:1-130` | ⏳ | 名称栏宽度贴图 |
| F4 | `name_text.draw(base+Point(38,-5))` | `UISkillBook.cpp:1-130` | ⏳ | 名称 ink 位 |
| F5 | `level_text.draw(base+Point(38,13))` | `UISkillBook.cpp:1-130` | ⏳ | 等级 ink 位 |
| F6 | SP 三态 Texture（sp_before/sp_after/used） | `UISkillBook.h:121-277` | ⏳ | `UIWindow.img/Skill/skillPoint/*` |
| F7 | `Charset sp_before/sp_after` | `UISkillBook.h:121-277` | ⏳ | 数字字符集节点 |
| F8 | 经典 tab_plate 借用 `Item/New` | `UISkillBook.cpp:142-250` | ⏳ | `UIWindow.img/Item/New` 节点 |
| F9 | `BT_CLOSE = UiAssetResolve::close_btn()`(BtClose2 描边X) | `UISkillBook.cpp:142-250` | ⏳ | `BtClose2` 贴图 |
| F10 | macro 面板 `Skill["macro"]["BtOK"]` | `UISkillBook.cpp:142-250` | ⏳ | `UIWindow.img/Skill/macro/BtOK` |
| F11 | `macro_backgrnd` 尺寸与 `dimension += macro_backgrnd.x` | `UISkillBook.cpp:1249 set_macro` | ⏳ | `UIWindow.img/SkillMacroEx/backgrnd` |
| F12 | `SP_4346_..._BTMACROOUT` 组合栏按钮态 | Angel `CUISkillEx::ShiftMacroUIState @ 0x8bad5a` | ⏳ | `UIWindow.img/SkillMacroEx/BtMacroOut` |

## §4 恢复条件与 P2 动作

orange-wz 恢复连接后（P2，需用户裁决 D14 后执行）：
1. 按 §3 逐项拉取节点 → 与当前端源码常量/截图比对 → 更新 README E/F/G 判定（`⏳` → `✅`/`⚠️`）。
2. `UIWindow.img/Skill/skillPoint` 若确认缺失，保留当前端 `send_spup` 降级路径并在 README §5 记为「新增（环境适配）」，不改回原生。
3. 跑 `build_smoke.bat` 验证（IMP 门禁恢复后）。

## §5 禁引 095 线

- `docs/features/port-095/**` 全禁（含 `evidence/opcode-v95/dec/*`）。
- 技能面无 095 注释污染点（区别于第七面 `Net/Packets/CashShopPackets.h:182` CubeRerollPacket 注释）。
- 基准唯一 = 083/v83（Angel 13339 IDB）；协议值 s9 优先。
