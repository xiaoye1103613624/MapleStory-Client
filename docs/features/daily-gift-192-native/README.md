# 192 原生 28 天每日礼物 · 真窗补全 + 商城发包层补齐 + 签到建表（2026-11-23）

> 用户三决策点拍板后一次性交付。研究结论纠偏：28 天「冒险岛每日礼物」是 **192 高版本官服原生系统**（不是 095、不是本包自造）；本端原 UIDailyGift 只是照 192 截图做的灰盒假窗。192 原生 14 天 quest 签到（AttendanceSystem.img.xml）与此 28 天每日礼物是**两套不同系统**——用户所说"192 签到"指的是后者（每日礼物）。

## 0. 证据链（纠偏关键）
- **版本钉死 192**：orange-wz 直读 `192客户端/Data/UI/UI_005.wz → UIWindow4.img/dailyGift`（backgrnd 741×647 + backgrnd2 741×677 + day_* + count/term 0-9 + button:Reward/Close + mvp*），导出 backgrnd2 与用户截图逐字一致；入口 `UI_002.wz → Statusbar3.img/mainBar/submenu/event/button:dailyGift`。095 排除（浩浩岛095 只有 OneADay/CSOneADay/BtGift/LogoutGift）。详见 `notes/resource-evidence.md` / `notes/protocol-evidence.md`。
- **协议一手（IDA 活实例 13339 + 192 服务端 jar 常量池）**：CUIDailyGift@0x1199d70，领奖 C→S opcode **710(0x2C6)** 仅 int itemId；S→C **1188(0x4A4)** 子包 0=DAILYGIFTDATA 配置 / 1=领奖结果。杀怪 300 = 服务端 quest 状态（按职业群映射 questId，GetQuestState==2 完成）；等级门 nLevelMin=33 随配置下发；MVP 加成查表。详见 `notes/protocol-evidence.md`。

## 1. 本轮实现（三块）
### A. 商城发包层全量补齐（保持 11-21 拍板 live 热卖版口径，不新增 opcode）
- `UICashShop.cpp`：五类发包接线——Gift(0x04)/Take(0x0D)/Put(0x0E)/Wish(0x05)/Package(0x1E)/InvSlot(0x06)/StorageSlot(0x07)/Mileage buy(useNX=2)，复用 `.agents/worktrees/channel-20page-verify/` 现成包体适配 live；购物车改 toggle + 计数 + 购买后清 SN；搜索改本地子串过滤（rebuild_items）；K 项 confirm 后 is_active 时 rebuild_items+update_items。
- `CashShopHandlers.cpp` J 项 4 处硬编码英文 → Locale（name_change_err/world_transfer_err/gacha_jackpot/gacha_result）。
- `UICashShop.h` 加 search_query/preview_background；`locale/{zh-CN,en-US}.ini` 加 7 键。
- 非本需求触碰：`Data/SkillData.h` 加 Stats 默认构造——修复并行会话 11:35 给 unordered_map<int32_t,Stats> 加成员导致的 C2512（无默认构造），阻塞构建的最小修复，零行为改动。

### B. 签到持久化
- dbx 在 `beidou` 库建 `checkin_state` 表（CREATE IF NOT EXISTS，SELECT 验证存在）。`checkin_state` 表此前缺失导致降级内存态、重启丢进度，本轮闭环。@daily 命令按用户决策**不修**。

### C. 192 每日礼物补成真窗（新私有协议 0x3721/0x3722，独立窗，不动 slot3 签到）
- 服务端（BeiDou-Server_s9）：DailyGiftStore/Rewards/Handler/Command + PacketCreator.dailyGiftSnapshot/Result + opcode/注册；`daily_gift_state.sql` 落盘（DDL 未执行，内存降级可用）；杀怪门用真实 quest **4512**（MobRequirement mob 9420512 count 300），等级门 33，28 天真实 v83 道具表。`mvn -o compile` BUILD SUCCESS。
- 客户端：OutPacket/PacketSwitch/WaveAShellHandlers（DailyGiftHandler 解析两 respType，自动开窗）+ UIDailyGift 接真数据（28 格真 ItemData 图标、killCount/300 进度、等级门、premium、按 resultCode 驱动领取）。
- 资源：192 `UIWindow4.img/dailyGift` 经 orange-wz `batch_convert_key(192零IV→GMS, verify:true)` → `Data/UI/DailyGiftStage.img`（764,391B，SHA `42060bba…`，GMS key 可解析 dailyGift/backgrnd）。**未覆盖任何现有 img**（copy_paste+save_as 覆盖大 img 实测会只留粘贴节点毁根，已避开）；客户端 `nl::nx::UI["DailyGiftStage.img"]["dailyGift"]` 加载，回退 window4。

## 2. 验证门禁
- 客户端 MSBuild /m:1 Release x64 EXIT=0、error=0；最终 exe SHA=`64edb57df197c5440b5f174f30fa3b0f06bc5182e6dfb579a44168a985223582`，已按铁律覆盖 live 正式名（src=live 一致，备份 `.bak-before-fullfix-20261123`）。
- 服务端 mvn compile BUILD SUCCESS；双端 opcode 0x3721/0x3722 与快照字段序逐字节核对一致。
- 资源：orange-wz GMS key 解析 live DailyGiftStage.img 的 dailyGift/backgrnd 通过；本端 `--resource-probe` 工具本轮 exit=1 且未刷新日志（工具自身配置问题，非新资源崩溃；img_av.log 无本次记录）。真机加载以启动后观察为准。
- **真机未验收**：协议联调、28 格图标/进度显示、商城各按钮实际发包效果均须用户实机确认（本 agent 无 GUI 权）。

## 3. 待用户 / 边界
- 服务端 `daily_gift_state.sql` 需在 MySQL 执行一次（本轮只落盘未执行，缺表时自动内存降级）；`checkin_state` 已建。
- 服务端需重启以装载 0x3721/0x3722 新 opcode 与 handler。
- 商城购物车/搜索仍为本地语义（s9 无对应 opcode）；BtChargeNX 开网页属原生做法未动。
- 192 原生 14 天 quest 签到（AttendanceSystem）未移植——与本期 28 天每日礼物是两套系统，如需另立项。
- 登录器可能回滚 exe，复核前实时核对 live SHA。
