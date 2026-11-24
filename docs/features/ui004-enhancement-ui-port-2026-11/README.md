# 192 `UI_004.wz/UIWindow2.img` 强化功能整理 + 移植流程 + 触发方式查证

- 日期：2026-11-21（Asia/Shanghai）
- 仓库：`E:\project\MapleStory-Client`
- 来源客户端：`E:\MXD\【2023.8.8江浩生日福利】怀旧岛192整合版\怀旧岛192客户端\冒险岛online192\Data\UI\UI_004.wz`（PKG1，97,338,108 B）
- 取证工具：orange-wz MCP（HTTP `127.0.0.1:10012/mcp`，工具未在本会话暴露为 `mcp__orange_wz__*`，改用仓库既有 `_orange_mcp` HTTP 桥直连）；key = 128B `_gms_key_128.b64` + IV `AAAAAA==`（192 新版本客户端零 IV，见 AGENTS.md §3）
- 证据文件：`docs/_uiwindow2_tree_d2.json`（UIWindow2.img 全树 depth2，628KB）、`docs/_uiwindow2_enh_nodes.json`、本目录 `notes/`
- 诚实边界：**192 客户端二进制加壳**（IDA 仅 3 函数/10 字符串，见 `ui006-cn-port-2026-11/notes/ida-192.md`），静态无法取证原生回调；以下触发结论以「本端已落地的原生行为（源自 Angel 095 反汇编）+ 192 道具描述文本（GBK dump）+ IMG 节点结构」三方交叉得出，未做真机双击实测。

---

## 一、功能清单（UIWindow2.img 内实际存在的强化相关节点）

`UIWindow2.img` 共 315 个顶层节点。其中强化/魔方/锤/潜能/附加/炼金/合成/星相关节点按功能族归类如下（**全部经 orange-wz 根节点树 depth1 取证**，节点后括号为该节点直接子节点数）：

### A. 魔方系（重置主潜能，CASH 页签 5062xxx 双击触发）
| 节点 | 对应道具/说明 | 子节点 |
|---|---|---|
| `Cube_Red` | 红色魔方（5062000 神奇魔方系，B→S） | backgrnd×4, Effect, BtOk, BtOnemore, remainBackgrnd |
| `Cube_Black` | 黑色魔方（5062010 终极神奇魔方，B→SS，可决定是否应用） | backgrnd×4, Effect, Before, After, BtOnemore, remainBackgrnd |
| `MiracleCube` | 神奇魔方基础壳 | backgrnd×3, Effect, BtOk, BtCancel, remainBackgrnd, panel |
| `MiracleCube_Amazing` / `_Master` / `_8th` / `_9th` / `_Memorial` / `_NewYear` | 惊人/大师/8周年/9周年/纪念/新年魔方（共用 MiracleCube 壳族） | — |
| `MiracleCube_Platinum` | **白金魔方**（5062006 白金神奇魔方，B→SS 最高潜能） | backgrnd×3, Effect, BtOk, BtCancel, remainBackgrnd |
| `HyperMiracleCube` | 超级魔方 | backgrnd×3, Effect, BtOk, BtCancel |
| `NewHexaCube` | 新版六角魔方 | backgrnd×4, Effect, BtOk, BtCancel, BtConfirm, textHighlight |
| `CN_10thHugeCube` | 国服10周年巨大魔方（内嵌 `MiracleCube_InGame`） | MiracleCube_InGame, backgrnd×2, Effect, BtOk, BtCancel |
| `Cube_InGame1/2/3`、`MiracleCube_InGame/2/3`、`Cube_Reflect` | 游戏内变体/反射魔方 | — |

### B. 附加潜能系（重置附加潜能，不碰主潜能）
| 节点 | 对应道具/说明 | 子节点 |
|---|---|---|
| `MiracleCube_Additional` | 附加神奇魔方（5062500 大师附加神奇魔方） | backgrnd×4, Effect, BtOk, BtOnemore, remainBackgrnd |
| `AddtionalCube_White` | 白色附加魔方（Before/After 对比） | backgrnd×4, Effect, Before, After, BtOnemore, remainBackgrnd |
| `AdditionalOptionTooltip` | 附加潜能 tooltip | — |
| `AdditionalPotentialStamp` | 附加潜能印章（Game1-3 / Package1-2） | 5 |

