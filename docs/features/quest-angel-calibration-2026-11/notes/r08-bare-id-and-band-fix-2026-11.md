# 第十一面 · 任务窗裸 ID + 底带按钮一次性修复 (r08)

> ⚠️ **本轮的按钮结论（§0 第 3 行、§1.2、§2.3、§4 遗留 2/3）已被用户 2026-11-23 23:32 当面纠正为错误**：
> 「寻找NPC位置」不是 tab0 常显，而是**只在详情页显示**；「详细」不是自造冗余钮，而是**详情页展开开关**。
> 以 [`r09-band-button-correction-2026-11.md`](r09-band-button-correction-2026-11.md) 为准。裸 ID 修复部分（§1.1/§2.1/§2.2）仍然有效。
>
> 时间：2026-11-23 22:50 CST。性质：**实现 + 构建验收**（非真机）。
> 触发：用户提供三张 `XiaoYe_2026-11-23_22-0*.png` 截图，问「任务有没有按原生写？这些 id 是什么？按钮位置对吗？」→ 确认「改全」。
> 范围：仅客户端 `UIQuestLog` / 新增共享 `QuestTextFormatter` / i18n / vcxproj；**零协议 / 零 WZ / 零服务端**。

## 0. 结论（先说结果）

| 问题 | 结论 |
| --- | --- |
| 任务按原生写了吗 | 结构按原生（r07 IMG/IDA 校准），但**文本令牌未展开** → 裸 ID；本轮补齐。 |
| `9310088` / `4031988` 是什么 | `9310088`=NPC id（工作人员O，任务 10000「奇怪的提议?!"）；`4031988`=道具 id（特殊工作人员徽章，任务 10002）。原生是 `#p` / `#t` 令牌，应显示可读名。 |
| 按钮位置对吗 | 坐标算法对（右锚 + 带中线），**但按钮集合/显隐错**：tab1/2 的「放弃」被隐藏的「详细/寻找NPC」顶离右锚。本轮按页签重排。 |
| 真机验收 | **未验收**。仅完成源码 + Release 构建（`build-quest-fullfix-20261123.bat` EXITCODE=0），未跑真机发包/截图。 |

## 1. 根因链（逐条行号级）

### 1.1 裸 ID 怎么冒出来的

1. **活数据原文**（`E:/MXD/BeiDou-Client-xy/Data/.nxcache/Quest.nx`，`tools/nx_inspect.py` 只读）：
   - `QuestInfo.img/10000/0` = `从#b#p9310088##k那里收到了紧急信息。`
   - `QuestInfo.img/10002/1` = `…箱子里的#b#t4031988##k回收回来。\r\n#i4031988# #t4031988# #b#c4031988# / 1…`
2. `Data/QuestData.cpp:161` `line.get_string()` 原样入 `pages/summary`；`:168` 用字面 `"\\n"` 拼段。
3. 旧 `UIQuestLog.cpp:804`（本轮改前）把 `sum` 原样 `emplace_back(Text(..., line, wrap))`，`Text` 默认 `formatted=true`（`Graphics/Text.h:128-129`）。
4. `GraphicsGL::LayoutBuilder::add`（`Graphics/GraphicsGL.cpp:1042-1276`）遇 `#p`/`#t` 只 `skip+=2` 吃掉前缀与闭合 `#`，**中间裸数字照常画**（`case 'p':1205-1209`、`case 't':1239-1245` 均为 `TODO: Needs implemented`）。→ 屏幕出现 `9310088` / `4031988`。
5. 正确展开能力只存在于 `UINpcTalk::format_text`（`IO/UITypes/UINpcTalk.cpp:870-1120`），但是私有成员、末尾还调 `strip_simple_choices()` 依赖对话框状态，任务窗无法直接复用。

### 1.2 按钮位置怎么错的

- 坐标算法 `relayout_slots()` 本身对（`BAND_MID=378` - 半高，右锚 `panel_right-9`；IMG 一手校验见 r07 §3）。
- `relayout_slots()` 旧实现**一次排开 6 颗**（MARKNPC/DETAIL/GIVEUP/OK/NO/ALERT），从右锚连续左移；`activate_buttons()` 用 `set_active(false)` 隐藏其中几颗，但**隐藏的钮仍占着 slot_tl 坐标**。于是 tab1/2 可见的「放弃」并不贴右缘——被不可见的「详细/寻找NPC」顶开。
- 原生逐页实测（`C:\Users\11036\Desktop\正确客户端截图\` 三张任务框 + orange-wz 10012 按钮精灵一手，见 `evidence/orange-wz-quest-tree-r08.json`）：

| 页签 | 原生底带（右→左） | 精灵 |
| --- | --- | --- |
| tab0 可开始 | 寻找NPC位置 | `BtMarkNpc` 81×17 |
| tab1 进行中 | 放弃(红) + 任务讯息(灰) | `BtGiveup` + `BtAlert` 56×16 |
| tab2 已完成 | 放弃(灰禁用) + 任务讯息(灰) | 同上 disabled 态 |

## 2. 本轮实现（行号级）

### 2.1 新增共享令牌展开器（无状态）

- `Util/QuestTextFormatter.h` / `.cpp`（新文件）：把 `UINpcTalk::format_text:872-1119` 的令牌规则按**相同语义**做成无状态函数，逐处展开 `#h/#p/#t/#z/#q/#m/#o/#c/#x`，吞 `#E/#I/#S/#K` 指引元；色标/图标标记（`#b/#k/#r/#i/#v/#s/#f/#B`）原文保留交 LayoutBuilder。
  - `#p` 取不到名（如 9310088）→ 用 `npcFallback` 补主语；**绝不把裸 id 留屏**（对齐原生 `format_text:903-911` 解析失败吞令牌语义）。
  - `keepSimpleChoices=true` 时不剥 `#L/#l`（任务描述无选项，也无 `simple_choices` 状态）。
