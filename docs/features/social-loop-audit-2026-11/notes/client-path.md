# 客户端路径记忆（社交线 · 2026-11-06）

## 基准裁决（用户 2026-11-06 明示）

> 「参照 083，原版功能可别参照 095 搞，找资源也是。」

- **功能参照 = GMS 083 原版**。禁止拿 095（浩浩岛095 / PLUS2 / `port-095` 线）反编译结果
  作为 083 原版能力证据；v95 符号（如 `ExpeditionIntermediary`、v95 opcode 表）只可作旁注。
- **资源同理**：补时装/图标/道具资源须取 083 适配来源，不走 095/192 系。

## 参照端与资源路径

| 用途 | 路径 | 说明 |
| --- | --- | --- |
| 083 参照端二进制（REA/IDA） | `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe`（IDB 同目录，r12 起一直在用） | v83 GMS113 基线 |
| 083 参照端 IDB 库 | `E:\MXD\BeiDou_idb\` | 预命名对齐 |
| 当前部署目标（本端产物） | `E:\MXD\BeiDou-Client-xy\MapleStory.exe` | build_smoke 直链到此 |
| 083 功能面权威清单 | `docs/features/v83-hud-quest/evidence/UIWindow.img.top-level.txt`（112 节点） | Wedding/Family/FamilyTree/UserList 均在表内；**无 Academy/BossParty** |
| 083 适配时装资源 | `E:\MXD\适用于083时装\` | 找时装优先 |
| 083 适配动漫皮肤 | `E:\MXD\动漫皮肤083.zip` | 优先 zip 内 083 版 |
| 083 适配阿尔泰皮肤 | `E:\MXD\111款阿尔泰皮肤分享（GMS083适配）\` | 同上 |
| 小册子图鉴（道具资源站） | `https://mxd.dvg.cn/item_list.php`（按 083 版本号筛选） | 本地没有再用 |

## 禁作参照（095 线，独立在研）

`E:\MXD\【2021最新】095PLUS2_10.1国庆版`、`Maplestory95.i64/.idb`、`docs/features/port-095/*`
（该目录内的 opcode-map/反编译注记仅服务 095 移植切片，不得引为 083 依据）。

## 对既有结论的影响（已复核）

1. 婚礼/家族/组队线全部证据本就来自 s9（v83 派生）+ v83 UIWindow 清单 + BeiDou v17.7 REA，**无污染**。
2. 「学院 = Academy 需 s9 立项」旧说法修正：Academy 只见于 095 线符号，083 清单无此窗
   ⇒ 按基准判「非原版功能，不做」；v83 中文「学院」= Family，已闭环。
3. 「远征 BossParty 面板缺口」旧说法修正：面板同样只有 095 证据 ⇒ 非 083 原版缺口；
   远征玩法（队伍+NPC+事件）083/s9 俱在，本端可参与。
   `UIUserList` 第 3 格按 r27-oracle（083 实包）应为「家族」页而非 BossParty（版式校准小项）。
