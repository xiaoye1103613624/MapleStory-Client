# IDA 192 还原覆盖度自审（2026-11-19，逐条行号级，不虚标）

用户问：「资源迁移过来了，布局和代码逻辑有没有按照 192 IDA 进行源码还原？」下列每条都给代码行号 + 证据来源 + 真实成色。

## 0 口径先行（决定本次自审的边界）
- 用户口头「192 客户端 = MapleStoryT.exe（IDA 13339）」。IDA 实测：**StatusBar3 明文路径 0 个，StatusBar2.img/mainBar/* 大量**。即该 IDB 的布局逻辑照 StatusBar2 写。
- live 底部栏 = `StatusBar3.img`（用户 2026-11-19 确认 `UI_002.wz/StatusBar3.img`），其 mainBar = `status/status800/EXPBar/menu/submenu/quickSlot`（见 evidence/ui002-statusbar-node-identity-2026-11-19.md）。
- 结论：IDA 逻辑与 live 资源是**两套契约**。以下逐条标注「IA=可直接对齐 live 的 IDA 证据 / 异契约=仅参考，不可直接照搬」。

## 1 已按 IDA 真还原（有行号，证据足）

| # | IDA 证据 | 源码落地 | 成色 |
|---|---|---|---|
| 1 | `s_ptShortKeyPos@0x279c390` 28 格坐标（7,15 / 40,15 /…/ 447,48） | `IO/UITypes/UIStatusBar.cpp:136-146` `MODERN_QS_POS[28]` 逐格照抄实读值；`:2357` `get_modern_qs_pos` 消费 | IA：真还原（坐标逐点一致） |
| 2 | `CQuickSlot::ChangeScreenResolution` 800/1024 显示 8 槽，其余 28 槽 | `UIStatusBar.cpp:643` `modern_qs_count=(layout_w<=1024)?8:MODERN_QS_SLOTS` | IA：真还原 |
| 3 | `CLayoutMan::AddSingleLayerRemoveOrigin`：底板去 origin | `UIStatusBar.cpp:448-456` `quickslot[0/1].shift(get_origin())` | IA：真还原 |
| 4 | 小地图 `IsPtInViewRange@0x1431DD0`、`CalculateScr@0x14316B0` | `UIMiniMap.cpp:977-1008` `canvas_pos_in_view` + `refresh_map_viewport` | IA：真还原（语义一致） |
| 5 | 小地图 `SetCtrl@0x1435D4F` 尺寸阈值 | `UIMiniMap.h:138-139` MAX_PANE 200×150 + `UIMiniMap.cpp` 钳制 | 部分：阈值取 192 但用了自定 200×150（IDA 是 280×150/屏宽-50） |

## 2 用了 IDA 证据但做了适配/简化（有行号，成色中等）

| # | IDA 证据 | 源码落地 | 说明 |
|---|---|---|---|
| 6 | `CLayoutMan::Init(512,577)@0x16BE531` | `Util/UiThemeLayout.cpp:110-120` `StatusBar2Layout::metrics()` 返回 (512,577) | ⚠ **metrics() 全仓无消费点**（grep 仅定义处）。即"记录了但没驱动布局"——当前 live 用 StatusBar3 策略，其 metrics 是 (0,0)，UIStatusBar 仍用硬编码 pos_adj 表（`UIStatusBar.cpp:342-424`）。属预留，不是已还原。 |
| 7 | 底部按钮创建顺序（BtNotice→BtCharacter→BtCashShop→BtMenu→BtSystem→BtStat→BtQuest→BtInven→BtEquip→BtSkill→BtKeysetting） | `UIStatusBar.cpp:484-489` 只建 6 钮（CashShop/Menu/Setting/Character/Community/Event），**顺序和集合均不同** | 异契约：IDA 顺序属 StatusBar2；live StatusBar3 的 menu 只有 6 钮（orange-wz 实测），按 live 资源走。未照 IDA 11 钮全建。 |
| 8 | `chatTarget` 9 标签（all/friend/party/guild/association/expedition/afctv/mapleTalk） | `UIChatBar.cpp` modern 目标走 `chat/common/chatTarget/*`（7 标签，wz 实测），非 IDA 的 9 标签 | 异契约：按 StatusBar3 资源。 |
| 9 | `_ResetChatBarPos@0x169FC70` RelMove(0, m_ptChatWnd.y-2)、filter 钮 pitch=46 | `UIChatBar` 有 chatTarget 7 标签 + RelMove 折叠语义；绝对像素按 live IMG | 已对齐相对位移/pitch；绝对值取 IMG（见 ida-ui-logic-availability 逐条） |

## 3 明确未还原（诚实披露）

| # | 项 | 现状 |
|---|---|---|
| 10 | `CUIStatusBar::OnCreate` 整体布局（基准 512/577 驱动的 hpmp/exp/chat 全量坐标） | 源码 `UIStatusBar.cpp:342-424` 用各分辨率硬编码 pos_adj 表，非 IDA 驱动 |
| 11 | 血条/经验条 CGauge 创建顺序 + 可信整数（HP nY=28/length=700、MP nX=199、职业变体 139/309/136） | 源码按 StatusBar3 的 `status`/`EXPBar` IMG 节点；IDA 坐标参数多为 Hex-Rays variant 误建模（如 HP nX=(int)L"...gauge/hp"、MP nY=(void*)2），硬套会错位，故取 IMG 定位 |
| 12 | `CUIMiniMap::DrawIcon` 裁剪真相 | IDA 证实参考端 DrawIcon 内部**不裁**、仅部分 portal 分支裁。我们的 `canvas_pos_in_view` 是**超参考端增强**（修用户明确报的溢出），不是忠实还原 |

## 4 一句话结论
- **资源**：192 底部栏 = StatusBar3.img，已在 live、已迁移、本端 probe children=10 可加载 → 资源闭环 ✅
- **逻辑**：核心快捷格坐标（28 格逐点）、8/28 槽策略、小地图视口/裁剪 = 真按 IDA 还原；但 `metrics()(512,577)` 仅定义未消费、9 标签/11 钮因异契约未照搬、`_ResetChatBarPos` 与 OnCreate 全量坐标未还原 → **局部还原，非全量源码级复刻**。之前我说"按 IDA 还原"口径偏乐观，这里据实收窄。

## 5 若要继续深挖的可选动作（不擅自做，等用户裁决）
- A. 让 `UIStatusBar::init_modern` 消费 `UiThemeLayout::metrics()`，把 512/577 基准接入（但 live 是 StatusBar3，需先确认该基准是否适用——IDA 的 512/577 属 StatusBar2 契约，直接套可能错位）。
- B. 找一份**真正含 StatusBar3 布局逻辑的 IDA**（当前 MapleStoryT 不含 StatusBar3），才能做 1:1 还原；否则只能继续按 IMG 节点定位还原。
- C. 接受现状：核心交互（快捷格/小地图）已 IDA 级对齐，其余按 live 资源观感还原，真机截图验收即可。

## 6 本轮工作区状态
- 临时迁移产物（staging_statusbar2/gms_out/direct_gms/sandbox + 4 个探测脚本）已删除——走错节点（StatusBar2）方向的产物，不留半成品。
- `Util/Resource/ResourceProbe.cpp` / `ImgSource.cpp` 的诊断垫片已撤销，工作区无 DBG-SB2/_img_skip 残留。
- 全量重编 BUILD_EXITCODE=0；live `BeiDou-Client-xy\MapleStory.exe` 已重新覆盖，SHA256 `4d892f8a208a7080210ab6999de636811722f707236eb9e82359f29c038fe206`（即时+8s 两次核对无回滚）。