### C. 锤系（增加卷轴可用次数，CASH 前缀 557 双击）
| 节点 | 对应道具/说明 | 子节点 |
|---|---|---|
| `GoldHammer` | **黄金锤**（557xxxx，黄金锤50%/100%） | backgrnd×3, BtStart, BtCancel, GaugeBar, EffectP, EffectE |
| `WhiteGoldHammer` | **白金锤**（白金黄锤） | 同 GoldHammer 结构 |

### D. 潜能印章系
| 节点 | 说明 |
|---|---|
| `PotentialStamp` | 潜能印章（金色/银色潜能印章，5530111/5530124，额外加1条潜能）— backgrnd×3, Effect, BtOk, BtCancel |

### E. 石/炼金/合成系
| 节点 | 说明 | 子节点 |
|---|---|---|
| `stonePurify` | **石净化/突破石类**（含 `stone`、`gauge`、`BtPurify`、`Probtab`、`success`/`fail`） | 11 |
| `jewelCraft` | **宝石炼金/合成**（backgrnd×3, BtOk, BtCancel, backAni） | 6 |
| `SynergyMachine` / `SynergyMachine2/3` | 协同/合成机 | 6 |
| `ScrollSynergyMachine` | 卷轴协同机 | 6 |
| `jewelSynergyMachine` | 宝石协同机 | — |
| `itemPot` | 物品罐（含 `satiety`, `pot`, `gauge`, `BtReset`） | 6 |
| `Maker` | 制造/合成器（BtStart, GaugeBar, ComboBox, randomMake 等） | 12 |

### F. 卷轴强化/保护系
| 节点 | 说明 |
|---|---|
| `EnchantSkill` | 强化技能窗（卷轴强化壳，`say0/1/2`, Effect, BtCancel） |
| `ItemProtector` | 物品保护（559 前缀） |
| `MasterPiece` | 大师作品（含 `eqp_back`, `BtUse`, `BtOk`, `Effect`） |
| `SoulCollector` | 灵魂收集器（19 子节点，含 Tab/page/soulPos） |
| `Reset` | 能力重置（AP/SP） |

> 说明：用户列举的「闪耀辐射魔方 / 混沌神奇 / 星岩」在 192 侧的名称对应为 `MiracleCube_Amazing`(惊人)、`Cube_Reflect`/`MiracleCube_Master`、`CN_10thHugeCube`(星之力相关活动魔方) 等；`Use.txt` 中未见独立「星岩」道具，星之力资源在 `Cash.txt` 以「星星N个交换券」(5530815+) 与「星之力12星/17星强化券」(5680445) 形式出现。这些命名差异需在拿到具体道具 ID 时再单点对齐。

---

## 二、各强化页面如何触发？（核心问题）

**结论：绝大多数强化页面 = 双击对应道具（卷轴/魔方/锤/印章），不是常驻专门入口。少数走装备栏拖放 + 快捷键/确认。没有独立的「强化」菜单树。**

### 触发方式证据链

1. **本端已实现并落地的原生行为（Angel 095 `OnDoubleClicked` @0x4efd25 按 prefix 分发）**，见 `IO/UITypes/UIItemInventory.cpp::doubleclick`（L752-1000）：

   | 道具 | 触发条件（代码行） | 打开/行为 |
   |---|---|---|
   | 强化卷轴 `204xxxx`（USE 页） | `item_id/10000==204` (L828-833) | 双击 → `UIEnchantSkill` 壳（不入 UseItem，避免卷轴被当消耗品；原生 204 不在普通 UseItem 之列） |
   | 金锤/恶锤 `557xxxx`（CASH 页） | `prefix==557` (L887-890) | 双击 → `UIViciousHammer`；再 `E` SoftKey 选装备栏槽或拖装备进窗 → `BtStart` 发包（notes r129/r195） |
   | 维加法 `561xxxx` | `prefix==561` (L891-894) | 双击 → `UIVegaSpell` |
   | 魔方 `243xxxx` / `5062xxx` | `prefix==243` (L981-987) | 双击 → 原生重洗「选中的装备」；本端未建 `UICube`，暂按装备栏槽位收口，服务端 `CashOperationHandler` action `0x40` 读 EQUIP 槽位 |
   | 潜能印章 `5530111/5530124` 等 | 交换券双击 (Cash.txt 描述) | 兑换得到印章后，双击印章 → 选装备 → 加潜能 |
   | 铁砧/合成 `5062400` | 神奇铁砧 | 拖两件同类型装备进窗 → BtOk |

