# W2 底栏四按钮（帮助 / 界面 / 目录 / 商城）+ UIKeyConfig classic 钮坐标

取证：`python tools/nx_inspect.py list|image "E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx" <depth> <path>`（只读）
截图：`C:\Users\11036\Desktop\正确客户端截图\键盘设置.png`（629×369，与 `UIWindow.img/KeyConfig/backgrnd` 629×373 同宽，1:1）
依据编号 = `notes/batch-conventions.md` §3（1 烤字 / 2 角锚算术 / 3 截图实测）。

---

## T-1 帮助 → `Npt_GuideLine.img`（`IO/UITypes/UIHelp.{h,cpp}`）

| 控件 | 节点路径 | WxH + origin | 窗口相对左上角 | 依据 |
| --- | --- | --- | --- | --- |
| 窗框上片 | `UIWindow.img/UtilDlgEx/t` | 529×28，origin (0,0) | (0, 0) | 1（`t` 是整幅顶条，含圆角与标题区） |
| 正文底 | `ColorBox`（无位图） | 529×300 白 | (0, 28) | 2（`t`/`s` 之间的空腔用纯色填，实测 UtilDlgEx 内腔为白） |
| 窗框下片 | `UIWindow.img/UtilDlgEx/s` | 529×58，origin (0,0) | (0, 328) | 1（`s` 是整幅底条） |
| 关闭钮 | `UIWindow.img/BtUIClose` | 32×15，origin (0,0) | (491, 6) | 2：`FRAME_W - w - 6 = 529-32-6 = 491`；y=6 落在 28 高的顶条垂直居中 `(28-15)/2≈6` |
| 正文文本 | `Npt_GuideLine.img/NptGuide/Text00…Text48` | 纯 string，零位图 | x=16，首行 y=40，行距 17 | 2：左右各留 16 → `TEXT_W = 529-16-61 = 452`（61 = 右侧留白 31 + 滚动条 30） |
| 滚动条 | `Basic.img/VScr`（`Slider::DEFAULT_SILVER`=9 → 本包无 `VScr9`，W0 钳到 `VScr`） | 宽 12–15 | x = `FRAME_W-30 = 499`，纵向 Range (40, 316) | 2 |
| 窗体总尺寸 | — | — | 529×386 = 28+300+58 | — |

文本处理事实（写入代码注释之外）：
- `NptGuide/TextNN` 是 **UTF-8**（`nx_inspect` 转储按 UTF-8 解码得正常中文），直接交给 `Text` → `GraphicsGL::utf8_next`（T12b 度量），**未做任何 GBK 拼接**。
- 串内换行是 IMG 脚本约定的**字面量** `\r\n`（反斜杠+字母，不是 CR/LF 字节）。`GraphicsGL::LayoutBuilder::add` 只认字面量 `\n`（`\r` 另起一行），所以 `\r\n` 会被展开成两个换行 → `UIHelp::guide_line()` 把 `\r\n` 折叠成单个 `\n`，孤立 `\r` / 真 CR/LF 字节同样归一。
- 49 条放不进单窗 → `Slider` 逐行滚，`Text::draw(DrawArgument, Range<int16_t>)` 的 `vertical` 是**屏幕绝对 y 裁剪**（`GraphicsGL.cpp:1311-1312,1393-1408`），用它裁正文区。

本批放弃 / 预留：
- `UIWindow.img/Notice/{0..4}` 只当整体海报（`Notice/3` 实测是「프리미엄 PC방」促销图），**不能**当通用窗框 → 未采用。
- `DialogImage.img` 本包只有 `Help{0[311x77],1[40x46],2[81x43]}`，是横幅素材不是框 → 未采用。
- `Npt_GuideLine.img/Version/Number`（int 2）：无版号显示位，预留未画。

---

## T-2 界面 → `UIWindow.img/ShortCut`（`IO/UITypes/UIMiscShells.cpp` + `.h`）

底板：`UIWindow.img/ShortCut/backgrnd` **93×245，origin (0,0)**。
条带实测（`nx_inspect image` + 逐行 alpha）：y2..18 = 「界面」白色标题牌；y19..235 = 深色内腔；y236..244 = 向下尖角（中心 x=46，y240 处 x41..51）。

11 颗按钮全部 `81×25`（四态齐，`normal/0` 满幅不透明，仅四角透明），标题牌下方只有 214px 内腔 → **原生就是压缩行距**：

`y(i) = 20 + i*20`，`x = 6`（= (93-81)/2）。末颗底边 `20 + 10*20 + 25 = 245` = 底板高，闭合。
相邻重叠 5px 只吃掉上一颗的下边框渐变行（`BtStat/normal/0` 逐行扫描：字形亮像素集中在 y4..14，y20..23 是纯渐变+下边框）→ 依据 2。

