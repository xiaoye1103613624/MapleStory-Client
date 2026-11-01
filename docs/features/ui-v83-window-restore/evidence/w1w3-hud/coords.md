# W1 底栏身份块 + W3 快捷格 · 坐标与字库取证

> 窗口原点约定：`UIStatusBar` classic 底条 = 屏幕 `(0, 649)`（1280×720），下文 rel 均为相对 `position`。
> 取证三源：**IDA `CreateCtrl` 常量表**（脚本化建控件，坐标权威）、**`nx_inspect` 节点实测**（WxH + origin）、**`正确客户端截图/进入游戏页.png` 逐像素对拍**。
> 日期：2026-10-31。代码位置：[UIStatusBar.cpp](../../../../IO/UITypes/UIStatusBar.cpp)。

## 0. 三条全局判据（先记）

1. `base/backgrnd` **817×71 是固定尺寸精灵**，旧代码把它拉伸到 VWIDTH 会失真；`origin` 实测全 `(0,0)`，所以 **CreateCtrl 的 TL 就是贴图 TL**（`add_btn` 仍加 origin 以防换资源）。
2. `gauge/bar` 与 `gauge/graduation` **340×31 是「同行三联」图集**（HP|MP|EXP 横排），不是上下叠放。→ 旧代码的段间距 `128/255` 是错的（把商城红按钮当成了段界）。
3. `HP/MP/EXP` 三个字标签**已烘焙在 `gauge/graduation` 图集里** → 禁止再建 `Text`，否则重影。同理 `base/backgrnd2` 的「LV.」字样烤在 `x10..29 / y47..59`。

## 1. W1 身份块（RC6）

| 元素 | 节点路径 | WxH / 字模 | rel 坐标 | 依据 |
| --- | --- | --- | --- | --- |
| 底条主板 | `StatusBar.img/base/backgrnd` | 817×71，origin(0,0) | (0,0)，不拉伸 | ② 节点实测 |
| 名字/量规底板（叠加层） | `StatusBar.img/base/backgrnd2` | 570×71 | (0,0) | 上带亮板 y6..29＝名字区；下带深色量规面板 y36..69 x0..562；LV 盒 x0..84 |
| 量规组原点 | `gauge/graduation`（暗边框） | 340×31 | `GAUGE_X=218, GAUGE_Y=38` | ③ 截图：暗框整幅跨 x218..557（=340）；边框 y700..717（18 行），HP 红填充 y702..715（14 行）→ 图集 y0=687 → rel 38 |
| HP 段（填充/边框） | `gauge/bar` x0..107 | 段宽 `SEG_W_HP=108`，裁起点 `SEG_HP=0` | 218+0 | ① 图集同行 1:1 排列，间距即打包间距 0/109/222 |
| MP 段 | `gauge/bar` x109..215 | `SEG_W_MP=107`, `SEG_MP=109` | 218+109 | 同上 |
| EXP 段 | `gauge/bar` x222..339 | `SEG_W_EXP=118`, `SEG_EXP=222` | 218+222 | 同上 |
| HP 闪光 | `gauge/hpFlash/0` | 109×18 | `hpmp_pos + (-1, 13)` | 图集边框自 y13 起（截图 700−687）；1×外扩 |
| MP 闪光 | `gauge/mpFlash/0` | 109×18 | `hpmp_pos + (SEG_MP-1, 13)` | 同上 |
| **LV 数字** | `Basic.img/LevelNo{0-9,blank}` | **11×13**，推进 11+1 | `lvbadge_pos=(50,47)`，多位数以单字中心 `x=lvbadge_pos.x()+5` 居中 | ③ 截图实测屏 x50..60 / y696..708 |
| LV 回退字库 | `Basic.img/ShowLevel{0,1,2}` + `ShowLevel/Bracket.right` | 推进 6−1 | `(50,48)` + 右括号紧随 | `LevelNo` 缺失才用；参考底板 x61..84 全暗、无可画 Bracket → 主路径不画括号 |
| 职业名 | — `OutlinedText(A12M, WHITE/DARKGREY)` | — | `joblabel_pos=(87,40)` | ③ 首行像素 y689 → rel 40。**旧值 21 落在聊天输入行 → 「职业没有显示」的真因** |
| 角色名 | 同上 | — | `namelabel_pos=(87,56)` | ③ 首行像素 y705 → rel 56（旧值 39 同样压在聊天行） |
| HP 数值 | `StatusBar.img/number` Charset | 数字 5×7，`[ ]` 2×9，`/` `%` 7×7 | `hpset_pos=(218+0+19, 41)` | ③ 截图 Lbracket 实测屏 x237 y690 = 段起点 + 烘焙标签宽 |
| MP 数值 | 同上 | 同上 | `mpset_pos=(218+109+22, 41)` | 截图 x349 |
| EXP 数值 | 同上 | 同上 | `statset_pos=(218+222+32, 41)` | 截图 x472 |

