# W-M1…W-M5 真机验证记录（2026-11-16）

## 构建

- `tools/build_smoke.bat`：先因 `SCROLL_EQUIP` 矩阵漂移 GATE FAIL；`python tools/gen_opcode_matrix.py` 后 GATE OK。
- MSBuild Release|x64|v142：`EXITCODE=0`；OutDir `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2026-11-16 17:08）。
- Post-build 同步 `locale/*.ini`（含 `cashshop.buy_confirm` / `gift_confirm`）。

## 进程与环境

- 登录服 `8484` / 频道 `7575` 在听（java pid 18172）。
- 残留幽灵进程 `MapleStory.exe` pid **47560**（HandleCount=0、无 ExecutablePath、Access Denied 杀不掉；父 46032 已留 dump）。新实例可并行启动，但偶发「账户正在连接」提示。
- 占用策略沿用 `npc-talk-utildlgex-align/notes/client-path.md` **B**：可杀则杀；本幽灵例外只能并存。

## 登录链路实机

| 步 | 结果 |
| --- | --- |
| 启动新 exe | 稳定（20s 监控 FPS≈240，未崩） |
| 账号 `admin` / 密码 `admin`（xy-config `login.pass`） | 可进世界选 / 频道选 |
| 频道选 → 角色选 | 可达（曾见 Admin Lv350 黑骑士） |
| 进图 `SetField` | **已进图**（`setfield_trace.log` 17:20:39，标题 `1-Admin`/`暗之黑骑士`） |
| 商城 | **已打开** CashShop UI：首页格「购买/礼物」、热销榜、EXIT；布局与 `商城.png` 一致 |
| 购买确认 | 点击「购买」受窗口被缩到 ~856×679 + 桌面焦点干扰，YesNo 弹层未稳定抓到；代码路径已接线 |
| 自动登录 | 验证后请将 `xy-config.yaml` `login.auto` 改回 `false`（本轮曾临时 true） |

## UI 截图对照

| 项 | 正确截图 | 结论 |
| --- | --- | --- |
| 商城 | `商城.png`：购买/礼物分钮、价目格 | W-M2：`BtBuy`→`UIYesNo` 确认后 `CashShopBuyPacket(1,sn)`；无效 SN/价目禁用买/礼；礼物先 YesNo 再 SoftKey。与截图「购买/礼物」语义一致 |
| NPC 对话 | `NPC对话.png` 结束/接受/拒绝；`NPC对话2.png` 下项+结束 | W-M5：AcceptDecline 改走 Angel `OnCreate_YESNO` 的 BtYes/BtNo 槽（勿优先 QYes）；CLOSE 底栏保留。与截图按钮色/文一致 |
| Enchant | 无 | Angel RefreshToolTip + IMG 178×206 + MinMS 加深壳 |
| CooldownList | 无 | 新窗 + cooltime 秒→ms；PlayerHandlers 接线 |
| PartyHP | 组员列表相关截图有，无独立 PartyHP 窗 | 维持 UserList 内嵌，`GAUGE_FILL_W=63` |

## 加深轮（同日晚）

| 项 | 结果 |
| --- | --- |
| 构建 | `build_smoke.bat` EXITCODE=0（GATE STALE REPORT/106 非阻塞） |
| 进图 | Admin 暗之黑骑士；商城可开（视口切 800×600） |
| CSNotice/4 | orange-wz：`CashShop.img/CSNotice/4` 子节点 0..4 CANVAS 齐 |
| Enchant Effect | orange-wz：`EnchantSkill/Effect` 0..25 齐；代码 `Animation(Effect)` |
| 购买确认不可见根因 | `UIStateCashShop::draw` 按 Type 升序 → `NOTICE`/`SOFTKEYBOARD` 被 `CASHSHOP` 盖住；已改 focused 后绘 + `send_key` 转发 |
| 购买确认实机 | 修复后重建再验（见 `02-deepen-round.md`） |

## 阻塞 / 后续

1. 幽灵 pid 47560 仍在（可并存）。
2. CD 秒数 / NpcTalk AcceptDecline / PartyHP 宽需进图再点验。
3. 商城预览已跟帧 + 货板试穿；MinMS 预览沙盒（物理/梯子/宠物）仍预留。礼物无原生 CSNotice → YesNo+SoftKey。PartyHP 独立窗预留。
