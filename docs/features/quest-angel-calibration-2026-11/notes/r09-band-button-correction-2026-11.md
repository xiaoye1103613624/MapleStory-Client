# 第十一面 · 底带按钮原生逻辑纠正 (r09)

> 时间：2026-11-23 23:32 CST。性质：**纠正 r08 的错误按钮结论 + 实现 + 构建验收**。
> 触发：用户当面确认三张 `XiaoYe_2026-11-23_22-0*.png` 是**本端截图**（裸 ID 修复方向正确），
> 同时纠正按钮逻辑：**「寻找NPC位置」只在详情页显示，「详细」按钮用于展开详情页，这是原生逻辑。**
> r08 把「详细」当自造冗余钮删掉、把「寻找NPC位置」固定在 tab0 —— **均错**，本轮改正。

## 0. 我上一轮错在哪（留痕，不掩盖）

| # | r08 的错误认知 | 用户纠正的原生逻辑 | 证据 |
| --- | --- | --- | --- |
| 1 | 「详细」是自造钮，原生底带没有 → `act(DETAIL,false)` 隐藏 | 「详细」是**详情页展开开关**，恒显 | r07 §2.2 IDA `CUIQuestInfo.m_bDetailOpen @0xb80` / `m_bDetailShown @0xb84`（详情展开/可见状态机）；用户当面确认 |
| 2 | 「寻找NPC位置」tab0 常显 | **只在详情页（detail_mode=true）显示** | 用户当面确认；r07 §2.2 `m_pBtMyLocation @0xc6c`（我的位置按钮随详情态出现） |
| 3 | tab1/2 = 放弃+任务讯息（摘要态），无「详细」 | 摘要态 tab1/2 = 放弃+任务讯息+**详细**；详情态统一 = 寻找NPC位置+详细 | 本端截图 + 用户说明 |

根因：r08 只用「参考端截图逐页静态看底带」下结论，没有把**详情态这个维度**纳入——
参考端截图恰好都停在摘要态，于是把「寻找NPC位置在 tab0 出现」误读成「tab0 专属」，
实际上那是**该截图当时处于详情态**。教训：客户端自绘 UI 的按钮显隐是「页签 × 状态机」二维，
不能只按单维度截图静态推断（已记入记忆偏好：截图来源端 + 一次性改全）。

## 1. 本轮实现（行号级，改后）

底带二维显隐（详情态 detail_mode × 页签 tab）：

| 状态 | 底带可见钮（右→左） |
| --- | --- |
| 详情态（任意 tab） | 寻找NPC位置 + 详细 |
| 摘要态 tab0 可开始 | 详细 |
| 摘要态 tab1 进行中 | 放弃(红) + 任务讯息(灰) + 详细 |
| 摘要态 tab2 已完成 | 放弃(灰禁用) + 任务讯息(灰) + 详细 |

- `relayout_slots()`（现 `:981-1018`）：`band_detail={MARKNPC,DETAIL}` / `band_tab0={DETAIL}` /
  `band_tab12={GIVEUP,ALERT,DETAIL}`，按 `detail_mode` 和 `tab` 二选一，从右锚连续左移。
- `activate_buttons()`（现 `:1058-1083`）：`DETAIL=true` 恒显；`MARKNPC=detail_mode`；
  `ALERT/GIVEUP=!detail_mode && (tab==1||2)`；`OK/NO=false`。
  灰态：ALERT 恒 `DISABLED`；GIVEUP 仅 tab1 可点、tab2 `DISABLED`。
- `button_pressed()` DETAIL 分支（现 `:1119-1127`）：切换 `detail_mode` 后**新增 `layout()` + `activate_buttons()`**，
  否则切态后按钮集合/坐标不刷新（r08 遗漏点）。
- `change_tab()`（现 `:843-846`）：已有 `layout()` + `activate_buttons()`，因判定读 `detail_mode`，切页自动正确。
- 修正 r08 残留注释（`:843`、`update_dimension` 附近）。

## 2. 验证

- **Release 构建**：`build_quest_fullfix_20261123.bat`（复用），`EXITCODE=0`，无 error C/LNK；
  产物 `build-quest-fullfix-20261123/MapleStory.exe`。源码改后重新构建，时间戳须晚于 `UIQuestLog.cpp`。
- **真机未验收**：未启动游戏、未发包、未部署。本轮结论基于用户对本端截图的确认 + 源码 + 构建。

## 3. 仍然成立的遗留

1. 裸 ID 修复（r08 §1.1/2.1/2.2）不受本轮影响，继续有效。
2. `MARKNPC`「寻找NPC位置」的**跳转功能**仍是显式预留（需 npc→map 反查 + UIWorldMap 标记入口 +
   封包证据，本包没有，不发明协议）；本轮只保证其**显隐/位置**符合原生（详情态出现）。
3. `9310088` 在 live String/Npc.img 缺席，详情/摘要里 NPC 名用占位「工作人员」，非精确名（无一手数据）。