2. **192 道具描述交叉印证**（`tools/_192txt/Cash.u8.txt`，GBK→UTF8 解码）：
   - `5062000 神奇魔方`："可以重新设置装备物品潜能的神秘魔方…B级以上，S级以下道具才能使用" → 双击后作用于装备。
   - `5062006 白金神奇魔方`："终极魔方…B级到SS级…最高等级潜能" → 对应 `MiracleCube_Platinum`。
   - `5064000 防爆卷轴`："使用于装备道具后…防止装备被破坏" → 走卷轴强化壳。
   - `5530xxx` 各类交换券："双击可以获得 X 卷轴/印章" → 先双击兑换，再双击实体道具进对应壳。

3. **IMG 节点结构印证**：每个魔方节点都有 `BtOk/BtOnemore`（继续重洗）、`remainBackgrnd`（剩余次数）、`Before/After`（结果对比），锤节点有 `BtStart/GaugeBar/EffectP/EffectE`（"选装备→启动→读条→成败特效"流程）——与「双击道具开壳、壳内选装备并确认」一致，不存在可常驻打开的总入口 UI。

4. **服务端协议印证**（`BeiDou-Server_s9/gms-server`）：
   - `CashOperationHandler.java:90-120` action `0x40` = 魔方重洗（读 EQUIP 槽位，写回 5 条潜能）。
   - `UseCashItemHandler.java:635-648` `itemType==557` = 金锤（`equip.setVicious+1`）。
   - `UseHammerHandler.java` + `USE_HAMMER(0x104)` / `VICIOUS_HAMMER(0x162)`。
   - `PacketCreator.java:595-607` equip 包尾部 5 个 int16 潜能 + 星之力 1B，与客户端 `ItemParser.add_equip` 对齐。
   - 这些 handler 的触发**都来自道具使用包（UseItem/CashOperation）**，而非「打开某个固定界面」。

### 唯一的"专门入口"例外
当前端有侧栏图标入口（`UISideIconBar.cpp:387-457 open_ui`，`side_icons.ini` 热重载 5s）与魔女森林/突破石等**活动类**功能绑定（L440-447），但**强化/魔方/锤/潜能等功能没有挂任何侧栏槽位**——即没有专门入口，完全靠双击道具。

---

## 三、移植到当前客户端（xy / BeiDou-Client-xy）的具体开发流程

> 强制门禁：AGENTS.md §3.1 跨客户端密钥转换硬门禁 + skill 门禁 8/18（WZ/IMG 走 orange-wz MCP、原生还原走 REA→IDA→IMG）。**禁止裸拷 `UI_004.wz` 或 `Copy-Item` 到 live Data。**

### 阶段 A — 证据解析（已完成部分）
1. orange-wz MCP `load_files(paths=[UI_004.wz], key={name:v192, ivBase64:"AAAAAA==", userKeyBase64:_gms_key_128.b64}, exclusive:false)`（本会话已验证可加载，根节点树 depth2 已落盘）。
2. 对每个目标功能节点 `get_node_tree_json(nodePath=UIWindow2.img/<节点>, includePng=false, maxDepth=3)` 取 origin/帧序/button 态；需要图片时单独 `get_node_detail`/`set_png` 导出。
3. 用 192 `Use.txt/Cash.txt`（GBK dump，`tools/_192txt/`）把节点映射到具体道具 ID 与 prefix（触发分发依据）。
4. 192 二进制加壳不可取证逻辑 → 协议字段以「本端现有 s9 服务端 equip 包 + 192 资源文案」为准，任何概率/数值**不得编造**（沿用 ui006 的诚实边界）。

