# R15 技能组合栏真身：`UIWindow.img/SkillMacro`（207x289），不是 `SkillMacroEx`

参考帧：`C:/Users/11036/Desktop/正确客户端截图/技能展开宏指令.png`（379x291，
左=技能窗 175x289，右=技能组合栏）。

## 1. 方法

`tools/nx_inspect.py image` 导出位图 → `E:/project/tmpl_np.py`（numpy alpha-masked SAD
模板匹配）在原生帧上滑窗。**SAD=0 = 逐像素全等**，是本轮所有坐标的唯一依据。

## 2. 外壳归因

| 候选 | 尺寸 | 在原生帧里的命中 | 结论 |
| --- | --- | --- | --- |
| `UIWindow.img/SkillMacro/backgrnd` | **207x289** | SAD 69 @截图(172,0)，右缘 172+207=**379=图宽** | **原生就是它** |
| `UIWindow.img/SkillMacroEx/backgrnd` | 181x303 | 303 > 图高 291，放不下 | 另一套版式（5 行全烘进底衬、无滚动条、无页脚） |

旧代码 `load_window_bg("SkillMacroEx")` 优先 ⇒ 一直用错外壳。

## 3. 组合栏内部件（窗口相对坐标，原点 = 截图 (172,0)）

| 件 | 节点 | 尺寸 | 命中 SAD | 窗口相对左上 |
| --- | --- | --- | --- | --- |
| 底衬 | `SkillMacro/backgrnd` | 207x289 | 69 | (0,0) |
| 分隔线 | `SkillMacro/line02` | 183x1 | **0** | (18,188) |
| 一行三格+箭头 | `SkillMacro/macroslot` | 124x38 | **0** | (18,53)，行距 **45** |
| 行尾宏选择钮 | `SkillMacro/Macroicon/0/icon` | 32x32 origin(0,32) | **0** | (147,56) |
|  | `Macroicon/1/icon` | 32x32 | **0** | (147,101) |
|  | `Macroicon/2/icon` | 32x32 | **0** | (147,146) |
| 名字区灰板 | `SkillMacro/macroname` | 164x57 | 14 | (18,195) |

`Macroicon` 有 `icon / iconMouseOver / iconDisabled` 三态 ⇒ 它是 **TwoSpriteButton 契约的
宏选择钮**（不是"结果技能图标"）。原生帧里 0/1/2 三行各命中一颗，3/4 未出现
⇒ 一屏三行、共五宏、超出靠滚动条。

`macroslot` 内三格中心：列亮度在 x=34 / 72 / 108 塌陷成边框 ⇒ 格中心 x = 18 / 55 / 92，
垂直中心 y = 19。

滚动条：截图 x358..372 整列非白 ⇒ 窗口 x=186；上箭头 y54..66、轨道 67..165、下箭头 166..178
⇒ `Slider(DEFAULT_SILVER, Range(67,165), x=186, rows=3)`。

## 4. 与技能窗的贴合

`UIWindow.img/Skill/backgrnd` = **175x289**（与组合栏同高）。原生帧里技能窗起于 x=0、
组合栏起于 x=172 ⇒ 两窗**重叠 3px**，顶边对齐。
故 `UISkillBook::button_pressed` 改为 `anchor_right_of(*this, -3)`。

## 5. 显式预留（原生有、资源/组件没有）

