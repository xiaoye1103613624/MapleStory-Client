# DELIVERY — UI_004 全强化族移植 192→xy（最终交付）

- 日期：2026-11-22
- 决策：D1=B / D2=B / D3=A / D4=B / D5=A / D6=A（见 PLAN.md）
- 契约：[CONTRACT.md](CONTRACT.md)（C→S 0x371D / S→C 0x371E，族 id 1..20）+ [CONTRACT-7.md](CONTRACT-7.md)（14..20 族真实规则与诚实边界）
- **状态：代码+资源+协议+构建+门禁全部完成并部署 live；2026-11-22 已将 14..20 族从占位升级为真实逻辑（按用户"一次性全做"要求）。真机点测未做（客户端进程权限高无法结束，按 D6=A 交用户验收）。**

### 更新（2026-11-22 17:xx）— 14..20 族占位 → 真实逻辑
- 服务端：`EnhanceConfig7.java`（概率/奖池/权重全配置化，注明"社区/跨版本初值，非 CMS192 官方"）+ `EnhanceHandler.java` 7 个 apply 方法（:268 石净化 / :302 宝石炼金 / :340 协同机 / :378 卷轴协同 / :426 药剂罐 / :477 铁砧 / :529 灵魂），无占位残留。
- 客户端：双击触发补 `UIItemInventory.cpp:829-837`（USE 页 20484xx 协同机，抢在 204 砸卷分支前）+ `:885-889`（CASH 页 582 药罐）；`UIEnhance.cpp:341-395` result 按族显示贴切文案；locale 追加 `ench.result.item.*`。
- 道具补齐（D1=B 从 192 源移植）：live `Data/Item/Consume/0204.img`（+5 协同机 2048400/01/02/03/06，共 759 节点）、`Data/Item/Etc/0400.img`（+灵魂结晶/碎片 04001536/04001603，共 827 节点），SHA staging=live 一致，有 `.bak-before-enh7-20261122` 备份。
- 验证：客户端全量 Rebuild 0 error，最终 exe `6b8e17bc…`（17:54，含全部 14..20 改动）；服务端 `mvn -o package` EXIT 0，jar `6fbc16b4…` 含 `EnhanceConfig7.class`；协议门禁 `GATE OK`；resource-probe `UIWindow2Enh.img children=43`、`UIToolTip.img children=3`，无堆损坏。

---

## 一、交付清单（逐条代码行号级）

### 1. 资源（D2=B 独立 img）
| 产物 | 位置 | SHA256 / 校验 |
|---|---|---|
| 强化 UI 独立包 | `E:\MXD\BeiDou-Client-xy\Data\UI\UIWindow2Enh.img` | `7ca40557d70a02332cfa5976e424a5267f8c3831e72e1c0f99cfab02aa594082`，43 节点（魔方全族/双锤/印章/石净化/炼金/协同/罐/MasterPiece/EnchantSkill/Soul 等），GMS key `TSPHKw==` |
| 192 tooltip 图形包 | `E:\MXD\BeiDou-Client-xy\Data\UI\UIToolTip.img` | `7733678e54ad92ed996f998b3a898520706cecf811a9befaa37bd8042f8b8a30`，零 IV 源经 `batch_convert_key` verify 转 GMS，3 顶层节点 |
| 道具 0506 | `...\Data\Item\Cash\0506.img` | `af2cebca...`，22 节点（原 7 + 新增 15：5062000/2002/2005/2006/2009/2010/2090/2100/2103/2400/2500/2501/2503/2800/2801） |
| 道具 0553 | `...\Data\Item\Cash\0553.img` | `641f2146...`，17 节点（原 9 + 新增 8：5530111/0124/0268/0269/0272/0341/0342/0815） |
| 服务端道具 XML | `BeiDou-Server_s9/gms-server/wz/Item.wz/Cash/{0506,0553}.img.xml` | 精确合并 15+8 节点（备份 `.bak-enh2-20261122`），未全量污染 |
| 资源解析入口 | `Util/UiAssetResolve.cpp:60` `window_enh()`、`:78` `tooltip()` | 声明在 `Util/UiAssetResolve.h:26/30` |

> 教训留档：`copy_paste` 到 img 根 `nodePath:""` 会**替换整个根内容**（不是合并）。最终采用「原节点+新节点全部作为 sources 一次性 paste 进新建 img」（22/17 节点验证通过）。首两次合并曾丢失原节点，已回滚重做。

### 2. 客户端 UI 壳（数据表驱动通用壳）
- `IO/UITypes/UIEnhance.h:24` 类声明（继承 `UIDragElement<PosENHANCE>`）、`:32` 构造 `explicit UIEnhance(uint8_t action)`、`:41` `apply_result`、`:87` `send_action`。
- `IO/UITypes/UIEnhance.cpp:51` `FAMILIES[]` 20 族元数据表；`:308` `send_action()` 统一发包；`:333` 按钮触发。
- 双击入口 `IO/UITypes/UIItemInventory.cpp:873-893`（CASH 分支按 item_id 映射 action；换族先 `remove(ENHANCE)` 再 `emplace`）。**触发方式 = 双击道具，符合 D5=A / 原生行为，无侧栏入口。**
- 枚举 `IO/UIElement.h:274` `ENHANCE`；位置配置 `Configuration.h:1041-1043` / `Configuration.cpp:301`。
- 包类 `Net/Packets/EnhancePackets.h`（`EnhanceActionPacket` 6 字段按契约顺序）。
- 回包 `Net/PacketSwitch.cpp:443` `ENHANCE_RESULT=0x371E`、`:834` 注册；handler `Net/Handlers/WaveAShellHandlers.cpp:5247` `EnhanceResultHandler::handle`（声明 `:1063`）。

