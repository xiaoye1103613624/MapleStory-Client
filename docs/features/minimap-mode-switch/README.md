# 小地图模式切换还原 + 悬浮 tip 内容层对齐（2026-11-05）

对应诉求（m00912 / m01051）：
> 「白屏问题解决了，小地图应该有模式切换的，而不是一直都是最大展开的，你对一下原客户端逻辑。鼠标悬浮小地图上的 tip 样式是和原客户端一样吗？」
>
> Q1 裁决：「完整还原，自带功能样式 img 里面是有的呀，没必要重新绘制吧？」
> Q2 裁决：「原生切换状态是记录的？每个图都是一样状态的吧」
> Q3 裁决：按原生通用 tip 规范补齐内容层。

本线**不重绘、不碰素材**：全部用运行时数据包里现成的真件贴图。REA/IDA/orange-wz MCP
本会话未挂载，按 `xiaoye-maple-story-dev` 门禁用等价路径取证：`tools/nx_inspect.py`（.nx 节点只读）、
部署目录 `*.img_runtime.json`（**运行时散装 img 权威数据**）、原生参考帧 + 源码控制流。

---

## 1 结论速览

| 问 | 定案 |
| --- | --- |
| 为什么「一直最大展开」 | `UIMiniMap.cpp` 换图时 `if (town && town.get_bool()) type = Type::MAX;` **城镇强制 MAX、覆盖用户设置**；且构造只注册了 `BT_MAP`，`BT_MIN/BT_MAX` 逻辑在、按钮不存在；切态也从不写回配置。 |
| 原生是不是全局单态、跨图一致 | 是。`Configuration.h` 的 `MiniMapType("minimap.type","2")` 是**单一全局值、非 per-map**；`user_type` 即它。旧代码的城镇强制是偏离原生的分支。 |
| 状态是否持久化 | 配置键在，但旧代码只有 `load()`、切态从不 `save()`。已补 `Setting<MiniMapType>::save()` + `Configuration::get().save()` 落盘到 `Settings`，重启保持。 |
| −/＋ 钮要不要重绘 | **不用**。全包 `Basic.img/BtMin`（蓝 −）/`BtMax`（灰 +）12×12 四态是现成真件，聊天栏早已使用。 |
| minimap 悬浮 tip 框体 | **已与原生一致**（共享 `TooltipBackdrop`，逐像素定案：实心深蓝 α1.0 + 灰 1px 边）。 |
| tip 内容层 | 按原生通用 tip 规范把标题改 `A12B`（honor `bolded`）；描述 `A12M`、分隔线白 0.4、对齐尺寸沿用。Mob/Npc 图标包内确无 → **显式预留，不臆造贴图**。 |

---

## 2 数据源翻案（旧文档错在哪）

`docs/features/ui-v83-window-restore/` 之前多处写「本包无 `BtMin/BtMax/BtSmall/BtBig/BtNpc`，
素材代次差异，无法还原」，并把 `refcrops/minimap_5x.png`（有 −/＋ 两钮）**排除出证据链**、
判为「不是本包客户端截图」。这是把**子树结论错误外推成全包结论**：

- 客户端运行时跑**散装 img 目录** `E:/MXD/BeiDou-Client-xy/Data/UI/UIWindow.img`
  （`_resource_probe.log`：`DataFormat: img` / `locator picks img (score=15)` /
  `UIWindow[MiniMap]: UIWindow2=no UIWindow=yes ... children=5`），不是 9 月的 `.nxcache/UI.nx`。
  项目既有教训：`docs/features/map-object-audit/evidence/codebuddy-transcript-947f8ba0.md:106`
  「打包源与运行时数据版本漂移…像素级修 bug 一律以部署数据为准」。
- `UIWindow.img_runtime.json` 的 `/MiniMap` 子树 = `['BtMap','MaxMap','Min','MinMap','title']`
  —— 确实**这棵子树**只有一颗 `BtMap`。（两数据源一致，旧结论对子树本身成立。）