| 件 | 现状 | 证据 |
| --- | --- | --- |
| 页脚「确定」钮 | **未画** | 对截图 (328..379, 262..290) 扫了 UI.nx 全部 1441 颗 26..56 x 12..26 位图，最优 SAD=63（`Incubator/BtOk/disabled` 40x19，烘焙的是「孵化」）。**包内没有「确定」这颗**，且原生按钮文字是烘在位图里的 ⇒ 该件在别的 .nx 或运行期描字。 |
| 「显示个性技能名」勾选框 | **未画**（标签已烘在 `macroname` y46..56） | 边框扫描在该列找不到任何竖直边线 ⇒ 方框是运行期描的，无资源。`SkillMacroSlot::shout` 字段已存在，接线待原生帧。 |
| 名字白输入框 | **未画**，宏名直接落在 `macroname` 灰板下半 (68,211) | `macroname` 位图内 >248 的纯白像素只有 1 个 ⇒ 框不在资源里。本工程无 TextField 组件（输入走 UISoftKey）。 |
| `macroslot2` 168x42（选中行底） | **未画** | 全帧最优 SAD=228（原生帧里没有选中行），位置无从校准。选中态改用 `Macroicon/N/iconMouseOver` 表达。 |

## 6. 代码

- `IO/UITypes/UIMiscShells.h` `UISkillMacroEx`：换壳 + `macroslot/macroname/line02` +
  5 组 `Macroicon` 两态 + `Slider` + `offset/selected`。
- `IO/UITypes/UIMiscShells.cpp`：新增文件级几何常量块与 `assign_skill_to(idx, id)`
  （旧的全局 `SkillMacros::assign_skill` 会把技能塞进别的宏）。
- `IO/UITypes/UISkillBook.cpp` `BT_MACRO`：`anchor_right_of(*this, -3)`。

## 7. 真机自验（2026-11-01 04:45，构建 md5 `51d3fe17…`）

抓帧脚本 `E:/project/_drive.py`（PrintWindow，DPI-aware）。步骤：`key k` 开技能窗 →
目视确认「宏指令」钮在帧坐标 (718,402) → `click` → 抓帧。

| 断言 | 结果 |
| --- | --- |
| 只弹**一扇**组合栏，不再刷出 UIMacro/UISkillEx/UISoftKey | ✅ 通过（`live-macro-panel-BEFORE-fix.png` 里也只有组合栏，无二级密码键盘）|
| 组合栏贴技能窗**右侧**、顶边对齐 | ✅ 通过（`anchor_right_of(*this, -3)`）|
| 壳 = `UIWindow.img/SkillMacro` 207×289，标题「技能组合栏」烘在 `backgrnd` 里 | ✅ 通过 |
| 行底 `macroslot`（1/2/3 三格）+ 箭头 + `Macroicon` 选择钮 + `macroname` 灰板 + 滚动条 | ✅ 通过（`live-macro-panel-3x.png`）|

### 自验暴露的第二个缺陷：宏条数为 0 时整块编辑区不画

首轮抓帧（改壳之后、本轮构建之前）面板是**一块白板** —— 只有壳、没有行。
查 `UIWindow.img/SkillMacro` 子树确认素材齐全（`backgrnd` 207x289 / `macroslot` 124x38 /
`macroname` 164x57 / `line02` 183x1 / `Macroicon/0..4`），所以不是缺资源，而是
`draw()` 的 `if (idx >= total) break;` 在 `SkillMacros::size() == 0` 时把整段跳过。

而 `size()` 会是 0，是因为 `SkillMacrosHandler`（recv `SKILL_MACROS`）只在服务端**重发**宏表时
才填 `count`：本号登录时没收到（首轮空白），换图（Henesys→Orbis）后服务端重发，第二轮就画出了 1 行。
⇒ 两条都修：
1. `rows = total > 0 ? total : 1`，宏表为空也必须渲染一行空槽（否则面板既看不出版式、也没有可点的选择钮）；
2. `send_cursor()` 的命中范围改成与 `draw()` 同一个 `rows` 界，避免 `total=1` 时点到第 3 行把
   `selected` 抬到不存在的宏上（`assign_skill_to` 会因此跳过前两条直接建第 3 条）。

**遗留（协议侧，未动）**：登录时宏表是否必然下发，需要抓一次真实 `SKILL_MACROS` 包体确认；
若服务端只在换图时发，属服务端行为，不属本 UI 线。