| 序 | 控件 | 节点 | WxH | 左上角 | 点击派发（复用已有窗口） |
| --- | --- | --- | --- | --- | --- |
| 0 | BtStat | `ShortCut/BtStat` | 81×25 | (6, 20) | `UIStatsInfo(stats)` |
| 1 | BtEquip | `ShortCut/BtEquip` | 81×25 | (6, 40) | `UIEquipInventory(inventory)` |
| 2 | BtItem | `ShortCut/BtItem` | 81×25 | (6, 60) | `UIItemInventory(inventory)` |
| 3 | BtSkill | `ShortCut/BtSkill` | 81×25 | (6, 80) | `UISkillBook(stats, skills)` |
| 4 | BtQuest | `ShortCut/BtQuest` | 81×25 | (6, 100) | `UIQuestLog(quests)` |
| 5 | BtMobbook | `ShortCut/BtMobbook` | 81×25 | (6, 120) | `UIMonsterBook()` |
| 6 | BtComm | `ShortCut/BtComm` | 81×25 | (6, 140) | `UIUserList(Tab::FRIEND)` |
| 7 | BtGuild | `ShortCut/BtGuild` | 81×25 | (6, 160) | `UIGuild()` |
| 8 | BtParty | `ShortCut/BtParty` | 81×25 | (6, 180) | `UIPartySearch()` |
| 9 | BtRanking | `ShortCut/BtRanking` | 81×25 | (6, 200) | `UIRanking()` |
| 10 | BtMessenger | `ShortCut/BtMessenger` | 81×25 | (6, 220) | `UIMessenger()` |

行序（能力值→装备→物品→技能→任务→图鉴→通讯→公会→组队→排行→传呼）**无原生截图佐证**（参考目录里没有打开「界面」气球的截图），只保证 11 颗全在；要改顺序只动 `ids[]/names[]` 两个数组。
绘制顺序 = `buttons` map 键升序 → 下面的钮压上面的钮下边框，与上表重叠方向一致。

窗口锚定（替掉写死的 `(8, vh-120)`）：气球自带向下尖角 → 尖角中心对准底栏「界面」钮中心。
```
qs_w      = wide ? 466 : 151            // StatusBar.img/base/quickSlot26 / quickSlot08
group_gap = wide ? 241 : 75             // 与 UIStatusBar.cpp:1975 同源
group_x   = VWIDTH - qs_w - group_gap   // 1280 → 573（= IDA BtShop 的 573，互证）
BtShort   = group_x + 168，54×34        // 中心 x = group_x + 195
position.x = group_x + 195 - 46          // 46 = 尖角中心 x
position.y = VHEIGHT - 71 - 245          // 71 = CLASSIC_BAR_H
```
1280×720 → 窗口左上角 (722, 404)。依据 2。

本批放弃 / 预留：
- `ShortCut` 下**没有**关闭钮、没有 BtShop/BtCashshop → 关闭仍走 Esc / `X`（`send_key` 原有语义未动）。
- `tip_label`（黄色「1-0 A-Z …」）在**有素材时不再绘制**（气球 245 高已被 11 颗钮占满），只在资源缺失的回退壳里保留；键盘快捷语义一字未改。

---

## T-2b 目录（`UIWindow.img/GameMenu`）— R14-12 已改，见 `gamemenu-balloon.md`

`GameMenu` = `BtChannel BtGameOpt BtQuit BtSkin BtSysOpt`（各 81×25）+ `backgrnd` **93×140**（alpha：row0..3 收角、row4..130 满宽 x0..92、row131..139 下摆收束到 x45..47）。

> ~~5×25 = 125，`140-125 = 15` → 若按 `y = 15 + i*25` 可无缝排布。~~
> **此推测已被截帧推翻**：底栏内腔装的是 **4** 颗钮、**步距 26**（行间隙使相邻钮之间留 1 行空隙），
> 且原生这一层不摆 `BtSkin`。原 `y=15+i*25` 假设会让 4 颗无缝粘连、5 颗超出下摆。

**当时的未改原因**（现已处理）：`UIGameMenu` 一颗钮都没实例化，且 `send_key` 是 `1..9 → Ranking/Counsel/…`。
用户报的「目录打开主菜单？」有两半：**行为**（点目录弹主菜单气球 = 原生正确，截帧可证）+ **版式**（缺 4 颗真钮）。
行为根因在 `UIStatusBar.cpp:719-738`（`BT_CLASSIC_MENU` classic 分支只 emplace `UIGameMenu`，不再同开 `UIOptionMenu`）。
版式根因与量值见 [gamemenu-balloon.md](gamemenu-balloon.md) §2..§5：4 颗 @ `(8, 22 + i*26)`。

---