- **全包**遍历 `*.img_runtime.json`：`Basic.img_runtime.json` 的 `/BtMin`、`/BtMax` 各 4 态
  `{normal,mouseOver,pressed,disabled}/0` **12×12**；项目聊天栏早已当真件用
  （`ui-v83-window-restore/README.md:70`、`evidence/w4-chatbar/coords.md:18`
  「逐像素 ASCII 比对：与 `Basic.img/BtMin`（横杠在第 7..8 行）完全一致；`BtMax` 是十字」）。
- ⇒ `minimap_5x.png` 顶带的蓝 − / 灰 + 正是 `Basic.img/BtMin|BtMax`，**回归为还原目标**。

`BtSmall/BtBig/BtNpc` 全包确无，保留为显式预留（不发明贴图、不臆造协议）。

已同步翻转/标注的旧文档：`ui-v83-window-restore/README.md §5`、`CHECKLIST.md`、
`notes/open-issues.md §C2`、`evidence/w11-map/minimap-band.md §1/§4/§6/§7`、
`evidence/w11-map/pixels-alpha.md`。

---

## 3 修复清单（源码，行号以 2026-11-05 工作区为准）

`IO/UITypes/UIMiniMap.cpp`：
- 构造 `:88-103`：新增 `nl::node basic = nl::nx::UI["Basic.img"];`，`BT_MIN`→`basic["BtMin"]`、
  `BT_MAX`→`basic["BtMax"]`（`MapleButton` + `UiAssetResolve::usable` 守卫）；保留 `BT_MAP`→`MiniMap["BtMap"]`。
  `toggle_buttons()` 的 `set_position` 会覆盖初始 `(0,0)`，故起始坐标无妨。
- 显式 `#include "../../Configuration.h"`（原有 `Setting<MiniMapType>` 靠传递包含）。
- `update()` `:253-254`：删 `nl::node town = Map["info"]["town"];` 与城镇强制，改 `type = user_type;`
  —— 全局单态跨图一致；`has_map` 的 2048 世界坐标门禁（上轮白屏线修复）**保留不动**。
- `send_key` 尾部 `:436`：删「任意键循环切态」调试分支（非原生行为）；UISoftKey ChangeMap / 'W' 分支保留。
- `button_pressed()` `:443-467`：新增
  `auto persist = [this](){ user_type = type; Setting<MiniMapType>::get().save(static_cast<uint8_t>(type)); Configuration::get().save(); };`
  `BT_MIN`/`BT_MAX` 加钳制 `if (type > Type::MIN) type -= 1;` / `if (type < Type::MAX) type += 1;`，
  切态后 `persist()`。返回态与 `toggle_buttons()` 的 `DISABLED`（MIN 禁 −、MAX 禁 +）配合原生置灰。
  `BT_MAP` 注释同步改正（缩放由真件钮逐级切换、持久化到 `minimap.type`）。
- `toggle_buttons()` `:559/:614/:629`：修两处宽度笔误 —— 原版两钮都不存在、所有 `+=` 恒为 0
  从未暴露，注册真件后必现：BT_MIN 位推进 `bt_max_width`→`bt_min_width`、SMALL/BIG 缺失时
  `bt_max_width`→`small_w`（否则 `BT_MAP` 右溢出 13px）。
- `update_canvas()` `:741`：`W_min = combined_text_width + bt_min_width + bt_max_width + bt_map_width + 18;`
  —— Min 三切片底宽补上两钮宽，与 `toggle_buttons():min_dimensions` 对齐。
- `send_cursor()` `:369/:393`：NPC 标记 / 传送点 tip 的 `show_map(..., bolded, portal)` 第 5 参
  `false`→`true`（让标题走 `A12B`）。