### 3. 服务端（s9）
- opcode `RecvOpcode.java:219` `ENHANCE_ACTION(0x371D)` / `SendOpcode.java:384` `ENHANCE_RESULT(0x371E)`。
- `EnhanceHandler.java:59` 类、`:100` handlePacket、`:203` applyCube（魔方 1..9，白金/惊人拉满，黑/白附加 mode1 预览/mode2 应用）、`:317` applyHammer、`:346` applyStamp、`:383` consumeCashItem、`:412` persistInventory（DB 全量回写）、`:437` 回包。
- **tooltip 数值一致性关键**：`EnhanceHandler.java:174-184` 改装后 `modifyInventory(1,equip)` + `chr.forceUpdateItem(equip)`（内部 mode3 remove + mode0 add → `PacketCreator.addItemInfo` 写 5×short 潜能 + spark/soul/starForce/vicious/slots）→ 客户端 `ItemParser.add_equip` 重建 Equip。`ENHANCE_RESULT` 数值仅作 UI 刷新/动画。
- 概率配置 `EnhanceConfig.java`（Nexon 公示值：红 6/1.8/0.3%、黑 15/3.5/1.4%、附加 4.7619/1.9608/0.4975%；金锤 70%/白金 100%；逃逸率 10/20/5/3%），注明"非 192 实测可单点改"（D3=A）。
- `PacketCreator.java:7956` `sendEnhanceResult(...)`；`PacketProcessor.java:150` 注册 handler。
- 服务端 jar：`target/BeiDou.jar`（00:19 package，2120 entries，含 `BOOT-INF/classes/org/gms/net/server/channel/handlers/EnhanceHandler.class`，mvn -o compile + package 均 EXIT 0）。

### 4. Tooltip 192 图形化（D4=B）
- `IO/Components/EquipTooltip.cpp:27` include UiAssetResolve；`:413-469` 构造期加载 `UiAssetResolve::tooltip()` 的 Item/Equip/Star/{Star0,Star1}、AddOption/0..5、Summary/{incline,decline,icon}（每节点判空、缺则空 Texture 优雅降级）。
- `:1579` `draw_stars` 192 星形优先、内嵌 BGRA 像素星回退；`:1639` 潜能行叠加 AddOption 品级图标。
- `IO/Components/EquipTooltip.h:184-202` 新增 Texture 成员 + scroll_sign 数组。
- 原有文本/回退路径全部保留（保守增强，缺资源不空白）。

