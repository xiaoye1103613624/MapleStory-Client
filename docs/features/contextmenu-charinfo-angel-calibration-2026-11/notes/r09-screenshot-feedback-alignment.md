# R09 — 用户 2026-11-17 截图反馈：右键菜单对齐 + 个人情报人气箭头（橙色实测重定位）

## 缘起（用户原话）
> 右击玩家显示的ui，名称上半部分和下面列表有些偏差，名称边框应该往左；下面列表左右两侧间距不对称；
> "个人情报"默认打开图3，图3内容、样式存在偏差；图3上的所有按钮涉及到的ui、逻辑、功能都有问题；图6是正确客户端截图；
> 右击所有功能按链路发现检查一下；

## 关键：截图来源端裁决（USER.md 教训，先确认再定性）
- 图1（右键菜单，146×156）：本端现状，截于 23:04:27（早于本轮部署）。
- 图3（个人情报，274×201）：本端现状，截于 23:05:32。
- 图6（个人请报框，511×197）：**用户明确指定为正确客户端截图**，含右侧"道具"子窗，是像素参照基准。
- 绝不用参考端截图当本端验收证据。本轮所有"修好"结论仅表示代码+构建层通过，真机观感须用户按图6路径复核。

## 证据（orange-wz MCP，128B key 规格，load_files→query→unload_all 收口）
| 节点 | 尺寸（实测） | 蓝条可视 x 范围 | 用途 |
|---|---|---|---|
| ContextMenu/t | 100×23 | 4..96（中心50） | 标题板（含装饰括号） |
| ContextMenu/c | 97×15 | 3..93（中心48） | 列表主体板 |
| ContextMenu/s | 97×7 | 4..92 | 列表底 |
| ContextMenu/BtInfo 等 | 85×15 | — | 菜单按钮 |
| UserInfo/backgrnd | 275×199 | — | 个人情报默认背景板（无爱心，见下） |

## 逐条行号级结论（用户每个反馈点）

### ① 菜单"名称边框偏右/名称框应往左"
- 根因：`UIContextMenu::draw`（`IO/UITypes/UIMiscShells.cpp:5358`）标题文字用 `dimension.x()/2`（t 板宽 100 → 锚点 50）；
  t 板从 `position` 左对齐绘制，蓝条中心 50；列表 c 板中心 48 → 标题整体相对列表右偏 2px。
- 修复：标题板 t 左移 `title_shift=(100/2 - 97/2)=1`px；标题文字锚点改用列表主体中心 `body_w/2=48`。

### ② 菜单"列表左右间距不对称"
- 根因：`rebuild_buttons`（`UIMiscShells.cpp:5272`）按钮居中基准用 `win_w=tile_t.width()=100` → `btn_x=(100-85)/2=7`；
  按钮实际落在 97 宽 c 板上 → 左留白 7 / 右留白 5，不对称。
- 修复：居中基准改用列表板宽 `body_w=tile_c.width()=97` → `btn_x=(97-85)/2=6`，左6/右6 对称。

### ③ 个人情报图3 vs 图6 的偏差
| 项 | 本端图3 | 正确端图6 | 定性/处置 |
|---|---|---|---|
| 默认背景 | backgrnd 275×199（`:74` 只画这一块） | 同 | ✅ 前轮已修，本轮不动 |
| 顶部按钮 学院/组队/道具/交易/想买的 | 有，坐标 (100,20)/(148,22)/(209,22)/(148,44)/(209,44) | 同 | ✅ 前轮像素对齐，不动 |
| 底部按钮 骑宠/宠物/收藏/排除道具 | 有 | 同 | ✅ 前轮对齐，不动 |
| 立绘左上角爱心 | **无** | 有（灰心） | ⚠️ 资源层差异：本端 `UserInfo/backgrnd` 资源本身未烘焙爱心（orange-wz 解码 275×199 左上空白）。按门禁"原生还原禁造贴图"，不程序化糊一个；如实列为资源版本差，非代码漏画 |
| 人气度 ↑↓ 按钮 | 看不见 | 有（x≈239..264,y≈113 两个灰箭头） | ✅ 本轮修：`draw_fame_arrows`（`UICharInfo.cpp:935-948`）旧位置 (158,113) 偏左 ~80px 且 `0xF2F2F2` 浅灰在浅底不可见 → 改 (239,113)、中灰底 `0xB4B4B4` + 白色 +/- 字形 |
| 右侧"道具"子窗 | 默认不显示（点 BtItem 才弹） | 图6为点开态 | ✅ 逻辑已有：`button_pressed(BtItem)`→`show_subpanel(ITEM)`→`draw_item_panel`（`:410-415/:919-924/:958`）；图3是默认态故无子窗，非缺陷 |
| 字段值颜色/字重 | 灰黑（EMPEROR） | 灰黑 | r04 列入保护区，实机已对齐，不动 |

