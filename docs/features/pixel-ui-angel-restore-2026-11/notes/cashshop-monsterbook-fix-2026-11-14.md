# 商城布局 + 怪物手册路由修复 · 2026-11-14

对照：图1 错误商城 / 图2 正确商城 / 图3·5 手册卡片页 / 图4 基本情报；`C:\Users\11036\Desktop\正确客户端截图`。
主路径：Angel IDA（`CCashShop::Init` / `CCSWnd_*::OnCreate` / `GetPlateRect`）+ orange-wz 128B（`CashShop.img` / `UIWindow.img/MonsterBook`）。

## 根因

| # | 现象 | 根因 | 证据 |
| --- | --- | --- | --- |
| 1 | 开怪物手册进商城 | v83 `B`=action22（MONSTERBOOK）。本枚举 `CASHSHOP=22` 图标是手册帽，classic 未绑定条挂 `CASHSHOP` → 拖帽绑到 `EnterCashShopPacket`；默认 `B→PROFESSION` 亦非手册。`KeyMapTranslate` 已双射 22↔`MONSTERCOLLECTION`，但 KeyConfig 图标/默认/托盘未闭环 | `KeyMapTranslate.h`；`UIKeyConfig` classic unbound；open-issues §F |
| 2 | 商城 UI 错乱（双 EXIT、钮漂浮、空格、准备商品弹层偏位） | 控件坐标用 Journey 现代布局（预览右、Status 顶、7 列格、滑条 x=1007）；Angel 子窗是 800×600 钉死原点。另 `SetCashShopHandler` 强制 1024×768，与 `Base/backgrnd` 800×600 错配 | `CCashShop::Init` CreateWnd；`CCSWnd_Status` (248/289/330/378,13/15)；`GetPlateRect` 2×5；orange-wz backgrnd 800×600、BtExit 168×49、CSList/Base 200×80 |
| 3 | 手册像素（卡片页左右颠倒、6 行） | 旧实现左 info / 右 5×6 格，来自对「基本情报」截图的误匹配；正确卡片页左 5×5 格、右怪名+HP/MP | 图3/5；`MonsterBook/cardSlot` 174×256；Angel BtSearch(175,29) 落左页 |

## Angel 子窗原点（`CCashShop::Init`）

| 子窗 | CreateWnd | 控件立即数 |
| --- | --- | --- |
| Char | (0,0) 256×316 | Preview(24,40)；Buy/Default/Takeoff y=237 |
| Tab | (272,17) 508×78 | CSTab/Tab 1..9 |
| List | (275,95) 412×430 | Plate 200×80 stride 206×81；Buy+(80,53) Gift+(119,53) |
| Best | (690,157) 90×358 | GetBestRect 69×n |
| Status | (254,530) 545×56 | Charge/Check/Coupon/Exit |
| Search | (690,97) 89×22 | CSItemSearch |

## 代码改动

- `IO/UITypes/UIKeyConfig.h` — 默认 `B→MONSTERCOLLECTION`
- `IO/UITypes/UIKeyConfig.cpp` — classic 未绑定条挂手册；`load_actions` 用 icon[22]/[23] 重绑
- `IO/UITypes/UICashShop.{h,cpp}` — Angel 子窗坐标；`MAX_ITEMS=10`；NoItem 居中 List
- `Net/Handlers/CashShopHandlers.cpp` — 进店视口 800×600
- `IO/UITypes/UIMonsterBook.{h,cpp}` — 左 5×5 格 / 右详情；无选中卡时图4 书级统计

## 自测清单

1. `B` / 界面气球「怪物手册」→ `UIMonsterBook`（不换图进商城）
2. 底栏商城钮 / `` ` ``（键位正确时）→ 进商城
3. 商城：单 EXIT 在底右；充值三圆钮在底栏；预览左；试穿三钮在预览下；中 2×5 商品格；无双 EXIT
4. 商品目录空时「正在准备商品」居中 List，不挡左下背包
5. 手册：左 5×5 + 搜索；右怪名 + HP/MP ???；右签可切；无选中时基本情报统计

## 构建 / 部署

- Staging：`build-cashshop-monsterbook\MapleStory.exe`（5101056 B，2026-11-14 02:37）
- Live：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
- SHA256（staging = live）：`098D1350282B600F60F6FC7E0855F041F3447011C9B376DB3241D6D7025E8DD5`
- 部署：`taskkill /F /IM MapleStory.exe /T` → `Copy-Item` → hash 对齐