`IO/Components/MapTooltip.cpp` `:320-324`：`set_minimap_title(bool bolded)` 旧实现忽略参数、恒
`A12M`；改 `title_label = Text(bolded ? A12B : A12M, CENTER, WHITE, title)`，与
`set_worldmap_title()`、`EquipTooltip.cpp:268/:511`（原生装备 tip 名称 A12B）一致。
`MapTooltip.h:50` 声明本就是 `set_minimap_title(bool bolded)`，无需改。

持久化语义（`Configuration.h:41/:65/:240-256`）：`Setting<>::save()` 只改内存 value，
**必须**再 `Configuration::get().save()` 才写盘（FILENAME="Settings"）。范式见
`UIOptionMenu.cpp:619-621`、`UILogin.cpp:426-427`。`load` 优先级 `xy-config.yaml` > `Settings`，
`xy-config` 无 `minimap.type` 键、不会覆盖。

---

## 4 tip 内容层：可落地项与显式预留

- 框体：`TooltipBackdrop::draw()`（`IO/Components/TextTooltip.h:28-58`）= `ColorBox(0.333,0.333,0.510,1.0)`
  实心深蓝 + 灰 `(0.5,0.5,0.5)` 1px 四边，5 个 tooltip 组件共用；
  `evidence/w5-tooltip/coords.md:82-93` + `native-verdicts-2026-11-03/native-tooltip-alpha-raw.txt:16-22`
  逐像素坐实与原生一致 → **框体不动**。
- 名称 `A12B`、描述 `A12M`、分隔线白 α0.4、对齐/尺寸沿用 → 按原生通用 tip 规范对齐。
- `Mob/Npc` 图标：构造取 `UiAssetResolve::window("ToolTip")["WorldMap"]["Mob"/"Npc"]`，
  运行时 `UIWindow.img_runtime.json` 的 `/ToolTip` children 只有 `['Equip']`、无 `WorldMap`；
  全包可作 minimap tip 通用 Mob/Npc 图标的节点**确无**（只有 `/MobGage/Mob` 按怪 ID 的
  25×25 血条图标）。用户禁重绘、无原生 minimap 专属 tip 参考帧 ⇒ `Mob.draw/Npc.draw`
  保持空纹理跳过，**显式预留**，不臆造贴图。
- `listNpc` 行悬浮 tip 走 `show_text()` → `TextTooltip` 纯文字框（非 `MapTooltip`），
  属另一条链路，本次不动。

---

## 5 构建 / 部署 / 协议门禁

- `tools/gen_opcode_matrix.py --check` → `GATE OK: opcode matrix matches both sources (s9 44d0d5d9ca)`。
- 直接调 `MSBuild MapleStory.sln /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v142 /m /nologo /v:minimal`
  （`export MSYS2_ARG_CONV_EXCL='*'`，单 `/p:` 避开 MSYS 路径转换）；**先确认部署目录无活客户端占 exe**，
  否则 `LINK : fatal error LNK1104: 无法打开 ...\MapleStory.exe`。删 obj 强制重编。
