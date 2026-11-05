# 工作树归属台账 · 2026-11-05（Wave 9.1 前置）

> 目的：把 308 处脏项按功能线切开，供原子提交。判据 = 本会话实际编辑记录 + `git diff` 交叉，不猜。
> 交织文件（两线以上共改同一文件）提交时必须 hunk 级暂存（`git add -p`），先例：
> [commit-hunks-B4b.md](../ui-v83-window-restore/evidence/w8-sysopt-behavior/commit-hunks-B4b.md)。

## 本会话功能线（9 笔原子提交的建议切分）

| # | 提交主题（gitmoji 按仓库规范） | 纯本线文件 | 交织文件（本线 hunk） |
| --- | --- | --- | --- |
| C1 | `⚡ feat(perf): 标题实时 FPS/延迟 + RTT 三后端` | Util/PerfLog.{h,cpp}、Net/Socket{Winsock,Posix,Asio}.{h,cpp}、Net/Session.{h,cpp}、Util/Patch/BootTimer.h | IO/Window.{h,cpp}（标题段/帧计数；底栏并发线同文件） |
| C2 | `⚡ perf(map): 视口剔除 + 伤害数字钳制` | Gameplay/MapleMap/MapObjects.cpp | Gameplay/Combat/Combat.cpp（钳制+手雷上行两 hunk；并发波1-3 若有残留一并核） |
| C3 | `🐛 fix(prewarm): 登录期并行预热进图根 + 装载互斥 + 换图 LRU/重绑 + gather 剪枝（depth==0） + mapload/ROOT 计时` | Util/Resource/{ImgSource,ResourceService}.{h,cpp} | Net/Handlers/SetFieldHandlers.cpp（fade 0.04+分段计时；商城 fade 在 CashShopHandlers/UICashShop） |
| C4 | `✨ feat(wave1): HP/MP 报警跟随角色（0x1000 上行+回灌+探针）` | — | IO/UITypes/UIOptionMenu.cpp、Net/Handlers/WaveAShellHandlers.cpp（UpdateHpMpAlert hunk）、Net/Packets/MiscOutPackets.h |
| C5 | `✨ feat(wave1): 大地图玩家层 376 回包 + 选中即请求` | Net/Handlers/WorldMapHandlers.{h,cpp}（新 .cpp）、IO/Keyboard.{h,cpp}?（不——挪 C7） | IO/UITypes/UIWorldMap.{h,cpp}（玩家层 hunk；**并发=侧栏动画 SIDE_EXPAND_MS，此文件当前编译依赖对方在途代码，建议 C5 放最后或等其收敛**）、Net/PacketSwitch.cpp（376 注册 hunk） |
| C6 | `✨ feat(wave1+3+4): 协议小票打包（快捷栏触发键存储/技能书+召唤袋路由/Buff 右键取消/一键AP/手雷上行/触怪仇恨）` | IO/Keyboard.{h,cpp}、Data/ItemData.{h,cpp}、Net/Packets/{CommonPackets,PlayerPackets,AttackAndSkillPackets,InventoryPackets,SkillMacroPackets}.h | Net/OutPacket.h（6 枚举 hunk）、Net/PacketSwitch.cpp、WaveAShellHandlers.cpp（QuickslotInit hunk）、UIStatsInfo/UIItemInventory/UIBuffList.{h,cpp}、Gameplay/Stage.{h,cpp}（AUTO_AGGRO hunk；Stage 并发=掉落/装载线）、IO/UITypes/UIShop/UICharSelect 预热 hunk |
| C7 | `📝 docs: parity-gap/wave1/wave3/wave4/perf-instrument + 工具（gap 扫描/pe_bits/buff 对拍/rtt 探针）` | docs/features/parity-gap-2026-11、wave1-consistency、wave3-wiring、wave4-battle-packets、perf-instrument-2026-11、tools/{gap_*,pe_bits,buff_hit_layout_check,_rtt_probe.c} | — |
| C8 | `🔧 build: 工程登记 WorldMapHandlers.cpp + PerfLog.cpp` | — | MapleStory.vcxproj（两行 hunk；并发可能另加 ClCompile） |

## 非本会话的脏项（**不代提交**，列名不列内容）

- 地图搜索/信息面板 v2（UIWorldMap 侧栏、UIMapSearchPanel、UIMapInfoPanel、locale、UI/MapInfo|MapSearch 资源）——另一条会话 11-05 晨交付，其记录在 MEMORY 第 5 条。
- GM 命令窗（UIAdmin、UIChatBar、Keyring 线）、字体兜底（Configuration/UIMiscShells）、RaceSelect/Messaging/MapObjectHandlers/PlayerHandlers/Drop/Reactor/Portals/FieldMarkers/Gauge/ChatBalloon/StatusBar/ViciousHammer/StoreBank/CharSelect 等——R28-B4b/GM 会话归属清单已存在的部分沿用旧账。
- `D fonts/SimHei/simhei.ttf` + 3 D 面板文件（UIMapDropPanel 删除？）——**是并发会话的在途删除，来历未明，先问再动**。

## 提交次序与安全阀

1. 先 C7（文档）与 C1/C2/C3（性能线，文件最干净）；
2. C4/C6 交织文件用 `git add -p` 按本表 hunk 拆；
3. **C5/C8 最后**：UIWorldMap.cpp 现含并发未定义符号风险（SIDE_EXPAND_MS 由本会话临时安置，见 wave1 README），等面板会话提交自己部分后我方可干净摘出；
4. 每笔提交前跑 `tools/build_smoke.bat`（GATE OK 才 commit；提交本身不改码）。
5. **等用户一句「按台账提交」再动 git**（门禁：不擅自 commit/push）。

---

## 执行结果（2026-11-05 08:45，未推）

| 笔 | hash | 主题 | 状态 |
| --- | --- | --- | --- |
| C7 | 995c8d5 | docs 全案归档（含证据 .txt 化，绕 .gitignore *.log） | ✅（标题/详情空行待安静窗口补） |
| C1 | 3c38f57 | 标题 FPS/RTT + PerfLog 通道（Window.cpp 按 hunk 摘，IME 两块留下） | ✅（同上补空行） |
| C2 | 7c29713 | 视口剔除 + 伤害钳制 + 手雷上行（包层同笔保可编译） | ✅ |
| C3 | fb4fe4e | 并行预热/互斥/LRU/gather 剪枝（实测 resource −31%）/淡入 | ✅ |
| C4 | 6d66352 | 快捷栏触发键存储 + SetHpMpAlert 包（补 HEAD 已被并发带入调用的依赖） | ✅ |
| C6 | 3d5cca0 | Buff 右键取消/一键AP/书袋路由/触怪仇恨 | ✅ |
| 工具 | b4d2280 | hunk_pick + fix_msg_blankline | ✅ |
| C5/C8 | — | 376 玩家层 + vcxproj 登记 | ⏸ 按计划等地图面板并发线收敛（WorldMapHandlers.{h,cpp} 保持 untracked、HEAD 无引用=自洽） |

- 每笔前 `build_smoke` GATE OK；末次全树 0 error（08:42 exe）。
- 历史重写（C7/C1 补空行）遇脏树拒（并发会话在动文件）——**不强推共享历史**，脚本备好入工具笔 message。
- 待用户答复：`fonts/SimHei/simhei.ttf` 与 UIMapDropPanel 的 3 个 `D` 删除是否授意；是否 push。
