# 192 魔女森林 / 突破石 完整移植 — 最终实现逻辑（2026-11-21）

## 1. 结论摘要

把 192 客户端「怀旧岛192整合版」`Data/UI/UI_006.wz → UIWindowCN.img` 中的
**魔女森林（`CN_50100`）** 与 **突破石（`CN_Limitbreakthrough`）** 完整移植到
`E:\MXD\BeiDou-Client-xy`，并在 s9 服务端新增协议与真实玩法（接现有经济）。

**已交付**：资源（实体化后转 GMS key）+ 客户端两窗口 UI + 双端协议 + 服务端玩法；
构建通过、opcode 门禁通过、本端 `--resource-probe` = OK、live exe/资源 SHA 实时核对一致。

**诚实边界**：
- 规则（概率/奖励/消耗）为**默认设计**，非 192 原生精确值（192 二进制加壳、服务端无实现，静态不可取证）。
- **真机未验收**：4 个客户端进程以更高权限运行无法结束，未做「进游戏点窗」人工点测；
  本端资源崩溃风险已由 `--resource-probe`（加载全部 UI 根含新 img，exit 0，无 0xc0000374）排除。
- 若用截图做真机验收，截图必须来自 **live 本端**，不得拿 192 参考端截图（USER.md 证据规则）。

## 2. 证据链（行号级）

| 维度 | 结论 | 证据 |
|---|---|---|
| 资源存在 | 两节点真实存在，底板 803×713 / 196×285，全部 canvas 可读 | `evidence/uicn_full_tree.json`、`evidence/canvas_manifest.txt`、`evidence/imgs/*.png` |
| 依赖链 | 突破石 32 条 `_outlink`→Cube_InGame1/Cube_Red；魔女森林外链→StatusBar3 scroll + 30 内链；全部 RESOLVED | `notes/wz-deps.md:32-56,199-240` |
| 原生逻辑 | 192 客户端加壳（仅 3 函数/10 字符串），关键词全 0 命中，静态不可取证 | `notes/ida-192.md` |
| 服务端/客户端 | s9 与主客户端对目标 7 关键词 0 命中，全新实现 | 会话只读取证报告 |
| 密钥 | 192 零 IV `AAAAAA==` 与本端 GMS `TSPHKw==` 不通用 | `AGENTS.md §3.1` |

## 3. 资源工程（关键）

- 源：`UI_006.wz/UIWindowCN.img` + 依赖 `UI_004.wz/UIWindow2.img(Cube_*)`、`UI_002.wz/StatusBar3.img(scroll)`。
- 因本端 nlnx `img_file` **不解析运行时 outlink**（`Texture.cpp:56-76` 的 map 特例不适用 UI），
  构建时把 75 个占位 canvas 用真实位图**内联展开**（`set_png` + 删除 `_outlink`/`_inlink`，origin 保留）。
- 转换：`batch_convert_key(零IV → GMS TSPHKw==, verify:true)` → `ok+verified`。
- 关键教训实证：live `UIWindow2.img` 原本**没有** Cube 节点，若只搬 UIWindowCN.img 不内联展开，突破石按钮/特效会全空。
- 产物：`E:\MXD\BeiDou-Client-xy\Data\UI\UIWindowCN.img`（28,825,229 B，SHA `85F95CF7D8D8C7A83F702CF0E02414D6D30061679B75D411CAAC104BD6CF67F7`）。
- 本端 probe：`[UIWindowCN.img] children=2`，`probe result: OK`，exit 0（无堆损坏）。

## 4. 协议（双端严格对齐）

| 功能 | 方向 | opcode | 结构 |
|---|---|---|---|
| 魔女森林 | C→S | `0x3719` (14105) | `byte action`(0=OPEN,1=DRAW) + [byte drawType 0..3] |
| 魔女森林 | S→C | `0x371A` | `byte respType`；0=状态(long nx,int draws,int secs)；1=结果(byte type,byte count,count×{int id,int qty,byte grade},long nxAfter) |
| 突破石 | C→S | `0x371B` (14107) | `byte action`(0=OPEN,1=USE) + [int itemId] |
| 突破石 | S→C | `0x371C` | `byte respType`；0=状态(long lb,int stones)；1=结果(byte success,long before,long after,int itemId,int remaining) |

`tools/gen_opcode_matrix.py --check` = **GATE OK**，双端无撞号。

## 5. 规则默认设计（可配置，后续可单点调）

### 魔女森林（`WitchForestRewards.java`）
- 50 池：`50`/`50X10` 点券单抽/十连；100 池：`100`/`100X10`。
- 奖励=已存在 s9 道具（红/橙药水、祝福卷、强化卷、稀有宝石等），权重式随机，评级 0..4 映射 WZ `Grade/0..4`。
- 抽奖前有二次确认弹窗（popUp confirm/cancel）后才扣券。
- 累计抽奖次数为 handler 内存态（重启归零，非持久化）。

### 突破石（`BreakthroughStoneConfig.java`）
- 三档突破石道具（itemId 2630800/1/2），成功率 30%/100%/100%，增量 50万/50万/100万。
- 成功调 `Character.gainLimitBreak(delta)`（`Character.java:2779`）→ 内部 clamp + `0x3730` 同步，
  客户端 `LimitBreakSyncHandler`（`CommonHandlers.cpp:52-61`）已能处理，未改破功显示链。