- **刻意不改** `UINpcTalk`（已验收对话框路径；两套实现规则同源、各自独立，零回归风险）。

### 2.2 接入任务摘要路径

- `IO/UITypes/UIQuestLog.cpp:14` 增 `#include "../../Util/QuestTextFormatter.h"`。
- `update_detail()` 摘要段（现 `:806-822`）：`emplace_back` 前先 `QuestTextFormatter::format(line, 0, npc_fallback, true)`，`npc_fallback = Locale::text("quest.detail.npc_unknown_inline")`。
- i18n：`locale/zh-CN.ini` / `en-US.ini` 增 `quest.detail.npc_unknown_inline`（zh=工作人员 / en=the staff）。

### 2.3 底带按页签重排

- `relayout_slots()`（现 `:976-1004`）：按 `tab` 选 `band_tab0={MARKNPC}` 或 `band_tab12={GIVEUP, ALERT}`，**只摆本页可见钮**，从右锚连续左移；放弃贴右缘（tab1/2）。
- `activate_buttons()`（现 `:1047-1075`）：`MARKNPC=tab==0`、`ALERT/GIVEUP=tab==1||2`、`DETAIL=false`（去掉自造常显「详细」，详情改为单击行）、`OK/NO=false`；`ALERT` 恒 `DISABLED`（灰）、`GIVEUP` 仅 tab1 可点，tab2 `DISABLED`（灰）。
- `change_tab()`（现 `:841`）：加 `layout()` 调用，切页时重排底带坐标，否则沿用上一页 slot。
- 更新 2 处过时注释（`:951` 底带盖住哪几颗、`:1123` ALERT 常灰语义）。

## 3. 验证与证据

- **Release 构建**：`build_quest_fullfix_20261123.bat`，`MapleStory.sln /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v142`，`EXITCODE=0`，无 `error C/LNK`；产物 `build-quest-fullfix-20261123/MapleStory.exe`（5,576,192 B，SHA256 前 32=`28eb00e3578535c6876efb591dde3e73`）。
- **橙色 MCP 一手**（`AGENTS.md` 门禁 8/18 主路径）：`orange-wz` 10012，128B key（`tools/_gms_key_128.b64`）+ `ivBase64=TSPHKw==` + `autoParse:true`，成功 `load_files` + `get_node_tree_json` `UIWindow.img/Quest` 全子树（存档 `evidence/orange-wz-quest-tree-r08.json`）；并按同规格实查 `String/Npc.img/9310088` → 「节点路径不存在」（该 NPC 在 live 数据确实缺席，故走 fallback）。
- **全链文本模拟**（Python 按 formatter + LayoutBuilder 双规则复算两条真实任务串）：
  - `10000/0` → `从工作人员那里收到了紧急信息。`（裸 `9310088` 消失）
  - `10002/1` → `…特殊工作人员徽章回收回来。[icon] 特殊工作人员徽章 3 / 1…`（裸 `4031988` 消失；图标由 `#i` LayoutBuilder 绘制，持有数由 `#c` 展开）
- **未做**：真机启动截图、发包验收、登录器部署。以上仅为源码 + 构建 + 数据层证明。

## 4. 限制 / 遗留（不伪造闭环）

1. `9310088` 在 `String/Npc.img` 缺席 → 描述里 `#p` 用占位「工作人员」补主语；**不是**精确 NPC 名（live 数据无此名，无法一手还原；未编造）。
2. `DETAIL` 按钮与 `detail_mode` 成员保留但 UI 不可达（详情态字段代码仍在，无害）；如需彻底删除可另起清理，本轮遵循最小改动。
3. `MARKNPC`（寻找NPC位置）仍是显式预留：原生要把 NPC 地图在世界地图点出，需 npc→map 反查 + UIWorldMap 标记入口，本包无对应封包证据——**不发明协议**，仅保证按钮显隐/位置与原生一致。
4. 真机截图来源端待用户确认：本轮三张 `XiaoYe_*` 截图按「当前开发端」分析（显示裸 ID，与原生参考图行为相反）；若它们其实是参考端，结论需重判。