### 1.1 number 字库推进模型（RC6 的正解）

`number` 全件 `origin=(0,0)`，**没有 `.` 字形**。参考截图逐列还原（HP `[50/50]` / MP `[50/50]` / EXP `[0.00%]` 全列吻合）：

```
'[' ']'  → getw + 2
其余字形 → getw + 1
'.'      → 4（字库缺，用 1×1 白点 ColorBox 补，下移 7px 贴基线）
垂直     → 以 9px 括号行为基准，7px 字形下移 1px 居中
```

实现：`hud_advance()` + `draw_hud_number()`（[UIStatusBar.cpp:136-175](../../../../IO/UITypes/UIStatusBar.cpp#L136)）。文本格式 `"[hp/max]"`、`"[mp/max]"`、`"[xx.xx%]"`（EXP 截两位小数）。

> **为什么这一处不算「位图字库替换」的完全达成**：原生底栏数值确实是 `number` 位图字库；旧代码用的是 `Text(A11B, YELLOW)`（这正是用户报的「数值颜色不对、字体不对」）。职业名/角色名原生是比例字体，仍走 `Text`/`OutlinedText`，**没有对应 Charset**，已在此说明而非静默跳过。

## 2. W3 快捷格（RC14）

### 2.1 面板选型（按格数，几何全部由图集自身反推，不靠截图）

| 面板 | 节点路径 | WxH | 列 | 等式 |
| --- | --- | --- | --- | --- |
| 窄屏 8 格 | `base/quickSlot08` | 151×80 | 4 | 151 = 9 + 4·28 + 3·7 + 9 |
| 宽屏 26 格 | `base/quickSlot26` | 466×80 | 13 | 466 = 9 + 13·28 + 12·7 + 9 |
| 宽屏 32 格 | `base/quickSlot32` | 571×80 | 16 | 571 = 9 + 16·28 + 15·7 + 9 |
| 行（两行通用） | — | 80 | 2 | 80 = 9 + 2·28 + 1·6 + 9 |

→ 槽 `QS_SLOT=28`，列距 `QS_PITCH_X=35`（28+7），行距 `QS_PITCH_Y=34`，首格 `(9,9)`。
⚠ `quickSlot*` 在 `StatusBar.img/base/` 下（**不是顶层**）；`base/quickSlot` 与 `base/quickSlot26` **像素全同**（已 diff 证实），故不再回退到 `quickSlot`。
宽屏当前取 `quickSlot26`（键位表现 26 键），表扩到 32 自动切 `quickSlot32`。

### 2.2 面板与按钮簇位置

| 元素 | 依据 | 值 |
| --- | --- | --- |
| 面板 | ③ 参考端贴底条右缘：1280 屏 x=814 = 1280−466；面板自身 y2 是 1px 高亮线，截图该线 y643 → 面板顶 y641 = 底条 649 − 8 | `quickslot_pos = (VWIDTH − qs_panel_w, −8)` |
| 按钮簇原点 | `group_x = quickslot_pos.x() − (wide ? 241 : 75)`，簇按 IDA `x` 递增 | — |
| `BtShop/BtNPT/BtMenu/BtShort` | ① IDA `CreateCtrl`：573/629/685/741 @ y543；窗高 578、top≈22 → **底条 rel y = CreateY − 507** → 543→36 | `group_x + {0,56,112,168}, 36` |
| `EquipKey/InvenKey/StatKey/SkillKey/KeySet/QuickSlot/QuickSlotD` | ① IDA：618/648/678/708/738/768 @ y515 → rel **8**；`QuickSlot`(收) 与 `QuickSlotD`(展) **同位单钮**，图标随 `quickslot_active` 切换（用户实测反馈，非两颗并存） | `group_x + {45,75,105,135,165,195,195}, 8` |
| `BtClaim/BtWhisper` | ③ 参考端在按钮簇首两格 y=8 | `group_x + {−15,15}, 8` |
| 簇 x 末端校验 | IDA `741+54=795`、`768+28=796` → 证明**设计网格宽 800**，非右移贴边。「800 网格 + right_adj」是旧版整体居中的产物，**已废弃**：左侧贴边 + 右侧面板贴 VWIDTH 即可 | — |

### 2.3 键帽层（`key` + `number`/`FontMemo` + 描边文字回退）

| 层 | 节点 | 尺寸 | 位置 | 说明 |
| --- | --- | --- | --- | --- |
| 贴纸 | `StatusBar.img/key/0..6` | 28×11 / 18×11 / 19×11 / 22×13 / 24×11 / 19×11 / 21×11 | 以**格左上角**为锚点（`key/0`「Shift」宽 28＝格宽） | 语义 = Shift/Ins/Hm/Pup/Ctrl/Del/End。⚠ **`key/7` 本包是 28×20 红箭头，不是 Pdn** → Pdn 走文字回退 |
| 位图数字 | `StatusBar.img/FontMemo` | 6×7（`'1'` 为 3×7） | 格 + `(1, QS_MEMO_Y)`，`QS_MEMO_Y = (11−7)/2 = 2`（贴纸带内居中） | NUM1–5 用 |
| 字母/Pdn 回退 | — `OutlinedText(A11M, WHITE/DARKGREY)` | — | 格 + `(1, QS_KEYTEXT_Y=1)` | **`Basic.img` 71 个根全查，无 E0 字母位图字体** → 只能文字回退，非偷懒 |
| 图标 | `Item.img` 图标 | origin.y=+32（WZ 锚点在贴图下方） | 格 + `(0, QS_ICON_SHIFT_Y=32)` | 裸 `Texture` 必须同样下移才落到格内 |
| 中文绑定名 | — `Text(A11M, maxwidth=28)` | 行高≈11 | 格 + `(0, QS_LABEL_Y=14)`（14+11 ≤ 28 落格下半带） | 断行复用 T12b 的 CJK 字宽度量，本批不重写换行 |

键位表 `CLASSIC_QS_KEYS[26]` = 参考端 KeyConfig 槽序（Shift Ins Home PgUp 1-5 A S D F | Ctrl Del End PgDn Q W E R T | Z X C V）。

## 3. 本包没有 / 已放弃 / 预留

| 项 | 结论 |
| --- | --- |
| `StatusBar.img/base/gauge` 子树 | **不存在**（`gauge` 是 `base` 的**兄弟根**，不是子件）→ 取用改 `nl::nx::UI["StatusBar.img"]["gauge"]` |
| 底栏展开/缩放钮 | `base` 下**没有** → 展开/缩放由 W4 用 `Basic.img/BtMax`/`BtMin` 实装（聊天栏行） |
| `number` 的 `.` 字形 | 无 → 1×1 白点 `classic_exp_dot` 补位（推进 4） |
| `ShowLevel/Bracket` | 参考底板对应区全暗，判定原生此处不画括号 → 仅作 `LevelNo` 缺失时的回退 |
| E0 字母位图字库 | `Basic.img` 全 71 根无 → 键帽字母走描边文字 |
| `key/7` = Pdn | 本包是红箭头（28×20），非键帽 → 排除在贴纸表外 |
| `quickSlot`（顶层同名件） | 与 `base/quickSlot26` 像素全同，已停止作为回退 |
| 旧「800 设计网格 + right_adj」居中 | 已废弃（依据 IDA 簇 x 末端 795/796 反证原生就是左贴边） |

## 4. 验收判据（统一清单里对应 W1/W3 两条）

- `HP [50/50]` 数字基线与 `正确客户端截图/进入游戏页.png` 偏差 **≤2px**；数字字模宽高 = **5×7**，括号 **2×9**。
- 职业名 rel `(87,40)`、角色名 rel `(87,56)`，两行都落在 `backgrnd2` 下带深色面板内，**不再压聊天行**。
- LV 数字橙色盒（`LevelNo` 11×13）居中在 `backgrnd2` 的 LV 盒（x0..84）内，「LV.」字样不重影。
- 快捷格两行 26 格、面板贴右缘 x=814、格内左上角键名可见、格下半带中文名可见。