- 门禁 `EXITCODE=0`、0 error；vcxproj `OutDir=E:\MXD\BeiDou-Client-xy\` 直出部署目录。
- 部署铁证链（源码→obj→exe）：`UIMiniMap.cpp` 10:03:11 / `MapTooltip.cpp` 09:51:10 →
  `UIMiniMap.obj` 10:04:29 / `MapTooltip.obj` 09:55:47 → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
  10:06:27 / 4676096 B。并行会话 13:06 重建过 exe（4687360 B），但 exe 内独特字面量
  `grep -a -c 'minimap btmin'` = 1、`'Basic.img/BtMin'` = 1、`'setfield_trace.log'` = 1 ⇒
  我的改动确实编进最终产物（Release 的 `LOG` 是 `void(0)`、字符串被优化，
  **不能**用 LOG 字符串判断，只能用非 LOG 的真实字面量 / 时间戳链）。

---

## 6 实机验证（决定性抓帧，归档在 `evidence/`）

| 帧 | 结论 |
| --- | --- |
| `01_minus_btn_mouseover.png` | 顶带三钮齐全：蓝 −（x419..430）/ 灰 +（x432..443，MAX 态置灰）/ 橙「大地图」（x445..480），y28..38；光标 MOUSEOVER 亮蓝。与 `minimap_5x.png` 吻合。 |
| `02_max_to_normal.png` | 点 − 后 **MAX→NORMAL**：72px 顶带 + 盾徽两行字 → 29px 顶带 + 场景预览。 |
| `03_normal_to_min_strip.png` | 再点 − **NORMAL→MIN**：收缩条「小地图」+ 一行「金银岛」+ −/＋/大地图三钮。 |
| `04_restart_persists_normal.png` | `Settings` 的 `minimap.type` 2→**1**（mtime 12:10:11 落盘）；kill + 冷启动后**直接 NORMAL 起步**，不再被城镇强制成 MAX ⇒ 持久化 + 全局单态 + 去强制三项同时坐实。 |

### 测试夹具教训（重要，防复发）
一切「点击无响应」最初都是**驱动伪影，不是代码 bug**：B4 修复删 `SDL_WarpMouseInWindow`
反馈环后，`IO/Window.cpp:298` 虚拟光标 = 锚定帧真实光标 + `gain`×累积相对位移
（`ui.sysopt.mouse_speed` 52% → gain=1.4），`UI::send_cursor(true)` 不带坐标、用虚拟光标命中
（`IO/UIElement.cpp:183-211`）。`SetCursorPos` 绝对跳变被 1.4× 放大，系统光标与游戏光标脱节。

死路（保留）：模板法找白手 → 误锁 Henesys 盾徽；双帧 diff → 场景动画淹没；pair 配对白斑 →
误锁 quickslot 键帽；小步闭环 aim → SDL 合并 motion 事件、shift 不固定。

**突破·锚定法**：`IO/Window.cpp:284-289` vcursor<0 时首次 motion 为锚定帧（virt=真实光标、
本帧不放大）。冷启动后**窗口刚出现立即** `SetCursorPos(client(640,360))` + 微动 1px 把锚点钉死，
之后 `Δvirt = 1.4×Δreal` 精确可控；一次跳到目标按钮即命中。驱动脚本找 `SDL_app` class 窗口
（`_drive2.py` 找 `MapleStoryClass` 会 `wins=[]`）。

---

## 7 防复发判据

1. 判「本包有没有某控件」必须遍历**全包** `*.img_runtime.json`，不能用某棵子树（如 `MiniMap`）外推。
2. 客户端跑散装 img；像素级结论一律以部署目录运行时数据为准，`.nxcache/*.nx` 只是旧缓存。
3. `Setting<>::save()` ≠ 落盘，必须配 `Configuration::get().save()`。
4. 切态逻辑改动要检查 `toggle_buttons()` 里所有 `+= 宽度` —— 按钮不存在时这些恒为 0，
   注册真件后才会暴露笔误。
5. Release `LOG` = `void(0)`；验证 exe 是否含改动用非 LOG 真实字面量 / 源码→obj→exe 时间戳链。
6. 自动点击驱动必须先锚定虚拟光标；「按钮点了没反应」先排除 1.4× 光标增益伪影。
7. 部署 exe 前先 kill 部署目录活客户端，避免 `LNK1104`。

## 8 相关文档

- `docs/features/s9-protocol-align/notes/enterfield-exp-misalign.md`（上一线：白屏 = s9 exp 字段错配）
- `docs/features/ui-v83-window-restore/evidence/w11-map/minimap-band.md`（顶带切片/按钮语义，§1/§4/§7 已翻案）
- `docs/features/ui-v83-window-restore/evidence/w5-tooltip/coords.md`（原生 tip 逐像素规格）
- `docs/features/ui-v83-window-restore/evidence/w4-chatbar/coords.md`（`Basic.img/BtMin|BtMax` 真件来源）
- `docs/features/ui-v83-window-restore/evidence/refcrops/minimap_5x.png`（原生 MAX 态参考帧）

未做 git commit（无用户授权）。