### ④ 图3 所有按钮的 UI/逻辑/功能（链路审计，逐项）
| 按钮 | UI 资源 | 逻辑出口 | 结论 |
|---|---|---|---|
| 学院 BtFamily(40×37) | UserInfo/BtFamily | 他人→FamilyAddPacket；自己→OpenFamily+UIFamily（`:387-400`） | ✅ |
| 组队邀请 BtParty | UserInfo/BtParty | PartyInvitePacket(name)（`:401-409`） | ✅ 自己态 disable（`:752`） |
| 交易邀请 BtTrade | UserInfo/BtTrade | TradeInvitePacket(cid)（`:467+`） | ✅ 自己态 disable（`:753`） |
| 道具 BtItem(60×19) | UserInfo/BtItem | show_subpanel(ITEM)（`:410-415`） | ✅ 右侧道具子窗；他人装备明细受 s9 协议边界限制（CHAR_INFO 不下发，非客户端可补） |
| 想买的 BtWish(60×19) | UserInfo/BtWish | show_subpanel(WISH)（`:422-427`） | ✅ |
| 骑宠 BtTamingShow(47×17) | UserInfo/BtTamingShow | show_subpanel(MOUNT)（`:435-439`） | ✅ |
| 宠物 BtPetShow(49×17) | UserInfo/BtPetShow | show_subpanel(PET)（`:440-444`） | ✅ |
| 收藏 BtCollectionShow(49×17) | UserInfo/BtCollectionShow | show_subpanel(MEDAL)（`:445-450`） | ✅ |
| 排除道具 BtExceptionShow(71×17) | UserInfo/BtExceptionShow | show_subpanel(EXCLUDE)（`:428-434`） | ✅ 数据受 PET_EXCEPTION_LIST 边界限制（handler 仅记录，未在本轮范围） |
| 人气 ↑↓ | 无 IMG 节点（程序化） | T/P 快捷键→BtPopUp/BtPopDown（`:612/:616`） | ✅ 本轮修可见性；校验语义仍走快捷键 |
| 关闭 BtClose | Basic.img/BtClose | deactivate()（`:381-386`） | ✅ |

### ⑤ 右键 8 项功能链路（r07 已全量审计，本轮复核无回归）
Info/Save/Trade/Whisper/Party/Guild/Claim/Family 全链路闭环（详见 r07）；弹出条件（仅命中玩家身体矩形）见 README 第一轮。本轮菜单对齐改动不影响功能逻辑。

## 本轮实际代码改动（4 处）
1. `UIMiscShells.cpp:5272` 按钮居中基准 100→97（列表板宽）。
2. `UIMiscShells.cpp:5341` 标题板 t 左移 `title_shift=1`px。
3. `UIMiscShells.cpp:5358` 标题文字锚点 50→48（列表中心）。
4. `UICharInfo.cpp:935-948` 人气箭头位置 (158→239)、颜色（浅灰→中灰+白字形）。

增量补丁：`patches/r09-menu-title-fame-alignment.diff`。

## 验证与诚实声明
- 构建 `tools/build_smoke.bat` EXITCODE=0，GATE OK，`error C`=0；主链 obj `x64/obj-Release/UICharInfo.obj` 23:28:01 / `UIMiscShells.obj` 23:28:40（> cpp 改动时间，确认入链）。
- 部署：首轮 LNK1104（exe 被游戏进程 36000 占用）→ 精确 taskkill 后重编成功；部署 exe 23:31:24 刷新，SHA `9e3c2c7ec220022080121ea53a1e18eaef38cbc873b921c449f03691f0fe530e`，10 秒后复查稳定未回滚。
- **真机未验收**：本轮未实机截图确认菜单标题/列表对齐、人气箭头可见。用户须按图1/图3 同样路径复核：右键玩家看标题与列表是否齐平、列表左右是否对称、人气行 ↑↓ 是否可见。编译通过 ≠ 观感验收。
- **资源层差异（非代码缺陷）**：本端 backgrnd 无爱心、他人装备明细无数据、排除道具数据未接线——均为资源/协议边界，未擅造。
- 僵尸进程 PID 26292（拒绝访问）为残留句柄，未阻止 exe 写入，SHA 稳定，不在本轮处理范围。