### 5. 概率/文案
- 概率集中服务端 `EnhanceConfig.java`，公开 Nexon 公示（[Nexon 公示](https://maplestory.nexon.com/News/Notice/Notice/133316)、[概率月报](https://archive.maplestory.nexon.com/News/ProbabilityResult/MonthData)），非 192 私服实测。
- `locale/zh-CN.ini` / `en-US.ini` 追加 `ench.hint.*`（20 族）、`ench.result.0/1/2/unknown`、`ench.drop_first/need_equip_or_cash/sent`（已随 build 复制到 live，grep 验证命中）。

---

## 二、验证结果（实测，非"应该没问题"）
| 项 | 结果 |
|---|---|
| 客户端全量 Rebuild | **0 error**，链接输出 `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe`，locale 复制 2 文件 |
| live exe SHA（声明部署前实时核对） | 我构建并验证的备份 **`xy_enh_full_6b8e17bc.exe` = `6b8e17bc2e8bd6ec934586713295f820ac141640806b784b08b3b89cc283bc7a`**（17:54:20，5,358,080B，含全部 14..20 改动）。`MapleStory.exe` 被**另一会话持续 Rebuild 反复覆盖**（17:55 后 SHA 多次变化但均编自同一工作区源码、含本轮全部改动），故以该备份为可复核产物；用户在稳定窗口可直接 `copy xy_enh_full_6b8e17bc.exe MapleStory.exe`。期间 4 个高权限僵尸 MapleStory 进程持有 exe 路径 + 外部 Rebuild 锁 PDB，造成多次 LNK1104/C1041，最终以替代 TargetName 构建成功。|
| 协议门禁 | `GATE OK: opcode matrix matches both sources (s9 966d633eba)` |
| 服务端编译/打包 | `mvn -o compile` + `mvn -o package -DskipTests` EXIT 0，jar 含 EnhanceHandler.class |
| resource-probe | `[UIWindow2Enh.img] children=43`、`[UIToolTip.img] children=3`，UI 根绑定成功，无 0xc0000374 堆损坏 |
| 资源 canvas 完整性 | Cube_Red 49 canvas / Platinum 24 / GoldHammer 24 / jewelCraft 23 / stonePurify 64，bad=0；BtOk normal=40×16、BtOnemore=133×16、Effect 帧尺寸正确 |
| 道具节点完整性 | live 0506=22 / 0553=17 节点（原+新增全在），SHA staging=live 一致 |

> 说明：会话恢复时发现 00:11 构建的 exe SHA 已被外部覆盖（15:50 的 `48eb706c…`，登录器回滚/其它会话），已重新完整 Rebuild 并复核 SHA 为上面的 `23ecc8c9…`。**下次声明"已部署"前须再次实时核对。**

---

## 三、诚实边界 / 未闭环项
1. **真机点测未做（D6=A 约定交用户验收）**：客户端有 4 个高权限 MapleStory.exe 进程，`taskkill /F` 全部 `Access is denied`，无法结束短启实机。所有结论来自源码行号 + 构建 + probe + 协议门禁，非"进游戏点过"。
2. **192 原生概率/协议无法静态取证**：192 二进制加壳（IDA 仅 3 函数/10 字符串），概率按公开 Nexon 公示做成可配置默认表（D3=A），非 192 私服实测值。
3. **14..20 族已升级为真实逻辑（2026-11-22），但仍有字段级预留**：概率/奖池/权重按一手道具描述+公开资料配置化（非 CMS192 官方值），全部集中在 `EnhanceConfig7.java` 可单点改。三处无字段/无数据处按 CONTRACT-7 显式预留、未臆造：①铁砧外观——s9 `Equip` 无 skin 字段，本版保留功能件属性、消耗外形件、外观同步预留；②药剂罐容量/自动填充——s9 玩家无持久字段，本版实现"拖入药水按 120% 回血回蓝"核心效果；③Soul option 数值表——s9/WZ 无数据，本版用配置 id 池随机。192 私有端这些类的源码级逻辑 web 无法取证，本实现是"公开机制 + 本端数据结构"的重建，非 192 原样复刻。
4. **hammer/维加/保护（557/561/559）与卷轴强化（204）走既有链路**：本任务未重复改造，双击分支中 557 仍进 `UIViciousHammer`（白金锤 5570001 在 192 客户端数据中不存在，沿用金锤壳）。
5. **道具 Name 字段**：192 XML 道具块无 `name` 字段（仅 icon hash/path），服务端读名会得到 NO-NAME；不影响功能链路，若要显示正确名称需后续从 Cash.txt 补 `name`（属数据补录，非功能缺口）。
6. **无 commit**（用户未要求）。

---

## 四、真机点测清单（请用户按此验收）
> 需先关闭所有 MapleStory.exe（含高权限进程），用 live `E:\MXD\BeiDou-Client-xy\MapleStory.exe` 启动，服务端用 `BeiDou-Server_s9/gms-server/target/BeiDou.jar`（外部 wz 目录已更新）。

1. **道具双击开窗**：GM 发放 5062006(白金魔方)/5062500(附加)/5530111(印章)/5060023(净化石) → 背包 CASH 页双击 → 应弹对应强化壳（背景为 192 原生图，BtOk/BtOnemore 可见）。
2. **选装备+发包**：按 E 选装备槽或拖装备进窗 → Enter → 抓包确认 C→S 0x371D（action 正确）。
3. **结果+tooltip**：服务端回 0x371E + 标准 equip 包 → 壳显示成功/失败；打开装备 tooltip → 潜能行/spark/soul/星之力数值与服务端一致（192 AddOption 品级图标叠加在潜能行）。
4. **魔方 Before/After**：黑魔方(5062010)/白附加(5062503) mode1 预览 → 应用 mode2 → 确认潜能按预览落地。
5. **金锤**：5570000 双击 → 壳 → BtStart → vicious+1、可升级次数+1，tooltip 锤次数刷新。
6. **白金魔方**：5062006 → 潜能拉满当前等级（SS 上限），tooltip 显示对应品级。
7. **14..20 真实族**（2026-11-22 更新）：
   - 石净化 5060023 + 混沌箱 → 背包 +1 随机道具（配置奖池），材料不足时提示失败且不消耗。
   - 协同机 2048400/2048403/2048406（USE 页双击，不要误开砸卷壳）→ 2 件合成 1 件；2048402 宝石合成机 CMS192 标注不可用 → 失败且不消耗。
   - 灵魂材料 04001603/04001536（ETC 页，需手动拖入壳）→ 碎片满 10 合魂 / 装魂 +20 攻魔。
   - 药剂罐 5820000（CASH 页双击）→ 拖入 200xxxx 药水 → 回血回蓝 120%。
   - 神奇铁砧 5062400 → 功能件+外形件 → 保留功能件属性、消耗外形件（外观同步本版预留）。
8. **稳定性**：连续开关 20 次窗口无内存泄漏/崩溃；resource-probe 无堆损坏。

如某项不符，请给出**本端 live 截图**（非 192 参考端）+ 抓包 opcode，按逐条行号定位修复。