## T-3 商城 → 可见提示（`IO/UITypes/UIStatusBar.cpp`）

- 提示通道：`UIChatBar::show_message(const char*, UIChatBar::MessageType::YELLOW)`（`UIChatBar.h:79` / `.cpp:1291`，`Messages.cpp:52` 已在用同一条），文案 `"商城暂未支持。"`，进聊天历史框，玩家可见。
- `BT_CLASSIC_SHOP` 与 `BT_CASHSHOP` 拆开：classic 只提示，**modern 仍发 `EnterCashShopPacket`**。
- 未动 opcode（40 vs 229 属 s9 线，归其它任务）；`UIShortCut::send_key` 里 `'$'` 的 `EnterCashShopPacket` 是既有键位语义，未动。

---

## T-4 `UIKeyConfig` 五钮（`IO/UITypes/UIKeyConfig.cpp`）

根：`UiAssetResolve::status_bar_root()["KeyConfig"]` → 本包无 `StatusBar.img/KeyConfig`，回退 `UIWindow.img/KeyConfig`，`backgrnd` **629×373**，origin (314,186)。
classic 分支用 `DrawArgument(bg.get_origin())` 绘制 → 底图 TL 与 `position` 对齐 → 按钮相对坐标即底图相对坐标。

`KeyConfig` 下**没有**新版 `button:OK/Cancel/Default/Delete/keySetting` 命名，只有 `Bt*`（`first_node` 的回退链正好接上）：

| 控件 | 节点 | WxH | 截图实测可见带 | 传给 `try_maple_btn` 的左上角 | 依据 |
| --- | --- | --- | --- | --- | --- |
| OK（确定） | `KeyConfig/BtOK` | 47×19 | x10..52 y236..250（橙底） | **(8, 234)** | 3 |
| CANCEL（取消） | `KeyConfig/BtCancel` | 47×19 | x60..102 y236..250 | **(58, 234)** | 3 |
| DEFAULT（取消修改） | `KeyConfig/BtDefault` | 65×19 | x114..174 y236..250（绿底） | **(112, 234)** | 3 |
| DELETE（全部消除） | `KeyConfig/BtDelete` | 71×19 | x179..245 y236..250 | **(177, 234)** | 3 |
| KEYSETTING（变更快捷键） | `KeyConfig/BtQuickSlot` | 96×18 | x262..353 y236..250 | **(260, 234)** | 3 |

算式：可见带比位图窄 2px/边（四态图带透明描边）→ `TL = 可见带min - 2`。
`DEFAULT`/`DELETE` 两钮**本包是有的**（`BtDefault`/`BtDelete`），任务书里「本包可能没有」的担心不成立；缺的只是 `button:*` 命名，已由 `first_node` 回退。

`else`（modern）分支此前五颗 `try_maple_btn` 都没传 position → 被 W0 的 `maple_btn_nopos()` 哨兵整批跳过。两支取的是**同一个** `KeyConfig` 根（构造函数开头只解析一次，见 `UIKeyConfig.cpp:71-74`），所以 if/else 已合并成一套坐标（`classic` 变量仍用于上面三张 backgrnd 的 origin 补偿）。

⚠ 任务书把两支的名字写反了：代码里 `if (classic)`（:112-123）**是**有坐标的那支，`else`（:126-130）才是没坐标的那支。本包 `UiAssetResolve::is_classic_hud()` 为真，实跑走 classic 支；classic 支旧值 `OK(490,343) CANCEL(540,343) DEFAULT(90,248) DELETE(155,248) KEYSETTING(250,343)` 与截图（五钮同在底图 y234 一行、最左 x=8 起）完全不符，已按截图实测一并改掉。

本批放弃 / 预留：
- `KeyConfig/BtHelp`（12×12）、`BtClose`（12×12）：`BtClose` 已由 `basic_close_btn()`→`BtClose2` 覆盖，`BtHelp` 无对应 Buttons 枚举，未接线。
- `Basic.img/BtClose3` 本包不存在（inventory §Basic 已记），`basic_close_btn()` 的回退实际永远落在 `BtClose2`。

存档件（本目录）：`keyconfig-nodes.txt` = `nx_inspect list UIWindow.img/KeyConfig` 全量节点与五态；`BtQuickSlot.png`（96×18）与 `BtHelp.png`（12×12）= 两支 `normal/0` 位图导出。
待线上确认：第 5 颗「变更快捷键」用了 `BtQuickSlot`，因为本包 `KeyConfig` 下没有语义更贴的节点，而 `BtHelp` 只有 12×12（放不下「变更快捷键」四字）。截图见 `BtQuickSlot` 可见带 x262..353 与参考端第 5 钮位置吻合，但**贴图文字是否真为「变更快捷键」只能开窗口看**（CHECKLIST D 区）。