- 客户端展示读 `damage::server_limit_break()`（`Damage.h:44-53`）。

## 6. 涉及文件

**客户端新增**：`IO/UITypes/UIWitchForest.{h,cpp}`、`UIBreakthroughStone.{h,cpp}`、`Net/Packets/WitchForestPackets.h`。
**客户端改**：`IO/UIElement.h`（Type +2，已整工程 Rebuild）、`Configuration.{h,cpp}`、`UISideIconBar.cpp`、`Net/OutPacket.h`、`Net/PacketSwitch.cpp`、`Net/Handlers/WaveAShellHandlers.{h,cpp}`、`MapleStory.vcxproj`、`docs/features/s9-protocol-align/protocol/opcode-matrix.json`。
**服务端新增**：`WitchForestHandler.java`、`WitchForestRewards.java`、`BreakthroughStoneHandler.java`、`BreakthroughStoneConfig.java`。
**服务端改**：`RecvOpcode.java`、`SendOpcode.java`、`PacketProcessor.java`、`PacketCreator.java`。

## 7. 验证与部署

- 客户端 `tools/build_smoke.bat`：EXITCODE=0，新 2 文件已编译进 obj，链接直接输出 live exe。
- 服务端 `mvn -q -DskipTests package`：成功，4 个新类编入 `BeiDou.jar`。
- 部署铁律：exe 与资源均落 live 实际运行目录，无变体名旁挂；实时 SHA：
  - live exe `342FA33631678023DC723F729FBCFA154C42A49655C5D1E33ECBE28834A4C4D5`
  - live UIWindowCN.img `85F95CF7D8D8C7A83F702CF0E02414D6D30061679B75D411CAAC104BD6CF67F7`
- 短启人工点测：**未做**（客户端进程权限不足无法结束 + 需服务端配合）；真机待用户验收。

## 8. 回滚

- 资源：删除 live `Data/UI/UIWindowCN.img` 即可（纯新增，无覆盖其它文件）。
- exe：需从 git/备份恢复（本次 MSBuild 直接覆盖 live 正式 exe；回滚前先备份当前 exe）。
- 服务端：替换回旧 `BeiDou.jar` 并重启服务端。
- 代码：本次改动均在 `feature/dev_0.0.1` 分支，可 git revert 单功能提交。

## 9. 全链路接入清单（2026-11-21 二次补全，已全部接入）

魔女森林 `CN_50100` 的 28 个顶层节点已**全部**在客户端有对应渲染/交互：

| 节点 | 接入方式 | 代码位置 |
|---|---|---|
| backgrnd | 主窗底板 | UIWitchForest.cpp draw() |
| button:50/50X10/100/100X10 | MapleButton 四态 + 二次确认弹窗 | ctor mkbtn / button_pressed |
| button:Help | 文字说明 | button_pressed |
| button:Notice | Notice 公告层（底板+正文） | draw() show_notice |
| button:Close | 关闭 | button_pressed |
| Number(0..9) | 累计次数数字图集 | draw_number() |
| Timer(0..9 + ':') | 倒计时 `M:SS` 图集 | draw_timer() |
| layer:miracleTime | 奇迹时间窗口特效（整点后10分钟） | draw() miracle>0 |
| layer:reward50Bg0..4 / reward100Bg0..4 | 主窗奖励底槽（普通/高级两栏） | draw() |
| Grade(0..4) / Type(0..4) | 评级/类型图标叠加奖励槽 | draw() |
| specialRewardCover0/1 | 评级4特殊奖励封面 | draw() + Result 演出 |
| popUp(backgrnd/confirm/cancel/confirm2) | 逐级 nlnx 访问 + 二次确认 | ctor / draw() show_popup |
| Result(backgrnd/frame/title50/title100/rewardSlot/layer:rewardBg0..10/button:OK) | 抽奖结果全屏演出窗 | draw() show_result |
| Result/Effect + EffectSS(各65帧) | 普通/特殊结果逐帧动画 | draw() fx_frame |

突破石 `CN_Limitbreakthrough` 的 7 个顶层节点亦全接入（backgrnd~4 / Effect 三组×12帧 / BtOk / BtOnemore）。

**协议同步补全（本轮）**：状态包 0x371A 增 `byte miracle`（奇迹时间），`secondsLeft` 由服务端按整点周期下发；结果包 0x371A 增 `byte special`（评级≥4 触发 EffectSS + specialRewardCover1）；Result/button:OK 关闭演出窗。

**重要 bug 修复**：nlnx `operator[]` 不支持 `"popUp/button:confirm"` 斜杠路径（`node.cpp:93-98` 按字面名查找），上一轮 popUp 按钮实际全是空 AreaButton 兜底；本轮全部改为逐级 `root["popUp"]["button:confirm"]`。

## 10. 待办（仅真机验收）

1. 真机验收：进游戏点开两窗，确认布局/按钮/抽奖与突破动画/数值同步（须 live 本端截图；4 个客户端进程权限高于代理无法结束，本代理未做进游戏点测，但 probe 已排除资源崩溃）。
2. 规则数值：拿到 192 官方概率/奖励表后，仅改 `WitchForestRewards` / `BreakthroughStoneConfig`（当前为授权的默认设计）。
3. 侧栏图标入口：`side_icons.ini` 可加 `witchforest`/`breakthrough` 图标（当前已有字符串 arg 入口 `witch`/`forest`/`breakthrough`/`stone`）。