### 阶段 B — 资源入库（orange-wz，强制转 key）
1. **导出**：从 192 零 IV 源 `create_img_file` / `copy_paste_nodes` 把目标节点（如 `MiracleCube_Platinum`、`GoldHammer`）复制到 staging img（建议先与源同 key，避免跨钥粘贴歧义）。
2. **outlink 内联**：本端 nlnx **不解析运行时 outlink**（`Graphics/Texture.cpp:56-76` 的 map 特例不适用于 UI）；若节点含 link 占位 canvas，必须 `set_png` + 删 link 内联展开（ui006 已踩坑，75 个 canvas）。
3. **转 key（硬门禁）**：`batch_convert_key(源 零IV AAAAAA== → 目标 GMS TSPHKw==, verify:true)` 输出到**独立 staging 目录**；不得只靠 `change_key`（实测可能 verified:true 但落盘仍按源 IV）。
4. `save_as` 最终产物到**异于 loaded 根**的路径 → `unload_all` → 原子替换进 live `E:\MXD\BeiDou-Client-xy\Data\UI`（客户端关闭时）。
5. **本端 probe 门禁**：`--resource-probe`（工作目录设 live，报告 `_resource_probe.log`）确认目标 img 出现在 UI 根、canvas 尺寸正确、无 `0xc0000374` 堆损坏；未过不得宣称入库。

### 阶段 C — 客户端代码（UI 壳 + 触发接线）
1. 新增 UI 类 `IO/UITypes/UI<功能>.{h,cpp}`（参考 `UIViciousHammer`/`UIBreakthroughStone` 骨架：backgrnd + BtOk/BtCancel + Effect + SoftKey 选装备）。
2. `IO/UIElement.h` 加 UI 枚举；`Configuration.{h,cpp}` 加位置配置。
3. **触发接线（按道具 prefix）** `UIItemInventory.cpp::doubleclick` 的 CASH/USE 分支加 `else if (prefix == X)` → `emplace<UI<功能>>()`（现有 557/561/559/243 即此模式）；USE 页 `204` 类卷轴进 `UIEnchantSkill` 或新壳。
4. SoftKey：`E` 选装备槽、`S` 选卷轴/材料槽、`R`/Enter 确认发包（延续 r129/r194 模式）。
5. `Net/OutPacket.h` 加 opcode；新增 `Net/Packets/<X>Packets.h`；`PacketSwitch.cpp` 加回包 + handler 注册；`WaveAShellHandlers.{h,cpp}` 加 handler。
6. 图标/文案：需要 i18n（`locale/zh-CN.ini` 等）；图标走 Photoshop MCP 出图（默认 1x、原生风格、多状态）后 orange-wz 入库。

### 阶段 D — 服务端（s9 gms-server）
1. 在 `RecvOpcode`/`SendOpcode`/`PacketProcessor` 注册新 opcode + handler（参考 `CashOperationHandler`/`UseCashItemHandler`）。
2. 概率/奖励表集中在独立 Config 类（如 `WitchForestRewards.java` 模式），数值待官方/抓包确认前用默认设计并写明。
3. 回包 equip 必须按 `PacketCreator.java:595-607` 写全 5 潜能 + 星之力 1B，与 `ItemParser.add_equip` 对齐，否则字段平移。
4. 日志仅上线级 INFO/WARN/ERROR，调试期 DBG 日志完成后删除。

### 阶段 E — 验证 / 部署（闭环）
1. 客户端 `build_smoke.bat` EXITCODE=0；产物 MSBuild 直出 live exe。
2. 服务端 `mvn package` 成功；opcode 门禁（客户端 opcode == 服务端）通过。
3. resource-probe 通过（阶段 B 已述）。
4. **部署铁律**：只部署到 live `E:\MXD\BeiDou-Client-xy\`，exe 覆盖正式 `MapleStory.exe`，禁止带日期后缀旁挂；声明「已部署」前实时核对 live SHA（防登录器回滚）。
5. 真机验收：进游戏双击对应道具 → 开窗 → 选装备 → 确认 → 看结果特效/潜能变化；截图须来自 **live 本端**（USER.md 硬规则，严禁拿 192 参考端截图当本端验收）。
6. 产物归档：本 README + `notes/`、`evidence/`（节点树/IDA 摘要/道具 ID 表）、`patches/`；最终逻辑文档；单功能原子 commit（用户要求时）。

---

## 四、待用户拍板项（如启动实施）
1. **范围**：先做哪一族？建议先 A（魔方系，node 最完整、服务端 `CashOperationHandler 0x40` 已通）或 C（锤系，557 契约现成）。
2. **数值来源**：概率/结果表是否按默认设计（可单点改）？还是等官方/抓包？
3. **触发入口**：是否接受"双击道具"这一原生路径（不另加侧栏专门入口）？
4. **外观道具完整性**：若目标道具当前 live 缺图标/部件，按 skill 门禁需确认「完整同步 / 仅图标+预留」。
