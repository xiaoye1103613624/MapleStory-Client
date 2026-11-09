# §F 键位 action id 错位 —— 已按方案甲修复（2026-11-02）

## 修复内容

用户授权"继续修复"后实施 [open-issues §F](../notes/open-issues.md) 推荐的**方案甲**：
只在收/发包边界做双向翻译，`KeyAction.h` 枚举与 `UIStateGame` 的 case **一字不动**。

| 文件 | 改动 |
| --- | --- |
| `Net/KeyMapTranslate.h`（新增） | `KeyAction::from_server(int32_t)` / `to_server(Id)` 双向翻译，含映射依据注释 |
| `Net/Handlers/PlayerHandlers.cpp` | `KeymapHandler::handle` 收包侧 `add_keymapping(i, type, KeyAction::from_server(action))` |
| `Net/Packets/PlayerPackets.h` | `ChangeKeyMapPacket` 出站 `write_int(KeyAction::to_server(...))`（与 from_server 互逆） |
| `IO/UITypes/UIStatusBar.cpp` | `BT_CLASSIC_SHOP`（HUD 商城钮）改为直接 `EnterCashShopPacket().dispatch()` |

## 映射表（四处，双射）

| v83 服务端 | 客户端枚举 | 效果 |
| --- | --- | --- |
| 22 MONSTERBOOK | 42 MONSTERCOLLECTION | `b` → 开怪物手册（原：误发进商城包） |
| 23 CASHSHOP | 22 CASHSHOP | `` ` `` → 进商城（原：只改聊天目标） |
| 24 TOALLIANCE | 23 ALLIANCECHAT | `5` → 联盟聊天目标（原：死键） |
| 27 MEDAL | (Id)28 空位 | `;` → 无操作（原：误开 Boss 队列表）；勋章窗落地后改映射 |

未列出的 id 双向原样透传。`H/G/L/O/F`（WHISPER/GUILD/NOTIFIER/PARTYSEARCH/FAMILY）
仍为死键 —— 属"未实现功能"，不属本条错位，需单独立项。

## 为何发包侧必须回译

`UIKeyConfig` 改键位会外发 `CHANGE_KEYMAP`。若只收译不回译，客户端 id（42/22/23）
会写进服务端键位存档；下次登录服务端原样回包，`from_server` 再翻一遍就串号
（如 23→22 存回、重登被翻成 42）。双向互逆保证线路上始终是 v83 语义、客户端内部
始终是本枚举语义，幂等。

## 顺带更正：HUD「商城」钮旧提示过时

open-issues C5 / CHECKLIST B14 记的「EnterCashShop 40 vs 229 跨线必失败」**对比对象错了**
（40 是 `ENTER_CASHSHOP`，229 是 `CASHSHOP_OPERATION`，两条独立协议线）。核实：
客户端 `ENTER_CASHSHOP=40` == s9 `ENTER_CASHSHOP(0x28)`，且 s9 `EnterCashShopHandler`
完整可用（开店 + showCashInventory/showGifts/showWishList/showCash）。故 HUD 商城钮
与 `` ` `` 键现在都直接进商城，不再给「商城暂未支持。」提示。

## 验证

- `read_lints` 0 错误；`build_smoke.bat` `GATE OK` / 真实退出码 0（矩阵重生成后 c2s 102 MATCHED、s2c 299 HANDLED，无新增撞值）。
- 真机复测配方（下次进图）：
  1. `b` → 应开怪物手册（不换图不进商城）；
  2. `` ` `` → 应进商城（HUD 同路径）；
  3. `;` → 应无反应（不再弹 Boss 队列表）；
  4. `5` → 聊天目标切到联盟频道；
  5. `UIKeyConfig` 改任一键位 → 保存 → 重登 → 绑定不串号（回译幂等）。

## 遗留（不在本条范围）

- 勋章窗（`;`/27 真身）未实现，`(Id)28` 空位是占位灭活；
- `H/G/L/O/F` 五键对应窗体（密语/公会/通知/组队搜索/家族）按需另行立项。

## 五死键接线（2026-11-07 补记，用户指认 083 键位表「学院」后落地）

> 用户质疑「083 按键里面不是有学院吗！」为真：v83 action 26 = FAMILY（中文端「学院」）
> 在 083 键位表内，且学院=家族窗 `UIFamily` 早已闭环 —— 缺的只是键位入口。
> 上一轮 social-loop-audit 中「学院非 083 功能」指 v95 导师 Academy，两者勿混。

`IO/UIStateGame.cpp` MENU 分支新增 case（KeyAction.h 枚举与 KeyMapTranslate 表**不动**）：

| 键 | v83 action | 本枚举落点 | 接线行为 | 窗体证据（均 083 系既有） |
| --- | --- | --- | --- | --- |
| `H` | 11 WHISPER | WHISPER | UIWhisperTargetDlg（`makeactive()+open()`，同 pick_classic_target 路径） | UIChatBar 密语弹层 |
| `G` | 17 GUILD | GUILD | UIGuild emplace/makeactive（同底栏公会钮 :954/:1076） | UIGuild 实体 |
| `O` | 25 PARTYSEARCH | MANAGELEGION(25) 槽 | `window("PartySearch")` 可用则 emplace<UIPartySearch>（222/223） | UIPartySearch 实体 |
| `F` | 26 FAMILY（学院） | MEDALS(26) 槽 | `OpenFamilyPacket().dispatch()` + UIFamily emplace/makeactive | UIFamily 全套（上轮闭环） |
| `L` | 20 NOTIFIER | NOTIFIER | **保持灭活 + LOG 留证**：s9 `SendOpcode`/`net/opcodes` 全文 0 命中 notifier 后端（实测 grep），通知文案全走 serverNotice/chat；按门禁 7 不做假窗 | — |

验证：`build_smoke.bat` GATE OK / EXITCODE=0，exe 已部署 00:02。
真机点测并入 pending-live-tests（下一条）。

